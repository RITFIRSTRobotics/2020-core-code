/**
 * core/network/lowlevel.c
 *
 * Handles OS-level interactions for networking
 *
 * @author Connor Henley, @thatging3rkid
 */
#define _DEFAULT_SOURCE // needed for hstrerror(...), clock_gettime(...)
 
// standards
#include <stdio.h>
#include <stdint.h>
#include <memory.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>

// math
#include <math.h>

// time
#include <time.h>

// networking
#include <netdb.h>
#include <arpa/inet.h>
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

#define _LLNET_UDP_BUFFER_LENGTH (65535)

static ArrayList_t* connections = NULL;
static int32_t llnet_time_offset = 0; // this value is added on a send, subtracted on a recieve

// Argument structure for the network send function/thread
typedef struct {
    WorkerConnection_t* connection;
    NetworkProtocol_t proto;
    IntermediateTLV_t* packet;
    uint32_t* rc;
    bool* finished;
} SendThreadArgs;

/**
 * Sends a packet out over the network
 *
 * @param _targs the argument structure
 * @returns NULL
 * @note function will always clean up the argument structure
 */
static void* _llnet_pckt_send(void* _targs) {
    // Convert to the correct structure type
    SendThreadArgs* targs = (SendThreadArgs*) _targs;

    // Get a buffer to copy the packet into
    uint32_t buf_len = targs->packet->length + LLNET_HEADER_LENGTH;
    uint8_t* buf = malloc(buf_len);

    // Generate the first word
    uint32_t pckt_len_net = htonl(targs->packet->length);
    memcpy(buf, &pckt_len_net, sizeof(uint32_t));
    buf[0] = targs->packet->type;

    // Get the timestamp and convert to milliseconds
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    targs->packet->timestamp = (uint32_t) ((ts.tv_sec * 1000) + round(ts.tv_nsec / 1.0e6) + llnet_time_offset);
    uint32_t timestamp = htonl(targs->packet->timestamp);
    memcpy((buf + 4), &timestamp, sizeof(uint32_t));

    // Copy the remaining data
    memcpy((buf + LLNET_HEADER_LENGTH), targs->packet->data, targs->packet->length);

    int err; // save errors

    // Handle the send based on the protocol
    if (targs->proto == np_TCP) {
        // Send the data using TCP
        err = write(targs->connection->tcp_fd, buf, buf_len);
    } else if (targs->proto == np_UDP) {
        // Send the data using UDP
        err = sendto(targs->connection->udp_fd, buf, buf_len, MSG_CONFIRM, (const struct sockaddr*) &targs->connection->other_addr,
            targs->connection->other_addr_len);
    }

    // Store the error
    if (targs->rc != NULL) {
        *(targs->rc) = (err < 0)? err : 0;
    }

    // Mark that we're finished
    if (targs->finished != NULL) {
        *(targs->finished) = true;
    }

    // Clean up
    free(buf);
    free(targs);
    targs = NULL;

    return NULL;
}

/**
 * Listens for incoming TCP data, decodes the data, then hands them to the handler
 *
 * @param _targs the connection to listen on
 * @returns NULL
 */
static void* _llnet_listener_tcp(void* _targs) {
    WorkerConnection_t* worker = (WorkerConnection_t*) _targs;
    worker->tcp_status = ls_OKAY;

    // Enable deferred cancelling (this is default, but expected behavior)
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);

    while (worker->thread_stop == false) {
        // Get some memory to store the data
        IntermediateTLV_t* tlv = malloc(sizeof(IntermediateTLV_t));

        // Read the first word of the header
        uint32_t header = 0;
        int err = read(worker->tcp_fd, &header, sizeof(uint32_t));

        // Error checking
        if (err < 0) {
            dbg_info("error reading TCP socket: %s\n", strerror(errno));
            free(tlv);
            break;
        }

        // Start the decode
        tlv->type = (header & 0xff000000) >> 24;
        tlv->length = ntohl((header & 0xffffff));

        // Read the timestamp
        uint32_t timestamp = 0;
        read(worker->tcp_fd, &timestamp, sizeof(uint32_t));
        tlv->timestamp = ntohl(timestamp);

        // Read the rest of the data into the packet
        tlv->data = malloc(tlv->length);
        err = read(worker->tcp_fd, tlv->data, tlv->length);

        // Error checking
        if (err < 0) {
            dbg_info("error reading TCP socket: %s\n", strerror(errno));
            free(tlv);
            break;
        }

        // Call the handler
        worker->on_packet(tlv);
    }

    worker->tcp_status = ls_DISCONNECTED;
    return NULL;
}

/**
 * Listens for incoming UDP data, decodes the data, the hands them to the handler
 *
 * @param _targs the connection to listen to
 * @returns NULL
 */
static void* _llnet_listener_udp(void* _targs) {
    WorkerConnection_t* worker = (WorkerConnection_t*) _targs;
    worker->udp_status = ls_OKAY;
    uint8_t* buf = malloc(_LLNET_UDP_BUFFER_LENGTH);

    // Enable deferred cancelling (this is default, but expected behavior)
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);

    while (worker->thread_stop == false) {
        // Get some memory to store the data
        IntermediateTLV_t* tlv = malloc(sizeof(IntermediateTLV_t));

        // Get the UDP packet in full
        int recv = recvfrom(worker->udp_fd, buf, _LLNET_UDP_BUFFER_LENGTH, MSG_WAITALL, (struct sockaddr*) &worker->other_addr,
            &worker->other_addr_len);

        // Error checking
        if (recv < 0) {
            dbg_info("error reading UDP socket: %s\n", strerror(errno));
            free(tlv);
            break;
        }
        if (recv < LLNET_HEADER_LENGTH) {
            dbg_warning("invalid header length %u\n", recv);
            free(tlv);
            continue;
        }

        // Start the decode
        uint32_t header = ((uint32_t*) buf)[0];
        tlv->type = (header & 0xff000000) >> 24;
        tlv->length = ntohl((header & 0xffffff));

        // Decode the timestamp
        uint32_t timestamp = ((uint32_t*) buf)[1];
        tlv->timestamp = ntohl(timestamp);

        // Save the rest of the data
        tlv->data = malloc(tlv->length);
        memcpy(tlv->data, (buf + LLNET_HEADER_LENGTH), tlv->length);

        // Call the handler
        worker->on_packet(tlv);
    }

    worker->udp_status = ls_DISCONNECTED;
    free(buf);
    return NULL;
}

/**
 * Accepts incoming connections over TCP, creates the necessary data structures
 * for the connection, and start the listener threads.
 *
 * @param _targs the connection to use
 * @returns NULL
 */
static void* _llnet_accepter_thread(void* _targs) {
    AccepterConnection_t* accepter = (AccepterConnection_t*) _targs;

    // Enable deferred cancelling (this is default, but expected behavior)
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);

    // Loop until someone tells us not to
    while (accepter->thread_stop == false) {
        // Make a data structure
        WorkerConnection_t* worker = malloc(sizeof(WorkerConnection_t));
        worker->udp_fd = socket(AF_INET, SOCK_DGRAM, 0);

        // Make sure the socket request was successful
        if (worker->udp_fd < 0) {
            dbg_error("UDP socket creation failed: %s\n", strerror(errno));
            close(worker->udp_fd);
            free(worker);
            exit(EXIT_FAILURE); // for now, exit on error
        }

        // Fill in everything else
        worker->thread_stop = false;
        worker->on_packet = accepter->on_packet;
        worker->tcp_status = ls_NOT_STARTED;
        worker->udp_status = ls_NOT_STARTED;

        // Accept the connection
        worker->other_addr_len = 0;
        worker->tcp_fd = accept(accepter->tcp_fd, (struct sockaddr*) &worker->other_addr, &worker->other_addr_len);
        if (worker->tcp_fd < 0) {
            dbg_warning("could not accept client: %s\n", strerror(errno));
            close(worker->udp_fd);
            free(worker);
            continue;
        }

        // Notify the handler that we got one
        if (accepter->on_connect != NULL) {
            accepter->on_connect(worker);
        }
        arraylist_add(connections, worker);

        worker->state = cs_WORKER;

        // Start listener threads
        pthread_create(&worker->tcp_thread, NULL, &_llnet_listener_tcp, (void*) worker);
        pthread_create(&worker->udp_thread, NULL, &_llnet_listener_udp, (void*) worker);
    }

    return NULL;
}

/**
 * @inherit
 */
NetConnection_t* llnet_connection_init() {
    // Get memory and set state to nothing
    NetConnection_t* connection = malloc(sizeof(NetConnection_t));
    connection->state = cs_NOTHING;
    connection->thread_stop = false;
    connection->on_packet = NULL;

    // Setup the TCP socket
    connection->tcp_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (connection->tcp_fd < 0) {
        dbg_error("TCP socket creation failed: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Set the SO_REUSEADDR and SO_REUSEPORT flags (because resource leaks *will* happen)
    int opt_value = 1;
    int err = setsockopt(connection->tcp_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt_value, sizeof(int));
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
WorkerConnection_t* llnet_connection_connect(NetConnection_t* connection, char* host, void (*handler)(IntermediateTLV_t*)) {
    // Make sure this connection isn't already setup
    if (connection->state != cs_NOTHING) {
        dbg_warning("connection already made (state=0x%02x), returning...\n", connection->state);
        return (WorkerConnection_t*) connection;
    }

    // Update this structure to a WorkerConnection
    WorkerConnection_t* worker = realloc(connection, sizeof(WorkerConnection_t));
    connection = NULL;
    worker->tcp_status = ls_NOT_STARTED;
    worker->udp_status = ls_NOT_STARTED;

    worker->on_packet = handler;
    memset(&worker->other_addr, 0, sizeof(struct sockaddr_in));
    worker->other_addr_len = 0;

    // Need to get the address for the given host
    struct hostent* host_addr = gethostbyname(host);
    if (host_addr == NULL) {
        dbg_warning("gethostbyname failed: %s\n", hstrerror(h_errno));
        return worker;
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
    worker->other_addr.sin_family = AF_INET;
    worker->other_addr.sin_port = htons(PORT_NUMBER);
    worker->other_addr.sin_addr = *((struct in_addr*) host_addr->h_addr_list[0]);

    // Create the TCP connection to the FMS
    int err = connect(worker->tcp_fd, (struct sockaddr*) &worker->other_addr, sizeof(struct sockaddr_in));
    if (err < 0) {
        dbg_warning("connect failed: %s\n", strerror(errno));
        return worker;
    }

    // Connection was successful
    worker->state = cs_WORKER;

    // Start listener threads
    pthread_create(&worker->tcp_thread, NULL, &_llnet_listener_tcp, (void*) worker);
    pthread_create(&worker->udp_thread, NULL, &_llnet_listener_udp, (void*) worker);

    return worker;
}

/**
 * @inherit
 */
uint32_t llnet_connection_send(WorkerConnection_t* connection, NetworkProtocol_t proto, IntermediateTLV_t* packet) {
    // Check to make sure this is actually a worker connection
    if (connection->state != cs_WORKER) {
        dbg_error("connection is not a worker client\n");
        return -1;
    }

    uint32_t* rc = malloc(sizeof(uint32_t)); // get some memory for the return value

    // Make the "thread arguments"
    SendThreadArgs* stargs = malloc(sizeof(SendThreadArgs));
    stargs->connection = connection;
    stargs->proto = proto;
    stargs->packet = packet;
    stargs->rc = rc;
    stargs->finished = NULL;

    _llnet_pckt_send((void*) stargs);

    uint32_t ret = *(rc);
    free(rc);
    return ret;
}

/**
 * @inherit
 */
void llnet_connection_send_thread(WorkerConnection_t* connection, NetworkProtocol_t proto, IntermediateTLV_t* packet, uint32_t* rc, bool* finished) {
    // Check to make sure this is actually a worker connection
    if (connection->state != cs_WORKER) {
        dbg_error("connection is not a worker client\n");
        *(rc) = -1;
        return;
    }

    // Make the thread arguments
    SendThreadArgs* stargs = malloc(sizeof(SendThreadArgs));
    stargs->connection = connection;
    stargs->proto = proto;
    stargs->packet = packet;
    stargs->rc = rc;
    stargs->finished = finished;

    // Start the thread
    pthread_t t;
    pthread_create(&t, NULL, _llnet_pckt_send, (void*) stargs);
    pthread_detach(t); // automatically releases resources when finished
}

/**
 * @inherit
 */
AccepterConnection_t* llnet_connection_listen(NetConnection_t* connection, void (*on_connect)(WorkerConnection_t*), void (*on_packet)(IntermediateTLV_t*)) {
    // Make sure the connection isn't already setup
    if (connection->state != cs_NOTHING) {
        dbg_warning("connection already made (state=0x%02x)\n", connection->state);
        return (AccepterConnection_t*) connection;
    }

    // Update the structure to an AccepterConnection
    AccepterConnection_t* accepter = realloc(connection, sizeof(AccepterConnection_t));
    connection = NULL;
    accepter->state = cs_ACCEPTER;
    accepter->on_packet = on_packet;
    accepter->on_connect = on_connect;

    // Setup the socket address struct (IPv4 only, on port PORT_NUMBER, accept any host)
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT_NUMBER);
    addr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket to accept any incoming connections for the FMS
    int err = bind(accepter->tcp_fd, (struct sockaddr*) &addr, sizeof(struct sockaddr_in));
    if (err < 0) {
        fprintf(stderr, "error: could not bind socket: %s\n", strerror(errno));
        close(accepter->tcp_fd);
        exit(EXIT_FAILURE);
    }

    // Setup the listen queue with a length of 4095 (MORE than enough)
    listen(accepter->tcp_fd, 0xfff);

    // Create the connections list
    connections = arraylist_init();

    // Spin up the acceptor thread
    pthread_create(&accepter->accepter_thread, NULL, &_llnet_accepter_thread, (void*) accepter);

    return accepter;
}

/**
 * @inherit
 */
void llnet_connection_free(NetConnection_t* connection) {
    if (connection->state == cs_WORKER) {
        // Do worker specific clean-up
        WorkerConnection_t* worker = (WorkerConnection_t*) connection;

        // Clean the TCP thread: cancel it and free resources by calling join
        pthread_cancel(worker->tcp_thread);
        pthread_join(worker->tcp_thread, NULL);

        // Clean the UDP thread: cancel it and free resources by calling join
        pthread_cancel(worker->udp_thread);
        pthread_join(worker->udp_thread, NULL);
    } else if (connection-> state == cs_ACCEPTER) {
        // Do accepter specific clean-up
        AccepterConnection_t* accepter = (AccepterConnection_t*) connection;

        // Clean the thread: cancel it and free resources by calling join
        pthread_cancel(accepter->accepter_thread);
        pthread_join(accepter->accepter_thread, NULL);
    }

    close(connection->tcp_fd);
    connection->tcp_fd = 0;

    close(connection->udp_fd);
    connection->udp_fd = 0;

    free(connection);
}
