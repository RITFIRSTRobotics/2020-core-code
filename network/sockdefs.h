/**
 * core/network/sockdefs.h
 *
 * Socket definitions and constants
 *
 * @author Connor Henley, @thatging3rkid
 */
#ifndef __CORE_NETWORK_SOCKDEFS
#define __CORE_NETWORK_SOCKDEFS

// allow C++ to parse this
#ifdef __cplusplus
extern "C" {
#endif

// Defines a structure to contain TCP socket info
typedef struct SocketTCP {
    int fd;
} SocketTCP_t;

// Defines a structure to contain UDP socket info
typedef struct SocketUDP {
    int fd;
} SocketUDP_t;


#ifdef __cplusplus
}
#endif

#endif
