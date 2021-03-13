//
// Created by Alex Kneipp on 2/15/20.
//
#include "messagehandler.h"
#include "hashtable.h"
#include "queue.h"
#include "packethandlers.h"

//Global static array so we can iterate over every type of packet easily
static PacketType_t PTYPES[NUM_PACKET_TYPES] = {
    pt_INIT,
    pt_STATE_REQUEST,
    pt_STATE_RESPONSE,
    pt_STATE_UPDATE,
    pt_CONFIG_REQUEST,
    pt_CONFIG_RESPONSE,
    pt_CONFIG_UPDATE,
    pt_USER_DATA,
    pt_UPDATE_STATUS,
    pt_DEBUG
};

// Key type = PacketType_t, Value type = Queue_t* <PTLVData_Base*>
static HashTable_t* buckets = NULL;

static uint32_t hash_PacketType(void* pt)
{
    return (uint32_t)((PacketType_t)(pt));
}

static int equals_PacketType(void* pt1, void* pt2)
{
    return (pt1 == pt2);
}


int mh_init()
{
    if(buckets != NULL)
    {
        return MH_ERR_ALREADY_INITIALIZED;
    }
    //Initialize the buckets hashtable
    //Use 2 times NUM_PACKET_TYPES as this is the smallest integer multiplication to ensure the table doesn't rehash
    buckets = hashtable_initSize(hash_PacketType, equals_PacketType, NULL, 2 * NUM_PACKET_TYPES);
    //Buckets failed to initialize, this error is non-recoverable.  Return err.
    if(buckets == NULL)
    {
        return MH_ERR_INITIALIZATION_FAILED;
    }
    // Assume we'll succeed from this point
    int rval = MH_SUCCESS;
    // Create a bucket for each packet type
    for(int i = 0; i < NUM_PACKET_TYPES; i++)
    {
        //Create the queue for this packet type
        Queue_t* bucketQueue = queue_init();
        //Queue creation succeeded
        if (bucketQueue != NULL)
        {
            //hashtable insertion failed, but this is potentially recoverable, return a warning but continue
            if(hashtable_put(buckets, (void*)(PTYPES[i]), bucketQueue) == 0)
            {
                rval = MH_WRN_INITIALIZATION_INCOMPLETE;
            }
        }
        //Queue creation failed, but this is potentially recoverable, return a warning but continue
        else
        {
            rval = MH_WRN_INITIALIZATION_INCOMPLETE;
        }
    }
    //Warning or success
    return rval;
}

void sort_packet(IntermediateTLV_t* packet)
{
    //TODO Get packet from ll interface,
    //Pass IntermediateTLV_t* to the correct unpacker,
    //Get the unpacked packet and put it in the correct bucket.
    PacketTLV_t* packetToSort;
    switch(packet->type)
    {
        case pt_INIT:
            packetToSort = unpackInit(packet);
            break;
        case pt_STATE_REQUEST:
            packetToSort = unpackStateRequest(packet);
            break;
        case pt_STATE_RESPONSE:
            packetToSort = unpackStateResponse(packet);
            break;
        case pt_STATE_UPDATE:
            packetToSort = unpackStateUpdate(packet);
            break;
        case pt_CONFIG_REQUEST:
            packetToSort = unpackConfigRequest(packet);
            break;
        case pt_CONFIG_RESPONSE:
            packetToSort = unpackConfigResponse(packet);
            break;
        case pt_CONFIG_UPDATE:
            packetToSort = unpackConfigUpdate(packet);
            break;
        case pt_USER_DATA:
            packetToSort = unpackUserData(packet);
            break;
        case pt_UPDATE_STATUS:
            //TODO log a message here that we're not handling updatestatus messages
            //TODO or handle UpdateStatus messages
            break;
        case pt_DEBUG:
            packetToSort = unpackDebug(packet);
            break;
    }
    queue_enqueue((Queue_t*)hashtable_get(buckets, (void*)((PacketType_t)packetToSort->type)), packetToSort);
}
