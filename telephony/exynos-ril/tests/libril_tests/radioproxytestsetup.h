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

#include <radioproxy.h>
#include "test_radio_hidl_1_6.h"
#include "test_radioconfig_hidl_1_1.h"

template<typename RadioProxyType>
struct RadioProxyTestSetup {
    std::shared_ptr<RadioProxyType> mRadioProxy;

    RadioProxyTestSetup()=default;
    RadioProxyTestSetup(const RadioProxyTestSetup& o) : mRadioProxy(o.mRadioProxy) {}
    RadioProxyTestSetup(RadioProxyTestSetup&& o) noexcept {
        mRadioProxy = std::move(o.mRadioProxy);
    }
    template<typename HidlRadioType>
    RadioProxyTestSetup(const ::android::sp<HidlRadioType>& hidlRadio) {
        setup(hidlRadio);
    }
    template<typename HidlRadioType, typename HidlRadioConfigType>
    RadioProxyTestSetup(const ::android::sp<HidlRadioType>& hidlRadio, const ::android::sp<HidlRadioConfigType>& hidlRadioConfig) {
        setup(hidlRadio, hidlRadioConfig);
    }
    ~RadioProxyTestSetup() { teardown(); }

    template<typename HidlRadioType>
    void setup(const ::android::sp<HidlRadioType>& hidlRadio) {
        mRadioProxy = std::make_shared<RadioProxyType>(0, RadioProxy::make(0, hidlRadio, nullptr));
    }
    template<typename HidlRadioType, typename HidlRadioConfigType>
    void setup(const ::android::sp<HidlRadioType>& hidlRadio, const ::android::sp<HidlRadioConfigType>& hidlRadioConfig) {
        mRadioProxy = std::make_shared<RadioProxyType>(0, RadioProxy::make(0, hidlRadio, hidlRadioConfig));
    }

    void teardown() {
        mRadioProxy = nullptr;
    }

    std::shared_ptr<RadioProxyType>& operator->() { return mRadioProxy; }
};