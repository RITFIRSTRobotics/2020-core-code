/**
 * core/collections/arraylist.c
 *
 * An array list implementation
 *
 * @author Connor Henley, @thatging3rkid
 */
#include <stdint.h>
#include <stdlib.h>
#include <memory.h>

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
    return arraylist_add_pos(list, list->len, element);
}

/**
 * @inherit
 */
ListError_t arraylist_add_pos(ArrayList_t* list, uint32_t pos, void* element) {
    // First, do a bounds check
    if (list->len < pos) {
        list->err = LIST_BOUNDS;
        return list->err;
    }

    // Next, check to see if we need to reallocate the array
    if ((list->len + 1) > list->allocated) {
        // Ask for array memory and make sure it was allocated
        void** temp = realloc(list->array, sizeof(void*) * (list->allocated + DEFAULT_LIST_STEP));
        if (temp == NULL) {
            list->err = LIST_MEMORY;
            return list->err;
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

    return list->err;
}

/**
 * @inherit
 */
void* arraylist_get(ArrayList_t* list, uint32_t pos) {
    // First, do a bounds check
    if (list->len == 0 || (list->len - 1) < pos) {
        list->err = LIST_BOUNDS;
        return NULL;
    }

    return list->array[pos];
}

/**
 * @inherit
 */
void* arraylist_remove(ArrayList_t* list, uint32_t pos) {
    // First, do a bounds check
    if (list->len == 0 || (list->len - 1) < pos) {
        list->err = LIST_BOUNDS;
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
    return res;
}

/**
 * @inherit
 */
uint32_t arraylist_size(ArrayList_t* list) {
    return list->len;
}

/**
 * @inherit
 */
void arraylist_free(ArrayList_t* list) {
    free(list->array);
    list->array = NULL;
    free(list);
    list = NULL;
}
