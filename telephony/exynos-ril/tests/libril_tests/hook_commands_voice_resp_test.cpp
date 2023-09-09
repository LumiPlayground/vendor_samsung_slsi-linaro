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
#include "commandsinfo.h"
#include "hookcommandstestsetup.h"
#include <slsi/radio_v2_0.h>
#include "radiovoiceresponse.h"
#include "radiocallbackadapter.h"

using namespace android::hardware::radio::impl;
namespace aidl_common = ::aidl::android::hardware::radio;
namespace aidl_voice = ::aidl::android::hardware::radio::voice;

static HookCommandsTestSetup<RadioVoiceResponse, RadioVoiceIndication> sTestStub;

TEST(VoiceCommandsRespHook, acceptCallResponse) {
    struct TestResponseCb : public RadioVoiceResponseCallbackAdapter {
        TestChecker checker;
        void acceptCallResponse(const aidl_common::RadioResponseInfo& /* info */) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioVoiceResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioVoiceIndicationCallback>(new RadioVoiceIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_ANSWER);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}

TEST(VoiceCommandsRespHook, cancelPendingUssdResponse) {
    struct TestResponseCb : public RadioVoiceResponseCallbackAdapter {
        TestChecker checker;
        void cancelPendingUssdResponse(const aidl_common::RadioResponseInfo& /* info */) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioVoiceResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioVoiceIndicationCallback>(new RadioVoiceIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_CANCEL_USSD);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}

TEST(VoiceCommandsRespHook, conferenceResponse) {
    struct TestResponseCb : public RadioVoiceResponseCallbackAdapter {
        TestChecker checker;
        void conferenceResponse(const aidl_common::RadioResponseInfo& /* info */) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioVoiceResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioVoiceIndicationCallback>(new RadioVoiceIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_CONFERENCE);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}

TEST(VoiceCommandsRespHook, dialResponse) {
    struct TestResponseCb : public RadioVoiceResponseCallbackAdapter {
        TestChecker checker;
        void dialResponse(const aidl_common::RadioResponseInfo& /* info */) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioVoiceResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioVoiceIndicationCallback>(new RadioVoiceIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_DIAL);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}

TEST(VoiceCommandsRespHook, emergencyDialResponse) {
    struct TestResponseCb : public RadioVoiceResponseCallbackAdapter {
        TestChecker checker;
        void emergencyDialResponse(const aidl_common::RadioResponseInfo& /* info */) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioVoiceResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioVoiceIndicationCallback>(new RadioVoiceIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_EMERGENCY_DIAL);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}

TEST(VoiceCommandsRespHook, exitEmergencyCallbackModeResponse) {
    struct TestResponseCb : public RadioVoiceResponseCallbackAdapter {
        TestChecker checker;
        void exitEmergencyCallbackModeResponse(const aidl_common::RadioResponseInfo& /* info */) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioVoiceResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioVoiceIndicationCallback>(new RadioVoiceIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_EXIT_EMERGENCY_CALLBACK_MODE);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}

TEST(VoiceCommandsRespHook, explicitCallTransferResponse) {
    struct TestResponseCb : public RadioVoiceResponseCallbackAdapter {
        TestChecker checker;
        void explicitCallTransferResponse(const aidl_common::RadioResponseInfo& /* info */) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioVoiceResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioVoiceIndicationCallback>(new RadioVoiceIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_EXPLICIT_CALL_TRANSFER);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}

TEST(VoiceCommandsRespHook, getCallForwardStatusResponse) {
    static RIL_CallForwardInfo *testVector[] = {
        new RIL_CallForwardInfo { 1, 0, 1, 91, (char *)"12345", 10 },
        new RIL_CallForwardInfo { 1, 1, 1, 91, (char *)"54321", 10 },
    };
    struct TestResponseCb : public RadioVoiceResponseCallbackAdapter {
        TestChecker checker;
        void getCallForwardStatusResponse(const aidl_common::RadioResponseInfo& /* info */,
                const std::vector<aidl_voice::CallForwardInfo>& callForwardInfos) override {
            checker.checked();
            for (size_t i = 0; i < callForwardInfos.size(); i++) {
                EXPECT_TRUE(callForwardInfos[i].status == testVector[i]->status);
                EXPECT_TRUE(callForwardInfos[i].reason == testVector[i]->reason);
                EXPECT_TRUE(callForwardInfos[i].serviceClass == testVector[i]->serviceClass);
                EXPECT_TRUE(callForwardInfos[i].toa == testVector[i]->toa);
                EXPECT_TRUE(callForwardInfos[i].number == testVector[i]->number);
                EXPECT_TRUE(callForwardInfos[i].timeSeconds == testVector[i]->timeSeconds);
            }
        }
    };
    auto responseCb = std::shared_ptr<RadioVoiceResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioVoiceIndicationCallback>(new RadioVoiceIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_QUERY_CALL_FORWARD_STATUS);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, &testVector, sizeof(testVector));
    sTestStub.teardown();
    static size_t size = sizeof(testVector) / sizeof(testVector[0]);
    for (size_t i = 0; i < size; i++) {
        delete testVector[i];
    }
}

TEST(VoiceCommandsRespHook, getCallWaitingResponse) {
    static int32_t testVector[] = {1, 1};
    struct TestResponseCb : public RadioVoiceResponseCallbackAdapter {
        TestChecker checker;
        void getCallWaitingResponse(const aidl_common::RadioResponseInfo& /* info */,
                bool enable, int32_t serviceClass) override {
            checker.checked();
            EXPECT_TRUE(enable == static_cast<bool>(testVector[0]));
            EXPECT_TRUE(serviceClass == testVector[1]);
        }
    };
    auto responseCb = std::shared_ptr<RadioVoiceResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioVoiceIndicationCallback>(new RadioVoiceIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_QUERY_CALL_WAITING);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, testVector, sizeof(testVector));
    sTestStub.teardown();
}

TEST(VoiceCommandsRespHook, getClipResponse) {
    static int32_t testVector = 0;
    struct TestResponseCb : public RadioVoiceResponseCallbackAdapter {
        TestChecker checker;
        void getClipResponse(const aidl_common::RadioResponseInfo& /* info */,
                aidl_voice::ClipStatus status) override {
            checker.checked();
            EXPECT_TRUE(static_cast<int32_t>(status) == testVector);
        }
    };
    auto responseCb = std::shared_ptr<RadioVoiceResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioVoiceIndicationCallback>(new RadioVoiceIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_QUERY_CLIP);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, &testVector, sizeof(testVector));
    sTestStub.teardown();
}

TEST(VoiceCommandsRespHook, getClirResponse) {
    static int32_t testVector[] = {0, 0};
    struct TestResponseCb : public RadioVoiceResponseCallbackAdapter {
        TestChecker checker;
        void getClirResponse(const aidl_common::RadioResponseInfo& /* info */,
                int32_t n, int32_t m) override {
            checker.checked();
            EXPECT_TRUE(n == testVector[0]);
            EXPECT_TRUE(m == testVector[1]);
        }
    };
    auto responseCb = std::shared_ptr<RadioVoiceResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioVoiceIndicationCallback>(new RadioVoiceIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_GET_CLIR);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, testVector, sizeof(testVector));
    sTestStub.teardown();
}

TEST(VoiceCommandsRespHook, getCurrentCallsResponse_1_6) {
    static RIL_UUS_Info uusInfo[] = {{RIL_UUS_TYPE1_IMPLICIT, RIL_UUS_DCS_USP, 0, (char *)""}};
    static RIL_Call_V1_6 *testVector[] = {
        new RIL_Call_V1_6 { RIL_CALL_ACTIVE, 1, 91, 0, 0, 0, 1, 0, (char *)"12345", 0, (char *)"", 0, uusInfo,
            RIL_AudioQuality::UNSPECIFIED, (char *)"" },
        new RIL_Call_V1_6 { RIL_CALL_HOLDING, 2, 91, 0, 0, 0, 1, 0, (char *)"12345", 0, (char *)"", 0, uusInfo,
            RIL_AudioQuality::UNSPECIFIED, (char *)"" },
    };
    struct TestResponseCb : public RadioVoiceResponseCallbackAdapter {
        TestChecker checker;
        void getCurrentCallsResponse(const aidl_common::RadioResponseInfo& /* info */,
                const std::vector<aidl_voice::Call>& calls) override {
            checker.checked();
            for (size_t i = 0; i < calls.size(); i++) {
                EXPECT_TRUE(calls[i].state == testVector[i]->state);
                EXPECT_TRUE(calls[i].index == testVector[i]->index);
                EXPECT_TRUE(calls[i].toa == testVector[i]->toa);
                EXPECT_TRUE(calls[i].isMpty == testVector[i]->isMpty);
                EXPECT_TRUE(calls[i].isMT == testVector[i]->isMT);
                EXPECT_TRUE(calls[i].als == testVector[i]->als);
                EXPECT_TRUE(calls[i].isVoice == testVector[i]->isVoice);
                EXPECT_TRUE(calls[i].isVoicePrivacy == testVector[i]->isVoicePrivacy);
                EXPECT_TRUE(calls[i].number == testVector[i]->number);
                EXPECT_TRUE(calls[i].numberPresentation == testVector[i]->numberPresentation);
                EXPECT_TRUE(calls[i].name == testVector[i]->name);
                EXPECT_TRUE(calls[i].namePresentation == testVector[i]->namePresentation);
                EXPECT_TRUE(calls[i].uusInfo[0].uusType == testVector[i]->uusInfo[0].uusType);
                EXPECT_TRUE(calls[i].uusInfo[0].uusDcs == testVector[i]->uusInfo[0].uusDcs);
                EXPECT_TRUE(static_cast<int32_t>(calls[i].audioQuality)
                        == static_cast<int32_t>(testVector[i]->audioQuality));
                EXPECT_TRUE(calls[i].forwardedNumber == testVector[i]->forwardedNumber);
            }
        }
    };
    auto responseCb = std::shared_ptr<RadioVoiceResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioVoiceIndicationCallback>(new RadioVoiceIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, ENCODE_REQUEST(RIL_REQUEST_GET_CURRENT_CALLS, HAL_VERSION_CODE(1,6)));
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, testVector, sizeof(testVector));
    sTestStub.teardown();
    static size_t size = sizeof(testVector) / sizeof(testVector[0]);
    for (size_t i = 0; i < size; i++) {
        delete testVector[i];
    }
}

TEST(VoiceCommandsRespHook, getLastCallFailCauseResponse) {
    static RIL_LastCallFailCauseInfo testVector = {
        RIL_LastCallFailCause::CALL_FAIL_NORMAL, (char *)"",
    };
    struct TestResponseCb : public RadioVoiceResponseCallbackAdapter {
        TestChecker checker;
        void getLastCallFailCauseResponse(const aidl_common::RadioResponseInfo& /* info */,
                const aidl_voice::LastCallFailCauseInfo& failCauseInfo) override {
            checker.checked();
            EXPECT_TRUE(failCauseInfo.causeCode == aidl_voice::LastCallFailCause(testVector.cause_code));
            EXPECT_TRUE(failCauseInfo.vendorCause == testVector.vendor_cause);
        }
    };
    auto responseCb = std::shared_ptr<RadioVoiceResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioVoiceIndicationCallback>(new RadioVoiceIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_LAST_CALL_FAIL_CAUSE);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, &testVector, sizeof(testVector));
    sTestStub.teardown();
}

TEST(VoiceCommandsRespHook, getMuteResponse) {
    static int testVector = 1;
    struct TestResponseCb : public RadioVoiceResponseCallbackAdapter {
        TestChecker checker;
        void getMuteResponse(const aidl_common::RadioResponseInfo& /* info */, bool enable) override {
            checker.checked();
            EXPECT_TRUE(enable == static_cast<bool>(testVector));
        }
    };
    auto responseCb = std::shared_ptr<RadioVoiceResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioVoiceIndicationCallback>(new RadioVoiceIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_GET_MUTE);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, &testVector, sizeof(testVector));
    sTestStub.teardown();
}

TEST(VoiceCommandsRespHook, getPreferredVoicePrivacyResponse) {
    static int testVector = 1;
    struct TestResponseCb : public RadioVoiceResponseCallbackAdapter {
        TestChecker checker;
        void getPreferredVoicePrivacyResponse(const aidl_common::RadioResponseInfo& /* info */,
                bool enable) override {
            checker.checked();
            EXPECT_TRUE(enable == static_cast<bool>(testVector));
        }
    };
    auto responseCb = std::shared_ptr<RadioVoiceResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioVoiceIndicationCallback>(new RadioVoiceIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_CDMA_QUERY_PREFERRED_VOICE_PRIVACY_MODE);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, &testVector, sizeof(testVector));
    sTestStub.teardown();
}

TEST(VoiceCommandsRespHook, getTTYModeResponse) {
    static int testVector = 0;
    struct TestResponseCb : public RadioVoiceResponseCallbackAdapter {
        TestChecker checker;
        void getTtyModeResponse(const aidl_common::RadioResponseInfo& /* info */,
                aidl_voice::TtyMode mode) override {
            checker.checked();
            EXPECT_TRUE(mode == aidl_voice::TtyMode(testVector));
        }
    };
    auto responseCb = std::shared_ptr<RadioVoiceResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioVoiceIndicationCallback>(new RadioVoiceIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_QUERY_TTY_MODE);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, &testVector, sizeof(testVector));
    sTestStub.teardown();
}

TEST(VoiceCommandsRespHook, handleStkCallSetupRequestFromSimResponse) {
    struct TestResponseCb : public RadioVoiceResponseCallbackAdapter {
        TestChecker checker;
        void handleStkCallSetupRequestFromSimResponse(const aidl_common::RadioResponseInfo& /* info */)
            override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioVoiceResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioVoiceIndicationCallback>(new RadioVoiceIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_STK_HANDLE_CALL_SETUP_REQUESTED_FROM_SIM);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}

TEST(VoiceCommandsRespHook, hangupConnectionResponse) {
    struct TestResponseCb : public RadioVoiceResponseCallbackAdapter {
        TestChecker checker;
        void hangupConnectionResponse(const aidl_common::RadioResponseInfo& /* info */) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioVoiceResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioVoiceIndicationCallback>(new RadioVoiceIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_HANGUP);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}

TEST(VoiceCommandsRespHook, hangupForegroundResumeBackgroundResponse) {
    struct TestResponseCb : public RadioVoiceResponseCallbackAdapter {
        TestChecker checker;
        void hangupForegroundResumeBackgroundResponse(const aidl_common::RadioResponseInfo& /* info */)
            override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioVoiceResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioVoiceIndicationCallback>(new RadioVoiceIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_HANGUP_FOREGROUND_RESUME_BACKGROUND);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}

TEST(VoiceCommandsRespHook, hangupWaitingOrBackgroundResponse) {
    struct TestResponseCb : public RadioVoiceResponseCallbackAdapter {
        TestChecker checker;
        void hangupWaitingOrBackgroundResponse(const aidl_common::RadioResponseInfo& /* info */) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioVoiceResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioVoiceIndicationCallback>(new RadioVoiceIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_HANGUP_WAITING_OR_BACKGROUND);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}

TEST(VoiceCommandsRespHook, rejectCallResponse) {
    struct TestResponseCb : public RadioVoiceResponseCallbackAdapter {
        TestChecker checker;
        void rejectCallResponse(const aidl_common::RadioResponseInfo& /* info */) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioVoiceResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioVoiceIndicationCallback>(new RadioVoiceIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_UDUB);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}

TEST(VoiceCommandsRespHook, sendBurstDtmfResponse) {
    struct TestResponseCb : public RadioVoiceResponseCallbackAdapter {
        TestChecker checker;
        void sendBurstDtmfResponse(const aidl_common::RadioResponseInfo& /* info */) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioVoiceResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioVoiceIndicationCallback>(new RadioVoiceIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_CDMA_BURST_DTMF);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}

TEST(VoiceCommandsRespHook, sendCDMAFeatureCodeResponse) {
    struct TestResponseCb : public RadioVoiceResponseCallbackAdapter {
        TestChecker checker;
        void sendCdmaFeatureCodeResponse(const aidl_common::RadioResponseInfo& /* info */) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioVoiceResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioVoiceIndicationCallback>(new RadioVoiceIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_CDMA_FLASH);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}

TEST(VoiceCommandsRespHook, sendDtmfResponse) {
    struct TestResponseCb : public RadioVoiceResponseCallbackAdapter {
        TestChecker checker;
        void sendDtmfResponse(const aidl_common::RadioResponseInfo& /* info */) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioVoiceResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioVoiceIndicationCallback>(new RadioVoiceIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_DTMF);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}

TEST(VoiceCommandsRespHook, sendUssdResponse) {
    struct TestResponseCb : public RadioVoiceResponseCallbackAdapter {
        TestChecker checker;
        void sendUssdResponse(const aidl_common::RadioResponseInfo& /* info */) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioVoiceResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioVoiceIndicationCallback>(new RadioVoiceIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_SEND_USSD);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}

TEST(VoiceCommandsRespHook, separateConnectionResponse) {
    struct TestResponseCb : public RadioVoiceResponseCallbackAdapter {
        TestChecker checker;
        void separateConnectionResponse(const aidl_common::RadioResponseInfo& /* info */) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioVoiceResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioVoiceIndicationCallback>(new RadioVoiceIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_SEPARATE_CONNECTION);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}

TEST(VoiceCommandsRespHook, setCallForwardResponse) {
    struct TestResponseCb : public RadioVoiceResponseCallbackAdapter {
        TestChecker checker;
        void setCallForwardResponse(const aidl_common::RadioResponseInfo& /* info */) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioVoiceResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioVoiceIndicationCallback>(new RadioVoiceIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_SET_CALL_FORWARD);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}

TEST(VoiceCommandsRespHook, setCallWaitingResponse) {
    struct TestResponseCb : public RadioVoiceResponseCallbackAdapter {
        TestChecker checker;
        void setCallWaitingResponse(const aidl_common::RadioResponseInfo& /* info */) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioVoiceResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioVoiceIndicationCallback>(new RadioVoiceIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_SET_CALL_WAITING);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}

TEST(VoiceCommandsRespHook, setClirResponse) {
    struct TestResponseCb : public RadioVoiceResponseCallbackAdapter {
        TestChecker checker;
        void setClirResponse(const aidl_common::RadioResponseInfo& /* info */) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioVoiceResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioVoiceIndicationCallback>(new RadioVoiceIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_SET_CLIR);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}

TEST(VoiceCommandsRespHook, setMuteResponse) {
    struct TestResponseCb : public RadioVoiceResponseCallbackAdapter {
        TestChecker checker;
        void setMuteResponse(const aidl_common::RadioResponseInfo& /* info */) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioVoiceResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioVoiceIndicationCallback>(new RadioVoiceIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_SET_MUTE);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}

TEST(VoiceCommandsRespHook, setPreferredVoicePrivacyResponse) {
    struct TestResponseCb : public RadioVoiceResponseCallbackAdapter {
        TestChecker checker;
        void setPreferredVoicePrivacyResponse(const aidl_common::RadioResponseInfo& /* info */) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioVoiceResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioVoiceIndicationCallback>(new RadioVoiceIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_CDMA_SET_PREFERRED_VOICE_PRIVACY_MODE);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}

TEST(VoiceCommandsRespHook, setTTYModeResponse) {
    struct TestResponseCb : public RadioVoiceResponseCallbackAdapter {
        TestChecker checker;
        void setTtyModeResponse(const aidl_common::RadioResponseInfo& /* info */) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioVoiceResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioVoiceIndicationCallback>(new RadioVoiceIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_SET_TTY_MODE);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}

TEST(VoiceCommandsRespHook, startDtmfResponse) {
    struct TestResponseCb : public RadioVoiceResponseCallbackAdapter {
        TestChecker checker;
        void startDtmfResponse(const aidl_common::RadioResponseInfo& /* info */) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioVoiceResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioVoiceIndicationCallback>(new RadioVoiceIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_DTMF_START);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}

TEST(VoiceCommandsRespHook, stopDtmfResponse) {
    struct TestResponseCb : public RadioVoiceResponseCallbackAdapter {
        TestChecker checker;
        void stopDtmfResponse(const aidl_common::RadioResponseInfo& /* info */) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioVoiceResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioVoiceIndicationCallback>(new RadioVoiceIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_DTMF_STOP);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}

TEST(VoiceCommandsRespHook, switchWaitingOrHoldingAndActiveResponse) {
    struct TestResponseCb : public RadioVoiceResponseCallbackAdapter {
        TestChecker checker;
        void switchWaitingOrHoldingAndActiveResponse(const aidl_common::RadioResponseInfo& /* info */)
            override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioVoiceResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioVoiceIndicationCallback>(new RadioVoiceIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}
