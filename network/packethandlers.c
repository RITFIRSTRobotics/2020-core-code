//
// Created by Alex Kneipp on 2/15/20.
//

#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "packethandlers.h"
#include "../collections/arraylist.h"

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

static List_t* parseKVList(char* rawList, size_t kvListLen)
{
    //TODO make sure we quit gracefully in the case of padding
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
        //TODO check to make sure keyLen > 0
        size_t keyLen = strlen(kvString);
        //Copy the key into memory we control
        //TODO check for failure
        char* key = malloc(sizeof(char) * (keyLen + 1));
        //TODO you can probably save time here by using strcpy since we're dynamically allocating memory according to
        //string size
        strncpy(key, kvString, keyLen+1);
        //The type is the first byte after the nul terminator
        KVPair_Type_t type = kvString[keyLen + 1];
        //The length is in the next 3 bytes
        size_t valueLength = kvString[keyLen+2] << 16 | kvString[keyLen+3] << 8 | kvString[keyLen+4];
        //Copy the value into memory we control
        //TODO check for failure
        void* value = malloc(valueLength);
        memcpy(value, kvString + keyLen + 5, valueLength);
        //Store the K-TLV structure
        //TODO check for failure
        KVPairTLV_t* thisPair = malloc(sizeof(KVPairTLV_t));
        thisPair->key = key;
        thisPair->type = type;
        thisPair->length = valueLength;
        thisPair->value = value;
        //Add the structure to the list
        arraylist_add(kvList, thisPair);
        //Increment the pointer to the next K-TLV
        //the length of the key + nul terminator + type + length + value + semicolon
        kvString +=keyLen+1+1+3+valueLength+1;
    }
    return (List_t*)kvList;
}

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
    unpacked->keys = getStringsFromArbitraryData(rawPacket->data, rawPacket->length);

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
    unpacked->pairs = getStringsFromArbitraryData(rawPacket->data, rawPacket->length);

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
    packet->data = unpacked;

    //Note that this may be NULL if there's a failure
    unpacked->new_pairs = parseKVList(rawPacket->data, rawPacket->length);

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

    packet->type = rawPacket->type;
    packet->timestamp = rawPacket->timestamp;
    packet->length = rawPacket->length;
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

}

void destroyInit(PacketTLV_t* initPacket)
{
    free(initPacket->data);
    initPacket->data = NULL;
    free(initPacket);
}
void destroyStateRequest(PacketTLV_t* stateRequestPacket)
{
    free(stateRequestPacket->data);
    stateRequestPacket->data = NULL;
    free(stateRequestPacket);
}
void destroyStateResponse(PacketTLV_t* stateResponsePacket)
{
    PTLVData_STATE_RESPONSE_t* data = (PTLVData_STATE_RESPONSE_t*)stateResponsePacket->data;
    free(data->arbitrary);
    data->arbitrary = NULL;
    free(data);
    data = NULL;
    free(stateResponsePacket);
}
void destroyStateUpdate(PacketTLV_t* stateUpdatePacket)
{
    PTLVData_STATE_UPDATE_t* data = (PTLVData_STATE_UPDATE_t*)stateUpdatePacket->data;
    free(data->arbitrary);
    data->arbitrary = NULL;
    free(data);
    data = NULL;
    free(stateUpdatePacket);
}
void destroyConfigRequest(PacketTLV_t* configRequestPacket)
{
    PTLVData_CONFIG_REQUEST_t* data = (PTLVData_CONFIG_REQUEST_t*)configRequestPacket->data;
    list_free(data->keys);
    data->keys = NULL;
    free(data);
    data = NULL;
    free(configRequestPacket);
}
void destroyConfigResponse(PacketTLV_t* configResponsePacket)
{
    PTLVData_CONFIG_RESPONSE_t* data = (PTLVData_CONFIG_RESPONSE_t*)configResponsePacket->data;
    list_free(data->pairs);
    data->pairs = NULL;
    free(data);
    data = NULL;
    free(configResponsePacket);
}
void destroyConfigUpdate(PacketTLV_t* configUpdatePacket)
{
    PTLVData_CONFIG_UPDATE_t* data = (PTLVData_CONFIG_UPDATE_t*)configUpdatePacket->data;
    list_free(data->new_pairs);
    data->new_pairs = NULL;
    free(data);
    data = NULL;
    free(configUpdatePacket);
}
void destroyUserData(PacketTLV_t* userDataPacket)
{
    free(userDataPacket->data);
    userDataPacket->data = NULL;
    free(userDataPacket);
}
void destroyDebug(PacketTLV_t* debugPacket)
{
    PTLVDATA_DEBUG_t* debugData = (PTLVDATA_DEBUG_t*)(debugPacket->data);
    free(debugData->arbitrary);
    debugData->arbitrary = NULL;
    free(debugData);
    debugData = NULL;
    free(debugPacket);
}
