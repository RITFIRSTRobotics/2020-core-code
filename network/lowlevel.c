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
#include <memory.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

// networking
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
#include "../collections/arraylist.h"

static ArrayList_t* connections = NULL;

// Argument structure for the acceptor thread
typedef struct {
    int server_fd;
    void (*on_connect)(NetworkConnection_t*);
    void (*on_packet)(IntermediateTLV_t*);
} AcceptorThreadArgs;

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
        NetworkConnection_t* connection = malloc(sizeof(NetworkConnection_t));
        connection->handler = targs->on_packet;

        // Accept the connection
        connection->tcp_fd = accept(targs->server_fd, &connection->client_addr, &connection->client_addr_len);
        if (connection->tcp_fd < 0) {
            fprintf(stderr, "error: could not accept client: %s\n", strerror(errno));
            free(connection);
            continue;
        }

        // Call on_connect and add to the connection list
        targs->on_connect(connection);
        arraylist_add(connections, connection);

        // Spin up listener threads
        llnet_listener_tcp_start(connection);
        llnet_listener_udp_start(connection);
    }

    return NULL;
}

/**
 * @inherit
 */
void llnet_acceptor_start(void (*on_connect)(NetworkConnection_t*),
        void (*on_packet)(IntermediateTLV_t*)) {
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

