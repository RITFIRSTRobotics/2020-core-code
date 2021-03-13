//
// Created by Alex Kneipp on 1/25/20.
//

#ifndef INC_2020_CORE_CODE_HASHTABLE_H
#define INC_2020_CORE_CODE_HASHTABLE_H

#include "list.h"

// Hash table structure
typedef struct HashTable{
    void** keys;
    void** values;
    size_t size;
    size_t mem_size;
    uint32_t (*hash)(void*);
    int (*key_equals)(void*, void*);
    int (*value_equals)(void*, void*);
    float fill_ratio;
} HashTable_t;

/**
 * Creates a new hashtable with 0 initial size.  The first insert will result in a rehash.
 * @param hash
 *  The function used to hash the keys.
 * @param key_equals
 *  The function used to compare between two keys.  Must return non-zero if the keys are equal
 * @param value_equals
 *  The function used to compare between two values.  Must return non-zero if the keys are equal.  This function is only
 *  necessary if the *ifValue or containsValue functions are used; it may be null otherwise.
 * @return
 *  An empty hashtable.
 */
HashTable_t* hashtable_init(uint32_t (*hash)(void*), int (*key_equals)(void*, void*), int (*value_equals)(void*, void*));
/**
 * Creates a new hashtable with an initial size.
 * @param hash
 *  The function used to hash the keys.
 * @param key_equals
 *  The function used to compare between two keys.  Must return non-zero if the keys are equal
 * @param value_equals
 *  The function used to compare between two values.  Must return non-zero if the keys are equal
 * @param size
 *  The amount of entries to allocate space for initially.
 * @return
 *  An empty hashtable with space for \p size entries.
 */
HashTable_t* hashtable_initSize(uint32_t (*hash)(void*), int (*key_equals)(void*, void*), int (*value_equals)(void*, void*), size_t size);

/**
 * Put a new key-value pair in the hashtable.
 * @param ht
 *  The hashtable object to insert into.
 * @param key
 *  The key for this object.  Keys may not be null.
 * @param value
 *  The value to insert into the hash table under \p key
 * @return
 *  Non-zero if the key was successfully inserted.
 */
int hashtable_put(HashTable_t* ht, void* key, void* value);

/**
 * @param ht
 *  The hashable object to get from.
 * @param key
 *  The key which the requested value is mapped to.
 * @return
 *  The value mapped to \p key or NULL if the key is not present.
 */
void* hashtable_get(HashTable_t* ht, void* key);
/**
 * @param ht
 *  The hashable object to get from.
 * @param key
 *  The key which the requested value is mapped to.
 * @param defaultValue
 *  The value to return if \p key is not mapped.
 * @return
 *  The value mapped to \p key or \p defaultValue if the key is not present.
 */
void* hashtable_getWithDefault(HashTable_t* ht, void* key, void* defaultValue);

/**
 * Removes a key-value pair from the hashtable.
 * @param ht
 *  The hashable object to operate on.
 * @param key
 *  The key of the k-v pair to remove
 * @return
 *  The removed value if the key was found and removed, NULL otherwise.
 */
void* hashtable_remove(HashTable_t* ht, void* key);
/**
 * Removes a key-value pair from the hashtable if both the key and value match the provided values.
 * Requires that the hash table was initialized with a non-null value-equals function pointer.
 * @param ht
 *  The hashable object to operate on.
 * @param key
 *  The key of the k-v pair to remove
 * @param value
 *  The value to check.
 * @return
 *  The removed value if the key-value pair was found and removed, NULL otherwise.
 */
void* hashtable_removeIfValue(HashTable_t* ht, void* key, void* value);

/**
 * Replaces the value mapped to a key in the hash table
 * @param ht
 *  The hashable object to operate on.
 * @param key
 *  The key to replace the value mapping of.
 * @param newValue
 *  The value to replace the old value with.
 * @return
 *  The old value if the key was found and replaced, NULL otherwise.
 */
void* hashtable_replace(HashTable_t* ht, void* key, void* newValue);
/**
 * Replaces the value mapped to a key in the hashtable if both the key and value match the provided values.
 * Requires that the hash table was initialized with a non-null value-equals function pointer.
 * @param ht
 *  The hashable object to operate on.
 * @param key
 *  The key to replace the value mapping of.
 * @param oldValue
 *  The value to check.
 * @param newValue
 *  The value to replace the old value with.
 * @return
 *  The old value if the key was found and replaced, NULL otherwise.
 */
void* hashtable_replaceIfValue(HashTable_t* ht, void* key, void* oldValue, void* newValue);

/**
 * @param ht
 *  The hashable object to operate on.
 * @param key
 *  The key to check
 * @return
 *  Non-zero if the key is mapped in the hashtable, zero otherwise.
 */
int hashtable_containsKey(HashTable_t* ht, void* key);
/**
 * Check if the provided value is in the hashtable.
 * Requires that the hash table was initialized with a non-null value-equals function pointer.
 * @param ht
 *  The hashable object to operate on.
 * @param value
 *  The key to check
 * @return
 *  Non-zero if the value is mapped in the hashtable, zero otherwise.
 */
int hashtable_containsValue(HashTable_t* ht, void* value);

/**
 * Checks the number of key-value mappings in the table.  DOES NOT return the amount of slots available in the table.
 * @param ht
 *  The hashable object to operate on.
 * @return
 *  The number of key-value mappings in the table.
 */
size_t hashtable_size(HashTable_t* ht);
/**
 * Checks if there are no key-value mappings in the table.
 * @param ht
 *  The hashable object to operate on.
 * @return
 *  Non-zero if there are no key-value mappings in the table.
 */
int hashtable_isEmpty(HashTable_t* ht);

/**
 * @param ht
 *  The hashable object to operate on.
 * @return
 *  A list of all of the keys currently mapped in the hashtable
 */
List_t* hashtable_getKeys(HashTable_t* ht);

/**
 * Frees all the memory used by the hashtable. DOES NOT call free on any keys or values in the table.
 * @param ht
 *  The hashtable to destroy.
 */
void hashtable_destroy(HashTable_t* ht);

#endif //INC_2020_CORE_CODE_HASHTABLE_H
