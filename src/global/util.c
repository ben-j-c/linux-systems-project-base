#include "util.h"

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