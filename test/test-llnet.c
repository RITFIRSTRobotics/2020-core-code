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

static ArrayList_t* t02_connections = NULL;

/**
 * Test the creation of low level networking structures
 */
int t01_creation() {
    NetConnection_t* c = llnet_connection_init();
    llnet_connection_free(c);
    return TEST_SUCCESS;
}

static void t02_on_connect(WorkerConnection_t* c) {
#if true
    dbg_info("connection created\n");
#endif

    arraylist_add(t02_connections, c);
}

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
