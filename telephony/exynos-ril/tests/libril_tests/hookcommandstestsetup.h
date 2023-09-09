/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd All Rights Reserved
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

#include <ril_internal.h>

template<typename ResponseHook, typename IndicationHook>
struct HookCommandsTestSetup {
    std::shared_ptr<ResponseHook> mResponseHook;
    std::shared_ptr<IndicationHook> mIndicationHook;

    HookCommandsTestSetup() {
        mResponseHook = ResponseHook::make();
        mIndicationHook = IndicationHook::make();
    }

    ~HookCommandsTestSetup() {
        teardown();
    }

    template<typename ResponseCallback, typename IndicationCallback>
    void setup(const std::shared_ptr<ResponseCallback>& responseCb,
               const std::shared_ptr<IndicationCallback>& indicationCb) {
        mResponseHook->setResponseFunction(responseCb);
        mIndicationHook->setResponseFunction(indicationCb);
        updateCommandsInfo(0, mResponseHook);
        updateUnsolResponseInfo(0, mIndicationHook);
    }

    void teardown() {
        mResponseHook->setResponseFunction(nullptr);
        mIndicationHook->setResponseFunction(nullptr);
    }
};