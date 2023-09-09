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
#include "messaging/hook/structs.h"
#include "commonStructs.h"
#include "collections.h"
#include <aidl/android/hardware/radio/RadioTechnology.h>

using namespace ::android::hardware::radio;
using namespace ::aidl::android::hardware::radio;
namespace aidl_messaging = ::aidl::android::hardware::radio::messaging;
using namespace ::android::hardware::radio;
using ::aidl::android::hardware::radio::RadioTechnology;

TEST(HookStructs, CdmaSmsAck_toHidl) {
    aidl_messaging::CdmaSmsAck testVector = {
        false, 0
    };
    auto test = compat::toHidl(testVector);
    EXPECT_TRUE(test.errorClass == (V1_0::CdmaSmsErrorClass)testVector.errorClass);
    EXPECT_TRUE(test.smsCauseCode == testVector.smsCauseCode);
}

TEST(HookStructs, CdmaSmsMessage_toAidl) {

    V1_0::CdmaSmsAddress cdmaSmsAddress = {
        V1_0::CdmaSmsDigitMode::FOUR_BIT,
        V1_0::CdmaSmsNumberMode::NOT_DATA_NETWORK,
        V1_0::CdmaSmsNumberType::UNKNOWN,
        V1_0::CdmaSmsNumberPlan::UNKNOWN,
        {}
    };
    V1_0::CdmaSmsSubaddress cdmaSmsSubaddress = {
        V1_0::CdmaSmsSubaddressType(0),
        false,
        {}
    };
    V1_0::CdmaSmsMessage testVector = {
        4098, false, 0,
        cdmaSmsAddress,
        cdmaSmsSubaddress,
        (std::vector<uint8_t>){15, 0, 3, 32, 3, 16, 1, 8, 16, 53, 76, 68, 6, 51, 106, 0}
    };
    auto test = compat::toAidl(testVector);
    EXPECT_TRUE(test.teleserviceId == static_cast<int32_t>(testVector.teleserviceId));
    EXPECT_TRUE(test.isServicePresent == testVector.isServicePresent);
    EXPECT_TRUE(test.serviceCategory == static_cast<int32_t>(testVector.serviceCategory));
}

TEST(HookStructs, CdmaSmsMessage_toHidl) {
    aidl_messaging::CdmaSmsAddress cdmaSmsAddress = {
        0,
        false,
        0,
        0,
        {}
    };
    aidl_messaging::CdmaSmsSubaddress cdmaSmsSubaddress = {
        0,
        false,
        {}
    };
    aidl_messaging::CdmaSmsMessage testVector = {
        4098, false, 0,
        cdmaSmsAddress,
        cdmaSmsSubaddress,
        (std::vector<uint8_t>){15, 0, 3, 32, 3, 16, 1, 8, 16, 53, 76, 68, 6, 51, 106, 0}
    };

    auto test = compat::toHidl(testVector);
    EXPECT_TRUE(test.teleserviceId == testVector.teleserviceId);
    EXPECT_TRUE(test.isServicePresent == testVector.isServicePresent);
    EXPECT_TRUE(test.serviceCategory == testVector.serviceCategory);
}

TEST(HookStructs, ImsSmsMessage_toHidl) {
    aidl_messaging::ImsSmsMessage testVector = {
        RadioTechnologyFamily::THREE_GPP2,
        false,
        0,
        {},
        {}
    };
    auto test = compat::toHidl(testVector);
    EXPECT_TRUE(test.retry == testVector.retry);
    EXPECT_TRUE(test.messageRef == testVector.messageRef);
}

TEST(HookStructs, GsmSmsMessage_toHidl) {
    aidl_messaging::GsmSmsMessage testVector = {
        "821012345", "01234567890ABCDEF"
    };
    auto test = compat::toHidl(testVector);
    EXPECT_TRUE(test.smscPdu == testVector.smscPdu);
    EXPECT_TRUE(test.pdu == testVector.pdu);
}

TEST(HookStructs, CdmaBroadcastSmsConfigInfo_toAidl) {
    V1_0::CdmaBroadcastSmsConfigInfo testVector = {
        0, 0, false
    };
    auto test = compat::toAidl(testVector);
    EXPECT_TRUE(test.serviceCategory == testVector.serviceCategory);
    EXPECT_TRUE(test.language == testVector.language);
    EXPECT_TRUE(test.selected == testVector.selected);
}

TEST(HookStructs, CdmaBroadcastSmsConfigInfo_toHidl) {
    aidl_messaging::CdmaBroadcastSmsConfigInfo testVector = {
        0, 0, false
    };
    auto test = compat::toHidl(testVector);
    EXPECT_TRUE(test.serviceCategory == testVector.serviceCategory);
    EXPECT_TRUE(test.language == testVector.language);
    EXPECT_TRUE(test.selected == testVector.selected);
}

TEST(HookStructs, GsmBroadcastSmsConfigInfo_toAidl) {
    V1_0::GsmBroadcastSmsConfigInfo testVector = {
        0, 0, 0, 0, false
    };
    auto test = compat::toAidl(testVector);
    EXPECT_TRUE(test.fromServiceId == testVector.fromServiceId);
    EXPECT_TRUE(test.toServiceId == testVector.toServiceId);
    EXPECT_TRUE(test.fromCodeScheme == testVector.fromCodeScheme);
    EXPECT_TRUE(test.toCodeScheme == testVector.toCodeScheme);
    EXPECT_TRUE(test.selected == testVector.selected);
}

TEST(HookStructs, GsmBroadcastSmsConfigInfo_toHidl) {
    aidl_messaging::GsmBroadcastSmsConfigInfo testVector = {
        0, 0, 0, 0, false
    };
    auto test = compat::toHidl(testVector);
    EXPECT_TRUE(test.fromServiceId == testVector.fromServiceId);
    EXPECT_TRUE(test.toServiceId == testVector.toServiceId);
    EXPECT_TRUE(test.fromCodeScheme == testVector.fromCodeScheme);
    EXPECT_TRUE(test.toCodeScheme == testVector.toCodeScheme);
    EXPECT_TRUE(test.selected == testVector.selected);
}

TEST(HookStructs, CdmaSmsWriteArgs_toHidl) {
    aidl_messaging::CdmaSmsWriteArgs testVector = {
        0, {}
    };
    auto test = compat::toHidl(testVector);
    EXPECT_TRUE(test.status == V1_0::CdmaSmsWriteArgsStatus(testVector.status));
}

TEST(HookStructs, SmsWriteArgs_toHidl) {
    aidl_messaging::SmsWriteArgs testVector = {
        0, "", "01000b916105770203f3000006d4f29c3e9b01"
    };
    auto test = compat::toHidl(testVector);
    EXPECT_TRUE(test.status == V1_0::SmsWriteArgsStatus(testVector.status));
    EXPECT_TRUE(test.pdu == testVector.pdu);
    EXPECT_TRUE(test.smsc == testVector.smsc);
}

TEST(HookStructs, SendSmsResult_toAidl) {
    V1_0::SendSmsResult testVector = {
        0, "", 0
    };
    auto test = compat::toAidl(testVector);
    EXPECT_TRUE(test.messageRef == testVector.messageRef);
    EXPECT_TRUE(test.ackPDU == testVector.ackPDU);
    EXPECT_TRUE(test.errorCode == testVector.errorCode);
}
