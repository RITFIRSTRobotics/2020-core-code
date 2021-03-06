/*
 * core/collections/queue.h
 *
 * A queue implementation using a linked list
 *
 * @author Connor Henley, @thatging3rkid
 */
#ifndef __CORE_COLLECTION_QUEUE
#define __CORE_COLLECTION_QUEUE

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <pthread.h>

#include "linkedlist.h"

// Queue structure "extends" LinkedList_t
#pragma pack(push, 1) // disable struct packing
typedef struct Queue {
    // "inherited" from List_t
    enum ListImplementation impl;
    enum ListError err;
    pthread_mutex_t mutex;

    // "inherited" from LinkedList
    struct LinkedListNode* first;
    struct LinkedListNode* last;

    // condition variable for the queue
    pthread_cond_t condition;
} Queue_t;
#pragma pack(pop) // return struct packing

/**
 * Initialize a queue
 *
 * @return the queue data structure (or NULL if an error occurred)
 * @error returns NULL if error occured
 */
Queue_t* queue_init();

/**
 * Add an item to the back of the queue
 *
 * @param queue the queue data structure
 * @param element the element to add to the queue
 * @return the underlying list error code (see enum ListError)
 * @error err is set to LIST_MEMORY if an memory request failed 
 */
ListError_t queue_enqueue(Queue_t* queue, void* element);

/**
 * Remove the item that is at the front of the queue
 *
 * @param queue the queue data structure
 * @return the element that was removed from the queue (or NULL if the queue is empty)
 * @error err is set to LIST_BOUNDS if the queue is empty
 */
void* queue_dequeue(Queue_t* queue);

/**
 * View the element that is at the front of the queue
 *
 * @param queue the queue data structure
 * @return the element that is at the front of the queue (or NULL if the queue is empty)
 * @error err is set to LIST_BOUNDS if the queue is empty 
 */
void* queue_peek(Queue_t* queue);

/**
 * View the element that is at a given point in the queue
 *
 * @param queue the queue data structure 
 * @param pos the position in the queue to peek at (0 would be the first item in line, 
 *     1 would be the second in line, etc.)
 * @return the element that is at the given position in the queue (or NULL if the position is invalid)
 * @error err is set to LIST_BOUNDS if the queue is empty or the position is invalid
 */
void* queue_peek_pos(Queue_t* queue, uint32_t pos);

/**
 * Get the size of the list (number of elements in the queue)
 *
 * @note the preferred way to test if the queue is empty is to call queue_peek and check for NULL
 * @param queue the queue data structure
 * @return the size of the list
 * @error none
 */
uint32_t queue_size(Queue_t* queue);

/**
 * Wait for the queue to have one or more elements in it
 *
 * @note this method is designed to be used instead of polling the list to see if it's empty
 * @param none
 * @returns none
 * @error none
 */
void queue_block(Queue_t* queue);

/**
 * Clean up the list
 *
 * @param queue the list data structure
 * @error none
 */
void queue_free(Queue_t* queue);

#ifdef __cplusplus
}
#endif

#endif
