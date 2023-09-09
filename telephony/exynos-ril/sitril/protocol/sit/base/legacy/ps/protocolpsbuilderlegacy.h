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
 * protocolpsbuilderlegacy.h
 *
 *  Created on: 2014. 12. 2.
 *      Author: sungwoo48.choi
 */

#ifndef __PROTOCOL_PS_BUILDER_LEGACY_H__
#define __PROTOCOL_PS_BUILDER_LEGACY_H__

#include "protocolpsbuilderinterface.h"
#include "protocolbuilder.h"
#include "protocolpsutil.h"
#include "rildef.h"

class ModemData;
class PdpContext;

class ProtocolPsBuilderLegacy : public ProtocolBuilder, public ProtocolPsBuilderInterface
{
protected:
   int mIpcVer = REQ_IPC_VERSION_0;
public:
    ProtocolPsBuilderLegacy() : ProtocolBuilder(), ProtocolPsBuilderInterface() {}
    virtual ~ProtocolPsBuilderLegacy() = default;

public:
    template <typename T>
    int FillSetupDataCallRequest(T &req, int rat, PdpContext *pPdpContext, int slotId, int reason, const char *addresses);
    ModemData *BuildSetupDataCall(int rat, PdpContext *pPdpContext, int slotId, int reason, const char *addresses);
    template<typename T>
    int FillSetupDataCallIpcV3(T &req, int rat, PdpContext *pPdpContext, int slotId, int reason, const char *addresses,
                           int pduSessionId, int optionalSliceInfoFlag, int optionalSliceInfoSst, int optionalSliceInfoSD,
                           int optionalSliceInfoMappedSst, int optionalSliceInfoMappedSd,
                           const std::optional<DataV2_0::RIL_TrafficDescriptor> &tdInfo, bool matchedAllFlag);
    ModemData *BuildSetupDataCall(int rat, PdpContext *pPdpContext, int slotId, int reason, const char *addresses,
                           int pduSessionId, int optionalSliceInfoFlag, int optionalSliceInfoSst, int optionalSliceInfoSD,
                           int optionalSliceInfoMappedSst, int optionalSliceInfoMappedSD,
                           const std::optional<DataV2_0::RIL_TrafficDescriptor> &tdInfo, bool matchedAllFlag, bool alwaysOnPdu);
    void FillHandoverIpAddress(sit_pdp_setup_data_call_req &req, int slotId, int reason, const char *addresses);
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
    template <typename T>
    int prepareSetDataProfileRequest(sit_pdp_set_data_profile_req &req, T *(&dpi));
    int prepareSetDataProfileRequest(sit_pdp_set_data_profile_req &req, const DataV2_0::RIL_DataProfileInfo *dpi);
    int checkOptSliceInfoType(int optionalSliceInfoSst, int optionalSliceInfoSD,
                              int optionalSliceInfoMappedSst, int optionalSliceInfoMappedSD);
    int FillSliceInfo(sit_pdp_setup_data_call_v2_req &req, int pduSessionId, int optionalSliceInfoFlag,
                      int optionalSliceInfoSst, int optionalSliceInfoSD, int optionalSliceInfoMappedSst,
                      int optionalSliceInfoMappedSD, const DataV2_0::RIL_TrafficDescriptor &tdInfo,
                      bool matchedAllFlag);
    bool FillSliceInfoExt(sit_pdp_setup_data_call_v3_req &req, const DataV2_0::RIL_TrafficDescriptor &tdInfo);
    template<typename T>
        void FillIpDescriptor(T &ipdesc, const RIL_IpDescriptors& RilIpDesc);
    void FillIp3Tuple(sit_pdp_ip3tuple &ip3Tuple, const DataV2_0::RIL_TrafficDescriptor &tdInfo);
};

#endif /* __PROTOCOL_PS_BUILDER_LEGACY_H__  */
