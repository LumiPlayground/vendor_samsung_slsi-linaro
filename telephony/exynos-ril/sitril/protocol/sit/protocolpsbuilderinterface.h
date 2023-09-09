/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

/*
 * protocolpsbuilderinterface.h
 *
 *  Created on: 2021. 10. 18
 *      Author: juneho.kwon
 */

#ifndef __PROTOCOL_PS_BUILDER_INTERFACE_H__
#define __PROTOCOL_PS_BUILDER_INTERFACE_H__

#include "protocolbuilder.h"
#include <telephony/ril.h>
#include <slsi/radio_v1_4.h>
#include <slsi/radio_v1_5.h>
#include <slsi/radio_v1_6.h>
#include <slsi/radio_v2_0.h>

class ModemData;
class PdpContext;

namespace android::hardware::radio::impl::data::V2_0 {
    struct RIL_TrafficDescriptor;
}

/**
 * ProtocolCallBuilderInterface
 */
class ProtocolPsBuilderInterface
{
public:
    const static int REQ_IPC_VERSION_0 = 0; // 0:legacy
    const static int REQ_IPC_VERSION_1 = 1; // 1:PduSessionId, SliceInfo, RADIO HAL 1.6
    const static int REQ_IPC_VERSION_2 = 2; // 2:big set of TD, alwaysOnPdu, RADIO HAL 2.0
    const static int MAX_OSID_SIZE = 16;
    const static int MAX_OSAPPID_SIZE = 255;
    const static int VDBG = 0;
    const static uint32_t TD_BITMASK_IP_DESCRIPTORS = 0x1F0;

    enum FLAG_FOR_TRAFFIC_DESCRIPTOR {
        TD_FLAG_MATCHED_ALL = (1 << 0),
        TD_FLAG_DNN         = (1 << 1),
        TD_FLAG_OSAPPID     = (1 << 2),
        TD_FLAG_OSID        = (1 << 3),
        TD_FLAG_IPV4_REMOTE_ADDRESS         = (1 << 4),
        TD_FLAG_IPV6_REMOTE_ADDRESS         = (1 << 5), // INCLUDE PREFIX LENGTH
        TD_FLAG_PROTOCOL_IDENTIFIER         = (1 << 6),
        TD_FLAG_SINGLE_REMOTE_PORT          = (1 << 7),
        TD_FLAG_REMOTE_PORT_RANGE_LIMIT     = (1 << 8),
        TD_FLAG_SECURITY_PARAMETER_INDEX    = (1 << 9),
        TD_FLAG_TOS_TRAFFIC_CLASS           = (1 << 10),
        TD_FLAG_FLOW_LABEL                  = (1 << 11),
        TD_FLAG_DESTINATION_MAC_ADDRESS     = (1 << 12),
        TD_FLAG_CTAG_VID                    = (1 << 13),
        TD_FLAG_STAG_VID                    = (1 << 14),
        TD_FLAG_CTAG_PCP_DEI                = (1 << 15),
        TD_FLAG_STAG_PCP_DEI                = (1 << 16),
        TD_FLAG_ETHERTYPE                   = (1 << 17),
        TD_FLAG_CONNECTION_CAPABILITIES     = (1 << 18),
        TD_FLAG_DESTINATION_FQDN            = (1 << 19),
        TD_FLAG_IP3TUPLE                    = (1 << 20),
    };
    enum FLAG_FOR_IP3TUPLE {
        IP3TUPLE_BITMAP_IPV4            = (1 << 0),
        IP3TUPLE_BITMAP_IPV6            = (1 << 1),
        IP3TUPLE_BITMAP_PROTOCOLID      = (1 << 2),
        IP3TUPLE_BITMAP_SINGLEPORT      = (1 << 3),
        IP3TUPLE_BITMAP_PORTRANGE       = (1 << 4),
    };


public:
    ProtocolPsBuilderInterface() = default;
    virtual ~ProtocolPsBuilderInterface() = default;

public:
    virtual ModemData *BuildSetupDataCall(int rat, PdpContext *pPdpContext, int slotId, int reason, const char *addresses) = 0;
    virtual ModemData *BuildSetupDataCall(int rat, PdpContext *pPdpContext, int slotId, int reason, const char *addresses,
                           int pduSessionId, int optionalSliceInfoFlag, int optionalSliceInfoSst, int optionalSliceInfoSD,
                           int optionalSliceInfoMappedSst, int optionalSliceInfoMappedSD, const std::optional<DataV2_0::RIL_TrafficDescriptor> &tdInfo, bool matchedAllFlag, bool alwaysOnPdu) = 0;
    virtual ModemData *BuildSetInitialAttachApn(PdpContext *pPdpContext, bool isEsmFlagZero = false) = 0;
    virtual ModemData *BuildSetDataProfile(const RIL_DataProfileInfo_v15 *dpi, bool isVzw) = 0;
    virtual ModemData *BuildSetDataProfile(const RIL_DataProfileInfo_V1_4 *dpi, bool isVzw) = 0;
    virtual ModemData *BuildSetDataProfile(const RIL_DataProfileInfo_V1_5 *dpi) = 0;
    virtual ModemData *BuildSetDataProfile(const DataV2_0::RIL_DataProfileInfo *dpi) = 0;
    virtual ModemData *BuildDeactDataCall(int cid, int reason) = 0;
    virtual ModemData *BuildGetDataCallList() = 0;
    virtual ModemData *BuildSetFastDormancyInfo(BYTE lcdOn, BYTE lcdOff, BYTE rel8LcdOn, BYTE rel8LcdOff) = 0;
    virtual ModemData *BuildStartKeepAlive(RIL_KeepaliveRequest reqData) = 0;
    virtual ModemData *BuildStopKeepAlive(int sessionHande) = 0;
    virtual ModemData *BuildDetach(int detachReason) = 0;
    virtual ModemData *BuildSetPreferredDataModem(int stackId) = 0;
    virtual ModemData *BuildSetPdnIpAddress(RIL_SetPdnIpAddressRequest reqData) = 0;
    virtual ModemData *BuildAllocatePduSessionId() = 0;
    virtual ModemData *BuildReleasePduSessionId(int sessionId) = 0;
    virtual ModemData *BuildStartHandover(int callId) = 0;
    virtual ModemData *BuildCancelHandover(int callId) = 0;
    virtual ModemData *BuildSlicingConfig(int version) = 0;
    virtual void SetIpcVer(int ipcVer) =0;
    virtual int GetIpcVer() =0;
};
#endif /* __PROTOCOL_PS_BUILDER_INTERFACE_H__ */
