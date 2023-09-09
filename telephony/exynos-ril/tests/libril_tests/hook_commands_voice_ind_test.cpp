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
#include <ril_internal.h>
#include "radiovoiceresponse.h"
#include "radiocallbackadapter.h"

using namespace android::hardware::radio::impl;
namespace aidl_common = ::aidl::android::hardware::radio;
namespace aidl_voice = ::aidl::android::hardware::radio::voice;

static HookCommandsTestSetup<RadioVoiceResponse, RadioVoiceIndication> sTestStub;

TEST(VoiceCommandsIndHook, callRing) {
    static RIL_CDMA_SignalInfoRecord testVector = {
        1, 1, 1, 1,
    };
    struct TestResponseCb : public RadioVoiceIndicationCallbackAdapter {
        TestChecker checker;
        void callRing(aidl_common::RadioIndicationType /* in_type */, bool isGsm,
                const aidl_voice::CdmaSignalInfoRecord& record) override {
            checker.checked();
            if (isGsm) {
                EXPECT_TRUE(record.isPresent == 0);
                EXPECT_TRUE(record.signalType == 0);
                EXPECT_TRUE(record.alertPitch == 0);
                EXPECT_TRUE(record.signal == 0);
            } else {
                EXPECT_TRUE(record.isPresent == testVector.isPresent);
                EXPECT_TRUE(record.signalType == testVector.signalType);
                EXPECT_TRUE(record.alertPitch == testVector.alertPitch);
                EXPECT_TRUE(record.signal == testVector.signal);
            }
        }
    };
    auto responseCb = std::shared_ptr<RadioVoiceResponseCallback>(new RadioVoiceResponseCallbackAdapter);
    auto indicationCb = std::shared_ptr<RadioVoiceIndicationCallback>(new TestResponseCb);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getUnsolRespInfo(0, RIL_UNSOL_CALL_RING);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0, RIL_E_SUCCESS, &testVector,
            sizeof(testVector));
    sTestStub.teardown();
}

TEST(VoiceCommandsIndHook, callStateChanged) {
    struct TestResponseCb : public RadioVoiceIndicationCallbackAdapter {
        TestChecker checker;
        void callStateChanged(aidl_common::RadioIndicationType /* in_type */) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioVoiceResponseCallback>(new RadioVoiceResponseCallbackAdapter);
    auto indicationCb = std::shared_ptr<RadioVoiceIndicationCallback>(new TestResponseCb);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getUnsolRespInfo(0, RIL_UNSOL_RESPONSE_CALL_STATE_CHANGED);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}

TEST(VoiceCommandsIndHook, cdmaCallWaiting) {
    static RIL_CDMA_SignalInfoRecord signalInfoRecord = {
        1, 1, 1, 1,
    };
    static RIL_CDMA_CallWaiting_v6 testVector = {
        (char *)"12345", 0, (char *)"", signalInfoRecord, 2, 0,
    };
    struct TestResponseCb : public RadioVoiceIndicationCallbackAdapter {
        TestChecker checker;
        void cdmaCallWaiting(aidl_common::RadioIndicationType /* in_type */,
                const aidl_voice::CdmaCallWaiting& callWaitingRecord) override {
            checker.checked();
            EXPECT_TRUE(callWaitingRecord.number == testVector.number);
            EXPECT_TRUE(callWaitingRecord.numberPresentation == testVector.numberPresentation);
            EXPECT_TRUE(callWaitingRecord.name == testVector.name);
            EXPECT_TRUE(callWaitingRecord.signalInfoRecord.isPresent
                    == testVector.signalInfoRecord.isPresent);
            EXPECT_TRUE(callWaitingRecord.signalInfoRecord.signalType
                    == testVector.signalInfoRecord.signalType);
            EXPECT_TRUE(callWaitingRecord.signalInfoRecord.alertPitch == testVector.signalInfoRecord.alertPitch);
            EXPECT_TRUE(callWaitingRecord.signalInfoRecord.signal == testVector.signalInfoRecord.signal);
            EXPECT_TRUE(callWaitingRecord.numberType == testVector.number_type);
            EXPECT_TRUE(callWaitingRecord.numberPlan == testVector.number_plan);
        }
    };
    auto responseCb = std::shared_ptr<RadioVoiceResponseCallback>(new RadioVoiceResponseCallbackAdapter);
    auto indicationCb = std::shared_ptr<RadioVoiceIndicationCallback>(new TestResponseCb);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getUnsolRespInfo(0, RIL_UNSOL_CDMA_CALL_WAITING);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0, RIL_E_SUCCESS, &testVector,
            sizeof(testVector));
    sTestStub.teardown();
}

TEST(VoiceCommandsIndHook, cdmaInfoRec) {
    static RIL_CDMA_InformationRecords testVector = {
        7,
        {
            {
                .name = RIL_CDMA_InfoRecName::RIL_CDMA_DISPLAY_INFO_REC,
                .rec.display = { 5, "12345" },
            },
            {
                .name = RIL_CDMA_InfoRecName::RIL_CDMA_CALLED_PARTY_NUMBER_INFO_REC,
                .rec.number = { 5, "12345", 1, 1, 0, 0 },
            },
            {
                .name = RIL_CDMA_InfoRecName::RIL_CDMA_SIGNAL_INFO_REC,
                .rec.signal = { 1, 1, 1, 1 },
            },
            {
                .name = RIL_CDMA_InfoRecName::RIL_CDMA_REDIRECTING_NUMBER_INFO_REC,
                .rec.redir = { { 5, "12345", 1, 1, 0, 0 },
                    RIL_CDMA_RedirectingReason::RIL_REDIRECTING_REASON_UNKNOWN },
            },
            {
                .name = RIL_CDMA_InfoRecName::RIL_CDMA_LINE_CONTROL_INFO_REC,
                .rec.lineCtrl = { 1, 2, 3, 4 },
            },
            {
                .name = RIL_CDMA_InfoRecName::RIL_CDMA_T53_CLIR_INFO_REC,
                .rec.clir = { 3 },
            },
            {
                .name = RIL_CDMA_InfoRecName::RIL_CDMA_T53_AUDIO_CONTROL_INFO_REC,
                .rec.audioCtrl = { 3, 7 },
            },
        }
    };
    struct TestResponseCb : public RadioVoiceIndicationCallbackAdapter {
        TestChecker checker;
        void cdmaInfoRec(aidl_common::RadioIndicationType /* in_type */,
                const std::vector<aidl_voice::CdmaInformationRecord>& records) override {
            checker.checked();
            for (size_t i = 0; i < records.size(); i++) {
                size_t infoNum = static_cast<size_t>(testVector.numberOfInfoRecs);
                for (size_t j = 0; j < infoNum; j++) {
                    if (records[i].name == testVector.infoRec[j].name) {
                        test(records[i], testVector.infoRec[j]);
                    }
                }
            }
        }
        void test(const aidl_voice::CdmaInformationRecord& record,
                const RIL_CDMA_InformationRecord& rilRecord) {
            EXPECT_TRUE(record.name == rilRecord.name);
            switch (record.name) {
                case aidl_voice::CdmaInformationRecord::NAME_DISPLAY:
                    test(record.display[0], rilRecord.rec.display);
                    break;
                case aidl_voice::CdmaInformationRecord::NAME_CALLED_PARTY_NUMBER:
                    test(record.number[0], rilRecord.rec.number);
                    break;
                case aidl_voice::CdmaInformationRecord::NAME_SIGNAL:
                    test(record.signal[0], rilRecord.rec.signal);
                    break;
                case aidl_voice::CdmaInformationRecord::NAME_REDIRECTING_NUMBER:
                    test(record.redir[0], rilRecord.rec.redir);
                    break;
                case aidl_voice::CdmaInformationRecord::NAME_LINE_CONTROL:
                    test(record.lineCtrl[0], rilRecord.rec.lineCtrl);
                    break;
                case aidl_voice::CdmaInformationRecord::NAME_T53_CLIR:
                    test(record.clir[0], rilRecord.rec.clir);
                    break;
                case aidl_voice::CdmaInformationRecord::NAME_T53_AUDIO_CONTROL:
                    test(record.audioCtrl[0], rilRecord.rec.audioCtrl);
                    break;
            }
        }
        void test(const aidl_voice::CdmaDisplayInfoRecord& record, const RIL_CDMA_DisplayInfoRecord& rilRecord) {
            EXPECT_TRUE(record.alphaBuf == rilRecord.alpha_buf);
        }
        void test(const aidl_voice::CdmaNumberInfoRecord& record, const RIL_CDMA_NumberInfoRecord& rilRecord) {
            EXPECT_TRUE(record.number == rilRecord.buf);
            EXPECT_TRUE(record.numberType == rilRecord.number_type);
            EXPECT_TRUE(record.numberPlan == rilRecord.number_plan);
            EXPECT_TRUE(record.pi == rilRecord.pi);
            EXPECT_TRUE(record.si == rilRecord.si);
        }
        void test(const aidl_voice::CdmaSignalInfoRecord& record, const RIL_CDMA_SignalInfoRecord& rilRecord) {
            EXPECT_TRUE(record.isPresent == rilRecord.isPresent);
            EXPECT_TRUE(record.signalType == rilRecord.signalType);
            EXPECT_TRUE(record.alertPitch == rilRecord.alertPitch);
            EXPECT_TRUE(record.signal == rilRecord.signal);
        }
        void test(const aidl_voice::CdmaRedirectingNumberInfoRecord& record,
                const RIL_CDMA_RedirectingNumberInfoRecord& rilRecord) {
            test(record.redirectingNumber, rilRecord.redirectingNumber);
            EXPECT_TRUE(record.redirectingReason == rilRecord.redirectingReason);
        }
        void test(const aidl_voice::CdmaLineControlInfoRecord& record,
                const RIL_CDMA_LineControlInfoRecord& rilRecord) {
            EXPECT_TRUE(record.lineCtrlPolarityIncluded == rilRecord.lineCtrlPolarityIncluded);
            EXPECT_TRUE(record.lineCtrlToggle == rilRecord.lineCtrlToggle);
            EXPECT_TRUE(record.lineCtrlReverse == rilRecord.lineCtrlReverse);
            EXPECT_TRUE(record.lineCtrlPowerDenial == rilRecord.lineCtrlPowerDenial);
        }
        void test(const aidl_voice::CdmaT53ClirInfoRecord& record,
                const RIL_CDMA_T53_CLIRInfoRecord& rilRecord) {
            EXPECT_TRUE(record.cause == rilRecord.cause);
        }
        void test(const aidl_voice::CdmaT53AudioControlInfoRecord& record,
                const RIL_CDMA_T53_AudioControlInfoRecord& rilRecord) {
            EXPECT_TRUE(record.upLink == rilRecord.upLink);
            EXPECT_TRUE(record.downLink == rilRecord.downLink);
        }
    };
    auto responseCb = std::shared_ptr<RadioVoiceResponseCallback>(new RadioVoiceResponseCallbackAdapter);
    auto indicationCb = std::shared_ptr<RadioVoiceIndicationCallback>(new TestResponseCb);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getUnsolRespInfo(0, RIL_UNSOL_CDMA_INFO_REC);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0, RIL_E_SUCCESS, &testVector,
            sizeof(testVector));
    sTestStub.teardown();
}

TEST(VoiceCommandsIndHook, cdmaOtaProvisionStatus) {
    static RIL_CDMA_OTA_ProvisionStatus testVector = CDMA_OTA_PROVISION_STATUS_SPL_UNLOCKED;
    struct TestResponseCb : public RadioVoiceIndicationCallbackAdapter {
        TestChecker checker;
        void cdmaOtaProvisionStatus(aidl_common::RadioIndicationType /* in_type */,
                aidl_voice::CdmaOtaProvisionStatus status) override {
            checker.checked();
            EXPECT_TRUE(static_cast<int32_t>(status) == static_cast<int32_t>(testVector));
        }
    };
    auto responseCb = std::shared_ptr<RadioVoiceResponseCallback>(new RadioVoiceResponseCallbackAdapter);
    auto indicationCb = std::shared_ptr<RadioVoiceIndicationCallback>(new TestResponseCb);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getUnsolRespInfo(0, RIL_UNSOL_CDMA_OTA_PROVISION_STATUS);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0, RIL_E_SUCCESS, &testVector,
            sizeof(testVector));
    sTestStub.teardown();
}

TEST(VoiceCommandsIndHook, currentEmergencyNumberList) {
    static RIL_EmergencyNumber testVector[] = {
        { (char *)"112", (char *)"001", (char *)"01",
            RIL_EmergencyServiceCategory::RIL_EMERGENCY_CATEGORY_POLICE, 0, nullptr,
            RIL_EmergencyNumberSource::RIL_EMERGENCY_NUMBER_SOURCE_DEFAULT },
        { (char *)"911", (char *)"001", (char *)"01",
            RIL_EmergencyServiceCategory::RIL_EMERGENCY_CATEGORY_AMBULANCE, 0, nullptr,
            RIL_EmergencyNumberSource::RIL_EMERGENCY_NUMBER_SOURCE_DEFAULT },
    };
    struct TestResponseCb : public RadioVoiceIndicationCallbackAdapter {
        TestChecker checker;
        void currentEmergencyNumberList(aidl_common::RadioIndicationType /* in_type */,
                const std::vector<aidl_voice::EmergencyNumber>& emergencyNumberList) override {
            checker.checked();
            for (size_t i = 0; i < emergencyNumberList.size(); i++) {
                EXPECT_TRUE(emergencyNumberList[i].number == testVector[i].number);
                EXPECT_TRUE(emergencyNumberList[i].mcc == testVector[i].mcc);
                EXPECT_TRUE(emergencyNumberList[i].mnc == testVector[i].mnc);
                EXPECT_TRUE(emergencyNumberList[i].categories == testVector[i].categories);
                EXPECT_TRUE(emergencyNumberList[i].sources == testVector[i].sources);
            }
        }
    };
    auto responseCb = std::shared_ptr<RadioVoiceResponseCallback>(new RadioVoiceResponseCallbackAdapter);
    auto indicationCb = std::shared_ptr<RadioVoiceIndicationCallback>(new TestResponseCb);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getUnsolRespInfo(0, RIL_UNSOL_EMERGENCY_NUMBER_LIST);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0, RIL_E_SUCCESS, testVector,
        sizeof(testVector));
    sTestStub.teardown();
}

TEST(VoiceCommandsIndHook, enterEmergencyCallbackMode) {
    struct TestResponseCb : public RadioVoiceIndicationCallbackAdapter {
        TestChecker checker;
        void enterEmergencyCallbackMode(aidl_common::RadioIndicationType /* in_type */) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioVoiceResponseCallback>(new RadioVoiceResponseCallbackAdapter);
    auto indicationCb = std::shared_ptr<RadioVoiceIndicationCallback>(new TestResponseCb);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getUnsolRespInfo(0, RIL_UNSOL_ENTER_EMERGENCY_CALLBACK_MODE);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}

TEST(VoiceCommandsIndHook, exitEmergencyCallbackMode) {
    struct TestResponseCb : public RadioVoiceIndicationCallbackAdapter {
        TestChecker checker;
        void exitEmergencyCallbackMode(aidl_common::RadioIndicationType /* in_type */) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioVoiceResponseCallback>(new RadioVoiceResponseCallbackAdapter);
    auto indicationCb = std::shared_ptr<RadioVoiceIndicationCallback>(new TestResponseCb);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getUnsolRespInfo(0, RIL_UNSOL_EXIT_EMERGENCY_CALLBACK_MODE);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}

TEST(VoiceCommandsIndHook, indicateRingbackTone) {
    static int testVector = true;
    struct TestResponseCb : public RadioVoiceIndicationCallbackAdapter {
        TestChecker checker;
        void indicateRingbackTone(aidl_common::RadioIndicationType /* in_type */, bool start) override {
            checker.checked();
            EXPECT_TRUE(start == static_cast<bool>(testVector));
        }
    };
    auto responseCb = std::shared_ptr<RadioVoiceResponseCallback>(new RadioVoiceResponseCallbackAdapter);
    auto indicationCb = std::shared_ptr<RadioVoiceIndicationCallback>(new TestResponseCb);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getUnsolRespInfo(0, RIL_UNSOL_RINGBACK_TONE);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0, RIL_E_SUCCESS, &testVector,
            sizeof(testVector));
    sTestStub.teardown();
}

TEST(VoiceCommandsIndHook, onSupplementaryServiceIndication) {
    static RIL_StkCcUnsolSsResponse testVector = {
        RIL_SsServiceType::SS_CFU, RIL_SsRequestType::SS_ACTIVATION, RIL_SsTeleserviceType::SS_ALL_TELESEVICES,
        1, RIL_Errno::RIL_E_SUCCESS, {},
    };
    struct TestResponseCb : public RadioVoiceIndicationCallbackAdapter {
        TestChecker checker;
        void onSupplementaryServiceIndication(aidl_common::RadioIndicationType /* in_type */,
                const aidl_voice::StkCcUnsolSsResult& ss) override {
            checker.checked();
            EXPECT_TRUE(ss.serviceType == testVector.serviceType);
            EXPECT_TRUE(ss.requestType == testVector.requestType);
            EXPECT_TRUE(ss.teleserviceType == testVector.teleserviceType);
            EXPECT_TRUE(ss.serviceClass == testVector.serviceClass);
            EXPECT_TRUE(static_cast<int32_t>(ss.result) == static_cast<int32_t>(testVector.result));
        }
    };
    auto responseCb = std::shared_ptr<RadioVoiceResponseCallback>(new RadioVoiceResponseCallbackAdapter);
    auto indicationCb = std::shared_ptr<RadioVoiceIndicationCallback>(new TestResponseCb);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getUnsolRespInfo(0, RIL_UNSOL_ON_SS);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0, RIL_E_SUCCESS, &testVector,
            sizeof(testVector));
    sTestStub.teardown();
}

TEST(VoiceCommandsIndHook, onUssd) {
    static const char *testVector[] = {
        "1", "ussd_ind_test",
    };
    struct TestResponseCb : public RadioVoiceIndicationCallbackAdapter {
        TestChecker checker;
        void onUssd(aidl_common::RadioIndicationType /* in_type */, aidl_voice::UssdModeType modeType,
                const std::string& msg) override {
            checker.checked();
            EXPECT_TRUE(static_cast<uint32_t>(modeType) == static_cast<uint32_t>(std::stoi(testVector[0])));
            EXPECT_TRUE(msg == testVector[1]);
        }
    };
    auto responseCb = std::shared_ptr<RadioVoiceResponseCallback>(new RadioVoiceResponseCallbackAdapter);
    auto indicationCb = std::shared_ptr<RadioVoiceIndicationCallback>(new TestResponseCb);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getUnsolRespInfo(0, RIL_UNSOL_ON_USSD);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0, RIL_E_SUCCESS, testVector,
            sizeof(testVector));
    sTestStub.teardown();
}

TEST(VoiceCommandsIndHook, resendIncallMute) {
    struct TestResponseCb : public RadioVoiceIndicationCallbackAdapter {
        TestChecker checker;
        void resendIncallMute(aidl_common::RadioIndicationType /* in_type */) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioVoiceResponseCallback>(new RadioVoiceResponseCallbackAdapter);
    auto indicationCb = std::shared_ptr<RadioVoiceIndicationCallback>(new TestResponseCb);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getUnsolRespInfo(0, RIL_UNSOL_RESEND_INCALL_MUTE);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}

TEST(VoiceCommandsIndHook, srvccStateNotify) {
    static RIL_SrvccState testVector = HANDOVER_STARTED;
    struct TestResponseCb : public RadioVoiceIndicationCallbackAdapter {
        TestChecker checker;
        void srvccStateNotify(aidl_common::RadioIndicationType /* in_type */,
                aidl_voice::SrvccState state) override {
            checker.checked();
            EXPECT_TRUE(static_cast<int32_t>(state) == static_cast<int32_t>(testVector));
        }
    };
    auto responseCb = std::shared_ptr<RadioVoiceResponseCallback>(new RadioVoiceResponseCallbackAdapter);
    auto indicationCb = std::shared_ptr<RadioVoiceIndicationCallback>(new TestResponseCb);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getUnsolRespInfo(0, RIL_UNSOL_SRVCC_STATE_NOTIFY);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0, RIL_E_SUCCESS, &testVector,
            sizeof(testVector));
    sTestStub.teardown();
}

TEST(VoiceCommandsIndHook, stkCallControlAlphaNotify) {
    static char testVector[] = "stkCallControlAlphaNotify";
    struct TestResponseCb : public RadioVoiceIndicationCallbackAdapter {
        TestChecker checker;
        void stkCallControlAlphaNotify(aidl_common::RadioIndicationType /* in_type */,
                const std::string& alpha) override {
            checker.checked();
            EXPECT_TRUE(alpha == testVector);
        }
    };
    auto responseCb = std::shared_ptr<RadioVoiceResponseCallback>(new RadioVoiceResponseCallbackAdapter);
    auto indicationCb = std::shared_ptr<RadioVoiceIndicationCallback>(new TestResponseCb);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getUnsolRespInfo(0, RIL_UNSOL_STK_CC_ALPHA_NOTIFY);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0, RIL_E_SUCCESS, testVector,
            sizeof(testVector));
    sTestStub.teardown();
}

TEST(VoiceCommandsIndHook, stkCallSetup) {
    static int32_t testVector = 1;
    struct TestResponseCb : public RadioVoiceIndicationCallbackAdapter {
        TestChecker checker;
        void stkCallSetup(aidl_common::RadioIndicationType /* in_type */, int64_t timeout) override {
            checker.checked();
            EXPECT_TRUE(timeout == static_cast<int64_t>(testVector));
        }
    };
    auto responseCb = std::shared_ptr<RadioVoiceResponseCallback>(new RadioVoiceResponseCallbackAdapter);
    auto indicationCb = std::shared_ptr<RadioVoiceIndicationCallback>(new TestResponseCb);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getUnsolRespInfo(0, RIL_UNSOL_STK_CALL_SETUP);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0, RIL_E_SUCCESS, &testVector,
            sizeof(testVector));
    sTestStub.teardown();
}
