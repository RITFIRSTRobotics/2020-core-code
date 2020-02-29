//
// Created by Alex Kneipp on 2/15/20.
//

#ifndef INC_2020_CORE_CODE_PACKETHANDLERS_H
#define INC_2020_CORE_CODE_PACKETHANDLERS_H

#include "packet.h"
#include "lowlevel.h"

PacketTLV_t* unpackInit(IntermediateTLV_t* rawPacket);
PacketTLV_t* unpackStateRequest(IntermediateTLV_t* rawPacket);
PacketTLV_t* unpackStateResponse(IntermediateTLV_t* rawPacket);
PacketTLV_t* unpackStateUpdate(IntermediateTLV_t* rawPacket);
PacketTLV_t* unpackConfigRequest(IntermediateTLV_t* rawPacket);
PacketTLV_t* unpackConfigResponse(IntermediateTLV_t* rawPacket);
PacketTLV_t* unpackConfigUpdate(IntermediateTLV_t* rawPacket);
PacketTLV_t* unpackUserData(IntermediateTLV_t* rawPacket);
PacketTLV_t* unpackDebug(IntermediateTLV_t* rawPacket);

#endif //INC_2020_CORE_CODE_PACKETHANDLERS_H
