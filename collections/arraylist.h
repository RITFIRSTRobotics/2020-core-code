/*
 * core/collections/arraylist.h
 *
 * An array list implementation
 *
 * @author Connor Henley, @thatging3rkid
 */
#ifndef __CORE_COLLECTION_ARRAYLIST

// do some preprocessor voodoo
#ifdef _LOCAL_HEADER
    #include "list.h"
#else
    #include "core/collections/list.h"
#endif

// Define list parameters
#define DEFAULT_LIST_LENGTH 10
#define DEFAULT_LIST_STEP   25

// Array list structure "extends" List_t
typedef struct ArrayList {
    // "inherited"
    enum ListImplementation impl;
    enum ListError err;
    pthread_mutex_t mutex;

    // Array list specific
    uint32_t len; // length of the array in elements
    uint32_t allocated; // length of memory allocated (in elements)
    void** array; // element array
} ArrayList_t;

/**
 * Initialize an array list with the default list length
 *
 * @return the array list data structure (or NULL if an error occured)
 * @error returns NULL if an error occured
 */
ArrayList_t* arraylist_init();

/**
 * Initialize an array list with a given list length
 *
 * @param init_len the initial list length
 * @return the array list data structure (or NULL if an error occurred)
 * @error returns NULL if error occurred
 */
ArrayList_t* arraylist_init_len(uint32_t init_len);

/**
 * Add an item to the end of the array list
 *
 * @param list the array list data structure
 * @param element the element to add to the list
 * @return the list error code (see enum ListError)
 * @error err is set to LIST_MEMORY if a memory request failed
 *            or LIST_BOUNDS if pos is out of bounds
 */ 
ListError_t arraylist_add(ArrayList_t* list, void* element);

/**
 * Add an item to the array list in any position
 *
 * @param list the array list data structure
 * @param pos the position to place the element (must be in range 0 to array length)
 * @param element the element to add to the list
 * @return the list error code (see enum ListError)
 * @error err is set to LIST_MEMORY if a memory request failed
 *            or LIST_BOUNDS if pos is out of bounds
 */
ListError_t arraylist_add_pos(ArrayList_t* list, uint32_t pos, void* element);

/**
 * Get an item from the array list
 *
 * @param list the array list data structure
 * @param pos the position to read from (must be in range 0 to array length)
 * @return the element read from the list
 * @error err is set to LIST_BOUNDS if pos is out of bounds
 */
void* arraylist_get(ArrayList_t* list, uint32_t pos);

/**
 * Remove an item from the array list
 *
 * @param list the array list data structure
 * @param pos the position of the element to remove
 * @return the element that was removed from the list
 * @error err is set to LIST_BOUNDS if pos is out of bounds
 */
void* arraylist_remove(ArrayList_t* list, uint32_t pos);

/**
 * Get the size of the array list (number of elements)
 *
 * @param list the array list data structure
 * @return the size of the list
 * @error none
 */
uint32_t arraylist_size(ArrayList_t* list);

/**
 * Clean up the array list
 *
 * @param list the array list data structure
 * @error none
 */
void arraylist_free(ArrayList_t* list);

#endif
