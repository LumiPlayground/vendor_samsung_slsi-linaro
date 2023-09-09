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
 * test_radio_response_1_2.h
 *
 *  Created on: 2021. 8. 5.
 */
#ifndef __TEST_RADIO_HIDL_UTILS_1_2__
#define __TEST_RADIO_HIDL_UTILS_1_2__

#include <android/hardware/radio/1.2/IRadio.h>
#include <android/hardware/radio/1.2/IRadioResponse.h>
#include <android/hardware/radio/1.2/IRadioIndication.h>
#include <telephony/ril.h>

using namespace ::android::hardware::radio::V1_2;
using namespace ::android::hardware::radio::V1_1;
using namespace ::android::hardware::radio::V1_0;

using ::android::sp;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;

struct TestRadioResponseBase_1_2 : public ::android::hardware::radio::V1_2::IRadioResponse {
    TestRadioResponseBase_1_2() = default;
    virtual ~TestRadioResponseBase_1_2() = default;

    virtual Return<void> getIccCardStatusResponse(
            const RadioResponseInfo& info,
            const ::android::hardware::radio::V1_0::CardStatus& cardStatus);

    virtual Return<void> supplyIccPinForAppResponse(const RadioResponseInfo& info,
                                            int32_t remainingRetries);

    virtual Return<void> supplyIccPukForAppResponse(const RadioResponseInfo& info,
                                            int32_t remainingRetries);

    virtual Return<void> supplyIccPin2ForAppResponse(const RadioResponseInfo& info,
                                             int32_t remainingRetries);

    virtual Return<void> supplyIccPuk2ForAppResponse(const RadioResponseInfo& info,
                                             int32_t remainingRetries);

    virtual Return<void> changeIccPinForAppResponse(const RadioResponseInfo& info,
                                            int32_t remainingRetries);

    virtual Return<void> changeIccPin2ForAppResponse(const RadioResponseInfo& info,
                                             int32_t remainingRetries);

    virtual Return<void> supplyNetworkDepersonalizationResponse(const RadioResponseInfo& info,
                                                        int32_t remainingRetries);

    virtual Return<void> getCurrentCallsResponse(
            const RadioResponseInfo& info,
            const ::android::hardware::hidl_vec<::android::hardware::radio::V1_0::Call>& calls);

    virtual Return<void> dialResponse(const RadioResponseInfo& info);

    virtual Return<void> getIMSIForAppResponse(const RadioResponseInfo& info,
                                       const ::android::hardware::hidl_string& imsi);

    virtual Return<void> hangupConnectionResponse(const RadioResponseInfo& info);

    virtual Return<void> hangupWaitingOrBackgroundResponse(const RadioResponseInfo& info);

    virtual Return<void> hangupForegroundResumeBackgroundResponse(const RadioResponseInfo& info);

    virtual Return<void> switchWaitingOrHoldingAndActiveResponse(const RadioResponseInfo& info);

    virtual Return<void> conferenceResponse(const RadioResponseInfo& info);

    virtual Return<void> rejectCallResponse(const RadioResponseInfo& info);

    virtual Return<void> getLastCallFailCauseResponse(const RadioResponseInfo& info,
                                              const LastCallFailCauseInfo& failCauseInfo);

    virtual Return<void> getSignalStrengthResponse(
            const RadioResponseInfo& info,
            const ::android::hardware::radio::V1_0::SignalStrength& sigStrength);

    virtual Return<void> getVoiceRegistrationStateResponse(
            const RadioResponseInfo& info,
            const ::android::hardware::radio::V1_0::VoiceRegStateResult& voiceRegResponse);

    virtual Return<void> getDataRegistrationStateResponse(
            const RadioResponseInfo& info,
            const ::android::hardware::radio::V1_0::DataRegStateResult& dataRegResponse);

    virtual Return<void> getOperatorResponse(const RadioResponseInfo& info,
                                     const ::android::hardware::hidl_string& longName,
                                     const ::android::hardware::hidl_string& shortName,
                                     const ::android::hardware::hidl_string& numeric);

    virtual Return<void> setRadioPowerResponse(const RadioResponseInfo& info);

    virtual Return<void> sendDtmfResponse(const RadioResponseInfo& info);

    virtual Return<void> sendSmsResponse(const RadioResponseInfo& info, const SendSmsResult& sms);

    virtual Return<void> sendSMSExpectMoreResponse(const RadioResponseInfo& info, const SendSmsResult& sms);

    virtual Return<void> setupDataCallResponse(
            const RadioResponseInfo& info,
            const android::hardware::radio::V1_0::SetupDataCallResult& dcResponse);

    virtual Return<void> iccIOForAppResponse(const RadioResponseInfo& info, const IccIoResult& iccIo);

    virtual Return<void> sendUssdResponse(const RadioResponseInfo& info);

    virtual Return<void> cancelPendingUssdResponse(const RadioResponseInfo& info);

    virtual Return<void> getClirResponse(const RadioResponseInfo& info, int32_t n, int32_t m);

    virtual Return<void> setClirResponse(const RadioResponseInfo& info);

    virtual Return<void> getCallForwardStatusResponse(
            const RadioResponseInfo& info,
            const ::android::hardware::hidl_vec<CallForwardInfo>& call_forwardInfos);

    virtual Return<void> setCallForwardResponse(const RadioResponseInfo& info);

    virtual Return<void> getCallWaitingResponse(const RadioResponseInfo& info, bool enable,
                                        int32_t serviceClass);

    virtual Return<void> setCallWaitingResponse(const RadioResponseInfo& info);

    virtual Return<void> acknowledgeLastIncomingGsmSmsResponse(const RadioResponseInfo& info);

    virtual Return<void> acceptCallResponse(const RadioResponseInfo& info);

    virtual Return<void> deactivateDataCallResponse(const RadioResponseInfo& info);

    virtual Return<void> getFacilityLockForAppResponse(const RadioResponseInfo& info, int32_t response);

    virtual Return<void> setFacilityLockForAppResponse(const RadioResponseInfo& info, int32_t retry);

    virtual Return<void> setBarringPasswordResponse(const RadioResponseInfo& info);

    virtual Return<void> getNetworkSelectionModeResponse(const RadioResponseInfo& info, bool manual);

    virtual Return<void> setNetworkSelectionModeAutomaticResponse(const RadioResponseInfo& info);

    virtual Return<void> setNetworkSelectionModeManualResponse(const RadioResponseInfo& info);

    virtual Return<void> getAvailableNetworksResponse(
            const RadioResponseInfo& info,
            const ::android::hardware::hidl_vec<OperatorInfo>& networkInfos);

    virtual Return<void> startDtmfResponse(const RadioResponseInfo& info);

    virtual Return<void> stopDtmfResponse(const RadioResponseInfo& info);

    virtual Return<void> getBasebandVersionResponse(const RadioResponseInfo& info,
                                            const ::android::hardware::hidl_string& version);

    virtual Return<void> separateConnectionResponse(const RadioResponseInfo& info);

    virtual Return<void> setMuteResponse(const RadioResponseInfo& info);

    virtual Return<void> getMuteResponse(const RadioResponseInfo& info, bool enable);

    virtual Return<void> getClipResponse(const RadioResponseInfo& info, ClipStatus status);

    virtual Return<void> getDataCallListResponse(
            const RadioResponseInfo& info,
            const ::android::hardware::hidl_vec<
                    android::hardware::radio::V1_0::SetupDataCallResult>& dcResponse);

    virtual Return<void> sendOemRilRequestRawResponse(const RadioResponseInfo& info,
                                              const ::android::hardware::hidl_vec<uint8_t>& data);

    virtual Return<void> sendOemRilRequestStringsResponse(
            const RadioResponseInfo& info,
            const ::android::hardware::hidl_vec<::android::hardware::hidl_string>& data);

    virtual Return<void> setSuppServiceNotificationsResponse(const RadioResponseInfo& info);

    virtual Return<void> writeSmsToSimResponse(const RadioResponseInfo& info, int32_t index);

    virtual Return<void> deleteSmsOnSimResponse(const RadioResponseInfo& info);

    virtual Return<void> setBandModeResponse(const RadioResponseInfo& info);

    virtual Return<void> getAvailableBandModesResponse(
            const RadioResponseInfo& info,
            const ::android::hardware::hidl_vec<RadioBandMode>& bandModes);

    virtual Return<void> sendEnvelopeResponse(const RadioResponseInfo& info,
                                      const ::android::hardware::hidl_string& commandResponse);

    virtual Return<void> sendTerminalResponseToSimResponse(const RadioResponseInfo& info);

    virtual Return<void> handleStkCallSetupRequestFromSimResponse(const RadioResponseInfo& info);

    virtual Return<void> explicitCallTransferResponse(const RadioResponseInfo& info);

    virtual Return<void> setPreferredNetworkTypeResponse(const RadioResponseInfo& info);

    virtual Return<void> getPreferredNetworkTypeResponse(const RadioResponseInfo& info,
                                                 PreferredNetworkType nwType);

    virtual Return<void> getNeighboringCidsResponse(
            const RadioResponseInfo& info,
            const ::android::hardware::hidl_vec<NeighboringCell>& cells);

    virtual Return<void> setLocationUpdatesResponse(const RadioResponseInfo& info);

    virtual Return<void> setCdmaSubscriptionSourceResponse(const RadioResponseInfo& info);

    virtual Return<void> setCdmaRoamingPreferenceResponse(const RadioResponseInfo& info);

    virtual Return<void> getCdmaRoamingPreferenceResponse(const RadioResponseInfo& info,
                                                  CdmaRoamingType type);

    virtual Return<void> setTTYModeResponse(const RadioResponseInfo& info);

    virtual Return<void> getTTYModeResponse(const RadioResponseInfo& info, TtyMode mode);

    virtual Return<void> setPreferredVoicePrivacyResponse(const RadioResponseInfo& info);

    virtual Return<void> getPreferredVoicePrivacyResponse(const RadioResponseInfo& info, bool enable);

    virtual Return<void> sendCDMAFeatureCodeResponse(const RadioResponseInfo& info);

    virtual Return<void> sendBurstDtmfResponse(const RadioResponseInfo& info);

    virtual Return<void> sendCdmaSmsResponse(const RadioResponseInfo& info, const SendSmsResult& sms);

    virtual Return<void> acknowledgeLastIncomingCdmaSmsResponse(const RadioResponseInfo& info);

    virtual Return<void> getGsmBroadcastConfigResponse(
            const RadioResponseInfo& info,
            const ::android::hardware::hidl_vec<GsmBroadcastSmsConfigInfo>& configs);

    virtual Return<void> setGsmBroadcastConfigResponse(const RadioResponseInfo& info);

    virtual Return<void> setGsmBroadcastActivationResponse(const RadioResponseInfo& info);

    virtual Return<void> getCdmaBroadcastConfigResponse(
            const RadioResponseInfo& info,
            const ::android::hardware::hidl_vec<CdmaBroadcastSmsConfigInfo>& configs);

    virtual Return<void> setCdmaBroadcastConfigResponse(const RadioResponseInfo& info);

    virtual Return<void> setCdmaBroadcastActivationResponse(const RadioResponseInfo& info);

    virtual Return<void> getCDMASubscriptionResponse(const RadioResponseInfo& info,
                                             const ::android::hardware::hidl_string& mdn,
                                             const ::android::hardware::hidl_string& hSid,
                                             const ::android::hardware::hidl_string& hNid,
                                             const ::android::hardware::hidl_string& min,
                                             const ::android::hardware::hidl_string& prl);

    virtual Return<void> writeSmsToRuimResponse(const RadioResponseInfo& info, uint32_t index);

    virtual Return<void> deleteSmsOnRuimResponse(const RadioResponseInfo& info);

    virtual Return<void> getDeviceIdentityResponse(const RadioResponseInfo& info,
                                           const ::android::hardware::hidl_string& imei,
                                           const ::android::hardware::hidl_string& imeisv,
                                           const ::android::hardware::hidl_string& esn,
                                           const ::android::hardware::hidl_string& meid);

    virtual Return<void> exitEmergencyCallbackModeResponse(const RadioResponseInfo& info);

    virtual Return<void> getSmscAddressResponse(const RadioResponseInfo& info,
                                        const ::android::hardware::hidl_string& smsc);

    virtual Return<void> setSmscAddressResponse(const RadioResponseInfo& info);

    virtual Return<void> reportSmsMemoryStatusResponse(const RadioResponseInfo& info);

    virtual Return<void> reportStkServiceIsRunningResponse(const RadioResponseInfo& info);

    virtual Return<void> getCdmaSubscriptionSourceResponse(const RadioResponseInfo& info,
                                                   CdmaSubscriptionSource source);

    virtual Return<void> requestIsimAuthenticationResponse(
            const RadioResponseInfo& info, const ::android::hardware::hidl_string& response);

    virtual Return<void> acknowledgeIncomingGsmSmsWithPduResponse(const RadioResponseInfo& info);

    virtual Return<void> sendEnvelopeWithStatusResponse(const RadioResponseInfo& info,
                                                const IccIoResult& iccIo);

    virtual Return<void> getVoiceRadioTechnologyResponse(
            const RadioResponseInfo& info, ::android::hardware::radio::V1_0::RadioTechnology rat);

    virtual Return<void> getCellInfoListResponse(
            const RadioResponseInfo& info,
            const ::android::hardware::hidl_vec<::android::hardware::radio::V1_0::CellInfo>&
                    cellInfo);

    virtual Return<void> setCellInfoListRateResponse(const RadioResponseInfo& info);

    virtual Return<void> setInitialAttachApnResponse(const RadioResponseInfo& info);

    virtual Return<void> getImsRegistrationStateResponse(const RadioResponseInfo& info, bool isRegistered,
                                                 RadioTechnologyFamily ratFamily);

    virtual Return<void> sendImsSmsResponse(const RadioResponseInfo& info, const SendSmsResult& sms);

    virtual Return<void> iccTransmitApduBasicChannelResponse(const RadioResponseInfo& info,
                                                     const IccIoResult& result);

    virtual Return<void> iccOpenLogicalChannelResponse(
            const RadioResponseInfo& info, int32_t channelId,
            const ::android::hardware::hidl_vec<int8_t>& selectResponse);

    virtual Return<void> iccCloseLogicalChannelResponse(const RadioResponseInfo& info);

    virtual Return<void> iccTransmitApduLogicalChannelResponse(const RadioResponseInfo& info,
                                                       const IccIoResult& result);

    virtual Return<void> nvReadItemResponse(const RadioResponseInfo& info,
                                    const ::android::hardware::hidl_string& result);

    virtual Return<void> nvWriteItemResponse(const RadioResponseInfo& info);

    virtual Return<void> nvWriteCdmaPrlResponse(const RadioResponseInfo& info);

    virtual Return<void> nvResetConfigResponse(const RadioResponseInfo& info);

    virtual Return<void> setUiccSubscriptionResponse(const RadioResponseInfo& info);

    virtual Return<void> setDataAllowedResponse(const RadioResponseInfo& info);

    virtual Return<void> getHardwareConfigResponse(
            const RadioResponseInfo& info,
            const ::android::hardware::hidl_vec<HardwareConfig>& config);

    virtual Return<void> requestIccSimAuthenticationResponse(const RadioResponseInfo& info,
                                                     const IccIoResult& result);

    virtual Return<void> setDataProfileResponse(const RadioResponseInfo& info);

    virtual Return<void> requestShutdownResponse(const RadioResponseInfo& info);

    virtual Return<void> getRadioCapabilityResponse(
            const RadioResponseInfo& info,
            const android::hardware::radio::V1_0::RadioCapability& rc);

    virtual Return<void> setRadioCapabilityResponse(
            const RadioResponseInfo& info,
            const android::hardware::radio::V1_0::RadioCapability& rc);

    virtual Return<void> startLceServiceResponse(const RadioResponseInfo& info,
                                         const LceStatusInfo& statusInfo);

    virtual Return<void> stopLceServiceResponse(const RadioResponseInfo& info,
                                        const LceStatusInfo& statusInfo);

    virtual Return<void> pullLceDataResponse(const RadioResponseInfo& info, const LceDataInfo& lceInfo);

    virtual Return<void> getModemActivityInfoResponse(const RadioResponseInfo& info,
                                              const ActivityStatsInfo& activityInfo);

    virtual Return<void> setAllowedCarriersResponse(const RadioResponseInfo& info, int32_t numAllowed);

    virtual Return<void> getAllowedCarriersResponse(const RadioResponseInfo& info, bool allAllowed,
                                            const CarrierRestrictions& carriers);

    virtual Return<void> sendDeviceStateResponse(const RadioResponseInfo& info);

    virtual Return<void> setIndicationFilterResponse(const RadioResponseInfo& info);

    virtual Return<void> setSimCardPowerResponse(const RadioResponseInfo& info);

    virtual Return<void> acknowledgeRequest(int32_t serial);

    /* 1.1 Api */
    virtual Return<void> setCarrierInfoForImsiEncryptionResponse(const RadioResponseInfo& info);

    virtual Return<void> setSimCardPowerResponse_1_1(const RadioResponseInfo& info);

    virtual Return<void> startNetworkScanResponse(const RadioResponseInfo& info);

    virtual Return<void> stopNetworkScanResponse(const RadioResponseInfo& info);

    virtual Return<void> startKeepaliveResponse(const RadioResponseInfo& info,
                                        const KeepaliveStatus& status);

    virtual Return<void> stopKeepaliveResponse(const RadioResponseInfo& info);

    /* 1.2 Api */
    virtual Return<void> setSignalStrengthReportingCriteriaResponse(const RadioResponseInfo& info);

    virtual Return<void> setLinkCapacityReportingCriteriaResponse(const RadioResponseInfo& info);

    virtual Return<void> getIccCardStatusResponse_1_2(
            const RadioResponseInfo& info,
            const ::android::hardware::radio::V1_2::CardStatus& card_status);

    virtual Return<void> getCurrentCallsResponse_1_2(
            const RadioResponseInfo& info,
            const ::android::hardware::hidl_vec<::android::hardware::radio::V1_2::Call>& calls);

    virtual Return<void> getSignalStrengthResponse_1_2(
            const RadioResponseInfo& info,
            const ::android::hardware::radio::V1_2::SignalStrength& sig_strength);

    virtual Return<void> getCellInfoListResponse_1_2(
            const RadioResponseInfo& info,
            const ::android::hardware::hidl_vec<::android::hardware::radio::V1_2::CellInfo>&
                    cellInfo);

    virtual Return<void> getVoiceRegistrationStateResponse_1_2(
            const RadioResponseInfo& info,
            const ::android::hardware::radio::V1_2::VoiceRegStateResult& voiceRegResponse);

    virtual Return<void> getDataRegistrationStateResponse_1_2(
            const RadioResponseInfo& info,
            const ::android::hardware::radio::V1_2::DataRegStateResult& dataRegResponse);
};

struct TestRadioIndicationBase_1_2 : public ::android::hardware::radio::V1_2::IRadioIndication {
    TestRadioIndicationBase_1_2() = default;
    virtual ~TestRadioIndicationBase_1_2() = default;

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

#endif /* __TEST_RADIO_HIDL_UTILS_1_2__ */
