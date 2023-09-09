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
#include <gtest/gtest.h>

#include "modemdatabuilder.h"
#include "modemdata.h"
#include "rildef.h"
#include "protocoladapter.h"
#include "legacy/misc/protocolmiscbuilderlegacy.h"

TEST(ProtocolMiscBuilderLegacy, ProtocolMiscBuilderLegacy_BuildSetSrsPwrLimit) {
    ProtocolMiscBuilderLegacy testBuilder;
    ModemData *modemData = testBuilder.BuildSetSrsPwrLimit(3);
    ASSERT_NE(modemData, nullptr);
    EXPECT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_OEM_SET_SRS_PWR_LIMIT);
    EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_oem_set_srs_pwr_limit_req));
    EXPECT_EQ(testAdapter.GetParameterLength(), 2U);
    sit_oem_set_srs_pwr_limit_req *p = (sit_oem_set_srs_pwr_limit_req *)modemData->GetRawData();
    EXPECT_EQ(p->level, 3);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolMiscBuilderLegacy, ProtocolMiscBuilderLegacy_BuildSetTraceDumpCfg) {
    ProtocolMiscBuilderLegacy testBuilder;
    ModemData *modemData = testBuilder.BuildSetTraceDumpCfg((BYTE)1);
    ASSERT_NE(modemData, nullptr);
    EXPECT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_OEM_SET_TRACE_DUMP_CFG);
    EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_oem_set_trace_dump_cfg_req));
    EXPECT_EQ(testAdapter.GetParameterLength(), 1U);
    sit_oem_set_trace_dump_cfg_req *p = (sit_oem_set_trace_dump_cfg_req *)modemData->GetRawData();
    EXPECT_EQ(p->config, 1);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolMiscBuilderLegacy, ProtocolMiscBuilderLegacy_GetModemActivityInfo) {
    ProtocolMiscBuilderLegacy testBuilder;
    auto modemData = std::unique_ptr<ModemData>(testBuilder.GetModemActivityInfo());
    ASSERT_TRUE(modemData);
    ProtocolReqAdapter testAdapter(modemData.get());
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_ACTIVITY_INFO);
    EXPECT_TRUE(testAdapter.GetToken() != INT_MAX);
}

TEST(ProtocolMiscBuilderLegacy, ProtocolMiscBuilderLegacy_BuildRadioConfigReset) {
    ProtocolMiscBuilderLegacy testBuilder;

    struct TestArgs {
        int input;
        int result;
    } testVector[] = {
        {0, 2},
        {1, 2},
        {2, 1},
        {3, 0},
    };

    for (auto& arg : testVector) {
        auto modemData = std::unique_ptr<ModemData>(testBuilder.BuildRadioConfigReset(arg.input));
        ASSERT_NE(modemData, nullptr);
        EXPECT_NE(modemData, nullptr);

        ProtocolReqAdapter testAdapter(modemData.get());
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_SET_CFG_DEFAULT);
        EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_misc_set_cfg_default_req));
        EXPECT_EQ(testAdapter.GetParameterLength(), 1U);
        sit_misc_set_cfg_default_req *p = (sit_misc_set_cfg_default_req *)modemData->GetRawData();
        EXPECT_EQ(p->type, arg.result);
    }
}

TEST(ProtocolMiscBuilderLegacy, ProtocolMiscBuilderLegacy_BuildSetVcrt) {
    ProtocolMiscBuilderLegacy testBuilder;
    ModemData *modemData = testBuilder.BuildSetVcrt(1);
    ASSERT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_OEM_SET_VCRT);
    EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_oem_set_vcrt_req));
    EXPECT_EQ(testAdapter.GetParameterLength(), 1U);
    sit_oem_set_vcrt_req *p = (sit_oem_set_vcrt_req *)modemData->GetRawData();
    EXPECT_EQ(p->vcrtMode, 1);

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolMiscBuilderLegacy, ProtocolMiscBuilderLegacy_BuildGetVcrt) {
    ProtocolMiscBuilderLegacy testBuilder;
    auto modemData = std::unique_ptr<ModemData>(testBuilder.BuildGetVcrt());
    ASSERT_TRUE(modemData);
    ProtocolReqAdapter testAdapter(modemData.get());
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_OEM_GET_VCRT);
}
