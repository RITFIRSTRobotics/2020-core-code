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

#define TEST_SUCCESS 0
#define TEST_FAILURE 1

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

    return TEST_SUCCESS;
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
        return TEST_FAILURE;
    }

    // start removing things
    for (size_t i = 1; i < 5; i += 1) {
        uintptr_t e = (uintptr_t)list_remove(list, 0);
        if (e != i) {
            print_dbgdata(list);
            fprintf(stderr, "error: at i=%lu incorrect value removed %lu\n", i, e);
            return TEST_FAILURE;
        }
    }

    // clean up list
    list_free(list);
    return TEST_SUCCESS;
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
        return TEST_FAILURE;
    }
    list_free(list); // cleanup 

    // error-handling: try to remove something out-of-bounds
    list = list_init(type); // make a new list
    list_add(list, (void*) 1);
    e = list_remove(list, 1);
    if (e != NULL || list->err != LIST_BOUNDS) {
        print_dbgdata(list);
        fprintf(stderr, "error: incorrect error code (result=%p)\n", e);
        return TEST_FAILURE;
    }
    list_free(list); // cleanup

    // error-handling: add to an invalid position
    list = list_init(type); // make a new list
    ListError_t result = list_add_pos(list, 1, (void*) 1);
    if (result != LIST_BOUNDS) {
        print_dbgdata(list);
        fprintf(stderr, "error: incorrect error code (result=%u)\n", result);
        return TEST_FAILURE;
    }
    
    list_free(list); // cleanup
    return TEST_SUCCESS;
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
    if (size != 4 || list->err != LIST_OKAY) {
        print_dbgdata(list);
        fprintf(stderr, "error: incorrect size %u\n", size);
        return TEST_FAILURE;
    }
    
    // start removing things
    for (size_t i = 0; i < 4; i += 1) {
        uintptr_t e = (uintptr_t)list_remove(list, 3 - i);
        if (e != (i + 1) || list->err != LIST_OKAY) {
            print_dbgdata(list);
            fprintf(stderr, "error: at i=%lu incorrect value removed %lu\n", i, e);
            return TEST_FAILURE;
        }
    }
    
    // clean up list
    list_free(list);
    return TEST_SUCCESS;
}

/*
 * The basics have been tested at this point, time to do some strenuous testing
 * Make an array of FULL_TEST_ITEMS, add all those items to a list, remove every other, then clean up
 */
int t05_strain_test(ListImplementation_t type, size_t elements) {
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
        if ((e % 2 != 0) || list->err != LIST_OKAY) {
            print_dbgdata(list);
            fprintf(stderr, "error: at i=%lu incorrect value removed %lu\n", i, e);
            return TEST_FAILURE;
        }
    }
    
    // make sure the rest of the values are odd
    for (size_t i = 0; i < (elements / 2); i += 1) {
        uintptr_t e = (uintptr_t) list_get(list, i);
        if ((e % 2 != 1) || list->err != LIST_OKAY) {
            print_dbgdata(list);
            fprintf(stderr, "error: at i=%lu incorrect value removed %lu\n", i, e);
            return TEST_FAILURE;
        }
    }
    
    // clean up
    list_free(list);
    return TEST_SUCCESS;
}

int t06_linkedlist_frontback_tests(uint32_t num_items) {
    // Run all the cases where data is inserted/removed fron the front/back of the list
    for (uint32_t mode = 0; mode < 4; mode += 1) {
        LinkedList_t* list = linkedlist_init();

        // Add items to the list
        for (uintptr_t data = 1; data < (num_items + 1); data += 1) {
            // Add the data
            if ((mode & 0x01) == 0) {
                linkedlist_add_front(list, (void*) data);
            } else if ((mode & 0x01) == 0x01) {
                linkedlist_add_back(list, (void*) data);
            }

            // Make sure the list is okay
            if (list->err != LIST_OKAY) {
                print_dbgdata(list);
                fprintf(stderr, "list not OKAY (mode=0x%0x, data=%u)\n", mode, (uint32_t) data);
                return TEST_FAILURE;
            }
        }

        // Remove items from the list and place them into an array
        uint32_t values[num_items];
        for (uint32_t i = 0; i < num_items; i += 1) {
            // Remove the data
            uintptr_t e = 0;
            if ((mode & 0x02) == 0) {
                e = (uintptr_t) linkedlist_remove_front(list);
            } else if ((mode & 0x02) == 0x02) {
                e = (uintptr_t) linkedlist_remove_back(list);
            }
            values[i] = (uint32_t) e;

            // Make sure the list is okay
            if (list->err != LIST_OKAY) {
                print_dbgdata(list);
                fprintf(stderr, "list not OKAY (mode=0x%0x, data=%u)\n", mode, (uint32_t) e);
                return TEST_FAILURE;
            }
        }

        // Reverse the output array if this is a front/front or rear/rear
        if (mode == 0 || mode == 3) {
            uint32_t tmp[num_items];
            memcpy(tmp, values, sizeof(uint32_t) * num_items);
            for (int i = 0; i < num_items; i += 1) {
                values[i] = tmp[num_items - i - 1];
            }
        }

        // Finally, make sure the array is right
        for (uint32_t i = 0; i < num_items; i += 1) {
            uint32_t j = values[i];
            if ((j - 1) != i) {
                print_dbgdata(list);
                fprintf(stderr, "incorrect value removed (j=%u, i=%u, mode=0x%0x, num_items=%u)\n", j, i, mode, num_items);
                return TEST_FAILURE;
            }
        }       

        linkedlist_free(list);
    }

    return TEST_SUCCESS;
}

/**
 * Code entry point
 *
 * Should run all the given tests
 */
int main() {
    // Run tests on both types of list
    int error = 0;
    ListImplementation_t types[] = {LIST_ARRAY, LIST_LINKED};
    for (size_t i = 0; i < num_elements(types); i += 1) {

        ListImplementation_t type = types[i];

        // Run the simple tests first and accumlate the error
        error += t01_creation(type);
        error += t02_basic_tests(type);
        error += t03_basic_error_handling(type);

        // Run the complex/strain tests, accumulating the same error value
        error += t05_strain_test(type, FULL_TEST_ITEMS);

        // Run custom tests, continuing to accumulate the error value
        if (type == LIST_ARRAY) {
            error += t04_arraylist_add_pos_0(); // only run once, since it's an array list only test
        } else if (type == LIST_LINKED) {
            for (int j = 2; j < 8; j += 1) {
                error += t06_linkedlist_frontback_tests((uint32_t) j);
            }
        }
        
    }

    // Tests finished, handle the error code
    if (error == 0) {
        printf("success!\n");
        return EXIT_SUCCESS;
    } else {
        printf("^^^ test errors\n");
        return EXIT_FAILURE;
    }
}
