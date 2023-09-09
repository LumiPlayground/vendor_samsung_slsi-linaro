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

#include "radiosimcallback.h"
#include "radioreturnstatus.h"
#include "radioresponseutil.h"
#include "binderdeathrecipient.h"

namespace android::hardware::radio::impl {

struct RadioSimIndication : public RadioSimIndicationCallback,
                            public RadioReturnStatus,
                            public RemoteDiedHelper {
    std::shared_ptr<::aidl::android::hardware::radio::sim::IRadioSimIndication> mIndication;

    SYNCHRONIZED_WRAPPER(so);

    void onRemoteDied() override {
        // no race-condition between onBinderDied and setResponseFunction.
        setResponseFunction(nullptr);
    }
    void setResponseFunction(
        const std::shared_ptr<::aidl::android::hardware::radio::sim::IRadioSimIndication>&
                radioSimIndication);

    void carrierInfoForImsiEncryption(
            ::aidl::android::hardware::radio::RadioIndicationType type) override;
    void cdmaSubscriptionSourceChanged(
            ::aidl::android::hardware::radio::RadioIndicationType type,
            ::aidl::android::hardware::radio::sim::CdmaSubscriptionSource cdmaSource) override;
    void simPhonebookChanged(
            ::aidl::android::hardware::radio::RadioIndicationType type) override;
    void simPhonebookRecordsReceived(
            ::aidl::android::hardware::radio::RadioIndicationType type,
            ::aidl::android::hardware::radio::sim::PbReceivedStatus status,
            const std::vector<::aidl::android::hardware::radio::sim::PhonebookRecordInfo>& records
            ) override;
    void simRefresh(
            ::aidl::android::hardware::radio::RadioIndicationType type,
            const ::aidl::android::hardware::radio::sim::SimRefreshResult& refreshResult) override;
    void simStatusChanged(
            ::aidl::android::hardware::radio::RadioIndicationType type) override;
    void stkEventNotify(
            ::aidl::android::hardware::radio::RadioIndicationType type,
            const std::string& cmd) override;
    void stkProactiveCommand(
            ::aidl::android::hardware::radio::RadioIndicationType type,
            const std::string& cmd) override;
    void stkSessionEnd(
            ::aidl::android::hardware::radio::RadioIndicationType type) override;
    void subscriptionStatusChanged(
            ::aidl::android::hardware::radio::RadioIndicationType type,
            bool activate) override;
    void uiccApplicationsEnablementChanged(
            ::aidl::android::hardware::radio::RadioIndicationType type,
            bool enabled) override;
};

}  // namespace android::hardware::radio::impl
