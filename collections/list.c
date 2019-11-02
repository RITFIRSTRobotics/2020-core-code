/**
 * src/collections/list.c
 *
 * A generic list wrapper, can be instantiated as a linked or array list
 *
 * @author Connor Henley, @thatging3rkid
 */
#include <stdint.h>
#include <stdlib.h>

#ifdef _LOCAL_HEADER
    #include "list.h"
    #include "arraylist.h"
    #include "linkedlist.h"
#else
    #include "core/collections/list.h"
    #include "core/collections/arraylist.h"
    #include "core/collections/linkedlist.h"
#endif

/**
 * @inherit
 */
List_t* list_init(ListImplementation_t impl) {
    if (impl == LIST_ARRAY) {
        return (List_t*) arraylist_init();
    } else if (impl == LIST_LINKED) {
        return (List_t*) linkedlist_init();
    } else {
        return NULL;
    }
}

/**
 * @inherit
 */
ListError_t list_add(List_t* list, void* element) {
    if (list->impl == LIST_ARRAY) {
        return arraylist_add((ArrayList_t*) list, element);
    } else if (list->impl == LIST_LINKED) {
        return linkedlist_add_back((LinkedList_t*) list, element);
    } else {
        list->err = LIST_INVALID_IMPL;
        return LIST_INVALID_IMPL;
    }
}

/**
 * @inherit
 */
ListError_t list_add_pos(List_t* list, uint32_t pos, void* element) {
    if (list->impl == LIST_ARRAY) {
        return arraylist_add_pos((ArrayList_t*) list, pos, element);
    } else if (list->impl == LIST_LINKED) {
        return linkedlist_add_pos((LinkedList_t*) list, pos, element);
    } else {
        list->err = LIST_INVALID_IMPL;
        return LIST_INVALID_IMPL;
    }
}

/**
 * @inherit
 */
void* list_get(List_t* list, uint32_t pos) {
    if (list->impl == LIST_ARRAY) {
        return arraylist_get((ArrayList_t*) list, pos);
    } else if (list->impl == LIST_LINKED) {
        return linkedlist_get((LinkedList_t*) list, pos);
    } else {
        list->err = LIST_INVALID_IMPL;
        return NULL;
    }
}

/**
 * @inherit
 */
void* list_remove(List_t* list, uint32_t pos) {
    if (list->impl == LIST_ARRAY) {
        return arraylist_remove((ArrayList_t*) list, pos);
    } else if (list->impl == LIST_LINKED) {
        return linkedlist_remove_pos((LinkedList_t*) list, pos);
    } else {
        list->err = LIST_INVALID_IMPL;
        return NULL;
    }
}

/**
 * @inherit
 */
uint32_t list_size(List_t* list) {
    if (list->impl == LIST_ARRAY) {
        return arraylist_size((ArrayList_t*) list);
    } else if (list->impl == LIST_LINKED) {
        return linkedlist_size((LinkedList_t*) list);
    } else {
        list->err = LIST_INVALID_IMPL;
        return 0;
    }
}

/**
 * @inherit
 */
void list_free(List_t* list) {
    if (list->impl == LIST_ARRAY) {
        arraylist_free((ArrayList_t*) list);
    } else if (list->impl == LIST_LINKED) {
        linkedlist_free((LinkedList_t*) list);
    } else {
        // generic list handler
        free(list);
        list = NULL;
    }
}
