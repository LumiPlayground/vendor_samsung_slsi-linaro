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
 * ril_oem_service.h
 *
 *  Created on: 2021. 5. 12.
 */

#ifndef __RIL_OEM_SERVICE_H__
#define __RIL_OEM_SERVICE_H__

#include <vendor/samsung_slsi/telephony/hardware/radio/1.1/IOemSamsungslsi.h>
#include <telephony/ril.h>
#include <ril_internal.h>

namespace vendor {
namespace radio {
namespace impl {
namespace legacy {

void registerAsService();
void initRadios();
android::sp<::vendor::samsung_slsi::telephony::hardware::radio::V1_1::IOemSamsungslsi> getRadio(int slotId);

}  // namespace legacy
}  // namespace impl


// vendor::radio callback functions

/**
 * solicited response
 */
int queryCOLPResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen);
int queryCOLRResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen);
int iccGetAtrResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen);
int sendUSSDWithDcsResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen);
int setEmcStatusResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen);
int setFemtoCellSearchResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen);
int setCdmaHybridModeResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen);
int getCdmaHybridModeResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen);
int queryBplmnSearchResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen);
int setNetworkSelectionModeManualResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen);
int dialWithCallTypeResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen);
int setVoiceOperationResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen);
int setBarringPasswordOverMmiResponse(int slotId,int responseType,int serial,RIL_Errno e,void * response,size_t responseLen);
int setDualNetworkTypeAndAllowDataResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen);
int setDsNetworkTypeResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen);
int deactivateDataCallWithReasonResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen);
int getSimLockStatusResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen);
int sendVsimNotificationResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen);
int sendVsimOperationResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen);
int sendRequestRawResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen);

/**
 * unsolicited response
 */
int suppSvcReturnResult(int slotId, int indicationType, int token, RIL_Errno e, void *response, size_t responselen);
int callPresentInd(int slotId, int indicationType, int token, RIL_Errno e, void *response, size_t responselen);
int wbAmrReportInd(int slotId, int indicationType, int token, RIL_Errno e, void *response, size_t responselen);
int vsimOperationInd(int slotId, int indicationType, int token, RIL_Errno e, void *response, size_t responselen);
int nasTimerStatusInd(int slotId, int indicationType, int token, RIL_Errno e, void *response, size_t responselen);
int emergencyActInd(int slotId, int indicationType, int token, RIL_Errno e, void *response, size_t responselen);
int onUssdWithDcsInd(int slotId, int indicationType, int token, RIL_Errno e, void *response, size_t responselen);
int volteAvailableInfoInd(int slotId, int indicationType, int token, RIL_Errno e, void *response, size_t responselen);
int iccIdInfoInd(int slotId, int indicationType, int token, RIL_Errno e, void *response, size_t responselen);
int emergencySupportRatModeInd(int slotId, int indicationType, int token, RIL_Errno e, void *response, size_t responselen);
int ussdCanceledInd(int slotId, int indicationType, int token, RIL_Errno e, void *response, size_t responselen);

/**
* @1.1
*/
/**
 * solicited response
 */
int setActivateVsimResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen);
int setEndcModeResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen);
int getEndcModeResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen);
int getSmsStorageOnSimResponse(int slotId,
                             int responseType, int serial, RIL_Errno e, void *response,
                             size_t responselen);
int setNrModeResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen);
int getNrModeResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen);

/**
* @1.2
*/
/**
 * solicited response
 */

int setupDataCallResponse_1_6(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responselen);
int getSlicingConfigResponse(int slotId, int responseType, int serial, RIL_Errno e, void* response, size_t responseLen);

/**
 * unsolicited response
 */
int currentPhysicalChannelConfigInd(int slotId, int indicationType, int token, RIL_Errno e, void *response, size_t responselen);
int endcCapabilityInd(int slotId, int indicationType, int token, RIL_Errno e, void *response, size_t responselen);
int simTrayStatusChangedInd(int slotId, int indicationType, int token, RIL_Errno e, void *response, size_t responselen);
int unthrottleApnInd(int slotId, int indicationType, int token, RIL_Errno e, void *response, size_t responselen);

}  // namespace radio
}  // namespace vendor

#endif /* __RIL_OEM_SERVICE_H__ */
