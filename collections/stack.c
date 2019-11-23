/**
 * core/collections/stack.c
 *
 * A stack implementation using a linked list
 *
 * @author Connor Henley, @thatging3rkid
 */
#include <stdint.h>

// do some preprocessor voodoo
#ifdef _LOCAL_HEADER
    #include "stack.h"
    #include "linkedlist.h"
#else
    #include "core/collections/stack.h"
    #include "core/collections/linkedlist.h"
#endif

/**
 * @inherit
 */
Stack_t* stack_init() {
    return (Stack_t*) linkedlist_init();
}

/**
 * @inherit
 */
ListError_t stack_push(Stack_t* stack, void* element) {
    return linkedlist_add_front((LinkedList_t*) stack, element);
}

/**
 * @inherit
 */
void* stack_pop(Stack_t* stack) {
    return linkedlist_remove_front((LinkedList_t*) stack);
}

/**
 * @inherit
 */
void* stack_peek(Stack_t* stack) {
    return linkedlist_get((LinkedList_t*) stack, 0);
}

/**
 * @inherit
 */
void* stack_peek_pos(Stack_t* stack, uint32_t pos) {
    return linkedlist_get((LinkedList_t*) stack, pos);
}

/**
 * @inherit
 */
uint32_t stack_size(Stack_t* stack) {
    return linkedlist_size((LinkedList_t*) stack);
}

/**
 * @inherit
 */
void stack_free(Stack_t* stack) {
    linkedlist_free((LinkedList_t*) stack);
}
