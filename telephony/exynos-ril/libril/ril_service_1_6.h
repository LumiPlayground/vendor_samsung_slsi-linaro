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

#ifndef RIL_SERVICE__1_6_H
#define RIL_SERVICE__1_6_H

#include <android/hardware/radio/1.6/IRadio.h>
#include <telephony/ril.h>

namespace radio {
namespace impl {
namespace v1_6 {

struct RadioImpl;

void registerAsService();
void initRadios();
android::sp<::android::hardware::radio::V1_0::IRadio> getRadio(int slotId);
android::sp<::android::hardware::radio::V1_6::IRadio> make(uint32_t slotId);
void checkReturnStatus(int32_t slotId, ::android::hardware::Return<void>& ret);

}  // v1_6
}  // impl

///////////////////////////////////////////////////////////////////////////
// radio 1.6
///////////////////////////////////////////////////////////////////////////
int setupDataCallResponse_1_6(int slotId,
                            int responseType, int serial, RIL_Errno e, void *response,
                            size_t responseLen);

int getDataCallListResponse_1_6(int slotId,
                            int responseType, int serial, RIL_Errno e,
                            void *response, size_t responseLen);

int dataCallListChangedInd_1_6(int slotId, int indicationType,
                           int token, RIL_Errno e, void *response, size_t responseLen);

int currentLinkCapacityEstimate_1_6(int slotId, int indicationType,
                           int token, RIL_Errno e, void *response, size_t responseLen);

int setNrDualConnectivityStateResponse(int slotId, int responseType,
                            int serial, RIL_Errno e, void *response, size_t responseLen);

int isNrDualConnectivityEnabledResponse(int slotId, int responseType,
                            int serial, RIL_Errno e, void *response, size_t responseLen);

int setRadioPowerResponse_1_6(int slotId, int responseType,
                            int serial, RIL_Errno e, void *response, size_t responseLen);

int sendSmsResponse_1_6(int slotId, int responseType,
                            int serial, RIL_Errno e, void *response, size_t responseLen);

int sendSmsExpectMoreResponse_1_6(int slotId, int responseType,
                            int serial, RIL_Errno e, void *response, size_t responseLen);

int sendCdmaSmsResponse_1_6(int slotId, int responseType,
                            int serial, RIL_Errno e, void *response, size_t responseLen);

int sendCdmaSmsExpectMoreResponse_1_6(int slotId, int responseType,
                            int serial, RIL_Errno e, void *response, size_t responseLen);

int setSimCardPowerResponse_1_6(int slotId, int responseType,
                            int serial, RIL_Errno e, void *response, size_t responseLen);

int allocatePduSessionIdResponse(int slotId, int responseType,
                            int serial, RIL_Errno e, void *response, size_t responseLen);

int releasePduSessionIdResponse(int slotId, int responseType,
                            int serial, RIL_Errno e, void *response, size_t responseLen);

int startHandoverResponse(int slotId, int responseType,
                            int serial, RIL_Errno e, void *response, size_t responseLen);

int cancelHandoverResponse(int slotId, int responseType,
                            int serial, RIL_Errno e, void *response, size_t responseLen);

int setAllowedNetworkTypesBitmapResponse(int slotId, int responseType,
                            int serial, RIL_Errno e, void *response, size_t responseLen);

int setDataThrottlingResponse(int slotId, int responseType,
                            int serial, RIL_Errno e, void *response, size_t responseLen);

int getSystemSelectionChannelsResponse(int slotId, int responseType,
                            int serial, RIL_Errno e, void *response, size_t responseLen);

int unthrottleApn(int slotId, int indicationType,
                           int token, RIL_Errno e, void *response, size_t responseLen);

int currentPhysicalChannelConfigs_1_6(int slotId, int indicationType,
                            int token, RIL_Errno e, void *response, size_t responselen);

int getCurrentCallsResponse_1_6(int slotId, int responseType,
                             int serial, RIL_Errno e, void *response, size_t responselen);

int getVoiceRegistrationStateResponse_1_6(int slotId,
                            int responseType, int serial, RIL_Errno e, void *response,
                            size_t responselen);

int getDataRegistrationStateResponse_1_6(int slotId,
                            int responseType, int serial, RIL_Errno e, void *response,
                            size_t responselen);

int getAllowedNetworkTypesBitmapResponse(int slotId,
                            int responseType, int serial, RIL_Errno e, void *response,
                            size_t responselen);

int getSignalStrengthResponse_1_6(int slotId,
                            int responseType, int serial, RIL_Errno e, void *response,
                            size_t responselen);

int currentSignalStrengthInd_1_6(int slotId,
                             int indicationType, int token, RIL_Errno e,
                             void *response, size_t responselen);

int getCellInfoListResponse_1_6(int slotId,
                            int responseType, int serial, RIL_Errno e, void *response,
                            size_t responselen);

int cellInfoListInd_1_6(int slotId,
                             int indicationType, int token, RIL_Errno e,
                             void *response, size_t responselen);

int networkScanResultInd_1_6(int slotId,
                             int indicationType, int token, RIL_Errno e,
                             void *response, size_t responselen);


int getSlicingConfigResponse(int slotId,
                            int responseType, int serial, RIL_Errno e, void *response,
                            size_t responselen);

int getSimPhonebookRecordsResponse(int slotId, int responseType, int serial,
                            RIL_Errno e, void *response, size_t responseLen);

int getSimPhonebookCapacityResponse(int slotId, int responseType, int serial,
                            RIL_Errno e, void *response, size_t responseLen);

int updateSimPhonebookRecordsResponse(int slotId, int responseType, int serial,
                            RIL_Errno e, void *response, size_t responseLen);

int simPhonebookChanged(int slotId,
                            int indicationType, int token, RIL_Errno e,
                            void *response, size_t responseLen);

int simPhonebookRecordsReceived(int slotId,
                            int indicationType, int token, RIL_Errno e,
                            void *response, size_t responseLen);


}  // radio

bool dispatchDataThrottling(int serial, int slotId, int request,
        android::hardware::radio::V1_6::DataThrottlingAction dataThrottlingAction,
        int64_t completionDurationMillis);

void populateResponseInfo(android::hardware::radio::V1_6::RadioResponseInfo& responseInfo,
        int serial, int responseType, RIL_Errno e);

int responseInt(android::hardware::radio::V1_6::RadioResponseInfo& responseInfo, int serial,
        int responseType, RIL_Errno e, void *response, size_t responseLen);

android::hardware::radio::V1_0::SendSmsResult makeSendSmsResult(android::hardware::radio::V1_6::RadioResponseInfo& responseInfo,
        int serial, int responseType, RIL_Errno e, void *response, size_t responseLen);

#endif  // RIL_SERVICE__1_6_H
