/**
 * core/utils/bounds.h
 *
 * Macros/functions used in array-bounding
 *
 * @author Connor Henley, @thatging3rkid
 * @note see https://stackoverflow.com/questions/3437404/min-and-max-in-c/3437484#3437484
 */
#ifndef __CORE_UTILS_BOUNDS
#define __CORE_UTILS_BOUNDS

/**
 * Finds the minimum of the two values
 *
 * @macro
 * @param a the first value
 * @param b the second value
 */
#define min(a,b) ({ __typeof__ (a) _a = (a); \
                    __typeof__ (b) _b = (b); \
                    _a < _b ? _a : _b; })

/**
 * Finds the maximum of the two values
 *
 * @macro
 * @param a the first value
 * @param b the second value
 */
#define max(a,b) ({ __typeof__ (a) _a = (a); \
                    __typeof__ (b) _b = (b); \
                    _a > _b ? _a : _b; })

#endif
