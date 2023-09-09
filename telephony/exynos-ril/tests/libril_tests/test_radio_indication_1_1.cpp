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
 * test_radio_indication_1_1.cpp
 *
 *  Created on: 2021. 8. 5.
 */
#include "test_radio_hidl_utils_1_1.h"

/* 1.1 Apis */
Return<void> TestRadioIndicationBase_1_1::carrierInfoForImsiEncryption(RadioIndicationType /*info*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_1::networkScanResult(
        RadioIndicationType /*type*/,
        const ::android::hardware::radio::V1_1::NetworkScanResult& /*result*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_1::keepaliveStatus(RadioIndicationType /*type*/,
                                                   const KeepaliveStatus& /*status*/) {
    return Void();
}

/* 1.0 Apis */
Return<void> TestRadioIndicationBase_1_1::radioStateChanged(RadioIndicationType /*type*/,
                                                     RadioState /*radioState*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_1::callStateChanged(RadioIndicationType /*type*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_1::networkStateChanged(RadioIndicationType /*type*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_1::newSms(RadioIndicationType /*type*/,
                                          const ::android::hardware::hidl_vec<uint8_t>& /*pdu*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_1::newSmsStatusReport(
        RadioIndicationType /*type*/, const ::android::hardware::hidl_vec<uint8_t>& /*pdu*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_1::newSmsOnSim(RadioIndicationType /*type*/,
                                               int32_t /*recordNumber*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_1::onUssd(RadioIndicationType /*type*/, UssdModeType /*modeType*/,
                                          const ::android::hardware::hidl_string& /*msg*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_1::nitzTimeReceived(
        RadioIndicationType /*type*/, const ::android::hardware::hidl_string& /*nitzTime*/,
        uint64_t /*receivedTime*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_1::currentSignalStrength(
        RadioIndicationType /*type*/,
        const ::android::hardware::radio::V1_0::SignalStrength& /*signalStrength*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_1::dataCallListChanged(
        RadioIndicationType /*type*/,
        const ::android::hardware::hidl_vec<android::hardware::radio::V1_0::SetupDataCallResult>&
        /*dcList*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_1::suppSvcNotify(RadioIndicationType /*type*/,
                                                 const SuppSvcNotification& /*suppSvc*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_1::stkSessionEnd(RadioIndicationType /*type*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_1::stkProactiveCommand(
        RadioIndicationType /*type*/, const ::android::hardware::hidl_string& /*cmd*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_1::stkEventNotify(RadioIndicationType /*type*/,
                                                  const ::android::hardware::hidl_string& /*cmd*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_1::stkCallSetup(RadioIndicationType /*type*/, int64_t /*timeout*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_1::simSmsStorageFull(RadioIndicationType /*type*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_1::simRefresh(RadioIndicationType /*type*/,
                                              const SimRefreshResult& /*refreshResult*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_1::callRing(RadioIndicationType /*type*/, bool /*isGsm*/,
                                            const CdmaSignalInfoRecord& /*record*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_1::simStatusChanged(RadioIndicationType /*type*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_1::cdmaNewSms(RadioIndicationType /*type*/,
                                              const CdmaSmsMessage& /*msg*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_1::newBroadcastSms(
        RadioIndicationType /*type*/, const ::android::hardware::hidl_vec<uint8_t>& /*data*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_1::cdmaRuimSmsStorageFull(RadioIndicationType /*type*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_1::restrictedStateChanged(RadioIndicationType /*type*/,
                                                          PhoneRestrictedState /*state*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_1::enterEmergencyCallbackMode(RadioIndicationType /*type*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_1::cdmaCallWaiting(RadioIndicationType /*type*/,
                                                   const CdmaCallWaiting& /*callWaitingRecord*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_1::cdmaOtaProvisionStatus(RadioIndicationType /*type*/,
                                                          CdmaOtaProvisionStatus /*status*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_1::cdmaInfoRec(RadioIndicationType /*type*/,
                                               const CdmaInformationRecords& /*records*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_1::indicateRingbackTone(RadioIndicationType /*type*/,
                                                        bool /*start*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_1::resendIncallMute(RadioIndicationType /*type*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_1::cdmaSubscriptionSourceChanged(
        RadioIndicationType /*type*/, CdmaSubscriptionSource /*cdmaSource*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_1::cdmaPrlChanged(RadioIndicationType /*type*/,
                                                  int32_t /*version*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_1::exitEmergencyCallbackMode(RadioIndicationType /*type*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_1::rilConnected(RadioIndicationType /*type*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_1::voiceRadioTechChanged(
        RadioIndicationType /*type*/, ::android::hardware::radio::V1_0::RadioTechnology /*rat*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_1::cellInfoList(
        RadioIndicationType /*type*/,
        const ::android::hardware::hidl_vec<
                ::android::hardware::radio::V1_0::CellInfo>& /*records*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_1::imsNetworkStateChanged(RadioIndicationType /*type*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_1::subscriptionStatusChanged(RadioIndicationType /*type*/,
                                                             bool /*activate*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_1::srvccStateNotify(RadioIndicationType /*type*/,
                                                    SrvccState /*state*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_1::hardwareConfigChanged(
        RadioIndicationType /*type*/,
        const ::android::hardware::hidl_vec<HardwareConfig>& /*configs*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_1::radioCapabilityIndication(
        RadioIndicationType /*type*/,
        const android::hardware::radio::V1_0::RadioCapability& /*rc*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_1::onSupplementaryServiceIndication(
        RadioIndicationType /*type*/, const StkCcUnsolSsResult& /*ss*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_1::stkCallControlAlphaNotify(
        RadioIndicationType /*type*/, const ::android::hardware::hidl_string& /*alpha*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_1::lceData(RadioIndicationType /*type*/,
                                           const LceDataInfo& /*lce*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_1::pcoData(RadioIndicationType /*type*/,
                                           const PcoDataInfo& /*pco*/) {
    return Void();
}

Return<void> TestRadioIndicationBase_1_1::modemReset(RadioIndicationType /*type*/,
                                              const ::android::hardware::hidl_string& /*reason*/) {
    return Void();
}
