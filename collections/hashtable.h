//
// Created by Alex Kneipp on 1/25/20.
//

#ifndef INC_2020_CORE_CODE_HASHTABLE_H
#define INC_2020_CORE_CODE_HASHTABLE_H

typedef struct HashTable * HashTable_t;

HashTable_t hashtable_init();
HashTable_t hashtable_initSize(size_t size);

int hashtable_put(HashTable_t ht, void* key, void* value);
int hashtable_putIfAbsent(HashTable_t ht, void* key, void* value);

void* hashtable_get(HashTable_t ht, void* key);

int hashtable_remove(HashTable_t ht, void* key);
int hashtable_removeIfValue(HashTable_t ht, void* key, void* value);

int hashtable_replace(HashTable_t ht, void* key, void* newValue);
int hashtable_replaceIfValue(HashTable_t ht, void* key, void* oldValue, void* newValue);

int hashtable_containsKey(HashTable_t ht, void* key);
int hashtable_containsValue(HashTable_t ht, void* value);

size_t hashtable_size(HashTable_t ht);
int hashtable_isEmpty(HashTable_t ht);

#endif //INC_2020_CORE_CODE_HASHTABLE_H
