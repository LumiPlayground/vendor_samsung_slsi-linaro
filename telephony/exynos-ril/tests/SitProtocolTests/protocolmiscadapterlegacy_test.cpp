/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd All Rights Reserved
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
#include <string.h>
#include "modemdatabuilder.h"
#include "modemdata.h"
#include "legacy/misc/protocolmiscadapterlegacy.h"
#include "sitdef.h"
#include "rildef.h"
#include <librilutils/textutils.h>
#include <slsi/radio_v2_0.h>
#include "modemdata.h"

#include <iostream>
using namespace std;

TEST(ProtocolMiscAdapterLegacy, ProtocolMiscResponseAdapterLegacy) {
    struct Test {
        uint32_t sitErrorCode;
        uint32_t errorCode;
    };
    std::vector<Test> testVector = {
        {RCM_E_SUCCESS, RIL_E_SUCCESS},
        {RCM_E_RADIO_NOT_AVAILABLE, RIL_E_RADIO_NOT_AVAILABLE},
        {RCM_E_GENERIC_FAILURE, RCM_E_GENERIC_FAILURE},
        {RCM_E_SIM_ABSENT, RIL_E_SIM_ABSENT},
    };

    for (auto& t : testVector) {
        auto modemData = std::unique_ptr<ModemData>(ModemDataBuilder::BuildResponse(
                SIT_GET_SIGNAL_STRENGTH, 0, t.sitErrorCode));
        ProtocolMiscResponseAdapterLegacy testAdapter(modemData.get());
        EXPECT_TRUE(testAdapter.GetErrorCode() == t.errorCode);
    }
}

TEST(ProtocolMiscAdapterLegacy, ProtocolMiscVersionAdapterLegacy) {

    sit_misc_get_baseband_version_rsp testResp;
    memset(&testResp, 0, sizeof(testResp));
    testResp.ver_mask = SIT_MISC_VER_MASK_SW_VERSION | SIT_MISC_VER_MASK_HW_VERSION;
    const char *TEST_SW_VER = "baseband_ver_1234";
    const char *TEST_HW_VER = "device_re_5678";
    strncpy(testResp.sw_version, TEST_SW_VER, strlen(TEST_SW_VER));
    strncpy(testResp.hw_version, TEST_HW_VER, strlen(TEST_HW_VER));

    char *data = ((sit_simple_data_format *)&testResp)->data;
    size_t datalen = sizeof(testResp) - sizeof(RCM_HEADER);
    ModemData *modemData = ModemDataBuilder::BuildResponse(
            SIT_GET_BASEBAND_VERSION, 0, RCM_E_SUCCESS,
            data, datalen);
    ASSERT_TRUE(modemData != nullptr);

    ProtocolMiscVersionAdapterLegacy testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_BASEBAND_VERSION);
    EXPECT_EQ(testAdapter.GetMask(), SIT_MISC_VER_MASK_SW_VERSION | SIT_MISC_VER_MASK_HW_VERSION);
    EXPECT_TRUE(TextUtils::Equals(testAdapter.GetSwVer(), TEST_SW_VER));
    EXPECT_TRUE(TextUtils::Equals(testAdapter.GetHwVer(), TEST_HW_VER));
    EXPECT_TRUE(TextUtils::IsEmpty(testAdapter.GetRfCalDate()));
    EXPECT_TRUE(TextUtils::IsEmpty(testAdapter.GetProdCode()));
    EXPECT_TRUE(TextUtils::IsEmpty(testAdapter.GetModelID()));
    EXPECT_EQ(testAdapter.GetPrlNamNum(), 0);
    EXPECT_TRUE(TextUtils::IsEmpty((const char *)testAdapter.GetPrlVersion()));
    EXPECT_EQ(testAdapter.GetEriNamNum(), 0);
    EXPECT_TRUE(TextUtils::IsEmpty((const char *)testAdapter.GetEriVersion()));
    EXPECT_TRUE(TextUtils::IsEmpty((const char *)testAdapter.GetCPChipSet()));

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolMiscAdapterLegacy, ProtocolMiscGetTtyAdapterLegacy) {
    int ttymode[] = {
            SIT_MISC_TTY_MODE_OFF,
            SIT_MISC_TTY_MODE_FULL,
            SIT_MISC_TTY_MODE_HCO,
            SIT_MISC_TTY_MODE_VCO,
    };

    int size = sizeof(ttymode) / sizeof(ttymode[0]);
    for (int i = 0; i < size; i++) {
        ModemData *modemData = ModemDataBuilder::BuildResponse(
                SIT_GET_TTY_MODE, 0, RCM_E_SUCCESS,
                ttymode + i, sizeof(int));
        ASSERT_TRUE(modemData != nullptr);

        ProtocolMiscGetTtyAdapterLegacy testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);
        EXPECT_EQ(testAdapter.GetId(), SIT_GET_TTY_MODE);
        EXPECT_EQ(testAdapter.GetTtyMode(), ttymode[i]);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    }  // endf for i ~
}

TEST(ProtocolMiscAdapterLegacy, ProtocolMiscIMEIAdapterLegacy) {
    sit_id_get_imei_rsp testResp;
    memset(&testResp, 0, sizeof(testResp));
    char TEST_IMEI[] = { 0x86, 0x73, 0x88, 0x02, 0x00, 0x31, 0x89, 0x50 };
    char TEST_IMEI_WRONG[] = { 0x86, 0x73, 0x88, 0x02, 0x00, 0x31, 0x89, 0x51 };
    memcpy(testResp.imei, TEST_IMEI, sizeof(TEST_IMEI));
    testResp.imei_len = sizeof(TEST_IMEI);

    char *data = ((sit_simple_data_format *)&testResp)->data;
    size_t datalen = sizeof(testResp) - sizeof(RCM_HEADER);
    ModemData *modemData = ModemDataBuilder::BuildResponse(
            SIT_GET_IMEI, 0, RCM_E_SUCCESS,
            data, datalen);
    ASSERT_TRUE(modemData != nullptr);

    ProtocolMiscIMEIAdapterLegacy testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_IMEI);
    EXPECT_EQ((unsigned long)/* NEED TO CHECK */ testAdapter.GetIMEILen(), sizeof(TEST_IMEI));
    EXPECT_TRUE(memcmp(testAdapter.GetIMEI(), TEST_IMEI, sizeof(TEST_IMEI)) == 0);
    EXPECT_FALSE(memcmp(testAdapter.GetIMEI(), TEST_IMEI_WRONG, sizeof(TEST_IMEI_WRONG)) == 0);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolMiscAdapterLegacy, ProtocolMiscIMEISVAdapterLegacy) {
    sit_id_get_imeisv_rsp testResp;
    memset(&testResp, 0, sizeof(testResp));
    char TEST_IMEISV[] = { 0x86, 0x73, 0x88, 0x02, 0x00, 0x31, 0x89, 0x50, 0x00, 0x01 };
    char TEST_IMEISV_WRONG[] = { 0x86, 0x73, 0x88, 0x02, 0x00, 0x31, 0x89, 0x51, 0x00, 0x01 };
    memcpy(testResp.imeisv, TEST_IMEISV, sizeof(TEST_IMEISV));
    testResp.imeisv_len = sizeof(TEST_IMEISV);

    char *data = ((sit_simple_data_format *)&testResp)->data;
    size_t datalen = sizeof(testResp) - sizeof(RCM_HEADER);
    ModemData *modemData = ModemDataBuilder::BuildResponse(
            SIT_GET_IMEISV, 0, RCM_E_SUCCESS,
            data, datalen);
    ASSERT_TRUE(modemData != nullptr);

    ProtocolMiscIMEISVAdapterLegacy testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_IMEISV);
    EXPECT_EQ((unsigned int)/* NEED TO CHECK */ testAdapter.GetIMEISVLen(), sizeof(TEST_IMEISV));
    EXPECT_TRUE(memcmp(testAdapter.GetIMEISV(), TEST_IMEISV, sizeof(TEST_IMEISV)) == 0);
    EXPECT_FALSE(memcmp(testAdapter.GetIMEISV(), TEST_IMEISV_WRONG, sizeof(TEST_IMEISV_WRONG)) == 0);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolMiscAdapterLegacy, ProtocolMiscDeviceIDAdapterLegacy) {

    char testPacket[] = {
        0x01, 0x00, 0x03, 0x04, 0x49, 0x00, 0x16, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x38, 0x36, 0x37,
        0x33, 0x38, 0x38, 0x30, 0x32, 0x30, 0x30, 0x33, 0x31, 0x38, 0x39, 0x35, 0x02, 0x30, 0x31, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0b, 0x31,
        0x32, 0x38, 0x30, 0x37, 0x38, 0x35, 0x39, 0x35, 0x30, 0x38, 0x0e, 0x30, 0x30, 0x30, 0x30, 0x30,
        0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30
    };
    sit_id_get_deviceid_rsp* testResp = (sit_id_get_deviceid_rsp *)(testPacket);

    ModemData modemData((char *)&testPacket, sizeof(testPacket));
    ProtocolMiscDeviceIDAdapterLegacy testAdapter(&modemData);
    EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_DEVICE_ID);
    EXPECT_EQ(testAdapter.GetIMEILen(), testResp->imei_len);
    EXPECT_TRUE(memcmp(testAdapter.GetIMEI(), testResp->imei, (size_t)testResp->imei_len) == 0);
    EXPECT_EQ(testAdapter.GetIMEISVLen(), testResp->imeisv_len);
    EXPECT_TRUE(memcmp(testAdapter.GetIMEISV(), testResp->imesv, (size_t)testResp->imeisv_len) == 0);
    EXPECT_EQ(testAdapter.GetESNLen(), testResp->esn_len);
    EXPECT_TRUE(memcmp(testAdapter.GetESN(), testResp->esn, (size_t)testResp->esn_len) == 0);
    EXPECT_EQ(testAdapter.GetMEIDLen(), testResp->meid_len);
    EXPECT_TRUE(memcmp(testAdapter.GetMEID(), testResp->meid, (size_t)testResp->meid_len) == 0);
}

TEST(ProtocolMiscAdapterLegacy, ProtocolMiscOemDisplayEngAdapterLegacy) {

    char testPacket[] = {
        0x02, 0x00, 0x0a, 0x09, 0xf7, 0x00, 0x00, 0x00, 0x06, 0x01, 0x53, 0x65, 0x6c, 0x65, 0x63, 0x74,
        0x20, 0x53, 0x49, 0x4d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d,
        0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d,
        0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x00, 0x00, 0x00, 0x03, 0x31, 0x29, 0x53, 0x49,
        0x4d, 0x20, 0x31, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x3d, 0x3d, 0x3d,
        0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d,
        0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x00, 0x00, 0x00, 0x05, 0x32, 0x29,
        0x53, 0x49, 0x4d, 0x20, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x3d,
        0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d,
        0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    sit_simple_ind_data_format *testResp = (sit_simple_ind_data_format *)testPacket;
    ModemData modemData((char *)&testPacket, sizeof(testPacket));
    ProtocolMiscOemDisplayEngAdapterLegacy testAdapter(&modemData);
    EXPECT_EQ(testAdapter.GetId(), SIT_IND_DISPLAY_ENG);
    EXPECT_EQ(testAdapter.GetLength(), testResp->hdr.length);
    EXPECT_TRUE(testAdapter.GetParameter() != nullptr);
    EXPECT_EQ(testAdapter.GetParameterLength(), sizeof(testPacket) - sizeof(RCM_IND_HEADER));
}

TEST(ProtocolMiscAdapterLegacy, ProtocolMiscPhoneResetAdapterLegacy) {
    struct {
        char resetType;
        char resetCause;
    } testVector[] = {
        { SIT_PWR_RESET_TYPE_PHONE_ONLY, 0 },
        { SIT_PWR_RESET_TYPE_PHONE_ONLY, 1 },
        { SIT_PWR_RESET_TYPE_RESET_BOTH, 0 },
        { SIT_PWR_RESET_TYPE_RESET_BOTH, 1 },
        { SIT_PWR_RESET_TYPE_SHUTDOWN, 0 },
        { SIT_PWR_RESET_TYPE_SHUTDOWN, 1 },
    };

    for (auto& t : testVector) {
        size_t size = sizeof(uint8_t) * 2;
        auto modemData = std::unique_ptr<ModemData>(
                                ModemDataBuilder::BuildIndication(SIT_IND_PHONE_RESET,
                                        &t, size));
        ProtocolMiscPhoneResetAdapterLegacy testAdapter(modemData.get());
        EXPECT_TRUE(testAdapter.GetType() == RCM_TYPE_INDICATION);
        EXPECT_EQ(testAdapter.GetId(), SIT_IND_PHONE_RESET);
        EXPECT_TRUE(testAdapter.GetParameter() != nullptr);
        EXPECT_TRUE(testAdapter.GetParameterLength() == 2);
        EXPECT_EQ(testAdapter.GetResetType(), t.resetType);
        EXPECT_EQ(testAdapter.GetResetCause(), t.resetCause);
    }
}

TEST(ProtocolMiscAdapterLegacy, ProtocolMiscDataStateChangeAdapterLegacy) {
    uint8_t testVector = 1;
    auto modemData = std::unique_ptr<ModemData>(
                        ModemDataBuilder::BuildIndication(SIT_IND_DATA_STATE_CHANGE,
                                            &testVector, sizeof(testVector)));
    ProtocolMiscDataStateChangeAdapterLegacy testAdapter(modemData.get());
    EXPECT_TRUE(testAdapter.GetType() == RCM_TYPE_INDICATION);
    EXPECT_TRUE(testAdapter.GetId() == SIT_IND_DATA_STATE_CHANGE);
    EXPECT_TRUE(testAdapter.GetParameter() != nullptr);
    EXPECT_TRUE(testAdapter.GetParameterLength() == sizeof(testVector));
    EXPECT_TRUE(testAdapter.GetExpectedState() == testVector);
}

TEST(ProtocolMiscAdapterLegacy, ProtocolMiscGetStackStatusAdapterLegacy) {
    uint8_t testVector[] = {
        SIT_PWR_STATCK_DISABLE,
        SIT_PWR_STATCK_ENABLE
    };

    for (auto& t : testVector) {
        auto modemData = std::unique_ptr<ModemData>(ModemDataBuilder::BuildResponse(
                SIT_GET_STACK_STATUS, 0, RCM_E_SUCCESS,
                &t, sizeof(uint8_t)));
        ProtocolMiscGetStackStatusAdapterLegacy testAdapter(modemData.get());
        EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);
        EXPECT_EQ(testAdapter.GetId(), SIT_GET_STACK_STATUS);
        EXPECT_EQ(testAdapter.GetMode(), t);
    }
}

// signal strength default values
RIL_GSM_SignalStrength_v12 gsmDefault = { 99, 99, INT_MAX };
RIL_CDMA_SignalStrength cdmaDefault = { -1, -1 };
RIL_EVDO_SignalStrength evdoDefault = { -1, -1, INT_MAX };
RIL_LTE_SignalStrength_V1_6 lteDefault = { {INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX}, INT_MAX };
RIL_TD_SCDMA_SignalStrength_V1_2 tdscdmaDefault = { 99, 99, 255 };
RIL_WCDMA_SignalStrength_V1_2 wcdmaDefault = { 99, 99, 255, 255 };
RIL_NR_SignalStrength_V1_6 nrDefault = { {INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX}, INT_MAX, 0, { 0, } };

void verifyRilSignalStrength(RIL_SignalStrength_V1_6& rilSignalStrength,
        SIGNAL_STRENGTH_V2 &ss, int flag) {
    if (flag & SIT_MISC_SIG_RAT_SIG_GSM) {
        EXPECT_EQ(rilSignalStrength.GSM_SignalStrength.signalStrength,
                  ss.GSM_SignalStrength.sig_str);
        EXPECT_EQ(rilSignalStrength.GSM_SignalStrength.bitErrorRate,
                  ss.GSM_SignalStrength.ber);
        EXPECT_EQ(rilSignalStrength.GSM_SignalStrength.timingAdvance,
                  ss.GSM_SignalStrength.ta);
    } else {
        EXPECT_TRUE(memcmp(&rilSignalStrength.GSM_SignalStrength, &gsmDefault,
                sizeof(RIL_GSM_SignalStrength_v12)) == 0);
    }

    if (flag & SIT_MISC_SIG_RAT_SIG_WCDMA) {
        EXPECT_EQ(rilSignalStrength.WCDMA_SignalStrength.signalStrength,
                  ss.WCDMA_SignalStrength.sig_str);
        EXPECT_EQ(rilSignalStrength.WCDMA_SignalStrength.bitErrorRate,
                  ss.WCDMA_SignalStrength.ber);
        EXPECT_EQ(rilSignalStrength.WCDMA_SignalStrength.rscp,
                  ss.WCDMA_SignalStrength.rscp);
        EXPECT_EQ(rilSignalStrength.WCDMA_SignalStrength.ecno,
                  ss.WCDMA_SignalStrength.ecno);
    } else {
        EXPECT_TRUE(memcmp(&rilSignalStrength.WCDMA_SignalStrength, &wcdmaDefault,
                sizeof(RIL_WCDMA_SignalStrength_V1_2)) == 0);
    }

    if (flag & SIT_MISC_SIG_RAT_SIG_LTE_V1_4) {
        EXPECT_EQ(rilSignalStrength.LTE_SignalStrength.signalStrength,
                  ss.LTE_SignalStrength.sig_str);
        EXPECT_EQ(rilSignalStrength.LTE_SignalStrength.rsrp,
                  ss.LTE_SignalStrength.rsrp);
        EXPECT_EQ(rilSignalStrength.LTE_SignalStrength.rsrq,
                  ss.LTE_SignalStrength.rsrq);
        EXPECT_EQ(rilSignalStrength.LTE_SignalStrength.rssnr,
                  ss.LTE_SignalStrength.rssnr);
        EXPECT_EQ(rilSignalStrength.LTE_SignalStrength.cqi,
                  ss.LTE_SignalStrength.cqi);
        EXPECT_EQ(rilSignalStrength.LTE_SignalStrength.timingAdvance,
                  ss.LTE_SignalStrength.timing_adv);
        EXPECT_EQ(rilSignalStrength.LTE_SignalStrength.cqiTableIndex, (unsigned int)INT_MAX);
    } else {
        EXPECT_TRUE(memcmp(&rilSignalStrength.LTE_SignalStrength, &lteDefault,
                sizeof(RIL_LTE_SignalStrength_V1_6)) == 0);
    }

    if (flag & SIT_MISC_SIG_RAT_SIG_CDMA_V1_4) {
        EXPECT_EQ(rilSignalStrength.CDMA_SignalStrength.dbm,
                  ss.CDMA_SignalStrength.dbm);
        EXPECT_EQ(rilSignalStrength.CDMA_SignalStrength.ecio,
                  ss.CDMA_SignalStrength.ecio);
    } else {
        EXPECT_TRUE(memcmp(&rilSignalStrength.CDMA_SignalStrength, &cdmaDefault,
                sizeof(RIL_CDMA_SignalStrength)) == 0);
    }

    if (flag & SIT_MISC_SIG_RAT_SIG_EVDO_V1_4) {
        EXPECT_EQ(rilSignalStrength.EVDO_SignalStrength.dbm,
                  ss.EVDO_SignalStrength.dbm);
        EXPECT_EQ(rilSignalStrength.EVDO_SignalStrength.ecio,
                  ss.EVDO_SignalStrength.ecio);
        EXPECT_EQ(rilSignalStrength.EVDO_SignalStrength.signalNoiseRatio,
                  ss.EVDO_SignalStrength.snr);
    } else {
        EXPECT_TRUE(memcmp(&rilSignalStrength.EVDO_SignalStrength, &evdoDefault,
                sizeof(RIL_EVDO_SignalStrength)) == 0);
    }

    if (flag & SIT_MISC_SIG_RAT_SIG_TD_SCDMA_V1_4) {
        EXPECT_EQ(rilSignalStrength.TD_SCDMA_SignalStrength.signalStrength,
                  ss.TD_SCDMA_SignalStrength.sig_str);
        EXPECT_EQ(rilSignalStrength.TD_SCDMA_SignalStrength.bitErrorRate,
                  ss.TD_SCDMA_SignalStrength.ber);
        EXPECT_EQ(rilSignalStrength.TD_SCDMA_SignalStrength.rscp,
                  ss.TD_SCDMA_SignalStrength.rscp);
    } else {
        EXPECT_TRUE(memcmp(&rilSignalStrength.TD_SCDMA_SignalStrength, &tdscdmaDefault,
                sizeof(RIL_TD_SCDMA_SignalStrength_V1_2)) == 0);
    }

    if (flag & SIT_MISC_SIG_RAT_SIG_NR_V1_4) {
        EXPECT_EQ(rilSignalStrength.NR_SignalStrength.ssRsrp,
                  ss.NR_SignalStrength.ss_rsrp);
        EXPECT_EQ(rilSignalStrength.NR_SignalStrength.ssRsrq,
                  ss.NR_SignalStrength.ss_rsrq);
        EXPECT_EQ(rilSignalStrength.NR_SignalStrength.ssSinr,
                  ss.NR_SignalStrength.ss_sinr);
        EXPECT_EQ(rilSignalStrength.NR_SignalStrength.csiRsrp,
                  ss.NR_SignalStrength.csi_rsrp);
        EXPECT_EQ(rilSignalStrength.NR_SignalStrength.csiRsrq,
                  ss.NR_SignalStrength.csi_rsrq);
        EXPECT_EQ(rilSignalStrength.NR_SignalStrength.csiSinr,
                  ss.NR_SignalStrength.csi_sinr);
        EXPECT_EQ(rilSignalStrength.NR_SignalStrength.csiCqiTableIndex, (unsigned int)INT_MAX);
        EXPECT_EQ(rilSignalStrength.NR_SignalStrength.csiCqiReportLen, 0);
    } else {
        EXPECT_TRUE(memcmp(&rilSignalStrength.NR_SignalStrength, &nrDefault,
                sizeof(RIL_NR_SignalStrength_V1_6)) == 0);
    }
}

void verifyRilSignalStrength(RIL_SignalStrength_V1_6& rilSignalStrength,
        SIGNAL_STRENGTH_V4 &ss, int flag) {
    if (flag & SIT_MISC_SIG_RAT_SIG_GSM) {
        EXPECT_EQ(rilSignalStrength.GSM_SignalStrength.signalStrength,
                  ss.GSM_SignalStrength.sig_str);
        EXPECT_EQ(rilSignalStrength.GSM_SignalStrength.bitErrorRate,
                  ss.GSM_SignalStrength.ber);
        EXPECT_EQ(rilSignalStrength.GSM_SignalStrength.timingAdvance,
                  ss.GSM_SignalStrength.ta);
    } else {
        EXPECT_TRUE(memcmp(&rilSignalStrength.GSM_SignalStrength, &gsmDefault,
                sizeof(RIL_GSM_SignalStrength_v12)) == 0);
    }

    if (flag & SIT_MISC_SIG_RAT_SIG_WCDMA) {
        EXPECT_EQ(rilSignalStrength.WCDMA_SignalStrength.signalStrength,
                  ss.WCDMA_SignalStrength.sig_str);
        EXPECT_EQ(rilSignalStrength.WCDMA_SignalStrength.bitErrorRate,
                  ss.WCDMA_SignalStrength.ber);
        EXPECT_EQ(rilSignalStrength.WCDMA_SignalStrength.rscp,
                  ss.WCDMA_SignalStrength.rscp);
        EXPECT_EQ(rilSignalStrength.WCDMA_SignalStrength.ecno,
                  ss.WCDMA_SignalStrength.ecno);
    } else {
        EXPECT_TRUE(memcmp(&rilSignalStrength.WCDMA_SignalStrength, &wcdmaDefault,
                sizeof(RIL_WCDMA_SignalStrength_V1_2)) == 0);
    }

    if (flag & SIT_MISC_SIG_RAT_SIG_LTE_V1_4) {
        EXPECT_EQ(rilSignalStrength.LTE_SignalStrength.signalStrength,
                  ss.LTE_SignalStrength.sig_str);
        EXPECT_EQ(rilSignalStrength.LTE_SignalStrength.rsrp,
                  ss.LTE_SignalStrength.rsrp);
        EXPECT_EQ(rilSignalStrength.LTE_SignalStrength.rsrq,
                  ss.LTE_SignalStrength.rsrq);
        EXPECT_EQ(rilSignalStrength.LTE_SignalStrength.rssnr,
                  ss.LTE_SignalStrength.rssnr);
        EXPECT_EQ(rilSignalStrength.LTE_SignalStrength.cqi,
                  ss.LTE_SignalStrength.cqi);
        EXPECT_EQ(rilSignalStrength.LTE_SignalStrength.timingAdvance,
                  ss.LTE_SignalStrength.timing_adv);
        EXPECT_EQ(rilSignalStrength.LTE_SignalStrength.cqiTableIndex,
                  ss.LTE_SignalStrength.cqi_table_index);
    } else {
        EXPECT_TRUE(memcmp(&rilSignalStrength.LTE_SignalStrength, &lteDefault,
                sizeof(RIL_LTE_SignalStrength_V1_6)) == 0);
    }

    if (flag & SIT_MISC_SIG_RAT_SIG_CDMA_V1_4) {
        EXPECT_EQ(rilSignalStrength.CDMA_SignalStrength.dbm,
                  ss.CDMA_SignalStrength.dbm);
        EXPECT_EQ(rilSignalStrength.CDMA_SignalStrength.ecio,
                  ss.CDMA_SignalStrength.ecio);
    } else {
        EXPECT_TRUE(memcmp(&rilSignalStrength.CDMA_SignalStrength, &cdmaDefault,
                sizeof(RIL_CDMA_SignalStrength)) == 0);
    }

    if (flag & SIT_MISC_SIG_RAT_SIG_EVDO_V1_4) {
        EXPECT_EQ(rilSignalStrength.EVDO_SignalStrength.dbm,
                  ss.EVDO_SignalStrength.dbm);
        EXPECT_EQ(rilSignalStrength.EVDO_SignalStrength.ecio,
                  ss.EVDO_SignalStrength.ecio);
        EXPECT_EQ(rilSignalStrength.EVDO_SignalStrength.signalNoiseRatio,
                  ss.EVDO_SignalStrength.snr);
    } else {
        EXPECT_TRUE(memcmp(&rilSignalStrength.EVDO_SignalStrength, &evdoDefault,
                sizeof(RIL_EVDO_SignalStrength)) == 0);
    }

    if (flag & SIT_MISC_SIG_RAT_SIG_TD_SCDMA_V1_4) {
        EXPECT_EQ(rilSignalStrength.TD_SCDMA_SignalStrength.signalStrength,
                  ss.TD_SCDMA_SignalStrength.sig_str);
        EXPECT_EQ(rilSignalStrength.TD_SCDMA_SignalStrength.bitErrorRate,
                  ss.TD_SCDMA_SignalStrength.ber);
        EXPECT_EQ(rilSignalStrength.TD_SCDMA_SignalStrength.rscp,
                  ss.TD_SCDMA_SignalStrength.rscp);
    } else {
        EXPECT_TRUE(memcmp(&rilSignalStrength.TD_SCDMA_SignalStrength, &tdscdmaDefault,
                sizeof(RIL_TD_SCDMA_SignalStrength_V1_2)) == 0);
    }

    if (flag & SIT_MISC_SIG_RAT_SIG_NR_V1_4) {
        EXPECT_EQ(rilSignalStrength.NR_SignalStrength.ssRsrp,
                  ss.NR_SignalStrength.ss_rsrp);
        EXPECT_EQ(rilSignalStrength.NR_SignalStrength.ssRsrq,
                  ss.NR_SignalStrength.ss_rsrq);
        EXPECT_EQ(rilSignalStrength.NR_SignalStrength.ssSinr,
                  ss.NR_SignalStrength.ss_sinr);
        EXPECT_EQ(rilSignalStrength.NR_SignalStrength.csiRsrp,
                  ss.NR_SignalStrength.csi_rsrp);
        EXPECT_EQ(rilSignalStrength.NR_SignalStrength.csiRsrq,
                  ss.NR_SignalStrength.csi_rsrq);
        EXPECT_EQ(rilSignalStrength.NR_SignalStrength.csiSinr,
                  ss.NR_SignalStrength.csi_sinr);
        EXPECT_EQ(rilSignalStrength.NR_SignalStrength.csiCqiTableIndex,
                  ss.NR_SignalStrength.cqi_table_index);
        EXPECT_EQ(rilSignalStrength.NR_SignalStrength.csiCqiReportLen, 1);
        EXPECT_EQ(rilSignalStrength.NR_SignalStrength.csiCqiReport[0],
                  (unsigned int)ss.NR_SignalStrength.wb_cqi);
    } else {
        EXPECT_TRUE(memcmp(&rilSignalStrength.NR_SignalStrength, &nrDefault,
                sizeof(RIL_NR_SignalStrength_V1_6)) == 0);
    }
}

TEST(ProtocolMiscAdapterLegacy, ProtocolSignalStrengthAdapterLegacy_version2) {
    // SignalStrength protocol version2 and 3 are same.
    SIGNAL_STRENGTH_V2 ss = {
        .GSM_SignalStrength = { 30, 5, 0 },
        .WCDMA_SignalStrength = { 25, 3, 80, 45 },
        .CDMA_SignalStrength = { 70, 120 },
        .EVDO_SignalStrength = { 65, 110, 8 },
        .TD_SCDMA_SignalStrength = { 27, 7, 90 },
        .LTE_SignalStrength = { 31, 50, 10, 100, 14, 0 },
        .NR_SignalStrength = { 100, 101, 102, 0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF }
    };

    int testVecotr[] = {
        SIT_MISC_SIG_RAT_SIG_GSM,
        SIT_MISC_SIG_RAT_SIG_WCDMA,
        SIT_MISC_SIG_RAT_SIG_CDMA_V1_4,
        SIT_MISC_SIG_RAT_SIG_CDMA_V1_4 | SIT_MISC_SIG_RAT_SIG_EVDO_V1_4,
        SIT_MISC_SIG_RAT_SIG_TD_SCDMA_V1_4,
        SIT_MISC_SIG_RAT_SIG_LTE_V1_4,
        SIT_MISC_SIG_RAT_SIG_LTE_V1_4 | SIT_MISC_SIG_RAT_SIG_NR_V1_4,
        SIT_MISC_SIG_RAT_SIG_NR_V1_4,
    };
    int size = sizeof(testVecotr) / sizeof(testVecotr[0]);

    for (int i = 0; i < size; i++) {
        sit_misc_signal_strength_rsp_v2 rsp;
        int flag = testVecotr[i];
        rsp.valid_rat_sig_flag = flag;
        memcpy(&rsp.sig_strength, &ss, sizeof(ss));
        ModemData *modemData = ModemDataBuilder::BuildResponse(
                SIT_GET_SIGNAL_STRENGTH, 0, RIL_E_SUCCESS,
                ((sit_simple_data_format *)&rsp)->data, sizeof(ss) + sizeof(uint16_t));
        ASSERT_NE(modemData, nullptr);

        ProtocolSignalStrengthAdapterLegacy testAdapter(modemData);
        RIL_SignalStrength_V1_6 result = testAdapter.GetSignalStrength(SIGNAL_STRENGTH_PROTOCOL_V2);
        verifyRilSignalStrength(result, ss, flag);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    }  // end for i ~
}

TEST(ProtocolMiscAdapterLegacy, ProtocolSignalStrengthAdapterLegacy_version4) {
    SIGNAL_STRENGTH_V4 ss = {
        .GSM_SignalStrength = { 30, 5, 0 },
        .WCDMA_SignalStrength = { 25, 3, 80, 45 },
        .CDMA_SignalStrength = { 70, 120 },
        .EVDO_SignalStrength = { 65, 110, 8 },
        .TD_SCDMA_SignalStrength = { 27, 7, 90 },
        .LTE_SignalStrength = { 31, 50, 10, 100, 3, 14, 0 },
        .NR_SignalStrength = { 100, 101, 102, 0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF, 1, 7,
                { INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX,
                  INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX,
                  INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX,
                  INT_MAX, INT_MAX, INT_MAX, INT_MAX } }
    };

    int testVecotr[] = {
        SIT_MISC_SIG_RAT_SIG_GSM,
        SIT_MISC_SIG_RAT_SIG_WCDMA,
        SIT_MISC_SIG_RAT_SIG_CDMA_V1_4,
        SIT_MISC_SIG_RAT_SIG_CDMA_V1_4 | SIT_MISC_SIG_RAT_SIG_EVDO_V1_4,
        SIT_MISC_SIG_RAT_SIG_TD_SCDMA_V1_4,
        SIT_MISC_SIG_RAT_SIG_LTE_V1_4,
        SIT_MISC_SIG_RAT_SIG_LTE_V1_4 | SIT_MISC_SIG_RAT_SIG_NR_V1_4,
        SIT_MISC_SIG_RAT_SIG_NR_V1_4,
    };
    int size = sizeof(testVecotr) / sizeof(testVecotr[0]);

    for (int i = 0; i < size; i++) {
        sit_misc_signal_strength_rsp_v4 rsp;
        int flag = testVecotr[i];
        rsp.valid_rat_sig_flag = flag;
        memcpy(&rsp.sig_strength, &ss, sizeof(ss));
        ModemData *modemData = ModemDataBuilder::BuildResponse(
                SIT_GET_SIGNAL_STRENGTH, 0, RIL_E_SUCCESS,
                ((sit_simple_data_format *)&rsp)->data, sizeof(ss) + sizeof(uint16_t));
        ASSERT_NE(modemData, nullptr);

        ProtocolSignalStrengthAdapterLegacy testAdapter(modemData);
        RIL_SignalStrength_V1_6 result = testAdapter.GetSignalStrength(SIGNAL_STRENGTH_PROTOCOL_V4);
        verifyRilSignalStrength(result, ss, flag);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    }  // end for i ~
}

TEST(ProtocolMiscAdapterLegacy, ProtocolSignalStrengthIndAdapterLegacy_version2) {
    // SignalStrength protocol version2 and 3 are same.
    SIGNAL_STRENGTH_V2 ss = {
        .GSM_SignalStrength = { 30, 5, 0 },
        .WCDMA_SignalStrength = { 25, 3, 80, 45 },
        .CDMA_SignalStrength = { 70, 120 },
        .EVDO_SignalStrength = { 65, 110, 8 },
        .TD_SCDMA_SignalStrength = { 27, 7, 90 },
        .LTE_SignalStrength = { 31, 50, 10, 100, 14, 0 },
        .NR_SignalStrength = { 100, 101, 102, 0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF }
    };

    int testVecotr[] = {
        SIT_MISC_SIG_RAT_SIG_GSM,
        SIT_MISC_SIG_RAT_SIG_WCDMA,
        SIT_MISC_SIG_RAT_SIG_CDMA_V1_4,
        SIT_MISC_SIG_RAT_SIG_CDMA_V1_4 | SIT_MISC_SIG_RAT_SIG_EVDO_V1_4,
        SIT_MISC_SIG_RAT_SIG_TD_SCDMA_V1_4,
        SIT_MISC_SIG_RAT_SIG_LTE_V1_4,
        SIT_MISC_SIG_RAT_SIG_LTE_V1_4 | SIT_MISC_SIG_RAT_SIG_NR_V1_4,
        SIT_MISC_SIG_RAT_SIG_NR_V1_4,
    };
    int size = sizeof(testVecotr) / sizeof(testVecotr[0]);

    for (int i = 0; i < size; i++) {
        sit_misc_signal_strength_ind_v2 ind;
        int flag = testVecotr[i];
        ind.valid_rat_sig_flag = flag;
        memcpy(&ind.sig_strength, &ss, sizeof(ss));
        ModemData *modemData = ModemDataBuilder::BuildIndication(
                SIT_IND_SIGNAL_STRENGTH, ((sit_simple_ind_data_format *)&ind)->data, sizeof(ss) + sizeof(uint16_t));
        ASSERT_NE(modemData, nullptr);

        ProtocolSignalStrengthIndAdapterLegacy testAdapter(modemData);
        EXPECT_TRUE(testAdapter.GetParameter() != nullptr);
        RIL_SignalStrength_V1_6 result = testAdapter.GetSignalStrength(SIGNAL_STRENGTH_PROTOCOL_V2);
        verifyRilSignalStrength(result, ss, flag);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    }  // end for i ~
}

TEST(ProtocolMiscAdapterLegacy, ProtocolSignalStrengthIndAdapterLegacy_version4) {
    SIGNAL_STRENGTH_V4 ss = {
        .GSM_SignalStrength = { 30, 5, 0 },
        .WCDMA_SignalStrength = { 25, 3, 80, 45 },
        .CDMA_SignalStrength = { 70, 120 },
        .EVDO_SignalStrength = { 65, 110, 8 },
        .TD_SCDMA_SignalStrength = { 27, 7, 90 },
        .LTE_SignalStrength = { 31, 50, 10, 100, 3, 14, 0 },
        .NR_SignalStrength = { 100, 101, 102, 0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF, 1, 7,
                { INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX,
                  INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX,
                  INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX,
                  INT_MAX, INT_MAX, INT_MAX, INT_MAX } }
    };

    int testVecotr[] = {
        SIT_MISC_SIG_RAT_SIG_GSM,
        SIT_MISC_SIG_RAT_SIG_WCDMA,
        SIT_MISC_SIG_RAT_SIG_CDMA_V1_4,
        SIT_MISC_SIG_RAT_SIG_CDMA_V1_4 | SIT_MISC_SIG_RAT_SIG_EVDO_V1_4,
        SIT_MISC_SIG_RAT_SIG_TD_SCDMA_V1_4,
        SIT_MISC_SIG_RAT_SIG_LTE_V1_4,
        SIT_MISC_SIG_RAT_SIG_LTE_V1_4 | SIT_MISC_SIG_RAT_SIG_NR_V1_4,
        SIT_MISC_SIG_RAT_SIG_NR_V1_4,
    };
    int size = sizeof(testVecotr) / sizeof(testVecotr[0]);

    for (int i = 0; i < size; i++) {
        sit_misc_signal_strength_ind_v4 ind;
        int flag = testVecotr[i];
        ind.valid_rat_sig_flag = flag;
        memcpy(&ind.sig_strength, &ss, sizeof(ss));
        ModemData *modemData = ModemDataBuilder::BuildIndication(
                SIT_IND_SIGNAL_STRENGTH, ((sit_simple_ind_data_format *)&ind)->data, sizeof(ss) + sizeof(uint16_t));
        ASSERT_NE(modemData, nullptr);

        ProtocolSignalStrengthIndAdapterLegacy testAdapter(modemData);
        RIL_SignalStrength_V1_6 result = testAdapter.GetSignalStrength(SIGNAL_STRENGTH_PROTOCOL_V4);
        verifyRilSignalStrength(result, ss, flag);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    }  // end for i ~
}

TEST(ProtocolMiscAdapterLegacy, ProtocolMiscNITZTimeAdapterLegacy) {
    sit_nitz_time_with_age testVector = {
        {0, 22, 2, 19, 12, 15, 30, 60, 0, 3, 0, {}}, 0,
    };
    auto modemData = std::unique_ptr<ModemData>(
                            ModemDataBuilder::BuildIndication(SIT_IND_NITZ_TIME_RECEIVED,
                                            &testVector, sizeof(testVector)));
    ProtocolMiscNITZTimeAdapterLegacy  testAdapter(modemData.get());
    EXPECT_TRUE(testAdapter.GetType() == RCM_TYPE_INDICATION);
    EXPECT_TRUE(testAdapter.GetId() == SIT_IND_NITZ_TIME_RECEIVED);
    EXPECT_TRUE(testAdapter.GetParameter());
    EXPECT_TRUE(testAdapter.GetParameterLength() == sizeof(testVector));
    EXPECT_TRUE(testAdapter.TimeInfoType() == 0);
    EXPECT_TRUE(testAdapter.DayLightValid() == static_cast<int32_t>(testVector.daylight_valid));
    EXPECT_TRUE(testAdapter.Year() == static_cast<int32_t>(testVector.year));
    EXPECT_TRUE(testAdapter.Month() == static_cast<int32_t>(testVector.month));
    EXPECT_TRUE(testAdapter.Day() == static_cast<int32_t>(testVector.day));
    EXPECT_TRUE(testAdapter.Hour() == static_cast<int32_t>(testVector.hour));
    EXPECT_TRUE(testAdapter.Minute() == static_cast<int32_t>(testVector.minute));
    EXPECT_TRUE(testAdapter.Second() == static_cast<int32_t>(testVector.second));
    EXPECT_TRUE(testAdapter.TimeZone() == static_cast<int32_t>(testVector.time_zone));
    EXPECT_TRUE(testAdapter.DayLightAdjust() == static_cast<int32_t>(testVector.daylight_adjust));
    EXPECT_TRUE(testAdapter.DayofWeek() == static_cast<int32_t>(testVector.day_of_week));
    EXPECT_TRUE(testAdapter.GetMMInfo() == static_cast<int32_t>(testVector.mminfo));
    EXPECT_TRUE(memcmp(testAdapter.GetPLMN(), testVector.plmn, MAX_PLMN_LEN) == 0);
    EXPECT_TRUE(testAdapter.age() == static_cast<int64_t>(testVector.daylight_valid));
}

TEST(ProtocolMiscAdapterLegacy, ProtocolMiscGetNITZTimeAdapterLegacy) {
    sit_nitz_time_with_age testVector = {
        {0, 22, 2, 19, 12, 15, 30, 60, 0, 3, 0, {}}, 0,
    };
    auto modemData = std::unique_ptr<ModemData>(
                            ModemDataBuilder::BuildResponse(SIT_GET_NITZ_TIME, 0, 0,
                                            &testVector, sizeof(testVector)));
    ProtocolMiscGetNITZTimeAdapterLegacy  testAdapter(modemData.get());
    EXPECT_TRUE(testAdapter.GetType() == RCM_TYPE_RESPONSE);
    EXPECT_TRUE(testAdapter.GetId() == SIT_GET_NITZ_TIME);
    EXPECT_TRUE(testAdapter.GetErrorCode() == RIL_E_SUCCESS);
    EXPECT_TRUE(testAdapter.GetParameter());
    EXPECT_TRUE(testAdapter.GetParameterLength() == sizeof(testVector));
    EXPECT_TRUE(testAdapter.TimeInfoType() == 0);
    EXPECT_TRUE(testAdapter.DayLightValid() == static_cast<int32_t>(testVector.daylight_valid));
    EXPECT_TRUE(testAdapter.Year() == static_cast<int32_t>(testVector.year));
    EXPECT_TRUE(testAdapter.Month() == static_cast<int32_t>(testVector.month));
    EXPECT_TRUE(testAdapter.Day() == static_cast<int32_t>(testVector.day));
    EXPECT_TRUE(testAdapter.Hour() == static_cast<int32_t>(testVector.hour));
    EXPECT_TRUE(testAdapter.Minute() == static_cast<int32_t>(testVector.minute));
    EXPECT_TRUE(testAdapter.Second() == static_cast<int32_t>(testVector.second));
    EXPECT_TRUE(testAdapter.TimeZone() == static_cast<int32_t>(testVector.time_zone));
    EXPECT_TRUE(testAdapter.DayLightAdjust() == static_cast<int32_t>(testVector.daylight_adjust));
    EXPECT_TRUE(testAdapter.DayofWeek() == static_cast<int32_t>(testVector.day_of_week));
    EXPECT_TRUE(testAdapter.GetMMInfo() == static_cast<int32_t>(testVector.mminfo));
    EXPECT_TRUE(memcmp(testAdapter.GetPLMN(), testVector.plmn, MAX_PLMN_LEN) == 0);
    EXPECT_TRUE(testAdapter.age() == static_cast<int64_t>(testVector.daylight_valid));
}

TEST(ProtocolMiscAdapterLegacy, ProtocolMiscGetTraceDumpCfgAdapter) {
    uint8_t testVector[] = {
        0, 1
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);
    for (int i = 0; i < size; i++) {
        ModemData *modemData = ModemDataBuilder::BuildResponse(
                SIT_OEM_GET_TRACE_DUMP_CFG, 0, RCM_E_SUCCESS,
                &testVector[i], 1);
        ASSERT_TRUE(modemData != nullptr);

        ProtocolMiscGetTraceDumpCfgAdapterLegacy testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);
        EXPECT_EQ(testAdapter.GetId(), SIT_OEM_GET_TRACE_DUMP_CFG);
        EXPECT_EQ(testAdapter.GetConfig(), testVector[i]);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    }  // end for i ~
}

TEST(ProtocolMiscAdapterLegacy, ProtocolMiscGetActivityInfoAdapterLegacy) {
    sit_misc_get_activity_info_rsp testVector {
        {}, 10, 14, { 1, 2, 3, 4, 5 }, 36,
    };
    auto modemData = std::unique_ptr<ModemData>(ModemDataBuilder::BuildResponse(
                                                    SIT_GET_ACTIVITY_INFO, 0, 0,
                                                    &testVector.sleep_mode_time_ms,
                                                    sizeof(testVector) - sizeof(RCM_HEADER)));
    ProtocolMiscGetActivityInfoAdapterLegacy test(modemData.get());
    EXPECT_EQ(test.GetType(), RCM_TYPE_RESPONSE);
    EXPECT_EQ(test.GetId(), SIT_GET_ACTIVITY_INFO);
    EXPECT_EQ(test.GetLength(), (uint32_t)sizeof(sit_misc_get_activity_info_rsp));
    EXPECT_TRUE(test.GetErrorCode() == RIL_E_SUCCESS);
    EXPECT_TRUE(test.GetParameter());
    EXPECT_EQ(test.GetParameterLength(), (uint32_t)(sizeof(testVector) - sizeof(RCM_HEADER)));
    EXPECT_EQ(test.GetSleepPeriod(), testVector.sleep_mode_time_ms);
    EXPECT_EQ(test.GetIdlePeriod(), testVector.idle_mode_time_ms);
    EXPECT_TRUE(memcmp(test.GetTxPeriod(), testVector.tx_mode_time_ms,
                        RIL_NUM_TX_POWER_LEVELS * sizeof(UINT32)) == 0);
    for (int i = 0; i < RIL_NUM_TX_POWER_LEVELS; i++) {
        EXPECT_EQ(test.GetTxPeriod()[i], testVector.tx_mode_time_ms[i]);
    }
    EXPECT_EQ(test.GetRxPeriod(), testVector.rx_mode_time_ms);
}

TEST(ProtocolMiscAdapterLegacy, ProtocolMiscGetActivityInfoRatSpecificAdapter) {
    namespace modem = android::hardware::radio::impl::modem::V2_0;
    sit_modem_activity_info testVector = {
        1, 2,
        {
            { { 10, 20, 0, 30, 40 }, 3 },  // G
            { { 40, 2, 1, 50, 6 }, 4 },    // U
            { { 8, 3, 7, 0 , 0 }, 5 },     // L
            { { 4, 8, 17, 9 , 6 }, 6 },    // N - sub6
            { { 23, 30, 0, 11 , 12 }, 7 }, // N - mmWave
        },
    };
    auto modemData = std::unique_ptr<ModemData>(ModemDataBuilder::BuildResponse(SIT_GET_ACTIVITY_INFO,
                                            0, 0, &testVector, sizeof(testVector)));
    ProtocolMiscGetActivityInfoRatSpecificAdapterLegacy test(modemData.get());
    EXPECT_EQ(test.GetType(), RCM_TYPE_RESPONSE);
    EXPECT_EQ(test.GetId(), SIT_GET_ACTIVITY_INFO);
    EXPECT_EQ(test.GetLength(), (uint32_t)sizeof(sit_misc_get_activity_info_rat_specific_rsp));
    EXPECT_TRUE(test.GetParameter());
    EXPECT_EQ(test.GetParameterLength(), (uint32_t)sizeof(sit_modem_activity_info));
    const modem::RIL_ActivityStatsInfo& info = test.info();
    EXPECT_EQ(info.sleepModeTimeMs, testVector.sleepModeTime);
    EXPECT_EQ(info.idleModeTimeMs, testVector.idleModeTime);
    EXPECT_EQ(info.techSpecificInfo.size(), ACTIVITY_INFO_RAT_MAX);

    constexpr RIL_RadioAccessNetworks ran[] = {
        RIL_RAN_GERAN,
        RIL_RAN_UTRAN,
        RIL_RAN_EUTRAN,
        (RIL_RadioAccessNetworks)ACCESS_NETWORK_NGRAN,
        (RIL_RadioAccessNetworks)ACCESS_NETWORK_NGRAN,
    };
    constexpr int32_t frequencyRange[] = {
        FREQUENCY_RANGE_UNKNOWN,
        FREQUENCY_RANGE_UNKNOWN,
        FREQUENCY_RANGE_UNKNOWN,
        FREQUENCY_RANGE_HIGH,
        FREQUENCY_RANGE_MMWAVE,
    };
    for (int i = 0; i < ACTIVITY_INFO_RAT_MAX; i++) {
        const modem::RIL_ActivityStatsTechSpecificInfo& techInfo = info.techSpecificInfo[i];
        EXPECT_EQ(techInfo.rat, ran[i]);
        EXPECT_EQ(techInfo.frequencyRange, frequencyRange[i]);
        EXPECT_EQ(techInfo.txmModetimeMs.size(), TX_MODE_RANGE_MAX);
        for (int j = 0; j < TX_MODE_RANGE_MAX; j++) {
            EXPECT_EQ(techInfo.txmModetimeMs[j], testVector.ratSpecificInfo[i].txModeTime[j]);
        }
        EXPECT_EQ(techInfo.rxModeTimeMs, testVector.ratSpecificInfo[i].rxModeTime);
    }
}

TEST(ProtocolMiscAdapterLegacy, ProtocolMiscGetActivityInfoRatSpecificAdapter_with_legacy_data) {
    namespace modem = android::hardware::radio::impl::modem::V2_0;
    sit_misc_get_activity_info_rsp testVector {
        {}, 10, 14, { 1, 2, 3, 4, 5 }, 36,
    };
    auto modemData = std::unique_ptr<ModemData>(ModemDataBuilder::BuildResponse(
                                                    SIT_GET_ACTIVITY_INFO, 0, 0,
                                                    &testVector.sleep_mode_time_ms,
                                                    sizeof(testVector) - sizeof(RCM_HEADER)));
    ProtocolMiscGetActivityInfoRatSpecificAdapterLegacy test(modemData.get());
    EXPECT_EQ(test.GetType(), RCM_TYPE_RESPONSE);
    EXPECT_EQ(test.GetId(), SIT_GET_ACTIVITY_INFO);
    EXPECT_EQ(test.GetLength(), (uint32_t)sizeof(sit_misc_get_activity_info_rsp));
    EXPECT_TRUE(test.GetParameter());
    EXPECT_EQ(test.GetParameterLength(), (uint32_t)(sizeof(testVector) - sizeof(RCM_HEADER)));
    const modem::RIL_ActivityStatsInfo& info = test.info();
    EXPECT_EQ(info.sleepModeTimeMs, (int)testVector.sleep_mode_time_ms);
    EXPECT_EQ(info.idleModeTimeMs, (int)testVector.idle_mode_time_ms);
    EXPECT_EQ(info.techSpecificInfo.size(), (size_t)1);
    const modem::RIL_ActivityStatsTechSpecificInfo& techInfo = info.techSpecificInfo[0];
    for (int i = 0; i < TX_MODE_RANGE_MAX; i++) {
        EXPECT_EQ(techInfo.txmModetimeMs[i], (int)testVector.tx_mode_time_ms[i]);
    }
    EXPECT_EQ(techInfo.rxModeTimeMs, (int)testVector.rx_mode_time_ms);
}

TEST(ProtocolMiscAdapterLegacy, ProtocolMiscPinControlAdapterLegacy) {
    uint8_t testVector[2] { 1, 0 };
    auto modemData = std::unique_ptr<ModemData>(
                            ModemDataBuilder::BuildIndication(SIT_IND_PIN_CONTROL,
                                            testVector, sizeof(testVector)));
    ProtocolMiscPinControlAdapterLegacy testAdapter(modemData.get());
    EXPECT_TRUE(testAdapter.GetType() == RCM_TYPE_INDICATION);
    EXPECT_TRUE(testAdapter.GetId() == SIT_IND_PIN_CONTROL);
    EXPECT_TRUE(testAdapter.GetParameter() != nullptr);
    EXPECT_TRUE(testAdapter.GetParameterLength() == sizeof(testVector));
    EXPECT_TRUE(testAdapter.GetSignal() == testVector[0]);
    EXPECT_TRUE(testAdapter.GetStatus() == testVector[1]);
}

TEST(ProtocolMiscAdapterLegacy, ProtocolGetVoiceOperationAdapterLegacy) {
    uint32_t testVector = 1;
    auto modemData = std::unique_ptr<ModemData>(
                            ModemDataBuilder::BuildResponse(SIT_GET_VOICE_OPERATION, 0, 0,
                                            &testVector, sizeof(testVector)));
    ProtocolGetVoiceOperationAdapterLegacy testAdapter(modemData.get());
    EXPECT_TRUE(testAdapter.GetType() == RCM_TYPE_RESPONSE);
    EXPECT_TRUE(testAdapter.GetId() == SIT_GET_VOICE_OPERATION);
    EXPECT_TRUE(testAdapter.GetErrorCode() == RIL_E_SUCCESS);
    EXPECT_TRUE(testAdapter.GetParameter());
    EXPECT_TRUE(testAdapter.GetParameterLength() == sizeof(testVector));
    EXPECT_TRUE(testAdapter.GetMode() == static_cast<int32_t>(testVector));
}

TEST(ProtocolMiscAdapterLegacy, ProtocolMiscGetHwConfigAdapterLegacy) {
    sit_misc_get_hw_config_rsp testVector = {};
    auto modemData = std::unique_ptr<ModemData>(
                            ModemDataBuilder::BuildResponse(SIT_GET_HW_CONFIG, 0, 0,
                                            (char *)&testVector + sizeof(RCM_HEADER),
                                            sizeof(sit_misc_get_hw_config_rsp) - sizeof(RCM_HEADER)));
    ProtocolMiscGetHwConfigAdapterLegacy testAdapter(modemData.get());
    EXPECT_TRUE(testAdapter.GetType() == RCM_TYPE_RESPONSE);
    EXPECT_TRUE(testAdapter.GetId() == SIT_GET_HW_CONFIG);
    EXPECT_TRUE(testAdapter.GetErrorCode() == RIL_E_SUCCESS);
    EXPECT_TRUE(testAdapter.GetParameter());
    EXPECT_TRUE(testAdapter.GetParameterLength() == sizeof(sit_misc_get_hw_config_rsp) - sizeof(RCM_HEADER));
    EXPECT_TRUE(testAdapter.GetNum() == 0);
}

TEST(ProtocolMiscAdapterLegacy, ProtocolMiscHwConfigChangeAdapterLegacy) {
    sit_misc_hw_config_change_ind testVector = {{}, 0, {}};
    auto modemData = std::unique_ptr<ModemData>(
                            ModemDataBuilder::BuildIndication(SIT_IND_HW_CONFIG_CHANGED,
                                            (char *)&testVector + sizeof(RCM_IND_HEADER),
                                            sizeof(sit_misc_hw_config_change_ind) - sizeof(RCM_IND_HEADER)));
    ProtocolMiscHwConfigChangeAdapterLegacy testAdapter(modemData.get());
    EXPECT_TRUE(testAdapter.GetType() == RCM_TYPE_INDICATION);
    EXPECT_TRUE(testAdapter.GetId() == SIT_IND_HW_CONFIG_CHANGED);
    EXPECT_TRUE(testAdapter.GetParameter() != nullptr);
    EXPECT_TRUE(testAdapter.GetParameterLength() == sizeof(sit_misc_hw_config_change_ind) - sizeof(RCM_IND_HEADER));
    EXPECT_TRUE(testAdapter.GetNum() == 0);
}

TEST(ProtocolMiscAdapterLegacy, ProtocolMiscCdmaPrlChangeAdapterLegacy) {
    int32_t testVector = 15;
    auto modemData = std::unique_ptr<ModemData>(
                            ModemDataBuilder::BuildIndication(SIT_IND_CDMA_PRL_CHANGED,
                                            &testVector, sizeof(testVector)));
    ProtocolMiscCdmaPrlChangeAdapterLegacy testAdapter(modemData.get());
    EXPECT_TRUE(testAdapter.GetType() == RCM_TYPE_INDICATION);
    EXPECT_TRUE(testAdapter.GetId() == SIT_IND_CDMA_PRL_CHANGED);
    EXPECT_TRUE(testAdapter.GetParameter() != nullptr);
    EXPECT_TRUE(testAdapter.GetParameterLength() == sizeof(testVector));
    EXPECT_TRUE(testAdapter.GetPrlVersion() == testVector);
}

TEST(ProtocolMiscAdapterLegacy, ProtocolMiscLceAdapterLegacy_start) {
    sit_pdp_start_lce_info_rsp testVector = {{}, 1, 1000};
    size_t size = sizeof(sit_pdp_start_lce_info_rsp) - sizeof(RCM_HEADER);
    auto modemData = std::unique_ptr<ModemData>(
                            ModemDataBuilder::BuildResponse(SIT_START_LCE_INFO, 0, 0,
                                            (char *)&testVector + sizeof(RCM_HEADER), size));
    ProtocolMiscLceAdapterLegacy testAdapter(modemData.get());
    EXPECT_TRUE(testAdapter.GetType() == RCM_TYPE_RESPONSE);
    EXPECT_TRUE(testAdapter.GetId() == SIT_START_LCE_INFO);
    EXPECT_TRUE(testAdapter.GetErrorCode() == RIL_E_SUCCESS);
    EXPECT_TRUE(testAdapter.GetParameter());
    EXPECT_TRUE(testAdapter.getLceStatus() == static_cast<int32_t>(testVector.status));
    EXPECT_TRUE(testAdapter.getActualIntervalMs() == static_cast<uint32_t>(testVector.interval));
}

TEST(ProtocolMiscAdapterLegacy, ProtocolMiscLceAdapterLegacy_stop) {
    sit_pdp_stop_lce_info_rsp testVector = {{}, 0, 3000};
    size_t size = sizeof(sit_pdp_stop_lce_info_rsp) - sizeof(RCM_HEADER);
    auto modemData = std::unique_ptr<ModemData>(
                            ModemDataBuilder::BuildResponse(SIT_STOP_LCE_INFO, 0, 0,
                                            (char *)&testVector + sizeof(RCM_HEADER), size));
    ProtocolMiscLceAdapterLegacy testAdapter(modemData.get());
    EXPECT_TRUE(testAdapter.GetType() == RCM_TYPE_RESPONSE);
    EXPECT_TRUE(testAdapter.GetId() == SIT_STOP_LCE_INFO);
    EXPECT_TRUE(testAdapter.GetErrorCode() == RIL_E_SUCCESS);
    EXPECT_TRUE(testAdapter.GetParameter());
    EXPECT_TRUE(testAdapter.GetParameterLength() == size);
    EXPECT_TRUE(testAdapter.getLceStatus() == static_cast<int32_t>(testVector.status));
    EXPECT_TRUE(testAdapter.getActualIntervalMs() == static_cast<uint32_t>(testVector.interval));
}

TEST(ProtocolMiscAdapterLegacy, ProtocolMiscLceAdapterLegacy_data) {
    sit_pdp_get_lce_data_rsp testVector = {{}, 100000, 200000, 1, 1};
    size_t size = sizeof(sit_pdp_get_lce_data_rsp) - sizeof(RCM_HEADER);
    auto modemData = std::unique_ptr<ModemData>(
                            ModemDataBuilder::BuildResponse(SIT_GET_LCE_DATA, 0, 0,
                                            (char *)&testVector + sizeof(RCM_HEADER), size));
    ProtocolMiscLceAdapterLegacy testAdapter(modemData.get());
    EXPECT_TRUE(testAdapter.GetType() == RCM_TYPE_RESPONSE);
    EXPECT_TRUE(testAdapter.GetId() == SIT_GET_LCE_DATA);
    EXPECT_TRUE(testAdapter.GetErrorCode() == RIL_E_SUCCESS);
    EXPECT_TRUE(testAdapter.GetParameter());
    EXPECT_TRUE(testAdapter.GetParameterLength() == size);
    EXPECT_TRUE(testAdapter.getDlCapacityKbps() == static_cast<uint32_t>(testVector.dl_lc));
    EXPECT_TRUE(testAdapter.getUlCapacityKbps() == static_cast<uint32_t>(testVector.ul_lc));
    EXPECT_TRUE(testAdapter.getConfidencelevel() == static_cast<int32_t>(testVector.conf_lvl));
    EXPECT_TRUE(testAdapter.getLceSuspended() == static_cast<int32_t>(testVector.is_suspended));
}

TEST(ProtocolMiscAdapterLegacy, ProtocolMiscLceIndAdapterLegacy) {
    sit_pdp_lce_data_ind testVector = {{}, 100000, 200000, 1, 1};
    size_t size = sizeof(sit_pdp_lce_data_ind) - sizeof(RCM_IND_HEADER);
    auto modemData = std::unique_ptr<ModemData>(
                            ModemDataBuilder::BuildIndication(SIT_IND_LCE_DATA,
                                            (char *)&testVector + sizeof(RCM_IND_HEADER), size));
    ProtocolMiscLceIndAdapterLegacy testAdapter(modemData.get());
    EXPECT_TRUE(testAdapter.GetType() == RCM_TYPE_INDICATION);
    EXPECT_TRUE(testAdapter.GetId() == SIT_IND_LCE_DATA);
    EXPECT_TRUE(testAdapter.GetParameter());
    EXPECT_TRUE(testAdapter.GetParameterLength() == size);
    EXPECT_TRUE(testAdapter.GetDLLc() == static_cast<int32_t>(testVector.dl_lc));
    EXPECT_TRUE(testAdapter.GetULLc() == static_cast<int32_t>(testVector.ul_lc));
    EXPECT_TRUE(testAdapter.GetConfLevel() == static_cast<int32_t>(testVector.conf_lvl));
    EXPECT_TRUE(testAdapter.GetIsSuspended() == static_cast<int32_t>(testVector.is_suspended));
}

TEST(ProtocolMiscAdapterLegacy, ProtocolMiscSarControlStateAdapterLegacy) {
    uint8_t testVector = 1;
    auto modemData = std::unique_ptr<ModemData>(
                            ModemDataBuilder::BuildIndication(SIT_IND_SAR_CONTROL_STATE,
                                            &testVector, sizeof(testVector)));
    ProtocolMiscSarControlStateAdapterLegacy testAdapter(modemData.get());
    EXPECT_TRUE(testAdapter.GetType() == RCM_TYPE_INDICATION);
    EXPECT_TRUE(testAdapter.GetId() == SIT_IND_SAR_CONTROL_STATE);
    EXPECT_TRUE(testAdapter.GetParameter());
    EXPECT_TRUE(testAdapter.GetParameterLength() == sizeof(testVector));
    EXPECT_TRUE(testAdapter.GetDeviceState() == static_cast<uint8_t>(testVector));
}

TEST(ProtocolMiscAdapterLegacy, ProtocolMiscSarRfConnectionAdapterLegacy) {
    uint8_t testVector = 1;
    auto modemData = std::unique_ptr<ModemData>(
                            ModemDataBuilder::BuildIndication(SIT_OEM_IND_RF_CONNECTION,
                                            &testVector, sizeof(testVector)));
    ProtocolMiscSarRfConnectionAdapterLegacy testAdapter(modemData.get());
    EXPECT_TRUE(testAdapter.GetType() == RCM_TYPE_INDICATION);
    EXPECT_TRUE(testAdapter.GetId() == SIT_OEM_IND_RF_CONNECTION);
    EXPECT_TRUE(testAdapter.GetParameter());
    EXPECT_TRUE(testAdapter.GetParameterLength() == sizeof(testVector));
    EXPECT_TRUE(testAdapter.GetRfState() == static_cast<uint8_t>(testVector));
}

TEST(ProtocolMiscAdapterLegacy, ProtocolMiscRssiScanResultAdapterLegacy ) {
    sit_misc_rssi_scan_ind testVector {};
    size_t size = sizeof(sit_misc_rssi_scan_ind) - sizeof(RCM_IND_HEADER);
    auto modemData = std::unique_ptr<ModemData>(
                            ModemDataBuilder::BuildIndication(SIT_IND_RSSI_SCAN,
                                            (char *)&testVector + sizeof(RCM_IND_HEADER), size));
    ProtocolMiscRssiScanResultAdapterLegacy testAdapter(modemData.get());
    EXPECT_TRUE(testAdapter.GetType() == RCM_TYPE_INDICATION);
    EXPECT_TRUE(testAdapter.GetId() == SIT_IND_RSSI_SCAN);
    EXPECT_TRUE(testAdapter.GetParameter());
    EXPECT_TRUE(testAdapter.GetParameterLength() == size);
    EXPECT_TRUE(testAdapter.GetTotalPage() == 0);
    EXPECT_TRUE(testAdapter.GetCurrentPage() == 0);
}

TEST(ProtocolMiscAdapterLegacy, ProtocolMiscATCommandAdapterLegacy ) {
    sit_misc_forwarding_at_command_ind testVector = {{}, 8, "\r\nATE1\r\n"};
    size_t size = 10;
    auto modemData = std::unique_ptr<ModemData>(
                            ModemDataBuilder::BuildIndication(SIT_IND_FORWARDING_AT_COMMAND,
                                            (char *)&testVector + sizeof(RCM_IND_HEADER), size));
    ProtocolMiscATCommandAdapterLegacy testAdapter(modemData.get());
    EXPECT_TRUE(testAdapter.GetType() == RCM_TYPE_INDICATION);
    EXPECT_TRUE(testAdapter.GetId() == SIT_IND_FORWARDING_AT_COMMAND);
    EXPECT_TRUE(testAdapter.GetParameter());
    EXPECT_TRUE(testAdapter.GetParameterLength() == size);
    EXPECT_TRUE(testAdapter.GetCommandLength() == static_cast<uint32_t>(testVector.length));
    EXPECT_TRUE(TextUtils::Equals(testAdapter.GetCommand(), testVector.data));
}

TEST(ProtocolMiscAdapterLegacy, ProtocolOemModemInfoIndAdapterLegacy) {
    sit_oem_nw_info_ind testVector = {{}, 1, 0, {}};
    size_t size = sizeof(sit_oem_nw_info_ind) - sizeof(RCM_IND_HEADER);
    auto modemData = std::unique_ptr<ModemData>(
                            ModemDataBuilder::BuildIndication(SIT_OEM_IND_NW_INFO,
                                            (char *)&testVector + sizeof(RCM_IND_HEADER), size));
    ProtocolOemModemInfoIndAdapterLegacy testAdapter(modemData.get());
    EXPECT_TRUE(testAdapter.GetType() == RCM_TYPE_INDICATION);
    EXPECT_TRUE(testAdapter.GetId() == SIT_OEM_IND_NW_INFO);
    EXPECT_TRUE(testAdapter.GetParameter());
    EXPECT_TRUE(testAdapter.GetParameterLength() == size);
    EXPECT_TRUE(testAdapter.GetCommandType() == static_cast<int32_t>(testVector.command));
    EXPECT_TRUE(testAdapter.GetSize() == static_cast<uint32_t>(testVector.length));
}

TEST(ProtocolMiscAdapterLegacy, ProtocolMiscSelflogStatusAdapterLegacy) {
    uint8_t testVector = 1;
    auto modemData = std::unique_ptr<ModemData>(
                            ModemDataBuilder::BuildIndication(SIT_IND_SELFLOG_STATUS,
                                            &testVector, sizeof(testVector)));
    ProtocolMiscSelflogStatusAdapterLegacy testAdapter(modemData.get());
    EXPECT_TRUE(testAdapter.GetType() == RCM_TYPE_INDICATION);
    EXPECT_TRUE(testAdapter.GetId() == SIT_IND_SELFLOG_STATUS);
    EXPECT_TRUE(testAdapter.GetParameter());
    EXPECT_TRUE(testAdapter.GetParameterLength() == sizeof(testVector));
    EXPECT_TRUE(testAdapter.GetIndSelflogStatus() == testVector);
}

TEST(ProtocolMiscAdapterLegacy, ProtocolCaBandwidthFilterIndAdapterLegacy) {
    sit_oem_ca_bw_filter_ind testVector = {{}, 2, 2000};
    size_t size = sizeof(sit_oem_ca_bw_filter_ind) - sizeof(RCM_IND_HEADER);
    auto modemData = std::unique_ptr<ModemData>(
                            ModemDataBuilder::BuildIndication(SIT_OEM_IND_CA_BW_FILTER,
                                            (char *)&testVector + sizeof(RCM_IND_HEADER), size));
    ProtocolCaBandwidthFilterIndAdapterLegacy testAdapter(modemData.get());
    EXPECT_TRUE(testAdapter.GetType() == RCM_TYPE_INDICATION);
    EXPECT_TRUE(testAdapter.GetId() == SIT_OEM_IND_CA_BW_FILTER);
    EXPECT_TRUE(testAdapter.GetParameter());
    EXPECT_TRUE(testAdapter.GetParameterLength() == size);
    EXPECT_TRUE(testAdapter.GetCaConfig() == (testVector.ca_config > 0 ? 1 : 0));
    EXPECT_TRUE(testAdapter.GetNRB() == static_cast<int32_t>(testVector.num_resource_block));
}

TEST(ProtocolMiscAdapterLegacy, ProtocolMiscCurrentLinkCapacityEstimateAdapterLegacy) {
    sit_misc_current_link_capa_estimate_ind testVector = {{}, 1000, 2000, 3000, 4000};
    size_t size = sizeof(sit_misc_current_link_capa_estimate_ind) - sizeof(RCM_IND_HEADER);
    auto modemData = std::unique_ptr<ModemData>(
                            ModemDataBuilder::BuildIndication(SIT_IND_CURRENT_LINK_CAPACITY_ESTIMATE,
                                            (char *)&testVector + sizeof(RCM_IND_HEADER), size));
    ProtocolMiscCurrentLinkCapacityEstimateAdapterLegacy  testAdapter(modemData.get());
    EXPECT_TRUE(testAdapter.GetType() == RCM_TYPE_INDICATION);
    EXPECT_TRUE(testAdapter.GetId() == SIT_IND_CURRENT_LINK_CAPACITY_ESTIMATE);
    EXPECT_TRUE(testAdapter.GetParameter());
    EXPECT_TRUE(testAdapter.GetParameterLength() == size);
    EXPECT_TRUE(testAdapter.GetDlCapaKbps() == static_cast<int32_t>(testVector.dl_capa_kbps));
    EXPECT_TRUE(testAdapter.GetUlCapaKbps() == static_cast<int32_t>(testVector.ul_capa_kbps));
    EXPECT_TRUE(testAdapter.GetSecDlCapaKbps() == static_cast<int32_t>(testVector.dcnr_sec_dl_capa_kbps));
    EXPECT_TRUE(testAdapter.GetSecUlCapaKbps() == static_cast<int32_t>(testVector.dcnr_sec_ul_capa_kbps));
}

TEST(ProtocolMiscAdapterLegacy, ProtocolMiscEndcCapabilityIndAdapterLegacy) {
    sit_oem_endc_capability_ind testVector = {{}, 1, 2};
    size_t size = sizeof(sit_oem_endc_capability_ind) - sizeof(RCM_IND_HEADER);
    auto modemData = std::unique_ptr<ModemData>(
                            ModemDataBuilder::BuildIndication(SIT_OEM_IND_ENDC_CAPABILITY,
                                            (char *)&testVector + sizeof(RCM_IND_HEADER), size));
    ProtocolMiscEndcCapabilityIndAdapterLegacy  testAdapter(modemData.get());
    EXPECT_TRUE(testAdapter.GetType() == RCM_TYPE_INDICATION);
    EXPECT_TRUE(testAdapter.GetId() == SIT_OEM_IND_ENDC_CAPABILITY);
    EXPECT_TRUE(testAdapter.GetParameter());
    EXPECT_TRUE(testAdapter.GetParameterLength() == size);
    EXPECT_TRUE(testAdapter.GetCapability() == static_cast<int32_t>(testVector.endc_capability));
    EXPECT_TRUE(testAdapter.GetCause() == static_cast<int32_t>(testVector.cause));
}

TEST(ProtocolMiscAdapterLegacy, ProtocolMiscSetSelflogProfileAdapterLegacy) {
    uint8_t testVector = 1;
    auto modemData = std::unique_ptr<ModemData>(
                            ModemDataBuilder::BuildResponse(SIT_SET_SELFLOG_PROFILE, 0, 0,
                                            &testVector, sizeof(testVector)));
    ProtocolMiscSetSelflogProfileAdapterLegacy testAdapter(modemData.get());
    EXPECT_TRUE(testAdapter.GetType() == RCM_TYPE_RESPONSE);
    EXPECT_TRUE(testAdapter.GetId() == SIT_SET_SELFLOG_PROFILE);
    EXPECT_TRUE(testAdapter.GetErrorCode() == RIL_E_SUCCESS);
    EXPECT_TRUE(testAdapter.GetParameter());
    EXPECT_TRUE(testAdapter.GetParameterLength() == sizeof(testVector));
    EXPECT_TRUE(testAdapter.GetResult() == static_cast<int32_t>(testVector));
}

TEST(ProtocolMiscAdapterLegacy, ProtocolMiscGetBinaryVersionInfoAdapterLegacy) {
    std::string version = "test_version";
    size_t size = version.size() + 1;
    std::vector<uint8_t> testVector(sizeof(uint16_t) + size);
    uint8_t *p = testVector.data();
    *(uint16_t*)p = static_cast<uint16_t>(size);
    memcpy(p + sizeof(uint16_t), version.c_str(), size);
    auto modemData = std::unique_ptr<ModemData>(
                            ModemDataBuilder::BuildResponse(SIT_OEM_GET_BINARY_VERSION_INFO, 0, 0,
                                            testVector.data(), testVector.size()));
    ProtocolMiscGetBinaryVersionInfoAdapterLegacy testAdapter(modemData.get());
    EXPECT_TRUE(testAdapter.GetType() == RCM_TYPE_RESPONSE);
    EXPECT_TRUE(testAdapter.GetId() == SIT_OEM_GET_BINARY_VERSION_INFO);
    EXPECT_TRUE(testAdapter.GetErrorCode() == RIL_E_SUCCESS);
    EXPECT_TRUE(testAdapter.GetParameter());
    EXPECT_TRUE(testAdapter.GetParameterLength() == testVector.size());
    EXPECT_TRUE(testAdapter.GetVersionInfoLength() == static_cast<int32_t>(version.size() + 1));
    EXPECT_TRUE(version == testAdapter.GetData());
}

TEST(ProtocolMiscAdapterLegacy, ProtocolRfConnectionStateAdapterLegacy) {
    uint8_t testVector = 1;
    auto modemData = std::unique_ptr<ModemData>(
                            ModemDataBuilder::BuildIndication(SIT_IND_RF_CONNECTION,
                                            &testVector, sizeof(testVector)));
    ProtocolRfConnectionStateAdapterLegacy testAdapter(modemData.get());
    EXPECT_TRUE(testAdapter.GetType() == RCM_TYPE_INDICATION);
    EXPECT_TRUE(testAdapter.GetId() == SIT_IND_RF_CONNECTION);
    EXPECT_TRUE(testAdapter.GetParameter());
    EXPECT_TRUE(testAdapter.GetParameterLength() == sizeof(testVector));
    EXPECT_TRUE(testAdapter.GetState() == testVector);
}

TEST(ProtocolMiscAdapterLegacy, ProtocolMiscSetDeviceInfoAdapterLegacy) {
    auto modemData = std::unique_ptr<ModemData>(
                            ModemDataBuilder::BuildResponse(SIT_SET_DEVICE_INFO, 0, 0));
    ProtocolMiscSetDeviceInfoAdapterLegacy testAdapter(modemData.get());
    EXPECT_TRUE(testAdapter.GetType() == RCM_TYPE_RESPONSE);
    EXPECT_TRUE(testAdapter.GetId() == SIT_SET_DEVICE_INFO);
    EXPECT_TRUE(testAdapter.GetErrorCode() == RIL_E_SUCCESS);
}

TEST(ProtocolMiscAdapterLegacy, ProtocolMiscDropMtCallInfoIndAdapterLegacy) {
    uint32_t testVector = 10;
    auto modemData = std::unique_ptr<ModemData>(
                            ModemDataBuilder::BuildIndication(SIT_OEM_IND_DROPPED_MT_CALL_INFO,
                                            &testVector, sizeof(testVector)));
    ProtocolMiscDropMtCallInfoIndAdapterLegacy  testAdapter(modemData.get());
    EXPECT_TRUE(testAdapter.GetType() == RCM_TYPE_INDICATION);
    EXPECT_TRUE(testAdapter.GetId() == SIT_OEM_IND_DROPPED_MT_CALL_INFO);
    EXPECT_TRUE(testAdapter.GetParameter());
    EXPECT_TRUE(testAdapter.GetParameterLength() == sizeof(testVector));
}

TEST(ProtocolMiscAdapterLegacy, ProtocolMiscCarrierConfigSimInfoIndAdapterLegacy) {
    sit_ind_carrier_config_sim_info testVector {};
    size_t size = sizeof(sit_ind_carrier_config_sim_info) - sizeof(RCM_IND_HEADER);
    auto modemData = std::unique_ptr<ModemData>(
                            ModemDataBuilder::BuildIndication(SIT_IND_CARRIER_CONFIG_SIM_INFO,
                                            (char *)&testVector + sizeof(RCM_IND_HEADER), size));
    ProtocolMiscCarrierConfigSimInfoIndAdapterLegacy  testAdapter(modemData.get());
    EXPECT_TRUE(testAdapter.GetType() == RCM_TYPE_INDICATION);
    EXPECT_TRUE(testAdapter.GetId() == SIT_IND_CARRIER_CONFIG_SIM_INFO);
}

TEST(ProtocolMiscAdapterLegacy, ProtocolMiscCarrierConfigStatusIndAdapterLegacy) {
    uint32_t testVector[] = {1, 10};
    auto modemData = std::unique_ptr<ModemData>(
                            ModemDataBuilder::BuildIndication(SIT_IND_CARRIER_CONFIG_STATUS,
                                            testVector, sizeof(testVector)));
    ProtocolMiscCarrierConfigStatusIndAdapterLegacy  testAdapter(modemData.get());
    EXPECT_TRUE(testAdapter.GetType() == RCM_TYPE_INDICATION);
    EXPECT_TRUE(testAdapter.GetId() == SIT_IND_CARRIER_CONFIG_STATUS);
    EXPECT_TRUE(testAdapter.GetParameter());
    EXPECT_TRUE(testAdapter.GetParameterLength() == sizeof(testVector));
    EXPECT_TRUE(testAdapter.GetStatus() == static_cast<int32_t>(testVector[0]));
    EXPECT_TRUE(testAdapter.GetCause() == static_cast<int32_t>(testVector[1]));
}

TEST(ProtocolMiscAdapterLegacy, ProtocolMiscGetTraceDumpCfgAdapterLegacy) {
    uint8_t testVector = 3;
    auto modemData = std::unique_ptr<ModemData>(
                            ModemDataBuilder::BuildResponse(SIT_OEM_GET_TRACE_DUMP_CFG, 0, 0,
                                            &testVector, sizeof(testVector)));
    ProtocolMiscGetTraceDumpCfgAdapterLegacy testAdapter(modemData.get());
    EXPECT_TRUE(testAdapter.GetType() == RCM_TYPE_RESPONSE);
    EXPECT_TRUE(testAdapter.GetId() == SIT_OEM_GET_TRACE_DUMP_CFG);
    EXPECT_TRUE(testAdapter.GetErrorCode() == RIL_E_SUCCESS);
    EXPECT_TRUE(testAdapter.GetParameter());
    EXPECT_TRUE(testAdapter.GetParameterLength() == sizeof(testVector));
    EXPECT_TRUE(testAdapter.GetConfig() == testVector);
}

TEST(ProtocolMiscAdapterLegacy, ProtocolMiscGetUeOperationModeAdapterLegacy) {
    uint8_t testVector = 3;
    auto modemData = std::unique_ptr<ModemData>(
                            ModemDataBuilder::BuildResponse(SIT_OEM_GET_UE_OPERATION_MODE, 0, 0,
                                            &testVector, sizeof(testVector)));
    ProtocolMiscGetUeOperationModeAdapterLegacy testAdapter(modemData.get());
    EXPECT_TRUE(testAdapter.GetType() == RCM_TYPE_RESPONSE);
    EXPECT_TRUE(testAdapter.GetId() == SIT_OEM_GET_UE_OPERATION_MODE);
    EXPECT_TRUE(testAdapter.GetErrorCode() == RIL_E_SUCCESS);
    EXPECT_TRUE(testAdapter.GetParameter());
    EXPECT_TRUE(testAdapter.GetParameterLength() == sizeof(testVector));
    EXPECT_TRUE(testAdapter.GetMode() == testVector);
}

TEST(ProtocolMiscAdapterLegacy, ProtocolMiscGetNrrrmMinReqEnableAdapterLegacy) {
    uint8_t testVector = 1;
    auto modemData = std::unique_ptr<ModemData>(
                            ModemDataBuilder::BuildResponse(SIT_OEM_GET_NRRRM_MIN_REQ_ENABLE, 0, 0,
                                            &testVector, sizeof(testVector)));
    ProtocolMiscGetNrrrmMinReqEnableAdapterLegacy testAdapter(modemData.get());
    EXPECT_TRUE(testAdapter.GetType() == RCM_TYPE_RESPONSE);
    EXPECT_TRUE(testAdapter.GetId() == SIT_OEM_GET_NRRRM_MIN_REQ_ENABLE);
    EXPECT_TRUE(testAdapter.GetErrorCode() == RIL_E_SUCCESS);
    EXPECT_TRUE(testAdapter.GetParameter());
    EXPECT_TRUE(testAdapter.GetParameterLength() == sizeof(testVector));
    EXPECT_TRUE(testAdapter.GetEnable() == testVector);
}

TEST(ProtocolMiscAdapterLegacy, ProtocolMiscGetVcrtAdapterLegacy) {
    uint8_t vcrtMode = 1;
    auto modemData = std::unique_ptr<ModemData>(
                            ModemDataBuilder::BuildResponse(SIT_OEM_GET_VCRT, 0, 0,
                                            &vcrtMode, sizeof(vcrtMode)));
    ProtocolMiscGetVcrtAdapterLegacy testAdapter(modemData.get());
    EXPECT_TRUE(testAdapter.GetType() == RCM_TYPE_RESPONSE);
    EXPECT_TRUE(testAdapter.GetId() == SIT_OEM_GET_VCRT);
    EXPECT_TRUE(testAdapter.GetErrorCode() == RIL_E_SUCCESS);
    EXPECT_TRUE(testAdapter.GetParameter());
    EXPECT_TRUE(testAdapter.GetParameterLength() == sizeof(vcrtMode));
    EXPECT_TRUE(testAdapter.GetVcrtMode() == vcrtMode);
}
