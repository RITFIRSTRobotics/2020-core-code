//
// Created by Alex Kneipp on 2/15/20.
//

#ifndef INC_2020_CORE_CODE_PACKETHANDLERS_H
#define INC_2020_CORE_CODE_PACKETHANDLERS_H

#include "pakcet.h"

PTLVData_INIT* unpackInit(PacketTLV* rawPacket);
PTLVData_STATE_REQUEST* unpackStateRequest(PacketTLV* rawPacket);
PTLVData_STATE_RESPONSE* unpackStateResponse(PacketTLV* rawPacket);
PTLVData_STATE_UPDATE* unpackStateUpdate(PacketTLV* rawPacket);
PTLVData_CONFIG_REQUEST* unpackConfigRequest(PacketTLV* rawPacket);
PTLVData_CONFIG_RESPONSE* unpackConfigResponse(PacketTLV* rawPacket);
PTLVData_CONFIG_UPDATE* unpackConfigUpdate(PacketTLV* rawPacket);
PTLVData_USER_DATA* unpackUserData(PacketTLV* rawPacket);
PTLVData_DEBUG* unpackDebug(PacketTLV* rawPacket);

#endif //INC_2020_CORE_CODE_PACKETHANDLERS_H
