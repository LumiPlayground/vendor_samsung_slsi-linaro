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
 * ril_service_common.h
 *
 *  Created on: 2020. 11. 18.
 *      Author: sungwoo48.choi
 */

#ifndef RIL_SERVICE_COMMON_H_
#define RIL_SERVICE_COMMON_H_

#include <hidl/HidlSupport.h>
#include <telephony/ril.h>
#include <ril_internal.h>

#if defined(ANDROID_MULTI_SIM)
#define CALL_ONREQUEST(a, b, c, d, e) \
        s_vendorFunctions->onRequest((a), (b), (c), (d), ((RIL_SOCKET_ID)(e)))
#define CALL_ONSTATEREQUEST(a) s_vendorFunctions->onStateRequest((RIL_SOCKET_ID)(a))
#else
#define CALL_ONREQUEST(a, b, c, d, e) s_vendorFunctions->onRequest((a), (b), (c), (d))
#define CALL_ONSTATEREQUEST(a) s_vendorFunctions->onStateRequest()
#endif

#define BOOL_TO_INT(x) (x ? 1 : 0)
#define ATOI_NULL_HANDLED(x) (x ? atoi(x) : -1)
#define ATOI_NULL_HANDLED_DEF(x, defaultVal) (x ? atoi(x) : defaultVal)

extern RIL_RadioFunctions *s_vendorFunctions;

namespace radio {
void registerService(RIL_RadioFunctions *callbacks);
pthread_rwlock_t * getRadioServiceRwlock(int slotId);

bool IsMultiSimEnabled();
int GetPhoneCount();

}  // radio

void memsetAndFreeStrings(int numPointers, ...);
void sendErrorResponse(android::RequestInfo *pRI, RIL_Errno err);
bool copyHidlStringToRil(char **dest, const android::hardware::hidl_string &src, android::RequestInfo *pRI, bool allowEmpty);
bool copyHidlStringToRil(char **dest, const android::hardware::hidl_string &src, android::RequestInfo *pRI);
bool dispatchVoid(int serial, int slotId, int request);
bool dispatchInts(int serial, int slotId, int request, int countInts, ...);
bool dispatchString(int serial, int slotId, int request, const char * str);
bool dispatchStrings(int serial, int slotId, int request, bool allowEmpty, int countStrings, ...);
bool dispatchStrings(int serial, int slotId, int request, const android::hardware::hidl_vec<android::hardware::hidl_string>& data);
bool dispatchRaw(int serial, int slotId, int request, const android::hardware::hidl_vec<uint8_t>& rawBytes);

#endif /* RIL_SERVICE_COMMON_H_ */
