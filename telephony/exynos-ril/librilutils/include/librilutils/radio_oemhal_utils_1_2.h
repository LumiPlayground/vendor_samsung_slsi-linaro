/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef RADIO_OEMHAL_UTILS_1_2_H
#define RADIO_OEMHAL_UTILS_1_2_H

#include <android/hardware/radio/1.5/types.h>
#include <librilutils/radio_hal_utils.h>
#include <slsi/radio_v1_6.h>

#include <vendor/samsung_slsi/telephony/hardware/radio/1.2/IOemSamsungslsi.h>
#include <vendor/samsung_slsi/telephony/hardware/radio/1.2/IOemSamsungslsiResponse.h>
#include <vendor/samsung_slsi/telephony/hardware/radio/1.2/IOemSamsungslsiIndication.h>

#include <string>
#include <vector>

namespace Radio2HalUtils {

    ///////////////////////////////////////////////////////////////////////////
    // radio 1.6(Oem HAL 1.2)  RIL => HAL convert function
    ///////////////////////////////////////////////////////////////////////////
    void convertVector(android::hardware::hidl_vec<hidl_string>&dst2, std::vector<std::string>& src);
    void convertLinkAddress(android::hardware::hidl_vec<::android::hardware::radio::V1_5::LinkAddress>&dst, char *str);
    void convertRilDataCallToHal(RIL_SetupDataCallResult_V1_6 *dcResponse, ::vendor::samsung_slsi::telephony::hardware::radio::V1_2::SetupDataCallResult& dcResult);
    void convertRilDataCallToHal(RIL_SetupDataCallResult_V1_6 *dcResponse, V1_5::SetupDataCallResult& dcResult);
    void convertRilDataCallListToHal(void *response, size_t responseLen, hidl_vec<vendor::samsung_slsi::telephony::hardware::radio::V1_2::SetupDataCallResult>& dcResultList);
    void convertRilSlicingConfigToHal(void * response, vendor::samsung_slsi::telephony::hardware::radio::V1_2::SlicingConfig& slicingConfig);
};
#endif  // RADIO_OEMHAL_UTILS_1_2_H

