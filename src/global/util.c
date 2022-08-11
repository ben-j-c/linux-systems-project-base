#include "util.h"
/**
 * Original author: Benjamin Correia (ben-j-c@github.com)
 * Date: 2022-08-11
 * License: MIT
 *
 * Description:
 * This is a utilities file. It defines several useful defines that leverage language features and
 * GCC specific features.
 */

#include <stdio.h>
#include <unistd.h>

void cleanup_file(FILE **f)
{
	if (*f) {
		fclose(*f);
	}
	*f = NULL;
}

void cleanup_fd(int *fd)
{
	if (*fd >= 0) {
		close(*fd);
		*fd = -1;
	}
}