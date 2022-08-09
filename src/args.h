#pragma once
#include "util.h"

struct arg_spec_s
{
	char example_arg[BIG_BUF_SZ];
};

int process_args(struct arg_spec_s *dst, int argc, char **argv);