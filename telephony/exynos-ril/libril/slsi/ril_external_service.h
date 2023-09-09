/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
*/

#ifndef RIL_EXTERNAL_SERVICE_H
#define RIL_EXTERNAL_SERVICE_H

#include <vendor/samsung_slsi/telephony/hardware/radioExternal/1.1/IOemSlsiRadioExternal.h>
#include <telephony/ril.h>
#include <telephony/ril_ext.h>
#include <slsi/ril_client.h>
#include <slsi/ril_external.h>
#include <ril_internal.h>
#include <libril/ril_ex.h>

#define RIL_EXTERNAL_SERVICE_NAME "rilExternal"

extern bool IS_DEBUGABLE();

typedef int (*radioExternal_responseFunction)(int, int, int, int, RIL_Errno, void *, size_t);

// reference in ril_service
extern bool IsMultiSimEnabled();
extern int GetPhoneCount();

namespace radioExternal {
void initVendorRadio();
void registerService(const RIL_RadioExternalFunctions *callbacks);
android::sp<::vendor::samsung_slsi::telephony::hardware::radioExternal::V1_1::IOemSlsiRadioExternal> getRadio();
void setVendorFunctions(RIL_RadioExternalFunctions *callbacks);


pthread_rwlock_t * getRadioExternalServiceRwlock();

int getOverallRadioState(int slot);
/**
 * solicited response
 */
int sendRequestRawResponse(int clientId, int rilcMsgId, int slotId, int serial, RIL_Errno e, void *response, size_t responseLen);

// response handler
int getAvailableNtworksResponse(int clientId, int rilcMsgId, int slotId, int serial,
        RIL_Errno e, void *response, size_t responseLen);
/**
 * unsolicited response
 */
int rilExternalRawIndication(int clientId, int rilcMsgId, int slotId, const void *indication, size_t indicationLen);

}   // namespace radioExternal

#endif  // RIL_EXTERNAL_SERVICE_H
