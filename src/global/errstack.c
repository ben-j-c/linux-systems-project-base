#include "errstack.h"

#include <bsd/string.h>
#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>

__thread char es_error_stack[1 << 16] = {0};
__thread char *es_end                 = NULL;
__thread char *es_start               = NULL;

#define ESPRINTF(f, n, args...) vsprintf(f, ##args)
#if _BSD_SOURCE || _XOPEN_SOURCE >= 500 || _ISOC99_SOURCE || _POSIX_C_SOURCE >= 200112L
#	undef ESPRINTF
#	define ESPRINTF(f, n, args...) vsnprintf(f, n, ##args)
#endif

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

void es_append(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	_append(format, args);
	va_end(args);
}

const char *es_dump(void)
{
	return es_error_stack;
}