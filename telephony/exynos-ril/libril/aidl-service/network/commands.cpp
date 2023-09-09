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
#include <aidl-log.h>
#include <librilutils/osalutils.h>
#include <slsi/radio_v2_0.h>
#include <ril_internal.h>
#include "commands.h"
#include "commonvendorstructs.h"
#include "vendorstructs.h"
#include <vector>

namespace android::hardware::radio::impl::network {

using namespace android::hardware::radio::impl::network;
namespace aidl = ::aidl::android::hardware::radio;

/**
 * NitzTimeReceivedInd
 */
int NitzTimeReceivedInd::onIndication(
            int slotId, int indicationType, int /* token */, RIL_Errno e,
            void *response, size_t responselen) {
#if VDBG
    RLOGD("%s::%s mCb=%p", TAG, __FUNCTION__, mCb.get());
#endif
    if (response == NULL || responselen != sizeof(V2_0::RIL_NitzTime)) {
        RLOGE("%s::%s: invalid response", TAG, __FUNCTION__);
        return 0;
    }

    const V2_0::RIL_NitzTime *nitzTimeData = (V2_0::RIL_NitzTime *)response;
    if (!sendNitzTimeData(nitzTimeData, true)) {
        return -1;
    }
    return 0;
}

void NitzTimeReceivedInd::onRadioServiceConnected() {
    RLOGD("%s::%s mCb=%p", TAG, __FUNCTION__, mCb.get());
    if (mLastNitzTimeData && sendNitzTimeData(mLastNitzTimeData.get(), false)) {
        mLastNitzTimeData = nullptr;
    }
}

void NitzTimeReceivedInd::onRadioServiceDionnected() {
    RLOGD("%s::%s mCb=%p", TAG, __FUNCTION__, mCb.get());
}

void NitzTimeReceivedInd::saveNitzTimeData(const V2_0::RIL_NitzTime *nitzTimeData) {
    if (nitzTimeData) {
        mLastNitzTimeData = std::shared_ptr<V2_0::RIL_NitzTime>(
                        new V2_0::RIL_NitzTime {
                            nitzTimeData->nitzTime,
                            nitzTimeData->age,
                            nitzTimeData->timestamp
                        });
    }
}

bool NitzTimeReceivedInd::sendNitzTimeData(const V2_0::RIL_NitzTime *nitzTimeData, bool save) {
    bool ret = false;
    if (nitzTimeData) {
        std::string nitzTime = nitzTimeData->nitzTime;
        int64_t receivedTime = OsalUtils::elapsedRealtime();
        int64_t age = nitzTimeData->age + (receivedTime - nitzTimeData->timestamp);
        if (mCb) {
            RLOGD("%s::%s: nitz time %s", TAG, __FUNCTION__, nitzTime.c_str());
            mCb->nitzTimeReceived(aidl::RadioIndicationType::UNSOLICITED,
                                    nitzTime, receivedTime, age);
            ret = true;
        } else {
            RLOGE("%s::%s: [%d] mCb == NULL", TAG, __FUNCTION__, mSlot);
            if (save) {
                saveNitzTimeData(nitzTimeData);
            }
        }
    }
    return ret;
}

/**
 * SetSignalStrengthReportingCriteriaCommand
 */
void SetSignalStrengthReportingCriteriaCommand::onRequest(int serial,
    const std::vector<aidl::network::SignalThresholdInfo>& signalThresholdInfos) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);

    for (auto& info : signalThresholdInfos) {
        RLOGD("%s %s", TAG, info.toString().c_str());
    }
#endif
    std::vector<V2_0::RIL_SignalThresholdInfo> rilInfos;
    for (auto& info : signalThresholdInfos) {
        V2_0::RIL_SignalThresholdInfo data {};
        data.signalMeasurement = (int32_t) info.signalMeasurement;
        data.hysteresisMs = info.hysteresisMs;
        data.hysteresisDb = info.hysteresisDb;
        data.thresholds = info.thresholds;
        data.isEnabled = info.isEnabled;
        data.ran = (int32_t) info.ran;
        rilInfos.push_back(data);
    }
    mVendorFunctions.onRequest(allocateRequestInfo(serial), rilInfos.data(),
                        sizeof(V2_0::RIL_SignalThresholdInfo) * rilInfos.size());
}

int SetSignalStrengthReportingCriteriaCommand::onResponse(int slotId,int responseType, int serial, RIL_Errno e,
                                          void *response, size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    if (mCb) {
        aidl::RadioResponseInfo responseInfo = populateResponseInfo(serial, responseType, e);
        mCb->setSignalStrengthReportingCriteriaResponse(responseInfo);
    } else {
        RLOGE("%s::%s: [%d] mCb == NULL", TAG, __FUNCTION__, slotId);
    }
    return 0;
}

/**
 * SetUsageSettingCommand
 */
void SetUsageSettingCommand::onRequest(int serial, ::aidl::android::hardware::radio::network::UsageSetting usageSetting) {
#if VDBG
    RLOGD("%s::%s: serial %d, usageSetting=%d", TAG, __FUNCTION__, serial, usageSetting);
#endif
    mVendorFunctions.dispatchInts(allocateRequestInfo(serial), { (int32_t) usageSetting });
}

int SetUsageSettingCommand::onResponse(int slotId,int responseType, int serial, RIL_Errno e,
                                          void *response, size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    if (mCb) {
        aidl::RadioResponseInfo responseInfo = populateResponseInfo(serial, responseType, e);
        mCb->setUsageSettingResponse(responseInfo);
    } else {
        RLOGE("%s::%s: [%d] mCb == NULL", TAG, __FUNCTION__, slotId);
    }
    return 0;
}

/**
 * GetUsageSettingCommand
 */
void GetUsageSettingCommand::onRequest(int serial) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    mVendorFunctions.dispatchVoid(allocateRequestInfo(serial));
}

int GetUsageSettingCommand::onResponse(int slotId,int responseType, int serial, RIL_Errno e,
                                          void *response, size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    if (mCb) {
        aidl::RadioResponseInfo responseInfo = populateResponseInfo(serial, responseType, e);
        aidl::network::UsageSetting resp = {};
        if (response == nullptr || responselen != sizeof(int)) {
            RLOGE("%s::%s Invalid response", TAG, __FUNCTION__);
            if (e == RIL_E_SUCCESS) responseInfo.error = aidl::RadioError(RIL_E_INVALID_RESPONSE);
        } else {
            int usageSetting = *(int *)response;
            resp = (aidl::network::UsageSetting) usageSetting;
        }
        mCb->getUsageSettingResponse(responseInfo, resp);
    } else {
        RLOGE("%s::%s: [%d] mCb == NULL", TAG, __FUNCTION__, slotId);
    }
    return 0;
}

}
