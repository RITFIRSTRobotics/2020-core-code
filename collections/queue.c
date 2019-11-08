/**
 * core/collections/queue.c
 *
 * A queue implementation using a linked list
 *
 * @author Connor Henley, @thatging3rkid
 */
#include <stdint.h>

// do some preprocessor voodoo
#ifdef _LOCAL_HEADER
    #include "queue.h"
    #include "linkedlist.h"
#else
    #include "core/collections/queue.h"
    #include "core/collections/linkedlist.h"
#endif

/**
 * @inherit
 */
Queue_t* queue_init() {
    return (Queue_t*) linkedlist_init();
}

/**
 * @inherit
 */
ListError_t queue_enqueue(Queue_t* queue, void* element) {
    return linkedlist_add_front((LinkedList_t*) queue, element);
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
void queue_free(Queue_t* queue) {
    linkedlist_free((LinkedList_t*) queue);
}
