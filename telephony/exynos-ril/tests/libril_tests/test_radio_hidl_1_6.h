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

#include <android/hardware/radio/1.6/IRadio.h>

struct TestRadioBase_1_6 : public ::android::hardware::radio::V1_6::IRadio {
    virtual ::android::hardware::Return<void> setResponseFunctions(const ::android::sp<::android::hardware::radio::V1_0::IRadioResponse>& /* radioResponse */, const ::android::sp<::android::hardware::radio::V1_0::IRadioIndication>& /* radioIndication */) override { return {}; }
    virtual ::android::hardware::Return<void> getIccCardStatus(int32_t /* serial */) override { return {}; }
    virtual ::android::hardware::Return<void> supplyIccPinForApp(int32_t /* serial */, const ::android::hardware::hidl_string& /* pin */, const ::android::hardware::hidl_string& /* aid */) override { return {}; }
    virtual ::android::hardware::Return<void> supplyIccPukForApp(int32_t /* serial */, const ::android::hardware::hidl_string& /* puk */, const ::android::hardware::hidl_string& /* pin */, const ::android::hardware::hidl_string& /* aid */) override { return {}; }
    virtual ::android::hardware::Return<void> supplyIccPin2ForApp(int32_t /* serial */, const ::android::hardware::hidl_string& /* pin2 */, const ::android::hardware::hidl_string& /* aid */) override { return {}; }
    virtual ::android::hardware::Return<void> supplyIccPuk2ForApp(int32_t /* serial */, const ::android::hardware::hidl_string& /* puk2 */, const ::android::hardware::hidl_string& /* pin2 */, const ::android::hardware::hidl_string& /* aid */) override { return {}; }
    virtual ::android::hardware::Return<void> changeIccPinForApp(int32_t /* serial */, const ::android::hardware::hidl_string& /* oldPin */, const ::android::hardware::hidl_string& /* newPin */, const ::android::hardware::hidl_string& /* aid */) override { return {}; }
    virtual ::android::hardware::Return<void> changeIccPin2ForApp(int32_t /* serial */, const ::android::hardware::hidl_string& /* oldPin2 */, const ::android::hardware::hidl_string& /* newPin2 */, const ::android::hardware::hidl_string& /* aid */) override { return {}; }
    virtual ::android::hardware::Return<void> supplyNetworkDepersonalization(int32_t /* serial */, const ::android::hardware::hidl_string& /* netPin */) override { return {}; }
    virtual ::android::hardware::Return<void> getCurrentCalls(int32_t /* serial */) override { return {}; }
    virtual ::android::hardware::Return<void> dial(int32_t /* serial */, const ::android::hardware::radio::V1_0::Dial& /* dialInfo */) override { return {}; }
    virtual ::android::hardware::Return<void> getImsiForApp(int32_t /* serial */, const ::android::hardware::hidl_string& /* aid */) override { return {}; }
    virtual ::android::hardware::Return<void> hangup(int32_t /* serial */, int32_t /* gsmIndex */) override { return {}; }
    virtual ::android::hardware::Return<void> hangupWaitingOrBackground(int32_t /* serial */) override { return {}; }
    virtual ::android::hardware::Return<void> hangupForegroundResumeBackground(int32_t /* serial */) override { return {}; }
    virtual ::android::hardware::Return<void> switchWaitingOrHoldingAndActive(int32_t /* serial */) override { return {}; }
    virtual ::android::hardware::Return<void> conference(int32_t /* serial */) override { return {}; }
    virtual ::android::hardware::Return<void> rejectCall(int32_t /* serial */) override { return {}; }
    virtual ::android::hardware::Return<void> getLastCallFailCause(int32_t /* serial */) override { return {}; }
    virtual ::android::hardware::Return<void> getSignalStrength(int32_t /* serial */) override { return {}; }
    virtual ::android::hardware::Return<void> getVoiceRegistrationState(int32_t /* serial */) override { return {}; }
    virtual ::android::hardware::Return<void> getDataRegistrationState(int32_t /* serial */) override { return {}; }
    virtual ::android::hardware::Return<void> getOperator(int32_t /* serial */) override { return {}; }
    virtual ::android::hardware::Return<void> setRadioPower(int32_t /* serial */, bool /* on */) override { return {}; }
    virtual ::android::hardware::Return<void> sendDtmf(int32_t /* serial */, const ::android::hardware::hidl_string& /* s */) override { return {}; }
    virtual ::android::hardware::Return<void> sendSms(int32_t /* serial */, const ::android::hardware::radio::V1_0::GsmSmsMessage& /* message */) override { return {}; }
    virtual ::android::hardware::Return<void> sendSMSExpectMore(int32_t /* serial */, const ::android::hardware::radio::V1_0::GsmSmsMessage& /* message */) override { return {}; }
    virtual ::android::hardware::Return<void> setupDataCall(int32_t /* serial */, ::android::hardware::radio::V1_0::RadioTechnology /* radioTechnology */, const ::android::hardware::radio::V1_0::DataProfileInfo& /* dataProfileInfo */, bool /* modemCognitive */, bool /* roamingAllowed */, bool /* isRoaming */) override { return {}; }
    virtual ::android::hardware::Return<void> iccIOForApp(int32_t /* serial */, const ::android::hardware::radio::V1_0::IccIo& /* iccIo */) override { return {}; }
    virtual ::android::hardware::Return<void> sendUssd(int32_t /* serial */, const ::android::hardware::hidl_string& /* ussd */) override { return {}; }
    virtual ::android::hardware::Return<void> cancelPendingUssd(int32_t /* serial */) override { return {}; }
    virtual ::android::hardware::Return<void> getClir(int32_t /* serial */) override { return {}; }
    virtual ::android::hardware::Return<void> setClir(int32_t /* serial */, int32_t /* status */) override { return {}; }
    virtual ::android::hardware::Return<void> getCallForwardStatus(int32_t /* serial */, const ::android::hardware::radio::V1_0::CallForwardInfo& /* callInfo */) override { return {}; }
    virtual ::android::hardware::Return<void> setCallForward(int32_t /* serial */, const ::android::hardware::radio::V1_0::CallForwardInfo& /* callInfo */) override { return {}; }
    virtual ::android::hardware::Return<void> getCallWaiting(int32_t /* serial */, int32_t /* serviceClass */) override { return {}; }
    virtual ::android::hardware::Return<void> setCallWaiting(int32_t /* serial */, bool /* enable */, int32_t /* serviceClass */) override { return {}; }
    virtual ::android::hardware::Return<void> acknowledgeLastIncomingGsmSms(int32_t /* serial */, bool /* success */, ::android::hardware::radio::V1_0::SmsAcknowledgeFailCause /* cause */) override { return {}; }
    virtual ::android::hardware::Return<void> acceptCall(int32_t /* serial */) override { return {}; }
    virtual ::android::hardware::Return<void> deactivateDataCall(int32_t /* serial */, int32_t /* cid */, bool /* reasonRadioShutDown */) override { return {}; }
    virtual ::android::hardware::Return<void> getFacilityLockForApp(int32_t /* serial */, const ::android::hardware::hidl_string& /* facility */, const ::android::hardware::hidl_string& /* password */, int32_t /* serviceClass */, const ::android::hardware::hidl_string& /* appId */) override { return {}; }
    virtual ::android::hardware::Return<void> setFacilityLockForApp(int32_t /* serial */, const ::android::hardware::hidl_string& /* facility */, bool /* lockState */, const ::android::hardware::hidl_string& /* password */, int32_t /* serviceClass */, const ::android::hardware::hidl_string& /* appId */) override { return {}; }
    virtual ::android::hardware::Return<void> setBarringPassword(int32_t /* serial */, const ::android::hardware::hidl_string& /* facility */, const ::android::hardware::hidl_string& /* oldPassword */, const ::android::hardware::hidl_string& /* newPassword */) override { return {}; }
    virtual ::android::hardware::Return<void> getNetworkSelectionMode(int32_t /* serial */) override { return {}; }
    virtual ::android::hardware::Return<void> setNetworkSelectionModeAutomatic(int32_t /* serial */) override { return {}; }
    virtual ::android::hardware::Return<void> setNetworkSelectionModeManual(int32_t /* serial */, const ::android::hardware::hidl_string& /* operatorNumeric */) override { return {}; }
    virtual ::android::hardware::Return<void> getAvailableNetworks(int32_t /* serial */) override { return {}; }
    virtual ::android::hardware::Return<void> startDtmf(int32_t /* serial */, const ::android::hardware::hidl_string& /* s */) override { return {}; }
    virtual ::android::hardware::Return<void> stopDtmf(int32_t /* serial */) override { return {}; }
    virtual ::android::hardware::Return<void> getBasebandVersion(int32_t /* serial */) override { return {}; }
    virtual ::android::hardware::Return<void> separateConnection(int32_t /* serial */, int32_t /* gsmIndex */) override { return {}; }
    virtual ::android::hardware::Return<void> setMute(int32_t /* serial */, bool /* enable */) override { return {}; }
    virtual ::android::hardware::Return<void> getMute(int32_t /* serial */) override { return {}; }
    virtual ::android::hardware::Return<void> getClip(int32_t /* serial */) override { return {}; }
    virtual ::android::hardware::Return<void> getDataCallList(int32_t /* serial */) override { return {}; }
    virtual ::android::hardware::Return<void> setSuppServiceNotifications(int32_t /* serial */, bool /* enable */) override { return {}; }
    virtual ::android::hardware::Return<void> writeSmsToSim(int32_t /* serial */, const ::android::hardware::radio::V1_0::SmsWriteArgs& /* smsWriteArgs */) override { return {}; }
    virtual ::android::hardware::Return<void> deleteSmsOnSim(int32_t /* serial */, int32_t /* index */) override { return {}; }
    virtual ::android::hardware::Return<void> setBandMode(int32_t /* serial */, ::android::hardware::radio::V1_0::RadioBandMode /* mode */) override { return {}; }
    virtual ::android::hardware::Return<void> getAvailableBandModes(int32_t /* serial */) override { return {}; }
    virtual ::android::hardware::Return<void> sendEnvelope(int32_t /* serial */, const ::android::hardware::hidl_string& /* command */) override { return {}; }
    virtual ::android::hardware::Return<void> sendTerminalResponseToSim(int32_t /* serial */, const ::android::hardware::hidl_string& /* commandResponse */) override { return {}; }
    virtual ::android::hardware::Return<void> handleStkCallSetupRequestFromSim(int32_t /* serial */, bool /* accept */) override { return {}; }
    virtual ::android::hardware::Return<void> explicitCallTransfer(int32_t /* serial */) override { return {}; }
    virtual ::android::hardware::Return<void> setPreferredNetworkType(int32_t /* serial */, ::android::hardware::radio::V1_0::PreferredNetworkType /* nwType */) override { return {}; }
    virtual ::android::hardware::Return<void> getPreferredNetworkType(int32_t /* serial */) override { return {}; }
    virtual ::android::hardware::Return<void> getNeighboringCids(int32_t /* serial */) override { return {}; }
    virtual ::android::hardware::Return<void> setLocationUpdates(int32_t /* serial */, bool /* enable */) override { return {}; }
    virtual ::android::hardware::Return<void> setCdmaSubscriptionSource(int32_t /* serial */, ::android::hardware::radio::V1_0::CdmaSubscriptionSource /* cdmaSub */) override { return {}; }
    virtual ::android::hardware::Return<void> setCdmaRoamingPreference(int32_t /* serial */, ::android::hardware::radio::V1_0::CdmaRoamingType /* type */) override { return {}; }
    virtual ::android::hardware::Return<void> getCdmaRoamingPreference(int32_t /* serial */) override { return {}; }
    virtual ::android::hardware::Return<void> setTTYMode(int32_t /* serial */, ::android::hardware::radio::V1_0::TtyMode /* mode */) override { return {}; }
    virtual ::android::hardware::Return<void> getTTYMode(int32_t /* serial */) override { return {}; }
    virtual ::android::hardware::Return<void> setPreferredVoicePrivacy(int32_t /* serial */, bool /* enable */) override { return {}; }
    virtual ::android::hardware::Return<void> getPreferredVoicePrivacy(int32_t /* serial */) override { return {}; }
    virtual ::android::hardware::Return<void> sendCDMAFeatureCode(int32_t /* serial */, const ::android::hardware::hidl_string& /* featureCode */) override { return {}; }
    virtual ::android::hardware::Return<void> sendBurstDtmf(int32_t /* serial */, const ::android::hardware::hidl_string& /* dtmf */, int32_t /* on */, int32_t /* off */) override { return {}; }
    virtual ::android::hardware::Return<void> sendCdmaSms(int32_t /* serial */, const ::android::hardware::radio::V1_0::CdmaSmsMessage& /* sms */) override { return {}; }
    virtual ::android::hardware::Return<void> acknowledgeLastIncomingCdmaSms(int32_t /* serial */, const ::android::hardware::radio::V1_0::CdmaSmsAck& /* smsAck */) override { return {}; }
    virtual ::android::hardware::Return<void> getGsmBroadcastConfig(int32_t /* serial */) override { return {}; }
    virtual ::android::hardware::Return<void> setGsmBroadcastConfig(int32_t /* serial */, const ::android::hardware::hidl_vec<::android::hardware::radio::V1_0::GsmBroadcastSmsConfigInfo>& /* configInfo */) override { return {}; }
    virtual ::android::hardware::Return<void> setGsmBroadcastActivation(int32_t /* serial */, bool /* activate */) override { return {}; }
    virtual ::android::hardware::Return<void> getCdmaBroadcastConfig(int32_t /* serial */) override { return {}; }
    virtual ::android::hardware::Return<void> setCdmaBroadcastConfig(int32_t /* serial */, const ::android::hardware::hidl_vec<::android::hardware::radio::V1_0::CdmaBroadcastSmsConfigInfo>& /* configInfo */) override { return {}; }
    virtual ::android::hardware::Return<void> setCdmaBroadcastActivation(int32_t /* serial */, bool /* activate */) override { return {}; }
    virtual ::android::hardware::Return<void> getCDMASubscription(int32_t /* serial */) override { return {}; }
    virtual ::android::hardware::Return<void> writeSmsToRuim(int32_t /* serial */, const ::android::hardware::radio::V1_0::CdmaSmsWriteArgs& /* cdmaSms */) override { return {}; }
    virtual ::android::hardware::Return<void> deleteSmsOnRuim(int32_t /* serial */, int32_t /* index */) override { return {}; }
    virtual ::android::hardware::Return<void> getDeviceIdentity(int32_t /* serial */) override { return {}; }
    virtual ::android::hardware::Return<void> exitEmergencyCallbackMode(int32_t /* serial */) override { return {}; }
    virtual ::android::hardware::Return<void> getSmscAddress(int32_t /* serial */) override { return {}; }
    virtual ::android::hardware::Return<void> setSmscAddress(int32_t /* serial */, const ::android::hardware::hidl_string& /* smsc */) override { return {}; }
    virtual ::android::hardware::Return<void> reportSmsMemoryStatus(int32_t /* serial */, bool /* available */) override { return {}; }
    virtual ::android::hardware::Return<void> reportStkServiceIsRunning(int32_t /* serial */) override { return {}; }
    virtual ::android::hardware::Return<void> getCdmaSubscriptionSource(int32_t /* serial */) override { return {}; }
    virtual ::android::hardware::Return<void> requestIsimAuthentication(int32_t /* serial */, const ::android::hardware::hidl_string& /* challenge */) override { return {}; }
    virtual ::android::hardware::Return<void> acknowledgeIncomingGsmSmsWithPdu(int32_t /* serial */, bool /* success */, const ::android::hardware::hidl_string& /* ackPdu */) override { return {}; }
    virtual ::android::hardware::Return<void> sendEnvelopeWithStatus(int32_t /* serial */, const ::android::hardware::hidl_string& /* contents */) override { return {}; }
    virtual ::android::hardware::Return<void> getVoiceRadioTechnology(int32_t /* serial */) override { return {}; }
    virtual ::android::hardware::Return<void> getCellInfoList(int32_t /* serial */) override { return {}; }
    virtual ::android::hardware::Return<void> setCellInfoListRate(int32_t /* serial */, int32_t /* rate */) override { return {}; }
    virtual ::android::hardware::Return<void> setInitialAttachApn(int32_t /* serial */, const ::android::hardware::radio::V1_0::DataProfileInfo& /* dataProfileInfo */, bool /* modemCognitive */, bool /* isRoaming */) override { return {}; }
    virtual ::android::hardware::Return<void> getImsRegistrationState(int32_t /* serial */) override { return {}; }
    virtual ::android::hardware::Return<void> sendImsSms(int32_t /* serial */, const ::android::hardware::radio::V1_0::ImsSmsMessage& /* message */) override { return {}; }
    virtual ::android::hardware::Return<void> iccTransmitApduBasicChannel(int32_t /* serial */, const ::android::hardware::radio::V1_0::SimApdu& /* message */) override { return {}; }
    virtual ::android::hardware::Return<void> iccOpenLogicalChannel(int32_t /* serial */, const ::android::hardware::hidl_string& /* aid */, int32_t /* p2 */) override { return {}; }
    virtual ::android::hardware::Return<void> iccCloseLogicalChannel(int32_t /* serial */, int32_t /* channelId */) override { return {}; }
    virtual ::android::hardware::Return<void> iccTransmitApduLogicalChannel(int32_t /* serial */, const ::android::hardware::radio::V1_0::SimApdu& /* message */) override { return {}; }
    virtual ::android::hardware::Return<void> nvReadItem(int32_t /* serial */, ::android::hardware::radio::V1_0::NvItem /* itemId */) override { return {}; }
    virtual ::android::hardware::Return<void> nvWriteItem(int32_t /* serial */, const ::android::hardware::radio::V1_0::NvWriteItem& /* item */) override { return {}; }
    virtual ::android::hardware::Return<void> nvWriteCdmaPrl(int32_t /* serial */, const ::android::hardware::hidl_vec<uint8_t>& /* prl */) override { return {}; }
    virtual ::android::hardware::Return<void> nvResetConfig(int32_t /* serial */, ::android::hardware::radio::V1_0::ResetNvType /* resetType */) override { return {}; }
    virtual ::android::hardware::Return<void> setUiccSubscription(int32_t /* serial */, const ::android::hardware::radio::V1_0::SelectUiccSub& /* uiccSub */) override { return {}; }
    virtual ::android::hardware::Return<void> setDataAllowed(int32_t /* serial */, bool /* allow */) override { return {}; }
    virtual ::android::hardware::Return<void> getHardwareConfig(int32_t /* serial */) override { return {}; }
    virtual ::android::hardware::Return<void> requestIccSimAuthentication(int32_t /* serial */, int32_t /* authContext */, const ::android::hardware::hidl_string& /* authData */, const ::android::hardware::hidl_string& /* aid */) override { return {}; }
    virtual ::android::hardware::Return<void> setDataProfile(int32_t /* serial */, const ::android::hardware::hidl_vec<::android::hardware::radio::V1_0::DataProfileInfo>& /* profiles */, bool /* isRoaming */) override { return {}; }
    virtual ::android::hardware::Return<void> requestShutdown(int32_t /* serial */) override { return {}; }
    virtual ::android::hardware::Return<void> getRadioCapability(int32_t /* serial */) override { return {}; }
    virtual ::android::hardware::Return<void> setRadioCapability(int32_t /* serial */, const ::android::hardware::radio::V1_0::RadioCapability& /* rc */) override { return {}; }
    virtual ::android::hardware::Return<void> startLceService(int32_t /* serial */, int32_t /* reportInterval */, bool /* pullMode */) override { return {}; }
    virtual ::android::hardware::Return<void> stopLceService(int32_t /* serial */) override { return {}; }
    virtual ::android::hardware::Return<void> pullLceData(int32_t /* serial */) override { return {}; }
    virtual ::android::hardware::Return<void> getModemActivityInfo(int32_t /* serial */) override { return {}; }
    virtual ::android::hardware::Return<void> setAllowedCarriers(int32_t /* serial */, bool /* allAllowed */, const ::android::hardware::radio::V1_0::CarrierRestrictions& /* carriers */) override { return {}; }
    virtual ::android::hardware::Return<void> getAllowedCarriers(int32_t /* serial */) override { return {}; }
    virtual ::android::hardware::Return<void> sendDeviceState(int32_t /* serial */, ::android::hardware::radio::V1_0::DeviceStateType /* deviceStateType */, bool /* state */) override { return {}; }
    virtual ::android::hardware::Return<void> setIndicationFilter(int32_t /* serial */, ::android::hardware::hidl_bitfield<::android::hardware::radio::V1_0::IndicationFilter> /* indicationFilter */) override { return {}; }
    virtual ::android::hardware::Return<void> setSimCardPower(int32_t /* serial */, bool /* powerUp */) override { return {}; }
    virtual ::android::hardware::Return<void> responseAcknowledgement() override { return {}; }
    virtual ::android::hardware::Return<void> setCarrierInfoForImsiEncryption(int32_t /* serial */, const ::android::hardware::radio::V1_1::ImsiEncryptionInfo& /* imsiEncryptionInfo */) override { return {}; }
    virtual ::android::hardware::Return<void> setSimCardPower_1_1(int32_t /* serial */, ::android::hardware::radio::V1_1::CardPowerState /* powerUp */) override { return {}; }
    virtual ::android::hardware::Return<void> startNetworkScan(int32_t /* serial */, const ::android::hardware::radio::V1_1::NetworkScanRequest& /* request */) override { return {}; }
    virtual ::android::hardware::Return<void> stopNetworkScan(int32_t /* serial */) override { return {}; }
    virtual ::android::hardware::Return<void> startKeepalive(int32_t /* serial */, const ::android::hardware::radio::V1_1::KeepaliveRequest& /* keepalive */) override { return {}; }
    virtual ::android::hardware::Return<void> stopKeepalive(int32_t /* serial */, int32_t /* sessionHandle */) override { return {}; }
    virtual ::android::hardware::Return<void> startNetworkScan_1_2(int32_t /* serial */, const ::android::hardware::radio::V1_2::NetworkScanRequest& /* request */) override { return {}; }
    virtual ::android::hardware::Return<void> setIndicationFilter_1_2(int32_t /* serial */, ::android::hardware::hidl_bitfield<::android::hardware::radio::V1_2::IndicationFilter> /* indicationFilter */) override { return {}; }
    virtual ::android::hardware::Return<void> setSignalStrengthReportingCriteria(int32_t /* serial */, int32_t /* hysteresisMs */, int32_t /* hysteresisDb */, const ::android::hardware::hidl_vec<int32_t>& /* thresholdsDbm */, ::android::hardware::radio::V1_2::AccessNetwork /* accessNetwork */) override { return {}; }
    virtual ::android::hardware::Return<void> setLinkCapacityReportingCriteria(int32_t /* serial */, int32_t /* hysteresisMs */, int32_t /* hysteresisDlKbps */, int32_t /* hysteresisUlKbps */, const ::android::hardware::hidl_vec<int32_t>& /* thresholdsDownlinkKbps */, const ::android::hardware::hidl_vec<int32_t>& /* thresholdsUplinkKbps */, ::android::hardware::radio::V1_2::AccessNetwork /* accessNetwork */) override { return {}; }
    virtual ::android::hardware::Return<void> setupDataCall_1_2(int32_t /* serial */, ::android::hardware::radio::V1_2::AccessNetwork /* accessNetwork */, const ::android::hardware::radio::V1_0::DataProfileInfo& /* dataProfileInfo */, bool /* modemCognitive */, bool /* roamingAllowed */, bool /* isRoaming */, ::android::hardware::radio::V1_2::DataRequestReason /* reason */, const ::android::hardware::hidl_vec<::android::hardware::hidl_string>& /* addresses */, const ::android::hardware::hidl_vec<::android::hardware::hidl_string>& /* dnses */) override { return {}; }
    virtual ::android::hardware::Return<void> deactivateDataCall_1_2(int32_t /* serial */, int32_t /* cid */, ::android::hardware::radio::V1_2::DataRequestReason /* reason */) override { return {}; }
    virtual ::android::hardware::Return<void> setSystemSelectionChannels(int32_t /* serial */, bool /* specifyChannels */, const ::android::hardware::hidl_vec<::android::hardware::radio::V1_1::RadioAccessSpecifier>& /* specifiers */) override { return {}; }
    virtual ::android::hardware::Return<void> enableModem(int32_t /* serial */, bool /* on */) override { return {}; }
    virtual ::android::hardware::Return<void> getModemStackStatus(int32_t /* serial */) override { return {}; }
    virtual ::android::hardware::Return<void> setupDataCall_1_4(int32_t /* serial */, ::android::hardware::radio::V1_4::AccessNetwork /* accessNetwork */, const ::android::hardware::radio::V1_4::DataProfileInfo& /* dataProfileInfo */, bool /* roamingAllowed */, ::android::hardware::radio::V1_2::DataRequestReason /* reason */, const ::android::hardware::hidl_vec<::android::hardware::hidl_string>& /* addresses */, const ::android::hardware::hidl_vec<::android::hardware::hidl_string>& /* dnses */) override { return {}; }
    virtual ::android::hardware::Return<void> setInitialAttachApn_1_4(int32_t /* serial */, const ::android::hardware::radio::V1_4::DataProfileInfo& /* dataProfileInfo */) override { return {}; }
    virtual ::android::hardware::Return<void> setDataProfile_1_4(int32_t /* serial */, const ::android::hardware::hidl_vec<::android::hardware::radio::V1_4::DataProfileInfo>& /* profiles */) override { return {}; }
    virtual ::android::hardware::Return<void> emergencyDial(int32_t /* serial */, const ::android::hardware::radio::V1_0::Dial& /* dialInfo */, ::android::hardware::hidl_bitfield<::android::hardware::radio::V1_4::EmergencyServiceCategory> /* categories */, const ::android::hardware::hidl_vec<::android::hardware::hidl_string>& /* urns */, ::android::hardware::radio::V1_4::EmergencyCallRouting /* routing */, bool /* hasKnownUserIntentEmergency */, bool /* isTesting */) override { return {}; }
    virtual ::android::hardware::Return<void> startNetworkScan_1_4(int32_t /* serial */, const ::android::hardware::radio::V1_2::NetworkScanRequest& /* request */) override { return {}; }
    virtual ::android::hardware::Return<void> getPreferredNetworkTypeBitmap(int32_t /* serial */) override { return {}; }
    virtual ::android::hardware::Return<void> setPreferredNetworkTypeBitmap(int32_t /* serial */, ::android::hardware::hidl_bitfield<::android::hardware::radio::V1_4::RadioAccessFamily> /* networkTypeBitmap */) override { return {}; }
    virtual ::android::hardware::Return<void> setAllowedCarriers_1_4(int32_t /* serial */, const ::android::hardware::radio::V1_4::CarrierRestrictionsWithPriority& /* carriers */, ::android::hardware::radio::V1_4::SimLockMultiSimPolicy /* multiSimPolicy */) override { return {}; }
    virtual ::android::hardware::Return<void> getAllowedCarriers_1_4(int32_t /* serial */) override { return {}; }
    virtual ::android::hardware::Return<void> getSignalStrength_1_4(int32_t /* serial */) override { return {}; }
    virtual ::android::hardware::Return<void> setSignalStrengthReportingCriteria_1_5(int32_t /* serial */, const ::android::hardware::radio::V1_5::SignalThresholdInfo& /* signalThresholdInfo */, ::android::hardware::radio::V1_5::AccessNetwork /* accessNetwork */) override { return {}; }
    virtual ::android::hardware::Return<void> setLinkCapacityReportingCriteria_1_5(int32_t /* serial */, int32_t /* hysteresisMs */, int32_t /* hysteresisDlKbps */, int32_t /* hysteresisUlKbps */, const ::android::hardware::hidl_vec<int32_t>& /* thresholdsDownlinkKbps */, const ::android::hardware::hidl_vec<int32_t>& /* thresholdsUplinkKbps */, ::android::hardware::radio::V1_5::AccessNetwork /* accessNetwork */) override { return {}; }
    virtual ::android::hardware::Return<void> enableUiccApplications(int32_t /* serial */, bool /* enable */) override { return {}; }
    virtual ::android::hardware::Return<void> areUiccApplicationsEnabled(int32_t /* serial */) override { return {}; }
    virtual ::android::hardware::Return<void> setSystemSelectionChannels_1_5(int32_t /* serial */, bool /* specifyChannels */, const ::android::hardware::hidl_vec<::android::hardware::radio::V1_5::RadioAccessSpecifier>& /* specifiers */) override { return {}; }
    virtual ::android::hardware::Return<void> startNetworkScan_1_5(int32_t /* serial */, const ::android::hardware::radio::V1_5::NetworkScanRequest& /* request */) override { return {}; }
    virtual ::android::hardware::Return<void> setupDataCall_1_5(int32_t /* serial */, ::android::hardware::radio::V1_5::AccessNetwork /* accessNetwork */, const ::android::hardware::radio::V1_5::DataProfileInfo& /* dataProfileInfo */, bool /* roamingAllowed */, ::android::hardware::radio::V1_2::DataRequestReason /* reason */, const ::android::hardware::hidl_vec<::android::hardware::radio::V1_5::LinkAddress>& /* addresses */, const ::android::hardware::hidl_vec<::android::hardware::hidl_string>& /* dnses */) override { return {}; }
    virtual ::android::hardware::Return<void> setInitialAttachApn_1_5(int32_t /* serial */, const ::android::hardware::radio::V1_5::DataProfileInfo& /* dataProfileInfo */) override { return {}; }
    virtual ::android::hardware::Return<void> setDataProfile_1_5(int32_t /* serial */, const ::android::hardware::hidl_vec<::android::hardware::radio::V1_5::DataProfileInfo>& /* profiles */) override { return {}; }
    virtual ::android::hardware::Return<void> setRadioPower_1_5(int32_t /* serial */, bool /* powerOn */, bool /* forEmergencyCall */, bool /* preferredForEmergencyCall */) override { return {}; }
    virtual ::android::hardware::Return<void> setIndicationFilter_1_5(int32_t /* serial */, ::android::hardware::hidl_bitfield<::android::hardware::radio::V1_5::IndicationFilter> /* indicationFilter */) override { return {}; }
    virtual ::android::hardware::Return<void> getBarringInfo(int32_t /* serial */) override { return {}; }
    virtual ::android::hardware::Return<void> getVoiceRegistrationState_1_5(int32_t /* serial */) override { return {}; }
    virtual ::android::hardware::Return<void> getDataRegistrationState_1_5(int32_t /* serial */) override { return {}; }
    virtual ::android::hardware::Return<void> setNetworkSelectionModeManual_1_5(int32_t /* serial */, const ::android::hardware::hidl_string& /* operatorNumeric */, ::android::hardware::radio::V1_5::RadioAccessNetworks /* ran */) override { return {}; }
    virtual ::android::hardware::Return<void> sendCdmaSmsExpectMore(int32_t /* serial */, const ::android::hardware::radio::V1_0::CdmaSmsMessage& /* sms */) override { return {}; }
    virtual ::android::hardware::Return<void> supplySimDepersonalization(int32_t /* serial */, ::android::hardware::radio::V1_5::PersoSubstate /* persoType */, const ::android::hardware::hidl_string& /* controlKey */) override { return {}; }
    virtual ::android::hardware::Return<void> setRadioPower_1_6(int32_t /* serial */, bool /* powerOn */, bool /* forEmergencyCall */, bool /* preferredForEmergencyCall */) override { return {}; }
    virtual ::android::hardware::Return<void> getDataCallList_1_6(int32_t /* serial */) override { return {}; }
    virtual ::android::hardware::Return<void> setupDataCall_1_6(int32_t /* serial */, ::android::hardware::radio::V1_5::AccessNetwork /* accessNetwork */, const ::android::hardware::radio::V1_5::DataProfileInfo& /* dataProfileInfo */, bool /* roamingAllowed */, ::android::hardware::radio::V1_2::DataRequestReason /* reason */, const ::android::hardware::hidl_vec<::android::hardware::radio::V1_5::LinkAddress>& /* addresses */, const ::android::hardware::hidl_vec<::android::hardware::hidl_string>& /* dnses */, int32_t /* pduSessionId */, const ::android::hardware::radio::V1_6::OptionalSliceInfo& /* sliceInfo */, const ::android::hardware::radio::V1_6::OptionalTrafficDescriptor& /* trafficDescriptor */, bool /* matchAllRuleAllowed */) override { return {}; }
    virtual ::android::hardware::Return<void> sendSms_1_6(int32_t /* serial */, const ::android::hardware::radio::V1_0::GsmSmsMessage& /* message */) override { return {}; }
    virtual ::android::hardware::Return<void> sendSmsExpectMore_1_6(int32_t /* serial */, const ::android::hardware::radio::V1_0::GsmSmsMessage& /* message */) override { return {}; }
    virtual ::android::hardware::Return<void> sendCdmaSms_1_6(int32_t /* serial */, const ::android::hardware::radio::V1_0::CdmaSmsMessage& /* sms */) override { return {}; }
    virtual ::android::hardware::Return<void> sendCdmaSmsExpectMore_1_6(int32_t /* serial */, const ::android::hardware::radio::V1_0::CdmaSmsMessage& /* sms */) override { return {}; }
    virtual ::android::hardware::Return<void> setSimCardPower_1_6(int32_t /* serial */, ::android::hardware::radio::V1_1::CardPowerState /* powerUp */) override { return {}; }
    virtual ::android::hardware::Return<void> setNrDualConnectivityState(int32_t /* serial */, ::android::hardware::radio::V1_6::NrDualConnectivityState /* nrDualConnectivityState */) override { return {}; }
    virtual ::android::hardware::Return<void> isNrDualConnectivityEnabled(int32_t /* serial */) override { return {}; }
    virtual ::android::hardware::Return<void> allocatePduSessionId(int32_t /* serial */) override { return {}; }
    virtual ::android::hardware::Return<void> releasePduSessionId(int32_t /* serial */, int32_t /* id */) override { return {}; }
    virtual ::android::hardware::Return<void> startHandover(int32_t /* serial */, int32_t /* callId */) override { return {}; }
    virtual ::android::hardware::Return<void> cancelHandover(int32_t /* serial */, int32_t /* callId */) override { return {}; }
    virtual ::android::hardware::Return<void> setAllowedNetworkTypesBitmap(uint32_t /* serial */, ::android::hardware::hidl_bitfield<::android::hardware::radio::V1_4::RadioAccessFamily> /* networkTypeBitmap */) override { return {}; }
    virtual ::android::hardware::Return<void> getAllowedNetworkTypesBitmap(int32_t /* serial */) override { return {}; }
    virtual ::android::hardware::Return<void> setDataThrottling(int32_t /* serial */, ::android::hardware::radio::V1_6::DataThrottlingAction /* dataThrottlingAction */, int64_t /* completionDurationMillis */) override { return {}; }
    virtual ::android::hardware::Return<void> emergencyDial_1_6(int32_t /* serial */, const ::android::hardware::radio::V1_0::Dial& /* dialInfo */, ::android::hardware::hidl_bitfield<::android::hardware::radio::V1_4::EmergencyServiceCategory> /* categories */, const ::android::hardware::hidl_vec<::android::hardware::hidl_string>& /* urns */, ::android::hardware::radio::V1_4::EmergencyCallRouting /* routing */, bool /* hasKnownUserIntentEmergency */, bool /* isTesting */) override { return {}; }
    virtual ::android::hardware::Return<void> getSystemSelectionChannels(int32_t /* serial */) override { return {}; }
    virtual ::android::hardware::Return<void> getCellInfoList_1_6(int32_t /* serial */) override { return {}; }
    virtual ::android::hardware::Return<void> getVoiceRegistrationState_1_6(int32_t /* serial */) override { return {}; }
    virtual ::android::hardware::Return<void> getSignalStrength_1_6(int32_t /* serial */) override { return {}; }
    virtual ::android::hardware::Return<void> getDataRegistrationState_1_6(int32_t /* serial */) override { return {}; }
    virtual ::android::hardware::Return<void> getCurrentCalls_1_6(int32_t /* serial */) override { return {}; }
    virtual ::android::hardware::Return<void> getSlicingConfig(int32_t /* serial */) override { return {}; }
    virtual ::android::hardware::Return<void> setCarrierInfoForImsiEncryption_1_6(int32_t /* serial */, const ::android::hardware::radio::V1_6::ImsiEncryptionInfo& /* imsiEncryptionInfo */) override { return {}; }
    virtual ::android::hardware::Return<void> getSimPhonebookRecords(int32_t /* serial */) override { return {}; }
    virtual ::android::hardware::Return<void> getSimPhonebookCapacity(int32_t /* serial */) override { return {}; }
    virtual ::android::hardware::Return<void> updateSimPhonebookRecords(int32_t /* serial */, const ::android::hardware::radio::V1_6::PhonebookRecordInfo& /* recordInfo */) override { return {}; }
};