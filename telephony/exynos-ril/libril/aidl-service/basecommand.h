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
#pragma once

#include <vendorfunctions.h>
#include <ril_internal.h>

struct BaseCommand {
    uint32_t mSlot = 0;
    VendorFunctions mVendorFunctions;
    int mRequest = -1;
    android::CommandInfo mInfo;

    BaseCommand() : mVendorFunctions(VendorFunctions()), mInfo({-1, 0}) {}
    BaseCommand(uint32_t slotId) : BaseCommand(VendorFunctions(slotId)) {}
    BaseCommand(const VendorFunctions& vendorFunctions)
                : mVendorFunctions(vendorFunctions), mInfo({-1, 0}) {
        mSlot = mVendorFunctions.slotId();
    }
    virtual ~BaseCommand() = default;

    void setSlotId(uint32_t slot) { mSlot = slot; }
    void setRequestId(int request) {
        if (request >= 0) {
            mRequest = request;
            mInfo = {
                mRequest,
                [this](int slotId, int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen) {
                    return onResponse(slotId, responseType, serial, e, response, responselen);
                },
            };
        }
    }
    android::RequestInfo *allocateRequestInfo(int serial);
    virtual int onResponse(int slotId, int responseType, int serial, RIL_Errno e,
                void *response, size_t responselen) = 0;
};

struct BaseIndication {
    uint32_t mSlot = 0;
    int mIndicationId = -1;

    BaseIndication() {}
    BaseIndication(uint32_t slot) : mSlot(slot) {}
    virtual ~BaseIndication() = default;

    void setSlotId(uint32_t slot) { mSlot = slot; }
    void setIndicationId(int indication) { mIndicationId = indication; }
    // override if DONT_WAKE
    virtual android::WakeType wakeType() { return android::WAKE_PARTIAL; }
    bool listen();
    virtual int onIndication(int slotId, int indicationType, int token, RIL_Errno e,
                    void *response, size_t responselen) = 0;
};