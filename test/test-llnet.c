/**
 * core/test/test-llnet.c
 *
 * Tests low level networking functionality
 *
 * @author Connor Henley, @thatging3rkid
 */
#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "test-utils.h"
#include "../network/lowlevel.h"
#include "../collections/arraylist.h"

// Debug stuff
#include "../utils/dbgprint.h"

#define DEBUG true

#define T02_PCKT_LENGTH (8)

static ArrayList_t* t02_connections = NULL;
static ArrayList_t* t02_svr_pckts = NULL;
static ArrayList_t* t02_clnt_pckts = NULL;

/**
 * Test the creation of low level networking structures
 */
int t01_creation() {
    NetConnection_t* c = llnet_connection_init();
    llnet_connection_free(c);
    return TEST_SUCCESS;
}

/**
 * Simple on-connection handler
 *
 * @param c the new connection
 */
static void t02_on_connect(WorkerConnection_t* c) {
#if DEBUG
    dbg_info("connection created\n");
#endif
    arraylist_add(t02_connections, c);
}

/**
 * Simple on-packet handler for the "server"
 *
 * @param pckt the packet recieved
 */
static void t02_svr_on_packet(IntermediateTLV_t* pckt) {
#if DEBUG
    dbg_info("packet recieved: type=0x%02x, len=%u, timestamp=%u\n", pckt->type, pckt->length, pckt->timestamp);
#endif
    arraylist_add(t02_svr_pckts, pckt);
}

/**
 * Simple on-packet handler for the "client"
 *
 * @param pckt the packet recieved
 */
static void t02_clnt_on_packet(IntermediateTLV_t* pckt) {
#if DEBUG
    dbg_info("packet recieved: type=0x%02x, len=%u, timestamp=%u\n", pckt->type, pckt->length, pckt->timestamp);
#endif
    arraylist_add(t02_clnt_pckts, pckt);
}

/**
 * Attempt to create a mock network and send some data around
 */
int t02_send_data() {
    // Initialize the basics
    t02_connections = arraylist_init();
    t02_svr_pckts = arraylist_init();
    t02_clnt_pckts = arraylist_init();
    AccepterConnection_t* accepter = llnet_connection_listen(llnet_connection_init(),
        t02_on_connect, t02_svr_on_packet);

    msleep(5); // give some time for the accepter to start up

    // Connect to the accepter
    WorkerConnection_t* worker1 = llnet_connection_connect(llnet_connection_init(),
        "localhost", t02_clnt_on_packet);

    // Wait for the listener to connect
    uint32_t size;
    bool connected = false;
    for (size_t i = 0; i < 10; i += 1) {
        size = arraylist_size(t02_connections);

        // Client connected correctly
        if (size == 1) {
            connected = true;
            break;
        } else {
            usleep(500); // wait before polling again
        }
    }

    // Return failure if the host did not connected after 10 tries
    if (!connected) {
        dbg_error("no client connected (length = %u)\n", size);
        llnet_connection_free((NetConnection_t*) worker1);
        llnet_connection_free((NetConnection_t*) accepter);
        return TEST_FAILURE;
    }

    // Build a packet to send
    IntermediateTLV_t* pckt1 = malloc(sizeof(IntermediateTLV_t));
    pckt1->type = 0xab; // pick a random type, shouldn't matter
    pckt1->length = T02_PCKT_LENGTH;
    pckt1->data = malloc(T02_PCKT_LENGTH);
    ((uint64_t*) pckt1->data)[0] = 0x1234567890abcdef;

    // Send the packet
    llnet_connection_send(worker1, np_TCP, pckt1);

    // Wait for the packet to be recieved by the server
    bool received = false;
    for (size_t i = 0; i < 10; i += 1) {
        size = arraylist_size(t02_svr_pckts);

        // See if we got it
        if (size == 1) {
            received = true;
            break;
        } else {
            usleep(500); // wait before polling again
        }
    }

    // Return failure if we did not get the packet after 10 tries
    if (!received) {
        dbg_error("no packet received (length = %u)\n", size);
        llnet_connection_free((NetConnection_t*) worker1);
        llnet_connection_free((NetConnection_t*) accepter);
        return TEST_FAILURE;
    }

    // Make sure the packet is correct
    IntermediateTLV_t* pckt_recvd = ((IntermediateTLV_t*) arraylist_get(t02_svr_pckts, 0));
    if (pckt_recvd->type != pckt1->type || pckt_recvd->length != pckt1->length ||
        pckt_recvd->timestamp != pckt1->timestamp || ((uint64_t*) pckt_recvd->data)[0] != ((uint64_t*) pckt1->data)[0]
    ) {
        dbg_error("Packet mismatch (sent: type=0x%02x, length=0x%06x, timestamp=0x%08x, data=0x%0lx)\n"
            "\t(recv: type=0x%02x, length=0x%06x, timestamp=0x%08x, data=0x%0lx)\n", pckt1->type, pckt1->length,
            pckt1->timestamp, ((uint64_t*) pckt1->data)[0], pckt_recvd->type, pckt_recvd->length, pckt_recvd->timestamp,
            ((uint64_t*) pckt_recvd->data)[0]);
        llnet_connection_free((NetConnection_t*) worker1);
        llnet_connection_free((NetConnection_t*) accepter);
        return TEST_FAILURE;
    }

    // Give back some resources
    llnet_packet_free(pckt1);
    pckt1 = NULL;
    arraylist_remove(t02_svr_pckts, 0);
    llnet_packet_free(pckt_recvd);
    pckt_recvd = NULL;

    // Build a new packet to send
    IntermediateTLV_t* pckt2 = malloc(sizeof(IntermediateTLV_t));
    pckt2->type = 0xcd; // pick a random type, it shouldn't matter
    pckt2->length = T02_PCKT_LENGTH;
    pckt2->data = malloc(T02_PCKT_LENGTH);
    ((uint64_t*) pckt2->data)[0] = 0xaabb1234ffff5678;

    // Send the packet
    llnet_connection_send(worker1, np_UDP, pckt2);

    // TODO debug

    // TODO compare

    // Give back some resources
    llnet_packet_free(pckt2);

    msleep(10);

    // Clean up network resources
    llnet_connection_free((NetConnection_t*) worker1);
    llnet_connection_free((NetConnection_t*) accepter);

    // Clean up buffers/lists
    arraylist_free(t02_connections);
    t02_connections = NULL;
    arraylist_free(t02_svr_pckts);
    t02_svr_pckts = NULL;
    arraylist_free(t02_clnt_pckts);
    t02_clnt_pckts = NULL;

    return TEST_SUCCESS;
}

/**
 * Entry point to the program
 */
int main() {
    // Run tests
    int error = 0;
    error += t01_creation();
    error += t02_send_data();

    // Tests finished, handle the error code
    if (error == TEST_SUCCESS) {
        printf("success!\n");
        return EXIT_SUCCESS;
    } else {
        printf("^^^ test errors\n");
        return EXIT_FAILURE;
    }
}
