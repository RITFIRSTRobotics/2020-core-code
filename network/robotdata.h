/*
 * core/network/robotdata.h
 *
 * Utility code to create RobotData structures and convert them for transmission
 *
 * @author Connor Henley, @thatging3rkid
 */
#include "core/network/constants.h"

/****************************** InitData ******************************/
/**
 * Initialize an InitData struct
 *
 * @param dir the direction of the InitData
 * @return a InitData struct that must be cleaned up when done
 */
rd_InitData_t* init_InitData(MessageDirection dir);

/**
 * Clean up an InitData struct
 *
 * @param the InitData struct to clean up
 */
free_InitData(rd_InitData_t* data);

/***************************** StatusData *****************************/
/**
 * Initialize an StatusData struct
 *
 * @return a StatusData struct that must be cleaned up when done
 */
rd_StatusData_t* init_StatusData();
 
/**
 * Clean up an StatusData struct
 *
 * @param the StatusData struct to clean up
 */
void free_StatusData(rd_StatusData_t* data);

/***************************** ConfigData *****************************/
/**
 * Initialize an ConfigData struct
 *
 * @return a ConfigData struct that must be cleaned up when done
 */
rd_ConfigData_t* init_ConfigData();
 
/**
 * Clean up an ConfigData struct
 *
 * @param the ConfigData struct to clean up
 */
void free_ConfigData(rd_ConfigData_t* data);

/*************************** ControllerData ***************************/
/**
 * Initialize an ControllerData struct
 *
 * @return a ControllerData struct that must be cleaned up when done
 */
rd_ControllerData_t* init_ControllerData();

/**
 * Clean up an ControllerData struct
 *
 * @param the ControllerData struct to clean up
 */
void free_ControllerData(rd_ControllerData_t* data);

/****************************** DebugData *****************************/
/**
 * Initialize an DebugData struct
 *
 * @return a DebugData struct that must be cleaned up when done
 */
rd_DebugData_t* init_DebugData();
 
/**
 * Clean up an DebugData struct
 *
 * @param the DebugData struct to clean up
 */
void free_DebugData(rd_DebugData_t* data);

RobotData* ControllerData_to_rd(ControllerData* data);
ControllerData* rd_to_ControllerData(RobotData* data);

byte* RobotData_to_bytes(RobotData* rd);
RobotData* bytes_to_RobotData(byte* bytes);

