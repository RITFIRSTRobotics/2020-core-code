/**
 * core/collections/list.h
 *
 * An abstract list implementation
 *
 * @author Connor Henley, @thatging3rkid
 */
#include <stdint.h>
#include <stdlib.h>
#include <memory.h>

// do some preprocessor voodoo
#ifdef _LOCAL_HEADER
    #include "list.h"
#else
    #include "core/collections/list.h"
#endif

/**
 * @inherit
 */
List_t* list_init() {
    return list_init_len(DEFAULT_LIST_LENGTH);
}

/**
 * @inherit
 */
List_t* list_init_len(uint32_t init_len) {
    // Ask for memory and make sure the it was allocated
    List_t* res = malloc(sizeof(List_t));
    if (res == NULL) {
        return NULL;
    }

    // Start filling in the struct
    res->len = 0;
    res->raw_len = init_len;
    res->err = LIST_OKAY;

    // Ask for array memory and make sure it was allocated
    res->array = malloc(sizeof(void*) * res->raw_len);
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
ListError_t list_add(List_t* list, void* element) {
    return list_add_pos(list, list->len, element);
}

/**
 * @inherit
 */
ListError_t list_add_pos(List_t* list, uint32_t pos, void* element) {
    // First, do a bounds check
    if ((list->len + 1) < pos) {
        list->err = LIST_BOUNDS;
        return list->err;
    }

    // Next, check to see if we need to reallocate the array
    if ((list->len + 1) > list->raw_len) {
        // Ask for array memory and make sure it was allocated
        void** temp = realloc(list->array, sizeof(void*) * (list->raw_len + DEFAULT_LIST_STEP));
        if (temp == NULL) {
            list->err = LIST_MEMORY;
            return list->err;
        }

        // Update the data structure with the new memory
        list->raw_len += DEFAULT_LIST_STEP;
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
void* list_get(List_t* list, uint32_t pos) {
    // First, do a bounds check
    if ((list->len + 1) < pos) {
        list->err = LIST_BOUNDS;
        return NULL;
    }

    return list->array[pos];
}

/**
 * @inherit
 */
void* list_remove(List_t* list, uint32_t pos) {
    // First, do a bounds check
    if ((list->len + 1) < pos) {
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
uint32_t list_size(List_t* list) {
    return list->len;
}

/**
 * @inherit
 */
void list_free(List_t* list) {
    free(list->array);
    list->array = NULL;
    free(list);
    list = NULL;
}
