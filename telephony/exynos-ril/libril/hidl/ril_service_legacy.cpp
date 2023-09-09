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

#define LOG_TAG "RILC"

#include <android/hardware/radio/1.5/IRadio.h>
#include <android/hardware/radio/1.5/IRadioResponse.h>
#include <android/hardware/radio/1.5/IRadioIndication.h>
#include <android/hardware/radio/1.5/types.h>

#include "ril_service_common.h"
#include "ril_service_legacy.h"

#include <telephony/ril.h>
#include <hidl/HidlTransportSupport.h>
#include <utils/Log.h>

#include <librilutils/vendor.h>
#include <slsi/radio_v1_5.h>
#include <librilutils/radio_conv_utils.h>
#include <librilutils/radio_hal_utils.h>

// HIDL HAL response
#include <hidl/hidlhalindication_modem.h>
#include <hidl/hidlhalresponse_modem.h>
#include <hidl/hidlhalindication_sim.h>
#include <hidl/hidlhalresponse_sim.h>
#include <hidl/hidlhalindication_messaging.h>
#include <hidl/hidlhalresponse_messaging.h>
#include <hidl/hidlhalresponse_network.h>
#include <hidl/hidlhalindication_network.h>
#include <hidl/hidlhalindication_voice.h>
#include <hidl/hidlhalresponse_voice.h>

using namespace radio::impl::legacy;

using namespace std;
using namespace android::hardware::radio;
using namespace android::hardware::radio::V1_0;
using ::android::hardware::configureRpcThreadpool;
using ::android::hardware::joinRpcThreadpool;
using ::android::hardware::Return;
using ::android::hardware::hidl_array;
using ::android::hardware::hidl_bitfield;
using ::android::hardware::hidl_death_recipient;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Void;
using android::CommandInfo;
using android::RequestInfo;
using android::requestToString;
using android::sp;
using android::wp;

#define CURRENT_RADIO_HAL "android::hardware::radio::V1_5::IRadio"

#if VDBG
static bool vdbg = true;
#else
static bool vdbg = false;
#endif

bool dispatchCallForwardStatus(int serial, int slotId, int request,
                              const CallForwardInfo& callInfo) {
    RequestInfo *pRI = android::addRequestToList(serial, slotId, request);
    if (pRI == NULL) {
        return false;
    }

    RIL_CallForwardInfo cf;
    cf.status = (int) callInfo.status;
    cf.reason = callInfo.reason;
    cf.serviceClass = callInfo.serviceClass;
    cf.toa = callInfo.toa;
    cf.timeSeconds = callInfo.timeSeconds;

    if (!copyHidlStringToRil(&cf.number, callInfo.number, pRI)) {
        return false;
    }

    CALL_ONREQUEST(request, &cf, sizeof(cf), pRI, slotId);

    memsetAndFreeStrings(1, cf.number);

    return true;
}

bool dispatchIccApdu(int serial, int slotId, int request, const SimApdu& message) {
    RequestInfo *pRI = android::addRequestToList(serial, slotId, request);
    if (pRI == NULL) {
        return false;
    }

    RIL_SIM_APDU apdu = {};

    apdu.sessionid = message.sessionId;
    apdu.cla = message.cla;
    apdu.instruction = message.instruction;
    apdu.p1 = message.p1;
    apdu.p2 = message.p2;
    apdu.p3 = message.p3;

    if (!copyHidlStringToRil(&apdu.data, message.data, pRI)) {
        return false;
    }

    CALL_ONREQUEST(request, &apdu, sizeof(apdu), pRI, slotId);

    memsetAndFreeStrings(1, apdu.data);

    return true;
}


void freeRilDataProfile(RIL_DataProfileInfo_V1_4& dataProfile) {
    memsetAndFreeStrings(3, dataProfile.apn, dataProfile.user, dataProfile.password);
    dataProfile.apn = dataProfile.user = dataProfile.password = NULL;
}

void freeRilDataProfile(RIL_DataProfileInfo_V1_5& dataProfile) {
    memsetAndFreeStrings(3, dataProfile.apn, dataProfile.user, dataProfile.password);
    dataProfile.apn = dataProfile.user = dataProfile.password = NULL;
}

bool convertHalDataProfileToRil(RIL_DataProfileInfo_V1_4& out, const V1_4::DataProfileInfo& dataProfileInfo, RequestInfo *pRI) {
    bool success = false;

    success = copyHidlStringToRil(&out.apn, dataProfileInfo.apn, pRI, true);
    if (success && !copyHidlStringToRil(&out.user, dataProfileInfo.user, pRI, true)) {
        success = false;
    }
    if (success && !copyHidlStringToRil(&out.password, dataProfileInfo.password, pRI, true)) {
        success = false;
    }

    if (!success) {
        freeRilDataProfile(out);
        return false;
    }
    out.profileId = (RIL_DataProfile) dataProfileInfo.profileId;
    out.protocol = (int) dataProfileInfo.protocol;
    out.roamingProtocol = (int) dataProfileInfo.roamingProtocol;
    out.authType = (int) dataProfileInfo.authType;
    out.type = (int) dataProfileInfo.type;
    out.maxConnsTime = dataProfileInfo.maxConnsTime;
    out.maxConns = dataProfileInfo.maxConns;
    out.waitTime = dataProfileInfo.waitTime;
    out.enabled = BOOL_TO_INT(dataProfileInfo.enabled);
    out.supportedApnTypesBitmap = (int)dataProfileInfo.supportedApnTypesBitmap;
    out.bearerBitmap = dataProfileInfo.bearerBitmap;
    out.mtu = dataProfileInfo.mtu;
    out.preferred = dataProfileInfo.preferred;
    out.persistent= dataProfileInfo.persistent;

    return true;

}

void freeRilLinkAddress(RIL_LinkAddress& out) {
    memsetAndFreeStrings(1, out.address);
}

bool convertHalLinkAddressToRil(RIL_LinkAddress& out, const V1_5::LinkAddress& linkAddress, RequestInfo *pRI) {
    if (!copyHidlStringToRil(&out.address, linkAddress.address, pRI, true)) {
        freeRilLinkAddress(out);
        return false;
    }

    out.properties = linkAddress.properties;
    out.deprecationTime = linkAddress.deprecationTime;
    out.expirationTime = linkAddress.expirationTime;

    return true;
}

bool convertHalDataProfileToRil(RIL_DataProfileInfo_V1_5& out, const V1_5::DataProfileInfo& dataProfileInfo, RequestInfo *pRI) {
    bool success = false;

    success = copyHidlStringToRil(&out.apn, dataProfileInfo.apn, pRI, true);
    if (success && !copyHidlStringToRil(&out.user, dataProfileInfo.user, pRI, true)) {
        success = false;
    }

    if (success && !copyHidlStringToRil(&out.password, dataProfileInfo.password, pRI, true)) {
        success = false;
    }

    if (!success) {
        freeRilDataProfile(out);
        return false;
    }
    out.profileId = (RIL_DataProfileId) dataProfileInfo.profileId;
    out.protocol = (PdpProtocolType) dataProfileInfo.protocol;
    out.roamingProtocol = (PdpProtocolType) dataProfileInfo.roamingProtocol;
    out.authType = (RIL_ApnAuthType) dataProfileInfo.authType;
    out.type = (RIL_DataProfileInfoType) dataProfileInfo.type;
    out.maxConnsTime = dataProfileInfo.maxConnsTime;
    out.maxConns = dataProfileInfo.maxConns;
    out.waitTime = dataProfileInfo.waitTime;
    out.enabled = dataProfileInfo.enabled;
    out.supportedApnTypesBitmap = (int)dataProfileInfo.supportedApnTypesBitmap;
    out.bearerBitmap = dataProfileInfo.bearerBitmap;
    out.mtuV4 = dataProfileInfo.mtuV4;
    out.mtuV6 = dataProfileInfo.mtuV6;
    out.preferred = dataProfileInfo.preferred;
    out.persistent = dataProfileInfo.persistent;

    return true;
}

bool fillSetupDataCallInfo_V1_5(RIL_SetupDataCallInfo_V1_5 &setupDataCallInfo, V1_5::AccessNetwork accessNetwork,
        const V1_5::DataProfileInfo& dataProfileInfo, bool roamingAllowed,
        V1_2::DataRequestReason reason, const hidl_vec<V1_5::LinkAddress>& addresses,
        const hidl_vec<hidl_string>& dnses, RequestInfo *pRI) {
    if (!convertHalDataProfileToRil(setupDataCallInfo.dataProfileInfo, dataProfileInfo, pRI)) {
        sendErrorResponse(pRI, RIL_E_NO_MEMORY);
        return false;
    }

    setupDataCallInfo.accessNetwork = (int)accessNetwork;
    setupDataCallInfo.roamingAllow = roamingAllowed;
    setupDataCallInfo.reason = (int)reason;

    size_t num = addresses.size();
    setupDataCallInfo.leng_addresses = num;

    setupDataCallInfo.linkAddress =
        (RIL_LinkAddress *) calloc(num, sizeof(RIL_LinkAddress));

    if (setupDataCallInfo.linkAddress == NULL) {
        RLOGE("Memory allocation failed for request %s", requestToString(pRI->pCI->requestNumber));
        sendErrorResponse(pRI, RIL_E_NO_MEMORY);
        return false;
    }

    for (size_t i = 0; i < num; i++) {
        if (!convertHalLinkAddressToRil(setupDataCallInfo.linkAddress[i], addresses[i], pRI)) {
            for (size_t j = 0; j < i; j++) {
                freeRilLinkAddress(setupDataCallInfo.linkAddress[j]);
            }
            sendErrorResponse(pRI, RIL_E_NO_MEMORY);
            free(setupDataCallInfo.linkAddress);
            return false;
        }
    }

    string dnsStr = "";
    for (size_t i = 0; i < dnses.size(); i++) {
        dnsStr.append(dnses[i]);
        dnsStr.append(" ");
    }
    if (dnsStr.size() > 0 &&
            !copyHidlStringToRil(&setupDataCallInfo.dnses, hidl_string(dnsStr.c_str()), pRI, true)) {
        freeRilDataProfile(setupDataCallInfo.dataProfileInfo);
        memsetAndFreeStrings(1, setupDataCallInfo.dnses);
        sendErrorResponse(pRI, RIL_E_NO_MEMORY);
        return false;
    }

    return true;
}

void freeSetupDataCallInfo_V1_5(RIL_SetupDataCallInfo_V1_5 &setupDataCallInfo,
                                   const hidl_vec<V1_5::LinkAddress>& addresses) {
    freeRilDataProfile(setupDataCallInfo.dataProfileInfo);

    size_t num = addresses.size();
    for (size_t i = 0; i < num; i++) {
        freeRilLinkAddress(setupDataCallInfo.linkAddress[i]);
    }

    if (setupDataCallInfo.linkAddress != NULL) {
        delete[] setupDataCallInfo.linkAddress;
        setupDataCallInfo.linkAddress = NULL;
    }

    memsetAndFreeStrings(1, setupDataCallInfo.dnses);
}

void constructCdmaSms(RIL_CDMA_SMS_Message &rcsm, const CdmaSmsMessage& sms) {
    rcsm.uTeleserviceID = sms.teleserviceId;
    rcsm.bIsServicePresent = BOOL_TO_INT(sms.isServicePresent);
    rcsm.uServicecategory = sms.serviceCategory;
    rcsm.sAddress.digit_mode = (RIL_CDMA_SMS_DigitMode) sms.address.digitMode;
    rcsm.sAddress.number_mode = (RIL_CDMA_SMS_NumberMode) sms.address.numberMode;
    rcsm.sAddress.number_type = (RIL_CDMA_SMS_NumberType) sms.address.numberType;
    rcsm.sAddress.number_plan = (RIL_CDMA_SMS_NumberPlan) sms.address.numberPlan;

    rcsm.sAddress.number_of_digits = sms.address.digits.size();
    int digitLimit= MIN((rcsm.sAddress.number_of_digits), RIL_CDMA_SMS_ADDRESS_MAX);
    for (int i = 0; i < digitLimit; i++) {
        rcsm.sAddress.digits[i] = sms.address.digits[i];
    }

    rcsm.sSubAddress.subaddressType = (RIL_CDMA_SMS_SubaddressType) sms.subAddress.subaddressType;
    rcsm.sSubAddress.odd = BOOL_TO_INT(sms.subAddress.odd);

    rcsm.sSubAddress.number_of_digits = sms.subAddress.digits.size();
    digitLimit= MIN((rcsm.sSubAddress.number_of_digits), RIL_CDMA_SMS_SUBADDRESS_MAX);
    for (int i = 0; i < digitLimit; i++) {
        rcsm.sSubAddress.digits[i] = sms.subAddress.digits[i];
    }

    rcsm.uBearerDataLen = sms.bearerData.size();
    digitLimit= MIN((rcsm.uBearerDataLen), RIL_CDMA_SMS_BEARER_DATA_MAX);
    for (int i = 0; i < digitLimit; i++) {
        rcsm.aBearerData[i] = sms.bearerData[i];
    }
}

#define HIDL_HAL_MODEM_RESPONSE(theHandler) \
    if (vdbg) RLOGD("radio::%s: serial %d", __FUNCTION__, serial); \
    if (radioService[slotId] == nullptr) { \
        RLOGW("HIDL_HAL_MODEM_RESPONSE: radioService[%d] is null.", slotId); return 0; } \
    return HidlHalModemResponse(radioService[slotId]->mHalResponse). \
                theHandler(slotId, responseType, serial, e, response, responselen); \

#define HIDL_HAL_MODEM_INDICATION(theHandler) \
    if (vdbg) RLOGD("radio::%s", __FUNCTION__); \
    if (radioService[slotId] == nullptr) { \
        RLOGW("HIDL_HAL_MODEM_INDICATION: radioService[%d] is null.", slotId); return 0; } \
    return HidlHalModemIndication(radioService[slotId]->mHalIndication). \
                theHandler(slotId, indicationType, token, e, response, responselen); \

#define HIDL_HAL_SIM_RESPONSE(theHandler) \
    if (vdbg) RLOGD("radio::%s: serial %d", __FUNCTION__, serial); \
    if (radioService[slotId] == nullptr) { \
        RLOGW("HIDL_HAL_SIM_RESPONSE: radioService[%d] is null.", slotId); return 0; } \
    return HidlHalSimResponse(radioService[slotId]->mHalResponse). \
                theHandler(slotId, responseType, serial, e, response, responseLen); \

#define HIDL_HAL_SIM_INDICATION(theHandler) \
    if (vdbg) RLOGD("radio::%s", __FUNCTION__); \
    if (radioService[slotId] == nullptr) { \
        RLOGW("HIDL_HAL_SIM_INDICATION: radioService[%d] is null.", slotId); return 0; } \
    return HidlHalSimIndication(radioService[slotId]->mHalIndication). \
                theHandler(slotId, indicationType, token, e, response, responseLen); \

#define HIDL_HAL_MESSAGING_RESPONSE(theHandler) \
    if (vdbg) RLOGD("radio::%s: serial %d", __FUNCTION__, serial); \
    if (radioService[slotId] == nullptr) { \
        RLOGW("HIDL_HAL_MESSAGING_RESPONSE: radioService[%d] is null.", slotId); return 0; } \
    return HidlHalMessagingResponse(radioService[slotId]->mHalResponse). \
                theHandler(slotId, responseType, serial, e, response, responseLen); \

#define HIDL_HAL_MESSAGING_INDICATION(theHandler) \
    if (vdbg) RLOGD("radio::%s", __FUNCTION__); \
    if (radioService[slotId] == nullptr) { \
        RLOGW("HIDL_HAL_MESSAGING_INDICATION: radioService[%d] is null.", slotId); return 0; } \
    return HidlHalMessagingIndication(radioService[slotId]->mHalIndication). \
                theHandler(slotId, indicationType, token, e, response, responseLen); \

#define HIDL_HAL_NETWORK_RESPONSE(theHandler) \
    if (vdbg) RLOGD("radio::%s: serial %d", __FUNCTION__, serial); \
    if (radioService[slotId] == nullptr) { \
        RLOGW("HIDL_HAL_NETWORK_RESPONSE: radioService[%d] is null.", slotId); return 0; } \
    return HidlHalNetworkResponse(radioService[slotId]->mHalResponse). \
                theHandler(slotId, responseType, serial, e, response, responselen); \

#define HIDL_HAL_NETWORK_INDICATION(theHandler) \
    if (vdbg) RLOGD("radio::%s", __FUNCTION__); \
    if (radioService[slotId] == nullptr) { \
        RLOGW("HIDL_HAL_NETWORK_INDICATION: radioService[%d] is null.", slotId); return 0; } \
    return HidlHalNetworkIndication(radioService[slotId]->mHalIndication). \
                theHandler(slotId, indicationType, token, e, response, responselen); \

#define HIDL_HAL_VOICE_RESPONSE(theHandler) \
    if (vdbg) RLOGD("radio::%s: serial %d", __FUNCTION__, serial); \
    if (radioService[slotId] == nullptr) { \
        RLOGW("HIDL_HAL_VOICE_RESPONSE: radioService[%d] is null.", slotId); return 0; } \
    return HidlHalVoiceResponse(radioService[slotId]->mHalResponse). \
                theHandler(slotId, responseType, serial, e, response, responseLen); \

#define HIDL_HAL_VOICE_INDICATION(theHandler) \
    if (vdbg) RLOGD("radio::%s", __FUNCTION__); \
    if (radioService[slotId] == nullptr) { \
        RLOGW("HIDL_HAL_VOICE_INDICATION: radioService[%d] is null.", slotId); return 0; } \
    return HidlHalVoiceIndication(radioService[slotId]->mHalIndication). \
                theHandler(slotId, indicationType, token, e, response, responseLen); \

namespace radio {
namespace impl {
namespace legacy {

struct RadioImpl;

#if (SIM_COUNT >= 2)
sp<RadioImpl> radioService[SIM_COUNT];
int64_t nitzTimeReceived[SIM_COUNT];
// counter used for synchronization. It is incremented every time response callbacks are updated.
volatile int32_t mCounterRadio[SIM_COUNT];
#else
sp<RadioImpl> radioService[1];
int64_t nitzTimeReceived[1];
// counter used for synchronization. It is incremented every time response callbacks are updated.
volatile int32_t mCounterRadio[1];
#endif

struct RadioImpl : public V1_5::IRadio {

    int32_t mSlotId;
    sp<IRadioResponse> mRadioResponse;
    sp<IRadioIndication> mRadioIndication;
    sp<V1_1::IRadioResponse> mRadioResponseV1_1;
    sp<V1_1::IRadioIndication> mRadioIndicationV1_1;
    sp<V1_2::IRadioResponse> mRadioResponseV1_2;
    sp<V1_2::IRadioIndication> mRadioIndicationV1_2;
    sp<V1_3::IRadioResponse> mRadioResponseV1_3;
    sp<V1_3::IRadioIndication> mRadioIndicationV1_3;
    sp<V1_4::IRadioResponse> mRadioResponseV1_4;
    sp<V1_4::IRadioIndication> mRadioIndicationV1_4;
    sp<V1_5::IRadioResponse> mRadioResponseV1_5;
    sp<V1_5::IRadioIndication> mRadioIndicationV1_5;

    HidlHalResponse mHalResponse;
    HidlHalIndication mHalIndication;

    Return<void> setResponseFunctions(
            const ::android::sp<IRadioResponse>& radioResponse,
            const ::android::sp<IRadioIndication>& radioIndication);

    Return<void> getIccCardStatus(int32_t serial);

    Return<void> supplyIccPinForApp(int32_t serial, const hidl_string& pin,
            const hidl_string& aid);

    Return<void> supplyIccPukForApp(int32_t serial, const hidl_string& puk,
            const hidl_string& pin, const hidl_string& aid);

    Return<void> supplyIccPin2ForApp(int32_t serial,
            const hidl_string& pin2,
            const hidl_string& aid);

    Return<void> supplyIccPuk2ForApp(int32_t serial, const hidl_string& puk2,
            const hidl_string& pin2, const hidl_string& aid);

    Return<void> changeIccPinForApp(int32_t serial, const hidl_string& oldPin,
            const hidl_string& newPin, const hidl_string& aid);

    Return<void> changeIccPin2ForApp(int32_t serial, const hidl_string& oldPin2,
            const hidl_string& newPin2, const hidl_string& aid);

    Return<void> supplyNetworkDepersonalization(int32_t serial, const hidl_string& netPin);

    Return<void> getCurrentCalls(int32_t serial);

    Return<void> dial(int32_t serial, const Dial& dialInfo);

    Return<void> getImsiForApp(int32_t serial,
            const ::android::hardware::hidl_string& aid);

    Return<void> hangup(int32_t serial, int32_t gsmIndex);

    Return<void> hangupWaitingOrBackground(int32_t serial);

    Return<void> hangupForegroundResumeBackground(int32_t serial);

    Return<void> switchWaitingOrHoldingAndActive(int32_t serial);

    Return<void> conference(int32_t serial);

    Return<void> rejectCall(int32_t serial);

    Return<void> getLastCallFailCause(int32_t serial);

    Return<void> getSignalStrength(int32_t serial);

    Return<void> getVoiceRegistrationState(int32_t serial);

    Return<void> getDataRegistrationState(int32_t serial);

    Return<void> getOperator(int32_t serial);

    Return<void> setRadioPower(int32_t serial, bool on);

    Return<void> sendDtmf(int32_t serial,
            const ::android::hardware::hidl_string& s);

    Return<void> sendSms(int32_t serial, const GsmSmsMessage& message);

    Return<void> sendSMSExpectMore(int32_t serial, const GsmSmsMessage& message);

    Return<void> setupDataCall(int32_t serial,
            RadioTechnology radioTechnology,
            const DataProfileInfo& profileInfo,
            bool modemCognitive,
            bool roamingAllowed,
            bool isRoaming);

    Return<void> iccIOForApp(int32_t serial,
            const IccIo& iccIo);

    Return<void> sendUssd(int32_t serial,
            const ::android::hardware::hidl_string& ussd);

    Return<void> cancelPendingUssd(int32_t serial);

    Return<void> getClir(int32_t serial);

    Return<void> setClir(int32_t serial, int32_t status);

    Return<void> getCallForwardStatus(int32_t serial,
            const CallForwardInfo& callInfo);

    Return<void> setCallForward(int32_t serial,
            const CallForwardInfo& callInfo);

    Return<void> getCallWaiting(int32_t serial, int32_t serviceClass);

    Return<void> setCallWaiting(int32_t serial, bool enable, int32_t serviceClass);

    Return<void> acknowledgeLastIncomingGsmSms(int32_t serial,
            bool success, SmsAcknowledgeFailCause cause);

    Return<void> acceptCall(int32_t serial);

    Return<void> deactivateDataCall(int32_t serial,
            int32_t cid, bool reasonRadioShutDown);

    Return<void> getFacilityLockForApp(int32_t serial,
            const ::android::hardware::hidl_string& facility,
            const ::android::hardware::hidl_string& password,
            int32_t serviceClass,
            const ::android::hardware::hidl_string& appId);

    Return<void> setFacilityLockForApp(int32_t serial,
            const ::android::hardware::hidl_string& facility,
            bool lockState,
            const ::android::hardware::hidl_string& password,
            int32_t serviceClass,
            const ::android::hardware::hidl_string& appId);

    Return<void> setBarringPassword(int32_t serial,
            const ::android::hardware::hidl_string& facility,
            const ::android::hardware::hidl_string& oldPassword,
            const ::android::hardware::hidl_string& newPassword);

    Return<void> getNetworkSelectionMode(int32_t serial);

    Return<void> setNetworkSelectionModeAutomatic(int32_t serial);

    Return<void> setNetworkSelectionModeManual(int32_t serial,
            const ::android::hardware::hidl_string& operatorNumeric);

    Return<void> getAvailableNetworks(int32_t serial);

    Return<void> startNetworkScan(int32_t serial, const V1_1::NetworkScanRequest& request);

    Return<void> stopNetworkScan(int32_t serial);

    Return<void> startDtmf(int32_t serial,
            const ::android::hardware::hidl_string& s);

    Return<void> stopDtmf(int32_t serial);

    Return<void> getBasebandVersion(int32_t serial);

    Return<void> separateConnection(int32_t serial, int32_t gsmIndex);

    Return<void> setMute(int32_t serial, bool enable);

    Return<void> getMute(int32_t serial);

    Return<void> getClip(int32_t serial);

    Return<void> getDataCallList(int32_t serial);

    Return<void> setSuppServiceNotifications(int32_t serial, bool enable);

    Return<void> writeSmsToSim(int32_t serial,
            const SmsWriteArgs& smsWriteArgs);

    Return<void> deleteSmsOnSim(int32_t serial, int32_t index);

    Return<void> setBandMode(int32_t serial, RadioBandMode mode);

    Return<void> getAvailableBandModes(int32_t serial);

    Return<void> sendEnvelope(int32_t serial,
            const ::android::hardware::hidl_string& command);

    Return<void> sendTerminalResponseToSim(int32_t serial,
            const ::android::hardware::hidl_string& commandResponse);

    Return<void> handleStkCallSetupRequestFromSim(int32_t serial, bool accept);

    Return<void> explicitCallTransfer(int32_t serial);

    Return<void> setPreferredNetworkType(int32_t serial, PreferredNetworkType nwType);

    Return<void> getPreferredNetworkType(int32_t serial);

    Return<void> getNeighboringCids(int32_t serial);

    Return<void> setLocationUpdates(int32_t serial, bool enable);

    Return<void> setCdmaSubscriptionSource(int32_t serial,
            CdmaSubscriptionSource cdmaSub);

    Return<void> setCdmaRoamingPreference(int32_t serial, CdmaRoamingType type);

    Return<void> getCdmaRoamingPreference(int32_t serial);

    Return<void> setTTYMode(int32_t serial, TtyMode mode);

    Return<void> getTTYMode(int32_t serial);

    Return<void> setPreferredVoicePrivacy(int32_t serial, bool enable);

    Return<void> getPreferredVoicePrivacy(int32_t serial);

    Return<void> sendCDMAFeatureCode(int32_t serial,
            const ::android::hardware::hidl_string& featureCode);

    Return<void> sendBurstDtmf(int32_t serial,
            const ::android::hardware::hidl_string& dtmf,
            int32_t on,
            int32_t off);

    Return<void> sendCdmaSms(int32_t serial, const CdmaSmsMessage& sms);

    Return<void> acknowledgeLastIncomingCdmaSms(int32_t serial,
            const CdmaSmsAck& smsAck);

    Return<void> getGsmBroadcastConfig(int32_t serial);

    Return<void> setGsmBroadcastConfig(int32_t serial,
            const hidl_vec<GsmBroadcastSmsConfigInfo>& configInfo);

    Return<void> setGsmBroadcastActivation(int32_t serial, bool activate);

    Return<void> getCdmaBroadcastConfig(int32_t serial);

    Return<void> setCdmaBroadcastConfig(int32_t serial,
            const hidl_vec<CdmaBroadcastSmsConfigInfo>& configInfo);

    Return<void> setCdmaBroadcastActivation(int32_t serial, bool activate);

    Return<void> getCDMASubscription(int32_t serial);

    Return<void> writeSmsToRuim(int32_t serial, const CdmaSmsWriteArgs& cdmaSms);

    Return<void> deleteSmsOnRuim(int32_t serial, int32_t index);

    Return<void> getDeviceIdentity(int32_t serial);

    Return<void> exitEmergencyCallbackMode(int32_t serial);

    Return<void> getSmscAddress(int32_t serial);

    Return<void> setSmscAddress(int32_t serial,
            const ::android::hardware::hidl_string& smsc);

    Return<void> reportSmsMemoryStatus(int32_t serial, bool available);

    Return<void> reportStkServiceIsRunning(int32_t serial);

    Return<void> getCdmaSubscriptionSource(int32_t serial);

    Return<void> requestIsimAuthentication(int32_t serial,
            const ::android::hardware::hidl_string& challenge);

    Return<void> acknowledgeIncomingGsmSmsWithPdu(int32_t serial,
            bool success,
            const ::android::hardware::hidl_string& ackPdu);

    Return<void> sendEnvelopeWithStatus(int32_t serial,
            const ::android::hardware::hidl_string& contents);

    Return<void> getVoiceRadioTechnology(int32_t serial);

    Return<void> getCellInfoList(int32_t serial);

    Return<void> setCellInfoListRate(int32_t serial, int32_t rate);

    Return<void> setInitialAttachApn(int32_t serial, const DataProfileInfo& dataProfileInfo,
            bool modemCognitive, bool isRoaming);

    Return<void> getImsRegistrationState(int32_t serial);

    Return<void> sendImsSms(int32_t serial, const ImsSmsMessage& message);

    Return<void> iccTransmitApduBasicChannel(int32_t serial, const SimApdu& message);

    Return<void> iccOpenLogicalChannel(int32_t serial,
            const ::android::hardware::hidl_string& aid, int32_t p2);

    Return<void> iccCloseLogicalChannel(int32_t serial, int32_t channelId);

    Return<void> iccTransmitApduLogicalChannel(int32_t serial, const SimApdu& message);

    Return<void> nvReadItem(int32_t serial, NvItem itemId);

    Return<void> nvWriteItem(int32_t serial, const NvWriteItem& item);

    Return<void> nvWriteCdmaPrl(int32_t serial,
            const ::android::hardware::hidl_vec<uint8_t>& prl);

    Return<void> nvResetConfig(int32_t serial, ResetNvType resetType);

    Return<void> setUiccSubscription(int32_t serial, const SelectUiccSub& uiccSub);

    Return<void> setDataAllowed(int32_t serial, bool allow);

    Return<void> getHardwareConfig(int32_t serial);

    Return<void> requestIccSimAuthentication(int32_t serial,
            int32_t authContext,
            const ::android::hardware::hidl_string& authData,
            const ::android::hardware::hidl_string& aid);

    Return<void> setDataProfile(int32_t serial,
            const ::android::hardware::hidl_vec<DataProfileInfo>& profiles, bool isRoaming);

    Return<void> requestShutdown(int32_t serial);

    Return<void> getRadioCapability(int32_t serial);

    Return<void> setRadioCapability(int32_t serial, const RadioCapability& rc);

    Return<void> startLceService(int32_t serial, int32_t reportInterval, bool pullMode);

    Return<void> stopLceService(int32_t serial);

    Return<void> pullLceData(int32_t serial);

    Return<void> getModemActivityInfo(int32_t serial);

    Return<void> setAllowedCarriers(int32_t serial,
            bool allAllowed,
            const CarrierRestrictions& carriers);

    Return<void> getAllowedCarriers(int32_t serial);

    Return<void> sendDeviceState(int32_t serial, DeviceStateType deviceStateType, bool state);

    Return<void> setIndicationFilter(int32_t serial, int32_t indicationFilter);

    Return<void> startKeepalive(int32_t serial, const V1_1::KeepaliveRequest& keepalive);

    Return<void> stopKeepalive(int32_t serial, int32_t sessionHandle);

    Return<void> setSimCardPower(int32_t serial, bool powerUp);
    Return<void> setSimCardPower_1_1(int32_t serial,
            const V1_1::CardPowerState state);

    Return<void> responseAcknowledgement();

    Return<void> setCarrierInfoForImsiEncryption(int32_t serial,
            const V1_1::ImsiEncryptionInfo& message);

    void sendStartNetworkScanRequest_V1_2(int32_t serial, int request, const V1_2::NetworkScanRequest &networkScanRequest);
    Return<void> startNetworkScan_1_2(int32_t serial, const V1_2::NetworkScanRequest& request);

    Return<void> setIndicationFilter_1_2(int32_t serial, hidl_bitfield<V1_2::IndicationFilter> indicationFilter);

    Return<void> setSignalStrengthReportingCriteria(int32_t serial, int32_t hysteresisMs, int32_t hysteresisDb,
            const hidl_vec<int32_t>& thresholdsDbm, V1_2::AccessNetwork accessNetwork);

    Return<void> setLinkCapacityReportingCriteria(int32_t serial, int32_t hysteresisMs, int32_t hysteresisDlKbps,
            int32_t hysteresisUlKbps, const hidl_vec<int32_t>& thresholdsDownlinkKbps,
            const hidl_vec<int32_t>& thresholdsUplinkKbps, V1_2::AccessNetwork accessNetwork);

    Return<void> setupDataCall_1_2(int32_t serial, V1_2::AccessNetwork accessNetwork,
            const DataProfileInfo& dataProfileInfo, bool modemCognitive, bool roamingAllowed, bool isRoaming, V1_2::DataRequestReason reason,
            const hidl_vec<hidl_string>& addresses, const hidl_vec<hidl_string>& dnses);

    Return<void> deactivateDataCall_1_2(int32_t serial, int32_t cid, V1_2::DataRequestReason reason);

    ///////////////////////////////////////////////////////////////////////////
    // radio 1.3
    ///////////////////////////////////////////////////////////////////////////
    Return<void> setSystemSelectionChannels(int32_t serial, bool specifyChannels,
            const hidl_vec<V1_1::RadioAccessSpecifier>& specifiers);
    Return<void> enableModem(int32_t serial, bool on);
    Return<void> getModemStackStatus(int32_t serial);

    ///////////////////////////////////////////////////////////////////////////
    // radio 1.4
    ///////////////////////////////////////////////////////////////////////////
    Return<void> setupDataCall_1_4(int32_t serial, V1_4::AccessNetwork accessNetwork,
            const V1_4::DataProfileInfo& dataProfileInfo, bool roamingAllowed,
            V1_2::DataRequestReason reason, const hidl_vec<hidl_string>& addresses,
            const hidl_vec<hidl_string>& dnses);
    Return<void> setInitialAttachApn_1_4(int32_t serial, const V1_4::DataProfileInfo& dataProfileInfo);
    Return<void> setDataProfile_1_4(int32_t serial, const hidl_vec<V1_4::DataProfileInfo>& profiles);
    Return<void> emergencyDial(int32_t serial, const Dial& dialInfo,
            hidl_bitfield<V1_4::EmergencyServiceCategory> categories,
            const hidl_vec<hidl_string>& urns, V1_4::EmergencyCallRouting routing,
            bool hasKnownUserIntentEmergency, bool isTesting);
    Return<void> startNetworkScan_1_4(int32_t serial, const V1_2::NetworkScanRequest& request);
    Return<void> getPreferredNetworkTypeBitmap(int32_t serial);
    Return<void> setPreferredNetworkTypeBitmap(int32_t serial, hidl_bitfield<V1_4::RadioAccessFamily> networkTypeBitmap);
    Return<void> setAllowedCarriers_1_4(int32_t serial, const V1_4::CarrierRestrictionsWithPriority& carriers,
            V1_4::SimLockMultiSimPolicy multiSimPolicy);
    Return<void> getAllowedCarriers_1_4(int32_t serial);
    Return<void> getSignalStrength_1_4(int32_t serial);

    ///////////////////////////////////////////////////////////////////////////
    // radio 1.5
    ///////////////////////////////////////////////////////////////////////////
    Return<void> setSignalStrengthReportingCriteria_1_5(int32_t serial,
            const V1_5::SignalThresholdInfo& signalThresholdInfo, V1_5::AccessNetwork accessNetwork);
    Return<void> enableUiccApplications(int32_t serial, bool enable);
    Return<void> areUiccApplicationsEnabled(int32_t serial);
    Return<void> setSystemSelectionChannels_1_5(int32_t serial, bool specifyChannels,
            const hidl_vec<V1_5::RadioAccessSpecifier>& specifiers);
    Return<void> startNetworkScan_1_5(int32_t serial, const V1_5::NetworkScanRequest& networkScanRequest);
    Return<void> setupDataCall_1_5(int32_t serial, V1_5::AccessNetwork accessNetwork,
            const V1_5::DataProfileInfo& dataProfileInfo, bool roamingAllowed,
            V1_2::DataRequestReason reason, const hidl_vec<V1_5::LinkAddress>& addresses,
            const hidl_vec<hidl_string>& dnses);
    Return<void> setInitialAttachApn_1_5(int32_t serial, const V1_5::DataProfileInfo& dataProfileInfo);
    Return<void> setDataProfile_1_5(int32_t serial, const hidl_vec<V1_5::DataProfileInfo>& profiles);
    Return<void> setRadioPower_1_5(int32_t serial, bool powerOn, bool forEmergencyCall,
            bool preferredForEmergencyCall);
    Return<void> setIndicationFilter_1_5(int32_t serial, hidl_bitfield<V1_5::IndicationFilter> indicationFilter);
    Return<void> getBarringInfo(int32_t serial);
    Return<void> getVoiceRegistrationState_1_5(int32_t serial);
    Return<void> getDataRegistrationState_1_5(int32_t serial);
    Return<void> setNetworkSelectionModeManual_1_5(int32_t serial, const ::android::hardware::hidl_string& operatorNumeric,
                    V1_5::RadioAccessNetworks ran);
    Return<void> sendCdmaSmsExpectMore(int32_t serial, const CdmaSmsMessage& sms);
    Return<void> supplySimDepersonalization(int32_t serial, V1_5::PersoSubstate persoType, const ::android::hardware::hidl_string& controlKey);
    Return<void> setLinkCapacityReportingCriteria_1_5(int32_t serial, int32_t hysteresisMs, int32_t hysteresisDlKbps,
            int32_t hysteresisUlKbps, const hidl_vec<int32_t>& thresholdsDownlinkKbps,
            const hidl_vec<int32_t>& thresholdsUplinkKbps, V1_5::AccessNetwork accessNetwork);

    void checkReturnStatus(Return<void>& ret);

    struct LocalDeathRecipient : public hidl_death_recipient {
        RadioImpl& mRadio;
        LocalDeathRecipient(RadioImpl& radio) :  mRadio(radio) {}
        ~LocalDeathRecipient()=default;
        void serviceDied(uint64_t cookie, const wp<IRadioResponse::IBase>& who);
    };
    sp<LocalDeathRecipient> mDeathRecipient;
};

void RadioImpl::LocalDeathRecipient::serviceDied(uint64_t cookie, const wp<IRadioResponse::IBase>&/* who*/) {
    int slotId = mRadio.mSlotId;
    RLOGE("serviceDied!!!: slotId=%d cookie=%u", slotId, (uint32_t)cookie);

    uint32_t counter = mCounterRadio[slotId];
    pthread_rwlock_t *radioServiceRwlockPtr = radio::getRadioServiceRwlock(slotId);
    // acquire wrlock
    int ret = pthread_rwlock_wrlock(radioServiceRwlockPtr);
    assert(ret == 0);

    // make sure the counter value has not changed
    if (counter == cookie) {
        radioService[slotId]->mRadioResponse = NULL;
        radioService[slotId]->mRadioIndication = NULL;
        radioService[slotId]->mRadioResponseV1_2 = NULL;
        radioService[slotId]->mRadioIndicationV1_2 = NULL;
        radioService[slotId]->mRadioResponseV1_3 = NULL;
        radioService[slotId]->mRadioIndicationV1_3 = NULL;
        radioService[slotId]->mRadioResponseV1_4 = NULL;
        radioService[slotId]->mRadioIndicationV1_4 = NULL;
        radioService[slotId]->mRadioResponseV1_5 = NULL;
        radioService[slotId]->mRadioIndicationV1_5 = NULL;
    } else {
        RLOGE("serviceDied: not resetting responseFunctions as they likely "
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
    RLOGD("%s[%d]", __FUNCTION__, mSlotId);

    pthread_rwlock_t *radioServiceRwlockPtr = radio::getRadioServiceRwlock(mSlotId);
    int ret = pthread_rwlock_wrlock(radioServiceRwlockPtr);
    assert(ret == 0);

    mRadioResponse = radioResponseParam;
    mRadioIndication = radioIndicationParam;

    mRadioResponseV1_1 = V1_1::IRadioResponse::castFrom(mRadioResponse).withDefault(nullptr);
    mRadioIndicationV1_1 = V1_1::IRadioIndication::castFrom(mRadioIndication).withDefault(nullptr);
    if (mRadioResponseV1_1 == nullptr || mRadioIndicationV1_1 == nullptr) {
        mRadioResponseV1_1 = nullptr;
        mRadioIndicationV1_1 = nullptr;
    }
    else {
        RLOGD("%s[%d]: V1_1 supportable", __FUNCTION__, mSlotId);
    }

    mRadioResponseV1_2 = V1_2::IRadioResponse::castFrom(mRadioResponse).withDefault(nullptr);
    mRadioIndicationV1_2 = V1_2::IRadioIndication::castFrom(mRadioIndication).withDefault(nullptr);
    if (mRadioResponseV1_2 == nullptr || mRadioIndicationV1_2 == nullptr) {
        mRadioResponseV1_2 = nullptr;
        mRadioIndicationV1_2 = nullptr;
    }
    else {
        RLOGD("%s[%d]: V1_2 supportable", __FUNCTION__, mSlotId);
    }

    mRadioResponseV1_3 = V1_3::IRadioResponse::castFrom(mRadioResponse).withDefault(nullptr);
    mRadioIndicationV1_3 = V1_3::IRadioIndication::castFrom(mRadioIndication).withDefault(nullptr);
    if (mRadioResponseV1_3 == nullptr || mRadioIndicationV1_3 == nullptr) {
        mRadioResponseV1_3 = nullptr;
        mRadioIndicationV1_3 = nullptr;
    }
    else {
        RLOGD("%s[%d]: V1_3 supportable", __FUNCTION__, mSlotId);
    }

    mRadioResponseV1_4 = V1_4::IRadioResponse::castFrom(mRadioResponse).withDefault(nullptr);
    mRadioIndicationV1_4 = V1_4::IRadioIndication::castFrom(mRadioIndication).withDefault(nullptr);
    if (mRadioResponseV1_4 == nullptr || mRadioIndicationV1_4 == nullptr) {
        mRadioResponseV1_4 = nullptr;
        mRadioIndicationV1_4 = nullptr;
    }
    else {
        RLOGD("%s[%d]: V1_4 supportable", __FUNCTION__, mSlotId);
    }

    mRadioResponseV1_5 = V1_5::IRadioResponse::castFrom(mRadioResponse).withDefault(nullptr);
    mRadioIndicationV1_5 = V1_5::IRadioIndication::castFrom(mRadioIndication).withDefault(nullptr);
    if (mRadioResponseV1_5 == nullptr || mRadioIndicationV1_5 == nullptr) {
        mRadioResponseV1_5 = nullptr;
        mRadioIndicationV1_5 = nullptr;
    }
    else {
        RLOGD("%s[%d]: V1_5 supportable", __FUNCTION__, mSlotId);
    }

    // HIDL response/indication
    // MUST update here.
    mHalResponse = HidlHalResponse(mRadioResponse);
    mHalIndication = HidlHalIndication(mRadioIndication);

    mCounterRadio[mSlotId]++;

    // linkToDeathRecipient
    if (mRadioResponse) {
        if (mDeathRecipient == nullptr) {
            mDeathRecipient = new LocalDeathRecipient(*this);
        }
        int32_t counter = mCounterRadio[mSlotId];
        mRadioResponse->linkToDeath(mDeathRecipient, counter);
        RLOGD("[%d] setResponseFunctions: linkToDeath cookie=%d", mSlotId, counter);
    }

    ret = pthread_rwlock_unlock(radioServiceRwlockPtr);
    assert(ret == 0);

    // client is connected. Send initial indications.
    android::onNewCommandConnect((RIL_SOCKET_ID) mSlotId);

    return Void();
}

Return<void> RadioImpl::getIccCardStatus(int32_t serial) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchVoid(serial, mSlotId, RIL_REQUEST_GET_SIM_STATUS);
    return Void();
}

Return<void> RadioImpl::supplyIccPinForApp(int32_t serial, const hidl_string& pin,
        const hidl_string& aid) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchStrings(serial, mSlotId, RIL_REQUEST_ENTER_SIM_PIN, true,
            2, pin.c_str(), aid.c_str());
    return Void();
}

Return<void> RadioImpl::supplyIccPukForApp(int32_t serial, const hidl_string& puk,
                                           const hidl_string& pin, const hidl_string& aid) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchStrings(serial, mSlotId, RIL_REQUEST_ENTER_SIM_PUK, true,
            3, puk.c_str(), pin.c_str(), aid.c_str());
    return Void();
}

Return<void> RadioImpl::supplyIccPin2ForApp(int32_t serial, const hidl_string& pin2,
                                            const hidl_string& aid) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchStrings(serial, mSlotId, RIL_REQUEST_ENTER_SIM_PIN2, true,
            2, pin2.c_str(), aid.c_str());
    return Void();
}

Return<void> RadioImpl::supplyIccPuk2ForApp(int32_t serial, const hidl_string& puk2,
                                            const hidl_string& pin2, const hidl_string& aid) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchStrings(serial, mSlotId, RIL_REQUEST_ENTER_SIM_PUK2, true,
            3, puk2.c_str(), pin2.c_str(), aid.c_str());
    return Void();
}

Return<void> RadioImpl::changeIccPinForApp(int32_t serial, const hidl_string& oldPin,
                                           const hidl_string& newPin, const hidl_string& aid) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchStrings(serial, mSlotId, RIL_REQUEST_CHANGE_SIM_PIN, true,
            3, oldPin.c_str(), newPin.c_str(), aid.c_str());
    return Void();
}

Return<void> RadioImpl::changeIccPin2ForApp(int32_t serial, const hidl_string& oldPin2,
                                            const hidl_string& newPin2, const hidl_string& aid) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchStrings(serial, mSlotId, RIL_REQUEST_CHANGE_SIM_PIN2, true,
            3, oldPin2.c_str(), newPin2.c_str(), aid.c_str());
    return Void();
}

Return<void> RadioImpl::supplyNetworkDepersonalization(int32_t serial,
                                                       const hidl_string& netPin) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchStrings(serial, mSlotId, RIL_REQUEST_ENTER_NETWORK_DEPERSONALIZATION, true,
            1, netPin.c_str());
    return Void();
}

Return<void> RadioImpl::getCurrentCalls(int32_t serial) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchVoid(serial, mSlotId, RIL_REQUEST_GET_CURRENT_CALLS);
    return Void();
}

Return<void> RadioImpl::dial(int32_t serial, const Dial& dialInfo) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    RequestInfo *pRI = android::addRequestToList(serial, mSlotId, RIL_REQUEST_DIAL);
    if (pRI == NULL) {
        return Void();
    }
    RIL_Dial dial = {};
    RIL_UUS_Info uusInfo = {};
    int32_t sizeOfDial = sizeof(dial);

    if (!copyHidlStringToRil(&dial.address, dialInfo.address, pRI)) {
        return Void();
    }
    dial.clir = (int) dialInfo.clir;

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

    CALL_ONREQUEST(RIL_REQUEST_DIAL, &dial, sizeOfDial, pRI, mSlotId);

    memsetAndFreeStrings(2, dial.address, uusInfo.uusData);

    return Void();
}

Return<void> RadioImpl::getImsiForApp(int32_t serial, const hidl_string& aid) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchStrings(serial, mSlotId, RIL_REQUEST_GET_IMSI, false,
            1, aid.c_str());
    return Void();
}

Return<void> RadioImpl::hangup(int32_t serial, int32_t gsmIndex) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchInts(serial, mSlotId, RIL_REQUEST_HANGUP, 1, gsmIndex);
    return Void();
}

Return<void> RadioImpl::hangupWaitingOrBackground(int32_t serial) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchVoid(serial, mSlotId, RIL_REQUEST_HANGUP_WAITING_OR_BACKGROUND);
    return Void();
}

Return<void> RadioImpl::hangupForegroundResumeBackground(int32_t serial) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchVoid(serial, mSlotId, RIL_REQUEST_HANGUP_FOREGROUND_RESUME_BACKGROUND);
    return Void();
}

Return<void> RadioImpl::switchWaitingOrHoldingAndActive(int32_t serial) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchVoid(serial, mSlotId, RIL_REQUEST_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE);
    return Void();
}

Return<void> RadioImpl::conference(int32_t serial) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchVoid(serial, mSlotId, RIL_REQUEST_CONFERENCE);
    return Void();
}

Return<void> RadioImpl::rejectCall(int32_t serial) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchVoid(serial, mSlotId, RIL_REQUEST_UDUB);
    return Void();
}

Return<void> RadioImpl::getLastCallFailCause(int32_t serial) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchVoid(serial, mSlotId, RIL_REQUEST_LAST_CALL_FAIL_CAUSE);
    return Void();
}

Return<void> RadioImpl::getSignalStrength(int32_t serial) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchVoid(serial, mSlotId, RIL_REQUEST_SIGNAL_STRENGTH);
    return Void();
}

Return<void> RadioImpl::getVoiceRegistrationState(int32_t serial) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchVoid(serial, mSlotId, RIL_REQUEST_VOICE_REGISTRATION_STATE);
    return Void();
}

Return<void> RadioImpl::getDataRegistrationState(int32_t serial) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchVoid(serial, mSlotId, RIL_REQUEST_DATA_REGISTRATION_STATE);
    return Void();
}

Return<void> RadioImpl::getOperator(int32_t serial) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchVoid(serial, mSlotId, RIL_REQUEST_OPERATOR);
    return Void();
}

Return<void> RadioImpl::setRadioPower(int32_t serial, bool on) {
    RLOGD("%s: serial %d on %d", __FUNCTION__, serial, on);
    dispatchInts(serial, mSlotId, RIL_REQUEST_RADIO_POWER, 1, BOOL_TO_INT(on));
    return Void();
}

Return<void> RadioImpl::sendDtmf(int32_t serial, const hidl_string& s) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchString(serial, mSlotId, RIL_REQUEST_DTMF, s.c_str());
    return Void();
}

Return<void> RadioImpl::sendSms(int32_t serial, const GsmSmsMessage& message) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchStrings(serial, mSlotId, RIL_REQUEST_SEND_SMS, false,
            2, message.smscPdu.c_str(), message.pdu.c_str());
    return Void();
}

Return<void> RadioImpl::sendSMSExpectMore(int32_t serial, const GsmSmsMessage& message) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchStrings(serial, mSlotId, RIL_REQUEST_SEND_SMS_EXPECT_MORE, false,
            2, message.smscPdu.c_str(), message.pdu.c_str());
    return Void();
}

/*
static bool convertMvnoTypeToString(MvnoType type, char *&str) {
    switch (type) {
        case MvnoType::IMSI:
            str = (char *)"imsi";
            return true;
        case MvnoType::GID:
            str = (char *)"gid";
            return true;
        case MvnoType::SPN:
            str = (char *)"spn";
            return true;
        case MvnoType::NONE:
            str = (char *)"";
            return true;
    }
    return false;
}
*/

Return<void> RadioImpl::setupDataCall(int32_t serial, RadioTechnology radioTechnology,
                                      const DataProfileInfo& dataProfileInfo, bool modemCognitive,
                                      bool roamingAllowed, bool isRoaming) {

#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    // We do not support lower APIs
    RLOGE("%s: Only support 1.4", __FUNCTION__);
    RequestInfo *pRI = android::addRequestToList(serial, mSlotId,
            RIL_REQUEST_SETUP_DATA_CALL);
    if (pRI != NULL) {
        sendErrorResponse(pRI, RIL_E_REQUEST_NOT_SUPPORTED);
    }
    // Intentional force return
    return Void();
}

Return<void> RadioImpl::iccIOForApp(int32_t serial, const IccIo& iccIo) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    RequestInfo *pRI = android::addRequestToList(serial, mSlotId, RIL_REQUEST_SIM_IO);
    if (pRI == NULL) {
        return Void();
    }

    RIL_SIM_IO_v6 rilIccIo = {};
    rilIccIo.command = iccIo.command;
    rilIccIo.fileid = iccIo.fileId;
    if (!copyHidlStringToRil(&rilIccIo.path, iccIo.path, pRI)) {
        return Void();
    }

    rilIccIo.p1 = iccIo.p1;
    rilIccIo.p2 = iccIo.p2;
    rilIccIo.p3 = iccIo.p3;

    if (!copyHidlStringToRil(&rilIccIo.data, iccIo.data, pRI)) {
        memsetAndFreeStrings(1, rilIccIo.path);
        return Void();
    }

    if (!copyHidlStringToRil(&rilIccIo.pin2, iccIo.pin2, pRI)) {
        memsetAndFreeStrings(2, rilIccIo.path, rilIccIo.data);
        return Void();
    }

    if (!copyHidlStringToRil(&rilIccIo.aidPtr, iccIo.aid, pRI)) {
        memsetAndFreeStrings(3, rilIccIo.path, rilIccIo.data, rilIccIo.pin2);
        return Void();
    }

    CALL_ONREQUEST(RIL_REQUEST_SIM_IO, &rilIccIo, sizeof(rilIccIo), pRI, mSlotId);

    memsetAndFreeStrings(4, rilIccIo.path, rilIccIo.data, rilIccIo.pin2, rilIccIo.aidPtr);

    return Void();
}

Return<void> RadioImpl::sendUssd(int32_t serial, const hidl_string& ussd) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchString(serial, mSlotId, RIL_REQUEST_SEND_USSD, ussd.c_str());
    return Void();
}

Return<void> RadioImpl::cancelPendingUssd(int32_t serial) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchVoid(serial, mSlotId, RIL_REQUEST_CANCEL_USSD);
    return Void();
}

Return<void> RadioImpl::getClir(int32_t serial) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchVoid(serial, mSlotId, RIL_REQUEST_GET_CLIR);
    return Void();
}

Return<void> RadioImpl::setClir(int32_t serial, int32_t status) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchInts(serial, mSlotId, RIL_REQUEST_SET_CLIR, 1, status);
    return Void();
}

Return<void> RadioImpl::getCallForwardStatus(int32_t serial, const CallForwardInfo& callInfo) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchCallForwardStatus(serial, mSlotId, RIL_REQUEST_QUERY_CALL_FORWARD_STATUS,
            callInfo);
    return Void();
}

Return<void> RadioImpl::setCallForward(int32_t serial, const CallForwardInfo& callInfo) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchCallForwardStatus(serial, mSlotId, RIL_REQUEST_SET_CALL_FORWARD,
            callInfo);
    return Void();
}

Return<void> RadioImpl::getCallWaiting(int32_t serial, int32_t serviceClass) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchInts(serial, mSlotId, RIL_REQUEST_QUERY_CALL_WAITING, 1, serviceClass);
    return Void();
}

Return<void> RadioImpl::setCallWaiting(int32_t serial, bool enable, int32_t serviceClass) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchInts(serial, mSlotId, RIL_REQUEST_SET_CALL_WAITING, 2, BOOL_TO_INT(enable),
            serviceClass);
    return Void();
}

Return<void> RadioImpl::acknowledgeLastIncomingGsmSms(int32_t serial,
                                                      bool success, SmsAcknowledgeFailCause cause) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchInts(serial, mSlotId, RIL_REQUEST_SMS_ACKNOWLEDGE, 2, BOOL_TO_INT(success),
            cause);
    return Void();
}

Return<void> RadioImpl::acceptCall(int32_t serial) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchVoid(serial, mSlotId, RIL_REQUEST_ANSWER);
    return Void();
}

Return<void> RadioImpl::deactivateDataCall(int32_t serial,
                                           int32_t cid, bool reasonRadioShutDown) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchStrings(serial, mSlotId, RIL_REQUEST_DEACTIVATE_DATA_CALL, false,
            2, (std::to_string(cid)).c_str(), reasonRadioShutDown ? "1" : "0");
    return Void();
}

Return<void> RadioImpl::getFacilityLockForApp(int32_t serial, const hidl_string& facility,
                                              const hidl_string& password, int32_t serviceClass,
                                              const hidl_string& appId) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchStrings(serial, mSlotId, RIL_REQUEST_QUERY_FACILITY_LOCK, true,
            4, facility.c_str(), password.c_str(),
            (std::to_string(serviceClass)).c_str(), appId.c_str());
    return Void();
}

Return<void> RadioImpl::setFacilityLockForApp(int32_t serial, const hidl_string& facility,
                                              bool lockState, const hidl_string& password,
                                              int32_t serviceClass, const hidl_string& appId) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchStrings(serial, mSlotId, RIL_REQUEST_SET_FACILITY_LOCK, true,
            5, facility.c_str(), lockState ? "1" : "0", password.c_str(),
            (std::to_string(serviceClass)).c_str(), appId.c_str() );
    return Void();
}

Return<void> RadioImpl::setBarringPassword(int32_t serial, const hidl_string& facility,
                                           const hidl_string& oldPassword,
                                           const hidl_string& newPassword) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchStrings(serial, mSlotId, RIL_REQUEST_CHANGE_BARRING_PASSWORD, true,
            3, facility.c_str(), oldPassword.c_str(), newPassword.c_str());
    return Void();
}

Return<void> RadioImpl::getNetworkSelectionMode(int32_t serial) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchVoid(serial, mSlotId, RIL_REQUEST_QUERY_NETWORK_SELECTION_MODE);
    return Void();
}

Return<void> RadioImpl::setNetworkSelectionModeAutomatic(int32_t serial) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchVoid(serial, mSlotId, RIL_REQUEST_SET_NETWORK_SELECTION_AUTOMATIC);
    return Void();
}

Return<void> RadioImpl::setNetworkSelectionModeManual(int32_t serial,
                                                      const hidl_string& operatorNumeric) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchStrings(serial, mSlotId, RIL_REQUEST_SET_NETWORK_SELECTION_MANUAL, true, 1,
            operatorNumeric.c_str());
    return Void();
}

Return<void> RadioImpl::getAvailableNetworks(int32_t serial) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchVoid(serial, mSlotId, RIL_REQUEST_QUERY_AVAILABLE_NETWORKS);
    return Void();
}

Return<void> RadioImpl::startNetworkScan(int32_t serial, const V1_1::NetworkScanRequest& request) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif

    RequestInfo *pRI = android::addRequestToList(serial, mSlotId, RIL_REQUEST_START_NETWORK_SCAN);
    if (pRI == NULL) {
        return Void();
    }

    if (request.specifiers.size() > MAX_RADIO_ACCESS_NETWORKS) {
        sendErrorResponse(pRI, RIL_E_INVALID_ARGUMENTS);
        return Void();
    }

    RIL_NetworkScanRequest scan_request = {};

    scan_request.type = (RIL_ScanType) request.type;
    scan_request.interval = request.interval;
    scan_request.specifiers_length = request.specifiers.size();
    for (size_t i = 0; i < request.specifiers.size(); ++i) {
        if (request.specifiers[i].geranBands.size() > MAX_BANDS ||
            request.specifiers[i].utranBands.size() > MAX_BANDS ||
            request.specifiers[i].eutranBands.size() > MAX_BANDS ||
            request.specifiers[i].channels.size() > MAX_CHANNELS) {
            sendErrorResponse(pRI, RIL_E_INVALID_ARGUMENTS);
            return Void();
        }
        const V1_1::RadioAccessSpecifier& ras_from =
                request.specifiers[i];
        RIL_RadioAccessSpecifier& ras_to = scan_request.specifiers[i];

        ras_to.radio_access_network = (RIL_RadioAccessNetworks) ras_from.radioAccessNetwork;
        ras_to.channels_length = ras_from.channels.size();

        std::copy(ras_from.channels.begin(), ras_from.channels.end(), ras_to.channels);
        const std::vector<uint32_t> * bands = nullptr;
        switch (request.specifiers[i].radioAccessNetwork) {
            case V1_1::RadioAccessNetworks::GERAN:
                ras_to.bands_length = ras_from.geranBands.size();
                bands = (std::vector<uint32_t> *) &ras_from.geranBands;
                break;
            case V1_1::RadioAccessNetworks::UTRAN:
                ras_to.bands_length = ras_from.utranBands.size();
                bands = (std::vector<uint32_t> *) &ras_from.utranBands;
                break;
            case V1_1::RadioAccessNetworks::EUTRAN:
                ras_to.bands_length = ras_from.eutranBands.size();
                bands = (std::vector<uint32_t> *) &ras_from.eutranBands;
                break;
            default:
                sendErrorResponse(pRI, RIL_E_INVALID_ARGUMENTS);
                return Void();
        }
        // safe to copy to geran_bands because it's a union member
        for (size_t idx = 0; idx < ras_to.bands_length; ++idx) {
            ras_to.bands.geran_bands[idx] = (RIL_GeranBands) (*bands)[idx];
        }
    }

    CALL_ONREQUEST(RIL_REQUEST_START_NETWORK_SCAN, &scan_request, sizeof(scan_request), pRI,
            mSlotId);

    return Void();
}

Return<void> RadioImpl::stopNetworkScan(int32_t serial) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchVoid(serial, mSlotId, RIL_REQUEST_STOP_NETWORK_SCAN);
    return Void();
}

Return<void> RadioImpl::startDtmf(int32_t serial, const hidl_string& s) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchString(serial, mSlotId, RIL_REQUEST_DTMF_START,
            s.c_str());
    return Void();
}

Return<void> RadioImpl::stopDtmf(int32_t serial) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchVoid(serial, mSlotId, RIL_REQUEST_DTMF_STOP);
    return Void();
}

Return<void> RadioImpl::getBasebandVersion(int32_t serial) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchVoid(serial, mSlotId, RIL_REQUEST_BASEBAND_VERSION);
    return Void();
}

Return<void> RadioImpl::separateConnection(int32_t serial, int32_t gsmIndex) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchInts(serial, mSlotId, RIL_REQUEST_SEPARATE_CONNECTION, 1, gsmIndex);
    return Void();
}

Return<void> RadioImpl::setMute(int32_t serial, bool enable) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchInts(serial, mSlotId, RIL_REQUEST_SET_MUTE, 1, BOOL_TO_INT(enable));
    return Void();
}

Return<void> RadioImpl::getMute(int32_t serial) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchVoid(serial, mSlotId, RIL_REQUEST_GET_MUTE);
    return Void();
}

Return<void> RadioImpl::getClip(int32_t serial) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchVoid(serial, mSlotId, RIL_REQUEST_QUERY_CLIP);
    return Void();
}

Return<void> RadioImpl::getDataCallList(int32_t serial) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchVoid(serial, mSlotId, RIL_REQUEST_DATA_CALL_LIST);
    return Void();
}

Return<void> RadioImpl::setSuppServiceNotifications(int32_t serial, bool enable) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchInts(serial, mSlotId, RIL_REQUEST_SET_SUPP_SVC_NOTIFICATION, 1,
            BOOL_TO_INT(enable));
    return Void();
}

Return<void> RadioImpl::writeSmsToSim(int32_t serial, const SmsWriteArgs& smsWriteArgs) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    RequestInfo *pRI = android::addRequestToList(serial, mSlotId, RIL_REQUEST_WRITE_SMS_TO_SIM);
    if (pRI == NULL) {
        return Void();
    }

    RIL_SMS_WriteArgs args;
    args.status = (int) smsWriteArgs.status;

    if (!copyHidlStringToRil(&args.pdu, smsWriteArgs.pdu, pRI)) {
        return Void();
    }

    if (!copyHidlStringToRil(&args.smsc, smsWriteArgs.smsc, pRI)) {
        memsetAndFreeStrings(1, args.pdu);
        return Void();
    }

    CALL_ONREQUEST(RIL_REQUEST_WRITE_SMS_TO_SIM, &args, sizeof(args), pRI, mSlotId);

    memsetAndFreeStrings(2, args.smsc, args.pdu);

    return Void();
}

Return<void> RadioImpl::deleteSmsOnSim(int32_t serial, int32_t index) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchInts(serial, mSlotId, RIL_REQUEST_DELETE_SMS_ON_SIM, 1, index);
    return Void();
}

Return<void> RadioImpl::setBandMode(int32_t serial, RadioBandMode mode) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchInts(serial, mSlotId, RIL_REQUEST_SET_BAND_MODE, 1, mode);
    return Void();
}

Return<void> RadioImpl::getAvailableBandModes(int32_t serial) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchVoid(serial, mSlotId, RIL_REQUEST_QUERY_AVAILABLE_BAND_MODE);
    return Void();
}

Return<void> RadioImpl::sendEnvelope(int32_t serial, const hidl_string& command) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchString(serial, mSlotId, RIL_REQUEST_STK_SEND_ENVELOPE_COMMAND,
            command.c_str());
    return Void();
}

Return<void> RadioImpl::sendTerminalResponseToSim(int32_t serial,
                                                  const hidl_string& commandResponse) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchString(serial, mSlotId, RIL_REQUEST_STK_SEND_TERMINAL_RESPONSE,
            commandResponse.c_str());
    return Void();
}

Return<void> RadioImpl::handleStkCallSetupRequestFromSim(int32_t serial, bool accept) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchInts(serial, mSlotId, RIL_REQUEST_STK_HANDLE_CALL_SETUP_REQUESTED_FROM_SIM,
            1, BOOL_TO_INT(accept));
    return Void();
}

Return<void> RadioImpl::explicitCallTransfer(int32_t serial) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchVoid(serial, mSlotId, RIL_REQUEST_EXPLICIT_CALL_TRANSFER);
    return Void();
}

Return<void> RadioImpl::setPreferredNetworkType(int32_t serial, PreferredNetworkType nwType) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchInts(serial, mSlotId, RIL_REQUEST_SET_PREFERRED_NETWORK_TYPE, 1, nwType);
    return Void();
}

Return<void> RadioImpl::getPreferredNetworkType(int32_t serial) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchVoid(serial, mSlotId, RIL_REQUEST_GET_PREFERRED_NETWORK_TYPE);
    return Void();
}

Return<void> RadioImpl::getNeighboringCids(int32_t serial) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchVoid(serial, mSlotId, RIL_REQUEST_GET_NEIGHBORING_CELL_IDS);
    return Void();
}

Return<void> RadioImpl::setLocationUpdates(int32_t serial, bool enable) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchInts(serial, mSlotId, RIL_REQUEST_SET_LOCATION_UPDATES, 1, BOOL_TO_INT(enable));
    return Void();
}

Return<void> RadioImpl::setCdmaSubscriptionSource(int32_t serial, CdmaSubscriptionSource cdmaSub) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchInts(serial, mSlotId, RIL_REQUEST_CDMA_SET_SUBSCRIPTION_SOURCE, 1, cdmaSub);
    return Void();
}

Return<void> RadioImpl::setCdmaRoamingPreference(int32_t serial, CdmaRoamingType type) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchInts(serial, mSlotId, RIL_REQUEST_CDMA_SET_ROAMING_PREFERENCE, 1, type);
    return Void();
}

Return<void> RadioImpl::getCdmaRoamingPreference(int32_t serial) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchVoid(serial, mSlotId, RIL_REQUEST_CDMA_QUERY_ROAMING_PREFERENCE);
    return Void();
}

Return<void> RadioImpl::setTTYMode(int32_t serial, TtyMode mode) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchInts(serial, mSlotId, RIL_REQUEST_SET_TTY_MODE, 1, mode);
    return Void();
}

Return<void> RadioImpl::getTTYMode(int32_t serial) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchVoid(serial, mSlotId, RIL_REQUEST_QUERY_TTY_MODE);
    return Void();
}

Return<void> RadioImpl::setPreferredVoicePrivacy(int32_t serial, bool enable) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchInts(serial, mSlotId, RIL_REQUEST_CDMA_SET_PREFERRED_VOICE_PRIVACY_MODE,
            1, BOOL_TO_INT(enable));
    return Void();
}

Return<void> RadioImpl::getPreferredVoicePrivacy(int32_t serial) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchVoid(serial, mSlotId, RIL_REQUEST_CDMA_QUERY_PREFERRED_VOICE_PRIVACY_MODE);
    return Void();
}

Return<void> RadioImpl::sendCDMAFeatureCode(int32_t serial, const hidl_string& featureCode) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchString(serial, mSlotId, RIL_REQUEST_CDMA_FLASH,
            featureCode.c_str());
    return Void();
}

Return<void> RadioImpl::sendBurstDtmf(int32_t serial, const hidl_string& dtmf, int32_t on,
                                      int32_t off) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchStrings(serial, mSlotId, RIL_REQUEST_CDMA_BURST_DTMF, false,
            3, dtmf.c_str(), (std::to_string(on)).c_str(),
            (std::to_string(off)).c_str());
    return Void();
}

Return<void> RadioImpl::sendCdmaSms(int32_t serial, const CdmaSmsMessage& sms) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    RequestInfo *pRI = android::addRequestToList(serial, mSlotId, RIL_REQUEST_CDMA_SEND_SMS);
    if (pRI == NULL) {
        return Void();
    }

    RIL_CDMA_SMS_Message rcsm = {};
    constructCdmaSms(rcsm, sms);

    CALL_ONREQUEST(pRI->pCI->requestNumber, &rcsm, sizeof(rcsm), pRI, mSlotId);
    return Void();
}

Return<void> RadioImpl::acknowledgeLastIncomingCdmaSms(int32_t serial, const CdmaSmsAck& smsAck) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    RequestInfo *pRI = android::addRequestToList(serial, mSlotId, RIL_REQUEST_CDMA_SMS_ACKNOWLEDGE);
    if (pRI == NULL) {
        return Void();
    }

    RIL_CDMA_SMS_Ack rcsa = {};

    rcsa.uErrorClass = (RIL_CDMA_SMS_ErrorClass) smsAck.errorClass;
    rcsa.uSMSCauseCode = smsAck.smsCauseCode;

    CALL_ONREQUEST(pRI->pCI->requestNumber, &rcsa, sizeof(rcsa), pRI, mSlotId);
    return Void();
}

Return<void> RadioImpl::getGsmBroadcastConfig(int32_t serial) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchVoid(serial, mSlotId, RIL_REQUEST_GSM_GET_BROADCAST_SMS_CONFIG);
    return Void();
}

Return<void> RadioImpl::setGsmBroadcastConfig(int32_t serial,
                                              const hidl_vec<GsmBroadcastSmsConfigInfo>&
                                              configInfo) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    RequestInfo *pRI = android::addRequestToList(serial, mSlotId,
            RIL_REQUEST_GSM_SET_BROADCAST_SMS_CONFIG);
    if (pRI == NULL) {
        return Void();
    }

    int num = configInfo.size();
    if (num > MAX_SMS_CONFIG_INFO_NUM) {
        RLOGW("%s: configInfo size(%d) is larger than MAX_SMS_CONFIG_INFO_NUM(%d)",
                __FUNCTION__, num, MAX_SMS_CONFIG_INFO_NUM);
        num = MAX_SMS_CONFIG_INFO_NUM;
    }

    RIL_GSM_BroadcastSmsConfigInfo gsmBci[num];
    RIL_GSM_BroadcastSmsConfigInfo *gsmBciPtrs[num];

    for (int i = 0 ; i < num ; i++ ) {
        gsmBciPtrs[i] = &gsmBci[i];
        gsmBci[i].fromServiceId = configInfo[i].fromServiceId;
        gsmBci[i].toServiceId = configInfo[i].toServiceId;
        gsmBci[i].fromCodeScheme = configInfo[i].fromCodeScheme;
        gsmBci[i].toCodeScheme = configInfo[i].toCodeScheme;
        gsmBci[i].selected = BOOL_TO_INT(configInfo[i].selected);
    }

    CALL_ONREQUEST(pRI->pCI->requestNumber, gsmBciPtrs,
            num * sizeof(RIL_GSM_BroadcastSmsConfigInfo *), pRI, mSlotId);
    return Void();
}

Return<void> RadioImpl::setGsmBroadcastActivation(int32_t serial, bool activate) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchInts(serial, mSlotId, RIL_REQUEST_GSM_SMS_BROADCAST_ACTIVATION,
            1, BOOL_TO_INT(!activate));
    return Void();
}

Return<void> RadioImpl::getCdmaBroadcastConfig(int32_t serial) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchVoid(serial, mSlotId, RIL_REQUEST_CDMA_GET_BROADCAST_SMS_CONFIG);
    return Void();
}

Return<void> RadioImpl::setCdmaBroadcastConfig(int32_t serial,
                                               const hidl_vec<CdmaBroadcastSmsConfigInfo>&
                                               configInfo) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    RequestInfo *pRI = android::addRequestToList(serial, mSlotId,
            RIL_REQUEST_CDMA_SET_BROADCAST_SMS_CONFIG);
    if (pRI == NULL) {
        return Void();
    }

    int num = configInfo.size();
    if (num > MAX_SMS_CDMA_CONFIG_INFO_NUM) {
        RLOGW("%s: configInfo size(%d) is larger than MAX_SMS_CDMA_CONFIG_INFO_NUM(%d)",
                __FUNCTION__, num, MAX_SMS_CDMA_CONFIG_INFO_NUM);
        num = MAX_SMS_CDMA_CONFIG_INFO_NUM;
    }

    RIL_CDMA_BroadcastSmsConfigInfo cdmaBci[num];
    RIL_CDMA_BroadcastSmsConfigInfo *cdmaBciPtrs[num];

    for (int i = 0 ; i < num ; i++ ) {
        cdmaBciPtrs[i] = &cdmaBci[i];
        cdmaBci[i].service_category = configInfo[i].serviceCategory;
        cdmaBci[i].language = configInfo[i].language;
        cdmaBci[i].selected = BOOL_TO_INT(configInfo[i].selected);
    }

    CALL_ONREQUEST(pRI->pCI->requestNumber, cdmaBciPtrs,
            num * sizeof(RIL_CDMA_BroadcastSmsConfigInfo *), pRI, mSlotId);
    return Void();
}

Return<void> RadioImpl::setCdmaBroadcastActivation(int32_t serial, bool activate) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchInts(serial, mSlotId, RIL_REQUEST_CDMA_SMS_BROADCAST_ACTIVATION,
            1, BOOL_TO_INT(!activate));
    return Void();
}

Return<void> RadioImpl::getCDMASubscription(int32_t serial) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchVoid(serial, mSlotId, RIL_REQUEST_CDMA_SUBSCRIPTION);
    return Void();
}

Return<void> RadioImpl::writeSmsToRuim(int32_t serial, const CdmaSmsWriteArgs& cdmaSms) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    RequestInfo *pRI = android::addRequestToList(serial, mSlotId,
            RIL_REQUEST_CDMA_WRITE_SMS_TO_RUIM);
    if (pRI == NULL) {
        return Void();
    }

    RIL_CDMA_SMS_WriteArgs rcsw = {};
    rcsw.status = (int) cdmaSms.status;
    constructCdmaSms(rcsw.message, cdmaSms.message);

    CALL_ONREQUEST(pRI->pCI->requestNumber, &rcsw, sizeof(rcsw), pRI, mSlotId);
    return Void();
}

Return<void> RadioImpl::deleteSmsOnRuim(int32_t serial, int32_t index) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchInts(serial, mSlotId, RIL_REQUEST_CDMA_DELETE_SMS_ON_RUIM, 1, index);
    return Void();
}

Return<void> RadioImpl::getDeviceIdentity(int32_t serial) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchVoid(serial, mSlotId, RIL_REQUEST_DEVICE_IDENTITY);
    return Void();
}

Return<void> RadioImpl::exitEmergencyCallbackMode(int32_t serial) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchVoid(serial, mSlotId, RIL_REQUEST_EXIT_EMERGENCY_CALLBACK_MODE);
    return Void();
}

Return<void> RadioImpl::getSmscAddress(int32_t serial) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchVoid(serial, mSlotId, RIL_REQUEST_GET_SMSC_ADDRESS);
    return Void();
}

Return<void> RadioImpl::setSmscAddress(int32_t serial, const hidl_string& smsc) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchString(serial, mSlotId, RIL_REQUEST_SET_SMSC_ADDRESS,
            smsc.c_str());
    return Void();
}

Return<void> RadioImpl::reportSmsMemoryStatus(int32_t serial, bool available) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchInts(serial, mSlotId, RIL_REQUEST_REPORT_SMS_MEMORY_STATUS, 1,
            BOOL_TO_INT(available));
    return Void();
}

Return<void> RadioImpl::reportStkServiceIsRunning(int32_t serial) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchVoid(serial, mSlotId, RIL_REQUEST_REPORT_STK_SERVICE_IS_RUNNING);
    return Void();
}

Return<void> RadioImpl::getCdmaSubscriptionSource(int32_t serial) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchVoid(serial, mSlotId, RIL_REQUEST_CDMA_GET_SUBSCRIPTION_SOURCE);
    return Void();
}

Return<void> RadioImpl::requestIsimAuthentication(int32_t serial, const hidl_string& challenge) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchString(serial, mSlotId, RIL_REQUEST_ISIM_AUTHENTICATION,
            challenge.c_str());
    return Void();
}

Return<void> RadioImpl::acknowledgeIncomingGsmSmsWithPdu(int32_t serial, bool success,
                                                         const hidl_string& ackPdu) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchStrings(serial, mSlotId, RIL_REQUEST_ACKNOWLEDGE_INCOMING_GSM_SMS_WITH_PDU, false,
            2, success ? "1" : "0", ackPdu.c_str());
    return Void();
}

Return<void> RadioImpl::sendEnvelopeWithStatus(int32_t serial, const hidl_string& contents) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchString(serial, mSlotId, RIL_REQUEST_STK_SEND_ENVELOPE_WITH_STATUS,
            contents.c_str());
    return Void();
}

Return<void> RadioImpl::getVoiceRadioTechnology(int32_t serial) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchVoid(serial, mSlotId, RIL_REQUEST_VOICE_RADIO_TECH);
    return Void();
}

Return<void> RadioImpl::getCellInfoList(int32_t serial) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchVoid(serial, mSlotId, RIL_REQUEST_GET_CELL_INFO_LIST);
    return Void();
}

Return<void> RadioImpl::setCellInfoListRate(int32_t serial, int32_t rate) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchInts(serial, mSlotId, RIL_REQUEST_SET_UNSOL_CELL_INFO_LIST_RATE, 1, rate);
    return Void();
}

Return<void> RadioImpl::setInitialAttachApn(int32_t serial, const DataProfileInfo& dataProfileInfo,
                                            bool modemCognitive, bool isRoaming) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    RequestInfo *pRI = android::addRequestToList(serial, mSlotId,
            RIL_REQUEST_SET_INITIAL_ATTACH_APN);
    if (pRI == NULL) {
        return Void();
    }

    if (s_vendorFunctions->version <= 14) {
        RLOGE("Unsupported RIL version %d, min version expected 15", s_vendorFunctions->version);
        if (pRI != NULL) {
            sendErrorResponse(pRI, RIL_E_REQUEST_NOT_SUPPORTED);
        }
    } else {
        RIL_InitialAttachApn_v15 iaa = {};

        if (!copyHidlStringToRil(&iaa.apn, dataProfileInfo.apn, pRI, true)) {
            return Void();
        }

        if (!copyHidlStringToRil(&iaa.protocol, dataProfileInfo.protocol, pRI, true)) {
            memsetAndFreeStrings(1, iaa.apn);
            return Void();
        }
        if (!copyHidlStringToRil(&iaa.roamingProtocol, dataProfileInfo.roamingProtocol, pRI, true)) {
            memsetAndFreeStrings(2, iaa.apn, iaa.protocol);
            return Void();
        }
        iaa.authtype = (int) dataProfileInfo.authType;
        if (!copyHidlStringToRil(&iaa.username, dataProfileInfo.user, pRI, true)) {
            memsetAndFreeStrings(3, iaa.apn, iaa.protocol, iaa.roamingProtocol);
            return Void();
        }
        if (!copyHidlStringToRil(&iaa.password, dataProfileInfo.password, pRI, true)) {
            memsetAndFreeStrings(4, iaa.apn, iaa.protocol, iaa.roamingProtocol, iaa.username);
            return Void();
        }
        iaa.supportedTypesBitmask = dataProfileInfo.supportedApnTypesBitmap;
        iaa.bearerBitmask = dataProfileInfo.bearerBitmap;
        iaa.modemCognitive = BOOL_TO_INT(modemCognitive);
        iaa.mtu = dataProfileInfo.mtu;

        /*
        if (!convertMvnoTypeToString(dataProfileInfo.mvnoType, iaa.mvnoType)) {
            sendErrorResponse(pRI, RIL_E_INVALID_ARGUMENTS);
            memsetAndFreeStrings(5, iaa.apn, iaa.protocol, iaa.roamingProtocol, iaa.username,
                    iaa.password);
            return Void();
        }

        if (!copyHidlStringToRil(&iaa.mvnoMatchData, dataProfileInfo.mvnoMatchData, pRI)) {
            memsetAndFreeStrings(5, iaa.apn, iaa.protocol, iaa.roamingProtocol, iaa.username,
                    iaa.password);
            return Void();
        }
        */

        CALL_ONREQUEST(RIL_REQUEST_SET_INITIAL_ATTACH_APN, &iaa, sizeof(iaa), pRI, mSlotId);

        memsetAndFreeStrings(5, iaa.apn, iaa.protocol, iaa.roamingProtocol, iaa.username,
                iaa.password);
    }

    return Void();
}

Return<void> RadioImpl::getImsRegistrationState(int32_t serial) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchVoid(serial, mSlotId, RIL_REQUEST_IMS_REGISTRATION_STATE);
    return Void();
}

bool dispatchImsGsmSms(const ImsSmsMessage& message, RequestInfo *pRI) {
    RIL_IMS_SMS_Message rism = {};
    char **pStrings;
    int countStrings = 2;
    int dataLen = sizeof(char *) * countStrings;

    rism.tech = RADIO_TECH_3GPP;
    rism.retry = BOOL_TO_INT(message.retry);
    rism.messageRef = message.messageRef;

    if (message.gsmMessage.size() != 1) {
        RLOGE("dispatchImsGsmSms: Invalid len %s", requestToString(pRI->pCI->requestNumber));
        sendErrorResponse(pRI, RIL_E_INVALID_ARGUMENTS);
        return false;
    }

    pStrings = (char **)calloc(countStrings, sizeof(char *));
    if (pStrings == NULL) {
        RLOGE("dispatchImsGsmSms: Memory allocation failed for request %s",
                requestToString(pRI->pCI->requestNumber));
        sendErrorResponse(pRI, RIL_E_NO_MEMORY);
        return false;
    }

    if (!copyHidlStringToRil(&pStrings[0], message.gsmMessage[0].smscPdu, pRI)) {
#ifdef MEMSET_FREED
        memset(pStrings, 0, dataLen);
#endif
        free(pStrings);
        return false;
    }

    if (!copyHidlStringToRil(&pStrings[1], message.gsmMessage[0].pdu, pRI)) {
        memsetAndFreeStrings(1, pStrings[0]);
#ifdef MEMSET_FREED
        memset(pStrings, 0, dataLen);
#endif
        free(pStrings);
        return false;
    }

    rism.message.gsmMessage = pStrings;
    CALL_ONREQUEST(pRI->pCI->requestNumber, &rism, sizeof(RIL_RadioTechnologyFamily) +
            sizeof(uint8_t) + sizeof(int32_t) + dataLen, pRI, pRI->socket_id);

    for (int i = 0 ; i < countStrings ; i++) {
        memsetAndFreeStrings(1, pStrings[i]);
    }

#ifdef MEMSET_FREED
    memset(pStrings, 0, dataLen);
#endif
    free(pStrings);

    return true;
}

struct ImsCdmaSms {
    RIL_IMS_SMS_Message imsSms;
    RIL_CDMA_SMS_Message cdmaSms;
};

bool dispatchImsCdmaSms(const ImsSmsMessage& message, RequestInfo *pRI) {
    ImsCdmaSms temp = {};

    if (message.cdmaMessage.size() != 1) {
        RLOGE("dispatchImsCdmaSms: Invalid len %s", requestToString(pRI->pCI->requestNumber));
        sendErrorResponse(pRI, RIL_E_INVALID_ARGUMENTS);
        return false;
    }

    temp.imsSms.tech = RADIO_TECH_3GPP2;
    temp.imsSms.retry = BOOL_TO_INT(message.retry);
    temp.imsSms.messageRef = message.messageRef;
    temp.imsSms.message.cdmaMessage = &temp.cdmaSms;

    constructCdmaSms(temp.cdmaSms, message.cdmaMessage[0]);

    // Vendor code expects payload length to include actual msg payload
    // (sizeof(RIL_CDMA_SMS_Message)) instead of (RIL_CDMA_SMS_Message *) + size of other fields in
    // RIL_IMS_SMS_Message
    int payloadLen = sizeof(RIL_RadioTechnologyFamily) + sizeof(uint8_t) + sizeof(int32_t)
            + sizeof(RIL_CDMA_SMS_Message);

    CALL_ONREQUEST(pRI->pCI->requestNumber, &temp.imsSms, payloadLen, pRI, pRI->socket_id);

    return true;
}

Return<void> RadioImpl::sendImsSms(int32_t serial, const ImsSmsMessage& message) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    RequestInfo *pRI = android::addRequestToList(serial, mSlotId, RIL_REQUEST_IMS_SEND_SMS);
    if (pRI == NULL) {
        return Void();
    }

    RIL_RadioTechnologyFamily format = (RIL_RadioTechnologyFamily) message.tech;

    if (RADIO_TECH_3GPP == format) {
        dispatchImsGsmSms(message, pRI);
    } else if (RADIO_TECH_3GPP2 == format) {
        dispatchImsCdmaSms(message, pRI);
    } else {
        RLOGE("sendImsSms: Invalid radio tech %s",
                requestToString(pRI->pCI->requestNumber));
        sendErrorResponse(pRI, RIL_E_INVALID_ARGUMENTS);
    }
    return Void();
}

Return<void> RadioImpl::iccTransmitApduBasicChannel(int32_t serial, const SimApdu& message) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchIccApdu(serial, mSlotId, RIL_REQUEST_SIM_TRANSMIT_APDU_BASIC, message);
    return Void();
}

Return<void> RadioImpl::iccOpenLogicalChannel(int32_t serial, const hidl_string& aid, int32_t p2) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    if (s_vendorFunctions->version < 15) {
        dispatchString(serial, mSlotId, RIL_REQUEST_SIM_OPEN_CHANNEL, aid.c_str());
    } else {
        RequestInfo *pRI = android::addRequestToList(serial, mSlotId, RIL_REQUEST_SIM_OPEN_CHANNEL);
        if (pRI == NULL) {
            return Void();
        }

        RIL_OpenChannelParams params = {};

        params.p2 = p2;

        if (!copyHidlStringToRil(&params.aidPtr, aid, pRI)) {
            return Void();
        }

        CALL_ONREQUEST(pRI->pCI->requestNumber, &params, sizeof(params), pRI, mSlotId);

        memsetAndFreeStrings(1, params.aidPtr);
    }
    return Void();
}

Return<void> RadioImpl::iccCloseLogicalChannel(int32_t serial, int32_t channelId) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchInts(serial, mSlotId, RIL_REQUEST_SIM_CLOSE_CHANNEL, 1, channelId);
    return Void();
}

Return<void> RadioImpl::iccTransmitApduLogicalChannel(int32_t serial, const SimApdu& message) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchIccApdu(serial, mSlotId, RIL_REQUEST_SIM_TRANSMIT_APDU_CHANNEL, message);
    return Void();
}

Return<void> RadioImpl::nvReadItem(int32_t serial, NvItem itemId) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    RequestInfo *pRI = android::addRequestToList(serial, mSlotId, RIL_REQUEST_NV_READ_ITEM);
    if (pRI == NULL) {
        return Void();
    }

    RIL_NV_ReadItem nvri = {};
    nvri.itemID = (RIL_NV_Item) itemId;

    CALL_ONREQUEST(pRI->pCI->requestNumber, &nvri, sizeof(nvri), pRI, mSlotId);
    return Void();
}

Return<void> RadioImpl::nvWriteItem(int32_t serial, const NvWriteItem& item) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    RequestInfo *pRI = android::addRequestToList(serial, mSlotId, RIL_REQUEST_NV_WRITE_ITEM);
    if (pRI == NULL) {
        return Void();
    }

    RIL_NV_WriteItem nvwi = {};

    nvwi.itemID = (RIL_NV_Item) item.itemId;

    if (!copyHidlStringToRil(&nvwi.value, item.value, pRI)) {
        return Void();
    }

    CALL_ONREQUEST(pRI->pCI->requestNumber, &nvwi, sizeof(nvwi), pRI, mSlotId);

    memsetAndFreeStrings(1, nvwi.value);
    return Void();
}

Return<void> RadioImpl::nvWriteCdmaPrl(int32_t serial, const hidl_vec<uint8_t>& prl) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchRaw(serial, mSlotId, RIL_REQUEST_NV_WRITE_CDMA_PRL, prl);
    return Void();
}

Return<void> RadioImpl::nvResetConfig(int32_t serial, ResetNvType resetType) {
    int rilResetType = -1;
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    /* Convert ResetNvType to RIL.h values
     * RIL_REQUEST_NV_RESET_CONFIG
     * 1 - reload all NV items
     * 2 - erase NV reset (SCRTN)
     * 3 - factory reset (RTN)
     */
    switch(resetType) {
      case ResetNvType::RELOAD:
        rilResetType = 1;
        break;
      case ResetNvType::ERASE:
        rilResetType = 2;
        break;
      case ResetNvType::FACTORY_RESET:
        rilResetType = 3;
        break;
    }
    dispatchInts(serial, mSlotId, RIL_REQUEST_NV_RESET_CONFIG, 1, rilResetType);
    return Void();
}

Return<void> RadioImpl::setUiccSubscription(int32_t serial, const SelectUiccSub& uiccSub) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    RequestInfo *pRI = android::addRequestToList(serial, mSlotId,
            RIL_REQUEST_SET_UICC_SUBSCRIPTION);
    if (pRI == NULL) {
        return Void();
    }

    RIL_SelectUiccSub rilUiccSub = {};

    rilUiccSub.slot = uiccSub.slot;
    rilUiccSub.app_index = uiccSub.appIndex;
    rilUiccSub.sub_type = (RIL_SubscriptionType) uiccSub.subType;
    rilUiccSub.act_status = (RIL_UiccSubActStatus) uiccSub.actStatus;

    CALL_ONREQUEST(pRI->pCI->requestNumber, &rilUiccSub, sizeof(rilUiccSub), pRI, mSlotId);
    return Void();
}

Return<void> RadioImpl::setDataAllowed(int32_t serial, bool allow) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchInts(serial, mSlotId, RIL_REQUEST_ALLOW_DATA, 1, BOOL_TO_INT(allow));
    return Void();
}

Return<void> RadioImpl::getHardwareConfig(int32_t serial) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchVoid(serial, mSlotId, RIL_REQUEST_GET_HARDWARE_CONFIG);
    return Void();
}

Return<void> RadioImpl::requestIccSimAuthentication(int32_t serial, int32_t authContext,
        const hidl_string& authData, const hidl_string& aid) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    RequestInfo *pRI = android::addRequestToList(serial, mSlotId, RIL_REQUEST_SIM_AUTHENTICATION);
    if (pRI == NULL) {
        return Void();
    }

    RIL_SimAuthentication pf = {};

    pf.authContext = authContext;

    if (!copyHidlStringToRil(&pf.authData, authData, pRI)) {
        return Void();
    }

    if (!copyHidlStringToRil(&pf.aid, aid, pRI)) {
        memsetAndFreeStrings(1, pf.authData);
        return Void();
    }

    CALL_ONREQUEST(pRI->pCI->requestNumber, &pf, sizeof(pf), pRI, mSlotId);

    memsetAndFreeStrings(2, pf.authData, pf.aid);
    return Void();
}

/**
 * @param numProfiles number of data profile
 * @param dataProfiles the pointer to the actual data profiles. The acceptable type is
          RIL_DataProfileInfo or RIL_DataProfileInfo_v15.
 * @param dataProfilePtrs the pointer to the pointers that point to each data profile structure
 * @param numfields number of string-type member in the data profile structure
 * @param ... the variadic parameters are pointers to each string-type member
 **/
template <typename T>
void freeSetDataProfileData(int numProfiles, T *dataProfiles, T **dataProfilePtrs,
                            int numfields, ...) {
    va_list args;
    va_start(args, numfields);

    // Iterate through each string-type field that need to be free.
    for (int i = 0; i < numfields; i++) {
        // Iterate through each data profile and free that specific string-type field.
        // The type 'char *T::*' is a type of pointer to a 'char *' member inside T structure.
        char *T::*ptr = va_arg(args, char *T::*);
        for (int j = 0; j < numProfiles; j++) {
            memsetAndFreeStrings(1, dataProfiles[j].*ptr);
        }
    }

    va_end(args);

#ifdef MEMSET_FREED
    memset(dataProfiles, 0, numProfiles * sizeof(T));
    memset(dataProfilePtrs, 0, numProfiles * sizeof(T *));
#endif
    free(dataProfiles);
    free(dataProfilePtrs);
}

Return<void> RadioImpl::setDataProfile(int32_t serial, const hidl_vec<DataProfileInfo>& profiles,
                                       bool isRoaming) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    RequestInfo *pRI = android::addRequestToList(serial, mSlotId, RIL_REQUEST_SET_DATA_PROFILE);
    if (pRI == NULL) {
        return Void();
    }

    size_t num = profiles.size();
    bool success = false;

    if (s_vendorFunctions->version <= 14) {

        RIL_DataProfileInfo *dataProfiles =
            (RIL_DataProfileInfo *) calloc(num, sizeof(RIL_DataProfileInfo));

        if (dataProfiles == NULL) {
            RLOGE("Memory allocation failed for request %s",
                    requestToString(pRI->pCI->requestNumber));
            sendErrorResponse(pRI, RIL_E_NO_MEMORY);
            return Void();
        }

        RIL_DataProfileInfo **dataProfilePtrs =
            (RIL_DataProfileInfo **) calloc(num, sizeof(RIL_DataProfileInfo *));
        if (dataProfilePtrs == NULL) {
            RLOGE("Memory allocation failed for request %s",
                    requestToString(pRI->pCI->requestNumber));
            free(dataProfiles);
            sendErrorResponse(pRI, RIL_E_NO_MEMORY);
            return Void();
        }

        for (size_t i = 0; i < num; i++) {
            dataProfilePtrs[i] = &dataProfiles[i];

            success = copyHidlStringToRil(&dataProfiles[i].apn, profiles[i].apn, pRI, true);

            const hidl_string &protocol =
                    (isRoaming ? profiles[i].roamingProtocol : profiles[i].protocol);

            if (success && !copyHidlStringToRil(&dataProfiles[i].protocol, protocol, pRI, true)) {
                success = false;
            }

            if (success && !copyHidlStringToRil(&dataProfiles[i].user, profiles[i].user, pRI,
                    true)) {
                success = false;
            }
            if (success && !copyHidlStringToRil(&dataProfiles[i].password, profiles[i].password,
                    pRI, true)) {
                success = false;
            }

            if (!success) {
                freeSetDataProfileData(num, dataProfiles, dataProfilePtrs, 4,
                    &RIL_DataProfileInfo::apn, &RIL_DataProfileInfo::protocol,
                    &RIL_DataProfileInfo::user, &RIL_DataProfileInfo::password);
                return Void();
            }

            dataProfiles[i].profileId = (RIL_DataProfile) profiles[i].profileId;
            dataProfiles[i].authType = (int) profiles[i].authType;
            dataProfiles[i].type = (int) profiles[i].type;
            dataProfiles[i].maxConnsTime = profiles[i].maxConnsTime;
            dataProfiles[i].maxConns = profiles[i].maxConns;
            dataProfiles[i].waitTime = profiles[i].waitTime;
            dataProfiles[i].enabled = BOOL_TO_INT(profiles[i].enabled);
        }

        CALL_ONREQUEST(RIL_REQUEST_SET_DATA_PROFILE, dataProfilePtrs,
                num * sizeof(RIL_DataProfileInfo *), pRI, mSlotId);

        freeSetDataProfileData(num, dataProfiles, dataProfilePtrs, 4,
                &RIL_DataProfileInfo::apn, &RIL_DataProfileInfo::protocol,
                &RIL_DataProfileInfo::user, &RIL_DataProfileInfo::password);
    } else {
        RIL_DataProfileInfo_v15 *dataProfiles =
            (RIL_DataProfileInfo_v15 *) calloc(num, sizeof(RIL_DataProfileInfo_v15));

        if (dataProfiles == NULL) {
            RLOGE("Memory allocation failed for request %s",
                    requestToString(pRI->pCI->requestNumber));
            sendErrorResponse(pRI, RIL_E_NO_MEMORY);
            return Void();
        }

        RIL_DataProfileInfo_v15 **dataProfilePtrs =
            (RIL_DataProfileInfo_v15 **) calloc(num, sizeof(RIL_DataProfileInfo_v15 *));
        if (dataProfilePtrs == NULL) {
            RLOGE("Memory allocation failed for request %s",
                    requestToString(pRI->pCI->requestNumber));
            free(dataProfiles);
            sendErrorResponse(pRI, RIL_E_NO_MEMORY);
            return Void();
        }

        for (size_t i = 0; i < num; i++) {
            dataProfilePtrs[i] = &dataProfiles[i];

            success = copyHidlStringToRil(&dataProfiles[i].apn, profiles[i].apn, pRI, true);
            if (success && !copyHidlStringToRil(&dataProfiles[i].protocol, profiles[i].protocol,
                    pRI)) {
                success = false;
            }
            if (success && !copyHidlStringToRil(&dataProfiles[i].roamingProtocol,
                    profiles[i].roamingProtocol, pRI, true)) {
                success = false;
            }
            if (success && !copyHidlStringToRil(&dataProfiles[i].user, profiles[i].user, pRI,
                    true)) {
                success = false;
            }
            if (success && !copyHidlStringToRil(&dataProfiles[i].password, profiles[i].password,
                    pRI, true)) {
                success = false;
            }
            /*
            if (success && !copyHidlStringToRil(&dataProfiles[i].mvnoMatchData,
                    profiles[i].mvnoMatchData, pRI, true)) {
                success = false;
            }

            if (success && !convertMvnoTypeToString(profiles[i].mvnoType,
                    dataProfiles[i].mvnoType)) {
                sendErrorResponse(pRI, RIL_E_INVALID_ARGUMENTS);
                success = false;
            }
            */

            if (!success) {
                freeSetDataProfileData(num, dataProfiles, dataProfilePtrs, 5,
                    &RIL_DataProfileInfo_v15::apn, &RIL_DataProfileInfo_v15::protocol,
                    &RIL_DataProfileInfo_v15::roamingProtocol, &RIL_DataProfileInfo_v15::user,
                    &RIL_DataProfileInfo_v15::password);
                return Void();
            }

            dataProfiles[i].profileId = (RIL_DataProfile) profiles[i].profileId;
            dataProfiles[i].authType = (int) profiles[i].authType;
            dataProfiles[i].type = (int) profiles[i].type;
            dataProfiles[i].maxConnsTime = profiles[i].maxConnsTime;
            dataProfiles[i].maxConns = profiles[i].maxConns;
            dataProfiles[i].waitTime = profiles[i].waitTime;
            dataProfiles[i].enabled = BOOL_TO_INT(profiles[i].enabled);
            dataProfiles[i].supportedTypesBitmask = profiles[i].supportedApnTypesBitmap;
            dataProfiles[i].bearerBitmask = profiles[i].bearerBitmap;
            dataProfiles[i].mtu = profiles[i].mtu;
        }

        CALL_ONREQUEST(RIL_REQUEST_SET_DATA_PROFILE, dataProfilePtrs,
                num * sizeof(RIL_DataProfileInfo_v15 *), pRI, mSlotId);

        freeSetDataProfileData(num, dataProfiles, dataProfilePtrs, 5,
                &RIL_DataProfileInfo_v15::apn, &RIL_DataProfileInfo_v15::protocol,
                &RIL_DataProfileInfo_v15::roamingProtocol, &RIL_DataProfileInfo_v15::user,
                &RIL_DataProfileInfo_v15::password);
    }

    return Void();
}

Return<void> RadioImpl::requestShutdown(int32_t serial) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchVoid(serial, mSlotId, RIL_REQUEST_SHUTDOWN);
    return Void();
}

Return<void> RadioImpl::getRadioCapability(int32_t serial) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchVoid(serial, mSlotId, RIL_REQUEST_GET_RADIO_CAPABILITY);
    return Void();
}

Return<void> RadioImpl::setRadioCapability(int32_t serial, const RadioCapability& rc) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    RequestInfo *pRI = android::addRequestToList(serial, mSlotId, RIL_REQUEST_SET_RADIO_CAPABILITY);
    if (pRI == NULL) {
        return Void();
    }

    RIL_RadioCapability rilRc = {};

    // TODO : set rilRc.version using HIDL version ?
    rilRc.session = rc.session;
    rilRc.phase = (int) rc.phase;
    rilRc.rat = (int) rc.raf;
    rilRc.status = (int) rc.status;
    strlcpy(rilRc.logicalModemUuid, rc.logicalModemUuid.c_str(), sizeof(rilRc.logicalModemUuid));

    CALL_ONREQUEST(pRI->pCI->requestNumber, &rilRc, sizeof(rilRc), pRI, mSlotId);

    return Void();
}

Return<void> RadioImpl::startLceService(int32_t serial, int32_t reportInterval, bool pullMode) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchInts(serial, mSlotId, RIL_REQUEST_START_LCE, 2, reportInterval,
            BOOL_TO_INT(pullMode));
    return Void();
}

Return<void> RadioImpl::stopLceService(int32_t serial) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchVoid(serial, mSlotId, RIL_REQUEST_STOP_LCE);
    return Void();
}

Return<void> RadioImpl::pullLceData(int32_t serial) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchVoid(serial, mSlotId, RIL_REQUEST_PULL_LCEDATA);
    return Void();
}

Return<void> RadioImpl::getModemActivityInfo(int32_t serial) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchVoid(serial, mSlotId, RIL_REQUEST_GET_ACTIVITY_INFO);
    return Void();
}

Return<void> RadioImpl::setAllowedCarriers(int32_t serial, bool allAllowed,
                                           const CarrierRestrictions& carriers) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    RequestInfo *pRI = android::addRequestToList(serial, mSlotId,
            RIL_REQUEST_SET_CARRIER_RESTRICTIONS);
    if (pRI == NULL) {
        return Void();
    }

    RIL_CarrierRestrictions cr = {};
    RIL_Carrier *allowedCarriers = NULL;
    RIL_Carrier *excludedCarriers = NULL;

    cr.len_allowed_carriers = carriers.allowedCarriers.size();
    allowedCarriers = (RIL_Carrier *)calloc(cr.len_allowed_carriers, sizeof(RIL_Carrier));
    if (allowedCarriers == NULL) {
        RLOGE("%s: Memory allocation failed for request %s",
                __FUNCTION__, requestToString(pRI->pCI->requestNumber));
        sendErrorResponse(pRI, RIL_E_NO_MEMORY);
        return Void();
    }
    cr.allowed_carriers = allowedCarriers;

    cr.len_excluded_carriers = carriers.excludedCarriers.size();
    excludedCarriers = (RIL_Carrier *)calloc(cr.len_excluded_carriers, sizeof(RIL_Carrier));
    if (excludedCarriers == NULL) {
        RLOGE("%s: Memory allocation failed for request %s",
                __FUNCTION__, requestToString(pRI->pCI->requestNumber));
        sendErrorResponse(pRI, RIL_E_NO_MEMORY);
#ifdef MEMSET_FREED
        memset(allowedCarriers, 0, cr.len_allowed_carriers * sizeof(RIL_Carrier));
#endif
        free(allowedCarriers);
        return Void();
    }
    cr.excluded_carriers = excludedCarriers;

    for (int i = 0; i < cr.len_allowed_carriers; i++) {
        allowedCarriers[i].mcc = carriers.allowedCarriers[i].mcc.c_str();
        allowedCarriers[i].mnc = carriers.allowedCarriers[i].mnc.c_str();
        allowedCarriers[i].match_type = (RIL_CarrierMatchType) carriers.allowedCarriers[i].matchType;
        allowedCarriers[i].match_data = carriers.allowedCarriers[i].matchData.c_str();
    }

    for (int i = 0; i < cr.len_excluded_carriers; i++) {
        excludedCarriers[i].mcc = carriers.excludedCarriers[i].mcc.c_str();
        excludedCarriers[i].mnc = carriers.excludedCarriers[i].mnc.c_str();
        excludedCarriers[i].match_type =
                (RIL_CarrierMatchType) carriers.excludedCarriers[i].matchType;
        excludedCarriers[i].match_data = carriers.excludedCarriers[i].matchData.c_str();
    }

    CALL_ONREQUEST(pRI->pCI->requestNumber, &cr, sizeof(RIL_CarrierRestrictions), pRI, mSlotId);

#ifdef MEMSET_FREED
    memset(allowedCarriers, 0, cr.len_allowed_carriers * sizeof(RIL_Carrier));
    memset(excludedCarriers, 0, cr.len_excluded_carriers * sizeof(RIL_Carrier));
#endif
    free(allowedCarriers);
    free(excludedCarriers);
    return Void();
}

Return<void> RadioImpl::getAllowedCarriers(int32_t serial) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchVoid(serial, mSlotId, RIL_REQUEST_GET_CARRIER_RESTRICTIONS);
    return Void();
}

Return<void> RadioImpl::sendDeviceState(int32_t serial, DeviceStateType deviceStateType,
                                        bool state) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    if (s_vendorFunctions->version < 15) {
        if (deviceStateType ==  DeviceStateType::LOW_DATA_EXPECTED) {
            RLOGD("%s: calling screen state %d", __FUNCTION__, BOOL_TO_INT(!state));
            dispatchInts(serial, mSlotId, RIL_REQUEST_SCREEN_STATE, 1, BOOL_TO_INT(!state));
        } else {
            RequestInfo *pRI = android::addRequestToList(serial, mSlotId,
                    RIL_REQUEST_SEND_DEVICE_STATE);
            sendErrorResponse(pRI, RIL_E_REQUEST_NOT_SUPPORTED);
        }
        return Void();
    }
    dispatchInts(serial, mSlotId, RIL_REQUEST_SEND_DEVICE_STATE, 2, (int) deviceStateType,
            BOOL_TO_INT(state));
    return Void();
}

Return<void> RadioImpl::setIndicationFilter(int32_t serial, int32_t indicationFilter) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    if (s_vendorFunctions->version < 15) {
        RequestInfo *pRI = android::addRequestToList(serial, mSlotId,
                RIL_REQUEST_SET_UNSOLICITED_RESPONSE_FILTER);
        sendErrorResponse(pRI, RIL_E_REQUEST_NOT_SUPPORTED);
        return Void();
    }
    dispatchInts(serial, mSlotId, RIL_REQUEST_SET_UNSOLICITED_RESPONSE_FILTER, 1, indicationFilter);
    return Void();
}

Return<void> RadioImpl::setSimCardPower(int32_t serial, bool powerUp) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    dispatchInts(serial, mSlotId, RIL_REQUEST_SET_SIM_CARD_POWER, 1, BOOL_TO_INT(powerUp));
    return Void();
}

Return<void> RadioImpl::setSimCardPower_1_1(int32_t serial, const V1_1::CardPowerState state) {
#if VDBG
    RLOGD("%s: serial %d state %d", __FUNCTION__, serial, state);
#endif
    int request = ENCODE_REQUEST(RIL_REQUEST_SET_SIM_CARD_POWER, HAL_VERSION_CODE(1, 1));
    dispatchInts(serial, mSlotId, request, 1, state);
    return Void();
}

Return<void> RadioImpl::setCarrierInfoForImsiEncryption(int32_t serial,
        const V1_1::ImsiEncryptionInfo& data) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    RequestInfo *pRI = android::addRequestToList(
            serial, mSlotId, RIL_REQUEST_SET_CARRIER_INFO_IMSI_ENCRYPTION);
    if (pRI == NULL) {
        return Void();
    }

    RIL_CarrierInfoForImsiEncryption imsiEncryption = {};

    if (!copyHidlStringToRil(&imsiEncryption.mnc, data.mnc, pRI)) {
        return Void();
    }
    if (!copyHidlStringToRil(&imsiEncryption.mcc, data.mcc, pRI)) {
        memsetAndFreeStrings(1, imsiEncryption.mnc);
        return Void();
    }
    if (!copyHidlStringToRil(&imsiEncryption.keyIdentifier, data.keyIdentifier, pRI)) {
        memsetAndFreeStrings(2, imsiEncryption.mnc, imsiEncryption.mcc);
        return Void();
    }
    imsiEncryption.carrierKeyLength = data.carrierKey.size();
    imsiEncryption.carrierKey = new uint8_t[imsiEncryption.carrierKeyLength];
    memcpy(imsiEncryption.carrierKey, data.carrierKey.data(), imsiEncryption.carrierKeyLength);
    imsiEncryption.expirationTime = data.expirationTime;
    CALL_ONREQUEST(pRI->pCI->requestNumber, &imsiEncryption,
            sizeof(RIL_CarrierInfoForImsiEncryption), pRI, mSlotId);
    delete[](imsiEncryption.carrierKey);
    memsetAndFreeStrings(3, imsiEncryption.mnc, imsiEncryption.mcc, imsiEncryption.keyIdentifier);
    return Void();
}

Return<void> RadioImpl::startKeepalive(int32_t serial, const V1_1::KeepaliveRequest& keepalive) {
#if VDBG
    RLOGD("%s: %d", __FUNCTION__, serial);
#endif
    RequestInfo *pRI = android::addRequestToList(serial, mSlotId, RIL_REQUEST_START_KEEPALIVE);
    if (pRI == NULL) {
        return Void();
    }

    RIL_KeepaliveRequest kaReq = {};

    kaReq.type = static_cast<RIL_KeepaliveType>(keepalive.type);
    switch(kaReq.type) {
        case NATT_IPV4:
            if (keepalive.sourceAddress.size() != 4 ||
                    keepalive.destinationAddress.size() != 4) {
                RLOGE("Invalid address for keepalive!");
                sendErrorResponse(pRI, RIL_E_INVALID_ARGUMENTS);
                return Void();
            }
            break;
        case NATT_IPV6:
            if (keepalive.sourceAddress.size() != 16 ||
                    keepalive.destinationAddress.size() != 16) {
                RLOGE("Invalid address for keepalive!");
                sendErrorResponse(pRI, RIL_E_INVALID_ARGUMENTS);
                return Void();
            }
            break;
        default:
            RLOGE("Unknown packet keepalive type!");
            sendErrorResponse(pRI, RIL_E_INVALID_ARGUMENTS);
            return Void();
    }

    ::memcpy(kaReq.sourceAddress, keepalive.sourceAddress.data(), keepalive.sourceAddress.size());
    kaReq.sourcePort = keepalive.sourcePort;
    if(kaReq.sourcePort>65535){
        RLOGE("Invaild sourcePort!");
        sendErrorResponse(pRI, RIL_E_INVALID_ARGUMENTS);
        return Void();
    }

    ::memcpy(kaReq.destinationAddress,
            keepalive.destinationAddress.data(), keepalive.destinationAddress.size());
    kaReq.destinationPort = keepalive.destinationPort;
    if(kaReq.destinationPort>65535){
        RLOGE("Invaild destinationPort!");
        sendErrorResponse(pRI, RIL_E_INVALID_ARGUMENTS);
        return Void();
    }

    kaReq.maxKeepaliveIntervalMillis = keepalive.maxKeepaliveIntervalMillis;
    kaReq.cid = keepalive.cid; // This is the context ID of the data call

    CALL_ONREQUEST(pRI->pCI->requestNumber, &kaReq, sizeof(RIL_KeepaliveRequest), pRI, mSlotId);
    return Void();
}

Return<void> RadioImpl::stopKeepalive(int32_t serial, int32_t sessionHandle) {
#if VDBG
    RLOGD("%s: %d", __FUNCTION__, serial);
#endif
    RequestInfo *pRI = android::addRequestToList(serial, mSlotId, RIL_REQUEST_STOP_KEEPALIVE);
    if (pRI == NULL) {
        return Void();
    }

    CALL_ONREQUEST(pRI->pCI->requestNumber, &sessionHandle, sizeof(uint32_t), pRI, mSlotId);
    return Void();
}

Return<void> RadioImpl::responseAcknowledgement() {
    android::releaseWakeLock();
    return Void();
}

void RadioImpl::sendStartNetworkScanRequest_V1_2(int32_t serial, int request, const V1_2::NetworkScanRequest &networkScanRequest) {
    RequestInfo *pRI = android::addRequestToList(serial, mSlotId, request);
    if (pRI == NULL) {
        return;
    }

    if (networkScanRequest.specifiers.size() > MAX_RADIO_ACCESS_NETWORKS) {
        sendErrorResponse(pRI, RIL_E_INVALID_ARGUMENTS);
        return;
    }

    RIL_NetworkScanRequest_V1_2 scan_request = {};

    scan_request.type = (RIL_ScanType) networkScanRequest.type;
    scan_request.interval = networkScanRequest.interval;
    scan_request.specifiers_length = networkScanRequest.specifiers.size();
    for (size_t i = 0; i < networkScanRequest.specifiers.size(); ++i) {
        if (networkScanRequest.specifiers[i].geranBands.size() > MAX_BANDS ||
            networkScanRequest.specifiers[i].utranBands.size() > MAX_BANDS ||
            networkScanRequest.specifiers[i].eutranBands.size() > MAX_BANDS ||
            networkScanRequest.specifiers[i].channels.size() > MAX_CHANNELS) {
            sendErrorResponse(pRI, RIL_E_INVALID_ARGUMENTS);
            return;
        }
        const V1_1::RadioAccessSpecifier& ras_from =
                networkScanRequest.specifiers[i];
        RIL_RadioAccessSpecifier& ras_to = scan_request.specifiers[i];

        ras_to.radio_access_network = (RIL_RadioAccessNetworks) ras_from.radioAccessNetwork;
        ras_to.channels_length = ras_from.channels.size();

        std::copy(ras_from.channels.begin(), ras_from.channels.end(), ras_to.channels);
        const std::vector<uint32_t> * bands = nullptr;
        switch (networkScanRequest.specifiers[i].radioAccessNetwork) {
            case V1_1::RadioAccessNetworks::GERAN:
                ras_to.bands_length = ras_from.geranBands.size();
                bands = (std::vector<uint32_t> *) &ras_from.geranBands;
                break;
            case V1_1::RadioAccessNetworks::UTRAN:
                ras_to.bands_length = ras_from.utranBands.size();
                bands = (std::vector<uint32_t> *) &ras_from.utranBands;
                break;
            case V1_1::RadioAccessNetworks::EUTRAN:
                ras_to.bands_length = ras_from.eutranBands.size();
                bands = (std::vector<uint32_t> *) &ras_from.eutranBands;
                break;
            default:
                sendErrorResponse(pRI, RIL_E_INVALID_ARGUMENTS);
                return;
        }
        // safe to copy to geran_bands because it's a union member
        for (size_t idx = 0; idx < ras_to.bands_length; ++idx) {
            ras_to.bands.geran_bands[idx] = (RIL_GeranBands) (*bands)[idx];
        }
    }

    scan_request.maxSearchTime = networkScanRequest.maxSearchTime;
    scan_request.incrementalResults = networkScanRequest.incrementalResults;
    scan_request.incrementalResultsPeriodicity = networkScanRequest.incrementalResultsPeriodicity;
    scan_request.numOfMccMncs = networkScanRequest.mccMncs.size();
    scan_request.mccMncs = (char **)calloc(scan_request.numOfMccMncs, sizeof(char *));
    if (scan_request.mccMncs == NULL) {
        RLOGE("Memory allocation failed for request %s", requestToString(pRI->pCI->requestNumber));
        return;
    }

    for (size_t i = 0; i < networkScanRequest.mccMncs.size(); i++) {
        if (!copyHidlStringToRil(&scan_request.mccMncs[i], networkScanRequest.mccMncs[i], pRI)) {
            for (size_t j = 0; j < i; j++) {
                memsetAndFreeStrings(1, scan_request.mccMncs[j]);
            }
            free(scan_request.mccMncs);
            return;
        }
    }

    CALL_ONREQUEST(request, &scan_request, sizeof(scan_request), pRI, mSlotId);

    if (scan_request.mccMncs != NULL) {
        for (int i = 0 ; i < scan_request.numOfMccMncs; i++) {
            memsetAndFreeStrings(1, scan_request.mccMncs[i]);
        }
        free(scan_request.mccMncs);
    }
}

Return<void> RadioImpl::startNetworkScan_1_2(int32_t serial, const V1_2::NetworkScanRequest &networkScanRequest) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif

    int request = ENCODE_REQUEST(RIL_REQUEST_START_NETWORK_SCAN, HAL_VERSION_CODE(1, 2));
    sendStartNetworkScanRequest_V1_2(serial, request, networkScanRequest);

    return Void();
}

Return<void> RadioImpl::setIndicationFilter_1_2(int32_t serial, hidl_bitfield<V1_2::IndicationFilter> indicationFilter) {
#if VDBG
    RLOGD("%s: %d", __FUNCTION__, serial);
#endif
    int request = ENCODE_REQUEST(RIL_REQUEST_SET_UNSOLICITED_RESPONSE_FILTER, HAL_VERSION_CODE(1,2));
    dispatchInts(serial, mSlotId, request, 1, indicationFilter);
    return Void();
}

Return<void> RadioImpl::setSignalStrengthReportingCriteria(int32_t serial, int32_t hysteresisMs, int32_t hysteresisDb,
        const hidl_vec<int32_t>& thresholdsDbm, V1_2::AccessNetwork accessNetwork) {
#if VDBG
    RLOGD("%s: %d", __FUNCTION__, serial);
#endif
    RequestInfo *pRI = android::addRequestToList(serial, mSlotId, RIL_REQUEST_SET_SIGNAL_STRENGTH_REPORTING_CRITERIA);
    if (pRI == NULL) {
        return Void();
    }

    RIL_SignalStrengthReportingCriteria_V1_2 ss = {};
    ss.hysteresisMs = hysteresisMs;
    ss.hysteresisDb = hysteresisDb;
    ss.numOfThresholdsDbm = thresholdsDbm.size();
    ss.thresholdsDbm = new uint32_t[ss.numOfThresholdsDbm];
    for (size_t i = 0; i < ss.numOfThresholdsDbm; i++) {
        ss.thresholdsDbm[i] = thresholdsDbm[i];
    }

    ss.accessNetwork = (RIL_AccessNetwork) accessNetwork;

    CALL_ONREQUEST(RIL_REQUEST_SET_SIGNAL_STRENGTH_REPORTING_CRITERIA, &ss, sizeof(ss), pRI, mSlotId);
    delete[] ss.thresholdsDbm;
    return Void();

}

Return<void> RadioImpl::setLinkCapacityReportingCriteria(int32_t serial, int32_t hysteresisMs, int32_t hysteresisDlKbps,
        int32_t hysteresisUlKbps, const hidl_vec<int32_t>& thresholdsDownlinkKbps,
        const hidl_vec<int32_t>& thresholdsUplinkKbps, V1_2::AccessNetwork accessNetwork) {
#if VDBG
    RLOGD("%s: %d", __FUNCTION__, serial);
#endif
    RequestInfo *pRI = android::addRequestToList(serial, mSlotId, RIL_REQUEST_SET_LINK_CAPACITY_REPORTING_CRITERIA);
    if (pRI == NULL) {
        return Void();
    }

    RIL_LinkCapacityReportingCriteria_V1_2 lc = {};
    lc.hysteresisMs = hysteresisMs;
    lc.hysteresisDlKbps = hysteresisDlKbps;
    lc.hysteresisUlKbps = hysteresisUlKbps;
    lc.numOfThresholdsDownlinkKbps = thresholdsDownlinkKbps.size();
    lc.thresholdsDownlinkKbps = new uint32_t[lc.numOfThresholdsDownlinkKbps];
    for (size_t i = 0; i < lc.numOfThresholdsDownlinkKbps; i++) {
        lc.thresholdsDownlinkKbps[i] = thresholdsDownlinkKbps[i];
    }

    lc.numOfThresholdsUplinkKbps = thresholdsUplinkKbps.size();
    lc.thresholdsUplinkKbps = new uint32_t[lc.numOfThresholdsUplinkKbps];
    for (size_t i = 0; i < lc.numOfThresholdsUplinkKbps; i++) {
        lc.thresholdsUplinkKbps[i] = thresholdsUplinkKbps[i];
    }
    lc.accessNetwork = (RIL_AccessNetwork) accessNetwork;

    CALL_ONREQUEST(RIL_REQUEST_SET_LINK_CAPACITY_REPORTING_CRITERIA, &lc, sizeof(lc), pRI, mSlotId);
    delete[] lc.thresholdsDownlinkKbps;
    delete[] lc.thresholdsUplinkKbps;
    return Void();
}

Return<void> RadioImpl::setupDataCall_1_2(int32_t serial, V1_2::AccessNetwork accessNetwork,
        const DataProfileInfo& dataProfileInfo, bool modemCognitive, bool roamingAllowed, bool isRoaming, V1_2::DataRequestReason reason,
        const hidl_vec<hidl_string>& addresses, const hidl_vec<hidl_string>& dnses) {
#if VDBG
    RLOGD("%s: %d", __FUNCTION__, serial);
#endif
    // We do not support lower APIs
    RLOGE("Only support 1.4");
    RequestInfo *pRI = android::addRequestToList(serial, mSlotId,
            RIL_REQUEST_SETUP_DATA_CALL);
    if (pRI != NULL) {
        sendErrorResponse(pRI, RIL_E_REQUEST_NOT_SUPPORTED);
    }
    // Intentional force return
    return Void();

    int request = ENCODE_REQUEST(RIL_REQUEST_SETUP_DATA_CALL, HAL_VERSION_CODE(1,2));
    char *mvnoTypeStr = NULL;
    /*
    if (!convertMvnoTypeToString(dataProfileInfo.mvnoType, mvnoTypeStr)) {
        RequestInfo *pRI = android::addRequestToList(serial, mSlotId,
                request);
        if (pRI != NULL) {
            sendErrorResponse(pRI, RIL_E_INVALID_ARGUMENTS);
        }
        return Void();
    }
    */

    int radioTechnology = RADIO_TECH_UNKNOWN;
    switch (accessNetwork) {
        case V1_2::AccessNetwork::GERAN:
            radioTechnology = RADIO_TECH_GPRS;
            break;
        case V1_2::AccessNetwork::UTRAN:
            radioTechnology = RADIO_TECH_UMTS;
            break;
        case V1_2::AccessNetwork::EUTRAN:
            radioTechnology = RADIO_TECH_LTE;
            break;
        case V1_2::AccessNetwork::CDMA2000:
            radioTechnology = RADIO_TECH_1xRTT;
            break;
        case V1_2::AccessNetwork::IWLAN:
            radioTechnology = RADIO_TECH_IWLAN;
            break;
        default:
            break;
    }

    string addressStr = "";
    for (size_t i = 0; i < addresses.size(); i++) {
        addressStr.append(addresses[i]);
        addressStr.append(" ");
    }

    string dnsStr = "";
    for (size_t i = 0; i < dnses.size(); i++) {
        dnsStr.append(dnses[i]);
        dnsStr.append(" ");
    }

    dispatchStrings(serial, mSlotId, request, true, 18,
        std::to_string((int) radioTechnology + 2).c_str(),
        std::to_string((int) dataProfileInfo.profileId).c_str(),
        dataProfileInfo.apn.c_str(),
        dataProfileInfo.user.c_str(),
        dataProfileInfo.password.c_str(),
        std::to_string((int) dataProfileInfo.authType).c_str(),
        dataProfileInfo.protocol.c_str(),
        dataProfileInfo.roamingProtocol.c_str(),
        std::to_string(dataProfileInfo.supportedApnTypesBitmap).c_str(),
        std::to_string(dataProfileInfo.bearerBitmap).c_str(),
        modemCognitive ? "1" : "0",
        std::to_string(dataProfileInfo.mtu).c_str(),
        mvnoTypeStr,
        dataProfileInfo.mvnoMatchData.c_str(),
        roamingAllowed ? "1" : "0",
        std::to_string((int)reason).c_str(),
        addressStr.c_str(),
        dnsStr.c_str());
    return Void();
}

Return<void> RadioImpl::deactivateDataCall_1_2(int32_t serial, int32_t cid, V1_2::DataRequestReason reason) {
#if VDBG
    RLOGD("%s: %d", __FUNCTION__, serial);
#endif
    int request = ENCODE_REQUEST(RIL_REQUEST_DEACTIVATE_DATA_CALL, HAL_VERSION_CODE(1,2));
    dispatchStrings(serial, mSlotId, request, false,
            2, std::to_string(cid).c_str(), std::to_string((int)reason).c_str());

    return Void();
}

///////////////////////////////////////////////////////////////////////////
// radio 1.3
///////////////////////////////////////////////////////////////////////////
int  convertHalToRILRadioAccessSpecifier(RIL_RadioAccessSpecifier_V1_5 *ras_to,
        const V1_1::RadioAccessSpecifier *ras_from) {
    ras_to->radio_access_network = (RIL_RadioAccessNetworks) ras_from->radioAccessNetwork;
    ras_to->channels_length = ras_from->channels.size();

    if (ras_to->channels_length > MAX_CHANNELS) {
        return -1;
    }

    std::copy(ras_from->channels.begin(), ras_from->channels.end(), ras_to->channels);

    const std::vector<uint32_t> * bands = nullptr;
    switch (ras_from->radioAccessNetwork) {
        case V1_1::RadioAccessNetworks::GERAN:
            ras_to->bands_length = ras_from->geranBands.size();
            bands = (std::vector<uint32_t> *) &ras_from->geranBands;
            break;
        case V1_1::RadioAccessNetworks::UTRAN:
            ras_to->bands_length = ras_from->utranBands.size();
            bands = (std::vector<uint32_t> *) &ras_from->utranBands;
            break;
        case V1_1::RadioAccessNetworks::EUTRAN:
            ras_to->bands_length = ras_from->eutranBands.size();
            bands = (std::vector<uint32_t> *) &ras_from->eutranBands;
            break;
        default:
            return -2;
    }

    if (ras_to->bands_length > MAX_BANDS ) {
        return -3;
    }

    // safe to copy to geran_bands because it's a union member
    for (size_t idx = 0; idx < ras_to->bands_length; ++idx) {
        ras_to->bands.geran_bands[idx] = (RIL_GeranBands) (*bands)[idx];
    }

    return 0;
}

Return<void> RadioImpl::setSystemSelectionChannels(int32_t serial, bool specifyChannels,
        const hidl_vec<V1_1::RadioAccessSpecifier>& specifiers) {
#if VDBG
    RLOGD("%s: %d", __FUNCTION__, serial);
#endif
    RequestInfo *pRI = android::addRequestToList(serial, mSlotId, RIL_REQUEST_SET_SYSTEM_SELECTION_CHANNELS);
    if (pRI == NULL) {
        return Void();
    }

    RIL_SysSelChannelRequest_V1_5 sscr = {};
    sscr.isSpecifyChannels = specifyChannels;
    sscr.specifiers_length = specifiers.size();
    if (sscr.specifiers_length > MAX_RADIO_ACCESS_NETWORKS) sscr.specifiers_length = MAX_RADIO_ACCESS_NETWORKS;
    for (size_t i = 0; i < sscr.specifiers_length; ++i) {
        const V1_1::RadioAccessSpecifier *ras_from = &specifiers[i];
        RIL_RadioAccessSpecifier_V1_5 *ras_to = &sscr.specifiers[i];

        int ret = convertHalToRILRadioAccessSpecifier(ras_to, ras_from);
        if (ret < 0) {
            RLOGE("%s: %d, invalid arg(%d)", __FUNCTION__, serial, ret);
            sendErrorResponse(pRI, RIL_E_INVALID_ARGUMENTS);
            return Void();
        }
    }

    CALL_ONREQUEST(RIL_REQUEST_SET_SYSTEM_SELECTION_CHANNELS, &sscr, sizeof(sscr), pRI, mSlotId);
    return Void();
}

Return<void> RadioImpl::enableModem(int32_t serial, bool on) {
#if VDBG
    RLOGD("%s: %d", __FUNCTION__, serial);
#endif
    dispatchInts(serial, mSlotId, RIL_REQUEST_ENABLE_MODEM, 1, BOOL_TO_INT(on));
    return Void();
}

Return<void> RadioImpl::getModemStackStatus(int32_t serial) {
#if VDBG
    RLOGD("%s: %d", __FUNCTION__, serial);
#endif
    dispatchVoid(serial, mSlotId, RIL_REQUEST_GET_MODEM_STATUS);
    return Void();
}

///////////////////////////////////////////////////////////////////////////
// radio 1.4
///////////////////////////////////////////////////////////////////////////
Return<void> RadioImpl::setupDataCall_1_4(int32_t serial, V1_4::AccessNetwork accessNetwork,
        const V1_4::DataProfileInfo& dataProfileInfo, bool roamingAllowed,
        V1_2::DataRequestReason reason, const hidl_vec<hidl_string>& addresses,
        const hidl_vec<hidl_string>& dnses) {
#if VDBG
    RLOGD("%s: %d", __FUNCTION__, serial);
#endif
    int request = ENCODE_REQUEST(RIL_REQUEST_SETUP_DATA_CALL, HAL_VERSION_CODE(1,4));
    RequestInfo *pRI = android::addRequestToList(serial, mSlotId, request);
    if (pRI == NULL) {
        return Void();
    }

    RIL_SetupDataCallInfo_V1_4 setupDataCallInfo = {};
    if (!convertHalDataProfileToRil(setupDataCallInfo.dataProfileInfo, dataProfileInfo, pRI)) {
        sendErrorResponse(pRI, RIL_E_NO_MEMORY);
        return Void();
    }

    setupDataCallInfo.accessNetwork = (int)accessNetwork;
    setupDataCallInfo.roamingAllow = roamingAllowed;
    setupDataCallInfo.reason = (int)reason;

    string addressStr = "";
    for (size_t i = 0; i < addresses.size(); i++) {
        addressStr.append(addresses[i]);
        addressStr.append(" ");
    }
    if (addressStr.size() > 0 &&
        !copyHidlStringToRil(&setupDataCallInfo.addresses, hidl_string(addressStr.c_str()), pRI, true)) {
        freeRilDataProfile(setupDataCallInfo.dataProfileInfo);
        sendErrorResponse(pRI, RIL_E_NO_MEMORY);
        return Void();
    }

    string dnsStr = "";
    for (size_t i = 0; i < dnses.size(); i++) {
        dnsStr.append(dnses[i]);
        dnsStr.append(" ");
    }
    if (dnsStr.size() > 0 &&
        !copyHidlStringToRil(&setupDataCallInfo.dnses, hidl_string(dnsStr.c_str()), pRI, true)) {
        freeRilDataProfile(setupDataCallInfo.dataProfileInfo);
        memsetAndFreeStrings(1, setupDataCallInfo.addresses);
        sendErrorResponse(pRI, RIL_E_NO_MEMORY);
        return Void();
    }

    CALL_ONREQUEST(request, &setupDataCallInfo, sizeof(setupDataCallInfo), pRI, mSlotId);

    freeRilDataProfile(setupDataCallInfo.dataProfileInfo);
    memsetAndFreeStrings(2, setupDataCallInfo.addresses, setupDataCallInfo.dnses);

    return Void();
}

Return<void> RadioImpl::setInitialAttachApn_1_4(int32_t serial, const V1_4::DataProfileInfo& dataProfileInfo) {
#if VDBG
    RLOGD("%s: %d", __FUNCTION__, serial);
#endif
    int request = ENCODE_REQUEST(RIL_REQUEST_SET_INITIAL_ATTACH_APN, HAL_VERSION_CODE(1,4));
    RequestInfo *pRI = android::addRequestToList(serial, mSlotId, request);
    if (pRI == NULL) {
        return Void();
    }

    RIL_DataProfileInfo_V1_4 dataProfile = {};
    memset(&dataProfile, 0, sizeof(dataProfile));
    if (!convertHalDataProfileToRil(dataProfile, dataProfileInfo, pRI)) {
        sendErrorResponse(pRI, RIL_E_NO_MEMORY);
        return Void();
    }
    CALL_ONREQUEST(request, &dataProfile, sizeof(dataProfile), pRI, mSlotId);

    freeRilDataProfile(dataProfile);
    return Void();
}

Return<void> RadioImpl::setDataProfile_1_4(int32_t serial, const hidl_vec<V1_4::DataProfileInfo>& profiles) {
#if VDBG
    RLOGD("%s: %d", __FUNCTION__, serial);
#endif
    int request = ENCODE_REQUEST(RIL_REQUEST_SET_DATA_PROFILE, HAL_VERSION_CODE(1,4));
    RequestInfo *pRI = android::addRequestToList(serial, mSlotId, request);
    if (pRI == NULL) {
        return Void();
    }

    size_t num = profiles.size();

    RIL_DataProfileInfo_V1_4 *dataProfiles =
        (RIL_DataProfileInfo_V1_4 *) calloc(num, sizeof(RIL_DataProfileInfo_V1_4));

    if (dataProfiles == NULL) {
        RLOGE("Memory allocation failed for request %s", requestToString(pRI->pCI->requestNumber));
        sendErrorResponse(pRI, RIL_E_NO_MEMORY);
        return Void();
    }

    RIL_DataProfileInfo_V1_4 **dataProfilePtrs =
        (RIL_DataProfileInfo_V1_4 **) calloc(num, sizeof(RIL_DataProfileInfo_V1_4 *));
    if (dataProfilePtrs == NULL) {
        RLOGE("Memory allocation failed for request %s", requestToString(pRI->pCI->requestNumber));
        free(dataProfiles);
        sendErrorResponse(pRI, RIL_E_NO_MEMORY);
        return Void();
    }

    for (size_t i = 0; i < num; i++) {
        dataProfilePtrs[i] = &dataProfiles[i];
        if (!convertHalDataProfileToRil(dataProfiles[i], profiles[i], pRI)) {
            for (size_t j = 0; j < i; j++) {
                freeRilDataProfile(dataProfiles[j]);
            }
            sendErrorResponse(pRI, RIL_E_NO_MEMORY);
#ifdef MEMSET_FREED
            memset(dataProfiles, 0, num * sizeof(RIL_DataProfileInfo_V1_4));
            memset(dataProfilePtrs, 0, num * sizeof(RIL_DataProfileInfo_V1_4 *));
#endif
            free(dataProfiles);
            free(dataProfilePtrs);
            return Void();
        }
    }

    CALL_ONREQUEST(request, dataProfilePtrs, num * sizeof(RIL_DataProfileInfo_V1_4 *), pRI, mSlotId);

    for (size_t i = 0; i < num; i++) {
        freeRilDataProfile(dataProfiles[i]);
    }

#ifdef MEMSET_FREED
    memset(dataProfiles, 0, num * sizeof(RIL_DataProfileInfo_V1_4));
    memset(dataProfilePtrs, 0, num * sizeof(RIL_DataProfileInfo_V1_4 *));
#endif
    free(dataProfiles);
    free(dataProfilePtrs);

    return Void();
}

Return<void> RadioImpl::emergencyDial(int32_t serial, const Dial& dialInfo,
        hidl_bitfield<V1_4::EmergencyServiceCategory> categories,
        const hidl_vec<hidl_string>& urns, V1_4::EmergencyCallRouting routing,
        bool hasKnownUserIntentEmergency, bool isTesting) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    int request = RIL_REQUEST_EMERGENCY_DIAL;
    sendEmergencyDial(serial, dialInfo, categories, urns, routing, hasKnownUserIntentEmergency, isTesting, request, mSlotId);
    return Void();
}

Return<void> RadioImpl::startNetworkScan_1_4(int32_t serial, const V1_2::NetworkScanRequest& networkScanRequest) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif

    int request = ENCODE_REQUEST(RIL_REQUEST_START_NETWORK_SCAN, HAL_VERSION_CODE(1,4));
    sendStartNetworkScanRequest_V1_2(serial, request, networkScanRequest);

    return Void();
}

Return<void> RadioImpl::getPreferredNetworkTypeBitmap(int32_t serial) {
#if VDBG
    RLOGD("%s: %d", __FUNCTION__, serial);
#endif
    int request = ENCODE_REQUEST(RIL_REQUEST_GET_PREFERRED_NETWORK_TYPE, HAL_VERSION_CODE(1,4));
    dispatchVoid(serial, mSlotId, request);
    return Void();
}

Return<void> RadioImpl::setPreferredNetworkTypeBitmap(int32_t serial, hidl_bitfield<V1_4::RadioAccessFamily> networkTypeBitmap) {
#if VDBG
    RLOGD("%s: %d", __FUNCTION__, serial);
#endif
    int request = ENCODE_REQUEST(RIL_REQUEST_SET_PREFERRED_NETWORK_TYPE, HAL_VERSION_CODE(1,4));
    dispatchInts(serial, mSlotId, request, 1, networkTypeBitmap);
    return Void();
}

Return<void> RadioImpl::setAllowedCarriers_1_4(int32_t serial, const V1_4::CarrierRestrictionsWithPriority& carriers,
        V1_4::SimLockMultiSimPolicy multiSimPolicy) {
#if VDBG
    RLOGD("%s: %d", __FUNCTION__, serial);
#endif
    int request = ENCODE_REQUEST(RIL_REQUEST_SET_CARRIER_RESTRICTIONS, HAL_VERSION_CODE(1, 4));
    RequestInfo *pRI = android::addRequestToList(serial, mSlotId, request);
    if (pRI == NULL) {
        return Void();
    }

    RIL_CarrierRestrictions_V1_4 cr = {};
    RIL_Carrier *allowedCarriers = NULL;
    RIL_Carrier *excludedCarriers = NULL;

    cr.carriers.len_allowed_carriers = carriers.allowedCarriers.size();
    allowedCarriers = (RIL_Carrier *)calloc(cr.carriers.len_allowed_carriers, sizeof(RIL_Carrier));
    if (allowedCarriers == NULL) {
        RLOGE("%s: Memory allocation failed for request %s",
                __FUNCTION__, requestToString(pRI->pCI->requestNumber));
        sendErrorResponse(pRI, RIL_E_NO_MEMORY);
        return Void();
    }
    cr.carriers.allowed_carriers = allowedCarriers;

    cr.carriers.len_excluded_carriers = carriers.excludedCarriers.size();
    excludedCarriers = (RIL_Carrier *)calloc(cr.carriers.len_excluded_carriers, sizeof(RIL_Carrier));
    if (excludedCarriers == NULL) {
        RLOGE("%s: Memory allocation failed for request %s",
                __FUNCTION__, requestToString(pRI->pCI->requestNumber));
        sendErrorResponse(pRI, RIL_E_NO_MEMORY);
#ifdef MEMSET_FREED
        memset(allowedCarriers, 0, cr.carriers.len_allowed_carriers * sizeof(RIL_Carrier));
#endif
        free(allowedCarriers);
        return Void();
    }
    cr.carriers.excluded_carriers = excludedCarriers;

    for (int i = 0; i < cr.carriers.len_allowed_carriers; i++) {
        allowedCarriers[i].mcc = carriers.allowedCarriers[i].mcc.c_str();
        allowedCarriers[i].mnc = carriers.allowedCarriers[i].mnc.c_str();
        allowedCarriers[i].match_type = (RIL_CarrierMatchType) carriers.allowedCarriers[i].matchType;
        allowedCarriers[i].match_data = carriers.allowedCarriers[i].matchData.c_str();
    }

    for (int i = 0; i < cr.carriers.len_excluded_carriers; i++) {
        excludedCarriers[i].mcc = carriers.excludedCarriers[i].mcc.c_str();
        excludedCarriers[i].mnc = carriers.excludedCarriers[i].mnc.c_str();
        excludedCarriers[i].match_type =
                (RIL_CarrierMatchType) carriers.excludedCarriers[i].matchType;
        excludedCarriers[i].match_data = carriers.excludedCarriers[i].matchData.c_str();
    }

    cr.carriers.allowedCarriersPrioritized = carriers.allowedCarriersPrioritized;
    cr.multiSimPolicy = (int)multiSimPolicy;

    CALL_ONREQUEST(request, &cr, sizeof(RIL_CarrierRestrictions_V1_4), pRI, mSlotId);

#ifdef MEMSET_FREED
    memset(allowedCarriers, 0, cr.carriers.len_allowed_carriers * sizeof(RIL_Carrier));
    memset(excludedCarriers, 0, cr.carriers.len_excluded_carriers * sizeof(RIL_Carrier));
#endif
    free(allowedCarriers);
    free(excludedCarriers);

    return Void();
}

Return<void> RadioImpl::getAllowedCarriers_1_4(int32_t serial) {
#if VDBG
    RLOGD("%s: %d", __FUNCTION__, serial);
#endif
    dispatchVoid(serial, mSlotId, ENCODE_REQUEST(RIL_REQUEST_GET_CARRIER_RESTRICTIONS, HAL_VERSION_CODE(1, 4)));
    return Void();
}

Return<void> RadioImpl::getSignalStrength_1_4(int32_t serial) {
#if VDBG
    RLOGD("%s: %d", __FUNCTION__, serial);
#endif
    dispatchVoid(serial, mSlotId, ENCODE_REQUEST(RIL_REQUEST_SIGNAL_STRENGTH, HAL_VERSION_CODE(1,4)));
    return Void();
}

///////////////////////////////////////////////////////////////////////////
// radio 1.5
///////////////////////////////////////////////////////////////////////////
Return<void> RadioImpl::setSignalStrengthReportingCriteria_1_5(int32_t serial,
        const V1_5::SignalThresholdInfo& signalThresholdInfo, V1_5::AccessNetwork accessNetwork) {
#if VDBG
    RLOGD("%s: %d", __FUNCTION__, serial);
#endif
    int request = ENCODE_REQUEST(RIL_REQUEST_SET_SIGNAL_STRENGTH_REPORTING_CRITERIA, HAL_VERSION_CODE(1,5));
    RequestInfo *pRI = android::addRequestToList(serial, mSlotId, request);
    if (pRI == NULL) {
        return Void();
    }

    RIL_SignalStrengthReportingCriteria_V1_5 ssrc = {};
    ssrc.accessNetwork = (int)accessNetwork;
    ssrc.signalThresholdInfo.signalMeasurement = (RIL_SignalMeasurementType)signalThresholdInfo.signalMeasurement;
    ssrc.signalThresholdInfo.hysteresisMs = signalThresholdInfo.hysteresisMs;
    ssrc.signalThresholdInfo.hysteresisDb = signalThresholdInfo.hysteresisDb;
    ssrc.signalThresholdInfo.len_thresholds = signalThresholdInfo.thresholds.size();
    ssrc.signalThresholdInfo.thresholds =  new int32_t[ssrc.signalThresholdInfo.len_thresholds];
    for (size_t i = 0; i < ssrc.signalThresholdInfo.len_thresholds; i++) {
        ssrc.signalThresholdInfo.thresholds[i] = signalThresholdInfo.thresholds[i];
    }
    ssrc.signalThresholdInfo.isEnabled = signalThresholdInfo.isEnabled;
    CALL_ONREQUEST(request, &ssrc, sizeof(ssrc), pRI, mSlotId);

    delete[] ssrc.signalThresholdInfo.thresholds;
    return Void();
}

Return<void> RadioImpl::enableUiccApplications(int32_t serial, bool enable) {
#if VDBG
    RLOGD("%s: %d", __FUNCTION__, serial);
#endif
    dispatchInts(serial, mSlotId, RIL_REQUEST_ENABLE_UICC_APPLICATIONS, 1, BOOL_TO_INT(enable));
    return Void();
}

Return<void> RadioImpl::areUiccApplicationsEnabled(int32_t serial) {
#if VDBG
    RLOGD("%s: %d", __FUNCTION__, serial);
#endif
    dispatchVoid(serial, mSlotId, RIL_REQUEST_GET_UICC_APPLICATIONS_ENABLEMENT);
    return Void();
}

int  convertHalToRILRadioAccessSpecifier(RIL_RadioAccessSpecifier_V1_5 *ras_to,
        const V1_5::RadioAccessSpecifier *ras_from) {
    ras_to->radio_access_network = (RIL_RadioAccessNetworks) ras_from->radioAccessNetwork;
    ras_to->channels_length = ras_from->channels.size();

    if (ras_to->channels_length > MAX_CHANNELS) {
        return -1;
    }

    std::copy(ras_from->channels.begin(), ras_from->channels.end(), ras_to->channels);

    const std::vector<uint32_t> * bands = nullptr;
    switch (ras_from->radioAccessNetwork) {
        case V1_5::RadioAccessNetworks::GERAN:
            ras_to->bands_length = ras_from->bands.geranBands().size();
            bands = (std::vector<uint32_t> *) &ras_from->bands.geranBands();
            break;
        case V1_5::RadioAccessNetworks::UTRAN:
            ras_to->bands_length = ras_from->bands.utranBands().size();
            bands = (std::vector<uint32_t> *) &ras_from->bands.utranBands();
            break;
        case V1_5::RadioAccessNetworks::EUTRAN:
            ras_to->bands_length = ras_from->bands.eutranBands().size();
            bands = (std::vector<uint32_t> *) &ras_from->bands.eutranBands();
            break;
        case V1_5::RadioAccessNetworks::NGRAN:
            ras_to->bands_length = ras_from->bands.ngranBands().size();
            bands = (std::vector<uint32_t> *) &ras_from->bands.ngranBands();
            break;
        default:
            return -2;
    }

    if (ras_to->bands_length > MAX_BANDS ) {
        return -3;
    }

    // safe to copy to geran_bands because it's a union member
    for (size_t idx = 0; idx < ras_to->bands_length; ++idx) {
        ras_to->bands.geran_bands[idx] = (RIL_GeranBands) (*bands)[idx];
    }

    return 0;
}

Return<void> RadioImpl::setSystemSelectionChannels_1_5(int32_t serial, bool specifyChannels,
        const hidl_vec<V1_5::RadioAccessSpecifier>& specifiers) {
#if VDBG
    RLOGD("%s: %d", __FUNCTION__, serial);
#endif
    int request = ENCODE_REQUEST(RIL_REQUEST_SET_SYSTEM_SELECTION_CHANNELS, HAL_VERSION_CODE(1,5));
    RequestInfo *pRI = android::addRequestToList(serial, mSlotId, request);
    if (pRI == NULL) {
        return Void();
    }

    RIL_SysSelChannelRequest_V1_5 sscr = {};
    sscr.isSpecifyChannels = specifyChannels;
    sscr.specifiers_length = specifiers.size();
    if (sscr.specifiers_length > MAX_RADIO_ACCESS_NETWORKS) sscr.specifiers_length = MAX_RADIO_ACCESS_NETWORKS;
    for (size_t i = 0; i < sscr.specifiers_length; ++i) {
        const V1_5::RadioAccessSpecifier *ras_from = &specifiers[i];
        RIL_RadioAccessSpecifier_V1_5 *ras_to = &sscr.specifiers[i];

        int ret = convertHalToRILRadioAccessSpecifier(ras_to, ras_from);
        if (ret < 0) {
            RLOGE("%s: %d, invalid arg(%d)", __FUNCTION__, serial, ret);
            sendErrorResponse(pRI, RIL_E_INVALID_ARGUMENTS);
            return Void();
        }
    }

    CALL_ONREQUEST(request, &sscr, sizeof(sscr), pRI, mSlotId);
    return Void();
}

Return<void> RadioImpl::startNetworkScan_1_5(int32_t serial, const V1_5::NetworkScanRequest& networkScanRequest) {
#if VDBG
    RLOGD("%s: %d", __FUNCTION__, serial);
#endif
    int request = ENCODE_REQUEST(RIL_REQUEST_START_NETWORK_SCAN, HAL_VERSION_CODE(1,5));
    RequestInfo *pRI = android::addRequestToList(serial, mSlotId, request);
    if (pRI == NULL) {
        return Void();
    }

    if (networkScanRequest.specifiers.size() > MAX_RADIO_ACCESS_NETWORKS) {
        sendErrorResponse(pRI, RIL_E_INVALID_ARGUMENTS);
        return Void();
    }

    RIL_NetworkScanRequest_V1_5 scan_request = {};

    scan_request.type = (RIL_ScanType) networkScanRequest.type;
    scan_request.interval = networkScanRequest.interval;
    scan_request.specifiers_length = networkScanRequest.specifiers.size();
    for (size_t i = 0; i < networkScanRequest.specifiers.size(); ++i) {
        if (networkScanRequest.specifiers[i].channels.size() > MAX_CHANNELS) {
            sendErrorResponse(pRI, RIL_E_INVALID_ARGUMENTS);
            return Void();
        }

        const V1_5::RadioAccessSpecifier& ras_from =
                networkScanRequest.specifiers[i];
        RIL_RadioAccessSpecifier_V1_5& ras_to = scan_request.specifiers[i];

        ras_to.radio_access_network = (RIL_RadioAccessNetworks) ras_from.radioAccessNetwork;
        ras_to.channels_length = ras_from.channels.size();
        std::copy(ras_from.channels.begin(), ras_from.channels.end(), ras_to.channels);

        const std::vector<uint32_t> * bands = nullptr;
        switch (networkScanRequest.specifiers[i].radioAccessNetwork) {
            case V1_5::RadioAccessNetworks::GERAN:
                if (ras_from.bands.geranBands().size() > MAX_BANDS) {
                    sendErrorResponse(pRI, RIL_E_INVALID_ARGUMENTS);
                    return Void();
                }
                ras_to.bands_length = ras_from.bands.geranBands().size();
                bands = (std::vector<uint32_t> *) &ras_from.bands.geranBands();
                break;
            case V1_5::RadioAccessNetworks::UTRAN:
                if (ras_from.bands.utranBands().size() > MAX_BANDS) {
                    sendErrorResponse(pRI, RIL_E_INVALID_ARGUMENTS);
                    return Void();
                }
                ras_to.bands_length = ras_from.bands.utranBands().size();
                bands = (std::vector<uint32_t> *) &ras_from.bands.utranBands();
                break;
            case V1_5::RadioAccessNetworks::EUTRAN:
                if (ras_from.bands.eutranBands().size() > MAX_BANDS) {
                    sendErrorResponse(pRI, RIL_E_INVALID_ARGUMENTS);
                    return Void();
                }
                ras_to.bands_length = ras_from.bands.eutranBands().size();
                bands = (std::vector<uint32_t> *) &ras_from.bands.eutranBands();
                break;
            case V1_5::RadioAccessNetworks::NGRAN:
                if (ras_from.bands.ngranBands().size() > MAX_BANDS) {
                    sendErrorResponse(pRI, RIL_E_INVALID_ARGUMENTS);
                    return Void();
                }
                ras_to.bands_length = ras_from.bands.ngranBands().size();
                bands = (std::vector<uint32_t> *) &ras_from.bands.ngranBands();
                break;
            default:
                sendErrorResponse(pRI, RIL_E_INVALID_ARGUMENTS);
                return Void();
        }
        // safe to copy to geran_bands because it's a union member
        for (size_t idx = 0; idx < ras_to.bands_length; ++idx) {
            ras_to.bands.geran_bands[idx] = (RIL_GeranBands) (*bands)[idx];
        }
    }

    scan_request.maxSearchTime = networkScanRequest.maxSearchTime;
    scan_request.incrementalResults = networkScanRequest.incrementalResults;
    scan_request.incrementalResultsPeriodicity = networkScanRequest.incrementalResultsPeriodicity;
    scan_request.numOfMccMncs = networkScanRequest.mccMncs.size();
    scan_request.mccMncs = (char **)calloc(scan_request.numOfMccMncs, sizeof(char *));
    if (scan_request.mccMncs == NULL) {
        RLOGE("Memory allocation failed for request %s", requestToString(pRI->pCI->requestNumber));
        return Void();
    }

    for (size_t i = 0; i < networkScanRequest.mccMncs.size(); i++) {
        if (!copyHidlStringToRil(&scan_request.mccMncs[i], networkScanRequest.mccMncs[i], pRI)) {
            for (size_t j = 0; j < i; j++) {
                memsetAndFreeStrings(1, scan_request.mccMncs[j]);
            }
            free(scan_request.mccMncs);
            return Void();
        }
    }

    CALL_ONREQUEST(request, &scan_request, sizeof(scan_request), pRI, mSlotId);

    if (scan_request.mccMncs != NULL) {
        for (int i = 0 ; i < scan_request.numOfMccMncs; i++) {
            memsetAndFreeStrings(1, scan_request.mccMncs[i]);
        }
        free(scan_request.mccMncs);
    }

    return Void();
}

Return<void> RadioImpl::setupDataCall_1_5(int32_t serial, V1_5::AccessNetwork accessNetwork,
        const V1_5::DataProfileInfo& dataProfileInfo, bool roamingAllowed,
        V1_2::DataRequestReason reason, const hidl_vec<V1_5::LinkAddress>& addresses,
        const hidl_vec<hidl_string>& dnses) {
#if VDBG
    RLOGD("%s: %d", __FUNCTION__, serial);
#endif
    int request = ENCODE_REQUEST(RIL_REQUEST_SETUP_DATA_CALL, HAL_VERSION_CODE(1,5));
    RequestInfo *pRI = android::addRequestToList(serial, mSlotId, request);
    if (pRI == NULL) {
        return Void();
    }

    RIL_SetupDataCallInfo_V1_5 setupDataCallInfo = {};
    bool ret = fillSetupDataCallInfo_V1_5(setupDataCallInfo, accessNetwork, dataProfileInfo,
                                   roamingAllowed, reason, addresses, dnses, pRI);
    if(!ret) return Void();

    CALL_ONREQUEST(request, &setupDataCallInfo, sizeof(setupDataCallInfo), pRI, mSlotId);

    freeSetupDataCallInfo_V1_5(*(RIL_SetupDataCallInfo_V1_5 *)&setupDataCallInfo, addresses);

    return Void();
}

Return<void> RadioImpl::setInitialAttachApn_1_5(int32_t serial, const V1_5::DataProfileInfo& dataProfileInfo) {
#if VDBG
    RLOGD("%s: %d", __FUNCTION__, serial);
#endif
    int request = ENCODE_REQUEST(RIL_REQUEST_SET_INITIAL_ATTACH_APN, HAL_VERSION_CODE(1,5));
    RequestInfo *pRI = android::addRequestToList(serial, mSlotId, request);
    if (pRI == NULL) {
        return Void();
    }

    RIL_DataProfileInfo_V1_5 dataProfile = {};
    memset(&dataProfile, 0, sizeof(dataProfile));
    if (!convertHalDataProfileToRil(dataProfile, dataProfileInfo, pRI)) {
        sendErrorResponse(pRI, RIL_E_NO_MEMORY);
        return Void();
    }
    CALL_ONREQUEST(request, &dataProfile, sizeof(dataProfile), pRI, mSlotId);

    freeRilDataProfile(dataProfile);

    return Void();
}

Return<void> RadioImpl::setDataProfile_1_5(int32_t serial, const hidl_vec<V1_5::DataProfileInfo>& profiles) {
#if VDBG
    RLOGD("%s: %d", __FUNCTION__, serial);
#endif
    int request = ENCODE_REQUEST(RIL_REQUEST_SET_DATA_PROFILE, HAL_VERSION_CODE(1,5));
    RequestInfo *pRI = android::addRequestToList(serial, mSlotId, request);
    if (pRI == NULL) {
        return Void();
    }

    size_t num = profiles.size();

    RIL_DataProfileInfo_V1_5 *dataProfiles =
        (RIL_DataProfileInfo_V1_5 *) calloc(num, sizeof(RIL_DataProfileInfo_V1_5));

    if (dataProfiles == NULL) {
        RLOGE("Memory allocation failed for request %s", requestToString(pRI->pCI->requestNumber));
        sendErrorResponse(pRI, RIL_E_NO_MEMORY);
        return Void();
    }

    RIL_DataProfileInfo_V1_5 **dataProfilePtrs =
        (RIL_DataProfileInfo_V1_5 **) calloc(num, sizeof(RIL_DataProfileInfo_V1_5 *));
    if (dataProfilePtrs == NULL) {
        RLOGE("Memory allocation failed for request %s", requestToString(pRI->pCI->requestNumber));
        free(dataProfiles);
        sendErrorResponse(pRI, RIL_E_NO_MEMORY);
        return Void();
    }

    for (size_t i = 0; i < num; i++) {
        dataProfilePtrs[i] = &dataProfiles[i];
        if (!convertHalDataProfileToRil(dataProfiles[i], profiles[i], pRI)) {
            for (size_t j = 0; j < i; j++) {
                freeRilDataProfile(dataProfiles[j]);
            }
            sendErrorResponse(pRI, RIL_E_NO_MEMORY);
#ifdef MEMSET_FREED
            memset(dataProfiles, 0, num * sizeof(RIL_DataProfileInfo_V1_5));
            memset(dataProfilePtrs, 0, num * sizeof(RIL_DataProfileInfo_V1_5 *));
#endif
            free(dataProfiles);
            free(dataProfilePtrs);
            return Void();
        }
    }

    CALL_ONREQUEST(request, dataProfilePtrs, num * sizeof(RIL_DataProfileInfo_V1_5 *), pRI, mSlotId);

    for (size_t i = 0; i < num; i++) {
        freeRilDataProfile(dataProfiles[i]);
    }

#ifdef MEMSET_FREED
    memset(dataProfiles, 0, num * sizeof(RIL_DataProfileInfo_V1_5));
    memset(dataProfilePtrs, 0, num * sizeof(RIL_DataProfileInfo_V1_5 *));
#endif
    free(dataProfiles);
    free(dataProfilePtrs);

    return Void();
}

Return<void> RadioImpl::setRadioPower_1_5(int32_t serial, bool powerOn, bool forEmergencyCall,
        bool preferredForEmergencyCall) {
#if VDBG
    RLOGD("%s: %d", __FUNCTION__, serial);
#endif
    int request = ENCODE_REQUEST(RIL_REQUEST_RADIO_POWER, HAL_VERSION_CODE(1,5));
    dispatchInts(serial, mSlotId, request , 3, BOOL_TO_INT(powerOn), BOOL_TO_INT(forEmergencyCall), BOOL_TO_INT(preferredForEmergencyCall));
    return Void();
}

Return<void> RadioImpl::setIndicationFilter_1_5(int32_t serial, hidl_bitfield<V1_5::IndicationFilter> indicationFilter) {
#if VDBG
    RLOGD("%s: %d", __FUNCTION__, serial);
#endif
    int request = ENCODE_REQUEST(RIL_REQUEST_SET_UNSOLICITED_RESPONSE_FILTER, HAL_VERSION_CODE(1,5));
    dispatchInts(serial, mSlotId, request, 1, indicationFilter);
    return Void();
}

Return<void> RadioImpl::getBarringInfo(int32_t serial) {
#if VDBG
    RLOGD("%s: %d", __FUNCTION__, serial);
#endif
    dispatchVoid(serial, mSlotId, RIL_REQUEST_GET_BARRING_INFO);
    return Void();
}

Return<void> RadioImpl::getVoiceRegistrationState_1_5(int32_t serial) {
#if VDBG
    RLOGD("%s: %d", __FUNCTION__, serial);
#endif
    dispatchVoid(serial, mSlotId, RIL_REQUEST_VOICE_REGISTRATION_STATE);
    return Void();
}

Return<void> RadioImpl::getDataRegistrationState_1_5(int32_t serial) {
#if VDBG
    RLOGD("%s: %d", __FUNCTION__, serial);
#endif
    dispatchVoid(serial, mSlotId, RIL_REQUEST_DATA_REGISTRATION_STATE);
    return Void();
}

Return<void> RadioImpl::setNetworkSelectionModeManual_1_5(int32_t serial, const hidl_string& operatorNumeric,
        V1_5::RadioAccessNetworks ran) {
#if VDBG
    RLOGD("%s: %d", __FUNCTION__, serial);
#endif
    int request = ENCODE_REQUEST(RIL_REQUEST_SET_NETWORK_SELECTION_MANUAL, HAL_VERSION_CODE(1,5));
    dispatchStrings(serial, mSlotId, request, true, 2,
            operatorNumeric.c_str(), std::to_string((int)ran).c_str());
    return Void();
}

Return<void> RadioImpl::sendCdmaSmsExpectMore(int32_t serial, const CdmaSmsMessage& sms) {
#if VDBG
    RLOGD("%s: %d", __FUNCTION__, serial);
#endif
    RequestInfo *pRI = android::addRequestToList(serial, mSlotId, RIL_REQUEST_CDMA_SEND_SMS_EXPECT_MORE);
    if (pRI == NULL) {
        return Void();
    }

    RIL_CDMA_SMS_Message rcsm = {};
    constructCdmaSms(rcsm, sms);

    CALL_ONREQUEST(pRI->pCI->requestNumber, &rcsm, sizeof(rcsm), pRI, mSlotId);
    return Void();
}

Return<void> RadioImpl::supplySimDepersonalization(int32_t serial, V1_5::PersoSubstate persoType, const hidl_string& controlKey) {
#if VDBG
    RLOGD("%s: %d", __FUNCTION__, serial);
#endif
    dispatchStrings(serial, mSlotId, RIL_REQUEST_SUPPLY_SIM_DEPERSONALIZATION, true, 2,
            std::to_string((int)persoType).c_str(), controlKey.c_str() );
    return Void();
}

Return<void> RadioImpl::setLinkCapacityReportingCriteria_1_5(int32_t serial, int32_t hysteresisMs, int32_t hysteresisDlKbps,
        int32_t hysteresisUlKbps, const hidl_vec<int32_t>& thresholdsDownlinkKbps,
        const hidl_vec<int32_t>& thresholdsUplinkKbps, V1_5::AccessNetwork accessNetwork) {
#if VDBG
    RLOGD("%s: %d", __FUNCTION__, serial);
#endif
    int request = ENCODE_REQUEST(RIL_REQUEST_SET_LINK_CAPACITY_REPORTING_CRITERIA, HAL_VERSION_CODE(1,5));
    RequestInfo *pRI = android::addRequestToList(serial, mSlotId, request);
    if (pRI == NULL) {
        return Void();
    }

    RIL_LinkCapacityReportingCriteria_V1_5 lc = {};
    lc.hysteresisMs = hysteresisMs;
    lc.hysteresisDlKbps = hysteresisDlKbps;
    lc.hysteresisUlKbps = hysteresisUlKbps;
    lc.numOfThresholdsDownlinkKbps = thresholdsDownlinkKbps.size();
    lc.thresholdsDownlinkKbps = new uint32_t[lc.numOfThresholdsDownlinkKbps];
    for (size_t i = 0; i < lc.numOfThresholdsDownlinkKbps; i++) {
        lc.thresholdsDownlinkKbps[i] = thresholdsDownlinkKbps[i];
    }

    lc.numOfThresholdsUplinkKbps = thresholdsUplinkKbps.size();
    lc.thresholdsUplinkKbps = new uint32_t[lc.numOfThresholdsUplinkKbps];
    for (size_t i = 0; i < lc.numOfThresholdsUplinkKbps; i++) {
        lc.thresholdsUplinkKbps[i] = thresholdsUplinkKbps[i];
    }
    lc.accessNetwork = (RIL_AccessNetwork_V1_5) accessNetwork;

    CALL_ONREQUEST(request, &lc, sizeof(lc), pRI, mSlotId);
    delete[] lc.thresholdsDownlinkKbps;
    delete[] lc.thresholdsUplinkKbps;

    return Void();
}

}  // legacy
}  // impl
}  // radio

/***************************************************************************************************
 * RESPONSE FUNCTIONS
 * Functions above are used for requests going from framework to vendor code. The ones below are
 * responses for those requests coming back from the vendor code.
 **************************************************************************************************/
void populateResponseInfo(RadioResponseInfo& responseInfo, int serial, int responseType,
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
    responseInfo.error = (RadioError) e;
}

int responseIntOrEmpty(RadioResponseInfo& responseInfo, int serial, int responseType, RIL_Errno e,
               void *response, size_t responseLen) {
    populateResponseInfo(responseInfo, serial, responseType, e);
    int ret = -1;

    if (response == NULL && responseLen == 0) {
        // Earlier RILs did not send a response for some cases although the interface
        // expected an integer as response. Do not return error if response is empty. Instead
        // Return -1 in those cases to maintain backward compatibility.
    } else if (response == NULL || responseLen != sizeof(int)) {
        RLOGE("responseIntOrEmpty: Invalid response");
        if (e == RIL_E_SUCCESS) responseInfo.error = RadioError::INVALID_RESPONSE;
    } else {
        int *p_int = (int *) response;
        ret = p_int[0];
    }
    return ret;
}

int responseInt(RadioResponseInfo& responseInfo, int serial, int responseType, RIL_Errno e,
               void *response, size_t responseLen) {
    populateResponseInfo(responseInfo, serial, responseType, e);
    int ret = -1;

    if (response == NULL || responseLen != sizeof(int)) {
        RLOGE("responseInt: Invalid response");
        if (e == RIL_E_SUCCESS) responseInfo.error = RadioError::INVALID_RESPONSE;
    } else {
        int *p_int = (int *) response;
        ret = p_int[0];
    }
    return ret;
}

IccIoResult responseIccIo(RadioResponseInfo& responseInfo, int serial, int responseType,
                           RIL_Errno e, void *response, size_t responseLen) {
    populateResponseInfo(responseInfo, serial, responseType, e);
    IccIoResult result = {};

    if (response == NULL || responseLen != sizeof(RIL_SIM_IO_Response)) {
        RLOGE("Invalid response: NULL");
        if (e == RIL_E_SUCCESS) responseInfo.error = RadioError::INVALID_RESPONSE;
        result.simResponse = hidl_string();
    } else {
        RIL_SIM_IO_Response *resp = (RIL_SIM_IO_Response *) response;
        result.sw1 = resp->sw1;
        result.sw2 = resp->sw2;
        result.simResponse = Radio2HalUtils::convertCharPtrToHidlString(resp->simResponse);
    }
    return result;
}

LceStatusInfo responseLceStatusInfo(RadioResponseInfo& responseInfo, int serial, int responseType,
                                    RIL_Errno e, void *response, size_t responseLen) {
    populateResponseInfo(responseInfo, serial, responseType, e);
    LceStatusInfo result = {};

    if (response == NULL || responseLen != sizeof(RIL_LceStatusInfo)) {
        RLOGE("Invalid response: NULL");
        if (e == RIL_E_SUCCESS) responseInfo.error = RadioError::INVALID_RESPONSE;
    } else {
        RIL_LceStatusInfo *resp = (RIL_LceStatusInfo *) response;
        result.lceStatus = (LceStatus) resp->lce_status;
        result.actualIntervalMs = (uint8_t) resp->actual_interval_ms;
    }
    return result;
}

SendSmsResult makeSendSmsResult(RadioResponseInfo& responseInfo, int serial, int responseType,
                                RIL_Errno e, void *response, size_t responseLen) {
    populateResponseInfo(responseInfo, serial, responseType, e);
    SendSmsResult result = {};

    if (response == NULL || responseLen != sizeof(RIL_SMS_Response)) {
        RLOGE("Invalid response: NULL");
        if (e == RIL_E_SUCCESS) responseInfo.error = RadioError::INVALID_RESPONSE;
        result.ackPDU = hidl_string();
    } else {
        RIL_SMS_Response *resp = (RIL_SMS_Response *) response;
        result.messageRef = resp->messageRef;
        result.ackPDU = Radio2HalUtils::convertCharPtrToHidlString(resp->ackPDU);
        result.errorCode = resp->errorCode;
    }
    return result;
}

void radio::acknowledgeRequest(int slotId, int serial) {
    if (radioService[slotId]->mRadioResponse != NULL) {
        Return<void> retStatus = radioService[slotId]->mRadioResponse->acknowledgeRequest(serial);
        radioService[slotId]->checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }
}

///////////////////////////////////////////////////////////////////////////
// radio 1.0
///////////////////////////////////////////////////////////////////////////
int radio::getIccCardStatusResponse(int slotId,
                                   int responseType, int serial, RIL_Errno e,
                                   void *response, size_t responseLen) {
    if (radioService[slotId] != NULL && radioService[slotId]->mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        CardStatus cardStatus = {CardState::ABSENT, PinState::UNKNOWN, -1, -1, -1, {}};
        RIL_CardStatus_v6 *p_cur = ((RIL_CardStatus_v6 *) response);
        if (response == NULL || responseLen != sizeof(RIL_CardStatus_v6)
                || p_cur->gsm_umts_subscription_app_index >= p_cur->num_applications
                || p_cur->cdma_subscription_app_index >= p_cur->num_applications
                || p_cur->ims_subscription_app_index >= p_cur->num_applications) {
            RLOGE("%s: Invalid response", __FUNCTION__);
            if (e == RIL_E_SUCCESS) responseInfo.error = RadioError::INVALID_RESPONSE;
        } else {
            cardStatus.cardState = (CardState) p_cur->card_state;
            cardStatus.universalPinState = (PinState) p_cur->universal_pin_state;
            cardStatus.gsmUmtsSubscriptionAppIndex = p_cur->gsm_umts_subscription_app_index;
            cardStatus.cdmaSubscriptionAppIndex = p_cur->cdma_subscription_app_index;
            cardStatus.imsSubscriptionAppIndex = p_cur->ims_subscription_app_index;

            RIL_AppStatus *rilAppStatus = p_cur->applications;
            cardStatus.applications.resize(p_cur->num_applications);
            AppStatus *appStatus = cardStatus.applications.data();
#if VDBG
            RLOGD("%s: num_applications %d", __FUNCTION__, p_cur->num_applications);
#endif
            for (int i = 0; i < p_cur->num_applications; i++) {
                appStatus[i].appType = (AppType) rilAppStatus[i].app_type;
                appStatus[i].appState = (AppState) rilAppStatus[i].app_state;
                appStatus[i].persoSubstate = (PersoSubstate) rilAppStatus[i].perso_substate;
                appStatus[i].aidPtr = Radio2HalUtils::convertCharPtrToHidlString(rilAppStatus[i].aid_ptr);
                appStatus[i].appLabelPtr = Radio2HalUtils::convertCharPtrToHidlString(
                        rilAppStatus[i].app_label_ptr);
                appStatus[i].pin1Replaced = rilAppStatus[i].pin1_replaced;
                appStatus[i].pin1 = (PinState) rilAppStatus[i].pin1;
                appStatus[i].pin2 = (PinState) rilAppStatus[i].pin2;
            }
        }

        Return<void> retStatus = radioService[slotId]->mRadioResponse->
                getIccCardStatusResponse(responseInfo, cardStatus);
        radioService[slotId]->checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int radio::supplyIccPinForAppResponse(int slotId,
                                     int responseType, int serial, RIL_Errno e,
                                     void *response, size_t responseLen) {
    HIDL_HAL_SIM_RESPONSE(supplyIccPinForAppResponse)
}

int radio::supplyIccPukForAppResponse(int slotId,
                                     int responseType, int serial, RIL_Errno e,
                                     void *response, size_t responseLen) {
    HIDL_HAL_SIM_RESPONSE(supplyIccPukForAppResponse)
}

int radio::supplyIccPin2ForAppResponse(int slotId,
                                      int responseType, int serial, RIL_Errno e,
                                      void *response, size_t responseLen) {
    HIDL_HAL_SIM_RESPONSE(supplyIccPin2ForAppResponse)
}

int radio::supplyIccPuk2ForAppResponse(int slotId,
                                      int responseType, int serial, RIL_Errno e,
                                      void *response, size_t responseLen) {
    HIDL_HAL_SIM_RESPONSE(supplyIccPuk2ForAppResponse)
}

int radio::changeIccPinForAppResponse(int slotId,
                                     int responseType, int serial, RIL_Errno e,
                                     void *response, size_t responseLen) {
    HIDL_HAL_SIM_RESPONSE(changeIccPinForAppResponse)
}

int radio::changeIccPin2ForAppResponse(int slotId,
                                      int responseType, int serial, RIL_Errno e,
                                      void *response, size_t responseLen) {
    HIDL_HAL_SIM_RESPONSE(changeIccPin2ForAppResponse)
}

int radio::supplyNetworkDepersonalizationResponse(int slotId,
                                                 int responseType, int serial, RIL_Errno e,
                                                 void *response, size_t responselen) {
    HIDL_HAL_NETWORK_RESPONSE(supplyNetworkDepersonalizationResponse)
}

static void fillCalls(hidl_vec<Call> &calls, void *response, size_t responseLen) {
    int num = (response == nullptr) ? 0 : (responseLen / sizeof(RIL_Call *));
    calls.resize(num);

    for (int i = 0; i < num; i++) {
        RIL_Call *p_cur = ((RIL_Call **)response)[i];
        /* each call info */
        calls[i].state = (CallState)p_cur->state;
        calls[i].index = p_cur->index;
        calls[i].toa = p_cur->toa;
        calls[i].isMpty = p_cur->isMpty;
        calls[i].isMT = p_cur->isMT;
        calls[i].als = p_cur->als;
        calls[i].isVoice = p_cur->isVoice;
        calls[i].isVoicePrivacy = p_cur->isVoicePrivacy;
        calls[i].number = Radio2HalUtils::convertCharPtrToHidlString(p_cur->number);
        calls[i].numberPresentation = (CallPresentation)p_cur->numberPresentation;
        calls[i].name = Radio2HalUtils::convertCharPtrToHidlString(p_cur->name);
        calls[i].namePresentation = (CallPresentation)p_cur->namePresentation;
        if (p_cur->uusInfo != NULL && p_cur->uusInfo->uusData != NULL) {
            RIL_UUS_Info *uusInfo = p_cur->uusInfo;
            calls[i].uusInfo.resize(1);
            calls[i].uusInfo[0].uusType = (UusType)uusInfo->uusType;
            calls[i].uusInfo[0].uusDcs = (UusDcs)uusInfo->uusDcs;
            // convert uusInfo->uusData to a null-terminated string
            char *nullTermStr = strndup(uusInfo->uusData, uusInfo->uusLength);
            calls[i].uusInfo[0].uusData = nullTermStr;
            free(nullTermStr);
        }
    }
}

int radio::getCurrentCallsResponse(int slotId,
                                  int responseType, int serial, RIL_Errno e,
                                  void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    if (radioService[slotId] != NULL && radioService[slotId]->mRadioResponseV1_2 != NULL) {
        return radio::getCurrentCallsResponse_1_2(slotId, responseType, serial, e, response, responseLen);
    }

    if (radioService[slotId] != NULL && radioService[slotId]->mRadioResponseV1_2 != NULL &&
        responseLen == sizeof(RIL_Call_V1_2)) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);

        hidl_vec<Call> calls;
        if ((response == NULL && responseLen != 0)
                || (responseLen % sizeof(RIL_Call_V1_2 *)) != 0) {
            RLOGE("%s: Invalid response", __FUNCTION__);
            if (e == RIL_E_SUCCESS) responseInfo.error = RadioError::INVALID_RESPONSE;
        } else {
            fillCalls(calls, response, responseLen);
        }

        Return<void> retStatus = radioService[slotId]->mRadioResponse->
                getCurrentCallsResponse(responseInfo, calls);
        radioService[slotId]->checkReturnStatus(retStatus);
    } else if (radioService[slotId]->mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);

        hidl_vec<Call> calls;
        if ((response == NULL && responseLen != 0)
                || (responseLen % sizeof(RIL_Call *)) != 0) {
            RLOGE("%s: Invalid response", __FUNCTION__);
            if (e == RIL_E_SUCCESS) responseInfo.error = RadioError::INVALID_RESPONSE;
        } else {
            fillCalls(calls, response, responseLen);
        }

        Return<void> retStatus = radioService[slotId]->mRadioResponse->
                getCurrentCallsResponse(responseInfo, calls);
        radioService[slotId]->checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int radio::dialResponse(int slotId,
                       int responseType, int serial, RIL_Errno e, void *response,
                       size_t responseLen) {
    HIDL_HAL_VOICE_RESPONSE(dialResponse)
}

int radio::getIMSIForAppResponse(int slotId,
                                int responseType, int serial, RIL_Errno e, void *response,
                                size_t responseLen) {
    HIDL_HAL_SIM_RESPONSE(getIMSIForAppResponse)
}

int radio::hangupConnectionResponse(int slotId,
                                   int responseType, int serial, RIL_Errno e,
                                   void *response, size_t responseLen) {
    HIDL_HAL_VOICE_RESPONSE(hangupConnectionResponse)
}

int radio::hangupWaitingOrBackgroundResponse(int slotId,
                                            int responseType, int serial, RIL_Errno e,
                                            void *response, size_t responseLen) {
    HIDL_HAL_VOICE_RESPONSE(hangupWaitingOrBackgroundResponse)
}

int radio::hangupForegroundResumeBackgroundResponse(int slotId, int responseType, int serial,
                                                    RIL_Errno e, void *response,
                                                    size_t responseLen) {
    HIDL_HAL_VOICE_RESPONSE(hangupForegroundResumeBackgroundResponse)
}

int radio::switchWaitingOrHoldingAndActiveResponse(int slotId, int responseType, int serial,
                                                   RIL_Errno e, void *response,
                                                   size_t responseLen) {
    HIDL_HAL_VOICE_RESPONSE(switchWaitingOrHoldingAndActiveResponse)
}

int radio::conferenceResponse(int slotId, int responseType,
                             int serial, RIL_Errno e, void *response, size_t responseLen) {
    HIDL_HAL_VOICE_RESPONSE(conferenceResponse)
}

int radio::rejectCallResponse(int slotId, int responseType,
                             int serial, RIL_Errno e, void *response, size_t responseLen) {
    HIDL_HAL_VOICE_RESPONSE(rejectCallResponse)
}

int radio::getLastCallFailCauseResponse(int slotId,
                                       int responseType, int serial, RIL_Errno e, void *response,
                                       size_t responseLen) {
    HIDL_HAL_VOICE_RESPONSE(getLastCallFailCauseResponse)
}

int radio::getSignalStrengthResponse(int slotId,
                                     int responseType, int serial, RIL_Errno e,
                                     void *response, size_t responselen) {
    HIDL_HAL_NETWORK_RESPONSE(getSignalStrengthResponse)
}

int radio::getVoiceRegistrationStateResponse(int slotId,
                                            int responseType, int serial, RIL_Errno e,
                                            void *response, size_t responselen) {
    HIDL_HAL_NETWORK_RESPONSE(getVoiceRegistrationStateResponse)
}

int radio::getDataRegistrationStateResponse(int slotId,
                                           int responseType, int serial, RIL_Errno e,
                                           void *response, size_t responselen) {
    HIDL_HAL_NETWORK_RESPONSE(getDataRegistrationStateResponse)
}

int radio::getOperatorResponse(int slotId,
                              int responseType, int serial, RIL_Errno e, void *response,
                              size_t responselen) {
    HIDL_HAL_NETWORK_RESPONSE(getOperatorResponse)
}

int radio::setRadioPowerResponse(int slotId,
                                int responseType, int serial, RIL_Errno e, void *response,
                                size_t responselen) {
    HIDL_HAL_MODEM_RESPONSE(setRadioPowerResponse)
}

int radio::sendDtmfResponse(int slotId,
                           int responseType, int serial, RIL_Errno e, void *response,
                           size_t responseLen) {
    HIDL_HAL_VOICE_RESPONSE(sendDtmfResponse)
}

int radio::sendSmsResponse(int slotId,
                          int responseType, int serial, RIL_Errno e, void *response,
                          size_t responseLen) {
    HIDL_HAL_MESSAGING_RESPONSE(sendSmsResponse)
}

int radio::sendSMSExpectMoreResponse(int slotId,
                                    int responseType, int serial, RIL_Errno e, void *response,
                                    size_t responseLen) {
    HIDL_HAL_MESSAGING_RESPONSE(sendSmsExpectMoreResponse)
}

int radio::setupDataCallResponse(int slotId,
                                 int responseType, int serial, RIL_Errno e, void *response,
                                 size_t responseLen) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif

    if (radioService[slotId] != NULL && radioService[slotId]->mRadioResponseV1_4 != NULL &&
        response != NULL && responseLen == sizeof(RIL_Data_Call_Response_V1_4)) {
        return setupDataCallResponse_1_4(slotId, responseType, serial, e, response, responseLen);
    }

    if (radioService[slotId] != NULL && radioService[slotId]->mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);

        SetupDataCallResult result = {};
        if (response == NULL || (responseLen % sizeof(RIL_Data_Call_Response_v11)) != 0) {
            if (response != NULL) {
                RLOGE("%s: Invalid response", __FUNCTION__);
                if (e == RIL_E_SUCCESS) responseInfo.error = RadioError::INVALID_RESPONSE;
            }
            result.status = DataCallFailCause::ERROR_UNSPECIFIED;
            result.type = hidl_string();
            result.ifname = hidl_string();
            result.addresses = hidl_string();
            result.dnses = hidl_string();
            result.gateways = hidl_string();
            result.pcscf = hidl_string();
            RLOGE("%s: Invalid response return", __FUNCTION__);
        } else {
            RLOGI("%s: v11 to 1_0 response return", __FUNCTION__);
            Radio2HalUtils::convertRilDataCallToHal((RIL_Data_Call_Response_v11 *) response, result);
        }

        Return<void> retStatus = radioService[slotId]->mRadioResponse->setupDataCallResponse(
                responseInfo, result);
        radioService[slotId]->checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int radio::iccIOForAppResponse(int slotId,
                      int responseType, int serial, RIL_Errno e, void *response,
                      size_t responseLen) {
    HIDL_HAL_SIM_RESPONSE(iccIoForAppResponse)
}

int radio::sendUssdResponse(int slotId,
                           int responseType, int serial, RIL_Errno e, void *response,
                           size_t responseLen) {
    HIDL_HAL_VOICE_RESPONSE(sendUssdResponse)
}

int radio::cancelPendingUssdResponse(int slotId,
                                    int responseType, int serial, RIL_Errno e, void *response,
                                    size_t responseLen) {
    HIDL_HAL_VOICE_RESPONSE(cancelPendingUssdResponse)
}

int radio::getClirResponse(int slotId,
                              int responseType, int serial, RIL_Errno e, void *response,
                              size_t responseLen) {
    HIDL_HAL_VOICE_RESPONSE(getClirResponse)
}

int radio::setClirResponse(int slotId,
                          int responseType, int serial, RIL_Errno e, void *response,
                          size_t responseLen) {
    HIDL_HAL_VOICE_RESPONSE(setClirResponse)
}

int radio::getCallForwardStatusResponse(int slotId,
                                       int responseType, int serial, RIL_Errno e,
                                       void *response, size_t responseLen) {
    HIDL_HAL_VOICE_RESPONSE(getCallForwardStatusResponse)
}

int radio::setCallForwardResponse(int slotId,
                                 int responseType, int serial, RIL_Errno e, void *response,
                                 size_t responseLen) {
    HIDL_HAL_VOICE_RESPONSE(setCallForwardResponse)
}

int radio::getCallWaitingResponse(int slotId,
                                 int responseType, int serial, RIL_Errno e, void *response,
                                 size_t responseLen) {
    HIDL_HAL_VOICE_RESPONSE(getCallWaitingResponse)
}

int radio::setCallWaitingResponse(int slotId,
                                 int responseType, int serial, RIL_Errno e, void *response,
                                 size_t responseLen) {
    HIDL_HAL_VOICE_RESPONSE(setCallWaitingResponse)
}

int radio::acknowledgeLastIncomingGsmSmsResponse(int slotId,
                                                int responseType, int serial, RIL_Errno e,
                                                void *response, size_t responseLen) {
    HIDL_HAL_MESSAGING_RESPONSE(acknowledgeLastIncomingGsmSmsResponse)
}

int radio::acceptCallResponse(int slotId,
                             int responseType, int serial, RIL_Errno e,
                             void *response, size_t responseLen) {
    HIDL_HAL_VOICE_RESPONSE(acceptCallResponse)
}

int radio::deactivateDataCallResponse(int slotId,
                                                int responseType, int serial, RIL_Errno e,
                                                void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif

    if (radioService[slotId] != NULL && radioService[slotId]->mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus = radioService[slotId]->mRadioResponse->deactivateDataCallResponse(
                responseInfo);
        radioService[slotId]->checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL",
                __FUNCTION__, slotId);
    }

    return 0;
}

int radio::getFacilityLockForAppResponse(int slotId,
                                        int responseType, int serial, RIL_Errno e,
                                        void *response, size_t responseLen) {
    HIDL_HAL_SIM_RESPONSE(getFacilityLockForAppResponse)
}

int radio::setFacilityLockForAppResponse(int slotId,
                                      int responseType, int serial, RIL_Errno e,
                                      void *response, size_t responseLen) {
    HIDL_HAL_SIM_RESPONSE(setFacilityLockForAppResponse)
}

int radio::setBarringPasswordResponse(int slotId,
                             int responseType, int serial, RIL_Errno e,
                             void *response, size_t responselen) {
    HIDL_HAL_NETWORK_RESPONSE(setBarringPasswordResponse)
}

int radio::getNetworkSelectionModeResponse(int slotId,
                                          int responseType, int serial, RIL_Errno e, void *response,
                                          size_t responselen) {
    HIDL_HAL_NETWORK_RESPONSE(getNetworkSelectionModeResponse)
}

int radio::setNetworkSelectionModeAutomaticResponse(int slotId, int responseType, int serial,
                                                    RIL_Errno e, void *response,
                                                    size_t responselen) {
    HIDL_HAL_NETWORK_RESPONSE(setNetworkSelectionModeAutomaticResponse)
}

int radio::setNetworkSelectionModeManualResponse(int slotId,
                             int responseType, int serial, RIL_Errno e,
                             void *response, size_t responselen) {
    HIDL_HAL_NETWORK_RESPONSE(setNetworkSelectionModeManualResponse)
}

int radio::getAvailableNetworksResponse(int slotId,
                              int responseType, int serial, RIL_Errno e, void *response,
                              size_t responselen) {
    HIDL_HAL_NETWORK_RESPONSE(getAvailableNetworksResponse)
}

int radio::startDtmfResponse(int slotId,
                            int responseType, int serial, RIL_Errno e,
                            void *response, size_t responseLen) {
    HIDL_HAL_VOICE_RESPONSE(startDtmfResponse)
}

int radio::stopDtmfResponse(int slotId,
                           int responseType, int serial, RIL_Errno e,
                           void *response, size_t responseLen) {
    HIDL_HAL_VOICE_RESPONSE(stopDtmfResponse)
}

int radio::getBasebandVersionResponse(int slotId,
                                     int responseType, int serial, RIL_Errno e,
                                     void *response, size_t responselen) {
    HIDL_HAL_MODEM_RESPONSE(getBasebandVersionResponse)
}

int radio::separateConnectionResponse(int slotId,
                                     int responseType, int serial, RIL_Errno e,
                                     void *response, size_t responseLen) {
    HIDL_HAL_VOICE_RESPONSE(separateConnectionResponse)
}

int radio::setMuteResponse(int slotId,
                          int responseType, int serial, RIL_Errno e,
                          void *response, size_t responseLen) {
    HIDL_HAL_VOICE_RESPONSE(setMuteResponse)
}

int radio::getMuteResponse(int slotId,
                          int responseType, int serial, RIL_Errno e, void *response,
                          size_t responseLen) {
    HIDL_HAL_VOICE_RESPONSE(getMuteResponse)
}

int radio::getClipResponse(int slotId,
                          int responseType, int serial, RIL_Errno e,
                          void *response, size_t responseLen) {
    HIDL_HAL_VOICE_RESPONSE(getClipResponse)
}

int radio::getDataCallListResponse(int slotId,
                                   int responseType, int serial, RIL_Errno e,
                                   void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    if (radioService[slotId] != NULL && radioService[slotId]->mRadioResponseV1_4 != NULL
        && response != NULL && responseLen > 0
        && responseLen % sizeof(RIL_Data_Call_Response_V1_4) == 0) {
        return radio::getDataCallListResponse_1_4(slotId, responseType, serial, e, response, responseLen);
    }

    if (radioService[slotId] != NULL && radioService[slotId]->mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);

        hidl_vec<SetupDataCallResult> ret;
        if ((response == NULL && responseLen != 0)
                || responseLen % sizeof(RIL_Data_Call_Response_v11) != 0) {
            RLOGE("%s: invalid response", __FUNCTION__);
            if (e == RIL_E_SUCCESS) responseInfo.error = RadioError::INVALID_RESPONSE;
        } else {
            Radio2HalUtils::convertRilDataCallListToHal(response, responseLen, ret);
        }

        Return<void> retStatus = radioService[slotId]->mRadioResponse->getDataCallListResponse(
                responseInfo, ret);
        radioService[slotId]->checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int radio::setSuppServiceNotificationsResponse(int slotId,
                                              int responseType, int serial, RIL_Errno e,
                                              void *response, size_t responselen) {
    HIDL_HAL_NETWORK_RESPONSE(setSuppServiceNotificationsResponse)
}

int radio::deleteSmsOnSimResponse(int slotId,
                                 int responseType, int serial, RIL_Errno e,
                                 void *response, size_t responseLen) {
    HIDL_HAL_MESSAGING_RESPONSE(deleteSmsOnSimResponse)
}

int radio::setBandModeResponse(int slotId,
                              int responseType, int serial, RIL_Errno e,
                              void *response, size_t responselen) {
    HIDL_HAL_NETWORK_RESPONSE(setBandModeResponse)
}

int radio::writeSmsToSimResponse(int slotId,
                                int responseType, int serial, RIL_Errno e,
                                void *response, size_t responseLen) {
    HIDL_HAL_MESSAGING_RESPONSE(writeSmsToSimResponse)
}

int radio::getAvailableBandModesResponse(int slotId,
                                        int responseType, int serial, RIL_Errno e, void *response,
                                        size_t responselen) {
    HIDL_HAL_NETWORK_RESPONSE(getAvailableBandModesResponse)
}

int radio::sendEnvelopeResponse(int slotId,
                               int responseType, int serial, RIL_Errno e,
                               void *response, size_t responseLen) {
    HIDL_HAL_SIM_RESPONSE(sendEnvelopeResponse)
}

int radio::sendTerminalResponseToSimResponse(int slotId,
                                            int responseType, int serial, RIL_Errno e,
                                            void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif

    if (radioService[slotId] != NULL && radioService[slotId]->mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus
                = radioService[slotId]->mRadioResponse->sendTerminalResponseToSimResponse(
                responseInfo);
        radioService[slotId]->checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL",
                __FUNCTION__, slotId);
    }

    return 0;
}

int radio::handleStkCallSetupRequestFromSimResponse(int slotId,
                                                   int responseType, int serial,
                                                   RIL_Errno e, void *response,
                                                   size_t responseLen) {
    HIDL_HAL_VOICE_RESPONSE(handleStkCallSetupRequestFromSimResponse)
}

int radio::explicitCallTransferResponse(int slotId,
                                       int responseType, int serial, RIL_Errno e,
                                       void *response, size_t responseLen) {
    HIDL_HAL_VOICE_RESPONSE(explicitCallTransferResponse)
}

int radio::setPreferredNetworkTypeResponse(int slotId,
                                 int responseType, int serial, RIL_Errno e,
                                 void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    if (radioService[slotId] != NULL && radioService[slotId]->mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus
                = radioService[slotId]->mRadioResponse->setPreferredNetworkTypeResponse(
                responseInfo);
        radioService[slotId]->checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL",
                __FUNCTION__, slotId);
    }

    return 0;
}


int radio::getPreferredNetworkTypeResponse(int slotId,
                                          int responseType, int serial, RIL_Errno e,
                                          void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    if (radioService[slotId] != NULL && radioService[slotId]->mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        int ret = responseInt(responseInfo, serial, responseType, e, response, responseLen);
        Return<void> retStatus
                = radioService[slotId]->mRadioResponse->getPreferredNetworkTypeResponse(
                responseInfo, (PreferredNetworkType) ret);
        radioService[slotId]->checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL",
                __FUNCTION__, slotId);
    }

    return 0;
}

int radio::getNeighboringCidsResponse(int slotId,
                                     int responseType, int serial, RIL_Errno e,
                                     void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif

    if (radioService[slotId] != NULL && radioService[slotId]->mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        hidl_vec<NeighboringCell> cells;

        if ((response == NULL && responseLen != 0)
                || responseLen % sizeof(RIL_NeighboringCell *) != 0) {
            RLOGE("%s Invalid response: NULL", __FUNCTION__);
            if (e == RIL_E_SUCCESS) responseInfo.error = RadioError::INVALID_RESPONSE;
        } else {
            int num = responseLen / sizeof(RIL_NeighboringCell *);
            cells.resize(num);
            for (int i = 0 ; i < num; i++) {
                RIL_NeighboringCell *resp = ((RIL_NeighboringCell **) response)[i];
                cells[i].cid = Radio2HalUtils::convertCharPtrToHidlString(resp->cid);
                cells[i].rssi = resp->rssi;
            }
        }

        Return<void> retStatus
                = radioService[slotId]->mRadioResponse->getNeighboringCidsResponse(responseInfo,
                cells);
        radioService[slotId]->checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL",
                 __FUNCTION__, slotId);
    }

    return 0;
}

int radio::setLocationUpdatesResponse(int slotId,
                                     int responseType, int serial, RIL_Errno e,
                                     void *response, size_t responselen) {
    HIDL_HAL_NETWORK_RESPONSE(setLocationUpdatesResponse)
}

int radio::setCdmaSubscriptionSourceResponse(int slotId,
                                 int responseType, int serial, RIL_Errno e,
                                 void *response, size_t responseLen) {
    HIDL_HAL_SIM_RESPONSE(setCdmaSubscriptionSourceResponse)
}

int radio::setCdmaRoamingPreferenceResponse(int slotId,
                                 int responseType, int serial, RIL_Errno e,
                                 void *response, size_t responselen) {
    HIDL_HAL_NETWORK_RESPONSE(setCdmaRoamingPreferenceResponse)
}

int radio::getCdmaRoamingPreferenceResponse(int slotId,
                                           int responseType, int serial, RIL_Errno e,
                                           void *response, size_t responselen) {
    HIDL_HAL_NETWORK_RESPONSE(getCdmaRoamingPreferenceResponse)
}

int radio::setTTYModeResponse(int slotId,
                             int responseType, int serial, RIL_Errno e,
                             void *response, size_t responseLen) {
    HIDL_HAL_VOICE_RESPONSE(setTTYModeResponse)
}

int radio::getTTYModeResponse(int slotId,
                             int responseType, int serial, RIL_Errno e,
                             void *response, size_t responseLen) {
    HIDL_HAL_VOICE_RESPONSE(getTTYModeResponse)
}

int radio::setPreferredVoicePrivacyResponse(int slotId,
                                 int responseType, int serial, RIL_Errno e,
                                 void *response, size_t responseLen) {
    HIDL_HAL_VOICE_RESPONSE(setPreferredVoicePrivacyResponse)
}

int radio::getPreferredVoicePrivacyResponse(int slotId,
                                           int responseType, int serial, RIL_Errno e,
                                           void *response, size_t responseLen) {
    HIDL_HAL_VOICE_RESPONSE(getPreferredVoicePrivacyResponse)
}

int radio::sendCDMAFeatureCodeResponse(int slotId,
                                 int responseType, int serial, RIL_Errno e,
                                 void *response, size_t responseLen) {
    HIDL_HAL_VOICE_RESPONSE(sendCDMAFeatureCodeResponse)
}

int radio::sendBurstDtmfResponse(int slotId,
                                 int responseType, int serial, RIL_Errno e,
                                 void *response, size_t responseLen) {
    HIDL_HAL_VOICE_RESPONSE(sendBurstDtmfResponse)
}

int radio::sendCdmaSmsResponse(int slotId,
                              int responseType, int serial, RIL_Errno e, void *response,
                              size_t responseLen) {
    HIDL_HAL_MESSAGING_RESPONSE(sendCdmaSmsResponse)
}

int radio::acknowledgeLastIncomingCdmaSmsResponse(int slotId,
                                                 int responseType, int serial, RIL_Errno e,
                                                 void *response, size_t responseLen) {
    HIDL_HAL_MESSAGING_RESPONSE(acknowledgeLastIncomingCdmaSmsResponse)
}

int radio::getGsmBroadcastConfigResponse(int slotId,
                                        int responseType, int serial, RIL_Errno e,
                                        void *response, size_t responseLen) {
    HIDL_HAL_MESSAGING_RESPONSE(getGsmBroadcastConfigResponse)
}

int radio::setGsmBroadcastConfigResponse(int slotId,
                                        int responseType, int serial, RIL_Errno e,
                                        void *response, size_t responseLen) {
    HIDL_HAL_MESSAGING_RESPONSE(setGsmBroadcastConfigResponse)
}

int radio::setGsmBroadcastActivationResponse(int slotId,
                                            int responseType, int serial, RIL_Errno e,
                                            void *response, size_t responseLen) {
    HIDL_HAL_MESSAGING_RESPONSE(setGsmBroadcastActivationResponse)
}

int radio::getCdmaBroadcastConfigResponse(int slotId,
                                         int responseType, int serial, RIL_Errno e,
                                         void *response, size_t responseLen) {
    HIDL_HAL_MESSAGING_RESPONSE(getCdmaBroadcastConfigResponse)
}

int radio::setCdmaBroadcastConfigResponse(int slotId,
                                         int responseType, int serial, RIL_Errno e,
                                         void *response, size_t responseLen) {
    HIDL_HAL_MESSAGING_RESPONSE(setCdmaBroadcastConfigResponse)
}

int radio::setCdmaBroadcastActivationResponse(int slotId,
                                             int responseType, int serial, RIL_Errno e,
                                             void *response, size_t responseLen) {
    HIDL_HAL_MESSAGING_RESPONSE(setCdmaBroadcastActivationResponse)
}

int radio::getCDMASubscriptionResponse(int slotId,
                                      int responseType, int serial, RIL_Errno e, void *response,
                                      size_t responseLen) {
    HIDL_HAL_SIM_RESPONSE(getCDMASubscriptionResponse)
}

int radio::writeSmsToRuimResponse(int slotId,
                                 int responseType, int serial, RIL_Errno e,
                                 void *response, size_t responseLen) {
    HIDL_HAL_MESSAGING_RESPONSE(writeSmsToRuimResponse)
}

int radio::deleteSmsOnRuimResponse(int slotId,
                                  int responseType, int serial, RIL_Errno e,
                                  void *response, size_t responseLen) {
    HIDL_HAL_MESSAGING_RESPONSE(deleteSmsOnRuimResponse)
}

int radio::getDeviceIdentityResponse(int slotId,
                                    int responseType, int serial, RIL_Errno e, void *response,
                                    size_t responselen) {
    HIDL_HAL_MODEM_RESPONSE(getDeviceIdentityResponse)
}

int radio::exitEmergencyCallbackModeResponse(int slotId,
                                            int responseType, int serial, RIL_Errno e,
                                            void *response, size_t responseLen) {
    HIDL_HAL_VOICE_RESPONSE(exitEmergencyCallbackModeResponse)
}

int radio::getSmscAddressResponse(int slotId,
                                  int responseType, int serial, RIL_Errno e,
                                  void *response, size_t responseLen) {
    HIDL_HAL_MESSAGING_RESPONSE(getSmscAddressResponse)
}

int radio::setSmscAddressResponse(int slotId,
                                             int responseType, int serial, RIL_Errno e,
                                             void *response, size_t responseLen) {
    HIDL_HAL_MESSAGING_RESPONSE(setSmscAddressResponse)
}

int radio::reportSmsMemoryStatusResponse(int slotId,
                                        int responseType, int serial, RIL_Errno e,
                                        void *response, size_t responseLen) {
    HIDL_HAL_MESSAGING_RESPONSE(reportSmsMemoryStatusResponse)
}

int radio::reportStkServiceIsRunningResponse(int slotId,
                                             int responseType, int serial, RIL_Errno e,
                                             void *response, size_t responseLen) {
    HIDL_HAL_SIM_RESPONSE(reportStkServiceIsRunningResponse)
}

int radio::getCdmaSubscriptionSourceResponse(int slotId,
                                            int responseType, int serial, RIL_Errno e,
                                            void *response, size_t responseLen) {
    HIDL_HAL_SIM_RESPONSE(getCdmaSubscriptionSourceResponse)
}

int radio::requestIsimAuthenticationResponse(int slotId,
                                            int responseType, int serial, RIL_Errno e,
                                            void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif

    if (radioService[slotId] != NULL && radioService[slotId]->mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus
                = radioService[slotId]->mRadioResponse->requestIsimAuthenticationResponse(
                responseInfo,
                Radio2HalUtils::convertCharPtrToHidlString((char *) response));
        radioService[slotId]->checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL",
                __FUNCTION__, slotId);
    }

    return 0;
}

int radio::acknowledgeIncomingGsmSmsWithPduResponse(int slotId,
                                                   int responseType,
                                                   int serial, RIL_Errno e, void *response,
                                                   size_t responseLen) {
    HIDL_HAL_MESSAGING_RESPONSE(acknowledgeIncomingGsmSmsWithPduResponse)
}

int radio::sendEnvelopeWithStatusResponse(int slotId,
                                         int responseType, int serial, RIL_Errno e, void *response,
                                         size_t responseLen) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif

    if (radioService[slotId] != NULL && radioService[slotId]->mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        IccIoResult result = responseIccIo(responseInfo, serial, responseType, e,
                response, responseLen);

        Return<void> retStatus
                = radioService[slotId]->mRadioResponse->sendEnvelopeWithStatusResponse(responseInfo,
                result);
        radioService[slotId]->checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL",
                __FUNCTION__, slotId);
    }

    return 0;
}

int radio::getVoiceRadioTechnologyResponse(int slotId,
                                          int responseType, int serial, RIL_Errno e,
                                          void *response, size_t responselen) {
    HIDL_HAL_NETWORK_RESPONSE(getVoiceRadioTechnologyResponse)
}

int radio::getCellInfoListResponse(int slotId,
                                   int responseType,
                                   int serial, RIL_Errno e, void *response,
                                   size_t responselen) {
    HIDL_HAL_NETWORK_RESPONSE(getCellInfoListResponse)
}

int radio::setCellInfoListRateResponse(int slotId,
                                       int responseType,
                                       int serial, RIL_Errno e, void *response,
                                       size_t responselen) {
    HIDL_HAL_NETWORK_RESPONSE(setCellInfoListRateResponse)
}

int radio::setInitialAttachApnResponse(int slotId,
                                       int responseType, int serial, RIL_Errno e,
                                       void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif

    if (radioService[slotId] != NULL && radioService[slotId]->mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus
                = radioService[slotId]->mRadioResponse->setInitialAttachApnResponse(responseInfo);
        radioService[slotId]->checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL",
                __FUNCTION__, slotId);
    }

    return 0;
}

int radio::getImsRegistrationStateResponse(int slotId,
                                           int responseType, int serial, RIL_Errno e,
                                           void *response, size_t responselen) {
    HIDL_HAL_NETWORK_RESPONSE(getImsRegistrationStateResponse)
}

int radio::sendImsSmsResponse(int slotId,
                              int responseType, int serial, RIL_Errno e, void *response,
                              size_t responseLen) {
    HIDL_HAL_MESSAGING_RESPONSE(sendImsSmsResponse)
}

int radio::iccTransmitApduBasicChannelResponse(int slotId,
                                               int responseType, int serial, RIL_Errno e,
                                               void *response, size_t responseLen) {
    HIDL_HAL_SIM_RESPONSE(iccTransmitApduBasicChannelResponse)
}

int radio::iccOpenLogicalChannelResponse(int slotId,
                                         int responseType, int serial, RIL_Errno e, void *response,
                                         size_t responseLen) {
    HIDL_HAL_SIM_RESPONSE(iccOpenLogicalChannelResponse)
}

int radio::iccCloseLogicalChannelResponse(int slotId,
                                          int responseType, int serial, RIL_Errno e,
                                          void *response, size_t responseLen) {
    HIDL_HAL_SIM_RESPONSE(iccCloseLogicalChannelResponse)
}

int radio::iccTransmitApduLogicalChannelResponse(int slotId,
                                                 int responseType, int serial, RIL_Errno e,
                                                 void *response, size_t responseLen) {
    HIDL_HAL_SIM_RESPONSE(iccTransmitApduLogicalChannelResponse)
}

int radio::nvReadItemResponse(int slotId,
                              int responseType, int serial, RIL_Errno e,
                              void *response, size_t responselen) {
    HIDL_HAL_MODEM_RESPONSE(nvReadItemResponse)
}

int radio::nvWriteItemResponse(int slotId,
                               int responseType, int serial, RIL_Errno e,
                               void *response, size_t responselen) {
    HIDL_HAL_MODEM_RESPONSE(nvWriteItemResponse)
}

int radio::nvWriteCdmaPrlResponse(int slotId,
                                  int responseType, int serial, RIL_Errno e,
                                  void *response, size_t responselen) {
    HIDL_HAL_MODEM_RESPONSE(nvWriteCdmaPrlResponse)
}

int radio::nvResetConfigResponse(int slotId,
                                 int responseType, int serial, RIL_Errno e,
                                 void *response, size_t responselen) {
    HIDL_HAL_MODEM_RESPONSE(nvResetConfigResponse)
}

int radio::setUiccSubscriptionResponse(int slotId,
                                       int responseType, int serial, RIL_Errno e,
                                       void *response, size_t responseLen) {
    HIDL_HAL_SIM_RESPONSE(setUiccSubscriptionResponse)
}

int radio::setDataAllowedResponse(int slotId,
                                  int responseType, int serial, RIL_Errno e,
                                  void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif

    if (radioService[slotId] != NULL && radioService[slotId]->mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus
                = radioService[slotId]->mRadioResponse->setDataAllowedResponse(responseInfo);
        radioService[slotId]->checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int radio::getHardwareConfigResponse(int slotId,
                                     int responseType, int serial, RIL_Errno e,
                                     void *response, size_t responselen) {
    HIDL_HAL_MODEM_RESPONSE(getHardwareConfigResponse)
}

int radio::requestIccSimAuthenticationResponse(int slotId,
                                               int responseType, int serial, RIL_Errno e,
                                               void *response, size_t responseLen) {
    HIDL_HAL_SIM_RESPONSE(requestIccSimAuthenticationResponse)
}

int radio::setDataProfileResponse(int slotId,
                                  int responseType, int serial, RIL_Errno e,
                                  void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif

    if (radioService[slotId] != NULL && radioService[slotId]->mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus
                = radioService[slotId]->mRadioResponse->setDataProfileResponse(responseInfo);
        radioService[slotId]->checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int radio::requestShutdownResponse(int slotId,
                                  int responseType, int serial, RIL_Errno e,
                                  void *response, size_t responselen) {
    HIDL_HAL_MODEM_RESPONSE(requestShutdownResponse)
}



int radio::getRadioCapabilityResponse(int slotId,
                                     int responseType, int serial, RIL_Errno e,
                                     void *response, size_t responselen) {
    HIDL_HAL_MODEM_RESPONSE(getRadioCapabilityResponse)
}

int radio::setRadioCapabilityResponse(int slotId,
                                     int responseType, int serial, RIL_Errno e,
                                     void *response, size_t responselen) {
    HIDL_HAL_MODEM_RESPONSE(setRadioCapabilityResponse)
}

int radio::startLceServiceResponse(int slotId,
                                   int responseType, int serial, RIL_Errno e,
                                   void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif

    if (radioService[slotId] != NULL && radioService[slotId]->mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        LceStatusInfo result = responseLceStatusInfo(responseInfo, serial, responseType, e,
                response, responseLen);

        Return<void> retStatus
                = radioService[slotId]->mRadioResponse->startLceServiceResponse(responseInfo,
                result);
        radioService[slotId]->checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int radio::stopLceServiceResponse(int slotId,
                                  int responseType, int serial, RIL_Errno e,
                                  void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif

    if (radioService[slotId] != NULL && radioService[slotId]->mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        LceStatusInfo result = responseLceStatusInfo(responseInfo, serial, responseType, e,
                response, responseLen);

        Return<void> retStatus
                = radioService[slotId]->mRadioResponse->stopLceServiceResponse(responseInfo,
                result);
        radioService[slotId]->checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int radio::pullLceDataResponse(int slotId,
                               int responseType, int serial, RIL_Errno e,
                               void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif

    if (radioService[slotId] != NULL && radioService[slotId]->mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);

        LceDataInfo result = {};
        if (response == NULL || responseLen != sizeof(RIL_LceDataInfo)) {
            RLOGE("%s: Invalid response", __FUNCTION__);
            if (e == RIL_E_SUCCESS) responseInfo.error = RadioError::INVALID_RESPONSE;
        } else {
            Radio2HalUtils::convertRilLceDataInfoToHal(response, responseLen, result);
        }

        Return<void> retStatus = radioService[slotId]->mRadioResponse->pullLceDataResponse(
                responseInfo, result);
        radioService[slotId]->checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int radio::getModemActivityInfoResponse(int slotId,
                                        int responseType, int serial, RIL_Errno e,
                                        void *response, size_t responselen) {
    HIDL_HAL_MODEM_RESPONSE(getModemActivityInfoResponse)
}

int radio::setAllowedCarriersResponse(int slotId,
                                      int responseType, int serial, RIL_Errno e,
                                      void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    if (radioService[slotId] != NULL && radioService[slotId]->mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        int ret = responseInt(responseInfo, serial, responseType, e, response, responseLen);
        Return<void> retStatus
                = radioService[slotId]->mRadioResponse->setAllowedCarriersResponse(responseInfo,
                ret);
        radioService[slotId]->checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL",
                __FUNCTION__, slotId);
    }

    return 0;
}

int radio::getAllowedCarriersResponse(int slotId,
                                      int responseType, int serial, RIL_Errno e,
                                      void *response, size_t responseLen) {
    HIDL_HAL_SIM_RESPONSE(getAllowedCarriersResponse);
}

int radio::sendDeviceStateResponse(int slotId,
                              int responseType, int serial, RIL_Errno e,
                              void *response, size_t responselen) {
    HIDL_HAL_MODEM_RESPONSE(sendDeviceStateResponse)
}

int radio::setCarrierInfoForImsiEncryptionResponse(int slotId,
                               int responseType, int serial, RIL_Errno e,
                               void *response, size_t responseLen) {
    HIDL_HAL_SIM_RESPONSE(setCarrierInfoForImsiEncryptionResponse)
}

int radio::setIndicationFilterResponse(int slotId,
                              int responseType, int serial, RIL_Errno e,
                              void *response, size_t responselen) {
    HIDL_HAL_NETWORK_RESPONSE(setIndicationFilterResponse)
}

int radio::setSimCardPowerResponse(int slotId,
                                   int responseType, int serial, RIL_Errno e,
                                   void *response, size_t responseLen) {
    HIDL_HAL_SIM_RESPONSE(setSimCardPowerResponse)
}

int radio::startNetworkScanResponse(int slotId, int responseType, int serial, RIL_Errno e,
                                    void *response, size_t responselen) {
    HIDL_HAL_NETWORK_RESPONSE(startNetworkScanResponse)
}

int radio::stopNetworkScanResponse(int slotId, int responseType, int serial, RIL_Errno e,
                                   void *response, size_t responselen) {
    HIDL_HAL_NETWORK_RESPONSE(stopNetworkScanResponse)
}

int radio::startKeepaliveResponse(int slotId, int responseType, int serial, RIL_Errno e,
                                    void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s: %d", __FUNCTION__, serial);
#endif
    RadioResponseInfo responseInfo = {};
    populateResponseInfo(responseInfo, serial, responseType, e);

    // If we don't have a radio service, there's nothing we can do
    if (radioService[slotId] == NULL || radioService[slotId]->mRadioResponseV1_1 == NULL) {
        RLOGE("%s: radioService[%d]->mRadioResponseV1_1 == NULL", __FUNCTION__, slotId);
        return 0;
    }

    V1_1::KeepaliveStatus ks = {};
    if (response == NULL || responseLen != sizeof(V1_1::KeepaliveStatus)) {
        RLOGE("%s: invalid response - %d", __FUNCTION__, static_cast<int>(e));
        if (e == RIL_E_SUCCESS) responseInfo.error = RadioError::INVALID_RESPONSE;
    } else {
        Radio2HalUtils::convertRilKeepaliveStatusToHal(static_cast<RIL_KeepaliveStatus*>(response), ks);
    }

    Return<void> retStatus =
            radioService[slotId]->mRadioResponseV1_1->startKeepaliveResponse(responseInfo, ks);
    radioService[slotId]->checkReturnStatus(retStatus);
    return 0;
}

int radio::stopKeepaliveResponse(int slotId, int responseType, int serial, RIL_Errno e,
                                    void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s: %d", __FUNCTION__, serial);
#endif
    RadioResponseInfo responseInfo = {};
    populateResponseInfo(responseInfo, serial, responseType, e);

    // If we don't have a radio service, there's nothing we can do
    if (radioService[slotId] == NULL || radioService[slotId]->mRadioResponseV1_1 == NULL) {
        RLOGE("%s: radioService[%d]->mRadioResponseV1_1 == NULL", __FUNCTION__, slotId);
        return 0;
    }

    Return<void> retStatus =
            radioService[slotId]->mRadioResponseV1_1->stopKeepaliveResponse(responseInfo);
    radioService[slotId]->checkReturnStatus(retStatus);
    return 0;
}

int radio::setSignalStrengthReportingCriteriaResponse(int slotId, int responseType, int serial, RIL_Errno e,
                                    void *response, size_t responselen) {
    HIDL_HAL_NETWORK_RESPONSE(setSignalStrengthReportingCriteriaResponse)
}

int radio::setLinkCapacityReportingCriteriaResponse(int slotId, int responseType, int serial, RIL_Errno e,
                                    void *response, size_t responselen) {
    HIDL_HAL_NETWORK_RESPONSE(setLinkCapacityReportingCriteriaResponse)
}

int radio::sendRequestRawResponse(int slotId,
                                  int responseType, int serial, RIL_Errno e,
                                  void *response, size_t responseLen) {
#if VDBG
   RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    return 0;
}

int radio::sendRequestStringsResponse(int slotId,
                                      int responseType, int serial, RIL_Errno e,
                                      void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    return 0;
}

/***************************************************************************************************
 * INDICATION FUNCTIONS
 * The below function handle unsolicited messages coming from the Radio
 * (messages for which there is no pending request)
 **************************************************************************************************/

RadioIndicationType convertIntToRadioIndicationType(int indicationType) {
    return indicationType == RESPONSE_UNSOLICITED ? (RadioIndicationType::UNSOLICITED) :
            (RadioIndicationType::UNSOLICITED_ACK_EXP);
}

int radio::radioStateChangedInd(int slotId,
                                 int indicationType, int token, RIL_Errno e, void *response,
                                 size_t responselen) {
    HIDL_HAL_MODEM_INDICATION(radioStateChangedInd)
}

int radio::callStateChangedInd(int slotId,
                               int indicationType, int token, RIL_Errno e, void *response,
                               size_t responseLen) {
    HIDL_HAL_VOICE_INDICATION(callStateChangedInd)
}

int radio::networkStateChangedInd(int slotId,
                                  int indicationType, int token, RIL_Errno e, void *response,
                                  size_t responselen) {
    HIDL_HAL_NETWORK_INDICATION(networkStateChangedInd)
}

int radio::newSmsInd(int slotId, int indicationType,
                     int token, RIL_Errno e, void *response, size_t responseLen) {
    HIDL_HAL_MESSAGING_INDICATION(newSmsInd)
}

int radio::newSmsStatusReportInd(int slotId,
                                 int indicationType, int token, RIL_Errno e, void *response,
                                 size_t responseLen) {
    HIDL_HAL_MESSAGING_INDICATION(newSmsStatusReportInd)
}

int radio::newSmsOnSimInd(int slotId, int indicationType,
                          int token, RIL_Errno e, void *response, size_t responseLen) {
    HIDL_HAL_MESSAGING_INDICATION(newSmsOnSimInd)
}

int radio::onUssdInd(int slotId, int indicationType,
                     int token, RIL_Errno e, void *response, size_t responseLen) {
    HIDL_HAL_VOICE_INDICATION(onUssdInd)
}

int radio::nitzTimeReceivedInd(int slotId,
                               int indicationType, int token, RIL_Errno e, void *response,
                               size_t responselen) {
    HIDL_HAL_NETWORK_INDICATION(nitzTimeReceivedInd)
}

int radio::currentSignalStrengthInd(int slotId,
                                    int indicationType, int token, RIL_Errno e,
                                    void *response, size_t responselen) {
    HIDL_HAL_NETWORK_INDICATION(currentSignalStrengthInd)
}

int radio::dataCallListChangedInd(int slotId,
                                  int indicationType, int token, RIL_Errno e, void *response,
                                  size_t responseLen) {
    if (radioService[slotId] != NULL && radioService[slotId]->mRadioIndication != NULL) {
        // null response is valid, it means empty datacall, no active datacall
        if ((response == NULL && responseLen != 0)
                || responseLen % sizeof(RIL_Data_Call_Response_v11) != 0) {
            RLOGE("%s: invalid response", __FUNCTION__);
            return 0;
        }
        hidl_vec<SetupDataCallResult> dcList;
        Radio2HalUtils::convertRilDataCallListToHal(response, responseLen, dcList);
#if VDBG
        RLOGD("%s", __FUNCTION__);
#endif
        Return<void> retStatus = radioService[slotId]->mRadioIndication->dataCallListChanged(
                convertIntToRadioIndicationType(indicationType), dcList);
        radioService[slotId]->checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioIndication == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int radio::suppSvcNotifyInd(int slotId, int indicationType,
                            int token, RIL_Errno e, void *response, size_t responselen) {
    HIDL_HAL_NETWORK_INDICATION(suppSvcNotifyInd)
}

int radio::stkSessionEndInd(int slotId, int indicationType,
                            int token, RIL_Errno e, void *response, size_t responseLen) {
    HIDL_HAL_SIM_INDICATION(stkSessionEnd)
}

int radio::stkProactiveCommandInd(int slotId,
                                  int indicationType, int token, RIL_Errno e, void *response,
                                  size_t responseLen) {
    HIDL_HAL_SIM_INDICATION(stkProactiveCommand)
}

int radio::stkEventNotifyInd(int slotId, int indicationType,
                             int token, RIL_Errno e, void *response, size_t responseLen) {
    HIDL_HAL_SIM_INDICATION(stkEventNotify)
}

int radio::stkCallSetupInd(int slotId, int indicationType,
                           int token, RIL_Errno e, void *response, size_t responseLen) {
    HIDL_HAL_VOICE_INDICATION(stkCallSetupInd)
}

int radio::simSmsStorageFullInd(int slotId,
                                int indicationType, int token, RIL_Errno e, void *response,
                                size_t responseLen) {
    HIDL_HAL_MESSAGING_INDICATION(simSmsStorageFullInd)
}

int radio::simRefreshInd(int slotId, int indicationType,
                         int token, RIL_Errno e, void *response, size_t responseLen) {
    HIDL_HAL_SIM_INDICATION(simRefresh)
}

int radio::callRingInd(int slotId, int indicationType,
                       int token, RIL_Errno e, void *response, size_t responseLen) {
    HIDL_HAL_VOICE_INDICATION(callRingInd)
}

int radio::simStatusChangedInd(int slotId,
                               int indicationType, int token, RIL_Errno e, void *response,
                               size_t responseLen) {
    HIDL_HAL_SIM_INDICATION(simStatusChanged)
}

int radio::cdmaNewSmsInd(int slotId, int indicationType,
                         int token, RIL_Errno e, void *response, size_t responseLen) {
    HIDL_HAL_MESSAGING_INDICATION(cdmaNewSmsInd)
}

int radio::newBroadcastSmsInd(int slotId,
                              int indicationType, int token, RIL_Errno e, void *response,
                              size_t responseLen) {
    HIDL_HAL_MESSAGING_INDICATION(newBroadcastSmsInd)
}

int radio::cdmaRuimSmsStorageFullInd(int slotId,
                                     int indicationType, int token, RIL_Errno e, void *response,
                                     size_t responseLen) {
    HIDL_HAL_MESSAGING_INDICATION(cdmaRuimSmsStorageFullInd)
}

int radio::restrictedStateChangedInd(int slotId,
                                     int indicationType, int token, RIL_Errno e, void *response,
                                     size_t responselen) {
    HIDL_HAL_NETWORK_INDICATION(restrictedStateChangedInd)
}

int radio::enterEmergencyCallbackModeInd(int slotId,
                                         int indicationType, int token, RIL_Errno e, void *response,
                                         size_t responseLen) {
    HIDL_HAL_VOICE_INDICATION(enterEmergencyCallbackModeInd)
}

int radio::cdmaCallWaitingInd(int slotId,
                              int indicationType, int token, RIL_Errno e, void *response,
                              size_t responseLen) {
    HIDL_HAL_VOICE_INDICATION(cdmaCallWaitingInd)
}

int radio::cdmaOtaProvisionStatusInd(int slotId,
                                     int indicationType, int token, RIL_Errno e, void *response,
                                     size_t responseLen) {
    HIDL_HAL_VOICE_INDICATION(cdmaOtaProvisionStatusInd)
}

int radio::cdmaInfoRecInd(int slotId,
                          int indicationType, int token, RIL_Errno e, void *response,
                          size_t responseLen) {
    HIDL_HAL_VOICE_INDICATION(cdmaInfoRecInd)
}

int radio::indicateRingbackToneInd(int slotId,
                                   int indicationType, int token, RIL_Errno e, void *response,
                                   size_t responseLen) {
    HIDL_HAL_VOICE_INDICATION(indicateRingbackToneInd)
}

int radio::resendIncallMuteInd(int slotId,
                               int indicationType, int token, RIL_Errno e, void *response,
                               size_t responseLen) {
    HIDL_HAL_VOICE_INDICATION(resendIncallMuteInd)
}

int radio::cdmaSubscriptionSourceChangedInd(int slotId,
                                            int indicationType, int token, RIL_Errno e,
                                            void *response, size_t responseLen) {
    HIDL_HAL_SIM_INDICATION(cdmaSubscriptionSourceChanged)
}

int radio::cdmaPrlChangedInd(int slotId,
                             int indicationType, int token, RIL_Errno e, void *response,
                             size_t responselen) {
    HIDL_HAL_NETWORK_INDICATION(cdmaPrlChangedInd)
}

int radio::exitEmergencyCallbackModeInd(int slotId,
                                        int indicationType, int token, RIL_Errno e, void *response,
                                        size_t responseLen) {
    HIDL_HAL_VOICE_INDICATION(exitEmergencyCallbackModeInd)
}

int radio::rilConnectedInd(int slotId,
                           int indicationType, int token, RIL_Errno e, void *response,
                           size_t responselen) {
    HIDL_HAL_MODEM_INDICATION(rilConnectedInd)
}

int radio::voiceRadioTechChangedInd(int slotId,
                                    int indicationType, int token, RIL_Errno e, void *response,
                                    size_t responselen) {
    HIDL_HAL_NETWORK_INDICATION(voiceRadioTechChangedInd)
}

int radio::cellInfoListInd(int slotId,
                           int indicationType, int token, RIL_Errno e, void *response,
                           size_t responselen) {
    HIDL_HAL_NETWORK_INDICATION(cellInfoListInd)
}

int radio::imsNetworkStateChangedInd(int slotId,
                                     int indicationType, int token, RIL_Errno e, void *response,
                                     size_t responselen) {
    HIDL_HAL_NETWORK_INDICATION(imsNetworkStateChangedInd)
}

int radio::subscriptionStatusChangedInd(int slotId,
                                        int indicationType, int token, RIL_Errno e, void *response,
                                        size_t responseLen) {
    HIDL_HAL_SIM_INDICATION(subscriptionStatusChanged)
}

int radio::srvccStateNotifyInd(int slotId,
                               int indicationType, int token, RIL_Errno e, void *response,
                               size_t responseLen) {
    HIDL_HAL_VOICE_INDICATION(srvccStateNotifyInd)
}

int radio::hardwareConfigChangedInd(int slotId,
                                    int indicationType, int token, RIL_Errno e, void *response,
                                    size_t responselen) {
    HIDL_HAL_MODEM_INDICATION(hardwareConfigChangedInd)
}

int radio::radioCapabilityIndicationInd(int slotId,
                                        int indicationType, int token, RIL_Errno e, void *response,
                                        size_t responselen) {
    HIDL_HAL_MODEM_INDICATION(radioCapabilityIndicationInd)
}

int radio::onSupplementaryServiceIndicationInd(int slotId,
                                               int indicationType, int token, RIL_Errno e,
                                               void *response, size_t responseLen) {
    HIDL_HAL_VOICE_INDICATION(onSupplementaryServiceIndicationInd)
}

int radio::stkCallControlAlphaNotifyInd(int slotId,
                                        int indicationType, int token, RIL_Errno e, void *response,
                                        size_t responseLen) {
    HIDL_HAL_VOICE_INDICATION(stkCallControlAlphaNotifyInd)
}

int radio::lceDataInd(int slotId,
                      int indicationType, int token, RIL_Errno e, void *response,
                      size_t responselen) {
    HIDL_HAL_NETWORK_INDICATION(lceDataInd)
}

int radio::pcoDataInd(int slotId,
                      int indicationType, int token, RIL_Errno e, void *response,
                      size_t responseLen) {
    if (radioService[slotId] != NULL && radioService[slotId]->mRadioIndication != NULL) {
        if (response == NULL || responseLen != sizeof(RIL_PCO_Data)) {
            RLOGE("%s: invalid response", __FUNCTION__);
            return 0;
        }

        PcoDataInfo pco = {};
        RIL_PCO_Data *rilPcoData = (RIL_PCO_Data *)response;
        pco.cid = rilPcoData->cid;
        pco.bearerProto = Radio2HalUtils::convertCharPtrToHidlString(rilPcoData->bearer_proto);
        pco.pcoId = rilPcoData->pco_id;
        pco.contents.setToExternal((uint8_t *) rilPcoData->contents, rilPcoData->contents_length);

#if VDBG
        RLOGD("%s", __FUNCTION__);
#endif
        Return<void> retStatus = radioService[slotId]->mRadioIndication->pcoData(
                convertIntToRadioIndicationType(indicationType), pco);
        radioService[slotId]->checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioIndication == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int radio::modemResetInd(int slotId,
                         int indicationType, int token, RIL_Errno e, void *response,
                         size_t responselen) {
    HIDL_HAL_MODEM_INDICATION(modemResetInd)
}

int radio::networkScanResultInd(int slotId,
                                int indicationType, int token, RIL_Errno e, void *response,
                                size_t responselen) {
    HIDL_HAL_NETWORK_INDICATION(networkScanResultInd)
}

int radio::carrierInfoForImsiEncryption(int slotId,
                                  int indicationType, int token, RIL_Errno e, void *response,
                                  size_t responseLen) {
    HIDL_HAL_SIM_INDICATION(carrierInfoForImsiEncryption)
}

int radio::keepaliveStatusInd(int slotId,
                         int indicationType, int token, RIL_Errno e, void *response,
                         size_t responseLen) {
#if VDBG
    RLOGD("%s: token=%d", __FUNCTION__, token);
#endif
    if (radioService[slotId] == NULL || radioService[slotId]->mRadioIndication == NULL) {
        RLOGE("%s: radioService[%d]->mRadioIndication == NULL", __FUNCTION__, slotId);
        return 0;
    }

    auto ret = V1_1::IRadioIndication::castFrom(
        radioService[slotId]->mRadioIndication);
    if (!ret.isOk()) {
        RLOGE("%s: ret.isOk() == false for radioService[%d]", __FUNCTION__, slotId);
        return 0;
    }
    sp<V1_1::IRadioIndication> radioIndicationV1_1 = ret;

    if (response == NULL || responseLen != sizeof(V1_1::KeepaliveStatus)) {
        RLOGE("%s: invalid response", __FUNCTION__);
        return 0;
    }

    V1_1::KeepaliveStatus ks;
    Radio2HalUtils::convertRilKeepaliveStatusToHal(static_cast<RIL_KeepaliveStatus*>(response), ks);

    Return<void> retStatus = radioIndicationV1_1->keepaliveStatus(
            convertIntToRadioIndicationType(indicationType), ks);
    radioService[slotId]->checkReturnStatus(retStatus);
    return 0;
}

///////////////////////////////////////////////////////////////////////////
// radio 1.2
///////////////////////////////////////////////////////////////////////////
int radio::currentPhysicalChannelConfigs(int slotId,
                         int indicationType, int token, RIL_Errno e, void *response,
                         size_t responselen) {
    HIDL_HAL_NETWORK_INDICATION(currentPhysicalChannelConfigs)
}

int radio::getIccCardStatusResponse_1_2(int slotId, int responseType,
                            int serial, RIL_Errno e, void *response, size_t responselen) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    if (radioService[slotId] != NULL && radioService[slotId]->mRadioResponseV1_2 != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);

        V1_2::CardStatus cardStatus = {};
        RIL_CardStatus_V1_2 *p_cur = ((RIL_CardStatus_V1_2 *) response);
        if (!Radio2HalUtils::convertRilCardStatusToHal(p_cur, responselen, cardStatus)) {
            if (e == RIL_E_SUCCESS) responseInfo.error = RadioError::INVALID_RESPONSE;
        }

        Return<void> retStatus = radioService[slotId]->mRadioResponseV1_2->
                getIccCardStatusResponse_1_2(responseInfo, cardStatus);
        radioService[slotId]->checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponseV1_2 == NULL",
                __FUNCTION__, slotId);
        if (responselen >= sizeof(RIL_CardStatus_v6)) {
            return radio::getIccCardStatusResponse(slotId, responseType, serial, e,
                    response, sizeof(RIL_CardStatus_v6));
        }
    }
    return 0;
}

int radio::getCurrentCallsResponse_1_2(int slotId,
                                  int responseType, int serial, RIL_Errno e,
                                  void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif

    if (radioService[slotId] != NULL && radioService[slotId]->mRadioResponseV1_2 != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);

        hidl_vec<V1_2::Call> calls;
        if ((response == NULL && responseLen != 0)
                || (responseLen % sizeof(RIL_Call_V1_2 *)) != 0) {
            RLOGE("%s: Invalid response", __FUNCTION__);
            if (e == RIL_E_SUCCESS) responseInfo.error = RadioError::INVALID_RESPONSE;
        } else {
            int num = responseLen / sizeof(RIL_Call_V1_2 *);
            calls.resize(num);

            for (int i = 0 ; i < num ; i++) {
                RIL_Call_V1_2 *p_cur = ((RIL_Call_V1_2 **) response)[i];
                if (!Radio2HalUtils::convertRilCallToHal(p_cur, sizeof(RIL_Call_V1_2), calls[i])) {
                    if (e == RIL_E_SUCCESS) responseInfo.error = RadioError::INVALID_RESPONSE;
                    break;
                }
            }
        }

        Return<void> retStatus = radioService[slotId]->mRadioResponseV1_2->
                getCurrentCallsResponse_1_2(responseInfo, calls);
        radioService[slotId]->checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponseV1_2 == NULL", __FUNCTION__,slotId);
    }

    return 0;
}

int radio::getVoiceRegistrationStateResponse_1_2(int slotId,
                                     int responseType, int serial, RIL_Errno e, void *response,
                                     size_t responselen) {
    HIDL_HAL_NETWORK_RESPONSE(getVoiceRegistrationStateResponse_1_2)
}

int radio::getDataRegistrationStateResponse_1_2(int slotId,
                                    int responseType, int serial, RIL_Errno e, void *response,
                                    size_t responselen) {
    HIDL_HAL_NETWORK_RESPONSE(getDataRegistrationStateResponse_1_2)
}

int radio::getSignalStrengthResponse_1_2(int slotId, int responseType,
                            int serial, RIL_Errno e, void *response, size_t responselen) {
    HIDL_HAL_NETWORK_RESPONSE(getSignalStrengthResponse_1_2)
}

int radio::getCellInfoListResponse_1_2(int slotId,
                            int responseType, int serial, RIL_Errno e, void *response,
                            size_t responselen) {
    HIDL_HAL_NETWORK_RESPONSE(getCellInfoListResponse_1_2)
}

int radio::currentSignalStrengthInd_1_2(int slotId,
                             int indicationType, int token, RIL_Errno e,
                             void *response, size_t responselen) {
    HIDL_HAL_NETWORK_INDICATION(currentSignalStrengthInd_1_2)
}

int radio::cellInfoListInd_1_2(int slotId,
                    int indicationType, int token, RIL_Errno e, void *response,
                    size_t responselen) {
    HIDL_HAL_NETWORK_INDICATION(cellInfoListInd_1_2)
}

int radio::networkScanResultInd_1_2(int slotId,
                         int indicationType, int token, RIL_Errno e, void *response,
                         size_t responselen) {
    HIDL_HAL_NETWORK_INDICATION(networkScanResultInd_1_2)
}

///////////////////////////////////////////////////////////////////////////
// radio 1.3
///////////////////////////////////////////////////////////////////////////
int radio::setSystemSelectionChannelsResponse(int slotId,
                          int responseType, int serial, RIL_Errno e,
                          void *response, size_t responselen) {
    HIDL_HAL_NETWORK_RESPONSE(setSystemSelectionChannelsResponse)
}

int radio::enableModemResponse(int slotId,
                          int responseType, int serial, RIL_Errno e,
                          void *response, size_t responselen) {
    HIDL_HAL_MODEM_RESPONSE(enableModemResponse)
}

int radio::getModemStackStatusResponse(int slotId,
                          int responseType, int serial, RIL_Errno e,
                          void *response, size_t responselen) {
    HIDL_HAL_MODEM_RESPONSE(getModemStackStatusResponse)
}

///////////////////////////////////////////////////////////////////////////
// radio 1.4
///////////////////////////////////////////////////////////////////////////
int radio::getIccCardStatusResponse_1_4(int slotId, int responseType,
                            int serial, RIL_Errno e, void *response, size_t responselen) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    if (radioService[slotId] != NULL && radioService[slotId]->mRadioResponseV1_4 != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);

        V1_4::CardStatus cardStatus = {};
        RIL_CardStatus_V1_4 *p_cur = ((RIL_CardStatus_V1_4 *) response);
        if (!Radio2HalUtils::convertRilCardStatusToHal(p_cur, responselen, cardStatus)) {
            if (e == RIL_E_SUCCESS) responseInfo.error = RadioError::INVALID_RESPONSE;
        }

        Return<void> retStatus = radioService[slotId]->mRadioResponseV1_4->
                getIccCardStatusResponse_1_4(responseInfo, cardStatus);
        radioService[slotId]->checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponseV1_4 == NULL",
                __FUNCTION__, slotId);
        if (responselen >= sizeof(RIL_CardStatus_V1_2)) {
            return radio::getIccCardStatusResponse_1_2(slotId, responseType, serial, e,
                    response, sizeof(RIL_CardStatus_V1_2));
        }
    }
    return 0;
}

int radio::getDataRegistrationStateResponse_1_4(int slotId,
                                    int responseType, int serial, RIL_Errno e, void *response,
                                    size_t responselen) {
    HIDL_HAL_NETWORK_RESPONSE(getDataRegistrationStateResponse_1_4)
}

int radio::getSignalStrengthResponse_1_4(int slotId, int responseType,
                            int serial, RIL_Errno e, void *response, size_t responselen) {
    HIDL_HAL_NETWORK_RESPONSE(getSignalStrengthResponse_1_4)
}

int radio::setupDataCallResponse_1_4(int slotId,
                          int responseType, int serial, RIL_Errno e, void *response,
                          size_t responseLen) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif

    if (radioService[slotId] != NULL && radioService[slotId]->mRadioResponseV1_4 != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);

        V1_4::SetupDataCallResult dcResult = {};
        vector<vector<string>> t;
        t.resize(4);
        // User v11 format, For V1_4, only two parameter are removed, and just type change for Cause
        if (response == NULL || responseLen != sizeof(RIL_Data_Call_Response_v11)) {
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
            RLOGD("%s: convertRilDataCallToHal", __FUNCTION__);
            Radio2HalUtils::convertRilDataCallToHal((RIL_Data_Call_Response_v11 *)response, dcResult, t, 0);
        }
        Return<void> retStatus = radioService[slotId]->mRadioResponseV1_4->setupDataCallResponse_1_4(
                responseInfo, dcResult);
        radioService[slotId]->checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponseV1_4 == NULL", __FUNCTION__, slotId);
        return radio::setupDataCallResponse(slotId, responseType, serial, e, response, responseLen);
    }

    return 0;
}

int radio::getDataCallListResponse_1_4(int slotId,
                            int responseType, int serial, RIL_Errno e,
                            void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    if (radioService[slotId] != NULL && radioService[slotId]->mRadioResponseV1_4 != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);

        hidl_vec<V1_4::SetupDataCallResult> ret;
        vector<vector<string>> tmp;
        if ((response == NULL && responseLen != 0)
                || responseLen % sizeof(RIL_Data_Call_Response_V1_4) != 0) {
            RLOGE("%s: invalid response", __FUNCTION__);
            if (e == RIL_E_SUCCESS) responseInfo.error = RadioError::INVALID_RESPONSE;
        } else {
            Radio2HalUtils::convertRilDataCallListToHal(response, responseLen, ret, tmp);
        }
        Return<void> retStatus = radioService[slotId]->mRadioResponseV1_4->getDataCallListResponse_1_4(
                responseInfo, ret);
        radioService[slotId]->checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponseV1_4 == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int radio::getDataCallListResponse_1_5(int slotId,
                            int responseType, int serial, RIL_Errno e,
                            void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    if (radioService[slotId] != NULL && radioService[slotId]->mRadioResponseV1_5 != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);

        hidl_vec<V1_5::SetupDataCallResult> ret;
        vector<vector<string>> tmp;
        if ((response == NULL && responseLen != 0)
                || responseLen % sizeof(RIL_Data_Call_Response_v11) != 0) {
            RLOGE("%s: invalid response", __FUNCTION__);
            if (e == RIL_E_SUCCESS) responseInfo.error = RadioError::INVALID_RESPONSE;
        } else {
            Radio2HalUtils::convertRilDataCallListToHal(response, responseLen, ret, tmp);
        }
        Return<void> retStatus = radioService[slotId]->mRadioResponseV1_5->getDataCallListResponse_1_5(
                responseInfo, ret);
        radioService[slotId]->checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponseV1_5 == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int radio::setPreferredNetworkTypeBitmapResponse(int slotId,
                          int responseType, int serial, RIL_Errno e,
                          void *response, size_t responselen) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    int ret =0;
    if (radioService[slotId] != NULL && radioService[slotId]->mRadioResponseV1_4 != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus = radioService[slotId]->mRadioResponseV1_4->setPreferredNetworkTypeBitmapResponse(responseInfo);
        radioService[slotId]->checkReturnStatus(retStatus);
    } else {
        // response is void, so no need to convert
        ret = radio::setPreferredNetworkTypeResponse(slotId, responseType, serial, e, response, responselen);
        RLOGE("%s: radioService[%d]->mRadioResponseV1_4 == NULL, ret = %d", __FUNCTION__, slotId, ret);
    }

    return ret;
}

int radio::getPreferredNetworkTypeBitmapResponse(int slotId,
                          int responseType, int serial, RIL_Errno e,
                          void *response, size_t responselen) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    int ret =0;
    if (radioService[slotId] != NULL && radioService[slotId]->mRadioResponseV1_4 != NULL) {
        RadioResponseInfo responseInfo = {};
        int ret = responseInt(responseInfo, serial, responseType, e, response, responselen);
        hidl_bitfield<V1_4::RadioAccessFamily> networkTypeBitmap = ret;
        Return<void> retStatus = radioService[slotId]->mRadioResponseV1_4->
            getPreferredNetworkTypeBitmapResponse(responseInfo, networkTypeBitmap);
        radioService[slotId]->checkReturnStatus(retStatus);
    } else {
        if (response != NULL && responselen >= sizeof(int)) {
            int *p_int = (int *) response;
            int netTypeBitmap = p_int[0];
            int netType = RadioConvUtils::getNetworkTypeFromRaf(netTypeBitmap);
            ret = radio::getPreferredNetworkTypeResponse(slotId, responseType, serial, e, &netType, sizeof(int));
        }
        RLOGE("%s: radioService[%d]->mRadioResponseV1_4 == NULL, ret = %d", __FUNCTION__, slotId, ret);
    }

    return ret;
}

int radio::getCellInfoListResponse_1_4(int slotId,
                                   int responseType,
                                   int serial, RIL_Errno e, void *response,
                                   size_t responselen) {
    HIDL_HAL_NETWORK_RESPONSE(getCellInfoListResponse_1_4)
}

int radio::setAllowedCarriersResponse_1_4(int slotId,
                              int responseType, int serial, RIL_Errno e,
                              void *response, size_t responseLen) {
    HIDL_HAL_SIM_RESPONSE(setAllowedCarriersResponse_1_4)
}

int radio::getAllowedCarriersResponse_1_4(int slotId,
                              int responseType, int serial, RIL_Errno e,
                              void *response, size_t responseLen) {
    HIDL_HAL_SIM_RESPONSE(getAllowedCarriersResponse_1_4);
}

int radio::startNetworkScanResponse_1_4(int slotId,
                             int responseType, int serial, RIL_Errno e, void *response,
                             size_t responselen) {
    HIDL_HAL_NETWORK_RESPONSE(startNetworkScanResponse_1_4)
}

int radio::emergencyDialResponse(int slotId,
                          int responseType, int serial, RIL_Errno e,
                          void *response, size_t responseLen) {
    HIDL_HAL_VOICE_RESPONSE(emergencyDialResponse)
}

int radio::currentSignalStrengthInd_1_4(int slotId,
                             int indicationType, int token, RIL_Errno e,
                             void *response, size_t responselen) {
    HIDL_HAL_NETWORK_INDICATION(currentSignalStrengthInd_1_4)
}

int radio::dataCallListChangedInd_1_4(int slotId, int indicationType,
                           int token, RIL_Errno e, void *response, size_t responselen) {
    int ret = 0;
    if (radioService[slotId] != NULL && radioService[slotId]->mRadioIndicationV1_4 != NULL) {
        // null response is valid, it means empty datacall, no active datacall, still use v11
        if ((response == NULL && responselen != 0)
            || responselen % sizeof(RIL_Data_Call_Response_v11) != 0) {
            RLOGE("%s: invalid response", __FUNCTION__);
            return 0;
        }

        hidl_vec<V1_4::SetupDataCallResult> dcList;
        vector<vector<string>> tmp;
        Radio2HalUtils::convertRilDataCallListToHal(response, responselen, dcList, tmp);
#if VDBG
        RLOGD("%s", __FUNCTION__);
#endif
        Return<void> retStatus = radioService[slotId]->mRadioIndicationV1_4->dataCallListChanged_1_4(
                convertIntToRadioIndicationType(indicationType), dcList);
        radioService[slotId]->checkReturnStatus(retStatus);
    } else {
        // In current response is RIL_Data_Call_Response_v11 is used here also.
        // No conversion is needed.
        // When RIL_Data_Call_Response_V1_4 is implemented, convertDataCallList shall be implemented.
        // RadioConvUtils::convertDataCallList(dataCallList, (RIL_Data_Call_Response_V1_4*)response, num);
        ret = radio::dataCallListChangedInd(slotId, indicationType, token, e, response, responselen);
        RLOGE("%s: radioService[%d]->mRadioIndicationV1_4 == NULL, ret = %d", __FUNCTION__, slotId, ret);
    }

    return ret;
}

int radio::dataCallListChangedInd_1_5(int slotId, int indicationType,
                           int token, RIL_Errno e, void *response, size_t responselen) {
    int ret = 0;
    if (radioService[slotId] != NULL && radioService[slotId]->mRadioIndicationV1_5 != NULL) {
        // null response is valid, it means empty datacall, no active datacall, still use v11
        if ((response == NULL && responselen != 0)
            || responselen % sizeof(RIL_Data_Call_Response_v11) != 0) {
            RLOGE("%s: invalid response : %p, len:%zu", __FUNCTION__, response, responselen);
            return 0;
        }

        hidl_vec<V1_5::SetupDataCallResult> dcList;
        vector<vector<string>> tmp;
        Radio2HalUtils::convertRilDataCallListToHal(response, responselen, dcList, tmp);
#if VDBG
        RLOGD("%s", __FUNCTION__);
#endif
        Return<void> retStatus = radioService[slotId]->mRadioIndicationV1_5->dataCallListChanged_1_5(
                convertIntToRadioIndicationType(indicationType), dcList);
        radioService[slotId]->checkReturnStatus(retStatus);
    } else {
        // In current response is RIL_Data_Call_Response_v11 is used here also.
        // No conversion is needed.
        // When RIL_Data_Call_Response_V1_5 is implemented, convertDataCallList shall be implemented.
        // RadioConvUtils::convertDataCallList(dataCallList, (RIL_Data_Call_Response_V1_5*)response, num);
        ret = radio::dataCallListChangedInd(slotId, indicationType, token, e, response, responselen);
        RLOGE("%s: radioService[%d]->mRadioIndicationV1_5 == NULL, ret = %d", __FUNCTION__, slotId, ret);
    }

    return ret;
}

int radio::cellInfoListInd_1_4(int slotId,
                    int indicationType, int token, RIL_Errno e, void *response,
                    size_t responselen) {
    HIDL_HAL_NETWORK_INDICATION(cellInfoListInd_1_4)
}

int radio::networkScanResultInd_1_4(int slotId,
                         int indicationType, int token, RIL_Errno e, void *response,
                         size_t responselen) {
    HIDL_HAL_NETWORK_INDICATION(networkScanResultInd_1_4)
}

int radio::currentPhysicalChannelConfigs_1_4(int slotId,
                       int indicationType, int token, RIL_Errno e, void *response,
                       size_t responselen) {
    HIDL_HAL_NETWORK_INDICATION(currentPhysicalChannelConfigs_1_4)
}

int radio::currentEmergencyNumberListInd(int slotId,
                            int indicationType, int token, RIL_Errno e, void *response,
                            size_t responseLen) {
    HIDL_HAL_VOICE_INDICATION(currentEmergencyNumberListInd)
}

///////////////////////////////////////////////////////////////////////////
// radio 1.5
///////////////////////////////////////////////////////////////////////////
int radio::setSignalStrengthReportingCriteriaResponse_1_5(int slotId,
                            int responseType, int serial, RIL_Errno e, void *response,
                            size_t responselen) {
    HIDL_HAL_NETWORK_RESPONSE(setSignalStrengthReportingCriteriaResponse_1_5)
}

int radio::enableUiccApplicationsResponse(int slotId,
                            int responseType, int serial, RIL_Errno e, void *response,
                            size_t responselen) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    if (radioService[slotId] != NULL && radioService[slotId]->mRadioResponseV1_5 == NULL) {
        RLOGE("%s: radioService[%d]->mRadioResponseV1_5 == NULL", __FUNCTION__, slotId);
        return 0;
    }

    RadioResponseInfo responseInfo = {};
    populateResponseInfo(responseInfo, serial, responseType, e);
    Return<void> retStatus =
            radioService[slotId]->mRadioResponseV1_5->enableUiccApplicationsResponse(responseInfo);
    radioService[slotId]->checkReturnStatus(retStatus);
    return 0;
}

int radio::areUiccApplicationsEnabledResponse(int slotId,
                            int responseType, int serial, RIL_Errno e, void *response,
                            size_t responselen) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    if (radioService[slotId] != NULL && radioService[slotId]->mRadioResponseV1_5 == NULL) {
        RLOGE("%s: radioService[%d]->mRadioResponseV1_5 == NULL", __FUNCTION__, slotId);
        return 0;
    }

    RadioResponseInfo responseInfo = {};
    int result = responseInt(responseInfo, serial, responseType, e, response, responselen);
    bool isEnabled =  result == 1 ? true : false;
    Return<void> retStatus =
            radioService[slotId]->mRadioResponseV1_5->areUiccApplicationsEnabledResponse(responseInfo, isEnabled);
    radioService[slotId]->checkReturnStatus(retStatus);
    return 0;
}

int radio::setSystemSelectionChannelsResponse_1_5(int slotId,
                            int responseType, int serial, RIL_Errno e, void *response,
                            size_t responselen) {
    HIDL_HAL_NETWORK_RESPONSE(setSystemSelectionChannelsResponse_1_5)
}

int radio::startNetworkScanResponse_1_5(int slotId,
                            int responseType, int serial, RIL_Errno e, void *response,
                            size_t responselen) {
    HIDL_HAL_NETWORK_RESPONSE(startNetworkScanResponse_1_5)
}

int radio::setupDataCallResponse_1_5(int slotId,
                            int responseType, int serial, RIL_Errno e, void *response,
                            size_t responselen) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    if (radioService[slotId] != NULL && radioService[slotId]->mRadioResponseV1_5 != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);

        V1_5::SetupDataCallResult dcResult = {};
        vector<vector<string>> t;
        t.resize(4);
        // User v11 format, For V1_4, only two parameter are removed, and just type change for Cause
        if (response == NULL || responselen != sizeof(RIL_Data_Call_Response_v11)) {
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
            Radio2HalUtils::convertRilDataCallToHal((RIL_Data_Call_Response_v11 *)response, dcResult, t, 0);
        }
        Return<void> retStatus = radioService[slotId]->mRadioResponseV1_5->setupDataCallResponse_1_5(
                responseInfo, dcResult);
        radioService[slotId]->checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponseV1_5 == NULL", __FUNCTION__, slotId);
        return radio::setupDataCallResponse_1_4(slotId, responseType, serial, e, response, responselen);
    }

    return 0;
}

int radio::setInitialAttachApnResponse_1_5(int slotId,
                            int responseType, int serial, RIL_Errno e, void *response,
                            size_t responselen) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif

    if (radioService[slotId] != NULL && radioService[slotId]->mRadioResponseV1_5 != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus
                = radioService[slotId]->mRadioResponseV1_5->setInitialAttachApnResponse_1_5(responseInfo);
        radioService[slotId]->checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponseV1_5 == NULL", __FUNCTION__, slotId);
        return radio::setInitialAttachApnResponse(slotId, responseType, serial, e, response, responselen);
    }

    return 0;
}

int radio::setDataProfileResponse_1_5(int slotId,
                            int responseType, int serial, RIL_Errno e, void *response,
                            size_t responselen) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif

    if (radioService[slotId] != NULL && radioService[slotId]->mRadioResponseV1_5 != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus
                = radioService[slotId]->mRadioResponseV1_5->setDataProfileResponse_1_5(responseInfo);
        radioService[slotId]->checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
        return radio::setDataProfileResponse(slotId, responseType, serial, e, response, responselen);
    }

    return 0;
}

int radio::setRadioPowerResponse_1_5(int slotId,
                            int responseType, int serial, RIL_Errno e, void *response,
                            size_t responselen) {
    HIDL_HAL_MODEM_RESPONSE(setRadioPowerResponse_1_5)
}

int radio::setIndicationFilterResponse_1_5(int slotId,
                            int responseType, int serial, RIL_Errno e, void *response,
                            size_t responselen) {
    HIDL_HAL_NETWORK_RESPONSE(setIndicationFilterResponse_1_5)
}

int radio::getBarringInfoResponse(int slotId,
                            int responseType, int serial, RIL_Errno e, void *response,
                            size_t responselen) {
    HIDL_HAL_NETWORK_RESPONSE(getBarringInfoResponse)
}

int radio::getVoiceRegistrationStateResponse_1_5(int slotId,
                            int responseType, int serial, RIL_Errno e, void *response,
                            size_t responselen) {
    HIDL_HAL_NETWORK_RESPONSE(getVoiceRegistrationStateResponse_1_5)
}

int radio::getDataRegistrationStateResponse_1_5(int slotId,
                            int responseType, int serial, RIL_Errno e, void *response,
                            size_t responselen) {
    HIDL_HAL_NETWORK_RESPONSE(getDataRegistrationStateResponse_1_5)
}

int radio::setNetworkSelectionModeManualResponse_1_5(int slotId,
                            int responseType, int serial, RIL_Errno e, void *response,
                            size_t responselen) {
    HIDL_HAL_NETWORK_RESPONSE(setNetworkSelectionModeManualResponse_1_5)
}

int radio::sendCdmaSmsExpectMoreResponse(int slotId,
                            int responseType, int serial, RIL_Errno e, void *response,
                            size_t responseLen) {
    HIDL_HAL_MESSAGING_RESPONSE(sendCdmaSmsExpectMoreResponse)
}

int radio::supplySimDepersonalizationResponse(int slotId,
                            int responseType, int serial, RIL_Errno e, void *response,
                            size_t responselen) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    // If we don't have a radio service, there's nothing we can do
    if (radioService[slotId]->mRadioResponseV1_5 == NULL) {
        RLOGE("%s: radioService[%d]->mRadioResponseV1_5 == NULL", __FUNCTION__, slotId);
        return 0;
    }

    RadioResponseInfo responseInfo = {};
    populateResponseInfo(responseInfo, serial, responseType, e);
    V1_5::PersoSubstate persoType = V1_5::PersoSubstate::UNKNOWN;
    int32_t remainingRetries = -1;
    if (response == NULL || responselen != 2*sizeof(int)) {
        RLOGE("%s: Invalid response", __FUNCTION__);
        if (e == RIL_E_SUCCESS) responseInfo.error = RadioError::INVALID_RESPONSE;
    } else {
        int *pRet = (int *)response;
        persoType = (V1_5::PersoSubstate)pRet[0];
        remainingRetries = pRet[1];
    }

    Return<void> retStatus =
            radioService[slotId]->mRadioResponseV1_5->supplySimDepersonalizationResponse(responseInfo, persoType, remainingRetries);
    radioService[slotId]->checkReturnStatus(retStatus);
    return 0;
}

int radio::setLinkCapacityReportingCriteriaResponse_1_5(int slotId,
                            int responseType, int serial, RIL_Errno e, void *response,
                            size_t responselen) {
    HIDL_HAL_NETWORK_RESPONSE(setLinkCapacityReportingCriteriaResponse_1_5)
}

int radio::getCellInfoListResponse_1_5(int slotId,
                                   int responseType,
                                   int serial, RIL_Errno e, void *response,
                                   size_t responselen) {
    HIDL_HAL_NETWORK_RESPONSE(getCellInfoListResponse_1_5)
}

int radio::getIccCardStatusResponse_1_5(int slotId, int responseType,
                            int serial, RIL_Errno e, void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    if (radioService[slotId]->mRadioResponseV1_5 != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);

        V1_5::CardStatus cardStatus = {};
        RIL_CardStatus_V1_4 *p_cur = ((RIL_CardStatus_V1_4 *) response);
        if (!Radio2HalUtils::convertRilCardStatusToHal(p_cur, responseLen, cardStatus.base)) {
            if (e == RIL_E_SUCCESS) responseInfo.error = RadioError::INVALID_RESPONSE;
        }

        size_t size = cardStatus.base.base.base.applications.size();
        cardStatus.applications.resize(size);
        for (size_t i = 0; i < size; i++) {
            cardStatus.applications[i].base = cardStatus.base.base.base.applications[i];
            cardStatus.applications[i].persoSubstate =
                    (V1_5::PersoSubstate)cardStatus.base.base.base.applications[i].persoSubstate;
        } // end for i ~
        Return<void> retStatus = radioService[slotId]->mRadioResponseV1_5->
                getIccCardStatusResponse_1_5(responseInfo, cardStatus);
        radioService[slotId]->checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponseV1_5 == NULL",
                __FUNCTION__, slotId);
        if (responseLen >= sizeof(RIL_CardStatus_V1_4)) {
            return radio::getIccCardStatusResponse_1_4(slotId, responseType, serial, e,
                    response, sizeof(RIL_CardStatus_V1_4));
        }
    }
    return 0;

}

int radio::uiccApplicationsEnablementChangedInd(int slotId,
                            int indicationType, int token, RIL_Errno e, void *response,
                            size_t responseLen) {
    HIDL_HAL_SIM_INDICATION(uiccApplicationsEnablementChanged)
}

int radio::registrationFailedInd(int slotId,
                            int indicationType, int token, RIL_Errno e, void *response,
                            size_t responselen) {
    HIDL_HAL_NETWORK_INDICATION(registrationFailedInd)
}

int radio::barringInfoChangedInd(int slotId,
                            int indicationType, int token, RIL_Errno e, void *response,
                            size_t responselen) {
    HIDL_HAL_NETWORK_INDICATION(barringInfoChangedInd)
}

int radio::cellInfoListInd_1_5(int slotId,
                            int indicationType, int token, RIL_Errno e, void *response,
                            size_t responselen) {
    HIDL_HAL_NETWORK_INDICATION(cellInfoListInd_1_5)
}

int radio::networkScanResultInd_1_5(int slotId,
                            int indicationType, int token, RIL_Errno e, void *response,
                            size_t responselen) {
    HIDL_HAL_NETWORK_INDICATION(networkScanResultInd_1_5)
}

// should acquire write lock for the corresponding service before calling this
void radio::setNitzTimeReceived(int slotId, long timeReceived) {
    nitzTimeReceived[slotId] = timeReceived;
}

void radio::impl::legacy::registerAsService() {
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

void radio::impl::legacy::initRadios() {
    int simCount = 1;
    #if (SIM_COUNT >= 2)
    simCount = SIM_COUNT;
    #endif

    for (int i = 0; i < simCount; i++) {
        pthread_rwlock_t *radioServiceRwlockPtr = getRadioServiceRwlock(i);
        int ret = pthread_rwlock_wrlock(radioServiceRwlockPtr);
        assert(ret == 0);

        radioService[i] = new RadioImpl;
        radioService[i]->mSlotId = i;

        ret = pthread_rwlock_unlock(radioServiceRwlockPtr);
        assert(ret == 0);
    }
}

sp<::android::hardware::radio::V1_0::IRadio> radio::impl::legacy::getRadio(int slotId) {
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

/* radio namespace : Shared for later version */
void radio::sendEmergencyDial(int32_t serial, const Dial &dialInfo,
                              hidl_bitfield<V1_4::EmergencyServiceCategory> categories,
                              const hidl_vec<hidl_string> &urns, V1_4::EmergencyCallRouting routing,
                              bool hasKnownUserIntentEmergency, bool isTesting, int request, int mSlotId) {
    RequestInfo *pRI = android::addRequestToList(serial, mSlotId, request);
    if (pRI == NULL) {
        return;
    }

    RIL_EmergencyDial emergencyDialInfo = {};
    RIL_Dial &dial = emergencyDialInfo.dialInfo;
    RIL_UUS_Info uusInfo = {};

    if (!copyHidlStringToRil(&dial.address, dialInfo.address, pRI)) {
        return;
    }
    dial.clir = (int)dialInfo.clir;
    if (dialInfo.uusInfo.size() != 0) {
        uusInfo.uusType = (RIL_UUS_Type)dialInfo.uusInfo[0].uusType;
        uusInfo.uusDcs = (RIL_UUS_DCS)dialInfo.uusInfo[0].uusDcs;

        if (dialInfo.uusInfo[0].uusData.size() == 0) {
            uusInfo.uusData = NULL;
            uusInfo.uusLength = 0;
        } else {
            if (!copyHidlStringToRil(&uusInfo.uusData, dialInfo.uusInfo[0].uusData, pRI)) {
                memsetAndFreeStrings(1, dial.address);
                return;
            }
            uusInfo.uusLength = dialInfo.uusInfo[0].uusData.size();
        }

        dial.uusInfo = &uusInfo;
    }
    emergencyDialInfo.categories = categories;
    emergencyDialInfo.len_urns = urns.size();
    int countStrings = urns.size();
    char **pStrings;
    pStrings = (char **)calloc(countStrings, sizeof(char *));
    if (pStrings == NULL) {
        RLOGE("Memory allocation failed for request %s", requestToString(RIL_REQUEST_EMERGENCY_DIAL));
        sendErrorResponse(pRI, RIL_E_NO_MEMORY);
        memsetAndFreeStrings(2, dial.address, uusInfo.uusData);
        return;
    }

    for (int i = 0; i < countStrings; i++) {
        if (!copyHidlStringToRil(&pStrings[i], urns[i], pRI)) {
            for (int j = 0; j < i; j++) {
                memsetAndFreeStrings(1, pStrings[j]);
            }
            free(pStrings);
            memsetAndFreeStrings(2, dial.address, uusInfo.uusData);
            return;
        }
    }
    emergencyDialInfo.len_urns = countStrings;
    emergencyDialInfo.urns = pStrings;
    emergencyDialInfo.routing = (int)routing;
    emergencyDialInfo.hasKnownUserIntentEmergency = hasKnownUserIntentEmergency;
    emergencyDialInfo.isTesting = isTesting;

    CALL_ONREQUEST(pRI->pCI->requestNumber, &emergencyDialInfo, sizeof(emergencyDialInfo), pRI, mSlotId);
    memsetAndFreeStrings(2, dial.address, uusInfo.uusData);
    if (pStrings != NULL) {
        for (int i = 0; i < countStrings; i++) {
            memsetAndFreeStrings(1, pStrings[i]);
        }

#ifdef MEMSET_FREED
        memset(pStrings, 0, countStrings * sizeof(char *));
#endif
        free(pStrings);
    }
}

sp<::android::hardware::radio::V1_5::IRadio> radio::impl::legacy::make(uint32_t slotId) {
    auto halRadio1_5 = sp<RadioImpl>::make();
    assert(halRadio1_5 != nullptr);
    halRadio1_5->mSlotId = slotId;
    return halRadio1_5;
}

