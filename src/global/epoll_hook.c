#include "epoll_hook.h"
/**
 * Copyright by Benjamin Joseph Correia.
 * Date: 2022-08-11
 * License: MIT
 *
 * Description:
 * This is a utilities file for epoll callbacks.
 */

#include <stdarg.h>
#include <stdlib.h>
#include <sys/epoll.h>

#include "data-structures/hashtable.h"
#include "errstack.h"
#include "util.h"

struct eh_ctx_s
{
	int epoll_fd;
	ht_st *hooks;
	bool threaded;
	bool oneshot;
};

struct eh_hook_s
{
	eh_ctx_st *owner;
	int fd;
	void *data;
	eh_hook_ft ops[EH_OPS_MAX];
};

static int _eh_op_to_epoll_define(const eh_ops_et op)
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
	if (op == EH_OPS_ALL) {
		return EPOLLIN | EPOLLOUT | EPOLLRDHUP | EPOLLPRI | EPOLLERR | EPOLLHUP;
	}
	return 0;
}

static uint32_t _get_epoll_flags(const eh_hook_st *hook, const eh_ops_et new_op)
{
	uint32_t flags = 0;
	size_t i;
	for (i = 0; i < EH_OPS_MAX; i++) {
		if (hook->ops[i] || (size_t) new_op == i) {
			flags |= _eh_op_to_epoll_define(i);
		}
	}
	return flags;
}

int eh_ctx_alloc(eh_ctx_st **const dst, const bool threaded, const bool oneshot)
{
	CLEANUP(eh_ctx_cleanup) eh_ctx_st *tmp = calloc(1, sizeof(*tmp));
	ES_NEW_ASRT_NM(tmp);
	tmp->threaded = threaded;
	tmp->oneshot  = oneshot;
	ES_NEW_INT_NM(tmp->epoll_fd = epoll_create1(EPOLL_CLOEXEC));
	ES_FWD_INT_NM(ht_int_alloc(&tmp->hooks, 0, NULL, NULL));
	*dst = MOVE_PZ(tmp);
	return 0;
}

void _cleanup_epoll_event(struct epoll_event **const evs)
{
	if (*evs)
		free(*evs);

	*evs = NULL;
}

int eh_ctx_wait(eh_ctx_st *const ctx, const size_t max_events, const int ms)
{
	int n_ev;
	int i;
	CLEANUP(_cleanup_epoll_event) struct epoll_event *evs = malloc(sizeof(*evs) * max_events);
	ES_NEW_ASRT_NM(ctx);
	ES_NEW_INT_NM(ctx->epoll_fd);
	ES_NEW_INT_ERRNO(n_ev = epoll_wait(ctx->epoll_fd, evs, 1, ms));
	for (i = 0; i < n_ev; i++) {
		bool new_events[EH_OPS_MAX] = {};
		bool hangup                 = false;
		size_t j;
		struct epoll_event *ev = &(evs[i]);
		eh_hook_st *hook       = ev->data.ptr;
		/*Parse epoll event flags*/
		for (j = 0; j < EH_OPS_MAX; j++) {
			int flag = _eh_op_to_epoll_define(j);
			if (flag & ev->events) {
				new_events[j] = true;
			}
		}
		hangup = new_events[EH_OPS_HUP];
		/*Run through hooked events. User is allowed to modify flag array. ALL is processed first*/
		if (hook->ops[EH_OPS_ALL]) {
			int ret;
			ES_NEW_INT_NM(ret = hook->ops[EH_OPS_ALL](ctx, hook, new_events));
			if (ret == 0) {
				continue;
			}
		}
		for (j = 0; j < EH_OPS_ALL; j++) {
			if (new_events[j] && hook->ops[j]) {
				int ret;
				ES_NEW_INT_NM(ret = hook->ops[j](ctx, hook, new_events));
				if (ret == 0) {
					break;
				}
			}
		}
		/*On hangup */
		if (hangup) {
			if (!ctx->oneshot) {
				eh_ctx_unreg_hook(ctx, hook);
			}
		} else if (ctx->oneshot) {
			struct epoll_event nev = {};
			nev.data.ptr           = hook;
			nev.events             = _get_epoll_flags(hook, EH_OPS_END);
			nev.events |= EPOLLONESHOT | (ctx->threaded ? EPOLLET : 0);
			ES_NEW_INT_ERRNO(epoll_ctl(ctx->epoll_fd, EPOLL_CTL_MOD, hook->fd, &nev));
		}
	}
	return n_ev;
}

int eh_ctx_reg_hook(eh_ctx_st *const ctx, eh_hook_st *const hook)
{
	struct epoll_event to_add = {};
	ES_NEW_ASRT_NM(ctx);
	ES_NEW_ASRT_NM(hook && hook->owner == NULL);
	ES_NEW_ASRT_NM(!ht_int_get(ctx->hooks, hook->fd));
	hook->owner     = ctx;
	to_add.data.ptr = hook;
	to_add.events   = _get_epoll_flags(hook, EH_OPS_END);
	if (ctx->threaded) {
		to_add.events |= EPOLLET;
	}
	if (ctx->oneshot) {
		to_add.events |= EPOLLONESHOT;
	}
	ES_NEW_INT_ERRNO(epoll_ctl(ctx->epoll_fd, EPOLL_CTL_ADD, hook->fd, &to_add));
	ES_NEW_INT_NM(ht_int_set(ctx->hooks, hook->fd, hook));
	return 0;
}

void eh_ctx_unreg_hook(eh_ctx_st *const ctx, eh_hook_st *const hook)
{
	if (!ctx || !hook || ctx != hook->owner) {
		return;
	}
	hook->owner = NULL;
	if (ctx->epoll_fd >= 0) {
		/*No need to handle errors, if it couldn't be deleted, it couldn't have been added*/
		epoll_ctl(ctx->epoll_fd, EPOLL_CTL_DEL, hook->fd, NULL);
	}
	ht_int_delete(ctx->hooks, hook->fd);
}

static int _ctx_cleanup_foreach(UNUSED const ht_st *ht,
                                UNUSED void *key,
                                void *value,
                                UNUSED void *data)
{
	eh_hook_cleanup((eh_hook_st **) &value);
	return 1;
}

void eh_ctx_cleanup(eh_ctx_st **const dst)
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

int eh_ctx_hook_alloc(eh_ctx_st *const ctx,
                      const int fd,
                      void *const data,
                      const eh_hook_ft (*ops)[EH_OPS_MAX])
{
	CLEANUP(eh_hook_cleanup) eh_hook_st *hook = NULL;
	ES_NEW_ASRT_NM(ctx);
	ES_FWD_INT_NM(eh_hook_alloc(&hook, fd, data, ops));
	ES_FWD_INT_NM(eh_ctx_reg_hook(ctx, hook));
	hook = NULL;
	return 0;
}

int eh_hook_alloc(eh_hook_st **const dst,
                  const int fd,
                  void *const data,
                  const eh_hook_ft (*ops)[EH_OPS_MAX])
{
	size_t i                                 = 0;
	CLEANUP(eh_hook_cleanup) eh_hook_st *tmp = calloc(1, sizeof(eh_hook_st));
	ES_NEW_ASRT_NM(tmp);
	ES_NEW_INT_NM(fd);
	tmp->fd   = fd;
	tmp->data = data;
	for (i = 0; i < ARRAY_SIZE(*ops); i++) {
		tmp->ops[i] = (*ops)[i];
	}
	*dst = MOVE_PZ(tmp);
	return 0;
}

int eh_hook_mod_set_cbf(eh_hook_st *const hook, const eh_ops_et op, eh_hook_ft const fn)
{
	ES_NEW_ASRT_NM(hook);
	ES_NEW_ASRT_NM(hook->owner);
	ES_NEW_ASRT_NM(op >= 0 && op < EH_OPS_MAX);
	if (hook->ops[op] == fn) {
		return 0;
	}
	if (_get_epoll_flags(hook, op) != _get_epoll_flags(hook, EH_OPS_END)) {
		struct epoll_event ev = {};
		ev.data.ptr           = hook;
		ev.events             = _get_epoll_flags(hook, op);
		ES_NEW_INT_ERRNO(epoll_ctl(hook->owner->epoll_fd, EPOLL_CTL_MOD, hook->fd, &ev));
	}
	hook->ops[op] = fn;
	return 1;
}

void eh_hook_set_data(eh_hook_st *const hook, void *const data)
{
	if (hook->data == data) {
		return;
	}
	hook->data = data;
}

void *eh_hook_get_data(const eh_hook_st *const hook)
{
	return hook->data;
}

int eh_hook_get_fd(const eh_hook_st *const hook)
{
	return hook->fd;
}

void eh_hook_get_ops(const eh_hook_st *const hook, eh_hook_ft (*const dst)[EH_OPS_MAX])
{
	size_t i;
	for (i = 0; i < ARRAY_SIZE(*dst); i++) {
		(*dst)[i] = hook->ops[i];
	}
}

void eh_hook_cleanup(eh_hook_st **const dst)
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
