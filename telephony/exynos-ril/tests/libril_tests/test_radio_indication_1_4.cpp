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
 * test_radio_indication_1_4.cpp
 *
 *  Created on: 2021. 8. 5.
 */
#include "test_radio_hidl_utils_1_4.h"

/* 1.4 Apis */
Return<void> TestRadioIndicationBase_1_4::currentPhysicalChannelConfigs_1_4(
        RadioIndicationType /*type*/,
        const ::android::hardware::hidl_vec<
                ::android::hardware::radio::V1_4::PhysicalChannelConfig>& /*configs*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_4::networkScanResult_1_4(
        RadioIndicationType /*type*/,
        const ::android::hardware::radio::V1_4::NetworkScanResult& /*result*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_4::cellInfoList_1_4(
        RadioIndicationType /*type*/,
        const ::android::hardware::hidl_vec<
                ::android::hardware::radio::V1_4::CellInfo>& /*records*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_4::currentEmergencyNumberList(
        RadioIndicationType /*type*/,
        const ::android::hardware::hidl_vec<EmergencyNumber>& /*emergencyNumberList*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_4::dataCallListChanged_1_4(
        RadioIndicationType /*type*/,
        const ::android::hardware::hidl_vec<android::hardware::radio::V1_4::SetupDataCallResult>&
        /*dcList*/) {
    return Void();
}

/* 1.2 Apis */
Return<void> TestRadioIndicationBase_1_4::networkScanResult_1_2(
        RadioIndicationType /*type*/,
        const ::android::hardware::radio::V1_2::NetworkScanResult& /*result*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_4::cellInfoList_1_2(
        RadioIndicationType /*type*/,
        const ::android::hardware::hidl_vec<
                ::android::hardware::radio::V1_2::CellInfo>& /*records*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_4::currentLinkCapacityEstimate(
        RadioIndicationType /*type*/,
        const ::android::hardware::radio::V1_2::LinkCapacityEstimate& /*lce*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_4::currentPhysicalChannelConfigs(
        RadioIndicationType /*type*/,
        const ::android::hardware::hidl_vec<
                ::android::hardware::radio::V1_2::PhysicalChannelConfig>& /*configs*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_4::currentSignalStrength_1_2(
        RadioIndicationType /*type*/,
        const ::android::hardware::radio::V1_2::SignalStrength& /*signalStrength*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_4::currentSignalStrength_1_4(
        RadioIndicationType /*type*/,
        const ::android::hardware::radio::V1_4::SignalStrength& /*signalStrength*/) {
    return Void();
}

/* 1.1 Apis */
Return<void> TestRadioIndicationBase_1_4::carrierInfoForImsiEncryption(RadioIndicationType /*info*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_4::networkScanResult(
        RadioIndicationType /*type*/,
        const ::android::hardware::radio::V1_1::NetworkScanResult& /*result*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_4::keepaliveStatus(RadioIndicationType /*type*/,
                                                   const KeepaliveStatus& /*status*/) {
    return Void();
}

/* 1.0 Apis */
Return<void> TestRadioIndicationBase_1_4::radioStateChanged(RadioIndicationType /*type*/,
                                                     RadioState /*radioState*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_4::callStateChanged(RadioIndicationType /*type*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_4::networkStateChanged(RadioIndicationType /*type*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_4::newSms(RadioIndicationType /*type*/,
                                          const ::android::hardware::hidl_vec<uint8_t>& /*pdu*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_4::newSmsStatusReport(
        RadioIndicationType /*type*/, const ::android::hardware::hidl_vec<uint8_t>& /*pdu*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_4::newSmsOnSim(RadioIndicationType /*type*/,
                                               int32_t /*recordNumber*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_4::onUssd(RadioIndicationType /*type*/, UssdModeType /*modeType*/,
                                          const ::android::hardware::hidl_string& /*msg*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_4::nitzTimeReceived(
        RadioIndicationType /*type*/, const ::android::hardware::hidl_string& /*nitzTime*/,
        uint64_t /*receivedTime*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_4::currentSignalStrength(
        RadioIndicationType /*type*/,
        const ::android::hardware::radio::V1_0::SignalStrength& /*signalStrength*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_4::dataCallListChanged(
        RadioIndicationType /*type*/,
        const ::android::hardware::hidl_vec<android::hardware::radio::V1_0::SetupDataCallResult>&
        /*dcList*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_4::suppSvcNotify(RadioIndicationType /*type*/,
                                                 const SuppSvcNotification& /*suppSvc*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_4::stkSessionEnd(RadioIndicationType /*type*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_4::stkProactiveCommand(
        RadioIndicationType /*type*/, const ::android::hardware::hidl_string& /*cmd*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_4::stkEventNotify(RadioIndicationType /*type*/,
                                                  const ::android::hardware::hidl_string& /*cmd*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_4::stkCallSetup(RadioIndicationType /*type*/, int64_t /*timeout*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_4::simSmsStorageFull(RadioIndicationType /*type*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_4::simRefresh(RadioIndicationType /*type*/,
                                              const SimRefreshResult& /*refreshResult*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_4::callRing(RadioIndicationType /*type*/, bool /*isGsm*/,
                                            const CdmaSignalInfoRecord& /*record*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_4::simStatusChanged(RadioIndicationType /*type*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_4::cdmaNewSms(RadioIndicationType /*type*/,
                                              const CdmaSmsMessage& /*msg*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_4::newBroadcastSms(
        RadioIndicationType /*type*/, const ::android::hardware::hidl_vec<uint8_t>& /*data*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_4::cdmaRuimSmsStorageFull(RadioIndicationType /*type*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_4::restrictedStateChanged(RadioIndicationType /*type*/,
                                                          PhoneRestrictedState /*state*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_4::enterEmergencyCallbackMode(RadioIndicationType /*type*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_4::cdmaCallWaiting(RadioIndicationType /*type*/,
                                                   const CdmaCallWaiting& /*callWaitingRecord*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_4::cdmaOtaProvisionStatus(RadioIndicationType /*type*/,
                                                          CdmaOtaProvisionStatus /*status*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_4::cdmaInfoRec(RadioIndicationType /*type*/,
                                               const CdmaInformationRecords& /*records*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_4::indicateRingbackTone(RadioIndicationType /*type*/,
                                                        bool /*start*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_4::resendIncallMute(RadioIndicationType /*type*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_4::cdmaSubscriptionSourceChanged(
        RadioIndicationType /*type*/, CdmaSubscriptionSource /*cdmaSource*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_4::cdmaPrlChanged(RadioIndicationType /*type*/,
                                                  int32_t /*version*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_4::exitEmergencyCallbackMode(RadioIndicationType /*type*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_4::rilConnected(RadioIndicationType /*type*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_4::voiceRadioTechChanged(
        RadioIndicationType /*type*/, ::android::hardware::radio::V1_0::RadioTechnology /*rat*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_4::cellInfoList(
        RadioIndicationType /*type*/,
        const ::android::hardware::hidl_vec<
                ::android::hardware::radio::V1_0::CellInfo>& /*records*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_4::imsNetworkStateChanged(RadioIndicationType /*type*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_4::subscriptionStatusChanged(RadioIndicationType /*type*/,
                                                             bool /*activate*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_4::srvccStateNotify(RadioIndicationType /*type*/,
                                                    SrvccState /*state*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_4::hardwareConfigChanged(
        RadioIndicationType /*type*/,
        const ::android::hardware::hidl_vec<HardwareConfig>& /*configs*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_4::radioCapabilityIndication(
        RadioIndicationType /*type*/,
        const android::hardware::radio::V1_0::RadioCapability& /*rc*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_4::onSupplementaryServiceIndication(
        RadioIndicationType /*type*/, const StkCcUnsolSsResult& /*ss*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_4::stkCallControlAlphaNotify(
        RadioIndicationType /*type*/, const ::android::hardware::hidl_string& /*alpha*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_4::lceData(RadioIndicationType /*type*/,
                                           const LceDataInfo& /*lce*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_4::pcoData(RadioIndicationType /*type*/,
                                           const PcoDataInfo& /*pco*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_4::modemReset(RadioIndicationType /*type*/,
                                              const ::android::hardware::hidl_string& /*reason*/) {
    return Void();
}
