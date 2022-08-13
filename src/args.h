#pragma once
/**
 * Copyright by Benjamin Joseph Correia.
 * Date: 2022-08-11
 * License: MIT
 *
 * Description:
 * This is a barebones implementation for an argument parser with one example option.
 */

#include "util.h"

struct arg_spec_s
{
	char example_arg[BIG_BUF_SZ];
};

int process_args(struct arg_spec_s *dst, int argc, char **argv);