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

#define LOG_TAG "RadioConfig"

// This include 1.1 types and 1.0 Request
#include <android/hardware/radio/config/1.1/IRadioConfig.h>
// This include 1.2 types and 1.0&1.2 response
#include <android/hardware/radio/config/1.2/IRadioConfigResponse.h>
// This includes 1.0 & 1.2 indication
#include <android/hardware/radio/config/1.2/IRadioConfigIndication.h>
// Need to include 1.0 types
#include <android/hardware/radio/config/1.2/types.h>

#include <telephony/ril.h>
#include <hwbinder/IPCThreadState.h>
#include <hwbinder/ProcessState.h>
#include <ril_service_common.h>
#include <radioconfig_service.h>

// HIDL HAL response
#include <hidl/config/hidlhalresponse_config.h>

#define LogD(format, ...)    ALOGD("%s() " format, __FUNCTION__, ##__VA_ARGS__)
#define LogE(format, ...)    ALOGE("%s() " format, __FUNCTION__, ##__VA_ARGS__)
#define LogW(format, ...)    ALOGW("%s() " format, __FUNCTION__, ##__VA_ARGS__)
#define LogI(format, ...)    ALOGI("%s() " format, __FUNCTION__, ##__VA_ARGS__)
#define LogV(format, ...)    ALOGI("%s() " format, __FUNCTION__, ##__VA_ARGS__)

#define ENTER_FUNC()        { ALOGD("%s() [<-- ", __FUNCTION__); }
#define LEAVE_FUNC()        { ALOGD("%s() [--> ", __FUNCTION__); }

using namespace android::hardware::radio::V1_0;
using namespace android::hardware::radio::config;
// Use 1.1 Namespace, this is independent to other version
using namespace android::hardware::radio::config::V1_1;
// Use 1.2 Namespace at default
using namespace android::hardware::radio::config::V1_2;
using namespace android::hardware::radio::config::V2_0;
using android::hardware::radio::config::V1_0::SlotState;
using ::android::hardware::radio::V1_0::RadioResponseInfo;
using ::android::hardware::radio::V1_0::RadioResponseType;
using ::android::hardware::radio::V1_0::RadioError;
using ::android::hardware::hidl_array;
using ::android::hardware::hidl_death_recipient;
using ::android::hardware::hidl_memory;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using android::CommandInfo;
using android::RequestInfo;
using android::sp;

#if VDBG
static bool vdbg = true;
#else
static bool vdbg = false;
#endif

#define HIDL_HAL_RADIO_CONFIG_RESPONSE(theHandler) \
    if (vdbg) RLOGD("radioConfigService::%s: serial %d", __FUNCTION__, serial); \
    if (radioconfigService == nullptr) { \
        printf("HIDL_HAL_RADIO_CONFIG_RESPONSE: radioConfigService is null.\n"); \
        RLOGW("HIDL_HAL_RADIO_CONFIG_RESPONSE: radioConfigService is null."); return 0; } \
    return HidlHalRadioConfigResponse(radioconfigService->mHalRadioConfigResponse). \
                theHandler(slotId, responseType, serial, e, response, responseLen); \

struct RadioConfigImpl;

RIL_RadioFunctions *s_radioconfig_vendorFunctions = NULL;
static CommandInfo *s_radioconfig_commands;

int s_radioconfig_registerCalled = 0;

// use default ril socket: [0]
sp<RadioConfigImpl> radioconfigService;

/*req id, data, datalen, token, socket id*/
#if defined(ANDROID_MULTI_SIM)
#define RADIOCONFIG_CALL_ONREQUEST(a, b, c, d, e) \
            s_radioconfig_vendorFunctions->onRequest((a), (b), (c), (d), ((RIL_SOCKET_ID)(e)))
#define RADIOCONFIG_CALL_ONSTATEREQUEST(a) s_radioconfig_vendorFunctions->onStateRequest((RIL_SOCKET_ID)(a))
#else
#define RADIOCONFIG_CALL_ONREQUEST(a, b, c, d, e) s_radioconfig_vendorFunctions->onRequest((a), (b), (c), (d))
#define RADIOCONFIG_CALL_ONSTATEREQUEST(a) s_radioconfig_vendorFunctions->onStateRequest()
#endif

namespace radioconfig {
const char * requestToString(int request) {
    switch(request) {
    case RIL_REQUEST_GET_SLOT_STATUS :
        return "RIL_REQUEST_GET_SLOT_STATUS";
    case RIL_REQUEST_SET_LOGICAL_TO_PHYSICAL_SLOT_MAPPING:
        return "RIL_REQUEST_SET_LOGICAL_TO_PHYSICAL_SLOT_MAPPING";
    case RIL_REQUEST_SET_PREFERRED_DATA_MODEM:
        return "RIL_REQUEST_SET_PREFERRED_DATA_MODEM";
    case RIL_REQUEST_GET_PHONE_CAPABILITY:
        return "RIL_REQUEST_GET_PHONE_CAPABILITY";
    case RIL_REQUEST_SET_MODEMS_CONFIG:
        return "RIL_REQUEST_SET_MODEMS_CONFIG";
    case RIL_REQUEST_GET_MODEMS_CONFIG:
        return "RIL_REQUEST_GET_MODEMS_CONFIG";
    case RIL_UNSOL_ICC_SLOT_STATUS:
        return "RIL_UNSOL_ICC_SLOT_STATUS";
    default: return "<unknown request>";
    } // end switch ~
}

void sendErrorResponse(RequestInfo *pRI, RIL_Errno err) {
    pRI->pCI->responseFunction((int) pRI->socket_id,
            (int) RadioResponseType::SOLICITED, pRI->token, err, NULL, 0);
}

bool dispatchVoid(int serial, int slotId, int request) {
    RequestInfo *pRI = android::addRequestToList(serial, slotId, request);
    if (pRI == NULL) {
        return false;
    }
    RADIOCONFIG_CALL_ONREQUEST(request, NULL, 0, pRI, slotId);
    return true;
}


bool dispatchInts(int serial, int slotId, int request, int countInts, ...) {
    RequestInfo *pRI = android::addRequestToList(serial, slotId, request);
    if (pRI == NULL) {
        return false;
    }

    int *pInts = (int *)calloc(countInts, sizeof(int));

    if (pInts == NULL) {
        RLOGE("Memory allocation failed for request %s", radioconfig::requestToString(request));
        radioconfig::sendErrorResponse(pRI, RIL_E_NO_MEMORY);
        return false;
    }
    va_list ap;
    va_start(ap, countInts);
    for (int i = 0; i < countInts; i++) {
        pInts[i] = va_arg(ap, int);
    }
    va_end(ap);

    RADIOCONFIG_CALL_ONREQUEST(request, pInts, countInts * sizeof(int), pRI, slotId);

    if (pInts != NULL) {
#ifdef MEMSET_FREED
        memset(pInts, 0, countInts * sizeof(int));
#endif
        free(pInts);
    }
    return true;
}

}

/* functions for Radio Config Service */
void populateResponseInfoForRadioConfig(RadioResponseInfo& responseInfo, int serial,
        int responseType, RIL_Errno e);
RadioIndicationType convertIntToRadioIndicationTypeForRadioConfig(int indicationType);

struct RadioConfigImpl : public android::hardware::radio::config::V1_1::IRadioConfig {
    RIL_SOCKET_ID rilSocketId;

    sp<V1_0::IRadioConfigResponse> mRadioConfigResponse;
    sp<V1_0::IRadioConfigIndication> mRadioConfigIndication;
    // FuncTbl for V1.1
    sp<V1_1::IRadioConfigResponse> mRadioConfigResponseV1_1;
    sp<V1_1::IRadioConfigIndication> mRadioConfigIndicationV1_1;
    // FuncTbl for V1.2
    sp<V1_2::IRadioConfigResponse> mRadioConfigResponseV1_2;
    sp<V1_2::IRadioConfigIndication> mRadioConfigIndicationV1_2;

    HidlHalRadioConfigResponse mHalRadioConfigResponse;

    Return<void> setResponseFunctions(
            const sp<V1_0::IRadioConfigResponse>& radioConfigResponse,
            const sp<V1_0::IRadioConfigIndication>& radioConfigIndication);

    /* 1.0 */
    Return<void> getSimSlotsStatus(int32_t serial);
    Return<void> setSimSlotsMapping(int32_t serial,
            const hidl_vec<uint32_t>& slotMap);

    /* 1.1 */
    Return<void> getPhoneCapability(int32_t serial);
    Return<void> setPreferredDataModem(int32_t serial, uint8_t modemId);
    Return<void> setModemsConfig(int32_t serial, const ModemsConfig &modemsConfig);
    Return<void> getModemsConfig(int32_t serial);

    /* 1.2 */
    // No new Request, Just Response is updated, and Indication is added

    /* 1.3 */
    Return<void> getPhoneCapability_1_3(int32_t serial);

    void checkReturnStatus(Return<void>& ret);
};

void populateResponseInfoForRadioConfig(RadioResponseInfo& responseInfo, int serial, int responseType,
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
    responseInfo.error = (RadioError)e;
}

RadioIndicationType convertIntToRadioIndicationTypeForRadioConfig(int indicationType) {
    return indicationType == RESPONSE_UNSOLICITED ? (RadioIndicationType::UNSOLICITED) :
            (RadioIndicationType::UNSOLICITED_ACK_EXP);
}

void checkReturnStatus(Return<void>& ret, bool isRadioConfigService) {
    ENTER_FUNC();
    if (ret.isOk() == false) {
        RLOGE("RadioConfig checkReturnStatus: unable to call response/indication callback");
        // Remote process hosting the callbacks must be dead. Reset the callback objects;
        // there's no other recovery to be done here. When the client process is back up, it will
        // call setResponseFunctions()

        // Caller should already hold rdlock, release that first
        // note the current counter to avoid overwriting updates made by another thread before
        // write lock is acquired.
        pthread_rwlock_t *radioConfigServiceRwlockPtr = radio::getRadioServiceRwlock(
                radioconfigService->rilSocketId);
        int ret = pthread_rwlock_unlock(radioConfigServiceRwlockPtr);
        assert(ret == 0);

        // acquire wrlock
        ret = pthread_rwlock_wrlock(radioConfigServiceRwlockPtr);
        assert(ret == 0);

        // make sure the counter value has not changed
        if (isRadioConfigService) {
            radioconfigService->mRadioConfigResponse = nullptr;
            radioconfigService->mRadioConfigIndication = nullptr;
            radioconfigService->mRadioConfigResponseV1_1 = nullptr;
            radioconfigService->mRadioConfigIndicationV1_1 = nullptr;
            radioconfigService->mRadioConfigResponseV1_2 = nullptr;
            radioconfigService->mRadioConfigIndicationV1_2 = nullptr;
        } else {
            RLOGE("RadioConfig checkReturnStatus: not resetting responseFunctions as they likely "
                    "got updated on another thread");
        }

        // release wrlock
        ret = pthread_rwlock_unlock(radioConfigServiceRwlockPtr);
        assert(ret == 0);

        // Reacquire rdlock
        ret = pthread_rwlock_rdlock(radioConfigServiceRwlockPtr);
        assert(ret == 0);
    }
    LEAVE_FUNC();
}

void RadioConfigImpl::checkReturnStatus(Return<void>& ret) {
    ::checkReturnStatus(ret, true);
}

// Need 1.0 version implementation in newer IRadioConfig version
Return<void> RadioConfigImpl::setResponseFunctions(
        const sp<V1_0::IRadioConfigResponse>& radioConfigResponse,
        const sp<V1_0::IRadioConfigIndication>& radioConfigIndication) {
    RLOGD("RadioConfigImpl::setResponseFunctions");

    mRadioConfigResponse = radioConfigResponse;
    mRadioConfigIndication = radioConfigIndication;

    // Prepare V1.1 FuncTbl, 1.1 has independent set
    mRadioConfigResponseV1_1 = V1_1::IRadioConfigResponse::castFrom(mRadioConfigResponse).withDefault(nullptr);
    if (mRadioConfigResponseV1_1 == nullptr) {
        RLOGD("setResponseFunctions: V1_1 Response not supported");
    }
    mRadioConfigIndicationV1_1 = V1_1::IRadioConfigIndication::castFrom(mRadioConfigIndication).withDefault(nullptr);
    if (mRadioConfigIndicationV1_1 == nullptr) {
        RLOGD("setResponseFunctions: V1_1 Indication not supported");
    }

    RLOGD("setResponseFunctions: V1_1 supported, Response:%p, Indication:%p",
                mRadioConfigResponseV1_1.get(), mRadioConfigIndicationV1_1.get());

    // Prepare V1.2 FuncTbl
    mRadioConfigResponseV1_2 = V1_2::IRadioConfigResponse::castFrom(mRadioConfigResponse).withDefault(nullptr);
    if (mRadioConfigResponseV1_2 == nullptr) {
        RLOGD("setResponseFunctions: V1_2 Response not supported");
    }
    mRadioConfigIndicationV1_2 = V1_2::IRadioConfigIndication::castFrom(mRadioConfigIndication).withDefault(nullptr);
    if (mRadioConfigIndicationV1_2 == nullptr) {
        RLOGD("setResponseFunctions: V1_2 Indication not supported");
    }

    RLOGD("setResponseFunctions: V1_2 supported, Response:%p, Indication:%p",
                mRadioConfigResponseV1_2.get(), mRadioConfigIndicationV1_2.get());

    // HIDL response/indication
    mHalRadioConfigResponse = HidlHalRadioConfigResponse(mRadioConfigResponse);

    return Void();
}

Return<void> RadioConfigImpl::getSimSlotsStatus(int32_t serial) {
    ENTER_FUNC();
    RequestInfo *pRI = nullptr;
    pRI = android::addRequestToList(serial, rilSocketId, RIL_REQUEST_GET_SLOT_STATUS);
    if (pRI == nullptr) {
        LogE("Request is null");
        LEAVE_FUNC();
        return Void();
    }

    LogD("OnRequest - serial: [%d]", serial);
    RADIOCONFIG_CALL_ONREQUEST(pRI->pCI->requestNumber, NULL, 0, pRI, rilSocketId);
    LEAVE_FUNC();
    return Void();
}

Return<void> RadioConfigImpl::setSimSlotsMapping(int32_t serial,
        const hidl_vec<uint32_t>& slotMap) {
    ENTER_FUNC();
    RequestInfo *pRI = android::addRequestToList(serial, rilSocketId, RIL_REQUEST_SET_LOGICAL_TO_PHYSICAL_SLOT_MAPPING);
    if (pRI == NULL) {
        LogE("Request is null");
        LEAVE_FUNC();
        return Void();
    }
    const uint32_t *uData = slotMap.data();

    LogD("OnRequest - serial: [%d]", serial);
    for (uint32_t i = 0; i < slotMap.size(); i++) {
        LogD("slotMap[%d]: [%d]", i, slotMap[i]);
    }
    RADIOCONFIG_CALL_ONREQUEST(pRI->pCI->requestNumber, (void *) uData, slotMap.size() * sizeof(uint32_t), pRI, rilSocketId);
    LEAVE_FUNC();
    return Void();
}

/* 1.1  Requests */
Return<void> RadioConfigImpl::getPhoneCapability(int32_t serial) {
#if VDBG
    RLOGD("getPhoneCapability: serial %d", serial);
#endif

    radioconfig::dispatchVoid(serial, RIL_SOCKET_1, RIL_REQUEST_GET_PHONE_CAPABILITY);
    return Void();
}

Return<void> RadioConfigImpl::setPreferredDataModem(int32_t serial, uint8_t modemId) {
    ENTER_FUNC();
    RequestInfo *pRI = android::addRequestToList(serial, rilSocketId, RIL_REQUEST_SET_PREFERRED_DATA_MODEM);
    if (pRI == NULL) {
        LogE("Request is null");
        LEAVE_FUNC();
        return Void();
    }
    LogD("OnRequest - serial: [%d] modemId=%d", serial, modemId);
    // send an integer to vendor RIL
    int val = modemId;
    RADIOCONFIG_CALL_ONREQUEST(pRI->pCI->requestNumber, (void *)&val, sizeof(val), pRI, rilSocketId);
    LEAVE_FUNC();
    return Void();
}

Return<void> RadioConfigImpl::setModemsConfig(int32_t serial, const ModemsConfig &modemsConfig) {
#if VDBG
        RLOGD("setModemsConfig: serial %d", serial);
#endif
        int numOfLiveModems = (int)modemsConfig.numOfLiveModems;
        radioconfig::dispatchInts(serial, RIL_SOCKET_1, RIL_REQUEST_SET_MODEMS_CONFIG, 1, numOfLiveModems);
    return Void();
}

Return<void> RadioConfigImpl::getModemsConfig(int32_t serial) {
#if VDBG
    RLOGD("getModemsConfig: serial %d", serial);
#endif
    RequestInfo *pRI = android::addRequestToList(serial, RIL_SOCKET_1, RIL_REQUEST_GET_MODEMS_CONFIG);
    // do nothing
    if (pRI != NULL) {
        // VTS IRadioConfig@1.1 RadioConfigHidlTest#getModemsConfig
        radioconfig::sendErrorResponse(pRI, RIL_E_REQUEST_NOT_SUPPORTED);
    }
    return Void();
}

/* 1.0 Response */
int radioconfig::getSimSlotsStatusResponse(int slotId, int responseType,
        int serial, RIL_Errno e, void *response, size_t responseLen) {
#if VDBG
    LogD("serial %d", serial);
#endif
    RadioResponseInfo info = {};
    populateResponseInfoForRadioConfig(info, serial, responseType, e);
    hidl_vec<V1_0::SimSlotStatus> slotStatus = {};


    if (radioconfigService != NULL && radioconfigService->mRadioConfigResponse != NULL) {
        if (e != RIL_E_SUCCESS) {
            slotStatus.resize(0);
        } else if ((response == NULL) || (responseLen == 0)) {
            LogE("Invalid response");
            if (e == RIL_E_SUCCESS) info.error = RadioError::INVALID_RESPONSE;
            slotStatus.resize(0);
        } else {
            int sstVer = *((int *) response);
            LogD("sstVer:[%d]", sstVer);
            if (sstVer >= 20) {
                const V2_0::RIL_SimSlotStatusResult resp = *((V2_0::RIL_SimSlotStatusResult *) response);
                slotStatus.resize(resp.mSimSlotStatus.size());
                for (uint32_t slotIndex = 0; slotIndex < resp.mSimSlotStatus.size(); slotIndex++) {
                    const V2_0::RIL_SimSlotStatus sst = resp.mSimSlotStatus.at(slotIndex);
                    slotStatus[slotIndex].cardState = (CardState) sst.cardState;
                    slotStatus[slotIndex].atr = sst.atr;

                    if (sst.port_info.size() > 0) {
                        const V2_0::RIL_PORT_INFO pi = sst.port_info.at(0);
                        slotStatus[slotIndex].slotState = (SlotState) (pi.port_state == true
                                ? PHYSICAL_SLOT_ACTIVE : PHYSICAL_SLOT_INACTIVE);
                        slotStatus[slotIndex].logicalSlotId = pi.logicalSlotId;
                        slotStatus[slotIndex].iccid = pi.iccid;
                    }
                }
            } else {
                RIL_SimSlotStatusResult_1_2 resp = *((RIL_SimSlotStatusResult_1_2 *) response);
                slotStatus.resize(resp.mSimSlotStatus.size());
                for (uint32_t slotIndex = 0; slotIndex < resp.mSimSlotStatus.size(); slotIndex++) {
                    const RIL_SimSlotStatus_1_2 sst = resp.mSimSlotStatus.at(slotIndex);
                    slotStatus[slotIndex].cardState = (CardState) sst.cardState;
                    slotStatus[slotIndex].slotState = (SlotState) sst.slotState;
                    slotStatus[slotIndex].logicalSlotId = sst.logicalSlotId;
                    slotStatus[slotIndex].atr = sst.atr;
                    slotStatus[slotIndex].iccid = sst.iccid;
                }
            }
        }

        LogD("send to frameworks");
        Return<void> retStatus =
                radioconfigService->mRadioConfigResponse->getSimSlotsStatusResponse(info, slotStatus);
        radioconfigService->checkReturnStatus(retStatus);
    } else {
        LogE("radioConfigService[%d]->mRadioConfigResponse == NULL", slotId);
    }

    return 0;
}

int radioconfig::setSimSlotsMappingResponse(int slotId, int responseType,
        int serial, RIL_Errno e, void *response, size_t responseLen) {
#if VDBG
    LogD("serial %d", serial);
#endif
    RadioResponseInfo info = {};
    populateResponseInfoForRadioConfig(info, serial, responseType, e);

    if (radioconfigService->mRadioConfigResponse != NULL) {
        Return<void> retStatus = radioconfigService->mRadioConfigResponse->setSimSlotsMappingResponse(info);
        radioconfigService->checkReturnStatus(retStatus);
    } else {
        LogE("radioConfigService[%d]->mRadioConfigResponse == NULL", slotId);
    }

    return 0;
}

/* 1.1 Response */
int radioconfig::getPhoneCapabilityResponse(int slotId, int responseType,
        int serial, RIL_Errno e, void *response, size_t responseLen) {
    HIDL_HAL_RADIO_CONFIG_RESPONSE(getPhoneCapabilityResponse)
}

int radioconfig::setPreferredDataModemResponse(int slotId, int responseType,
        int serial, RIL_Errno e, void *response, size_t responseLen) {
    HIDL_HAL_RADIO_CONFIG_RESPONSE(setPreferredDataModemResponse)
}

int radioconfig::setModemsConfigResponse(int slotId, int responseType,
        int serial, RIL_Errno e, void *response, size_t responseLen) {
    HIDL_HAL_RADIO_CONFIG_RESPONSE(setModemsConfigResponse)
}

int radioconfig::getModemsConfigResponse(int slotId, int responseType,
        int serial, RIL_Errno e, void *response, size_t responseLen) {
    HIDL_HAL_RADIO_CONFIG_RESPONSE(getModemsConfigResponse)
}

/* 1.2 Response */
int radioconfig::getSimSlotsStatusResponse_1_2(int slotId, int responseType,
        int serial, RIL_Errno e, void *response, size_t responseLen) {
#if VDBG
    LogD("serial %d", serial);
#endif
    RadioResponseInfo info = {};
    populateResponseInfoForRadioConfig(info, serial, responseType, e);
    hidl_vec<V1_2::SimSlotStatus> slotStatus = {};
    if (radioconfigService != NULL && radioconfigService->mRadioConfigResponseV1_2 != NULL) {
        if (e != RIL_E_SUCCESS) {
            slotStatus.resize(0);
        } else if ((response == NULL) || (responseLen == 0)) {
            LogE("Invalid response");
            if (e == RIL_E_SUCCESS) info.error = RadioError::INVALID_RESPONSE;
            slotStatus.resize(0);
        } else {
            int sstVer = *((int *) response);
            LogD("sstVer:[%d]", sstVer);
            if (sstVer >= 20) {
                const V2_0::RIL_SimSlotStatusResult resp = *((V2_0::RIL_SimSlotStatusResult *) response);
                slotStatus.resize(resp.mSimSlotStatus.size());
                for (uint32_t slotIndex = 0; slotIndex < resp.mSimSlotStatus.size(); slotIndex++) {
                    const V2_0::RIL_SimSlotStatus sst = resp.mSimSlotStatus.at(slotIndex);
                    slotStatus[slotIndex].base.cardState = (CardState) sst.cardState;
                    slotStatus[slotIndex].base.atr = sst.atr;
                    slotStatus[slotIndex].eid = sst.eid;

                    if (sst.port_info.size() > 0) {
                        const V2_0::RIL_PORT_INFO pi = sst.port_info.at(0);
                        slotStatus[slotIndex].base.slotState = (SlotState) (pi.port_state == true
                                ? PHYSICAL_SLOT_ACTIVE : PHYSICAL_SLOT_INACTIVE);
                        slotStatus[slotIndex].base.logicalSlotId = pi.logicalSlotId;
                        slotStatus[slotIndex].base.iccid = pi.iccid;
                    }
                }
            } else {
                RIL_SimSlotStatusResult_1_2 resp = *((RIL_SimSlotStatusResult_1_2 *) response);
                slotStatus.resize(resp.mSimSlotStatus.size());
                for (uint32_t slotIndex = 0; slotIndex < resp.mSimSlotStatus.size(); slotIndex++) {
                    const RIL_SimSlotStatus_1_2 sst = resp.mSimSlotStatus.at(slotIndex);
                    slotStatus[slotIndex].base.cardState = (CardState) sst.cardState;
                    slotStatus[slotIndex].base.slotState = (SlotState) sst.slotState;
                    slotStatus[slotIndex].base.logicalSlotId = sst.logicalSlotId;
                    slotStatus[slotIndex].base.atr = sst.atr;
                    slotStatus[slotIndex].base.iccid = sst.iccid;
                    slotStatus[slotIndex].eid = sst.eid;
                }
            }
        }
        LogD("send to frameworks");
        Return<void> retStatus =
                radioconfigService->mRadioConfigResponseV1_2->getSimSlotsStatusResponse_1_2(info, slotStatus);
        radioconfigService->checkReturnStatus(retStatus);
    } else {
        LogE("radioConfigService[%d]->mRadioConfigResponseV1_2 == NULL", slotId);
        return radioconfig::getSimSlotsStatusResponse(slotId, responseType, serial, e,
                response, responseLen);
    }

    return 0;
}

/* 1.0 Indication */
int radioconfig::simSlotsStatusChanged(int slotId, int indicationType, int token, RIL_Errno e,
        void *response, size_t responseLen) {
    hidl_vec<V1_0::SimSlotStatus> slotStatus = {};
    if (radioconfigService != NULL && radioconfigService->mRadioConfigIndication != NULL) {
        if (response == NULL || responseLen == 0) {
            LogE("invalid response");
            return 0;
        }

        int sstVer = *((int *) response);
        LogD("sstVer:[%d]", sstVer);

        if (sstVer >= 20) {
            const V2_0::RIL_SimSlotStatusResult resp = *((V2_0::RIL_SimSlotStatusResult *) response);
            slotStatus.resize(resp.mSimSlotStatus.size());
            for (uint32_t slotIndex = 0; slotIndex < resp.mSimSlotStatus.size(); slotIndex++) {
                const V2_0::RIL_SimSlotStatus sst = resp.mSimSlotStatus.at(slotIndex);
                slotStatus[slotIndex].cardState = (CardState) sst.cardState;
                slotStatus[slotIndex].atr = sst.atr;

                if (sst.port_info.size() > 0) {
                    const V2_0::RIL_PORT_INFO pi = sst.port_info.at(0);
                    slotStatus[slotIndex].slotState = (SlotState) (pi.port_state == true
                            ? PHYSICAL_SLOT_ACTIVE : PHYSICAL_SLOT_INACTIVE);
                    slotStatus[slotIndex].logicalSlotId = pi.logicalSlotId;
                    slotStatus[slotIndex].iccid = pi.iccid;
                }
            }
        } else {
            RIL_SimSlotStatusResult_1_2 resp = *((RIL_SimSlotStatusResult_1_2 *) response);
            slotStatus.resize(resp.mSimSlotStatus.size());
            for (uint32_t slotIndex = 0; slotIndex < resp.mSimSlotStatus.size(); slotIndex++) {
                const RIL_SimSlotStatus_1_2 sst = resp.mSimSlotStatus.at(slotIndex);
                slotStatus[slotIndex].cardState = (CardState) sst.cardState;
                slotStatus[slotIndex].slotState = (SlotState) sst.slotState;
                slotStatus[slotIndex].logicalSlotId = sst.logicalSlotId;
                slotStatus[slotIndex].atr = sst.atr;
                slotStatus[slotIndex].iccid = sst.iccid;
            }
        }

        Return<void> retStatus = radioconfigService->mRadioConfigIndication->simSlotsStatusChanged(
                convertIntToRadioIndicationTypeForRadioConfig(indicationType), slotStatus);
        radioconfigService->checkReturnStatus(retStatus);
    } else {
        LogE("radioconfigService[%d]->mRadioConfigIndication == NULL", slotId);
    }

    return 0;
}

/* 1.1 Indication : none */

/* 1.2 Indication */
int radioconfig::simSlotsStatusChanged_1_2(int slotId, int indicationType, int token, RIL_Errno e,
        void *response, size_t responseLen) {
    hidl_vec<V1_2::SimSlotStatus> slotStatus = {};
    if (radioconfigService != NULL && radioconfigService->mRadioConfigIndicationV1_2 != NULL) {
        if (response == NULL || responseLen == 0) {
            LogE("invalid response");
            return 0;
        }

        int sstVer = *((int *) response);
        LogD("sstVer:[%d]", sstVer);

        if (sstVer >= 20) {
            const V2_0::RIL_SimSlotStatusResult resp = *((V2_0::RIL_SimSlotStatusResult *) response);
            slotStatus.resize(resp.mSimSlotStatus.size());
            for (uint32_t slotIndex = 0; slotIndex < resp.mSimSlotStatus.size(); slotIndex++) {
                const V2_0::RIL_SimSlotStatus sst = resp.mSimSlotStatus.at(slotIndex);
                slotStatus[slotIndex].base.cardState = (CardState) sst.cardState;
                slotStatus[slotIndex].base.atr = sst.atr;
                slotStatus[slotIndex].eid = sst.eid;

                if (sst.port_info.size() > 0) {
                    const V2_0::RIL_PORT_INFO pi = sst.port_info.at(0);
                    slotStatus[slotIndex].base.slotState = (SlotState) (pi.port_state == true
                            ? PHYSICAL_SLOT_ACTIVE : PHYSICAL_SLOT_INACTIVE);
                    slotStatus[slotIndex].base.logicalSlotId = pi.logicalSlotId;
                    slotStatus[slotIndex].base.iccid = pi.iccid;
                }
            }
        } else {
            RIL_SimSlotStatusResult_1_2 resp = *((RIL_SimSlotStatusResult_1_2 *) response);
            slotStatus.resize(resp.mSimSlotStatus.size());
            for (uint32_t slotIndex = 0; slotIndex < resp.mSimSlotStatus.size(); slotIndex++) {
                const RIL_SimSlotStatus_1_2 sst = resp.mSimSlotStatus.at(slotIndex);
                slotStatus[slotIndex].base.cardState = (CardState) sst.cardState;
                slotStatus[slotIndex].base.slotState = (SlotState) sst.slotState;
                slotStatus[slotIndex].base.logicalSlotId = sst.logicalSlotId;
                slotStatus[slotIndex].base.atr = sst.atr;
                slotStatus[slotIndex].base.iccid = sst.iccid;
                slotStatus[slotIndex].eid = sst.eid;
            }
        }

        Return<void> retStatus = radioconfigService->mRadioConfigIndicationV1_2->simSlotsStatusChanged_1_2(
                convertIntToRadioIndicationTypeForRadioConfig(indicationType), slotStatus);
        radioconfigService->checkReturnStatus(retStatus);
    } else {
        LogE("radioconfigService[%d]->mRadioConfigIndicationV1_2 == NULL", slotId);
        return radioconfig::simSlotsStatusChanged(slotId, indicationType, token, e,
                response, responseLen);
    }

    return 0;
}

void radioconfig::initRadioConfig() {
    if (s_radioconfig_registerCalled > 0) {
        LogE("RIL_RadioConfig_register has been called more than once. Subsequent call ignored");
        return;
    }

    radioconfigService = new RadioConfigImpl;
    radioconfigService->rilSocketId = RIL_SOCKET_1;
}

android::sp<::android::hardware::radio::config::V1_0::IRadioConfig> radioconfig::getRadioConfig() {
    return radioconfigService;
}

void radioconfig::registerService(RIL_RadioFunctions *callbacks, android::CommandInfo *commands) {
    if (s_radioconfig_registerCalled > 0) {
        LogE("RIL_RadioConfig_register has been called more than once. Subsequent call ignored");
        return;
    }
    s_radioconfig_vendorFunctions = callbacks;
    s_radioconfig_commands = commands;

#ifndef __RADIO_HAL_AIDL__
    using namespace android::hardware;

    RIL_SOCKET_ID socketId = RIL_SOCKET_1;
    radioconfigService = new RadioConfigImpl;
    radioconfigService->rilSocketId = socketId;
    LogI("starting IRadioConfig");
    android::status_t status = radioconfigService->registerAsService();
    LogD("started IRadioConfig status %d", status);
#else
    LogD("HIDL IRadioConfig not supported");
#endif
}

sp<::android::hardware::radio::config::V1_1::IRadioConfig> radioconfig::make(uint32_t slotId) {
    auto halRadioConfig1_1 = sp<RadioConfigImpl>::make();
    assert(halRadioConfig1_1 != nullptr);
    halRadioConfig1_1->rilSocketId = (RIL_SOCKET_ID) slotId;
    return halRadioConfig1_1;
}
