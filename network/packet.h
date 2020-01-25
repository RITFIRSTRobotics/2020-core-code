/*
 * core/network/packet.h
 *
 * Defines the layout and constants used to transmit data between the robots 
 * and the FMS according to the definition in core/doc.
 *
 * @author Connor Henley, @thatging3rkid
 */
#ifndef __CORE_PACKET_CONSTANTS
#define __CORE_PACKET_CONSTANTS

// allow C++ to parse this
#ifdef __cplusplus
extern "C" {
#endif

#include "../collections/list.h"
#include <stdint.h>
#include <stdbool.h>

// Defines an identifier for each different type of packet sent
typedef enum PacketType {
    pt_INIT            = 0x00,
    pt_STATE_REQUEST   = 0x10,
    pt_STATE_RESPONSE  = 0x11,
    pt_STATE_UPDATE    = 0x12,
    pt_CONFIG_REQUEST  = 0x20,
    pt_CONFIG_RESPONSE = 0x21,
    pt_CONFIG_UPDATE   = 0x22,
    pt_USER_DATA       = 0x30,
    pt_UPDATE_STATUS   = 0x40,
    pt_DEBUG           = 0xff
} PacketType_t;

// Defines the header of a packet sent over the network
typedef struct PacketTLV {
    PacketType_t type:8;
    uint32_t length:24;
    uint32_t timestamp;
    PTLVData_Base_t* data;
} PacketTLV_t;

// Defines the possible states of the robot
typedef enum RobotState {
    rs_INITIALIZED = 0x00,
    rs_DISABLED    = 0x01,
    rs_ENABLED     = 0x02,
    rs_E_STOPPED   = 0xff
} RobotState_t;

// Defines the different key-value pair types
typedef enum KVPair_Type {
    kv_Integer = 0x00,
    kv_Float   = 0x01,
    kv_Double  = 0x02,
    kv_CString = 0x03,
    kv_Boolean = 0x04
} KVPair_Type_t;

// Defines a key-value pair TLV for network transmission
typedef struct KVPairTLV {
    char* key;
    KVPair_Type_t type:8;
    uint32_t length:24;
    void* value;
} KVPairTLV_t;

// Defines the "abstract" struct for data sent in a packet
typedef struct PTLVData_Base {
    // empty
} PTLVData_Base_t;

// Defines the INIT packet struct, which "extends" the PTLVData_Base struct
typedef struct PTLVData_INIT {
    uint32_t robot_uuid;
} PTLVData_INIT_t;

// Defines the STATE_REQUEST struct, which "extends" the PTLVData_Base struct
typedef struct PTLVData_STATE_REQUEST {
    // empty
} PTLVData_STATE_REQUEST_t;

// Defines the STATE_RESPONSE struct, which "extends" the PTLVData_Base struct
typedef struct PTLVData_STATE_RESPONSE {
    RobotState_t state:8;
    uint32_t reserved:24;
    void* arbitrary;
} PTLVData_STATE_RESPONSE_t;

// Defines the STATE_UPDATE struct, which "extends" the PTLVData_Base struct
typedef struct PTLVData_STATE_UPDATE {
    RobotState_t new_state:8;
    uint32_t reserved:24;
    void* arbitrary;
} PTLVData_STATE_UPDATE_t;

// Defines the CONFIG_REQUEST struct, which "extends" the PTLVData_Base struct
typedef struct PTLVData_CONFIG_REQUEST {
    List_t* keys;
} PTLVData_CONFIG_REQUEST_t;

// Defines the CONFIG_RESPONSE struct, which "extends" the PTLVData_Base struct
typedef struct PTLVData_CONFIG_RESPONSE {
    List_t* pairs;
} PTLVData_CONFIG_RESPONSE_t;

// Defines the CONFIG_UPDATE struct, which "extends" the PTLVData_Base struct
typedef struct PTLVData_CONFIG_UPDATE {
    List_t* new_pairs;
} PTLVData_CONFIG_UPDATE_t;

// Defines the USER_DATA struct, which "extends" the PTLVData_Base struct
typedef struct PTLVData_USER_DATA {
    uint8_t left_stick_x;
    uint8_t left_stick_y;
    uint8_t right_stick_x;
    uint8_t right_stick_y;
    bool button_a:1;
    bool button_b:1;
    uint16_t controller_uuid;
} PTLVData_USER_DATA_t;

// Defines the possible return codes for an UPDATE_STATUS packet
typedef enum UpdateStatusCode {
    sc_SUCCESS           = 0x00,
    sc_INVALID_STATE     = 0x01,
    sc_PERMISSION_DENIED = 0x02
} UpdateStatusCode_t;

// Defines the UPDATE_STATUS struct, which "extends" the PTLVData_Base struct
typedef struct PTLVData_UPDATE_STATUS {
    UpdateStatusCode_t code:8;
    uint32_t reserved:24;
} PTLVData_UPDATE_STATUS_t;

// Defines the possible code statuses in a DEBUG packet
typedef enum DebugCodeStatus {
    cs_UP_TO_DATE = 0x00,
    cs_AHEAD      = 0x01
} DebugCodeStatus_t;

// Defines the DEBUG struct, which "extends" the PTLVData_Base struct
typedef struct PTLVData_DEBUG {
    DebugCodeStatus_t code_status:4;
    uint32_t commit_hash:28;
    uint32_t robot_uuid;
    RobotState_t state:8;
    uint8_t reserved;
    uint16_t config_entries;
    void* arbitrary;
}

#ifdef __cplusplus
}
#endif

#endif