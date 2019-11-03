/**
 * core/test/test-list.c
 *
 * Runs tests against the list implementation
 *
 * @author Connor Henley, @thatging3rkid
 */
#ifndef _LOCAL_HEADER
    #error "This code should not be run outside of testing\n"
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "../collections/list.h"
#include "../collections/arraylist.h"
#include "../collections/linkedlist.h"

#define FULL_TEST_ITEMS 2048

/**
 * Returns the number of elements in a static array
 *
 * @macro
 * @param a the array
 */
#define num_elements(a) (sizeof(a)/sizeof(a[0])) // returns the number of elements in the array

/**
 * Prints debugging information about the given list
 *
 * @macro
 * @param x the list data structure
 */
#define print_dbgdata(x) { \
    fprintf(stderr, "[%s@%s:%i] ", __func__, __FILE__, __LINE__); \
    if (x != NULL && x->impl == LIST_ARRAY) { \
        fprintf(stderr, "{impl=LIST_ARRAY, err=0x%0x, len=%u, allocated=%u} ", (x)->err, ((ArrayList_t*) x)->len, ((ArrayList_t*) x)->allocated); \
    } else if (x != NULL && x->impl == LIST_LINKED) { \
        fprintf(stderr, "{impl=LIST_LINKED, err=0x%0x, first=%p, last=%p} ", (x)->err, ((LinkedList_t*) x)->first, ((LinkedList_t*) x)->last); \
    } \
}

/*
 * Start off by creating a list and destroying it
 */
int t01_creation(ListImplementation_t type) {
    List_t* list = list_init(type);
    list_free(list);

    return 0;
}

/*
 * Do some simple testing: add a couple items, remove them, then destroy the list
 */
int t02_basic_tests(ListImplementation_t type) {
    // make list
    List_t* list = list_init(type);
    list_add(list, (void*) 1);
    list_add(list, (void*) 2);
    list_add(list, (void*) 3);
    list_add(list, (void*) 4);

    // size check
    uint32_t size = list_size(list);
    if (size != 4) {
        print_dbgdata(list);
        fprintf(stderr, "error: incorrect size %u (type=%u)\n", size, type);
        return EXIT_FAILURE;
    }

    // start removing things
    for (size_t i = 1; i < 5; i += 1) {
        uintptr_t e = (uintptr_t)list_remove(list, 0);
        if (e != i) {
            print_dbgdata(list);
            fprintf(stderr, "error: at i=%lu incorrect value removed %lu\n", i, e);
            return EXIT_FAILURE;
        }
    }

    // clean up list
    list_free(list);
    return 0;
}

/*
 * Run some basic error handling checks
 */
int t03_basic_error_handling(ListImplementation_t type) {
    // error-handling: try to remove something from an empty list
    List_t* list = list_init(type); // make a new list
    void* e = list_remove(list, 0);
    if (e != NULL || list->err != LIST_BOUNDS) {
        print_dbgdata(list);
        fprintf(stderr, "error: incorrect error code (result=%p)\n", e);
        return EXIT_FAILURE;
    }
    list_free(list); // cleanup 

    // error-handling: try to remove something out-of-bounds
    list = list_init(type); // make a new list
    list_add(list, (void*) 1);
    e = list_remove(list, 1);
    if (e != NULL || list->err != LIST_BOUNDS) {
        print_dbgdata(list);
        fprintf(stderr, "error: incorrect error code (result=%p)\n", e);
        return EXIT_FAILURE;
    }
    list_free(list); // cleanup

    // error-handling: add to an invalid position
    list = list_init(type); // make a new list
    ListError_t result = list_add_pos(list, 1, (void*) 1);
    if (result != LIST_BOUNDS) {
        print_dbgdata(list);
        fprintf(stderr, "error: incorrect error code (result=%u)\n", result);
        return EXIT_FAILURE;
    }
    
    list_free(list); // cleanup
    return 0;
}

/*
 * Test the arraylist_add_pos(...) function by putting data into the list as position 0
 */
int t04_arraylist_add_pos_0() {
    // make list
    List_t* list = list_init(LIST_ARRAY);
    list_add_pos(list, 0, (void*) 1);
    list_add_pos(list, 0, (void*) 2);
    list_add_pos(list, 0, (void*) 3);
    list_add_pos(list, 0, (void*) 4);
    
    // size check
    uint32_t size = list_size(list);
    if (size != 4) {
        print_dbgdata(list);
        fprintf(stderr, "error: incorrect size %u\n", size);
        return EXIT_FAILURE;
    }
    
    // start removing things
    for (size_t i = 0; i < 4; i += 1) {
        uintptr_t e = (uintptr_t)list_remove(list, 3 - i);
        if (e != (i + 1) || list->err != LIST_OKAY) {
            print_dbgdata(list);
            fprintf(stderr, "error: at i=%lu incorrect value removed %lu\n", i, e);
            return EXIT_FAILURE;
        }
    }
    
    // clean up list
    list_free(list);
    return 0;
}

/*
 * The basics have been tested at this point, time to do some strenuous testing
 * Make an array of FULL_TEST_ITEMS, add all those items to a list, remove every other, then clean up
 */
t05_strain_test(ListImplementation_t type, size_t elements) {
    // make the array
    uintptr_t items[elements];
    for (size_t i = 0; i < elements; i += 1) {
        items[i] = i;
    }
    
    // make list and add every item
    List_t* list = list_init(type);
    for (size_t i = 0; i < elements; i += 1) {
        list_add(list, (void*) items[i]);
    }
    
    // remove every-other
    for (size_t i = 0; i < (elements / 2); i += 1) {
        uintptr_t e = (uintptr_t) list_remove(list, i);
        if (e % 2 != 0) {
            print_dbgdata(list);
            fprintf(stderr, "error: at i=%lu incorrect value removed %lu\n", i, e);
            return EXIT_FAILURE;
        }
    }
    
    // make sure the rest of the values are odd
    for (size_t i = 0; i < (elements / 2); i += 1) {
        uintptr_t e = (uintptr_t) list_get(list, i);
        if (e % 2 != 1) {
            print_dbgdata(list);
            fprintf(stderr, "error: at i=%lu incorrect value removed %lu\n", i, e);
            return EXIT_FAILURE;
        }
    }
    
    // clean up
    list_free(list);
    return 0;
}


int main() {

    // Run tests on both types of list
    ListImplementation_t types[] = {LIST_ARRAY, LIST_LINKED};
    for (size_t i = 0; i < num_elements(types); i += 1) {

        ListImplementation_t type = types[i];

        t01_creation(type);
        t02_basic_tests(type);
        t03_basic_error_handling(type);

        if (type == LIST_ARRAY) {
            t04_arraylist_add_pos_0(); // only run once, since it's an array list only test
        }

        t05_strain_test(type, FULL_TEST_ITEMS);
    }

    // finished
    printf("success!\n");
    return EXIT_SUCCESS;
}
