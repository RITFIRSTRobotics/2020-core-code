/*
 * core/collections/stack.h
 *
 * A stack implementation using a linked list
 *
 * @author Connor Henley, @thatging3rkid
 */
#ifndef __CORE_COLLECTION_STACK
#define __CORE_COLLECTION_STACK

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <pthread.h>

// do some preprocessor voodoo
#ifdef _LOCAL_HEADER
    #include "linkedlist.h"
#else
    #include "core/collections/linkedlist.h"
#endif

// Use a linked list as the backend for the stack
typedef struct LinkedList Stack_t;

/**
 * Initialize a stack
 *
 * @return the stack data structure (or NULL if an error occurred)
 * @error returns NULL if error occured
 */
Stack_t* stack_init();

/**
 * Add an item to the top of the stack
 *
 * @param stack the stack data structure
 * @param element the element to add to the stack
 * @return the underlying list error code (see enum ListError)
 * @error err is set to LIST_MEMORY if an memory request failed 
 */
ListError_t stack_push(Stack_t* stack, void* element);

/**
 * Remove the item that is on top of the stack
 *
 * @param stack the stack data structure
 * @return the element that was removed from the stack (or NULL if the stack is empty)
 * @error err is set to LIST_BOUNDS if the stack is empty
 */
void* stack_pop(Stack_t* stack);

/**
 * View the element that is on top of the stack
 *
 * @param stack the stack data structure
 * @return the element that is on top of the stack (or NULL if the stack is empty)
 * @error err is set to LIST_BOUNDS if the stack is empty 
 */
void* stack_peek(Stack_t* stack);

/**
 * View the element that is at a given point in the stack
 *
 * @param stack the stack data structure 
 * @param pos the position in the stack to peek at (0 would be the top-most item, 
 *     1 would be one down from the top, etc.)
 * @return the element that is at the given position in the stack (or NULL if the position is invalid)
 * @error err is set to LIST_BOUNDS if the stack is empty or the position is invalid
 */
void* stack_peek_pos(Stack_t* stack, uint32_t pos);

/**
 * Get the size of the list (number of elements in the stack)
 *
 * @note the preferred way to test if the list is empty is to call stack_peek and check for NULL
 * @param stack the stack data structure
 * @return the size of the list
 * @error none
 */
uint32_t stack_size(Stack_t* stack);

/**
 * Clean up the list
 *
 * @param stack the stack data structure
 * @error none
 */
void stack_free(Stack_t* stack);

#ifdef __cplusplus
}
#endif

#endif
