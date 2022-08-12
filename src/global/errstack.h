#pragma once
#include <errno.h>

void es_append(const char *format, ...);
void es_reset(void);
void es_print(void);

/**
 * ES_XXX_YYY_ZZZ
 *
 * XXX:
 *    - NEW: Reset error stack and push new error
 *    - FWD: push new error
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
#	define ES_NEW_INT(statement, fmt, args...)                                                    \
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
#	define ES_FWD_INT(statement, fmt, args...)                                                    \
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
#	define ES_NEW(fmt, args...)                                                                   \
		({                                                                                         \
			es_reset();                                                                            \
			es_append("(%d @ %s:%d \"" fmt "\" )", -1, __FUNCTION__, __LINE__, ##args);            \
		})

/* Push message */
#	define ES_PUSH(fmt, args...)                                                                  \
		({ es_append("\n    (%d @ %s:%d \"" fmt "\" )", -1, __FUNCTION__, __LINE__, ##args); })

#else
#	define ES_NEW_INT(statement, fmt, args...)                                                    \
		({                                                                                         \
			int _st_value = (statement);                                                           \
			if ((_st_value) < 0) {                                                                 \
				return _st_value;                                                                  \
			}                                                                                      \
			_st_value;                                                                             \
		})
#	define ES_FWD_INT(statement, fmt, args...)                                                    \
		({                                                                                         \
			int _st_value = (statement);                                                           \
			if ((_st_value) < 0) {                                                                 \
				return _st_value;                                                                  \
			}                                                                                      \
			_st_value;                                                                             \
		})
#	define ES_NEW_PUT(fmt, args...) ({ ; })
#	define ES_FWD_PUT(fmt, args...) ({ ; })
#endif

#define ES_NEW_ASRT(statement, fmt, ...) ES_NEW_INT((statement) ? 0 : -1, fmt, ##__VA_ARGS__)
#define ES_FWD_ASRT(statement, fmt, ...) ES_FWD_INT((statement) ? 0 : -1, fmt, ##__VA_ARGS__)

#define ES_NEW_INT_NM(statement)  ES_NEW_INT(statement, "")
#define ES_FWD_INT_NM(statement)  ES_FWD_INT(statement, "")
#define ES_NEW_ASRT_NM(statement) ES_NEW_ASRT(statement, "")
#define ES_FWD_ASRT_NM(statement) ES_FWD_ASRT(statement, "")
#define ES_NEW_NM()               ES_NEW("")
#define ES_FWD_NM()               ES_PUSH("")

#define ES_NEW_INT_ERRNO(statement)  ES_NEW_INT(statement, "errno %d: %s", errno, strerror(errno))
#define ES_FWD_INT_ERRNO(statement)  ES_FWD_INT(statement, "errno %d: %s", errno, strerror(errno))
#define ES_NEW_ASRT_ERRNO(statement) ES_NEW_ASRT(statement, "errno %d: %s", errno, strerror(errno))
#define ES_FWD_ASRT_ERRNO(statement) ES_FWD_ASRT(statement, "errno %d: %s", errno, strerror(errno))
#define ES_NEW_ERRNO()               ES_NEW("errno %d: %s", errno, strerror(errno))

#define ES_PRINT() ({ es_print(); })

#define ES_SUCCESS() ({ return 0; })