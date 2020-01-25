//
// Created by Alex Kneipp on 1/25/20.
//
#include <stdlib.h>
#include <stdint.h>

struct HashTable{
    void** keys;
    void** values;
    size_t size;
    size_t mem_size;
    uint32_t (*hash)(void*);
    int (*key_equals)(void*, void*);
    int (*value_equals)(void*, void*);
    float fill_ratio;
};

#include "hashtable.h"


