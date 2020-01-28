/**
 * core/utils/dbgprint.h
 *
 * Functions and macros that pretty-print things automatically
 *
 * @author Connor Henley, @thatging3rkid
 */
#ifndef __CORE_UTILS_DBGPRINT
#define __CORE_UTILS_DBGPRINT

// allow C++ to parse this
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

/**
 * Print an info string to stderr with line and file info
 *
 * @param str the string to be printed after debugging info
 * @note MUST be a macro to expand correctly
 * @note newline is not printed
 */
#define dbg_info(str...) fprintf(stderr, "[inf][%s:%d] ", __FILE__, __LINE__); fprintf(stderr, str);

/**
 * Print a warning string to stderr with line and file info
 *
 * @param str the string to be printed after debugging info
 * @note: MUST be a macro to expand correctly
 * @note newline is not printed
 */
#define dbg_warning(str...) fprintf(stderr, "[wrn][%s:%d] ", __FILE__, __LINE__); fprintf(stderr, str);

/**
 * Print an error string to stderr with file and line info
 *
 * @param str the string to print after debugging info
 * @note: MUST be a macro to expand correctly
 * @note newline is not printed
 */
#define dbg_error(str...) fprintf(stderr, "[err][%s:%d] ", __FILE__, __LINE__); fprintf(stderr, str);

#ifdef __cplusplus
}
#endif

#endif
