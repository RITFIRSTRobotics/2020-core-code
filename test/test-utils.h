/**
 * core/test/test-utils.h
 *
 * Utility macros for testing code
 *
 * @author Connor Henley, @thatging3rkid
 */
#ifndef __CORE_TEST_UTILS
#define __CORE_TEST_UTILS

// allow C++ to parse this
#ifdef __cplusplus
extern "C" {
#endif

#include <unistd.h>

#define TEST_SUCCESS 0
#define TEST_FAILURE 1

/**
 * Returns the number of elements in a static array
 *
 * @macro
 * @param a the array
 */
#define num_elements(a) (sizeof(a)/sizeof(a[0])) // returns the number of elements in the array

/**
 * Sleeps for a specified amount of time
 *
 * @macro
 * @param t the length of time to sleep for (in milliseconds)
 */
#define msleep(t) usleep(t*1000)

#ifdef __cplusplus
}
#endif

#endif
