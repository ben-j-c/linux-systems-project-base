#define _GNU_SOURCE
#include "errstack.h"
/**
 * Copyright by Benjamin Joseph Correia.
 * Date: 2022-08-11
 * License: MIT
 *
 * Description:
 * This is an implementation for an error logging system.
 */

#include <bsd/string.h>
#include <stdarg.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/sendfile.h>
#include <unistd.h>

#include "util.h"

#ifdef ES_NO_DEBUG

#elif !defined(ES_BUFFER_BACKED)
__thread ssize_t es_bytes = 0;
__thread int es_fd        = -1;
#else
__thread char es_error_stack[1 << 16] = {0};
__thread char *es_end                 = NULL;
__thread char *es_start               = NULL;
#	define ESPRINTF(f, n, args...) vsprintf(f, ##args)
#	if _BSD_SOURCE || _XOPEN_SOURCE >= 500 || _ISOC99_SOURCE || _POSIX_C_SOURCE >= 200112L
#		undef ESPRINTF
#		define ESPRINTF(f, n, args...) vsnprintf(f, n, ##args)
#	endif
#endif

#ifdef ES_NO_DEBUG

void es_reset(void)
{
	;
}

void es_print(void)
{
	printf("Error traces disabled at compile time. This should not be seen\n");
}

#elif !defined(ES_BUFFER_BACKED)
static void _append(const char *format, va_list args)
{
	int n_bytes = 0;
	if (es_fd < 0) {
		es_bytes = 0;
		es_fd    = memfd_create("errstack", MFD_CLOEXEC);
		if (es_fd < 0) {
			return;
		}
	}
	n_bytes = vdprintf(es_fd, format, args);
	if (n_bytes > 0) {
		es_bytes += n_bytes;
	}
}

void es_reset(void)
{
	int ret;
	if (es_fd < 0) {
		return;
	}
again:
	ret = ftruncate(es_fd, 0);
	if (ret < 0 && errno != EINTR) {
		es_fd = -1;
		return;
	} else if (ret < 0) {
		goto again;
	}
}

void es_print(void)
{
	char buff[1 << 10];
	ssize_t ret;
	fflush(stdout);
	lseek(es_fd, 0, SEEK_SET);
	while (es_bytes > 0) {
		const ssize_t expected = MIN((ssize_t) sizeof(buff), es_bytes);
		ssize_t n_bytes;
again:
		n_bytes = read(es_fd, buff, expected);
		if (n_bytes < 0) {
			if (errno == EINTR) {
				goto again;
			}
again2:
			ret = close(es_fd);
			if (ret < 0 && errno == EINTR) {
				goto again2;
			}
			es_fd = -1;
			return;
		}
again3:
		ret = write(STDOUT_FILENO, buff, n_bytes);
		if (ret < 0 && errno == EINTR) {
			goto again3;
		}
		es_bytes -= expected;
	}
	return;
}

#else

static void _append(const char *format, va_list args)
{
	int n_bytes = 0;
	if (!es_end)
		es_end = &(es_error_stack[sizeof(es_error_stack) - 1]);
	if (!es_start)
		es_start = es_error_stack;

	n_bytes = ESPRINTF(es_start, (size_t) es_end - (size_t) es_start, format, args);
	es_start += n_bytes;
	if ((size_t) es_end < (size_t) es_start) {
		es_start = es_end;
	}
}

void es_reset(void)
{
	es_start          = es_error_stack;
	es_error_stack[0] = '\0';
}

void es_print(void)
{
	printf("%s", es_error_stack);
}
#endif

#ifdef ES_NO_DEBUG

void es_append(UNUSED const char *format, ...)
{
	;
}

#else

void es_append(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	_append(format, args);
	va_end(args);
}

#endif
