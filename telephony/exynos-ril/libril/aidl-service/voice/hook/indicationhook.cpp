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

#include "radiovoiceresponse.h"
#include "commonStructs.h"
#include "structs.h"
#include "collections.h"
#include <ril_internal.h>

namespace aidl_common = ::aidl::android::hardware::radio;
namespace aidl_voice = ::aidl::android::hardware::radio::voice;
using namespace ::android::hardware::radio;
using namespace ::android::hardware::radio::compat;
using namespace ::android::hardware::radio::impl;
using ::android::hardware::Return;
using ::android::hardware::Void;

#define TAG "RadioVoiceIndicationHook"

void RadioVoiceIndicationHook::setResponseFunction(std::shared_ptr<RadioVoiceIndicationCallback> voiceCb) {
    RLOGD("%s::%s voiceCb=%p", TAG, __FUNCTION__, voiceCb.get());
    mVoiceCb = voiceCb;
}

Return<void> RadioVoiceIndicationHook::callRing(V1_0::RadioIndicationType type,
        bool isGsm,
        const V1_0::CdmaSignalInfoRecord& record) {
#if VDBG
    RLOGD("%s::%s mVoiceCb=%p", TAG, __FUNCTION__, mVoiceCb.get());
#endif
    if (mVoiceCb) {
        mVoiceCb->callRing(toAidl(type), isGsm, toAidl(record));
    }
    return Void();
}

Return<void> RadioVoiceIndicationHook::callStateChanged(V1_0::RadioIndicationType type) {
#if VDBG
    RLOGD("%s::%s mVoiceCb=%p", TAG, __FUNCTION__, mVoiceCb.get());
#endif
    if (mVoiceCb) {
        mVoiceCb->callStateChanged(toAidl(type));
    }
    return Void();
}

Return<void> RadioVoiceIndicationHook::cdmaCallWaiting(V1_0::RadioIndicationType type,
        const ::android::hardware::radio::V1_0::CdmaCallWaiting& callWaitingRecord) {
#if VDBG
    RLOGD("%s::%s mVoiceCb=%p", TAG, __FUNCTION__, mVoiceCb.get());
#endif
    if (mVoiceCb) {
        mVoiceCb->cdmaCallWaiting(toAidl(type), toAidl(callWaitingRecord));
    }
    return Void();
}

Return<void> RadioVoiceIndicationHook::cdmaInfoRec(V1_0::RadioIndicationType type,
        const ::android::hardware::radio::V1_0::CdmaInformationRecords& records) {
#if VDBG
    RLOGD("%s::%s mVoiceCb=%p", TAG, __FUNCTION__, mVoiceCb.get());
#endif
    if (mVoiceCb) {
        mVoiceCb->cdmaInfoRec(toAidl(type), toAidl(records.infoRec));
    }
    return Void();
}

Return<void> RadioVoiceIndicationHook::cdmaOtaProvisionStatus(V1_0::RadioIndicationType type,
        ::android::hardware::radio::V1_0::CdmaOtaProvisionStatus status) {
#if VDBG
    RLOGD("%s::%s mVoiceCb=%p", TAG, __FUNCTION__, mVoiceCb.get());
#endif
    if (mVoiceCb) {
        mVoiceCb->cdmaOtaProvisionStatus(toAidl(type), aidl_voice::CdmaOtaProvisionStatus(status));
    }
    return Void();
}

Return<void> RadioVoiceIndicationHook::currentEmergencyNumberList(V1_0::RadioIndicationType type,
        const ::android::hardware::hidl_vec<::android::hardware::radio::V1_4::EmergencyNumber>&
        emergencyNumberList) {
#if VDBG
    RLOGD("%s::%s mVoiceCb=%p", TAG, __FUNCTION__, mVoiceCb.get());
#endif
    if (mVoiceCb) {
        mVoiceCb->currentEmergencyNumberList(toAidl(type), toAidl(emergencyNumberList));
    }
    return Void();
}

Return<void> RadioVoiceIndicationHook::enterEmergencyCallbackMode(V1_0::RadioIndicationType type) {
#if VDBG
    RLOGD("%s::%s mVoiceCb=%p", TAG, __FUNCTION__, mVoiceCb.get());
#endif
    if (mVoiceCb) {
        mVoiceCb->enterEmergencyCallbackMode(toAidl(type));
    }
    return Void();
}

Return<void> RadioVoiceIndicationHook::exitEmergencyCallbackMode(V1_0::RadioIndicationType type) {
#if VDBG
    RLOGD("%s::%s mVoiceCb=%p", TAG, __FUNCTION__, mVoiceCb.get());
#endif
    if (mVoiceCb) {
        mVoiceCb->exitEmergencyCallbackMode(toAidl(type));
    }
    return Void();
}

Return<void> RadioVoiceIndicationHook::indicateRingbackTone(V1_0::RadioIndicationType type, bool start) {
#if VDBG
    RLOGD("%s::%s mVoiceCb=%p", TAG, __FUNCTION__, mVoiceCb.get());
#endif
    if (mVoiceCb) {
        mVoiceCb->indicateRingbackTone(toAidl(type), start);
    }
    return Void();
}

Return<void> RadioVoiceIndicationHook::onSupplementaryServiceIndication(V1_0::RadioIndicationType type,
        const ::android::hardware::radio::V1_0::StkCcUnsolSsResult& ss) {
#if VDBG
    RLOGD("%s::%s mVoiceCb=%p", TAG, __FUNCTION__, mVoiceCb.get());
#endif
    if (mVoiceCb) {
        mVoiceCb->onSupplementaryServiceIndication(toAidl(type), toAidl(ss));
    }
    return Void();
}

Return<void> RadioVoiceIndicationHook::onUssd(V1_0::RadioIndicationType type, V1_0::UssdModeType modeType,
        const ::android::hardware::hidl_string &msg) {
#if VDBG
    RLOGD("%s::%s mVoiceCb=%p", TAG, __FUNCTION__, mVoiceCb.get());
#endif
    if (mVoiceCb) {
        mVoiceCb->onUssd(toAidl(type), ((aidl_voice::UssdModeType)modeType), msg);
    }
    return Void();
}

Return<void> RadioVoiceIndicationHook::resendIncallMute(V1_0::RadioIndicationType type) {
#if VDBG
    RLOGD("%s::%s mVoiceCb=%p", TAG, __FUNCTION__, mVoiceCb.get());
#endif
    if (mVoiceCb) {
        mVoiceCb->resendIncallMute(toAidl(type));
    }
    return Void();
}

Return<void> RadioVoiceIndicationHook::srvccStateNotify(V1_0::RadioIndicationType type,
        ::android::hardware::radio::V1_0::SrvccState state) {
#if VDBG
    RLOGD("%s::%s mVoiceCb=%p", TAG, __FUNCTION__, mVoiceCb.get());
#endif
    if (mVoiceCb) {
        mVoiceCb->srvccStateNotify(toAidl(type), aidl_voice::SrvccState(state));
    }
    return Void();
}

Return<void> RadioVoiceIndicationHook::stkCallControlAlphaNotify(V1_0::RadioIndicationType type,
        const ::android::hardware::hidl_string& alpha) {
#if VDBG
    RLOGD("%s::%s mVoiceCb=%p", TAG, __FUNCTION__, mVoiceCb.get());
#endif
    if (mVoiceCb) {
        mVoiceCb->stkCallControlAlphaNotify(toAidl(type), alpha);
    }
    return Void();
}

Return<void> RadioVoiceIndicationHook::stkCallSetup(V1_0::RadioIndicationType type, int64_t timeout) {
#if VDBG
    RLOGD("%s::%s mVoiceCb=%p", TAG, __FUNCTION__, mVoiceCb.get());
#endif
    if (mVoiceCb) {
        mVoiceCb->stkCallSetup(toAidl(type), timeout);
    }
    return Void();
}
