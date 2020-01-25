/**
 * core/network/lowlevel.c
 *
 * Handles OS-level interactions for networking
 *
 * @author Connor Henley, @thatging3rkid
 */
// standards
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

// networking
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>

// local things
#if _LOCAL_HEADER
    #include "lowlevel.h"
    #include "constants.h"
#else
    #include "core/network/lowlevel.h"
    #include "core/network/constants.h"
#endif



