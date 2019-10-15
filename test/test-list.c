/**
 * core/test/test-list.c
 *
 * Runs tests against the list implementation
 *
 * @author Connor Henley, @thatging3rkid
 */
#ifndef _LOCAL_HEADER
    #error "This code should not be run outside of testing"
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "../collections/list.h"

#define FULL_TEST_ITEMS 2048

int main() {

    /*
     * Start off by creating a list and destroying it
     */
    List_t* list = list_init();
    list_free(list);

    /*
     * Do some simple testing: add a couple items, remove them, then destroy the list
     */

    // make list
    list = list_init();
    list_add(list, (void*) 1);
    list_add(list, (void*) 2);
    list_add(list, (void*) 3);
    list_add(list, (void*) 4);

    // size check
    uint32_t size = list_size(list);
    if (size != 4) {
        fprintf(stderr, "error: incorrect size %u\n", size);
        return EXIT_FAILURE;
    }

    // start removing things
    for (size_t i = 1; i < 5; i += 1) {
        uintptr_t e = (uintptr_t)list_remove(list, 0);
        if (e != i) {
            fprintf(stderr, "error: at i=%lu incorrect value removed %lu\n", i, e);
            return EXIT_FAILURE;
        }
    }

    // clean up list
    list_free(list);

    /*
     * Test the list_add_pos(...) function by putting data into the list as position 0
     */
     
    // make list
    list = list_init();
    list_add_pos(list, 0, (void*) 1);
    list_add_pos(list, 0, (void*) 2);
    list_add_pos(list, 0, (void*) 3);
    list_add_pos(list, 0, (void*) 4);

    // size check
    size = list_size(list);
    if (size != 4) {
        fprintf(stderr, "error: incorrect size %u\n", size);
        return EXIT_FAILURE;
    }

    // start removing things
    for (size_t i = 0; i < 4; i += 1) {
        uintptr_t e = (uintptr_t)list_remove(list, 3 - i);
        if (e != (i + 1)) {
            fprintf(stderr, "error: at i=%lu incorrect value removed %lu\n", i, e);
            return EXIT_FAILURE;
        }
    }

    // clean up list
    list_free(list);

    /*
     * The basics have been tested at this point, time to do some strenuous testing
     * Make an array of FULL_TEST_ITEMS, add all those items to a list, remove every other, then clean up
     */

    // make the array
    uintptr_t items[FULL_TEST_ITEMS];
    for (size_t i = 0; i < FULL_TEST_ITEMS; i += 1) {
        items[i] = i;
    }

    // make list and add every item
    list = list_init();
    for (size_t i = 0; i < FULL_TEST_ITEMS; i += 1) {
        list_add(list, (void*) items[i]);
    }

    // remove every-other
    for (size_t i = 0; i < (FULL_TEST_ITEMS / 2); i += 1) {
        uintptr_t e = (uintptr_t) list_remove(list, i);
        if (e % 2 != 0) {
            fprintf(stderr, "error: at i=%lu incorrect value removed %lu\n", i, e);
            return EXIT_FAILURE;
        }
    }

    // make sure the rest of the values are odd
    for (size_t i = 0; i < (FULL_TEST_ITEMS / 2); i += 1) {
        uintptr_t e = (uintptr_t) list_get(list, i);
        if (e % 2 != 1) {
            fprintf(stderr, "error: at i=%lu incorrect value removed %lu\n", i, e);
            return EXIT_FAILURE;
        }
    }

    // clean up
    list_free(list);

    // finished
    printf("success!\n");
    return EXIT_SUCCESS;
}
