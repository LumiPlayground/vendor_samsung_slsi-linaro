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
#include "voice/hook/structs.h"
#include "commonStructs.h"
#include "collections.h"

using namespace ::android::hardware::radio;
using namespace ::aidl::android::hardware::radio;
namespace aidl_voice = ::aidl::android::hardware::radio::voice;

TEST(HookStructs, Dial_toHidl) {
    aidl_voice::Dial testVector = {
        "12345", aidl_voice::Dial::CLIR_DEFAULT, {},
    };
    auto test = compat::toHidl(testVector);
    EXPECT_TRUE(test.address == testVector.address);
    EXPECT_TRUE(test.clir == V1_0::Clir{testVector.clir});
    EXPECT_TRUE(test.uusInfo == compat::toHidl(testVector.uusInfo));
}

TEST(HookStructs, UusInfo_toHidl) {
    aidl_voice::UusInfo testVector = {
        aidl_voice::UusInfo::UUS_TYPE_TYPE1_IMPLICIT, aidl_voice::UusInfo::UUS_DCS_USP, "",
    };
    auto test = compat::toHidl(testVector);
    EXPECT_TRUE(test.uusType == V1_0::UusType{testVector.uusType});
    EXPECT_TRUE(test.uusDcs == V1_0::UusDcs{testVector.uusDcs});
    EXPECT_TRUE(test.uusData == testVector.uusData);
}

TEST(HookStructs, CallForwardInfo_toAidl) {
    V1_0::CallForwardInfo testVector = {
        V1_0::CallForwardInfoStatus::ENABLE, 0, 1, 0, "12345", 0,
    };
    auto test = compat::toAidl(testVector);
    EXPECT_TRUE(test.status == static_cast<int32_t>(testVector.status));
    EXPECT_TRUE(test.reason == testVector.reason);
    EXPECT_TRUE(test.serviceClass == testVector.serviceClass);
    EXPECT_TRUE(test.toa == testVector.toa);
    EXPECT_TRUE(test.number == testVector.number);
    EXPECT_TRUE(test.timeSeconds == testVector.timeSeconds);
}

TEST(HookStructs, CallForwardInfo_toHidl) {
    aidl_voice::CallForwardInfo testVector = {
        aidl_voice::CallForwardInfo::STATUS_ENABLE, 0, 1, 0, "12345", 0,
    };
    auto test = compat::toHidl(testVector);
    EXPECT_TRUE(test.status == V1_0::CallForwardInfoStatus{testVector.status});
    EXPECT_TRUE(test.reason == testVector.reason);
    EXPECT_TRUE(test.serviceClass == testVector.serviceClass);
    EXPECT_TRUE(test.toa == testVector.toa);
    EXPECT_TRUE(test.number == testVector.number);
    EXPECT_TRUE(test.timeSeconds == testVector.timeSeconds);
}

TEST(HookStructs, CdmaSignalInfoRecord_toAidl) {
    V1_0::CdmaSignalInfoRecord testVector = {
        true, 1, 1, 1,
    };
    auto test = compat::toAidl(testVector);
    EXPECT_TRUE(test.isPresent == testVector.isPresent);
    EXPECT_TRUE(test.signalType == testVector.signalType);
    EXPECT_TRUE(test.alertPitch == testVector.alertPitch);
    EXPECT_TRUE(test.signal == testVector.signal);
}

TEST(HookStructs, CdmaCallWaiting_toAidl) {
    V1_0::CdmaSignalInfoRecord signalInfoRecord = {
        true, 1, 1, 1,
    };
    V1_0::CdmaCallWaiting testVector = {
        "12345", V1_0::CdmaCallWaitingNumberPresentation::ALLOWED, "", signalInfoRecord,
        V1_0::CdmaCallWaitingNumberType::UNKNOWN, V1_0::CdmaCallWaitingNumberPlan::UNKNOWN,
    };
    auto test = compat::toAidl(testVector);
    EXPECT_TRUE(test.number == testVector.number);
    EXPECT_TRUE(test.numberPresentation == static_cast<int32_t>(testVector.numberPresentation));
    EXPECT_TRUE(test.name == testVector.name);
    EXPECT_TRUE(test.numberType == static_cast<int32_t>(testVector.numberType));
    EXPECT_TRUE(test.numberPlan == static_cast<int32_t>(testVector.numberPlan));
}

TEST(HookStructs, CdmaInformationRecord_toAidl) {
    V1_0::CdmaInformationRecords testVector = {
        {
            { V1_0::CdmaInfoRecName::DISPLAY, {{"abc"}}, {}, {}, {}, {}, {}, {} },
            { V1_0::CdmaInfoRecName::SIGNAL, {}, {}, {{true, 1, 1, 1}}, {}, {}, {}, {} },
            { V1_0::CdmaInfoRecName::REDIRECTING_NUMBER, {}, {}, {},
                {{{}, V1_0::CdmaRedirectingReason::UNKNOWN}}, {}, {}, {} },
        },
    };
    for (size_t i = 0; i < testVector.infoRec.size(); i++) {
        auto test = compat::toAidl(testVector.infoRec[i]);
        EXPECT_TRUE(test.name == static_cast<int32_t>(testVector.infoRec[i].name));
        EXPECT_TRUE(test.display == compat::toAidl(testVector.infoRec[i].display));
        EXPECT_TRUE(test.number == compat::toAidl(testVector.infoRec[i].number));
        EXPECT_TRUE(test.signal == compat::toAidl(testVector.infoRec[i].signal));
        EXPECT_TRUE(test.redir == compat::toAidl(testVector.infoRec[i].redir));
        EXPECT_TRUE(test.clir == compat::toAidl(testVector.infoRec[i].clir));
        EXPECT_TRUE(test.audioCtrl == compat::toAidl(testVector.infoRec[i].audioCtrl));
    }
}

TEST(HookStructs, CdmaDisplayInfoRecord_toAidl) {
    V1_0::CdmaDisplayInfoRecord testVector = {
        "abc",
    };
    auto test = compat::toAidl(testVector);
    EXPECT_TRUE(test.alphaBuf == testVector.alphaBuf);
}

TEST(HookStructs, CdmaNumberInfoRecord_toAidl) {
    V1_0::CdmaNumberInfoRecord testVector = {
        "12345", 1, 1, 1, 1,
    };
    auto test = compat::toAidl(testVector);
    EXPECT_TRUE(test.number == testVector.number);
    EXPECT_TRUE(test.numberType == static_cast<int8_t>(testVector.numberType));
    EXPECT_TRUE(test.numberPlan == static_cast<int8_t>(testVector.numberPlan));
    EXPECT_TRUE(test.pi == static_cast<int8_t>(testVector.pi));
    EXPECT_TRUE(test.si == static_cast<int8_t>(testVector.si));
}

TEST(HookStructs, CdmaRedirectingNumberInfoRecord_toAidl) {
    V1_0::CdmaRedirectingNumberInfoRecord testVector = {
        {}, V1_0::CdmaRedirectingReason::UNKNOWN,
    };
    auto test = compat::toAidl(testVector);
    EXPECT_TRUE(test.redirectingNumber == compat::toAidl(testVector.redirectingNumber));
    EXPECT_TRUE(test.redirectingReason == static_cast<int32_t>(testVector.redirectingReason));
}

TEST(HookStructs, CdmaLineControlInfoRecord_toAidl) {
    V1_0::CdmaLineControlInfoRecord testVector = {
        1, 1, 1, 1,
    };
    auto test = compat::toAidl(testVector);
    EXPECT_TRUE(test.lineCtrlPolarityIncluded == static_cast<int8_t>(testVector.lineCtrlPolarityIncluded));
    EXPECT_TRUE(test.lineCtrlToggle == static_cast<int8_t>(testVector.lineCtrlToggle));
    EXPECT_TRUE(test.lineCtrlReverse == static_cast<int8_t>(testVector.lineCtrlReverse));
    EXPECT_TRUE(test.lineCtrlPowerDenial == static_cast<int8_t>(testVector.lineCtrlPowerDenial));
}

TEST(HookStructs, CdmaT53ClirInfoRecord_toAidl) {
    V1_0::CdmaT53ClirInfoRecord testVector = {
        1,
    };
    auto test = compat::toAidl(testVector);
    EXPECT_TRUE(test.cause == static_cast<int8_t>(testVector.cause));
}

TEST(HookStructs, CdmaT53AudioControlInfoRecord_toAidl) {
    V1_0::CdmaT53AudioControlInfoRecord testVector = {
        1, 1,
    };
    auto test = compat::toAidl(testVector);
    EXPECT_TRUE(test.upLink == static_cast<int8_t>(testVector.upLink));
    EXPECT_TRUE(test.downLink == static_cast<int8_t>(testVector.downLink));
}

TEST(HookStructs, EmergencyNumber_toAidl) {
    V1_4::EmergencyNumber testVector = {
        "119", "450", "03", 2, {}, 8,
    };
    auto test = compat::toAidl(testVector);
    EXPECT_TRUE(test.number == testVector.number);
    EXPECT_TRUE(test.mcc == testVector.mcc);
    EXPECT_TRUE(test.mnc == testVector.mnc);
    EXPECT_TRUE(test.categories == static_cast<int32_t>(testVector.categories));
    EXPECT_TRUE(test.urns == compat::toAidl(testVector.urns));
    EXPECT_TRUE(test.sources == static_cast<int32_t>(testVector.sources));
}

TEST(HookStructs, StkCcUnsolSsResult_toAidl) {
    V1_0::StkCcUnsolSsResult testVector = {
        V1_0::SsServiceType::CFU, V1_0::SsRequestType::ACTIVATION, V1_0::SsTeleserviceType::ALL_TELESEVICES,
        1, V1_0::RadioError::NONE, {}, {},
    };
    auto test = compat::toAidl(testVector);
    EXPECT_TRUE(test.serviceType == static_cast<int32_t>(testVector.serviceType));
    EXPECT_TRUE(test.requestType == static_cast<int32_t>(testVector.requestType));
    EXPECT_TRUE(test.teleserviceType == static_cast<int32_t>(testVector.teleserviceType));
    EXPECT_TRUE(test.serviceClass == static_cast<int32_t>(testVector.serviceClass));
    EXPECT_TRUE(static_cast<int32_t>(test.result) == static_cast<int32_t>(testVector.result));
    EXPECT_TRUE(test.ssInfo == compat::toAidl(testVector.ssInfo));
    EXPECT_TRUE(test.cfData == compat::toAidl(testVector.cfData));
}

TEST(HookStructs, SsInfoData_toAidl) {
    V1_0::SsInfoData testVector = {
        {},
    };
    auto test = compat::toAidl(testVector);
    EXPECT_TRUE(test.ssInfo == static_cast<std::vector<int32_t>>(testVector.ssInfo));
}

TEST(HookStructs, CfData_toAidl) {
    V1_0::CfData testVector = {
        {},
    };
    auto test = compat::toAidl(testVector);
    EXPECT_TRUE(test.cfInfo == compat::toAidl(testVector.cfInfo));
}

TEST(HookStructs, Call_V1_0_toAidl) {
    V1_0::Call testVector = {
        V1_0::CallState::ACTIVE, 1, 1, false, false, 0, true, false, "12345", V1_0::CallPresentation::ALLOWED,
        "", V1_0::CallPresentation::ALLOWED, {},
    };
    auto test = compat::toAidl(testVector);
    EXPECT_TRUE(test.state == static_cast<int32_t>(testVector.state));
    EXPECT_TRUE(test.index == testVector.index);
    EXPECT_TRUE(test.toa == testVector.toa);
    EXPECT_TRUE(test.isMpty == testVector.isMpty);
    EXPECT_TRUE(test.isMT == testVector.isMT);
    EXPECT_TRUE(test.als == static_cast<int8_t>(testVector.als));
    EXPECT_TRUE(test.isVoice == testVector.isVoice);
    EXPECT_TRUE(test.isVoicePrivacy == testVector.isVoicePrivacy);
    EXPECT_TRUE(test.number == testVector.number);
    EXPECT_TRUE(test.numberPresentation == static_cast<int32_t>(testVector.numberPresentation));
    EXPECT_TRUE(test.uusInfo == compat::toAidl(testVector.uusInfo));
}

TEST(HookStructs, Call_V1_2_toAidl) {
    V1_2::Call testVector = {
        {
            V1_0::CallState::ACTIVE, 1, 1, false, false, 0, true, false, "12345",
            V1_0::CallPresentation::ALLOWED, "", V1_0::CallPresentation::ALLOWED, {},
        },
        V1_2::AudioQuality::UNSPECIFIED,
    };
    auto test = compat::toAidl(testVector);
    EXPECT_TRUE(test.state == static_cast<int32_t>(testVector.base.state));
    EXPECT_TRUE(test.index == testVector.base.index);
    EXPECT_TRUE(test.toa == testVector.base.toa);
    EXPECT_TRUE(test.isMpty == testVector.base.isMpty);
    EXPECT_TRUE(test.isMT == testVector.base.isMT);
    EXPECT_TRUE(test.als == static_cast<int8_t>(testVector.base.als));
    EXPECT_TRUE(test.isVoice == testVector.base.isVoice);
    EXPECT_TRUE(test.isVoicePrivacy == testVector.base.isVoicePrivacy);
    EXPECT_TRUE(test.number == testVector.base.number);
    EXPECT_TRUE(test.numberPresentation == static_cast<int32_t>(testVector.base.numberPresentation));
    EXPECT_TRUE(test.uusInfo == compat::toAidl(testVector.base.uusInfo));
    EXPECT_TRUE(static_cast<int32_t>(test.audioQuality) == static_cast<int32_t>(testVector.audioQuality));
}

TEST(HookStructs, Call_V1_6_toAidl) {
    V1_6::Call testVector = {
        {
            {
                V1_0::CallState::ACTIVE, 1, 1, false, false, 0, true, false, "12345",
                V1_0::CallPresentation::ALLOWED, "", V1_0::CallPresentation::ALLOWED, {},
            },
            V1_2::AudioQuality::UNSPECIFIED,
        },
        "123",
    };
    auto test = compat::toAidl(testVector);
    EXPECT_TRUE(test.state == static_cast<int32_t>(testVector.base.base.state));
    EXPECT_TRUE(test.index == testVector.base.base.index);
    EXPECT_TRUE(test.toa == testVector.base.base.toa);
    EXPECT_TRUE(test.isMpty == testVector.base.base.isMpty);
    EXPECT_TRUE(test.isMT == testVector.base.base.isMT);
    EXPECT_TRUE(test.als == static_cast<int8_t>(testVector.base.base.als));
    EXPECT_TRUE(test.isVoice == testVector.base.base.isVoice);
    EXPECT_TRUE(test.isVoicePrivacy == testVector.base.base.isVoicePrivacy);
    EXPECT_TRUE(test.number == testVector.base.base.number);
    EXPECT_TRUE(test.numberPresentation == static_cast<int32_t>(testVector.base.base.numberPresentation));
    EXPECT_TRUE(test.uusInfo == compat::toAidl(testVector.base.base.uusInfo));
    EXPECT_TRUE(static_cast<int32_t>(test.audioQuality) == static_cast<int32_t>(testVector.base.audioQuality));
    EXPECT_TRUE(test.forwardedNumber == testVector.forwardedNumber);
}

TEST(HookStructs, UusInfo_toAidl) {
    V1_0::UusInfo testVector = {
        V1_0::UusType::TYPE1_IMPLICIT, V1_0::UusDcs::USP, "",
    };
    auto test = compat::toAidl(testVector);
    EXPECT_TRUE(test.uusType == static_cast<int32_t>(testVector.uusType));
    EXPECT_TRUE(test.uusDcs == static_cast<int32_t>(testVector.uusDcs));
    EXPECT_TRUE(test.uusData == testVector.uusData);
}

TEST(HookStructs, LastCallFailCauseInfo_toAidl) {
    V1_0::LastCallFailCauseInfo testVector = {
        V1_0::LastCallFailCause::NORMAL, "",
    };
    auto test = compat::toAidl(testVector);
    EXPECT_TRUE(static_cast<int32_t>(test.causeCode) == static_cast<int32_t>(testVector.causeCode));
    EXPECT_TRUE(test.vendorCause == testVector.vendorCause);
}
