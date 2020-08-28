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

#define MIN(x,y) (x<y?x:y)

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


void setup(PacketType_t setupType)
{
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
            char* key1 = (char*)malloc(strlen(CONFIG_KEYS[0]) + 1);
            strcpy(key1, CONFIG_KEYS[0]);
            char* key2 = (char*)malloc(strlen(CONFIG_KEYS[1])+ 1);
            strcpy(key2, CONFIG_KEYS[1]);
            char* key3 = (char*)malloc(strlen(CONFIG_KEYS[2]) + 1);
            strcpy(key3, CONFIG_KEYS[2]);
            arraylist_add((ArrayList_t*)knownGoodConfigRequest.keys, key1);
            arraylist_add((ArrayList_t*)knownGoodConfigRequest.keys, key2);
            arraylist_add((ArrayList_t*)knownGoodConfigRequest.keys, key3);
            break;
        case pt_CONFIG_RESPONSE:

            break;
        case pt_CONFIG_UPDATE:

            break;
        case pt_DEBUG:

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
    for(unsigned int i = 0; i < MIN(list_size(unpackedData->keys), numConfigKeys); i++)
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

    // Tests finished, handle the error code
    if (allErrors == 0) {
        return EXIT_SUCCESS;
    } else {
        return EXIT_FAILURE;
    }
}