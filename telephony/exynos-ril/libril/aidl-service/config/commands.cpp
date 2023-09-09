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
#include <aidl-log.h>
#include <ril_internal.h>
#include "commands.h"
#include "commonvendorstructs.h"
#include "vendorstructs.h"

namespace android::hardware::radio::impl::config {

namespace aidl = ::aidl::android::hardware::radio;
namespace aidl_config = ::aidl::android::hardware::radio::config;

/**
 * GetHalDeviceCapabilitiesCommand
 */
void GetHalDeviceCapabilitiesCommand::onRequest(int serial) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    mVendorFunctions.dispatchVoid(allocateRequestInfo(serial));
}

int GetHalDeviceCapabilitiesCommand::onResponse(int slotId,int responseType, int serial, RIL_Errno e,
                                          void *response, size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    if (mCb) {
        aidl::RadioResponseInfo responseInfo = populateResponseInfo(serial, responseType, e);
        bool resp = false;
        if (response == nullptr || responselen != sizeof(int)) {
            RLOGE("%s::%s Invalid response", TAG, __FUNCTION__);
            if (e == RIL_E_SUCCESS) responseInfo.error = aidl::RadioError(RIL_E_INVALID_RESPONSE);
        } else {
            resp = (((int32_t *) response)[0] == 1) ? true : false;
        }
        mCb->getHalDeviceCapabilitiesResponse(responseInfo, resp);
    } else {
        RLOGE("%s::%s: [%d] mCb == NULL", TAG, __FUNCTION__, slotId);
    }
    return 0;
}

/**
 * getSimSlotsStatus
 */
void GetSimSlotsStatusCommand::onRequest(int serial) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    mVendorFunctions.dispatchVoid(allocateRequestInfo(serial));
}

int GetSimSlotsStatusCommand::onResponse(int slotId, int responseType, int serial, RIL_Errno e,
        void * response, size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    if (mCb) {
        aidl::RadioResponseInfo responseInfo = populateResponseInfo(serial, responseType, e);
        std::vector<aidl_config::SimSlotStatus> slotStatus = {};

        if (response == nullptr || responselen == 0) {
            RLOGE("%s::%s Invalid response", TAG, __FUNCTION__);
            if (e == RIL_E_SUCCESS) responseInfo.error = aidl::RadioError(RIL_E_INVALID_RESPONSE);
        } else {
            int sstVer = *((int *) response);
            RLOGD("%s::%s sstVer:[%d]", TAG, __FUNCTION__, sstVer);
            if (sstVer >= 20) {
                const V2_0::RIL_SimSlotStatusResult resp = *((V2_0::RIL_SimSlotStatusResult *) response);
                for (uint32_t slotIndex = 0; slotIndex < resp.mSimSlotStatus.size(); slotIndex++) {
                    const V2_0::RIL_SimSlotStatus sst = resp.mSimSlotStatus.at(slotIndex);
                    slotStatus.push_back(toAidl(sst));
                }
            } else {
                const RIL_SimSlotStatusResult_1_2 resp = *((RIL_SimSlotStatusResult_1_2 *) response);
                for (uint32_t slotIndex = 0; slotIndex < resp.mSimSlotStatus.size(); slotIndex++) {
                    RIL_SimSlotStatus_1_2 sst = resp.mSimSlotStatus.at(slotIndex);
                    slotStatus.push_back(toAidl(sst));
                }
            }
            mCb->getSimSlotsStatusResponse(responseInfo, slotStatus);
        }
    } else {
        RLOGE("%s::%s: [%d] mCb == NULL", TAG, __FUNCTION__, slotId);
    }
    return 0;
}

/**
 * SetLogicalToPhysicalSlotMapping
 */
void SetLogicalToPhysicalSlotPortMapping::onRequest(int serial,
        const std::vector<aidl_config::SlotPortMapping>& slotMap) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    V2_0::RIL_SlotPortMappingReq slotPortMappingReq = {};
    slotPortMappingReq.num_slots = slotMap.size();
    RLOGD("slotPortMappingReq num_slots:[%d]", slotPortMappingReq.num_slots);
    for (int i = 0; i < slotPortMappingReq.num_slots; i++) {
        slotPortMappingReq.slotPortMap[i].physicalSlotId = slotMap[i].physicalSlotId;
        slotPortMappingReq.slotPortMap[i].portId = slotMap[i].portId;
        RLOGD("slotPortMappingReq index:[%d], physicalSlotId:[%d], portId[%d]", i,
                slotPortMappingReq.slotPortMap[i].physicalSlotId, slotPortMappingReq.slotPortMap[i].portId);
    }

    mVendorFunctions.onRequest(allocateRequestInfo(serial), (void *) &slotPortMappingReq,
            sizeof(V2_0::RIL_SlotPortMappingReq));
}

int SetLogicalToPhysicalSlotPortMapping::onResponse(int slotId, int responseType, int serial, RIL_Errno e,
        void * response, size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    if (mCb) {
        aidl::RadioResponseInfo responseInfo = populateResponseInfo(serial, responseType, e);
        mCb->setSimSlotsMappingResponse(responseInfo);
    } else {
        RLOGE("%s::%s: [%d] mCb == NULL", TAG, __FUNCTION__, slotId);
    }
    return 0;
}

/**
 * SimSlotsStatusChanged
 */
int SimSlotsStatusChangedIndication::onIndication(int slotId, int indicationType, int token, RIL_Errno e,
        void *response, size_t responselen) {
#if VDBG
    RLOGD("%s::%s: mCb=%p", TAG, __FUNCTION__, mCb.get());
#endif
    if (mCb) {
        std::vector<aidl_config::SimSlotStatus> slotStatus = {};
        if (response == nullptr || responselen == 0) {
            RLOGE("invalid Indication");
            return 0;
        }
        int sstVer = *((int *) response);
        RLOGD("%s::%s sstVer:[%d]", TAG, __FUNCTION__, sstVer);
        if (sstVer >= 20) {
            const V2_0::RIL_SimSlotStatusResult resp = *((V2_0::RIL_SimSlotStatusResult *) response);
            for (uint32_t slotIndex = 0; slotIndex < resp.mSimSlotStatus.size(); slotIndex++) {
                const V2_0::RIL_SimSlotStatus sst = resp.mSimSlotStatus.at(slotIndex);
                slotStatus.push_back(toAidl(sst));
            }
        } else {
            const RIL_SimSlotStatusResult_1_2 resp = *((RIL_SimSlotStatusResult_1_2 *) response);
            for (uint32_t slotIndex = 0; slotIndex < resp.mSimSlotStatus.size(); slotIndex++) {
                RIL_SimSlotStatus_1_2 sst = resp.mSimSlotStatus.at(slotIndex);
                slotStatus.push_back(toAidl(sst));
            }
        }
        mCb->simSlotsStatusChanged(aidl::RadioIndicationType(indicationType), slotStatus);
    } else {
        RLOGE("%s::%s: [%d] mCb == NULL", TAG, __FUNCTION__, slotId);
    }
    return 0;
}
}

