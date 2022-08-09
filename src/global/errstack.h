#pragma once
#include <errno.h>

void es_append(const char *format, ...);
const char *es_dump(void);
void es_reset(void);

/**
 * ES_XXX_YYY_ZZZ
 *
 * XXX:
 *    - ERR: Reset error stack and push new error
 *    - PUSH: push new error
 * YYY:
 *    - INT: Check if statement < 0, if so follow XXX and then return
 *    - ASRT: Check if statement != 0, if so follow XXX and then return
 *
 * ZZZ:
 *    - NM: No format string available
 *    - ERRNO: Format string should be errno and its strerror result
 */

#ifndef ES_NO_DEBUG
/* Reset error stack, push message, and return with error value */
#	define ES_ERR_INT(statement, fmt, args...)                                                    \
		({                                                                                         \
			int _st_value = (statement);                                                           \
			if ((_st_value) < 0) {                                                                 \
				es_reset();                                                                        \
				es_append("(%d (origin) @ %s:%d \"" fmt "\" )",                                    \
				          _st_value,                                                               \
				          __FUNCTION__,                                                            \
				          __LINE__,                                                                \
				          ##args);                                                                 \
				return _st_value;                                                                  \
			}                                                                                      \
			_st_value;                                                                             \
		})

/* push message, and return with error value*/
#	define ES_PUSH_INT(statement, fmt, args...)                                                   \
		({                                                                                         \
			int _st_value = (statement);                                                           \
			if ((_st_value) < 0) {                                                                 \
				es_append("\n    (%d @ %s:%d \"" fmt "\" )",                                       \
				          _st_value,                                                               \
				          __FUNCTION__,                                                            \
				          __LINE__,                                                                \
				          ##args);                                                                 \
				return _st_value;                                                                  \
			}                                                                                      \
			_st_value;                                                                             \
		})

/* Reset error stack and push message */
#	define ES_ERR(fmt, args...)                                                                   \
		({                                                                                         \
			es_reset();                                                                            \
			es_append("(%d @ %s:%d \"" fmt "\" )", -1, __FUNCTION__, __LINE__, ##args);            \
		})

/* Push message */
#	define ES_PUSH(fmt, args...)                                                                  \
		({ es_append("\n    (%d @ %s:%d \"" fmt "\" )", -1, __FUNCTION__, __LINE__, ##args); })

#else
#	define ES_ERR_INT(statement, fmt, args...)                                                    \
		({                                                                                         \
			int _st_value = (statement);                                                           \
			if ((_st_value) < 0) {                                                                 \
				return _st_value;                                                                  \
			}                                                                                      \
			_st_value;                                                                             \
		})
#	define ES_PUSH_INT(statement, fmt, args...)                                                   \
		({                                                                                         \
			int _st_value = (statement);                                                           \
			if ((_st_value) < 0) {                                                                 \
				return _st_value;                                                                  \
			}                                                                                      \
			_st_value;                                                                             \
		})
#	define ES_ERR_PUT(fmt, args...)  ({ ; })
#	define ES_PUSH_PUT(fmt, args...) ({ ; })
#endif

#define ES_ERR_ASRT(statement, fmt, ...)  ES_ERR_INT((statement) ? 0 : -1, fmt, ##__VA_ARGS__)
#define ES_PUSH_ASRT(statement, fmt, ...) ES_PUSH_INT((statement) ? 0 : -1, fmt, ##__VA_ARGS__)

#define ES_ERR_INT_NM(statement)   ES_ERR_INT(statement, "")
#define ES_PUSH_INT_NM(statement)  ES_PUSH_INT(statement, "")
#define ES_ERR_ASRT_NM(statement)  ES_ERR_ASRT(statement, "")
#define ES_PUSH_ASRT_NM(statement) ES_PUSH_ASRT(statement, "")
#define ES_ERR_NM()                ES_ERR("")
#define ES_PUSH_NM()               ES_PUSH("")

#define ES_ERR_INT_ERRNO(statement)  ES_ERR_INT(statement, "errno %d: %s", errno, strerror(errno))
#define ES_PUSH_INT_ERRNO(statement) ES_PUSH_INT(statement, "errno %d: %s", errno, strerror(errno))
#define ES_ERR_ASRT_ERRNO(statement) ES_ERR_ASRT(statement, "errno %d: %s", errno, strerror(errno))
#define ES_PUSH_ASRT_ERRNO(statement)                                                              \
	ES_PUSH_ASRT(statement, "errno %d: %s", errno, strerror(errno))
#define ES_ERR_ERRNO() ES_ERR("errno %d: %s", errno, strerror(errno))

#define ES_DUMP() ({ es_dump(); })

#define ES_SUCCESS() ({ return 0; })