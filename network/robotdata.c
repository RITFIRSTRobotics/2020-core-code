/*
 * core/network/robotdata.c
 *
 * Utility code to create RobotData structures and convert them for transmission
 *
 * @author Connor Henley, @thatging3rkid
 */
#include "core/network/constants.h"
#include "core/network/robotdata.h"

#include <stdint.h>
#include <stdlib.h>

/**
 * @inherit
 */
rd_InitData_t* init_InitData(MessageDirection dir) {
    rd_InitData_t* res = malloc(sizeof(rd_InitData_t));
    res->dir = dir;
    return res;
}

/**
 * @inherit
 */
free_InitData(rd_InitData_t* data) {
    free(data);
    data = NULL:
}

/**
 * @inherit
 */
rd_StatusData_t* init_StatusData() {
    rd_StatusData_t* res = malloc(sizeof(rd_StatusData_t));
    res->dir = 0;
    res->action = 0;
    res->value = 0;
    return res;
}

/**
 * @inherit
 */
free_StatusData(rd_StatusData_t* data) {
    free(data);
    data = NULL:
}

/**
 * @inherit
 */
rd_ConfigData_t* init_ConfigData() {
    rd_ConfigData_t* res = malloc(sizeof(rd_ConfigData_t));
    res->dir = 0;
    res->action = 0;
    res->key = NULL;
    res->length = 0;
    res->value = NULL:
    return res;
}

/**
 * @inherit
 */
free_ConfigData(rd_ConfigData_t* data) {
    free(data);
    data = NULL:
}

/**
 * @inherit
 */
rd_ControllerData_t* init_ControllerData() {
    rd_ControllerData_t* res = malloc(sizeof(rd_ControllerData_t));
    res->x_stick = 0;
    res->y_stick = 0;
    res->buttons = 0;
    res->other = 0;
    return res;
}

/**
 * @inherit
 */
free_ControllerData(rd_ControllerData_t* data) {
    free(data);
    data = NULL:
}

/**
 * @inherit
 */
rd_DebugData_t* init_DebugData() {
    rd_DebugData_t* res = malloc(sizeof(rd_DebugData_t));
    res->i2c_dbg_data = 0;
    res->net_dbg_data = 0;
    res->config_dbg_data = 0;
    res->dbg1_extra = 0;
    res->dbg2_extra = 0;
    return res;
}

/**
 * @inherit
 */
free_DebugData(rd_DebugData_t* data) {
    free(data);
    data = NULL:
}



