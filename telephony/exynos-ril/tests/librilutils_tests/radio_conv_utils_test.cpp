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
 * radio_conv_utils_test.cpp
 *
 *  Created on: 2021. 7. 14.
 */
#include <gtest/gtest.h>
#include <librilutils/radio_conv_utils.h>

TEST(RadioConvUtils, convertCellInfo_convertSignalStrengthLte) {
    RIL_CellInfo_V1_6 cellInfolte = {
        .cellInfoType = RIL_CELL_INFO_TYPE_LTE,
        .registered = true,
        .timeStampType = RIL_TIMESTAMP_TYPE_OEM_RIL,
        .connectionStatus = PRIMARY_SERVING,
        .CellInfo.lte.cellIdentityLte = {
                INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX,
                {}, INT_MAX, 0, nullptr, { false, {}, INT_MAX }, 0, {}
        },
        .CellInfo.lte.signalStrengthLte = { { 30, 20, -100, 0, 0, 0 }, 10 },
        .CellInfo.lte.isEndcAvailable = false,
    };

    RIL_CellInfo_V1_5 out {};
    RadioConvUtils::convertCellInfo(out, cellInfolte);

    RIL_LTE_SignalStrength_v8& test = out.CellInfo.lte.signalStrengthLte;
    RIL_LTE_SignalStrength_V1_6& lte = cellInfolte.CellInfo.lte.signalStrengthLte;
    EXPECT_EQ(test.signalStrength, lte.signalStrength);
    EXPECT_EQ(test.rsrp, lte.rsrp);
    EXPECT_EQ(test.rsrq, lte.rsrq);
    EXPECT_EQ(test.rssnr, lte.rssnr);
    EXPECT_EQ(test.cqi, lte.cqi);
    EXPECT_EQ(test.timingAdvance, lte.timingAdvance);
}

TEST(RadioConvUtils, convertCellInfo_convertSignalStrengthNr) {
    RIL_CellInfo_V1_6 cellInfoNr = {
        .cellInfoType = (RIL_CellInfoType)RIL_CELL_INFO_TYPE_NR,
        .registered = true,
        .timeStampType = RIL_TIMESTAMP_TYPE_OEM_RIL,
        .connectionStatus = PRIMARY_SERVING,
        .CellInfo.nr.cellIdentityNr = {
                INT_MAX, INT_MAX, LONG_MAX, INT_MAX, INT_MAX, INT_MAX,
                {}, 0, nullptr, 0, {},
        },
        .CellInfo.nr.signalStrengthNr = { { INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX }, INT_MAX, 0, {} },
    };

    RIL_CellInfo_V1_5 out {};
    RadioConvUtils::convertCellInfo(out, cellInfoNr);

    RIL_NR_SignalStrength_V1_4& test = out.CellInfo.nr.signalStrengthNr;
    RIL_NR_SignalStrength_V1_6 nr = cellInfoNr.CellInfo.nr.signalStrengthNr;
    EXPECT_EQ(test.ssRsrp, nr.ssRsrp);
    EXPECT_EQ(test.ssRsrq, nr.ssRsrq);
    EXPECT_EQ(test.ssSinr, nr.ssSinr);
    EXPECT_EQ(test.csiRsrp, nr.csiRsrp);
    EXPECT_EQ(test.csiRsrq, nr.csiRsrq);
    EXPECT_EQ(test.csiSinr, nr.csiSinr);
}

TEST(RadioConvUtils, convertPhysicalChannelConfig_V1_4) {
    RIL_PhysicalChannelConfig test {};
    RIL_PhysicalChannelConfig_V1_4 config = {
        .status = RIL_CellConnectionStatus::PRIMARY_SERVING,
        .cellBandwidthDownlink = 100,
    };
    RadioConvUtils::convertPhysicalChannelConfig(test, config);
    EXPECT_EQ((int)test.status, config.status);
    EXPECT_EQ(test.cellBandwidthDownlink, config.cellBandwidthDownlink);

}
TEST(RadioConvUtils, convertPhysicalChannelConfig_V1_6) {
    RIL_PhysicalChannelConfig_V1_4 test {};
    RIL_PhysicalChannelConfig_V1_6 config = {
        RIL_CellConnectionStatus::PRIMARY_SERVING, (RIL_RadioTechnology)RADIO_TECH_NR,
        1000, 1100, 100, 40, 0, 0, 2000, { .ngranBand = NGRAN_BAND_1 },
    };
    RadioConvUtils::convertPhysicalChannelConfig(test, config);
    EXPECT_EQ(test.status, (int)config.status);
    EXPECT_EQ(test.rat, config.rat);
    EXPECT_EQ(test.cellBandwidthDownlink, config.cellBandwidthDownlink);
    EXPECT_EQ(test.rfInfoType, 2);
    EXPECT_EQ(test.rfInfo.channelNumber, config.downlinkChannelNumber);
    EXPECT_EQ(test.len_contextIds, config.len_contextIds);
    EXPECT_EQ(test.physicalCellId, config.physicalCellId);
}

TEST(RadioConvUtils, convertVoiceRegistrationStateResult) {
    RIL_RegStateResult test {};
    RIL_RegStateResult_V1_6 voiceRegStateResult = {
        RIL_RegState::RIL_REG_HOME,
        RIL_RadioTechnology::RADIO_TECH_1xRTT,
        REG_FAIL_CAUSE_NONE,
        {}, {}, {},
    };
    RadioConvUtils::convertVoiceRegistrationStateResult(test, voiceRegStateResult);
    EXPECT_EQ(test.regState, voiceRegStateResult.regState);
    EXPECT_EQ(test.rat, voiceRegStateResult.rat);
    EXPECT_EQ(test.reasonForDenial, voiceRegStateResult.reasonForDenial);
}

static char test_apn[] = "test";
static char test_user[] = "user";
static char test_password[] = "password";

RIL_DataProfileInfo_V1_5 dataProfileInfo_V1_5 = {
    .profileId = RIL_DataProfileId::DEFAULT,
    .apn = &test_apn[0],
    .protocol = PdpProtocolType::PDP_PROTOCOL_TYPE_IPV4V6,
    .roamingProtocol = PdpProtocolType::PDP_PROTOCOL_TYPE_IPV4V6,
    .authType = RIL_ApnAuthType::NO_PAP_NO_CHAP,
    .user = &test_user[0],
    .password = &test_password[0],
    .type = THREE_GPP,
    .maxConnsTime = 0,
    .maxConns = 0,
    .waitTime = 0,
    .enabled = true,
    .supportedApnTypesBitmap = 0xFFFFFFFF,
    .bearerBitmap = 0xFFFFFFFF,
    .mtuV4 = 1500,
    .mtuV6 = 1500,
    .preferred = true,
    .persistent = true
};

RIL_DataProfileInfo_V1_5 dataProfileInfo_V1_5_null = {
    .profileId = RIL_DataProfileId::DEFAULT,
    .apn = nullptr,
    .protocol = PdpProtocolType::PDP_PROTOCOL_TYPE_IPV4V6,
    .roamingProtocol = PdpProtocolType::PDP_PROTOCOL_TYPE_IPV4V6,
    .authType = RIL_ApnAuthType::NO_PAP_NO_CHAP,
    .user = nullptr,
    .password = nullptr,
    .type = THREE_GPP,
    .maxConnsTime = 0,
    .maxConns = 0,
    .waitTime = 0,
    .enabled = true,
    .supportedApnTypesBitmap = 0xFFFFFFFF,
    .bearerBitmap = 0xFFFFFFFF,
    .mtuV4 = 1500,
    .mtuV6 = 1500,
    .preferred = true,
    .persistent = true
};

TEST(RadioConvUtils, copyDataProfileInfo_V1_5_to_V2_0) {
    RIL_DataProfileInfo_V1_5 &src = dataProfileInfo_V1_5;
    DataV2_0::RIL_DataProfileInfo dst;
    RadioConvUtils::CopyDataProfileInfo(dst, src);
    EXPECT_EQ(dst.apn, std::string(src.apn));
    EXPECT_EQ(dst.user, std::string(src.user));
    EXPECT_EQ(dst.password, std::string(src.password));

    RIL_DataProfileInfo_V1_5 &src_null = dataProfileInfo_V1_5_null;
    RadioConvUtils::CopyDataProfileInfo(dst, src_null);
    EXPECT_EQ(dst.apn, std::string(""));
    EXPECT_EQ(dst.user, std::string(""));
    EXPECT_EQ(dst.password, std::string(""));
    // Other field is equivalent
}
