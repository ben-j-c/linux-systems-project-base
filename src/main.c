#include <data-structures/vec.h>
#include <errno.h>
#include <fcntl.h>
#include <pty.h>
#include <signal.h>
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
#include "errstack.h"
#include "util.h"

struct prog_state_s
{
	int epoll;
};

static void _state_cleanup(struct prog_state_s *state)
{
	if (state->epoll >= 0) {
		close(state->epoll);
	}
}

static int _event_loop(struct prog_state_s *state, struct arg_spec_s *args)
{
	/* System */
	struct epoll_event ev = {0};
	int nfds              = 0;

	printf("Arg given %s\n", args->example_arg);

	while (({
		nfds = epoll_wait(state->epoll, &ev, 1, 10);
		nfds != -1;
	})) {
		if (nfds > 0) {
			ES_ERR("Didn't expect any epoll events.");
			return -1;
		}
	}
	ES_ERR_ASRT(nfds >= 0, "errno: %s", strerror(errno));
	return 0;
}

static int _pipeline(int argc, char **argv)
{
	struct arg_spec_s args                            = {};
	CLEANUP(_state_cleanup) struct prog_state_s state = {};
	ES_PUSH_INT(process_args(&args, argc, argv), "Failed to process args.");
	ES_ERR_INT_NM(state.epoll = epoll_create1(0));
	ES_PUSH_INT(_event_loop(&state, &args), "Failure in event loop.");
	return 0;
}

struct some_struct_s
{
	int32_t a;
	int64_t b;
};

int main(int argc, char **argv)
{
	int retval = -1;
	/* Execute program */
	retval = _pipeline(argc, argv);
	if (retval < 0) {
		ES_PUSH("Pipeline failed");
		printf("Unrecoverable: [ %s\n ]\n", ES_DUMP());
		return -1;
	}
	return 0;
}