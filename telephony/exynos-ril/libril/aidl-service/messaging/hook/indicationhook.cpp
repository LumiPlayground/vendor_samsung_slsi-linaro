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
#include <aidl-service/aidl-log.h>

#include "collections.h"
#include "commonStructs.h"
#include "radiomessagingresponse.h"
#include "structs.h"
#include <ril_internal.h>

namespace aidl_messaging = ::aidl::android::hardware::radio::messaging;

using namespace ::android::hardware::radio::V1_6;
using namespace ::android::hardware::radio::V1_5;
using namespace ::android::hardware::radio::V1_4;
using namespace ::android::hardware::radio::V1_3;
using namespace ::android::hardware::radio::V1_2;
using namespace ::android::hardware::radio::V1_1;
using namespace ::android::hardware::radio::V1_0;

using namespace ::android::hardware::radio::compat;
using namespace ::android::hardware::radio::impl;
using ::android::hardware::Return;
using ::android::hardware::Void;

#define TAG "RadioMessagingIndicationHook"

void RadioMessagingIndicationHook::setResponseFunction(std::shared_ptr<RadioMessagingIndicationCallback> messagingCb) {
    RLOGD("%s::%s messagingCb=%p", TAG, __FUNCTION__, messagingCb.get());
    mMessagingCb = messagingCb;
}

Return<void> RadioMessagingIndicationHook::cdmaNewSms(RadioIndicationType type, const CdmaSmsMessage &msg) {
#if VDBG
    RLOGD("%s::%s mMessagingCb=%p", TAG, __FUNCTION__, mMessagingCb.get());
#endif
    if (mMessagingCb) {
        mMessagingCb->cdmaNewSms(toAidl(type), toAidl(msg));
    }
    return Void();
}

Return<void> RadioMessagingIndicationHook::cdmaRuimSmsStorageFull(RadioIndicationType type) {
#if VDBG
    RLOGD("%s::%s mMessagingCb=%p", TAG, __FUNCTION__, mMessagingCb.get());
#endif
    if (mMessagingCb) {
        mMessagingCb->cdmaRuimSmsStorageFull(toAidl(type));
    }
    return Void();
}

Return<void> RadioMessagingIndicationHook::newBroadcastSms(RadioIndicationType type, const ::android::hardware::hidl_vec<uint8_t> &data) {
#if VDBG
    RLOGD("%s::%s mMessagingCb=%p", TAG, __FUNCTION__, mMessagingCb.get());
#endif
    if (mMessagingCb) {
        // TODO: need to check to use proper toAidl()
        std::vector<uint8_t> out(data.size());
        for (size_t i = 0; i < data.size(); i++) {
            out[i] = data[i];
        }

        mMessagingCb->newBroadcastSms(toAidl(type), out);
    }
    return Void();
}

Return<void> RadioMessagingIndicationHook::newSms(RadioIndicationType type, const ::android::hardware::hidl_vec<uint8_t> &pdu) {
#if VDBG
    RLOGD("%s::%s mMessagingCb=%p", TAG, __FUNCTION__, mMessagingCb.get());
#endif
    if (mMessagingCb) {
        // TODO: need to check to use proper toAidl()
        std::vector<uint8_t> out(pdu.size());
        for (size_t i = 0; i < pdu.size(); i++) {
            out[i] = pdu[i];
        }

        mMessagingCb->newSms(toAidl(type), out);
    }
    return Void();
}

Return<void> RadioMessagingIndicationHook::newSmsOnSim(RadioIndicationType type, int32_t recordNumber) {
#if VDBG
    RLOGD("%s::%s mMessagingCb=%p", TAG, __FUNCTION__, mMessagingCb.get());
#endif
    if (mMessagingCb) {
        mMessagingCb->newSmsOnSim(toAidl(type), recordNumber);
    }
    return Void();
}

Return<void> RadioMessagingIndicationHook::newSmsStatusReport(RadioIndicationType type, const ::android::hardware::hidl_vec<uint8_t> &pdu) {
#if VDBG
    RLOGD("%s::%s mMessagingCb=%p", TAG, __FUNCTION__, mMessagingCb.get());
#endif
    if (mMessagingCb) {
        // TODO: need to check to use proper toAidl()
        std::vector<uint8_t> out(pdu.size());
        for (size_t i = 0; i < pdu.size(); i++) {
            out[i] = pdu[i];
        }

        mMessagingCb->newSmsStatusReport(toAidl(type), out);
    }
    return Void();
}

Return<void> RadioMessagingIndicationHook::simSmsStorageFull(RadioIndicationType type) {
#if VDBG
    RLOGD("%s::%s mMessagingCb=%p", TAG, __FUNCTION__, mMessagingCb.get());
#endif
    if (mMessagingCb) {
        mMessagingCb->simSmsStorageFull(toAidl(type));
    }
    return Void();
}
