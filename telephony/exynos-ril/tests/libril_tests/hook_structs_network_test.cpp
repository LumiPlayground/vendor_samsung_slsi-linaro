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
#include "network/hook/structs.h"
#include "commonStructs.h"
#include "collections.h"
#include <aidl/android/hardware/radio/RadioTechnology.h>

using namespace ::android::hardware::radio;
namespace aidl_network = ::aidl::android::hardware::radio::network;
using ::aidl::android::hardware::radio::AccessNetwork;
using ::aidl::android::hardware::radio::RadioTechnology;

TEST(HookStructs, RadioBandMode_toAidl) {
    auto test = compat::toAidl(V1_0::RadioBandMode::BAND_MODE_USA);
    EXPECT_TRUE(test == aidl_network::RadioBandMode(V1_0::RadioBandMode::BAND_MODE_USA));
}

TEST(HookStructs, GeranBands_toAidl) {
    auto testVector = V1_1::GeranBands::BAND_850;
    auto test = compat::toAidl(testVector);
    EXPECT_TRUE(test == aidl_network::GeranBands(testVector));
}

TEST(HookStructs, GeranBands_toHidl) {
    auto testVector = aidl_network::GeranBands::BAND_P900;
    auto test = compat::toHidl(testVector);
    EXPECT_TRUE(test == V1_1::GeranBands(testVector));
}

TEST(HookStructs, UtranBands_toAidl) {
    auto testVector = V1_5::UtranBands::BAND_10;
    auto test = compat::toAidl(testVector);
    EXPECT_TRUE(test == aidl_network::UtranBands(testVector));
}

TEST(HookStructs, UtranBands_toHidl) {
    auto testVector = aidl_network::UtranBands::BAND_10;
    auto test = compat::toHidl(testVector);
    EXPECT_TRUE(test == V1_5::UtranBands(testVector));
}

TEST(HookStructs, EutranBands_toAidl) {
    auto testVector = V1_5::EutranBands::BAND_3;
    auto test = compat::toAidl(testVector);
    EXPECT_TRUE(test == aidl_network::EutranBands(testVector));
}

TEST(HookStructs, EutranBands_toHidl) {
    auto testVector = aidl_network::EutranBands::BAND_3;
    auto test = compat::toHidl(testVector);
    EXPECT_TRUE(test == V1_5::EutranBands(testVector));
}

TEST(HookStructs, NgranBands_toAidl) {
    auto testVector = V1_5::NgranBands::BAND_30;
    auto test = compat::toAidl(testVector);
    EXPECT_TRUE(test == aidl_network::NgranBands(testVector));
}

TEST(HookStructs, NgranBands_toHidl) {
    auto testVector = aidl_network::NgranBands::BAND_30;
    auto test = compat::toHidl(testVector);
    EXPECT_TRUE(test == V1_5::NgranBands(testVector));
}

TEST(HookStructs, SignalThresholdInfo_toHidl) {
    aidl_network::SignalThresholdInfo testVector = {
        10, 1000, 2000, {-50, -40, -30, -20, -10}, true, AccessNetwork::EUTRAN,
    };
    auto test = compat::toHidl(testVector);
    EXPECT_TRUE(test.signalMeasurement == V1_5::SignalMeasurementType{testVector.signalMeasurement});
    EXPECT_TRUE(test.hysteresisMs == testVector.hysteresisMs);
    EXPECT_TRUE(test.hysteresisDb == testVector.hysteresisDb);
    EXPECT_TRUE(test.thresholds == testVector.thresholds);
    EXPECT_TRUE(test.isEnabled == testVector.isEnabled);
}

TEST(HookStructs, RadioAccessSpecifier_toAidl) {
    TestChecker checker;
    V1_5::RadioAccessSpecifier::Bands none;
    V1_5::RadioAccessSpecifier::Bands geranBands;
    V1_5::RadioAccessSpecifier::Bands utranBands;
    V1_5::RadioAccessSpecifier::Bands eutranBands;
    V1_5::RadioAccessSpecifier::Bands ngranBands;
    geranBands.geranBands({V1_1::GeranBands::BAND_T380, V1_1::GeranBands::BAND_T410, V1_1::GeranBands::BAND_710});
    utranBands.utranBands({V1_5::UtranBands::BAND_6, V1_5::UtranBands::BAND_12, V1_5::UtranBands::BAND_20});
    eutranBands.eutranBands({V1_5::EutranBands::BAND_1, V1_5::EutranBands::BAND_3, V1_5::EutranBands::BAND_7});
    ngranBands.ngranBands({V1_5::NgranBands::BAND_25, V1_5::NgranBands::BAND_51, V1_5::NgranBands::BAND_74});
    std::initializer_list<V1_5::RadioAccessSpecifier> testVector = {
        {V1_5::RadioAccessNetworks::GERAN, geranBands, {}},
        {V1_5::RadioAccessNetworks::UTRAN , utranBands, {}},
        {V1_5::RadioAccessNetworks::EUTRAN, eutranBands, {}},
        {V1_5::RadioAccessNetworks::NGRAN, ngranBands, {}},
        {V1_5::RadioAccessNetworks::UNKNOWN, none, {}},
        {V1_5::RadioAccessNetworks::CDMA2000, none, {}},
        {V1_5::RadioAccessNetworks(6), none, {}},
    };
    for (auto& testcase : testVector) {
        compat::toAidl(testcase);
    }
    checker.checked();
}

TEST(HookStructs, RadioAccessSpecifier_toHidl) {
    TestChecker checker;
    std::initializer_list<aidl_network::RadioAccessSpecifier> testVector = {
        {AccessNetwork::GERAN, std::initializer_list<aidl_network::GeranBands> {aidl_network::GeranBands::BAND_T380, aidl_network::GeranBands::BAND_T410, aidl_network::GeranBands::BAND_710}, {}},
        {AccessNetwork::UTRAN, std::initializer_list<aidl_network::UtranBands> {aidl_network::UtranBands::BAND_6, aidl_network::UtranBands::BAND_12, aidl_network::UtranBands::BAND_20}, {}},
        {AccessNetwork::EUTRAN, std::initializer_list<aidl_network::EutranBands> {aidl_network::EutranBands::BAND_1, aidl_network::EutranBands::BAND_3, aidl_network::EutranBands::BAND_7}, {}},
        {AccessNetwork::EUTRAN, std::initializer_list<aidl_network::NgranBands> {aidl_network::NgranBands::BAND_1, aidl_network::NgranBands::BAND_3, aidl_network::NgranBands::BAND_7}, {}},
        {AccessNetwork::CDMA2000, {}, {}},
        {AccessNetwork::UNKNOWN, {}, {}},
        {AccessNetwork::IWLAN, {}, {}},
    };
    for (auto& testcase : testVector) {
        compat::toHidl(testcase);
    }
    checker.checked();
}

TEST(HookStructs, NetworkScanRequest_toHidl) {
    aidl_network::NetworkScanRequest testVector = {
        0, 3,
        {{AccessNetwork::EUTRAN, std::initializer_list<aidl_network::EutranBands> {aidl_network::EutranBands::BAND_1, aidl_network::EutranBands::BAND_3, aidl_network::EutranBands::BAND_7}, {}},},
        30000, true, 3000, {"00101", "45005"},
    };
    auto test = compat::toHidl(testVector);
    EXPECT_TRUE(test.type == V1_1::ScanType{testVector.type});
    EXPECT_TRUE(test.interval == testVector.interval);
    EXPECT_TRUE(test.maxSearchTime == testVector.maxSearchTime);
    EXPECT_TRUE(test.incrementalResults == testVector.incrementalResults);
    EXPECT_TRUE(test.incrementalResultsPeriodicity == testVector.incrementalResultsPeriodicity);
    EXPECT_TRUE(test.mccMncs == compat::toHidl(testVector.mccMncs));
}

TEST(HookStructs, CellIdentity_toAidl) {
    TestChecker checker;
    V1_5::CellIdentity noinit;
    V1_5::CellIdentity gsm;
    V1_5::CellIdentity wcdma;
    V1_5::CellIdentity tdscdma;
    V1_5::CellIdentity cdma;
    V1_5::CellIdentity lte;
    V1_5::CellIdentity nr;
    V1_5::OptionalCsgInfo opt;
    opt.csgInfo({true, "csg001", 1234,});
    gsm.gsm({});
    wcdma.wcdma({.optionalCsgInfo = opt});
    tdscdma.tdscdma({});
    cdma.cdma({});
    lte.lte({});
    nr.nr({});
    std::initializer_list<V1_5::CellIdentity> testVector = {
        noinit, gsm, wcdma, tdscdma, cdma, lte, nr,
    };

    for (auto& testcase : testVector) {
        compat::toAidl(testcase);
    }
    checker.checked();
}

TEST(HookStructs, BarringInfo_toAidl) {
    TestChecker checker;
    V1_5::BarringInfo::BarringTypeSpecificInfo noinit;
    V1_5::BarringInfo::BarringTypeSpecificInfo conditional;
    conditional.conditional({});
    std::initializer_list<V1_5::BarringInfo> testVector = {
        {V1_5::BarringInfo::ServiceType::CS_SERVICE, V1_5::BarringInfo::BarringType::NONE, noinit},
        {V1_5::BarringInfo::ServiceType::CS_SERVICE, V1_5::BarringInfo::BarringType::CONDITIONAL, conditional},
    };

    for (auto& testcase : testVector) {
        compat::toAidl(testcase);
    }
    checker.checked();
}

TEST(HookStructs, ClosedSubscriberGroupInfo_toAidl) {
    V1_5::ClosedSubscriberGroupInfo testVector = {
        true, "csg001", 1234,
    };
    auto test = compat::toAidl(testVector);
    EXPECT_TRUE(test.csgIndication == testVector.csgIndication);
    EXPECT_TRUE(test.homeNodebName == testVector.homeNodebName);
    EXPECT_TRUE(test.csgIdentity == testVector.csgIdentity);
}

TEST(HookStructs, CellInfo_V1_5_toAidl) {
    TestChecker checker;
    V1_5::CellInfo::CellInfoRatSpecificInfo gsm;
    V1_5::CellInfo::CellInfoRatSpecificInfo wcdma;
    V1_5::CellInfo::CellInfoRatSpecificInfo tdscdma;
    V1_5::CellInfo::CellInfoRatSpecificInfo lte;
    V1_5::CellInfo::CellInfoRatSpecificInfo nr;
    V1_5::CellInfo::CellInfoRatSpecificInfo cdma;
    gsm.gsm({});
    wcdma.wcdma({});
    tdscdma.tdscdma({});
    lte.lte({});
    nr.nr({});
    cdma.cdma({});
    std::initializer_list<V1_5::CellInfo> testVector = {
        {true, V1_0::TimeStampType::OEM_RIL, 0, V1_2::CellConnectionStatus::PRIMARY_SERVING, gsm},
        {true, V1_0::TimeStampType::OEM_RIL, 0, V1_2::CellConnectionStatus::PRIMARY_SERVING, wcdma},
        {true, V1_0::TimeStampType::OEM_RIL, 0, V1_2::CellConnectionStatus::PRIMARY_SERVING, tdscdma},
        {true, V1_0::TimeStampType::OEM_RIL, 0, V1_2::CellConnectionStatus::PRIMARY_SERVING, lte},
        {true, V1_0::TimeStampType::OEM_RIL, 0, V1_2::CellConnectionStatus::PRIMARY_SERVING, nr},
        {true, V1_0::TimeStampType::OEM_RIL, 0, V1_2::CellConnectionStatus::PRIMARY_SERVING, cdma},
    };
    for (auto& testcase : testVector) {
        compat::toAidl(testcase);
    }
    checker.checked();
}

TEST(HookStructs, CellInfo_V1_6_toAidl) {
    TestChecker checker;
    V1_6::CellInfo::CellInfoRatSpecificInfo gsm;
    V1_6::CellInfo::CellInfoRatSpecificInfo wcdma;
    V1_6::CellInfo::CellInfoRatSpecificInfo tdscdma;
    V1_6::CellInfo::CellInfoRatSpecificInfo lte;
    V1_6::CellInfo::CellInfoRatSpecificInfo nr;
    V1_6::CellInfo::CellInfoRatSpecificInfo cdma;
    gsm.gsm({});
    wcdma.wcdma({});
    tdscdma.tdscdma({});
    lte.lte({});
    nr.nr({});
    cdma.cdma({});
    std::initializer_list<V1_6::CellInfo> testVector = {
        {true, V1_2::CellConnectionStatus::PRIMARY_SERVING, gsm},
        {true, V1_2::CellConnectionStatus::PRIMARY_SERVING, wcdma},
        {true, V1_2::CellConnectionStatus::PRIMARY_SERVING, tdscdma},
        {true, V1_2::CellConnectionStatus::PRIMARY_SERVING, lte},
        {true, V1_2::CellConnectionStatus::PRIMARY_SERVING, nr},
        {true, V1_2::CellConnectionStatus::PRIMARY_SERVING, cdma},
    };
    for (auto& testcase : testVector) {
        compat::toAidl(testcase);
    }
    checker.checked();
}

TEST(HookStructs, LinkCapacityEstimate_V1_2_toAidl) {
    V1_2::LinkCapacityEstimate testVector = {
        1234, 5678
    };
    auto test = compat::toAidl(testVector);
    EXPECT_TRUE(test.downlinkCapacityKbps == static_cast<int32_t>(testVector.downlinkCapacityKbps));
    EXPECT_TRUE(test.uplinkCapacityKbps == static_cast<int32_t>(testVector.uplinkCapacityKbps));
}

TEST(HookStructs, LinkCapacityEstimate_V1_6_toAidl) {
    V1_6::LinkCapacityEstimate testVector = {
        1234, 5678, 1357, 2468
    };
    auto test = compat::toAidl(testVector);
    EXPECT_TRUE(test.downlinkCapacityKbps == static_cast<int32_t>(testVector.downlinkCapacityKbps));
    EXPECT_TRUE(test.uplinkCapacityKbps == static_cast<int32_t>(testVector.uplinkCapacityKbps));
    EXPECT_TRUE(test.secondaryDownlinkCapacityKbps == static_cast<int32_t>(testVector.secondaryDownlinkCapacityKbps));
    EXPECT_TRUE(test.secondaryUplinkCapacityKbps == static_cast<int32_t>(testVector.secondaryUplinkCapacityKbps));
}

TEST(HookStructs, PhysicalChannelConfig_V1_4_toAidl) {
    V1_4::RadioFrequencyInfo range;
    V1_4::RadioFrequencyInfo channel;
    range.range(V1_4::FrequencyRange::HIGH);
    channel.channelNumber(1400);
    std::initializer_list<V1_4::PhysicalChannelConfig> testVector = {
        {{V1_2::CellConnectionStatus::PRIMARY_SERVING, 20}, V1_4::RadioTechnology::LTE, range, {}, 0},
        {{V1_2::CellConnectionStatus::PRIMARY_SERVING, 20}, V1_4::RadioTechnology::LTE, channel, {}, 0},
    };
    for (auto& testcase : testVector) {
        auto test = compat::toAidl(testcase);
        EXPECT_TRUE(test.status == aidl_network::CellConnectionStatus(testcase.base.status));
        EXPECT_TRUE(test.rat == RadioTechnology(testcase.rat));
        EXPECT_TRUE(test.cellBandwidthDownlinkKhz == testcase.base.cellBandwidthDownlink);
        EXPECT_TRUE(test.contextIds == static_cast<std::vector<int32_t>>(testcase.contextIds));
        EXPECT_TRUE(test.physicalCellId == static_cast<int32_t>(testcase.physicalCellId));
    }
}

TEST(HookStructs, PhysicalChannelConfig_V1_6_toAidl) {
    V1_6::PhysicalChannelConfig::Band geranBand;
    V1_6::PhysicalChannelConfig::Band utranBand;
    V1_6::PhysicalChannelConfig::Band eutranBand;
    V1_6::PhysicalChannelConfig::Band ngranBand;
    geranBand.geranBand(V1_1::GeranBands::BAND_450);
    utranBand.utranBand(V1_1::UtranBands::BAND_1);
    eutranBand.eutranBand(V1_1::EutranBands::BAND_5);
    ngranBand.ngranBand(V1_6::NgranBands::BAND_8);

    std::initializer_list<V1_6::PhysicalChannelConfig> testVector = {
        {V1_2::CellConnectionStatus::PRIMARY_SERVING, V1_4::RadioTechnology::EDGE, 1500, 1600, 10000, 20000, {}, 65300, geranBand},
        {V1_2::CellConnectionStatus::PRIMARY_SERVING, V1_4::RadioTechnology::UMTS, 1500, 1600, 10000, 20000, {}, 65300, utranBand},
        {V1_2::CellConnectionStatus::PRIMARY_SERVING, V1_4::RadioTechnology::LTE, 1500, 1600, 10000, 20000, {}, 65300, eutranBand},
        {V1_2::CellConnectionStatus::PRIMARY_SERVING, V1_4::RadioTechnology::NR, 1500, 1600, 10000, 20000, {}, 65300, ngranBand},
    };
    for (auto& testcase : testVector) {
        auto test = compat::toAidl(testcase);
        EXPECT_TRUE(test.status == aidl_network::CellConnectionStatus(testcase.status));
        EXPECT_TRUE(test.rat == RadioTechnology(testcase.rat));
        EXPECT_TRUE(test.downlinkChannelNumber == testcase.downlinkChannelNumber);
        EXPECT_TRUE(test.uplinkChannelNumber == testcase.uplinkChannelNumber);
        EXPECT_TRUE(test.cellBandwidthDownlinkKhz == testcase.cellBandwidthDownlinkKhz);
        EXPECT_TRUE(test.cellBandwidthUplinkKhz == testcase.cellBandwidthUplinkKhz);
        EXPECT_TRUE(test.contextIds == static_cast<std::vector<int32_t>>(testcase.contextIds));
        EXPECT_TRUE(test.physicalCellId == static_cast<int32_t>(testcase.physicalCellId));
    }
}

TEST(HookStructs, SignalStrength_V1_4_toAidl) {
    TestChecker checker;
    V1_4::SignalStrength testVector = {};
    auto test = compat::toAidl(testVector);
    checker.checked();
}

TEST(HookStructs, SignalStrength_V1_6_toAidl) {
    TestChecker checker;
    V1_6::SignalStrength testVector = {};
    auto test = compat::toAidl(testVector);
    checker.checked();
}

TEST(HookStructs, NetworkScanResult_V1_5_toAidl) {
    V1_5::NetworkScanResult testVector = {
        V1_1::ScanStatus::PARTIAL, V1_0::RadioError::NONE, {},
    };
    auto test = compat::toAidl(testVector);
    EXPECT_TRUE(test.status == static_cast<int32_t>(testVector.status));
    EXPECT_TRUE(static_cast<int32_t>(test.error) == static_cast<int32_t>(testVector.error));
    EXPECT_TRUE(test.networkInfos == compat::toAidl(testVector.networkInfos));
}

TEST(HookStructs, NetworkScanResult_V1_6_toAidl) {
    V1_6::NetworkScanResult testVector = {
        V1_1::ScanStatus::PARTIAL, V1_6::RadioError::NONE, {},
    };
    auto test = compat::toAidl(testVector);
    EXPECT_TRUE(test.status == static_cast<int32_t>(testVector.status));
    EXPECT_TRUE(static_cast<int32_t>(test.error) == static_cast<int32_t>(testVector.error));
    EXPECT_TRUE(test.networkInfos == compat::toAidl(testVector.networkInfos));
}

TEST(HookStructs, SuppSvcNotification_toAidl) {
    V1_0::SuppSvcNotification testVector = {
        true, 10, 1, 2, "12345"
    };
    auto test = compat::toAidl(testVector);
    EXPECT_TRUE(test.isMT == testVector.isMT);
    EXPECT_TRUE(test.code == testVector.code);
    EXPECT_TRUE(test.index == testVector.index);
    EXPECT_TRUE(test.type == testVector.type);
    EXPECT_TRUE(test.number == testVector.number);
}

TEST(HookStructs, OperatorInfo_toAidl) {
    V1_0::OperatorInfo testVector = {
        "alpha long", "alpha short", "00101", V1_0::OperatorStatus::AVAILABLE
    };
    auto test = compat::toAidl(testVector);
    EXPECT_TRUE(test.alphaLong == testVector.alphaLong);
    EXPECT_TRUE(test.alphaShort == testVector.alphaShort);
    EXPECT_TRUE(test.operatorNumeric == testVector.operatorNumeric);
    EXPECT_TRUE(test.status == static_cast<int32_t>(testVector.status));
}

TEST(HookStructs, RegStateResult_V1_5_toAidl) {
    V1_5::RegStateResult::AccessTechnologySpecificInfo noinit;
    V1_5::RegStateResult::AccessTechnologySpecificInfo cdmaInfo;
    V1_5::RegStateResult::AccessTechnologySpecificInfo eutranInfo;
    cdmaInfo.cdmaInfo({});
    eutranInfo.eutranInfo({});
    std::initializer_list<V1_5::RegStateResult> testVector = {
        {V1_0::RegState::REG_HOME, V1_4::RadioTechnology::UMTS, V1_5::RegistrationFailCause::NONE, {}, "00101", noinit},
        {V1_0::RegState::REG_HOME, V1_4::RadioTechnology::ONE_X_RTT, V1_5::RegistrationFailCause::NONE, {}, "00101", cdmaInfo},
        {V1_0::RegState::REG_HOME, V1_4::RadioTechnology::LTE, V1_5::RegistrationFailCause::NONE, {}, "00101", eutranInfo},
    };
    for (auto& testcase : testVector) {
        auto test = compat::toAidl(testcase);
        EXPECT_TRUE(test.regState == aidl_network::RegState(testcase.regState));
        EXPECT_TRUE(test.rat == RadioTechnology(testcase.rat));
        EXPECT_TRUE(test.reasonForDenial == aidl_network::RegistrationFailCause(testcase.reasonForDenial));
        EXPECT_TRUE(test.cellIdentity == compat::toAidl(testcase.cellIdentity));
        EXPECT_TRUE(test.registeredPlmn == testcase.registeredPlmn);
    }
}

TEST(HookStructs, RegStateResult_V1_6_toAidl) {
    V1_6::RegStateResult::AccessTechnologySpecificInfo noinit;
    V1_6::RegStateResult::AccessTechnologySpecificInfo cdmaInfo;
    V1_6::RegStateResult::AccessTechnologySpecificInfo eutranInfo;
    V1_6::RegStateResult::AccessTechnologySpecificInfo ngranNrVopsInfo;
    V1_6::RegStateResult::AccessTechnologySpecificInfo geranDtmSupported;
    cdmaInfo.cdmaInfo({});
    eutranInfo.eutranInfo({});
    ngranNrVopsInfo.ngranNrVopsInfo({});
    geranDtmSupported.geranDtmSupported(true);
    std::initializer_list<V1_6::RegStateResult> testVector = {
        {V1_0::RegState::REG_HOME, V1_4::RadioTechnology::UMTS, V1_5::RegistrationFailCause::NONE, {}, "00101", noinit},
        {V1_0::RegState::REG_HOME, V1_4::RadioTechnology::ONE_X_RTT, V1_5::RegistrationFailCause::NONE, {}, "00101", cdmaInfo},
        {V1_0::RegState::REG_HOME, V1_4::RadioTechnology::LTE, V1_5::RegistrationFailCause::NONE, {}, "00101", eutranInfo},
        {V1_0::RegState::REG_HOME, V1_4::RadioTechnology::NR, V1_5::RegistrationFailCause::NONE, {}, "00101", ngranNrVopsInfo},
        {V1_0::RegState::REG_HOME, V1_4::RadioTechnology::EDGE, V1_5::RegistrationFailCause::NONE, {}, "00101", geranDtmSupported},
    };
    for (auto& testcase : testVector) {
        auto test = compat::toAidl(testcase);
        EXPECT_TRUE(test.regState == aidl_network::RegState(testcase.regState));
        EXPECT_TRUE(test.rat == RadioTechnology(testcase.rat));
        EXPECT_TRUE(test.reasonForDenial == aidl_network::RegistrationFailCause(testcase.reasonForDenial));
        EXPECT_TRUE(test.cellIdentity == compat::toAidl(testcase.cellIdentity));
        EXPECT_TRUE(test.registeredPlmn == testcase.registeredPlmn);
    }
}

TEST(HookStructs, LceDataInfo_toAidl) {
    V1_0::LceDataInfo testVector = {
        100, 15, true
    };
    auto test = compat::toAidl(testVector);
    EXPECT_TRUE(test.lastHopCapacityKbps == static_cast<int32_t>(testVector.lastHopCapacityKbps));
    EXPECT_TRUE(test.confidenceLevel == static_cast<int8_t>(testVector.confidenceLevel));
    EXPECT_TRUE(test.lceSuspended == testVector.lceSuspended);
}