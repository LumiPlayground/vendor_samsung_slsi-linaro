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

    Return<void> getIccCardStatus(int32_t serial);

    Return<void> supplyIccPinForApp(int32_t serial, const hidl_string& pin,
            const hidl_string& aid);

    Return<void> supplyIccPukForApp(int32_t serial, const hidl_string& puk,
            const hidl_string& pin, const hidl_string& aid);

    Return<void> supplyIccPin2ForApp(int32_t serial,
            const hidl_string& pin2,
            const hidl_string& aid);

    Return<void> supplyIccPuk2ForApp(int32_t serial, const hidl_string& puk2,
            const hidl_string& pin2, const hidl_string& aid);

    Return<void> changeIccPinForApp(int32_t serial, const hidl_string& oldPin,
            const hidl_string& newPin, const hidl_string& aid);

    Return<void> changeIccPin2ForApp(int32_t serial, const hidl_string& oldPin2,
            const hidl_string& newPin2, const hidl_string& aid);

    Return<void> supplyNetworkDepersonalization(int32_t serial, const hidl_string& netPin);

    Return<void> getCurrentCalls(int32_t serial);

    Return<void> dial(int32_t serial, const Dial& dialInfo);

    Return<void> getImsiForApp(int32_t serial,
            const ::android::hardware::hidl_string& aid);

    Return<void> hangup(int32_t serial, int32_t gsmIndex);

    Return<void> hangupWaitingOrBackground(int32_t serial);

    Return<void> hangupForegroundResumeBackground(int32_t serial);

    Return<void> switchWaitingOrHoldingAndActive(int32_t serial);

    Return<void> conference(int32_t serial);

    Return<void> rejectCall(int32_t serial);

    Return<void> getLastCallFailCause(int32_t serial);

    Return<void> getSignalStrength(int32_t serial);

    Return<void> getVoiceRegistrationState(int32_t serial);

    Return<void> getDataRegistrationState(int32_t serial);

    Return<void> getOperator(int32_t serial);

    Return<void> setRadioPower(int32_t serial, bool on);

    Return<void> sendDtmf(int32_t serial,
            const ::android::hardware::hidl_string& s);

    Return<void> sendSms(int32_t serial, const GsmSmsMessage& message);

    Return<void> sendSMSExpectMore(int32_t serial, const GsmSmsMessage& message);

    Return<void> setupDataCall(int32_t serial,
            RadioTechnology radioTechnology,
            const DataProfileInfo& profileInfo,
            bool modemCognitive,
            bool roamingAllowed,
            bool isRoaming);

    Return<void> iccIOForApp(int32_t serial,
            const IccIo& iccIo);

    Return<void> sendUssd(int32_t serial,
            const ::android::hardware::hidl_string& ussd);

    Return<void> cancelPendingUssd(int32_t serial);

    Return<void> getClir(int32_t serial);

    Return<void> setClir(int32_t serial, int32_t status);

    Return<void> getCallForwardStatus(int32_t serial,
            const CallForwardInfo& callInfo);

    Return<void> setCallForward(int32_t serial,
            const CallForwardInfo& callInfo);

    Return<void> getCallWaiting(int32_t serial, int32_t serviceClass);

    Return<void> setCallWaiting(int32_t serial, bool enable, int32_t serviceClass);

    Return<void> acknowledgeLastIncomingGsmSms(int32_t serial,
            bool success, SmsAcknowledgeFailCause cause);

    Return<void> acceptCall(int32_t serial);

    Return<void> deactivateDataCall(int32_t serial,
            int32_t cid, bool reasonRadioShutDown);

    Return<void> getFacilityLockForApp(int32_t serial,
            const ::android::hardware::hidl_string& facility,
            const ::android::hardware::hidl_string& password,
            int32_t serviceClass,
            const ::android::hardware::hidl_string& appId);

    Return<void> setFacilityLockForApp(int32_t serial,
            const ::android::hardware::hidl_string& facility,
            bool lockState,
            const ::android::hardware::hidl_string& password,
            int32_t serviceClass,
            const ::android::hardware::hidl_string& appId);

    Return<void> setBarringPassword(int32_t serial,
            const ::android::hardware::hidl_string& facility,
            const ::android::hardware::hidl_string& oldPassword,
            const ::android::hardware::hidl_string& newPassword);

    Return<void> getNetworkSelectionMode(int32_t serial);

    Return<void> setNetworkSelectionModeAutomatic(int32_t serial);

    Return<void> setNetworkSelectionModeManual(int32_t serial,
            const ::android::hardware::hidl_string& operatorNumeric);

    Return<void> getAvailableNetworks(int32_t serial);

    Return<void> startNetworkScan(int32_t serial, const V1_1::NetworkScanRequest& request);

    Return<void> stopNetworkScan(int32_t serial);

    Return<void> startDtmf(int32_t serial,
            const ::android::hardware::hidl_string& s);

    Return<void> stopDtmf(int32_t serial);

    Return<void> getBasebandVersion(int32_t serial);

    Return<void> separateConnection(int32_t serial, int32_t gsmIndex);

    Return<void> setMute(int32_t serial, bool enable);

    Return<void> getMute(int32_t serial);

    Return<void> getClip(int32_t serial);

    Return<void> getDataCallList(int32_t serial);

    Return<void> setSuppServiceNotifications(int32_t serial, bool enable);

    Return<void> writeSmsToSim(int32_t serial,
            const SmsWriteArgs& smsWriteArgs);

    Return<void> deleteSmsOnSim(int32_t serial, int32_t index);

    Return<void> setBandMode(int32_t serial, RadioBandMode mode);

    Return<void> getAvailableBandModes(int32_t serial);

    Return<void> sendEnvelope(int32_t serial,
            const ::android::hardware::hidl_string& command);

    Return<void> sendTerminalResponseToSim(int32_t serial,
            const ::android::hardware::hidl_string& commandResponse);

    Return<void> handleStkCallSetupRequestFromSim(int32_t serial, bool accept);

    Return<void> explicitCallTransfer(int32_t serial);

    Return<void> setPreferredNetworkType(int32_t serial, PreferredNetworkType nwType);

    Return<void> getPreferredNetworkType(int32_t serial);

    Return<void> getNeighboringCids(int32_t serial);

    Return<void> setLocationUpdates(int32_t serial, bool enable);

    Return<void> setCdmaSubscriptionSource(int32_t serial,
            CdmaSubscriptionSource cdmaSub);

    Return<void> setCdmaRoamingPreference(int32_t serial, CdmaRoamingType type);

    Return<void> getCdmaRoamingPreference(int32_t serial);

    Return<void> setTTYMode(int32_t serial, TtyMode mode);

    Return<void> getTTYMode(int32_t serial);

    Return<void> setPreferredVoicePrivacy(int32_t serial, bool enable);

    Return<void> getPreferredVoicePrivacy(int32_t serial);

    Return<void> sendCDMAFeatureCode(int32_t serial,
            const ::android::hardware::hidl_string& featureCode);

    Return<void> sendBurstDtmf(int32_t serial,
            const ::android::hardware::hidl_string& dtmf,
            int32_t on,
            int32_t off);

    Return<void> sendCdmaSms(int32_t serial, const CdmaSmsMessage& sms);

    Return<void> acknowledgeLastIncomingCdmaSms(int32_t serial,
            const CdmaSmsAck& smsAck);

    Return<void> getGsmBroadcastConfig(int32_t serial);

    Return<void> setGsmBroadcastConfig(int32_t serial,
            const hidl_vec<GsmBroadcastSmsConfigInfo>& configInfo);

    Return<void> setGsmBroadcastActivation(int32_t serial, bool activate);

    Return<void> getCdmaBroadcastConfig(int32_t serial);

    Return<void> setCdmaBroadcastConfig(int32_t serial,
            const hidl_vec<CdmaBroadcastSmsConfigInfo>& configInfo);

    Return<void> setCdmaBroadcastActivation(int32_t serial, bool activate);

    Return<void> getCDMASubscription(int32_t serial);

    Return<void> writeSmsToRuim(int32_t serial, const CdmaSmsWriteArgs& cdmaSms);

    Return<void> deleteSmsOnRuim(int32_t serial, int32_t index);

    Return<void> getDeviceIdentity(int32_t serial);

    Return<void> exitEmergencyCallbackMode(int32_t serial);

    Return<void> getSmscAddress(int32_t serial);

    Return<void> setSmscAddress(int32_t serial,
            const ::android::hardware::hidl_string& smsc);

    Return<void> reportSmsMemoryStatus(int32_t serial, bool available);

    Return<void> reportStkServiceIsRunning(int32_t serial);

    Return<void> getCdmaSubscriptionSource(int32_t serial);

    Return<void> requestIsimAuthentication(int32_t serial,
            const ::android::hardware::hidl_string& challenge);

    Return<void> acknowledgeIncomingGsmSmsWithPdu(int32_t serial,
            bool success,
            const ::android::hardware::hidl_string& ackPdu);

    Return<void> sendEnvelopeWithStatus(int32_t serial,
            const ::android::hardware::hidl_string& contents);

    Return<void> getVoiceRadioTechnology(int32_t serial);

    Return<void> getCellInfoList(int32_t serial);

    Return<void> setCellInfoListRate(int32_t serial, int32_t rate);

    Return<void> setInitialAttachApn(int32_t serial, const DataProfileInfo& dataProfileInfo,
            bool modemCognitive, bool isRoaming);

    Return<void> getImsRegistrationState(int32_t serial);

    Return<void> sendImsSms(int32_t serial, const ImsSmsMessage& message);

    Return<void> iccTransmitApduBasicChannel(int32_t serial, const SimApdu& message);

    Return<void> iccOpenLogicalChannel(int32_t serial,
            const ::android::hardware::hidl_string& aid, int32_t p2);

    Return<void> iccCloseLogicalChannel(int32_t serial, int32_t channelId);

    Return<void> iccTransmitApduLogicalChannel(int32_t serial, const SimApdu& message);

    Return<void> nvReadItem(int32_t serial, NvItem itemId);

    Return<void> nvWriteItem(int32_t serial, const NvWriteItem& item);

    Return<void> nvWriteCdmaPrl(int32_t serial,
            const ::android::hardware::hidl_vec<uint8_t>& prl);

    Return<void> nvResetConfig(int32_t serial, ResetNvType resetType);

    Return<void> setUiccSubscription(int32_t serial, const SelectUiccSub& uiccSub);

    Return<void> setDataAllowed(int32_t serial, bool allow);

    Return<void> getHardwareConfig(int32_t serial);

    Return<void> requestIccSimAuthentication(int32_t serial,
            int32_t authContext,
            const ::android::hardware::hidl_string& authData,
            const ::android::hardware::hidl_string& aid);

    Return<void> setDataProfile(int32_t serial,
            const ::android::hardware::hidl_vec<DataProfileInfo>& profiles, bool isRoaming);

    Return<void> requestShutdown(int32_t serial);

    Return<void> getRadioCapability(int32_t serial);

    Return<void> setRadioCapability(int32_t serial, const RadioCapability& rc);

    Return<void> startLceService(int32_t serial, int32_t reportInterval, bool pullMode);

    Return<void> stopLceService(int32_t serial);

    Return<void> pullLceData(int32_t serial);

    Return<void> getModemActivityInfo(int32_t serial);

    Return<void> setAllowedCarriers(int32_t serial,
            bool allAllowed,
            const CarrierRestrictions& carriers);

    Return<void> getAllowedCarriers(int32_t serial);

    Return<void> sendDeviceState(int32_t serial, DeviceStateType deviceStateType, bool state);

    Return<void> setIndicationFilter(int32_t serial, int32_t indicationFilter);

    Return<void> startKeepalive(int32_t serial, const V1_1::KeepaliveRequest& keepalive);

    Return<void> stopKeepalive(int32_t serial, int32_t sessionHandle);

    Return<void> setSimCardPower(int32_t serial, bool powerUp);
    Return<void> setSimCardPower_1_1(int32_t serial,
            const V1_1::CardPowerState state);

    Return<void> responseAcknowledgement();

    Return<void> setCarrierInfoForImsiEncryption(int32_t serial,
            const V1_1::ImsiEncryptionInfo& message);

    Return<void> startNetworkScan_1_2(int32_t serial, const V1_2::NetworkScanRequest& request);

    Return<void> setIndicationFilter_1_2(int32_t serial, hidl_bitfield<V1_2::IndicationFilter> indicationFilter);

    Return<void> setSignalStrengthReportingCriteria(int32_t serial, int32_t hysteresisMs, int32_t hysteresisDb,
            const hidl_vec<int32_t>& thresholdsDbm, V1_2::AccessNetwork accessNetwork);

    Return<void> setLinkCapacityReportingCriteria(int32_t serial, int32_t hysteresisMs, int32_t hysteresisDlKbps,
            int32_t hysteresisUlKbps, const hidl_vec<int32_t>& thresholdsDownlinkKbps,
            const hidl_vec<int32_t>& thresholdsUplinkKbps, V1_2::AccessNetwork accessNetwork);

    Return<void> setupDataCall_1_2(int32_t serial, V1_2::AccessNetwork accessNetwork,
            const DataProfileInfo& dataProfileInfo, bool modemCognitive, bool roamingAllowed, bool isRoaming, V1_2::DataRequestReason reason,
            const hidl_vec<hidl_string>& addresses, const hidl_vec<hidl_string>& dnses);

    Return<void> deactivateDataCall_1_2(int32_t serial, int32_t cid, V1_2::DataRequestReason reason);

    ///////////////////////////////////////////////////////////////////////////
    // radio 1.3
    ///////////////////////////////////////////////////////////////////////////
    Return<void> setSystemSelectionChannels(int32_t serial, bool specifyChannels,
            const hidl_vec<V1_1::RadioAccessSpecifier>& specifiers);
    Return<void> enableModem(int32_t serial, bool on);
    Return<void> getModemStackStatus(int32_t serial);

    ///////////////////////////////////////////////////////////////////////////
    // radio 1.4
    ///////////////////////////////////////////////////////////////////////////
    Return<void> setupDataCall_1_4(int32_t serial, V1_4::AccessNetwork accessNetwork,
            const V1_4::DataProfileInfo& dataProfileInfo, bool roamingAllowed,
            V1_2::DataRequestReason reason, const hidl_vec<hidl_string>& addresses,
            const hidl_vec<hidl_string>& dnses);
    Return<void> setInitialAttachApn_1_4(int32_t serial, const V1_4::DataProfileInfo& dataProfileInfo);
    Return<void> setDataProfile_1_4(int32_t serial, const hidl_vec<V1_4::DataProfileInfo>& profiles);
    Return<void> emergencyDial(int32_t serial, const Dial& dialInfo,
            hidl_bitfield<V1_4::EmergencyServiceCategory> categories,
            const hidl_vec<hidl_string>& urns, V1_4::EmergencyCallRouting routing,
            bool hasKnownUserIntentEmergency, bool isTesting);
    Return<void> startNetworkScan_1_4(int32_t serial, const V1_2::NetworkScanRequest& request);
    Return<void> getPreferredNetworkTypeBitmap(int32_t serial);
    Return<void> setPreferredNetworkTypeBitmap(int32_t serial, hidl_bitfield<V1_4::RadioAccessFamily> networkTypeBitmap);
    Return<void> setAllowedCarriers_1_4(int32_t serial, const V1_4::CarrierRestrictionsWithPriority& carriers,
            V1_4::SimLockMultiSimPolicy multiSimPolicy);
    Return<void> getAllowedCarriers_1_4(int32_t serial);
    Return<void> getSignalStrength_1_4(int32_t serial);

    ///////////////////////////////////////////////////////////////////////////
    // radio 1.5
    ///////////////////////////////////////////////////////////////////////////
    Return<void> setSignalStrengthReportingCriteria_1_5(int32_t serial,
            const V1_5::SignalThresholdInfo& signalThresholdInfo, V1_5::AccessNetwork accessNetwork);
    Return<void> enableUiccApplications(int32_t serial, bool enable);
    Return<void> areUiccApplicationsEnabled(int32_t serial);
    Return<void> setSystemSelectionChannels_1_5(int32_t serial, bool specifyChannels,
            const hidl_vec<V1_5::RadioAccessSpecifier>& specifiers);
    Return<void> startNetworkScan_1_5(int32_t serial, const V1_5::NetworkScanRequest& networkScanRequest);
    Return<void> setupDataCall_1_5(int32_t serial, V1_5::AccessNetwork accessNetwork,
            const V1_5::DataProfileInfo& dataProfileInfo, bool roamingAllowed,
            V1_2::DataRequestReason reason, const hidl_vec<V1_5::LinkAddress>& addresses,
            const hidl_vec<hidl_string>& dnses);
    Return<void> setInitialAttachApn_1_5(int32_t serial, const V1_5::DataProfileInfo& dataProfileInfo);
    Return<void> setDataProfile_1_5(int32_t serial, const hidl_vec<V1_5::DataProfileInfo>& profiles);
    Return<void> setRadioPower_1_5(int32_t serial, bool powerOn, bool forEmergencyCall,
            bool preferredForEmergencyCall);
    Return<void> setIndicationFilter_1_5(int32_t serial, hidl_bitfield<V1_5::IndicationFilter> indicationFilter);
    Return<void> getBarringInfo(int32_t serial);
    Return<void> getVoiceRegistrationState_1_5(int32_t serial);
    Return<void> getDataRegistrationState_1_5(int32_t serial);
    Return<void> setNetworkSelectionModeManual_1_5(int32_t serial, const ::android::hardware::hidl_string& operatorNumeric,
                    V1_5::RadioAccessNetworks ran);
    Return<void> sendCdmaSmsExpectMore(int32_t serial, const CdmaSmsMessage& sms);
    Return<void> supplySimDepersonalization(int32_t serial, V1_5::PersoSubstate persoType, const ::android::hardware::hidl_string& controlKey);
    Return<void> setLinkCapacityReportingCriteria_1_5(int32_t serial, int32_t hysteresisMs, int32_t hysteresisDlKbps,
            int32_t hysteresisUlKbps, const hidl_vec<int32_t>& thresholdsDownlinkKbps,
            const hidl_vec<int32_t>& thresholdsUplinkKbps, V1_5::AccessNetwork accessNetwork);
