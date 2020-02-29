//
// Created by Alex Kneipp on 2/29/20.
//

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
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

static IntermediateTLV_t* userDataPacket;

void setup()
{
    userDataPacket = malloc(sizeof(IntermediateTLV_t));
    userDataPacket->type = pt_USER_DATA;
    userDataPacket->length = 8;
    userDataPacket->data = USER_DATA_DATA;
    userDataPacket->timestamp = 0xAAAAAAAA;
}

int t01_testUserDataUnpacking() {
    setup();
    int errCount = 0;
    PacketTLV_t *unpackedPacket = unpackUserData(userDataPacket);
    if (unpackedPacket == NULL) {
        dbg_error("unpackUserData returned NULL!")
        errCount++;
    }

    if (unpackedPacket->type != pt_USER_DATA) {
        dbg_error("unpackUserData returned incorrect packetType!");
        errCount++;
    }

    if (unpackedPacket->timestamp != 0xAAAAAAAA) {
        dbg_error("unpackUserData returned incorrect timestamp!");
        errCount++;
    }

    if (unpackedPacket->length != 8) {
        dbg_error("unpackUserData returned incorrect length!");
        errCount++;
    }

    if (((PTLVData_USER_DATA_t *)(unpackedPacket->data))->left_stick_x != knownGoodUserData.left_stick_x)
    {
        dbg_error("unpackUserData incorrectly unpacked the left stick x axis!");
        errCount++;
    }
    if (((PTLVData_USER_DATA_t *) (unpackedPacket->data))->left_stick_y != knownGoodUserData.left_stick_y)
    {
        dbg_error("unpackUserData incorrectly unpacked the left stick y axis!");
        errCount++;
    }
    if (((PTLVData_USER_DATA_t *) (unpackedPacket->data))->right_stick_x != knownGoodUserData.right_stick_x)
    {
        dbg_error("unpackUserData incorrectly unpacked the right stick x axis!");
        errCount++;
    }
    if (((PTLVData_USER_DATA_t *) (unpackedPacket->data))->right_stick_y != knownGoodUserData.right_stick_y)
    {
        dbg_error("unpackUserData incorrectly unpacked the right stick y axis!");
        errCount++;
    }

    if (((PTLVData_USER_DATA_t *) (unpackedPacket->data))->button_a != knownGoodUserData.button_a)
    {
        dbg_error("unpackUserData incorrectly unpacked button A!");
        errCount++;
    }
    if (((PTLVData_USER_DATA_t *) (unpackedPacket->data))->button_b != knownGoodUserData.button_b)
    {
        dbg_error("unpackUserData incorrectly unpacked button B!");
        errCount++;
    }

    if (((PTLVData_USER_DATA_t *) (unpackedPacket->data))->controller_uuid != knownGoodUserData.controller_uuid)
    {
        dbg_error("unpackUserData incorrectly unpacked controller UUID!");
        errCount++;
    }
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
        dbg_info("success!\n");
    }
    else {
        dbg_info("^^^ test errors\n");
    }
    allErrors += error;
    // Tests finished, handle the error code
    if (allErrors == 0) {
        return EXIT_SUCCESS;
    } else {
        return EXIT_FAILURE;
    }
}