/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef __RADIO_HAL_UTILS_H__
#define __RADIO_HAL_UTILS_H__

#include <android/hardware/radio/1.5/IRadio.h>
#include <android/hardware/radio/1.5/IRadioResponse.h>
#include <android/hardware/radio/1.5/IRadioIndication.h>
#include <android/hardware/radio/1.5/types.h>

#include <slsi/radio_v1_5.h>

#include <string>

using namespace std;
using namespace android::hardware::radio;
using namespace android::hardware::radio::V1_0;
using ::android::hardware::hidl_vec;
using ::android::hardware::hidl_string;

namespace Radio2HalUtils {
   /* RIL structure => HAL */
    ///////////////////////////////////////////////////////////////////////////
    // Common convert function
    ///////////////////////////////////////////////////////////////////////////
    hidl_string convertCharPtrToHidlString(const char *ptr);
    void parseStrings(std::vector<std::string>&dst1, hidl_vec<hidl_string>&dst2, char *str);
    int convertResponseStringEntryToInt(char **response, int index, int numStrings);
    int convertResponseHexStringEntryToInt(char **response, int index, int numStrings);

    ///////////////////////////////////////////////////////////////////////////
    // radio 1.4  RIL => HAL convert function
    ///////////////////////////////////////////////////////////////////////////
    V1_4::PdpProtocolType getPdpType(const char *protocol);

    int convertOperatorStatusToInt(const char *str);

    ///////////////////////////////////////////////////////////////////////////
    // radio 1.0  RIL => HAL convert function
    ///////////////////////////////////////////////////////////////////////////
    bool convertRilCardStatusToHal(RIL_CardStatus_v6 *cardStatus, size_t cardStatusLen, CardStatus& out);
    bool convertRilCallToHal(RIL_Call *rilCall, size_t rilCallLen, Call& call);
    void fillCellIdentityResponse(CellIdentity &cellIdentity, RIL_CellIdentity_v16 &rilCellIdentity);
    void convertRilSignalStrengthToHal(void *response, size_t responseLen, SignalStrength& signalStrength);
    void convertRilCellInfoListToHal(void *response, size_t responseLen, hidl_vec<CellInfo>& records);
    void convertRilKeepaliveStatusToHal(const RIL_KeepaliveStatus *rilStatus, V1_1::KeepaliveStatus& halStatus);
    void convertRilDataCallToHal(RIL_Data_Call_Response_v11 *dcResponse, SetupDataCallResult& dcResult);
    void convertRilDataCallListToHal(void *response, size_t responseLen, hidl_vec<SetupDataCallResult>& dcResultList);
    void convertRilCdmaSignalInfoRecordToHal(RIL_CDMA_SignalInfoRecord *signalInfoRecord, CdmaSignalInfoRecord& record);
    void convertRilHardwareConfigListToHal(void *response, size_t responseLen, hidl_vec<HardwareConfig>& records);
    void convertRilRadioCapabilityToHal(void *response, size_t responseLen, RadioCapability& rc);
    void convertRilLceDataInfoToHal(void *response, size_t responseLen, LceDataInfo& lce);

    ///////////////////////////////////////////////////////////////////////////
    // radio 1.2  RIL => HAL convert function
    ///////////////////////////////////////////////////////////////////////////
    bool convertRilCardStatusToHal(RIL_CardStatus_V1_2 *cardStatus, size_t cardStatusLen, V1_2::CardStatus& out);
    bool convertRilCallToHal(RIL_Call_V1_2 *rilCall, size_t rilCallLen, V1_2::Call& call);
    void fillCellIdentityResponse(V1_2::CellIdentity &cellIdentity, RIL_CellIdentity_V1_2 &rilCellIdentity);
    bool convertRilDataRegStateToHal(RIL_DataRegistrationStateResponse_V1_2 *dataRegState, size_t responselen,
                                        V1_2::DataRegStateResult& dataRegResponse);
    void convertRilSignalStrengthToHal(void *response, size_t responseLen, V1_2::SignalStrength& signalStrength);
    void convertRilCellInfoGsmToHal(RIL_CellInfoGsm_V1_2 *rillCellInfo, V1_2::CellInfoGsm& cellInfo);
    void convertRilCellInfoWcdmaToHal(RIL_CellInfoWcdma_V1_2 *rillCellInfo, V1_2::CellInfoWcdma& cellInfo);
    void convertRilCellInfoCdmaToHal(RIL_CellInfoCdma_V1_2 *rillCellInfo, V1_2::CellInfoCdma& cellInfo);
    void convertRilCellInfoLteToHal(RIL_CellInfoLte_V1_2 *rillCellInfo, V1_2::CellInfoLte& cellInfo);
    void convertRilCellInfoTdscdmaToHal(RIL_CellInfoTdscdma_V1_2 *rillCellInfo, V1_2::CellInfoTdscdma& cellInfo);
    void convertRilCellInfoListToHal(void *response, size_t responseLen, hidl_vec<V1_2::CellInfo>& records);
    void convertRilPhysicalChannelConfigToHal(void *response, size_t responseLen, V1_2::PhysicalChannelConfig& config);

    ///////////////////////////////////////////////////////////////////////////
    // radio 1.4  RIL => HAL convert function
    ///////////////////////////////////////////////////////////////////////////
    bool convertRilCardStatusToHal(RIL_CardStatus_V1_4 *cardStatus, size_t cardStatusLen, V1_4::CardStatus& out);
    bool convertRilDataRegStateToHal(RIL_DataRegistrationStateResponse_V1_4 *dataRegState, size_t responselen,
                                        V1_4::DataRegStateResult& dataRegResponse);
    void convertRilDataCallToHal(RIL_Data_Call_Response_v11 *dcResponse, V1_4::SetupDataCallResult& dcResult, std::vector<std::vector<std::string>> &tmp, int idx);
    //void convertRilDataCallToHal(RIL_Data_Call_Response_V1_4 *dcResponse, V1_4::SetupDataCallResult& dcResult);
    void convertRilDataCallListToHal(void *response, size_t responseLen, hidl_vec<V1_4::SetupDataCallResult>& dcResultList, std::vector<std::vector<std::string>>& t);
    void convertRilSignalStrengthToHal(void *response, size_t responseLen, V1_4::SignalStrength& signalStrength);
    void convertRilCellInfoLteToHal(RIL_CellInfoLte_V1_4 *rillCellInfo, V1_4::CellInfoLte& cellInfo);
    void convertRilCellInfoNrToHal(RIL_CellInfoNr_V1_4 *rillCellInfo, V1_4::CellInfoNr& cellInfo);
    void convertRilCellInfoListToHal(void *response, size_t responseLen, hidl_vec<V1_4::CellInfo>& records);
    void convertRilPhysicalChannelConfigToHal(void *response, size_t responseLen, V1_4::PhysicalChannelConfig& config);
    void convertRilEmergencyNumberToHal(void *response, size_t responselen, V1_4::EmergencyNumber& emergencyNumber);
    void convertRilEmergencyNumberListToHal(void *response, size_t responselen, hidl_vec<V1_4::EmergencyNumber>& emergencyNumberList);

    ///////////////////////////////////////////////////////////////////////////
    // radio 1.5  RIL => HAL convert function
    ///////////////////////////////////////////////////////////////////////////
    void convertRilDataCallListToHal(void *response, size_t responseLen, hidl_vec<V1_5::SetupDataCallResult>& dcResultList, std::vector<std::vector<std::string>>& t);
    void convertRilBarringInfoToHal(V1_5::BarringInfo& to, RIL_BarringInfo& from);
    void convertRilBarringInfosToHal(hidl_vec<V1_5::BarringInfo>& halRecords, RIL_BarringInfo *prilBarringInfos, size_t numOfInfos);
    void convertRilCsgInfoToHal(V1_5::ClosedSubscriberGroupInfo& out, const RIL_ClosedSubscriberGroupInfo& csg);
    void convertRilOptionalCsgInfoToHal(V1_5::OptionalCsgInfo& out, const RIL_OptionalCsgInfo& csg);
    bool convertRilRegStateToHal(RIL_RegStateResult *from, size_t responselen, V1_5::RegStateResult& to);

    void convertRilCellIdentityToHal(V1_5::CellIdentity& out, RIL_CellIdentity_V1_5& rilCellIdentity);
    // GSM cell Identity
    void convertRilCellIdentityGsmToHal(V1_0::CellIdentityGsm& out, RIL_CellIdentityGsm_V1_5& rilCellId);
    void convertRilCellIdentityGsmToHal(V1_2::CellIdentityGsm& out, RIL_CellIdentityGsm_V1_5& rilCellId);
    void convertRilCellIdentityGsmToHal(V1_5::CellIdentityGsm& out, RIL_CellIdentityGsm_V1_5& rilCellId);
    // WCDMA cell Identity
    void convertRilCellIdentityWcdmaToHal(V1_0::CellIdentityWcdma& out, RIL_CellIdentityWcdma_V1_5& rilCellId);
    void convertRilCellIdentityWcdmaToHal(V1_2::CellIdentityWcdma& out, RIL_CellIdentityWcdma_V1_5& rilCellId);
    void convertRilCellIdentityWcdmaToHal(V1_5::CellIdentityWcdma& out, RIL_CellIdentityWcdma_V1_5& rilCellId);
    // TDSCDMA cell Identity
    void convertRilCellIdentityTdscdmaToHal(V1_0::CellIdentityTdscdma& out, RIL_CellIdentityTdscdma_V1_5& rilCellId);
    void convertRilCellIdentityTdscdmaToHal(V1_2::CellIdentityTdscdma& out, RIL_CellIdentityTdscdma_V1_5& rilCellId);
    void convertRilCellIdentityTdscdmaToHal(V1_5::CellIdentityTdscdma& out, RIL_CellIdentityTdscdma_V1_5& rilCellId);
    // CDMA cell Identity
    void convertRilCellIdentityCdmaToHal(V1_0::CellIdentityCdma& out, RIL_CellIdentityCdma_V1_2& rilCellId);
    void convertRilCellIdentityCdmaToHal(V1_2::CellIdentityCdma& out, RIL_CellIdentityCdma_V1_2& rilCellId);
     // LTE cell Identity
    void convertRilCellIdentityLteToHal(V1_0::CellIdentityLte& out, RIL_CellIdentityLte_V1_5& rilCellId);
    void convertRilCellIdentityLteToHal(V1_2::CellIdentityLte& out, RIL_CellIdentityLte_V1_5& rilCellId);
    void convertRilCellIdentityLteToHal(V1_5::CellIdentityLte& out, RIL_CellIdentityLte_V1_5& rilCellId);
    // NR cell Identity
    void convertRilCellIdentityNrToHal(V1_4::CellIdentityNr& out, RIL_CellIdentityNr_V1_5& rilCellId);
    void convertRilCellIdentityNrToHal(V1_5::CellIdentityNr& out, RIL_CellIdentityNr_V1_5& rilCellId);

    // Cell Info
    void convertRilCellInfoGsmToHal(RIL_CellInfoGsm_V1_5 *rillCellInfo, V1_5::CellInfoGsm& cellInfo);
    void convertRilCellInfoWcdmaToHal(RIL_CellInfoWcdma_V1_5 *rillCellInfo, V1_5::CellInfoWcdma& cellInfo);
    void convertRilCellInfoLteToHal(RIL_CellInfoLte_V1_5 *rillCellInfo, V1_5::CellInfoLte& cellInfo);
    void convertRilCellInfoTdscdmaToHal(RIL_CellInfoTdscdma_V1_5 *rillCellInfo, V1_5::CellInfoTdscdma& cellInfo);
    void convertRilCellInfoNrToHal(RIL_CellInfoNr_V1_5 *rillCellInfo, V1_5::CellInfoNr& cellInfo);
    void convertRilCellInfoListToHal(void *response, size_t responseLen, hidl_vec<V1_5::CellInfo>& records);

    //Data
    void convertRilDataCallToHal(RIL_Data_Call_Response_v11 *dcResponse,
            V1_5::SetupDataCallResult& dcResult, std::vector<std::vector<std::string>> &tmp, int idx);
    void convertLinkAddress(std::vector<std::string>&container, hidl_vec<V1_5::LinkAddress>&dst, char *str);

    // radio 1.5  RIL_RadioAccessSpecifier_V1_5 => HAL 1.5 RadioAccessSpecifier
    bool convertRilRadioAccessSpecifierToHal(void *response, size_t responseLen, V1_5::RadioAccessSpecifier& ras);

    ///////////////////////////////////////////////////////////////////////////
    // radio 1.6  RIL => HAL convert function
    ///////////////////////////////////////////////////////////////////////////
    // SimPB
    int convertStringToAdnString(const std::string &data, unsigned char *pDst, int dstLen);
};
#endif
