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
#include <string.h>
#include <stdbool.h>

#include "test-utils.h"
#include "../utils/bounds.h"
#include "../network/lowlevel.h"
#include "../collections/arraylist.h"

// Debug stuff
#include "../utils/dbgprint.h"

#define DEBUG true

#define T02_PCKT_LENGTH (8)
#define NUMBER_OF_POLLS (16)
#define POLL_SLEEP_TIME (100)

static ArrayList_t* t02_connections = NULL;
static ArrayList_t* t02_svr_pckts = NULL;
static ArrayList_t* t02_clnt_pckts = NULL;

/**
 * Check if two packets are equal, including all fields
 *
 * @param p1 the first packet
 * @param p2 the second packet
 * @returns true if the two packets are equal, else false
 */
static bool packet_equals(IntermediateTLV_t* p1, IntermediateTLV_t* p2) {
    // First, setup a return value and check the types
    bool match = true;
    if (p1->type != p2->type) {
        match = false;
    }

    // Next, check the length
    if (p1->length != p2->length) {
        match = false;
    }

    // Then, check the timestamp
    if (p1->timestamp != p2->timestamp) {
        match = false;
    }

    // Finally, we need to check the data, which we should only do if we're good so far
    for (size_t i = 0; (i < p1->length) && match; i += 1) {
        if (p1->data[i] != p2->data[i]) {
            match = false;
        }
    }

    // Print the results
    if (!match) {
        // Print data information
        char buf1[128];
        char buf2[128];

        // Print data if it's an even length
        if (p1->length == 1) {
            snprintf(buf1, 128, "(0x%x)", ((uint8_t*) p1->data)[0]);
            snprintf(buf2, 128, "(0x%x)", ((uint8_t*) p2->data)[0]);
        } else if (p1->length == 2) {
            snprintf(buf1, 128, "(0x%x)", ((uint16_t*) p1->data)[0]);
            snprintf(buf2, 128, "(0x%x)", ((uint16_t*) p2->data)[0]);
        } else if (p1->length == 4) {
            snprintf(buf1, 128, "(0x%x)", ((uint32_t*) p1->data)[0]);
            snprintf(buf2, 128, "(0x%x)", ((uint32_t*) p2->data)[0]);
        } else if (p1->length == 8) {
            snprintf(buf1, 128, "(0x%lx)", ((uint64_t*) p1->data)[0]);
            snprintf(buf2, 128, "(0x%lx)", ((uint64_t*) p2->data)[0]);
        } else {
             // We got a wierd length, print the first few bytes
            buf1[0] = 0;
            buf2[0] = 0;
            int32_t t = p1->length; // trick gcc that this isn't a bit-field
            size_t length = min(t, 8);
            for (size_t i = 0; i < length; i += 1) {
                // Print each string and append it
                char buf[8];
                snprintf(buf, 8, "0x%02x", p1->data[i]);
                strcat(buf1, buf);
                snprintf(buf, 8, "0x%02x", p2->data[i]);
                strcat(buf2, buf);

                // Add delimiter
                if (i != (length - 1)) {
                    strcat(buf1, ", ");
                    strcat(buf2, ", ");
                }
            }
        }

        // Print the error text
        dbg_error("Packet mismatch (sent: type=0x%02x, length=0x%06x, timestamp=0x%08x, data=%s)\n"
            "\t(recv: type=0x%02x, length=0x%06x, timestamp=0x%08x, data=%s)\n", p1->type, p1->length,
            p1->timestamp, buf1, p2->type, p2->length, p2->timestamp, buf2);
    }

    return match;
}

/**
 * Blocks until the given ArrayList is not empty or the number of polls is exceeded
 *
 * @param arraylist the list to poll on
 * @return true if something is in the list, else false
 */
static bool arraylist_poll(ArrayList_t* arraylist) {
    bool received = false;
    for (size_t i = 0; i < NUMBER_OF_POLLS; i += 1) {
        // See if we got it
        if (arraylist_size(arraylist) != 0) {
            received = true;
            break;
        } else {
            usleep(POLL_SLEEP_TIME); // wait before polling again
        }
    }

    return received;
}

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
static void t02_svr_on_packet(uint32_t id, IntermediateTLV_t* pckt) {
#if DEBUG
    dbg_info("packet recieved on thread id=%u: type=0x%02x, len=%u, timestamp=%u\n", id, pckt->type, pckt->length, pckt->timestamp);
#endif
    arraylist_add(t02_svr_pckts, pckt);
}

/**
 * Simple on-packet handler for the "client"
 *
 * @param pckt the packet recieved
 */
static void t02_clnt_on_packet(uint32_t id, IntermediateTLV_t* pckt) {
#if DEBUG
    dbg_info("packet recieved on thread id=%u: type=0x%02x, len=%u, timestamp=%u\n", id, pckt->type, pckt->length, pckt->timestamp);
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
    bool connected = arraylist_poll(t02_connections);

    // Return failure if the host did not connected after 10 tries
    if (!connected) {
        dbg_error("no client connected (length = %u)\n", arraylist_size(t02_connections));
        llnet_connection_free((NetConnection_t*) worker1);
        llnet_connection_free((NetConnection_t*) accepter);
        return TEST_FAILURE;
    }

    // Test to see if the IDs have been assigned right
    WorkerConnection_t* w1 = llnet_connection_get(1);
    WorkerConnection_t* w2 = llnet_connection_get(2);
    if (w1 == NULL || w2 == NULL || !(w1 != worker1 || w2 != worker1)) {
        dbg_error("connection IDs are incorrect (w1=%p, w2=%p)\n", w1, w2);
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
    bool received = arraylist_poll(t02_svr_pckts);

    // Return failure if we did not get the packet after 10 tries
    if (!received) {
        dbg_error("no packet received (length = %u)\n", arraylist_size(t02_svr_pckts));
        llnet_connection_free((NetConnection_t*) worker1);
        llnet_connection_free((NetConnection_t*) accepter);
        return TEST_FAILURE;
    }

    // Make sure the packet is correct
    IntermediateTLV_t* pckt_recvd = ((IntermediateTLV_t*) arraylist_get(t02_svr_pckts, 0));
    if (!packet_equals(pckt1, pckt_recvd)) {
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

    // Build and send a reply packet
    IntermediateTLV_t* pckt2 = malloc(sizeof(IntermediateTLV_t));
    pckt2->type = 0xab; // pick a random type, shouldn't matter
    pckt2->length = T02_PCKT_LENGTH;
    pckt2->data = malloc(T02_PCKT_LENGTH);
    ((uint64_t*) pckt2->data)[0] = 0xffffa620fd93b002;

    // Send the packet
    WorkerConnection_t* worker_a = arraylist_get(t02_connections, 0);
    llnet_connection_send(worker_a, np_TCP, pckt2);

    // Wait for the packet to be recieved by the server
    received = arraylist_poll(t02_clnt_pckts);

    // Return failure if we did not get the packet after 10 tries
    if (!received) {
        dbg_error("no packet received (length = %u)\n", arraylist_size(t02_clnt_pckts));
        llnet_connection_free((NetConnection_t*) worker1);
        llnet_connection_free((NetConnection_t*) accepter);
        return TEST_FAILURE;
    }

    // Make sure the packet is correct
    pckt_recvd = ((IntermediateTLV_t*) arraylist_get(t02_clnt_pckts, 0));
    if (!packet_equals(pckt2, pckt_recvd)) {
        llnet_connection_free((NetConnection_t*) worker1);
        llnet_connection_free((NetConnection_t*) accepter);
        return TEST_FAILURE;
    }

    // Give back some resources
    llnet_packet_free(pckt2);
    pckt2 = NULL;
    arraylist_remove(t02_clnt_pckts, 0);
    llnet_packet_free(pckt_recvd);
    pckt_recvd = NULL;

    // Build a new packet to send
    IntermediateTLV_t* pckt3 = malloc(sizeof(IntermediateTLV_t));
    pckt3->type = 0xcd; // pick a random type, it shouldn't matter
    pckt3->length = T02_PCKT_LENGTH;
    pckt3->data = malloc(T02_PCKT_LENGTH);
    ((uint64_t*) pckt3->data)[0] = 0xaabb1234ffff5678;

    // Send the packet
    llnet_connection_send(worker1, np_UDP, pckt3);

    /*
     * NOTE: all UDP packets will come in on the Worker (client), as it binds to the OS later
     */

    // Wait for the packet to be recieved
    received = arraylist_poll(t02_clnt_pckts);

    // Return failure if we did not get the packet
    if (!received) {
        dbg_error("no packet received (length = %u)\n", arraylist_size(t02_clnt_pckts));
        llnet_connection_free((NetConnection_t*) worker1);
        llnet_connection_free((NetConnection_t*) accepter);
        return TEST_FAILURE;
    }

    // Make sure the packet is correct
    pckt_recvd = ((IntermediateTLV_t*) arraylist_get(t02_clnt_pckts, 0));
    if (!packet_equals(pckt3, pckt_recvd)) {
        llnet_connection_free((NetConnection_t*) worker1);
        llnet_connection_free((NetConnection_t*) accepter);
        return TEST_FAILURE;
    }

    // Give back some resources
    llnet_packet_free(pckt3);
    pckt3 = NULL;
    arraylist_remove(t02_clnt_pckts, 0);
    llnet_packet_free(pckt_recvd);
    pckt_recvd = NULL;

    // One last packet, build and send it
    IntermediateTLV_t* pckt4 = malloc(sizeof(IntermediateTLV_t));
    pckt4->type = 0xcd; // pick a random type, it shouldn't matter
    pckt4->length = T02_PCKT_LENGTH;
    pckt4->data = malloc(T02_PCKT_LENGTH);
    ((uint64_t*) pckt4->data)[0] = 0x58587167abcd1234;

    // Send the packet
    llnet_connection_send(worker_a, np_UDP, pckt4);

    // Wait for the packet to be recieved
    received = arraylist_poll(t02_svr_pckts);

    // Return failure if we did not get the packet
    if (!received) {
        dbg_error("no packet received (length = %u)\n", arraylist_size(t02_svr_pckts));
        llnet_connection_free((NetConnection_t*) worker1);
        llnet_connection_free((NetConnection_t*) accepter);
        return TEST_FAILURE;
    }

    // Make sure the packet is correct
    pckt_recvd = ((IntermediateTLV_t*) arraylist_get(t02_svr_pckts, 0));
    if (!packet_equals(pckt4, pckt_recvd)) {
        llnet_connection_free((NetConnection_t*) worker1);
        llnet_connection_free((NetConnection_t*) accepter);
        return TEST_FAILURE;
    }

    // Give back resources
    llnet_packet_free(pckt4);
    pckt4 = NULL;
    arraylist_remove(t02_clnt_pckts, 0);
    llnet_packet_free(pckt_recvd);
    pckt_recvd = NULL;

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
