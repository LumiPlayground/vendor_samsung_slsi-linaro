/*
 * Copyright (c) 2016 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/*
 * ril_service_1_6.cpp
 *
 *  Created on: 2020. 11. 18.
 *      Author: sungwoo48.choi
 */

#define LOG_TAG "RILC"

#include <android/hardware/radio/1.6/IRadio.h>
#include <android/hardware/radio/1.6/IRadioResponse.h>
#include <android/hardware/radio/1.6/IRadioIndication.h>
#include <android/hardware/radio/1.6/types.h>

#include "ril_service_common.h"
#include "ril_service_legacy.h"
#include "ril_service_1_6.h"

#include <hidl/HidlTransportSupport.h>

#include <slsi/radio_v1_6.h>
#include <librilutils/radio_conv_utils.h>
#include <librilutils/radio_hal_utils.h>
#include <librilutils/radio_hal_utils_1_6.h>

#if VDBG
static bool vdbg = true;
#else
static bool vdbg = false;
#endif

// HIDL HAL response
#include <hidl/1.6/hidlhalresponse_modem.h>
#include <hidl/1.6/hidlhalresponse_sim.h>
#include <hidl/1.6/hidlhalindication_sim.h>
#include <hidl/1.6/hidlhalresponse_messaging.h>
#include <hidl/1.6/hidlhalresponse_network.h>
#include <hidl/1.6/hidlhalindication_network.h>
#include <hidl/1.6/hidlhalresponse_data.h>
#include <hidl/1.6/hidlhalindication_data.h>
#include <hidl/1.6/hidlhalresponse_voice.h>

#define NUM_ELEMS(a)     (sizeof (a) / sizeof (a)[0])

using namespace radio::impl::v1_6;

using namespace android::hardware::radio;
using namespace android::hardware::radio::V1_0;
using ::android::hardware::configureRpcThreadpool;
using ::android::hardware::Return;
using ::android::hardware::hidl_array;
using ::android::hardware::hidl_bitfield;
using ::android::hardware::hidl_death_recipient;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Void;
using android::CommandInfo;
using android::UnsolResponseInfo;
using android::RequestInfo;
using android::requestToString;
using android::sp;
using android::wp;

#define HIDL_HAL_MODEM_RESPONSE(theHandler) \
    if (vdbg) RLOGD("radio::%s: serial %d", __FUNCTION__, serial); \
    if (radioService[slotId] == nullptr) { \
        RLOGW("HIDL_HAL_MODEM_RESPONSE: radioService[%d] is null.", slotId); return 0; } \
    return HidlHalModemResponse_1_6(radioService[slotId]->mHalResponse). \
                theHandler(slotId, responseType, serial, e, response, responselen); \

#define HIDL_HAL_MODEM_INDICATION(theHandler) \
    if (vdbg) RLOGD("radio::%s", __FUNCTION__); \
    if (radioService[slotId] == nullptr) { \
        RLOGW("HIDL_HAL_MODEM_INDICATION: radioService[%d] is null.", slotId); return 0; } \
    return HidlHalModemIndication_1_6(radioService[slotId]->mHalIndication). \
                theHandler(slotId, indicationType, token, e, response, responselen); \

#define HIDL_HAL_SIM_RESPONSE(theHandler) \
    if (vdbg) RLOGD("radio::%s: serial %d", __FUNCTION__, serial); \
    if (radioService[slotId] == nullptr) { \
        RLOGW("HIDL_HAL_SIM_RESPONSE: radioService[%d] is null.", slotId); return 0; } \
    return HidlHalSimResponse_1_6(radioService[slotId]->mHalResponse). \
                theHandler(slotId, responseType, serial, e, response, responseLen); \

#define HIDL_HAL_SIM_INDICATION(theHandler) \
    if (vdbg) RLOGD("radio::%s", __FUNCTION__); \
    if (radioService[slotId] == nullptr) { \
        RLOGW("HIDL_HAL_SIM_INDICATION: radioService[%d] is null.", slotId); return 0; } \
    return HidlHalSimIndication_1_6(radioService[slotId]->mHalIndication). \
                theHandler(slotId, indicationType, token, e, response, responseLen); \

#define HIDL_HAL_MESSAGING_RESPONSE(theHandler) \
    if (vdbg) RLOGD("radio::%s: serial %d", __FUNCTION__, serial); \
    if (radioService[slotId] == nullptr) { \
        RLOGW("HIDL_HAL_MESSAGING_RESPONSE: radioService[%d] is null.", slotId); return 0; } \
    return HidlHalMessagingResponse_1_6(radioService[slotId]->mHalResponse). \
                theHandler(slotId, responseType, serial, e, response, responseLen); \

#define HIDL_HAL_NETWORK_RESPONSE(theHandler) \
    if (vdbg) RLOGD("radio::%s: serial %d", __FUNCTION__, serial); \
    if (radioService[slotId] == nullptr) { \
        RLOGW("HIDL_HAL_NETWORK_RESPONSE: radioService[%d] is null.", slotId); return 0; } \
    return HidlHalNetworkResponse_1_6(radioService[slotId]->mHalResponse). \
                theHandler(slotId, responseType, serial, e, response, responselen); \

#define HIDL_HAL_NETWORK_INDICATION(theHandler) \
    if (vdbg) RLOGD("radio::%s", __FUNCTION__); \
    if (radioService[slotId] == nullptr) { \
        RLOGW("HIDL_HAL_NETWORK_INDICATION: radioService[%d] is null.", slotId); return 0; } \
    return HidlHalNetworkIndication_1_6(radioService[slotId]->mHalIndication). \
                theHandler(slotId, indicationType, token, e, response, responselen); \

#define HIDL_HAL_DATA_RESPONSE(theHandler) \
    if (vdbg) RLOGD("radio::%s: serial %d", __FUNCTION__, serial); \
    if (radioService[slotId] == nullptr) { \
        RLOGW("HIDL_HAL_DATA_RESPONSE: radioService[%d] is null.", slotId); return 0; } \
    return HidlHalDataResponse_1_6(radioService[slotId]->mHalResponse). \
                theHandler(slotId, responseType, serial, e, response, responseLen); \

#define HIDL_HAL_DATA_INDICATION(theHandler) \
    if (vdbg) RLOGD("radio::%s", __FUNCTION__); \
    if (radioService[slotId] == nullptr) { \
        RLOGW("HIDL_HAL_DATA_INDICATION: radioService[%d] is null.", slotId); return 0; } \
    return HidlHalDataIndication_1_6(radioService[slotId]->mHalIndication). \
               theHandler(slotId, indicationType, token, e, response, responseLen); \

#define HIDL_HAL_VOICE_RESPONSE(theHandler) \
    if (vdbg) RLOGD("radio::%s: serial %d", __FUNCTION__, serial); \
    if (radioService[slotId] == nullptr) { \
        RLOGW("HIDL_HAL_VOICE_RESPONSE: radioService[%d] is null.", slotId); return 0; } \
    return HidlHalVoiceResponse_1_6(radioService[slotId]->mHalResponse). \
                theHandler(slotId, responseType, serial, e, response, responseLen); \

#define HIDL_HAL_VOICE_INDICATION(theHandler) \
    if (vdbg) RLOGD("radio::%s", __FUNCTION__); \
    if (radioService[slotId] == nullptr) { \
        RLOGW("HIDL_HAL_VOICE_INDICATION: radioService[%d] is null.", slotId); return 0; } \
    return HidlHalVoiceIndication_1_6(radioService[slotId]->mHalIndication). \
                theHandler(slotId, indicationType, token, e, response, responseLen); \

namespace radio {
namespace impl {
namespace v1_6 {

struct RadioImpl;

#if (SIM_COUNT >= 2)
sp<RadioImpl> radioService[SIM_COUNT];
// counter used for synchronization. It is incremented every time response callbacks are updated.
volatile int32_t mCounterRadio[SIM_COUNT];
#else
sp<RadioImpl> radioService[1];
volatile int32_t mCounterRadio[1];
#endif

// Temporary compile option
#define CURRENT_RADIO_HAL "android::hardware::radio::V1_6::IRadio"
struct RadioImpl : public V1_6::IRadio {

    int32_t mSlotId;
    sp<V1_5::IRadio> mRadioLegacy;
    sp<IRadioResponse> mRadioResponse;
    sp<IRadioIndication> mRadioIndication;
    sp<V1_6::IRadioResponse> mRadioResponseV1_6;
    sp<V1_6::IRadioIndication> mRadioIndicationV1_6;

    HidlHalResponse_1_6 mHalResponse;
    HidlHalIndication_1_6 mHalIndication;

    Return<void> setResponseFunctions(
            const ::android::sp<IRadioResponse>& radioResponse,
            const ::android::sp<IRadioIndication>& radioIndication);

    // ril_legacy proxy functions
    #include "ril_service_legacy_proxy_fun.inc"

    ///////////////////////////////////////////////////////////////////////////
    // radio 1.6
    ///////////////////////////////////////////////////////////////////////////
    Return<void> getDataCallList_1_6(int32_t serial);
    Return<void> setupDataCall_1_6(int32_t serial, V1_5::AccessNetwork accessNetwork,
            const V1_5::DataProfileInfo& dataProfileInfo, bool roamingAllowed,
            V1_2::DataRequestReason reason, const hidl_vec<V1_5::LinkAddress>& addresses,
            const hidl_vec<hidl_string>& dnses, int32_t pduSessionId, const V1_6::OptionalSliceInfo& sliceInfo,
            const V1_6::OptionalTrafficDescriptor& trafficDescriptor, bool matchAllRuleAllowed);
    Return<void> setNrDualConnectivityState(int32_t serial, V1_6::NrDualConnectivityState nrDualConnectivityState);
    Return<void> isNrDualConnectivityEnabled(int32_t serial);
    Return<void> setRadioPower_1_6(int32_t serial, bool powerOn, bool forEmergencyCall, bool preferredForEmergencyCall);
    Return<void> sendSms_1_6(int32_t serial, const V1_0::GsmSmsMessage& message);
    Return<void> sendSmsExpectMore_1_6(int32_t serial, const V1_0::GsmSmsMessage& message);
    Return<void> sendCdmaSms_1_6(int32_t serial, const V1_0::CdmaSmsMessage& sms);
    Return<void> sendCdmaSmsExpectMore_1_6(int32_t serial, const V1_0::CdmaSmsMessage& sms);
    Return<void> setSimCardPower_1_6(int32_t serial, const V1_1::CardPowerState state);
    Return<void> allocatePduSessionId(int32_t serial);
    Return<void> releasePduSessionId(int32_t serial, int32_t id);
    Return<void> startHandover(int32_t serial, int32_t callId);
    Return<void> cancelHandover(int32_t serial, int32_t callId);
    Return<void> setAllowedNetworkTypesBitmap(uint32_t serial, hidl_bitfield<V1_4::RadioAccessFamily> networkTypeBitmap);
    Return<void> setDataThrottling(int32_t serial, V1_6::DataThrottlingAction dataThrottlingAction,
            int64_t completionDurationMillis);
    Return<void> getSystemSelectionChannels(int32_t serial);
    Return<void> getCurrentCalls_1_6(int32_t serial);
    Return<void> getVoiceRegistrationState_1_6(int32_t serial);
    Return<void> getDataRegistrationState_1_6(int32_t serial);
    Return<void> getCellInfoList_1_6(int32_t serial);
    Return<void> getSignalStrength_1_6(int32_t serial);
    Return<void> getAllowedNetworkTypesBitmap(int32_t serial);
    Return<void> emergencyDial_1_6(
        int32_t serial, const Dial &dialInfo,
        hidl_bitfield<V1_4::EmergencyServiceCategory> categories,
        const hidl_vec<hidl_string> &urns, V1_4::EmergencyCallRouting routing,
        bool hasKnownUserIntentEmergency, bool isTesting);
    Return<void> getSlicingConfig(int32_t serial);
    Return<void> setCarrierInfoForImsiEncryption_1_6(int32_t serial, const ::android::hardware::radio::V1_6::ImsiEncryptionInfo& imsiEncryptionInfo);
    Return<void> getSimPhonebookRecords(int32_t serial);
    Return<void> getSimPhonebookCapacity(int32_t serial);
    Return<void> updateSimPhonebookRecords(int32_t serial,
            const ::android::hardware::radio::V1_6::PhonebookRecordInfo& recordInfo);

    void checkReturnStatus(Return<void>& ret);

    struct LocalDeathRecipient : public hidl_death_recipient {
        RadioImpl& mRadio;
        LocalDeathRecipient(RadioImpl& radio) :  mRadio(radio) {}
        ~LocalDeathRecipient()=default;
        void serviceDied(uint64_t cookie, const wp<V1_6::IRadioResponse::IBase>& who);
    };
    sp<LocalDeathRecipient> mDeathRecipient;
};

void RadioImpl::LocalDeathRecipient::serviceDied(uint64_t cookie, const wp<V1_6::IRadioResponse::IBase>&/* who*/) {
    int slotId = mRadio.mSlotId;
    RLOGE("V1_6::serviceDied!!!: slotId=%d cookie=%u", slotId, (uint32_t)cookie);

    uint32_t counter = mCounterRadio[slotId];
    pthread_rwlock_t *radioServiceRwlockPtr = radio::getRadioServiceRwlock(slotId);
    // acquire wrlock
    int ret = pthread_rwlock_wrlock(radioServiceRwlockPtr);
    assert(ret == 0);

    // make sure the counter value has not changed
    if (counter == cookie) {
        mRadio.mRadioResponse = NULL;
        mRadio.mRadioIndication = NULL;
        mRadio.mRadioResponseV1_6 = NULL;
        mRadio.mRadioIndicationV1_6 = NULL;
    } else {
        RLOGE("V1_6::serviceDied: not resetting responseFunctions as they likely "
                "got updated on another thread");
    }

    // release wrlock
    ret = pthread_rwlock_unlock(radioServiceRwlockPtr);
    assert(ret == 0);
}

void checkReturnStatus(int32_t slotId, Return<void>& ret) {
    if (radioService[slotId] != nullptr) {
        radioService[slotId]->checkReturnStatus(ret);
    }
}

void RadioImpl::checkReturnStatus(Return<void>& ret) {
    if (ret.isOk() == false) {
        RLOGE("checkReturnStatus: unable to call response/indication callback.");
        RLOGE("checkReturnStatus: remote process might be died.");
    }
}

Return<void> RadioImpl::setResponseFunctions(
        const ::android::sp<IRadioResponse>& radioResponseParam,
        const ::android::sp<IRadioIndication>& radioIndicationParam) {
    RLOGD("%s", __FUNCTION__);
    RLOGD("%s radioResponseParam=%p radioIndicationParam=%p", __FUNCTION__, radioResponseParam.get(), radioIndicationParam.get());

    // for radio 1.6 at first
    pthread_rwlock_t *radioServiceRwlockPtr = radio::getRadioServiceRwlock(mSlotId);
    int ret = pthread_rwlock_wrlock(radioServiceRwlockPtr);
    assert(ret == 0);

    mRadioResponse = radioResponseParam;
    mRadioIndication = radioIndicationParam;

    mRadioResponseV1_6 = V1_6::IRadioResponse::castFrom(mRadioResponse).withDefault(nullptr);
    mRadioIndicationV1_6 = V1_6::IRadioIndication::castFrom(mRadioIndication).withDefault(nullptr);
    if (mRadioResponseV1_6 == nullptr || mRadioIndicationV1_6 == nullptr) {
        mRadioResponseV1_6 = nullptr;
        mRadioIndicationV1_6 = nullptr;
    } else {
        RLOGD("%s[%d]: V1_6 supportable", __FUNCTION__, mSlotId);
    }

    // HIDL response/indication
    // MUST update here.
    mHalResponse = HidlHalResponse_1_6(mRadioResponse);
    mHalIndication = HidlHalIndication_1_6(mRadioIndication);

    mCounterRadio[mSlotId]++;

    // linkToDeathRecipient
    if (mRadioResponseV1_6) {
        if (mDeathRecipient == nullptr) {
            mDeathRecipient = new LocalDeathRecipient(*this);
        }
        int32_t counter = mCounterRadio[mSlotId];
        mRadioResponseV1_6->linkToDeath(mDeathRecipient, counter);
        RLOGD("[%d] V1_6::setResponseFunctions: linkToDeath cookie=%d", mSlotId, counter);
    }

    ret = pthread_rwlock_unlock(radioServiceRwlockPtr);
    assert(ret == 0);

    // call legacy the next
    assert(mRadioLegacy != nullptr);
    mRadioLegacy->setResponseFunctions(radioResponseParam, radioIndicationParam);

    return Void();
}

// ril_legacy proxy body
#include "ril_service_legacy_proxy_body.inc"

///////////////////////////////////////////////////////////////////////////
// radio 1.6
///////////////////////////////////////////////////////////////////////////
Return<void> RadioImpl::getDataCallList_1_6(int32_t serial) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    int request = ENCODE_REQUEST(RIL_REQUEST_DATA_CALL_LIST, HAL_VERSION_CODE(1, 6));
    dispatchVoid(serial, mSlotId, request);
    return Void();
}

bool fillSetupDataCallInfo_V1_6(RIL_SetupDataCallInfo_V1_6 &setupDataCallInfo,
        int32_t pduSessionId, const V1_6::OptionalSliceInfo& sliceInfo,
        const V1_6::OptionalTrafficDescriptor& trafficDescriptor, bool matchAllRuleAllowed) {

    setupDataCallInfo.pduSessionId = pduSessionId;

    if (sliceInfo.getDiscriminator() !=
       V1_6::OptionalSliceInfo::hidl_discriminator::noinit) {
        V1_6::SliceInfo inputInfo = sliceInfo.value();
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

     if (trafficDescriptor.getDiscriminator() !=
        V1_6::OptionalTrafficDescriptor::hidl_discriminator::noinit) {
        static const int MAX_DNN_LEN = 101;
        static const int MAX_OSAPPID_LEN = 272;
         V1_6::TrafficDescriptor tdInfo = trafficDescriptor.value();
         setupDataCallInfo.OptionalTrafficDescriptorFlag = RIL_OPT_INFO_VALID;
         if(tdInfo.dnn.getDiscriminator() != V1_6::OptionalDnn::hidl_discriminator::noinit){
             RLOGD("%s: set dnn Info", __FUNCTION__);
             RIL_OptionalDNN dnn;
             memset(dnn.value, 0, sizeof(RIL_OptionalDNN));
             setupDataCallInfo.tdInfo.dnnFlag = RIL_OPT_INFO_VALID;
             const char* input = tdInfo.dnn.value().c_str();
             size_t dnnLength = tdInfo.dnn.value().size();
             dnnLength = dnnLength >= MAX_DNN_LEN ? MAX_DNN_LEN : dnnLength;
             setupDataCallInfo.tdInfo.dnn.clear();

             for (size_t i = 0; i < dnnLength; i++) dnn.value[i] = input[i];
             RLOGD("%s: set dnn Info , dnn string : %s", __FUNCTION__, dnn.value);
             setupDataCallInfo.tdInfo.dnn.push_back(dnn);
         } else {
             RLOGD("%s: no dnn info", __FUNCTION__);
             setupDataCallInfo.tdInfo.dnnFlag = RIL_OPT_INFO_INVALID;
         }
         if (tdInfo.osAppId.getDiscriminator() != V1_6::OptionalOsAppId::hidl_discriminator::noinit) {
             RLOGD("%s: set osAppId Info", __FUNCTION__);
             RIL_OptionalOSAppId osappid;
             memset(osappid.value, 0, sizeof(RIL_OptionalOSAppId));
             setupDataCallInfo.tdInfo.osAppIdFlag = RIL_OPT_INFO_VALID;
             size_t osAppIdLength = tdInfo.osAppId.value().osAppId.size();
             osAppIdLength = osAppIdLength >= MAX_OSAPPID_LEN ? MAX_OSAPPID_LEN : osAppIdLength;
             for(size_t i = 0; i < osAppIdLength; i++)
                 osappid.value[i] = tdInfo.osAppId.value().osAppId[i];
             setupDataCallInfo.tdInfo.osAppId.push_back(osappid);
         }else {
             RLOGD("%s: no osAppId info", __FUNCTION__);
             setupDataCallInfo.tdInfo.osAppIdFlag = RIL_OPT_INFO_INVALID;
         }
     } else {
         RLOGD("%s: no opt traffic info", __FUNCTION__);
         setupDataCallInfo.OptionalTrafficDescriptorFlag = RIL_OPT_INFO_INVALID;
         setupDataCallInfo.tdInfo.dnn.clear();
         setupDataCallInfo.tdInfo.osAppId.clear();
     }

    setupDataCallInfo.matchAllRuleAllowed = matchAllRuleAllowed;

    return true;
}

Return<void> RadioImpl::setupDataCall_1_6(int32_t serial, V1_5::AccessNetwork accessNetwork,
        const V1_5::DataProfileInfo& dataProfileInfo, bool roamingAllowed,
        V1_2::DataRequestReason reason, const hidl_vec<V1_5::LinkAddress>& addresses,
        const hidl_vec<hidl_string>& dnses, int32_t pduSessionId, const V1_6::OptionalSliceInfo& sliceInfo,
        const V1_6::OptionalTrafficDescriptor& trafficDescriptor, bool matchAllRuleAllowed) {
#if VDBG
    RLOGD("%s: %d", __FUNCTION__, serial);
#endif
    int request = ENCODE_REQUEST(RIL_REQUEST_SETUP_DATA_CALL, HAL_VERSION_CODE(1, 6));
    RequestInfo *pRI = android::addRequestToList(serial, mSlotId, request);
    if (pRI == NULL) {
        return Void();
    }

    RIL_SetupDataCallInfo_V1_6 setupDataCallInfo = {};

    bool ret = fillSetupDataCallInfo_V1_5(setupDataCallInfo,
                                   accessNetwork, dataProfileInfo,
                                   roamingAllowed, reason, addresses, dnses, pRI);
    if(!ret) return Void();

    fillSetupDataCallInfo_V1_6(setupDataCallInfo, pduSessionId, sliceInfo, trafficDescriptor, matchAllRuleAllowed);

    CALL_ONREQUEST(request, &setupDataCallInfo, sizeof(setupDataCallInfo), pRI, mSlotId);

    freeSetupDataCallInfo_V1_5(setupDataCallInfo, addresses);

    return Void();
}

Return<void> RadioImpl::setNrDualConnectivityState(int32_t serial, V1_6::NrDualConnectivityState nrDualConnectivityState) {
#if VDBG
    RLOGD("%s: %d", __FUNCTION__, serial);
#endif
    /* Convert NrDualConnectivityState values
     * RIL_REQUEST_ENABLE_NR_DUAL_CONNECTIVITY
     * 0 - DISABLE
     * 1 - ENABLE
     * 2 - DISABLE IMMEDIATE
     */
    int state = -1;
    switch(nrDualConnectivityState) {
      case V1_6::NrDualConnectivityState::DISABLE:
        state = 0;
        break;
      case V1_6::NrDualConnectivityState::ENABLE:
        state = 1;
        break;
      case V1_6::NrDualConnectivityState::DISABLE_IMMEDIATE:
        state = 2;
        break;
    }
    dispatchInts(serial, mSlotId, RIL_REQUEST_ENABLE_NR_DUAL_CONNECTIVITY, 1, state);
    return Void();
}

Return<void> RadioImpl::isNrDualConnectivityEnabled(int32_t serial) {
#if VDBG
    RLOGD("%s: %d", __FUNCTION__, serial);
#endif
    dispatchVoid(serial, mSlotId, RIL_REQUEST_IS_NR_DUAL_CONNECTIVITY_ENABLED);
    return Void();
}

Return<void> RadioImpl::setRadioPower_1_6(int32_t serial, bool powerOn, bool forEmergencyCall, bool preferredForEmergencyCall) {
#if VDBG
    RLOGD("%s: %d", __FUNCTION__, serial);
#endif
    int request = ENCODE_REQUEST(RIL_REQUEST_RADIO_POWER, HAL_VERSION_CODE(1,6));
    dispatchInts(serial, mSlotId, request , 3, BOOL_TO_INT(powerOn), BOOL_TO_INT(forEmergencyCall), BOOL_TO_INT(preferredForEmergencyCall));
    return Void();
}

Return<void> RadioImpl::sendSms_1_6(int32_t serial, const V1_0::GsmSmsMessage& message) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    int request = ENCODE_REQUEST(RIL_REQUEST_SEND_SMS, HAL_VERSION_CODE(1,6));
    dispatchStrings(serial, mSlotId, request, false, 2, message.smscPdu.c_str(), message.pdu.c_str());
    return Void();
}

Return<void> RadioImpl::sendSmsExpectMore_1_6(int32_t serial, const V1_0::GsmSmsMessage& message) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    int request = ENCODE_REQUEST(RIL_REQUEST_SEND_SMS_EXPECT_MORE, HAL_VERSION_CODE(1,6));
    dispatchStrings(serial, mSlotId, request, false, 2, message.smscPdu.c_str(), message.pdu.c_str());
    return Void();
}

Return<void> RadioImpl::sendCdmaSms_1_6(int32_t serial, const V1_0::CdmaSmsMessage& sms) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    int request = ENCODE_REQUEST(RIL_REQUEST_CDMA_SEND_SMS, HAL_VERSION_CODE(1,6));
    RequestInfo *pRI = android::addRequestToList(serial, mSlotId, request);
    if (pRI == NULL) {
        return Void();
    }

    RIL_CDMA_SMS_Message rcsm = {};
    constructCdmaSms(rcsm, sms);

    CALL_ONREQUEST(pRI->pCI->requestNumber, &rcsm, sizeof(rcsm), pRI, mSlotId);
    return Void();
}

Return<void> RadioImpl::sendCdmaSmsExpectMore_1_6(int32_t serial, const V1_0::CdmaSmsMessage& sms) {
#if VDBG
    RLOGD("%s: %d", __FUNCTION__, serial);
#endif
    int request = ENCODE_REQUEST(RIL_REQUEST_CDMA_SEND_SMS_EXPECT_MORE, HAL_VERSION_CODE(1,6));
    RequestInfo *pRI = android::addRequestToList(serial, mSlotId, request);
    if (pRI == NULL) {
        return Void();
    }

    RIL_CDMA_SMS_Message rcsm = {};
    constructCdmaSms(rcsm, sms);

    CALL_ONREQUEST(pRI->pCI->requestNumber, &rcsm, sizeof(rcsm), pRI, mSlotId);
    return Void();
}

Return<void> RadioImpl::setSimCardPower_1_6(int32_t serial, const V1_1::CardPowerState state) {
#if VDBG
    RLOGD("%s: serial %d state %d", __FUNCTION__, serial, state);
#endif
    int request = ENCODE_REQUEST(RIL_REQUEST_SET_SIM_CARD_POWER, HAL_VERSION_CODE(1, 6));
    dispatchInts(serial, mSlotId, request, 1, state);
    return Void();
}

Return<void> RadioImpl::allocatePduSessionId(int32_t serial) {
#if VDBG
    RLOGD("%s: %d", __FUNCTION__, serial);
#endif
    dispatchVoid(serial, mSlotId, RIL_REQUEST_ALLOCATE_PDU_SESSION_ID);
    return Void();
}

Return<void> RadioImpl::releasePduSessionId(int32_t serial, int32_t id) {
#if VDBG
    RLOGD("%s: %d", __FUNCTION__, serial);
#endif
    dispatchInts(serial, mSlotId, RIL_REQUEST_RELEASE_PDU_SESSION_ID, 1, id);
    return Void();
}

Return<void> RadioImpl::startHandover(int32_t serial, int32_t callId) {
#if VDBG
    RLOGD("%s: %d", __FUNCTION__, serial);
#endif
    dispatchInts(serial, mSlotId, RIL_REQUEST_START_HANDOVER, 1, callId);
    return Void();
}

Return<void> RadioImpl::cancelHandover(int32_t serial, int32_t callId) {
#if VDBG
    RLOGD("%s: %d", __FUNCTION__, serial);
#endif
    dispatchInts(serial, mSlotId, RIL_REQUEST_CANCEL_HANDOVER, 1, callId);
    return Void();
}

Return<void> RadioImpl::setAllowedNetworkTypesBitmap(uint32_t serial, hidl_bitfield<V1_4::RadioAccessFamily> networkTypeBitmap) {
#if VDBG
    RLOGD("%s: %d", __FUNCTION__, serial);
#endif
    dispatchInts(serial, mSlotId, RIL_REQUEST_SET_ALLOWED_NETWORK_TYPES_BITMAP, 1, networkTypeBitmap);
    return Void();
}

Return<void> RadioImpl::setDataThrottling(int32_t serial, V1_6::DataThrottlingAction dataThrottlingAction,
        int64_t completionDurationMillis) {
#if VDBG
    RLOGD("%s: %d", __FUNCTION__, serial);
#endif
    dispatchDataThrottling(serial, mSlotId, RIL_REQUEST_SET_DATA_THROTTLING, dataThrottlingAction, completionDurationMillis);
    return Void();
}

Return<void> RadioImpl::getSystemSelectionChannels(int32_t serial) {
#if VDBG
    RLOGD("%s: %d", __FUNCTION__, serial);
#endif
    dispatchVoid(serial, mSlotId, RIL_REQUEST_GET_SYSTEM_SELECTION_CHANNELS);
    return Void();
}

Return<void> RadioImpl::getCurrentCalls_1_6(int32_t serial) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    int request = ENCODE_REQUEST(RIL_REQUEST_GET_CURRENT_CALLS, HAL_VERSION_CODE(1,6));
    dispatchVoid(serial, mSlotId, request);
    return Void();
}

Return<void> RadioImpl::getVoiceRegistrationState_1_6(int32_t serial) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    int request = ENCODE_REQUEST(RIL_REQUEST_VOICE_REGISTRATION_STATE, HAL_VERSION_CODE(1,6));
    dispatchVoid(serial, mSlotId, request);
    return Void();
}

Return<void> RadioImpl::getDataRegistrationState_1_6(int32_t serial) {
#if VDBG
    RLOGD("%s: %d", __FUNCTION__, serial);
#endif
    int request = ENCODE_REQUEST(RIL_REQUEST_DATA_REGISTRATION_STATE, HAL_VERSION_CODE(1,6));
    dispatchVoid(serial, mSlotId, request);
    return Void();
}

Return<void> RadioImpl::getCellInfoList_1_6(int32_t serial) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    //FIXME: use a legacy request temporary
    dispatchVoid(serial, mSlotId, ENCODE_REQUEST(RIL_REQUEST_GET_CELL_INFO_LIST, HAL_VERSION_CODE(1,6)));
    return Void();
}

Return<void> RadioImpl::getSignalStrength_1_6(int32_t serial) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchVoid(serial, mSlotId, ENCODE_REQUEST(RIL_REQUEST_SIGNAL_STRENGTH, HAL_VERSION_CODE(1,6)));
    return Void();
}

Return<void> RadioImpl::getAllowedNetworkTypesBitmap(int32_t serial) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchVoid(serial, mSlotId, RIL_REQUEST_GET_ALLOWED_NETWORK_TYPES_BITMAP);
    return Void();
}

Return<void> RadioImpl::emergencyDial_1_6(int32_t serial, const Dial& dialInfo,
        hidl_bitfield<V1_4::EmergencyServiceCategory> categories,
        const hidl_vec<hidl_string>& urns, V1_4::EmergencyCallRouting routing,
        bool hasKnownUserIntentEmergency, bool isTesting) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    int request = ENCODE_REQUEST(RIL_REQUEST_EMERGENCY_DIAL, HAL_VERSION_CODE(1,6));
    sendEmergencyDial(serial, dialInfo, categories, urns, routing, hasKnownUserIntentEmergency, isTesting, request, mSlotId);
    return Void();
}

Return<void> RadioImpl::getSlicingConfig(int32_t serial) {
#if VDBG
        RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
        int request = ENCODE_REQUEST(RIL_REQUEST_SLICING_CONFIG, HAL_VERSION_CODE(1, 6));
        dispatchVoid(serial, mSlotId, request);
        return Void();
}

Return<void> RadioImpl::setCarrierInfoForImsiEncryption_1_6(int32_t serial, const ::android::hardware::radio::V1_6::ImsiEncryptionInfo& imsiEncryptionInfo) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif

    int request = ENCODE_REQUEST(RIL_REQUEST_SET_CARRIER_INFO_IMSI_ENCRYPTION, HAL_VERSION_CODE(1,6));
    RequestInfo *pRI = android::addRequestToList(serial, mSlotId, request);
    if (pRI == NULL) {
        return Void();
    }

    RIL_CarrierInfoForImsiEncryption_1_6 imsiEncryption = {};

    if (!copyHidlStringToRil(&imsiEncryption.mnc, imsiEncryptionInfo.base.mnc, pRI)) {
        return Void();
    }
    if (!copyHidlStringToRil(&imsiEncryption.mcc, imsiEncryptionInfo.base.mcc, pRI)) {
        memsetAndFreeStrings(1, imsiEncryption.mnc);
        return Void();
    }
    if (!copyHidlStringToRil(&imsiEncryption.keyIdentifier, imsiEncryptionInfo.base.keyIdentifier, pRI)) {
        memsetAndFreeStrings(2, imsiEncryption.mnc, imsiEncryption.mcc);
        return Void();
    }
    imsiEncryption.carrierKeyLength = imsiEncryptionInfo.base.carrierKey.size();
    imsiEncryption.carrierKey = new uint8_t[imsiEncryption.carrierKeyLength];
    memcpy(imsiEncryption.carrierKey, imsiEncryptionInfo.base.carrierKey.data(), imsiEncryption.carrierKeyLength);
    imsiEncryption.expirationTime = imsiEncryptionInfo.base.expirationTime;
    imsiEncryption.keyType = (PublicKeyType)imsiEncryptionInfo.keyType;

    CALL_ONREQUEST(pRI->pCI->requestNumber, &imsiEncryption,
            sizeof(RIL_CarrierInfoForImsiEncryption_1_6), pRI, mSlotId);
    delete[](imsiEncryption.carrierKey);
    memsetAndFreeStrings(3, imsiEncryption.mnc, imsiEncryption.mcc, imsiEncryption.keyIdentifier);
    return Void();
}

Return<void> RadioImpl::getSimPhonebookRecords(int32_t serial) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchVoid(serial, mSlotId, RIL_REQUEST_GET_SIM_PHONEBOOK_RECORDS);
    return Void();
}

Return<void> RadioImpl::getSimPhonebookCapacity(int32_t serial) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchVoid(serial, mSlotId, RIL_REQUEST_GET_SIM_PHONEBOOK_CAPACITY);
    return Void();
}

Return<void> RadioImpl::updateSimPhonebookRecords(int32_t serial, const V1_6::PhonebookRecordInfo& pbRecordInfo) {
#if VDBG
    RLOGD("%s: %d", __FUNCTION__, serial);
#endif
    RequestInfo *pRI = android::addRequestToList(serial, mSlotId, RIL_REQUEST_UPDATE_SIM_PHONEBOOK_RECORDS);
    if (pRI == NULL) {
        return Void();
    }

    int length = 0, pb_length = 0;
    RIL_UpdatePbEntry updateReq = {};
    length = sizeof(RIL_UpdatePbEntry) - sizeof(RIL_3GPb *);
    updateReq.type = PB_ADN_3G;
    updateReq.recordId = pbRecordInfo.recordId;

    if (updateReq.recordId == 0) {
        updateReq.mode = PB_INSERT;
    } else {
        if (pbRecordInfo.name.size() == 0 && pbRecordInfo.number.size() == 0) {
            updateReq.mode = PB_DELETE;
        } else {
            updateReq.mode = PB_UPDATE;
        }
    }

    int cnt = 0;
    if (updateReq.mode == PB_INSERT || updateReq.mode == PB_UPDATE) {
        // allocate pb3g as pbRecordInfo data size.
        int emailSize = (int) pbRecordInfo.emails.size();
        int anrSize = (int) pbRecordInfo.additionalNumbers.size();
        int data_size = 2 + emailSize + anrSize;
        // check pbRecordInfo.name string encoding is UTF-8 or GSM7BIT
        int strSize = pbRecordInfo.name.size();
        int numSize = pbRecordInfo.number.size();
        if (strSize == 0 || numSize == 0) {
            RLOGE("%s, strSize[%d] or numSize[%d] is Invalid!!", __FUNCTION__,
                    strSize, numSize);
            return Void();
        }

        RLOGD("%s, Received name:%s", __FUNCTION__, pbRecordInfo.name.c_str());
        updateReq.pb3g = new RIL_3GPb[data_size] {};

        char ucs2Data[MAX_NAME_LEN] = { 0, };
        int maxUcs2DataLen = MAX_NAME_LEN / 2;
        int wordCnt = Radio2HalUtils::convertStringToAdnString(pbRecordInfo.name,
                (unsigned char *) ucs2Data, maxUcs2DataLen);

        // name copy
        updateReq.pb3g[cnt].type3g = PB_NAME;
        if (wordCnt < strSize) {
            updateReq.pb3g[cnt].data_type = UCS2;
            updateReq.pb3g[cnt].data_len = (wordCnt * 2);
            updateReq.pb3g[cnt].data = (char *) malloc(updateReq.pb3g[cnt].data_len);
            memcpy(updateReq.pb3g[cnt].data, ucs2Data, updateReq.pb3g[cnt].data_len);
            RLOGD("encoded name:%s", updateReq.pb3g[cnt].data);
        } else {
            updateReq.pb3g[cnt].data_type = ASCII;
            updateReq.pb3g[cnt].data_len = pbRecordInfo.name.size();
            if (!copyHidlStringToRil(&updateReq.pb3g[cnt].data, pbRecordInfo.name, pRI)) {
                RLOGE("encoded name is invalid");
                return Void();
            } else {
                RLOGD("encoded name:%s", updateReq.pb3g[cnt].data);
            }
        }

        pb_length += updateReq.pb3g[cnt].data_len;
        RLOGD("%s, word cnt:%d, name type:%d, length:%d", __FUNCTION__,
                wordCnt, updateReq.pb3g[cnt].data_type, updateReq.pb3g[cnt].data_len);
        pb_length += (sizeof(int) * 3);
        cnt++;

        // number copy
        updateReq.pb3g[cnt].type3g = PB_NUMBER;
        updateReq.pb3g[cnt].data_len = pbRecordInfo.number.size();
        if (updateReq.pb3g[cnt].data_len > 0) {
            updateReq.pb3g[cnt].data_type = 0x80 | (UNKNOWN_TYPE << 4) | TELEPHONY_NUMBERING_PLAN;
            pb_length += updateReq.pb3g[cnt].data_len;
            if (!copyHidlStringToRil(&updateReq.pb3g[cnt].data, pbRecordInfo.number, pRI)) {
                RLOGE("number is invalid" );
                return Void();
            }
        }

        pb_length += (sizeof(int) * 3);
        cnt++;

        int emailTagArr[MAX_EF_SET] = {(int) PB_EMAIL, (int) PB_EMAILA, (int) PB_EMAILB, (int) PB_EMAILC};
        int anrTagArr[MAX_EF_SET] = {(int) PB_ANR, (int) PB_ANRA, (int) PB_ANRB, (int) PB_ANRC};
        // copy email
        if (emailSize > 0) {
            emailSize = (emailSize > MAX_EF_SET ? MAX_EF_SET : emailSize);
            for (int i = 0; i < emailSize; i++) {
                updateReq.pb3g[cnt].type3g = emailTagArr[i];
                updateReq.pb3g[cnt].data_len = pbRecordInfo.emails[i].size();
                if (updateReq.pb3g[cnt].data_len > 0) {
                    updateReq.pb3g[cnt].data_type = GSM7BIT;
                    pb_length += updateReq.pb3g[cnt].data_len;
                    if (!copyHidlStringToRil(&updateReq.pb3g[cnt].data, pbRecordInfo.emails[i], pRI)) {
                        RLOGE("email address is invalid" );
                    }
                }
                pb_length += (sizeof(int) * 3);
                cnt++;
            }
        }

        // copy anr
        if (anrSize > 0) {
            anrSize = (anrSize > MAX_EF_SET ? MAX_EF_SET : anrSize);
            for (int i = 0; i < anrSize; i++) {
                updateReq.pb3g[cnt].type3g = anrTagArr[i];
                updateReq.pb3g[cnt].data_len = pbRecordInfo.additionalNumbers[i].size() + 1;
                if (updateReq.pb3g[cnt].data_len > 0) {
                    updateReq.pb3g[cnt].data_type
                            = 0x80 | (UNKNOWN_TYPE << 4) | TELEPHONY_NUMBERING_PLAN;
                    updateReq.pb3g[cnt].data = (char *) malloc(updateReq.pb3g[cnt].data_len);
                    // TODO: anr type should be added to PhonebookRecordInfo if it is needed.
                    // anr type
                    updateReq.pb3g[cnt].data[0] = 0;
                    // anr data
                    memcpy(updateReq.pb3g[cnt].data + 1, pbRecordInfo.additionalNumbers[i].c_str(),
                            updateReq.pb3g[cnt].data_len - 1);
                    pb_length += updateReq.pb3g[cnt].data_len;
                }

                pb_length += (sizeof(int) * 3);
                cnt++;
            }
        }

        updateReq.pb3gCnt = cnt;
        // update length of pb to update.
        updateReq.length = pb_length;
        length += pb_length;
        RLOGD("%s: %s, recordId:%d, cnt:%d, pb_length:%d",
                __FUNCTION__, (updateReq.mode == PB_INSERT ? "PB_INSERT" : "PB_UPDATE"),
                updateReq.recordId, cnt, pb_length);
    } else {
        // PB_DELETE
        updateReq.length = 0;
        updateReq.pb3g = NULL;
        RLOGD("%s: PB_DELETE, recordId:%d", __FUNCTION__, updateReq.recordId);
    }

    CALL_ONREQUEST(pRI->pCI->requestNumber, &updateReq, length, pRI, mSlotId);

    if (updateReq.pb3g != NULL) {
        for (int i = 0; i < cnt; i++) {
            if (updateReq.pb3g[i].data) {
                free(updateReq.pb3g[i].data);
            }
        }
        delete[] updateReq.pb3g;
        updateReq.pb3g = NULL;
    }

    return Void();
}

}  // v1_6
}  // impl
}  // radio

bool dispatchDataThrottling(int serial, int slotId, int request, V1_6::DataThrottlingAction dataThrottlingAction,
            int64_t completionDurationMillis) {
    RequestInfo *pRI = android::addRequestToList(serial, slotId, request);
    if (pRI == NULL) {
        return false;
    }

    RIL_DataThrottling dataThrottling = {};

    dataThrottling.dataThrottlingAction = (RIL_DataThrottlingAction)dataThrottlingAction;
    dataThrottling.completionDurationMillis = completionDurationMillis;

    CALL_ONREQUEST(request, &dataThrottling, sizeof(dataThrottling), pRI, slotId);

    return true;
}

void populateResponseInfo(V1_6::RadioResponseInfo& responseInfo, int serial, int responseType,
                         RIL_Errno e) {
    responseInfo.serial = serial;
    switch (responseType) {
        case RESPONSE_SOLICITED:
            responseInfo.type = RadioResponseType::SOLICITED;
            break;
        case RESPONSE_SOLICITED_ACK_EXP:
            responseInfo.type = RadioResponseType::SOLICITED_ACK_EXP;
            break;
    }
    responseInfo.error = (V1_6::RadioError) e;
}

int responseInt(V1_6::RadioResponseInfo& responseInfo, int serial, int responseType, RIL_Errno e,
               void *response, size_t responseLen) {
    populateResponseInfo(responseInfo, serial, responseType, e);
    int ret = -1;

    if (response == NULL || responseLen != sizeof(int)) {
        RLOGE("responseInt: Invalid response");
        if (e == RIL_E_SUCCESS) responseInfo.error = V1_6::RadioError::INVALID_RESPONSE;
    } else {
        int *p_int = (int *) response;
        ret = p_int[0];
    }
    return ret;
}

SendSmsResult makeSendSmsResult(V1_6::RadioResponseInfo& responseInfo, int serial, int responseType,
                                RIL_Errno e, void *response, size_t responseLen) {
    populateResponseInfo(responseInfo, serial, responseType, e);
    SendSmsResult result = {};

    if (response == NULL || responseLen != sizeof(RIL_SMS_Response)) {
        RLOGE("Invalid response: NULL");
        if (e == RIL_E_SUCCESS) responseInfo.error = V1_6::RadioError::INVALID_RESPONSE;
        result.ackPDU = hidl_string();
    } else {
        RIL_SMS_Response *resp = (RIL_SMS_Response *) response;
        result.messageRef = resp->messageRef;
        result.ackPDU = Radio2HalUtils::convertCharPtrToHidlString(resp->ackPDU);
        result.errorCode = resp->errorCode;
    }
    return result;
}


///////////////////////////////////////////////////////////////////////////
// radio 1.6
///////////////////////////////////////////////////////////////////////////
int radio::setupDataCallResponse_1_6(int slotId,
                            int responseType, int serial, RIL_Errno e, void *response,
                            size_t responselen) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif

    if (radioService[slotId] != NULL && radioService[slotId]->mRadioResponseV1_6 != NULL) {
        V1_6::RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);

        V1_6::SetupDataCallResult dcResult = {};
        // User v11 format, For V1_4, only two parameter are removed, and just type change for Cause
        if (response == NULL) {
            RLOGE("%s: Invalid response", __FUNCTION__);
            if (e == RIL_E_SUCCESS) responseInfo.error = V1_6::RadioError::INVALID_RESPONSE;
            dcResult.cause = V1_6::DataCallFailCause::ERROR_UNSPECIFIED;
            dcResult.type = V1_4::PdpProtocolType::UNKNOWN;
            dcResult.ifname = hidl_string();
            dcResult.addresses.resize(0);
            dcResult.dnses.resize(0);
            dcResult.gateways.resize(0);
            dcResult.pcscf.resize(0);
        } else {
            RLOGE("%s: convertRilDataCallToHal", __FUNCTION__);
            Radio2HalUtils::convertRilDataCallToHal(*static_cast<RIL_SetupDataCallResult_V1_6 *>(response), dcResult);
        }

        Return<void> retStatus = radioService[slotId]->mRadioResponseV1_6->setupDataCallResponse_1_6(responseInfo, dcResult);
        radioService[slotId]->checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponseV1_6 == NULL", __FUNCTION__, slotId);
        return radio::setupDataCallResponse_1_5(slotId, responseType, serial, e, response, responselen);
    }
    return 0;
}

int radio::getDataCallListResponse_1_6(int slotId,
                            int responseType, int serial, RIL_Errno e,
                            void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    int ret = 0;
    if (radioService[slotId] != NULL && radioService[slotId]->mRadioResponseV1_6 != NULL) {
        V1_6::RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);

        hidl_vec<V1_6::SetupDataCallResult> dcList;
        if ((response == NULL && responseLen != 0)
            || responseLen % sizeof(RIL_SetupDataCallResult_V1_6) != 0 ) {
            RLOGE("%s: invalid response", __FUNCTION__);
            if (e == RIL_E_SUCCESS) responseInfo.error = V1_6::RadioError::INVALID_RESPONSE;
        } else {
            Radio2HalUtils::convertRilDataCallListToHal(response, responseLen, dcList);
        }
        Return<void> retStatus = radioService[slotId]->mRadioResponseV1_6->getDataCallListResponse_1_6(
                    responseInfo, dcList);
        radioService[slotId]->checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponseV1_6 == NULL fallback to 1_5", __FUNCTION__, slotId);
        ret = radio::getDataCallListResponse_1_5(slotId, responseType, serial, e, response, responseLen);
    }
    return ret;
}

int radio::dataCallListChangedInd_1_6(int slotId, int indicationType,
                           int token, RIL_Errno e, void *response, size_t responseLen) {
    HIDL_HAL_DATA_INDICATION(dataCallListChangedInd_1_6)
}

int radio::currentLinkCapacityEstimate_1_6(int slotId, int indicationType,
                           int token, RIL_Errno e, void *response, size_t responselen) {
    HIDL_HAL_NETWORK_INDICATION(currentLinkCapacityEstimate_1_6)
}

int radio::setNrDualConnectivityStateResponse(int slotId, int responseType,
                            int serial, RIL_Errno e, void *response, size_t responselen) {
    HIDL_HAL_NETWORK_RESPONSE(setNrDualConnectivityStateResponse)
}

int radio::isNrDualConnectivityEnabledResponse(int slotId, int responseType,
                            int serial, RIL_Errno e, void *response, size_t responselen) {
    HIDL_HAL_NETWORK_RESPONSE(isNrDualConnectivityEnabledResponse)
}

int radio::setRadioPowerResponse_1_6(int slotId, int responseType,
                            int serial, RIL_Errno e, void *response, size_t responselen) {
    HIDL_HAL_MODEM_RESPONSE(setRadioPowerResponse_1_6)
}

int radio::sendSmsResponse_1_6(int slotId, int responseType,
                            int serial, RIL_Errno e, void *response, size_t responseLen) {
    HIDL_HAL_MESSAGING_RESPONSE(sendSmsResponse_1_6)
}

int radio::sendSmsExpectMoreResponse_1_6(int slotId, int responseType,
                            int serial, RIL_Errno e, void *response, size_t responseLen) {
    HIDL_HAL_MESSAGING_RESPONSE(sendSmsExpectMoreResponse_1_6)
}

int radio::sendCdmaSmsResponse_1_6(int slotId, int responseType,
                            int serial, RIL_Errno e, void *response, size_t responseLen) {
    HIDL_HAL_MESSAGING_RESPONSE(sendCdmaSmsResponse_1_6)
}

int radio::sendCdmaSmsExpectMoreResponse_1_6(int slotId, int responseType,
                            int serial, RIL_Errno e, void *response, size_t responseLen) {
    HIDL_HAL_MESSAGING_RESPONSE(sendCdmaSmsExpectMoreResponse_1_6)
}

int radio::setSimCardPowerResponse_1_6(int slotId, int responseType,
                            int serial, RIL_Errno e, void *response, size_t responseLen) {
    HIDL_HAL_SIM_RESPONSE(setSimCardPowerResponse_1_6)
}

int radio::allocatePduSessionIdResponse(int slotId, int responseType,
                            int serial, RIL_Errno e, void *response, size_t responseLen) {
    HIDL_HAL_DATA_RESPONSE(allocatePduSessionIdResponse)
}

int radio::releasePduSessionIdResponse(int slotId, int responseType,
                            int serial, RIL_Errno e, void *response, size_t responseLen) {
    HIDL_HAL_DATA_RESPONSE(releasePduSessionIdResponse)
}

int radio::startHandoverResponse(int slotId, int responseType,
                            int serial, RIL_Errno e, void *response, size_t responseLen) {
    HIDL_HAL_DATA_RESPONSE(startHandoverResponse)
}

int radio::cancelHandoverResponse(int slotId, int responseType,
                            int serial, RIL_Errno e, void *response, size_t responseLen) {
    HIDL_HAL_DATA_RESPONSE(cancelHandoverResponse)
}

int radio::setAllowedNetworkTypesBitmapResponse(int slotId, int responseType,
                            int serial, RIL_Errno e, void *response, size_t responselen) {
    HIDL_HAL_NETWORK_RESPONSE(setAllowedNetworkTypesBitmapResponse)
}

int radio::setDataThrottlingResponse(int slotId, int responseType,
                            int serial, RIL_Errno e, void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    if (radioService[slotId] != NULL && radioService[slotId]->mRadioResponseV1_6 != NULL) {
        V1_6::RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus
            = radioService[slotId]->mRadioResponseV1_6->setDataThrottlingResponse(responseInfo);
        radioService[slotId]->checkReturnStatus(retStatus);
    }
    else {
        RLOGE("%s: radioService[%d]->mRadioResponseV1_6 == NULL", __FUNCTION__, slotId);
    }
    return 0;
}

int radio::getSystemSelectionChannelsResponse(int slotId, int responseType,
                            int serial, RIL_Errno e, void *response, size_t responselen) {
    HIDL_HAL_NETWORK_RESPONSE(getSystemSelectionChannelsResponse)
}

int radio::unthrottleApn(int slotId, int indicationType,
                           int token, RIL_Errno e, void *response, size_t responseLen) {
    if (radioService[slotId] != NULL && radioService[slotId]->mRadioIndicationV1_6 != nullptr) {
        if (response == NULL) {
            RLOGE("%s: invalid response", __FUNCTION__);
            return 0;
        }
#if VDBG
    RLOGD("%s", __FUNCTION__);
#endif
        Return<void> retStatus = radioService[slotId]->mRadioIndicationV1_6->unthrottleApn(
                    convertIntToRadioIndicationType(indicationType), Radio2HalUtils::convertCharPtrToHidlString((char *) response));
        radioService[slotId]->checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioIndicationV1_6 == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int radio::currentPhysicalChannelConfigs_1_6(int slotId, int indicationType,
                            int token, RIL_Errno e, void *response, size_t responselen) {
    HIDL_HAL_NETWORK_INDICATION(currentPhysicalChannelConfigs_1_6);
}

int radio::getCurrentCallsResponse_1_6(int slotId,
                                  int responseType, int serial, RIL_Errno e,
                                  void *response, size_t responseLen) {
    HIDL_HAL_VOICE_RESPONSE(getCurrentCallsResponse_1_6)
}

int radio::getVoiceRegistrationStateResponse_1_6(int slotId,
                            int responseType, int serial, RIL_Errno e, void *response,
                            size_t responselen) {
    HIDL_HAL_NETWORK_RESPONSE(getVoiceRegistrationStateResponse_1_6)
}

int radio::getDataRegistrationStateResponse_1_6(int slotId,
                            int responseType, int serial, RIL_Errno e, void *response,
                            size_t responselen) {
    HIDL_HAL_NETWORK_RESPONSE(getDataRegistrationStateResponse_1_6)
}

int radio::getAllowedNetworkTypesBitmapResponse(int slotId, int responseType,
                            int serial, RIL_Errno e, void *response, size_t responselen) {
    HIDL_HAL_NETWORK_RESPONSE(getAllowedNetworkTypesBitmapResponse)
}

int radio::getSignalStrengthResponse_1_6(int slotId, int responseType,
                            int serial, RIL_Errno e, void *response, size_t responselen) {
    HIDL_HAL_NETWORK_RESPONSE(getSignalStrengthResponse_1_6)
}

int radio::getSlicingConfigResponse(int slotId, int responseType, int serial,
                                        RIL_Errno e, void* response, size_t responseLen) {
#if VDBG
    RLOGD("getSlicingConfigResponse: serial %d", serial);
#endif
    if (radioService[slotId] != NULL && radioService[slotId]->mRadioResponseV1_6 != NULL) {
        V1_6::RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);

        V1_6::SlicingConfig slicingConfig = {};

        if (response == NULL) {
            RLOGE("%s: Invalid response", __FUNCTION__);
            if (e == RIL_E_SUCCESS){
                responseInfo.error = V1_6::RadioError::INVALID_RESPONSE;
            }else if(e == RIL_E_MODEM_ERR){
                responseInfo.error = V1_6::RadioError::MODEM_ERR;
            }
        } else {
            Radio2HalUtils::convertRilSlicingConfigToHal(*static_cast<RIL_SlicingConfig*>(response), slicingConfig);
        }

        Return<void> retStatus = radioService[slotId]->mRadioResponseV1_6->
                getSlicingConfigResponse(responseInfo, slicingConfig);
        radioService[slotId]->checkReturnStatus(retStatus);
    } else {
        RLOGE("getSlicingConfigResponse: radioService[%d]->mRadioResponse == NULL", slotId);
    }

    return 0;
}

int radio::currentSignalStrengthInd_1_6(int slotId,
                                    int indicationType, int token, RIL_Errno e,
                                    void *response, size_t responselen) {
    HIDL_HAL_NETWORK_INDICATION(currentSignalStrengthInd_1_6)
}

int radio::getCellInfoListResponse_1_6(int slotId,
                                   int responseType,
                                   int serial, RIL_Errno e, void *response,
                                   size_t responselen) {
    HIDL_HAL_NETWORK_RESPONSE(getCellInfoListResponse_1_6)
}

int radio::cellInfoListInd_1_6(int slotId,
                            int indicationType, int token, RIL_Errno e, void *response,
                            size_t responselen) {
    HIDL_HAL_NETWORK_INDICATION(cellInfoListInd_1_6)
}

int radio::networkScanResultInd_1_6(int slotId,
                            int indicationType, int token, RIL_Errno e, void *response,
                            size_t responselen) {
    HIDL_HAL_NETWORK_INDICATION(networkScanResultInd_1_6);
}

int radio::getSimPhonebookRecordsResponse(int slotId, int responseType,
                            int serial, RIL_Errno e, void *response, size_t responseLen) {
    HIDL_HAL_SIM_RESPONSE(getSimPhonebookRecordsResponse)
}

int radio::getSimPhonebookCapacityResponse(int slotId, int responseType,
                            int serial, RIL_Errno e, void *response, size_t responseLen) {
    HIDL_HAL_SIM_RESPONSE(getSimPhonebookCapacityResponse)
}

int radio::updateSimPhonebookRecordsResponse(int slotId, int responseType,
                            int serial, RIL_Errno e, void *response, size_t responseLen) {
    HIDL_HAL_SIM_RESPONSE(updateSimPhonebookRecordsResponse)
}

int radio::simPhonebookChanged(int slotId, int indicationType,
                           int token, RIL_Errno e, void *response, size_t responseLen) {
    HIDL_HAL_SIM_INDICATION(simPhonebookChanged);
}

int radio::simPhonebookRecordsReceived(int slotId, int indicationType,
                           int token, RIL_Errno e, void *response, size_t responseLen) {
    HIDL_HAL_SIM_INDICATION(simPhonebookRecordsReceived);
}

void radio::impl::v1_6::registerAsService() {
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

    // init V1_6 and legacy radios
    initRadios();

    configureRpcThreadpool(1, false /* callerWillJoin */);
    for (int i = 0; i < simCount; i++) {
        pthread_rwlock_t *radioServiceRwlockPtr = getRadioServiceRwlock(i);
        int ret = pthread_rwlock_wrlock(radioServiceRwlockPtr);
        assert(ret == 0);

        sp<RadioImpl> radio = radioService[i];
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
    #include "radio/1.6/ril_commands.h"
};

// update unsolicited response handlers
static UnsolResponseInfo s_unsolResponses[] = {
    #include "radio/1.6/ril_unsol_commands.h"
};

void updateCommandInfo_1_6() {
    android::updateCommandInfo(s_commands, NUM_ELEMS(s_commands));
    android::updateUnsolResponseInfo(s_unsolResponses, NUM_ELEMS(s_unsolResponses));
}

void radio::impl::v1_6::initRadios() {
    int simCount = 1;
    #if (SIM_COUNT >= 2)
    simCount = SIM_COUNT;
    #endif


    // init legacy radios at first
    radio::impl::legacy::initRadios();

    // update solicited and unsolicited response handlers for V1_6 radio
    updateCommandInfo_1_6();

    // create radio instance
    for (int i = 0; i < simCount; i++) {
        pthread_rwlock_t *radioServiceRwlockPtr = getRadioServiceRwlock(i);
        int ret = pthread_rwlock_wrlock(radioServiceRwlockPtr);
        assert(ret == 0);

        radioService[i] = new RadioImpl;
        radioService[i]->mSlotId = i;

        // set a legacy radio for each slotId
        // a legacy radio must not be null and must be an instance of V1_5 radio.
        android::sp<::android::hardware::radio::V1_0::IRadio> radioLegacy =
                radio::impl::legacy::getRadio(i);
        assert(radioLegacy != nullptr);
        radioService[i]->mRadioLegacy =
                ::android::hardware::radio::V1_5::IRadio::castFrom(radioLegacy).withDefault(nullptr);
        assert(radioService[i]->mRadioLegacy != nullptr);

        ret = pthread_rwlock_unlock(radioServiceRwlockPtr);
        assert(ret == 0);
    }
}

sp<::android::hardware::radio::V1_0::IRadio> radio::impl::v1_6::getRadio(int slotId) {
    int simCount = 1;
    #if (SIM_COUNT >= 2)
    simCount = SIM_COUNT;
    #endif

    if (slotId < 0 || slotId >= simCount) {
        return nullptr;
    }
    sp<::android::hardware::radio::V1_0::IRadio> radio = radioService[slotId];
    return radio;
}

sp<::android::hardware::radio::V1_6::IRadio> radio::impl::v1_6::make(uint32_t slotId) {
    auto halRadio1_6 = sp<RadioImpl>::make();
    assert(halRadio1_6 != nullptr);
    halRadio1_6->mSlotId = slotId;
    halRadio1_6->mRadioLegacy = radio::impl::legacy::make(slotId);
    assert(halRadio1_6->mRadioLegacy == nullptr);
    return halRadio1_6;
}
