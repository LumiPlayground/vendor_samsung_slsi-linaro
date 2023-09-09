/*
 * Copyright (C) 2021 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <aidl-service/aidl-log.h>

#include "radiomessagingresponse.h"
#include "commonStructs.h"
#include "structs.h"
#include "collections.h"
#include <ril_internal.h>

using namespace ::android::hardware::radio::V1_6;
using namespace ::android::hardware::radio::V1_5;
using namespace ::android::hardware::radio::V1_4;
using namespace ::android::hardware::radio::V1_3;
using namespace ::android::hardware::radio::V1_2;
using namespace ::android::hardware::radio::V1_1;
using namespace ::android::hardware::radio::V1_0;

using namespace ::android::hardware::radio;
using namespace ::android::hardware::radio::compat;
using namespace ::android::hardware::radio::impl;
using ::android::hardware::hidl_string;
using ::android::hardware::Return;
using ::android::hardware::Void;

#define TAG "RadioMessagingResponseHook"

void RadioMessagingResponseHook::setResponseFunction(std::shared_ptr<RadioMessagingResponseCallback> messagingCb) {
    RLOGD("%s::%s messagingCb=%p", TAG, __FUNCTION__, messagingCb.get());
    mMessagingCb = messagingCb;
}

Return<void> RadioMessagingResponseHook::acknowledgeIncomingGsmSmsWithPduResponse(const V1_0::RadioResponseInfo &info) {
#if VDBG
    RLOGD("%s::%s mMessagingCb=%p", TAG, __FUNCTION__, mMessagingCb.get());
#endif
    if (mMessagingCb) {
        mMessagingCb->acknowledgeIncomingGsmSmsWithPduResponse(toAidl(info));
    }
    return Void();
}

Return<void> RadioMessagingResponseHook::acknowledgeLastIncomingCdmaSmsResponse(const V1_0::RadioResponseInfo &info) {
#if VDBG
    RLOGD("%s::%s mMessagingCb=%p", TAG, __FUNCTION__, mMessagingCb.get());
#endif
    if (mMessagingCb) {
        mMessagingCb->acknowledgeLastIncomingCdmaSmsResponse(toAidl(info));
    }
    return Void();
}

Return<void> RadioMessagingResponseHook::acknowledgeLastIncomingGsmSmsResponse(const V1_0::RadioResponseInfo &info) {
#if VDBG
    RLOGD("%s::%s mMessagingCb=%p", TAG, __FUNCTION__, mMessagingCb.get());
#endif
    if (mMessagingCb) {
        mMessagingCb->acknowledgeLastIncomingGsmSmsResponse(toAidl(info));
    }
    return Void();
}

Return<void> RadioMessagingResponseHook::deleteSmsOnRuimResponse(const V1_0::RadioResponseInfo &info) {
#if VDBG
    RLOGD("%s::%s mMessagingCb=%p", TAG, __FUNCTION__, mMessagingCb.get());
#endif
    if (mMessagingCb) {
        mMessagingCb->deleteSmsOnRuimResponse(toAidl(info));
    }
    return Void();
}

Return<void> RadioMessagingResponseHook::deleteSmsOnSimResponse(const V1_0::RadioResponseInfo &info) {
#if VDBG
    RLOGD("%s::%s mMessagingCb=%p", TAG, __FUNCTION__, mMessagingCb.get());
#endif
    if (mMessagingCb) {
        mMessagingCb->deleteSmsOnSimResponse(toAidl(info));
    }
    return Void();
}

Return<void> RadioMessagingResponseHook::getCdmaBroadcastConfigResponse(const V1_0::RadioResponseInfo &info, const ::android::hardware::hidl_vec<CdmaBroadcastSmsConfigInfo> &configs) {
#if VDBG
    RLOGD("%s::%s mMessagingCb=%p", TAG, __FUNCTION__, mMessagingCb.get());
#endif
    if (mMessagingCb) {
        mMessagingCb->getCdmaBroadcastConfigResponse(toAidl(info), toAidl(configs));
    }
    return Void();
}

Return<void> RadioMessagingResponseHook::getGsmBroadcastConfigResponse(const V1_0::RadioResponseInfo &info, const ::android::hardware::hidl_vec<GsmBroadcastSmsConfigInfo> &configs) {
#if VDBG
    RLOGD("%s::%s mMessagingCb=%p", TAG, __FUNCTION__, mMessagingCb.get());
#endif
    if (mMessagingCb) {
        mMessagingCb->getGsmBroadcastConfigResponse(toAidl(info), toAidl(configs));
    }
    return Void();
}

Return<void> RadioMessagingResponseHook::getSmscAddressResponse(const V1_0::RadioResponseInfo &info, const hidl_string &smsc) {
#if VDBG
    RLOGD("%s::%s mMessagingCb=%p", TAG, __FUNCTION__, mMessagingCb.get());
#endif
    if (mMessagingCb) {
        mMessagingCb->getSmscAddressResponse(toAidl(info), smsc);
    }
    return Void();
}

Return<void> RadioMessagingResponseHook::reportSmsMemoryStatusResponse(const V1_0::RadioResponseInfo &info) {
#if VDBG
    RLOGD("%s::%s mMessagingCb=%p", TAG, __FUNCTION__, mMessagingCb.get());
#endif
    if (mMessagingCb) {
        mMessagingCb->reportSmsMemoryStatusResponse(toAidl(info));
    }
    return Void();
}

Return<void> RadioMessagingResponseHook::sendCdmaSmsExpectMoreResponse(const V1_0::RadioResponseInfo &info, const SendSmsResult &sms) {
#if VDBG
    RLOGD("%s::%s mMessagingCb=%p", TAG, __FUNCTION__, mMessagingCb.get());
#endif
    if (mMessagingCb) {
        mMessagingCb->sendCdmaSmsExpectMoreResponse(toAidl(info), toAidl(sms));
    }
    return Void();
}

Return<void> RadioMessagingResponseHook::sendCdmaSmsExpectMoreResponse_1_6(const V1_6::RadioResponseInfo &info, const SendSmsResult &sms) {
#if VDBG
    RLOGD("%s::%s mMessagingCb=%p", TAG, __FUNCTION__, mMessagingCb.get());
#endif
    if (mMessagingCb) {
        mMessagingCb->sendCdmaSmsExpectMoreResponse(toAidl(info), toAidl(sms));
    }
    return Void();
}

Return<void> RadioMessagingResponseHook::sendCdmaSmsResponse(const V1_0::RadioResponseInfo &info, const SendSmsResult &sms) {
#if VDBG
    RLOGD("%s::%s mMessagingCb=%p", TAG, __FUNCTION__, mMessagingCb.get());
#endif
    if (mMessagingCb) {
        mMessagingCb->sendCdmaSmsResponse(toAidl(info), toAidl(sms));
    }
    return Void();
}

Return<void> RadioMessagingResponseHook::sendCdmaSmsResponse_1_6(const V1_6::RadioResponseInfo &info, const SendSmsResult &sms) {
#if VDBG
    RLOGD("%s::%s mMessagingCb=%p", TAG, __FUNCTION__, mMessagingCb.get());
#endif
    if (mMessagingCb) {
        mMessagingCb->sendCdmaSmsResponse(toAidl(info), toAidl(sms));
    }
    return Void();
}

Return<void> RadioMessagingResponseHook::sendImsSmsResponse(const V1_0::RadioResponseInfo &info, const SendSmsResult &sms) {
#if VDBG
    RLOGD("%s::%s mMessagingCb=%p", TAG, __FUNCTION__, mMessagingCb.get());
#endif
    if (mMessagingCb) {
        mMessagingCb->sendImsSmsResponse(toAidl(info), toAidl(sms));
    }
    return Void();
}

Return<void> RadioMessagingResponseHook::sendSMSExpectMoreResponse(const V1_0::RadioResponseInfo &info, const SendSmsResult &sms) {
#if VDBG
    RLOGD("%s::%s mMessagingCb=%p", TAG, __FUNCTION__, mMessagingCb.get());
#endif
    if (mMessagingCb) {
        mMessagingCb->sendSmsExpectMoreResponse(toAidl(info), toAidl(sms));
    }
    return Void();
}

Return<void> RadioMessagingResponseHook::sendSmsExpectMoreResponse_1_6(const V1_6::RadioResponseInfo &info, const SendSmsResult &sms) {
#if VDBG
    RLOGD("%s::%s mMessagingCb=%p", TAG, __FUNCTION__, mMessagingCb.get());
#endif
    if (mMessagingCb) {
        mMessagingCb->sendSmsExpectMoreResponse(toAidl(info), toAidl(sms));
    }
    return Void();
}

Return<void> RadioMessagingResponseHook::sendSmsResponse(const V1_0::RadioResponseInfo &info, const SendSmsResult &sms) {
#if VDBG
    RLOGD("%s::%s mMessagingCb=%p", TAG, __FUNCTION__, mMessagingCb.get());
#endif
    if (mMessagingCb) {
        mMessagingCb->sendSmsResponse(toAidl(info), toAidl(sms));
    }
    return Void();
}

Return<void> RadioMessagingResponseHook::sendSmsResponse_1_6(const V1_6::RadioResponseInfo &info, const SendSmsResult &sms) {
#if VDBG
    RLOGD("%s::%s mMessagingCb=%p", TAG, __FUNCTION__, mMessagingCb.get());
#endif
    if (mMessagingCb) {
        mMessagingCb->sendSmsResponse(toAidl(info), toAidl(sms));
    }
    return Void();
}

Return<void> RadioMessagingResponseHook::setCdmaBroadcastActivationResponse(const V1_0::RadioResponseInfo &info) {
#if VDBG
    RLOGD("%s::%s mMessagingCb=%p", TAG, __FUNCTION__, mMessagingCb.get());
#endif
    if (mMessagingCb) {
        mMessagingCb->setCdmaBroadcastActivationResponse(toAidl(info));
    }
    return Void();
}

Return<void> RadioMessagingResponseHook::setCdmaBroadcastConfigResponse(const V1_0::RadioResponseInfo &info) {
#if VDBG
    RLOGD("%s::%s mMessagingCb=%p", TAG, __FUNCTION__, mMessagingCb.get());
#endif
    if (mMessagingCb) {
        mMessagingCb->setCdmaBroadcastConfigResponse(toAidl(info));
    }
    return Void();
}

Return<void> RadioMessagingResponseHook::setGsmBroadcastActivationResponse(const V1_0::RadioResponseInfo &info) {
#if VDBG
    RLOGD("%s::%s mMessagingCb=%p", TAG, __FUNCTION__, mMessagingCb.get());
#endif
    if (mMessagingCb) {
        mMessagingCb->setGsmBroadcastActivationResponse(toAidl(info));
    }
    return Void();
}

Return<void> RadioMessagingResponseHook::setGsmBroadcastConfigResponse(const V1_0::RadioResponseInfo &info) {
#if VDBG
    RLOGD("%s::%s mMessagingCb=%p", TAG, __FUNCTION__, mMessagingCb.get());
#endif
    if (mMessagingCb) {
        mMessagingCb->setGsmBroadcastConfigResponse(toAidl(info));
    }
    return Void();
}

Return<void> RadioMessagingResponseHook::setSmscAddressResponse(const V1_0::RadioResponseInfo &info) {
#if VDBG
    RLOGD("%s::%s mMessagingCb=%p", TAG, __FUNCTION__, mMessagingCb.get());
#endif
    if (mMessagingCb) {
        mMessagingCb->setSmscAddressResponse(toAidl(info));
    }
    return Void();
}

Return<void> RadioMessagingResponseHook::writeSmsToRuimResponse(const V1_0::RadioResponseInfo &info, uint32_t index) {
#if VDBG
    RLOGD("%s::%s mMessagingCb=%p", TAG, __FUNCTION__, mMessagingCb.get());
#endif
    if (mMessagingCb) {
        mMessagingCb->writeSmsToRuimResponse(toAidl(info), index);
    }
    return Void();
}

Return<void> RadioMessagingResponseHook::writeSmsToSimResponse(const V1_0::RadioResponseInfo &info, int32_t index) {
#if VDBG
    RLOGD("%s::%s mMessagingCb=%p", TAG, __FUNCTION__, mMessagingCb.get());
#endif
    if (mMessagingCb) {
        mMessagingCb->writeSmsToSimResponse(toAidl(info), index);
    }
    return Void();
}
