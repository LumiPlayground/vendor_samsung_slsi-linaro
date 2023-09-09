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
#include "radiovoiceindication.h"

namespace android::hardware::radio::impl {

void RadioVoiceIndication::setResponseFunction(
    const std::shared_ptr<::aidl::android::hardware::radio::voice::IRadioVoiceIndication>&
        radioVoiceIndication) {
    execute([&]() {
        mIndication = radioVoiceIndication;
        if (mIndication) {
            linkToDeath(mIndication->asBinder().get());
        }
    });
}

void RadioVoiceIndication::callRing(::aidl::android::hardware::radio::RadioIndicationType type, bool isGsm,
                                    const ::aidl::android::hardware::radio::voice::CdmaSignalInfoRecord& record) {
    execute([&]() {
        if (mIndication) {
            auto astatus = mIndication->callRing(type, isGsm, record);
            checkReturnStatus(astatus);
        }
    });
}

void RadioVoiceIndication::callStateChanged(::aidl::android::hardware::radio::RadioIndicationType type) {
    execute([&]() {
        if (mIndication) {
            auto astatus = mIndication->callStateChanged(type);
            checkReturnStatus(astatus);
        }
    });
}

void RadioVoiceIndication::cdmaCallWaiting(::aidl::android::hardware::radio::RadioIndicationType type,
                                           const ::aidl::android::hardware::radio::voice::CdmaCallWaiting& callWaitingRecord) {
    execute([&]() {
        if (mIndication) {
            auto astatus = mIndication->cdmaCallWaiting(type, callWaitingRecord);
            checkReturnStatus(astatus);
        }
    });
}

void RadioVoiceIndication::cdmaInfoRec(::aidl::android::hardware::radio::RadioIndicationType type,
                                       const std::vector<::aidl::android::hardware::radio::voice::CdmaInformationRecord>& records) {
    execute([&]() {
        if (mIndication) {
            auto astatus = mIndication->cdmaInfoRec(type, records);
            checkReturnStatus(astatus);
        }
    });
}

void RadioVoiceIndication::cdmaOtaProvisionStatus(::aidl::android::hardware::radio::RadioIndicationType type,
                                                  ::aidl::android::hardware::radio::voice::CdmaOtaProvisionStatus status) {
    execute([&]() {
        if (mIndication) {
            auto astatus = mIndication->cdmaOtaProvisionStatus(type, status);
            checkReturnStatus(astatus);
        }
    });
}

void RadioVoiceIndication::currentEmergencyNumberList(::aidl::android::hardware::radio::RadioIndicationType type,
                                                      const std::vector<::aidl::android::hardware::radio::voice::EmergencyNumber>& emergencyNumberList) {
    execute([&]() {
        if (mIndication) {
            auto astatus = mIndication->currentEmergencyNumberList(type, emergencyNumberList);
            checkReturnStatus(astatus);
        }
    });
}

void RadioVoiceIndication::enterEmergencyCallbackMode(
    ::aidl::android::hardware::radio::RadioIndicationType type) {
    execute([&]() {
        if (mIndication) {
            auto astatus = mIndication->enterEmergencyCallbackMode(type);
            checkReturnStatus(astatus);
        }
    });
}

void RadioVoiceIndication::exitEmergencyCallbackMode(
    ::aidl::android::hardware::radio::RadioIndicationType type) {
    execute([&]() {
        if (mIndication) {
            auto astatus = mIndication->exitEmergencyCallbackMode(type);
            checkReturnStatus(astatus);
        }
    });
}

void RadioVoiceIndication::indicateRingbackTone(::aidl::android::hardware::radio::RadioIndicationType type,
                                                bool start) {
    execute([&]() {
        if (mIndication) {
            auto astatus = mIndication->indicateRingbackTone(type, start);
            checkReturnStatus(astatus);
        }
    });
}

void RadioVoiceIndication::onSupplementaryServiceIndication(
    ::aidl::android::hardware::radio::RadioIndicationType type,
    const ::aidl::android::hardware::radio::voice::StkCcUnsolSsResult& ss) {
    execute([&]() {
        if (mIndication) {
            auto astatus = mIndication->onSupplementaryServiceIndication(type, ss);
            checkReturnStatus(astatus);
        }
    });
}

void RadioVoiceIndication::onUssd(::aidl::android::hardware::radio::RadioIndicationType type,
                                  ::aidl::android::hardware::radio::voice::UssdModeType modeType,
                                  const std::string& msg) {
    execute([&]() {
        if (mIndication) {
            auto astatus = mIndication->onUssd(type, modeType, msg);
            checkReturnStatus(astatus);
        }
    });
}

void RadioVoiceIndication::resendIncallMute(::aidl::android::hardware::radio::RadioIndicationType type) {
    execute([&]() {
        if (mIndication) {
            auto astatus = mIndication->resendIncallMute(type);
            checkReturnStatus(astatus);
        }
    });
}

void RadioVoiceIndication::srvccStateNotify(::aidl::android::hardware::radio::RadioIndicationType type,
                                            ::aidl::android::hardware::radio::voice::SrvccState state) {
    execute([&]() {
        if (mIndication) {
            auto astatus = mIndication->srvccStateNotify(type, state);
            checkReturnStatus(astatus);
        }
    });
}

void RadioVoiceIndication::stkCallControlAlphaNotify(::aidl::android::hardware::radio::RadioIndicationType type,
                                                     const std::string& alpha) {
    execute([&]() {
        if (mIndication) {
            auto astatus = mIndication->stkCallControlAlphaNotify(type, alpha);
            checkReturnStatus(astatus);
        }
    });
}

void RadioVoiceIndication::stkCallSetup(::aidl::android::hardware::radio::RadioIndicationType type,
                                        int64_t timeout) {
    execute([&]() {
        if (mIndication) {
            auto astatus = mIndication->stkCallSetup(type, timeout);
            checkReturnStatus(astatus);
        }
    });
}

}  // namespace android::hardware::radio::impl
