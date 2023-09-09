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
 * protocolpsbuilder.cpp
 *
 *  Created on: 2021. 10. 18.
 *      Author: juneho.kwon
 */
#include "protocolpsbuilder.h"
#include "protocolpsbuilderlegacy.h"
#include "protocolpsbuilderjson.h"

ProtocolPsBuilder::ProtocolPsBuilder(): protocolPsBuilderInterface() {
    if (IsPayloadTypeJson()) {
        protocolPsBuilderInterface = new ProtocolPsBuilderJson();
    } else {
        protocolPsBuilderInterface = new ProtocolPsBuilderLegacy();
    }
}

ProtocolPsBuilder::~ProtocolPsBuilder()
{
    delete protocolPsBuilderInterface;
    protocolPsBuilderInterface = NULL;
}

// Cover under HAL Radio Interface 1.5
ModemData *ProtocolPsBuilder::BuildSetupDataCall(int rat, PdpContext *pPdpContext, int slotId, int reason, const char *addresses)
{
    return protocolPsBuilderInterface->BuildSetupDataCall(rat, pPdpContext, slotId, reason, addresses);
}

ModemData *ProtocolPsBuilder::BuildSetupDataCall(int rat, PdpContext *pPdpContext, int slotId, int reason, const char *addresses,
                           int pduSessionId, int optionalSliceInfoFlag, int optionalSliceInfoSst, int optionalSliceInfoSD,
                           int optionalSliceInfoMappedSst, int optionalSliceInfoMappedSD,
                           const std::optional<DataV2_0::RIL_TrafficDescriptor> &tdInfo, bool matchedAllFlag, bool alwaysOnPdu) {
    return protocolPsBuilderInterface->BuildSetupDataCall(rat, pPdpContext, slotId, reason, addresses, pduSessionId, optionalSliceInfoFlag,
                            optionalSliceInfoSst, optionalSliceInfoSD, optionalSliceInfoMappedSst, optionalSliceInfoMappedSD, tdInfo, matchedAllFlag, alwaysOnPdu);
}

ModemData *ProtocolPsBuilder::BuildSetInitialAttachApn(PdpContext *pPdpContext, bool isEsmFlagZero)
{
    return protocolPsBuilderInterface->BuildSetInitialAttachApn(pPdpContext, isEsmFlagZero);
}

ModemData *ProtocolPsBuilder::BuildSetDataProfile(const RIL_DataProfileInfo_v15 *dpi, bool isVzw)
{
    return protocolPsBuilderInterface->BuildSetDataProfile(dpi, isVzw);
}

ModemData *ProtocolPsBuilder::BuildSetDataProfile(const RIL_DataProfileInfo_V1_4 *dpi, bool isVzw)
{
    return protocolPsBuilderInterface->BuildSetDataProfile(dpi, isVzw);
}

ModemData *ProtocolPsBuilder::BuildSetDataProfile(const RIL_DataProfileInfo_V1_5 *dpi)
{
    return protocolPsBuilderInterface->BuildSetDataProfile(dpi);
}

ModemData *ProtocolPsBuilder::BuildSetDataProfile(const DataV2_0::RIL_DataProfileInfo *dpi)
{
    return protocolPsBuilderInterface->BuildSetDataProfile(dpi);
}

ModemData *ProtocolPsBuilder::BuildDeactDataCall(int cid, int reason)
{
    return protocolPsBuilderInterface->BuildDeactDataCall(cid, reason);
}

ModemData *ProtocolPsBuilder::BuildGetDataCallList()
{
    return protocolPsBuilderInterface->BuildGetDataCallList();
}

ModemData *ProtocolPsBuilder::BuildSetFastDormancyInfo(BYTE lcdOn, BYTE lcdOff, BYTE rel8LcdOn, BYTE rel8LcdOff)
{
    return protocolPsBuilderInterface->BuildSetFastDormancyInfo(lcdOn, lcdOff, rel8LcdOn, rel8LcdOff);
}

ModemData *ProtocolPsBuilder::BuildStartKeepAlive(RIL_KeepaliveRequest reqData)
{
    return protocolPsBuilderInterface->BuildStartKeepAlive(reqData);
}

ModemData *ProtocolPsBuilder::BuildStopKeepAlive(int sessionHande)
{
    return protocolPsBuilderInterface->BuildStopKeepAlive(sessionHande);
}

ModemData *ProtocolPsBuilder::BuildDetach(int detachReason)
{
    return protocolPsBuilderInterface->BuildDetach(detachReason);
}

ModemData *ProtocolPsBuilder::BuildSetPreferredDataModem(int stackId)
{
    return protocolPsBuilderInterface->BuildSetPreferredDataModem(stackId);
}

ModemData *ProtocolPsBuilder::BuildSetPdnIpAddress(RIL_SetPdnIpAddressRequest reqData)
{
    return protocolPsBuilderInterface->BuildSetPdnIpAddress(reqData);
}

ModemData *ProtocolPsBuilder::BuildAllocatePduSessionId()
{
    return protocolPsBuilderInterface->BuildAllocatePduSessionId();
}

ModemData *ProtocolPsBuilder::BuildReleasePduSessionId(int sessionId)
{
    return protocolPsBuilderInterface->BuildReleasePduSessionId(sessionId);
}

ModemData *ProtocolPsBuilder::BuildStartHandover(int callId)
{
    return protocolPsBuilderInterface->BuildStartHandover(callId);
}

ModemData *ProtocolPsBuilder::BuildCancelHandover(int callId)
{
    return protocolPsBuilderInterface->BuildCancelHandover(callId);
}

ModemData *ProtocolPsBuilder::BuildSlicingConfig(int version)
{
    return protocolPsBuilderInterface->BuildSlicingConfig(version);
}

void ProtocolPsBuilder::SetIpcVer(int version)
{
    return protocolPsBuilderInterface->SetIpcVer(version);
}

int ProtocolPsBuilder::GetIpcVer()
{
    return protocolPsBuilderInterface->GetIpcVer();
}
