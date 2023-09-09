/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */
/*
 * test_radio_response_1_6.h
 *
 *  Created on: 2021. 8. 11.
 */
#ifndef __TEST_RADIO_HIDL_UTILS_1_6_H__
#define __TEST_RADIO_HIDL_UTILS_1_6_H__

#include <android/hardware/radio/1.6/IRadio.h>
#include <android/hardware/radio/1.6/IRadioResponse.h>
#include <android/hardware/radio/1.6/IRadioIndication.h>
#include <telephony/ril.h>

using namespace ::android::hardware::radio::V1_6;
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

struct TestRadioResponseBase_1_6 : public ::android::hardware::radio::V1_6::IRadioResponse {
    TestRadioResponseBase_1_6() = default;
    virtual ~TestRadioResponseBase_1_6() = default;

    virtual Return<void> getIccCardStatusResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::radio::V1_0::CardStatus& cardStatus);

    virtual Return<void> supplyIccPinForAppResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
                                            int32_t remainingRetries);

    virtual Return<void> supplyIccPukForAppResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
                                            int32_t remainingRetries);

    virtual Return<void> supplyIccPin2ForAppResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
                                             int32_t remainingRetries);

    virtual Return<void> supplyIccPuk2ForAppResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
                                             int32_t remainingRetries);

    virtual Return<void> changeIccPinForAppResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
                                            int32_t remainingRetries);

    virtual Return<void> changeIccPin2ForAppResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
                                             int32_t remainingRetries);

    virtual Return<void> supplyNetworkDepersonalizationResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
                                                        int32_t remainingRetries);

    virtual Return<void> getCurrentCallsResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::hidl_vec<::android::hardware::radio::V1_0::Call>& calls);

    virtual Return<void> dialResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> getIMSIForAppResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
                                       const ::android::hardware::hidl_string& imsi);

    virtual Return<void> hangupConnectionResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> hangupWaitingOrBackgroundResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> hangupForegroundResumeBackgroundResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> switchWaitingOrHoldingAndActiveResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> conferenceResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> rejectCallResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> getLastCallFailCauseResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
                                              const LastCallFailCauseInfo& failCauseInfo);

    virtual Return<void> getSignalStrengthResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::radio::V1_0::SignalStrength& sigStrength);

    virtual Return<void> getVoiceRegistrationStateResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::radio::V1_0::VoiceRegStateResult& voiceRegResponse);

    virtual Return<void> getDataRegistrationStateResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::radio::V1_0::DataRegStateResult& dataRegResponse);

    virtual Return<void> getOperatorResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
                                     const ::android::hardware::hidl_string& longName,
                                     const ::android::hardware::hidl_string& shortName,
                                     const ::android::hardware::hidl_string& numeric);

    virtual Return<void> setRadioPowerResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> sendDtmfResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> sendSmsResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, const SendSmsResult& sms);

    virtual Return<void> sendSMSExpectMoreResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, const SendSmsResult& sms);

    virtual Return<void> setupDataCallResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const android::hardware::radio::V1_0::SetupDataCallResult& dcResponse);

    virtual Return<void> iccIOForAppResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, const IccIoResult& iccIo);

    virtual Return<void> sendUssdResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> cancelPendingUssdResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> getClirResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, int32_t n, int32_t m);

    virtual Return<void> setClirResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> getCallForwardStatusResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::hidl_vec<CallForwardInfo>& call_forwardInfos);

    virtual Return<void> setCallForwardResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> getCallWaitingResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, bool enable,
                                        int32_t serviceClass);

    virtual Return<void> setCallWaitingResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> acknowledgeLastIncomingGsmSmsResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> acceptCallResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> deactivateDataCallResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> getFacilityLockForAppResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, int32_t response);

    virtual Return<void> setFacilityLockForAppResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, int32_t retry);

    virtual Return<void> setBarringPasswordResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> getNetworkSelectionModeResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, bool manual);

    virtual Return<void> setNetworkSelectionModeAutomaticResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> setNetworkSelectionModeManualResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> getAvailableNetworksResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::hidl_vec<OperatorInfo>& networkInfos);

    virtual Return<void> startDtmfResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> stopDtmfResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> getBasebandVersionResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
                                            const ::android::hardware::hidl_string& version);

    virtual Return<void> separateConnectionResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> setMuteResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> getMuteResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, bool enable);

    virtual Return<void> getClipResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, ClipStatus status);

    virtual Return<void> getDataCallListResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::hidl_vec<
                    android::hardware::radio::V1_0::SetupDataCallResult>& dcResponse);

    virtual Return<void> sendOemRilRequestRawResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
                                              const ::android::hardware::hidl_vec<uint8_t>& data);

    virtual Return<void> sendOemRilRequestStringsResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::hidl_vec<::android::hardware::hidl_string>& data);

    virtual Return<void> setSuppServiceNotificationsResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> writeSmsToSimResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, int32_t index);

    virtual Return<void> deleteSmsOnSimResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> setBandModeResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> getAvailableBandModesResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::hidl_vec<RadioBandMode>& bandModes);

    virtual Return<void> sendEnvelopeResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
                                      const ::android::hardware::hidl_string& commandResponse);

    virtual Return<void> sendTerminalResponseToSimResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> handleStkCallSetupRequestFromSimResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> explicitCallTransferResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> setPreferredNetworkTypeResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> getPreferredNetworkTypeResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
                                                 PreferredNetworkType nwType);

    virtual Return<void> getNeighboringCidsResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::hidl_vec<NeighboringCell>& cells);

    virtual Return<void> setLocationUpdatesResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> setCdmaSubscriptionSourceResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> setCdmaRoamingPreferenceResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> getCdmaRoamingPreferenceResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
                                                  CdmaRoamingType type);

    virtual Return<void> setTTYModeResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> getTTYModeResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, TtyMode mode);

    virtual Return<void> setPreferredVoicePrivacyResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> getPreferredVoicePrivacyResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, bool enable);

    virtual Return<void> sendCDMAFeatureCodeResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> sendBurstDtmfResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> sendCdmaSmsResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, const SendSmsResult& sms);

    virtual Return<void> acknowledgeLastIncomingCdmaSmsResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> getGsmBroadcastConfigResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::hidl_vec<GsmBroadcastSmsConfigInfo>& configs);

    virtual Return<void> setGsmBroadcastConfigResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> setGsmBroadcastActivationResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> getCdmaBroadcastConfigResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::hidl_vec<CdmaBroadcastSmsConfigInfo>& configs);

    virtual Return<void> setCdmaBroadcastConfigResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> setCdmaBroadcastActivationResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> getCDMASubscriptionResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
                                             const ::android::hardware::hidl_string& mdn,
                                             const ::android::hardware::hidl_string& hSid,
                                             const ::android::hardware::hidl_string& hNid,
                                             const ::android::hardware::hidl_string& min,
                                             const ::android::hardware::hidl_string& prl);

    virtual Return<void> writeSmsToRuimResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, uint32_t index);

    virtual Return<void> deleteSmsOnRuimResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> getDeviceIdentityResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
                                           const ::android::hardware::hidl_string& imei,
                                           const ::android::hardware::hidl_string& imeisv,
                                           const ::android::hardware::hidl_string& esn,
                                           const ::android::hardware::hidl_string& meid);

    virtual Return<void> exitEmergencyCallbackModeResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> getSmscAddressResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
                                        const ::android::hardware::hidl_string& smsc);

    virtual Return<void> setSmscAddressResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> reportSmsMemoryStatusResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> reportStkServiceIsRunningResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> getCdmaSubscriptionSourceResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
                                                   CdmaSubscriptionSource source);

    virtual Return<void> requestIsimAuthenticationResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info, const ::android::hardware::hidl_string& response);

    virtual Return<void> acknowledgeIncomingGsmSmsWithPduResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> sendEnvelopeWithStatusResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
                                                const IccIoResult& iccIo);

    virtual Return<void> getVoiceRadioTechnologyResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info, ::android::hardware::radio::V1_0::RadioTechnology rat);

    virtual Return<void> getCellInfoListResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::hidl_vec<::android::hardware::radio::V1_0::CellInfo>&
                    cellInfo);

    virtual Return<void> setCellInfoListRateResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> setInitialAttachApnResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> getImsRegistrationStateResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, bool isRegistered,
                                                 RadioTechnologyFamily ratFamily);

    virtual Return<void> sendImsSmsResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, const SendSmsResult& sms);

    virtual Return<void> iccTransmitApduBasicChannelResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
                                                     const IccIoResult& result);

    virtual Return<void> iccOpenLogicalChannelResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info, int32_t channelId,
            const ::android::hardware::hidl_vec<int8_t>& selectResponse);

    virtual Return<void> iccCloseLogicalChannelResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> iccTransmitApduLogicalChannelResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
                                                       const IccIoResult& result);

    virtual Return<void> nvReadItemResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
                                    const ::android::hardware::hidl_string& result);

    virtual Return<void> nvWriteItemResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> nvWriteCdmaPrlResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> nvResetConfigResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> setUiccSubscriptionResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> setDataAllowedResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> getHardwareConfigResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::hidl_vec<HardwareConfig>& config);

    virtual Return<void> requestIccSimAuthenticationResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
                                                     const IccIoResult& result);

    virtual Return<void> setDataProfileResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> requestShutdownResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> getRadioCapabilityResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const android::hardware::radio::V1_0::RadioCapability& rc);

    virtual Return<void> setRadioCapabilityResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const android::hardware::radio::V1_0::RadioCapability& rc);

    virtual Return<void> startLceServiceResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
                                         const LceStatusInfo& statusInfo);

    virtual Return<void> stopLceServiceResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
                                        const LceStatusInfo& statusInfo);

    virtual Return<void> pullLceDataResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, const LceDataInfo& lceInfo);

    virtual Return<void> getModemActivityInfoResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
                                              const ActivityStatsInfo& activityInfo);

    virtual Return<void> setAllowedCarriersResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, int32_t numAllowed);

    virtual Return<void> getAllowedCarriersResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, bool allAllowed,
                                            const CarrierRestrictions& carriers);

    virtual Return<void> sendDeviceStateResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> setIndicationFilterResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> setSimCardPowerResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> acknowledgeRequest(int32_t serial);

    /* 1.1 Api */
    virtual Return<void> setCarrierInfoForImsiEncryptionResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> setSimCardPowerResponse_1_1(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> startNetworkScanResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> stopNetworkScanResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> startKeepaliveResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
                                        const KeepaliveStatus& status);

    virtual Return<void> stopKeepaliveResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    /* 1.2 Api */
    virtual Return<void> setSignalStrengthReportingCriteriaResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> setLinkCapacityReportingCriteriaResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> getIccCardStatusResponse_1_2(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::radio::V1_2::CardStatus& card_status);

    virtual Return<void> getCurrentCallsResponse_1_2(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::hidl_vec<::android::hardware::radio::V1_2::Call>& calls);

    virtual Return<void> getSignalStrengthResponse_1_2(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::radio::V1_2::SignalStrength& sig_strength);

    virtual Return<void> getCellInfoListResponse_1_2(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::hidl_vec<::android::hardware::radio::V1_2::CellInfo>&
                    cellInfo);

    virtual Return<void> getVoiceRegistrationStateResponse_1_2(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::radio::V1_2::VoiceRegStateResult& voiceRegResponse);

    virtual Return<void> getDataRegistrationStateResponse_1_2(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::radio::V1_2::DataRegStateResult& dataRegResponse);

    /* 1.3 Api */
    virtual Return<void> setSystemSelectionChannelsResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> enableModemResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> getModemStackStatusResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, const bool enabled);

    /* 1.4 Api */
    virtual Return<void> emergencyDialResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> startNetworkScanResponse_1_4(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> getSignalStrengthResponse_1_4(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::radio::V1_4::SignalStrength& sig_strength);

    virtual Return<void> getCellInfoListResponse_1_4(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::hidl_vec<::android::hardware::radio::V1_4::CellInfo>&
                    cellInfo);

    virtual Return<void> getDataRegistrationStateResponse_1_4(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::radio::V1_4::DataRegStateResult& dataRegResponse);

    virtual Return<void> getIccCardStatusResponse_1_4(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::radio::V1_4::CardStatus& card_status);

    virtual Return<void> getPreferredNetworkTypeBitmapResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::hidl_bitfield<
                    ::android::hardware::radio::V1_4::RadioAccessFamily>
                    networkTypeBitmap);

    virtual Return<void> setPreferredNetworkTypeBitmapResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> getDataCallListResponse_1_4(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::hidl_vec<
                    ::android::hardware::radio::V1_4::SetupDataCallResult>& dcResponse);

    virtual Return<void> setupDataCallResponse_1_4(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const android::hardware::radio::V1_4::SetupDataCallResult& dcResponse);

    virtual Return<void> setAllowedCarriersResponse_1_4(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> getAllowedCarriersResponse_1_4(const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
                                                const CarrierRestrictionsWithPriority& carriers,
                                                SimLockMultiSimPolicy multiSimPolicy);

    /* 1.5 Api */
    virtual Return<void> setSignalStrengthReportingCriteriaResponse_1_5(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> setLinkCapacityReportingCriteriaResponse_1_5(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> enableUiccApplicationsResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> areUiccApplicationsEnabledResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, bool enabled);

    virtual Return<void> canToggleUiccApplicationsEnablementResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
                                                             bool canToggle);

    virtual Return<void> setSystemSelectionChannelsResponse_1_5(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> startNetworkScanResponse_1_5(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> setupDataCallResponse_1_5(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const android::hardware::radio::V1_5::SetupDataCallResult& dcResponse);

    virtual Return<void> getDataCallListResponse_1_5(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const hidl_vec<::android::hardware::radio::V1_5::SetupDataCallResult>& dcResponse);

    virtual Return<void> setInitialAttachApnResponse_1_5(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> setDataProfileResponse_1_5(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> setRadioPowerResponse_1_5(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> setIndicationFilterResponse_1_5(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> getBarringInfoResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::radio::V1_5::CellIdentity& cellIdentity,
            const ::android::hardware::hidl_vec<::android::hardware::radio::V1_5::BarringInfo>&
                    barringInfos);

    virtual Return<void> getVoiceRegistrationStateResponse_1_5(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::radio::V1_5::RegStateResult& regResponse);

    virtual Return<void> getDataRegistrationStateResponse_1_5(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::radio::V1_5::RegStateResult& regResponse);

    virtual Return<void> getCellInfoListResponse_1_5(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::hidl_vec<::android::hardware::radio::V1_5::CellInfo>&
                    cellInfo);

    virtual Return<void> setNetworkSelectionModeManualResponse_1_5(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual Return<void> sendCdmaSmsExpectMoreResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
                                               const SendSmsResult& sms);

    virtual Return<void> supplySimDepersonalizationResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            ::android::hardware::radio::V1_5::PersoSubstate persoType, int32_t remainingRetries);

    virtual Return<void> getIccCardStatusResponse_1_5(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::radio::V1_5::CardStatus& card_status);

    /* 1.6 Api */
      Return<void> setRadioPowerResponse_1_6(
              const ::android::hardware::radio::V1_6::RadioResponseInfo& info);

      Return<void> setupDataCallResponse_1_6(
              const ::android::hardware::radio::V1_6::RadioResponseInfo& info,
              const android::hardware::radio::V1_6::SetupDataCallResult& dcResponse);

      Return<void> getDataCallListResponse_1_6(
              const ::android::hardware::radio::V1_6::RadioResponseInfo& info,
              const hidl_vec<::android::hardware::radio::V1_6::SetupDataCallResult>& dcResponse);

      Return<void> sendSmsResponse_1_6(
              const ::android::hardware::radio::V1_6::RadioResponseInfo& info,
              const SendSmsResult& sms);

      Return<void> sendSmsExpectMoreResponse_1_6(
              const ::android::hardware::radio::V1_6::RadioResponseInfo& info,
              const SendSmsResult& sms);

      Return<void> sendCdmaSmsResponse_1_6(
              const ::android::hardware::radio::V1_6::RadioResponseInfo& info,
              const SendSmsResult& sms);

      Return<void> setSimCardPowerResponse_1_6(
              const ::android::hardware::radio::V1_6::RadioResponseInfo& info);

      Return<void> sendCdmaSmsExpectMoreResponse_1_6(
              const ::android::hardware::radio::V1_6::RadioResponseInfo& info,
              const SendSmsResult& sms);

      Return<void> setNrDualConnectivityStateResponse(
              const ::android::hardware::radio::V1_6::RadioResponseInfo& info);
      Return<void> isNrDualConnectivityEnabledResponse(
              const ::android::hardware::radio::V1_6::RadioResponseInfo& info, bool isEnabled);

      Return<void> allocatePduSessionIdResponse(
              const ::android::hardware::radio::V1_6::RadioResponseInfo& info, int32_t id);

      Return<void> releasePduSessionIdResponse(
              const ::android::hardware::radio::V1_6::RadioResponseInfo& info);

      Return<void> startHandoverResponse(
              const ::android::hardware::radio::V1_6::RadioResponseInfo& info);

      Return<void> cancelHandoverResponse(
              const ::android::hardware::radio::V1_6::RadioResponseInfo& info);

      Return<void> setAllowedNetworkTypesBitmapResponse(
              const ::android::hardware::radio::V1_6::RadioResponseInfo& info);

      Return<void> getAllowedNetworkTypesBitmapResponse(
              const ::android::hardware::radio::V1_6::RadioResponseInfo& info,
              const ::android::hardware::hidl_bitfield<
                      ::android::hardware::radio::V1_4::RadioAccessFamily>
                      networkTypeBitmap);

      Return<void> setDataThrottlingResponse(
              const ::android::hardware::radio::V1_6::RadioResponseInfo& info);

      Return<void> getSystemSelectionChannelsResponse(
              const ::android::hardware::radio::V1_6::RadioResponseInfo& info,
              const hidl_vec<::android::hardware::radio::V1_5::RadioAccessSpecifier>& specifier);

      Return<void> getSignalStrengthResponse_1_6(
              const ::android::hardware::radio::V1_6::RadioResponseInfo& info,
              const ::android::hardware::radio::V1_6::SignalStrength& sig_strength);

      Return<void> getCellInfoListResponse_1_6(
              const ::android::hardware::radio::V1_6::RadioResponseInfo& info,
              const ::android::hardware::hidl_vec<::android::hardware::radio::V1_6::CellInfo>&
                      cellInfo);

      Return<void> getVoiceRegistrationStateResponse_1_6(
              const ::android::hardware::radio::V1_6::RadioResponseInfo& info,
              const ::android::hardware::radio::V1_6::RegStateResult& regResponse);

      Return<void> getDataRegistrationStateResponse_1_6(
              const ::android::hardware::radio::V1_6::RadioResponseInfo& info,
              const ::android::hardware::radio::V1_6::RegStateResult& regResponse);

      Return<void> getCurrentCallsResponse_1_6(
              const ::android::hardware::radio::V1_6::RadioResponseInfo& info,
              const ::android::hardware::hidl_vec<::android::hardware::radio::V1_6::Call>& calls);

      Return<void> getSlicingConfigResponse(
              const ::android::hardware::radio::V1_6::RadioResponseInfo& info,
              const ::android::hardware::radio::V1_6::SlicingConfig& slicingConfig);

      Return<void> getSimPhonebookRecordsResponse(
              const ::android::hardware::radio::V1_6::RadioResponseInfo& info);

      Return<void> getSimPhonebookCapacityResponse(
              const ::android::hardware::radio::V1_6::RadioResponseInfo& info,
              const ::android::hardware::radio::V1_6::PhonebookCapacity& capacity);

      Return<void> updateSimPhonebookRecordsResponse(
              const ::android::hardware::radio::V1_6::RadioResponseInfo& info,
              int32_t updatedRecordIndex);
};

struct TestRadioIndicationBase_1_6 : public ::android::hardware::radio::V1_6::IRadioIndication {
    TestRadioIndicationBase_1_6() = default;
    virtual ~TestRadioIndicationBase_1_6() = default;

    /* 1.6 Api */
    Return<void> dataCallListChanged_1_6(
            RadioIndicationType type,
            const hidl_vec<::android::hardware::radio::V1_6::SetupDataCallResult>& dcList);

    Return<void> unthrottleApn(RadioIndicationType type,
                               const ::android::hardware::hidl_string& apn);

    Return<void> currentSignalStrength_1_6(
            RadioIndicationType type,
            const ::android::hardware::radio::V1_6::SignalStrength& signalStrength);

    Return<void> networkScanResult_1_6(
            RadioIndicationType type,
            const ::android::hardware::radio::V1_6::NetworkScanResult& result);

    Return<void> cellInfoList_1_6(
            RadioIndicationType type,
            const ::android::hardware::hidl_vec<::android::hardware::radio::V1_6::CellInfo>&
                    records);

    Return<void> currentLinkCapacityEstimate_1_6(
            RadioIndicationType type,
            const ::android::hardware::radio::V1_6::LinkCapacityEstimate& lce);

    Return<void> currentPhysicalChannelConfigs_1_6(
            RadioIndicationType type,
            const ::android::hardware::hidl_vec<
                    ::android::hardware::radio::V1_6::PhysicalChannelConfig>& configs);

    Return<void> simPhonebookChanged(RadioIndicationType type);

    Return<void> simPhonebookRecordsReceived(
            RadioIndicationType type,
            ::android::hardware::radio::V1_6::PbReceivedStatus status,
            const ::android::hardware::hidl_vec<::android::hardware::radio::V1_6::PhonebookRecordInfo>&
                    records);

    /* 1.5 Api */
    virtual Return<void> uiccApplicationsEnablementChanged(RadioIndicationType type, bool enabled);

    virtual Return<void> registrationFailed(
            RadioIndicationType type,
            const ::android::hardware::radio::V1_5::CellIdentity& cellIdentity,
            const ::android::hardware::hidl_string& chosenPlmn,
            ::android::hardware::hidl_bitfield<::android::hardware::radio::V1_5::Domain> domain,
            int32_t causeCode, int32_t additionalCauseCode);

    virtual Return<void> barringInfoChanged(
            RadioIndicationType /*type*/,
            const ::android::hardware::radio::V1_5::CellIdentity& /*cellIdentity*/,
            const ::android::hardware::hidl_vec<::android::hardware::radio::V1_5::BarringInfo>&
            /*barringInfos*/);

    virtual Return<void> networkScanResult_1_5(
            RadioIndicationType type,
            const ::android::hardware::radio::V1_5::NetworkScanResult& result);

    virtual Return<void> cellInfoList_1_5(
            RadioIndicationType type,
            const ::android::hardware::hidl_vec<::android::hardware::radio::V1_5::CellInfo>&
                    records);

    virtual Return<void> dataCallListChanged_1_5(
            RadioIndicationType type,
            const hidl_vec<::android::hardware::radio::V1_5::SetupDataCallResult>& dcList);

    /* 1.4 Api */
    virtual Return<void> currentEmergencyNumberList(
            RadioIndicationType type,
            const ::android::hardware::hidl_vec<EmergencyNumber>& emergencyNumberList);

    virtual Return<void> cellInfoList_1_4(
            RadioIndicationType type,
            const ::android::hardware::hidl_vec<::android::hardware::radio::V1_4::CellInfo>&
                    records);

    virtual Return<void> currentSignalStrength_1_4(
            RadioIndicationType type,
            const ::android::hardware::radio::V1_4::SignalStrength& signalStrength);

    virtual Return<void> networkScanResult_1_4(
            RadioIndicationType type,
            const ::android::hardware::radio::V1_4::NetworkScanResult& result);

    virtual Return<void> currentPhysicalChannelConfigs_1_4(
            RadioIndicationType type,
            const ::android::hardware::hidl_vec<
                    ::android::hardware::radio::V1_4::PhysicalChannelConfig>& configs);

    virtual Return<void> dataCallListChanged_1_4(
            RadioIndicationType type,
            const ::android::hardware::hidl_vec<
                    android::hardware::radio::V1_4::SetupDataCallResult>& dcList);

    /* 1.2 Api */
    virtual Return<void> networkScanResult_1_2(
            RadioIndicationType type,
            const ::android::hardware::radio::V1_2::NetworkScanResult& result);

    virtual Return<void> cellInfoList_1_2(
            RadioIndicationType type,
            const ::android::hardware::hidl_vec<::android::hardware::radio::V1_2::CellInfo>&
                    records);

    virtual Return<void> currentLinkCapacityEstimate(
            RadioIndicationType type,
            const ::android::hardware::radio::V1_2::LinkCapacityEstimate& lce);

    virtual Return<void> currentPhysicalChannelConfigs(
            RadioIndicationType type,
            const ::android::hardware::hidl_vec<
                    ::android::hardware::radio::V1_2::PhysicalChannelConfig>& configs);

    virtual Return<void> currentSignalStrength_1_2(
            RadioIndicationType type,
            const ::android::hardware::radio::V1_2::SignalStrength& signalStrength);

    /* 1.1 Api */
    virtual Return<void> carrierInfoForImsiEncryption(RadioIndicationType info);

    virtual Return<void> networkScanResult(
            RadioIndicationType type,
            const ::android::hardware::radio::V1_1::NetworkScanResult& result);

    virtual Return<void> keepaliveStatus(RadioIndicationType type, const KeepaliveStatus& status);

    /* 1.0 Api */
    virtual Return<void> radioStateChanged(RadioIndicationType type, RadioState radioState);

    virtual Return<void> callStateChanged(RadioIndicationType type);

    virtual Return<void> networkStateChanged(RadioIndicationType type);

    virtual Return<void> newSms(RadioIndicationType type,
                        const ::android::hardware::hidl_vec<uint8_t>& pdu);

    virtual Return<void> newSmsStatusReport(RadioIndicationType type,
                                    const ::android::hardware::hidl_vec<uint8_t>& pdu);

    virtual Return<void> newSmsOnSim(RadioIndicationType type, int32_t recordNumber);

    virtual Return<void> onUssd(RadioIndicationType type, UssdModeType modeType,
                        const ::android::hardware::hidl_string& msg);

    virtual Return<void> nitzTimeReceived(RadioIndicationType type,
                                  const ::android::hardware::hidl_string& nitzTime,
                                  uint64_t receivedTime);

    virtual Return<void> currentSignalStrength(
            RadioIndicationType type,
            const ::android::hardware::radio::V1_0::SignalStrength& signalStrength);

    virtual Return<void> dataCallListChanged(
            RadioIndicationType type,
            const ::android::hardware::hidl_vec<
                    android::hardware::radio::V1_0::SetupDataCallResult>& dcList);

    virtual Return<void> suppSvcNotify(RadioIndicationType type, const SuppSvcNotification& suppSvc);

    virtual Return<void> stkSessionEnd(RadioIndicationType type);

    virtual Return<void> stkProactiveCommand(RadioIndicationType type,
                                     const ::android::hardware::hidl_string& cmd);

    virtual Return<void> stkEventNotify(RadioIndicationType type,
                                const ::android::hardware::hidl_string& cmd);

    virtual Return<void> stkCallSetup(RadioIndicationType type, int64_t timeout);

    virtual Return<void> simSmsStorageFull(RadioIndicationType type);

    virtual Return<void> simRefresh(RadioIndicationType type, const SimRefreshResult& refreshResult);

    virtual Return<void> callRing(RadioIndicationType type, bool isGsm, const CdmaSignalInfoRecord& record);

    virtual Return<void> simStatusChanged(RadioIndicationType type);

    virtual Return<void> cdmaNewSms(RadioIndicationType type, const CdmaSmsMessage& msg);

    virtual Return<void> newBroadcastSms(RadioIndicationType type,
                                 const ::android::hardware::hidl_vec<uint8_t>& data);

    virtual Return<void> cdmaRuimSmsStorageFull(RadioIndicationType type);

    virtual Return<void> restrictedStateChanged(RadioIndicationType type, PhoneRestrictedState state);

    virtual Return<void> enterEmergencyCallbackMode(RadioIndicationType type);

    virtual Return<void> cdmaCallWaiting(RadioIndicationType type,
                                 const CdmaCallWaiting& callWaitingRecord);

    virtual Return<void> cdmaOtaProvisionStatus(RadioIndicationType type, CdmaOtaProvisionStatus status);

    virtual Return<void> cdmaInfoRec(RadioIndicationType type, const CdmaInformationRecords& records);

    virtual Return<void> indicateRingbackTone(RadioIndicationType type, bool start);

    virtual Return<void> resendIncallMute(RadioIndicationType type);

    virtual Return<void> cdmaSubscriptionSourceChanged(RadioIndicationType type,
                                               CdmaSubscriptionSource cdmaSource);

    virtual Return<void> cdmaPrlChanged(RadioIndicationType type, int32_t version);

    virtual Return<void> exitEmergencyCallbackMode(RadioIndicationType type);

    virtual Return<void> rilConnected(RadioIndicationType type);

    virtual Return<void> voiceRadioTechChanged(RadioIndicationType type,
                                       ::android::hardware::radio::V1_0::RadioTechnology rat);

    virtual Return<void> cellInfoList(
            RadioIndicationType type,
            const ::android::hardware::hidl_vec<::android::hardware::radio::V1_0::CellInfo>&
                    records);

    virtual Return<void> imsNetworkStateChanged(RadioIndicationType type);

    virtual Return<void> subscriptionStatusChanged(RadioIndicationType type, bool activate);

    virtual Return<void> srvccStateNotify(RadioIndicationType type, SrvccState state);

    virtual Return<void> hardwareConfigChanged(
            RadioIndicationType type, const ::android::hardware::hidl_vec<HardwareConfig>& configs);

    virtual Return<void> radioCapabilityIndication(
            RadioIndicationType type, const android::hardware::radio::V1_0::RadioCapability& rc);

    virtual Return<void> onSupplementaryServiceIndication(RadioIndicationType type,
                                                  const StkCcUnsolSsResult& ss);

    virtual Return<void> stkCallControlAlphaNotify(RadioIndicationType type,
                                           const ::android::hardware::hidl_string& alpha);

    virtual Return<void> lceData(RadioIndicationType type, const LceDataInfo& lce);

    virtual Return<void> pcoData(RadioIndicationType type, const PcoDataInfo& pco);

    virtual Return<void> modemReset(RadioIndicationType type,
                            const ::android::hardware::hidl_string& reason);
};

#endif /* __TEST_RADIO_HIDL_UTILS_1_6_H__ */
