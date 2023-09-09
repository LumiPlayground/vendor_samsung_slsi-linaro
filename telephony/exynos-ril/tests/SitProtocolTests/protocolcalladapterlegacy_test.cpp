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
#include "legacy/cs/protocolcalladapterlegacy.h"
#include "callreqdata.h"
#include "calljsondef.h"
#include "json/json.h"
#include "sitdef.h"

#include <android/log.h>

using namespace std;

TEST(ProtocolCallAdapter, ProtocolSrvccHoIndAdapterLegacy) {
    struct {
        int hoType;
        int hoFailCause;
        int hoStatus;
    } testVector[] = {
        { 1, 0, SIT_CALL_SRVCC_HO_STATUS_STARTED },
        { 1, 0, SIT_CALL_SRVCC_HO_STATUS_COMPLETED },
        { 1, 0, SIT_CALL_SRVCC_HO_STATUS_REINVITED_REQUESTED },
        { 1, 0, SIT_CALL_SRVCC_HO_STATUS_DISCONNECT_REQUESTED },
        { 1, 0, SIT_CALL_SRVCC_HO_STATUS_FAILED },
        { 1, 0, SIT_CALL_SRVCC_HO_STATUS_START_FAILED },
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);
    for(int i = 0; i < size; i++) {
        BYTE srvccInfo[3];
        srvccInfo[0] = testVector[i].hoType;
        srvccInfo[1] = testVector[i].hoStatus;
        srvccInfo[2] = testVector[i].hoFailCause;

        unsigned int totalLen = sizeof(srvccInfo) + sizeof(RCM_IND_HEADER);

        ModemData *modemData = ModemDataBuilder::BuildIndication(SIT_IND_SRVCC_HO, (void *)srvccInfo, sizeof(srvccInfo));
        ASSERT_NE(modemData, nullptr);

        ProtocolSrvccHoIndAdapterLegacy testAdapter(modemData);
        //General checks for indication
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
        EXPECT_EQ(testAdapter.GetId(), SIT_IND_SRVCC_HO);
        EXPECT_EQ(testAdapter.GetLength(),  totalLen);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetParameterLength(), sizeof(srvccInfo));
        //Payload data checks
        EXPECT_EQ(testAdapter.GetSrvccHoType(), testVector[i].hoType);
        EXPECT_EQ(testAdapter.GetSrvccHoStatus(), testVector[i].hoStatus);
        EXPECT_EQ(testAdapter.GetSrvccHoFailCause(), testVector[i].hoFailCause);

        delete modemData;
        modemData = nullptr;
    }
}
