#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <sys/epoll.h>

typedef struct eh_ctx_s eh_ctx_st;
typedef struct eh_hook_s eh_hook_st;

typedef enum eh_ops_e
{
	EH_OPS_END = -1,
	EH_OPS_IN,
	EH_OPS_OUT,
	EH_OPS_RD_HUP,
	EH_OPS_EXCEPTIONAL,
	EH_OPS_ERR,
	EH_OPS_HUP,
	EH_OPS_ALL,
	EH_OPS_MAX,
} eh_ops_et;

typedef int (*eh_hook_ft)(eh_ctx_st *ctx, eh_hook_st *hook, bool ops[EH_OPS_MAX]);
typedef struct eh_ops_mapping_s
{
	eh_ops_et op;
	eh_hook_ft fn;
} eh_ops_mapping_st;

typedef enum eh_mod_hook_e
{
	EH_MOD_ADD_CBF,
	EH_MOD_DEL_CBF,
	EH_MOD_SET_DATA,
} eh_mod_hook_et;

int eh_ctx_alloc(eh_ctx_st **dst, bool threaded);
int eh_ctx_wait(eh_ctx_st *ctx, size_t max_events, int ms);
int eh_ctx_reg_hook(eh_ctx_st *ctx, eh_hook_st *hook);
void eh_ctx_unreg_hook(eh_ctx_st *ctx, eh_hook_st *hook);
void eh_ctx_cleanup(eh_ctx_st **dst);
int eh_ctx_hook_alloc(eh_ctx_st *dst, int fd, void *data, const struct eh_ops_mapping_s ops[]);

int eh_hook_alloc(eh_hook_st **dst, int fd, void *data, const struct eh_ops_mapping_s ops[]);
int eh_hook_mod_set_cbf(eh_hook_st *hook, eh_ops_et op, eh_hook_ft fn);
void eh_hook_set_data(eh_hook_st *hook, void *data);
void *eh_hook_get_data(eh_hook_st *hook);
int eh_hook_get_fd(eh_hook_st *hook);
void eh_hook_cleanup(eh_hook_st **dst);