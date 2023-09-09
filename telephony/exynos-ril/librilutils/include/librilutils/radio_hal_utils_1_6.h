/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef __RADIO_HAL_UTILS_1_6_H__
#define __RADIO_HAL_UTILS_1_6_H__

#include <android/hardware/radio/1.6/types.h>
#include <librilutils/radio_hal_utils.h>
#include <slsi/radio_v1_6.h>

#include <string>
#include <vector>

using android::hardware::hidl_string;
using android::hardware::hidl_vec;

namespace Radio2HalUtils {

    ///////////////////////////////////////////////////////////////////////////
    // radio 1.6  RIL => HAL convert function
    ///////////////////////////////////////////////////////////////////////////
    void convertRilPhysicalChannelConfigToHal(void *response, size_t responseLen, android::hardware::radio::V1_6::PhysicalChannelConfig& config);
    void convertVector(android::hardware::hidl_vec<hidl_string>&dst2, const std::vector<std::string>&src );
    void convertRilDataCallToHal(const RIL_SetupDataCallResult_V1_6 &dcResponse, ::android::hardware::radio::V1_6::SetupDataCallResult& dcResult);
    void convertLinkAddress(android::hardware::hidl_vec<::android::hardware::radio::V1_5::LinkAddress>&dst, const char *str);
    void parseString(android::hardware::hidl_vec<android::hardware::hidl_string>&dst, char *str);
    void convertRilDataCallListToHal(const void *response, size_t responseLen, hidl_vec<android::hardware::radio::V1_6::SetupDataCallResult>& dcResultList);
    bool convertRilCallToHal(RIL_Call_V1_6 *rilCall, size_t rilCallLen, android::hardware::radio::V1_6::Call& call);
    bool convertRilRegStateToHal(RIL_RegStateResult_V1_6 *from, size_t responselen, android::hardware::radio::V1_6::RegStateResult& to);
    void convertRilSignalStrengthToHal(void *response, size_t responseLen, android::hardware::radio::V1_6::SignalStrength& signalStrength);
    void convertRilSlicingConfigToHal(const RIL_SlicingConfig &rilSlicingConfig, android::hardware::radio::V1_6::SlicingConfig& slicingConfig);

    // Cell Info
    void convertRilCellInfoLteToHal(RIL_CellInfoLte_V1_6 *rilCellInfo, android::hardware::radio::V1_6::CellInfoLte& cellInfo);
    void convertRilCellInfoNrToHal(RIL_CellInfoNr_V1_6 *rilCellInfo, android::hardware::radio::V1_6::CellInfoNr& cellInfo);
    void convertRilCellInfoListToHal(void *response, size_t responseLen, hidl_vec<android::hardware::radio::V1_6::CellInfo>& records);
};
#endif  // __RADIO_HAL_UTILS_1_6_H__
