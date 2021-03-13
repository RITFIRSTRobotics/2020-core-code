/**
 * dbgcommit.h
 *
 * Header file consisting of build information
 *
 * @note include this file in your source code, it will ensure everything is defined
 *
 * @author Connor Henley, cxh1451@rit.edu
 */
#ifndef __CORE_UTILS_DBGCOMMIT
#define __CORE_UTILS_DBGCOMMIT

#include "dbgcommit.gen.h" // NOTE: do not include this file in your source code,
                           // it is not guaranteed to exist

// allow C++ to parse this
#ifdef __cplusplus
extern "C" {
#endif

// defines the different types of builds
typedef enum {
    DBG_BT_ONPUSH,
    DBG_BT_FULL,
    DBG_BT_DEV
} DBG_BuildType_t;

/**
 * GIT_COMMIT_HASH
 *
 * Defines a 28-bit integer that represents the git commit hash of the current
 * build. Will be defined as 0xfffffff when an error occurs.
 */
#ifndef GIT_COMMIT_HASH
    #define GIT_COMMIT_HASH (0xfffffff)
#endif

/**
 * DBG_BUILD_TYPE
 *
 * Defines the current build type (ONPUSH=shortened automated build (currently
 * does not build docs), FULL=full automated build, DEV=development).
 */
#ifndef DBG_BUILD_TYPE
    #define DBG_BUILD_TYPE (DBG_BT_DEV)
#endif

#ifdef __cplusplus
}
#endif

#endif
