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
 * radio_hal_utils_test_api.h
 *
 *  Created on: 2021. 8. 5.
 */

#ifndef __RADIO_HAL_UTILS_TEST_API_H__
#define __RADIO_HAL_UTILS_TEST_API_H__

#include <android/hardware/radio/1.5/types.h>
#include <slsi/radio_v1_5.h>

using namespace android::hardware::radio;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;

void testCellIdentityOperatorNames(V1_2::CellIdentityOperatorNames& operatorNames,
        RIL_CellIdentityOperatorNames& rilOperatorNames);

void testCellIdentityAdditionalPlmns(hidl_vec<hidl_string>& additionalPlmns,
        size_t leng_plmns, char **plmns);

void testCellIdentityGsm(V1_0::CellIdentityGsm& cellIdentityGsm,
        RIL_CellIdentityGsm_v12& rilCellIdentityGsm);

void testCellIdentityGsm(V1_2::CellIdentityGsm& cellIdentityGsm,
        RIL_CellIdentityGsm_V1_2& rilCellIdentityGsm);

void testCellIdentityGsm(V1_5::CellIdentityGsm& cellIdentityGsm,
        RIL_CellIdentityGsm_V1_5& rilCellIdentityGsm);

void testCellIdentityWcdma(V1_0::CellIdentityWcdma& cellIdentityWcdma,
        RIL_CellIdentityWcdma_v12& rilCellIdentityWcdma);

void testCellIdentityWcdma(V1_2::CellIdentityWcdma& cellIdentityWcdma,
        RIL_CellIdentityWcdma_V1_2& rilCellIdentityWcdma);

void testCellIdentityWcdma(V1_5::CellIdentityWcdma& cellIdentityWcdma,
        RIL_CellIdentityWcdma_V1_5& rilCellIdentityWcdma);

void testCellIdentityCdma(V1_0::CellIdentityCdma& cellIdentityCdma,
        RIL_CellIdentityCdma& rilCellIdentityCdma);

void testCellIdentityCdma(V1_2::CellIdentityCdma& cellIdentityCdma,
        RIL_CellIdentityCdma_V1_2& rilCellIdentityCdma);

void testCellIdentityLte(V1_0::CellIdentityLte& cellIdentityLte,
        RIL_CellIdentityLte_v12& rilCellIdentityLte);

void testCellIdentityLte(V1_2::CellIdentityLte& cellIdentityLte,
        RIL_CellIdentityLte_V1_2& rilCellIdentityLte);

void testCellIdentityLte(V1_5::CellIdentityLte& cellIdentityLte,
        RIL_CellIdentityLte_V1_5& rilCellIdentityLte);

void testCellIdentityTdscdma(V1_0::CellIdentityTdscdma& cellIdentityTdscdma,
        RIL_CellIdentityTdscdma& rilCellIdentityTdscdma);

void testCellIdentityTdscdma(V1_2::CellIdentityTdscdma& cellIdentityTdscdma,
        RIL_CellIdentityTdscdma_V1_2& rilCellIdentityTdscdma);

void testCellIdentityTdscdma(V1_5::CellIdentityTdscdma& cellIdentityTdscdma,
        RIL_CellIdentityTdscdma_V1_5& rilCellIdentityTdscdma);

void testCellIdentityNr(V1_4::CellIdentityNr& cellIdentityNr,
        RIL_CellIdentityNr_V1_4& rilCellIdentityNr);

void testCellIdentityNr(V1_5::CellIdentityNr& cellIdentityNr,
        RIL_CellIdentityNr_V1_5& rilCellIdentityNr);

void testGsmSignalStrength(V1_0::GsmSignalStrength& gw,
        RIL_GW_SignalStrength& GW_SignalStrength);

void testGsmSignalStrength(V1_0::GsmSignalStrength& gsm,
        RIL_GSM_SignalStrength_v12& signalStrengthGsm);

void testCdmaSignalStrength(V1_0::CdmaSignalStrength& cdma,
        RIL_CDMA_SignalStrength& CDMA_SignalStrength);

void testEvdoSignalStrength(V1_0::EvdoSignalStrength& evdo,
        RIL_EVDO_SignalStrength& EVDO_SignalStrength);

void testLteSignalStrength(V1_0::LteSignalStrength& lte,
        RIL_LTE_SignalStrength_v8& LTE_SignalStrength);

void testTdScdmaSignalStrength(V1_0::TdScdmaSignalStrength& tdScdma,
        RIL_TD_SCDMA_SignalStrength& TD_SCDMA_SignalStrength);

void testTdScdmaSignalStrength(V1_2::TdscdmaSignalStrength& tdscdma,
        RIL_TD_SCDMA_SignalStrength_V1_2& signalStrengthTdscdma);

void testWcdmaSignalStrength(V1_0::WcdmaSignalStrength& wcdma,
        RIL_SignalStrengthWcdma& signalStrengthWcdma);

void testWcdmaSignalStrength(V1_2::WcdmaSignalStrength& wcdma,
        RIL_WCDMA_SignalStrength_V1_2& signalStrengthWcdma);

void testNrSignalStrength(V1_4::NrSignalStrength& nr,
        RIL_NR_SignalStrength_V1_4& signalStrengthNr);

void testCellInfoGsm(V1_0::CellInfoGsm& gsm, RIL_CellInfoGsm_v12& rilCellInfoGsm);

void testCellInfoGsm(V1_2::CellInfoGsm& gsm, RIL_CellInfoGsm_V1_2& rilCellInfoGsm);

void testCellInfoGsm(V1_5::CellInfoGsm& gsm, RIL_CellInfoGsm_V1_5& rilCellInfoGsm);

void testCellInfoCdma(V1_0::CellInfoCdma& cdma, RIL_CellInfoCdma& rilCellInfoCdma);

void testCellInfoCdma(V1_2::CellInfoCdma& cdma, RIL_CellInfoCdma_V1_2& rilCellInfoCdma);

void testCellInfoLte(V1_0::CellInfoLte& lte, RIL_CellInfoLte_v12& rilCellInfoLte);

void testCellInfoLte(V1_2::CellInfoLte& lte, RIL_CellInfoLte_V1_2& rilCellInfoLte);

void testCellInfoLte(V1_4::CellInfoLte& lte, RIL_CellInfoLte_V1_4& rilCellInfoLte);

void testCellInfoLte(V1_5::CellInfoLte& lte, RIL_CellInfoLte_V1_5& rilCellInfoLte);

void testCellInfoWcdma(V1_0::CellInfoWcdma& wcdma, RIL_CellInfoWcdma_v12& rilCellInfoWcdma);

void testCellInfoWcdma(V1_2::CellInfoWcdma& wcdma, RIL_CellInfoWcdma_V1_2& rilCellInfoWcdma);

void testCellInfoWcdma(V1_5::CellInfoWcdma& wcdma, RIL_CellInfoWcdma_V1_5& rilCellInfoWcdma);

void testCellInfoTdscdma(V1_0::CellInfoTdscdma& tdscdma, RIL_CellInfoTdscdma& rilCellInfoTdscdma);

void testCellInfoTdscdma(V1_2::CellInfoTdscdma& tdscdma, RIL_CellInfoTdscdma_V1_2& rilCellInfoTdscdma);

void testCellInfoTdscdma(V1_5::CellInfoTdscdma& tdscdma, RIL_CellInfoTdscdma_V1_5& rilCellInfoTdscdma);

void testCellInfoNr(V1_4::CellInfoNr& nr, RIL_CellInfoNr_V1_4& rilCellInfoNr);

void testCellInfoNr(V1_5::CellInfoNr& nr, RIL_CellInfoNr_V1_5& rilCellInfoNr);

void testCellInfo(V1_0::CellInfo& cellInfo, RIL_CellInfo_v12& rilCellInfo);

void testCellInfo(V1_2::CellInfo& cellInfo, RIL_CellInfo_V1_2& rilCellInfo);

void testCellInfo(V1_4::CellInfo& cellInfo, RIL_CellInfo_V1_4& rilCellInfo);

void testCellInfo(V1_5::CellInfo& cellInfo, RIL_CellInfo_V1_5& rilCellInfo);

void testCellIdentityDiscriminator(V1_5::CellIdentity::hidl_discriminator descriptor,
        int rilCellInfoType);

#endif  // __RADIO_HAL_UTILS_TEST_API_H__
