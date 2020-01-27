//
// Created by Alex Kneipp (@ahkneipp) on 1/26/20.
//

#ifndef _LOCAL_HEADER
    #error "This code should not be run outside of testing!\n"
#endif

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
    return (uint32_t)(uint);
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
            printf("%d:%d\n",(int)ht_result, (int)(i));
        }
    }
    hashtable_destroy(ht);
    return result;
}

int t06_putGetWithDefault()
{
    int result = TEST_SUCCESS;
    void* testVals[MAX_TEST_SIZE];
    HashTable_t ht = hashtable_initSize(hash, key_equals, value_equals, MAX_TEST_SIZE * 2);
    for(unsigned long i = 0; i < MAX_TEST_SIZE; i++)
    {
        testVals[i] =(void*) i;
        hashtable_put(ht, testVals[i], testVals[i]);
    }
    //NULL (=0) is not allowed as a key
    for(unsigned long i = 1; i<MAX_TEST_SIZE; i++)
    {
        void* ht_result = hashtable_getWithDefault(ht, (void*)i, (void*)i+1);
        if(ht_result!= (void*)i)
        {
            result += TEST_FAILURE;
            printf("%d:%d\n",(int)ht_result, (int)(i));
        }
    }
    void* defaultGet = hashtable_getWithDefault(ht, (void*) MAX_TEST_SIZE + 1, NULL);
    if(defaultGet != NULL)
    {
        result += TEST_FAILURE;
    }
    //Test with another default value so we're sure it isn't always returning null
    defaultGet = hashtable_getWithDefault(ht, (void*) MAX_TEST_SIZE + 1, (void*)1);
    if(defaultGet != (void*)1)
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
    if(putResult = hashtable_put(ht, NULL, NULL) != 0)
    {
        printf("putResult: %d\n", putResult);
        result += TEST_FAILURE;
    }
    hashtable_destroy(ht);
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

    // Tests finished, handle the error code
    if (allErrors == 0) {
        return EXIT_SUCCESS;
    } else {
        return EXIT_FAILURE;
    }
}

