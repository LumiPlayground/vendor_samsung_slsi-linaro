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
#include "basecommand.h"

/**
 * BaseCommand
 */
android::RequestInfo *BaseCommand::allocateRequestInfo(int serial) {
    if (mRequest < 0) {
        RLOGE("BaseCommand::allocateRequestInfo: invalid request");
        return nullptr;
    }
    auto pRI = (android::RequestInfo *)calloc(1, sizeof(android::RequestInfo));
    if (pRI) {
        memset(pRI, 0, sizeof(android::RequestInfo));
        pRI->token = serial;
        pRI->socket_id = (RIL_SOCKET_ID)mSlot;
        pRI->pCI = &mInfo;
    }
    return pRI;
}

/**
 * BaseIndication
 */
bool BaseIndication::listen() {
    if (mIndicationId < 0) {
        RLOGE("BaseIndication::listen: invalid indication");
        return false;
    }
    auto func = [this](int slotId, int indicationType, int token, RIL_Errno e,
                        void *response, size_t responselen) {
                    return onIndication(slotId, indicationType, token, e,
                                            response, responselen);
    };
    android::updateUnsolResponseInfo(mSlot, { mIndicationId, func, wakeType() });
    return true;
}