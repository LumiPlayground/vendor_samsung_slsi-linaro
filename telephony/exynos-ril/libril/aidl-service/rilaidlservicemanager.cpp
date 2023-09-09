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

#include "rilaidlservicemanager.h"
#include <radiocallbackadapter.h>
#include <aidl-utils.h>
#include "radioproxy.h"
#include "rilaidlservice.h"
#include "radioresponse.h"
#include "radiomodemservice.h"
#include "radiomodemproxy.h"
#include "radiodataservice.h"
#include "radiodataproxy.h"
#include "radioconfigservice.h"
#include "radioconfigproxy.h"
#include "radiosimservice.h"
#include "radiosimproxy.h"
#include "radionetworkservice.h"
#include "radionetworkproxy.h"
#include "radiomessagingservice.h"
#include "radiomessagingproxy.h"
#include "radiovoiceservice.h"
#include "radiovoiceproxy.h"
#include "ril_service_1_6.h"
#include "radioconfig_service.h"
#include <thread>

using namespace android::hardware::radio::impl;

RilAidlServiceManager::RilAidlServiceManager() {
    #if (SIM_COUNT >= 2)
    mSimCount = SIM_COUNT;
    #endif
    for (uint32_t i = 0; i < mSimCount; i++) {
        auto radio = RadioProxy::make(i, radio::impl::v1_6::make(i), radioconfig::make(i));
        auto response = RadioResponse::make(i);
        auto indication = RadioIndication::make(i);
        radio->setResponseFunctions(response, indication);

        // proxy for sub categories
        radio->modem(RadioModemProxy::make(i, radio));
        radio->data(RadioDataProxy::make(i, radio));
        radio->sim(RadioSimProxy::make(i, radio));
        radio->network(RadioNetworkProxy::make(i, radio));
        radio->messaging(RadioMessagingProxy::make(i, radio));
        radio->voice(RadioVoiceProxy::make(i, radio));

        // IRadioConfig is a single service.
        if (i == RIL_SOCKET_1) {
            radio->config(RadioConfigProxy::make(i, radio));
        }
        mRadio.push_back(radio);
        mRadioResponse.push_back(response);
        mRadioIndication.push_back(indication);

    }
}

bool RilAidlServiceManager::publishRadios() {
    std::vector<std::shared_ptr<RilAidlService>> services;
    for (uint32_t i = 0; i < mSimCount; i++) {
        auto radio = mRadio.at(i);
        auto responseHook = mRadioResponse.at(i);
        auto indicationHook = mRadioIndication.at(i);

        services.push_back(std::make_shared<RadioModemService>(i, radio, responseHook, indicationHook));
        services.push_back(std::make_shared<RadioDataService>(i, radio, responseHook, indicationHook));
        services.push_back(std::make_shared<RadioSimService>(i, radio, responseHook, indicationHook));
        services.push_back(std::make_shared<RadioNetworkService>(i, radio, responseHook, indicationHook));
        services.push_back(std::make_shared<RadioMessagingService>(i, radio, responseHook, indicationHook));
        services.push_back(std::make_shared<RadioVoiceService>(i, radio, responseHook, indicationHook));

        // IRadioConfig is a single service.
        if (i == RIL_SOCKET_1) {
            // add IRadioConfig here
            services.push_back(std::make_shared<RadioConfigService>(i, radio, responseHook, indicationHook));
        }
    }

    for (auto& s : services) {
        if (s->publish()) {
            mPublishedService.push_back(s);
        }
    }

    return (mPublishedService.size() > 0);
}

std::shared_ptr<RilAidlServiceManager> RilAidlServiceManager::instance;

bool RilAidlServiceManager::init() {
    bool ret = false;
#ifdef __RADIO_HAL_AIDL__
    instance = std::make_shared<RilAidlServiceManager>();
    ret = instance->publishRadios();
#endif
    return ret;
}

// this function will be called by rild.c.
void rilc_thread_pool() {
#ifdef __RADIO_HAL_AIDL__
    android::hardware::radio::impl::joinThreadPool();
#endif
}
