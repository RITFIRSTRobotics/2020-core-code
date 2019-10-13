/*
 * core/network/constants.h
 *
 * Networking constants used to transmit data between the robots and the FMS
 *
 * @author Connor Henley, @thatging3rkid
 */
#ifndef __CORE_NETWORKING_CONSTANTS
#define __CORE_NETWORKING_CONSTANTS
#include <stdint.h>

// Define the robot data type, this defines how the rest of the data in the
// packet should be handled.
typedef enum RobotDataType {
    INIT      = 0x0000,
    STATUS    = 0x0001,
    CONFIG    = 0x0002,
    USER_DATA = 0x1000,
    DEBUG     = 0xffff
} RobotDataType_t;

// Defines a robot data packet. The robot should decode the type and length,
// then decode the data (value) based on the type.
typedef struct RobotData {
    RobotDataType_t type,
    uint32_t length;
    byte* data;    
} RobotData_t;

// Defines the direction the message is going (whether it's a request or repsonse)
typedef enum MessageDirection {
    REQUEST  = 0,
    RESPONSE = 1,
    EVENT    = 2
} MessageDirection_t;

// Defines the action that should be done to handle the message (either get or set)
typedef enum MessageAction {
    // 0 is reserved
    GET = 1,
    SET = 2    
} MessageAction_t;

// Defines how the robot handles incoming data. If the robot is enabled, it will
// configure outputs according to the data. If the robot is disabled, all outputs
// will be off. If the robot is E-Stopped, it will disable outputs and not accept
// any more packets.
typedef enum RobotState {
    // 0 is reserved
    DISABLED = 1,
    ENABLED  = 2,
    ESTOPPED = 0xff
} RobotState_t;

/*
 * Robot Data sub-types
 */

// Defines a signal sent during the initialization sequence. The robot sends 
// INIT requests to the FMS, which will respond with a INIT response when it is
// ready to start working with that robot. INIT event should never happen.
// Should only be used with RobotDataType INIT
typedef struct rd_InitData {
    MessageDirection dir;
}

// Defines a signal sent to get/change the status of a robot. The FMS will send
// a set request and the robot should change its state accordingly then send back
// a set response where the value is the error code. Any device can send a get 
// request and the other device should return a get response with the proper 
// status data. Additionally, any device can raise an event and the other device
// should accept the value as an error code and handle it accordingly.
// Should only be used with RobotDataType STATUS
typedef struct rd_StatusData {
    MessageDirection dir;
    MessageAction action;
    uint32_t value;
} rd_StatusData_t;

// Defines a signal sent to get/change a configuration item of a robot. Any
// device can send a get/set request and the other device should change its 
// state accordingly then send back a get/set response (in the case of a get,
// the value should be the requested value, in the case of a set, the value
// should be an error code).
// Should only be used with RobotDataType CONFIG
typedef struct rd_ConfigData {
    MessageDirection dir;
    MessageAction action;

    char* key; // NULL-terminated string 
    uint32_t length;
    byte* value;
} rd_ConfigData_t;

// Defines data sent to the robot from the controller.
// Should only be used with RobotDataType USER_DATA
typedef struct rd_ControllerData {
    uint8_t x_stick;
    uint8_t y_stick;
    uint8_t buttons;
    uint8_t other; // not currently used,, but compiler would pad here anyways
} rd_ControllerData_t;

// Defines data that the robot will send to the FMS for debugging
// Should only be used with RobotDataType DEBUG
typedef struct rd_DebugData {
    uint64_t i2c_dbg_data;
    uint64_t net_dbg_data;
    uint64_t config_dbg_data;

    // extra unused debugging data
    uint64_t dbg1_extra;
    uint64_t dbg2_extra;
} rd_DebugData_t;

#endif