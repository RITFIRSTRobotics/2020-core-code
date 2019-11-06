/**
 * core/collections/arraylist.c
 *
 * An array list implementation
 *
 * @author Connor Henley, @thatging3rkid
 */
#define _GNU_SOURCE // required for pthread mutex attributes
#include <stdint.h>
#include <stdlib.h>
#include <memory.h>
#include <pthread.h>

// do some preprocessor voodoo
#ifdef _LOCAL_HEADER
    #include "list.h"
    #include "arraylist.h"
#else
    #include "core/collections/list.h"
    #include "core/collections/arraylist.h"
#endif

/**
 * @inherit
 */
ArrayList_t* arraylist_init() {
    return arraylist_init_len(DEFAULT_LIST_LENGTH);
}

/**
 * @inherit
 */
ArrayList_t* arraylist_init_len(uint32_t init_len) {
    // Ask for memory and make sure the it was allocated
    ArrayList_t* res = malloc(sizeof(ArrayList_t));
    if (res == NULL) {
        return NULL;
    }

    // Start filling in the struct
    res->impl = LIST_ARRAY;
    res->err = LIST_OKAY;
    res->len = 0;
    res->allocated = init_len;

    // Initialize the mutex
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&res->mutex, &attr);

    // Ask for array memory and make sure it was allocated
    res->array = malloc(sizeof(void*) * res->allocated);
    if (res->array == NULL) {
        // No memory, destroy the structure
        free(res);
        res = NULL;
        return NULL;
    }

    return res;
}

/**
 * @inherit
 */
ListError_t arraylist_add(ArrayList_t* list, void* element) {
    // Lock the list (i.e. let any pending operation finish), run the add position, then unlock
    pthread_mutex_lock(&list->mutex);
    ListError_t result = arraylist_add_pos(list, list->len, element);
    pthread_mutex_unlock(&list->mutex);
    return result;
}

/**
 * @inherit
 */
ListError_t arraylist_add_pos(ArrayList_t* list, uint32_t pos, void* element) {
    // First, lock the list
    pthread_mutex_lock(&list->mutex);

    // Next, do a bounds check
    if (list->len < pos) {
        list->err = LIST_BOUNDS;
        pthread_mutex_unlock(&list->mutex);
        return LIST_BOUNDS;
    }

    // Next, check to see if we need to reallocate the array
    if ((list->len + 1) > list->allocated) {
        // Ask for array memory and make sure it was allocated
        void** temp = realloc(list->array, sizeof(void*) * (list->allocated + DEFAULT_LIST_STEP));
        if (temp == NULL) {
            list->err = LIST_MEMORY;
            pthread_mutex_unlock(&list->mutex);
            return LIST_MEMORY;
        }

        // Update the data structure with the new memory
        list->allocated += DEFAULT_LIST_STEP;
        list->array = temp;
    }

    // Next, check to see if we need to move anything before we insert the new value
    if (list->len > pos) {
        void* t1;
        void* t2 = list->array[pos];

        // Read the value from memory, save the previous value, loop
        for (size_t i = pos; i < list->len - 1; i += 1) {
            t1 = t2;
            t2 = list->array[i + 1];
            list->array[i + 1] = t1;
        }
        list->array[list->len] = t2;
    }

    // Finally, insert the value
    list->array[pos] = element;
    list->len += 1;

    // Return the lock and return the error code
    ListError_t tmp = list->err;
    pthread_mutex_unlock(&list->mutex);
    return tmp;
}

/**
 * @inherit
 */
void* arraylist_get(ArrayList_t* list, uint32_t pos) {
    // First, lock the list
    pthread_mutex_lock(&list->mutex);

    // Next, do a bounds check
    if (list->len == 0 || (list->len - 1) < pos) {
        list->err = LIST_BOUNDS;
        pthread_mutex_unlock(&list->mutex);
        return NULL;
    }

    // Get value and unlock list
    void* e = list->array[pos];
    pthread_mutex_unlock(&list->mutex);
    return e;
}

/**
 * @inherit
 */
void* arraylist_remove(ArrayList_t* list, uint32_t pos) {
    // First, lock the list
    pthread_mutex_lock(&list->mutex);

    // Next, do a bounds check
    if (list->len == 0 || (list->len - 1) < pos) {
        list->err = LIST_BOUNDS;
        pthread_mutex_unlock(&list->mutex);
        return NULL;
    }

    // Save the removed object
    void* res = list->array[pos];

    // Manually shift everything back a position
    for (size_t i = pos; i < list->len - 1; i += 1) {
        list->array[i] = list->array[i + 1];
    }
    list->array[list->len - 1] = 0; // clear the last position
    list->len -= 1;

    /*
     * todo: decrease the allocated memory if it gets too big
     */

    // Unlock the list
    pthread_mutex_unlock(&list->mutex);
    return res;
}

/**
 * @inherit
 */
uint32_t arraylist_size(ArrayList_t* list) {
    // Lock the list (i.e. let any pending operation finish), get the size, then unlock
    pthread_mutex_lock(&list->mutex);
    uint32_t len = list->len;
    pthread_mutex_unlock(&list->mutex);
    return len;
}

/**
 * @inherit
 */
void arraylist_free(ArrayList_t* list) {
    free(list->array);
    list->array = NULL;
    pthread_mutex_destroy(&list->mutex);
    free(list);
    list = NULL;
}
