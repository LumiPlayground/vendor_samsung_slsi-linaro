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
#include <aidl-service/aidl-log.h>

#include "radiodataresponse.h"
#include "commonStructs.h"
#include "structs.h"
#include "collections.h"

#include <ril_internal.h>

namespace aidl_data = ::aidl::android::hardware::radio::data;
using namespace ::android::hardware::radio;
using namespace ::android::hardware::radio::compat;
using namespace ::android::hardware::radio::impl;
using ::android::hardware::Return;
using ::android::hardware::Void;

#define TAG "RadioDataIndicationHook"

void RadioDataIndicationHook::setResponseFunction(
    std::shared_ptr<RadioDataIndicationCallback> dataCb) {
    RLOGD("%s::%s dataCb=%p", TAG, __FUNCTION__, dataCb.get());
    mDataCb = dataCb;
}

Return<void> RadioDataIndicationHook::dataCallListChanged_1_6(
    V1_0::RadioIndicationType type,
    const ::android::hardware::hidl_vec<
        ::android::hardware::radio::V1_6::SetupDataCallResult> &dcList) {
    //NEED TO BE CHECKED
    std::vector<aidl_data::SetupDataCallResult> convertedList;
    for (auto &i : dcList)
        convertedList.push_back(toAidl(i));

    if (mDataCb)
        mDataCb->dataCallListChanged(toAidl(type), convertedList);

    return Void();
}

Return<void> RadioDataIndicationHook::pcoData(V1_0::RadioIndicationType type,
                                              const V1_0::PcoDataInfo &pco) {
#if VDBG
    RLOGD("%s::%s mDataCb=%p", TAG, __FUNCTION__, mDataCb.get());
#endif
    if (mDataCb) {
        mDataCb->pcoData(toAidl(type), toAidl(pco));
    }
    return Void();
}

Return<void>
RadioDataIndicationHook::keepaliveStatus(V1_0::RadioIndicationType type,
                                         const V1_1::KeepaliveStatus &status) {
#if VDBG
    RLOGD("%s::%s mDataCb=%p", TAG, __FUNCTION__, mDataCb.get());
#endif
    if (mDataCb) {
        mDataCb->keepaliveStatus(toAidl(type), toAidl(status));
    }
    return Void();
}
