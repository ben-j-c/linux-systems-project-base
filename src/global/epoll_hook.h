#pragma once
/**
 * Copyright by Benjamin Joseph Correia.
 * Date: 2022-08-11
 * License: MIT
 *
 * Description:
 * This is a utilities file. It defines useful interfaces for epoll.
 *
 * How to:
 * 1. Allocate a context
 * 2. Add file descriptors to the context via *_hook_alloc and eh_ctx_reg_hook
 */

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

/**
 * @brief Create a new epoll hook context.
 *
 * @param dst The location where the context will be stored
 * @param threaded A flag denoting whether this context needs to support multiple threads
 * @param oneshot A flag denoting whether this context needs to support one shot
 *
 * @returns >= 0 on success, -1 on failure. errno is also set by epoll_create1
 */
int eh_ctx_alloc(eh_ctx_st **dst, bool threaded, bool oneshot);
/**
 * @brief Handle up to max_events, calling associated registered hooks.
 *
 * @param ctx Working context
 * @param max_events Passed through to epoll_wait, and allocates this many epoll_events
 * @param ms Passed through to epoll_wait
 * @return >=0 on success < on failure; errno is set
 */
int eh_ctx_wait(eh_ctx_st *ctx, size_t max_events, int ms);
/**
 * @brief Register this hook to this context and start accepting events
 *
 * @param ctx Working context
 * @param hook Working hook
 * @return >=0 on success < on failure; errno is set
 */
int eh_ctx_reg_hook(eh_ctx_st *ctx, eh_hook_st *hook);
/**
 * @brief Unregister this hook from this context (stop waiting for events and deallocate hook). If a
 * failure occurs, errno is set, but process continues.
 *
 * @param ctx Working context
 * @param hook Working hook
 */
void eh_ctx_unreg_hook(eh_ctx_st *ctx, eh_hook_st *hook);
/**
 * @brief A __attribute__((cleanup())) safe implementation to free a context
 *
 * @param dst Any context (including NULL)
 */
void eh_ctx_cleanup(eh_ctx_st **dst);
/**
 * @brief Allocate a hook and then register it with this context
 *
 * @param dst The target context to hold the hook
 * @param fd The newly listening fd
 * @param data Arbitrary user data
 * @param ops The list of operations to be used
 * @return >=0 on success < on failure; errno is set
 */
int eh_ctx_hook_alloc(eh_ctx_st *dst, int fd, void *data, const eh_hook_ft (*ops)[EH_OPS_MAX]);
/**
 * @brief Find a registered hook by its file descriptor
 *
 * @param ctx Working context
 * @param fd File descriptor previously used
 * @return eh_hook_st*
 */
eh_hook_st *eh_ctx_get_hook_by_fd(eh_ctx_st *ctx, int fd);

/**
 * @brief Create a new epoll hook.
 *
 * @param dst The location where the hook will be stored
 * @param fd The file descriptor to wait on
 * @param data An arbitrary void pointer passed in to eh_hook_ft functions
 * @param ops an array of hook functions and their associated operation
 * @returns >= 0 on success, -1 on failure. errno is also set by epoll_create1
 */
int eh_hook_alloc(eh_hook_st **dst, int fd, void *data, const eh_hook_ft (*ops)[EH_OPS_MAX]);
/**
 * @brief Modify a hook to use a new callback for a particular operation
 *
 * @param hook Working hook
 * @param op Selected operation the user wants to swap out
 * @param fn A callback function to use, or NULL to clear the operation
 * @return >= 0 on success, -1 on failure. errno is also set by epoll_ctl.
 */
int eh_hook_mod_set_cbf(eh_hook_st *hook, eh_ops_et op, eh_hook_ft fn);
/**
 * @brief Modify a hook to use a different pointer
 *
 * @param hook Working hook
 * @param data Arbitrary user data
 */
void eh_hook_set_data(eh_hook_st *hook, void *data);
/**
 * @brief Get user data
 *
 * @param hook Working hook
 * @return Previously set user data
 */
void *eh_hook_get_data(const eh_hook_st *hook);
/**
 * @brief Get watched fd
 *
 * @param hook Working hook
 * @return The fd
 */
int eh_hook_get_fd(const eh_hook_st *hook);
/**
 * @brief Get currently set operations
 *
 * @param ops A pointer to where the ops should be written
 */
void eh_hook_get_ops(const eh_hook_st *hook, eh_hook_ft (*dst)[EH_OPS_MAX]);
/**
 * @brief __attribute((cleanup())) safe implementation
 *
 * @param dst Any hook (even NULL, or failed allocation)
 */
void eh_hook_cleanup(eh_hook_st **dst);
