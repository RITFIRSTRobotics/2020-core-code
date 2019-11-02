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
#include <stdbool.h>

// do some preprocessor voodoo
#ifdef _LOCAL_HEADER
    #include "list.h"
    #include "linkedlist.h"
#else
    #include "core/collections/list.h"
    #include "core/collections/linkedlist.h"
#endif

/**
 * Create a linked list node
 *
 * @private
 * @param data the data for this node
 * @param next the next node in the list
 * @param prev the previous node in the list
 * @return returns NULL if an error occured
 */
static LinkedListNode_t* _create_llnode(void* data, LinkedListNode_t* next, LinkedListNode_t* prev) {
    // Ask for memory and made sure that it was allocated
    LinkedListNode_t* res = malloc(sizeof(LinkedListNode_t));
    if (res == NULL) {
        return NULL;
    }

    // Setup the fields
    res->data = data;
    res->next = next;
    res->prev = prev;
    return res;
}

/**
 * Destory a linked list node
 *
 * @private
 * @param node the linked list node to destroy
 * @param refs if true, references to this node in the neighbor nodes will be destroyed
 */
static void _destroy_llnode(LinkedListNode_t* node, bool refs) {
    // Clear the fields of this node and the neighboring nodes if specified
    if (refs && node->next != NULL) {
        node->next->prev = NULL;
    }
    node->next = NULL;
    if (refs && node->prev != NULL) {
        node->prev->next = NULL;
    }
    node->prev = NULL;
    node->data = NULL;

    // Free the memory
    free(node);
    node = NULL;
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
    LinkedListNode_t* new = _create_llnode(element, list->first, NULL);
    if (new == NULL) {
        list->err = LIST_MEMORY;
        return list->err;
    }

    // See if the list is empty or not
    if (list->first == NULL && list->last == NULL) {
        // List is empty, just update the last element pointer
        list->last = new;
    } else {
        // List is not empty, so update the previous first element
        list->first->prev = new;
    }

    // Update the data structure
    list->first = new;

    return LIST_OKAY;
}

/**
 * @inherit
 */
ListError_t linkedlist_add_back(LinkedList_t* list, void* element) {
    // Create a new node and populate it
    LinkedListNode_t* new = _create_llnode(element, NULL, list->last);
    if (new == NULL) {
        list->err = LIST_MEMORY;
        return list->err;
    }

    // See if the list is empty or not
    if (list->first == NULL && list->last == NULL) {
        // List is empty, just update the first element pointer
        list->first = new;
    } else {
        // List is not empty, so update the previous last element
        list->last->next = new;
    }
    
    // Update the data structure
    list->last = new;
    
    return LIST_OKAY;
}

/**
 * @inherit
 */
ListError_t linkedlist_add_pos(LinkedList_t* list, uint32_t pos, void* element) {
    // First, do a bounds check
    uint32_t size = linkedlist_size(list);
    if (size < pos) {
        list->err = LIST_BOUNDS;
        return list->err;
    }

    // Use other methods if adding to the front of back of the list
    if (pos == 0) {
        return linkedlist_add_front(list, element);
    } else if (pos == size) {
        return linkedlist_add_back(list, element);
    }  

    // Pick the two neighboring nodes
    LinkedListNode_t* left = list->first;
    uint32_t count = pos;
    while (count > 1) {
        left = left->next;
        count -= 1;
    }
    LinkedListNode_t* right = left->next;

    // Make sure this node has valid neighbors
    if (left == NULL || right == NULL) {
        // At least one neighbor is missing, add_front or add_back should have been used
        list->err = LIST_INVALID_STATE;
        return list->err;
    }

    // Make the new node and link it in
    LinkedListNode_t* new = _create_llnode(element, left, right);
    left->next = new;    
    right->next = new;

    return LIST_OKAY;
}

/**
 * @inherit
 */
void* linkedlist_get(LinkedList_t* list, uint32_t pos) {
    uint32_t count = 0;
    LinkedListNode_t* node = list->first;

    // Iterate over all nodes
    while (node != NULL && count < pos) {
        node = node->next;
        count += 1;
    }

    // Check to see if the node was found
    if (node == NULL) {
        return NULL;
    } else {
        return node->data;
    }
}

/**
 * @inherit
 */
void* linkedlist_remove_front(LinkedList_t* list) {
    // Make sure the list is not empty
    if (list->first == NULL || list->last == NULL) {
        list->err = LIST_BOUNDS;
        return NULL;
    }

    // Remove the first node and patch the second node up
    LinkedListNode_t* remove = list->first;
    void* data = remove->data;
    list->first = remove->next;
    _destroy_llnode(remove, true);
    return data;
}

/**
 *
 */
void* linkedlist_remove_back(LinkedList_t* list) {
    // Make sure the list is not empty
    if (list->first == NULL || list->last == NULL) {
        list->err = LIST_BOUNDS;
        return NULL;
    }

    // Remove the last node and patch the second-to-last node up
    LinkedListNode_t* remove = list->last;
    void* data = remove->data;
    list->last = remove->prev;
    _destroy_llnode(remove, true);
    return data;
}


/**
 * @inherit
 */
void* linkedlist_remove_pos(LinkedList_t* list, uint32_t pos) {
    // First, do a bounds check
    uint32_t size = linkedlist_size(list);
    if (size < pos) {
        list->err = LIST_BOUNDS;
        return NULL;
    }

    // Use other methods if adding to the front of back of the list
    if (pos == 0) {
        return linkedlist_remove_front(list);
    } else if (pos == size) {
        return linkedlist_remove_back(list);
    }

    // Find the afflicted node
    LinkedListNode_t* remove = list->first;
    uint32_t count = pos;
    while (count > 0) {
        remove = remove->next;
        count -= 1;
    }
    LinkedListNode_t* left = remove->prev;
    LinkedListNode_t* right = remove->next;

    // Make sure this node has valid neighbors
    if (left == NULL || right == NULL) {
        // At least one neighbor is missing, remove_front or remove_back should have been used
        list->err = LIST_INVALID_STATE;
        return NULL;
    }

    // Remove the afflicted node and patch the other two together
    void* data = remove->data;
    _destroy_llnode(remove, true);
    left->next = right;
    right->prev = left;

    return data;
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
    // Clean any nodes if the last has them
    if (list->first != NULL && list->last != NULL) {
        // Iterate over each element and clean them up
        LinkedListNode_t* cur = list->first;
        LinkedListNode_t* next;
        while (cur != NULL) {
            next = cur->next;
            _destroy_llnode(cur, true);
            cur = next;
        }
    }
    
    // Clean up the data structure
    free(list);
    list = NULL;
}
