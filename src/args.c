#include "args.h"

#include <argp.h>
#include <errno.h>
#include <unistd.h>

#include "errstack.h"

static error_t _parse_opt_x(UNUSED int key, char *arg, struct argp_state *state)
{
	struct arg_spec_s *spec = state->input;
	if (!spec) {
		ES_NEW_NM();
		return ENOTRECOVERABLE;
	}
	if (arg) {
		STRLCPY(spec->example_arg, arg);
	} else if (state->next < state->argc) {
		STRLCPY(spec->example_arg, state->argv[state->next++]);
	} else {
		ES_NEW("Argument for -x not present");
		return ENOTRECOVERABLE;
	}
	return 0;
}

static error_t _on_end_opt_x(UNUSED struct argp_state *state)
{
	return 0;
}

static error_t (*opt_func[])(int key, char *arg, struct argp_state *state) = {
    ['x'] = _parse_opt_x,
};

static error_t (*opt_on_end_func[])(struct argp_state *state) = {
    _on_end_opt_x,
};

static error_t _parse_opt(int key, char *arg, struct argp_state *state)
{
	error_t retval = 0;
	size_t i;
	switch (key) {
	case ARGP_KEY_ARG:
		break;
	case ARGP_KEY_ARGS:
		break;
	case ARGP_KEY_END:
		for (i = 0; i < ARRAY_SIZE(opt_on_end_func); i++) {
			retval = opt_on_end_func[i](state);
			if (retval != 0) {
				ES_PUSH("Failed to run argument process ending function.");
				return retval;
			}
		}
		break;
	case ARGP_KEY_NO_ARGS:
		break;
	case ARGP_KEY_INIT:
		break;
	case ARGP_KEY_SUCCESS:
		break;
	case ARGP_KEY_ERROR:
		break;
	case ARGP_KEY_FINI:
		break;
	default:
		if (key < 0 || key >= (int) ARRAY_SIZE(opt_func) || opt_func[key] == NULL) {
			return ARGP_ERR_UNKNOWN;
		}
		retval = opt_func[key](key, arg, state);
		if (retval != 0) {
			ES_PUSH("Failed to run argument process function.");
		}
	}
	return retval;
}

int process_args(struct arg_spec_s *dst, int argc, char **argv)
{
	struct argp_option opts[] = {
	    {
	        .name  = "example-arg",
	        .key   = 'x',
	        .arg   = "EXAMPLE_ARG_NAME",
	        .doc   = "EXAMPLE description",
	        .flags = OPTION_ARG_OPTIONAL,
	    },
	    {0},
	};
	struct argp spec = {
	    .args_doc = "Example program description",
	    .options  = opts,
	    .parser   = _parse_opt,
	};
	error_t retval = argp_parse(&spec, argc, argv, 0, NULL, dst);
	ES_FWD_ASRT(retval == 0, "Error parsing arguments %d(%s)", retval, strerror(retval));
	return 0;
}