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
 * radio_indication_1_6.cpp
 */
#include "radio_hidl_utils_1_6.h"

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

/* 1.6 Apis */
Return<void> RadioIndicationBase_1_6::dataCallListChanged_1_6(
        RadioIndicationType /*type*/,
        const hidl_vec<android::hardware::radio::V1_6::SetupDataCallResult>& /*dcList*/) {
    return Void();
}

Return<void> RadioIndicationBase_1_6::unthrottleApn(RadioIndicationType /*type*/,
                                                 const ::android::hardware::hidl_string& /*apn*/) {
    return Void();
}

Return<void> RadioIndicationBase_1_6::currentSignalStrength_1_6(
        RadioIndicationType /*type*/,
        const ::android::hardware::radio::V1_6::SignalStrength& /*signalStrength*/) {
    return Void();
}

Return<void> RadioIndicationBase_1_6::networkScanResult_1_6(
        RadioIndicationType /*type*/,
        const ::android::hardware::radio::V1_6::NetworkScanResult& /*result*/) {
    return Void();
}

Return<void> RadioIndicationBase_1_6::cellInfoList_1_6(
        RadioIndicationType /*type*/,
        const ::android::hardware::hidl_vec<
                ::android::hardware::radio::V1_6::CellInfo>& /*records*/) {
    return Void();
}

Return<void> RadioIndicationBase_1_6::currentLinkCapacityEstimate_1_6(
        RadioIndicationType /*type*/,
        const ::android::hardware::radio::V1_6::LinkCapacityEstimate& /*lce*/) {
    return Void();
}

Return<void> RadioIndicationBase_1_6::currentPhysicalChannelConfigs_1_6(
        RadioIndicationType /*type*/,
        const ::android::hardware::hidl_vec<
                ::android::hardware::radio::V1_6::PhysicalChannelConfig>& /*configs*/) {
    return Void();
}

Return<void> RadioIndicationBase_1_6::simPhonebookChanged(
        RadioIndicationType /*type*/) {
    return Void();
}

Return<void> RadioIndicationBase_1_6::simPhonebookRecordsReceived(
        RadioIndicationType /*type*/,
        ::android::hardware::radio::V1_6::PbReceivedStatus /*status*/,
        const ::android::hardware::hidl_vec<
        ::android::hardware::radio::V1_6::PhonebookRecordInfo>& /*records*/) {
    return Void();
}

/* 1.5 Apis */
Return<void> RadioIndicationBase_1_6::uiccApplicationsEnablementChanged(RadioIndicationType /*type*/,
                                                                     bool /*enabled*/) {
    return Void();
}

Return<void> RadioIndicationBase_1_6::registrationFailed(
        RadioIndicationType /*type*/,
        const ::android::hardware::radio::V1_5::CellIdentity& /*cellIdentity*/,
        const hidl_string& /*chosenPlmn*/,
        ::android::hardware::hidl_bitfield<::android::hardware::radio::V1_5::Domain> /*domain*/,
        int32_t /*causeCode*/, int32_t /*additionalCauseCode*/) {
    return Void();
}

Return<void> RadioIndicationBase_1_6::barringInfoChanged(
        RadioIndicationType /*type*/,
        const ::android::hardware::radio::V1_5::CellIdentity& /*cellIdentity*/,
        const hidl_vec<::android::hardware::radio::V1_5::BarringInfo>& /*barringInfos*/) {
    return Void();
}

Return<void> RadioIndicationBase_1_6::networkScanResult_1_5(
        RadioIndicationType /*type*/,
        const ::android::hardware::radio::V1_5::NetworkScanResult& /*result*/) {
    return Void();
}

Return<void> RadioIndicationBase_1_6::cellInfoList_1_5(
        RadioIndicationType /*type*/,
        const hidl_vec<::android::hardware::radio::V1_5::CellInfo>& /*records*/) {
    return Void();
}

Return<void> RadioIndicationBase_1_6::dataCallListChanged_1_5(
        RadioIndicationType /*type*/,
        const hidl_vec<android::hardware::radio::V1_5::SetupDataCallResult>& /*dcList*/) {
    return Void();
}

/* 1.4 Apis */
Return<void> RadioIndicationBase_1_6::currentPhysicalChannelConfigs_1_4(
        RadioIndicationType /*type*/,
        const ::android::hardware::hidl_vec<
                ::android::hardware::radio::V1_4::PhysicalChannelConfig>& /*configs*/) {
    return Void();
}

Return<void> RadioIndicationBase_1_6::networkScanResult_1_4(
        RadioIndicationType /*type*/,
        const ::android::hardware::radio::V1_4::NetworkScanResult& /*result*/) {
    return Void();
}

Return<void> RadioIndicationBase_1_6::cellInfoList_1_4(
        RadioIndicationType /*type*/,
        const ::android::hardware::hidl_vec<
                ::android::hardware::radio::V1_4::CellInfo>& /*records*/) {
    return Void();
}

Return<void> RadioIndicationBase_1_6::currentEmergencyNumberList(
        RadioIndicationType /*type*/,
        const ::android::hardware::hidl_vec<EmergencyNumber>& /*emergencyNumberList*/) {
    return Void();
}

Return<void> RadioIndicationBase_1_6::dataCallListChanged_1_4(
        RadioIndicationType /*type*/,
        const ::android::hardware::hidl_vec<android::hardware::radio::V1_4::SetupDataCallResult>&
        /*dcList*/) {
    return Void();
}

/* 1.2 Apis */
Return<void> RadioIndicationBase_1_6::networkScanResult_1_2(
        RadioIndicationType /*type*/,
        const ::android::hardware::radio::V1_2::NetworkScanResult& /*result*/) {
    return Void();
}

Return<void> RadioIndicationBase_1_6::cellInfoList_1_2(
        RadioIndicationType /*type*/,
        const ::android::hardware::hidl_vec<
                ::android::hardware::radio::V1_2::CellInfo>& /*records*/) {
    return Void();
}

Return<void> RadioIndicationBase_1_6::currentLinkCapacityEstimate(
        RadioIndicationType /*type*/,
        const ::android::hardware::radio::V1_2::LinkCapacityEstimate& /*lce*/) {
    return Void();
}

Return<void> RadioIndicationBase_1_6::currentPhysicalChannelConfigs(
        RadioIndicationType /*type*/,
        const ::android::hardware::hidl_vec<
                ::android::hardware::radio::V1_2::PhysicalChannelConfig>& /*configs*/) {
    return Void();
}

Return<void> RadioIndicationBase_1_6::currentSignalStrength_1_2(
        RadioIndicationType /*type*/,
        const ::android::hardware::radio::V1_2::SignalStrength& /*signalStrength*/) {
    return Void();
}

Return<void> RadioIndicationBase_1_6::currentSignalStrength_1_4(
        RadioIndicationType /*type*/,
        const ::android::hardware::radio::V1_4::SignalStrength& /*signalStrength*/) {
    return Void();
}

/* 1.1 Apis */
Return<void> RadioIndicationBase_1_6::carrierInfoForImsiEncryption(RadioIndicationType /*info*/) {
    return Void();
}

Return<void> RadioIndicationBase_1_6::networkScanResult(
        RadioIndicationType /*type*/,
        const ::android::hardware::radio::V1_1::NetworkScanResult& /*result*/) {
    return Void();
}

Return<void> RadioIndicationBase_1_6::keepaliveStatus(RadioIndicationType /*type*/,
                                                   const KeepaliveStatus& /*status*/) {
    return Void();
}

/* 1.0 Apis */
Return<void> RadioIndicationBase_1_6::radioStateChanged(RadioIndicationType /*type*/,
                                                     RadioState /*radioState*/) {
    return Void();
}

Return<void> RadioIndicationBase_1_6::callStateChanged(RadioIndicationType /*type*/) {
    return Void();
}

Return<void> RadioIndicationBase_1_6::networkStateChanged(RadioIndicationType /*type*/) {
    return Void();
}

Return<void> RadioIndicationBase_1_6::newSms(RadioIndicationType /*type*/,
                                          const ::android::hardware::hidl_vec<uint8_t>& /*pdu*/) {
    return Void();
}

Return<void> RadioIndicationBase_1_6::newSmsStatusReport(
        RadioIndicationType /*type*/, const ::android::hardware::hidl_vec<uint8_t>& /*pdu*/) {
    return Void();
}

Return<void> RadioIndicationBase_1_6::newSmsOnSim(RadioIndicationType /*type*/,
                                               int32_t /*recordNumber*/) {
    return Void();
}

Return<void> RadioIndicationBase_1_6::onUssd(RadioIndicationType /*type*/, UssdModeType /*modeType*/,
                                          const ::android::hardware::hidl_string& /*msg*/) {
    return Void();
}

Return<void> RadioIndicationBase_1_6::nitzTimeReceived(
        RadioIndicationType /*type*/, const ::android::hardware::hidl_string& /*nitzTime*/,
        uint64_t /*receivedTime*/) {
    return Void();
}

Return<void> RadioIndicationBase_1_6::currentSignalStrength(
        RadioIndicationType /*type*/,
        const ::android::hardware::radio::V1_0::SignalStrength& /*signalStrength*/) {
    return Void();
}

Return<void> RadioIndicationBase_1_6::dataCallListChanged(
        RadioIndicationType /*type*/,
        const ::android::hardware::hidl_vec<android::hardware::radio::V1_0::SetupDataCallResult>&
        /*dcList*/) {
    return Void();
}

Return<void> RadioIndicationBase_1_6::suppSvcNotify(RadioIndicationType /*type*/,
                                                 const SuppSvcNotification& /*suppSvc*/) {
    return Void();
}

Return<void> RadioIndicationBase_1_6::stkSessionEnd(RadioIndicationType /*type*/) {
    return Void();
}

Return<void> RadioIndicationBase_1_6::stkProactiveCommand(
        RadioIndicationType /*type*/, const ::android::hardware::hidl_string& /*cmd*/) {
    return Void();
}

Return<void> RadioIndicationBase_1_6::stkEventNotify(RadioIndicationType /*type*/,
                                                  const ::android::hardware::hidl_string& /*cmd*/) {
    return Void();
}

Return<void> RadioIndicationBase_1_6::stkCallSetup(RadioIndicationType /*type*/, int64_t /*timeout*/) {
    return Void();
}

Return<void> RadioIndicationBase_1_6::simSmsStorageFull(RadioIndicationType /*type*/) {
    return Void();
}

Return<void> RadioIndicationBase_1_6::simRefresh(RadioIndicationType /*type*/,
                                              const SimRefreshResult& /*refreshResult*/) {
    return Void();
}

Return<void> RadioIndicationBase_1_6::callRing(RadioIndicationType /*type*/, bool /*isGsm*/,
                                            const CdmaSignalInfoRecord& /*record*/) {
    return Void();
}

Return<void> RadioIndicationBase_1_6::simStatusChanged(RadioIndicationType /*type*/) {
    return Void();
}

Return<void> RadioIndicationBase_1_6::cdmaNewSms(RadioIndicationType /*type*/,
                                              const CdmaSmsMessage& /*msg*/) {
    return Void();
}

Return<void> RadioIndicationBase_1_6::newBroadcastSms(
        RadioIndicationType /*type*/, const ::android::hardware::hidl_vec<uint8_t>& /*data*/) {
    return Void();
}

Return<void> RadioIndicationBase_1_6::cdmaRuimSmsStorageFull(RadioIndicationType /*type*/) {
    return Void();
}

Return<void> RadioIndicationBase_1_6::restrictedStateChanged(RadioIndicationType /*type*/,
                                                          PhoneRestrictedState /*state*/) {
    return Void();
}

Return<void> RadioIndicationBase_1_6::enterEmergencyCallbackMode(RadioIndicationType /*type*/) {
    return Void();
}

Return<void> RadioIndicationBase_1_6::cdmaCallWaiting(RadioIndicationType /*type*/,
                                                   const CdmaCallWaiting& /*callWaitingRecord*/) {
    return Void();
}

Return<void> RadioIndicationBase_1_6::cdmaOtaProvisionStatus(RadioIndicationType /*type*/,
                                                          CdmaOtaProvisionStatus /*status*/) {
    return Void();
}

Return<void> RadioIndicationBase_1_6::cdmaInfoRec(RadioIndicationType /*type*/,
                                               const CdmaInformationRecords& /*records*/) {
    return Void();
}

Return<void> RadioIndicationBase_1_6::indicateRingbackTone(RadioIndicationType /*type*/,
                                                        bool /*start*/) {
    return Void();
}

Return<void> RadioIndicationBase_1_6::resendIncallMute(RadioIndicationType /*type*/) {
    return Void();
}

Return<void> RadioIndicationBase_1_6::cdmaSubscriptionSourceChanged(
        RadioIndicationType /*type*/, CdmaSubscriptionSource /*cdmaSource*/) {
    return Void();
}

Return<void> RadioIndicationBase_1_6::cdmaPrlChanged(RadioIndicationType /*type*/,
                                                  int32_t /*version*/) {
    return Void();
}

Return<void> RadioIndicationBase_1_6::exitEmergencyCallbackMode(RadioIndicationType /*type*/) {
    return Void();
}

Return<void> RadioIndicationBase_1_6::rilConnected(RadioIndicationType /*type*/) {
    return Void();
}

Return<void> RadioIndicationBase_1_6::voiceRadioTechChanged(
        RadioIndicationType /*type*/, ::android::hardware::radio::V1_0::RadioTechnology /*rat*/) {
    return Void();
}

Return<void> RadioIndicationBase_1_6::cellInfoList(
        RadioIndicationType /*type*/,
        const ::android::hardware::hidl_vec<
                ::android::hardware::radio::V1_0::CellInfo>& /*records*/) {
    return Void();
}

Return<void> RadioIndicationBase_1_6::imsNetworkStateChanged(RadioIndicationType /*type*/) {
    return Void();
}

Return<void> RadioIndicationBase_1_6::subscriptionStatusChanged(RadioIndicationType /*type*/,
                                                             bool /*activate*/) {
    return Void();
}

Return<void> RadioIndicationBase_1_6::srvccStateNotify(RadioIndicationType /*type*/,
                                                    SrvccState /*state*/) {
    return Void();
}

Return<void> RadioIndicationBase_1_6::hardwareConfigChanged(
        RadioIndicationType /*type*/,
        const ::android::hardware::hidl_vec<HardwareConfig>& /*configs*/) {
    return Void();
}

Return<void> RadioIndicationBase_1_6::radioCapabilityIndication(
        RadioIndicationType /*type*/,
        const android::hardware::radio::V1_0::RadioCapability& /*rc*/) {
    return Void();
}

Return<void> RadioIndicationBase_1_6::onSupplementaryServiceIndication(
        RadioIndicationType /*type*/, const StkCcUnsolSsResult& /*ss*/) {
    return Void();
}

Return<void> RadioIndicationBase_1_6::stkCallControlAlphaNotify(
        RadioIndicationType /*type*/, const ::android::hardware::hidl_string& /*alpha*/) {
    return Void();
}

Return<void> RadioIndicationBase_1_6::lceData(RadioIndicationType /*type*/,
                                           const LceDataInfo& /*lce*/) {
    return Void();
}

Return<void> RadioIndicationBase_1_6::pcoData(RadioIndicationType /*type*/,
                                           const PcoDataInfo& /*pco*/) {
    return Void();
}

Return<void> RadioIndicationBase_1_6::modemReset(RadioIndicationType /*type*/,
                                              const ::android::hardware::hidl_string& /*reason*/) {
    return Void();
}
