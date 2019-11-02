/*
 * core/collections/linkedlist.c
 *
 * A linked list implementation
 *
 * @author Connor Henley, @thatging3rkid
 */
#ifndef __CORE_COLLECTION_LINKEDLIST

// do some preprocessor voodoo
#ifdef _LOCAL_HEADER
    #include "list.h"
#else
    #include "core/collections/list.h"
#endif

// Linked list node structure (single link)
typedef struct LinkedListNode {
    void* data;
    struct LinkedListNode* next;
} LinkedListNode_t;

// Linked list structure "extends" List_t
typedef struct LinkedList {
    // "inherited"
    enum ListImplementation impl;
    enum ListError err;

    // Linked list specific
    struct LinkedListNode* first;
    struct LinkedListNode* last;
} LinkedList_t;

/**
 * Initialize an linked list with the default list length
 *
 * @return the linked list data structure (or NULL if an error occured)
 * @error returns NULL if an error occured
 */
LinkedList_t* linkedlist_init();

/**
 * Add an item to the front of the linked list
 *
 * @param list the linked list data structure
 * @param element the element to add to the list
 * @return the list error code (see enum ListError)
 * @error err is set to LIST_MEMORY if a memory request failed
 */
ListError_t linkedlist_add_front(LinkedList_t* list, void* element);

/**
 * Add an item to the back of the linked list
 *
 * @param list the linked list data structure
 * @param element the element to add to the list
 * @return the list error code (see enum ListError)
 * @error err is set to LIST_MEMORY if a memory request failed
 */
ListError_t linkedlist_add_back(LinkedList_t* list, void* element);

/**
 * Add an item to the linked list in any position
 *
 * @param list the linked list data structure
 * @param pos the position to place the element (must be in range 0 to list length)
 * @param element the element to add to the list
 * @return the list error code (see enum ListError)
 * @error err is set to LIST_MEMORY if a memory request failed
 */ 
ListError_t linkedlist_add_pos(LinkedList_t* list, uint32_t pos, void* element);

/**
 * Get an item from the linked list
 *
 * @param list the linked list data structure
 * @param pos the position to read from (must be in range 0 to list length)
 * @return the element read from the list
 * @error err is set to LIST_BOUNDS if pos is out of bounds
 */
void* linkedlist_get(LinkedList_t* list, uint32_t pos);

/**
 * Remove an item from the linked list
 *
 * @param list the linked list data structure
 * @param pos the position of the element to remove
 * @return the element that was removed from the list
 * @error err is set to LIST_BOUNDS if pos is out of bounds
 */
void* linkedlist_remove(LinkedList_t* list, uint32_t pos);

/**
 * Get the size of the linked list (number of elements)
 *
 * @param list the linked list data structure
 * @return the size of the list 
 * @error none
 */
uint32_t linkedlist_size(LinkedList_t* list);

/**
 * Clean up the linked list 
 *
 * @param list the linked list data structure
 * @error none
 */
void linkedlist_free(LinkedList_t* list);

#endif
