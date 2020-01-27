/**
 * core/network/lowlevel.c
 *
 * Handles OS-level interactions for networking
 *
 * @author Connor Henley, @thatging3rkid
 */
#define _DEFAULT_SOURCE // needed for hstrerror(...)
 
// standards
#include <stdio.h>
#include <stdint.h>
#include <memory.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

// networking
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>

// threading
#include <pthread.h>

// errno
#include <errno.h>

// local things
#include "lowlevel.h"
#include "constants.h"
#include "../utils/dbgprint.h"
#include "../collections/arraylist.h"

static ArrayList_t* connections = NULL;

// Argument structure for the acceptor thread
typedef struct {
    int server_fd;
    void (*on_connect)(NetworkConnection_t*);
    void (*on_packet)(IntermediateTLV_t*);
} AcceptorThreadArgs;

/**
 * @inherit
 */
NetworkConnection_t* llnet_connection_init() {
    // Get memory and set state to nothing
    NetworkConnection_t* connection = malloc(sizeof(NetworkConnection_t));
    connection->state = cs_NOTHING;

    // Setup the TCP socket
    connection->tcp_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (connection->tcp_fd < 0) {
        dbg_error("TCP socket creation failed: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Set the SO_REUSEADDR and SO_REUSEPORT flags (because resource leaks *will* happen)
    int opt_value = 1;
    int err = setsockopt(connection->tcp_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt_value, sizeof(opt_value));
    if (err < 0) {
        dbg_error("could not set socket options: %s\n", strerror(errno));
        close(connection->tcp_fd);
        free(connection);
        exit(EXIT_FAILURE); // for now, exit on error
    }

    // Setup the UDP socket
    connection->udp_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (connection->udp_fd < 0) {
        dbg_error("error: UDP socket creation failed: %s\n", strerror(errno));
        close(connection->udp_fd);
        free(connection);
        exit(EXIT_FAILURE); // for now, exit on error
    }

    // all finished
    return connection;    
}

/**
 * @inherit
 */
ClientNetworkConnection_t* llnet_connection_connect(NetworkConnection_t* connection, char* host, void (*handler)(IntermediateTLV_t*)) {
    // Make sure this connection isn't already setup
    if (connection->state != cs_NOTHING) {
        dbg_warning("connection already made (status=%02x), returning...\n", connection->state);
        return (ClientNetworkConnection_t*) connection;
    }

    // Update this structure to a ClientNetworkConnection
    ClientNetworkConnection_t* client = realloc(connection, sizeof(ClientNetworkConnection_t));
    connection = NULL;
    client->handler = handler;
    memset(&client->server_addr, 0, sizeof(struct sockaddr));
    client->server_addr_len = 0;

    // Need to get the address for the given host
    struct hostent* host_addr = gethostbyname(host);
    if (host_addr == NULL) {
        dbg_warning("gethostbyname failed: %s\n", hstrerror(h_errno));
        return client;
    }

// Debugging: print gethostbyname data
#if false
    dbg_info("struct hostent {\n");
    dbg_info("\th_name=%s\n", host_addr->h_name);
    dbg_info("\th_addrtype=%d\n", host_addr->h_addrtype);
    dbg_info("\th_length=%d\n", host_addr->h_length);
    dbg_info("\th_addr_list[0]=%s\n", inet_ntoa(*((struct in_addr*) host_addr->h_addr_list[0])));
    dbg_info("}\n");
#endif

    // Setup the server address
    client->server_addr.sin_family = AF_INET;
    client->server_addr.sin_port = htons(PORT_NUMBER);
    client->server_addr.sin_addr = *((struct in_addr*) host_addr->h_addr_list[0]);

    // Create the TCP connection to the FMS
    int err = connect(client->tcp_fd, (struct sockaddr*) &client->server_addr, sizeof(client->server_addr));
    if (err < 0) {
        dbg_warning("connect failed: %s\n", strerror(errno));
        return client;
    }

    // Connection was successful
    client->state = cs_CONNECTED;

    // TODO start listener threads
    
    return client;
}

//
// TODO continue reworking (all below)
//

/**
 * Accepts incoming connections, setup a data structure, call the on_connect
 * function, and spin up listener threads.
 *
 * @param targs a structure with the pre-configured server socket and handlers
 */
static void* llnet_acceptor_thread(void* _targs) {
    // Setup the accept loop: accept a new connection, notify, spin up new thread
    AcceptorThreadArgs* targs = (AcceptorThreadArgs*) _targs;
    while (true) {
        // Make the data structure
        ClientNetworkConnection_t* client = malloc(sizeof(ClientNetworkConnection_t));
        client->handler = targs->on_packet;
        client->udp_fd = 0;
        
        // Accept the connection
        client->tcp_fd = accept(targs->server_fd, (struct sockaddr*) &client->server_addr, &client->server_addr_len);
        if (client->tcp_fd < 0) {
            fprintf(stderr, "error: could not accept client: %s\n", strerror(errno));
            free(client);
            continue;
        }

        // Call on_connect and add to the connection list
        targs->on_connect((NetworkConnection_t*) client);
        arraylist_add(connections, client);

        // Spin up listener threads
        // TODO

        client->state = cs_CONNECTED;
    }

    return NULL;
}

/**
 * @inherit
 */
void llnet_acceptor_start(void (*on_connect)(NetworkConnection_t*), void (*on_packet)(IntermediateTLV_t*)) {
    // Setup the server socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        fprintf(stderr, "error: socket creation failed: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Set the SO_REUSEADDR and SO_REUSEPORT flags (because a resource leaks will happen)
    int opt_value = 1;
    int err = setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt_value, sizeof(opt_value));
    if (err < 0) {
        fprintf(stderr, "error: could not set socket options: %s\n", strerror(errno));
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Setup the socket address struct (IPv4 only, on port PORT_NUMBER, accept any host)
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT_NUMBER);
    addr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket to accept any incoming connections for the FMS
    err = bind(server_fd, (struct sockaddr*) &addr, sizeof(addr));
    if (err < 0) {
        fprintf(stderr, "error: could not bind socket: %s\n", strerror(errno));
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Setup the listen queue with a length of 4095 (MORE than enough)
    listen(server_fd, 0xfff);

    // Create the connections list
    connections = arraylist_init();

    // Spin up the acceptor thread
    AcceptorThreadArgs* targs = malloc(sizeof(AcceptorThreadArgs));
    targs->server_fd = server_fd;
    targs->on_connect = on_connect;
    targs->on_packet = on_packet;
    pthread_t acceptor;
    pthread_create(&acceptor, NULL, &llnet_acceptor_thread, (void*) targs);
}

