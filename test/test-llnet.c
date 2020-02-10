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

#define T02_PCKT_LENGTH (8)

static ArrayList_t* t02_connections = NULL;

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
#if true
    dbg_info("connection created\n");
#endif

    arraylist_add(t02_connections, c);
}

/**
 * Simple on-packet handler
 *
 * @param pckt the packet recieved
 */
static void t02_on_packet(IntermediateTLV_t* pckt) {
    dbg_info("packet recieved: type=0x%02x, len=%u, timestamp=%u\n", pckt->type, pckt->length, pckt->timestamp);
    free(pckt);
}

/**
 * Attempt to create a mock network and send some data around
 */
int t02_send_data() {
    // Initialize the basics
    t02_connections = arraylist_init();
    AccepterConnection_t* accepter = llnet_connection_listen(llnet_connection_init(),
        t02_on_connect, t02_on_packet);

    msleep(5); // give some time for the accepter to start up

    // Connect to the accepter
    WorkerConnection_t* worker1 = llnet_connection_connect(llnet_connection_init(),
        "localhost", t02_on_packet);

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
        dbg_error("connections length is incorrect (length = %u)\n", size);
        arraylist_free(t02_connections);
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

    // Send the packet and free resources
    llnet_connection_send(worker1, np_TCP, pckt1);
    free(pckt1->data);
    free(pckt1);

    // Clean up resources
    msleep(10);
    llnet_connection_free((NetConnection_t*) worker1);
    llnet_connection_free((NetConnection_t*) accepter);

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
