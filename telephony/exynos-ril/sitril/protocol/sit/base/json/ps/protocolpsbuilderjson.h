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
 * ProtocolPsBuilderJson.h
 *
 *  Created on: 2021. 10. 18
 *      Author: juneho.kwon
 */

#ifndef __PROTOCOL_PS_BUILDER_JSON_H__
#define __PROTOCOL_PS_BUILDER_JSON_H__

#include "protocolpsbuilderinterface.h"
#include "protocoljsonbuilder.h"
#include "protocolpsutil.h"

class PdpContext;

/**
 * ProtocolPsBuilderJson
 */
class ProtocolPsBuilderJson : public ProtocolJsonBuilder, public ProtocolPsBuilderInterface
{
protected:
   int mIpcVer = REQ_IPC_VERSION_0;

public:
    ProtocolPsBuilderJson()
        : ProtocolJsonBuilder(), ProtocolPsBuilderInterface() {}
    virtual ~ProtocolPsBuilderJson() = default;

public:
    template <typename T>
    int FillSetupDataCallRequest(T &req, int rat, PdpContext *pPdpContext, int reason, const char *addresses);
    template <typename T>
    void FillHandoverIpAddress(T &jsonReqData, int reason, const char *addresses);
    ModemData *BuildSetupDataCall(int rat, PdpContext *pPdpContext, int /*slotId*/, int reason, const char *addresses);
    ModemData *BuildSetupDataCall(int rat, PdpContext *pPdpContext, int /*slotId*/, int reason, const char *addresses,
                           int pduSessionId, int optionalSliceInfoFlag, int optionalSliceInfoSst, int optionalSliceInfoSD,
                           int optionalSliceInfoMappedSst, int optionalSliceInfoMappedSD, const std::optional<DataV2_0::RIL_TrafficDescriptor> &tdInfo, bool matchedAllFlag, bool alwaysOnPdu);
    ModemData *BuildSetInitialAttachApn(PdpContext *pPdpContext, bool isEsmFlagZero = false);
    ModemData *BuildSetDataProfile(const RIL_DataProfileInfo_v15 *dpi, bool isVzw);
    ModemData *BuildSetDataProfile(const RIL_DataProfileInfo_V1_4 *dpi, bool isVzw);
    ModemData *BuildSetDataProfile(const RIL_DataProfileInfo_V1_5 *dpi);
    ModemData *BuildSetDataProfile(const DataV2_0::RIL_DataProfileInfo *dpi);
    ModemData *BuildDeactDataCall(int cid, int reason);
    ModemData *BuildGetDataCallList();
    ModemData *BuildSetFastDormancyInfo(BYTE lcdOn, BYTE lcdOff, BYTE rel8LcdOn, BYTE rel8LcdOff);
    ModemData *BuildStartKeepAlive(RIL_KeepaliveRequest reqData);
    ModemData *BuildStopKeepAlive(int sessionHande);
    ModemData *BuildDetach(int detachReason);
    ModemData *BuildSetPreferredDataModem(int stackId);
    ModemData *BuildSetPdnIpAddress(RIL_SetPdnIpAddressRequest reqData);
    ModemData *BuildAllocatePduSessionId();
    ModemData *BuildReleasePduSessionId(int sessionId);
    ModemData *BuildStartHandover(int callId);
    ModemData *BuildCancelHandover(int callId);
    ModemData *BuildSlicingConfig(int version);
    void SetIpcVer(int version){ mIpcVer = version;};
    int GetIpcVer(){return mIpcVer;};

private:
    template <typename T>
    bool FillApnInfo(T &req, PdpContext *pPdpContext, bool isEsmFlagZero, bool forInitialAttach);
    template <typename T>
    bool FillDataProfileId(T &req, PdpContext *pPdpContext, bool forInitialAttach);
    void prepareSetDataProfileRequest(Json::Value &jsonData, const DataV2_0::RIL_DataProfileInfo *dpi);
    template <typename T>
    void prepareSetDataProfileRequest(Json::Value &jsonData, T *(&dpi));
    int checkOptSliceInfoType(int optionalSliceInfoSst, int optionalSliceInfoSD,
                      int optionalSliceInfoMappedSst, int optionalSliceInfoMappedSD);

};

#endif /* __PROTOCOL_PS_BUILDER_INTERFACE_H__ */

