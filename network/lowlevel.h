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

// Defines the possible protocols that can be used to send a packet
typedef enum NetworkProtocol {
    np_TCP,
    np_UDP
} NetworkProtocol_t;

// Defines the possible states of a connection (unconnected, listening, connected)
typedef enum ConnectionState {
    cs_NOTHING,
    cs_LISTENER,
    cs_CONNECTED
} ConnectionState_t;

// Defines a structure to store a minimally decoded packet
typedef struct IntermediateTLV {
    uint32_t type:8;
    uint32_t length:24;
    uint8_t* data;
} IntermediateTLV_t;

// Defines an "abstract" structure to store connection information
typedef struct NetworkConnection {
    // Shared fields
    ConnectionState_t state;
    int tcp_fd;
    int udp_fd;
} NetworkConnection_t;

// Defines a structure to store connection information for clients
typedef struct ClientNetworkConnection {
    // "inherited"
    ConnectionState_t state;
    int tcp_fd;
    int udp_fd;

    // incoming packet handler
    void (*handler)(IntermediateTLV_t*);

    // server address (used for TCP and UDP)
    struct sockaddr_in server_addr;
    socklen_t server_addr_len;
} ClientNetworkConnection_t;

// Defines a structure to store connection information for servers
typedef struct ServerNetworkConnection {
    // "inherited"
    ConnectionState_t state;
    int tcp_fd;
    int udp_fd;

    // incoming connection handler
    void (*connected)(ClientNetworkConnection_t*);

    // TCP listener
    ListenerStatus_t* tcp_status;

    // UDP listener
    ListenerStatus_t* udp_status;
    struct sockaddr_in client_addr;
    socklen_t client_addr_len;
} ServerNetworkConnection_t;

/**
 * Initializes a network connection data structure
 *
 * @return the "abstract" connection data structure
 */
NetworkConnection_t* llnet_connection_init();

/**
 * Connects the client to a server. This process converts the connection to a 
 * client connection, connects to the server over TCP and configures the UDP
 * socket for communication with the server.
 *
 * @param connection the network connection to use to connect. This structure
 *        will be converted to a ClientNetworkConnection structure, it's memory
 *        will be realloc'd by this function and returned.
 * @param host a C-string representing the host to connect to. This can be an
 *        IP address (i.e "1.2.3.4") or a domain name that requires DNS 
 *        (i.e "google.com").
 * @param (*handler) the handler function that gets called on every incoming
 *        packet. This function is passed a pointer to a TLV structure that
 *        contains the recieved data.
 * @note If the router is configured as a DNS server (and is used as the DNS
 *       server) where DHCP connections are registered as DNS entries, this will 
 *       allow hostnames ("ritfirst-fms") to successfully lookup.
 * @returns the converted network connection structure.
 */
ClientNetworkConnection_t* llnet_connection_connect(NetworkConnection_t* connection,
    char* host, void (*handler)(IntermediateTLV_t*));

/**
 * Sends a packet over the network
 *
 * @param connection the connection to send the packet out using (e.g. which robot
 *        should the packet be sent to). Can be sent either with a connected client
 *        network connection or a listening server network connection.
 * @param proto the protocol to use (TCP vs UDP); this should match the definition
 *        in the control protocol.
 * @param packet the packet to send out, in IntermediateTLV form
 * @param new_thread if true, this send will be done in a new thread
 * @returns the error code from the failed OS interaction, if one occured. A result
 *          of zero indicates success.
 */
uint32_t llnet_connection_send(NetworkConnection_t* connection, NetworkProtocol_t proto, 
    IntermediateTLV_t packet, bool new_thread);

/**
 * Sets this network connection to a listening connection and starts up listener
 * threads. This is done by reconfiguring the TCP and UDP sockets as necessary,
 * spinning up an acceptor thread (which accepts new connections on TCP), and
 * spinning up a TCP and UDP listener thread.
 *
 * @param (*on_connect) the handler function that is called after every new
 *        connection. The function is passed a pointer to a structure that can
 *        contains connection data and can be used to send a packet.
 * @param (*on_packet) the handler function that gets called on every incoming
 *        packet. This function is passed a pointer to a TLV structure that
 *        contains the recieved data.
 * @note this should only be run on the server-side of the connection (i.e. FMS)
 *       but this code resides in core so that it can be tested with the rest 
 *       of the low-level networking code.
 */
void llnet_connection_listen(void (*on_connect)(ClientNetworkConnection_t*),
    void (*on_packet)(IntermediateTLV_t*));

/**
 * Cleans up the network connection
 *
 * @param connection the network connection structure to clean up
 */
void llnet_connection_free(NetworkConnection_t* connection);

#ifdef __cplusplus
}
#endif

#endif
