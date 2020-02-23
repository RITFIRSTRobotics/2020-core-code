//
// Created by Alex Kneipp (@ahkneipp) on 1/26/20.
//
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <memory.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include "../collections/hashtable.h"
#include "../collections/list.h"

#define TEST_SUCCESS 0
#define TEST_FAILURE 1
#define MAX_TEST_SIZE 1024

static uint32_t hash(void* uint)
{
    return (uint32_t)((uintptr_t) uint);
}

static int key_equals(void* a, void* b)
{
    return a == b;
}
static int value_equals(void* a, void* b)
{
    return a == b;
}

int t01_initNoValueEquals()
{
    HashTable_t ht = hashtable_init(hash, key_equals, NULL);
    int result = TEST_SUCCESS;
    if(!hashtable_isEmpty(ht))
    {
        result += TEST_FAILURE;
    }
    hashtable_destroy(ht);
    return result;
}

int t02_initValueEquals()
{
    HashTable_t ht = hashtable_init(hash, key_equals, value_equals);
    int result = TEST_SUCCESS;
    if(!hashtable_isEmpty(ht))
    {
        result += TEST_FAILURE;
    }
    hashtable_destroy(ht);
    return result;
}

int t03_initInitialSize()
{
    HashTable_t ht = hashtable_initSize(hash, key_equals, value_equals, 100);
    int result = TEST_SUCCESS;
    if(!hashtable_isEmpty(ht))
    {
        result += TEST_FAILURE;
    }
    if(hashtable_size(ht) != 0)
    {
        result += TEST_FAILURE;
    }
    hashtable_destroy(ht);
    return result;
}

int t04_putGetNoInitialSize()
{
    int result = TEST_SUCCESS;
    void* testVals[MAX_TEST_SIZE];
    HashTable_t ht = hashtable_init(hash, key_equals, value_equals);
    for(unsigned long i = 1; i < MAX_TEST_SIZE; i++)
    {
        testVals[i] =(void*) i;
        hashtable_put(ht, testVals[i], testVals[i]);
    }
    for(unsigned long i = 0; i<MAX_TEST_SIZE; i++)
    {
        if(hashtable_get(ht, (void*)i) != (void*)i)
        {
            result += TEST_FAILURE;
        }
    }
    hashtable_destroy(ht);
    return result;
}

int t05_putGetInitialSizeNoRehash()
{
    int result = TEST_SUCCESS;
    void* testVals[MAX_TEST_SIZE];
    HashTable_t ht = hashtable_initSize(hash, key_equals, value_equals, MAX_TEST_SIZE * 2);
    for(unsigned long i = 1; i < MAX_TEST_SIZE; i++)
    {
        testVals[i] =(void*) i;
        hashtable_put(ht, testVals[i], testVals[i]);
    }
    for(unsigned long i = 0; i<MAX_TEST_SIZE; i++)
    {
        void* ht_result = hashtable_get(ht, (void*)i);
        if(ht_result!= (void*)i)
        {
            result += TEST_FAILURE;
            printf("%d:%d\n",(int)(intptr_t)ht_result, (int)(i));
        }
    }
    hashtable_destroy(ht);
    return result;
}

int t06_putGetWithDefault()
{
    int result = TEST_SUCCESS;
    void* testvals[MAX_TEST_SIZE];
    HashTable_t ht = hashtable_initSize(hash, key_equals, value_equals, MAX_TEST_SIZE * 2);
    for(unsigned long i = 0; i < MAX_TEST_SIZE; i++)
    {
        testvals[i] =(void*) i;
        hashtable_put(ht, testvals[i], testvals[i]);
    }
    //NULL (=0) is not allowed as a key
    for(unsigned long i = 1; i<MAX_TEST_SIZE; i++)
    {
        void* ht_result = hashtable_getWithDefault(ht, (void*)i, (void*)i+1);
        if(ht_result!= (void*)i)
        {
            result += TEST_FAILURE;
            printf("%d:%d\n",(int)(intptr_t)ht_result, (int)(i));
        }
    }
    void* defaultget = hashtable_getWithDefault(ht, (void*) MAX_TEST_SIZE + 1, NULL);
    if(defaultget != NULL)
    {
        result += TEST_FAILURE;
    }
    //test with another default value so we're sure it isn't always returning NULL
    defaultget = hashtable_getWithDefault(ht, (void*) MAX_TEST_SIZE + 1, (void*)1);
    if(defaultget != (void*)1)
    {
        result += TEST_FAILURE;
    }
    hashtable_destroy(ht);
    return result;
}

int t07_hashtableReturnsFailureOnNullKeyInsert()
{
    int result = TEST_SUCCESS;
    HashTable_t ht = hashtable_initSize(hash, key_equals, value_equals, MAX_TEST_SIZE * 2);
    int putResult;
    if((putResult = hashtable_put(ht, NULL, NULL)) != 0)
    {
        printf("putResult: %d\n", putResult);
        result += TEST_FAILURE;
    }
    hashtable_destroy(ht);
    return result;
}

int t08_remove()
{
    int result = TEST_SUCCESS;
    void* testvals[MAX_TEST_SIZE];
    HashTable_t ht = hashtable_initSize(hash, key_equals, value_equals, MAX_TEST_SIZE * 2);
    for(unsigned long i = 0; i < MAX_TEST_SIZE; i++)
    {
        testvals[i] =(void*) i;
        hashtable_put(ht, testvals[i], testvals[i]);
    }
    //NULL (=0) is not allowed as a key
    for(unsigned long i = 1; i<MAX_TEST_SIZE; i++)
    {
        void* ht_result = hashtable_remove(ht, (void*)i);
        if(ht_result!= (void*)i)
        {
            result += TEST_FAILURE;
            printf("%d:%d\n",(int)(intptr_t)ht_result, (int)(i));
        }
        ht_result = hashtable_get(ht, (void*)i);
        if(ht_result != NULL)
        {
            result += TEST_FAILURE;
            printf("Failed to remove key!");
        }
    }
    hashtable_destroy(ht);
    return result;
}

int t09_removeIfValue()
{
    int result = TEST_SUCCESS;
    void* testvals[MAX_TEST_SIZE];
    HashTable_t ht = hashtable_initSize(hash, key_equals, value_equals, MAX_TEST_SIZE * 2);
    for(unsigned long i = 0; i < MAX_TEST_SIZE; i++)
    {
        testvals[i] =(void*) i;
        hashtable_put(ht, testvals[i], testvals[i]);
    }
    //NULL (=0) is not allowed as a key
    for(unsigned long i = 1; i<MAX_TEST_SIZE; i++)
    {
        void* ht_result = hashtable_removeIfValue(ht, (void*)i, (void *)(uintptr_t)(i+1));
        if(ht_result!= NULL)
        {
            result += TEST_FAILURE;
        }
        ht_result = hashtable_get(ht, (void*)i);
        if(ht_result != (void*)i)
        {
            result += TEST_FAILURE;
            printf("Unexpectedly removed key!");
        }
    }
    for(unsigned long i = 1; i<MAX_TEST_SIZE; i++)
    {
        void* ht_result = hashtable_removeIfValue(ht, (void*)i, (void *)(uintptr_t)i);
        if(ht_result!= (void*)i)
        {
            result += TEST_FAILURE;
            printf("%d:%d\n",(int)(intptr_t)ht_result, (int)(i));
        }
        ht_result = hashtable_get(ht, (void*)i);
        if(ht_result != NULL)
        {
            result += TEST_FAILURE;
            printf("Failed to remove key!");
        }
    }
    hashtable_destroy(ht);
    return result;
}

int t10_replace()
{
    int result = TEST_SUCCESS;
    void* testvals[MAX_TEST_SIZE];
    HashTable_t ht = hashtable_initSize(hash, key_equals, value_equals, MAX_TEST_SIZE * 2);
    for(unsigned long i = 0; i < MAX_TEST_SIZE; i++)
    {
        testvals[i] =(void*) i;
        hashtable_put(ht, testvals[i], testvals[i]);
    }
    //NULL (=0) is not allowed as a key
    for(unsigned long i = 1; i<MAX_TEST_SIZE; i++)
    {
        void* ht_result = hashtable_replace(ht, (void*)i, (void *)(uintptr_t)(i+1));
        if(ht_result != (void *)(uintptr_t)i)
        {
            result += TEST_FAILURE;
        }
        ht_result = hashtable_get(ht, (void*)i);
        if(ht_result != (void*)(i + 1))
        {
            result += TEST_FAILURE;
            printf("Failed to replace key!");
        }
    }
    hashtable_destroy(ht);
    return result;
}

int t11_replaceIfValue()
{
    int result = TEST_SUCCESS;
    void* testvals[MAX_TEST_SIZE];
    HashTable_t ht = hashtable_initSize(hash, key_equals, value_equals, MAX_TEST_SIZE * 2);
    for(unsigned long i = 0; i < MAX_TEST_SIZE; i++)
    {
        testvals[i] =(void*) i;
        hashtable_put(ht, testvals[i], testvals[i]);
    }
    //NULL (=0) is not allowed as a key
    for(unsigned long i = 1; i<MAX_TEST_SIZE; i++)
    {
        void* ht_result = hashtable_replaceIfValue(ht, (void*)i, (void *)(uintptr_t)(i+1), (void *)(uintptr_t)(i+2));
        if(ht_result!= NULL)
        {
            result += TEST_FAILURE;
        }
        ht_result = hashtable_get(ht, (void*)i);
        if(ht_result != (void*)i)
        {
            result += TEST_FAILURE;
            printf("Unexpectedly replaced key!");
        }
    }
    for(unsigned long i = 1; i<MAX_TEST_SIZE; i++)
    {
        void* ht_result = hashtable_replaceIfValue(ht, (void*)i, (void *)(uintptr_t)i, (void *)(uintptr_t)(i+1));
        if(ht_result!= (void*)(i))
        {
            result += TEST_FAILURE;
            printf("%d:%d\n",(int)(intptr_t)ht_result, (int)(intptr_t)(i));
        }
        ht_result = hashtable_get(ht, (void*)i);
        if(ht_result != (void*)(i+1))
        {
            result += TEST_FAILURE;
            printf("Failed to remove key!");
        }
    }
    hashtable_destroy(ht);
    return result;
}

int t12_checkContainsKey()
{
    int result = TEST_SUCCESS;
    HashTable_t ht = hashtable_initSize(hash, key_equals, value_equals, MAX_TEST_SIZE * 2);
    //Insert odd keys into the hashtable
    for(unsigned long i = 1; i < MAX_TEST_SIZE * 2; i+=2)
    {
        hashtable_put(ht, (void*)i, (void*)i);
    }
    for(unsigned long i = 1; i < MAX_TEST_SIZE * 2; i++)
    {
        //If we're missing an odd key according to containsKey, fail
        if(i % 2 == 1)
        {
            if(!hashtable_containsKey(ht, (void*)i))
            {
                printf("Missing %lu\n", i);
                result += TEST_FAILURE;
            }
        }
        //If we have an even key according to containsKey, fail
        else
        {
            if(hashtable_containsKey(ht, (void*)i))
            {
                printf("Has %lu unexpectedly\n", i);
                result += TEST_FAILURE;
            }
        }
    }
    hashtable_destroy(ht);
    return result;
}

int t13_testContainsValue()
{
    int result = TEST_SUCCESS;
    HashTable_t ht = hashtable_initSize(hash, key_equals, value_equals, MAX_TEST_SIZE * 2);
    //Insert odd keys into the hashtable
    for(unsigned long i = 1; i < MAX_TEST_SIZE * 2; i+=2)
    {
        hashtable_put(ht, (void*)i, (void*)i);
    }
    for(unsigned long i = 1; i < MAX_TEST_SIZE * 2; i++)
    {
        //If we're missing an odd key according to containsKey, fail
        if(i % 2 == 1)
        {
            if(!hashtable_containsValue(ht, (void*)i))
            {
                printf("Missing %lu\n", i);
                result += TEST_FAILURE;
            }
        }
            //If we have an even key according to containsKey, fail
        else
        {
            if(hashtable_containsValue(ht, (void*)i))
            {
                printf("Has %lu unexpectedly\n", i);
                result += TEST_FAILURE;
            }
        }
    }
    hashtable_destroy(ht);
    return result;
}

int t14_checkKeysList()
{
    int result = TEST_SUCCESS;
    void* testvals[MAX_TEST_SIZE - 1];
    HashTable_t ht = hashtable_initSize(hash, key_equals, value_equals, MAX_TEST_SIZE * 2);
    for(unsigned long i = 1; i < MAX_TEST_SIZE; i++)
    {
        testvals[i - 1] =(void*) i;
        hashtable_put(ht, (void *)(uintptr_t)i, (void *)(uintptr_t)i);
    }
    List_t* keys = hashtable_getKeys(ht);
    if(keys == NULL)
    {
        printf("Error when creating list!\n");
        return TEST_FAILURE;
    }
    //Check to make sure the size of the returned list is correct
    if(list_size(keys) != MAX_TEST_SIZE - 1)
    {
        printf("Incorrect keysList size\n");
        result += TEST_FAILURE;
    }

    void* listKey = NULL;
    int failFlag;
    for(unsigned long i = 1; i < MAX_TEST_SIZE; i++)
    {
        listKey = list_get(keys, i-1);
        for(unsigned long j = 0; j < MAX_TEST_SIZE; j++)
        {
            failFlag = 1;
            if(listKey == testvals[j])
            {
                failFlag = 0;
            }
        }
        if(!failFlag)
        {
            result += TEST_FAILURE;
            printf("Missing Key %lu\n", (unsigned long)listKey);
        }
    }
    hashtable_destroy(ht);
    list_free(keys);
    return result;
}

int main() {
    // Run tests on both types of list
    int error = 0;
    int allErrors = 0;
    printf("Starting Test01!\n");
    error = t01_initNoValueEquals();
    if(error == 0 )
    {
        printf("success!\n");
    }
    else
    {
        printf("^^^ test errors\n");
    }
    allErrors += error;

    printf("Starting Test02!\n");
    error = t02_initValueEquals();
    if(error == 0 )
    {
        printf("success!\n");
    }
    else
    {
        printf("^^^ test errors\n");
    }
    allErrors += error;

    printf("Starting Test03!\n");
    error = t03_initInitialSize();
    if(error == 0 )
    {
        printf("success!\n");
    }
    else
    {
        printf("^^^ test errors\n");
    }
    allErrors += error;

    printf("Starting Test04!\n");
    error = t04_putGetNoInitialSize();
    if(error == 0 )
    {
        printf("success!\n");
    }
    else
    {
        printf("^^^ test errors\n");
    }
    allErrors += error;

    printf("Starting Test05!\n");
    error = t05_putGetInitialSizeNoRehash();
    if(error == 0 )
    {
        printf("success!\n");
    }
    else
    {
        printf("^^^ test errors\n");
    }
    allErrors += error;

    printf("Starting Test06!\n");
    error = t06_putGetWithDefault();
    if(error == 0 )
    {
        printf("success!\n");
    }
    else
    {
        printf("^^^ test errors\n");
    }
    allErrors += error;

    printf("Starting Test07!\n");
    error = t07_hashtableReturnsFailureOnNullKeyInsert();
    if(error == 0 )
    {
        printf("success!\n");
    }
    else
    {
        printf("^^^ test errors\n");
    }
    allErrors += error;

    printf("Starting Test08!\n");
    error = t08_remove();
    if(error == 0 )
    {
        printf("success!\n");
    }
    else
    {
        printf("^^^ test errors\n");
    }
    allErrors += error;

    printf("Starting Test09!\n");
    error = t09_removeIfValue();
    if(error == 0 )
    {
        printf("success!\n");
    }
    else
    {
        printf("^^^ test errors\n");
    }
    allErrors += error;

    printf("Starting Test10!\n");
    error = t10_replace();
    if(error == 0)
    {
        printf("success!\n");
    }
    else
    {
        printf("^^^ test errors\n");
    }
    allErrors += error;

    printf("Starting Test11!\n");
    error = t11_replaceIfValue();
    if(error == 0)
    {
        printf("success!\n");
    }
    else
    {
        printf("^^^ test errors\n");
    }
    allErrors += error;

    printf("Starting Test12!\n");
    error = t12_checkContainsKey();
    if(error == 0)
    {
        printf("success!\n");
    }
    else
    {
        printf("^^^ test errors\n");
    }
    allErrors += error;

    printf("Starting Test13!\n");
    error = t13_testContainsValue();
    if(error == 0)
    {
        printf("success!\n");
    }
    else
    {
        printf("^^^ test errors\n");
    }
    allErrors += error;

    printf("Starting Test14!\n");
    error = t14_checkKeysList();
    if(error == 0)
    {
        printf("success!\n");
    }
    else
    {
        printf("^^^ test errors\n");
    }
    allErrors += error;

    // Tests finished, handle the error code
    if (allErrors == 0) {
        return EXIT_SUCCESS;
    } else {
        return EXIT_FAILURE;
    }
}

