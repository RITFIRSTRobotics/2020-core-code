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
#ifndef __cplusplus
extern "C" {
#endif //__cplusplus
    /**
     * Create a new KVPairTLV.
     * @param key
     *  The key in the KV pair. This can be safely free'd after this function returns
     * @param type
     *  The type of Value the KV pair holds
     * @param value
     *  The value to store in the kvPair. If \p type is kv_CString, this may be free'd after this function returns.
     * @return
     *  A pointer to a new KVPairTLV_t, with \p key as its key, \p type as its type, and \p value as its value.
     *  The \p length field is calculated from \p type and \p value.
     */
    KVPairTLV_t* KVPairTLV_create(char* key, KVPair_Type_t type, void* value);

    /**
     * Destroy a KVPairTLV*.  If the type is kv_CString, the value field will be free'd.
     * @param pairToDestroy
     *  The KV pair to free the memory of.
     */
    void KVPairTLV_destroy(KVPairTLV_t* pairToDestroy);

#ifndef __cplusplus
}
#endif //__cplusplus

#endif //__CORE_NETWORK_UTILS