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
#include <telephony/ril_mcc.h>
#include <telephony/ril_mnc.h>
#include <ril_internal.h>
#include "radionetworkresponse.h"
#include "radiocallbackadapter.h"

using namespace android::hardware::radio::impl;
namespace aidl_common = ::aidl::android::hardware::radio;
namespace aidl_network = ::aidl::android::hardware::radio::network;

static HookCommandsTestSetup<RadioNetworkResponse, RadioNetworkIndication> sTestStub;

static void testCellIdentityLte(const aidl_network::CellIdentityLte& lte, const RIL_CellIdentityLte_V1_5& rilLte) {
    EXPECT_TRUE(lte.mcc == ril::util::mcc::decode(rilLte.mcc));
    EXPECT_TRUE(lte.mnc == ril::util::mnc::decode(rilLte.mnc));
    EXPECT_EQ(lte.ci, rilLte.ci);
    EXPECT_EQ(lte.pci, rilLte.pci);
    EXPECT_EQ(lte.tac, rilLte.tac);
    EXPECT_EQ(lte.earfcn, rilLte.earfcn);
    EXPECT_TRUE(lte.operatorNames.alphaLong == rilLte.operatorNames.alphaLong);
    EXPECT_TRUE(lte.operatorNames.alphaShort == rilLte.operatorNames.alphaShort);
    EXPECT_EQ(lte.bandwidth, rilLte.bandwidth);
}

static void testCellIdentity(const aidl_network::CellIdentity& cellIdentity,
                const RIL_CellIdentity_V1_5& rilCellIdentity) {
    switch (cellIdentity.getTag()) {
    case aidl_network::CellIdentity::Tag::noinit:
        EXPECT_EQ(rilCellIdentity.cellInfoType, RIL_CellInfoType::RIL_CELL_INFO_TYPE_NONE);
        break;
    case aidl_network::CellIdentity::Tag::gsm:
        EXPECT_EQ(rilCellIdentity.cellInfoType, RIL_CellInfoType::RIL_CELL_INFO_TYPE_GSM);
        break;
    case aidl_network::CellIdentity::Tag::wcdma:
        EXPECT_EQ(rilCellIdentity.cellInfoType, RIL_CellInfoType::RIL_CELL_INFO_TYPE_WCDMA);
        break;
    case aidl_network::CellIdentity::Tag::tdscdma:
        EXPECT_EQ(rilCellIdentity.cellInfoType, RIL_CellInfoType::RIL_CELL_INFO_TYPE_TD_SCDMA);
        break;
    case aidl_network::CellIdentity::Tag::cdma:
        EXPECT_EQ(rilCellIdentity.cellInfoType, RIL_CellInfoType::RIL_CELL_INFO_TYPE_CDMA);
        break;
    case aidl_network::CellIdentity::Tag::lte:
        EXPECT_EQ(rilCellIdentity.cellInfoType, RIL_CellInfoType::RIL_CELL_INFO_TYPE_LTE);
        testCellIdentityLte(cellIdentity.get<aidl_network::CellIdentity::Tag::lte>(), rilCellIdentity.lte);
        break;
    case aidl_network::CellIdentity::Tag::nr:
        EXPECT_EQ((int)rilCellIdentity.cellInfoType, (int)RIL_CellInfoType_V1_4::RIL_CELL_INFO_TYPE_NR);
        break;
    default:
        ASSERT_TRUE(false);
        break;
    };
}

TEST(NetworkCommandsIndHook, barringInfoChanged) {
    RIL_BarringInfo testBarringInfo[] = {
        { EMERGENCY, BARRING_TYPE_UNCONDITIONAL, {{0, 0, true}} },
        { CS_SERVICE, BARRING_TYPE_UNCONDITIONAL, {{0, 0, true}} },
    };
    static RIL_CellIdAndBarringInfo testVector = {
        {
            .cellInfoType = RIL_CellInfoType::RIL_CELL_INFO_TYPE_LTE,
            .lte = {
                311, ril::util::mnc::encode(480, 3), 1000, 300, 35000, 1500,
                {"test long", "test short"}, 40, 0, 0, {}, 0, {}
            },
        },
        2, testBarringInfo,
    };
    struct TestIndicationCb : public RadioNetworkIndicationCallbackAdapter {
        TestChecker checker;
        void barringInfoChanged(aidl_common::RadioIndicationType /* type */,
                const aidl_network::CellIdentity& cellIdentity,
                const std::vector<aidl_network::BarringInfo>& barringInfos) override {
            checker.checked();
            testCellIdentity(cellIdentity, testVector.cellIdentity);
            EXPECT_TRUE(barringInfos.size() == static_cast<size_t>(testVector.numOfrecord));
            for (size_t i = 0; i < barringInfos.size(); i++) {
                test(barringInfos[i], testVector.precords[i]);
            }
        }

        void test(const aidl_network::BarringInfo& barringInfo, const RIL_BarringInfo& rilBarringInfo) {
            EXPECT_TRUE(barringInfo.serviceType == static_cast<int32_t>(rilBarringInfo.service));
            EXPECT_TRUE(barringInfo.barringType == static_cast<int32_t>(rilBarringInfo.type));

        }
    };
    auto responseCb = std::shared_ptr<RadioNetworkResponseCallback>(new RadioNetworkResponseCallbackAdapter);
    auto indicationCb = std::shared_ptr<RadioNetworkIndicationCallback>(new TestIndicationCb);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getUnsolRespInfo(0, RIL_UNSOL_BARRING_INFO_CHANGED);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0, RIL_E_SUCCESS, &testVector, sizeof(testVector));
    sTestStub.teardown();
}

TEST(NetworkCommandsIndHook, cdmaPrlChangedInd) {
    static int32_t testVector = 1;
    struct TestIndicationCb : public RadioNetworkIndicationCallbackAdapter {
        TestChecker checker;
        void cdmaPrlChanged(aidl_common::RadioIndicationType /* type */, int version) override {
            checker.checked();
            EXPECT_TRUE(version == testVector);
        }
    };
    auto responseCb = std::shared_ptr<RadioNetworkResponseCallback>(new RadioNetworkResponseCallbackAdapter);
    auto indicationCb = std::shared_ptr<RadioNetworkIndicationCallback>(new TestIndicationCb);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getUnsolRespInfo(0, RIL_UNSOL_CDMA_PRL_CHANGED);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0, RIL_E_SUCCESS, &testVector, sizeof(testVector));
    sTestStub.teardown();
}

TEST(NetworkCommandsIndHook, currentSignalStrengthInd_1_6) {
    static RIL_SignalStrength_V1_6 testVector = {
        { 60, 125, 0 },
        { 70, 125 },
        { 70, 125, 3 },
        { {15, 140, 10, -100, 0, 0x7FFFFFFE}, 1 },
        { 15, 3, 255 },
        { 15, 3, 255, 255 },
        { {44, 7, -10, 44, 7, -10}, 1, 3, {1, 2, 3} }
    };
    struct TestIndicationCb : public RadioNetworkIndicationCallbackAdapter {
        TestChecker checker;
        void currentSignalStrength(aidl_common::RadioIndicationType /* type */,
            const aidl_network::SignalStrength& signalStrength) {
            checker.checked();
            test(signalStrength.lte, testVector.LTE_SignalStrength);
        }

        void test(const aidl_network::LteSignalStrength& signalStrength,
                const RIL_LTE_SignalStrength_V1_6& rilSignalStrength) {
            EXPECT_TRUE(signalStrength.signalStrength == rilSignalStrength.signalStrength);
            EXPECT_TRUE(signalStrength.rsrp == rilSignalStrength.rsrp);
            EXPECT_TRUE(signalStrength.rsrq == rilSignalStrength.rsrq);
            EXPECT_TRUE(signalStrength.rssnr == rilSignalStrength.rssnr);
            EXPECT_TRUE(signalStrength.cqi == rilSignalStrength.cqi);
            EXPECT_TRUE(signalStrength.timingAdvance == rilSignalStrength.timingAdvance);
            EXPECT_TRUE(signalStrength.cqiTableIndex == static_cast<int32_t>(rilSignalStrength.cqiTableIndex));
        }
    };
    auto responseCb = std::shared_ptr<RadioNetworkResponseCallback>(new RadioNetworkResponseCallbackAdapter);
    auto indicationCb = std::shared_ptr<RadioNetworkIndicationCallback>(new TestIndicationCb);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getUnsolRespInfo(0, RIL_UNSOL_SIGNAL_STRENGTH);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0, RIL_E_SUCCESS, &testVector, sizeof(testVector));
    sTestStub.teardown();
}

TEST(NetworkCommandsIndHook, imsNetworkStateChanged) {
    struct TestIndicationCb : public RadioNetworkIndicationCallbackAdapter {
        TestChecker checker;
        void imsNetworkStateChanged(aidl_common::RadioIndicationType /* type */) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioNetworkResponseCallback>(new RadioNetworkResponseCallbackAdapter);
    auto indicationCb = std::shared_ptr<RadioNetworkIndicationCallback>(new TestIndicationCb);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getUnsolRespInfo(0, RIL_UNSOL_RESPONSE_IMS_NETWORK_STATE_CHANGED);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}

TEST(NetworkCommandsIndHook, networkScanResult_1_5) {
    static RIL_CellInfo_V1_5 cellInfos[] = {
        {}, {}
    };
    static RIL_NetworkScanResult_V1_5 testVector[] = {
        { RIL_ScanStatus::PARTIAL, 1, cellInfos, RIL_E_SUCCESS },
        { RIL_ScanStatus::COMPLETE, 2, cellInfos, RIL_E_SUCCESS },
    };
    static size_t size = NUM_ELEMS(testVector);
    for (size_t i = 0; i < size; i++) {
        static const RIL_NetworkScanResult_V1_5& rilScanResult = testVector[i];
        struct TestIndicationCb : public RadioNetworkIndicationCallbackAdapter {
            TestChecker checker;
            void networkScanResult(aidl_common::RadioIndicationType type,
                        const aidl_network::NetworkScanResult& result) override {
                checker.checked();
                EXPECT_EQ(type, aidl_common::RadioIndicationType::UNSOLICITED);
                EXPECT_EQ(result.status, rilScanResult.status);
                EXPECT_EQ((int)result.error, (int)rilScanResult.error);
                EXPECT_EQ(result.networkInfos.size(), rilScanResult.network_infos_length);
            }

            void test(const aidl_network::CellInfo& cellInfo, const RIL_CellInfo_V1_5& rilCellInfo) {
                EXPECT_EQ(cellInfo.registered, rilCellInfo.registered);
                EXPECT_EQ((int)cellInfo.connectionStatus, (int)rilCellInfo.connectionStatus);
            }
        };
        auto responseCb = std::shared_ptr<RadioNetworkResponseCallback>(new RadioNetworkResponseCallbackAdapter);
        auto indicationCb = std::shared_ptr<RadioNetworkIndicationCallback>(new TestIndicationCb);
        sTestStub.setup(responseCb, indicationCb);
        auto test = android::getUnsolRespInfo(0, RIL_UNSOL_NETWORK_SCAN_RESULT);
        ASSERT_NE(test, nullptr);
        ASSERT_NE(test->responseFunction, nullptr);
        test->responseFunction(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0, RIL_E_SUCCESS, testVector, sizeof(testVector));
    }
    sTestStub.teardown();
}

TEST(NetworkCommandsIndHook, networkStateChanged) {
    struct TestIndicationCb : public RadioNetworkIndicationCallbackAdapter {
        TestChecker checker;
        void networkStateChanged(aidl_common::RadioIndicationType /* type */) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioNetworkResponseCallback>(new RadioNetworkResponseCallbackAdapter);
    auto indicationCb = std::shared_ptr<RadioNetworkIndicationCallback>(new TestIndicationCb);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getUnsolRespInfo(0, RIL_UNSOL_RESPONSE_VOICE_NETWORK_STATE_CHANGED);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}

TEST(NetworkCommandsIndHook, registrationFailedInd) {
    static RIL_RegistrationFailed testVector = {
        {
            .cellInfoType = RIL_CellInfoType::RIL_CELL_INFO_TYPE_LTE,
            .lte = {
                311, ril::util::mnc::encode(480, 3), 1000, 300, 35000, 1500,
                {"test long", "test short"}, 40, 0, 0, {}, 0, {}
            },
        },
        (char *)"311480", RIL_Domain::DOMAIN_CS, INT_MAX, INT_MAX
    };
    struct TestIndicationCb : public RadioNetworkIndicationCallbackAdapter {
        TestChecker checker;
        void registrationFailed(aidl_common::RadioIndicationType type,
                const aidl_network::CellIdentity& cellIdentity,
                const std::string& chosenPlmn, int domain,
                int causeCode, int additionalCauseCode) override {
            checker.checked();
            EXPECT_EQ(type, aidl_common::RadioIndicationType::UNSOLICITED);
            testCellIdentity(cellIdentity, testVector.cellIdentity);
            EXPECT_TRUE(chosenPlmn == testVector.chosenPlmn);
            EXPECT_EQ(domain, testVector.domainBitmap);
            EXPECT_EQ(causeCode, testVector.causeCode);
            EXPECT_EQ(additionalCauseCode, testVector.additionalCauseCode);
        }
    };
    auto responseCb = std::shared_ptr<RadioNetworkResponseCallback>(new RadioNetworkResponseCallbackAdapter);
    auto indicationCb = std::shared_ptr<RadioNetworkIndicationCallback>(new TestIndicationCb);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getUnsolRespInfo(0, RIL_UNSOL_REGISTRATION_FAILED);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0, RIL_E_SUCCESS, &testVector, sizeof(testVector));
    sTestStub.teardown();
}

TEST(NetworkCommandsIndHook, restrictedStateChanged) {
    static int32_t testVector = RIL_RESTRICTED_STATE_CS_NORMAL;
    struct TestIndicationCb : public RadioNetworkIndicationCallbackAdapter {
        TestChecker checker;
        void restrictedStateChanged(aidl_common::RadioIndicationType /* type */,
            aidl_network::PhoneRestrictedState state) override {
            checker.checked();
            EXPECT_TRUE(state == aidl_network::PhoneRestrictedState(testVector));
        }
    };
    auto responseCb = std::shared_ptr<RadioNetworkResponseCallback>(new RadioNetworkResponseCallbackAdapter);
    auto indicationCb = std::shared_ptr<RadioNetworkIndicationCallback>(new TestIndicationCb);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getUnsolRespInfo(0, RIL_UNSOL_RESTRICTED_STATE_CHANGED);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0, RIL_E_SUCCESS, &testVector, sizeof(testVector));
    sTestStub.teardown();
}

TEST(NetworkCommandsIndHook, suppSvcNotify) {
    static RIL_SuppSvcNotification testVector = {
        1, 1, 1, 1, (char *)"01012345678",
    };
    struct TestIndicationCb : public RadioNetworkIndicationCallbackAdapter {
        TestChecker checker;
        void suppSvcNotify(aidl_common::RadioIndicationType /* type */,
            const aidl_network::SuppSvcNotification& suppSvc) override {
            checker.checked();
            EXPECT_TRUE(suppSvc.isMT == testVector.notificationType);
            EXPECT_TRUE(suppSvc.code == testVector.code);
            EXPECT_TRUE(suppSvc.index == testVector.index);
            EXPECT_TRUE(suppSvc.type == testVector.type);
            EXPECT_TRUE(suppSvc.number == testVector.number);
        }
    };
    auto responseCb = std::shared_ptr<RadioNetworkResponseCallback>(new RadioNetworkResponseCallbackAdapter);
    auto indicationCb = std::shared_ptr<RadioNetworkIndicationCallback>(new TestIndicationCb);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getUnsolRespInfo(0, RIL_UNSOL_SUPP_SVC_NOTIFICATION);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0, RIL_E_SUCCESS, &testVector, sizeof(testVector));
    sTestStub.teardown();
}

TEST(NetworkCommandsIndHook, voiceRadioTechChanged) {
    static int32_t testVector = RADIO_TECH_LTE;
    struct TestIndicationCb : public RadioNetworkIndicationCallbackAdapter {
        TestChecker checker;
        void voiceRadioTechChanged(aidl_common::RadioIndicationType /* type */,
            aidl_common::RadioTechnology rat) override {
            checker.checked();
            EXPECT_TRUE(rat == aidl_common::RadioTechnology(testVector));
        }
    };
    auto responseCb = std::shared_ptr<RadioNetworkResponseCallback>(new RadioNetworkResponseCallbackAdapter);
    auto indicationCb = std::shared_ptr<RadioNetworkIndicationCallback>(new TestIndicationCb);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getUnsolRespInfo(0, RIL_UNSOL_VOICE_RADIO_TECH_CHANGED);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0, RIL_E_SUCCESS, &testVector, sizeof(testVector));
    sTestStub.teardown();
}

TEST(NetworkCommandsIndHook, currentLinkCapacityEstimate_1_6) {
    static RIL_LinkCapacityEstimate_V1_6 testVector = {
        200000, 300000, 400000, 500000,
    };
    struct TestIndicationCb : public RadioNetworkIndicationCallbackAdapter {
        TestChecker checker;
        void currentLinkCapacityEstimate(aidl_common::RadioIndicationType type,
                    const aidl_network::LinkCapacityEstimate &lce) override {
            checker.checked();
            EXPECT_EQ(type, aidl_common::RadioIndicationType::UNSOLICITED);
            EXPECT_EQ(lce.downlinkCapacityKbps, (int)testVector.downlinkCapacityKbps);
            EXPECT_EQ(lce.uplinkCapacityKbps, (int)testVector.uplinkCapacityKbps);
            EXPECT_EQ(lce.secondaryDownlinkCapacityKbps, (int)testVector.DCNRSecondaryDownlinkCapacityKbps);
            EXPECT_EQ(lce.secondaryUplinkCapacityKbps, (int)testVector.DCNRSecondaryUplinkCapacityKbps);
        }
    };
    auto responseCb = std::shared_ptr<RadioNetworkResponseCallback>(new RadioNetworkResponseCallbackAdapter);
    auto indicationCb = std::shared_ptr<RadioNetworkIndicationCallback>(new TestIndicationCb);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getUnsolRespInfo(0, RIL_UNSOL_LCEDATA_RECV);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0, RIL_E_SUCCESS, &testVector, sizeof(testVector));
    sTestStub.teardown();
}

TEST(NetworkCommandsIndHook, cellInfoList_1_6) {
    static RIL_CellInfo_V1_6 testVector[] = {
        {}, {},
    };
    static size_t size = NUM_ELEMS(testVector);
    struct TestIndicationCb : public RadioNetworkIndicationCallbackAdapter {
        TestChecker checker;
        void cellInfoList(aidl_common::RadioIndicationType type,
                const std::vector<aidl_network::CellInfo>& records) override {
            checker.checked();
            EXPECT_EQ(type, aidl_common::RadioIndicationType::UNSOLICITED);
            EXPECT_EQ(records.size(), size);
        }
    };
    auto responseCb = std::shared_ptr<RadioNetworkResponseCallback>(new RadioNetworkResponseCallbackAdapter);
    auto indicationCb = std::shared_ptr<RadioNetworkIndicationCallback>(new TestIndicationCb);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getUnsolRespInfo(0, RIL_UNSOL_CELL_INFO_LIST);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0, RIL_E_SUCCESS, &testVector, sizeof(testVector));
    sTestStub.teardown();
}

TEST(NetworkCommandsIndHook, currentPhysicalChannelConfigs_1_6) {
    static int contextIds1[] = { 0 };
    static int contextIds2[] = { 0, 1 };
    static RIL_PhysicalChannelConfig_V1_6 testVector[] = {
        {
            RIL_CellConnectionStatus::PRIMARY_SERVING,
            RADIO_TECH_LTE, 1000, 1100, 40, 20,
            NUM_ELEMS(contextIds1), contextIds1,
            3000, {}
        },
        {
            RIL_CellConnectionStatus::SECONDARY_SERVING,
            RADIO_TECH_LTE, 2000, 2100, 80, 40,
            NUM_ELEMS(contextIds2), contextIds2,
            5000, {}
        },
    };
    static size_t size = NUM_ELEMS(testVector);
    struct TestIndicationCb : public RadioNetworkIndicationCallbackAdapter {
        TestChecker checker;
        void currentPhysicalChannelConfigs(aidl_common::RadioIndicationType type,
                const std::vector<aidl_network::PhysicalChannelConfig>& configs) override {
            checker.checked();
            EXPECT_EQ(type, aidl_common::RadioIndicationType::UNSOLICITED);
            EXPECT_EQ(configs.size(), size);
            if (configs.size() > 0) {
                for (size_t i = 0; i < configs.size(); i++) {
                    test(configs[i], testVector[i]);
                }
            }
        }

        void test(const aidl_network::PhysicalChannelConfig& pcc,
                  const RIL_PhysicalChannelConfig_V1_6& rilPcc) {
            EXPECT_EQ((int)pcc.status, (int)rilPcc.status);
            EXPECT_EQ((int)pcc.rat, (int)rilPcc.rat);
            EXPECT_EQ(pcc.downlinkChannelNumber, rilPcc.downlinkChannelNumber);
            EXPECT_EQ(pcc.uplinkChannelNumber, rilPcc.uplinkChannelNumber);
            EXPECT_EQ(pcc.cellBandwidthDownlinkKhz, rilPcc.cellBandwidthDownlink);
            EXPECT_EQ(pcc.cellBandwidthUplinkKhz, rilPcc.cellBandwidthUplink);
            for (size_t i = 0; i < pcc.contextIds.size(); i++) {
                EXPECT_EQ(pcc.contextIds[i], rilPcc.contextIds[i]);
            }
            EXPECT_EQ(pcc.physicalCellId, static_cast<int32_t>(rilPcc.physicalCellId));
        }
    };
    auto responseCb = std::shared_ptr<RadioNetworkResponseCallback>(new RadioNetworkResponseCallbackAdapter);
    auto indicationCb = std::shared_ptr<RadioNetworkIndicationCallback>(new TestIndicationCb);
    sTestStub.setup(responseCb, indicationCb);

    auto test = android::getUnsolRespInfo(0, RIL_UNSOL_PHYSICAL_CHANNEL_CONFIG);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0, RIL_E_SUCCESS, testVector, sizeof(testVector));
    sTestStub.teardown();
}

TEST(NetworkCommandsIndHook, networkScanResult_1_6) {
    static RIL_CellInfo_V1_6 cellInfos[] = {
        {}, {}
    };
    static RIL_NetworkScanResult_V1_6 testVector[] = {
        { RIL_ScanStatus::PARTIAL, 1, cellInfos, RIL_E_SUCCESS },
        { RIL_ScanStatus::COMPLETE, 2, cellInfos, RIL_E_SUCCESS },
    };
    static size_t size = NUM_ELEMS(testVector);
    for (size_t i = 0; i < size; i++) {
        static const RIL_NetworkScanResult_V1_6& rilScanResult = testVector[i];
        struct TestIndicationCb : public RadioNetworkIndicationCallbackAdapter {
            TestChecker checker;
            void networkScanResult(aidl_common::RadioIndicationType type,
                        const aidl_network::NetworkScanResult& result) override {
                checker.checked();
                EXPECT_EQ(type, aidl_common::RadioIndicationType::UNSOLICITED);
                EXPECT_EQ(result.status, rilScanResult.status);
                EXPECT_EQ((int)result.error, (int)rilScanResult.error);
                EXPECT_EQ(result.networkInfos.size(), rilScanResult.network_infos_length);
            }

            void test(const aidl_network::CellInfo& cellInfo, const RIL_CellInfo_V1_6& rilCellInfo) {
                EXPECT_EQ(cellInfo.registered, rilCellInfo.registered);
                EXPECT_EQ((int)cellInfo.connectionStatus, (int)rilCellInfo.connectionStatus);
            }
        };
        auto responseCb = std::shared_ptr<RadioNetworkResponseCallback>(new RadioNetworkResponseCallbackAdapter);
        auto indicationCb = std::shared_ptr<RadioNetworkIndicationCallback>(new TestIndicationCb);
        sTestStub.setup(responseCb, indicationCb);
        auto test = android::getUnsolRespInfo(0, RIL_UNSOL_NETWORK_SCAN_RESULT);
        ASSERT_NE(test, nullptr);
        ASSERT_NE(test->responseFunction, nullptr);
        test->responseFunction(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0, RIL_E_SUCCESS, testVector, sizeof(testVector));
    }
    sTestStub.teardown();
}
