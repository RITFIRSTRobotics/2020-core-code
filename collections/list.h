/*
 * core/collections/list.h
 *
 * A generic list wrapper, can be instantiated as a linked or array list
 *
 * @author Connor Henley, @thatging3rkid
 */
#ifndef __CORE_COLLECTION_LIST
#define __CORE_COLLECTION_LIST

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <pthread.h>

// Error enumeration
typedef enum ListError {
    LIST_OKAY          = 0,
    LIST_BOUNDS        = 0x1000,
    LIST_MEMORY        = 0x1001,
    LIST_INVALID_IMPL  = 0x1002,
    LIST_INVALID_STATE = 0x1003,
    LIST_UNSUPPORTED   = 0x1004
} ListError_t;

// List implementation enumeration
typedef enum ListImplementation {
    LIST_ARRAY  = 0,
    LIST_LINKED = 0x01
} ListImplementation_t;

// "Abstract" list structure
typedef struct List {
    enum ListImplementation impl;
    enum ListError err;
    pthread_mutex_t mutex;
    
    // abstract: add more below here
} List_t;

/**
 * Initialize a list with the default list length
 *
 * @param impl the list implementation to use
 * @return the list data structure (or NULL if an error occurred)
 * @error returns NULL if error occurred
 */
List_t* list_init(ListImplementation_t impl);

/**
 * Add an item to the end of the list
 *
 * @param list the list data structure
 * @param element the element to add to the end of the list
 * @return the list error code (see enum ListError)
 * @error err is set to LIST_MEMORY if a memory request failed
 */ 
ListError_t list_add(List_t* list, void* element);

/**
 * Add an item to the list in any position
 *
 * @param list the list data structure
 * @param pos the position to place the element (must be in range 0 to array length)
 * @param element the element to add to the list
 * @return the list error code (see enum ListError)
 * @error err is set to LIST_MEMORY if a memory request failed
 *            or LIST_BOUNDS if pos is out of bounds
 */ 
ListError_t list_add_pos(List_t* list, uint32_t pos, void* element);

/**
 * Get an item from the list
 *
 * @param list the list data structure
 * @param pos the position to read from (must be in range 0 to array length)
 * @return the element read from the list
 * @error err is set to LIST_BOUNDS if pos is out of bounds
 */
void* list_get(List_t* list, uint32_t pos);

/**
 * Remove an item from the list
 *
 * @param list the list data structure
 * @param pos the position of the element to remove
 * @return the element that was removed from the list
 * @error err is set to LIST_BOUNDS if pos is out of bounds
 */
void* list_remove(List_t* list, uint32_t pos);

/**
 * Get the size of the list (number of elements)
 *
 * @param list the list data structure
 * @return the size of the list 
 * @error none
 */
uint32_t list_size(List_t* list);

/**
 * Clean up the list 
 *
 * @param list the list data structure
 * @error none
 */
void list_free(List_t* list);

#ifdef __cplusplus
}
#endif

#endif
