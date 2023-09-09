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

#include <radiomodemcallback.h>
#include <radiodatacallback.h>
#include <radiomessagingcallback.h>
#include <radionetworkcallback.h>
#include <radiosimcallback.h>
#include <radioconfigcallback.h>
#include <radiovoicecallback.h>

namespace android::hardware::radio::impl {

struct RadioModemCallbackAdapter : public RadioModemCallback {
    virtual ~RadioModemCallbackAdapter() = default;
    virtual void setResponseFunctions(
            const std::shared_ptr<RadioModemResponseCallback>& /* responseCb */,
            const std::shared_ptr<RadioModemIndicationCallback>& /* indicationCb */) override {}
    virtual void enableModem(int32_t /*serial*/, bool /* on */) override {}
    virtual void getBasebandVersion(int32_t /*serial*/) override {}
    virtual void getDeviceIdentity(int32_t /*serial*/) override {}
    virtual void getHardwareConfig(int32_t /*serial*/) override {}
    virtual void getModemActivityInfo(int32_t /*serial*/) override {}
    virtual void getModemStackStatus(int32_t /*serial*/) override {}
    virtual void getRadioCapability(int32_t /*serial*/) override {}
    virtual void nvReadItem(
        int32_t /*serial*/, ::aidl::android::hardware::radio::modem::NvItem /* itemId */) override {}
    virtual void nvResetConfig(
        int32_t /*serial*/, ::aidl::android::hardware::radio::modem::ResetNvType /* type */) override {}
    virtual void nvWriteCdmaPrl(int32_t /*serial*/, const std::vector<uint8_t>& /* prl */) override {}
    virtual void nvWriteItem(
        int32_t /*serial*/, const ::aidl::android::hardware::radio::modem::NvWriteItem& /* i */) override {}
    virtual void requestShutdown(int32_t /*serial*/) override {}
    virtual void responseAcknowledgement() override {}
    virtual void sendDeviceState(
        int32_t /*serial*/, ::aidl::android::hardware::radio::modem::DeviceStateType /* stateType */,
        bool /* state */) override {}
    virtual void setRadioCapability(
        int32_t /*serial*/, const ::aidl::android::hardware::radio::modem::RadioCapability& /* rc */) override {}
    virtual void setRadioPower(int32_t /*serial*/, bool /* powerOn */, bool /* forEmergencyCall */,
                               bool /* preferredForEmergencyCall */) override {}
};

struct RadioModemResponseCallbackAdapter : public RadioModemResponseCallback {
    virtual ~RadioModemResponseCallbackAdapter() = default;
    virtual void setResponseFunction(
            const std::shared_ptr<::aidl::android::hardware::radio::modem::IRadioModemResponse>&
                    /* radioModemResponse */) {}
    virtual void acknowledgeRequest(int32_t /*serial*/) override {}
    virtual void enableModemResponse(
        const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/) override {}
    virtual void getBasebandVersionResponse(
        const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/,
        const std::string& /* version */) override {}
    virtual void getDeviceIdentityResponse(
        const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/,
        const std::string& /* imei */, const std::string& /* imeisv */,
        const std::string& /* esn */, const std::string& /* meid */) override {}
    virtual void getHardwareConfigResponse(
        const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/,
        const std::vector<::aidl::android::hardware::radio::modem::HardwareConfig>& /* config */) override {}
    virtual void getModemActivityInfoResponse(
        const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/,
        const ::aidl::android::hardware::radio::modem::ActivityStatsInfo& /* activityInfo */) override {}
    virtual void getModemStackStatusResponse(
        const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/,
        bool /* isEnabled */) {}
    virtual void getRadioCapabilityResponse(
        const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/,
        const ::aidl::android::hardware::radio::modem::RadioCapability& /* rc */) override {}
    virtual void nvReadItemResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/,
                                    const std::string& /* result */) override {}
    virtual void nvResetConfigResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/) override {}
    virtual void nvWriteCdmaPrlResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/) override {}
    virtual void nvWriteItemResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/) override {}
    virtual void requestShutdownResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/) override {}
    virtual void sendDeviceStateResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/) override {}
    virtual void setRadioCapabilityResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/,
                                            const ::aidl::android::hardware::radio::modem::RadioCapability& /* rc */) override {}
    virtual void setRadioPowerResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/) override {}
};

struct RadioModemIndicationCallbackAdapter : public RadioModemIndicationCallback {
    virtual ~RadioModemIndicationCallbackAdapter() = default;
    virtual void setResponseFunction(
            const std::shared_ptr<::aidl::android::hardware::radio::modem::IRadioModemIndication>&
                    /* radioModemIndication */) override {}
    virtual void hardwareConfigChanged(
        ::aidl::android::hardware::radio::RadioIndicationType /* in_type */,
        const std::vector<::aidl::android::hardware::radio::modem::HardwareConfig>& /* in_configs */) override {}
    virtual void modemReset(
        ::aidl::android::hardware::radio::RadioIndicationType /* in_type */,
        const std::string& /* in_reason */) override {}
    virtual void radioCapabilityIndication(::aidl::android::hardware::radio::RadioIndicationType /* in_type */,
                                           const ::aidl::android::hardware::radio::modem::RadioCapability& /* in_rc */) override {}
    virtual void radioStateChanged(
        ::aidl::android::hardware::radio::RadioIndicationType /* in_type */,
        ::aidl::android::hardware::radio::modem::RadioState /* in_radioState */) override {}
    virtual void rilConnected(::aidl::android::hardware::radio::RadioIndicationType /* in_type */) override {}
};

struct RadioDataCallbackAdapter : public RadioDataCallback {
    virtual ~RadioDataCallbackAdapter() = default;
    virtual void setResponseFunctions(
            const std::shared_ptr<RadioDataResponseCallback>& /* responseCb */,
            const std::shared_ptr<RadioDataIndicationCallback>& /* indicationCb */) override {}
    virtual void allocatePduSessionId(int32_t /*serial*/) override {}
    virtual void cancelHandover(int32_t /*serial*/, int32_t /*callId*/) override {}
    virtual void deactivateDataCall(
            int32_t /*serial*/, int32_t /*cid*/,
            ::aidl::android::hardware::radio::data::DataRequestReason /*reason*/) override {}
    virtual void getDataCallList(int32_t /*serial*/) override {}
    virtual void getSlicingConfig(int32_t /*serial*/) override {}
    virtual void releasePduSessionId(int32_t /*serial*/, int32_t /*id*/) override {}
    virtual void responseAcknowledgement() override {}
    virtual void setDataAllowed(int32_t /*serial*/, bool /*allow*/) override {}
    virtual void setDataProfile(int32_t /*serial*/,
            const std::vector<::aidl::android::hardware::radio::data::DataProfileInfo>& /*profiles*/)
            override {}
    virtual void setDataThrottling(int32_t /*serial*/,
            ::aidl::android::hardware::radio::data::DataThrottlingAction /*dataThrottlingAction*/,
            int64_t /*completionDurationMillis*/) override {}
    virtual void setInitialAttachApn(
            int32_t /*serial*/,
            const std::optional<::aidl::android::hardware::radio::data::DataProfileInfo>& /*dpInfo*/) override {}
    virtual void setupDataCall(
            int32_t /*serial*/, ::aidl::android::hardware::radio::AccessNetwork /*accessNetwork*/,
            const ::aidl::android::hardware::radio::data::DataProfileInfo& /*dataProfileInfo*/,
            bool /*roamingAllowed*/, ::aidl::android::hardware::radio::data::DataRequestReason /*reason*/,
            const std::vector<::aidl::android::hardware::radio::data::LinkAddress>& /*addresses*/,
            const std::vector<std::string>& /*dnses*/, int32_t /*pduSessionId*/,
            const std::optional<::aidl::android::hardware::radio::data::SliceInfo>& /*sliceInfo*/,
            bool /*matchAllRuleAllowed*/) override{}
    virtual void startHandover(int32_t /*serial*/, int32_t /*callId*/) override{}
    virtual void startKeepalive(int32_t /*serial*/,
            const ::aidl::android::hardware::radio::data::KeepaliveRequest& /*keepalive*/) override{}
    virtual void stopKeepalive(int32_t /*serial*/, int32_t /*sessionHandle*/) override{}
};

struct RadioDataResponseCallbackAdapter : public RadioDataResponseCallback {
    virtual ~RadioDataResponseCallbackAdapter() = default;
    virtual void setResponseFunction(
            const std::shared_ptr<::aidl::android::hardware::radio::data::IRadioDataResponse>&
                    /* radioDataResponse */) {}

    virtual void acknowledgeRequest(int32_t /*serial*/) override {}

    virtual void allocatePduSessionIdResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/,	int /*id*/) override {}
    virtual void cancelHandoverResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/) override {}
    virtual void deactivateDataCallResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/) override {}
    virtual void getDataCallListResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/,
                const std::vector<::aidl::android::hardware::radio::data::SetupDataCallResult> /*dcResponse*/) override {}
    virtual void getSlicingConfigResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/,
                const ::aidl::android::hardware::radio::data::SlicingConfig /*slicingConfig*/) override {}
    virtual void releasePduSessionIdResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/) override {}
    virtual void setDataAllowedResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/) override {}
    virtual void setDataThrottlingResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/) override {}
    virtual void setInitialAttachApnResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/) override {}
    virtual void setDataProfileResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/) override {}
    virtual void setupDataCallResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/,
                const ::aidl::android::hardware::radio::data::SetupDataCallResult /*dcResponse*/) override {}
    virtual void startHandoverResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/) override {}
    virtual void startKeepaliveResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/,
                const::aidl::android::hardware::radio::data::KeepaliveStatus /*status*/) override {}
    virtual void stopKeepaliveResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/) override {}


};

struct RadioDataIndicationCallbackAdapter : public RadioDataIndicationCallback {
    virtual ~RadioDataIndicationCallbackAdapter() = default;
    virtual void setResponseFunction(
            const std::shared_ptr<::aidl::android::hardware::radio::data::IRadioDataIndication>&
                    /* radioDataIndication */) override {}

    virtual void dataCallListChanged(
        const ::aidl::android::hardware::radio::RadioIndicationType /* type*/,
        const std::vector<
            ::aidl::android::hardware::radio::data::SetupDataCallResult>
            /*dcList*/) override {}
    virtual void keepaliveStatus(
        const ::aidl::android::hardware::radio::RadioIndicationType /*type*/,
        const ::aidl::android::hardware::radio::data::KeepaliveStatus /*status*/)
        override {}
    virtual void pcoData(
        const ::aidl::android::hardware::radio::RadioIndicationType /*type*/,
        const ::aidl::android::hardware::radio::data::PcoDataInfo /*pco*/) override {}
    virtual void unthrottleApn(
        const ::aidl::android::hardware::radio::RadioIndicationType /*type*/,
        const ::aidl::android::hardware::radio::data::DataProfileInfo /*dp*/) override {}
};

struct RadioMessagingCallbackAdapter : public RadioMessagingCallback {
    virtual ~RadioMessagingCallbackAdapter() = default;
    virtual void setResponseFunctions(
            const std::shared_ptr<RadioMessagingResponseCallback>& /* responseCb */,
            const std::shared_ptr<RadioMessagingIndicationCallback>& /* indicationCb */) override {}
    virtual void acknowledgeIncomingGsmSmsWithPdu(int32_t /*serial*/, bool /* success */,
                                                          const std::string& /* ackPdu */) override {}
    virtual void acknowledgeLastIncomingCdmaSms(int32_t /*serial*/,
            const ::aidl::android::hardware::radio::messaging::CdmaSmsAck& /* smsAck */) override {}
    virtual void acknowledgeLastIncomingGsmSms(
            int32_t /*serial*/, bool /* success */,
            ::aidl::android::hardware::radio::messaging::SmsAcknowledgeFailCause /* cause */) override {}
    virtual void deleteSmsOnRuim(int32_t /*serial*/, int32_t /* index */) override {}
    virtual void deleteSmsOnSim(int32_t /*serial*/, int32_t /* index */) override {}
    virtual void getCdmaBroadcastConfig(int32_t /*serial*/) override {}
    virtual void getGsmBroadcastConfig(int32_t /*serial*/) override {}
    virtual void getSmscAddress(int32_t /*serial*/) override {}
    virtual void reportSmsMemoryStatus(int32_t /*serial*/, bool /* available */) override {}
    virtual void responseAcknowledgement() override {}
    virtual void sendCdmaSms(int32_t /*serial*/,
            const ::aidl::android::hardware::radio::messaging::CdmaSmsMessage& /* sms */) override {}
    virtual void sendCdmaSmsExpectMore(int32_t /*serial*/,
            const ::aidl::android::hardware::radio::messaging::CdmaSmsMessage& /* sms */) override {}
    virtual void sendImsSms(int32_t /*serial*/,
            const ::aidl::android::hardware::radio::messaging::ImsSmsMessage& /* message */) override {}
    virtual void sendSms(int32_t /*serial*/,
            const ::aidl::android::hardware::radio::messaging::GsmSmsMessage& /* message */) override {}
    virtual void sendSmsExpectMore(int32_t /*serial*/,
            const ::aidl::android::hardware::radio::messaging::GsmSmsMessage& /* message */) override {}
    virtual void setCdmaBroadcastActivation(int32_t /*serial*/, bool /* activate */) override {}
    virtual void setCdmaBroadcastConfig(int32_t /*serial*/,
            const std::vector<::aidl::android::hardware::radio::messaging::CdmaBroadcastSmsConfigInfo>&
                    /* configInfo */) override {}
    virtual void setGsmBroadcastActivation(int32_t /*serial*/, bool /* activate */) override {}
    virtual void setGsmBroadcastConfig(int32_t /*serial*/,
            const std::vector<::aidl::android::hardware::radio::messaging::GsmBroadcastSmsConfigInfo>&
                    /* configInfo */) override {}
    virtual void setSmscAddress(int32_t /*serial*/, const std::string& /* smsc */) override {}
    virtual void writeSmsToRuim(int32_t /*serial*/,
            const ::aidl::android::hardware::radio::messaging::CdmaSmsWriteArgs& /* cdmaSms */) override {}
    virtual void writeSmsToSim(int32_t /*serial*/,
            const ::aidl::android::hardware::radio::messaging::SmsWriteArgs& /* smsWriteArgs */) override {}
};

struct RadioMessagingResponseCallbackAdapter : public RadioMessagingResponseCallback {
    virtual ~RadioMessagingResponseCallbackAdapter() = default;
    virtual void setResponseFunction(const std::shared_ptr<::aidl::android::hardware::radio::messaging::IRadioMessagingResponse>& /*radioMessagingResponse*/) {}
    virtual void acknowledgeIncomingGsmSmsWithPduResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/) {}
    virtual void acknowledgeLastIncomingCdmaSmsResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/) {}
    virtual void acknowledgeLastIncomingGsmSmsResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/) {}
    virtual void acknowledgeRequest(int32_t /*serial*/) {}
    virtual void deleteSmsOnRuimResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/) {}
    virtual void deleteSmsOnSimResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/) {}
    virtual void getCdmaBroadcastConfigResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/, const std::vector<::aidl::android::hardware::radio::messaging::CdmaBroadcastSmsConfigInfo>& /*configs*/) {}
    virtual void getGsmBroadcastConfigResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/, const std::vector<::aidl::android::hardware::radio::messaging::GsmBroadcastSmsConfigInfo>& /*configs*/) {}
    virtual void getSmscAddressResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/, const std::string& /*smsc*/) {}
    virtual void reportSmsMemoryStatusResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/) {}
    virtual void sendCdmaSmsExpectMoreResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/, const ::aidl::android::hardware::radio::messaging::SendSmsResult& /*sms*/) {}
    virtual void sendCdmaSmsResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/, const ::aidl::android::hardware::radio::messaging::SendSmsResult& /*sms*/) {}
    virtual void sendImsSmsResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/, const ::aidl::android::hardware::radio::messaging::SendSmsResult& /*sms*/) {}
    virtual void sendSmsExpectMoreResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/, const ::aidl::android::hardware::radio::messaging::SendSmsResult& /*sms*/) {}
    virtual void sendSmsResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/, const ::aidl::android::hardware::radio::messaging::SendSmsResult& /*sms*/) {}
    virtual void setCdmaBroadcastActivationResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/) {}
    virtual void setCdmaBroadcastConfigResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/) {}
    virtual void setGsmBroadcastActivationResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/) {}
    virtual void setGsmBroadcastConfigResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/) {}
    virtual void setSmscAddressResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/) {}
    virtual void writeSmsToRuimResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/, int32_t /*index*/) {}
    virtual void writeSmsToSimResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/, int32_t /*index*/) {}
};

struct RadioMessagingIndicationCallbackAdapter : public RadioMessagingIndicationCallback {
    virtual ~RadioMessagingIndicationCallbackAdapter() = default;
    virtual void setResponseFunction(const std::shared_ptr<::aidl::android::hardware::radio::messaging::IRadioMessagingIndication>& /*radioMessagingIndication*/) {}
    virtual void cdmaNewSms(::aidl::android::hardware::radio::RadioIndicationType /*type*/, const ::aidl::android::hardware::radio::messaging::CdmaSmsMessage& /*msg*/) {}
    virtual void cdmaRuimSmsStorageFull(::aidl::android::hardware::radio::RadioIndicationType /*type*/) {}
    virtual void newBroadcastSms(::aidl::android::hardware::radio::RadioIndicationType /*type*/, const std::vector<uint8_t>& /*data*/) {}
    virtual void newSms(::aidl::android::hardware::radio::RadioIndicationType /*type*/, const std::vector<uint8_t>& /*pdu*/) {}
    virtual void newSmsOnSim(::aidl::android::hardware::radio::RadioIndicationType /*type*/, int32_t /*recordNumber*/) {}
    virtual void newSmsStatusReport(::aidl::android::hardware::radio::RadioIndicationType /*type*/, const std::vector<uint8_t>& /*pdu*/) {}
    virtual void simSmsStorageFull(::aidl::android::hardware::radio::RadioIndicationType /*type*/) {}
};

struct RadioNetworkCallbackAdapter : public RadioNetworkCallback {
    virtual ~RadioNetworkCallbackAdapter() = default;
    virtual void setResponseFunctions(
            const std::shared_ptr<RadioNetworkResponseCallback>& /*responseCb*/,
            const std::shared_ptr<RadioNetworkIndicationCallback>& /*indicationCb*/) override {}
    virtual void getAllowedNetworkTypesBitmap(int32_t /*serial*/) override {}
    virtual void getAvailableBandModes(int32_t /*serial*/) override {}
    virtual void getAvailableNetworks(int32_t /*serial*/) override {}
    virtual void getBarringInfo(int32_t /*serial*/) override {}
    virtual void getCdmaRoamingPreference(int32_t /*serial*/) override {}
    virtual void getCellInfoList(int32_t /*serial*/) override {}
    virtual void getDataRegistrationState(int32_t /*serial*/) override {}
    virtual void getImsRegistrationState(int32_t /*serial*/) override {}
    virtual void getNetworkSelectionMode(int32_t /*serial*/) override {}
    virtual void getOperator(int32_t /*serial*/) override {}
    virtual void getSignalStrength(int32_t /*serial*/) override {}
    virtual void getSystemSelectionChannels(int32_t /*serial*/) override {}
    virtual void getVoiceRadioTechnology(int32_t /*serial*/) override {}
    virtual void getVoiceRegistrationState(int32_t /*serial*/) override {}
    virtual void isNrDualConnectivityEnabled(int32_t /*serial*/) override {}
    virtual void responseAcknowledgement() override {}
    virtual void setAllowedNetworkTypesBitmap(int32_t /*serial*/, int32_t /*networkTypeBitmap*/) override {}
    virtual void setBandMode(int32_t /*serial*/,
            ::aidl::android::hardware::radio::network::RadioBandMode /*mode*/) override {}
    virtual void setBarringPassword(int32_t /*serial*/, const std::string& /*facility*/,
                                            const std::string& /*oldPassword*/,
                                            const std::string& /*newPassword*/) override {}
    virtual void setCdmaRoamingPreference(int32_t /*serial*/,
            ::aidl::android::hardware::radio::network::CdmaRoamingType /*type*/) override {}
    virtual void setCellInfoListRate(int32_t /*serial*/, int32_t /*rate*/) override {}
    virtual void setIndicationFilter(int32_t /*serial*/, int32_t /*indicationFilter*/) override {}
    virtual void setLinkCapacityReportingCriteria(
            int32_t /*serial*/, int32_t /*hysteresisMs*/, int32_t /*hysteresisDlKbps*/,
            int32_t /*hysteresisUlKbps*/, const std::vector<int32_t>& /*thresholdsDownlinkKbps*/,
            const std::vector<int32_t>& /*thresholdsUplinkKbps*/,
            ::aidl::android::hardware::radio::AccessNetwork /*accessNetwork*/) override {}
    virtual void setLocationUpdates(int32_t /*serial*/, bool /*enable*/) override {}
    virtual void setNetworkSelectionModeAutomatic(int32_t /*serial*/) override {}
    virtual void setNetworkSelectionModeManual(int32_t /*serial*/, const std::string& /*operatorNumeric*/,
            ::aidl::android::hardware::radio::AccessNetwork /*ran*/) override {}
    virtual void setNrDualConnectivityState(int32_t /*serial*/,
            ::aidl::android::hardware::radio::network::NrDualConnectivityState /*nrSt*/) override {}
    virtual void setSignalStrengthReportingCriteria(int32_t /*serial*/,
            const std::vector<::aidl::android::hardware::radio::network::SignalThresholdInfo>&
                    /*signalThresholdInfos*/) override {}
    virtual void setSuppServiceNotifications(int32_t /*serial*/, bool /*enable*/) override {}
    virtual void setSystemSelectionChannels(int32_t /*serial*/, bool /*specifyChannels*/,
            const std::vector<::aidl::android::hardware::radio::network::RadioAccessSpecifier>&
                    /*specifiers*/) override {}
    virtual void startNetworkScan(int32_t /*serial*/,
            const ::aidl::android::hardware::radio::network::NetworkScanRequest& /*request*/) override {}
    virtual void stopNetworkScan(int32_t /*serial*/) override {}
    virtual void supplyNetworkDepersonalization(int32_t /*serial*/, const std::string& /*netPin*/) override {}
    virtual void setUsageSetting(int32_t /* serial */,
            ::aidl::android::hardware::radio::network::UsageSetting /* usageSetting */) override {}
    virtual void  getUsageSetting(int32_t /* serial */) override {}

};

struct RadioNetworkResponseCallbackAdapter : public RadioNetworkResponseCallback{
    virtual ~RadioNetworkResponseCallbackAdapter() = default;
    virtual void setResponseFunction(
            const std::shared_ptr<::aidl::android::hardware::radio::network::IRadioNetworkResponse>&
                /* radioNetworkResponse */) {}
    virtual void acknowledgeRequest(int32_t /*serial*/) override {}
    virtual void getAllowedNetworkTypesBitmapResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /* info */,
            int /* networkTypeBitmap */) override {}
    virtual void getAvailableBandModesResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /* info */,
            const std::vector<::aidl::android::hardware::radio::network::RadioBandMode>& /* bandModes */) override {}
    virtual void getAvailableNetworksResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /* info */,
            const std::vector<::aidl::android::hardware::radio::network::OperatorInfo>& /* networkInfos */) override {}
    virtual void getBarringInfoResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /* info */,
            const ::aidl::android::hardware::radio::network::CellIdentity& /* cellIdentity */,
            const std::vector<::aidl::android::hardware::radio::network::BarringInfo>& /* barringInfos */) override {}
    virtual void getCdmaRoamingPreferenceResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /* info */,
            ::aidl::android::hardware::radio::network::CdmaRoamingType /* type */) override {}
    virtual void getCellInfoListResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /* info */,
            const std::vector<::aidl::android::hardware::radio::network::CellInfo>& /* cellInfo */) override {}
    virtual void getDataRegistrationStateResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /* info */,
            const ::aidl::android::hardware::radio::network::RegStateResult& /* dataRegResponse */) override {}
    virtual void getImsRegistrationStateResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /* info */,
            bool /* isRegistered */, ::aidl::android::hardware::radio::RadioTechnologyFamily /* ratFamily */) override {}
    virtual void getNetworkSelectionModeResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /* info */, bool /* manual */) override {}
    virtual void getOperatorResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /* info */,
            const std::string& /* longName */, const std::string& /* shortName */, const std::string& /* numeric */) override {}
    virtual void getSignalStrengthResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /* info */,
            const ::aidl::android::hardware::radio::network::SignalStrength& /* signalStrength */) override {}
    virtual void getSystemSelectionChannelsResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /* info */,
            const std::vector<::aidl::android::hardware::radio::network::RadioAccessSpecifier>& /* specifiers */) override {}
    virtual void getVoiceRadioTechnologyResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /* info */,
            ::aidl::android::hardware::radio::RadioTechnology /* rat */) override {}
    virtual void getVoiceRegistrationStateResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /* info */,
            const ::aidl::android::hardware::radio::network::RegStateResult& /* voiceRegResponse */) override {}
    virtual void isNrDualConnectivityEnabledResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /* info */, bool /* isEnabled */) override {}
    virtual void setAllowedNetworkTypesBitmapResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /* info */) override {}
    virtual void setBandModeResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /* info */) override {}
    virtual void setBarringPasswordResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /* info */) override {}
    virtual void setCdmaRoamingPreferenceResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /* info */) override {}
    virtual void setCellInfoListRateResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /* info */) override {}
    virtual void setIndicationFilterResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /* info */) override {}
    virtual void setLinkCapacityReportingCriteriaResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /* info */) override {}
    virtual void setLocationUpdatesResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /* info */) override {}
    virtual void setNetworkSelectionModeAutomaticResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /* info */) override {}
    virtual void setNetworkSelectionModeManualResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /* info */) override {}
    virtual void setNrDualConnectivityStateResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /* info */) override {}
    virtual void setSignalStrengthReportingCriteriaResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /* info */) override {}
    virtual void setSuppServiceNotificationsResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /* info */) override {}
    virtual void setSystemSelectionChannelsResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /* info */) override {}
    virtual void startNetworkScanResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /* info */) override {}
    virtual void stopNetworkScanResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /* info */) override {}
    virtual void supplyNetworkDepersonalizationResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /* info */, int /* remainingRetries */) override {}
    virtual void setUsageSettingResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /* info */) override {}
    virtual void getUsageSettingResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /* info */,
            ::aidl::android::hardware::radio::network::UsageSetting /* usageSetting */) override {}
};

struct RadioNetworkIndicationCallbackAdapter : public RadioNetworkIndicationCallback {
    virtual ~RadioNetworkIndicationCallbackAdapter() = default;
    virtual void setResponseFunction(
            const std::shared_ptr<::aidl::android::hardware::radio::network::IRadioNetworkIndication>&
                    /* radioNetworkIndication */) {}
    virtual void barringInfoChanged(::aidl::android::hardware::radio::RadioIndicationType /* type */,
            const ::aidl::android::hardware::radio::network::CellIdentity& /* cellIdentity */,
            const std::vector<::aidl::android::hardware::radio::network::BarringInfo>& /* barringInfos */) override {}
    virtual void cdmaPrlChanged(::aidl::android::hardware::radio::RadioIndicationType /* type */, int /* version */) override {}
    virtual void cellInfoList(::aidl::android::hardware::radio::RadioIndicationType /* type */,
            const std::vector<::aidl::android::hardware::radio::network::CellInfo>& /* records */) override {}
    virtual void currentLinkCapacityEstimate(::aidl::android::hardware::radio::RadioIndicationType /* type */,
            const ::aidl::android::hardware::radio::network::LinkCapacityEstimate& /* lce */) override {}
    virtual void currentPhysicalChannelConfigs(::aidl::android::hardware::radio::RadioIndicationType /* type */,
            const std::vector<::aidl::android::hardware::radio::network::PhysicalChannelConfig>& /* configs */) override {}
    virtual void currentSignalStrength(::aidl::android::hardware::radio::RadioIndicationType /* type */,
            const ::aidl::android::hardware::radio::network::SignalStrength& /* signalStrength */) override {}
    virtual void imsNetworkStateChanged(::aidl::android::hardware::radio::RadioIndicationType /* type */) override {}
    virtual void networkScanResult(::aidl::android::hardware::radio::RadioIndicationType /* type */,
            const ::aidl::android::hardware::radio::network::NetworkScanResult& /* result */) override {}
    virtual void networkStateChanged(::aidl::android::hardware::radio::RadioIndicationType /* type */) override {}
    virtual void nitzTimeReceived(::aidl::android::hardware::radio::RadioIndicationType /* type */,
            const std::string& /* nitzTime */, long /* receivedTimeMs */, long /* ageMs */) override {}
    virtual void registrationFailed(::aidl::android::hardware::radio::RadioIndicationType /* type */,
            const ::aidl::android::hardware::radio::network::CellIdentity& /* cellIdentity */,
            const std::string& /* chosenPlmn */, int /* domain */,
            int /* causeCode */, int /* additionalCauseCode */) override {}
    virtual void restrictedStateChanged(::aidl::android::hardware::radio::RadioIndicationType /* type */,
            ::aidl::android::hardware::radio::network::PhoneRestrictedState /* state */) override {}
    virtual void suppSvcNotify(::aidl::android::hardware::radio::RadioIndicationType /* type */,
            const ::aidl::android::hardware::radio::network::SuppSvcNotification& /* suppSvc */) override {}
    virtual void voiceRadioTechChanged(::aidl::android::hardware::radio::RadioIndicationType /* type */,
            ::aidl::android::hardware::radio::RadioTechnology /* rat */) override {}
};

struct RadioSimCallbackAdapter : public RadioSimCallback {
    virtual ~RadioSimCallbackAdapter() = default;
    virtual void setResponseFunctions(
            const std::shared_ptr<RadioSimResponseCallback>& /* responseCb */,
            const std::shared_ptr<RadioSimIndicationCallback>& /* indicationCb */) override {}
    virtual void areUiccApplicationsEnabled(int32_t /*serial*/) override {}
    virtual void changeIccPin2ForApp(int32_t /*serial*/, const std::string& /*oldPin2*/,
            const std::string& /*newPin2*/, const std::string& /*aid*/) override {}
    virtual void changeIccPinForApp(int32_t /*serial*/, const std::string& /*oldPin*/,
            const std::string& /*newPin*/, const std::string& /*aid*/) override {}
    virtual void enableUiccApplications(int32_t /*serial*/, bool /*enable*/) override {}
    virtual void getAllowedCarriers(int32_t /*serial*/) override {}
    virtual void getCdmaSubscription(int32_t /*serial*/) override {}
    virtual void getCdmaSubscriptionSource(int32_t /*serial*/) override {}
    virtual void getFacilityLockForApp(int32_t /*serial*/,
            const std::string& /*facility*/, const std::string& /*password*/,
            int32_t /*serviceClass*/, const std::string& /*appId*/) override {}
    virtual void getIccCardStatus(int32_t /*serial*/) override {}
    virtual void getImsiForApp(int32_t /*serial*/, const std::string& /*aid*/) override {}
    virtual void getSimPhonebookCapacity(int32_t /*serial*/) override {}
    virtual void getSimPhonebookRecords(int32_t /*serial*/) override {}
    virtual void iccCloseLogicalChannel(int32_t /*serial*/,
            int32_t /*channelId*/) override {}
    virtual void iccIoForApp(int32_t /*serial*/,
            const ::aidl::android::hardware::radio::sim::IccIo& /*iccIo*/) override {}
    virtual void iccOpenLogicalChannel(int32_t /*serial*/, const std::string& /*aid*/,
            int32_t /*p2&*/) override {}
    virtual void iccTransmitApduBasicChannel(int32_t /*serial*/,
            const ::aidl::android::hardware::radio::sim::SimApdu& /*message*/) override {}
    virtual void iccTransmitApduLogicalChannel(int32_t /*serial*/,
            const ::aidl::android::hardware::radio::sim::SimApdu& /*message*/) override {}
    virtual void reportStkServiceIsRunning(int32_t /*serial*/) override {}
    virtual void requestIccSimAuthentication(int32_t /*serial*/,
            int32_t /*authContext*/, const std::string& /*authData*/,
            const std::string& /*aid*/) override {}
    virtual void responseAcknowledgement() override {}
    virtual void sendEnvelope(int32_t /*serial*/, const std::string& /*command*/) override {}
    virtual void sendEnvelopeWithStatus(int32_t /*serial*/,
            const std::string& /*contents*/) override {}
    virtual void sendTerminalResponseToSim(int32_t /*serial*/,
            const std::string& /*commandResponse*/) override {}
    virtual void setAllowedCarriers(int32_t /*serial*/,
            const ::aidl::android::hardware::radio::sim::CarrierRestrictions& /*carriers*/,
            ::aidl::android::hardware::radio::sim::SimLockMultiSimPolicy /*multiSimPolicy*/
            ) override {}
    virtual void setCarrierInfoForImsiEncryption(int32_t /*serial*/,
            const ::aidl::android::hardware::radio::sim::ImsiEncryptionInfo& /*imsiEncryptionInfo*/
            ) override {}
    virtual void setCdmaSubscriptionSource(int32_t /*serial*/,
            ::aidl::android::hardware::radio::sim::CdmaSubscriptionSource /*cdmaSub*/) override {}
    virtual void setFacilityLockForApp(int32_t /*serial*/,
            const std::string& /*facility*/, bool /*lockState*/, const std::string& /*passwd*/,
            int32_t /*serviceClass*/, const std::string& /*appId*/) override {}
    virtual void setSimCardPower(int32_t /*serial*/,
            ::aidl::android::hardware::radio::sim::CardPowerState /*powerUp*/) override {}
    virtual void setUiccSubscription(int32_t /*serial*/,
            const ::aidl::android::hardware::radio::sim::SelectUiccSub& /*uiccSub*/) override {}
    virtual void supplyIccPin2ForApp(int32_t /*serial*/, const std::string& /*pin2*/,
            const std::string& /*aid*/) override {}
    virtual void supplyIccPinForApp(int32_t /*serial*/, const std::string& /*pin*/,
            const std::string& /*aid*/) override {}
    virtual void supplyIccPuk2ForApp(int32_t /*serial*/, const std::string& /*puk2*/,
            const std::string& /*pin2*/, const std::string& /*aid*/) override {}
    virtual void supplyIccPukForApp(int32_t /*serial*/, const std::string& /*puk*/,
            const std::string& /*pin*/, const std::string& /*aid*/) override {}
    virtual void supplySimDepersonalization(
            int32_t /*serial*/, ::aidl::android::hardware::radio::sim::PersoSubstate /*persoType*/,
            const std::string& /*controlKey*/) override {}
    virtual void updateSimPhonebookRecords(int32_t /*serial*/,
            const ::aidl::android::hardware::radio::sim::PhonebookRecordInfo& /*recordInfo*/
            ) override {}
};

struct RadioSimResponseCallbackAdapter : public RadioSimResponseCallback {
    virtual ~RadioSimResponseCallbackAdapter() = default;
    virtual void setResponseFunction(
            const std::shared_ptr<::aidl::android::hardware::radio::sim::IRadioSimResponse>&) {}
    virtual void acknowledgeRequest(int32_t) {}
    virtual void areUiccApplicationsEnabledResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo&, bool) {}
    virtual void changeIccPin2ForAppResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo&, int) {}
    virtual void changeIccPinForAppResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo&, int) {}
    virtual void enableUiccApplicationsResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo&) {}
    virtual void getAllowedCarriersResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo&,
            const ::aidl::android::hardware::radio::sim::CarrierRestrictions&,
            const ::aidl::android::hardware::radio::sim::SimLockMultiSimPolicy&) {}
    virtual void getCdmaSubscriptionResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo&,
            const std::string&, const std::string&, const std::string&,
            const std::string&, const std::string&) {}
    virtual void getCdmaSubscriptionSourceResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo&,
            ::aidl::android::hardware::radio::sim::CdmaSubscriptionSource) {}
    virtual void getFacilityLockForAppResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo&, int) {}
    virtual void getIccCardStatusResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo&,
            const ::aidl::android::hardware::radio::sim::CardStatus&) {}
    virtual void getIMSIForAppResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo&, const std::string&) {}
    virtual void getSimPhonebookCapacityResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo&,
            const ::aidl::android::hardware::radio::sim::PhonebookCapacity&) {}
    virtual void getSimPhonebookRecordsResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo&) {}
    virtual void iccCloseLogicalChannelResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo&) {}
    virtual void iccIoForAppResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo&,
            const ::aidl::android::hardware::radio::sim::IccIoResult&) {}
    virtual void iccOpenLogicalChannelResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo&, int,
            const std::vector<uint8_t>&) {}
    virtual void iccTransmitApduBasicChannelResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo&,
            const ::aidl::android::hardware::radio::sim::IccIoResult&) {}
    virtual void iccTransmitApduLogicalChannelResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo&,
            const ::aidl::android::hardware::radio::sim::IccIoResult&) {}
    virtual void reportStkServiceIsRunningResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo&) {}
    virtual void requestIccSimAuthenticationResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo&,
            const ::aidl::android::hardware::radio::sim::IccIoResult&) {}
    virtual void sendEnvelopeResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo&,
            const std::string&) {}
    virtual void sendEnvelopeWithStatusResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo&,
            const ::aidl::android::hardware::radio::sim::IccIoResult&) {}
    virtual void sendTerminalResponseToSimResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo&) {}
    virtual void setAllowedCarriersResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo&) {}
    virtual void setCarrierInfoForImsiEncryptionResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo&) {}
    virtual void setCdmaSubscriptionSourceResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo&) {}
    virtual void setFacilityLockForAppResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo&, int32_t) {}
    virtual void setSimCardPowerResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo&) {}
    virtual void setUiccSubscriptionResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo&) {}
    virtual void supplyIccPin2ForAppResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo&, int32_t) {}
    virtual void supplyIccPinForAppResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo&, int32_t) {}
    virtual void supplyIccPuk2ForAppResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo&, int32_t) {}
    virtual void supplyIccPukForAppResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo&, int32_t) {}
    virtual void supplySimDepersonalizationResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo&,
            const ::aidl::android::hardware::radio::sim::PersoSubstate, int32_t) {}
    virtual void updateSimPhonebookRecordsResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo&, int32_t) {}
};

struct RadioSimIndicationCallbackAdapter : public RadioSimIndicationCallback {
    virtual ~RadioSimIndicationCallbackAdapter() = default;
    virtual void setResponseFunction(
        const std::shared_ptr<::aidl::android::hardware::radio::sim::IRadioSimIndication>&) {};
    virtual void carrierInfoForImsiEncryption(
            ::aidl::android::hardware::radio::RadioIndicationType) {}
    virtual void cdmaSubscriptionSourceChanged(
            ::aidl::android::hardware::radio::RadioIndicationType,
            ::aidl::android::hardware::radio::sim::CdmaSubscriptionSource) {}
    virtual void simPhonebookChanged(::aidl::android::hardware::radio::RadioIndicationType) {}
    virtual void simPhonebookRecordsReceived(
            ::aidl::android::hardware::radio::RadioIndicationType,
            ::aidl::android::hardware::radio::sim::PbReceivedStatus,
            const std::vector<::aidl::android::hardware::radio::sim::PhonebookRecordInfo>&) {}
    virtual void simRefresh(
            ::aidl::android::hardware::radio::RadioIndicationType,
            const ::aidl::android::hardware::radio::sim::SimRefreshResult&) {}
    virtual void simStatusChanged(
            ::aidl::android::hardware::radio::RadioIndicationType) {}
    virtual void stkEventNotify(
            ::aidl::android::hardware::radio::RadioIndicationType, const std::string&) {}
    virtual void stkProactiveCommand(
            ::aidl::android::hardware::radio::RadioIndicationType, const std::string&) {}
    virtual void stkSessionEnd(::aidl::android::hardware::radio::RadioIndicationType) {}
    virtual void subscriptionStatusChanged(
            ::aidl::android::hardware::radio::RadioIndicationType, bool) {}
    virtual void uiccApplicationsEnablementChanged(
            ::aidl::android::hardware::radio::RadioIndicationType, bool) {}
};

struct RadioConfigCallbackAdapter : public RadioConfigCallback {
    virtual ~RadioConfigCallbackAdapter() = default;
    virtual void setResponseFunctions(
            const std::shared_ptr<RadioConfigResponseCallback>& /* responseCb */,
            const std::shared_ptr<RadioConfigIndicationCallback>& /* indicationCb */) override {}
    virtual void getHalDeviceCapabilities(int32_t /*serial*/) override {}
    virtual void getNumOfLiveModems(int32_t /*serial*/) override {}
    virtual void getPhoneCapability(int32_t /*serial*/) override {}
    virtual void getSimSlotsStatus(int32_t /*serial*/) override {}
    virtual void setNumOfLiveModems(int32_t /*serial*/, int8_t /* numOfLiveModems */) override {}
    virtual void setPreferredDataModem(int32_t /*serial*/, int8_t /* modemId */) override {}
    virtual void setSimSlotsMapping(int32_t /*serial*/,
            const std::vector<aidl::android::hardware::radio::config::SlotPortMapping>& /* slotMap */)
            override {}
};

struct RadioConfigResponseCallbackAdapter : public RadioConfigResponseCallback {
    virtual ~RadioConfigResponseCallbackAdapter() = default;
    virtual void setResponseFunction(
            const std::shared_ptr<::aidl::android::hardware::radio::config::IRadioConfigResponse>&
                    /* radioConfigResponse */) override {}
    virtual void getHalDeviceCapabilitiesResponse(
        const ::aidl::android::hardware::radio::RadioResponseInfo& /* info*/,
        bool /* modemReducedFeatureSet1 */) override {}
    virtual void getNumOfLiveModemsResponse(
        const ::aidl::android::hardware::radio::RadioResponseInfo& /* info*/,
        int8_t /* numOfLiveModems */) override {}
    virtual void getPhoneCapabilityResponse(
        const ::aidl::android::hardware::radio::RadioResponseInfo& /* info*/,
        const ::aidl::android::hardware::radio::config::PhoneCapability& /* phoneCapability */) override {}
    virtual void getSimSlotsStatusResponse(
        const ::aidl::android::hardware::radio::RadioResponseInfo& /* info*/,
        const std::vector<::aidl::android::hardware::radio::config::SimSlotStatus>& /* slotStatus */) override {}
    virtual void setNumOfLiveModemsResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /* info*/) override {}
    virtual void setPreferredDataModemResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /* info*/) override {}
    virtual void setSimSlotsMappingResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /* info*/) override {}
};

struct RadioConfigIndicationCallbackAdapter : public RadioConfigIndicationCallback {
    virtual ~RadioConfigIndicationCallbackAdapter() = default;
    virtual void setResponseFunction(
            const std::shared_ptr<::aidl::android::hardware::radio::config::IRadioConfigIndication>&
                    /* radioConfigIndication */) override {}
    virtual void simSlotsStatusChanged(
        ::aidl::android::hardware::radio::RadioIndicationType /* type*/,
        const std::vector<::aidl::android::hardware::radio::config::SimSlotStatus>& /* slotStatus */) override {}
};

struct RadioVoiceCallbackAdapter : public RadioVoiceCallback {
    virtual ~RadioVoiceCallbackAdapter() = default;
    virtual void setResponseFunctions(
            const std::shared_ptr<RadioVoiceResponseCallback>& /* responseCb */,
            const std::shared_ptr<RadioVoiceIndicationCallback>& /* indicationCb */) override {}
    virtual void acceptCall(int32_t /*serial*/) override {}
    virtual void cancelPendingUssd(int32_t /*serial*/) override {}
    virtual void conference(int32_t /*serial*/) override {}
    virtual void dial(int32_t /*serial*/, const ::aidl::android::hardware::radio::voice::Dial& /* dialInfo */) override {}
    virtual void emergencyDial(int32_t /*serial*/, const ::aidl::android::hardware::radio::voice::Dial& /* dialInfo */,
            int32_t /* categories */, const std::vector<std::string>& /* urns */,
            ::aidl::android::hardware::radio::voice::EmergencyCallRouting /* routing */,
            bool /* hasKnownUserIntentEmergency */, bool /* isTesting */) override {}
    virtual void exitEmergencyCallbackMode(int32_t /*serial*/) override {}
    virtual void explicitCallTransfer(int32_t /*serial*/) override {}
    virtual void getCallForwardStatus(int32_t /*serial*/,
            const ::aidl::android::hardware::radio::voice::CallForwardInfo& /* callInfo */) override {}
    virtual void getCallWaiting(int32_t /*serial*/, int32_t /* serviceClass */) override {}
    virtual void getClip(int32_t /*serial*/) override {}
    virtual void getClir(int32_t /*serial*/) override {}
    virtual void getCurrentCalls(int32_t /*serial*/) override {}
    virtual void getLastCallFailCause(int32_t /*serial*/) override {}
    virtual void getMute(int32_t /*serial*/) override {}
    virtual void getPreferredVoicePrivacy(int32_t /*serial*/) override {}
    virtual void getTtyMode(int32_t /*serial*/) override {}
    virtual void handleStkCallSetupRequestFromSim(int32_t /*serial*/, bool /* accept */) override {}
    virtual void hangup(int32_t /*serial*/, int32_t /* gsmIndex */) override {}
    virtual void hangupForegroundResumeBackground(int32_t /*serial*/) override {}
    virtual void hangupWaitingOrBackground(int32_t /*serial*/) override {}
    virtual void isVoNrEnabled(int32_t /*serial*/) override {}
    virtual void rejectCall(int32_t /*serial*/) override {}
    virtual void responseAcknowledgement() override {}
    virtual void sendBurstDtmf(int32_t /*serial*/, const std::string& /* dtmf */, int32_t /* on */, int32_t /* off */) override {}
    virtual void sendCdmaFeatureCode(int32_t /*serial*/, const std::string& /* fcode */) override {}
    virtual void sendDtmf(int32_t /*serial*/, const std::string& /* s */) override {}
    virtual void sendUssd(int32_t /*serial*/, const std::string& /* ussd */) override {}
    virtual void separateConnection(int32_t /*serial*/, int32_t /* gsmIndex */) override {}
    virtual void setCallForward(int32_t /*serial*/,
            const ::aidl::android::hardware::radio::voice::CallForwardInfo& /* callInfo */) override {}
    virtual void setCallWaiting(int32_t /*serial*/, bool /* enable */, int32_t /* serviceClass */) override {}
    virtual void setClir(int32_t /*serial*/, int32_t /* status */) override {}
    virtual void setMute(int32_t /*serial*/, bool /* enable */) override {}
    virtual void setPreferredVoicePrivacy(int32_t /*serial*/, bool /* enable */) override {}
    virtual void setTtyMode(int32_t /*serial*/, ::aidl::android::hardware::radio::voice::TtyMode /* mode */) override {}
    virtual void setVoNrEnabled(int32_t /*serial*/, bool /* enable */) override {}
    virtual void startDtmf(int32_t /*serial*/, const std::string& /* s */) override {}
    virtual void stopDtmf(int32_t /*serial*/) override {}
    virtual void switchWaitingOrHoldingAndActive(int32_t /*serial*/) override {}
};

struct RadioVoiceResponseCallbackAdapter : public RadioVoiceResponseCallback {
    virtual ~RadioVoiceResponseCallbackAdapter() = default;
    virtual void setResponseFunction(
            const std::shared_ptr<::aidl::android::hardware::radio::voice::IRadioVoiceResponse>&
            /* radioVoiceResponse */) {}
    virtual void acceptCallResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/) override {}
    virtual void acknowledgeRequest(int32_t /*serial*/) override {}
    virtual void cancelPendingUssdResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/) override {}
    virtual void conferenceResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/) override {}
    virtual void dialResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/) override {}
    virtual void emergencyDialResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/) override {}
    virtual void exitEmergencyCallbackModeResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/) override {}
    virtual void explicitCallTransferResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/)
        override {}
    virtual void getCallForwardStatusResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/,
            const std::vector<::aidl::android::hardware::radio::voice::CallForwardInfo>& /*callForwardInfos*/) override {}
    virtual void getCallWaitingResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/,
            bool /*enable*/, int32_t /*serviceClass*/) override {}
    virtual void getClipResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/,
            ::aidl::android::hardware::radio::voice::ClipStatus /*status*/) override {}
    virtual void getClirResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/, int32_t /*n*/,
            int32_t /*m*/) override {}
    virtual void getCurrentCallsResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/,
            const std::vector<::aidl::android::hardware::radio::voice::Call>& /*calls*/) override {}
    virtual void getLastCallFailCauseResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/,
            const ::aidl::android::hardware::radio::voice::LastCallFailCauseInfo& /*failCauseinfo*/) override {}
    virtual void getMuteResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/, bool /*enable*/)
        override {}
    virtual void getPreferredVoicePrivacyResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/, bool /*enable*/) override {}
    virtual void getTtyModeResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/,
            ::aidl::android::hardware::radio::voice::TtyMode /*mode*/) override {}
    virtual void handleStkCallSetupRequestFromSimResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/) override {}
    virtual void hangupConnectionResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/) override {}
    virtual void hangupForegroundResumeBackgroundResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/) override {}
    virtual void hangupWaitingOrBackgroundResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/) override {}
    virtual void isVoNrEnabledResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/,
            bool /*enable*/) override {}
    virtual void rejectCallResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/) override {}
    virtual void sendBurstDtmfResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/) override {}
    virtual void sendCdmaFeatureCodeResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/)
        override {}
    virtual void sendDtmfResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/) override {}
    virtual void sendUssdResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/) override {}
    virtual void separateConnectionResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/) override {}
    virtual void setCallForwardResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/) override {}
    virtual void setCallWaitingResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/) override {}
    virtual void setClirResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/) override {}
    virtual void setMuteResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/) override {}
    virtual void setPreferredVoicePrivacyResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/) override {}
    virtual void setTtyModeResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/) override {}
    virtual void setVoNrEnabledResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/) override {}
    virtual void startDtmfResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/) override {}
    virtual void stopDtmfResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/) override {}
    virtual void switchWaitingOrHoldingAndActiveResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/) override {}
};

struct RadioVoiceIndicationCallbackAdapter : public RadioVoiceIndicationCallback {
    virtual ~RadioVoiceIndicationCallbackAdapter() = default;
    virtual void setResponseFunction(
            const std::shared_ptr<::aidl::android::hardware::radio::voice::IRadioVoiceIndication>&
            /* radioVoiceIndication */) {}
    virtual void callRing(::aidl::android::hardware::radio::RadioIndicationType /*type*/, bool /*isGsm*/,
            const ::aidl::android::hardware::radio::voice::CdmaSignalInfoRecord& /*record*/) override {}
    virtual void callStateChanged(::aidl::android::hardware::radio::RadioIndicationType /*type*/) override {}
    virtual void cdmaCallWaiting(::aidl::android::hardware::radio::RadioIndicationType /*type*/,
            const ::aidl::android::hardware::radio::voice::CdmaCallWaiting& /*callWaitingRecord*/) override {}
    virtual void cdmaInfoRec(::aidl::android::hardware::radio::RadioIndicationType /*type*/,
            const std::vector<::aidl::android::hardware::radio::voice::CdmaInformationRecord>& /*records*/) override {}
    virtual void cdmaOtaProvisionStatus(::aidl::android::hardware::radio::RadioIndicationType /*type*/,
            ::aidl::android::hardware::radio::voice::CdmaOtaProvisionStatus /*status*/) override {}
    virtual void currentEmergencyNumberList(::aidl::android::hardware::radio::RadioIndicationType /*type*/,
            const std::vector<::aidl::android::hardware::radio::voice::EmergencyNumber>& /*emergencyNumberList*/)
        override {}
    virtual void enterEmergencyCallbackMode(::aidl::android::hardware::radio::RadioIndicationType /*type*/) override {}
    virtual void exitEmergencyCallbackMode(::aidl::android::hardware::radio::RadioIndicationType /*type*/) override {}
    virtual void indicateRingbackTone(::aidl::android::hardware::radio::RadioIndicationType /*type*/, bool /*start*/)
        override {}
    virtual void onSupplementaryServiceIndication(::aidl::android::hardware::radio::RadioIndicationType /*type*/,
            const ::aidl::android::hardware::radio::voice::StkCcUnsolSsResult& /*ss*/) override {}
    virtual void onUssd(::aidl::android::hardware::radio::RadioIndicationType /*type*/,
            ::aidl::android::hardware::radio::voice::UssdModeType /* modeType */, const std::string& /* msg */) override {}
    virtual void resendIncallMute(::aidl::android::hardware::radio::RadioIndicationType /*type*/) override {}
    virtual void srvccStateNotify(::aidl::android::hardware::radio::RadioIndicationType /*type*/,
            ::aidl::android::hardware::radio::voice::SrvccState /*state*/) override {}
    virtual void stkCallControlAlphaNotify(::aidl::android::hardware::radio::RadioIndicationType /*type*/,
            const std::string& /*alpha*/) override {}
    virtual void stkCallSetup(::aidl::android::hardware::radio::RadioIndicationType /*type*/, int64_t /*timeout*/) override {}
};

}
