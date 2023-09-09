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

TEST(NetworkCommandsRespHook, getAvailableBandModesResponse) {
    static int32_t testVector[] = {
        1, 2, 3, 4, 5,
    };
    struct TestResponseCb : public RadioNetworkResponseCallbackAdapter {
        TestChecker checker;
        void getAvailableBandModesResponse(const aidl_common::RadioResponseInfo& /* info */,
                const std::vector<aidl_network::RadioBandMode>& bandModes) override {
            checker.checked();
            EXPECT_TRUE(bandModes.size() == NUM_ELEMS(testVector));
            for (size_t i = 0; i < bandModes.size(); i++) {
                EXPECT_TRUE(static_cast<int32_t>(bandModes[i]) == testVector[i]);
            }
        }
    };
    auto responseCb = std::shared_ptr<RadioNetworkResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioNetworkIndicationCallback>(new RadioNetworkIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_QUERY_AVAILABLE_BAND_MODE);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, testVector, sizeof(testVector));
    sTestStub.teardown();
}

TEST(NetworkCommandsRespHook, getAvailableNetworksResponse) {
    static const char *testVector[] = {
        "alpha long", "alpha short", "00101", "current",
    };
    struct TestResponseCb : public RadioNetworkResponseCallbackAdapter {
        TestChecker checker;
        void getAvailableNetworksResponse(const aidl_common::RadioResponseInfo& /* info */,
            const std::vector<aidl_network::OperatorInfo>& networkInfos) override {
            checker.checked();
            EXPECT_TRUE(networkInfos.size() == NUM_ELEMS(testVector) / 4);
            for (size_t i = 0; i < networkInfos.size(); i++) {
                auto& info = networkInfos[i];
                EXPECT_TRUE(info.alphaLong == testVector[i * 4]);
                EXPECT_TRUE(info.alphaShort == testVector[i * 4 + 1]);
                EXPECT_TRUE(info.operatorNumeric == testVector[i * 4 + 2]);
            }
        }
    };
    auto responseCb = std::shared_ptr<RadioNetworkResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioNetworkIndicationCallback>(new RadioNetworkIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_QUERY_AVAILABLE_NETWORKS);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, testVector, sizeof(testVector));
    sTestStub.teardown();
}

TEST(NetworkCommandsRespHook, getCdmaRoamingPreferenceResponse) {
    static int32_t testVector = 1;
    struct TestResponseCb : public RadioNetworkResponseCallbackAdapter {
        TestChecker checker;
        void getCdmaRoamingPreferenceResponse(const aidl_common::RadioResponseInfo& /* info */,
            aidl_network::CdmaRoamingType type) override {
            checker.checked();
            EXPECT_TRUE(static_cast<int32_t>(type) == testVector);
        }
    };
    auto responseCb = std::shared_ptr<RadioNetworkResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioNetworkIndicationCallback>(new RadioNetworkIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_CDMA_QUERY_ROAMING_PREFERENCE);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, &testVector, sizeof(testVector));
    sTestStub.teardown();
}

TEST(NetworkCommandsRespHook, getImsRegistrationStateResponse) {
    static int32_t testVector[] = {1, 0x1234};
    struct TestResponseCb : public RadioNetworkResponseCallbackAdapter {
        TestChecker checker;
        void getImsRegistrationStateResponse(const aidl_common::RadioResponseInfo& /* info */,
            bool isRegistered, aidl_common::RadioTechnologyFamily ratFamily) override {
            checker.checked();
            EXPECT_TRUE(isRegistered == static_cast<bool>(testVector[0]));
            EXPECT_TRUE(static_cast<int32_t>(ratFamily) == testVector[1]);
        }
    };
    auto responseCb = std::shared_ptr<RadioNetworkResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioNetworkIndicationCallback>(new RadioNetworkIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_IMS_REGISTRATION_STATE);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, testVector, sizeof(testVector));
    sTestStub.teardown();
}

TEST(NetworkCommandsRespHook, getNetworkSelectionModeResponse) {
    static int32_t testVector = 1;
    struct TestResponseCb : public RadioNetworkResponseCallbackAdapter {
        TestChecker checker;
        void getNetworkSelectionModeResponse(const aidl_common::RadioResponseInfo& /* info */, bool manual) override {
            checker.checked();
            EXPECT_TRUE(manual == static_cast<bool>(testVector));
        }
    };
    auto responseCb = std::shared_ptr<RadioNetworkResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioNetworkIndicationCallback>(new RadioNetworkIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_QUERY_NETWORK_SELECTION_MODE);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, &testVector, sizeof(testVector));
    sTestStub.teardown();
}

TEST(NetworkCommandsRespHook, getOperatorResponse) {
    static const char *testVector[] = {
        "alpha long", "alpha short", "00101",
    };
    struct TestResponseCb : public RadioNetworkResponseCallbackAdapter {
        TestChecker checker;
        void getOperatorResponse(const aidl_common::RadioResponseInfo& /* info */,
            const std::string& longName, const std::string& shortName,
            const std::string& numeric) override {
            checker.checked();
            EXPECT_TRUE(longName == testVector[0]);
            EXPECT_TRUE(shortName == testVector[1]);
            EXPECT_TRUE(numeric == testVector[2]);
        }
    };
    auto responseCb = std::shared_ptr<RadioNetworkResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioNetworkIndicationCallback>(new RadioNetworkIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_OPERATOR);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, testVector, sizeof(testVector));
    sTestStub.teardown();
}

TEST(NetworkCommandsRespHook, getVoiceRadioTechnologyResponse) {
    static int32_t testVector = RADIO_TECH_LTE;
    struct TestResponseCb : public RadioNetworkResponseCallbackAdapter {
        TestChecker checker;
        void getVoiceRadioTechnologyResponse(const aidl_common::RadioResponseInfo& /* info */,
            aidl_common::RadioTechnology rat) override {
            checker.checked();
            EXPECT_TRUE(static_cast<int32_t>(rat) == testVector);
        }
    };
    auto responseCb = std::shared_ptr<RadioNetworkResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioNetworkIndicationCallback>(new RadioNetworkIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_VOICE_RADIO_TECH);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, &testVector, sizeof(testVector));
    sTestStub.teardown();
}

TEST(NetworkCommandsRespHook, setBandModeResponse) {
    struct TestResponseCb : public RadioNetworkResponseCallbackAdapter {
        TestChecker checker;
        void setBandModeResponse(const aidl_common::RadioResponseInfo& /* info */) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioNetworkResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioNetworkIndicationCallback>(new RadioNetworkIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_SET_BAND_MODE);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}

TEST(NetworkCommandsRespHook, setBarringPasswordResponse) {
    struct TestResponseCb : public RadioNetworkResponseCallbackAdapter {
        TestChecker checker;
        void setBarringPasswordResponse(const aidl_common::RadioResponseInfo& /* info */) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioNetworkResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioNetworkIndicationCallback>(new RadioNetworkIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_CHANGE_BARRING_PASSWORD);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}

TEST(NetworkCommandsRespHook, setCdmaRoamingPreferenceResponse) {
    struct TestResponseCb : public RadioNetworkResponseCallbackAdapter {
        TestChecker checker;
        void setCdmaRoamingPreferenceResponse(const aidl_common::RadioResponseInfo& /* info */) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioNetworkResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioNetworkIndicationCallback>(new RadioNetworkIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_CDMA_SET_ROAMING_PREFERENCE);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}

TEST(NetworkCommandsRespHook, setCellInfoListRateResponse) {
    struct TestResponseCb : public RadioNetworkResponseCallbackAdapter {
        TestChecker checker;
        void setCellInfoListRateResponse(const aidl_common::RadioResponseInfo& /* info */) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioNetworkResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioNetworkIndicationCallback>(new RadioNetworkIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_SET_UNSOL_CELL_INFO_LIST_RATE);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}

TEST(NetworkCommandsRespHook, setLocationUpdatesResponse) {
    struct TestResponseCb : public RadioNetworkResponseCallbackAdapter {
        TestChecker checker;
        void setLocationUpdatesResponse(const aidl_common::RadioResponseInfo& /* info */) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioNetworkResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioNetworkIndicationCallback>(new RadioNetworkIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_SET_LOCATION_UPDATES);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}

TEST(NetworkCommandsRespHook, setNetworkSelectionModeAutomaticResponse) {
    struct TestResponseCb : public RadioNetworkResponseCallbackAdapter {
        TestChecker checker;
        void setNetworkSelectionModeAutomaticResponse(const aidl_common::RadioResponseInfo& /* info */) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioNetworkResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioNetworkIndicationCallback>(new RadioNetworkIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_SET_NETWORK_SELECTION_AUTOMATIC);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}

TEST(NetworkCommandsRespHook, setSuppServiceNotificationsResponse) {
    struct TestResponseCb : public RadioNetworkResponseCallbackAdapter {
        TestChecker checker;
        void setSuppServiceNotificationsResponse(const aidl_common::RadioResponseInfo& /* info */) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioNetworkResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioNetworkIndicationCallback>(new RadioNetworkIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_SET_SUPP_SVC_NOTIFICATION);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}

TEST(NetworkCommandsRespHook, supplyNetworkDepersonalizationResponse) {
    static int32_t testVector = 1;
    struct TestResponseCb : public RadioNetworkResponseCallbackAdapter {
        TestChecker checker;
        void supplyNetworkDepersonalizationResponse(const aidl_common::RadioResponseInfo& /* info */,
                int remainingRetries) override {
            checker.checked();
            EXPECT_TRUE(remainingRetries == testVector);
        }
    };
    auto responseCb = std::shared_ptr<RadioNetworkResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioNetworkIndicationCallback>(new RadioNetworkIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_ENTER_NETWORK_DEPERSONALIZATION);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, &testVector, sizeof(testVector));
    sTestStub.teardown();
}

TEST(NetworkCommandsRespHook, stopNetworkScanResponse) {
    struct TestResponseCb : public RadioNetworkResponseCallbackAdapter {
        TestChecker checker;
        void stopNetworkScanResponse(const aidl_common::RadioResponseInfo& /* info */) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioNetworkResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioNetworkIndicationCallback>(new RadioNetworkIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_STOP_NETWORK_SCAN);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}

TEST(NetworkCommandsRespHook, getBarringInfoResponse) {
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
    struct TestResponseCb : public RadioNetworkResponseCallbackAdapter {
        TestChecker checker;
        void getBarringInfoResponse(const aidl_common::RadioResponseInfo& /* info */,
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
    auto responseCb = std::shared_ptr<RadioNetworkResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioNetworkIndicationCallback>(new RadioNetworkIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_GET_BARRING_INFO);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, &testVector, sizeof(testVector));
    sTestStub.teardown();
}

TEST(NetworkCommandsRespHook, setIndicationFilterResponse_1_5) {
    struct TestResponseCb : public RadioNetworkResponseCallbackAdapter {
        TestChecker checker;
        void setIndicationFilterResponse(const aidl_common::RadioResponseInfo& /* info */) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioNetworkResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioNetworkIndicationCallback>(new RadioNetworkIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, ENCODE_REQUEST(RIL_REQUEST_SET_UNSOLICITED_RESPONSE_FILTER, HAL_VERSION_CODE(1,5)));
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}

TEST(NetworkCommandsRespHook, setLinkCapacityReportingCriteriaResponse_1_5) {
    struct TestResponseCb : public RadioNetworkResponseCallbackAdapter {
        TestChecker checker;
        void setLinkCapacityReportingCriteriaResponse(const aidl_common::RadioResponseInfo& /* info */) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioNetworkResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioNetworkIndicationCallback>(new RadioNetworkIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, ENCODE_REQUEST(RIL_REQUEST_SET_LINK_CAPACITY_REPORTING_CRITERIA, HAL_VERSION_CODE(1,5)));
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}

TEST(NetworkCommandsRespHook, setNetworkSelectionModeManualResponse_1_5) {
    struct TestResponseCb : public RadioNetworkResponseCallbackAdapter {
        TestChecker checker;
        void setNetworkSelectionModeManualResponse(const aidl_common::RadioResponseInfo& /* info */) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioNetworkResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioNetworkIndicationCallback>(new RadioNetworkIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, ENCODE_REQUEST(RIL_REQUEST_SET_NETWORK_SELECTION_MANUAL, HAL_VERSION_CODE(1,5)));
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}

TEST(NetworkCommandsRespHook, setSignalStrengthReportingCriteriaResponse_1_5) {
    struct TestResponseCb : public RadioNetworkResponseCallbackAdapter {
        TestChecker checker;
        void setSignalStrengthReportingCriteriaResponse(const aidl_common::RadioResponseInfo& /* info */) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioNetworkResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioNetworkIndicationCallback>(new RadioNetworkIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, ENCODE_REQUEST(RIL_REQUEST_SET_SIGNAL_STRENGTH_REPORTING_CRITERIA, HAL_VERSION_CODE(1,5)));
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}

TEST(NetworkCommandsRespHook, setSystemSelectionChannelsResponse_1_5) {
    struct TestResponseCb : public RadioNetworkResponseCallbackAdapter {
        TestChecker checker;
        void setSystemSelectionChannelsResponse(const aidl_common::RadioResponseInfo& /* info */) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioNetworkResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioNetworkIndicationCallback>(new RadioNetworkIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, ENCODE_REQUEST(RIL_REQUEST_SET_SYSTEM_SELECTION_CHANNELS, HAL_VERSION_CODE(1,5)));
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}

TEST(NetworkCommandsRespHook, startNetworkScanResponse_1_5) {
    struct TestResponseCb : public RadioNetworkResponseCallbackAdapter {
        TestChecker checker;
        void startNetworkScanResponse(const aidl_common::RadioResponseInfo& /* info */) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioNetworkResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioNetworkIndicationCallback>(new RadioNetworkIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, ENCODE_REQUEST(RIL_REQUEST_START_NETWORK_SCAN, HAL_VERSION_CODE(1,5)));
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}

TEST(NetworkCommandsRespHook, getAllowedNetworkTypesBitmapResponse) {
    static int32_t testVector = 0x4321;
    struct TestResponseCb : public RadioNetworkResponseCallbackAdapter {
        TestChecker checker;
        void getAllowedNetworkTypesBitmapResponse(const aidl_common::RadioResponseInfo& /* info */,
            int networkTypeBitmap) override {
            checker.checked();
            EXPECT_TRUE(networkTypeBitmap == testVector);
        }
    };
    auto responseCb = std::shared_ptr<RadioNetworkResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioNetworkIndicationCallback>(new RadioNetworkIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_GET_ALLOWED_NETWORK_TYPES_BITMAP);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, &testVector, sizeof(testVector));
    sTestStub.teardown();
}

TEST(NetworkCommandsRespHook, getCellInfoListResponse_1_6) {
    static RIL_CellInfo_V1_6 testVector[] = {
        {}, {},
    };
    struct TestResponseCb : public RadioNetworkResponseCallbackAdapter {
        TestChecker checker;
        void getCellInfoListResponse(const aidl_common::RadioResponseInfo& /* info */,
            const std::vector<aidl_network::CellInfo>& cellInfo) override {
            checker.checked();
            EXPECT_EQ(cellInfo.size(), NUM_ELEMS(testVector));
        }
    };
    auto responseCb = std::shared_ptr<RadioNetworkResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioNetworkIndicationCallback>(new RadioNetworkIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, ENCODE_REQUEST(RIL_REQUEST_GET_CELL_INFO_LIST, HAL_VERSION_CODE(1,6)));
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, &testVector, sizeof(testVector));
    sTestStub.teardown();
}

TEST(NetworkCommandsRespHook, getDataRegistrationStateResponse_1_6) {
    static RIL_RegStateResult_V1_6 testVector = {
        RIL_RegState::RIL_REG_HOME, RIL_RadioTechnology::RADIO_TECH_LTE,
        RIL_RegistrationFailCause::REG_FAIL_CAUSE_NONE, {},
        "00101", { .eutranInfo = {{true, true}, {true, false, true}}},
    };
    struct TestResponseCb : public RadioNetworkResponseCallbackAdapter {
        TestChecker checker;
        void getDataRegistrationStateResponse(const aidl_common::RadioResponseInfo& /* info */,
            const aidl_network::RegStateResult& dataRegResponse) override {
            checker.checked();
            EXPECT_TRUE(static_cast<int32_t>(dataRegResponse.regState) == testVector.regState);
            EXPECT_TRUE(static_cast<int32_t>(dataRegResponse.rat) == testVector.rat);
            EXPECT_TRUE(static_cast<int32_t>(dataRegResponse.reasonForDenial) == testVector.reasonForDenial);
            testCellIdentity(dataRegResponse.cellIdentity, testVector.cellIdentity);
            EXPECT_TRUE(dataRegResponse.registeredPlmn == testVector.registeredPlmn);
            using Tag = aidl_network::AccessTechnologySpecificInfo::Tag;
            auto& lteVops = dataRegResponse.accessTechnologySpecificInfo.get<Tag::eutranInfo>().lteVopsInfo;
            auto& nrIndicators = dataRegResponse.accessTechnologySpecificInfo.get<Tag::eutranInfo>().nrIndicators;
            auto& testLteVops = testVector.accessTechnologySpecificInfo.eutranInfo.lteVopsInfo;
            auto& testNrIndicators = testVector.accessTechnologySpecificInfo.eutranInfo.nrIndicators;
            EXPECT_TRUE(lteVops.isVopsSupported == testLteVops.isVopsSupported);
            EXPECT_TRUE(lteVops.isEmcBearerSupported == testLteVops.isEmcBearerSupported);
            EXPECT_TRUE(nrIndicators.isEndcAvailable == testNrIndicators.isEndcAvailable);
            EXPECT_TRUE(nrIndicators.isDcNrRestricted == testNrIndicators.isDcNrRestricted);
            EXPECT_TRUE(nrIndicators.isNrAvailable == testNrIndicators.isNrAvailable);
        }
    };
    auto responseCb = std::shared_ptr<RadioNetworkResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioNetworkIndicationCallback>(new RadioNetworkIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, ENCODE_REQUEST(RIL_REQUEST_DATA_REGISTRATION_STATE, HAL_VERSION_CODE(1,6)));
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, &testVector, sizeof(testVector));
    sTestStub.teardown();
}

TEST(NetworkCommandsRespHook, getSignalStrengthResponse_1_6) {
    static RIL_SignalStrength_V1_6 testVector = {
        { 60, 125, 0 },
        { 70, 125 },
        { 70, 125, 3 },
        { {15, 140, 10, -100, 0, 0x7FFFFFFE}, 1 },
        { 15, 3, 255 },
        { 15, 3, 255, 255 },
        { {44, 7, -10, 44, 7, -10}, 1, 3, {1, 2, 3} }
    };
    struct TestResponseCb : public RadioNetworkResponseCallbackAdapter {
        TestChecker checker;
        void getSignalStrengthResponse(const aidl_common::RadioResponseInfo& /* info */,
            const aidl_network::SignalStrength& signalStrength) override {
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
    auto responseCb = std::shared_ptr<RadioNetworkResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioNetworkIndicationCallback>(new RadioNetworkIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, ENCODE_REQUEST(RIL_REQUEST_SIGNAL_STRENGTH, HAL_VERSION_CODE(1,6)));
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, &testVector, sizeof(testVector));
    sTestStub.teardown();
}

TEST(NetworkCommandsRespHook, getSystemSelectionChannelsResponse) {
    static RIL_RadioAccessSpecifier_V1_5 testVector[] = {
        {}, {}
    };
    struct TestResponseCb : public RadioNetworkResponseCallbackAdapter {
        TestChecker checker;
        void getSystemSelectionChannelsResponse(const aidl_common::RadioResponseInfo& /* info */,
            const std::vector<aidl_network::RadioAccessSpecifier>& specifiers) override {
            checker.checked();
            EXPECT_TRUE(specifiers.size() == NUM_ELEMS(testVector));
        }
    };
    auto responseCb = std::shared_ptr<RadioNetworkResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioNetworkIndicationCallback>(new RadioNetworkIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_GET_SYSTEM_SELECTION_CHANNELS);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, testVector, sizeof(testVector));
    sTestStub.teardown();
}

TEST(NetworkCommandsRespHook, getVoiceRegistrationStateResponse_1_6) {
    static RIL_RegStateResult_V1_6 testVector = {
        RIL_RegState::RIL_REG_HOME, RIL_RadioTechnology::RADIO_TECH_LTE,
        RIL_RegistrationFailCause::REG_FAIL_CAUSE_NONE, {}, "00101", {},
    };
    struct TestResponseCb : public RadioNetworkResponseCallbackAdapter {
        TestChecker checker;
        void getVoiceRegistrationStateResponse(const aidl_common::RadioResponseInfo& /* info */,
            const aidl_network::RegStateResult& dataRegResponse) override {
            checker.checked();
            EXPECT_TRUE(static_cast<int32_t>(dataRegResponse.regState) == testVector.regState);
            EXPECT_TRUE(static_cast<int32_t>(dataRegResponse.rat) == testVector.rat);
            EXPECT_TRUE(static_cast<int32_t>(dataRegResponse.reasonForDenial) == testVector.reasonForDenial);
            testCellIdentity(dataRegResponse.cellIdentity, testVector.cellIdentity);
            EXPECT_TRUE(dataRegResponse.registeredPlmn == testVector.registeredPlmn);
        }
    };
    auto responseCb = std::shared_ptr<RadioNetworkResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioNetworkIndicationCallback>(new RadioNetworkIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, ENCODE_REQUEST(RIL_REQUEST_VOICE_REGISTRATION_STATE, HAL_VERSION_CODE(1,6)));
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, &testVector, sizeof(testVector));
    sTestStub.teardown();
}

TEST(NetworkCommandsRespHook, isNrDualConnectivityEnabledResponse) {
    static int32_t testVector = 1;
    struct TestResponseCb : public RadioNetworkResponseCallbackAdapter {
        TestChecker checker;
        void isNrDualConnectivityEnabledResponse(const aidl_common::RadioResponseInfo& /* info */,
                bool isEnabled) override {
            checker.checked();
            EXPECT_TRUE(isEnabled == static_cast<bool>(testVector));
        }
    };
    auto responseCb = std::shared_ptr<RadioNetworkResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioNetworkIndicationCallback>(new RadioNetworkIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_IS_NR_DUAL_CONNECTIVITY_ENABLED);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, &testVector, sizeof(testVector));
    sTestStub.teardown();
}

TEST(NetworkCommandsRespHook, setNrDualConnectivityStateResponse) {
    struct TestResponseCb : public RadioNetworkResponseCallbackAdapter {
        TestChecker checker;
        void setNrDualConnectivityStateResponse(const aidl_common::RadioResponseInfo& /* info */) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioNetworkResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioNetworkIndicationCallback>(new RadioNetworkIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_ENABLE_NR_DUAL_CONNECTIVITY);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}

TEST(NetworkCommandsRespHook, setAllowedNetworkTypesBitmapResponse) {
    struct TestResponseCb : public RadioNetworkResponseCallbackAdapter {
        TestChecker checker;
        void setAllowedNetworkTypesBitmapResponse(const aidl_common::RadioResponseInfo& /* info */) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioNetworkResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioNetworkIndicationCallback>(new RadioNetworkIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_SET_ALLOWED_NETWORK_TYPES_BITMAP);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}
