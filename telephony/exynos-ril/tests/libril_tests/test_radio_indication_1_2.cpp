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
 * test_radio_indication_1_2.cpp
 *
 *  Created on: 2021. 8. 5.
 */
#include "test_radio_hidl_utils_1_2.h"

/* 1.2 Apis */
Return<void> TestRadioIndicationBase_1_2::networkScanResult_1_2(
        RadioIndicationType /*type*/,
        const ::android::hardware::radio::V1_2::NetworkScanResult& /*result*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_2::cellInfoList_1_2(
        RadioIndicationType /*type*/,
        const ::android::hardware::hidl_vec<
                ::android::hardware::radio::V1_2::CellInfo>& /*records*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_2::currentLinkCapacityEstimate(
        RadioIndicationType /*type*/,
        const ::android::hardware::radio::V1_2::LinkCapacityEstimate& /*lce*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_2::currentPhysicalChannelConfigs(
        RadioIndicationType /*type*/,
        const ::android::hardware::hidl_vec<
                ::android::hardware::radio::V1_2::PhysicalChannelConfig>& /*configs*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_2::currentSignalStrength_1_2(
        RadioIndicationType /*type*/,
        const ::android::hardware::radio::V1_2::SignalStrength& /*signalStrength*/) {
    return Void();
}

/* 1.1 Apis */
Return<void> TestRadioIndicationBase_1_2::carrierInfoForImsiEncryption(RadioIndicationType /*info*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_2::networkScanResult(
        RadioIndicationType /*type*/,
        const ::android::hardware::radio::V1_1::NetworkScanResult& /*result*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_2::keepaliveStatus(RadioIndicationType /*type*/,
                                                   const KeepaliveStatus& /*status*/) {
    return Void();
}

/* 1.0 Apis */
Return<void> TestRadioIndicationBase_1_2::radioStateChanged(RadioIndicationType /*type*/,
                                                     RadioState /*radioState*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_2::callStateChanged(RadioIndicationType /*type*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_2::networkStateChanged(RadioIndicationType /*type*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_2::newSms(RadioIndicationType /*type*/,
                                          const ::android::hardware::hidl_vec<uint8_t>& /*pdu*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_2::newSmsStatusReport(
        RadioIndicationType /*type*/, const ::android::hardware::hidl_vec<uint8_t>& /*pdu*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_2::newSmsOnSim(RadioIndicationType /*type*/,
                                               int32_t /*recordNumber*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_2::onUssd(RadioIndicationType /*type*/, UssdModeType /*modeType*/,
                                          const ::android::hardware::hidl_string& /*msg*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_2::nitzTimeReceived(
        RadioIndicationType /*type*/, const ::android::hardware::hidl_string& /*nitzTime*/,
        uint64_t /*receivedTime*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_2::currentSignalStrength(
        RadioIndicationType /*type*/,
        const ::android::hardware::radio::V1_0::SignalStrength& /*signalStrength*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_2::dataCallListChanged(
        RadioIndicationType /*type*/,
        const ::android::hardware::hidl_vec<android::hardware::radio::V1_0::SetupDataCallResult>&
        /*dcList*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_2::suppSvcNotify(RadioIndicationType /*type*/,
                                                 const SuppSvcNotification& /*suppSvc*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_2::stkSessionEnd(RadioIndicationType /*type*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_2::stkProactiveCommand(
        RadioIndicationType /*type*/, const ::android::hardware::hidl_string& /*cmd*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_2::stkEventNotify(RadioIndicationType /*type*/,
                                                  const ::android::hardware::hidl_string& /*cmd*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_2::stkCallSetup(RadioIndicationType /*type*/, int64_t /*timeout*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_2::simSmsStorageFull(RadioIndicationType /*type*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_2::simRefresh(RadioIndicationType /*type*/,
                                              const SimRefreshResult& /*refreshResult*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_2::callRing(RadioIndicationType /*type*/, bool /*isGsm*/,
                                            const CdmaSignalInfoRecord& /*record*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_2::simStatusChanged(RadioIndicationType /*type*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_2::cdmaNewSms(RadioIndicationType /*type*/,
                                              const CdmaSmsMessage& /*msg*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_2::newBroadcastSms(
        RadioIndicationType /*type*/, const ::android::hardware::hidl_vec<uint8_t>& /*data*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_2::cdmaRuimSmsStorageFull(RadioIndicationType /*type*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_2::restrictedStateChanged(RadioIndicationType /*type*/,
                                                          PhoneRestrictedState /*state*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_2::enterEmergencyCallbackMode(RadioIndicationType /*type*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_2::cdmaCallWaiting(RadioIndicationType /*type*/,
                                                   const CdmaCallWaiting& /*callWaitingRecord*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_2::cdmaOtaProvisionStatus(RadioIndicationType /*type*/,
                                                          CdmaOtaProvisionStatus /*status*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_2::cdmaInfoRec(RadioIndicationType /*type*/,
                                               const CdmaInformationRecords& /*records*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_2::indicateRingbackTone(RadioIndicationType /*type*/,
                                                        bool /*start*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_2::resendIncallMute(RadioIndicationType /*type*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_2::cdmaSubscriptionSourceChanged(
        RadioIndicationType /*type*/, CdmaSubscriptionSource /*cdmaSource*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_2::cdmaPrlChanged(RadioIndicationType /*type*/,
                                                  int32_t /*version*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_2::exitEmergencyCallbackMode(RadioIndicationType /*type*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_2::rilConnected(RadioIndicationType /*type*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_2::voiceRadioTechChanged(
        RadioIndicationType /*type*/, ::android::hardware::radio::V1_0::RadioTechnology /*rat*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_2::cellInfoList(
        RadioIndicationType /*type*/,
        const ::android::hardware::hidl_vec<
                ::android::hardware::radio::V1_0::CellInfo>& /*records*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_2::imsNetworkStateChanged(RadioIndicationType /*type*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_2::subscriptionStatusChanged(RadioIndicationType /*type*/,
                                                             bool /*activate*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_2::srvccStateNotify(RadioIndicationType /*type*/,
                                                    SrvccState /*state*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_2::hardwareConfigChanged(
        RadioIndicationType /*type*/,
        const ::android::hardware::hidl_vec<HardwareConfig>& /*configs*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_2::radioCapabilityIndication(
        RadioIndicationType /*type*/,
        const android::hardware::radio::V1_0::RadioCapability& /*rc*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_2::onSupplementaryServiceIndication(
        RadioIndicationType /*type*/, const StkCcUnsolSsResult& /*ss*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_2::stkCallControlAlphaNotify(
        RadioIndicationType /*type*/, const ::android::hardware::hidl_string& /*alpha*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_2::lceData(RadioIndicationType /*type*/,
                                           const LceDataInfo& /*lce*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_2::pcoData(RadioIndicationType /*type*/,
                                           const PcoDataInfo& /*pco*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_2::modemReset(RadioIndicationType /*type*/,
                                              const ::android::hardware::hidl_string& /*reason*/) {
    return Void();
}
