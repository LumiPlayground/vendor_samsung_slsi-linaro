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

#pragma once

#include "radiovoicecallback.h"
#include "radioreturnstatus.h"
#include "radioresponseutil.h"
#include "binderdeathrecipient.h"

namespace android::hardware::radio::impl {

struct RadioVoiceIndication : public RadioVoiceIndicationCallback,
                            public RadioReturnStatus,
                            public RemoteDiedHelper {
    std::shared_ptr<::aidl::android::hardware::radio::voice::IRadioVoiceIndication> mIndication;

    SYNCHRONIZED_WRAPPER(so);

    void onRemoteDied() override {
        // no race-condition between onBinderDied and setResponseFunction.
        setResponseFunction(nullptr);
    }
    void setResponseFunction(
            const std::shared_ptr<::aidl::android::hardware::radio::voice::IRadioVoiceIndication>&
            radioVoiceIndication);

    void callRing(::aidl::android::hardware::radio::RadioIndicationType type, bool isGsm,
            const ::aidl::android::hardware::radio::voice::CdmaSignalInfoRecord& record) override;

    void callStateChanged(::aidl::android::hardware::radio::RadioIndicationType type) override;

    void cdmaCallWaiting(::aidl::android::hardware::radio::RadioIndicationType type,
            const ::aidl::android::hardware::radio::voice::CdmaCallWaiting& callWaitingRecord) override;

    void cdmaInfoRec(::aidl::android::hardware::radio::RadioIndicationType type,
            const std::vector<::aidl::android::hardware::radio::voice::CdmaInformationRecord>& records) override;

    void cdmaOtaProvisionStatus(::aidl::android::hardware::radio::RadioIndicationType type,
            ::aidl::android::hardware::radio::voice::CdmaOtaProvisionStatus status) override;

    void currentEmergencyNumberList(::aidl::android::hardware::radio::RadioIndicationType type,
            const std::vector<::aidl::android::hardware::radio::voice::EmergencyNumber>& emergencyNumberList)
        override;

    void enterEmergencyCallbackMode(::aidl::android::hardware::radio::RadioIndicationType type) override;

    void exitEmergencyCallbackMode(::aidl::android::hardware::radio::RadioIndicationType type) override;

    void indicateRingbackTone(::aidl::android::hardware::radio::RadioIndicationType type, bool start) override;

    void onSupplementaryServiceIndication(::aidl::android::hardware::radio::RadioIndicationType type,
            const ::aidl::android::hardware::radio::voice::StkCcUnsolSsResult& ss) override;

    void onUssd(::aidl::android::hardware::radio::RadioIndicationType type,
            ::aidl::android::hardware::radio::voice::UssdModeType modeType, const std::string& msg) override;

    void resendIncallMute(::aidl::android::hardware::radio::RadioIndicationType type) override;

    void srvccStateNotify(::aidl::android::hardware::radio::RadioIndicationType type,
            ::aidl::android::hardware::radio::voice::SrvccState state) override;

    void stkCallControlAlphaNotify(::aidl::android::hardware::radio::RadioIndicationType type,
            const std::string& alpha) override;

    void stkCallSetup(::aidl::android::hardware::radio::RadioIndicationType type, int64_t timeout) override;
};

}
