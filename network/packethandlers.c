//
// Created by Alex Kneipp on 2/15/20.
//

#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "packethandlers.h"
#include "../collections/arraylist.h"

/**
 * Creates a PacketTLV_t* from a ll packet.
 * The returned packet contains the type, length, and timestamp from the LL packet.
 * @param rawPacket
 *  An IntermediateTLV_t* packet from the low-level network interface
 * @return
 *  A PacketTLV_t* with the type, length, and timestamp set, and the data field set to null.
 *  This function returns NULL if the memory allocation for the return packet fails.
 */
static PacketTLV_t* createBasePacket(IntermediateTLV_t* rawPacket)
{
    //Allocate memory for the packet and check for success
    PacketTLV_t* packet = malloc(sizeof(PacketTLV_t));
    if(packet == NULL)
    {
        return NULL;
    }

    //Copy the data from the intermediate packet into the proper packet
    packet->type = rawPacket->type;
    packet->length = rawPacket->length;
    packet->timestamp = rawPacket->timestamp;
    //Zero out the data field for safety's sake
    packet->data = NULL;
    return packet;
}

/**
 * Parses a key-value list as defined by the network protocol definition into a List of KV pairs.
 * @param rawList
 *  A byte array which encodes a list of KV pairs.
 * @param kvListLen
 *  The length of the rawList character array.
 * @return
 *  A List* of KVPairTLV_t* or NULL if there was an error
 */
static List_t* parseKVList(char* rawList, size_t kvListLen)
{
    //TODO consider doing format validation and setting ERRNO appropriately
    ArrayList_t* kvList = arraylist_init();
    if(kvList == NULL)
    {
        return NULL;
    }
    char* kvString = rawList;
    //While we haven't hit the end of the list
    while(kvString < (rawList + kvListLen))
    {
        //Get the length of the key
        size_t keyLen = strlen(kvString);
        //We've found a nul byte at the beginning of a string for some reason, probably because of padding
        //Check the next byte until we hit the end of the raw buffer
        if(keyLen == 0)
        {
            kvString++;
            continue;
        }
        //Copy the key into memory we control
        char* key = malloc(sizeof(char) * (keyLen + 1));
        if(key != NULL)
        {
            //memcpy for speed, since we already know string size
            memcpy(key, kvString, keyLen+1);
            //Force nul termination
            key[keyLen] = '\0';
        }
        //The type is the first byte after the nul terminator
        KVPair_Type_t type = kvString[keyLen + 1];
        //The length is in the next 3 bytes
        size_t valueLength = kvString[keyLen+2] << 16 | kvString[keyLen+3] << 8 | kvString[keyLen+4];
        //Copy the value into memory we control
        void* value = malloc(valueLength);
        if(value != NULL)
        {
            memcpy(value, kvString + keyLen + 5, valueLength);
        }
        //Store the K-TLV structure
        KVPairTLV_t* thisPair = malloc(sizeof(KVPairTLV_t));
        if(thisPair != NULL && value != NULL && key != NULL)
        {
            thisPair->key = key;
            thisPair->type = type;
            thisPair->length = valueLength;
            thisPair->value = value;
            //Add the structure to the list
            arraylist_add(kvList, thisPair);
        }
        //There was a failure somewhere
        else
        {
            //Clean up the memory we have successfully allocated
            if(thisPair)
                free(thisPair);
            if(value)
                free(value);
            if(key)
                free(key);
        }
        //Increment the pointer to the next K-TLV
        //the length of the key + nul terminator + type + length + value + semicolon
        kvString +=keyLen+1+1+3+valueLength+1;
    }
    return (List_t*)kvList;
}

/**
 * Grabs several nul-terminated strings out of an array of bytes.
 * Sequential nul bytes will be interpreted as an empty string.
 * @param rawList
 *  The raw list of nul-terminated strings.
 * @param rawLen
 *  The length of the byte array.
 * @return
 *  A List_t* of strings found in \p rawList.
 */
static List_t* getStringsFromArbitraryData(char* rawList, size_t rawLen)
{
    ArrayList_t* strings = arraylist_init();
    char* currentString = rawList;
    while(currentString < rawList + rawLen)
    {
        size_t len = strlen(currentString);
        //String length plus nul terminator
        char* str = malloc(sizeof(char) * (len + 1));
        strcpy(str, currentString);
        arraylist_add(strings, str);
        //String length plus nul terminator
        currentString += len + 1;
    }
    return (List_t*) strings;
}

PacketTLV_t* unpackInit(IntermediateTLV_t* rawPacket)
{
    PacketTLV_t* packet = createBasePacket(rawPacket);
    if (packet == NULL)
    {
        //Free the raw packet
        llnet_packet_free(rawPacket);
        return NULL;
    }

    PTLVData_INIT_t* unpacked = malloc(sizeof(PTLVData_INIT_t));
    //Allocation failed
    if(unpacked == NULL)
    {
        //packet allocation succeeded, so free that memory
        free(packet);
        //Free the raw packet
        llnet_packet_free(rawPacket);
        return NULL;
    }

    //Unpack the UUID from the raw data
    unpacked->robot_uuid = ((uint32_t*)(rawPacket->data))[0];
    //Dump it in the packet
    packet->data = (PTLVData_Base_t*)unpacked;
    //We own the packet memory, free it
    llnet_packet_free(rawPacket);
    return packet;
}

PacketTLV_t* unpackStateRequest(IntermediateTLV_t* rawPacket)
{
    PacketTLV_t* packet = createBasePacket(rawPacket);
    if (packet == NULL)
    {
        //Free the raw packet
        llnet_packet_free(rawPacket);
        return NULL;
    }

    //An empty struct has 0 size, so don't bother allocating memory for the data portion
    packet->data = NULL;
    //We own the packet memory, free it
    llnet_packet_free(rawPacket);
    return packet;
}

PacketTLV_t* unpackStateResponse(IntermediateTLV_t* rawPacket)
{
    PacketTLV_t* packet = createBasePacket(rawPacket);
    if (packet == NULL)
    {
        //Free the raw packet
        llnet_packet_free(rawPacket);
        return NULL;
    }

    errno = 0;
    PTLVData_STATE_RESPONSE_t* unpacked = calloc(1,sizeof(PTLVData_STATE_RESPONSE_t));
    //Allocation failed
    if(unpacked == NULL && errno != 0)
    {
        //packet allocation succeeded, so free that memory
        free(packet);
        //Free the raw packet
        llnet_packet_free(rawPacket);
        return NULL;
    }
    //Byte one is the state
    unpacked->state = (RobotState_t)rawPacket->data[0];
    //The reserved isn't currently used, but we can use it in the future, so unpack it
    unpacked->reserved = (uint32_t)(rawPacket->data[1]) << 16 |
            (uint32_t)(rawPacket->data[2]) << 8 | rawPacket->data[3];
    //Total packet size - defined data size
    size_t arbitrarySize = rawPacket->length - 4;
    void* arbitraryData = NULL;
    //There's arbitrary data here!
    if(arbitrarySize > 0)
    {
        //Allocate memory for the arbitrary data and copy it into the new buffer.
        //Note, if allocation fails we just dump the data
        arbitraryData = calloc(arbitrarySize, 1);
        if(arbitraryData != NULL)
        {
            memcpy(arbitraryData, rawPacket->data + 4, arbitrarySize);
        }
    }
    unpacked->arbitrary = arbitraryData;
    //Dump it in the packet
    packet->data = (PTLVData_Base_t*)unpacked;
    //We own the packet memory, free it
    llnet_packet_free(rawPacket);
    return packet;
}

PacketTLV_t* unpackStateUpdate(IntermediateTLV_t* rawPacket)
{
    PacketTLV_t* packet = createBasePacket(rawPacket);
    if (packet == NULL)
    {
        //Free the raw packet
        llnet_packet_free(rawPacket);
        return NULL;
    }

    errno = 0;
    PTLVData_STATE_UPDATE_t* unpacked = calloc(1,sizeof(PTLVData_STATE_UPDATE_t));
    //Allocation failed
    if(unpacked == NULL && errno != 0)
    {
        //packet allocation succeeded, so free that memory
        free(packet);
        //Free the raw packet
        llnet_packet_free(rawPacket);
        return NULL;
    }
    //Byte one is the state
    unpacked->new_state = (RobotState_t)rawPacket->data[0];
    //The reserved isn't currently used, but we can use it in the future, so unpack it
    unpacked->reserved = (uint32_t)(rawPacket->data[1]) << 16 |
                         (uint32_t)(rawPacket->data[2]) << 8 | rawPacket->data[3];
    //Total packet size - defined data size
    size_t arbitrarySize = rawPacket->length - 4;
    void* arbitraryData = NULL;
    //There's arbitrary data here!
    if(arbitrarySize > 0)
    {
        //Allocate memory for the arbitrary data and copy it into the new buffer.
        //Note, if allocation fails we just dump the data
        arbitraryData = calloc(arbitrarySize, 1);
        if(arbitraryData != NULL)
        {
            memcpy(arbitraryData, rawPacket->data + 4, arbitrarySize);
        }
    }
    unpacked->arbitrary = arbitraryData;
    //Dump it in the packet
    packet->data = (PTLVData_Base_t*)unpacked;
    //We own the packet memory, free it
    llnet_packet_free(rawPacket);
    return packet;
}

PacketTLV_t* unpackConfigRequest(IntermediateTLV_t* rawPacket)
{
    PacketTLV_t* packet = createBasePacket(rawPacket);
    if (packet == NULL)
    {
        //Free the raw packet
        llnet_packet_free(rawPacket);
        return NULL;
    }

    errno = 0;
    PTLVData_CONFIG_REQUEST_t* unpacked = calloc(1,sizeof(PTLVData_CONFIG_REQUEST_t));
    //Allocation failed
    if(unpacked == NULL && errno != 0)
    {
        //packet allocation succeeded, so free that memory
        free(packet);
        //Free the raw packet
        llnet_packet_free(rawPacket);
        return NULL;
    }
    packet->type = rawPacket->type;
    packet->timestamp = rawPacket->timestamp;
    packet->length = rawPacket->length;
    packet->data = (PTLVData_Base_t*)unpacked;
    unpacked->keys = getStringsFromArbitraryData((char*)rawPacket->data, rawPacket->length);

    llnet_packet_free(rawPacket);
    return packet;
}

PacketTLV_t* unpackConfigResponse(IntermediateTLV_t* rawPacket)
{
    PacketTLV_t* packet = createBasePacket(rawPacket);
    if (packet == NULL)
    {
        //Free the raw packet
        llnet_packet_free(rawPacket);
        return NULL;
    }

    errno = 0;
    PTLVData_CONFIG_RESPONSE_t* unpacked = calloc(1,sizeof(PTLVData_CONFIG_RESPONSE_t));
    //Allocation failed
    if(unpacked == NULL && errno != 0)
    {
        //packet allocation succeeded, so free that memory
        free(packet);
        //Free the raw packet
        llnet_packet_free(rawPacket);
        return NULL;
    }
    packet->type = rawPacket->type;
    packet->timestamp = rawPacket->timestamp;
    packet->length = rawPacket->length;
    packet->data = (PTLVData_Base_t*)unpacked;
    unpacked->pairs = getStringsFromArbitraryData((char*)rawPacket->data, rawPacket->length);

    llnet_packet_free(rawPacket);
    return packet;
}

PacketTLV_t* unpackConfigUpdate(IntermediateTLV_t* rawPacket)
{
    PacketTLV_t* packet = createBasePacket(rawPacket);
    if (packet == NULL)
    {
        //Free the raw packet
        llnet_packet_free(rawPacket);
        return NULL;
    }

    errno = 0;
    PTLVData_CONFIG_UPDATE_t* unpacked = calloc(1,sizeof(PTLVData_CONFIG_UPDATE_t));
    //Allocation failed
    if(unpacked == NULL && errno != 0)
    {
        //packet allocation succeeded, so free that memory
        free(packet);
        //Free the raw packet
        llnet_packet_free(rawPacket);
        return NULL;
    }
    packet->length = rawPacket->length;
    packet->timestamp = rawPacket->timestamp;
    packet->type = rawPacket->type;
    packet->data = (PTLVData_Base_t*)unpacked;

    //Note that this may be NULL if there's a failure
    unpacked->new_pairs = parseKVList((char*)rawPacket->data, rawPacket->length);

    llnet_packet_free(rawPacket);
    return packet;
}

PacketTLV_t* unpackUserData(IntermediateTLV_t* rawPacket)
{
    PacketTLV_t* packet = createBasePacket(rawPacket);
    if (packet == NULL)
    {
        //Free the raw packet
        llnet_packet_free(rawPacket);
        return NULL;
    }

    errno = 0;
    PTLVData_USER_DATA_t* unpacked = calloc(1,sizeof(PTLVData_USER_DATA_t));
    //Allocation failed
    if(unpacked == NULL && errno != 0)
    {
        //packet allocation succeeded, so free that memory
        free(packet);
        //Free the raw packet
        llnet_packet_free(rawPacket);
        return NULL;
    }

    packet->data = (PTLVData_Base_t*)unpacked;

    unpacked->left_stick_x = rawPacket->data[0];
    unpacked->left_stick_y = rawPacket->data[1];
    unpacked->right_stick_x = rawPacket->data[2];
    unpacked->right_stick_y = rawPacket->data[3];
    unpacked->button_a = (rawPacket->data[4] & 0x80) != 0;
    unpacked->button_b = (rawPacket->data[4] & 0x40) != 0;
    unpacked->controller_uuid = (((uint16_t)(rawPacket->data[6])) << 8) | rawPacket->data[7];

    llnet_packet_free(rawPacket);
    return packet;
}

PacketTLV_t* unpackDebug(IntermediateTLV_t* rawPacket)
{
    PacketTLV_t* packet = createBasePacket(rawPacket);
    if (packet == NULL)
    {
        //Free the raw packet
        llnet_packet_free(rawPacket);
        return NULL;
    }

    errno = 0;
    PTLVData_DEBUG_t* unpacked = calloc(1,sizeof(PTLVData_DEBUG_t));
    //Allocation failed
    if(unpacked == NULL && errno != 0)
    {
        //packet allocation succeeded, so free that memory
        free(packet);
        //Free the raw packet
        llnet_packet_free(rawPacket);
        return NULL;
    }

    packet->data = (PTLVData_Base_t*)unpacked;

    unpacked->code_status = rawPacket->data[0];
    unpacked->commit_hash = rawPacket->data[1] << 16| rawPacket->data[2] << 8 | rawPacket->data[3];
    unpacked->robot_uuid = ((uint32_t*)(rawPacket->data))[1];
    unpacked->state = (RobotState_t)(rawPacket->data[8]);
    unpacked->config_entries = ((uint16_t*)(rawPacket->data))[5];
    unpacked->arbitrary = (void*)(rawPacket->data + 12);

    llnet_packet_free(rawPacket);
    return packet;
}

void destroyInit(PacketTLV_t* initPacket)
{
    if(initPacket->type != pt_INIT)
    {
        //TODO log error message like other checks
    }
    free(initPacket->data);
    initPacket->data = NULL;
    free(initPacket);
}
void destroyStateRequest(PacketTLV_t* stateRequestPacket)
{
    if(stateRequestPacket->type != pt_STATE_REQUEST)
    {
        //TODO log error message like other checks
    }
    free(stateRequestPacket->data);
    stateRequestPacket->data = NULL;
    free(stateRequestPacket);
}
void destroyStateResponse(PacketTLV_t* stateResponsePacket)
{
    if(stateResponsePacket->type == pt_STATE_RESPONSE)
    {
        PTLVData_STATE_RESPONSE_t* data = (PTLVData_STATE_RESPONSE_t*)stateResponsePacket->data;
        free(data->arbitrary);
        data->arbitrary = NULL;
        //We're guaranteed to free this later
        data = NULL;
    }
    else
    {
        //Incorrect packet type or corrupted packet, refusing to free possibly non-existent packet-specific data.
        //This may cause data leaks!
        //TODO log error message
        //TODO log packet type and what type the programmer wants us to free
    }
    //Free the packet structure shared by all packets
    free(stateResponsePacket->data);
    stateResponsePacket->data = NULL;
    free(stateResponsePacket);
}
void destroyStateUpdate(PacketTLV_t* stateUpdatePacket)
{
    if(stateUpdatePacket->type == pt_STATE_UPDATE)
    {
        PTLVData_STATE_UPDATE_t* data = (PTLVData_STATE_UPDATE_t*)stateUpdatePacket->data;
        free(data->arbitrary);
        data->arbitrary = NULL;
        //We're guaranteed to free this later
        data = NULL;
    }
    else
    {
        //Incorrect packet type or corrupted packet, refusing to free possibly non-existent packet-specific data.
        //This may cause data leaks!
        //TODO log error message
        //TODO log packet type and what type the programmer wants us to free
    }
    free(stateUpdatePacket->data);
    stateUpdatePacket->data = NULL;
    free(stateUpdatePacket);
}
void destroyConfigRequest(PacketTLV_t* configRequestPacket)
{
    if(configRequestPacket->type == pt_CONFIG_REQUEST)
    {
        PTLVData_CONFIG_REQUEST_t* data = (PTLVData_CONFIG_REQUEST_t*)configRequestPacket->data;
        list_free(data->keys);
        data->keys = NULL;
        //We're guaranteed to free this later
        data = NULL;
    }
    else
    {
        //Incorrect packet type or corrupted packet, refusing to free possibly non-existent packet-specific data.
        //This may cause data leaks!
        //TODO log error message
        //TODO log packet type and what type the programmer wants us to free
    }
    free(configRequestPacket->data);
    configRequestPacket->data = NULL;
    free(configRequestPacket);
}
void destroyConfigResponse(PacketTLV_t* configResponsePacket)
{
    if(configResponsePacket->type == pt_CONFIG_RESPONSE)
    {
        PTLVData_CONFIG_RESPONSE_t* data = (PTLVData_CONFIG_RESPONSE_t*)configResponsePacket->data;
        list_free(data->pairs);
        data->pairs = NULL;
        //We're guaranteed to free this later
        data = NULL;
    }
    else
    {
        //Incorrect packet type or corrupted packet, refusing to free possibly non-existent packet-specific data.
        //This may cause data leaks!
        //TODO log error message
        //TODO log packet type and what type the programmer wants us to free
    }
    free(configResponsePacket->data);
    configResponsePacket->data = NULL;
    free(configResponsePacket);
}
void destroyConfigUpdate(PacketTLV_t* configUpdatePacket)
{
    if(configUpdatePacket->type == pt_CONFIG_UPDATE)
    {
        PTLVData_CONFIG_UPDATE_t* data = (PTLVData_CONFIG_UPDATE_t*)configUpdatePacket->data;
        list_free(data->new_pairs);
        data->new_pairs = NULL;
        data = NULL;
        //We're guaranteed to free this later
    }
    else
    {
        //Incorrect packet type or corrupted packet, refusing to free possibly non-existent packet-specific data.
        //This may cause data leaks!
        //TODO log error message
        //TODO log packet type and what type the programmer wants us to free
    }
    free(configUpdatePacket->data);
    configUpdatePacket->data = NULL;
    free(configUpdatePacket);
}
void destroyUserData(PacketTLV_t* userDataPacket)
{
    if(userDataPacket->type != pt_USER_DATA)
    {
        //TODO log error message like other checks
    }
    free(userDataPacket->data);
    userDataPacket->data = NULL;
    free(userDataPacket);
}
void destroyDebug(PacketTLV_t* debugPacket)
{
    if(debugPacket->type == pt_CONFIG_UPDATE)
    {
        PTLVData_DEBUG_t* debugData = (PTLVData_DEBUG_t*)(debugPacket->data);
        free(debugData->arbitrary);
        debugData->arbitrary = NULL;
        //We're guaranteed to free this later.
        debugData = NULL;
    }
    else
    {
        //Incorrect packet type or corrupted packet, refusing to free possibly non-existent packet-specific data.
        //This may cause data leaks!
        //TODO log error message
        //TODO log packet type and what type the programmer wants us to free
    }
    free(debugPacket->data);
    debugPacket->data = NULL;
    free(debugPacket);
}
