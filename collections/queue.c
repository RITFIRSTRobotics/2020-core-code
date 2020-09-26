/**
 * core/collections/queue.c
 *
 * A queue implementation using a linked list
 *
 * @author Connor Henley, @thatging3rkid
 */
#include <stdint.h>
#include <stdlib.h>

#include "queue.h"
#include "linkedlist.h"

/**
 * @inherit
 */
Queue_t* queue_init() {
    // Use the linked list initializer, but realloc
    Queue_t* queue = (Queue_t*) linkedlist_init();
    if (queue == NULL) {
        return NULL;
    } else if (queue->err != LIST_OKAY || queue->impl != LIST_LINKED) {
        queue->impl = LIST_INVALID_IMPL;
        return queue;
    }

    // Increase the memory space
    Queue_t* temp = realloc(queue, sizeof(Queue_t));
    if (temp == NULL) {
        queue->err = LIST_MEMORY;
        return queue;
    }
    queue = temp;

    // Setup the condition variable
    pthread_cond_init(&queue->condition, NULL);

    return queue;
}

/**
 * @inherit
 */
ListError_t queue_enqueue(Queue_t* queue, void* element) {
    // Insert data, notify threads of new data, and return
    ListError_t err = linkedlist_add_front((LinkedList_t*) queue, element);
    pthread_cond_signal(&queue->condition);
    return err;
}

/**
 * @inherit
 */
void* queue_dequeue(Queue_t* queue) {
    return linkedlist_remove_back((LinkedList_t*) queue);
}

/**
 * @inherit
 */
void* queue_peek(Queue_t* queue) {
    return linkedlist_get((LinkedList_t*) queue, 0);
}

/**
 * @inherit
 */
void* queue_peek_pos(Queue_t* queue, uint32_t pos) {
    return linkedlist_get((LinkedList_t*) queue, pos);
}

/**
 * @inherit
 */
uint32_t queue_size(Queue_t* queue) {
    return linkedlist_size((LinkedList_t*) queue);
}

/**
 * @inherit
 */
void queue_block(Queue_t* queue) {
    // Wait for something to be entered into the queue
    pthread_mutex_lock(&queue->mutex);
    while (queue_peek(queue) == NULL) {
        pthread_cond_wait(&queue->condition, &queue->mutex);
    }
    pthread_mutex_unlock(&queue->mutex);
}

/**
 * @inherit
 */
void queue_free(Queue_t* queue) {
    pthread_cond_destroy(&queue->condition);
    linkedlist_free((LinkedList_t*) queue);
}
