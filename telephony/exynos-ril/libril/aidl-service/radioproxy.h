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

#include <android/hardware/radio/1.6/IRadio.h>
#include <android/hardware/radio/config/1.1/IRadioConfig.h>
#include <radiomodemcallback.h>
#include <radiodatacallback.h>
#include <radiomessagingcallback.h>
#include <radiosimcallback.h>
#include <radioconfigcallback.h>
#include <radionetworkcallback.h>
#include <radiovoicecallback.h>

struct RadioResponse;
struct RadioIndication;

// RadioProxy
// HIDL independent radio interface
// For a legacy, try to get HIDL radio using hidl().
struct RadioProxy {
    uint32_t mSlot;
    ::android::sp<::android::hardware::radio::V1_6::IRadio> mHidlRadio;
    ::android::sp<::android::hardware::radio::config::V1_1::IRadioConfig> mHidlRadioConfig;
    std::shared_ptr<RadioResponse> mRadioResponse;
    std::shared_ptr<RadioIndication> mRadioIndication;

    std::shared_ptr<android::hardware::radio::impl::RadioModemCallback> mRadioModem;
    std::shared_ptr<android::hardware::radio::impl::RadioDataCallback> mRadioData;
    std::shared_ptr<android::hardware::radio::impl::RadioMessagingCallback> mRadioMessaging;
    std::shared_ptr<android::hardware::radio::impl::RadioNetworkCallback> mRadioNetwork;
    std::shared_ptr<android::hardware::radio::impl::RadioSimCallback> mRadioSim;
    std::shared_ptr<android::hardware::radio::impl::RadioConfigCallback> mRadioConfig;
    std::shared_ptr<android::hardware::radio::impl::RadioVoiceCallback> mRadioVoice;

    RadioProxy(uint32_t slot, ::android::sp<::android::hardware::radio::V1_6::IRadio> hidlRadio)
            : mSlot(slot), mHidlRadio(hidlRadio) {}

    ::android::sp<::android::hardware::radio::V1_6::IRadio> hidl() { return mHidlRadio; }
    ::android::sp<::android::hardware::radio::config::V1_1::IRadioConfig> hidlConfig() { return mHidlRadioConfig; }
    void setResponseFunctions(const std::shared_ptr<RadioResponse>& radioResponse,
                                const std::shared_ptr<RadioIndication>& radioIndication) {
        mRadioResponse = radioResponse;
        mRadioIndication = radioIndication;
    }
    std::shared_ptr<RadioResponse> response() { return mRadioResponse; }
    std::shared_ptr<RadioIndication> indication() { return mRadioIndication; }

    void modem(const std::shared_ptr<android::hardware::radio::impl::RadioModemCallback>& radio) { mRadioModem = radio; }
    std::shared_ptr<android::hardware::radio::impl::RadioModemCallback> modem() { return mRadioModem; }

    void data(const std::shared_ptr<android::hardware::radio::impl::RadioDataCallback>& radio) { mRadioData = radio; }
    std::shared_ptr<android::hardware::radio::impl::RadioDataCallback> data() { return mRadioData; }

    void messaging(const std::shared_ptr<android::hardware::radio::impl::RadioMessagingCallback>& radio) { mRadioMessaging = radio; }
    std::shared_ptr<android::hardware::radio::impl::RadioMessagingCallback> messaging() { return mRadioMessaging; }

    void network(const std::shared_ptr<android::hardware::radio::impl::RadioNetworkCallback>& radio) { mRadioNetwork = radio; }
    std::shared_ptr<android::hardware::radio::impl::RadioNetworkCallback> network() { return mRadioNetwork; }

    void sim(const std::shared_ptr<android::hardware::radio::impl::RadioSimCallback>& radio) { mRadioSim = radio; }
    std::shared_ptr<android::hardware::radio::impl::RadioSimCallback> sim() { return mRadioSim; }

    void config(const std::shared_ptr<android::hardware::radio::impl::RadioConfigCallback>& radio) { mRadioConfig = radio; }
    std::shared_ptr<android::hardware::radio::impl::RadioConfigCallback> config() { return mRadioConfig; }

    void voice(const std::shared_ptr<android::hardware::radio::impl::RadioVoiceCallback>& radio) { mRadioVoice = radio; }
    std::shared_ptr<android::hardware::radio::impl::RadioVoiceCallback> voice() { return mRadioVoice; }

    static std::shared_ptr<RadioProxy> make(uint32_t slot,
                                    const ::android::sp<::android::hardware::radio::V1_6::IRadio>& hidlRadio,
                                    const ::android::sp<::android::hardware::radio::config::V1_1::IRadioConfig>& hidlRadioConfig) {
        auto radio = std::make_shared<RadioProxy>(slot, hidlRadio);
        radio->mHidlRadioConfig = hidlRadioConfig;
        return radio;
    }
};
