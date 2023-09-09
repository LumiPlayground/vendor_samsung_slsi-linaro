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
#include <testutils.h>
#include <radiocallbackadapter.h>

using namespace android::hardware::radio::impl;
using ::aidl::android::hardware::radio::modem::DeviceStateType;
using ::aidl::android::hardware::radio::modem::NvItem;
using ::aidl::android::hardware::radio::modem::ResetNvType;

TEST(RadioCallbackAdapter, RadioModemCallbackAdapter) {
    TestChecker checker;
    auto radio = std::make_shared<RadioModemCallbackAdapter>();
    radio->setResponseFunctions(nullptr, nullptr);
    radio->enableModem(0, true);
    radio->getBasebandVersion(0);
    radio->getDeviceIdentity(0);
    radio->getHardwareConfig(0);
    radio->getModemActivityInfo(0);
    radio->getModemStackStatus(0);
    radio->getRadioCapability(0);
    radio->nvReadItem(0, NvItem::CDMA_MEID);
    radio->nvResetConfig(0, ResetNvType::FACTORY_RESET);
    radio->nvWriteCdmaPrl(0, {});
    radio->nvWriteItem(0, {});
    radio->requestShutdown(0);
    radio->responseAcknowledgement();
    radio->sendDeviceState(0, DeviceStateType::POWER_SAVE_MODE, true);
    radio->setRadioCapability(0, {});
    radio->setRadioPower(0, true, false, false);
    checker.set(true);
}

TEST(RadioCallbackAdapter, RadioModemResponseCallbackAdapter) {
    TestChecker checker;
    auto response = std::make_shared<RadioModemResponseCallbackAdapter>();
    response->setResponseFunction(nullptr);
    response->acknowledgeRequest(0);
    response->enableModemResponse({});
    response->getBasebandVersionResponse({}, "");
    response->getDeviceIdentityResponse({}, "", "", "", "");
    response->getHardwareConfigResponse({}, {});
    response->getModemActivityInfoResponse({}, {});
    response->getModemStackStatusResponse({}, true);
    response->getRadioCapabilityResponse({}, {});
    response->nvReadItemResponse({}, "");
    response->nvResetConfigResponse({});
    response->nvWriteCdmaPrlResponse({});
    response->nvWriteItemResponse({});
    response->requestShutdownResponse({});
    response->sendDeviceStateResponse({});
    response->setRadioCapabilityResponse({}, {});
    response->setRadioPowerResponse({});
    checker.set(true);
}


TEST(RadioCallbackAdapter, RadioModemIndicationCallbackAdapter) {
    TestChecker checker;
    auto indication = std::make_shared<RadioModemIndicationCallbackAdapter>();
    indication->setResponseFunction(nullptr);
    indication->hardwareConfigChanged({}, {});
    indication->modemReset({}, {});
    indication->radioCapabilityIndication({}, {});
    indication->radioStateChanged({}, {});
    indication->rilConnected({});
    checker.set(true);
}