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
#include "radioproxytestsetup.h"
#include "radionetworkproxy.h"

namespace aidl_common = ::aidl::android::hardware::radio;
namespace aidl_network = ::aidl::android::hardware::radio::network;
using ::aidl::android::hardware::radio::AccessNetwork;

using namespace android::hardware::radio;
using ::android::hardware::hidl_bitfield;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using android::sp;

TEST(RadioNetworkProxyHook, getAllowedNetworkTypesBitmap) {
    static int32_t s = INTrand(true);
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> getAllowedNetworkTypesBitmap(int32_t serial) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            return {};
        }
    };
    RadioProxyTestSetup<RadioNetworkProxy> test = sp<TestRadio>::make();
    test->getAllowedNetworkTypesBitmap(s);
}

TEST(RadioNetworkProxyHook, getAvailableBandModes) {
    static int32_t s = INTrand(true);
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> getAvailableBandModes(int32_t serial) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            return {};
        }
    };
    RadioProxyTestSetup<RadioNetworkProxy> test = sp<TestRadio>::make();
    test->getAvailableBandModes(s);
}

TEST(RadioNetworkProxyHook, getAvailableNetworks) {
    static int32_t s = INTrand(true);
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> getAvailableNetworks(int32_t serial) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            return {};
        }
    };
    RadioProxyTestSetup<RadioNetworkProxy> test = sp<TestRadio>::make();
    test->getAvailableNetworks(s);
}

TEST(RadioNetworkProxyHook, getBarringInfo) {
    static int32_t s = INTrand(true);
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> getBarringInfo(int32_t serial) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            return {};
        }
    };
    RadioProxyTestSetup<RadioNetworkProxy> test = sp<TestRadio>::make();
    test->getBarringInfo(s);
}

TEST(RadioNetworkProxyHook, getCdmaRoamingPreference) {
    static int32_t s = INTrand(true);
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> getCdmaRoamingPreference(int32_t serial) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            return {};
        }
    };
    RadioProxyTestSetup<RadioNetworkProxy> test = sp<TestRadio>::make();
    test->getCdmaRoamingPreference(s);
}

TEST(RadioNetworkProxyHook, getCellInfoList) {
    static int32_t s = INTrand(true);
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> getCellInfoList_1_6(int32_t serial) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            return {};
        }
    };
    RadioProxyTestSetup<RadioNetworkProxy> test = sp<TestRadio>::make();
    test->getCellInfoList(s);
}

TEST(RadioNetworkProxyHook, getDataRegistrationState) {
    static int32_t s = INTrand(true);
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> getDataRegistrationState_1_6(int32_t serial) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            return {};
        }
    };
    RadioProxyTestSetup<RadioNetworkProxy> test = sp<TestRadio>::make();
    test->getDataRegistrationState(s);
}

TEST(RadioNetworkProxyHook, getImsRegistrationState) {
    static int32_t s = INTrand(true);
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> getImsRegistrationState(int32_t serial) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            return {};
        }
    };
    RadioProxyTestSetup<RadioNetworkProxy> test = sp<TestRadio>::make();
    test->getImsRegistrationState(s);
}

TEST(RadioNetworkProxyHook, getNetworkSelectionMode) {
    static int32_t s = INTrand(true);
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> getNetworkSelectionMode(int32_t serial) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            return {};
        }
    };
    RadioProxyTestSetup<RadioNetworkProxy> test = sp<TestRadio>::make();
    test->getNetworkSelectionMode(s);
}

TEST(RadioNetworkProxyHook, getOperator) {
    static int32_t s = INTrand(true);
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> getOperator(int32_t serial) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            return {};
        }
    };
    RadioProxyTestSetup<RadioNetworkProxy> test = sp<TestRadio>::make();
    test->getOperator(s);
}

TEST(RadioNetworkProxyHook, getSignalStrength) {
    static int32_t s = INTrand(true);
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> getSignalStrength_1_6(int32_t serial) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            return {};
        }
    };
    RadioProxyTestSetup<RadioNetworkProxy> test = sp<TestRadio>::make();
    test->getSignalStrength(s);
}

TEST(RadioNetworkProxyHook, getSystemSelectionChannels) {
    static int32_t s = INTrand(true);
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> getSystemSelectionChannels(int32_t serial) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            return {};
        }
    };
    RadioProxyTestSetup<RadioNetworkProxy> test = sp<TestRadio>::make();
    test->getSystemSelectionChannels(s);
}

TEST(RadioNetworkProxyHook, getVoiceRadioTechnology) {
    static int32_t s = INTrand(true);
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> getVoiceRadioTechnology(int32_t serial) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            return {};
        }
    };
    RadioProxyTestSetup<RadioNetworkProxy> test = sp<TestRadio>::make();
    test->getVoiceRadioTechnology(s);
}

TEST(RadioNetworkProxyHook, getVoiceRegistrationState) {
    static int32_t s = INTrand(true);
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> getVoiceRegistrationState_1_6(int32_t serial) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            return {};
        }
    };
    RadioProxyTestSetup<RadioNetworkProxy> test = sp<TestRadio>::make();
    test->getVoiceRegistrationState(s);
}

TEST(RadioNetworkProxyHook, isNrDualConnectivityEnabled) {
    static int32_t s = INTrand(true);
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> isNrDualConnectivityEnabled(int32_t serial) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            return {};
        }
    };
    RadioProxyTestSetup<RadioNetworkProxy> test = sp<TestRadio>::make();
    test->isNrDualConnectivityEnabled(s);
}

TEST(RadioNetworkProxyHook, responseAcknowledgement) {
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> responseAcknowledgement() override {
            checker.checked();
            return {};
        }
    };
    RadioProxyTestSetup<RadioNetworkProxy> test = sp<TestRadio>::make();
    test->responseAcknowledgement();
}

TEST(RadioNetworkProxyHook, setAllowedNetworkTypesBitmap) {
    static int32_t s = INTrand(true);
    static int32_t testVector = 0x1234;
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> setAllowedNetworkTypesBitmap(uint32_t serial,
                hidl_bitfield<V1_4::RadioAccessFamily> networkTypeBitmap) override {
            checker.checked();
            EXPECT_TRUE(static_cast<int32_t>(serial) == s);
            EXPECT_TRUE(static_cast<int32_t>(networkTypeBitmap) == testVector);
            return {};
        }
    };
    RadioProxyTestSetup<RadioNetworkProxy> test = sp<TestRadio>::make();
    test->setAllowedNetworkTypesBitmap(s, testVector);
}

TEST(RadioNetworkProxyHook, setBandMode) {
    static int32_t s = INTrand(true);
    static aidl_network::RadioBandMode testVector = aidl_network::RadioBandMode(3);
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> setBandMode(int32_t serial, V1_0::RadioBandMode mode) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            EXPECT_TRUE(static_cast<int32_t>(mode) == static_cast<int32_t>(testVector));
            return {};
        }
    };
    RadioProxyTestSetup<RadioNetworkProxy> test = sp<TestRadio>::make();
    test->setBandMode(s, testVector);
}

TEST(RadioNetworkProxyHook, setBarringPassword) {
    static int32_t s = INTrand(true);
    static struct TestArgs {
        std::string facility;
        std::string oldPassword;
        std::string newPassword;
    } testVector = {
        "facility", "old_pass", "new_pass",
    };
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> setBarringPassword(int32_t serial,
                const hidl_string& facility, const hidl_string& oldPassword,
                const hidl_string& newPassword) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            EXPECT_TRUE(facility == testVector.facility);
            EXPECT_TRUE(oldPassword == testVector.oldPassword);
            EXPECT_TRUE(newPassword == testVector.newPassword);
            return {};
        }
    };
    RadioProxyTestSetup<RadioNetworkProxy> test = sp<TestRadio>::make();
    test->setBarringPassword(s, testVector.facility, testVector.oldPassword, testVector.newPassword);
}

TEST(RadioNetworkProxyHook, setCdmaRoamingPreference) {
    static int32_t s = INTrand(true);
    static aidl_network::CdmaRoamingType testVector = aidl_network::CdmaRoamingType(3);
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> setCdmaRoamingPreference(int32_t serial, V1_0::CdmaRoamingType type) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            EXPECT_TRUE(static_cast<int32_t>(type) == static_cast<int32_t>(testVector));
            return {};
        }
    };
    RadioProxyTestSetup<RadioNetworkProxy> test = sp<TestRadio>::make();
    test->setCdmaRoamingPreference(s, testVector);
}

TEST(RadioNetworkProxyHook, setCellInfoListRate) {
    static int32_t s = INTrand(true);
    static int32_t testVector = 100;
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> setCellInfoListRate(int32_t serial, int32_t rate) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            EXPECT_TRUE(rate == testVector);
            return {};
        }
    };
    RadioProxyTestSetup<RadioNetworkProxy> test = sp<TestRadio>::make();
    test->setCellInfoListRate(s, testVector);
}

TEST(RadioNetworkProxyHook, setIndicationFilter) {
    static int32_t s = INTrand(true);
    static int32_t testVector = 100;
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> setIndicationFilter_1_5(int32_t serial,
                hidl_bitfield<V1_5::IndicationFilter> indicationFilter) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            EXPECT_TRUE(indicationFilter == testVector);
            return {};
        }
    };
    RadioProxyTestSetup<RadioNetworkProxy> test = sp<TestRadio>::make();
    test->setIndicationFilter(s, testVector);
}

TEST(RadioNetworkProxyHook, setLinkCapacityReportingCriteria) {
    static int32_t s = INTrand(true);
    static struct TestArgs {
        int32_t hysteresisMs;
        int32_t hysteresisDlKbps;
        int32_t hysteresisUlKbps;
        std::vector<int32_t> thresholdsDownlinkKbps;
        std::vector<int32_t> thresholdsUplinkKbps;
        aidl_common::AccessNetwork accessNetwork;
    } testVector = {
        1000, 2000, 3000, {1, 2, 3, 4, 5}, {1, 2, 3, 4, 5}, aidl_common::AccessNetwork(14),
    };
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> setLinkCapacityReportingCriteria_1_5(int32_t serial, int32_t hysteresisMs,
                int32_t hysteresisDlKbps, int32_t hysteresisUlKbps, const hidl_vec<int32_t>& thresholdsDownlinkKbps,
                const hidl_vec<int32_t>& thresholdsUplinkKbps, V1_5::AccessNetwork accessNetwork) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            EXPECT_TRUE(hysteresisMs == testVector.hysteresisMs);
            EXPECT_TRUE(hysteresisDlKbps == testVector.hysteresisDlKbps);
            EXPECT_TRUE(hysteresisUlKbps == testVector.hysteresisUlKbps);
            EXPECT_TRUE(thresholdsDownlinkKbps == testVector.thresholdsDownlinkKbps);
            EXPECT_TRUE(thresholdsUplinkKbps == testVector.thresholdsUplinkKbps);
            EXPECT_TRUE(static_cast<int32_t>(accessNetwork) == static_cast<int32_t>(testVector.accessNetwork));
            return {};
        }
    };
    RadioProxyTestSetup<RadioNetworkProxy> test = sp<TestRadio>::make();
    test->setLinkCapacityReportingCriteria(s, testVector.hysteresisMs, testVector.hysteresisDlKbps,
            testVector.hysteresisUlKbps, testVector.thresholdsDownlinkKbps, testVector.thresholdsUplinkKbps,
            testVector.accessNetwork);
}

TEST(RadioNetworkProxyHook, setLocationUpdates) {
    static int32_t s = INTrand(true);
    static bool testVector = true;
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> setLocationUpdates(int32_t serial, bool enable) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            EXPECT_TRUE(enable == testVector);
            return {};
        }
    };
    RadioProxyTestSetup<RadioNetworkProxy> test = sp<TestRadio>::make();
    test->setLocationUpdates(s, testVector);
}

TEST(RadioNetworkProxyHook, setNetworkSelectionModeAutomatic) {
    static int32_t s = INTrand(true);
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> setNetworkSelectionModeAutomatic(int32_t serial) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            return {};
        }
    };
    RadioProxyTestSetup<RadioNetworkProxy> test = sp<TestRadio>::make();
    test->setNetworkSelectionModeAutomatic(s);
}

TEST(RadioNetworkProxyHook, setNetworkSelectionModeManual) {
    struct TestNetwork {
        std::string operatorNumeric;
        AccessNetwork accessNetwork;
        V1_5::RadioAccessNetworks ran;
    };
    static std::initializer_list<TestNetwork> testVector = {
        { "311480", AccessNetwork::UNKNOWN, V1_5::RadioAccessNetworks::UNKNOWN },
        { "45005", AccessNetwork::GERAN, V1_5::RadioAccessNetworks::GERAN },
        { "311480", AccessNetwork::UTRAN, V1_5::RadioAccessNetworks::UTRAN },
        { "45006", AccessNetwork::EUTRAN, V1_5::RadioAccessNetworks::EUTRAN },
        { "00101", AccessNetwork::NGRAN, V1_5::RadioAccessNetworks::NGRAN },
    };
    for (auto& cur : testVector) {
        struct TestRadio : public TestRadioBase_1_6 {
            TestNetwork mTestNetwork;
            TestChecker mChecker;
            TestRadio(const TestNetwork& testNetwork) : mTestNetwork(testNetwork) {}
            virtual Return<void> setNetworkSelectionModeManual_1_5(
                    int32_t /* serial */,
                    const hidl_string& operatorNumeric,
                    V1_5::RadioAccessNetworks ran) override {
                mChecker.set(true);
                EXPECT_TRUE(operatorNumeric == mTestNetwork.operatorNumeric);
                EXPECT_EQ(ran, mTestNetwork.ran);
                return {};
            }
        };
        RadioProxyTestSetup<RadioNetworkProxy> test = sp<TestRadio>::make(cur);
        test->setNetworkSelectionModeManual(INTrand(true), cur.operatorNumeric, cur.accessNetwork);
    }
}

TEST(RadioNetworkProxyHook, setNrDualConnectivityState) {
    static int32_t s = INTrand(true);
    static aidl_network::NrDualConnectivityState testVector = aidl_network::NrDualConnectivityState(3);
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> setNrDualConnectivityState(int32_t serial,
                V1_6::NrDualConnectivityState nrDualConnectivityState) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            EXPECT_TRUE(static_cast<int32_t>(nrDualConnectivityState) == static_cast<int32_t>(testVector));
            return {};
        }
    };
    RadioProxyTestSetup<RadioNetworkProxy> test = sp<TestRadio>::make();
    test->setNrDualConnectivityState(s, testVector);
}

TEST(RadioNetworkProxyHook, setSuppServiceNotifications) {
    static int32_t s = INTrand(true);
    static bool testVector = true;
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> setSuppServiceNotifications(int32_t serial, bool enable) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            EXPECT_TRUE(enable == testVector);
            return {};
        }
    };
    RadioProxyTestSetup<RadioNetworkProxy> test = sp<TestRadio>::make();
    test->setSuppServiceNotifications(s, testVector);
}

TEST(RadioNetworkProxyHook, setSystemSelectionChannels) {
    static int32_t s = INTrand(true);
    static struct TestArgs {
        bool specifyChannels;
        std::vector<aidl_network::RadioAccessSpecifier> specifiers;
    } testVector = {
        true, {{}, {}, {}},
    };
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> setSystemSelectionChannels_1_5(int32_t serial, bool specifyChannels,
                const hidl_vec<V1_5::RadioAccessSpecifier>& specifiers) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            EXPECT_TRUE(specifyChannels == testVector.specifyChannels);
            EXPECT_TRUE(specifiers.size() == testVector.specifiers.size());
            return {};
        }
    };
    RadioProxyTestSetup<RadioNetworkProxy> test = sp<TestRadio>::make();
    test->setSystemSelectionChannels(s, testVector.specifyChannels, testVector.specifiers);
}

TEST(RadioNetworkProxyHook, startNetworkScan) {
    static int32_t s = INTrand(true);
    static aidl_network::NetworkScanRequest testVector = {
        0, 3,
        {{AccessNetwork::EUTRAN, std::initializer_list<aidl_network::EutranBands> {aidl_network::EutranBands::BAND_1, aidl_network::EutranBands::BAND_3, aidl_network::EutranBands::BAND_7}, {}},},
        30000, true, 3000, {"00101", "45005"},
    };
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> startNetworkScan_1_5(int32_t serial, const V1_5::NetworkScanRequest& request) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            EXPECT_TRUE(request.type == V1_1::ScanType{testVector.type});
            EXPECT_TRUE(request.interval == testVector.interval);
            EXPECT_TRUE(request.maxSearchTime == testVector.maxSearchTime);
            EXPECT_TRUE(request.incrementalResults == testVector.incrementalResults);
            EXPECT_TRUE(request.incrementalResultsPeriodicity == testVector.incrementalResultsPeriodicity);
            return {};
        }
    };
    RadioProxyTestSetup<RadioNetworkProxy> test = sp<TestRadio>::make();
    test->startNetworkScan(s, testVector);
}

TEST(RadioNetworkProxyHook, stopNetworkScan) {
    static int32_t s = INTrand(true);
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> stopNetworkScan(int32_t serial) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            return {};
        }
    };
    RadioProxyTestSetup<RadioNetworkProxy> test = sp<TestRadio>::make();
    test->stopNetworkScan(s);
}

TEST(RadioNetworkProxyHook, supplyNetworkDepersonalization) {
    static int32_t s = INTrand(true);
    static std::string testVector = "pin1234";
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> supplyNetworkDepersonalization(int32_t serial, const hidl_string& netPin) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            EXPECT_TRUE(netPin == testVector);
            return {};
        }
    };
    RadioProxyTestSetup<RadioNetworkProxy> test = sp<TestRadio>::make();
    test->supplyNetworkDepersonalization(s, testVector);
}