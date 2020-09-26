//
// Created by Alex Kneipp on 2/29/20.
//

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "../network/packethandlers.h"
#include "../network/packet.h"
#include "../network/lowlevel.h"
#include "../utils/dbgprint.h"
#include "../collections/arraylist.h"
#include "../collections/list.h"
#include "../utils/bounds.h"
#include "../network/netutils.h"

static const uint8_t INIT_DATA[] = {
        0x55, 0xAA, 0x55, 0xAA
};

static PTLVData_INIT_t knownGoodInit={
       0xAA55AA55
};

static IntermediateTLV_t* initPacket = NULL;

static const uint8_t USER_DATA_DATA[] = {
        //Left Joy X, left joy y, right joy x, right joy y
        0x55,         0xAA,       0xCC,        0x33,
        //Buttons,    Unused,     ID byte 1,   ID byte 0
        0x80,         0x00,       0xFF,        0x00,
};

static PTLVData_USER_DATA_t knownGoodUserData = {
        0x55, 0xAA, 0xCC, 0x33, 1, 0, 0xFF00
};

static IntermediateTLV_t* userDataPacket = NULL;

//State request is empty except for header, no need for known-good or data array
static IntermediateTLV_t* stateRequestPacket = NULL;

static const uint8_t STATE_RESPONSE_DATA[] = {
    0xFF, 0x01, 0x02, 0x03,
    0x00, 0x01, 0x02, 0x03,
    0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0A, 0x0B,
    0x0C, 0x0D, 0x0E, 0x0F
};

static PTLVData_STATE_RESPONSE_t knownGoodStateResponse = {
        0xFF, 0x010203, NULL
};

IntermediateTLV_t* stateResponsePacket = NULL;

static const uint8_t STATE_UPDATE_DATA[] = {
        //new code, reserved
        0x02, 0xFF, 0xAA, 0x55,
        //Arbitrary data
        0x00, 0x01, 0x02, 0x03,
        0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B,
        0x0C, 0x0D, 0x0E, 0x0F
};

static PTLVData_STATE_UPDATE_t knownGoodStateUpdate = {
        0x02, 0xFFAA55, NULL
};

static IntermediateTLV_t* stateUpdatePacket = NULL;

static const unsigned int numConfigKeys = 3;
static const char* CONFIG_KEYS[] = {
        "test_key",
        "test_key2",
        "test_key3"
};

static const uint8_t CONFIG_REQUEST_DATA[] = {
        //"test_key", "test_key2", "test_key3"
        0x74, 0x65, 0x73, 0x74,
        0x5F, 0x6B, 0x65, 0x79,
        0x00, 0x74, 0x65, 0x73,
        0x74, 0x5F, 0x6B, 0x65,
        0x79, 0x32, 0x00, 0x74,
        0x65, 0x73, 0x74, 0x5F,
        0x6B, 0x65, 0x79, 0x33,
        0x00, 0x00, 0x00, 0x00
};

//This type holds a List_t for the keys requested, this is populated in setup
static PTLVData_CONFIG_REQUEST_t knownGoodConfigRequest = {
        NULL
};

static IntermediateTLV_t* configRequestPacket = NULL;

static const uint8_t CONFIG_RESPONSE_DATA[] = {
        //"test_key"
        0x74, 0x65, 0x73, 0x74,
        0x5F, 0x6B, 0x65, 0x79,
        0x00,
        //Integer value 0x55 and seperator
        0x00, 0x00, 0x00, 0x08,
        0x55, 0x00, 0x00, 0x55,
        0x3B,
        //"test_key2"
        0x74, 0x65, 0x73, 0x74,
        0x5F, 0x6B, 0x65, 0x79,
        0x32, 0x00,
        //boolean value False, and seperator
        0x04, 0x00, 0x00, 0x05,
        0x00, 0x3B,
        //"test_key3"
        0x74, 0x65, 0x73, 0x74,
        0x5F, 0x6B, 0x65, 0x79,
        0x33, 0x00,
        //C-string "Hello, world"
        0x03, 0x00, 0x00, 0x11,
        0x48, 0x65, 0x6C, 0x6C,
        0x6F, 0x2C, 0x20, 0x77,
        0x6F, 0x72, 0x6C, 0x64,
        0x00,
        //3 bytes padding required
        0x00, 0x00, 0x00
};
//This type holds a List_t for the keys response, this is populated in setup
static PTLVData_CONFIG_RESPONSE_t knownGoodConfigResponse = {
    NULL
};
static IntermediateTLV_t* configResponsePacket = NULL;

static const uint8_t CONFIG_UPDATE_DATA[] = {
        //"test_key"
        0x74, 0x65, 0x73, 0x74,
        0x5F, 0x6B, 0x65, 0x79,
        0x00,
        //Integer value 0x55 and seperator
        0x00, 0x00, 0x00, 0x08,
        0x55, 0x00, 0x00, 0x55,
        0x3B,
        //"test_key2"
        0x74, 0x65, 0x73, 0x74,
        0x5F, 0x6B, 0x65, 0x79,
        0x32, 0x00,
        //boolean value False, and seperator
        0x04, 0x00, 0x00, 0x05,
        0x00, 0x3B,
        //"test_key3"
        0x74, 0x65, 0x73, 0x74,
        0x5F, 0x6B, 0x65, 0x79,
        0x33, 0x00,
        //C-string "Hello, world"
        0x03, 0x00, 0x00, 0x11,
        0x48, 0x65, 0x6C, 0x6C,
        0x6F, 0x2C, 0x20, 0x77,
        0x6F, 0x72, 0x6C, 0x64,
        0x00,
        //3 bytes padding required
        0x00, 0x00, 0x00
};
//This type holds a List_t for the keys response, this is populated in setup
static PTLVData_CONFIG_UPDATE_t knownGoodConfigUpdate = {
        NULL
};
static IntermediateTLV_t* configUpdatePacket = NULL;

static const uint8_t DEBUG_DATA[] = {
    //LE packing of code status and git hash, may cause test to fail on BE computers
    0x1c, 0x96, 0x8e, 0x41,
    //Robot UUID
    0x01, 0x02, 0x02, 0x01,
    //Robot state, rsvd, numConfigEntries (2 bytes)
    0x02, 0x00, 0xFF, 0xFF,
    //Arbitrary data
    0x01, 0x02, 0x03, 0x04,
    0x04, 0x03, 0x02, 0x01,
    0xAA, 0x55, 0xAA, 0x55,
    0xFF, 0x00, 0xFF, 0x00,
    0x00, 0xFF, 0x00, 0xFF
};
static PTLVData_DEBUG_t knownGoodDebug = {
    0x01,0x0c968e41,0x01020201,0x02, 0x00, 0xFFFF,NULL
};
static IntermediateTLV_t* debugPacket=NULL;

void setup(PacketType_t setupType)
{
    char* key0;
    char* key1;
    char* key2;
    KVPairTLV_t* tlv0;
    KVPairTLV_t* tlv1;
    KVPairTLV_t* tlv2;
    switch(setupType)
    {
        case pt_INIT:
            initPacket = malloc(sizeof(IntermediateTLV_t));
            initPacket->type = pt_INIT;
            initPacket->length = sizeof(INIT_DATA);
            initPacket->data = (uint8_t*)malloc(sizeof(INIT_DATA));
            memcpy(initPacket->data, INIT_DATA, sizeof(INIT_DATA));
            initPacket->timestamp = 0xAAAAAAAA;
            break;
        case pt_USER_DATA:
            userDataPacket = malloc(sizeof(IntermediateTLV_t));
            userDataPacket->type = pt_USER_DATA;
            userDataPacket->length = sizeof(USER_DATA_DATA);
            userDataPacket->data = (uint8_t*)malloc(sizeof(USER_DATA_DATA));
            memcpy(userDataPacket->data, USER_DATA_DATA, sizeof(USER_DATA_DATA));
            userDataPacket->timestamp = 0xAAAAAAAA;
            break;
        case pt_STATE_REQUEST:
            stateRequestPacket = malloc(sizeof(IntermediateTLV_t));
            stateRequestPacket->type = pt_STATE_REQUEST;
            stateRequestPacket->length = 0;
            stateRequestPacket->data = NULL;
            stateRequestPacket->timestamp = 0xAAAAAAAA;
            break;
        case pt_STATE_RESPONSE:
            stateResponsePacket = malloc(sizeof(IntermediateTLV_t));
            stateResponsePacket->type = pt_STATE_RESPONSE;
            stateResponsePacket->length = sizeof(STATE_RESPONSE_DATA);
            stateResponsePacket->timestamp = 0xAAAAAAAA;
            stateResponsePacket->data = malloc(sizeof(STATE_RESPONSE_DATA));
            memcpy(stateResponsePacket->data, STATE_RESPONSE_DATA, sizeof(STATE_RESPONSE_DATA));
            knownGoodStateResponse.arbitrary = malloc(sizeof(STATE_RESPONSE_DATA) - 4);
            memcpy(knownGoodStateResponse.arbitrary, STATE_RESPONSE_DATA+4, sizeof(STATE_RESPONSE_DATA) - 4);
            break;
        case pt_STATE_UPDATE:
            stateUpdatePacket = malloc(sizeof(IntermediateTLV_t));
            stateUpdatePacket->type = pt_STATE_UPDATE;
            stateUpdatePacket->length = sizeof(STATE_UPDATE_DATA);
            stateUpdatePacket->timestamp = 0xAAAAAAAA;
            stateUpdatePacket->data = malloc(sizeof(STATE_UPDATE_DATA));
            memcpy(stateUpdatePacket->data, STATE_UPDATE_DATA, sizeof(STATE_UPDATE_DATA));
            knownGoodStateUpdate.arbitrary = malloc(sizeof(STATE_UPDATE_DATA) - 4);
            memcpy(knownGoodStateUpdate.arbitrary, STATE_UPDATE_DATA+4, sizeof(STATE_UPDATE_DATA) - 4);
            break;
        case pt_CONFIG_REQUEST:
            configRequestPacket = malloc(sizeof(IntermediateTLV_t));
            configRequestPacket->type = pt_CONFIG_REQUEST;
            configRequestPacket->length = sizeof(CONFIG_REQUEST_DATA);
            configRequestPacket->timestamp = 0xAAAAAAAA;
            configRequestPacket->data = malloc(sizeof(CONFIG_REQUEST_DATA));
            memcpy(configRequestPacket->data, CONFIG_REQUEST_DATA, sizeof(CONFIG_REQUEST_DATA));
            knownGoodConfigRequest.keys = (List_t*)arraylist_init_len(3);
            key0 = (char*)malloc(strlen(CONFIG_KEYS[0]) + 1);
            strcpy(key0, CONFIG_KEYS[0]);
            key1 = (char*)malloc(strlen(CONFIG_KEYS[1])+ 1);
            strcpy(key1, CONFIG_KEYS[1]);
            key2 = (char*)malloc(strlen(CONFIG_KEYS[2]) + 1);
            strcpy(key2, CONFIG_KEYS[2]);
            arraylist_add((ArrayList_t*)knownGoodConfigRequest.keys, key0);
            arraylist_add((ArrayList_t*)knownGoodConfigRequest.keys, key1);
            arraylist_add((ArrayList_t*)knownGoodConfigRequest.keys, key2);
            break;
        case pt_CONFIG_RESPONSE:
            configResponsePacket = malloc(sizeof(IntermediateTLV_t));
            configResponsePacket->type = pt_CONFIG_RESPONSE;
            configResponsePacket->length = sizeof(CONFIG_RESPONSE_DATA);
            configResponsePacket->timestamp = 0xAAAAAAAA;
            configResponsePacket->data = malloc(sizeof(CONFIG_RESPONSE_DATA));
            memcpy(configResponsePacket->data, CONFIG_RESPONSE_DATA, sizeof(CONFIG_RESPONSE_DATA));
            knownGoodConfigResponse.pairs = (List_t*)arraylist_init_len(3);
            key0 = (char*)malloc(strlen(CONFIG_KEYS[0]) + 1);
            strcpy(key0, CONFIG_KEYS[0]);
            key1 = (char*)malloc(strlen(CONFIG_KEYS[1])+ 1);
            strcpy(key1, CONFIG_KEYS[1]);
            key2 = (char*)malloc(strlen(CONFIG_KEYS[2]) + 1);
            strcpy(key2, CONFIG_KEYS[2]);
            tlv0 = malloc(sizeof(KVPairTLV_t));
            tlv1 = malloc(sizeof(KVPairTLV_t));
            tlv2 = malloc(sizeof(KVPairTLV_t));
            tlv0->key = key0;
            tlv1->key = key1;
            tlv2->key = key2;
            tlv0->length = 8;
            tlv1->length = 5;
            tlv2->length = 17;
            tlv0->type = kv_Integer;
            tlv1->type = kv_Boolean;
            tlv2->type = kv_CString;
            //Stupid BE/LE difference, making me test symmetric numbers
            tlv0->value.Integer = 0x55000055;
            tlv1->value.Boolean = 0x00;
            //Length of string "Hello, world" + nul terminator
            tlv2->value.CString = (char*)malloc(13);
            strcpy(tlv2->value.CString, "Hello, world");
            list_add(knownGoodConfigResponse.pairs, tlv0);
            list_add(knownGoodConfigResponse.pairs, tlv1);
            list_add(knownGoodConfigResponse.pairs, tlv2);
            break;
        case pt_CONFIG_UPDATE:
            configUpdatePacket = malloc(sizeof(IntermediateTLV_t));
            configUpdatePacket->type = pt_CONFIG_UPDATE;
            configUpdatePacket->length = sizeof(CONFIG_UPDATE_DATA);
            configUpdatePacket->timestamp = 0xAAAAAAAA;
            configUpdatePacket->data = malloc(sizeof(CONFIG_UPDATE_DATA));
            memcpy(configUpdatePacket->data, CONFIG_UPDATE_DATA, sizeof(CONFIG_UPDATE_DATA));
            knownGoodConfigUpdate.new_pairs = (List_t*)arraylist_init_len(3);
            key0 = (char*)malloc(strlen(CONFIG_KEYS[0]) + 1);
            strcpy(key0, CONFIG_KEYS[0]);
            key1 = (char*)malloc(strlen(CONFIG_KEYS[1])+ 1);
            strcpy(key1, CONFIG_KEYS[1]);
            key2 = (char*)malloc(strlen(CONFIG_KEYS[2]) + 1);
            strcpy(key2, CONFIG_KEYS[2]);
            tlv0 = malloc(sizeof(KVPairTLV_t));
            tlv1 = malloc(sizeof(KVPairTLV_t));
            tlv2 = malloc(sizeof(KVPairTLV_t));
            tlv0->key = key0;
            tlv1->key = key1;
            tlv2->key = key2;
            tlv0->length = 8;
            tlv1->length = 5;
            tlv2->length = 17;
            tlv0->type = kv_Integer;
            tlv1->type = kv_Boolean;
            tlv2->type = kv_CString;
            //Stupid BE/LE difference, making me test symmetric numbers
            tlv0->value.Integer = 0x55000055;
            tlv1->value.Boolean = 0x00;
            //Length of string "Hello, world" + nul terminator
            tlv2->value.CString = (char*)malloc(13);
            strcpy(tlv2->value.CString, "Hello, world");
            list_add(knownGoodConfigUpdate.new_pairs, tlv0);
            list_add(knownGoodConfigUpdate.new_pairs, tlv1);
            list_add(knownGoodConfigUpdate.new_pairs, tlv2);
            break;
        case pt_DEBUG:
            debugPacket = malloc(sizeof(IntermediateTLV_t));
            debugPacket->type = pt_DEBUG;
            debugPacket->length = sizeof(DEBUG_DATA);
            debugPacket->timestamp = 0xAAAAAAAA;
            debugPacket->data = malloc(sizeof(DEBUG_DATA));
            memcpy(debugPacket->data, DEBUG_DATA, sizeof(DEBUG_DATA));
            knownGoodDebug.arbitrary = malloc(sizeof(DEBUG_DATA) - 12);
            memcpy(knownGoodDebug.arbitrary, DEBUG_DATA+12, sizeof(DEBUG_DATA) - 12);
            break;
        default:
            break;
    }
}

void teardown(PacketType_t setupType)
{
    switch(setupType)
    {
        case pt_INIT:
            initPacket = NULL;
            break;
        case pt_USER_DATA:
            userDataPacket = NULL;
            break;
        case pt_STATE_REQUEST:
            stateRequestPacket = NULL;
            break;
        case pt_STATE_RESPONSE:
            free(knownGoodStateResponse.arbitrary);
            knownGoodStateResponse.arbitrary = NULL;
            stateResponsePacket = NULL;
            break;
        case pt_STATE_UPDATE:
            free(knownGoodStateUpdate.arbitrary);
            knownGoodStateUpdate.arbitrary = NULL;
            break;
        case pt_CONFIG_REQUEST:
            free(arraylist_get((ArrayList_t*)knownGoodConfigRequest.keys, 0));
            free(arraylist_get((ArrayList_t*)knownGoodConfigRequest.keys, 1));
            free(arraylist_get((ArrayList_t*)knownGoodConfigRequest.keys, 2));
            arraylist_free((ArrayList_t*)knownGoodConfigRequest.keys);
            break;
        case pt_CONFIG_RESPONSE:
            //Free all the known-good TLVs
            for(unsigned int i = 0; i < list_size(knownGoodConfigResponse.pairs); i++)
            {
                KVPairTLV_destroy(list_get(knownGoodConfigResponse.pairs, i));
            }

            list_free(knownGoodConfigResponse.pairs);
            break;
        case pt_CONFIG_UPDATE:
            //Free all the known-good TLVs
            for(unsigned int i = 0; i < list_size(knownGoodConfigUpdate.new_pairs); i++)
            {
                KVPairTLV_destroy(list_get(knownGoodConfigUpdate.new_pairs, i));
            }
            list_free(knownGoodConfigUpdate.new_pairs);
            break;
        case pt_DEBUG:
            free(knownGoodDebug.arbitrary);
            break;
        default:
            break;
    }
}

int t01_testUserDataUnpacking() {
    setup(pt_USER_DATA);
    int errCount = 0;
    PacketTLV_t *unpackedPacket = unpackUserData(userDataPacket);
    //Something failed in the function.  They system is probably out of memory
    if (unpackedPacket == NULL) {
        dbg_error("unpackUserData returned NULL!\n")
        errCount++;
    }

    //Check if the type was stored correctly
    if (unpackedPacket->type != pt_USER_DATA) {
        dbg_error("unpackUserData returned incorrect packetType!\n");
        errCount++;
    }

    //Check if the timestamp was stored correctly
    if (unpackedPacket->timestamp != 0xAAAAAAAA) {
        dbg_error("unpackUserData returned incorrect timestamp!\n");
        errCount++;
    }

    //Check if the length was stored correctly
    if (unpackedPacket->length != 8) {
        dbg_error("unpackUserData returned incorrect length!\n");
        errCount++;
    }

    //Check if the left and right sticks were unpacked correctly
    if (((PTLVData_USER_DATA_t *)(unpackedPacket->data))->left_stick_x != knownGoodUserData.left_stick_x)
    {
        dbg_error("unpackUserData incorrectly unpacked the left stick x axis!\n");
        errCount++;
    }
    if (((PTLVData_USER_DATA_t *) (unpackedPacket->data))->left_stick_y != knownGoodUserData.left_stick_y)
    {
        dbg_error("unpackUserData incorrectly unpacked the left stick y axis!\n");
        errCount++;
    }
    if (((PTLVData_USER_DATA_t *) (unpackedPacket->data))->right_stick_x != knownGoodUserData.right_stick_x)
    {
        dbg_error("unpackUserData incorrectly unpacked the right stick x axis!\n");
        errCount++;
    }
    if (((PTLVData_USER_DATA_t *) (unpackedPacket->data))->right_stick_y != knownGoodUserData.right_stick_y)
    {
        dbg_error("unpackUserData incorrectly unpacked the right stick y axis!\n");
        errCount++;
    }

    //Check if the buttons were unpacked correctly
    if (((PTLVData_USER_DATA_t *) (unpackedPacket->data))->button_a != knownGoodUserData.button_a)
    {
        dbg_error("unpackUserData incorrectly unpacked button A!\n");
        errCount++;
    }
    if (((PTLVData_USER_DATA_t *) (unpackedPacket->data))->button_b != knownGoodUserData.button_b)
    {
        dbg_error("unpackUserData incorrectly unpacked button B!\n");
        errCount++;
    }

    //Check if the controller UUID was unpacked correctly
    if (((PTLVData_USER_DATA_t *) (unpackedPacket->data))->controller_uuid != knownGoodUserData.controller_uuid)
    {
        fprintf(stderr,"Unpacked controller UUID: %hx\n", ((PTLVData_USER_DATA_t*)(unpackedPacket->data))->controller_uuid);
        dbg_error("unpackUserData incorrectly unpacked controller UUID!\n");
        errCount++;
    }
    teardown(pt_USER_DATA);
    destroyUserData(unpackedPacket);
    return errCount;
}

int t02_testStateUpdateUnpacking()
{
    setup(pt_STATE_UPDATE);
    int errCount = 0;
    PacketTLV_t *unpackedPacket = unpackStateUpdate(stateUpdatePacket);
    //Something failed in the function.  They system is probably out of memory
    if (unpackedPacket == NULL) {
        dbg_error("unpackStateUpdate returned NULL!\n")
        errCount++;
    }

    //Check if the type was stored correctly
    if (unpackedPacket->type != pt_STATE_UPDATE) {
        dbg_error("unpackStateUpdate returned incorrect packetType!\n");
        errCount++;
    }

    //Check if the timestamp was stored correctly
    if (unpackedPacket->timestamp != 0xAAAAAAAA) {
        dbg_error("unpackStateUpdate returned incorrect timestamp!\n");
        errCount++;
    }

    //Check if the length was stored correctly
    if (unpackedPacket->length != sizeof(STATE_UPDATE_DATA)) {
        dbg_error("unpackStateUpdate returned incorrect length!\n");
        errCount++;
    }

    //Check the state and reserved fields
    PTLVData_STATE_UPDATE_t* unpackedData = (PTLVData_STATE_UPDATE_t*)unpackedPacket->data;
    if(unpackedData->new_state != knownGoodStateUpdate.new_state)
    {
        dbg_error("unpackStateUpdate returned incorrect state!\n");
        errCount++;
    }
    if(unpackedData->reserved != knownGoodStateUpdate.reserved)
    {
        //Only a warning since this is not currently critical
        dbg_warning("unpackStateUpdate returned incorrect reserved data!\n");
    }

    //Check the arbitrary data
    uint8_t* arbitraryData = (uint8_t*)unpackedData->arbitrary;
    //No data was returned
    if(arbitraryData == NULL)
    {
        dbg_error("UnpackStateUpdate did not unpack arbitrary data!");
        errCount++;
    }
    //Check contents of data
    else
    {
        for (int i = 0; i < unpackedPacket->length-4; i++)
        {
            if (arbitraryData[i] != ((uint8_t*)(knownGoodStateUpdate.arbitrary))[i])
            {
                dbg_error("UnpackStateUpdate incorrectly unpacked arbitrary data");
                errCount++;
                break;
            }
        }
    }
    teardown(pt_STATE_UPDATE);
    destroyStateUpdate(unpackedPacket);
    return errCount;
}

int t03_testInitUnpacking()
{
    setup(pt_INIT);
    int errCount = 0;
    PacketTLV_t *unpackedPacket = unpackInit(initPacket);
    //Something failed in the function.  They system is probably out of memory
    if (unpackedPacket == NULL) {
        dbg_error("unpackInit returned NULL!\n")
        errCount++;
    }

    //Check if the type was stored correctly
    if (unpackedPacket->type != pt_INIT) {
        dbg_error("unpackInit returned incorrect packetType!\n");
        errCount++;
    }

    //Check if the timestamp was stored correctly
    if (unpackedPacket->timestamp != 0xAAAAAAAA) {
        dbg_error("unpackInit returned incorrect timestamp!\n");
        errCount++;
    }

    //Check if the length was stored correctly
    if (unpackedPacket->length != sizeof(INIT_DATA)) {
        dbg_error("unpackInit returned incorrect length!\n");
        errCount++;
    }

    //Check the state and reserved fields
    PTLVData_INIT_t* unpackedData = (PTLVData_INIT_t*)unpackedPacket->data;
    if(unpackedData->robot_uuid != knownGoodInit.robot_uuid)
    {
        dbg_error("unpackInit returned incorrect robot_uuid!\n");
        printf("%x\n", unpackedData->robot_uuid);
        errCount++;
    }
    teardown(pt_INIT);
    destroyInit(unpackedPacket);
    return errCount;
}

int t04_testStateRequestUnpacking()
{
    setup(pt_STATE_REQUEST);
    int errCount = 0;
    PacketTLV_t *unpackedPacket = unpackStateRequest(stateRequestPacket);
    //Something failed in the function.  They system is probably out of memory
    if (unpackedPacket == NULL) {
        dbg_error("unpackStateRequest returned NULL!\n")
        errCount++;
    }

    //Check if the type was stored correctly
    if (unpackedPacket->type != pt_STATE_REQUEST) {
        dbg_error("unpackStateRequest returned incorrect packetType!\n");
        errCount++;
    }

    //Check if the timestamp was stored correctly
    if (unpackedPacket->timestamp != 0xAAAAAAAA) {
        dbg_error("unpackStateRequest returned incorrect timestamp!\n");
        errCount++;
    }

    //Check if the length was stored correctly
    if (unpackedPacket->length != 0) {
        dbg_error("unpackStateRequest returned incorrect length!\n");
        errCount++;
    }

    //Check the state and reserved fields
    struct PTLVData_STATE_REQUEST_t* unpackedData = (struct PTLVData_STATE_REQUEST_t*)unpackedPacket->data;
    if(unpackedData != NULL)
    {
        dbg_error("unpackStateRequest unpacked data from empty packet\n");
        errCount++;
    }
    teardown(pt_STATE_REQUEST);
    destroyStateRequest(unpackedPacket);
    return errCount;
}

int t05_testStateResponseUnpacking()
{
    setup(pt_STATE_RESPONSE);
    int errCount = 0;
    PacketTLV_t *unpackedPacket = unpackStateResponse(stateResponsePacket);
    //Something failed in the function.  They system is probably out of memory
    if (unpackedPacket == NULL) {
        dbg_error("unpackStateResponse returned NULL!\n")
        errCount++;
    }

    //Check if the type was stored correctly
    if (unpackedPacket->type != pt_STATE_RESPONSE) {
        dbg_error("unpackStateResponse returned incorrect packetType!\n");
        errCount++;
    }

    //Check if the timestamp was stored correctly
    if (unpackedPacket->timestamp != 0xAAAAAAAA) {
        dbg_error("unpackStateResponse returned incorrect timestamp!\n");
        errCount++;
    }

    //Check if the length was stored correctly
    if (unpackedPacket->length != sizeof(STATE_RESPONSE_DATA)) {
        dbg_error("unpackStateResponse returned incorrect length!\n");
        errCount++;
    }

    //Check the state and reserved fields
    PTLVData_STATE_RESPONSE_t* unpackedData = (PTLVData_STATE_RESPONSE_t*)unpackedPacket->data;
    if(unpackedData->state != knownGoodStateResponse.state)
    {
        dbg_error("unpackStateResponse returned incorrect state!\n");
        errCount++;
    }
    if(unpackedData->reserved != knownGoodStateResponse.reserved)
    {
        //Only a warning since this is not currently critical
        dbg_warning("unpackStateResponse returned incorrect reserved data!\n");
    }

    //Check the arbitrary data
    uint8_t* arbitraryData = (uint8_t*)unpackedData->arbitrary;
    //No data was returned
    if(arbitraryData == NULL)
    {
        dbg_error("UnpackStateResponse did not unpack arbitrary data!");
        errCount++;
    }
        //Check contents of data
    else
    {
        for (int i = 0; i < unpackedPacket->length-4; i++)
        {
            if (arbitraryData[i] != ((uint8_t*)(knownGoodStateResponse.arbitrary))[i])
            {
                dbg_error("UnpackStateUpdate incorrectly unpacked arbitrary data");
                errCount++;
                break;
            }
        }
    }
    teardown(pt_STATE_RESPONSE);
    destroyStateResponse(unpackedPacket);
    return errCount;
}

int t06_testConfigRequestUnpacking()
{
    setup(pt_CONFIG_REQUEST);
    int errCount = 0;
    PacketTLV_t *unpackedPacket = unpackConfigRequest(configRequestPacket);
    //Something failed in the function.  They system is probably out of memory
    if (unpackedPacket == NULL) {
        dbg_error("unpackConfigRequest returned NULL!\n")
        errCount++;
    }

    //Check if the type was stored correctly
    if (unpackedPacket->type != pt_CONFIG_REQUEST) {
        dbg_error("unpackConfigRequest returned incorrect packetType!\n");
        errCount++;
    }

    //Check if the timestamp was stored correctly
    if (unpackedPacket->timestamp != 0xAAAAAAAA) {
        dbg_error("unpackConfigRequest returned incorrect timestamp!\n");
        errCount++;
    }

    //Check if the length was stored correctly
    if (unpackedPacket->length != sizeof(CONFIG_REQUEST_DATA)) {
        dbg_error("unpackConfigRequest returned incorrect length!\n");
        errCount++;
    }

    //Check that the keys were unpacked
    PTLVData_CONFIG_REQUEST_t* unpackedData = (PTLVData_CONFIG_REQUEST_t*)unpackedPacket->data;
    if(unpackedData->keys == NULL)
    {
        dbg_error("unpackConfigRequest did not unpack the request list");
        errCount++;
    }
    //Check that the correct number of keys were unpacked
    if(list_size(unpackedData->keys) != numConfigKeys)
    {
        dbg_error("unpackConfigRequest did not unpack the correct number of keys");
        printf("%ud != %ud", list_size(unpackedData->keys), numConfigKeys);
        errCount++;
    }
    //Check the contents of the keys list
    for(unsigned int i = 0; i < min(list_size(unpackedData->keys), numConfigKeys); i++)
    {
        if(strcmp(list_get(unpackedData->keys,i), CONFIG_KEYS[i]) != 0)
        {
            dbg_error("unpackConfigRequest did not correctly unpack the request list");
            printf("%s != %s", (char*)list_get(unpackedData->keys, i), CONFIG_KEYS[i]);
            errCount++;
        }
    }
    teardown(pt_CONFIG_REQUEST);
    destroyConfigRequest(unpackedPacket);
    return errCount;
}

int t07_testConfigResponseUnpacking()
{
    setup(pt_CONFIG_RESPONSE);
    int errCount = 0;
    PacketTLV_t *unpackedPacket = unpackConfigResponse(configResponsePacket);
    //Something failed in the function.  They system is probably out of memory
    if (unpackedPacket == NULL) {
        dbg_error("unpackConfigResponse returned NULL!\n")
        errCount++;
    }

    //Check if the type was stored correctly
    if (unpackedPacket->type != pt_CONFIG_RESPONSE) {
        dbg_error("unpackConfigResponse returned incorrect packetType!\n");
        errCount++;
    }

    //Check if the timestamp was stored correctly
    if (unpackedPacket->timestamp != 0xAAAAAAAA) {
        dbg_error("unpackConfigResponse returned incorrect timestamp!\n");
        errCount++;
    }

    //Check if the length was stored correctly
    if (unpackedPacket->length != sizeof(CONFIG_RESPONSE_DATA)) {
        dbg_error("unpackConfigResponse returned incorrect length!\n");
        errCount++;
    }

    //Check that the keys were unpacked
    PTLVData_CONFIG_RESPONSE_t* unpackedData = (PTLVData_CONFIG_RESPONSE_t*)unpackedPacket->data;
    if(unpackedData->pairs == NULL)
    {
        dbg_error("unpackConfigResponse did not unpack the config value list\n");
        errCount++;
    }
    //Check that the correct number of keys were unpacked
    if(list_size(unpackedData->pairs) != numConfigKeys)
    {
        dbg_error("unpackConfigResponse did not unpack the correct number of values\n");
        printf("%u != %u\n", list_size(unpackedData->pairs), numConfigKeys);
        errCount++;
    }
    //Check the contents of the keys list
    KVPairTLV_t* testPair = NULL;
    KVPairTLV_t* knownGoodPair = NULL;
    for(unsigned int i = 0; i < min(list_size(unpackedData->pairs), numConfigKeys); i++)
    {
        testPair = (KVPairTLV_t*)(list_get(unpackedData->pairs, i));
        knownGoodPair = (KVPairTLV_t*)(list_get(knownGoodConfigResponse.pairs, i));
        if(strcmp(testPair->key, CONFIG_KEYS[i]) != 0)
        {
            dbg_error("unpackConfigResponse did not correctly unpack the request keys\n");
            printf("%s != %s\n", testPair->key, CONFIG_KEYS[i]);
            errCount++;
        }
        if(!KVPairTLV_equals(testPair, knownGoodPair))
        {
            dbg_error("unpackConfigResponse did not correctly unpack the request KVPairs\n");
            errCount++;
        }
    }
    teardown(pt_CONFIG_RESPONSE);
    destroyConfigResponse(unpackedPacket);
    return errCount;
}

int t08_testConfigUpdateUnpacking()
{
    setup(pt_CONFIG_UPDATE);
    int errCount = 0;
    PacketTLV_t *unpackedPacket = unpackConfigUpdate(configUpdatePacket);
    //Something failed in the function.  They system is probably out of memory
    if (unpackedPacket == NULL) {
        dbg_error("unpackConfigUpdate returned NULL!\n")
        errCount++;
    }

    //Check if the type was stored correctly
    if (unpackedPacket->type != pt_CONFIG_UPDATE) {
        dbg_error("unpackConfigUpdate returned incorrect packetType!\n");
        errCount++;
    }

    //Check if the timestamp was stored correctly
    if (unpackedPacket->timestamp != 0xAAAAAAAA) {
        dbg_error("unpackConfigUpdate returned incorrect timestamp!\n");
        errCount++;
    }

    //Check if the length was stored correctly
    if (unpackedPacket->length != sizeof(CONFIG_UPDATE_DATA)) {
        dbg_error("unpackConfigUpdate returned incorrect length!\n");
        errCount++;
    }

    //Check that the keys were unpacked
    PTLVData_CONFIG_UPDATE_t* unpackedData = (PTLVData_CONFIG_UPDATE_t*)unpackedPacket->data;
    if(unpackedData->new_pairs == NULL)
    {
        dbg_error("unpackConfigUpdate did not unpack the config value list\n");
        errCount++;
    }
    //Check that the correct number of keys were unpacked
    if(list_size(unpackedData->new_pairs) != numConfigKeys)
    {
        dbg_error("unpackConfigUpdate did not unpack the correct number of values\n");
        printf("%u != %u\n", list_size(unpackedData->new_pairs), numConfigKeys);
        errCount++;
    }
    //Check the contents of the keys list
    KVPairTLV_t* testPair = NULL;
    KVPairTLV_t* knownGoodPair = NULL;
    for(unsigned int i = 0; i < min(list_size(unpackedData->new_pairs), numConfigKeys); i++)
    {
        testPair = (KVPairTLV_t*)(list_get(unpackedData->new_pairs, i));
        knownGoodPair = (KVPairTLV_t*)(list_get(knownGoodConfigUpdate.new_pairs, i));
        if(strcmp(testPair->key, CONFIG_KEYS[i]) != 0)
        {
            dbg_error("unpackConfigUpdate did not correctly unpack the request keys\n");
            printf("%s != %s\n", testPair->key, CONFIG_KEYS[i]);
            errCount++;
        }
        if(!KVPairTLV_equals(testPair, knownGoodPair))
        {
            dbg_error("unpackConfigUpdate did not correctly unpack the request KVPairs\n");
            errCount++;
        }
    }
    teardown(pt_CONFIG_UPDATE);
    destroyConfigUpdate(unpackedPacket);
    return errCount;
}

int t09_testDebugUnpacking()
{
    setup(pt_DEBUG);
    int errCount = 0;
    PacketTLV_t *unpackedPacket = unpackDebug(debugPacket);
    //Something failed in the function.  They system is probably out of memory
    if (unpackedPacket == NULL) {
        dbg_error("unpackDebug returned NULL!\n")
        errCount++;
    }

    //Check if the type was stored correctly
    if (unpackedPacket->type != pt_DEBUG) {
        dbg_error("unpackDebug returned incorrect packetType!\n");
        errCount++;
    }

    //Check if the timestamp was stored correctly
    if (unpackedPacket->timestamp != 0xAAAAAAAA) {
        dbg_error("unpackDebug returned incorrect timestamp!\n");
        errCount++;
    }

    //Check if the length was stored correctly
    if (unpackedPacket->length != sizeof(DEBUG_DATA)) {
        dbg_error("unpackDebug returned incorrect length!\n");
        errCount++;
    }

    PTLVData_DEBUG_t* unpackedData = (PTLVData_DEBUG_t*)(unpackedPacket->data);
    if(unpackedData->state != knownGoodDebug.state)
    {
        dbg_error("unpackDebug returned incorrect state!\n");
        printf("Note, this could be an artifact of host endianness\n\n");
        errCount++;
    }
    if(unpackedData->commit_hash != knownGoodDebug.commit_hash)
    {
        dbg_error("unpackDebug returned incorrect commit hash!\n");
        printf("%x != %x\n", unpackedData->commit_hash, knownGoodDebug.commit_hash);
        printf("Note, this could be an artifact of host endianness\n\n");
        errCount++;
    }
    if(unpackedData->robot_uuid != knownGoodDebug.robot_uuid)
    {
        dbg_error("unpackDebug returned incorrect robot uuid!\n");
        fprintf(stderr, "%x != %x\n", unpackedData->robot_uuid, knownGoodDebug.robot_uuid);
        errCount++;
    }
    if(unpackedData->config_entries != knownGoodDebug.config_entries)
    {
        dbg_error("unpackDebug returned incorrect number of config entries!\n");
        errCount++;
    }
    if(unpackedData->reserved != knownGoodDebug.reserved)
    {
        dbg_warning("UnpackDebug incorrectly unpacked reserved field!\n");
    }

//Check the arbitrary data
    uint8_t* arbitraryData = (uint8_t*)unpackedData->arbitrary;
    //No data was returned
    if(arbitraryData == NULL)
    {
        dbg_error("UnpackDebug did not unpack arbitrary data!");
        errCount++;
    }
    //Check contents of data
    else
    {
        for (int i = 0; i < unpackedPacket->length-12; i++)
        {
            if (arbitraryData[i] != ((uint8_t*)(knownGoodDebug.arbitrary))[i])
            {
                dbg_error("UnpackDebug incorrectly unpacked arbitrary data\n");
                printf("%hhu != %hhu\n", arbitraryData[i], ((uint8_t*)(knownGoodDebug.arbitrary))[i]);
                errCount++;
                break;
            }
        }
    }
    teardown(pt_DEBUG);
    destroyDebug(unpackedPacket);
    return errCount;
}

int main()
{
    // Run tests on both types of list
    int error = 0;
    int allErrors = 0;
    printf("Starting Test01!\n");
    error = t01_testUserDataUnpacking();
    if(error == 0 ) {
        printf("success!\n");
    }
    else {
        printf("^^^ test errors\n");
    }
    allErrors += error;

    printf("Starting Test02!\n");
    error = t02_testStateUpdateUnpacking();
    if(error == 0 ) {
        printf("success!\n");
    }
    else {
        printf("^^^ test errors\n");
    }
    allErrors += error;

    printf("Starting Test03!\n");
    error = t03_testInitUnpacking();
    if(error == 0 ) {
        printf("success!\n");
    }
    else {
        printf("^^^ test errors\n");
    }
    allErrors += error;

    printf("Starting Test04!\n");
    error = t04_testStateRequestUnpacking();
    if(error == 0 ) {
        printf("success!\n");
    }
    else {
        printf("^^^ test errors\n");
    }
    allErrors += error;

    printf("Starting Test05!\n");
    error = t05_testStateResponseUnpacking();
    if(error == 0 ) {
        printf("success!\n");
    }
    else {
        printf("^^^ test errors\n");
    }
    allErrors += error;

    printf("Starting Test06!\n");
    error = t06_testConfigRequestUnpacking();
    if(error == 0 ) {
        printf("success!\n");
    }
    else {
        printf("^^^ test errors\n");
    }
    allErrors += error;

    printf("Starting Test07!\n");
    error = t07_testConfigResponseUnpacking();
    if(error == 0 ) {
        printf("success!\n");
    }
    else {
        printf("^^^ test errors\n");
    }
    allErrors += error;

    printf("Starting Test08!\n");
    error = t08_testConfigUpdateUnpacking();
    if(error == 0 ) {
        printf("success!\n");
    }
    else {
        printf("^^^ test errors\n");
    }
    allErrors += error;

    printf("Starting Test09!\n");
    error = t09_testDebugUnpacking();
    if(error == 0 ) {
        printf("success!\n");
    }
    else {
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