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
    void* testVals[1024];
    HashTable_t ht = hashtable_init(hash, key_equals, value_equals);
    for(unsigned long i = 0; i < 1024; i++)
    {
        testVals[i] =(void*) i;
        hashtable_put(ht, testVals[i], testVals[i]);
    }
    for(unsigned long i = 0; i<1024; i++)
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
    void* testVals[1024];
    HashTable_t ht = hashtable_initSize(hash, key_equals, value_equals, 2048);
    for(unsigned long i = 0; i < 1024; i++)
    {
        testVals[i] =(void*) i;
        hashtable_put(ht, testVals[i], testVals[i]);
    }
    for(unsigned long i = 0; i<1024; i++)
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

int main() {
    // Run tests on both types of list
    int error = 0;
    printf("Starting Test01!\n");
    error += t01_initNoValueEquals();
    printf("Starting Test02!\n");
    error += t02_initValueEquals();
    printf("Starting Test03!\n");
    error += t03_initInitialSize();
    printf("Starting Test04!\n");
    error += t04_putGetNoInitialSize();
    printf("Starting Test05!\n");
    error += t05_putGetInitialSizeNoRehash();

    // Tests finished, handle the error code
    if (error == 0) {
        printf("success!\n");
        return EXIT_SUCCESS;
    } else {
        printf("^^^ test errors\n");
        return EXIT_FAILURE;
    }
}

