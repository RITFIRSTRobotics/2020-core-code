/*
 * core/collections/list.h
 *
 * An abstract list implementation
 *
 * @author Connor Henley, @thatging3rkid
 */
#ifndef __CORE_COLLECTION_LIST
#define __CORE_COLLECTION_LIST
#include <stdint.h>

#define DEFAULT_LIST_LENGTH 10
#define DEFAULT_LIST_STEP   25

// Error enumeration
typedef enum ListError {
    LIST_OKAY   = 0,
    LIST_BOUNDS = 0x1000,
    LIST_MEMORY = 0x1001
} ListError_t;

// List structure
typedef struct List {
    uint32_t len;
    uint32_t raw_len;
    enum ListError err;
    // 32-bit pad, could be used in the future
    void** array; 
} List_t;

/**
 * Initialize a list with the default list length
 *
 * @return the list data structure (or NULL if an error occurred)
 * @error returns NULL if error occurred
 */
List_t* list_init();

/**
 * Initialize a list with a given list length
 *
 * @param init_len the initial list length
 * @return the list data structure (or NULL if an error occurred)
 * @error returns NULL if error occurred
 */
List_t* list_init_len(uint32_t init_len);

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

#endif
