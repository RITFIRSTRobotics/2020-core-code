/**
 * core/collections/linkedlist.c
 *
 * A linked list implementation
 *
 * @author Connor Henley, @thatging3rkid
 */
#include <stdint.h>
#include <stdlib.h>
#include <memory.h>

// do some preprocessor voodoo
#ifdef _LOCAL_HEADER
    #include "linkedlist.h"
#else
    #include "core/collections/linkedlist.h"
#endif

/**
 * Create a linked list node
 *
 * @private
 * @return returns NULL if an error occured
 */
static LinkedListNode_t* _create_llnode() {
    // Ask for memory and made sure that it was allocated
    LinkedListNode_t* res = malloc(sizeof(LinkedListNode_t));
    if (res == NULL) {
        return NULL;
    }

    // Clear the fields
    res->data = NULL;
    res->next = NULL;
    return res;
}

/**
 * @inherit
 */
LinkedList_t* linkedlist_init() {
    // Ask for memory and made sure that it was allocated
    LinkedList_t* res = malloc(sizeof(LinkedList_t));
    if (res == NULL) {
        return NULL;
    }

    // Start filling in the struct
    res->impl = LIST_LINKED;
    res->err = LIST_OKAY;
    res->first = NULL;
    res->last = NULL;

    return res;    
}

/**
 * @inherit
 */
ListError_t linkedlist_add_front(LinkedList_t* list, void* element) {
    // Create a node and populate it
    LinkedListNode_t* new = _create_llnode();
    if (new == NULL) {
        list->err = LIST_MEMORY;
        return list->err;
    }
    new->data = element;
    new->next = list->first;

    // Update the first/last poniters of the data structure
    if (list->last == NULL) {
        list->last = new; // catch case if this is an empty list
    }

    list->first = new;
    return LIST_OKAY;
}

/**
 * @inherit
 */
ListError_t linkedlist_add_back(LinkedList_t* list, void* element) {
    // Create a new node and populate it
    LinkedListNode_t* new = _create_llnode();
    if (new == NULL) {
        list->err = LIST_MEMORY;
        return list->err;
    }
    new->data = element;
    new->next = NULL; // end of list

    // Update the first/last poniters of the data structure
    if (list->first == NULL) {
        list->first = new; // catch case if this is an empty list
    } else {
        list->last->next = new;
    }

    list->last = new;
    return LIST_OKAY;
}

/**
 * @inherit
 */
ListError_t linkedlist_add_pos(LinkedList_t* list, uint32_t pos, void* element) {
    return LIST_OKAY;
}

/**
 * @inherit
 */
void* linkedlist_get(LinkedList_t* list, uint32_t pos) {
    return NULL;
}

/**
 * @inherit
 */
void* linkedlist_remove(LinkedList_t* list, uint32_t pos) {
    return NULL;
}

/**
 * @inherit
 */
uint32_t linkedlist_size(LinkedList_t* list) {
    uint32_t count = 0;
    LinkedListNode_t* node = list->first;

    // Iterate over all nodes
    while (node != NULL) {
        node = node->next;
        count += 1;
    }

    return count;
}

/**
 * @inherit
 */
void linkedlist_free(LinkedList_t* list) {
    
}
