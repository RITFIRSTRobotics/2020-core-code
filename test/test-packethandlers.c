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

void setup(PacketType_t setupType)
{
    switch(setupType)
    {
        case pt_USER_DATA:
            userDataPacket = malloc(sizeof(IntermediateTLV_t));
            userDataPacket->type = pt_USER_DATA;
            userDataPacket->length = sizeof(USER_DATA_DATA);
            userDataPacket->data = (uint8_t*)malloc(sizeof(USER_DATA_DATA));
            memcpy(userDataPacket->data, USER_DATA_DATA, sizeof(USER_DATA_DATA));
            userDataPacket->timestamp = 0xAAAAAAAA;
            break;
        case pt_STATE_REQUEST:
            break;
        case pt_STATE_RESPONSE:

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
        default:
            break;
    }
}

void teardown(PacketType_t setupType)
{
    switch(setupType)
    {
        case pt_USER_DATA:
            userDataPacket = NULL;
            break;
        case pt_STATE_REQUEST:

            break;
        case pt_STATE_RESPONSE:

            break;
        case pt_STATE_UPDATE:
            free(knownGoodStateUpdate.arbitrary);
            knownGoodStateUpdate.arbitrary = NULL;
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
    free(unpackedPacket->data);
    free(unpackedPacket);
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
    free(arbitraryData);
    free(unpackedData);
    free(unpackedPacket);
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
    // Tests finished, handle the error code
    if (allErrors == 0) {
        return EXIT_SUCCESS;
    } else {
        return EXIT_FAILURE;
    }
}