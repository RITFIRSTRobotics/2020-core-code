/**
 * Networking utility function definitions.
 * @author Alex Kneipp, @ahkneipp
 */

#include "netutils.h"
#include <string.h>
#include <malloc.h>

/// Create a new KVPairTLV_t*.
KVPairTLV_t* KVPairTLV_create(const char* key, KVPair_Type_t type, size_t length, KVPair_Value_u value)
{
    //Copy the key provided into the internal field
    KVPairTLV_t* kvPair = (KVPairTLV_t*)malloc(sizeof(KVPairTLV_t));
    if(kvPair == NULL)
    {
        //TODO log message
        return NULL;
    }
    kvPair->key = (char*) malloc(strlen(key) + 1);
    strcpy(kvPair->key, key);
    //Copy the type
    kvPair->type = type;
    kvPair->length = length;
    //This is the default, if the type is a CString, then the value will be updated in the switch
    kvPair->value = value;
    //We need to copy the value if it's a CString, since we don't own the memory the caller has handed us
    if(type == kv_CString)
    {
        kvPair->value.CString = (char*)malloc(strlen(value.CString) + 1);
        strcpy(kvPair->value.CString, value.CString);
    }
    return kvPair;
}

///Create a new KVPairTLV from a buffer in memory
KVPairTLV_t* KVPairTLV_createFromMemory(const char* start, char** end)
{
    const char* key = (const char*)start;
    size_t keyLen = strlen(key);
    if(keyLen == 0)
    {
        *end = NULL;
        return NULL;
    }
    //The start of the value is at the start of the buffer + length of the key + nul terminator.
    const char* TLVStart = (char*)start+keyLen+1;
    //The type is just the first byte of the buffer
    KVPair_Type_t type = (KVPair_Type_t)(*(TLVStart));
    //Next 3 bytes are length of full TLV, so subtract 4 for header
    size_t valueLength = (TLVStart[1] << 16 | TLVStart[2] << 8 | TLVStart[3]) - 4;
    KVPair_Value_u value;
    char* valStart = (char*)(TLVStart + 4);
    switch(type)
    {
        case kv_CString:
            value.CString = valStart;
            break;
        case kv_Integer:
            value.Integer = *((int32_t*)(valStart));
            break;
        case kv_Double:
            value.Double = *((double*)(valStart));
            break;
        case kv_Float:
            value.Float = *((float*)(valStart));
            break;
        case kv_Boolean:
            value.Boolean = *((int8_t*)(valStart));
            break;
    }
    *end = valStart+valueLength;
    return KVPairTLV_create(key, type, valueLength+4, value);
}

///Check if two KVPairTLV's are equal
bool KVPairTLV_equals(KVPairTLV_t* kv1, KVPairTLV_t* kv2)
{
    //If the metadata isn't equal, it doesn't make sense to check the values
    if(strcmp(kv1->key, kv2->key))
    {
        return false;
    }
    if(kv1->type != kv2->type)
    {
        return false;
    }
    if(kv1->length != kv2->length)
    {
        return false;
    }
    switch(kv1->type)
    {
        case kv_CString:
            return !strcmp(kv1->value.CString, kv2->value.CString);
            break;
        case kv_Integer:
            return kv1->value.Integer == kv2->value.Integer;
            break;
        case kv_Double:
            return kv1->value.Double == kv2->value.Double;
            break;
        case kv_Float:
            return kv1->value.Float == kv2->value.Float;
            break;
        case kv_Boolean:
            return kv1->value.Boolean == kv2->value.Boolean;
            break;
    }
    return true;
}

///Free the memory used by a KVPairTLV_t*
void KVPairTLV_destroy(KVPairTLV_t* pairToDestroy)
{
    free(pairToDestroy->key);
    //Only kv_CString types allocate additional memory for the value.
    if(pairToDestroy->type == kv_CString)
    {
        free(pairToDestroy->value.CString);
    }
    free(pairToDestroy);
}
