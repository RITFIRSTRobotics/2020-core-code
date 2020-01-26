/**
 * core/network/lowlevel.h
 *
 * Defines functions that interface directly with the OS network API
 *
 * @author Connor Henley, @thatging3rkid
 */
#ifndef __CORE_NETWORK_LOW_LEVEL_NET
#define __CORE_NETWORK_LOW_LEVEL_NET

// allow C++ to parse this
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

// Define an enum that keeps track of the current state of a listener
typedef enum ListenerStatus {
    ls_OKAY         = 0x00,
    ls_DISCONNECTED = 0x01
} ListenerStatus_t;

typedef enum NetworkProtocol {
    np_TCP,
    np_UDP
} NetworkProtocol_t;

// Defines a structure to store a minimally decoded packet
typedef struct IntermediateTLV {
    uint32_t type:8;
    uint32_t length:24;
    uint8_t* data;
} IntermediateTLV_t;

// Defines a structure to store connection information
typedef struct NetworkConnection {
    // Shared connection data
    void (*handler)(IntermediateTLV_t*);
    struct sockaddr client_addr;
    socklen_t client_addr_len;

    // TCP connection data
    int tcp_fd;
    ListenerStatus_t* tcp_status;

    // UDP connection data
    int udp_fd;
    ListenerStatus_t* udp_status;

} NetworkConnection_t;

/**
 * Starts a thread that accepts new TCP connections. After a new connection is
 * started, the thread sets up the data structure for this connection and starts
 * the listeners for TCP and UDP.
 *
 * @param (*on_connect) the handler function that is called after every new
 *        connection. The function is passed a pointer to a structure that can
 *        contains connection data and can be used to send a packet.
 * @param (*on_packet) the handler function that gets called on every incoming
 *        packet. This function is passed a pointer to a TLV structure that
 *        contains the recieved data.
 * @note this should only be run on the server-side of the connection (i.e. FMS)
 */
void llnet_acceptor_start(void (*on_connect)(NetworkConnection_t*),
    void (*on_packet)(IntermediateTLV_t*));

/**
 * Sets up a listener to listen to TCP packets
 *
 * @param connection the connection that the listener should listen on. The
 *        given structure should be fully populated with at least a TCP file
 *        descriptor and a handler function.
 */
void llnet_listener_tcp_start(NetworkConnection_t* connection);

/**
 * Sets up a listener to listen to UDP packets
 *
 * @param connection the connection that the listener should listen on. The
 *        given structure should be fully populated with at least a UDP file
 *        descriptor, a handler function. and a client address.
 */
void llnet_listener_udp_start(NetworkConnection_t* connection);

/**
 * Sends a packet over the network
 *
 * @param connection the connection to send the packet out using (e.g. which robot
 *        should the packet be sent to)
 * @param proto the protocol to use (TCP vs UDP); this should match the definition
 *        in the control protocol.
 * @param packet the packet to send out, in IntermediateTLV form
 * @param new_thread if true, this send will be done in a new thread
 */
void llnet_sender_send(NetworkConnection_t* connection, NetworkProtocol_t proto,
    IntermediateTLV_t packet, bool new_thread);

#ifdef __cplusplus
}
#endif

#endif
