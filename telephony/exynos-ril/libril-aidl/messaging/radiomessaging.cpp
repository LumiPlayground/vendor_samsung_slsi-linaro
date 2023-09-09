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
#include "radiomessaging.h"
#include "radiomessagingindication.h"
#include "radiomessagingresponse.h"

#define TAG "RadioMessaging"

namespace android::hardware::radio::impl {

using namespace std::string_literals;
using ::ndk::ScopedAStatus;
namespace aidl = ::aidl::android::hardware::radio::messaging;
constexpr auto ok = &ScopedAStatus::ok;

RadioMessaging::RadioMessaging(std::shared_ptr<RadioMessagingCallback> radio, uint32_t /* slot */)
    : mRadio(radio) {
    mCallbacks = {
        mRadio,
        std::make_shared<RadioMessagingResponse>(),
        std::make_shared<RadioMessagingIndication>()
    };
}

ScopedAStatus RadioMessaging::setResponseFunctions(
    const std::shared_ptr<aidl::IRadioMessagingResponse> &radioMessagingResponse,
    const std::shared_ptr<aidl::IRadioMessagingIndication> &radioMessagingIndication) {
#if VDBG
    RLOGD("%s::%s radioMessagingResponse=%p radioMessagingIndication=%p", TAG, __FUNCTION__,
          radioMessagingResponse.get(), radioMessagingIndication.get());
#endif
    mCallbacks.setResponseFunctions(radioMessagingResponse, radioMessagingIndication);
    return ok();
}

ScopedAStatus RadioMessaging::acknowledgeIncomingGsmSmsWithPdu(int32_t serial, bool success, const std::string &ackPdu) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->acknowledgeIncomingGsmSmsWithPdu(serial, success, ackPdu);
    return ok();
}

ScopedAStatus RadioMessaging::acknowledgeLastIncomingCdmaSms(int32_t serial, const aidl::CdmaSmsAck &smsAck) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->acknowledgeLastIncomingCdmaSms(serial, smsAck);
    return ok();
}

ScopedAStatus RadioMessaging::acknowledgeLastIncomingGsmSms(int32_t serial, bool success, aidl::SmsAcknowledgeFailCause cause) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->acknowledgeLastIncomingGsmSms(serial, success, cause);
    return ok();
}

ScopedAStatus RadioMessaging::deleteSmsOnRuim(int32_t serial, int32_t index) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->deleteSmsOnRuim(serial, index);
    return ok();
}

ScopedAStatus RadioMessaging::deleteSmsOnSim(int32_t serial, int32_t index) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->deleteSmsOnSim(serial, index);
    return ok();
}

ScopedAStatus RadioMessaging::getCdmaBroadcastConfig(int32_t serial) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->getCdmaBroadcastConfig(serial);
    return ok();
}

ScopedAStatus RadioMessaging::getGsmBroadcastConfig(int32_t serial) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->getGsmBroadcastConfig(serial);
    return ok();
}

ScopedAStatus RadioMessaging::getSmscAddress(int32_t serial) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->getSmscAddress(serial);
    return ok();
}

ScopedAStatus RadioMessaging::reportSmsMemoryStatus(int32_t serial, bool available) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->reportSmsMemoryStatus(serial, available);
    return ok();
}

ScopedAStatus RadioMessaging::responseAcknowledgement() {
    return ok();
}

ScopedAStatus RadioMessaging::sendCdmaSms(int32_t serial, const aidl::CdmaSmsMessage &sms) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->sendCdmaSms(serial, sms);
    return ok();
}

ScopedAStatus RadioMessaging::sendCdmaSmsExpectMore(int32_t serial, const aidl::CdmaSmsMessage &sms) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->sendCdmaSmsExpectMore(serial, sms);
    return ok();
}

ScopedAStatus RadioMessaging::sendImsSms(int32_t serial, const aidl::ImsSmsMessage &message) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->sendImsSms(serial, message);
    return ok();
}

ScopedAStatus RadioMessaging::sendSms(int32_t serial, const aidl::GsmSmsMessage &message) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->sendSms(serial, message);
    return ok();
}

ScopedAStatus RadioMessaging::sendSmsExpectMore(int32_t serial, const aidl::GsmSmsMessage &message) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->sendSmsExpectMore(serial, message);
    return ok();
}

ScopedAStatus RadioMessaging::setCdmaBroadcastActivation(int32_t serial, bool activate) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->setCdmaBroadcastActivation(serial, activate);
    return ok();
}

ScopedAStatus RadioMessaging::setCdmaBroadcastConfig(int32_t serial, const std::vector<aidl::CdmaBroadcastSmsConfigInfo> &configInfo) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->setCdmaBroadcastConfig(serial, configInfo);
    return ok();
}

ScopedAStatus RadioMessaging::setGsmBroadcastActivation(int32_t serial, bool activate) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->setGsmBroadcastActivation(serial, activate);
    return ok();
}

ScopedAStatus RadioMessaging::setGsmBroadcastConfig(int32_t serial, const std::vector<aidl::GsmBroadcastSmsConfigInfo> &configInfo) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->setGsmBroadcastConfig(serial, configInfo);
    return ok();
}

ScopedAStatus RadioMessaging::setSmscAddress(int32_t serial, const std::string &smsc) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->setSmscAddress(serial, smsc);
    return ok();
}

ScopedAStatus RadioMessaging::writeSmsToRuim(int32_t serial, const aidl::CdmaSmsWriteArgs &cdmaSms) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->writeSmsToRuim(serial, cdmaSms);
    return ok();
}

ScopedAStatus RadioMessaging::writeSmsToSim(int32_t serial, const aidl::SmsWriteArgs &smsWriteArgs) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->writeSmsToSim(serial, smsWriteArgs);
    return ok();
}

std::shared_ptr<RadioMessaging> RadioMessaging::make(std::shared_ptr<RadioMessagingCallback> radio, uint32_t slot) {
#if VDBG
    RLOGD("%s::%s slot %u", TAG, __FUNCTION__, slot);
#endif
    return ndk::SharedRefBase::make<RadioMessaging>(radio, slot);
}

}  // namespace android::hardware::radio::impl
