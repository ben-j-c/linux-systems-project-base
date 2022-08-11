#include "epoll_hook.h"

#include <stdarg.h>
#include <stdlib.h>
#include <sys/epoll.h>

#include "data-structures/hashtable.h"
#include "errstack.h"
#include "util.h"

eh_ops_et _epoll_define_to_eh_op(int epoll_define)
{
	if (epoll_define == EPOLLIN) {
		return EH_OPS_IN;
	}
	if (epoll_define == EPOLLOUT) {
		return EH_OPS_OUT;
	}
	if (epoll_define == EPOLLRDHUP) {
		return EH_OPS_RD_HUP;
	}
	if (epoll_define == EPOLLPRI) {
		return EH_OPS_EXCEPTIONAL;
	}
	if (epoll_define == EPOLLERR) {
		return EH_OPS_ERR;
	}
	if (epoll_define == EPOLLHUP) {
		return EH_OPS_HUP;
	}
	return EH_OPS_END;
}

int _eh_op_to_epoll_define(eh_ops_et op)
{
	if (op == EH_OPS_IN) {
		return EPOLLIN;
	}
	if (op == EH_OPS_OUT) {
		return EPOLLOUT;
	}
	if (op == EH_OPS_RD_HUP) {
		return EPOLLRDHUP;
	}
	if (op == EH_OPS_EXCEPTIONAL) {
		return EPOLLPRI;
	}
	if (op == EH_OPS_ERR) {
		return EPOLLERR;
	}
	if (op == EH_OPS_HUP) {
		return EPOLLHUP;
	}
	return 0;
}

struct eh_ctx_s
{
	int epoll_fd;
	ht_st *hooks;
};

struct eh_hook_s
{
	eh_ctx_st *owner;
	int fd;
	void *data;
	eh_hook_ft ops[EH_OPS_MAX];
};

int eh_ctx_alloc(eh_ctx_st **dst, UNUSED bool threaded)
{
	CLEANUP(eh_ctx_cleanup) eh_ctx_st *tmp = calloc(1, sizeof(*tmp));
	ES_ERR_ASRT_NM(tmp);
	ES_ERR_INT_NM(tmp->epoll_fd = epoll_create1(EPOLL_CLOEXEC));
	ES_PUSH_INT_NM(ht_int_alloc(&tmp->hooks, 0, NULL, NULL));
	*dst = MOVE_PZ(tmp);
	return 0;
}

void _cleanup_epoll_event(struct epoll_event **evs)
{
	if (*evs)
		free(*evs);

	*evs = NULL;
}

int eh_ctx_wait(eh_ctx_st *ctx, size_t max_events, int ms)
{
	int n_ev;
	int i;
	CLEANUP(_cleanup_epoll_event) struct epoll_event *evs = malloc(sizeof(*evs) * max_events);
	ES_ERR_ASRT_NM(ctx);
	ES_ERR_INT_NM(ctx->epoll_fd);
	ES_ERR_INT_ERRNO(n_ev = epoll_wait(ctx->epoll_fd, evs, 1, ms));
	for (i = 0; i < n_ev; i++) {
		bool new_events[EH_OPS_MAX] = {};
		size_t j;
		struct epoll_event *ev = &(evs[i]);
		eh_hook_st *hook       = ev->data.ptr;
		for (j = 0; j < EH_OPS_MAX; j++) {
			int flag = _eh_op_to_epoll_define(j);
			if (flag & ev->events) {
				new_events[j] = true;
			}
		}
		if (hook->ops[EH_OPS_ALL]) {
			int ret;
			ES_ERR_INT_NM(ret = hook->ops[EH_OPS_ALL](ctx, hook, new_events));
			if (ret == 0) {
				continue;
			}
		}
		for (j = 0; j < EH_OPS_ALL; j++) {
			if (new_events[j] && hook->ops[j]) {
				int ret;
				ES_ERR_INT_NM(ret = hook->ops[j](ctx, hook, new_events));
				if (ret == 0) {
					break;
				}
			}
		}
	}
	return n_ev;
}

static uint32_t _get_epoll_flags(const eh_hook_st *hook, eh_ops_et new_op)
{
	uint32_t flags = 0;
	if (hook->ops[EH_OPS_IN] || new_op == EH_OPS_IN) {
		flags |= EPOLLIN;
	}
	if (hook->ops[EH_OPS_OUT] || new_op == EH_OPS_OUT) {
		flags |= EPOLLOUT;
	}
	if (hook->ops[EH_OPS_RD_HUP] || new_op == EH_OPS_RD_HUP) {
		flags |= EPOLLRDHUP;
	}
	if (hook->ops[EH_OPS_EXCEPTIONAL] || new_op == EH_OPS_EXCEPTIONAL) {
		flags |= EPOLLPRI;
	}
	if (hook->ops[EH_OPS_ERR] || new_op == EH_OPS_ERR) {
		flags |= EPOLLERR;
	}
	if (hook->ops[EH_OPS_HUP] || new_op == EH_OPS_HUP) {
		flags |= EPOLLHUP;
	}
	if (hook->ops[EH_OPS_ALL] || new_op == EH_OPS_ALL) {
		flags |= EPOLLIN | EPOLLOUT | EPOLLRDHUP | EPOLLPRI | EPOLLERR | EPOLLHUP;
	}
	return flags;
}

int eh_ctx_reg_hook(eh_ctx_st *ctx, eh_hook_st *hook)
{
	struct epoll_event to_add = {};
	ES_ERR_ASRT_NM(ctx);
	ES_ERR_ASRT_NM(hook);
	hook->owner     = ctx;
	to_add.data.ptr = hook;
	to_add.events   = _get_epoll_flags(hook, EH_OPS_END);
	ES_ERR_INT_ERRNO(epoll_ctl(ctx->epoll_fd, EPOLL_CTL_ADD, hook->fd, &to_add));
	ES_ERR_INT_NM(ht_int_set(ctx->hooks, hook->fd, hook));
	return 0;
}

void eh_ctx_unreg_hook(eh_ctx_st *ctx, eh_hook_st *hook)
{
	if (!ctx || !hook) {
		return;
	}
	hook->owner = NULL;
	if (ctx->epoll_fd >= 0) {
		/*No need to handle errors, if it couldn't be deleted, it couldn't have been added*/
		epoll_ctl(ctx->epoll_fd, EPOLL_CTL_DEL, hook->fd, NULL);
	}
	ht_int_delete(ctx->hooks, hook->fd);
}

int _ctx_cleanup_foreach(UNUSED const ht_st *ht, UNUSED void *key, void *value, UNUSED void *data)
{
	eh_hook_cleanup((eh_hook_st **) &value);
	return 1;
}

void eh_ctx_cleanup(eh_ctx_st **dst)
{
	if (!*dst) {
		return;
	}
	if ((*dst)->epoll_fd >= 0) {
		close((*dst)->epoll_fd);
		(*dst)->epoll_fd = -1;
	}
	if ((*dst)->hooks) {
		ht_foreach((*dst)->hooks, _ctx_cleanup_foreach, *dst);
	}
	free(*dst);
	*dst = NULL;
}

int eh_ctx_hook_alloc(eh_ctx_st *ctx, int fd, void *data, const struct eh_ops_mapping_s ops[])
{
	eh_hook_st *hook = NULL;
	ES_ERR_ASRT_NM(ctx);
	ES_PUSH_INT_NM(eh_hook_alloc(&hook, fd, data, ops));
	ES_PUSH_INT_NM(eh_ctx_reg_hook(ctx, hook));
	return 0;
}

int eh_hook_alloc(eh_hook_st **dst, int fd, void *data, const struct eh_ops_mapping_s ops[])
{
	size_t i                                 = 0;
	CLEANUP(eh_hook_cleanup) eh_hook_st *tmp = calloc(1, sizeof(eh_hook_st));
	ES_ERR_ASRT_NM(tmp);
	ES_ERR_INT_NM(fd);
	tmp->fd   = fd;
	tmp->data = data;
	while (ops[i].op != EH_OPS_END) {
		tmp->ops[ops[i].op] = ops[i].fn;
		i++;
	}
	*dst = MOVE_PZ(tmp);
	return 0;
}

int eh_hook_mod_set_cbf(eh_hook_st *hook, eh_ops_et op, eh_hook_ft fn)
{
	struct epoll_event ev = {};
	ES_ERR_ASRT_NM(hook);
	ES_ERR_ASRT_NM(hook->owner);
	ES_ERR_ASRT_NM(op >= 0 && op < EH_OPS_MAX);
	if (hook->ops[op] == fn) {
		return 0;
	}
	ev.data.ptr = hook;
	ev.events   = _get_epoll_flags(hook, op);
	ES_ERR_INT_ERRNO(epoll_ctl(hook->owner->epoll_fd, EPOLL_CTL_MOD, hook->fd, &ev));
	hook->ops[op] = fn;
	return 1;
}

void eh_hook_set_data(eh_hook_st *hook, void *data)
{
	if (hook->data == data) {
		return;
	}
	hook->data = data;
}

void *eh_hook_get_data(eh_hook_st *hook)
{
	return hook->data;
}

int eh_hook_get_fd(eh_hook_st *hook)
{
	return hook->fd;
}

void eh_hook_cleanup(eh_hook_st **dst)
{
	if (!*dst) {
		return;
	}
	if ((*dst)->owner) {
		eh_ctx_unreg_hook((*dst)->owner, *dst);
	}
	free(*dst);
	*dst = NULL;
	return;
}
