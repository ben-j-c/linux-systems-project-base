#include <errno.h>
#include <fcntl.h>
#include <pty.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/inotify.h>
#include <sys/stat.h>
#include <unistd.h>

#include "args.h"
#include "data-structures/hashtable.h"
#include "epoll_hook.h"
#include "errstack.h"
#include "util.h"

struct prog_state_s
{
	eh_ctx_st *epoll_ctx;
};

static void _state_cleanup(struct prog_state_s *state)
{
	eh_ctx_cleanup(&state->epoll_ctx);
}

static int _event_loop(struct prog_state_s *state, struct arg_spec_s *args)
{
	printf("Arg given %s\n", args->example_arg);
	for (;;) {
		ES_FWD_INT_NM(eh_ctx_wait(state->epoll_ctx, 100, 10));
	}
	return 0;
}

int _some_fun2(void)
{
	ES_NEW("Some other error");
	return -1;
}

int _some_fun1(void)
{
	ES_FWD_INT(_some_fun2(), "Some other error");
	return 1;
}

int _some_fun(void)
{
	ES_FWD_INT(_some_fun1(), "Some other error");
	return 1;
}

static int _on_stdin(UNUSED eh_ctx_st *ctx, eh_hook_st *hook, UNUSED bool ops[EH_OPS_MAX])
{
	ssize_t n_read        = 0;
	char buff[BIG_BUF_SZ] = {};
	if (!ops[EH_OPS_IN]) {
		return 1;
	}
	while ((n_read = read(eh_hook_get_fd(hook), buff, sizeof(buff))) > 0) {
		buff[n_read] = '\0';
		printf("Got values: %s\n", buff);
		if (_some_fun() < 0) {
			ES_PRINT();
		}
	};
	if (n_read < 0 && errno != EWOULDBLOCK) {
		ES_NEW_ASRT_ERRNO(n_read);
	}
	return 1;
}

static int _pipeline(int argc, char **argv)
{
	puts("starting pipeline");
	struct arg_spec_s args                            = {};
	CLEANUP(_state_cleanup) struct prog_state_s state = {};
	ES_FWD_INT(process_args(&args, argc, argv), "Failed to process args.");
	ES_FWD_INT(eh_ctx_alloc(&state.epoll_ctx, false, false),
	           "Failed to allocate new epoll context");
	ES_NEW_INT_ERRNO(
	    fcntl(STDIN_FILENO, F_SETFD, ES_NEW_INT_ERRNO(fcntl(STDIN_FILENO, F_GETFD) | O_NONBLOCK)));
	ES_NEW_INT_ERRNO(fcntl(STDOUT_FILENO,
	                       F_SETFD,
	                       ES_NEW_INT_ERRNO(fcntl(STDOUT_FILENO, F_GETFD) | O_NONBLOCK)));
	ES_FWD_INT(eh_ctx_hook_alloc(state.epoll_ctx,
	                             0,
	                             NULL,
	                             (eh_ops_mapping_st[]){
	                                 {
	                                     .fn = _on_stdin,
	                                     .op = EH_OPS_ALL,
	                                 },
	                                 {
	                                     .op = EH_OPS_END,
	                                 },
	                             }),
	           "Failed to make stdin hook");
	ES_FWD_INT(_event_loop(&state, &args), "Failure in event loop.");
	return 0;
}

int main(int argc, char **argv)
{
	int retval = -1;
	/* Execute program */
	retval = _pipeline(argc, argv);
	if (retval < 0) {
		ES_PUSH("Pipeline failed");
		printf("Unrecoverable: [ ");
		ES_PRINT();
		printf("\n ]\n");
		return -1;
	}
	return 0;
}