//
// Created by Alex Kneipp on 1/25/20.
//
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "arraylist.h"

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

/**
 * Finds the location of a key in the hashtable
 * @param ht
 *  The hashtable to search
 * @param key
 *  The key to find
 * @return
 *  The index in \p ht's keys array that holds \p key, or -1 if \p key was not found
 */
static int hashtable_findKeyLocation(HashTable_t ht, void* key)
{
    //Hash the key
    uint32_t loc = ht->hash(key);
    uint32_t initialLoc = loc;
    do
    {
        if(ht->keys[loc] == NULL)
        {
            return -1;
        }

        //If this is the key, return the location
        if(ht->key_equals(key, ht->keys[loc]))
        {
            return loc;
        }
        //Otherwise check the next one
        loc++;
        loc %= ht->mem_size;
        //While we haven't checked the entire list
    }while(loc != initialLoc);
    //We never found it
    return -1;
}

/**
 * Inserts a value into the hashtable, assuming there exists open space for the value to be inserted
 * @param ht
 *  The hashtable to insert into
 * @param key
 *  The key to map
 * @param value
 *  The value to map
 * @return
 *  1 if successfully inserted, 0 if the key already exists in the table.
 */
static int hashtable_insert(HashTable_t ht, void* key, void* value)
{
    //Hash the key
    uint32_t loc = ht->hash(key);
    //while the place we want to go is filled, increment, looping around to the start
    while(ht->keys[loc] != NULL)
    {
        if(ht->key_equals(key, ht->keys[loc]))
        {
            return 0;
        }
        loc++;
        loc %= ht->mem_size;
    }
    //Put the key and value in the first free spaces we find.
    ht->keys[loc] = key;
    ht->values[loc] = value;
    ht->size++;
    return 1;
}

/**
 * Rehashes the hashtable
 * @param ht
 *  The table to rehash
 * @return
 *  0 on failure, 1 otherwise
 */
static int hashtable_rehash(HashTable_t ht)
{
    //Store the old keys, values, and memory size to pull from later.
    void** oldKeys = ht->keys;
    void** oldValues = ht->values;
    size_t oldMemSize = ht->mem_size;
    //Calculate a new memory size guaranteed to be large enough
    size_t newMemSize = ht->size * (ht->fill_ratio + 1);
    //Allocate new memory and set it to 0
    ht->keys = (void**)(malloc(sizeof(void*) * newMemSize));
    memset(ht->keys, 0x00, sizeof(void*) * newMemSize);
    ht->values = (void**)(malloc(sizeof(void*) * newMemSize));
    memset(ht->values, 0x00, sizeof(void*) * newMemSize);
    //TODO check for failure.
    ht->mem_size = newMemSize;
    //For all of the old memory
    for(size_t i = 0; i < oldMemSize ; i++)
    {
        //Didn't hold a key, skip it
        if(oldKeys[i] == NULL)
        {
            continue;
        }
        //Don't need to check the return because we're starting from a known good hashtable
        hashtable_insert(ht, oldKeys[i], oldValues[i]);
    }
    //Clean up the old memory
    free(oldKeys);
    oldKeys = NULL;
    free(oldValues);
    oldValues = NULL;
    return 1;
}

///Create a new hash table with no initial size
HashTable_t hashtable_init(uint32_t (*hash)(void*), int (*key_equals)(void*, void*), int (*value_equals)(void*, void*))
{
    HashTable_t ht = (HashTable_t)(malloc(sizeof(struct HashTable)));
    //If memory allocation fails, return null because everything has gone wrong
    if(ht == NULL)
    {
        return NULL;
    }
    ht->keys = NULL;
    ht->values = NULL;
    ht->size = 0;
    ht->mem_size = 0;
    ht->hash = hash;
    ht->key_equals = key_equals;
    ht->value_equals = value_equals;
    //Default value of fill_ratio is .7 because that's what I've read before.
    ht->fill_ratio = 0.7f;
    return ht;
}

///Create a new hash-table with an initial size.
HashTable_t hashtable_initSize(uint32_t (*hash)(void*), int (*key_equals)(void*, void*), int (*value_equals)(void*, void*), size_t size)
{
    //Get an empty hash-table
    HashTable_t ht = hashtable_init(hash, key_equals, value_equals);
    ht->keys = (void**)(malloc(sizeof(void*) * size));
    //If memory allocation fails, return null because everything has gone wrong
    if(ht->keys == NULL)
    {
        free(ht);
        return NULL;
    }
    ht->values = (void**)(malloc(sizeof(void*) * size));
    //If memory allocation fails, return null because everything has gone wrong
    if(ht->values == NULL)
    {
        free(ht);
        return NULL;
    }
    ht->mem_size = size;
    return ht;
}

///Insert a new key-value pair into the HashTable, checking sizes and rehashing if necessary
int hashtable_put(HashTable_t ht, void* key, void* value)
{
    //The memory for keys and values has not yet been initialized, initialize them now.
    if(ht->mem_size == 0)
    {
        //Allocate memory so we obey the fill ratio, rounded up
        size_t newSize = (size_t)(1 + ht->fill_ratio + 1);
        ht->keys = (void**)(malloc(sizeof(void*) * newSize));
        //If memory allocation fails, return zero because everything has gone wrong
        if(ht->keys == NULL)
        {
            return 0;
        }
        ht->values = (void**)(malloc(sizeof(void*) * newSize));
        if(ht->values == NULL)
        {
            return 0;
        }
        ht->mem_size = newSize;
    }

    //If we're going to overrun our fill_ratio value, rehash
    if(ht->size + 1 > (size_t)(ht->mem_size * ht->fill_ratio))
    {
        hashtable_rehash(ht);
    }
    return hashtable_insert(ht, key, value);
}

///Get a value out of the hashtable based on the provided key.
void* hashtable_get(HashTable_t ht, void* key)
{
    //Find the key if it exists
    int keyLocation = hashtable_findKeyLocation(ht, key);
    //Return null if it doesn't
    if(keyLocation == -1)
    {
        return NULL;
    }
    //Return the value
    return ht->values[keyLocation];
}

///Gets a value out of the ht, or returns a default value if we couldn't find it.
void* hashtable_getWithDefault(HashTable_t ht, void* key, void* defaultValue)
{
    //Try to find the value as usual
    void* actualValue = hashtable_get(ht, key);
    //We couldn't find it, return the default
    if(actualValue == NULL)
    {
        return defaultValue;
    }
    //We found it! return it.
    return actualValue;
}

/**
 * Removes a key-value pair from the hashtable.
 * @param ht
 *  The hashable object to operate on.
 * @param key
 *  The key of the k-v pair to remove
 * @return
 *  The removed value if the key was found and removed, NULL otherwise.
 */
void* hashtable_remove(HashTable_t ht, void* key)
{
    int keyLoc = hashtable_findKeyLocation(ht, key);
    //If we found an empty spot before we found the correct key, we don't have the key they're looking for, return null
    if(keyLoc == -1)
    {
        return NULL;
    }
    void* val = ht->values[keyLoc];
    ht->keys[keyLoc] = NULL;
    ht->values[keyLoc] = NULL;
    ht->size--;
    return val;
}

///Remove a k-v pair from the table if the value matches the provided value
void* hashtable_removeIfValue(HashTable_t ht, void* key, void* value)
{
    int keyLoc = hashtable_findKeyLocation(ht, key);
    //If we found an empty spot before we found the correct key, we don't have the key they're looking for.
    //Or if the value doesn't match the provided value, return null
    if(keyLoc == -1 || !ht->value_equals(ht->values[keyLoc], value))
    {
        return NULL;
    }
    //Save the value to be returned later
    void* val = ht->values[keyLoc];
    //Remove the value and key from the table and decrement the size
    ht->keys[keyLoc] = NULL;
    ht->values[keyLoc] = NULL;
    ht->size--;
    return val;
}

/// Replaces the value mapped to a key in the hash table
void* hashtable_replace(HashTable_t ht, void* key, void* newValue)
{
    int keyLoc = hashtable_findKeyLocation(ht, key);
    if(keyLoc == -1)
    {
        return NULL;
    }
    void* val = ht->values[keyLoc];
    ht->values[keyLoc] = newValue;
    return val;
}
/// Replaces the value mapped to a key in the hashtable if both the key and value match the provided values.
void* hashtable_replaceIfValue(HashTable_t ht, void* key, void* oldValue, void* newValue)
{
    int keyLoc = hashtable_findKeyLocation(ht, key);
    if(keyLoc == -1 || !ht->value_equals(ht->values[keyLoc], oldValue))
    {
        return NULL;
    }
    void* val = ht->values[keyLoc];
    ht->values[keyLoc] = newValue;
    return val;
}

///Returns non-zero if the provided key is mapped in the hashtable
int hashtable_containsKey(HashTable_t ht, void* key)
{
    return hashtable_findKeyLocation(ht, key) != -1;
}

///Returns non-zero if the provided value is mapped in the hashtable
int hashtable_containsValue(HashTable_t ht, void* value)
{
    for(size_t i = 0; i < ht->mem_size; i++)
    {
        if(ht->value_equals(ht->values[i], value))
        {
            return 1;
        }
    }
    return 0;
}

/// Checks the number of key-value mappings in the table.  DOES NOT return the amount of slots available in the table.
size_t hashtable_size(HashTable_t ht)
{
    return ht->size;
}

/// Checks if there are no key-value mappings in the table.
int hashtable_isEmpty(HashTable_t ht)
{
    return ht->size == 0;
}

/// Returns a list of all of the keys currently mapped in the hashtable
List_t* hashtable_getKeys(HashTable_t ht)
{
    ArrayList_t* keys = arraylist_init_len(ht->size);
    for(size_t i = 0; i < ht->mem_size; i++)
    {
        if(ht->keys[i] != NULL)
        {
            arraylist_add(keys, ht->keys[i]);
        }
    }
    return (List_t*)keys;
}

/// Frees all the memory used by the hashtable. DOES NOT call free on any keys or values in the table.
void hashtable_destroy(HashTable_t ht)
{
    free(ht->keys);
    ht->keys = NULL;
    free(ht->values);
    ht->values = NULL;
    free(ht);
    ht = NULL;
}
