/**
 * Networking utility function definitions.
 * @author Alex Kneipp, @ahkneipp
 */

#include "netutils.h"
#include <string.h>
#include <malloc.h>

/// Create a new KVPairTLV_t*.
KVPairTLV_t* KVPairTLV_create(char* key, KVPair_Type_t type, void* value)
{
    //Copy the key provided into the internal field
    KVPairTLV_t* kvPair = (KVPairTLV_t*)malloc(sizeof(KVPairTLV_t));
    kvPair->key = (char*) malloc(strlen(key) + 1);
    strcpy(kvPair->key, key);
    //Copy the type
    kvPair->type = type;
    //This is the default, if the type is a CString, then the value will be updated in the switch
    kvPair->value = value;
    //Ensure the length and value is correct
    switch(type)
    {
        case kv_CString:
            //metadata size + data size + nul terminator
            kvPair->length = 4 + strlen((char*)value) + 1;
            kvPair->value = malloc(strlen((char*)value) + 1);
            strcpy(kvPair->value, (char*)value);
            break;
        case kv_Double:
            //metadata size + 2 words
            kvPair->length = 12;
            break;
        case kv_Boolean:
            //metadata size + 1 byte
            kvPair->length = 5;
            break;
        //Floats and integers have the same length
        case kv_Float:
        case kv_Integer:
            //metadata size + 1 word
            kvPair->length = 8;
            break;
    }
    return kvPair;
}

///Free the memory used by a KVPairTLV_t*
void KVPairTLV_destroy(KVPairTLV_t* pairToDestroy)
{
    free(pairToDestroy->key);
    //Only kv_CString types allocate additional memory for the value.
    if(pairToDestroy->type == kv_CString)
    {
        free(pairToDestroy->value);
    }
    free(pairToDestroy);
}
