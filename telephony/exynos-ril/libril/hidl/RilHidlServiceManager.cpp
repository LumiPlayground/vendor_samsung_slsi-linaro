/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
*/
/*
 * RilHidlServiceManager.cpp
 *
 *  Created on: 2020. 11. 19.
 *  Modified on: 2021. 5. 12.
 *      Author: sungwoo48.choi
 */
#define LOG_TAG "RILC"

#include "RilHidlServiceManager.h"
#include "ril_service_legacy.h"
#ifdef __RADIO_HAL_V1_6__
#include "ril_service_1_6.h"
#endif
#include "slsi/ril_oem_service.h"
#include <utils/Log.h>

#define REGISTER_VENDOR_RADIO 1

#ifdef DISABLE_VENDOR_RADIO
#undef REGISTER_VENDOR_RADIO
#define REGISTER_VENDOR_RADIO 0
#endif  // DISABLE_VENDOR_RADIO

RilHidlServiceManager *RilHidlServiceManager::instance = nullptr;

RilHidlServiceManager *RilHidlServiceManager::getInstance() {
    if (instance == nullptr) {
        instance = new RilHidlServiceManager();
    }
    return instance;
}

RilHidlServiceManager::RilHidlServiceManager() {
}

RilHidlServiceManager::~RilHidlServiceManager() {
}

void RilHidlServiceManager::init() {

#ifndef __RADIO_HAL_AIDL__
#ifdef __RADIO_HAL_V1_6__
    radio::impl::v1_6::registerAsService();
#else
    radio::impl::legacy::registerAsService();
#endif
#endif

#if REGISTER_VENDOR_RADIO
    vendor::radio::impl::legacy::registerAsService();
#endif  // REGISTER_VENDOR_RADIO
}

int RilHidlServiceManager::getCurrentRadioHalVersionCode() {
    return vendor::radio::getCurrentHalVersionCode();
}

int RilHidlServiceManager::getMaxRadioHalVersionCode() {
    return vendor::radio::getMaxHalVersionCode();
}
