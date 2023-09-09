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
#include "radiomessagingproxy.h"
#include "structs.h"
#include "collections.h"
#include <ril_internal.h>

using namespace ::android::hardware::radio;
namespace impl = android::hardware::radio::impl;
namespace aidl_messaging = ::aidl::android::hardware::radio::messaging;
using ::android::sp;

void RadioMessagingProxy::acknowledgeIncomingGsmSmsWithPdu(int32_t serial, bool success, const std::string &ackPdu) {
    mHidlRadio->acknowledgeIncomingGsmSmsWithPdu(serial, success, ackPdu);
}

void RadioMessagingProxy::acknowledgeLastIncomingCdmaSms(int32_t serial, const aidl_messaging::CdmaSmsAck &smsAck) {
    mHidlRadio->acknowledgeLastIncomingCdmaSms(serial, compat::toHidl(smsAck));
}

void RadioMessagingProxy::acknowledgeLastIncomingGsmSms(int32_t serial, bool success, aidl_messaging::SmsAcknowledgeFailCause cause) {
    mHidlRadio->acknowledgeLastIncomingGsmSms(serial, success, V1_0::SmsAcknowledgeFailCause(cause));
}

void RadioMessagingProxy::deleteSmsOnRuim(int32_t serial, int32_t index) {
    mHidlRadio->deleteSmsOnRuim(serial, index);
}

void RadioMessagingProxy::deleteSmsOnSim(int32_t serial, int32_t index) {
    mHidlRadio->deleteSmsOnSim(serial, index);
}

void RadioMessagingProxy::getCdmaBroadcastConfig(int32_t serial) {
    mHidlRadio->getCdmaBroadcastConfig(serial);
}

void RadioMessagingProxy::getGsmBroadcastConfig(int32_t serial) {
    mHidlRadio->getGsmBroadcastConfig(serial);
}

void RadioMessagingProxy::getSmscAddress(int32_t serial) {
    mHidlRadio->getSmscAddress(serial);
}

void RadioMessagingProxy::reportSmsMemoryStatus(int32_t serial, bool available) {
    mHidlRadio->reportSmsMemoryStatus(serial, available);
}

void RadioMessagingProxy::responseAcknowledgement() {
    mHidlRadio->responseAcknowledgement();
}

void RadioMessagingProxy::sendCdmaSms(int32_t serial, const aidl_messaging::CdmaSmsMessage &sms) {
    mHidlRadio->sendCdmaSms_1_6(serial, compat::toHidl(sms));
}

void RadioMessagingProxy::sendCdmaSmsExpectMore(int32_t serial, const aidl_messaging::CdmaSmsMessage &sms) {
    mHidlRadio->sendCdmaSmsExpectMore_1_6(serial, compat::toHidl(sms));
}

void RadioMessagingProxy::sendImsSms(int32_t serial, const aidl_messaging::ImsSmsMessage &message) {
    mHidlRadio->sendImsSms(serial, compat::toHidl(message));
}

void RadioMessagingProxy::sendSms(int32_t serial, const aidl_messaging::GsmSmsMessage &message) {
    mHidlRadio->sendSms_1_6(serial, compat::toHidl(message));
}

void RadioMessagingProxy::sendSmsExpectMore(int32_t serial, const aidl_messaging::GsmSmsMessage &message) {
    mHidlRadio->sendSmsExpectMore_1_6(serial, compat::toHidl(message));
}

void RadioMessagingProxy::setCdmaBroadcastActivation(int32_t serial, bool activate) {
    mHidlRadio->setCdmaBroadcastActivation(serial, activate);
}

void RadioMessagingProxy::setCdmaBroadcastConfig(int32_t serial, const std::vector<aidl_messaging::CdmaBroadcastSmsConfigInfo> &configInfo) {
    mHidlRadio->setCdmaBroadcastConfig(serial, compat::toHidl(configInfo));
}

void RadioMessagingProxy::setGsmBroadcastActivation(int32_t serial, bool activate) {
    mHidlRadio->setGsmBroadcastActivation(serial, activate);
}

void RadioMessagingProxy::setGsmBroadcastConfig(int32_t serial, const std::vector<aidl_messaging::GsmBroadcastSmsConfigInfo> &configInfo) {
    mHidlRadio->setGsmBroadcastConfig(serial, compat::toHidl(configInfo));
}

void RadioMessagingProxy::setSmscAddress(int32_t serial, const std::string &smsc) {
    mHidlRadio->setSmscAddress(serial, smsc);
}

void RadioMessagingProxy::writeSmsToRuim(int32_t serial, const aidl_messaging::CdmaSmsWriteArgs &cdmaSms) {
    mHidlRadio->writeSmsToRuim(serial, compat::toHidl(cdmaSms));
}

void RadioMessagingProxy::writeSmsToSim(int32_t serial, const aidl_messaging::SmsWriteArgs &smsWriteArgs) {
    mHidlRadio->writeSmsToSim(serial, compat::toHidl(smsWriteArgs));
}
