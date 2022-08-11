#pragma once
/**
 * Copyright by Benjamin Joseph Correia.
 * Date: 2022-08-11
 * License: MIT
 *
 * Description:
 * This is a utilities file. It defines several useful defines that leverage language features and
 * GCC specific features.
 */
#include <bsd/string.h>
#include <stdbool.h>
#include <stdio.h>

#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))
#define STRLCPY(dst, src) strlcpy(dst, src, ARRAY_SIZE(dst))
#define BIG_BUF_SZ        (256)
#define MID_BUF_SZ        (128)
#define SMALL_BUF_SZ      (64)

#define CONCAT_(A, B) A##B
#define CONCAT(A, B)  CONCAT_(A, B)

/*Move a value from Y to rvalue, Y will be zeroed. Useful for cleanup functions.*/
#define MOVE_VZ(Y)                                                                                 \
	({                                                                                             \
		__typeof__(Y) _y = Y;                                                                      \
		Y                = 0;                                                                      \
		_y;                                                                                        \
	})
/*Move a value from Y to rvalue, Y will be set to -1. Useful for cleanup functions.*/
#define MOVE_VN(Y)                                                                                 \
	({                                                                                             \
		__typeof__(Y) _y = Y;                                                                      \
		Y                = -1;                                                                     \
		_y;                                                                                        \
	})
/*Move a pointer from Y to rvalue, Y will be NULL. Useful for cleanup functions.*/
#define MOVE_PZ(Y)                                                                                 \
	({                                                                                             \
		__typeof__(Y) _y = Y;                                                                      \
		Y                = NULL;                                                                   \
		_y;                                                                                        \
	})

#ifdef __GNUC__
#	define CLEANUP(X)  __attribute__((cleanup(X)))
#	define CLEAN_FD    __attribute__((cleanup(cleanup_fd)))
#	define CLEAN_FILE  CLEANUP(cleanup_file)
#	define UNUSED      __attribute__((unused))
#	define WARN_UNUSED __attribute_warn_unused_result__
#endif

typedef struct big_buff_s
{
	char str[BIG_BUF_SZ];
} big_buff_t;

void cleanup_file(FILE **f);
void cleanup_fd(int *fd);