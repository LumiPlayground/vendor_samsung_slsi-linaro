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

#include "radiomodemcallback.h"
#include "radioreturnstatus.h"
#include "radioresponseutil.h"
#include "binderdeathrecipient.h"

namespace android::hardware::radio::impl {

struct RadioModemIndication : public RadioModemIndicationCallback,
                            public RadioReturnStatus,
                            public RemoteDiedHelper {
    std::shared_ptr<::aidl::android::hardware::radio::modem::IRadioModemIndication> mIndication;

    SYNCHRONIZED_WRAPPER(so);

    void onRemoteDied() override {
        // no race-condition between onBinderDied and setResponseFunction.
        setResponseFunction(nullptr);
    }
    void setResponseFunction(
            const std::shared_ptr<::aidl::android::hardware::radio::modem::IRadioModemIndication>&
                    radioModemIndication) override;

    void hardwareConfigChanged(::aidl::android::hardware::radio::RadioIndicationType type,
                               const std::vector<::aidl::android::hardware::radio::modem::HardwareConfig>& configs) override;
    void modemReset(::aidl::android::hardware::radio::RadioIndicationType type,
                    const std::string& reason) override;
    void radioCapabilityIndication(::aidl::android::hardware::radio::RadioIndicationType type,
                                   const ::aidl::android::hardware::radio::modem::RadioCapability& rc) override;
    void radioStateChanged(::aidl::android::hardware::radio::RadioIndicationType type,
                           ::aidl::android::hardware::radio::modem::RadioState radioState) override;
    void rilConnected(::aidl::android::hardware::radio::RadioIndicationType type) override;
};

}  // namespace android::hardware::radio::impl
