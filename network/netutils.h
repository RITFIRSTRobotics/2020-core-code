/**
 * core/network/netutils.h
 *
 * Utility functions used in networking
 *
 * @author Connor Henley, @thatging3rkid
 * @author Alex Kneipp, @ahkneipp
 */
#ifndef __CORE_NETWORK_UTILS
#define __CORE_NETWORK_UTILS

#include "packet.h"

// allow C++ to parse this
#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/**
 * Create a new KVPairTLV.
 * @param key
 *  The key in the KV pair. This can be safely free'd after this function returns
 * @param type
 *  The type of Value the KV pair holds
 * @param length
 *  The length of the TLV structure in bytes.
 * @param value
 *  The value to store in the kvPair. If \p type is kv_CString, this may be free'd after this function returns.
 * @return
 *  A pointer to a new KVPairTLV_t, with \p key as its key, \p type as its type,
 *  \p length as its length, and \p value as its value.
 */
KVPairTLV_t* KVPairTLV_create(const char* key, KVPair_Type_t type, size_t length, KVPair_Value_u value);

/**
 * Create a new KVPairTLV from a memory address containing a KV-TLV as defined in the protocol definition
 * @param start
 *  A pointer to the start of a KVPairTLV in a buffer.
 * @param end
 *  A pointer to the byte after the last byte of the KVPairTLV pointed to by start.
 *  This will be set to NULL if the most recent call did not contain a KVPairTLV structure.
 * @return
 *  A pointer to a new KVPairTLV_t with values extracted from memory.
 */
KVPairTLV_t* KVPairTLV_createFromMemory(const char* start, char** end);

/**
 * Check if two KVPairTLV's are equal.
 * @param kv1
 *  The first KVPairTLV to compare.
 * @param kv2
 *  The second KVPairTLV to compare.
 * @return
 *  1 if kv1 == kv2, 0 otherwise
 */
int KVPairTLV_equals(KVPairTLV_t* kv1, KVPairTLV_t* kv2);

/**
 * Destroy a KVPairTLV*.  If the type is kv_CString, the value field will be free'd.
 * @param pairToDestroy
 *  The KV pair to free the memory of.
 */
void KVPairTLV_destroy(KVPairTLV_t* pairToDestroy);
    
#ifdef __cplusplus
}
#endif //__cplusplus

#endif //__CORE_NETWORK_UTILS
