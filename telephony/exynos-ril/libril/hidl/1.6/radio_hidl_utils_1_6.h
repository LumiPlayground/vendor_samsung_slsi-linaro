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
#include <android/hardware/radio/1.6/IRadioResponse.h>
#include <android/hardware/radio/1.6/IRadioIndication.h>

struct RadioResponseBase_1_6 : public ::android::hardware::radio::V1_6::IRadioResponse {
    RadioResponseBase_1_6() = default;
    virtual ~RadioResponseBase_1_6() = default;

    virtual ::android::hardware::Return<void> getIccCardStatusResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::radio::V1_0::CardStatus& cardStatus);

    virtual ::android::hardware::Return<void> supplyIccPinForAppResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
                                            int32_t remainingRetries);

    virtual ::android::hardware::Return<void> supplyIccPukForAppResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
                                            int32_t remainingRetries);

    virtual ::android::hardware::Return<void> supplyIccPin2ForAppResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
                                             int32_t remainingRetries);

    virtual ::android::hardware::Return<void> supplyIccPuk2ForAppResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
                                             int32_t remainingRetries);

    virtual ::android::hardware::Return<void> changeIccPinForAppResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
                                            int32_t remainingRetries);

    virtual ::android::hardware::Return<void> changeIccPin2ForAppResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
                                             int32_t remainingRetries);

    virtual ::android::hardware::Return<void> supplyNetworkDepersonalizationResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
                                                        int32_t remainingRetries);

    virtual ::android::hardware::Return<void> getCurrentCallsResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::hidl_vec<::android::hardware::radio::V1_0::Call>& calls);

    virtual ::android::hardware::Return<void> dialResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> getIMSIForAppResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
                                       const ::android::hardware::hidl_string& imsi);

    virtual ::android::hardware::Return<void> hangupConnectionResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> hangupWaitingOrBackgroundResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> hangupForegroundResumeBackgroundResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> switchWaitingOrHoldingAndActiveResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> conferenceResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> rejectCallResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> getLastCallFailCauseResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
                                              const ::android::hardware::radio::V1_0::LastCallFailCauseInfo& failCauseInfo);

    virtual ::android::hardware::Return<void> getSignalStrengthResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::radio::V1_0::SignalStrength& sigStrength);

    virtual ::android::hardware::Return<void> getVoiceRegistrationStateResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::radio::V1_0::VoiceRegStateResult& voiceRegResponse);

    virtual ::android::hardware::Return<void> getDataRegistrationStateResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::radio::V1_0::DataRegStateResult& dataRegResponse);

    virtual ::android::hardware::Return<void> getOperatorResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
                                     const ::android::hardware::hidl_string& longName,
                                     const ::android::hardware::hidl_string& shortName,
                                     const ::android::hardware::hidl_string& numeric);

    virtual ::android::hardware::Return<void> setRadioPowerResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> sendDtmfResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> sendSmsResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, const ::android::hardware::radio::V1_0::SendSmsResult& sms);

    virtual ::android::hardware::Return<void> sendSMSExpectMoreResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, const ::android::hardware::radio::V1_0::SendSmsResult& sms);

    virtual ::android::hardware::Return<void> setupDataCallResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const android::hardware::radio::V1_0::SetupDataCallResult& dcResponse);

    virtual ::android::hardware::Return<void> iccIOForAppResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, const ::android::hardware::radio::V1_0::IccIoResult& iccIo);

    virtual ::android::hardware::Return<void> sendUssdResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> cancelPendingUssdResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> getClirResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, int32_t n, int32_t m);

    virtual ::android::hardware::Return<void> setClirResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> getCallForwardStatusResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::hidl_vec<::android::hardware::radio::V1_0::CallForwardInfo>& call_forwardInfos);

    virtual ::android::hardware::Return<void> setCallForwardResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> getCallWaitingResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, bool enable,
                                        int32_t serviceClass);

    virtual ::android::hardware::Return<void> setCallWaitingResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> acknowledgeLastIncomingGsmSmsResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> acceptCallResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> deactivateDataCallResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> getFacilityLockForAppResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, int32_t response);

    virtual ::android::hardware::Return<void> setFacilityLockForAppResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, int32_t retry);

    virtual ::android::hardware::Return<void> setBarringPasswordResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> getNetworkSelectionModeResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, bool manual);

    virtual ::android::hardware::Return<void> setNetworkSelectionModeAutomaticResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> setNetworkSelectionModeManualResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> getAvailableNetworksResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::hidl_vec<::android::hardware::radio::V1_0::OperatorInfo>& networkInfos);

    virtual ::android::hardware::Return<void> startDtmfResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> stopDtmfResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> getBasebandVersionResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
                                            const ::android::hardware::hidl_string& version);

    virtual ::android::hardware::Return<void> separateConnectionResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> setMuteResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> getMuteResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, bool enable);

    virtual ::android::hardware::Return<void> getClipResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, ::android::hardware::radio::V1_0::ClipStatus status);

    virtual ::android::hardware::Return<void> getDataCallListResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::hidl_vec<
                    android::hardware::radio::V1_0::SetupDataCallResult>& dcResponse);

    virtual ::android::hardware::Return<void> sendOemRilRequestRawResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
                                              const ::android::hardware::hidl_vec<uint8_t>& data);

    virtual ::android::hardware::Return<void> sendOemRilRequestStringsResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::hidl_vec<::android::hardware::hidl_string>& data);

    virtual ::android::hardware::Return<void> setSuppServiceNotificationsResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> writeSmsToSimResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, int32_t index);

    virtual ::android::hardware::Return<void> deleteSmsOnSimResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> setBandModeResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> getAvailableBandModesResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::hidl_vec<::android::hardware::radio::V1_0::RadioBandMode>& bandModes);

    virtual ::android::hardware::Return<void> sendEnvelopeResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
                                      const ::android::hardware::hidl_string& commandResponse);

    virtual ::android::hardware::Return<void> sendTerminalResponseToSimResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> handleStkCallSetupRequestFromSimResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> explicitCallTransferResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> setPreferredNetworkTypeResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> getPreferredNetworkTypeResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
                                                 ::android::hardware::radio::V1_0::PreferredNetworkType nwType);

    virtual ::android::hardware::Return<void> getNeighboringCidsResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::hidl_vec<::android::hardware::radio::V1_0::NeighboringCell>& cells);

    virtual ::android::hardware::Return<void> setLocationUpdatesResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> setCdmaSubscriptionSourceResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> setCdmaRoamingPreferenceResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> getCdmaRoamingPreferenceResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
                                                  ::android::hardware::radio::V1_0::CdmaRoamingType type);

    virtual ::android::hardware::Return<void> setTTYModeResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> getTTYModeResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, ::android::hardware::radio::V1_0::TtyMode mode);

    virtual ::android::hardware::Return<void> setPreferredVoicePrivacyResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> getPreferredVoicePrivacyResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, bool enable);

    virtual ::android::hardware::Return<void> sendCDMAFeatureCodeResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> sendBurstDtmfResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> sendCdmaSmsResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, const ::android::hardware::radio::V1_0::SendSmsResult& sms);

    virtual ::android::hardware::Return<void> acknowledgeLastIncomingCdmaSmsResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> getGsmBroadcastConfigResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::hidl_vec<::android::hardware::radio::V1_0::GsmBroadcastSmsConfigInfo>& configs);

    virtual ::android::hardware::Return<void> setGsmBroadcastConfigResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> setGsmBroadcastActivationResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> getCdmaBroadcastConfigResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::hidl_vec<::android::hardware::radio::V1_0::CdmaBroadcastSmsConfigInfo>& configs);

    virtual ::android::hardware::Return<void> setCdmaBroadcastConfigResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> setCdmaBroadcastActivationResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> getCDMASubscriptionResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
                                             const ::android::hardware::hidl_string& mdn,
                                             const ::android::hardware::hidl_string& hSid,
                                             const ::android::hardware::hidl_string& hNid,
                                             const ::android::hardware::hidl_string& min,
                                             const ::android::hardware::hidl_string& prl);

    virtual ::android::hardware::Return<void> writeSmsToRuimResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, uint32_t index);

    virtual ::android::hardware::Return<void> deleteSmsOnRuimResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> getDeviceIdentityResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
                                           const ::android::hardware::hidl_string& imei,
                                           const ::android::hardware::hidl_string& imeisv,
                                           const ::android::hardware::hidl_string& esn,
                                           const ::android::hardware::hidl_string& meid);

    virtual ::android::hardware::Return<void> exitEmergencyCallbackModeResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> getSmscAddressResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
                                        const ::android::hardware::hidl_string& smsc);

    virtual ::android::hardware::Return<void> setSmscAddressResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> reportSmsMemoryStatusResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> reportStkServiceIsRunningResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> getCdmaSubscriptionSourceResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
                                                   ::android::hardware::radio::V1_0::CdmaSubscriptionSource source);

    virtual ::android::hardware::Return<void> requestIsimAuthenticationResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info, const ::android::hardware::hidl_string& response);

    virtual ::android::hardware::Return<void> acknowledgeIncomingGsmSmsWithPduResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> sendEnvelopeWithStatusResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
                                                const ::android::hardware::radio::V1_0::IccIoResult& iccIo);

    virtual ::android::hardware::Return<void> getVoiceRadioTechnologyResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info, ::android::hardware::radio::V1_0::RadioTechnology rat);

    virtual ::android::hardware::Return<void> getCellInfoListResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::hidl_vec<::android::hardware::radio::V1_0::CellInfo>&
                    cellInfo);

    virtual ::android::hardware::Return<void> setCellInfoListRateResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> setInitialAttachApnResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> getImsRegistrationStateResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, bool isRegistered,
                                                 ::android::hardware::radio::V1_0::RadioTechnologyFamily ratFamily);

    virtual ::android::hardware::Return<void> sendImsSmsResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, const ::android::hardware::radio::V1_0::SendSmsResult& sms);

    virtual ::android::hardware::Return<void> iccTransmitApduBasicChannelResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
                                                     const ::android::hardware::radio::V1_0::IccIoResult& result);

    virtual ::android::hardware::Return<void> iccOpenLogicalChannelResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info, int32_t channelId,
            const ::android::hardware::hidl_vec<int8_t>& selectResponse);

    virtual ::android::hardware::Return<void> iccCloseLogicalChannelResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> iccTransmitApduLogicalChannelResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
                                                       const ::android::hardware::radio::V1_0::IccIoResult& result);

    virtual ::android::hardware::Return<void> nvReadItemResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
                                    const ::android::hardware::hidl_string& result);

    virtual ::android::hardware::Return<void> nvWriteItemResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> nvWriteCdmaPrlResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> nvResetConfigResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> setUiccSubscriptionResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> setDataAllowedResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> getHardwareConfigResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::hidl_vec<::android::hardware::radio::V1_0::HardwareConfig>& config);

    virtual ::android::hardware::Return<void> requestIccSimAuthenticationResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
                                                     const ::android::hardware::radio::V1_0::IccIoResult& result);

    virtual ::android::hardware::Return<void> setDataProfileResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> requestShutdownResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> getRadioCapabilityResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const android::hardware::radio::V1_0::RadioCapability& rc);

    virtual ::android::hardware::Return<void> setRadioCapabilityResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const android::hardware::radio::V1_0::RadioCapability& rc);

    virtual ::android::hardware::Return<void> startLceServiceResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
                                         const ::android::hardware::radio::V1_0::LceStatusInfo& statusInfo);

    virtual ::android::hardware::Return<void> stopLceServiceResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
                                        const ::android::hardware::radio::V1_0::LceStatusInfo& statusInfo);

    virtual ::android::hardware::Return<void> pullLceDataResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, const ::android::hardware::radio::V1_0::LceDataInfo& lceInfo);

    virtual ::android::hardware::Return<void> getModemActivityInfoResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
                                              const ::android::hardware::radio::V1_0::ActivityStatsInfo& activityInfo);

    virtual ::android::hardware::Return<void> setAllowedCarriersResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, int32_t numAllowed);

    virtual ::android::hardware::Return<void> getAllowedCarriersResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, bool allAllowed,
                                            const ::android::hardware::radio::V1_0::CarrierRestrictions& carriers);

    virtual ::android::hardware::Return<void> sendDeviceStateResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> setIndicationFilterResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> setSimCardPowerResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> acknowledgeRequest(int32_t serial);

    /* 1.1 Api */
    virtual ::android::hardware::Return<void> setCarrierInfoForImsiEncryptionResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> setSimCardPowerResponse_1_1(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> startNetworkScanResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> stopNetworkScanResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> startKeepaliveResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
                                        const ::android::hardware::radio::V1_1::KeepaliveStatus& status);

    virtual ::android::hardware::Return<void> stopKeepaliveResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    /* 1.2 Api */
    virtual ::android::hardware::Return<void> setSignalStrengthReportingCriteriaResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> setLinkCapacityReportingCriteriaResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> getIccCardStatusResponse_1_2(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::radio::V1_2::CardStatus& card_status);

    virtual ::android::hardware::Return<void> getCurrentCallsResponse_1_2(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::hidl_vec<::android::hardware::radio::V1_2::Call>& calls);

    virtual ::android::hardware::Return<void> getSignalStrengthResponse_1_2(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::radio::V1_2::SignalStrength& sig_strength);

    virtual ::android::hardware::Return<void> getCellInfoListResponse_1_2(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::hidl_vec<::android::hardware::radio::V1_2::CellInfo>&
                    cellInfo);

    virtual ::android::hardware::Return<void> getVoiceRegistrationStateResponse_1_2(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::radio::V1_2::VoiceRegStateResult& voiceRegResponse);

    virtual ::android::hardware::Return<void> getDataRegistrationStateResponse_1_2(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::radio::V1_2::DataRegStateResult& dataRegResponse);

    /* 1.3 Api */
    virtual ::android::hardware::Return<void> setSystemSelectionChannelsResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> enableModemResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> getModemStackStatusResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, const bool enabled);

    /* 1.4 Api */
    virtual ::android::hardware::Return<void> emergencyDialResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> startNetworkScanResponse_1_4(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> getSignalStrengthResponse_1_4(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::radio::V1_4::SignalStrength& sig_strength);

    virtual ::android::hardware::Return<void> getCellInfoListResponse_1_4(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::hidl_vec<::android::hardware::radio::V1_4::CellInfo>&
                    cellInfo);

    virtual ::android::hardware::Return<void> getDataRegistrationStateResponse_1_4(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::radio::V1_4::DataRegStateResult& dataRegResponse);

    virtual ::android::hardware::Return<void> getIccCardStatusResponse_1_4(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::radio::V1_4::CardStatus& card_status);

    virtual ::android::hardware::Return<void> getPreferredNetworkTypeBitmapResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::hidl_bitfield<
                    ::android::hardware::radio::V1_4::RadioAccessFamily>
                    networkTypeBitmap);

    virtual ::android::hardware::Return<void> setPreferredNetworkTypeBitmapResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> getDataCallListResponse_1_4(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::hidl_vec<
                    ::android::hardware::radio::V1_4::SetupDataCallResult>& dcResponse);

    virtual ::android::hardware::Return<void> setupDataCallResponse_1_4(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const android::hardware::radio::V1_4::SetupDataCallResult& dcResponse);

    virtual ::android::hardware::Return<void> setAllowedCarriersResponse_1_4(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> getAllowedCarriersResponse_1_4(const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
                                                const ::android::hardware::radio::V1_4::CarrierRestrictionsWithPriority& carriers,
                                                ::android::hardware::radio::V1_4::SimLockMultiSimPolicy multiSimPolicy);

    /* 1.5 Api */
    virtual ::android::hardware::Return<void> setSignalStrengthReportingCriteriaResponse_1_5(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> setLinkCapacityReportingCriteriaResponse_1_5(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> enableUiccApplicationsResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> areUiccApplicationsEnabledResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, bool enabled);

    virtual ::android::hardware::Return<void> canToggleUiccApplicationsEnablementResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
                                                             bool canToggle);

    virtual ::android::hardware::Return<void> setSystemSelectionChannelsResponse_1_5(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> startNetworkScanResponse_1_5(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> setupDataCallResponse_1_5(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const android::hardware::radio::V1_5::SetupDataCallResult& dcResponse);

    virtual ::android::hardware::Return<void> getDataCallListResponse_1_5(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::hidl_vec<::android::hardware::radio::V1_5::SetupDataCallResult>& dcResponse);

    virtual ::android::hardware::Return<void> setInitialAttachApnResponse_1_5(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> setDataProfileResponse_1_5(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> setRadioPowerResponse_1_5(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> setIndicationFilterResponse_1_5(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> getBarringInfoResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::radio::V1_5::CellIdentity& cellIdentity,
            const ::android::hardware::hidl_vec<::android::hardware::radio::V1_5::BarringInfo>&
                    barringInfos);

    virtual ::android::hardware::Return<void> getVoiceRegistrationStateResponse_1_5(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::radio::V1_5::RegStateResult& regResponse);

    virtual ::android::hardware::Return<void> getDataRegistrationStateResponse_1_5(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::radio::V1_5::RegStateResult& regResponse);

    virtual ::android::hardware::Return<void> getCellInfoListResponse_1_5(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::hidl_vec<::android::hardware::radio::V1_5::CellInfo>&
                    cellInfo);

    virtual ::android::hardware::Return<void> setNetworkSelectionModeManualResponse_1_5(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> sendCdmaSmsExpectMoreResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
                                               const ::android::hardware::radio::V1_0::SendSmsResult& sms);

    virtual ::android::hardware::Return<void> supplySimDepersonalizationResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            ::android::hardware::radio::V1_5::PersoSubstate persoType, int32_t remainingRetries);

    virtual ::android::hardware::Return<void> getIccCardStatusResponse_1_5(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::radio::V1_5::CardStatus& card_status);

    /* 1.6 Api */
    virtual ::android::hardware::Return<void> setRadioPowerResponse_1_6(
              const ::android::hardware::radio::V1_6::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> setupDataCallResponse_1_6(
              const ::android::hardware::radio::V1_6::RadioResponseInfo& info,
              const android::hardware::radio::V1_6::SetupDataCallResult& dcResponse);

    virtual ::android::hardware::Return<void> getDataCallListResponse_1_6(
              const ::android::hardware::radio::V1_6::RadioResponseInfo& info,
              const ::android::hardware::hidl_vec<::android::hardware::radio::V1_6::SetupDataCallResult>& dcResponse);

    virtual ::android::hardware::Return<void> sendSmsResponse_1_6(
              const ::android::hardware::radio::V1_6::RadioResponseInfo& info,
              const ::android::hardware::radio::V1_0::SendSmsResult& sms);

    virtual ::android::hardware::Return<void> sendSmsExpectMoreResponse_1_6(
              const ::android::hardware::radio::V1_6::RadioResponseInfo& info,
              const ::android::hardware::radio::V1_0::SendSmsResult& sms);

    virtual ::android::hardware::Return<void> sendCdmaSmsResponse_1_6(
              const ::android::hardware::radio::V1_6::RadioResponseInfo& info,
              const ::android::hardware::radio::V1_0::SendSmsResult& sms);

    virtual ::android::hardware::Return<void> setSimCardPowerResponse_1_6(
              const ::android::hardware::radio::V1_6::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> sendCdmaSmsExpectMoreResponse_1_6(
              const ::android::hardware::radio::V1_6::RadioResponseInfo& info,
              const ::android::hardware::radio::V1_0::SendSmsResult& sms);

    virtual ::android::hardware::Return<void> setNrDualConnectivityStateResponse(
              const ::android::hardware::radio::V1_6::RadioResponseInfo& info);
    virtual ::android::hardware::Return<void> isNrDualConnectivityEnabledResponse(
              const ::android::hardware::radio::V1_6::RadioResponseInfo& info, bool isEnabled);

    virtual ::android::hardware::Return<void> allocatePduSessionIdResponse(
              const ::android::hardware::radio::V1_6::RadioResponseInfo& info, int32_t id);

    virtual ::android::hardware::Return<void> releasePduSessionIdResponse(
              const ::android::hardware::radio::V1_6::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> startHandoverResponse(
              const ::android::hardware::radio::V1_6::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> cancelHandoverResponse(
              const ::android::hardware::radio::V1_6::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> setAllowedNetworkTypesBitmapResponse(
              const ::android::hardware::radio::V1_6::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> getAllowedNetworkTypesBitmapResponse(
              const ::android::hardware::radio::V1_6::RadioResponseInfo& info,
              const ::android::hardware::hidl_bitfield<
                      ::android::hardware::radio::V1_4::RadioAccessFamily>
                      networkTypeBitmap);

    virtual ::android::hardware::Return<void> setDataThrottlingResponse(
              const ::android::hardware::radio::V1_6::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> getSystemSelectionChannelsResponse(
              const ::android::hardware::radio::V1_6::RadioResponseInfo& info,
              const ::android::hardware::hidl_vec<::android::hardware::radio::V1_5::RadioAccessSpecifier>& specifier);

    virtual ::android::hardware::Return<void> getSignalStrengthResponse_1_6(
              const ::android::hardware::radio::V1_6::RadioResponseInfo& info,
              const ::android::hardware::radio::V1_6::SignalStrength& sig_strength);

    virtual ::android::hardware::Return<void> getCellInfoListResponse_1_6(
              const ::android::hardware::radio::V1_6::RadioResponseInfo& info,
              const ::android::hardware::hidl_vec<::android::hardware::radio::V1_6::CellInfo>&
                      cellInfo);

    virtual ::android::hardware::Return<void> getVoiceRegistrationStateResponse_1_6(
              const ::android::hardware::radio::V1_6::RadioResponseInfo& info,
              const ::android::hardware::radio::V1_6::RegStateResult& regResponse);

    virtual ::android::hardware::Return<void> getDataRegistrationStateResponse_1_6(
              const ::android::hardware::radio::V1_6::RadioResponseInfo& info,
              const ::android::hardware::radio::V1_6::RegStateResult& regResponse);

    virtual ::android::hardware::Return<void> getCurrentCallsResponse_1_6(
              const ::android::hardware::radio::V1_6::RadioResponseInfo& info,
              const ::android::hardware::hidl_vec<::android::hardware::radio::V1_6::Call>& calls);

    virtual ::android::hardware::Return<void> getSlicingConfigResponse(
              const ::android::hardware::radio::V1_6::RadioResponseInfo& info,
              const ::android::hardware::radio::V1_6::SlicingConfig& slicingConfig);

    virtual ::android::hardware::Return<void> getSimPhonebookRecordsResponse(
              const ::android::hardware::radio::V1_6::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> getSimPhonebookCapacityResponse(
              const ::android::hardware::radio::V1_6::RadioResponseInfo& info,
              const ::android::hardware::radio::V1_6::PhonebookCapacity& capacity);

    virtual ::android::hardware::Return<void> updateSimPhonebookRecordsResponse(
              const ::android::hardware::radio::V1_6::RadioResponseInfo& info,
              int32_t updatedRecordIndex);
};

struct RadioIndicationBase_1_6 : public ::android::hardware::radio::V1_6::IRadioIndication {
    RadioIndicationBase_1_6() = default;
    virtual ~RadioIndicationBase_1_6() = default;

    /* 1.6 Api */
    virtual ::android::hardware::Return<void> dataCallListChanged_1_6(
            ::android::hardware::radio::V1_0::RadioIndicationType type,
            const ::android::hardware::hidl_vec<::android::hardware::radio::V1_6::SetupDataCallResult>& dcList);

    virtual ::android::hardware::Return<void> unthrottleApn(::android::hardware::radio::V1_0::RadioIndicationType type,
                               const ::android::hardware::hidl_string& apn);

    virtual ::android::hardware::Return<void> currentSignalStrength_1_6(
            ::android::hardware::radio::V1_0::RadioIndicationType type,
            const ::android::hardware::radio::V1_6::SignalStrength& signalStrength);

    virtual ::android::hardware::Return<void> networkScanResult_1_6(
            ::android::hardware::radio::V1_0::RadioIndicationType type,
            const ::android::hardware::radio::V1_6::NetworkScanResult& result);

    virtual ::android::hardware::Return<void> cellInfoList_1_6(
            ::android::hardware::radio::V1_0::RadioIndicationType type,
            const ::android::hardware::hidl_vec<::android::hardware::radio::V1_6::CellInfo>&
                    records);

    virtual ::android::hardware::Return<void> currentLinkCapacityEstimate_1_6(
            ::android::hardware::radio::V1_0::RadioIndicationType type,
            const ::android::hardware::radio::V1_6::LinkCapacityEstimate& lce);

    virtual ::android::hardware::Return<void> currentPhysicalChannelConfigs_1_6(
            ::android::hardware::radio::V1_0::RadioIndicationType type,
            const ::android::hardware::hidl_vec<
                    ::android::hardware::radio::V1_6::PhysicalChannelConfig>& configs);

    virtual ::android::hardware::Return<void> simPhonebookChanged(::android::hardware::radio::V1_0::RadioIndicationType type);

    virtual ::android::hardware::Return<void> simPhonebookRecordsReceived(
            ::android::hardware::radio::V1_0::RadioIndicationType type,
            ::android::hardware::radio::V1_6::PbReceivedStatus status,
            const ::android::hardware::hidl_vec<::android::hardware::radio::V1_6::PhonebookRecordInfo>&
                    records);

    /* 1.5 Api */
    virtual ::android::hardware::Return<void> uiccApplicationsEnablementChanged(::android::hardware::radio::V1_0::RadioIndicationType type, bool enabled);

    virtual ::android::hardware::Return<void> registrationFailed(
            ::android::hardware::radio::V1_0::RadioIndicationType type,
            const ::android::hardware::radio::V1_5::CellIdentity& cellIdentity,
            const ::android::hardware::hidl_string& chosenPlmn,
            ::android::hardware::hidl_bitfield<::android::hardware::radio::V1_5::Domain> domain,
            int32_t causeCode, int32_t additionalCauseCode);

    virtual ::android::hardware::Return<void> barringInfoChanged(
            ::android::hardware::radio::V1_0::RadioIndicationType /*type*/,
            const ::android::hardware::radio::V1_5::CellIdentity& /*cellIdentity*/,
            const ::android::hardware::hidl_vec<::android::hardware::radio::V1_5::BarringInfo>&
            /*barringInfos*/);

    virtual ::android::hardware::Return<void> networkScanResult_1_5(
            ::android::hardware::radio::V1_0::RadioIndicationType type,
            const ::android::hardware::radio::V1_5::NetworkScanResult& result);

    virtual ::android::hardware::Return<void> cellInfoList_1_5(
            ::android::hardware::radio::V1_0::RadioIndicationType type,
            const ::android::hardware::hidl_vec<::android::hardware::radio::V1_5::CellInfo>&
                    records);

    virtual ::android::hardware::Return<void> dataCallListChanged_1_5(
            ::android::hardware::radio::V1_0::RadioIndicationType type,
            const ::android::hardware::hidl_vec<::android::hardware::radio::V1_5::SetupDataCallResult>& dcList);

    /* 1.4 Api */
    virtual ::android::hardware::Return<void> currentEmergencyNumberList(
            ::android::hardware::radio::V1_0::RadioIndicationType type,
            const ::android::hardware::hidl_vec<::android::hardware::radio::V1_4::EmergencyNumber>& emergencyNumberList);

    virtual ::android::hardware::Return<void> cellInfoList_1_4(
            ::android::hardware::radio::V1_0::RadioIndicationType type,
            const ::android::hardware::hidl_vec<::android::hardware::radio::V1_4::CellInfo>&
                    records);

    virtual ::android::hardware::Return<void> currentSignalStrength_1_4(
            ::android::hardware::radio::V1_0::RadioIndicationType type,
            const ::android::hardware::radio::V1_4::SignalStrength& signalStrength);

    virtual ::android::hardware::Return<void> networkScanResult_1_4(
            ::android::hardware::radio::V1_0::RadioIndicationType type,
            const ::android::hardware::radio::V1_4::NetworkScanResult& result);

    virtual ::android::hardware::Return<void> currentPhysicalChannelConfigs_1_4(
            ::android::hardware::radio::V1_0::RadioIndicationType type,
            const ::android::hardware::hidl_vec<
                    ::android::hardware::radio::V1_4::PhysicalChannelConfig>& configs);

    virtual ::android::hardware::Return<void> dataCallListChanged_1_4(
            ::android::hardware::radio::V1_0::RadioIndicationType type,
            const ::android::hardware::hidl_vec<
                    android::hardware::radio::V1_4::SetupDataCallResult>& dcList);

    /* 1.2 Api */
    virtual ::android::hardware::Return<void> networkScanResult_1_2(
            ::android::hardware::radio::V1_0::RadioIndicationType type,
            const ::android::hardware::radio::V1_2::NetworkScanResult& result);

    virtual ::android::hardware::Return<void> cellInfoList_1_2(
            ::android::hardware::radio::V1_0::RadioIndicationType type,
            const ::android::hardware::hidl_vec<::android::hardware::radio::V1_2::CellInfo>&
                    records);

    virtual ::android::hardware::Return<void> currentLinkCapacityEstimate(
            ::android::hardware::radio::V1_0::RadioIndicationType type,
            const ::android::hardware::radio::V1_2::LinkCapacityEstimate& lce);

    virtual ::android::hardware::Return<void> currentPhysicalChannelConfigs(
            ::android::hardware::radio::V1_0::RadioIndicationType type,
            const ::android::hardware::hidl_vec<
                    ::android::hardware::radio::V1_2::PhysicalChannelConfig>& configs);

    virtual ::android::hardware::Return<void> currentSignalStrength_1_2(
            ::android::hardware::radio::V1_0::RadioIndicationType type,
            const ::android::hardware::radio::V1_2::SignalStrength& signalStrength);

    /* 1.1 Api */
    virtual ::android::hardware::Return<void> carrierInfoForImsiEncryption(::android::hardware::radio::V1_0::RadioIndicationType info);

    virtual ::android::hardware::Return<void> networkScanResult(
            ::android::hardware::radio::V1_0::RadioIndicationType type,
            const ::android::hardware::radio::V1_1::NetworkScanResult& result);

    virtual ::android::hardware::Return<void> keepaliveStatus(::android::hardware::radio::V1_0::RadioIndicationType type, const ::android::hardware::radio::V1_1::KeepaliveStatus& status);

    /* 1.0 Api */
    virtual ::android::hardware::Return<void> radioStateChanged(::android::hardware::radio::V1_0::RadioIndicationType type, ::android::hardware::radio::V1_0::RadioState radioState);

    virtual ::android::hardware::Return<void> callStateChanged(::android::hardware::radio::V1_0::RadioIndicationType type);

    virtual ::android::hardware::Return<void> networkStateChanged(::android::hardware::radio::V1_0::RadioIndicationType type);

    virtual ::android::hardware::Return<void> newSms(::android::hardware::radio::V1_0::RadioIndicationType type,
                        const ::android::hardware::hidl_vec<uint8_t>& pdu);

    virtual ::android::hardware::Return<void> newSmsStatusReport(::android::hardware::radio::V1_0::RadioIndicationType type,
                                    const ::android::hardware::hidl_vec<uint8_t>& pdu);

    virtual ::android::hardware::Return<void> newSmsOnSim(::android::hardware::radio::V1_0::RadioIndicationType type, int32_t recordNumber);

    virtual ::android::hardware::Return<void> onUssd(::android::hardware::radio::V1_0::RadioIndicationType type, ::android::hardware::radio::V1_0::UssdModeType modeType,
                        const ::android::hardware::hidl_string& msg);

    virtual ::android::hardware::Return<void> nitzTimeReceived(::android::hardware::radio::V1_0::RadioIndicationType type,
                                  const ::android::hardware::hidl_string& nitzTime,
                                  uint64_t receivedTime);

    virtual ::android::hardware::Return<void> currentSignalStrength(
            ::android::hardware::radio::V1_0::RadioIndicationType type,
            const ::android::hardware::radio::V1_0::SignalStrength& signalStrength);

    virtual ::android::hardware::Return<void> dataCallListChanged(
            ::android::hardware::radio::V1_0::RadioIndicationType type,
            const ::android::hardware::hidl_vec<
                    android::hardware::radio::V1_0::SetupDataCallResult>& dcList);

    virtual ::android::hardware::Return<void> suppSvcNotify(::android::hardware::radio::V1_0::RadioIndicationType type, const ::android::hardware::radio::V1_0::SuppSvcNotification& suppSvc);

    virtual ::android::hardware::Return<void> stkSessionEnd(::android::hardware::radio::V1_0::RadioIndicationType type);

    virtual ::android::hardware::Return<void> stkProactiveCommand(::android::hardware::radio::V1_0::RadioIndicationType type,
                                     const ::android::hardware::hidl_string& cmd);

    virtual ::android::hardware::Return<void> stkEventNotify(::android::hardware::radio::V1_0::RadioIndicationType type,
                                const ::android::hardware::hidl_string& cmd);

    virtual ::android::hardware::Return<void> stkCallSetup(::android::hardware::radio::V1_0::RadioIndicationType type, int64_t timeout);

    virtual ::android::hardware::Return<void> simSmsStorageFull(::android::hardware::radio::V1_0::RadioIndicationType type);

    virtual ::android::hardware::Return<void> simRefresh(::android::hardware::radio::V1_0::RadioIndicationType type, const ::android::hardware::radio::V1_0::SimRefreshResult& refreshResult);

    virtual ::android::hardware::Return<void> callRing(::android::hardware::radio::V1_0::RadioIndicationType type, bool isGsm, const ::android::hardware::radio::V1_0::CdmaSignalInfoRecord& record);

    virtual ::android::hardware::Return<void> simStatusChanged(::android::hardware::radio::V1_0::RadioIndicationType type);

    virtual ::android::hardware::Return<void> cdmaNewSms(::android::hardware::radio::V1_0::RadioIndicationType type, const ::android::hardware::radio::V1_0::CdmaSmsMessage& msg);

    virtual ::android::hardware::Return<void> newBroadcastSms(::android::hardware::radio::V1_0::RadioIndicationType type,
                                 const ::android::hardware::hidl_vec<uint8_t>& data);

    virtual ::android::hardware::Return<void> cdmaRuimSmsStorageFull(::android::hardware::radio::V1_0::RadioIndicationType type);

    virtual ::android::hardware::Return<void> restrictedStateChanged(::android::hardware::radio::V1_0::RadioIndicationType type, ::android::hardware::radio::V1_0::PhoneRestrictedState state);

    virtual ::android::hardware::Return<void> enterEmergencyCallbackMode(::android::hardware::radio::V1_0::RadioIndicationType type);

    virtual ::android::hardware::Return<void> cdmaCallWaiting(::android::hardware::radio::V1_0::RadioIndicationType type,
                                 const ::android::hardware::radio::V1_0::CdmaCallWaiting& callWaitingRecord);

    virtual ::android::hardware::Return<void> cdmaOtaProvisionStatus(::android::hardware::radio::V1_0::RadioIndicationType type, ::android::hardware::radio::V1_0::CdmaOtaProvisionStatus status);

    virtual ::android::hardware::Return<void> cdmaInfoRec(::android::hardware::radio::V1_0::RadioIndicationType type, const ::android::hardware::radio::V1_0::CdmaInformationRecords& records);

    virtual ::android::hardware::Return<void> indicateRingbackTone(::android::hardware::radio::V1_0::RadioIndicationType type, bool start);

    virtual ::android::hardware::Return<void> resendIncallMute(::android::hardware::radio::V1_0::RadioIndicationType type);

    virtual ::android::hardware::Return<void> cdmaSubscriptionSourceChanged(::android::hardware::radio::V1_0::RadioIndicationType type,
                                               ::android::hardware::radio::V1_0::CdmaSubscriptionSource cdmaSource);

    virtual ::android::hardware::Return<void> cdmaPrlChanged(::android::hardware::radio::V1_0::RadioIndicationType type, int32_t version);

    virtual ::android::hardware::Return<void> exitEmergencyCallbackMode(::android::hardware::radio::V1_0::RadioIndicationType type);

    virtual ::android::hardware::Return<void> rilConnected(::android::hardware::radio::V1_0::RadioIndicationType type);

    virtual ::android::hardware::Return<void> voiceRadioTechChanged(::android::hardware::radio::V1_0::RadioIndicationType type,
                                       ::android::hardware::radio::V1_0::RadioTechnology rat);

    virtual ::android::hardware::Return<void> cellInfoList(
            ::android::hardware::radio::V1_0::RadioIndicationType type,
            const ::android::hardware::hidl_vec<::android::hardware::radio::V1_0::CellInfo>&
                    records);

    virtual ::android::hardware::Return<void> imsNetworkStateChanged(::android::hardware::radio::V1_0::RadioIndicationType type);

    virtual ::android::hardware::Return<void> subscriptionStatusChanged(::android::hardware::radio::V1_0::RadioIndicationType type, bool activate);

    virtual ::android::hardware::Return<void> srvccStateNotify(::android::hardware::radio::V1_0::RadioIndicationType type, ::android::hardware::radio::V1_0::SrvccState state);

    virtual ::android::hardware::Return<void> hardwareConfigChanged(
            ::android::hardware::radio::V1_0::RadioIndicationType type, const ::android::hardware::hidl_vec<::android::hardware::radio::V1_0::HardwareConfig>& configs);

    virtual ::android::hardware::Return<void> radioCapabilityIndication(
            ::android::hardware::radio::V1_0::RadioIndicationType type, const android::hardware::radio::V1_0::RadioCapability& rc);

    virtual ::android::hardware::Return<void> onSupplementaryServiceIndication(::android::hardware::radio::V1_0::RadioIndicationType type,
                                                  const ::android::hardware::radio::V1_0::StkCcUnsolSsResult& ss);

    virtual ::android::hardware::Return<void> stkCallControlAlphaNotify(::android::hardware::radio::V1_0::RadioIndicationType type,
                                           const ::android::hardware::hidl_string& alpha);

    virtual ::android::hardware::Return<void> lceData(::android::hardware::radio::V1_0::RadioIndicationType type, const ::android::hardware::radio::V1_0::LceDataInfo& lce);

    virtual ::android::hardware::Return<void> pcoData(::android::hardware::radio::V1_0::RadioIndicationType type, const ::android::hardware::radio::V1_0::PcoDataInfo& pco);

    virtual ::android::hardware::Return<void> modemReset(::android::hardware::radio::V1_0::RadioIndicationType type,
                            const ::android::hardware::hidl_string& reason);
};