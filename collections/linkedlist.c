/**
 * core/collections/linkedlist.c
 *
 * A linked list implementation
 *
 * @author Connor Henley, @thatging3rkid
 */
#define _GNU_SOURCE // required for pthread mutex attributes
#include <stdint.h>
#include <stdlib.h>
#include <memory.h>
#include <pthread.h>
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
static LinkedListNode_t* _llnode_create(void* data, LinkedListNode_t* next, LinkedListNode_t* prev) {
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
 * Find the linked list node in a given position
 *
 * @private
 * @param list the list data structure
 * @param pos the position of the node
 * @param the pre-calculated size of the list
 * @note if the size has not been calculated, it is faster to search the list than use this method
 * @return the node, or NULL if it could not be found
 */
static LinkedListNode_t* _llnode_get(LinkedList_t* list, uint32_t pos, uint32_t size) {
    // Decide whether to search from the front or back
    LinkedListNode_t* node = NULL;
    if (pos < (size / 2)) {
        // Start from the front
        uint32_t count = 0;
        node = list->first;
        while (node != NULL && count < pos) {
            node = node->next;
            count += 1;
        }
    } else {
        // Start from the back
        uint32_t count = size - 1;
        node = list->last;
        while (node != NULL && count > pos) {
            node = node->prev;
            count -= 1;
        }
    }

    return node;
}

/**
 * Destory a linked list node
 *
 * @private
 * @param node the linked list node to destroy
 * @param refs if true, references to this node in the neighbor nodes will be destroyed
 */
static void _llnode_free(LinkedListNode_t* node, bool refs) {
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

    // Initialize the mutex
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&res->mutex, &attr);

    return res;    
}

/**
 * @inherit
 */
ListError_t linkedlist_add_front(LinkedList_t* list, void* element) {
    // First, lock the list
    pthread_mutex_lock(&list->mutex);
    
    // Create a node and populate it
    LinkedListNode_t* new = _llnode_create(element, list->first, NULL);
    if (new == NULL) {
        list->err = LIST_MEMORY;
        pthread_mutex_unlock(&list->mutex);
        return LIST_MEMORY;
    }

    // See if the list is empty or not
    if (list->first == NULL && list->last == NULL) {
        // List is empty, just update the last element pointer
        list->last = new;
    } else {
        // List is not empty, so update the previous first element
        list->first->prev = new;
    }

    // Update the list, then unlock it
    list->first = new;
    pthread_mutex_unlock(&list->mutex);
    
    return LIST_OKAY;
}

/**
 * @inherit
 */
ListError_t linkedlist_add_back(LinkedList_t* list, void* element) {
    // First, lock the list
    pthread_mutex_lock(&list->mutex);

    // Create a new node and populate it
    LinkedListNode_t* new = _llnode_create(element, NULL, list->last);
    if (new == NULL) {
        list->err = LIST_MEMORY;
        pthread_mutex_unlock(&list->mutex);
        return LIST_MEMORY;
    }

    // See if the list is empty or not
    if (list->first == NULL && list->last == NULL) {
        // List is empty, just update the first element pointer
        list->first = new;
    } else {
        // List is not empty, so update the previous last element
        list->last->next = new;
    }
    
    // Update the list, then unlock it
    list->last = new;
    pthread_mutex_unlock(&list->mutex);
    
    return LIST_OKAY;
}

/**
 * @inherit
 */
ListError_t linkedlist_add_pos(LinkedList_t* list, uint32_t pos, void* element) {
    // First, lock the list
    pthread_mutex_lock(&list->mutex);

    // Next, do a bounds check
    uint32_t size = linkedlist_size(list);
    if (size < pos) {
        list->err = LIST_BOUNDS;
        pthread_mutex_unlock(&list->mutex);
        return LIST_BOUNDS;
    }

    // Use other methods if adding to the front of back of the list
    if (pos == 0) {
        ListError_t err = linkedlist_add_front(list, element);
        pthread_mutex_unlock(&list->mutex);
        return err;
    } else if (pos == size) {
        ListError_t err = linkedlist_add_back(list, element);
        pthread_mutex_unlock(&list->mutex);
        return err;
    }

    // Pick the two neighboring nodes
    LinkedListNode_t* right = _llnode_get(list, pos, size);
    LinkedListNode_t* left = right->prev;

    // Make sure this node has valid neighbors
    if (left == NULL || right == NULL) {
        // At least one neighbor is missing, add_front or add_back should have been used
        list->err = LIST_INVALID_STATE;
        return list->err;
    }

    // Make the new node and link it in
    LinkedListNode_t* new = _llnode_create(element, left, right);
    left->next = new;    
    right->next = new;

    // Unlock the list
    pthread_mutex_unlock(&list->mutex);

    return LIST_OKAY;
}

/**
 * @inherit
 */
void* linkedlist_get(LinkedList_t* list, uint32_t pos) {
    // Lock the list and find the node
    pthread_mutex_lock(&list->mutex);

    // Next, do a bounds check
    uint32_t size = linkedlist_size(list);
    if (size <= pos) {
        list->err = LIST_BOUNDS;
        pthread_mutex_unlock(&list->mutex);
        return NULL;
    }

    // Find the data and return the mutex
    void* result = _llnode_get(list, pos, size)->data;
    pthread_mutex_unlock(&list->mutex);
    return result;
}

/**
 * @inherit
 */
void* linkedlist_remove_front(LinkedList_t* list) {
    // First, lock the list
    pthread_mutex_lock(&list->mutex);

    // Make sure the list is not empty
    if (list->first == NULL || list->last == NULL) {
        list->err = LIST_BOUNDS;
        pthread_mutex_unlock(&list->mutex);
        return NULL;
    }

    // Remove the first node and patch the second node up
    LinkedListNode_t* remove = list->first;
    void* data = remove->data;
    list->first = remove->next;
    if (remove->next == NULL) {
        list->last = NULL;
    }
    _llnode_free(remove, true);
    pthread_mutex_unlock(&list->mutex);
    return data;
}

/**
 *
 */
void* linkedlist_remove_back(LinkedList_t* list) {
    // First, lock the list
    pthread_mutex_lock(&list->mutex);

    // Make sure the list is not empty
    if (list->first == NULL || list->last == NULL) {
        list->err = LIST_BOUNDS;
        pthread_mutex_unlock(&list->mutex);
        return NULL;
    }

    // Remove the last node and patch the second-to-last node up
    LinkedListNode_t* remove = list->last;
    void* data = remove->data;
    list->last = remove->prev;
    if (remove->prev == NULL) {
        list->first = NULL;
    }
    _llnode_free(remove, true);
    pthread_mutex_unlock(&list->mutex);
    return data;
}


/**
 * @inherit
 */
void* linkedlist_remove_pos(LinkedList_t* list, uint32_t pos) {
    // First, lock the list
    pthread_mutex_lock(&list->mutex);
    
    // Next, do a bounds check
    uint32_t size = linkedlist_size(list);
    if (size <= pos) {
        list->err = LIST_BOUNDS;
        pthread_mutex_unlock(&list->mutex);
        return NULL;
    }

    // Use other methods if adding to the front of back of the list
    if (pos == 0) {
        void* result = linkedlist_remove_front(list);
        pthread_mutex_unlock(&list->mutex);
        return result;
    } else if (pos == size) {
        void* result = linkedlist_remove_back(list);
        pthread_mutex_unlock(&list->mutex);
        return result;
    }

    // Find the afflicted node
    LinkedListNode_t* remove = _llnode_get(list, pos, size);
    LinkedListNode_t* left = remove->prev;
    LinkedListNode_t* right = remove->next;

    // Make sure this node has valid neighbors
    if (left == NULL || right == NULL) {
        // At least one neighbor is missing, remove_front or remove_back should have been used
        list->err = LIST_INVALID_STATE;
        pthread_mutex_unlock(&list->mutex);
        return NULL;
    }

    // Remove the afflicted node and patch the other two together
    void* data = remove->data;
    _llnode_free(remove, true);
    left->next = right;
    right->prev = left;
    pthread_mutex_unlock(&list->mutex);
    
    return data;
}

/**
 * @inherit
 */
uint32_t linkedlist_size(LinkedList_t* list) {
    // First, lock the list
    pthread_mutex_lock(&list->mutex);
    
    uint32_t count = 0;
    LinkedListNode_t* node = list->first;

    // Iterate over all nodes
    while (node != NULL) {
        node = node->next;
        count += 1;
    }

    // Unlock the list
    pthread_mutex_unlock(&list->mutex);
    
    return count;
}

/**
 * @inherit
 */
void linkedlist_free(LinkedList_t* list) {
    // Clean any nodes if the last has them
    pthread_mutex_lock(&list->mutex);
    if (list->first != NULL && list->last != NULL) {
        // Iterate over each element and clean them up
        LinkedListNode_t* cur = list->first;
        LinkedListNode_t* next;
        while (cur != NULL) {
            next = cur->next;
            _llnode_free(cur, true);
            cur = next;
        }
    }

    // Clean up the mutex (destroying a locked mutex is undefined)
    pthread_mutex_unlock(&list->mutex);
    pthread_mutex_destroy(&list->mutex);
    
    // Clean up the data structure
    free(list);
    list = NULL;
}
