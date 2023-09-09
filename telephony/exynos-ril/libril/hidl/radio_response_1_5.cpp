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
/*
 * radio_response_1_6.cpp
 */
#include "radio_hidl_utils_1_5.h"

using namespace ::android::hardware::radio::V1_5;
using namespace ::android::hardware::radio::V1_4;
using namespace ::android::hardware::radio::V1_3;
using namespace ::android::hardware::radio::V1_2;
using namespace ::android::hardware::radio::V1_1;
using namespace ::android::hardware::radio::V1_0;
using ::android::sp;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;

/* 1.0 Apis */
Return<void> RadioResponseBase_1_5::getIccCardStatusResponse(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/,
        const ::android::hardware::radio::V1_0::CardStatus& /*card_status*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::supplyIccPinForAppResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/,
                                                            int32_t /*remainingRetries*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::supplyIccPukForAppResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/,
                                                            int32_t /*remainingRetries*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::supplyIccPin2ForAppResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/,
                                                             int32_t /*remainingRetries*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::supplyIccPuk2ForAppResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/,
                                                             int32_t /*remainingRetries*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::changeIccPinForAppResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/,
                                                            int32_t /*remainingRetries*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::changeIccPin2ForAppResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/,
                                                             int32_t /*remainingRetries*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::supplyNetworkDepersonalizationResponse(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/, int32_t /*remainingRetries*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::getCurrentCallsResponse(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/,
        const ::android::hardware::hidl_vec<::android::hardware::radio::V1_0::Call>& /*calls*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::dialResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::getIMSIForAppResponse(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/, const ::android::hardware::hidl_string& /*imsi*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::hangupConnectionResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::hangupWaitingOrBackgroundResponse(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::hangupForegroundResumeBackgroundResponse(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::switchWaitingOrHoldingAndActiveResponse(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::conferenceResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::rejectCallResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::getLastCallFailCauseResponse(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/, const LastCallFailCauseInfo& /*failCauseInfo*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::getSignalStrengthResponse(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/,
        const ::android::hardware::radio::V1_0::SignalStrength& /*sig_strength*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::getVoiceRegistrationStateResponse(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/,
        const ::android::hardware::radio::V1_0::VoiceRegStateResult& /*voiceRegResponse*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::getDataRegistrationStateResponse(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/,
        const ::android::hardware::radio::V1_0::DataRegStateResult& /*dataRegResponse*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::getOperatorResponse(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/, const ::android::hardware::hidl_string& /*longName*/,
        const ::android::hardware::hidl_string& /*shortName*/,
        const ::android::hardware::hidl_string& /*numeric*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::setRadioPowerResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::sendDtmfResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::sendSmsResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/,
                                                 const SendSmsResult& /*sms*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::sendSMSExpectMoreResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/,
                                                           const SendSmsResult& /*sms*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::setupDataCallResponse(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/,
        const android::hardware::radio::V1_0::SetupDataCallResult& /*dcResponse*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::iccIOForAppResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/,
                                                     const IccIoResult& /*iccIo*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::sendUssdResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::cancelPendingUssdResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::getClirResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/, int32_t /*n*/,
                                                 int32_t /*m*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::setClirResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::getCallForwardStatusResponse(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/, const ::android::hardware::hidl_vec<CallForwardInfo>&
        /*callForwardInfos*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::setCallForwardResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::getCallWaitingResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/,
                                                        bool /*enable*/, int32_t /*serviceClass*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::setCallWaitingResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::acknowledgeLastIncomingGsmSmsResponse(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::acceptCallResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::deactivateDataCallResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::getFacilityLockForAppResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/,
                                                               int32_t /*response*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::setFacilityLockForAppResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/,
                                                               int32_t /*retry*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::setBarringPasswordResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::getNetworkSelectionModeResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/,
                                                                 bool /*manual*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::setNetworkSelectionModeAutomaticResponse(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::setNetworkSelectionModeManualResponse(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::getAvailableNetworksResponse(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/,
        const ::android::hardware::hidl_vec<OperatorInfo>& /*networkInfos*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::startDtmfResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::stopDtmfResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::getBasebandVersionResponse(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/, const ::android::hardware::hidl_string& /*version*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::separateConnectionResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::setMuteResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::getMuteResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/,
                                                 bool /*enable*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::getClipResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/,
                                                 ClipStatus /*status*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::getDataCallListResponse(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/,
        const ::android::hardware::hidl_vec<android::hardware::radio::V1_0::SetupDataCallResult>&
        /*dcResponse*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::sendOemRilRequestRawResponse(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/, const ::android::hardware::hidl_vec<uint8_t>& /*data*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::sendOemRilRequestStringsResponse(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/,
        const ::android::hardware::hidl_vec<::android::hardware::hidl_string>& /*data*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::setSuppServiceNotificationsResponse(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::writeSmsToSimResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/,
                                                       int32_t /*index*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::deleteSmsOnSimResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::setBandModeResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::getAvailableBandModesResponse(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/,
        const ::android::hardware::hidl_vec<RadioBandMode>& /*bandModes*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::sendEnvelopeResponse(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/,
        const ::android::hardware::hidl_string& /*commandResponse*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::sendTerminalResponseToSimResponse(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::handleStkCallSetupRequestFromSimResponse(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::explicitCallTransferResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::setPreferredNetworkTypeResponse(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::getPreferredNetworkTypeResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/,
                                                                 PreferredNetworkType /*nw_type*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::getNeighboringCidsResponse(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/,
        const ::android::hardware::hidl_vec<NeighboringCell>& /*cells*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::setLocationUpdatesResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::setCdmaSubscriptionSourceResponse(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::setCdmaRoamingPreferenceResponse(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::getCdmaRoamingPreferenceResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/,
                                                                  CdmaRoamingType /*type*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::setTTYModeResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::getTTYModeResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/,
                                                    TtyMode /*mode*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::setPreferredVoicePrivacyResponse(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::getPreferredVoicePrivacyResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/,
                                                                  bool /*enable*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::sendCDMAFeatureCodeResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::sendBurstDtmfResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::sendCdmaSmsResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/,
                                                     const SendSmsResult& /*sms*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::acknowledgeLastIncomingCdmaSmsResponse(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::getGsmBroadcastConfigResponse(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/,
        const ::android::hardware::hidl_vec<GsmBroadcastSmsConfigInfo>& /*configs*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::setGsmBroadcastConfigResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::setGsmBroadcastActivationResponse(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::getCdmaBroadcastConfigResponse(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/,
        const ::android::hardware::hidl_vec<CdmaBroadcastSmsConfigInfo>& /*configs*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::setCdmaBroadcastConfigResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::setCdmaBroadcastActivationResponse(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::getCDMASubscriptionResponse(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/, const ::android::hardware::hidl_string& /*mdn*/,
        const ::android::hardware::hidl_string& /*hSid*/,
        const ::android::hardware::hidl_string& /*hNid*/,
        const ::android::hardware::hidl_string& /*min*/,
        const ::android::hardware::hidl_string& /*prl*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::writeSmsToRuimResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/,
                                                        uint32_t /*index*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::deleteSmsOnRuimResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::getDeviceIdentityResponse(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/, const ::android::hardware::hidl_string& /*imei*/,
        const ::android::hardware::hidl_string& /*imeisv*/,
        const ::android::hardware::hidl_string& /*esn*/,
        const ::android::hardware::hidl_string& /*meid*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::exitEmergencyCallbackModeResponse(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::getSmscAddressResponse(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/, const ::android::hardware::hidl_string& /*smsc*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::setSmscAddressResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::reportSmsMemoryStatusResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::reportStkServiceIsRunningResponse(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::getCdmaSubscriptionSourceResponse(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/, CdmaSubscriptionSource /*source*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::requestIsimAuthenticationResponse(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/, const ::android::hardware::hidl_string& /*response*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::acknowledgeIncomingGsmSmsWithPduResponse(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::sendEnvelopeWithStatusResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/,
                                                                const IccIoResult& /*iccIo*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::getVoiceRadioTechnologyResponse(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/,
        ::android::hardware::radio::V1_0::RadioTechnology /*rat*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::getCellInfoListResponse(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/,
        const ::android::hardware::hidl_vec<
                ::android::hardware::radio::V1_0::CellInfo>& /*cellInfo*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::setCellInfoListRateResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::setInitialAttachApnResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::getImsRegistrationStateResponse(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/, bool /*isRegistered*/,
        RadioTechnologyFamily /*ratFamily*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::sendImsSmsResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/,
                                                    const SendSmsResult& /*sms*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::iccTransmitApduBasicChannelResponse(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/, const IccIoResult& /*result*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::iccOpenLogicalChannelResponse(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/, int32_t /*channelId*/,
        const ::android::hardware::hidl_vec<int8_t>& /*selectResponse*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::iccCloseLogicalChannelResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::iccTransmitApduLogicalChannelResponse(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/, const IccIoResult& /*result*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::nvReadItemResponse(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/, const ::android::hardware::hidl_string& /*result*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::nvWriteItemResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::nvWriteCdmaPrlResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::nvResetConfigResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::setUiccSubscriptionResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::setDataAllowedResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::getHardwareConfigResponse(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/,
        const ::android::hardware::hidl_vec<HardwareConfig>& /*config*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::requestIccSimAuthenticationResponse(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/, const IccIoResult& /*result*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::setDataProfileResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::requestShutdownResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::getRadioCapabilityResponse(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/,
        const android::hardware::radio::V1_0::RadioCapability& /*rc*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::setRadioCapabilityResponse(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/,
        const android::hardware::radio::V1_0::RadioCapability& /*rc*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::startLceServiceResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/,
                                                         const LceStatusInfo& /*statusInfo*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::stopLceServiceResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/,
                                                        const LceStatusInfo& /*statusInfo*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::pullLceDataResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/,
                                                     const LceDataInfo& /*lceInfo*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::getModemActivityInfoResponse(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/, const ActivityStatsInfo& /*activityInfo*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::setAllowedCarriersResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/,
                                                            int32_t /*numAllowed*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::getAllowedCarriersResponse(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/, bool /*allAllowed*/,
        const CarrierRestrictions& /*carriers*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::sendDeviceStateResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::setIndicationFilterResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::setSimCardPowerResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::acknowledgeRequest(int32_t /*serial*/) {
    return Void();
}

/* 1.1 Apis */
Return<void> RadioResponseBase_1_5::setCarrierInfoForImsiEncryptionResponse(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::setSimCardPowerResponse_1_1(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::startNetworkScanResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::stopNetworkScanResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::startKeepaliveResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/,
                                                        const KeepaliveStatus& /*status*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::stopKeepaliveResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

/* 1.2 Apis */
Return<void> RadioResponseBase_1_5::setSignalStrengthReportingCriteriaResponse(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::setLinkCapacityReportingCriteriaResponse(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::getIccCardStatusResponse_1_2(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/,
        const ::android::hardware::radio::V1_2::CardStatus& /*card_status*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::getCurrentCallsResponse_1_2(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/,
        const ::android::hardware::hidl_vec<::android::hardware::radio::V1_2::Call>& /*calls*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::getSignalStrengthResponse_1_2(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/,
        const ::android::hardware::radio::V1_2::SignalStrength& /*sig_strength*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::getSignalStrengthResponse_1_4(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/,
        const ::android::hardware::radio::V1_4::SignalStrength& /*sig_strength*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::getCellInfoListResponse_1_2(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/,
        const ::android::hardware::hidl_vec<
                ::android::hardware::radio::V1_2::CellInfo>& /*cellInfo*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::getVoiceRegistrationStateResponse_1_2(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/,
        const ::android::hardware::radio::V1_2::VoiceRegStateResult& /*voiceRegResponse*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::getDataRegistrationStateResponse_1_2(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/,
        const ::android::hardware::radio::V1_2::DataRegStateResult& /*dataRegResponse*/) {
    return Void();
}

/* 1.3 Apis */
Return<void> RadioResponseBase_1_5::setSystemSelectionChannelsResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::enableModemResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::getModemStackStatusResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/,
                                                             const bool /*enabled*/) {
    return Void();
}

/* 1.4 Apis */
Return<void> RadioResponseBase_1_5::emergencyDialResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::startNetworkScanResponse_1_4(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::getDataRegistrationStateResponse_1_4(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/,
        const ::android::hardware::radio::V1_4::DataRegStateResult& /*dataRegResponse*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::getCellInfoListResponse_1_4(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/,
        const ::android::hardware::hidl_vec<
                ::android::hardware::radio::V1_4::CellInfo>& /*cellInfo*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::getIccCardStatusResponse_1_4(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/,
        const ::android::hardware::radio::V1_4::CardStatus& /*card_status*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::getPreferredNetworkTypeBitmapResponse(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/, const ::android::hardware::hidl_bitfield<
                                               ::android::hardware::radio::V1_4::RadioAccessFamily>
                                               /*networkTypeBitmap*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::setPreferredNetworkTypeBitmapResponse(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::getDataCallListResponse_1_4(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/,
        const ::android::hardware::hidl_vec<::android::hardware::radio::V1_4::SetupDataCallResult>&
        /*dcResponse*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::setupDataCallResponse_1_4(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/,
        const android::hardware::radio::V1_4::SetupDataCallResult& /*dcResponse*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::setAllowedCarriersResponse_1_4(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::getAllowedCarriersResponse_1_4(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/, const CarrierRestrictionsWithPriority& /*carriers*/,
        SimLockMultiSimPolicy /*multiSimPolicy*/) {
    return Void();
}

/* 1.5 Apis */
Return<void> RadioResponseBase_1_5::setSignalStrengthReportingCriteriaResponse_1_5(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /* info */) {
    return Void();
}

Return<void> RadioResponseBase_1_5::setLinkCapacityReportingCriteriaResponse_1_5(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /* info */) {
    return Void();
}

Return<void> RadioResponseBase_1_5::enableUiccApplicationsResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /* info */) {
    return Void();
}

Return<void> RadioResponseBase_1_5::areUiccApplicationsEnabledResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /* info */,
                                                                    bool /* enabled */) {
    return Void();
}

Return<void> RadioResponseBase_1_5::canToggleUiccApplicationsEnablementResponse(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /* info*/, bool /* canToggle*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::setSystemSelectionChannelsResponse_1_5(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::startNetworkScanResponse_1_5(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::setupDataCallResponse_1_5(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/,
        const android::hardware::radio::V1_5::SetupDataCallResult& /* dcResponse */) {
    return Void();
}

Return<void> RadioResponseBase_1_5::getDataCallListResponse_1_5(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/,
        const hidl_vec<::android::hardware::radio::V1_5::SetupDataCallResult>& /* dcResponse */) {
    return Void();
}

Return<void> RadioResponseBase_1_5::setInitialAttachApnResponse_1_5(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::setDataProfileResponse_1_5(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::setRadioPowerResponse_1_5(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::setIndicationFilterResponse_1_5(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::getBarringInfoResponse(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/,
        const ::android::hardware::radio::V1_5::CellIdentity& /*cellIdentity*/,
        const ::android::hardware::hidl_vec<::android::hardware::radio::V1_5::BarringInfo>&
                /*barringInfos*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::getVoiceRegistrationStateResponse_1_5(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/,
        const ::android::hardware::radio::V1_5::RegStateResult& /*regResponse*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::getDataRegistrationStateResponse_1_5(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/,
        const ::android::hardware::radio::V1_5::RegStateResult& /*regResponse*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::getCellInfoListResponse_1_5(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/,
        const ::android::hardware::hidl_vec<
                ::android::hardware::radio::V1_5::CellInfo>& /*cellInfo*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::setNetworkSelectionModeManualResponse_1_5(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::sendCdmaSmsExpectMoreResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/,
                                                               const SendSmsResult& /*sms*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::supplySimDepersonalizationResponse(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/,
        ::android::hardware::radio::V1_5::PersoSubstate /*persoType*/,
        int32_t /*remainingRetries*/) {
    return Void();
}

Return<void> RadioResponseBase_1_5::getIccCardStatusResponse_1_5(
        const ::android::hardware::radio::V1_0::RadioResponseInfo& /*info*/,
        const ::android::hardware::radio::V1_5::CardStatus& /*card_status*/) {
    return Void();
}
