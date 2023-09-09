/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd All Rights Reserved
 * PROPRIETARY/CONFIDENTIAL
 *
 * This software is the confidential and proprietary information of
 * SAMSUNG ELECTRONICS ("Confidential Information").
 *
 * You shall not disclose such Confidential Information and shall use it
 * only in accordance with the terms of the license agreement
 * you entered into with SAMSUNG ELECTRONICS.
 *
 * SAMSUNG make no representations or warranties about the suitability of
 * the software, either express or implied, including but not limited to
 * the implied warranties of merchantability, fitness for a particular
 * purpose, or non-infringement. SAMSUNG shall not be liable for any
 * damages suffered by license as a result of using, modifying or
 * distributing this software or its derivatives.
 */

#include <hidl-log.h>
#include <hidl/1.6/hidlhalindication_network.h>
#include <hidl/hidlhalindication_network.h>
#include <librilutils/radio_hal_utils_1_6.h>
#include <librilutils/radio_conv_utils.h>

#include "ril_service_common.h"
#include "ril_service_legacy.h"

#define TAG "HidlHalNetworkIndication"

using ::android::hardware::hidl_string;
using ::android::hardware::Return;

int HidlHalNetworkIndication_1_6::currentLinkCapacityEstimate_1_6(
        int slotId, int indicationType, int token, RIL_Errno e, void *response,
        size_t responselen) {
#if VDBG
    RLOGD("%s", __FUNCTION__);
#endif
    if (mRadioIndicationV1_6 != nullptr) {
        if (response == NULL || responselen != sizeof(RIL_LinkCapacityEstimate_V1_6)) {
            RLOGE("%s: invalid response", __FUNCTION__);
            return 0;
        }
        V1_6::LinkCapacityEstimate lce = {};
        RIL_LinkCapacityEstimate_V1_6 *resp = (RIL_LinkCapacityEstimate_V1_6 *)response;
        lce.downlinkCapacityKbps = resp->downlinkCapacityKbps;
        lce.uplinkCapacityKbps = resp->uplinkCapacityKbps;
        lce.secondaryDownlinkCapacityKbps = resp->DCNRSecondaryDownlinkCapacityKbps;
        lce.secondaryUplinkCapacityKbps = resp->DCNRSecondaryUplinkCapacityKbps;
        Return<void> retStatus = mRadioIndicationV1_6->currentLinkCapacityEstimate_1_6(
                convertIntToRadioIndicationType(indicationType), lce);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioIndicationV1_6 == NULL", __FUNCTION__, slotId);
        if (response != NULL && responselen >= sizeof(RIL_LinkCapacityEstimate)) {
            RIL_LinkCapacityEstimate lce = {
                ((RIL_LinkCapacityEstimate_V1_6 *)response)->downlinkCapacityKbps,
                ((RIL_LinkCapacityEstimate_V1_6 *)response)->uplinkCapacityKbps,
            };
            return HidlHalNetworkIndication(*this).lceDataInd( slotId, indicationType, token, e,
                             &lce, sizeof(RIL_LinkCapacityEstimate));
        }
    }
    return 0;
}

int HidlHalNetworkIndication_1_6::cellInfoListInd_1_6(
        int slotId, int indicationType, int token, RIL_Errno e, void *response,
        size_t responselen) {
#if VDBG
    RLOGD("%s", __FUNCTION__);
#endif
    if (mRadioIndicationV1_6 != NULL) {
        if ((response == NULL && responselen != 0) || responselen % sizeof(RIL_CellInfo_V1_6) != 0) {
            RLOGE("%s: invalid response", __FUNCTION__);
            return 0;
        }
        hidl_vec<V1_6::CellInfo> records;
        Radio2HalUtils::convertRilCellInfoListToHal(response, responselen, records);
        Return<void> retStatus = mRadioIndicationV1_6->cellInfoList_1_6(
                convertIntToRadioIndicationType(indicationType), records);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioIndicationV1_6 == NULL", __FUNCTION__, slotId);
        if (response != NULL && responselen > 0
                && responselen % sizeof(RIL_CellInfo_V1_6) == 0) {
            int numOfCellInfo = responselen / sizeof(RIL_CellInfo_V1_6);
            RIL_CellInfo_V1_5 *cellInfoV1_5 = new RIL_CellInfo_V1_5[numOfCellInfo] {};
            for (int i = 0; i < numOfCellInfo; i++) {
                RadioConvUtils::convertCellInfo(cellInfoV1_5[i], ((RIL_CellInfo_V1_6 *)response)[i]);
            } // end for i ~
            int ret =  HidlHalNetworkIndication(*this).cellInfoListInd_1_5(slotId, indicationType, token, e,
                    cellInfoV1_5, sizeof(RIL_CellInfo_V1_5) * numOfCellInfo);
            delete[] cellInfoV1_5;
            return ret;
        }
    }
    return 0;
}

int HidlHalNetworkIndication_1_6::currentPhysicalChannelConfigs_1_6(
        int slotId, int indicationType, int token, RIL_Errno e, void *response,
        size_t responselen) {
#if VDBG
    RLOGD("%s", __FUNCTION__);
#endif
    if (mRadioIndicationV1_6 != NULL) {
        if ((response == NULL && responselen != 0) || responselen % sizeof(RIL_PhysicalChannelConfig_V1_6) != 0) {
            RLOGE("%s: invalid response", __FUNCTION__);
            return 0;
        }

        hidl_vec<V1_6::PhysicalChannelConfig> configs;
        int num = responselen / sizeof(RIL_PhysicalChannelConfig_V1_6);
        configs.resize(num);
        for (int i = 0 ; i < num; i++) {
            RIL_PhysicalChannelConfig_V1_6 *resp = &((RIL_PhysicalChannelConfig_V1_6 *)response)[i];
            Radio2HalUtils::convertRilPhysicalChannelConfigToHal(resp, sizeof(RIL_PhysicalChannelConfig_V1_6), configs[i]);
        }

        Return<void> retStatus = mRadioIndicationV1_6->currentPhysicalChannelConfigs_1_6(
                convertIntToRadioIndicationType(indicationType), configs);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioIndicationV1_6 == NULL", __FUNCTION__, slotId);
        // an empty list means RRC idle.
        // must not report if data and datalen is incorrect.
        if ((response == NULL && responselen != 0) || responselen % sizeof(RIL_PhysicalChannelConfig_V1_6) != 0) {
            RLOGE("%s: invalid response", __FUNCTION__);
            return 0;
        }
        RIL_PhysicalChannelConfig_V1_4 *configsV1_4 = NULL;
        int num = response ? responselen / sizeof(RIL_PhysicalChannelConfig_V1_6) : 0;
        if (num > 0) {
            configsV1_4 = new RIL_PhysicalChannelConfig_V1_4[num] {};
            for (int i = 0; i < num; i++) {
                RadioConvUtils::convertPhysicalChannelConfig(
                        configsV1_4[i], ((RIL_PhysicalChannelConfig_V1_6 *)response)[i]);
            }
        }
        HidlHalNetworkIndication(*this).
                currentPhysicalChannelConfigs_1_4(slotId, indicationType, token, e,
                        configsV1_4, sizeof(RIL_PhysicalChannelConfig_V1_4) * num);
        if (configsV1_4) {
            for (int i = 0; i < num; i++) {
                RadioConvUtils::freePhysicalChannelConfig(configsV1_4[i]);
            }
            delete[] configsV1_4;
        }
    }
    return 0;
}

int HidlHalNetworkIndication_1_6::currentSignalStrengthInd_1_6(
        int slotId, int indicationType, int token, RIL_Errno e, void *response,
        size_t responselen) {
#if VDBG
    RLOGD("%s", __FUNCTION__);
#endif
    if (mRadioIndicationV1_6 != NULL) {
        if (response == NULL || responselen != sizeof(RIL_SignalStrength_V1_6)) {
            RLOGE("%s: invalid response", __FUNCTION__);
            return 0;
        }
        V1_6::SignalStrength signalStrength = {};
        Radio2HalUtils::convertRilSignalStrengthToHal(response, responselen, signalStrength);
        Return<void> retStatus = mRadioIndicationV1_6->currentSignalStrength_1_6(
                convertIntToRadioIndicationType(indicationType), signalStrength);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioIndicationV1_6 == NULL", __FUNCTION__, slotId);
        if (response != NULL && responselen >= sizeof(RIL_SignalStrength_V1_4)) {
            RIL_SignalStrength_V1_4 signalStrength = {};
            RadioConvUtils::convertSignalStrengthResult(signalStrength, *((RIL_SignalStrength_V1_6 *)response));
            return HidlHalNetworkIndication(*this).currentSignalStrengthInd_1_4(slotId,
                    indicationType, token, e, &signalStrength, sizeof(RIL_SignalStrength_V1_4));
        }
    }
    return 0;
}

int HidlHalNetworkIndication_1_6::networkScanResultInd_1_6(
        int slotId, int indicationType, int token, RIL_Errno e, void *response,
        size_t responselen) {
#if VDBG
    RLOGD("%s", __FUNCTION__);
#endif
    if (mRadioIndicationV1_6 != NULL) {
        if (response == NULL || responselen == 0) {
            RLOGE("%s: invalid response", __FUNCTION__);
            return 0;
        }

        RIL_NetworkScanResult_V1_6 *networkScanResult = (RIL_NetworkScanResult_V1_6 *) response;

        V1_6::NetworkScanResult result;
        result.status = (V1_1::ScanStatus) networkScanResult->status;
        result.error = (V1_6::RadioError) networkScanResult->error;
        Radio2HalUtils::convertRilCellInfoListToHal(
                networkScanResult->network_infos,
                networkScanResult->network_infos_length * sizeof(RIL_CellInfo_V1_6),
                result.networkInfos);

        Return<void> retStatus = mRadioIndicationV1_6->networkScanResult_1_6(
                convertIntToRadioIndicationType(indicationType), result);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioIndicationV1_6 == NULL", __FUNCTION__, slotId);
        if (response != NULL && responselen > 0
                && responselen % sizeof(RIL_NetworkScanResult_V1_6) == 0) {
            RIL_NetworkScanResult_V1_6 *pscanResultV1_6 = (RIL_NetworkScanResult_V1_6 *) response;
            RIL_NetworkScanResult_V1_5 scanResultV1_5;
            scanResultV1_5.status = pscanResultV1_6->status;
            scanResultV1_5.network_infos_length = pscanResultV1_6->network_infos_length;
            scanResultV1_5.error= pscanResultV1_6->error;
            RIL_CellInfo_V1_5 *cellInfoV1_5List = new RIL_CellInfo_V1_5[scanResultV1_5.network_infos_length] {};
            for (uint32_t i = 0; i < scanResultV1_5.network_infos_length; i++) {
                RadioConvUtils::convertCellInfo(cellInfoV1_5List[i], pscanResultV1_6->network_infos[i]);
            } // end for i ~
            scanResultV1_5.network_infos = cellInfoV1_5List;
            int ret = HidlHalNetworkIndication(*this).networkScanResultInd_1_5(
                            slotId, indicationType, token, e,
                            &scanResultV1_5, sizeof(RIL_NetworkScanResult_V1_5));
            delete[] cellInfoV1_5List;
            return ret;
        }
    }
    return 0;
}

