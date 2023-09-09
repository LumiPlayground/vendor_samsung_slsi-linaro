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
 * ril_oem_service.cpp
 *
 *  Created on: 2021. 5. 12.
 */
#define LOG_TAG "RILC_OEM"

#include <ril_service_1_6.h>

#include <vendor/samsung_slsi/telephony/hardware/radio/1.2/IOemSamsungslsi.h>
#include <vendor/samsung_slsi/telephony/hardware/radio/1.2/IOemSamsungslsiResponse.h>
#include <vendor/samsung_slsi/telephony/hardware/radio/1.2/IOemSamsungslsiIndication.h>

#include "ril_oem_service.h"
#include "ril_service_common.h"
#include "ril_service_legacy.h"

#include <telephony/ril.h>
#include <telephony/ril_mnc.h>
#include <telephony/ril_ext.h>
#include <hidl/HidlTransportSupport.h>
#include <inttypes.h>

#include <librilutils/vendor.h>
#include <librilutils/radio_conv_utils.h>
#include <librilutils/radio_hal_utils.h>
#include <librilutils/radio_oemhal_utils_1_2.h>

using namespace vendor::radio::impl::legacy;

using namespace android::hardware::radio;
using namespace android::hardware::radio::V1_0;
using namespace vendor::samsung_slsi::telephony::hardware::radio::V1_0;
using ::android::hardware::configureRpcThreadpool;
using ::android::hardware::joinRpcThreadpool;
using ::android::hardware::Return;
using ::android::hardware::hidl_bitfield;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::hidl_array;
using ::android::hardware::Void;
using android::CommandInfo;
using android::UnsolResponseInfo;
using android::RequestInfo;
using android::requestToString;
using android::sp;

namespace OEM_V1_2 =  vendor::samsung_slsi::telephony::hardware::radio::V1_2;

namespace vendor {
namespace radio {
namespace impl {
namespace legacy {

struct OemRadioImpl;

#if (SIM_COUNT >= 2)
sp<OemRadioImpl> radioService[SIM_COUNT];
int64_t nitzTimeReceived[SIM_COUNT];
// counter used for synchronization. It is incremented every time response callbacks are updated.
volatile int32_t mCounterRadio[SIM_COUNT];
#else
sp<OemRadioImpl> radioService[1];
// counter used for synchronization. It is incremented every time response callbacks are updated.
volatile int32_t mCounterRadio[1];
#endif

/**
 * OemRadioImpl
 *
 * Samsung S.LSI specific interfaces
 */
#define CURRENT_RADIO_HAL "vendor::samsung_slsi::telephony::hardware::radio::V1_2::IOemSamsungslsi"
struct OemRadioImpl : public vendor::samsung_slsi::telephony::hardware::radio::V1_2::IOemSamsungslsi {
    int32_t mSlotId;
    sp<IOemSamsungslsiResponse> mOemResponse;
    sp<IOemSamsungslsiIndication> mOemIndication;
    sp<vendor::samsung_slsi::telephony::hardware::radio::V1_1::IOemSamsungslsiResponse> mOemResponseV1_1;
    sp<vendor::samsung_slsi::telephony::hardware::radio::V1_1::IOemSamsungslsiIndication> mOemIndicationV1_1;

    sp<vendor::samsung_slsi::telephony::hardware::radio::V1_2::IOemSamsungslsiResponse> mOemResponseV1_2;
    sp<vendor::samsung_slsi::telephony::hardware::radio::V1_2::IOemSamsungslsiIndication> mOemIndicationV1_2;

    void checkReturnStatus(Return<void>& ret);

    Return<void> setResponseFunctions(
            const ::android::sp<IOemSamsungslsiResponse>& oemResponse,
            const ::android::sp<IOemSamsungslsiIndication>& oemIndication);

    Return<void> sendRequestRaw(int32_t serial,
            const ::android::hardware::hidl_vec<uint8_t>& data);

    Return<void> queryCOLP(int32_t serial);
    Return<void> queryCOLR(int32_t serial);
    Return<void> iccGetAtr(int32_t serial);
    Return<void> sendUSSDWithDcs(int32_t serial, const hidl_string& dcs, const hidl_string& ussd);
    Return<void> setPreferredUplmn(int32_t serial, int32_t networkType, int32_t indMode, int32_t index, const hidl_string& plmn);
    Return<void> getPreferredUplmn(int32_t serial);
    Return<void> setEmcStatus(int32_t serial, int32_t status, int32_t rat);
    Return<void> setFemtoCellSearch(int32_t serial, int32_t searchMode);
    Return<void> setCdmaHybridMode(int32_t serial, int32_t hybridMode);
    Return<void> getCdmaHybridMode(int32_t serial);
    Return<void> queryBplmnSearch(int32_t serial);
    Return<void> setNetworkSelectionModeManual(int32_t serial, const hidl_string& operatorNumeric, const hidl_string& rat);
    Return<void> dialWithCallType(int32_t serial, const Dial& dialInfo, int callType);
    Return<void> setVoiceOperation(int32_t serial, int32_t prefMode);
    Return<void> setBarringPasswordOverMmi(int32_t serial, const hidl_string& facility, const hidl_string& oldPassword, const hidl_string& newPassword, const hidl_string& newPasswordAgain);
    Return<void> setDualNetworkTypeAndAllowData(int32_t serial, int32_t networkTypeForPrimary, int32_t networkTypeForSecondary, int32_t allowDataForPrimary, int32_t allowDataForSecondary);
    Return<void> setDsNetworkType(int32_t serial, int32_t networkType);
    Return<void> deactivateDataCallWithReason(int32_t serial, int32_t cid, int32_t reason);
    Return<void> emulateInd(int32_t serial, int32_t msgid, int32_t option);
    Return<void> getSimLockStatus(int32_t serial);
    Return<void> sendVsimNotification(int32_t serial, int32_t transactionId, int32_t eventId, int32_t simType);
    Return<void> sendVsimOperation(int32_t serial, int32_t transactionId, int32_t eventId, int32_t result, const hidl_string& data);
    Return<void> setActivateVsim(int32_t serial, int32_t slot, const hidl_string& iccid, const hidl_string& imsi, const hidl_string& hplmn, int32_t vsimState, int32_t vsimCardType);
    Return<void> setEndcMode(int32_t serial, bool enable);
    Return<void> getEndcMode(int32_t serial);
    Return<void> getSmsStorageOnSim(int32_t serial, int32_t simId);
    Return<void> setNrMode(int32_t serial, int32_t mode);
    Return<void> getNrMode(int32_t serial);

    void generalResponse(int serial, RIL_Errno e);

    /* 1.2 */
    /* Modified version from Radio HAL 1.6 */
    Return<void> setupDataCall_1_6(int32_t serial, V1_5::AccessNetwork accessNetwork,
                                   const V1_5::DataProfileInfo& dataProfileInfo, bool roamingAllowed,
                                   V1_2::DataRequestReason reason, const hidl_vec<V1_5::LinkAddress>& addresses,
                                   const hidl_vec<hidl_string>& dnses, int32_t pduSessionId, const vendor::samsung_slsi::telephony::hardware::radio::V1_2::OptionalSliceInfo& sliceInfo,
                                   const vendor::samsung_slsi::telephony::hardware::radio::V1_2::OptionalTrafficDescriptor& trafficDescriptor, bool matchAllRuleAllowed,
                                   const vendor::samsung_slsi::telephony::hardware::radio::V1_2::OptionalVerifyDescriptor& verifyDescriptor);
    Return<void> getSlicingConfig(int32_t serial);
};

void checkReturnStatus(int32_t slotId, Return<void>& ret) {
    if (ret.isOk() == false) {
        RLOGE("%s: unable to call response/indication callback", __FUNCTION__);
        // Remote process hosting the callbacks must be dead. Reset the callback objects;
        // there's no other recovery to be done here. When the client process is back up, it will
        // call setResponseFunctions()

        // Caller should already hold rdlock, release that first
        // note the current counter to avoid overwriting updates made by another thread before
        // write lock is acquired.
        int counter = mCounterRadio[slotId];
        pthread_rwlock_t *radioServiceRwlockPtr = ::radio::getRadioServiceRwlock(slotId);
        int ret = pthread_rwlock_unlock(radioServiceRwlockPtr);
        assert(ret == 0);

        // acquire wrlock
        ret = pthread_rwlock_wrlock(radioServiceRwlockPtr);
        assert(ret == 0);

        // make sure the counter value has not changed
        if (counter == mCounterRadio[slotId]) {
            radioService[slotId]->mOemResponse = NULL;
            radioService[slotId]->mOemIndication = NULL;
            mCounterRadio[slotId]++;
        } else {
            RLOGE("%s: not resetting responseFunctions as they likely "
                    "got updated on another thread", __FUNCTION__);
        }

        // release wrlock
        ret = pthread_rwlock_unlock(radioServiceRwlockPtr);
        assert(ret == 0);

        // Reacquire rdlock
        ret = pthread_rwlock_rdlock(radioServiceRwlockPtr);
        assert(ret == 0);
    }
}

void OemRadioImpl::checkReturnStatus(Return<void>& ret) {
    vendor::radio::impl::legacy::checkReturnStatus(mSlotId, ret);
}

Return<void> OemRadioImpl::sendRequestRaw(int32_t serial, const hidl_vec<uint8_t>& data) {
#if VDBG
    RLOGD("OemRadioImpl::%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchRaw(serial, mSlotId, RIL_REQUEST_OEM_HOOK_RAW, data);
    return Void();
}

Return<void> OemRadioImpl::setResponseFunctions(
        const ::android::sp<IOemSamsungslsiResponse>& oemResponseParam,
        const ::android::sp<IOemSamsungslsiIndication>& oemIndicationParam) {
#if VDBG
    RLOGD("OemRadioImpl::%s", __FUNCTION__);
#endif

    pthread_rwlock_t *radioServiceRwlockPtr = ::radio::getRadioServiceRwlock(mSlotId);
    int ret = pthread_rwlock_wrlock(radioServiceRwlockPtr);
    assert(ret == 0);

    mOemResponse = oemResponseParam;
    mOemIndication = oemIndicationParam;

    mOemResponseV1_1 = vendor::samsung_slsi::telephony::hardware::radio::V1_1::IOemSamsungslsiResponse::castFrom(mOemResponse).withDefault(nullptr);
    mOemIndicationV1_1 = vendor::samsung_slsi::telephony::hardware::radio::V1_1::IOemSamsungslsiIndication::castFrom(mOemIndication).withDefault(nullptr);
    if (mOemResponseV1_1 == nullptr || mOemIndicationV1_1 == nullptr) {
        mOemResponseV1_1 = nullptr;
        mOemIndicationV1_1 = nullptr;
    }
    else {
        RLOGD("OemRadioImpl::%s available for @1.1", __FUNCTION__);
    }

    mOemResponseV1_2 = vendor::samsung_slsi::telephony::hardware::radio::V1_2::IOemSamsungslsiResponse::castFrom(mOemResponse).withDefault(nullptr);
    mOemIndicationV1_2 = vendor::samsung_slsi::telephony::hardware::radio::V1_2::IOemSamsungslsiIndication::castFrom(mOemIndication).withDefault(nullptr);
    if (mOemIndicationV1_2 != nullptr && mOemResponseV1_2 != nullptr) {
        RLOGD("OemRadioImpl::%s available for @1.2", __FUNCTION__);
    } else {
        mOemResponseV1_2 = nullptr;
        mOemIndicationV1_2 = nullptr;
    }

    mCounterRadio[mSlotId]++;

    ret = pthread_rwlock_unlock(radioServiceRwlockPtr);
    assert(ret == 0);

    return Void();
}

Return<void> OemRadioImpl::queryCOLP(int32_t serial) {
    RLOGD("OemRadioImpl::%s: ", __FUNCTION__);
    dispatchVoid(serial, mSlotId, RIL_REQUEST_QUERY_COLP);
    return Void();
}
Return<void> OemRadioImpl::queryCOLR(int32_t serial) {
    RLOGD("OemRadioImpl::%s: ", __FUNCTION__);
    dispatchVoid(serial, mSlotId, RIL_REQUEST_QUERY_COLR);
    return Void();
}
Return<void> OemRadioImpl::iccGetAtr(int32_t serial) {
    RLOGD("OemRadioImpl::%s: ", __FUNCTION__);
    dispatchVoid(serial, mSlotId, RIL_REQUEST_SIM_GET_ATR);
    return Void();
}
Return<void> OemRadioImpl::sendUSSDWithDcs(int32_t serial, const hidl_string& dcs, const hidl_string& ussd) {
    RLOGD("OemRadioImpl::%s: ", __FUNCTION__);
    dispatchStrings(serial, mSlotId, RIL_REQUEST_SEND_ENCODED_USSD, true, 2, dcs.c_str(), ussd.c_str());
    return Void();
}

Return<void> OemRadioImpl::setPreferredUplmn(int32_t serial, int32_t networkType, int32_t indMode, int32_t index, const hidl_string& plmn) {
    RLOGD("OemRadioImpl::%s: serial %d networkType %d indMode %d index %d plmn %s", __FUNCTION__, serial, networkType, indMode, index, plmn.c_str());
    // Not support since S version
    RequestInfo *pRI = android::addRequestToList(serial, mSlotId, RIL_REQUEST_SET_UPLMN);
    if (pRI != NULL) {
        sendErrorResponse(pRI, RIL_E_REQUEST_NOT_SUPPORTED);
    }
    return Void();
}

Return<void> OemRadioImpl::getPreferredUplmn(int32_t serial) {
    RLOGD("OemRadioImpl::%s: ", __FUNCTION__);
    // Not support since S version
    RequestInfo *pRI = android::addRequestToList(serial, mSlotId, RIL_REQUEST_GET_UPLMN);
    if (pRI != NULL) {
        sendErrorResponse(pRI, RIL_E_REQUEST_NOT_SUPPORTED);
    }
    return Void();
}
Return<void> OemRadioImpl::setEmcStatus(int32_t serial, int32_t status, int32_t rat) {
    RLOGD("OemRadioImpl::%s: ", __FUNCTION__);
    dispatchInts(serial, mSlotId, RIL_REQUEST_SET_EMERGENCY_CALL_STATUS, 2, status, rat);
    return Void();
}
Return<void> OemRadioImpl::setFemtoCellSearch(int32_t serial, int32_t searchMode) {
    RLOGD("OemRadioImpl::%s: ", __FUNCTION__);
    dispatchInts(serial, mSlotId, RIL_REQUEST_SET_FEMTO_CELL_SRCH, 1, searchMode);
    return Void();
}
Return<void> OemRadioImpl::setCdmaHybridMode(int32_t serial, int32_t hybridMode) {
    RLOGD("OemRadioImpl::%s: ", __FUNCTION__);
    dispatchInts(serial, mSlotId, RIL_REQUEST_SET_CDMA_HYBRID_MODE, 1, hybridMode);
    return Void();
}
Return<void> OemRadioImpl::getCdmaHybridMode(int32_t serial) {
    RLOGD("OemRadioImpl::%s: ", __FUNCTION__);
    dispatchVoid(serial, mSlotId, RIL_REQUEST_GET_CDMA_HYBRID_MODE);
    return Void();
}
Return<void> OemRadioImpl::queryBplmnSearch(int32_t serial) {
    RLOGD("OemRadioImpl::%s: ", __FUNCTION__);
    dispatchVoid(serial, mSlotId, RIL_REQUEST_QUERY_BPLMN_SEARCH);
    return Void();
}
Return<void> OemRadioImpl::setNetworkSelectionModeManual(int32_t serial,
                                                     const hidl_string& operatorNumeric, const hidl_string& rat) {
#if VDBG
    RLOGD("OemRadioImpl::%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchStrings(serial, mSlotId, RIL_REQUEST_SET_NETWORK_SELECTION_MANUAL_WITH_RAT,
            false, 2, operatorNumeric.c_str(), rat.c_str());
    return Void();
}
Return<void> OemRadioImpl::dialWithCallType(int32_t serial, const Dial& dialInfo, int32_t callType) {
    RLOGD("OemRadioImpl::%s: serial %d", __FUNCTION__, serial);
    RequestInfo *pRI = android::addRequestToList(serial, mSlotId, RIL_REQUEST_DIAL_WITH_CALL_TYPE);
    if (pRI == NULL) {
        return Void();
    }
    RIL_Dial_Ext dial = {};
    RIL_UUS_Info uusInfo = {};
    int32_t sizeOfDial = sizeof(dial);

    if (!copyHidlStringToRil(&dial.address, dialInfo.address, pRI)) {
        return Void();
    }
    dial.clir = (int) dialInfo.clir;
    dial.callType = callType;

    if (dialInfo.uusInfo.size() != 0) {
        uusInfo.uusType = (RIL_UUS_Type) dialInfo.uusInfo[0].uusType;
        uusInfo.uusDcs = (RIL_UUS_DCS) dialInfo.uusInfo[0].uusDcs;

        if (dialInfo.uusInfo[0].uusData.size() == 0) {
            uusInfo.uusData = NULL;
            uusInfo.uusLength = 0;
        } else {
            if (!copyHidlStringToRil(&uusInfo.uusData, dialInfo.uusInfo[0].uusData, pRI)) {
                memsetAndFreeStrings(1, dial.address);
                return Void();
            }
            uusInfo.uusLength = dialInfo.uusInfo[0].uusData.size();
        }

        dial.uusInfo = &uusInfo;
    }

    CALL_ONREQUEST(RIL_REQUEST_DIAL_WITH_CALL_TYPE, &dial, sizeOfDial, pRI, mSlotId);

    memsetAndFreeStrings(2, dial.address, uusInfo.uusData);

    return Void();
}
Return<void> OemRadioImpl::setVoiceOperation(int32_t serial, int32_t prefMode) {
    RLOGD("OemRadioImpl::%s: prefMode %d", __FUNCTION__, prefMode);
    dispatchInts(serial, mSlotId, RIL_REQUEST_SET_VOICE_OPERATION, 1, prefMode);
    return Void();
}

Return<void> OemRadioImpl::setBarringPasswordOverMmi(int32_t serial, const hidl_string& facility, const hidl_string& oldPassword,
                                                     const hidl_string& newPassword, const hidl_string& newPasswordAgain) {
    RLOGD("OemRadioImpl::%s: serial %d", __FUNCTION__, serial);
    dispatchStrings(serial, mSlotId, RIL_REQUEST_CHANGE_BARRING_PASSWORD_OVER_MMI,
            false, 4, facility.c_str(), oldPassword.c_str(), newPassword.c_str(), newPasswordAgain.c_str());
    return Void();
}

Return<void> OemRadioImpl::setDualNetworkTypeAndAllowData(int32_t serial, int32_t networkTypeForPrimary, int32_t networkTypeForSecondary,
                                                     int32_t allowDataForPrimary, int32_t allowDataForSecondary) {
    RLOGD("OemRadioImpl::%s: serial %d", __FUNCTION__, serial);
    dispatchInts(serial, mSlotId, RIL_REQUEST_SET_DUAL_NETWORK_AND_ALLOW_DATA,
            4, networkTypeForPrimary, networkTypeForSecondary, allowDataForPrimary, allowDataForSecondary);
    return Void();
}

Return<void> OemRadioImpl::setDsNetworkType(int32_t serial, int32_t networkType) {
    RLOGD("OemRadioImpl::%s: serial %d", __FUNCTION__, serial);
    dispatchInts(serial, mSlotId, RIL_REQUEST_SET_DS_NETWORK_TYPE, 1, networkType);
    return Void();
}

Return<void> OemRadioImpl::deactivateDataCallWithReason(int32_t serial, int32_t cid, int32_t reason) {
    RLOGD("OemRadioImpl::%s: serial %d, cid %d, reason %d", __FUNCTION__, serial, cid, reason);
    dispatchStrings(serial, mSlotId, RIL_REQUEST_DEACTIVATE_DATA_CALL_WITH_REASON, false, 2, (std::to_string(cid)).c_str(), (std::to_string(reason)).c_str());
    return Void();
}

Return<void> OemRadioImpl::emulateInd(int32_t serial, int32_t msgid, int32_t option) {
    RLOGD("OemRadioImpl::%s: serial %d, msgid %d, option %d", __FUNCTION__, serial, msgid, option);
    // Not support since S version
    RequestInfo *pRI = android::addRequestToList(serial, mSlotId, RIL_REQUEST_EMULATE_IND);
    if (pRI != NULL) {
        sendErrorResponse(pRI, RIL_E_REQUEST_NOT_SUPPORTED);
    }
    return Void();
}

Return<void> OemRadioImpl::getSimLockStatus(int32_t serial) {
    RLOGD("OemRadioImpl::%s::: serial %d", __FUNCTION__, serial);
    dispatchVoid(serial, mSlotId, RIL_REQUEST_GET_SIM_LOCK_STATUS);
    return Void();
}

Return<void> OemRadioImpl::sendVsimNotification(int32_t serial, int32_t transactionId, int32_t eventId, int32_t simType) {
    RLOGD("OemRadioImpl::%s::: serial %d", __FUNCTION__, serial);
    dispatchInts(serial, mSlotId, RIL_LOCAL_REQUEST_VSIM_NOTIFICATION, 3, transactionId, eventId, simType);
    return Void();
}

Return<void> OemRadioImpl::sendVsimOperation(int32_t serial, int32_t transactionId, int32_t eventId, int32_t result, const hidl_string& data) {
    RLOGD("OemRadioImpl::%s: serial %d", __FUNCTION__, serial);

    RequestInfo *pRI = android::addRequestToList(serial, mSlotId, RIL_LOCAL_REQUEST_VSIM_OPERATION);
    if (pRI == NULL) {
        return Void();
    }

    RIL_VsimOperationEvent event = {};

    event.transaction_id = transactionId;
    event.eventId = eventId;
    event.result = result;
    event.data_length = data.size();

    if (!copyHidlStringToRil(&event.data, data, pRI)) {
        return Void();
    }

    CALL_ONREQUEST(pRI->pCI->requestNumber, &event, sizeof(event), pRI, mSlotId);
    memsetAndFreeStrings(1, event.data);
    return Void();
}

Return<void> OemRadioImpl::setActivateVsim(int32_t serial, int32_t slot,
        const hidl_string& iccid, const hidl_string& imsi, const hidl_string& hplmn,
        int32_t vsimState, int32_t vsimCardType) {
    RLOGD("OemRadioImpl::%s: serial %d", __FUNCTION__, serial);

    RequestInfo *pRI = android::addRequestToList(serial, mSlotId, RIL_REQUEST_SET_ACTIVATE_VSIM);
    if (pRI == NULL) {
        return Void();
    }

    RIL_SetActivateVsim event = {};

    event.simSlot = slot;
    if (!copyHidlStringToRil(&event.iccid, iccid, pRI)) {
        return Void();
    }

    if (!copyHidlStringToRil(&event.imsi, imsi, pRI)) {
        memsetAndFreeStrings(1, event.iccid);
        return Void();
    }

    if (!copyHidlStringToRil(&event.hplmn, hplmn, pRI)) {
        memsetAndFreeStrings(2, event.iccid, event.imsi);
        return Void();
    }

    event.vsimState = vsimState;
    event.vsimCardType = vsimCardType;

    CALL_ONREQUEST(RIL_REQUEST_SET_ACTIVATE_VSIM, &event, sizeof(event), pRI, mSlotId);
    memsetAndFreeStrings(3, event.iccid, event.imsi, event.hplmn);
    return Void();
}

Return<void> OemRadioImpl::setEndcMode(int32_t serial, bool enable) {
    RLOGD("OemRadioImpl::%s: serial %d", __FUNCTION__, serial);
    dispatchInts(serial, mSlotId, RIL_REQUEST_SET_ENDC_MODE, 1, enable ? 1 : 0);
    return Void();
}

Return<void> OemRadioImpl::getEndcMode(int32_t serial) {
    RLOGD("OemRadioImpl::%s: serial %d", __FUNCTION__, serial);
    dispatchVoid(serial, mSlotId, RIL_REQUEST_GET_ENDC_MODE);
    return Void();
}

Return<void> OemRadioImpl::getSmsStorageOnSim(int32_t serial, int32_t simId) {
#if VDBG
    RLOGD("OemRadioImpl::%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchInts(serial, mSlotId, RIL_REQUEST_GET_SMS_STORAGE_ON_SIM, 1, simId);
    return Void();
}

Return<void> OemRadioImpl::setNrMode(int32_t serial, int32_t mode) {
    RLOGD("%s: serial %d", __FUNCTION__, serial);
    dispatchInts(serial, mSlotId, RIL_REQUEST_SET_NR_MODE, 1, mode);
    return Void();
}

Return<void> OemRadioImpl::getNrMode(int32_t serial) {
    RLOGD("%s: serial %d", __FUNCTION__, serial);
    dispatchVoid(serial, mSlotId, RIL_REQUEST_GET_NR_MODE);
    return Void();
}

void OemRadioImpl::generalResponse(int serial, RIL_Errno e) {
    const int slotId = mSlotId;
    if (radioService[slotId]->mOemResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, RESPONSE_SOLICITED, e);

        Return<void> retStatus
                = radioService[slotId]->mOemResponse->generalResponse(responseInfo);
        radioService[slotId]->checkReturnStatus(retStatus);
    } else {
        RLOGE("OemRadioImpl::%s: radioService[%d]->mOemResponse == NULL", __FUNCTION__, slotId);
    }
}

/* 1.6 ::radio */
bool fillSetupDataCallInfo_V1_6_aosp(RIL_SetupDataCallInfo_V1_6 &setupDataCallInfo,
                                    int32_t pduSessionId, const OEM_V1_2::OptionalSliceInfo& sliceInfo,
                                    const OEM_V1_2::OptionalTrafficDescriptor_aosp& trafficDescriptor, bool matchAllRuleAllowed) {
    setupDataCallInfo.pduSessionId = pduSessionId;

    if (sliceInfo.getDiscriminator() != OEM_V1_2::OptionalSliceInfo::hidl_discriminator::noinit) {
        OEM_V1_2::SliceInfo inputInfo = sliceInfo.value();
        setupDataCallInfo.OptionalSliceinfoFlag = RIL_OPT_INFO_VALID;
        setupDataCallInfo.sliceInfo.sst = (RIL_SliceServiceType)inputInfo.sst;
        setupDataCallInfo.sliceInfo.sliceDifferentiator = inputInfo.sliceDifferentiator;
        setupDataCallInfo.sliceInfo.mappedHplmnSD = inputInfo.mappedHplmnSD;
        setupDataCallInfo.sliceInfo.mappedHplmnSst = (RIL_SliceServiceType)inputInfo.mappedHplmnSst;
    } else {
        RLOGD("%s: no opt slice info", __FUNCTION__);
        setupDataCallInfo.OptionalSliceinfoFlag = RIL_OPT_INFO_INVALID;
        setupDataCallInfo.sliceInfo.sst = SST_NONE;
        setupDataCallInfo.sliceInfo.sliceDifferentiator = -1;
        setupDataCallInfo.sliceInfo.mappedHplmnSst = SST_NONE;
        setupDataCallInfo.sliceInfo.mappedHplmnSD = -1;
    }

    if (trafficDescriptor.getDiscriminator() != OEM_V1_2::OptionalTrafficDescriptor_aosp::hidl_discriminator::noinit) {
        OEM_V1_2::TrafficDescriptor_aosp tdInfo = trafficDescriptor.value();
        setupDataCallInfo.OptionalTrafficDescriptorFlag = RIL_OPT_INFO_VALID;

        //memset(setupDataCallInfo.tdInfo.dnn.value, 0, sizeof(RIL_OptionalDNN));
        setupDataCallInfo.tdInfo.dnn.clear();
        if (tdInfo.dnn.getDiscriminator() != OEM_V1_2::OptionalDnn::hidl_discriminator::noinit) {
            RLOGD("%s: set dnn Info", __FUNCTION__);
            setupDataCallInfo.tdInfo.dnnFlag = RIL_OPT_INFO_VALID;
            RIL_OptionalDNN dnn;
             memset(dnn.value, 0, sizeof(RIL_OptionalDNN));
            const char* input = tdInfo.dnn.value().c_str();

            for (size_t i = 0; i < tdInfo.dnn.value().size(); i++)
                dnn.value[i] = input[i];

            setupDataCallInfo.tdInfo.dnn.push_back(dnn);
        } else {
            setupDataCallInfo.tdInfo.dnnFlag = RIL_OPT_INFO_INVALID;
        }

        //memset(setupDataCallInfo.tdInfo.osAppId.value, 0, sizeof(RIL_OptionalOSAppId));
        setupDataCallInfo.tdInfo.osAppId.clear();
        if (tdInfo.osAppId.getDiscriminator() != OEM_V1_2::OptionalOsAppId::hidl_discriminator::noinit) {
            RLOGD("%s: set osAppId Info", __FUNCTION__);
            setupDataCallInfo.tdInfo.osAppIdFlag = RIL_OPT_INFO_VALID;

            RIL_OptionalOSAppId osappid;
            memset(osappid.value, 0, sizeof(RIL_OptionalOSAppId));

            size_t osAppIdLength = tdInfo.osAppId.value().osAppId.size();
            for (size_t i = 0; i < osAppIdLength; i++)
                osappid.value[i] = tdInfo.osAppId.value().osAppId[i];

            setupDataCallInfo.tdInfo.osAppId.push_back(osappid);
        } else {
            setupDataCallInfo.tdInfo.osAppIdFlag = RIL_OPT_INFO_INVALID;
        }
    } else {
        RLOGD("%s: no opt traffic info", __FUNCTION__);
        setupDataCallInfo.OptionalTrafficDescriptorFlag = RIL_OPT_INFO_INVALID;
        //memset(&setupDataCallInfo.tdInfo.dnn, 0, sizeof(RIL_OptionalDNN));
        //memset(&setupDataCallInfo.tdInfo.osAppId, 0, sizeof(RIL_OptionalOSAppId));
        setupDataCallInfo.tdInfo.dnn.clear();
        setupDataCallInfo.tdInfo.osAppId.clear();
    }

    setupDataCallInfo.matchAllRuleAllowed = matchAllRuleAllowed;

    return true;
}

/* 1.2 vendor::radio */
bool fillSetupDataCallInfo_V1_6(RIL_SetupDataCallInfo_V1_6 &setupDataCallInfo,
                                int32_t pduSessionId, const OEM_V1_2::OptionalSliceInfo& sliceInfo,
                                const OEM_V1_2::OptionalTrafficDescriptor& trafficDescriptor, bool matchAllRuleAllowed) {
    OEM_V1_2::OptionalTrafficDescriptor_aosp tmpTDInfo;

    if (trafficDescriptor.getDiscriminator() != OEM_V1_2::OptionalTrafficDescriptor::hidl_discriminator::noinit) {
        if(trafficDescriptor.value().dnn.getDiscriminator() != OEM_V1_2::OptionalDnn::hidl_discriminator::noinit){
            tmpTDInfo.value().dnn.value() = trafficDescriptor.value().dnn.value();
        }else{
            RLOGD("%s: no dnn info", __FUNCTION__);
        }

        if (trafficDescriptor.value().osAppId.getDiscriminator() != OEM_V1_2::OptionalOsAppId::hidl_discriminator::noinit) {
            tmpTDInfo.value().osAppId.value() = trafficDescriptor.value().osAppId.value();
        }else{
            RLOGD("%s: no osAppId info", __FUNCTION__);
        }
    }
    //tmpTDInfo.value().dnn.value() = trafficDescriptor.value().dnn.value();
    //tmpTDInfo.value().osAppId.value() = trafficDescriptor.value().osAppId.value();

    fillSetupDataCallInfo_V1_6_aosp(setupDataCallInfo, pduSessionId, sliceInfo, tmpTDInfo, matchAllRuleAllowed);

    if (trafficDescriptor.getDiscriminator() != OEM_V1_2::OptionalTrafficDescriptor::hidl_discriminator::noinit) {
        OEM_V1_2::TrafficDescriptor tdInfo = trafficDescriptor.value();

        if (trafficDescriptor.value().ipDescriptors.getDiscriminator() != OEM_V1_2::OptionalIpDescriptors::hidl_discriminator::noinit) {
            RLOGD("%s: set osAppId Info", __FUNCTION__);
            setupDataCallInfo.tdInfo.ipDescriptorsFlag = RIL_OPT_INFO_VALID;
            const OEM_V1_2::IpDescriptors ipd = trafficDescriptor.value().ipDescriptors.value();
            RIL_IpDescriptors &rilIpd = setupDataCallInfo.tdInfo.ipDescriptors;
            rilIpd.ipv4.push_back(ipd.ipv4);
            rilIpd.maskV4.push_back(ipd.maskV4);
            rilIpd.ipv6.push_back(ipd.ipv6);
            rilIpd.prefixLength.push_back(ipd.prefixLength);
            rilIpd.port.push_back(ipd.port);
            rilIpd.portStartRange.push_back(ipd.portStartRange);
            rilIpd.portEndRange.push_back(ipd.portEndRange);
            rilIpd.protocolId.push_back(ipd.protocolId);
        } else {
            setupDataCallInfo.tdInfo.ipDescriptorsFlag = RIL_OPT_INFO_INVALID;
            RLOGD("%s: no ipDescriptors info", __FUNCTION__);
        }

        if (trafficDescriptor.value().domainDescriptors.getDiscriminator() != OEM_V1_2::OptionalDomainDescriptors::hidl_discriminator::noinit) {
            RLOGD("%s: set osAppId Info", __FUNCTION__);
            setupDataCallInfo.tdInfo.domainDescriptorsFlag = RIL_OPT_INFO_VALID;
            RIL_DomainDescriptors domainDescriptor = {};
            domainDescriptor.value = trafficDescriptor.value().domainDescriptors.value();
            setupDataCallInfo.tdInfo.domainDescriptors.push_back(domainDescriptor);
        } else {
            setupDataCallInfo.tdInfo.domainDescriptorsFlag = RIL_OPT_INFO_INVALID;
            RLOGD("%s: no domainDescriptors info", __FUNCTION__);
        }

        if (trafficDescriptor.value().connectionCapabilities.getDiscriminator() != OEM_V1_2::OptionalConnectionCapabilities::hidl_discriminator::noinit) {
            RLOGD("%s: set osAppId Info", __FUNCTION__);
            setupDataCallInfo.tdInfo.connectionCapabilitiesFlag = RIL_OPT_INFO_VALID;
            RIL_ConnectionCapabilities connectionCapabilitie = {};
            connectionCapabilitie.value = trafficDescriptor.value().connectionCapabilities.value();
            setupDataCallInfo.tdInfo.connectionCapabilities.push_back(connectionCapabilitie);
        } else {
            setupDataCallInfo.tdInfo.connectionCapabilitiesFlag = RIL_OPT_INFO_INVALID;
            RLOGD("%s: no connectionCapabilities info", __FUNCTION__);
        }
    } else {
        RLOGD("%s: no opt traffic info", __FUNCTION__);
        //OEM_V1_2::TrafficDescriptor tdInfo = trafficDescriptor.value();

        setupDataCallInfo.tdInfo.ipDescriptorsFlag = RIL_OPT_INFO_INVALID;
        setupDataCallInfo.tdInfo.domainDescriptorsFlag = RIL_OPT_INFO_INVALID;
        setupDataCallInfo.tdInfo.connectionCapabilitiesFlag = RIL_OPT_INFO_INVALID;
    }
    return true;
}

Return<void> OemRadioImpl::setupDataCall_1_6(int32_t serial, V1_5::AccessNetwork accessNetwork,
                                             const V1_5::DataProfileInfo& dataProfileInfo, bool roamingAllowed,
                                             V1_2::DataRequestReason reason, const hidl_vec<V1_5::LinkAddress>& addresses,
                                             const hidl_vec<hidl_string>& dnses, int32_t pduSessionId, const vendor::samsung_slsi::telephony::hardware::radio::V1_2::OptionalSliceInfo& sliceInfo,
                                             const vendor::samsung_slsi::telephony::hardware::radio::V1_2::OptionalTrafficDescriptor& trafficDescriptor, bool matchAllRuleAllowed,
                                             const vendor::samsung_slsi::telephony::hardware::radio::V1_2::OptionalVerifyDescriptor& verifyDescriptor) {
#if VDBG
    RLOGD("%s: %d", __FUNCTION__, serial);
#endif

    int request = ENCODE_REQUEST(RIL_REQUEST_SETUP_DATA_CALL, HAL_VERSION_CODE(1, 6));
    RequestInfo *pRI = android::addRequestToList(serial, mSlotId, request);
    if (pRI == NULL) {
        return Void();
    }

    RIL_SetupDataCallInfo_V1_6 setupDataCallInfo = {};

    bool ret = fillSetupDataCallInfo_V1_5(*((RIL_SetupDataCallInfo_V1_5*)&setupDataCallInfo),
                                          accessNetwork, dataProfileInfo,
                                          roamingAllowed, reason, addresses, dnses, pRI);

    if (!ret) return Void();

    // fillSetupDataCallInfo_V1_5
    if (!convertHalDataProfileToRil(setupDataCallInfo.dataProfileInfo, dataProfileInfo, pRI)) {
        sendErrorResponse(pRI, RIL_E_NO_MEMORY);
        return Void();
    }

    fillSetupDataCallInfo_V1_6(setupDataCallInfo, pduSessionId, sliceInfo, trafficDescriptor, matchAllRuleAllowed);

    CALL_ONREQUEST(request, &setupDataCallInfo, sizeof(setupDataCallInfo), pRI, mSlotId);

    freeSetupDataCallInfo_V1_5(*(RIL_SetupDataCallInfo_V1_5 *)&setupDataCallInfo, addresses);

    return Void();
}

Return<void> OemRadioImpl::getSlicingConfig(int32_t serial) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    int request = ENCODE_REQUEST(RIL_REQUEST_SLICING_CONFIG, HAL_VERSION_CODE(1, 6));
    dispatchVoid(serial, mSlotId, request);
    return Void();
}

}  // namespace legacy
}  // namespace impl

/**
 * solicited response
 */
int queryCOLPResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen) {
#if VDBG
    RLOGD("OemRadioImpl::%s: serial %d", __FUNCTION__, serial);
#endif
    if (radioService[slotId]->mOemResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        int status = responseInt(responseInfo, serial, responseType, e, response, responseLen);
        Return<void> retStatus
                = radioService[slotId]->mOemResponse->queryCOLPResponse(responseInfo, status);
        radioService[slotId]->checkReturnStatus(retStatus);
    } else {
        RLOGE("OemRadioImpl::%s: radioService[%d]->mOemResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int queryCOLRResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen) {
#if VDBG
    RLOGD("OemRadioImpl::%s: serial %d", __FUNCTION__, serial);
#endif
    if (radioService[slotId]->mOemResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        int status = responseInt(responseInfo, serial, responseType, e, response, responseLen);
        Return<void> retStatus
                = radioService[slotId]->mOemResponse->queryCOLRResponse(responseInfo, status);
        radioService[slotId]->checkReturnStatus(retStatus);
    } else {
        RLOGE("OemRadioImpl::%s: radioService[%d]->mOemResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int iccGetAtrResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen) {
#if VDBG
    RLOGD("OemRadioImpl::%s: serial %d", __FUNCTION__, serial);
#endif
    if (radioService[slotId]->mOemResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus
                = radioService[slotId]->mOemResponse->iccGetAtrResponse(responseInfo, Radio2HalUtils::convertCharPtrToHidlString((char *) response));
        radioService[slotId]->checkReturnStatus(retStatus);
    } else {
        RLOGE("OemRadioImpl::%s: radioService[%d]->mOemResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int sendUSSDWithDcsResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen) {
#if VDBG
    RLOGD("OemRadioImpl::%s: serial %d", __FUNCTION__, serial);
#endif
    if (radioService[slotId]->mOemResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus
                = radioService[slotId]->mOemResponse->sendUSSDWithDcsResponse(responseInfo);
        radioService[slotId]->checkReturnStatus(retStatus);
    } else {
        RLOGE("OemRadioImpl::%s: radioService[%d]->mOemResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int setEmcStatusResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen) {
#if VDBG
    RLOGD("OemRadioImpl::%s: serial %d", __FUNCTION__, serial);
#endif
    if (radioService[slotId]->mOemResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus
                = radioService[slotId]->mOemResponse->setEmcStatusResponse(responseInfo);
        radioService[slotId]->checkReturnStatus(retStatus);
    } else {
        RLOGE("OemRadioImpl::%s: radioService[%d]->mOemResponse == NULL", __FUNCTION__, slotId);
    }
    return 0;
}

int setFemtoCellSearchResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen) {
#if VDBG
    RLOGD("OemRadioImpl::%s: serial %d", __FUNCTION__, serial);
#endif
    if (radioService[slotId]->mOemResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);

        int32_t result = 0;
        hidl_string numeric;
        int numStrings = responseLen / sizeof(char *);
        if (response == NULL || numStrings != 2) {
            RLOGE("OemRadioImpl::%s Invalid response: NULL", __FUNCTION__);
            if (e == RIL_E_SUCCESS) responseInfo.error = RadioError::INVALID_RESPONSE;

        } else {
            char **resp = (char **) response;
            result = atoi(resp[0]);
            numeric = Radio2HalUtils::convertCharPtrToHidlString(resp[1]);
        }

        Return<void> retStatus
                = radioService[slotId]->mOemResponse->setFemtoCellSearchResponse(responseInfo, result, numeric);
        radioService[slotId]->checkReturnStatus(retStatus);
    } else {
        RLOGE("OemRadioImpl::%s: radioService[%d]->mOemResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int setCdmaHybridModeResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen) {
#if VDBG
    RLOGD("OemRadioImpl::%s: serial %d", __FUNCTION__, serial);
#endif
    if (radioService[slotId]->mOemResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus
                = radioService[slotId]->mOemResponse->setCdmaHybridModeResponse(responseInfo);
        radioService[slotId]->checkReturnStatus(retStatus);
    } else {
        RLOGE("OemRadioImpl::%s: radioService[%d]->mOemResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int getCdmaHybridModeResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen) {
#if VDBG
    RLOGD("OemRadioImpl::%s: serial %d", __FUNCTION__, serial);
#endif
    if (radioService[slotId]->mOemResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        int mode = responseInt(responseInfo, serial, responseType, e, response, responseLen);
        Return<void> retStatus
                = radioService[slotId]->mOemResponse->getCdmaHybridModeResponse(responseInfo, mode);
        radioService[slotId]->checkReturnStatus(retStatus);
    } else {
        RLOGE("OemRadioImpl::%s: radioService[%d]->mOemResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int queryBplmnSearchResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen) {
#if VDBG
    RLOGD("OemRadioImpl::%s: serial %d", __FUNCTION__, serial);
#endif
    if (radioService[slotId]->mOemResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        hidl_vec<OperatorInfoEx> networkInfos;
        if ((response == NULL && responseLen != 0)
                || responseLen % (5 * sizeof(char *))!= 0) {
            RLOGE("OemRadioImpl::%s Invalid response: NULL", __FUNCTION__);
            if (e == RIL_E_SUCCESS) responseInfo.error = RadioError::INVALID_RESPONSE;
        } else {
            char **resp = (char **) response;
            int numStrings = responseLen / sizeof(char *);
            networkInfos.resize(numStrings/5);
            for (int i = 0, j = 0; i < numStrings; i = i + 5, j++) {
                networkInfos[j].alphaLong = Radio2HalUtils::convertCharPtrToHidlString(resp[i]);
                networkInfos[j].alphaShort = Radio2HalUtils::convertCharPtrToHidlString(resp[i + 1]);
                networkInfos[j].operatorNumeric = Radio2HalUtils::convertCharPtrToHidlString(resp[i + 2]);
                int status = Radio2HalUtils::convertOperatorStatusToInt(resp[i + 3]);
                if (status == -1) {
                    if (e == RIL_E_SUCCESS) responseInfo.error = RadioError::INVALID_RESPONSE;
                } else {
                    networkInfos[j].status = (OperatorStatus) status;
                }
                networkInfos[j].rat = ATOI_NULL_HANDLED(resp[i + 4]);
            }
        }

        Return<void> retStatus
                = radioService[slotId]->mOemResponse->queryBplmnSearchResponse(responseInfo, networkInfos);
        radioService[slotId]->checkReturnStatus(retStatus);
    } else {
        RLOGE("OemRadioImpl::%s: radioService[%d]->mOemResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int setNetworkSelectionModeManualResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen) {
#if VDBG
        RLOGD("OemRadioImpl::%s: serial %d", __FUNCTION__, serial);
#endif
    if (radioService[slotId]->mOemResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus
                = radioService[slotId]->mOemResponse->setNetworkSelectionModeManualResponse(responseInfo);
        radioService[slotId]->checkReturnStatus(retStatus);
    } else {
        RLOGE("OemRadioImpl::%s: radioService[%d]->mOemResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int dialWithCallTypeResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen) {
#if VDBG
    RLOGD("OemRadioImpl::%s: serial %d", __FUNCTION__, serial);
#endif
    if (radioService[slotId]->mOemResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus
                = radioService[slotId]->mOemResponse->dialWithCallTypeResponse(responseInfo);
        radioService[slotId]->checkReturnStatus(retStatus);
    } else {
        RLOGE("OemRadioImpl::%s: radioService[%d]->mOemResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int setVoiceOperationResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen) {
#if VDBG
    RLOGD("OemRadioImpl::%s: serial %d", __FUNCTION__, serial);
#endif
    if (radioService[slotId]->mOemResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus
                = radioService[slotId]->mOemResponse->setVoiceOperationResponse(responseInfo);
        radioService[slotId]->checkReturnStatus(retStatus);
    } else {
        RLOGE("OemRadioImpl::%s: radioService[%d]->mOemResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int setBarringPasswordOverMmiResponse(int slotId,int responseType,int serial,RIL_Errno e,void * response,size_t responseLen) {
#if VDBG
    RLOGD("OemRadioImpl::%s: serial %d", __FUNCTION__, serial);
#endif
    if (radioService[slotId]->mOemResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus
                = radioService[slotId]->mOemResponse->setBarringPasswordOverMmiResponse(responseInfo);
        radioService[slotId]->checkReturnStatus(retStatus);
    } else {
        RLOGE("OemRadioImpl::%s: radioService[%d]->mOemResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int setDualNetworkTypeAndAllowDataResponse(int slotId,int responseType,int serial,RIL_Errno e,void * response,size_t responseLen) {
#if VDBG
    RLOGD("OemRadioImpl::%s: serial %d", __FUNCTION__, serial);
#endif
    if (radioService[slotId]->mOemResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus
                = radioService[slotId]->mOemResponse->setDualNetworkTypeAndAllowDataResponse(responseInfo);
        radioService[slotId]->checkReturnStatus(retStatus);
    } else {
        RLOGE("OemRadioImpl::%s: radioService[%d]->mOemResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int setDsNetworkTypeResponse(int slotId,int responseType,int serial,RIL_Errno e,void * response,size_t responseLen) {
#if VDBG
    RLOGD("OemRadioImpl::%s: serial %d", __FUNCTION__, serial);
#endif
    if (radioService[slotId]->mOemResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus
                = radioService[slotId]->mOemResponse->setDsNetworkTypeResponse(responseInfo);
        radioService[slotId]->checkReturnStatus(retStatus);
    } else {
        RLOGE("OemRadioImpl::%s: radioService[%d]->mOemResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int deactivateDataCallWithReasonResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen) {
#if VDBG
    RLOGD("OemRadioImpl::%s: serial %d", __FUNCTION__, serial);
#endif
    if (radioService[slotId]->mOemResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus
                = radioService[slotId]->mOemResponse->deactivateDataCallWithReasonResponse(responseInfo);
        radioService[slotId]->checkReturnStatus(retStatus);
    } else {
        RLOGE("OemRadioImpl::%s: radioService[%d]->mOemResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int getSimLockStatusResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen) {
#if VDBG
   RLOGD("OemRadioImpl::%s: serial %d", __FUNCTION__, serial);
#endif
   if (radioService[slotId]->mOemResponse != NULL) {
       RadioResponseInfo responseInfo = {};
       populateResponseInfo(responseInfo, serial, responseType, e);
       int policy = -1;
       int status = -1;
       int lockType = -1;
       int maxRetryCount = 0;
       int remainCount = 0;
       hidl_vec<hidl_string> lockCode;

       if (response == NULL || responseLen != sizeof(RIL_SimLockStatus)) {
           RLOGE("OemRadioImpl::%s Invalid response: NULL", __FUNCTION__);
           if (e == RIL_E_SUCCESS)
               responseInfo.error = RadioError::INVALID_RESPONSE;
       } else {

           if (e == RIL_E_SUCCESS) {
               RIL_SimLockStatus *simlockStatus = (RIL_SimLockStatus *)response;
               policy = simlockStatus->policy;
               status = simlockStatus->status;
               lockType = simlockStatus->lockType;
               maxRetryCount = simlockStatus->maxRetryCount;
               remainCount = simlockStatus->remainCount;
               if (simlockStatus->numOfLockCode > 0) {
                   lockCode.resize(simlockStatus->numOfLockCode);
                   for (int i = 0; i < simlockStatus->numOfLockCode; i++) {
                       lockCode[i] = Radio2HalUtils::convertCharPtrToHidlString(simlockStatus->lockCode[i]);
                   } // end for i ~
               }
           }
       }

       Return<void> retStatus
               = radioService[slotId]->mOemResponse->getSimLockStatusResponse(responseInfo,
                       policy, status, lockType, maxRetryCount, remainCount, lockCode);
       radioService[slotId]->checkReturnStatus(retStatus);

   } else {
       RLOGE("OemRadioImpl::%s: radioService[%d]->mOemResponse == NULL", __FUNCTION__, slotId);
   }
   return 0;
}

int sendVsimNotificationResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen) {
#if VDBG
    RLOGD("OemRadioImpl::%s: serial %d", __FUNCTION__, serial);
#endif
    if (radioService[slotId]->mOemResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus
                = radioService[slotId]->mOemResponse->sendVsimNotificationResponse(responseInfo);
        radioService[slotId]->checkReturnStatus(retStatus);
    } else {
        RLOGE("OemRadioImpl::%s: radioService[%d]->mOemResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int sendVsimOperationResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen) {
#if VDBG
    RLOGD("OemRadioImpl::%s: serial %d", __FUNCTION__, serial);
#endif
    if (radioService[slotId]->mOemResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus
                = radioService[slotId]->mOemResponse->sendVsimOperationResponse(responseInfo);
        radioService[slotId]->checkReturnStatus(retStatus);
    } else {
        RLOGE("OemRadioImpl::%s: radioService[%d]->mOemResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int sendRequestRawResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen) {
#if VDBG
   RLOGD("OemRadioImpl::%s: serial %d", __FUNCTION__, serial);
#endif

    if (radioService[slotId]->mOemResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        hidl_vec<uint8_t> data;

        if (response == NULL) {
            RLOGE("OemRadioImpl::%s: Invalid response", __FUNCTION__);
            if (e == RIL_E_SUCCESS) responseInfo.error = RadioError::INVALID_RESPONSE;
        } else {
            data.setToExternal((uint8_t *) response, responseLen);
        }
        Return<void> retStatus
                = radioService[slotId]->mOemResponse->sendRequestRawResponse(responseInfo, data);
        radioService[slotId]->checkReturnStatus(retStatus);
    } else {
        RLOGE("OemRadioImpl::%s: radioService[%d]->mOemResponse == NULL",
                __FUNCTION__, slotId);
    }

    return 0;
}


/**
 * unsolicited response
 */
int suppSvcReturnResult(int slotId, int indicationType, int token, RIL_Errno e, void *response, size_t responselen) {
#if VDBG
    RLOGD("OemRadioImpl::%s", __FUNCTION__);
#endif
    if (radioService[slotId] != NULL && radioService[slotId]->mOemIndication != NULL) {
        if (response == NULL) {
            RLOGE("OemRadioImpl::%s: invalid response", __FUNCTION__);
            return 0;
        }

        hidl_string numeric;
        numeric = Radio2HalUtils::convertCharPtrToHidlString((char *)response);
        Return<void> retStatus = radioService[slotId]->mOemIndication->suppSvcReturnResult(
                convertIntToRadioIndicationType(indicationType), numeric);
        radioService[slotId]->checkReturnStatus(retStatus);
    } else {
        RLOGE("OemRadioImpl::%s: radioService[%d]->mOemIndication == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int callPresentInd(int slotId, int indicationType, int token, RIL_Errno e, void *response, size_t responselen) {
#if VDBG
    RLOGD("OemRadioImpl::%s", __FUNCTION__);
#endif
    if (radioService[slotId] != NULL && radioService[slotId]->mOemIndication != NULL) {
        Return<void> retStatus = radioService[slotId]->mOemIndication->callPresentInd(convertIntToRadioIndicationType(indicationType));
        radioService[slotId]->checkReturnStatus(retStatus);
    } else {
        RLOGE("OemRadioImpl::%s: radioService[%d]->mOemIndication == NULL", __FUNCTION__, slotId);
    }
    return 0;
}

int wbAmrReportInd(int slotId, int indicationType, int token, RIL_Errno e, void *response, size_t responselen) {
#if VDBG
    RLOGD("OemRadioImpl::%s", __FUNCTION__);
#endif
    if (radioService[slotId] != NULL && radioService[slotId]->mOemIndication != NULL) {
        if (response == NULL) {
            RLOGE("OemRadioImpl::%s: invalid response", __FUNCTION__);
            return 0;
        }
        bool on = ((int *)response)[0] > 0;
        Return<void> retStatus = radioService[slotId]->mOemIndication->wbAmrReportInd(convertIntToRadioIndicationType(indicationType), on);
        radioService[slotId]->checkReturnStatus(retStatus);
    } else {
        RLOGE("OemRadioImpl::%s: radioService[%d]->mOemIndication == NULL", __FUNCTION__, slotId);
    }
    return 0;
}

int vsimOperationInd(int slotId, int indicationType, int token, RIL_Errno e, void *response, size_t responselen) {
#if VDBG
    RLOGD("OemRadioImpl::%s", __FUNCTION__);
#endif
    if (radioService[slotId] != NULL && radioService[slotId]->mOemIndication != NULL) {
        if (response == NULL) {
            RLOGE("OemRadioImpl::%s: invalid response", __FUNCTION__);
            return 0;
        }
        VsimOperationEvent vsimEvent = {};
        vsimEvent.transactionId = ((RIL_VsimOperationEvent *)response)->transaction_id;
        vsimEvent.eventId = ((RIL_VsimOperationEvent *)response)->eventId;
        vsimEvent.result = ((RIL_VsimOperationEvent *)response)->result;
        vsimEvent.datalen = ((RIL_VsimOperationEvent *)response)->data_length;
        if (vsimEvent.datalen > 0 && ((RIL_VsimOperationEvent *)response)->data != NULL) {
            vsimEvent.data = Radio2HalUtils::convertCharPtrToHidlString(((RIL_VsimOperationEvent *)response)->data);
        }
        Return<void> retStatus = radioService[slotId]->mOemIndication->vsimOperationInd(convertIntToRadioIndicationType(indicationType), vsimEvent);
        radioService[slotId]->checkReturnStatus(retStatus);
    } else {
        RLOGE("OemRadioImpl::%s: radioService[%d]->mOemIndication == NULL", __FUNCTION__, slotId);
    }
    return 0;
}

int nasTimerStatusInd(int slotId, int indicationType, int token, RIL_Errno e, void *response, size_t responselen) {
#if VDBG
    RLOGD("OemRadioImpl::%s", __FUNCTION__);
#endif
    if (radioService[slotId] != NULL && radioService[slotId]->mOemIndication != NULL) {
        if (response == NULL) {
            RLOGE("OemRadioImpl::%s: invalid response", __FUNCTION__);
            return 0;
        }
        NasTimerStatus status = {};
        status.type = ((RIL_NasTimerStatus *)response)->type;
        status.status = ((RIL_NasTimerStatus *)response)->status;
        status.value = ((RIL_NasTimerStatus *)response)->value;
        status.apn = Radio2HalUtils::convertCharPtrToHidlString(((RIL_NasTimerStatus *)response)->apn);
        Return<void> retStatus = radioService[slotId]->mOemIndication->nasTimerStatusInd(convertIntToRadioIndicationType(indicationType), status);
        radioService[slotId]->checkReturnStatus(retStatus);
    } else {
        RLOGE("OemRadioImpl::%s: radioService[%d]->mOemIndication == NULL", __FUNCTION__, slotId);
    }
    return 0;
}

int emergencyActInd(int slotId, int indicationType, int token, RIL_Errno e, void *response, size_t responselen) {
#if VDBG
    RLOGD("OemRadioImpl::%s", __FUNCTION__);
#endif
    if (radioService[slotId] != NULL && radioService[slotId]->mOemIndication != NULL) {
        if (response == NULL) {
            RLOGE("OemRadioImpl::%s: invalid response", __FUNCTION__);
            return 0;
        }
        int32_t act = ((int*)response)[0];
        int32_t status = ((int*)response)[1];
        Return<void> retStatus = radioService[slotId]->mOemIndication->emergencyActInd(convertIntToRadioIndicationType(indicationType), act, status);
        radioService[slotId]->checkReturnStatus(retStatus);
    } else {
        RLOGE("OemRadioImpl::%s: radioService[%d]->mOemIndication == NULL", __FUNCTION__, slotId);
    }
    return 0;
}

int onUssdWithDcsInd(int slotId, int indicationType, int token, RIL_Errno e, void *response, size_t responselen) {
#if VDBG
    RLOGD("OemRadioImpl::%s", __FUNCTION__);
#endif
    if (radioService[slotId] != NULL && radioService[slotId]->mOemIndication != NULL) {
        if (response == NULL || responselen != 3 * sizeof(char *)) {
            RLOGE("OemRadioImpl::%s: invalid response", __FUNCTION__);
            return 0;
        }

        char **resp = (char **) response;
        hidl_string modeType = Radio2HalUtils::convertCharPtrToHidlString(resp[0]);
        hidl_string msg = Radio2HalUtils::convertCharPtrToHidlString(resp[1]);
        hidl_string dcs = Radio2HalUtils::convertCharPtrToHidlString(resp[2]);

        Return<void> retStatus = radioService[slotId]->mOemIndication->onUssdWithDcsInd(convertIntToRadioIndicationType(indicationType), modeType, msg, dcs);
        radioService[slotId]->checkReturnStatus(retStatus);
    } else {
        RLOGE("OemRadioImpl::%s: radioService[%d]->mOemIndication == NULL", __FUNCTION__, slotId);
    }
    return 0;
}

int volteAvailableInfoInd(int slotId, int indicationType, int token, RIL_Errno e, void *response, size_t responselen) {
#if VDBG
    RLOGD("OemRadioImpl::%s", __FUNCTION__);
#endif
    if (radioService[slotId] != NULL && radioService[slotId]->mOemIndication != NULL) {
        if (response == NULL) {
            RLOGE("OemRadioImpl::%s: invalid response", __FUNCTION__);
            return 0;
        }

        int32_t volteAvailable = ((int*)response)[0];
        int32_t emcAvailable = ((int*)response)[1];

        Return<void> retStatus = radioService[slotId]->mOemIndication->volteAvailableInfoInd(convertIntToRadioIndicationType(indicationType), volteAvailable, emcAvailable);
        radioService[slotId]->checkReturnStatus(retStatus);
    } else {
        RLOGE("OemRadioImpl::%s: radioService[%d]->mOemIndication == NULL", __FUNCTION__, slotId);
    }
    return 0;
}

int iccIdInfoInd(int slotId, int indicationType, int token, RIL_Errno e, void *response, size_t responselen) {
#if VDBG
    RLOGD("OemRadioImpl::%s", __FUNCTION__);
#endif
    if (radioService[slotId] != NULL && radioService[slotId]->mOemIndication != NULL) {
        if (response == NULL) {
            RLOGE("OemRadioImpl::%s: invalid response", __FUNCTION__);
            return 0;
        }

        hidl_string iccIdInfo = Radio2HalUtils::convertCharPtrToHidlString((char *)response);

        Return<void> retStatus = radioService[slotId]->mOemIndication->iccIdInfoInd(convertIntToRadioIndicationType(indicationType), iccIdInfo);
        radioService[slotId]->checkReturnStatus(retStatus);
    } else {
        RLOGE("OemRadioImpl::%s: radioService[%d]->mOemIndication == NULL", __FUNCTION__, slotId);
    }
    return 0;
}

int emergencySupportRatModeInd(int slotId, int indicationType, int token, RIL_Errno e, void *response, size_t responselen) {
#if VDBG
    RLOGD("OemRadioImpl::%s", __FUNCTION__);
#endif
    if (radioService[slotId] != NULL && radioService[slotId]->mOemIndication != NULL) {
        if (response == NULL) {
            RLOGE("OemRadioImpl::%s: invalid response", __FUNCTION__);
            return 0;
        }

        int32_t supportRatMode = ((int*)response)[0];

        Return<void> retStatus = radioService[slotId]->mOemIndication->emergencySupportRatModeInd(convertIntToRadioIndicationType(indicationType), supportRatMode);
        radioService[slotId]->checkReturnStatus(retStatus);
    } else {
        RLOGE("OemRadioImpl::%s: radioService[%d]->mOemIndication == NULL", __FUNCTION__, slotId);
    }
    return 0;
}

int ussdCanceledInd(int slotId, int indicationType, int token, RIL_Errno e, void *response, size_t responselen) {
#if VDBG
    RLOGD("OemRadioImpl::%s", __FUNCTION__);
#endif
    if (radioService[slotId] != NULL && radioService[slotId]->mOemIndication != NULL) {
        if (response == NULL) {
            RLOGE("OemRadioImpl::%s: invalid response", __FUNCTION__);
            return 0;
        }

        Return<void> retStatus = radioService[slotId]->mOemIndication->ussdCanceledInd(convertIntToRadioIndicationType(indicationType));
        radioService[slotId]->checkReturnStatus(retStatus);
    } else {
        RLOGE("OemRadioImpl::%s: radioService[%d]->mOemIndication == NULL", __FUNCTION__, slotId);
    }
    return 0;
}

/**
 * @1.1
 */
int setActivateVsimResponse(int slotId, int responseType, int serial,
        RIL_Errno e, void *response, size_t responseLen)
{
#if VDBG
    RLOGD("OemRadioImpl::%s: serial %d", __FUNCTION__, serial);
#endif
    if (radioService[slotId]->mOemResponseV1_1 != NULL)  {
        RadioResponseInfo responseInfo = {};
        int ret = responseInt(responseInfo, serial, responseType, e, response, responseLen);
        Return<void> retStatus
            = radioService[slotId]->mOemResponseV1_1->setActivateVsimResponse(responseInfo, ret);
        radioService[slotId]->checkReturnStatus(retStatus);
    } else {
        RLOGE("OemRadioImpl::%s: radioService[%d]->mOemResponseV1_1 == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int setEndcModeResponse(int slotId, int responseType, int serial,
        RIL_Errno e, void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s", __FUNCTION__);
#endif
    if (radioService[slotId] != NULL && radioService[slotId]->mOemResponseV1_1 != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus
            = radioService[slotId]->mOemResponseV1_1->setEndcModeResponse(responseInfo);
        radioService[slotId]->checkReturnStatus(retStatus);
    }
    else {
        RLOGE("OemRadioImpl::%s: radioService[%d]->mOemResponseV1_1 == NULL", __FUNCTION__, slotId);
    }
    return 0;
}
int getEndcModeResponse(int slotId, int responseType, int serial,
        RIL_Errno e, void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s", __FUNCTION__);
#endif
    if (radioService[slotId] != NULL && radioService[slotId]->mOemResponseV1_1 != NULL) {
        RadioResponseInfo responseInfo = {};
        bool enable = responseInt(responseInfo, serial, responseType, e, response, responseLen) > 0 ? true : false;
        Return<void> retStatus
            = radioService[slotId]->mOemResponseV1_1->getEndcModeResponse(responseInfo, enable);
        radioService[slotId]->checkReturnStatus(retStatus);
    }
    else {
        RLOGE("OemRadioImpl::%s: radioService[%d]->mOemResponseV1_1 == NULL", __FUNCTION__, slotId);
    }
    return 0;
}

int getSmsStorageOnSimResponse(int slotId,
                                 int responseType, int serial, RIL_Errno e,
                                 void *response, size_t responseLen) {
#if VDBG
    RLOGD("OemRadioImpl::%s: serial %d", __FUNCTION__, serial);
#endif

    if (radioService[slotId] != NULL && radioService[slotId]->mOemResponseV1_1 != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        int ret[2] = {0, 0};
        if (e == RIL_E_SUCCESS && response != NULL) {
            RIL_StorageStatus *pResponse = (RIL_StorageStatus *)response;
            // return only total and used capacity.
            ret[0] = pResponse->total_num;
            ret[1] = pResponse->used_num;
        }

        Return<void> retStatus
                = radioService[slotId]->mOemResponseV1_1->getSmsStorageOnSimResponse(responseInfo, ret);
        radioService[slotId]->checkReturnStatus(retStatus);
    } else {
        RLOGE("OemRadioImpl::%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int currentPhysicalChannelConfigInd(int slotId, int indicationType,
        int token, RIL_Errno e, void *response, size_t responselen)
{
#if VDBG
    RLOGD("OemRadioImpl::%s", __FUNCTION__);
#endif
    if (radioService[slotId] != NULL && radioService[slotId]->mOemIndicationV1_1 != NULL) {
        if (response == NULL || responselen == 0 || responselen % sizeof(int) != 0) {
            RLOGE("OemRadioImpl::%s: invalid response", __FUNCTION__);
            return 0;
        }

        int rat = ((int *)response)[0];
        int status = ((int *)response)[1];
        Return<void> retStatus = radioService[slotId]->mOemIndicationV1_1->currentNrPhysicalChannelConfigs(
                convertIntToRadioIndicationType(indicationType),
                rat, status);
        radioService[slotId]->checkReturnStatus(retStatus);
    } else {
        RLOGE("OemRadioImpl::%s: radioService[%d]->mOemIndicationV1_1 == NULL", __FUNCTION__, slotId);
    }
    return 0;
}

int endcCapabilityInd(int slotId, int indicationType,
                                                   int token, RIL_Errno e, void *response, size_t responselen) {
#if VDBG
        RLOGD("OemRadioImpl::%s", __FUNCTION__);
#endif
    if (radioService[slotId] != NULL && radioService[slotId]->mOemIndicationV1_1 != NULL) {
        if (response == NULL || responselen == 0 || responselen % sizeof(int) != 0) {
            RLOGE("OemRadioImpl::%s: invalid response", __FUNCTION__);
            return 0;
        }

        int capability = ((int *)response)[0];
        int cause = ((int *)response)[1];
        Return<void> retStatus = radioService[slotId]->mOemIndicationV1_1->endcCapabilityInd(
                                    convertIntToRadioIndicationType(indicationType), capability, cause);
        radioService[slotId]->checkReturnStatus(retStatus);
    } else {
        RLOGE("OemRadioImpl::%s: radioService[%d]->mOemIndicationV1_1 == NULL", __FUNCTION__, slotId);
    }
    return 0;
}

int simTrayStatusChangedInd(int slotId, int indicationType,
                                                   int token, RIL_Errno e, void *response, size_t responselen) {
#if VDBG
    RLOGD("OemRadioImpl::%s", __FUNCTION__);
#endif
    if (radioService[slotId] != NULL && radioService[slotId]->mOemIndicationV1_1 != NULL) {
        if (response == NULL || responselen == 0 || responselen % sizeof(int) != 0) {
            RLOGE("OemRadioImpl::%s: invalid response", __FUNCTION__);
            return 0;
        }

        int state = *((int *)response);
        Return<void> retStatus = radioService[slotId]->mOemIndicationV1_1->simTrayStatusChangedInd(
                                    convertIntToRadioIndicationType(indicationType), state);
        radioService[slotId]->checkReturnStatus(retStatus);
    } else {
        RLOGE("OemRadioImpl::%s: radioService[%d]->mOemIndicationV1_1 == NULL", __FUNCTION__, slotId);
    }
    return 0;
}

int unthrottleApnInd(int slotId, int indicationType,
                                                   int token, RIL_Errno e, void *response, size_t responselen) {
#if VDBG
RLOGD("OemRadioImpl::%s", __FUNCTION__);
#endif

    if (radioService[slotId] != NULL && radioService[slotId]->mOemIndicationV1_2 != NULL) {
        if (response == NULL || responselen == 0 || responselen % sizeof(int) != 0) {
            RLOGE("OemRadioImpl::%s: invalid response", __FUNCTION__);
            return 0;
        }

        DataV2_0::RIL_DataProfileInfo *in = (DataV2_0::RIL_DataProfileInfo *)response;
        RLOGE("[OemUnthrottleApnInd] %s", in->toString().c_str());

        OEM_V1_2::DataProfileInfo out = {};
        out.profileId = (::android::hardware::radio::V1_0::DataProfileId)in->profileId;
        out.apn = in->apn;
        out.protocol = (::android::hardware::radio::V1_4::PdpProtocolType)in->protocol;
        out.roamingProtocol = (::android::hardware::radio::V1_4::PdpProtocolType)in->roamingProtocol;
        out.authType = (::android::hardware::radio::V1_0::ApnAuthType)in->authType;
        out.user = in->user;
        out.password = in->password;
        out.type = (::android::hardware::radio::V1_0::DataProfileInfoType)in->type;
        out.maxConnsTime = static_cast<int32_t>(in->maxConnsTime);
        out.maxConns = static_cast<int32_t>(in->maxConns);
        out.waitTime = static_cast<int32_t>(in->waitTime);
        out.enabled = in->enabled;
        out.supportedApnTypesBitmap = static_cast<int32_t>(in->supportedApnTypesBitmap);
        out.bearerBitmap = static_cast<int32_t>(in->bearerBitmap);
        out.mtuV4 = static_cast<int32_t>(in->mtuV4);
        out.mtuV6 = static_cast<int32_t>(in->mtuV6);
        out.preferred = in->preferred;
        out.persistent = in->persistent;
        out.alwaysOn = in->alwaysOn;
        //TdInfo
        ::android::hardware::radio::V1_6::OptionalDnn tmpDnn;
        tmpDnn.value(out.apn);
        out.trafficDescriptor = {tmpDnn, {}};

        Return<void> retStatus = radioService[slotId]->mOemIndicationV1_2->unthrottleApn(convertIntToRadioIndicationType(indicationType), out);
        radioService[slotId]->checkReturnStatus(retStatus);
    }else {
        RLOGE("OemRadioImpl::%s: radioService[%d]->mOemIndicationV1_2 == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int setNrModeResponse(int slotId, int responseType, int serial,
        RIL_Errno e, void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s", __FUNCTION__);
#endif
    if (radioService[slotId] != NULL && radioService[slotId]->mOemResponseV1_1 != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus
            = radioService[slotId]->mOemResponseV1_1->setNrModeResponse(responseInfo);
        radioService[slotId]->checkReturnStatus(retStatus);
    }
    else {
        RLOGE("%s: radioService[%d]->mOemResponseV1_1 == NULL", __FUNCTION__, slotId);
    }
    return 0;
}
int getNrModeResponse(int slotId, int responseType, int serial,
        RIL_Errno e, void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s", __FUNCTION__);
#endif
    if (radioService[slotId] != NULL && radioService[slotId]->mOemResponseV1_1 != NULL) {
        RadioResponseInfo responseInfo = {};
        int mode = responseInt(responseInfo, serial, responseType, e, response, responseLen);
        Return<void> retStatus
            = radioService[slotId]->mOemResponseV1_1->getNrModeResponse(responseInfo, mode);
        radioService[slotId]->checkReturnStatus(retStatus);
    }
    else {
        RLOGE("%s: radioService[%d]->mOemResponseV1_1 == NULL", __FUNCTION__, slotId);
    }
    return 0;
}

int setupDataCallResponse_1_6(int slotId,
                              int responseType, int serial, RIL_Errno e, void *response,
                              size_t responselen) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif

    /*
     * TBD: consider some method to deliver 1.6 information
     */
    /* vendor radio IF implementation */
    if (radioService[slotId] != NULL && radioService[slotId]->mOemResponseV1_2 != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);

        V1_5::SetupDataCallResult dcResult = {};
        vendor::samsung_slsi::telephony::hardware::radio::V1_2::SetupDataCallResult dcResultV1_6 = {};
        // User v11 format, For V1_4, only two parameter are removed, and just type change for Cause
        if (response == NULL) {
            RLOGE("%s: Invalid response", __FUNCTION__);
            if (e == RIL_E_SUCCESS) responseInfo.error = RadioError::INVALID_RESPONSE;
            dcResult.cause = V1_4::DataCallFailCause::ERROR_UNSPECIFIED;
            dcResult.type = V1_4::PdpProtocolType::UNKNOWN;
            dcResult.ifname = hidl_string();
            dcResult.addresses.resize(0);
            dcResult.dnses.resize(0);
            dcResult.gateways.resize(0);
            dcResult.pcscf.resize(0);
        }
        else {
            RLOGE("%s: convertRilDataCallToHal", __FUNCTION__);
            Radio2HalUtils::convertRilDataCallToHal((RIL_SetupDataCallResult_V1_6 *)response, dcResult);
            Radio2HalUtils::convertRilDataCallToHal((RIL_SetupDataCallResult_V1_6 *)response, dcResultV1_6);
        }

        Return<void> retStatus = radioService[slotId]->mOemResponseV1_2->setupDataCallResponse_1_6(responseInfo, dcResult);

        radioService[slotId]->checkReturnStatus(retStatus);

        // Notify Ursp

    } else {
        RLOGE("%s: radioService[%d]->mOemResponseV1_2 == NULL, fallback to radio::V1_6", __FUNCTION__, slotId);
        return radio::setupDataCallResponse_1_6(slotId, responseType, serial, e, response, responselen);
    }
    return 0;
}

// consider move to RadioHalUtil@librilutils.so
/* 1.6 */
//void convertRilSlicingConfigToHal(void *dcResponse, vendor::samsung_slsi::telephony::hardware::radio::V1_2::SlicingConfig& dcResult);

/* 1.6 */
/*
void convertRilSlicingConfigToHal(void *dcResponse, vendor::samsung_slsi::telephony::hardware::radio::V1_2::SlicingConfig& dcResult) {
    RLOGD("convertRilSlicingConfigToHal");

    RIL_SlicingConfig * rilSlicingConfig = (RIL_SlicingConfig*) dcResponse;

    int urspRule_count = 0;
    int trafficDescriptor_count = 0;
    int routeSelectionDescriptor_count = 0;
    int sliceInfo_count = 0;
    int dnn_count = 0;

    // it_1 == urspRules : precedence, trafficDescriptors, routeSelectionDescriptor
    dcResult.urspRules.resize(rilSlicingConfig->urspNum);
    for (std::vector<RIL_UrspRule >::iterator it_1 = rilSlicingConfig->urspRules.begin(); it_1 != rilSlicingConfig->urspRules.end(); it_1++, urspRule_count++) {
        dcResult.urspRules[urspRule_count].precedence = it_1->precedence;

        // it_2 == trafficDescriptors : dnn, osAppId
        // trafficDescriptor num do not come from CP. Just 1 now.
        trafficDescriptor_count = 0;
        dcResult.urspRules[urspRule_count].trafficDescriptors.resize(trafficDescriptor_count + 1);
        for (std::vector<RIL_TrafficDescriptor >::iterator it_2 = it_1->trafficDescriptors.begin(); it_2 != it_1->trafficDescriptors.end(); it_2++, trafficDescriptor_count++) {
            vendor::samsung_slsi::telephony::hardware::radio::V1_2::OptionalDnn tmpDnn;
            vendor::samsung_slsi::telephony::hardware::radio::V1_2::OptionalOsAppId tmpOsAppID;
            vendor::samsung_slsi::telephony::hardware::radio::V1_2::OsAppId tmpOsAppIdValue;
            //The original type was TrafficDescriptor_aosp.
            vendor::samsung_slsi::telephony::hardware::radio::V1_2::TrafficDescriptor tmpTDInfo;

            tmpDnn.value(Radio2HalUtils::convertCharPtrToHidlString(it_2->dnn[0].value));

            int length = strlen((char*) (it_2->osAppId[0].value));
            tmpOsAppIdValue.osAppId.resize(length);
            for (int idx = 0; idx < length; idx++)
                tmpOsAppIdValue.osAppId[idx] = it_2->osAppId[0].value[idx];
            tmpOsAppID.value(tmpOsAppIdValue);

            tmpTDInfo.dnn = tmpDnn;
            tmpTDInfo.osAppId = tmpOsAppID;
            dcResult.urspRules[urspRule_count].trafficDescriptors[trafficDescriptor_count] = tmpTDInfo;
        }

        // it_2 == RouteSelectionDescriptor : precedence, routeSelectionDescriptorParams
        routeSelectionDescriptor_count = 0;
        dcResult.urspRules[urspRule_count].routeSelectionDescriptor.resize(it_1->RSDNum);
        for (std::vector<RIL_RouteSelectionDescriptor >::iterator it_2 = it_1->routeSelectionDescriptor.begin() ; it_2 != it_1->routeSelectionDescriptor.end(); it_2++, routeSelectionDescriptor_count++) {
            // printf("Descriptor precedence : %d \n", it_2->precedence);
            dcResult.urspRules[urspRule_count].routeSelectionDescriptor[routeSelectionDescriptor_count].precedence = it_2->precedence;
            dcResult.urspRules[urspRule_count].routeSelectionDescriptor[routeSelectionDescriptor_count].sessionType.value((V1_4::PdpProtocolType)it_2->sessionType.value);
            dcResult.urspRules[urspRule_count].routeSelectionDescriptor[routeSelectionDescriptor_count].sscMode.value((vendor::samsung_slsi::telephony::hardware::radio::V1_2::SscMode)it_2->sscMode.value);

            // it_3 == SliceInfo
            sliceInfo_count = 0;
            dcResult.urspRules[urspRule_count].routeSelectionDescriptor[routeSelectionDescriptor_count].sliceInfo.resize(it_2->sliceNum);
            for (std::vector<RIL_SliceInfo >::iterator it_3 = it_2->sliceInfo.begin(); it_3 != it_2->sliceInfo.end(); it_3++, sliceInfo_count++)   {
                dcResult.urspRules[urspRule_count].routeSelectionDescriptor[routeSelectionDescriptor_count].sliceInfo[sliceInfo_count].sst = (vendor::samsung_slsi::telephony::hardware::radio::V1_2::SliceServiceType)it_3->sst;
                dcResult.urspRules[urspRule_count].routeSelectionDescriptor[routeSelectionDescriptor_count].sliceInfo[sliceInfo_count].sliceDifferentiator = it_3->sliceDifferentiator;
                dcResult.urspRules[urspRule_count].routeSelectionDescriptor[routeSelectionDescriptor_count].sliceInfo[sliceInfo_count].mappedHplmnSst = (vendor::samsung_slsi::telephony::hardware::radio::V1_2::SliceServiceType)it_3->mappedHplmnSst;
                dcResult.urspRules[urspRule_count].routeSelectionDescriptor[routeSelectionDescriptor_count].sliceInfo[sliceInfo_count].mappedHplmnSD = it_3->mappedHplmnSD;
                dcResult.urspRules[urspRule_count].routeSelectionDescriptor[routeSelectionDescriptor_count].sliceInfo[sliceInfo_count].status = (vendor::samsung_slsi::telephony::hardware::radio::V1_2::SliceStatus)it_3->status;
            }

            // it_3 == dnn
            dnn_count = 0;
            dcResult.urspRules[urspRule_count].routeSelectionDescriptor[routeSelectionDescriptor_count].dnn.resize(it_2->dnnNum);
            for (std::vector<RIL_OptionalDNN >::iterator it_3 = it_2->dnn.begin(); it_3 != it_2->dnn.end(); it_3++, dnn_count++) {
                dcResult.urspRules[urspRule_count].routeSelectionDescriptor[routeSelectionDescriptor_count].dnn[dnn_count] = Radio2HalUtils::convertCharPtrToHidlString(it_3->value);
            }
        }
    }

    sliceInfo_count = 0;
    dcResult.sliceInfo.resize(rilSlicingConfig->sliceInfoNum);
    for (std::vector<RIL_SliceInfo >::iterator it_1 = rilSlicingConfig->sliceInfo.begin(); it_1 != rilSlicingConfig->sliceInfo.end(); it_1++, sliceInfo_count++) {
        dcResult.sliceInfo[sliceInfo_count].sst = (vendor::samsung_slsi::telephony::hardware::radio::V1_2::SliceServiceType)it_1->sst;
        dcResult.sliceInfo[sliceInfo_count].sliceDifferentiator = it_1->sliceDifferentiator;
        dcResult.sliceInfo[sliceInfo_count].mappedHplmnSst = (vendor::samsung_slsi::telephony::hardware::radio::V1_2::SliceServiceType)it_1->mappedHplmnSst;
        dcResult.sliceInfo[sliceInfo_count].mappedHplmnSD = it_1->mappedHplmnSD;
        dcResult.sliceInfo[sliceInfo_count].status = (vendor::samsung_slsi::telephony::hardware::radio::V1_2::SliceStatus)it_1->status;
    }
}
*/

int getSlicingConfigResponse(int slotId, int responseType, int serial,
                             RIL_Errno e, void* response, size_t responseLen) {
#if VDBG
    RLOGD("getSlicingConfigResponse: serial %d", serial);
#endif

    if (radioService[slotId] != NULL && radioService[slotId]->mOemResponseV1_2 != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);

        vendor::samsung_slsi::telephony::hardware::radio::V1_2::SlicingConfig slicingConfig = {};

        if (response == NULL) {
            RLOGE("%s: Invalid response", __FUNCTION__);
            if (e == RIL_E_SUCCESS) {
                responseInfo.error = RadioError::INVALID_RESPONSE;
            } else if (e == RIL_E_MODEM_ERR) {
                responseInfo.error = RadioError::MODEM_ERR;
            }
        } else {
            Radio2HalUtils::convertRilSlicingConfigToHal(response, slicingConfig);
        }
        Return<void> retStatus
            = radioService[slotId]->mOemResponseV1_2->getSlicingConfigResponse(responseInfo, slicingConfig);
       radioService[slotId]->checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mOemResponseV1_2 == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

}  // namespace radio
}  // namespace vendor


void vendor::radio::impl::legacy::registerAsService() {
    using namespace android::hardware;
    int simCount = 1;
    const char *serviceNames[] = {
            android::RIL_getServiceName()
            #if (SIM_COUNT >= 2)
            , RIL2_SERVICE_NAME
            #if (SIM_COUNT >= 3)
            , RIL3_SERVICE_NAME
            #if (SIM_COUNT >= 4)
            , RIL4_SERVICE_NAME
            #endif
            #endif
            #endif
            };

    #if (SIM_COUNT >= 2)
    simCount = SIM_COUNT;
    #endif

    // init vendor radio
    initRadios();

    configureRpcThreadpool(1, false /* callerWillJoin */);
    for (int i = 0; i < simCount; i++) {
        pthread_rwlock_t *radioServiceRwlockPtr = ::radio::getRadioServiceRwlock(i);
        int ret = pthread_rwlock_wrlock(radioServiceRwlockPtr);
        assert(ret == 0);

        sp<OemRadioImpl> radio = radioService[i];
        if (radio != nullptr) {
            android::status_t status = radio->registerAsService(serviceNames[i]);
            if (status == android::OK) {
                RLOGD("registerService: starting %s %s", CURRENT_RADIO_HAL, serviceNames[i]);
            }
            else {
                RLOGD("registerService: failed to start %s %s", CURRENT_RADIO_HAL, serviceNames[i]);
            }
        }

        ret = pthread_rwlock_unlock(radioServiceRwlockPtr);
        assert(ret == 0);
    }
}

// update solicited response handlers
static CommandInfo s_commands[] = {
    #include "slsi/ril_ext_commands.h"
};

// update unsolicited response handlers
static UnsolResponseInfo s_unsolResponses[] = {
    #include "slsi/ril_ext_unsol_commands.h"
};

void updateCommandInfo_vendor_radio() {
    android::updateCommandInfo(s_commands, NUM_ELEMS(s_commands));
    android::updateUnsolResponseInfo(s_unsolResponses, NUM_ELEMS(s_unsolResponses));
}


void vendor::radio::impl::legacy::initRadios() {
    int simCount = 1;
    #if (SIM_COUNT >= 2)
    simCount = SIM_COUNT;
    #endif

    // update solicited and unsolicited response handlers for V1_6 radio
    updateCommandInfo_vendor_radio();

    for (int i = 0; i < simCount; i++) {
        pthread_rwlock_t *radioServiceRwlockPtr = ::radio::getRadioServiceRwlock(i);
        int ret = pthread_rwlock_wrlock(radioServiceRwlockPtr);
        assert(ret == 0);

        radioService[i] = new OemRadioImpl;
        radioService[i]->mSlotId = i;

        ret = pthread_rwlock_unlock(radioServiceRwlockPtr);
        assert(ret == 0);
    }
}

android::sp<::vendor::samsung_slsi::telephony::hardware::radio::V1_1::IOemSamsungslsi>
vendor::radio::impl::legacy::getRadio(int slotId) {
    int simCount = 1;
    #if (SIM_COUNT >= 2)
    simCount = SIM_COUNT;
    #endif

    if (slotId < 0 || slotId >= simCount) {
        return nullptr;
    }
    sp<::vendor::samsung_slsi::telephony::hardware::radio::V1_1::IOemSamsungslsi> radio = radioService[slotId];
    return radio;
}
