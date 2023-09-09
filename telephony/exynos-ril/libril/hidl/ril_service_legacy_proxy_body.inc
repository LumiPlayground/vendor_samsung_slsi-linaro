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

Return<void> RadioImpl::getIccCardStatus(int32_t serial) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->getIccCardStatus(serial);
    }
    return Void();
}

Return<void> RadioImpl::supplyIccPinForApp(int32_t serial, const hidl_string& pin,
        const hidl_string& aid) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->supplyIccPinForApp(serial, pin, aid);
    }
    return Void();
}

Return<void> RadioImpl::supplyIccPukForApp(int32_t serial, const hidl_string& puk,
                                           const hidl_string& pin, const hidl_string& aid) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->supplyIccPukForApp(serial, puk, pin, aid);
    }
    return Void();
}

Return<void> RadioImpl::supplyIccPin2ForApp(int32_t serial, const hidl_string& pin2,
                                            const hidl_string& aid) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->supplyIccPin2ForApp(serial, pin2, aid);
    }
    return Void();
}

Return<void> RadioImpl::supplyIccPuk2ForApp(int32_t serial, const hidl_string& puk2,
                                            const hidl_string& pin2, const hidl_string& aid) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->supplyIccPuk2ForApp(serial, puk2, pin2, aid);
    }
    return Void();
}

Return<void> RadioImpl::changeIccPinForApp(int32_t serial, const hidl_string& oldPin,
                                           const hidl_string& newPin, const hidl_string& aid) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->changeIccPinForApp(serial, oldPin, newPin, aid);
    }
    return Void();
}

Return<void> RadioImpl::changeIccPin2ForApp(int32_t serial, const hidl_string& oldPin2,
                                            const hidl_string& newPin2, const hidl_string& aid) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->changeIccPin2ForApp(serial, oldPin2, newPin2, aid);
    }
    return Void();
}

Return<void> RadioImpl::supplyNetworkDepersonalization(int32_t serial,
                                                       const hidl_string& netPin) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->supplyNetworkDepersonalization(serial, netPin);
    }
    return Void();
}

Return<void> RadioImpl::getCurrentCalls(int32_t serial) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->getCurrentCalls(serial);
    }
    return Void();
}

Return<void> RadioImpl::dial(int32_t serial, const Dial& dialInfo) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->dial(serial, dialInfo);
    }
    return Void();
}

Return<void> RadioImpl::getImsiForApp(int32_t serial, const hidl_string& aid) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->getImsiForApp(serial, aid);
    }
    return Void();
}

Return<void> RadioImpl::hangup(int32_t serial, int32_t gsmIndex) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->hangup(serial, gsmIndex);
    }
    return Void();
}

Return<void> RadioImpl::hangupWaitingOrBackground(int32_t serial) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->hangupWaitingOrBackground(serial);
    }
    return Void();
}

Return<void> RadioImpl::hangupForegroundResumeBackground(int32_t serial) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->hangupForegroundResumeBackground(serial);
    }
    return Void();
}

Return<void> RadioImpl::switchWaitingOrHoldingAndActive(int32_t serial) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->switchWaitingOrHoldingAndActive(serial);
    }
    return Void();
}

Return<void> RadioImpl::conference(int32_t serial) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->conference(serial);
    }
    return Void();
}

Return<void> RadioImpl::rejectCall(int32_t serial) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->rejectCall(serial);
    }
    return Void();
}

Return<void> RadioImpl::getLastCallFailCause(int32_t serial) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->getLastCallFailCause(serial);
    }
    return Void();
}

Return<void> RadioImpl::getSignalStrength(int32_t serial) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->getSignalStrength(serial);
    }
    return Void();
}

Return<void> RadioImpl::getVoiceRegistrationState(int32_t serial) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->getVoiceRegistrationState(serial);
    }
    return Void();
}

Return<void> RadioImpl::getDataRegistrationState(int32_t serial) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->getDataRegistrationState(serial);
    }
    return Void();
}

Return<void> RadioImpl::getOperator(int32_t serial) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->getOperator(serial);
    }
    return Void();
}

Return<void> RadioImpl::setRadioPower(int32_t serial, bool on) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->setRadioPower(serial, on);
    }
    return Void();
}

Return<void> RadioImpl::sendDtmf(int32_t serial, const hidl_string& s) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->sendDtmf(serial, s);
    }
    return Void();
}

Return<void> RadioImpl::sendSms(int32_t serial, const GsmSmsMessage& message) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->sendSms(serial, message);
    }
    return Void();
}

Return<void> RadioImpl::sendSMSExpectMore(int32_t serial, const GsmSmsMessage& message) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->sendSMSExpectMore(serial, message);
    }
    return Void();
}

Return<void> RadioImpl::setupDataCall(int32_t serial, RadioTechnology radioTechnology,
                                      const DataProfileInfo& dataProfileInfo, bool modemCognitive,
                                      bool roamingAllowed, bool isRoaming) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->setupDataCall(serial, radioTechnology, dataProfileInfo,
                modemCognitive, roamingAllowed, isRoaming);
    }
    return Void();
}

Return<void> RadioImpl::iccIOForApp(int32_t serial, const IccIo& iccIo) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->iccIOForApp(serial, iccIo);
    }
    return Void();
}

Return<void> RadioImpl::sendUssd(int32_t serial, const hidl_string& ussd) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->sendUssd(serial, ussd);
    }
    return Void();
}

Return<void> RadioImpl::cancelPendingUssd(int32_t serial) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->cancelPendingUssd(serial);
    }
    return Void();
}

Return<void> RadioImpl::getClir(int32_t serial) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->getClir(serial);
    }
    return Void();
}

Return<void> RadioImpl::setClir(int32_t serial, int32_t status) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->setClir(serial, status);
    }
    return Void();
}

Return<void> RadioImpl::getCallForwardStatus(int32_t serial, const CallForwardInfo& callInfo) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->getCallForwardStatus(serial, callInfo);
    }
    return Void();
}

Return<void> RadioImpl::setCallForward(int32_t serial, const CallForwardInfo& callInfo) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->setCallForward(serial, callInfo);
    }
    return Void();
}

Return<void> RadioImpl::getCallWaiting(int32_t serial, int32_t serviceClass) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->getCallWaiting(serial, serviceClass);
    }
    return Void();
}

Return<void> RadioImpl::setCallWaiting(int32_t serial, bool enable, int32_t serviceClass) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->setCallWaiting(serial, enable, serviceClass);
    }
    return Void();
}

Return<void> RadioImpl::acknowledgeLastIncomingGsmSms(int32_t serial,
                                                      bool success, SmsAcknowledgeFailCause cause) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->acknowledgeLastIncomingGsmSms(serial, success, cause);
    }
    return Void();
}

Return<void> RadioImpl::acceptCall(int32_t serial) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->acceptCall(serial);
    }
    return Void();
}

Return<void> RadioImpl::deactivateDataCall(int32_t serial,
                                           int32_t cid, bool reasonRadioShutDown) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->deactivateDataCall(serial, cid, reasonRadioShutDown);
    }
    return Void();
}

Return<void> RadioImpl::getFacilityLockForApp(int32_t serial, const hidl_string& facility,
                                              const hidl_string& password, int32_t serviceClass,
                                              const hidl_string& appId) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->getFacilityLockForApp(serial, facility, password, serviceClass, appId);
    }
    return Void();
}

Return<void> RadioImpl::setFacilityLockForApp(int32_t serial, const hidl_string& facility,
                                              bool lockState, const hidl_string& password,
                                              int32_t serviceClass, const hidl_string& appId) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->setFacilityLockForApp(serial, facility, lockState,
                password, serviceClass, appId);
    }
    return Void();
}

Return<void> RadioImpl::setBarringPassword(int32_t serial, const hidl_string& facility,
                                           const hidl_string& oldPassword,
                                           const hidl_string& newPassword) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->setBarringPassword(serial, facility, oldPassword, newPassword);
    }
    return Void();
}

Return<void> RadioImpl::getNetworkSelectionMode(int32_t serial) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->getNetworkSelectionMode(serial);
    }
    return Void();
}

Return<void> RadioImpl::setNetworkSelectionModeAutomatic(int32_t serial) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->setNetworkSelectionModeAutomatic(serial);
    }
    return Void();
}

Return<void> RadioImpl::setNetworkSelectionModeManual(int32_t serial,
                                                      const hidl_string& operatorNumeric) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->setNetworkSelectionModeManual(serial, operatorNumeric);
    }
    return Void();
}

Return<void> RadioImpl::getAvailableNetworks(int32_t serial) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->getAvailableNetworks(serial);
    }
    return Void();
}

Return<void> RadioImpl::startNetworkScan(int32_t serial, const V1_1::NetworkScanRequest& request) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->startNetworkScan(serial, request);
    }
    return Void();
}

Return<void> RadioImpl::stopNetworkScan(int32_t serial) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->stopNetworkScan(serial);
    }
    return Void();
}

Return<void> RadioImpl::startDtmf(int32_t serial, const hidl_string& s) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->startDtmf(serial, s);
    }
    return Void();
}

Return<void> RadioImpl::stopDtmf(int32_t serial) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->stopDtmf(serial);
    }
    return Void();
}

Return<void> RadioImpl::getBasebandVersion(int32_t serial) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->getBasebandVersion(serial);
    }
    return Void();
}

Return<void> RadioImpl::separateConnection(int32_t serial, int32_t gsmIndex) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->separateConnection(serial, gsmIndex);
    }
    return Void();
}

Return<void> RadioImpl::setMute(int32_t serial, bool enable) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->setMute(serial, enable);
    }
    return Void();
}

Return<void> RadioImpl::getMute(int32_t serial) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->getMute(serial);
    }
    return Void();
}

Return<void> RadioImpl::getClip(int32_t serial) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->getClip(serial);
    }
    return Void();
}

Return<void> RadioImpl::getDataCallList(int32_t serial) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->getDataCallList(serial);
    }
    return Void();
}

Return<void> RadioImpl::setSuppServiceNotifications(int32_t serial, bool enable) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->setSuppServiceNotifications(serial, enable);
    }
    return Void();
}

Return<void> RadioImpl::writeSmsToSim(int32_t serial, const SmsWriteArgs& smsWriteArgs) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->writeSmsToSim(serial, smsWriteArgs);
    }
    return Void();
}

Return<void> RadioImpl::deleteSmsOnSim(int32_t serial, int32_t index) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->deleteSmsOnSim(serial, index);
    }
    return Void();
}

Return<void> RadioImpl::setBandMode(int32_t serial, RadioBandMode mode) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->setBandMode(serial, mode);
    }
    return Void();
}

Return<void> RadioImpl::getAvailableBandModes(int32_t serial) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->getAvailableBandModes(serial);
    }
    return Void();
}

Return<void> RadioImpl::sendEnvelope(int32_t serial, const hidl_string& command) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->sendEnvelope(serial, command);
    }
    return Void();
}

Return<void> RadioImpl::sendTerminalResponseToSim(int32_t serial,
                                                  const hidl_string& commandResponse) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->sendTerminalResponseToSim(serial, commandResponse);
    }
    return Void();
}

Return<void> RadioImpl::handleStkCallSetupRequestFromSim(int32_t serial, bool accept) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->handleStkCallSetupRequestFromSim(serial, accept);
    }
    return Void();
}

Return<void> RadioImpl::explicitCallTransfer(int32_t serial) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->explicitCallTransfer(serial);
    }
    return Void();
}

Return<void> RadioImpl::setPreferredNetworkType(int32_t serial, PreferredNetworkType nwType) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->setPreferredNetworkType(serial, nwType);
    }
    return Void();
}

Return<void> RadioImpl::getPreferredNetworkType(int32_t serial) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->getPreferredNetworkType(serial);
    }
    return Void();
}

Return<void> RadioImpl::getNeighboringCids(int32_t serial) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->getNeighboringCids(serial);
    }
    return Void();
}

Return<void> RadioImpl::setLocationUpdates(int32_t serial, bool enable) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->setLocationUpdates(serial, enable);
    }
    return Void();
}

Return<void> RadioImpl::setCdmaSubscriptionSource(int32_t serial, CdmaSubscriptionSource cdmaSub) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->setCdmaSubscriptionSource(serial, cdmaSub);
    }
    return Void();
}

Return<void> RadioImpl::setCdmaRoamingPreference(int32_t serial, CdmaRoamingType type) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->setCdmaRoamingPreference(serial, type);
    }
    return Void();
}

Return<void> RadioImpl::getCdmaRoamingPreference(int32_t serial) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->getCdmaRoamingPreference(serial);
    }
    return Void();
}

Return<void> RadioImpl::setTTYMode(int32_t serial, TtyMode mode) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->setTTYMode(serial, mode);
    }
    return Void();
}

Return<void> RadioImpl::getTTYMode(int32_t serial) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->getTTYMode(serial);
    }
    return Void();
}

Return<void> RadioImpl::setPreferredVoicePrivacy(int32_t serial, bool enable) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->setPreferredVoicePrivacy(serial, enable);
    }
    return Void();
}

Return<void> RadioImpl::getPreferredVoicePrivacy(int32_t serial) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->getPreferredVoicePrivacy(serial);
    }
    return Void();
}

Return<void> RadioImpl::sendCDMAFeatureCode(int32_t serial, const hidl_string& featureCode) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->sendCDMAFeatureCode(serial, featureCode);
    }
    return Void();
}

Return<void> RadioImpl::sendBurstDtmf(int32_t serial, const hidl_string& dtmf, int32_t on,
                                      int32_t off) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->sendBurstDtmf(serial, dtmf, on, off);
    }
    return Void();
}

Return<void> RadioImpl::sendCdmaSms(int32_t serial, const CdmaSmsMessage& sms) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->sendCdmaSms(serial, sms);
    }
    return Void();
}

Return<void> RadioImpl::acknowledgeLastIncomingCdmaSms(int32_t serial, const CdmaSmsAck& smsAck) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->acknowledgeLastIncomingCdmaSms(serial, smsAck);
    }
    return Void();
}

Return<void> RadioImpl::getGsmBroadcastConfig(int32_t serial) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->getGsmBroadcastConfig(serial);
    }
    return Void();
}

Return<void> RadioImpl::setGsmBroadcastConfig(int32_t serial,
                                              const hidl_vec<GsmBroadcastSmsConfigInfo>&
                                              configInfo) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->setGsmBroadcastConfig(serial, configInfo);
    }
    return Void();
}

Return<void> RadioImpl::setGsmBroadcastActivation(int32_t serial, bool activate) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->setGsmBroadcastActivation(serial, activate);
    }
    return Void();
}

Return<void> RadioImpl::getCdmaBroadcastConfig(int32_t serial) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->getCdmaBroadcastConfig(serial);
    }
    return Void();
}

Return<void> RadioImpl::setCdmaBroadcastConfig(int32_t serial,
                                               const hidl_vec<CdmaBroadcastSmsConfigInfo>&
                                               configInfo) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->setCdmaBroadcastConfig(serial, configInfo);
    }
    return Void();
}

Return<void> RadioImpl::setCdmaBroadcastActivation(int32_t serial, bool activate) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->setCdmaBroadcastActivation(serial, activate);
    }
    return Void();
}

Return<void> RadioImpl::getCDMASubscription(int32_t serial) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->getCDMASubscription(serial);
    }
    return Void();
}

Return<void> RadioImpl::writeSmsToRuim(int32_t serial, const CdmaSmsWriteArgs& cdmaSms) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->writeSmsToRuim(serial, cdmaSms);
    }
    return Void();
}

Return<void> RadioImpl::deleteSmsOnRuim(int32_t serial, int32_t index) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->deleteSmsOnRuim(serial, index);
    }
    return Void();
}

Return<void> RadioImpl::getDeviceIdentity(int32_t serial) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->getDeviceIdentity(serial);
    }
    return Void();
}

Return<void> RadioImpl::exitEmergencyCallbackMode(int32_t serial) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->exitEmergencyCallbackMode(serial);
    }
    return Void();
}

Return<void> RadioImpl::getSmscAddress(int32_t serial) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->getSmscAddress(serial);
    }
    return Void();
}

Return<void> RadioImpl::setSmscAddress(int32_t serial, const hidl_string& smsc) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->setSmscAddress(serial, smsc);
    }
    return Void();
}

Return<void> RadioImpl::reportSmsMemoryStatus(int32_t serial, bool available) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->reportSmsMemoryStatus(serial, available);
    }
    return Void();
}

Return<void> RadioImpl::reportStkServiceIsRunning(int32_t serial) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->reportStkServiceIsRunning(serial);
    }
    return Void();
}

Return<void> RadioImpl::getCdmaSubscriptionSource(int32_t serial) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->getCdmaSubscriptionSource(serial);
    }
    return Void();
}

Return<void> RadioImpl::requestIsimAuthentication(int32_t serial, const hidl_string& challenge) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->requestIsimAuthentication(serial, challenge);
    }
    return Void();
}

Return<void> RadioImpl::acknowledgeIncomingGsmSmsWithPdu(int32_t serial, bool success,
                                                         const hidl_string& ackPdu) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->acknowledgeIncomingGsmSmsWithPdu(serial, success, ackPdu);
    }
    return Void();
}

Return<void> RadioImpl::sendEnvelopeWithStatus(int32_t serial, const hidl_string& contents) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->sendEnvelopeWithStatus(serial, contents);
    }
    return Void();
}

Return<void> RadioImpl::getVoiceRadioTechnology(int32_t serial) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->getVoiceRadioTechnology(serial);
    }
    return Void();
}

Return<void> RadioImpl::getCellInfoList(int32_t serial) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->getCellInfoList(serial);
    }
    return Void();
}

Return<void> RadioImpl::setCellInfoListRate(int32_t serial, int32_t rate) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->setCellInfoListRate(serial, rate);
    }
    return Void();
}

Return<void> RadioImpl::setInitialAttachApn(int32_t serial, const DataProfileInfo& dataProfileInfo,
                                            bool modemCognitive, bool isRoaming) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->setInitialAttachApn(serial, dataProfileInfo, modemCognitive, isRoaming);
    }
    return Void();
}

Return<void> RadioImpl::getImsRegistrationState(int32_t serial) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->getImsRegistrationState(serial);
    }
    return Void();
}

Return<void> RadioImpl::sendImsSms(int32_t serial, const ImsSmsMessage& message) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->sendImsSms(serial, message);
    }
    return Void();
}

Return<void> RadioImpl::iccTransmitApduBasicChannel(int32_t serial, const SimApdu& message) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->iccTransmitApduBasicChannel(serial, message);
    }
    return Void();
}

Return<void> RadioImpl::iccOpenLogicalChannel(int32_t serial, const hidl_string& aid, int32_t p2) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->iccOpenLogicalChannel(serial, aid, p2);
    }
    return Void();
}

Return<void> RadioImpl::iccCloseLogicalChannel(int32_t serial, int32_t channelId) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->iccCloseLogicalChannel(serial, channelId);
    }
    return Void();
}

Return<void> RadioImpl::iccTransmitApduLogicalChannel(int32_t serial, const SimApdu& message) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->iccTransmitApduLogicalChannel(serial, message);
    }
    return Void();
}

Return<void> RadioImpl::nvReadItem(int32_t serial, NvItem itemId) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->nvReadItem(serial, itemId);
    }
    return Void();
}

Return<void> RadioImpl::nvWriteItem(int32_t serial, const NvWriteItem& item) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->nvWriteItem(serial, item);
    }
    return Void();
}

Return<void> RadioImpl::nvWriteCdmaPrl(int32_t serial, const hidl_vec<uint8_t>& prl) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->nvWriteCdmaPrl(serial, prl);
    }
    return Void();
}

Return<void> RadioImpl::nvResetConfig(int32_t serial, ResetNvType resetType) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->nvResetConfig(serial, resetType);
    }
    return Void();
}

Return<void> RadioImpl::setUiccSubscription(int32_t serial, const SelectUiccSub& uiccSub) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->setUiccSubscription(serial, uiccSub);
    }
    return Void();
}

Return<void> RadioImpl::setDataAllowed(int32_t serial, bool allow) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->setDataAllowed(serial, allow);
    }
    return Void();
}

Return<void> RadioImpl::getHardwareConfig(int32_t serial) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->getHardwareConfig(serial);
    }
    return Void();
}

Return<void> RadioImpl::requestIccSimAuthentication(int32_t serial, int32_t authContext,
        const hidl_string& authData, const hidl_string& aid) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->requestIccSimAuthentication(serial, authContext, authData, aid);
    }
    return Void();
}

Return<void> RadioImpl::setDataProfile(int32_t serial, const hidl_vec<DataProfileInfo>& profiles,
                                       bool isRoaming) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->setDataProfile(serial, profiles, isRoaming);
    }
    return Void();
}

Return<void> RadioImpl::requestShutdown(int32_t serial) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->requestShutdown(serial);
    }
    return Void();
}

Return<void> RadioImpl::getRadioCapability(int32_t serial) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->getRadioCapability(serial);
    }
    return Void();
}

Return<void> RadioImpl::setRadioCapability(int32_t serial, const RadioCapability& rc) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->setRadioCapability(serial, rc);
    }
    return Void();
}

Return<void> RadioImpl::startLceService(int32_t serial, int32_t reportInterval, bool pullMode) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->startLceService(serial, reportInterval, pullMode);
    }
    return Void();
}

Return<void> RadioImpl::stopLceService(int32_t serial) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->stopLceService(serial);
    }
    return Void();
}

Return<void> RadioImpl::pullLceData(int32_t serial) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->pullLceData(serial);
    }
    return Void();
}

Return<void> RadioImpl::getModemActivityInfo(int32_t serial) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->getModemActivityInfo(serial);
    }
    return Void();
}

Return<void> RadioImpl::setAllowedCarriers(int32_t serial, bool allAllowed,
                                           const CarrierRestrictions& carriers) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->setAllowedCarriers(serial, allAllowed, carriers);
    }
    return Void();
}

Return<void> RadioImpl::getAllowedCarriers(int32_t serial) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->getAllowedCarriers(serial);
    }
    return Void();
}

Return<void> RadioImpl::sendDeviceState(int32_t serial, DeviceStateType deviceStateType,
                                        bool state) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->sendDeviceState(serial, deviceStateType, state);
    }
    return Void();
}

Return<void> RadioImpl::setIndicationFilter(int32_t serial, int32_t indicationFilter) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->setIndicationFilter(serial, indicationFilter);
    }
    return Void();
}

Return<void> RadioImpl::setSimCardPower(int32_t serial, bool powerUp) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->setSimCardPower(serial, powerUp);
    }
    return Void();
}

Return<void> RadioImpl::responseAcknowledgement() {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->responseAcknowledgement();
    }
    return Void();
}

Return<void> RadioImpl::setSimCardPower_1_1(int32_t serial, const V1_1::CardPowerState state) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->setSimCardPower_1_1(serial, state);
    }
    return Void();
}

Return<void> RadioImpl::setCarrierInfoForImsiEncryption(int32_t serial,
        const V1_1::ImsiEncryptionInfo& data) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->setCarrierInfoForImsiEncryption(serial, data);
    }
    return Void();
}

Return<void> RadioImpl::startKeepalive(int32_t serial, const V1_1::KeepaliveRequest& keepalive) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->startKeepalive(serial, keepalive);
    }
    return Void();
}

Return<void> RadioImpl::stopKeepalive(int32_t serial, int32_t sessionHandle) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->stopKeepalive(serial, sessionHandle);
    }
    return Void();
}

Return<void> RadioImpl::startNetworkScan_1_2(int32_t serial, const V1_2::NetworkScanRequest& networkScanRequest) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->startNetworkScan_1_2(serial, networkScanRequest);
    }
    return Void();
}

Return<void> RadioImpl::setIndicationFilter_1_2(int32_t serial, hidl_bitfield<V1_2::IndicationFilter> indicationFilter) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->setIndicationFilter_1_2(serial, indicationFilter);
    }
    return Void();
}

Return<void> RadioImpl::setSignalStrengthReportingCriteria(int32_t serial, int32_t hysteresisMs, int32_t hysteresisDb,
        const hidl_vec<int32_t>& thresholdsDbm, V1_2::AccessNetwork accessNetwork) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->setSignalStrengthReportingCriteria(serial, hysteresisMs, hysteresisDb,
                thresholdsDbm, accessNetwork);
    }
    return Void();
}

Return<void> RadioImpl::setLinkCapacityReportingCriteria(int32_t serial, int32_t hysteresisMs, int32_t hysteresisDlKbps,
        int32_t hysteresisUlKbps, const hidl_vec<int32_t>& thresholdsDownlinkKbps,
        const hidl_vec<int32_t>& thresholdsUplinkKbps, V1_2::AccessNetwork accessNetwork) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->setLinkCapacityReportingCriteria(serial, hysteresisMs, hysteresisDlKbps,
                hysteresisUlKbps, thresholdsDownlinkKbps, thresholdsUplinkKbps, accessNetwork);
    }
    return Void();
}

Return<void> RadioImpl::setupDataCall_1_2(int32_t serial, V1_2::AccessNetwork accessNetwork,
        const DataProfileInfo& dataProfileInfo, bool modemCognitive, bool roamingAllowed, bool isRoaming, V1_2::DataRequestReason reason,
        const hidl_vec<hidl_string>& addresses, const hidl_vec<hidl_string>& dnses) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->setupDataCall_1_2(serial, accessNetwork, dataProfileInfo,
                modemCognitive, roamingAllowed, isRoaming, reason,
                addresses, dnses);
    }
    return Void();
}

Return<void> RadioImpl::deactivateDataCall_1_2(int32_t serial, int32_t cid, V1_2::DataRequestReason reason) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->deactivateDataCall_1_2(serial, cid, reason);
    }
    return Void();
}

Return<void> RadioImpl::setSystemSelectionChannels(int32_t serial, bool specifyChannels,
        const hidl_vec<V1_1::RadioAccessSpecifier>& specifiers) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->setSystemSelectionChannels(serial, specifyChannels, specifiers);
    }
    return Void();
}

Return<void> RadioImpl::enableModem(int32_t serial, bool on) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->enableModem(serial, on);
    }
    return Void();
}

Return<void> RadioImpl::getModemStackStatus(int32_t serial) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->getModemStackStatus(serial);
    }
    return Void();
}

Return<void> RadioImpl::setupDataCall_1_4(int32_t serial, V1_4::AccessNetwork accessNetwork,
        const V1_4::DataProfileInfo& dataProfileInfo, bool roamingAllowed,
        V1_2::DataRequestReason reason, const hidl_vec<hidl_string>& addresses,
        const hidl_vec<hidl_string>& dnses) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->setupDataCall_1_4(serial, accessNetwork, dataProfileInfo,
                roamingAllowed, reason, addresses, dnses);
    }
    return Void();
}

Return<void> RadioImpl::setInitialAttachApn_1_4(int32_t serial, const V1_4::DataProfileInfo& dataProfileInfo) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->setInitialAttachApn_1_4(serial, dataProfileInfo);
    }
    return Void();
}

Return<void> RadioImpl::setDataProfile_1_4(int32_t serial, const hidl_vec<V1_4::DataProfileInfo>& profiles) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->setDataProfile_1_4(serial, profiles);
    }
    return Void();
}

Return<void> RadioImpl::emergencyDial(int32_t serial, const Dial& dialInfo,
        hidl_bitfield<V1_4::EmergencyServiceCategory> categories,
        const hidl_vec<hidl_string>& urns, V1_4::EmergencyCallRouting routing,
        bool hasKnownUserIntentEmergency, bool isTesting) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->emergencyDial(serial, dialInfo, categories, urns, routing,
                hasKnownUserIntentEmergency, isTesting);
    }
    return Void();
}

Return<void> RadioImpl::startNetworkScan_1_4(int32_t serial, const V1_2::NetworkScanRequest& networkScanRequest) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->startNetworkScan_1_4(serial, networkScanRequest);
    }
    return Void();
}

Return<void> RadioImpl::getPreferredNetworkTypeBitmap(int32_t serial) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->getPreferredNetworkTypeBitmap(serial);
    }
    return Void();
}

Return<void> RadioImpl::setPreferredNetworkTypeBitmap(int32_t serial, hidl_bitfield<V1_4::RadioAccessFamily> networkTypeBitmap) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->setPreferredNetworkTypeBitmap(serial, networkTypeBitmap);
    }
    return Void();
}

Return<void> RadioImpl::setAllowedCarriers_1_4(int32_t serial, const V1_4::CarrierRestrictionsWithPriority& carriers,
        V1_4::SimLockMultiSimPolicy multiSimPolicy) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->setAllowedCarriers_1_4(serial, carriers, multiSimPolicy);
    }
    return Void();
}

Return<void> RadioImpl::getAllowedCarriers_1_4(int32_t serial) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->getAllowedCarriers_1_4(serial);
    }
    return Void();
}

Return<void> RadioImpl::getSignalStrength_1_4(int32_t serial) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->getSignalStrength_1_4(serial);
    }
    return Void();
}

Return<void> RadioImpl::setSignalStrengthReportingCriteria_1_5(int32_t serial,
        const V1_5::SignalThresholdInfo& signalThresholdInfo, V1_5::AccessNetwork accessNetwork) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->setSignalStrengthReportingCriteria_1_5(serial, signalThresholdInfo, accessNetwork);
    }
    return Void();
}

Return<void> RadioImpl::enableUiccApplications(int32_t serial, bool enable) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->enableUiccApplications(serial, enable);
    }
    return Void();
}

Return<void> RadioImpl::areUiccApplicationsEnabled(int32_t serial) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->areUiccApplicationsEnabled(serial);
    }
    return Void();
}

Return<void> RadioImpl::setSystemSelectionChannels_1_5(int32_t serial, bool specifyChannels,
        const hidl_vec<V1_5::RadioAccessSpecifier>& specifiers) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->setSystemSelectionChannels_1_5(serial, specifyChannels, specifiers);
    }
    return Void();
}

Return<void> RadioImpl::startNetworkScan_1_5(int32_t serial, const V1_5::NetworkScanRequest& networkScanRequest) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->startNetworkScan_1_5(serial, networkScanRequest);
    }
    return Void();
}

Return<void> RadioImpl::setupDataCall_1_5(int32_t serial, V1_5::AccessNetwork accessNetwork,
        const V1_5::DataProfileInfo& dataProfileInfo, bool roamingAllowed,
        V1_2::DataRequestReason reason, const hidl_vec<V1_5::LinkAddress>& addresses,
        const hidl_vec<hidl_string>& dnses) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->setupDataCall_1_5(serial, accessNetwork, dataProfileInfo,
                roamingAllowed, reason, addresses, dnses);
    }
    return Void();
}

Return<void> RadioImpl::setInitialAttachApn_1_5(int32_t serial, const V1_5::DataProfileInfo& dataProfileInfo) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->setInitialAttachApn_1_5(serial, dataProfileInfo);
    }
    return Void();
}

Return<void> RadioImpl::setDataProfile_1_5(int32_t serial, const hidl_vec<V1_5::DataProfileInfo>& profiles) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->setDataProfile_1_5(serial, profiles);
    }
    return Void();
}

Return<void> RadioImpl::setRadioPower_1_5(int32_t serial, bool powerOn, bool forEmergencyCall,
        bool preferredForEmergencyCall) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->setRadioPower_1_5(serial, powerOn, forEmergencyCall, preferredForEmergencyCall);
    }
    return Void();
}

Return<void> RadioImpl::setIndicationFilter_1_5(int32_t serial, hidl_bitfield<V1_5::IndicationFilter> indicationFilter) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->setIndicationFilter_1_5(serial, indicationFilter);
    }
    return Void();
}

Return<void> RadioImpl::getBarringInfo(int32_t serial) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->getBarringInfo(serial);
    }
    return Void();
}

Return<void> RadioImpl::getVoiceRegistrationState_1_5(int32_t serial) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->getVoiceRegistrationState_1_5(serial);
    }
    return Void();
}

Return<void> RadioImpl::getDataRegistrationState_1_5(int32_t serial) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->getDataRegistrationState_1_5(serial);
    }
    return Void();
}

Return<void> RadioImpl::setNetworkSelectionModeManual_1_5(int32_t serial, const hidl_string& operatorNumeric,
        V1_5::RadioAccessNetworks ran) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->setNetworkSelectionModeManual_1_5(serial, operatorNumeric, ran);
    }
    return Void();
}

Return<void> RadioImpl::sendCdmaSmsExpectMore(int32_t serial, const CdmaSmsMessage& sms) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->sendCdmaSmsExpectMore(serial, sms);
    }
    return Void();
}

Return<void> RadioImpl::supplySimDepersonalization(int32_t serial, V1_5::PersoSubstate persoType, const hidl_string& controlKey) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->supplySimDepersonalization(serial, persoType, controlKey);
    }
    return Void();
}

Return<void> RadioImpl::setLinkCapacityReportingCriteria_1_5(int32_t serial, int32_t hysteresisMs, int32_t hysteresisDlKbps,
        int32_t hysteresisUlKbps, const hidl_vec<int32_t>& thresholdsDownlinkKbps,
        const hidl_vec<int32_t>& thresholdsUplinkKbps, V1_5::AccessNetwork accessNetwork) {
    if (mRadioLegacy != nullptr) {
        mRadioLegacy->setLinkCapacityReportingCriteria_1_5(serial, hysteresisMs, hysteresisDlKbps, hysteresisUlKbps,
                thresholdsDownlinkKbps, thresholdsUplinkKbps, accessNetwork);
    }
    return Void();
}
