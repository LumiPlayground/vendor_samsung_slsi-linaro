/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef __RADIO_COV_UTILS_H__
#define __RADIO_COV_UTILS_H__

#include <telephony/ril.h>
#include <slsi/radio_v1_6.h>
#include <slsi/radio_v2_0.h>
#include <list>

#define NETWORK_TYPE_BITMAP_GSM     (RAF_GPRS | RAF_EDGE | RAF_GSM)
#define NETWORK_TYPE_BITMAP_CDMA    (RAF_IS95A | RAF_1xRTT)
#define NETWORK_TYPE_BITMAP_EVDO    (RAF_EVDO_0 | RAF_EVDO_A | RAF_EVDO_B | RAF_EHRPD)
#define NETWORK_TYPE_BITMAP_HS      (RAF_HSDPA | RAF_HSUPA | RAF_HSPA | RAF_HSPAP)
#define NETWORK_TYPE_BITMAP_WCDMA   (NETWORK_TYPE_BITMAP_HS | RAF_UMTS)
#define NETWORK_TYPE_BITMAP_LTE     (RAF_LTE | RAF_LTE_CA)
#define NETWORK_TYPE_BITMAP_NR      (RAF_NR)
#define NETWORK_TYPE_BITMAP_TDS_CDMA (RAF_TD_SCDMA)

#define NETWORK_TYPE_BITMAP_GSM_WCDMA           (NETWORK_TYPE_BITMAP_GSM | NETWORK_TYPE_BITMAP_WCDMA)
#define NETWORK_TYPE_BITMAP_GSM_ONLY            (NETWORK_TYPE_BITMAP_GSM)
#define NETWORK_TYPE_BITMAP_WCDMA_ONLY          (NETWORK_TYPE_BITMAP_WCDMA)
#define NETWORK_TYPE_BITMAP_GSM_WCDMA_AUTO      (NETWORK_TYPE_BITMAP_GSM | NETWORK_TYPE_BITMAP_WCDMA)
#define NETWORK_TYPE_BITMAP_CDMA_EVDO_AUTO      (NETWORK_TYPE_BITMAP_CDMA | NETWORK_TYPE_BITMAP_EVDO)
#define NETWORK_TYPE_BITMAP_CDMA_ONLY           (NETWORK_TYPE_BITMAP_CDMA)
#define NETWORK_TYPE_BITMAP_EVDO_ONLY           (NETWORK_TYPE_BITMAP_EVDO)
#define NETWORK_TYPE_BITMAP_GSM_WCDMA_CDMA_EVDO_AUTO    (NETWORK_TYPE_BITMAP_GSM_WCDMA_AUTO | NETWORK_TYPE_BITMAP_CDMA_EVDO_AUTO)
#define NETWORK_TYPE_BITMAP_LTE_CDMA_EVDO       (NETWORK_TYPE_BITMAP_LTE | NETWORK_TYPE_BITMAP_CDMA_EVDO_AUTO)
#define NETWORK_TYPE_BITMAP_LTE_GSM_WCDMA       (NETWORK_TYPE_BITMAP_LTE | NETWORK_TYPE_BITMAP_GSM_WCDMA_AUTO)
#define NETWORK_TYPE_BITMAP_LTE_CMDA_EVDO_GSM_WCDMA (NETWORK_TYPE_BITMAP_LTE_GSM_WCDMA | NETWORK_TYPE_BITMAP_LTE_CDMA_EVDO)
#define NETWORK_TYPE_BITMAP_LTE_ONLY            (NETWORK_TYPE_BITMAP_LTE)
#define NETWORK_TYPE_BITMAP_LTE_WCDMA           (NETWORK_TYPE_BITMAP_LTE | NETWORK_TYPE_BITMAP_WCDMA)
#define NETWORK_TYPE_BITMAP_TD_SCDMA_ONLY       (RAF_TD_SCDMA)
#define NETWORK_TYPE_BITMAP_TD_SCDMA_WCDMA      (NETWORK_TYPE_BITMAP_WCDMA | RAF_TD_SCDMA)
#define NETWORK_TYPE_BITMAP_TD_SCDMA_LTE        (NETWORK_TYPE_BITMAP_LTE | RAF_TD_SCDMA)
#define NETWORK_TYPE_BITMAP_TD_SCDMA_GSM        (NETWORK_TYPE_BITMAP_GSM | RAF_TD_SCDMA)
#define NETWORK_TYPE_BITMAP_TD_SCDMA_GSM_LTE    (NETWORK_TYPE_BITMAP_LTE | NETWORK_TYPE_BITMAP_GSM | RAF_TD_SCDMA)
#define NETWORK_TYPE_BITMAP_TD_SCDMA_GSM_WCDMA  (NETWORK_TYPE_BITMAP_GSM_WCDMA | RAF_TD_SCDMA)
#define NETWORK_TYPE_BITMAP_TD_SCDMA_WCDMA_LTE  (NETWORK_TYPE_BITMAP_LTE_WCDMA | RAF_TD_SCDMA)
#define NETWORK_TYPE_BITMAP_TD_SCDMA_GSM_WCDMA_LTE  (NETWORK_TYPE_BITMAP_LTE_GSM_WCDMA | RAF_TD_SCDMA)
#define NETWORK_TYPE_BITMAP_TD_SCDMA_GSM_WCDMA_CDMA_EVDO_AUTO   (NETWORK_TYPE_BITMAP_GSM_WCDMA_CDMA_EVDO_AUTO | RAF_TD_SCDMA)
#define NETWORK_TYPE_BITMAP_TD_SCDMA_LTE_CDMA_EVDO_GSM_WCDMA    (NETWORK_TYPE_BITMAP_LTE_CMDA_EVDO_GSM_WCDMA | RAF_TD_SCDMA)
#define NETWORK_TYPE_BITMAP_NR_ONLY                 (RAF_NR)
#define NETWORK_TYPE_BITMAP_NR_LTE_CDMA_EVDO_AUTO   (NETWORK_TYPE_BITMAP_LTE_CDMA_EVDO | RAF_NR)
#define NETWORK_TYPE_BITMAP_NR_LTE_CDMA_GSM_WCDMA   (NETWORK_TYPE_BITMAP_LTE_GSM_WCDMA | RAF_NR)
#define NETWORK_TYPE_BITMAP_NR_LTE_CMDA_EVDO_GSM_WCDMA  (NETWORK_TYPE_BITMAP_LTE_CMDA_EVDO_GSM_WCDMA | RAF_NR)
#define NETWORK_TYPE_BITMAP_NR_LTE                  (NETWORK_TYPE_BITMAP_LTE_ONLY | RAF_NR)
#define NETWORK_TYPE_BITMAP_NR_LTE_WCDMA            (NETWORK_TYPE_BITMAP_LTE_WCDMA | RAF_NR)
#define NETWORK_TYPE_BITMAP_NR_TD_SCDMA_LTE         (NETWORK_TYPE_BITMAP_TD_SCDMA_LTE | RAF_NR)
#define NETWORK_TYPE_BITMAP_NR_TD_SCDMA_LTE_GSM     (NETWORK_TYPE_BITMAP_TD_SCDMA_GSM_LTE | RAF_NR)
#define NETWORK_TYPE_BITMAP_NR_TD_SCDMA_WCDMA_LTE   (NETWORK_TYPE_BITMAP_TD_SCDMA_WCDMA_LTE | RAF_NR)
#define NETWORK_TYPE_BITMAP_NR_TD_SCDMA_GSM_WCDMA_LTE   (NETWORK_TYPE_BITMAP_TD_SCDMA_GSM_WCDMA_LTE | RAF_NR)
#define NETWORK_TYPE_BITMAP_NR_TD_SCDMA_LTE_CDMA_EVDO_GSM_WCDMA (NETWORK_TYPE_BITMAP_TD_SCDMA_LTE_CDMA_EVDO_GSM_WCDMA | RAF_NR)

using namespace std;

namespace RadioConvUtils {
    int getAdjustedRaf(int raf);
    int getNetworkTypeFromRaf(int raf);
    int getRafFromNetworkType(int networkType);
    void printSupportedNetworkTypeBitmap();

    int getCellInfoTypeRadioTechnology(int rat);
    int getRadioTechnologyToAccessNetworkType(int rat);
    bool isServiceTypeCfQuery(RIL_SsServiceType serType, RIL_SsRequestType reqType);

    // Cell Identity
    void convertCellIdentityGsm(RIL_CellIdentityGsm_v12& out, const RIL_CellIdentityGsm_V1_2& cellIdentityGsm);
    void convertCellIdentityCdma(RIL_CellIdentityCdma& out, const RIL_CellIdentityCdma_V1_2& cellIdentityCdma);
    void convertCellIdentityWcdma(RIL_CellIdentityWcdma_v12& out, const RIL_CellIdentityWcdma_V1_2& cellIdentityWcdma);
    void convertCellIdentityLte(RIL_CellIdentityLte_v12& out, const RIL_CellIdentityLte_V1_2& cellIdentityLte);
    void convertCellIdentityTdscdma(RIL_CellIdentityTdscdma& out, const RIL_CellIdentityTdscdma_V1_2& cellIdentityTdscdma);

    // 1.5
    void fillOperatorName(RIL_CellIdentityOperatorNames& to, const RIL_CellIdentityOperatorNames& from);
    void convertCellIdentity(RIL_CellIdentity_V1_2& out, const RIL_CellIdentity_V1_5& cellIdentity);
    void convertCellIdentity(RIL_CellIdentity_v16& out, const RIL_CellIdentity_V1_5& cellIdentity);
    void convertCellIdentity(RIL_CellIdentity_v16& out, const RIL_CellIdentity_V1_2& cellIdentity);
    void convertCellIdentityGsm(RIL_CellIdentityGsm_V1_2 &out, const RIL_CellIdentityGsm_V1_5 &cellId);
    void convertCellIdentityGsm(RIL_CellIdentityGsm_v12& out, const RIL_CellIdentityGsm_V1_5& cellIdentityGsm);
    void convertCellIdentityWcdma(RIL_CellIdentityWcdma_V1_2 &out, const RIL_CellIdentityWcdma_V1_5 &cellId);
    void convertCellIdentityWcdma(RIL_CellIdentityWcdma_v12& out, const RIL_CellIdentityWcdma_V1_5& cellIdentityWcdma);
    void convertCellIdentityTdscdma(RIL_CellIdentityTdscdma_V1_2 &out, const RIL_CellIdentityTdscdma_V1_5 &cellId);
    void convertCellIdentityTdscdma(RIL_CellIdentityTdscdma& out, const RIL_CellIdentityTdscdma_V1_5& cellIdentityTdscdma);
    void convertCellIdentityLte(RIL_CellIdentityLte_V1_2 &out, const RIL_CellIdentityLte_V1_5 &cellId);
    void convertCellIdentityLte(RIL_CellIdentityLte_v12& out, const RIL_CellIdentityLte_V1_5& cellIdentityLte);
    void convertCellIdentityNr(RIL_CellIdentityNr_V1_4 &out, const RIL_CellIdentityNr_V1_5 &cellId);

    // Reg result
    void convertDataRegistrationStateResult(
            RIL_RegStateResult& out, RIL_RegStateResult_V1_6& dataRegStateResult);
    void convertDataRegistrationStateResult(
            RIL_DataRegistrationStateResponse_V1_4& out, RIL_RegStateResult& dataRegStateResult);
    void convertDataRegistrationStateResult(
            RIL_DataRegistrationStateResponse_V1_2& out, RIL_DataRegistrationStateResponse_V1_4& dataRegStateResult);
    void convertDataRegistrationStateResult(
            RIL_DataRegistrationStateResponse& out, RIL_DataRegistrationStateResponse_V1_2& dataRegStateResult);
    void convertVoiceRegistrationStateResult(
            RIL_RegStateResult& out, RIL_RegStateResult_V1_6& voiceRegStateResult);
    void convertVoiceRegistrationStateResult(
            RIL_VoiceRegistrationStateResponse_V1_2& out, RIL_RegStateResult& voiceRegStateResult);
    void convertVoiceRegistrationStateResult(
            RIL_VoiceRegistrationStateResponse& out,
            RIL_VoiceRegistrationStateResponse_V1_2& voiceRegStateResult);

    // Signal strength
    void convertSignalStrengthResult(RIL_SignalStrength_v10& out, const RIL_SignalStrength_V1_2& rilSignalStrength);
    void convertSignalStrengthResult(RIL_SignalStrength_v10& out, const RIL_SignalStrength_V1_4& currentSignalStrength);
    void convertSignalStrengthResult(RIL_SignalStrength_V1_2& out, const RIL_SignalStrength_V1_4& currentSignalStrength);
    void convertSignalStrengthResult(RIL_SignalStrength_V1_4& out, const RIL_SignalStrength_V1_6& currentSignalStrength);

    // Cell Info
    void convertCellInfoGsm(RIL_CellInfoGsm_v12& out, const RIL_CellInfoGsm_V1_2& gsm);
    void convertCellInfoCdma(RIL_CellInfoCdma& out, const RIL_CellInfoCdma_V1_2& cdma);
    void convertCellInfoWcdma(RIL_CellInfoWcdma_v12& out, const RIL_CellInfoWcdma_V1_2& wcdma);
    void convertCellInfoTdscdma(RIL_CellInfoTdscdma& out, const RIL_CellInfoTdscdma_V1_2& tdscdma);
    void convertCellInfoLte(RIL_CellInfoLte_v12& out, const RIL_CellInfoLte_V1_2& lte);
    void convertCellInfoLte(RIL_CellInfoLte_v12& out, const RIL_CellInfoLte_V1_4& lte);
    void convertCellInfoLte(RIL_CellInfoLte_V1_2& out, const RIL_CellInfoLte_V1_4& lte);
    void convertCellInfo(RIL_CellInfo_v12 &out, const RIL_CellInfo_V1_2 &rillCellInfo);
    void convertCellInfo(RIL_CellInfo_v12& out, const RIL_CellInfo_V1_4& cellInfo);
    void convertCellInfo(RIL_CellInfo_V1_2& out, const RIL_CellInfo_V1_4& cellInfo);
    void convertCellInfo(RIL_CellInfo_V1_5& out, const RIL_CellInfo_V1_6& rillCellInfo);
    void convertCellInfoList(list<RIL_CellInfo_v12>& out, list<RIL_CellInfo_V1_4>& cellInfoList);
    void convertCellInfoList(list<RIL_CellInfo_V1_2>& out, list<RIL_CellInfo_V1_4>& cellInfoList);

    // 1.5
    void convertCellInfo(RIL_CellInfo_V1_4& out, const RIL_CellInfo_V1_5& rillCellInfo);

    // Physical channel config
    void convertPhysicalChannelConfig(RIL_PhysicalChannelConfig& out, const RIL_PhysicalChannelConfig_V1_4& config);
    void convertPhysicalChannelConfig(RIL_PhysicalChannelConfig_V1_4& out, const RIL_PhysicalChannelConfig_V1_6& config);
    void dupPhysicalChannelConfig(RIL_PhysicalChannelConfig_V1_4& out, const RIL_PhysicalChannelConfig_V1_4& config);
    void dupPhysicalChannelConfig(RIL_PhysicalChannelConfig_V1_6& out, const RIL_PhysicalChannelConfig_V1_6& config);
    void freePhysicalChannelConfig(RIL_PhysicalChannelConfig_V1_4& pcc);

    // Data Call list
    bool CopyDataProfileInfo(DataV2_0::RIL_DataProfileInfo &dest, const DataV2_0::RIL_DataProfileInfo &src);
    bool CopyDataProfileInfo(DataV2_0::RIL_DataProfileInfo &dest, const RIL_DataProfileInfo_V1_5 &src);
    bool CopyDataProfileInfo(DataV2_0::RIL_DataProfileInfo &dest, const RIL_DataProfileInfo_V1_4 &src);
    bool CopyDataProfileInfo(DataV2_0::RIL_DataProfileInfo &dest, const RIL_DataProfileInfo_v15 &src);

    bool CopyDataProfileInfo(RIL_DataProfileInfo_V1_5 &dest, const RIL_DataProfileInfo_V1_5 &src);
    bool CopyDataProfileInfo(RIL_DataProfileInfo_V1_5 &dest, const RIL_DataProfileInfo_V1_4 &src);
    bool CopyDataProfileInfo(RIL_DataProfileInfo_V1_5 &dest, const RIL_DataProfileInfo_v15 &src);
    bool CopyDataProfileInfo(RIL_DataProfileInfo_v15 &dest, const RIL_DataProfileInfo_v15 &src);
    bool CopyDataProfileInfo(RIL_DataProfileInfo_v15 &dest, const RIL_DataProfileInfo &src);
    bool CopyDataProfileInfo(RIL_DataProfileInfo_V1_5 &dest, const RIL_InitialAttachApn_v15 &src);
    void FreeDataProfileInfo(RIL_DataProfileInfo_V1_4 &dpi);
    void FreeDataProfileInfo(RIL_DataProfileInfo_V1_5 &dpi);
    void CopyLinkAddress(RIL_LinkAddress &dest, const RIL_LinkAddress &src);
    void CopyLinkAddress(RIL_LinkAddress &dest, const DataV2_0::RIL_LinkAddress &src);
    void CopyLinkAddress(DataV2_0::RIL_LinkAddress &dest, const RIL_LinkAddress &src);
    void FreeLinkAddress(RIL_LinkAddress &linkAddress);

    const char* setProtocolType(PdpProtocolType src);
    void convertSetupDataCallInfo(RIL_Data_Call_Response_v11* dst, RIL_SetupDataCallResult_V1_6* src);
    void convertDataCallInfo(RIL_Data_Call_Response_v11* dst, void* src, int num);
    void freeIPAddrInfo(RIL_Data_Call_Response_v11 &dcV11);
    void convertDnn(std::vector<std::string>&dest, std::vector<RIL_OptionalDNN>& src);
    void convertOSAppId(std::vector<DataV2_0::RIL_OSAppId> &dst, std::vector<RIL_OptionalOSAppId> &src);

    void convertSetupDataCallInfo(RIL_SetupDataCallInfo_V1_6& out, RIL_SetupDataCallInfo_V1_5& from);
    void convertSetupDataCallInfo(RIL_SetupDataCallInfo_V1_5& out, RIL_SetupDataCallInfo_V1_4& from);
    void convertDataCall(RIL_Data_Call_Response_v11 &out, RIL_Data_Call_Response_V1_4 &dataCall);
    void convertDataCallList(RIL_Data_Call_Response_v11 *outlist, RIL_Data_Call_Response_V1_4 *dataCallList, int num);

    //Network
    void convertRadioAccessSpecifiers(RIL_RadioAccessSpecifier_V1_5 &out, RIL_RadioAccessSpecifier &ras);
};
#endif
