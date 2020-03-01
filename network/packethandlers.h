//
// Created by Alex Kneipp on 2/15/20.
//

/**
 * @file packethandlers.h
 * Header file defining the functions used to unpack packets received from the low-level network interface.
 * Note that these should not be called by user code.  This file is primarily meant as an aggregation of functions
 * used by the messagehandler.
 */

#ifndef INC_2020_CORE_CODE_PACKETHANDLERS_H
#define INC_2020_CORE_CODE_PACKETHANDLERS_H

#include "packet.h"
#include "lowlevel.h"

/**
 * Handler function to unpack an INIT packet when received by the low-level network interface.
 * This function will always attempt to free \p rawPacket.
 * @param rawPacket
 *  The packet which was received by the low-level network interface.  It should have a \p type of pt_INIT.
 * @return
 *  A PacketTLV_t* with a \p data type of PTLVData_INIT_t.  If the function fails at any point it returns NULL.
 */
PacketTLV_t* unpackInit(IntermediateTLV_t* rawPacket);

/**
 * Handler function to unpack a STATE_REQUEST packet when received by the low-level network interface.
 * This function will always attempt to free \p rawPacket.  Consumers of STATE_REQUEST packets should not check
 * the \p data field of the return, since STATE_REQUEST packets contain no information beyond the header.  The data
 * field of the return contains NULL.
 * @param rawPacket
 *  The packet which was received by the low-level network interface.  It should have a \p type of pt_STATE_REQUEST.
 * @return
 *  A PacketTLV_t* containing the information contained in the packet header.
 *  If the function fails (Usually during memory allocation) it returns NULL.
 */
PacketTLV_t* unpackStateRequest(IntermediateTLV_t* rawPacket);

/**
 * Handler function to unpack a STATE_RESPONSE packet when received by the low-level network interface. This function
 * will always attempt to free \p rawPacket.
 * @param rawPacket
 *  The packet which was received by the low-level network interface.  It should have a \p type of pt_STATE_RESPONSE.
 * @return
 *  A PacketTLV_t* containing the information contained in the packet header.
 *  If the function fails (Usually during memory allocation) it returns NULL.
 *  The arbitrary data field is always a pointer to memory, which the consumer should handle or ignore.
 */
PacketTLV_t* unpackStateResponse(IntermediateTLV_t* rawPacket);

/**
 * Handler function to unpack a STATE_UPDATE packet when received by the low-level network interface. This function
 * will always attempt to free \p rawPacket.
 * @param rawPacket
 *  The packet which was received by the low-level network interface.  It should have a \p type of pt_STATE_UPDATE.
 * @return
 *  A PacketTLV_t* containing the information contained in the packet header.
 *  If the function fails (Usually during memory allocation) it returns NULL.
 *  The arbitrary data field is always a pointer to memory, which the consumer should handle or ignore.
 */
PacketTLV_t* unpackStateUpdate(IntermediateTLV_t* rawPacket);

/**
 * Handler function to unpack a CONFIG_REQUEST packet when received by the low-level network interface. This function
 * will always attempt to free \p rawPacket.
 * @param rawPacket
 *  The packet which was received by the low-level network interface.  It should have a \p type of pt_CONFIG_REQUEST.
 * @return
 *  A PacketTLV_t* containing the information contained in the packet header.
 *  If the function fails (Usually during memory allocation) it returns NULL.
 */
PacketTLV_t* unpackConfigRequest(IntermediateTLV_t* rawPacket);

/**
 * Handler function to unpack a CONFIG_RESPONSE packet when received by the low-level network interface. This function
 * will always attempt to free \p rawPacket.
 * @param rawPacket
 *  The packet which was received by the low-level network interface.  It should have a \p type of pt_CONFIG_RESPONSE.
 * @return
 *  A PacketTLV_t* containing the information contained in the packet header.
 *  If the function fails (Usually during memory allocation) it returns NULL.
 */
PacketTLV_t* unpackConfigResponse(IntermediateTLV_t* rawPacket);

/**
 * Handler function to unpack a CONFIG_UPDATE packet when received by the low-level network interface. This function
 * will always attempt to free \p rawPacket.
 * @param rawPacket
 *  The packet which was received by the low-level network interface.  It should have a \p type of pt_CONFIG_UPDATE.
 * @return
 *  A PacketTLV_t* containing the information contained in the packet header.
 *  If the function fails (Usually during memory allocation) it returns NULL.
 */
PacketTLV_t* unpackConfigUpdate(IntermediateTLV_t* rawPacket);

/**
 * Handler function to unpack a USER_DATA packet when received by the low-level network interface. This function
 * will always attempt to free \p rawPacket.
 * @param rawPacket
 *  The packet which was received by the low-level network interface.  It should have a \p type of pt_USER_DATA.
 * @return
 *  A PacketTLV_t* containing the information contained in the packet header.
 *  If the function fails (Usually during memory allocation) it returns NULL.
 */
PacketTLV_t* unpackUserData(IntermediateTLV_t* rawPacket);

/**
 * Handler function to unpack a DEBUG packet when received by the low-level network interface. This function
 * will always attempt to free \p rawPacket.
 * @param rawPacket
 *  The packet which was received by the low-level network interface.  It should have a \p type of pt_DEBUG.
 * @return
 *  A PacketTLV_t* containing the information contained in the packet header.
 *  If the function fails (Usually during memory allocation) it returns NULL.
 *  The arbitrary data field will always be a pointer to memory.  The consumer should handle or ignore the data as
 *  necessary.
 */
PacketTLV_t* unpackDebug(IntermediateTLV_t* rawPacket);


/**
 * Cleans up an INIT packet.
 * @param initPacket
 *  A packet returned by unpackInit()
 */
void destroyInit(PacketTLV_t* initPacket);

/**
 * Cleans up a STATE_REQUEST packet.
 * @param stateRequestPacket
 *  A packet returned by unpackStateRequest()
 */
void destroyStateRequest(PacketTLV_t* stateRequestPacket);

/**
 * Cleans up a STATE_RESPONSE packet.
 * @param stateResponsePacket
 *  A packet returned by unpackStateResponse()
 */
void destroyStateResponse(PacketTLV_t* stateResponsePacket);

/**
 * Cleans up a STATE_UPDATE packet
 * @param stateUpdatePacket
 *  A packet returned by unpackStateUpdate()
 */
void destroyStateUpdate(PacketTLV_t* stateUpdatePacket);

/**
 * Cleans up a CONFIG_REQUEST packet.
 * @param configRequestPacket
 *  A packet returned by unpackConfigRequest()
 */
void destroyConfigRequest(PacketTLV_t* configRequestPacket);

/**
 * Cleans up a CONFIG_RESPONSE packet.
 * @param configResponsePacket
 *  A packet returned by unpackConfigResponse()
 */
void destroyConfigResponse(PacketTLV_t* configResponsePacket);

/**
 * Cleans up a CONFIG_UPDATE packet.
 * @param configUpdatePacket
 *  A packet returned by unpackConfigUpdate()
 */
void destroyConfigUpdate(PacketTLV_t* configUpdatePacket);

/**
 * Cleans up a USER_DATA packet.
 * @param userDataPacket
 *  A packet returned by unpackUserData()
 */
void destroyUserData(PacketTLV_t* userDataPacket);

/**
 * Cleans up a DEBUG packet.
 * @param debugPacket
 *  A packet returned by unpackDebug()
 */
void destroyDebug(PacketTLV_t* debugPacket);

#endif //INC_2020_CORE_CODE_PACKETHANDLERS_H
