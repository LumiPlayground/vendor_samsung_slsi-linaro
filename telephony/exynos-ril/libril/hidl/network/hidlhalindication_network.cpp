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
#include <hidl/hidlhalindication_network.h>
#include <librilutils/radio_hal_utils.h>
#include <librilutils/radio_conv_utils.h>
#include "ril_service_common.h"
#include "ril_service_legacy.h"

#define TAG "HidlHalNetworkIndication"

#if (SIM_COUNT >= 2)
int64_t nitzTimeReceived[SIM_COUNT];
#else
int64_t nitzTimeReceived[1];
#endif

using ::android::hardware::Return;
using ::android::hardware::hidl_string;

int HidlHalNetworkIndication::lceDataInd(int slotId, int indicationType, int token,
            RIL_Errno e, void *response, size_t responselen) {
#if VDBG
    RLOGD("%s::%s", TAG, __FUNCTION__);
#endif
    if (mRadioIndicationV1_2 != nullptr) {
        if (response == NULL || responselen != sizeof(RIL_LinkCapacityEstimate)) {
            RLOGE("%s: invalid response", __FUNCTION__);
            return 0;
        }
        V1_2::LinkCapacityEstimate lce = {};
        RIL_LinkCapacityEstimate *resp = (RIL_LinkCapacityEstimate *)response;
        lce.downlinkCapacityKbps = resp->downlinkCapacityKbps;
        lce.uplinkCapacityKbps = resp->uplinkCapacityKbps;
        Return<void> retStatus = mRadioIndicationV1_2->currentLinkCapacityEstimate(
                convertIntToRadioIndicationType(indicationType), lce);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioIndicationV1_2 == NULL", __FUNCTION__, slotId);
    }
    return 0;
}

int HidlHalNetworkIndication::barringInfoChangedInd(int slotId,
                         int indicationType, int token, RIL_Errno e, void *response,
                         size_t responselen) {
#if VDBG
    RLOGD("%s::%s", TAG, __FUNCTION__);
#endif
    if (mRadioIndicationV1_5 != NULL) {
        if((response == NULL || responselen == 0)){
            RLOGE("%s: invalid response:", __FUNCTION__);
            return 0;
        }

        RIL_CellIdAndBarringInfo *bic = (RIL_CellIdAndBarringInfo *)response;
        V1_5::CellIdentity cellIdentity;
        hidl_vec<V1_5::BarringInfo> records;
        Radio2HalUtils::convertRilCellIdentityToHal(cellIdentity, bic->cellIdentity);
        records.resize(bic->numOfrecord);
        Radio2HalUtils::convertRilBarringInfosToHal(records, bic->precords, bic->numOfrecord);
        Return<void> retStatus = mRadioIndicationV1_5->barringInfoChanged(
                convertIntToRadioIndicationType(indicationType), cellIdentity, records);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioIndicationV1_5 == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalNetworkIndication::cdmaPrlChangedInd(int slotId,
                         int indicationType, int token, RIL_Errno e, void *response,
                         size_t responselen) {
#if VDBG
    RLOGD("%s::%s", TAG, __FUNCTION__);
#endif

    if (mRadioIndication != NULL) {
        if (response == NULL || responselen != sizeof(int)) {
            RLOGE("%s: invalid response", __FUNCTION__);
            return 0;
        }
        int32_t version = ((int32_t *) response)[0];
#if VDBG
        RLOGD("%s: version %d", __FUNCTION__, version);
#endif
        Return<void> retStatus = mRadioIndication->cdmaPrlChanged(
                convertIntToRadioIndicationType(indicationType), version);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioIndication == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalNetworkIndication::cellInfoListInd(int slotId,
                         int indicationType, int token, RIL_Errno e, void *response,
                         size_t responselen) {
#if VDBG
    RLOGD("%s::%s", TAG, __FUNCTION__);
#endif
    if (mRadioIndication != nullptr) {
        if ((response == NULL && responselen != 0) || responselen % sizeof(RIL_CellInfo_v12) != 0) {
            RLOGE("%s: invalid response", __FUNCTION__);
            return 0;
        }
        hidl_vec<CellInfo> records;
        Radio2HalUtils::convertRilCellInfoListToHal(response, responselen, records);
        Return<void> retStatus = mRadioIndication->cellInfoList(
                convertIntToRadioIndicationType(indicationType), records);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioIndication == NULL", __FUNCTION__, slotId);
    }
    return 0;
}

int HidlHalNetworkIndication::cellInfoListInd_1_2(int slotId,
                         int indicationType, int token, RIL_Errno e, void *response,
                         size_t responselen) {
#if VDBG
    RLOGD("%s::%s", TAG, __FUNCTION__);
#endif
    if (mRadioIndicationV1_2 != nullptr) {
        if ((response == NULL && responselen != 0) || responselen % sizeof(RIL_CellInfo_V1_2) != 0) {
            RLOGE("%s: invalid response", __FUNCTION__);
            return 0;
        }
        hidl_vec<V1_2::CellInfo> records;
        Radio2HalUtils::convertRilCellInfoListToHal(response, responselen, records);
        Return<void> retStatus = mRadioIndicationV1_2->cellInfoList_1_2(
                convertIntToRadioIndicationType(indicationType), records);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioIndicationV1_2 == NULL", __FUNCTION__, slotId);
        if (response != NULL && responselen > 0
                && responselen % sizeof(RIL_CellInfo_V1_2) == 0) {
            int numOfCellInfo = responselen / sizeof(RIL_CellInfo_V1_2);
            RIL_CellInfo_v12 *cellInfo = new RIL_CellInfo_v12[numOfCellInfo] {};
            for (int i = 0; i < numOfCellInfo; i++) {
                RadioConvUtils::convertCellInfo(cellInfo[i], ((RIL_CellInfo_V1_2 *)response)[i]);
            } // end for i ~
            int ret = this->cellInfoListInd(slotId, indicationType, token, e, cellInfo,
                    sizeof(RIL_CellInfo_v12) * numOfCellInfo);
            delete[] cellInfo;
            return ret;
        }
    }
    return 0;
}

int HidlHalNetworkIndication::cellInfoListInd_1_4(int slotId,
                         int indicationType, int token, RIL_Errno e, void *response,
                         size_t responselen) {
#if VDBG
    RLOGD("%s::%s", TAG, __FUNCTION__);
#endif
    if (mRadioIndicationV1_4 != nullptr) {
        if ((response == NULL && responselen != 0) || responselen % sizeof(RIL_CellInfo_V1_4) != 0) {
            RLOGE("%s: invalid response", __FUNCTION__);
            return 0;
        }
        hidl_vec<V1_4::CellInfo> records;
        Radio2HalUtils::convertRilCellInfoListToHal(response, responselen, records);
        Return<void> retStatus = mRadioIndicationV1_4->cellInfoList_1_4(
                convertIntToRadioIndicationType(indicationType), records);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioIndicationV1_4 == NULL", __FUNCTION__, slotId);
        if (response != NULL && responselen > 0
                && responselen % sizeof(RIL_CellInfo_V1_4) == 0) {
            int numOfCellInfo = responselen / sizeof(RIL_CellInfo_V1_4);
            RIL_CellInfo_V1_2 *cellInfoV1_2 = new RIL_CellInfo_V1_2[numOfCellInfo] {};
            for (int i = 0; i < numOfCellInfo; i++) {
                RadioConvUtils::convertCellInfo(cellInfoV1_2[i], ((RIL_CellInfo_V1_4 *)response)[i]);
            } // end for i ~
            int ret = this->cellInfoListInd_1_2(slotId, indicationType, token, e, cellInfoV1_2,
                    sizeof(RIL_CellInfo_V1_2) * numOfCellInfo);
            delete[] cellInfoV1_2;
            return ret;
        }
    }
    return 0;
}

int HidlHalNetworkIndication::cellInfoListInd_1_5(int slotId,
                         int indicationType, int token, RIL_Errno e, void *response,
                         size_t responselen) {
#if VDBG
    RLOGD("%s::%s", TAG, __FUNCTION__);
#endif
    if (mRadioIndicationV1_5 != NULL) {
        if ((response == NULL && responselen != 0) || responselen % sizeof(RIL_CellInfo_V1_5) != 0) {
            RLOGE("%s: invalid response", __FUNCTION__);
            return 0;
        }
        hidl_vec<V1_5::CellInfo> records;
        Radio2HalUtils::convertRilCellInfoListToHal(response, responselen, records);
        Return<void> retStatus = mRadioIndicationV1_5->cellInfoList_1_5(
                convertIntToRadioIndicationType(indicationType), records);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioIndicationV1_5 == NULL", __FUNCTION__, slotId);
        if (response != NULL && responselen > 0
                && responselen % sizeof(RIL_CellInfo_V1_5) == 0) {
            int numOfCellInfo = responselen / sizeof(RIL_CellInfo_V1_5);
            RIL_CellInfo_V1_4 *cellInfoV1_4 = new RIL_CellInfo_V1_4[numOfCellInfo] {};
            for (int i = 0; i < numOfCellInfo; i++) {
                RadioConvUtils::convertCellInfo(cellInfoV1_4[i], ((RIL_CellInfo_V1_5 *)response)[i]);
            } // end for i ~
            int ret = this->cellInfoListInd_1_4(slotId, indicationType, token, e, cellInfoV1_4,
                    sizeof(RIL_CellInfo_V1_4) * numOfCellInfo);
            delete[] cellInfoV1_4;
            return ret;
        }
    }
    return 0;
}

int HidlHalNetworkIndication::currentPhysicalChannelConfigs(int slotId,
                         int indicationType, int token, RIL_Errno e, void *response,
                         size_t responselen) {
#if VDBG
    RLOGD("%s::%s", TAG, __FUNCTION__);
#endif
    if (mRadioIndicationV1_2 != NULL) {
        if ((response == NULL && responselen != 0) || responselen % sizeof(RIL_PhysicalChannelConfig) != 0) {
            RLOGE("%s: invalid response", __FUNCTION__);
            return 0;
        }

        hidl_vec<V1_2::PhysicalChannelConfig> configs;
        int num = responselen / sizeof(RIL_PhysicalChannelConfig);
        configs.resize(num);
        for (int i = 0 ; i < num; i++) {
            RIL_PhysicalChannelConfig *resp = &((RIL_PhysicalChannelConfig *) response)[i];
            configs[i].status = (V1_2::CellConnectionStatus) resp->status;
            configs[i].cellBandwidthDownlink = resp->cellBandwidthDownlink;
        }

        Return<void> retStatus = mRadioIndicationV1_2->currentPhysicalChannelConfigs(
                convertIntToRadioIndicationType(indicationType), configs);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioIndicationV1_2 == NULL", __FUNCTION__, slotId);
    }
    return 0;
}

int HidlHalNetworkIndication::currentPhysicalChannelConfigs_1_4(int slotId,
                         int indicationType, int token, RIL_Errno e, void *response,
                         size_t responselen) {
#if VDBG
    RLOGD("%s::%s", TAG, __FUNCTION__);
#endif
    if (mRadioIndicationV1_4 != NULL) {
        if ((response == NULL && responselen != 0) || responselen % sizeof(RIL_PhysicalChannelConfig_V1_4) != 0) {
            RLOGE("%s: invalid response", __FUNCTION__);
            return 0;
        }

        hidl_vec<V1_4::PhysicalChannelConfig> configs;
        int num = responselen / sizeof(RIL_PhysicalChannelConfig_V1_4);
        configs.resize(num);
        for (int i = 0 ; i < num; i++) {
            RIL_PhysicalChannelConfig_V1_4 *resp = &((RIL_PhysicalChannelConfig_V1_4 *)response)[i];
            Radio2HalUtils::convertRilPhysicalChannelConfigToHal(resp, sizeof(RIL_PhysicalChannelConfig_V1_4), configs[i]);
        }

        Return<void> retStatus = mRadioIndicationV1_4->currentPhysicalChannelConfigs_1_4(
                convertIntToRadioIndicationType(indicationType), configs);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioIndicationV1_4 == NULL", __FUNCTION__, slotId);
        RIL_PhysicalChannelConfig  *configs = NULL;
        size_t num = responselen / sizeof(RIL_PhysicalChannelConfig_V1_4);
        if (num > 0) {
            configs = new RIL_PhysicalChannelConfig[num] {};
            for (size_t i = 0; response && i < num; i++) {
                configs[i].status =
                        (RIL_CellConnectionStatus)((RIL_PhysicalChannelConfig_V1_4 *)response)[i].status;
                configs[i].cellBandwidthDownlink =
                    (RIL_CellConnectionStatus)((RIL_PhysicalChannelConfig_V1_4 *)response)[i].cellBandwidthDownlink;
            }
        }
        this->currentPhysicalChannelConfigs(slotId, indicationType, token, e,
                                configs, sizeof(RIL_PhysicalChannelConfig) * num);
        if (configs) {
            delete[] configs;
        }
    }
    return 0;
}

int HidlHalNetworkIndication::currentSignalStrengthInd(int slotId,
                         int indicationType, int token, RIL_Errno e, void *response,
                         size_t responselen) {
#if VDBG
    RLOGD("%s::%s", TAG, __FUNCTION__);
#endif
    if (mRadioIndication != nullptr) {
        if (response == NULL || responselen != sizeof(RIL_SignalStrength_v10)) {
            RLOGE("%s: invalid response", __FUNCTION__);
            return 0;
        }
        SignalStrength signalStrength = {};
        Radio2HalUtils::convertRilSignalStrengthToHal(response, responselen, signalStrength);
        Return<void> retStatus = mRadioIndication->currentSignalStrength(
                convertIntToRadioIndicationType(indicationType), signalStrength);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioIndication == NULL", __FUNCTION__, slotId);
    }
    return 0;
}

int HidlHalNetworkIndication::currentSignalStrengthInd_1_2(int slotId,
                         int indicationType, int token, RIL_Errno e, void *response,
                         size_t responselen) {
#if VDBG
    RLOGD("%s::%s", TAG, __FUNCTION__);
#endif
    if (mRadioIndicationV1_2 != nullptr) {
        if (response == NULL || responselen != sizeof(RIL_SignalStrength_V1_2)) {
            RLOGE("%s: invalid response", __FUNCTION__);
            return 0;
        }
        V1_2::SignalStrength signalStrength = {};
        Radio2HalUtils::convertRilSignalStrengthToHal(response, responselen, signalStrength);
        Return<void> retStatus = mRadioIndicationV1_2->currentSignalStrength_1_2(
                convertIntToRadioIndicationType(indicationType), signalStrength);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioIndicationV1_2 == NULL", __FUNCTION__, slotId);
        if (response != NULL && responselen >= sizeof(RIL_SignalStrength_V1_2)) {
            RIL_SignalStrength_v10 signalStrength = {};
            RadioConvUtils::convertSignalStrengthResult(signalStrength,
                    *((RIL_SignalStrength_V1_2 *)response));
            return this->currentSignalStrengthInd(slotId, indicationType, token, e,
                    &signalStrength, sizeof(RIL_SignalStrength_v10));
        }
    }
    return 0;
}

int HidlHalNetworkIndication::currentSignalStrengthInd_1_4(int slotId,
                         int indicationType, int token, RIL_Errno e, void *response,
                         size_t responselen) {
#if VDBG
    RLOGD("%s::%s", TAG, __FUNCTION__);
#endif
    if (mRadioIndicationV1_4 != NULL) {
        if (response == NULL || responselen != sizeof(RIL_SignalStrength_V1_4)) {
            RLOGE("%s: invalid response", __FUNCTION__);
            return 0;
        }
        V1_4::SignalStrength signalStrength = {};
        Radio2HalUtils::convertRilSignalStrengthToHal(response, responselen, signalStrength);
        Return<void> retStatus = mRadioIndicationV1_4->currentSignalStrength_1_4(
                convertIntToRadioIndicationType(indicationType), signalStrength);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioIndicationV1_4 == NULL", __FUNCTION__, slotId);
        if (response != NULL && responselen >= sizeof(RIL_SignalStrength_V1_2)) {
            RIL_SignalStrength_V1_2 signalStrength = {};
            RadioConvUtils::convertSignalStrengthResult(signalStrength, *((RIL_SignalStrength_V1_4 *)response));
            return this->currentSignalStrengthInd_1_2(slotId, indicationType, token, e,
                    &signalStrength, sizeof(RIL_SignalStrength_V1_2));
        }
    }
    return 0;
}

int HidlHalNetworkIndication::imsNetworkStateChangedInd(int slotId,
                         int indicationType, int token, RIL_Errno e, void *response,
                         size_t responselen) {
#if VDBG
    RLOGD("%s::%s", TAG, __FUNCTION__);
#endif
    if (mRadioIndication != NULL) {
        Return<void> retStatus = mRadioIndication->imsNetworkStateChanged(
                convertIntToRadioIndicationType(indicationType));
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioIndication == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalNetworkIndication::networkScanResultInd(int slotId,
                         int indicationType, int token, RIL_Errno e, void *response,
                         size_t responselen) {
#if VDBG
    RLOGD("%s::%s", TAG, __FUNCTION__);
#endif
    if (mRadioIndicationV1_1 != NULL) {
        if (response == NULL || responselen == 0) {
            RLOGE("%s: invalid response", __FUNCTION__);
            return 0;
        }

        RIL_NetworkScanResult *networkScanResult = (RIL_NetworkScanResult *) response;

        V1_1::NetworkScanResult result;
        result.status = (V1_1::ScanStatus) networkScanResult->status;
        result.error = (RadioError) networkScanResult->error;
        Radio2HalUtils::convertRilCellInfoListToHal(
                networkScanResult->network_infos,
                networkScanResult->network_infos_length * sizeof(RIL_CellInfo_v12),
                result.networkInfos);

        Return<void> retStatus = mRadioIndicationV1_1->networkScanResult(
                convertIntToRadioIndicationType(indicationType), result);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioIndicationV1_1 == NULL", __FUNCTION__, slotId);
    }
    return 0;
}

int HidlHalNetworkIndication::networkScanResultInd_1_2(int slotId,
                         int indicationType, int token, RIL_Errno e, void *response,
                         size_t responselen) {
#if VDBG
    RLOGD("%s::%s", TAG, __FUNCTION__);
#endif
    if (mRadioIndicationV1_2 != NULL) {
        if (response == NULL || responselen == 0) {
            RLOGE("%s: invalid response", __FUNCTION__);
            return 0;
        }

        RIL_NetworkScanResult_V1_2 *networkScanResult = (RIL_NetworkScanResult_V1_2 *) response;

        V1_2::NetworkScanResult result;
        result.status = (V1_1::ScanStatus) networkScanResult->status;
        result.error = (RadioError) networkScanResult->error;
        Radio2HalUtils::convertRilCellInfoListToHal(
                networkScanResult->network_infos,
                networkScanResult->network_infos_length * sizeof(RIL_CellInfo_V1_2),
                result.networkInfos);

        Return<void> retStatus = mRadioIndicationV1_2->networkScanResult_1_2(
                convertIntToRadioIndicationType(indicationType), result);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioIndicationV1_2 == NULL", __FUNCTION__, slotId);
        if (response != NULL && responselen > 0
                && responselen % sizeof(RIL_NetworkScanResult_V1_2) == 0) {
            RIL_NetworkScanResult_V1_2 *pscanResultV1_2 = (RIL_NetworkScanResult_V1_2 *) response;
            RIL_NetworkScanResult scanResult;
            scanResult.status = pscanResultV1_2->status;
            scanResult.network_infos_length = pscanResultV1_2->network_infos_length;
            scanResult.error= pscanResultV1_2->error;
            RIL_CellInfo_v12 *cellInfoList = new RIL_CellInfo_v12[scanResult.network_infos_length] {};
            if (cellInfoList != NULL) {
                for (uint32_t i = 0; i < scanResult.network_infos_length; i++) {
                    RadioConvUtils::convertCellInfo(cellInfoList[i], pscanResultV1_2->network_infos[i]);
                } // end for i ~
                scanResult.network_infos = cellInfoList;
                int ret = this->networkScanResultInd(slotId, indicationType, token, e, &scanResult,
                        sizeof(RIL_NetworkScanResult));
                delete[] cellInfoList;
                return ret;
            }
        }
    }
    return 0;
}

int HidlHalNetworkIndication::networkScanResultInd_1_4(int slotId,
                         int indicationType, int token, RIL_Errno e, void *response,
                         size_t responselen) {
#if VDBG
    RLOGD("%s::%s", TAG, __FUNCTION__);
#endif
    if (mRadioIndicationV1_4 != NULL) {
        if (response == NULL || responselen == 0) {
            RLOGE("%s: invalid response", __FUNCTION__);
            return 0;
        }

        RIL_NetworkScanResult_V1_4 *networkScanResult = (RIL_NetworkScanResult_V1_4 *) response;

        V1_4::NetworkScanResult result;
        result.status = (V1_1::ScanStatus) networkScanResult->status;
        result.error = (RadioError) networkScanResult->error;
        Radio2HalUtils::convertRilCellInfoListToHal(
                networkScanResult->network_infos,
                networkScanResult->network_infos_length * sizeof(RIL_CellInfo_V1_4),
                result.networkInfos);

        Return<void> retStatus = mRadioIndicationV1_4->networkScanResult_1_4(
                convertIntToRadioIndicationType(indicationType), result);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioIndicationV1_4 == NULL", __FUNCTION__, slotId);
        if (response != NULL && responselen > 0
                && responselen % sizeof(RIL_NetworkScanResult_V1_4) == 0) {
            RIL_NetworkScanResult_V1_4 *pscanResultV1_4 = (RIL_NetworkScanResult_V1_4 *) response;
            RIL_NetworkScanResult_V1_2 scanResultV1_2;
            scanResultV1_2.status = pscanResultV1_4->status;
            scanResultV1_2.network_infos_length = pscanResultV1_4->network_infos_length;
            scanResultV1_2.error= pscanResultV1_4->error;
            RIL_CellInfo_V1_2 *cellInfoV1_2List = new RIL_CellInfo_V1_2[scanResultV1_2.network_infos_length] {};
            if (cellInfoV1_2List != NULL) {
                for (uint32_t i = 0; i < scanResultV1_2.network_infos_length; i++) {
                    RadioConvUtils::convertCellInfo(cellInfoV1_2List[i], pscanResultV1_4->network_infos[i]);
                } // end for i ~
                scanResultV1_2.network_infos = cellInfoV1_2List;
                int ret = this->networkScanResultInd_1_2(slotId, indicationType, token, e, &scanResultV1_2,
                        sizeof(RIL_NetworkScanResult_V1_2));
                delete[] cellInfoV1_2List;
                return ret;
            }
        }
    }
    return 0;
}

int HidlHalNetworkIndication::networkScanResultInd_1_5(int slotId,
                         int indicationType, int token, RIL_Errno e, void *response,
                         size_t responselen) {
#if VDBG
    RLOGD("%s::%s", TAG, __FUNCTION__);
#endif
    if (mRadioIndicationV1_5 != NULL) {
        if (response == NULL || responselen == 0) {
            RLOGE("%s: invalid response", __FUNCTION__);
            return 0;
        }

        RIL_NetworkScanResult_V1_5 *networkScanResult = (RIL_NetworkScanResult_V1_5 *) response;

        V1_5::NetworkScanResult result;
        result.status = (V1_1::ScanStatus) networkScanResult->status;
        result.error = (RadioError) networkScanResult->error;
        Radio2HalUtils::convertRilCellInfoListToHal(
                networkScanResult->network_infos,
                networkScanResult->network_infos_length * sizeof(RIL_CellInfo_V1_5),
                result.networkInfos);

        Return<void> retStatus = mRadioIndicationV1_5->networkScanResult_1_5(
                convertIntToRadioIndicationType(indicationType), result);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioIndicationV1_5 == NULL", __FUNCTION__, slotId);
        if (response != NULL && responselen > 0
                && responselen % sizeof(RIL_NetworkScanResult_V1_5) == 0) {
            RIL_NetworkScanResult_V1_5 *pscanResultV1_5 = (RIL_NetworkScanResult_V1_5 *) response;
            RIL_NetworkScanResult_V1_4 scanResultV1_4;
            scanResultV1_4.status = pscanResultV1_5->status;
            scanResultV1_4.network_infos_length = pscanResultV1_5->network_infos_length;
            scanResultV1_4.error= pscanResultV1_5->error;
            RIL_CellInfo_V1_4 *cellInfoV1_4List = new RIL_CellInfo_V1_4[scanResultV1_4.network_infos_length] {};
            for (uint32_t i = 0; i < scanResultV1_4.network_infos_length; i++) {
                RadioConvUtils::convertCellInfo(cellInfoV1_4List[i], pscanResultV1_5->network_infos[i]);
            } // end for i ~
            scanResultV1_4.network_infos = cellInfoV1_4List;
            int ret = this->networkScanResultInd_1_4(slotId, indicationType, token, e,
                                 &scanResultV1_4, sizeof(RIL_NetworkScanResult_V1_4));
            delete[] cellInfoV1_4List;
            return ret;
        }
    }
    return 0;
}

int HidlHalNetworkIndication::networkStateChangedInd(int slotId,
                         int indicationType, int token, RIL_Errno e, void *response,
                         size_t responselen) {
#if VDBG
    RLOGD("%s::%s", TAG, __FUNCTION__);
#endif
    if (mRadioIndication != NULL) {
        Return<void> retStatus = mRadioIndication->networkStateChanged(
                convertIntToRadioIndicationType(indicationType));
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioIndication == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

void setNitzTimeReceived(int slotId, long timeReceived) {
    nitzTimeReceived[slotId] = timeReceived;
}

int HidlHalNetworkIndication::nitzTimeReceivedInd(int slotId,
                         int indicationType, int token, RIL_Errno e, void *response,
                         size_t responselen) {
#if VDBG
    RLOGD("%s::%s", TAG, __FUNCTION__);
#endif
    if (mRadioIndication != NULL) {
        if (response == NULL || responselen == 0) {
            RLOGE("%s: invalid response", __FUNCTION__);
            return 0;
        }
        hidl_string nitzTime = Radio2HalUtils::convertCharPtrToHidlString((char *) response);
#if VDBG
        RLOGD("%s: nitzTime %s receivedTime %ld", __FUNCTION__, nitzTime.c_str(),
                (long)nitzTimeReceived[slotId]);
#endif
        Return<void> retStatus = mRadioIndication->nitzTimeReceived(
                convertIntToRadioIndicationType(indicationType), nitzTime,
                nitzTimeReceived[slotId]);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioIndication == NULL", __FUNCTION__, slotId);
        return -1;
    }

    return 0;
}

int HidlHalNetworkIndication::registrationFailedInd(int slotId,
                         int indicationType, int token, RIL_Errno e, void *response,
                         size_t responselen) {
#if VDBG
    RLOGD("%s::%s", TAG, __FUNCTION__);
#endif
    if (mRadioIndicationV1_5 != NULL) {
        if (response == NULL || responselen == 0 || responselen != sizeof(RIL_RegistrationFailed)) {
            RLOGE("%s: invalid response", __FUNCTION__);
            return 0;
        }

        RIL_RegistrationFailed *pregFailed = (RIL_RegistrationFailed *)response;
        V1_5::CellIdentity cellIdentity = {};
        Radio2HalUtils::convertRilCellIdentityToHal(cellIdentity, pregFailed->cellIdentity);
        hidl_string chosenPlmn = Radio2HalUtils::convertCharPtrToHidlString(pregFailed->chosenPlmn);
        ::android::hardware::hidl_bitfield<V1_5::Domain> domainBitmap = pregFailed->domainBitmap;
        int32_t causeCode = pregFailed->causeCode;
        int32_t additionalCauseCode = pregFailed->additionalCauseCode;
        Return<void> retStatus = mRadioIndicationV1_5->registrationFailed(
                convertIntToRadioIndicationType(indicationType), cellIdentity, chosenPlmn, domainBitmap, causeCode, additionalCauseCode);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioIndicationV1_5 == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalNetworkIndication::restrictedStateChangedInd(int slotId,
                         int indicationType, int token, RIL_Errno e, void *response,
                         size_t responselen) {
#if VDBG
    RLOGD("%s::%s", TAG, __FUNCTION__);
#endif
    if (mRadioIndication != NULL) {
        if (response == NULL || responselen != sizeof(int)) {
            RLOGE("%s: invalid response", __FUNCTION__);
            return 0;
        }
        int32_t state = ((int32_t *) response)[0];
#if VDBG
        RLOGD("%s: state %d", __FUNCTION__, state);
#endif
        Return<void> retStatus = mRadioIndication->restrictedStateChanged(
                convertIntToRadioIndicationType(indicationType), (PhoneRestrictedState) state);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioIndication == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalNetworkIndication::suppSvcNotifyInd(int slotId,
                         int indicationType, int token, RIL_Errno e, void *response,
                         size_t responselen) {
#if VDBG
    RLOGD("%s::%s", TAG, __FUNCTION__);
#endif
    if (mRadioIndication != NULL) {
        if (response == NULL || responselen != sizeof(RIL_SuppSvcNotification)) {
            RLOGE("%s: invalid response", __FUNCTION__);
            return 0;
        }

        SuppSvcNotification suppSvc = {};
        RIL_SuppSvcNotification *ssn = (RIL_SuppSvcNotification *) response;
        suppSvc.isMT = ssn->notificationType;
        suppSvc.code = ssn->code;
        suppSvc.index = ssn->index;
        suppSvc.type = ssn->type;
        suppSvc.number = Radio2HalUtils::convertCharPtrToHidlString(ssn->number);

#if VDBG
        RLOGD("%s: isMT %d code %d index %d type %d",
                __FUNCTION__, suppSvc.isMT, suppSvc.code, suppSvc.index, suppSvc.type);
#endif
        Return<void> retStatus = mRadioIndication->suppSvcNotify(
                convertIntToRadioIndicationType(indicationType), suppSvc);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioIndication == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalNetworkIndication::voiceRadioTechChangedInd(int slotId,
                         int indicationType, int token, RIL_Errno e, void *response,
                         size_t responselen) {
#if VDBG
    RLOGD("%s::%s", TAG, __FUNCTION__);
#endif
    if (mRadioIndication != NULL) {
        if (response == NULL || responselen != sizeof(int)) {
            RLOGE("%s: invalid response", __FUNCTION__);
            return 0;
        }
        int32_t rat = ((int32_t *) response)[0];
#if VDBG
        RLOGD("%s: rat %d", __FUNCTION__, rat);
#endif
        Return<void> retStatus = mRadioIndication->voiceRadioTechChanged(
                convertIntToRadioIndicationType(indicationType), (RadioTechnology) rat);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioIndication == NULL", __FUNCTION__, slotId);
    }
    return 0;
}