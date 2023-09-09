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
#include "modemdata.h"
#include "protocoladapter.h"
#include "protocolbuilder.h"
#include "sitdef.h"
#include "tokengen.h"

static bool s_init = []() { TokenGen::Init(); return true; }();

class testProtocolBuilder : public ProtocolBuilder {
public:
    void testInitRequestHeader(RCM_HEADER *hdr, int id) {
        InitRequestHeader(hdr, id);
    }
    void testInitRequestHeader(RCM_HEADER *hdr, int id, int length) {
        InitRequestHeader(hdr, id, length);
    }
    void testInitIndRequestHeader(RCM_IND_HEADER *hdr, int id, int length) {
        InitIndRequestHeader(hdr, id, length);
    }
    void testInitRequestHeader(RCM_HEADER *hdr, int id, int length, RCM_TOKEN token) {
        InitRequestHeader(hdr, id, length, token);
    }
};

TEST(ProtocolBuilder, testInitRequestHeader1) {
    TokenGen::Init();

    RCM_HEADER hdr;
    testProtocolBuilder testBuilder;
    testBuilder.testInitRequestHeader(&hdr, SIT_GET_PS_REG_STATE);

    ModemData modemData((char *)&hdr, sizeof(hdr));
    ProtocolReqAdapter testAdapter(&modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_PS_REG_STATE);
    EXPECT_NE(testAdapter.GetToken(), (unsigned int)/* NEED TO CHECK */ -1);
}

TEST(ProtocolBuilder, testInitRequestHeader2) {
    TokenGen::Init();

    RCM_HEADER hdr;
    unsigned int length = sizeof(hdr) + 10;
    testProtocolBuilder testBuilder;
    testBuilder.testInitRequestHeader(&hdr, SIT_GET_PS_REG_STATE, length);

    ModemData modemData((char *)&hdr, sizeof(hdr));
    ProtocolReqAdapter testAdapter(&modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_PS_REG_STATE);
    EXPECT_NE(testAdapter.GetToken(), (unsigned int)/* NEED TO CHECK */ -1);
    EXPECT_EQ(testAdapter.GetLength(), length);
}

TEST(ProtocolBuilder, testInitRequestHeader3) {
    TokenGen::Init();

    RCM_HEADER hdr;
    unsigned int length = sizeof(hdr);
    uint32_t token = 150;
    testProtocolBuilder testBuilder;
    testBuilder.testInitRequestHeader(&hdr, SIT_GET_PS_REG_STATE, length, token);

    ModemData modemData((char *)&hdr, sizeof(hdr));
    ProtocolReqAdapter testAdapter(&modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_PS_REG_STATE);
    EXPECT_EQ(testAdapter.GetToken(), token);
    EXPECT_EQ(testAdapter.GetLength(), length);
}

TEST(ProtocolBuilder, testInitIndRequestHeader) {
    TokenGen::Init();

    RCM_IND_HEADER hdr;
    unsigned int length = sizeof(hdr);
    testProtocolBuilder testBuilder;
    testBuilder.testInitIndRequestHeader(&hdr, SIT_IND_AIMS_CALL_MANAGE, length);

    ModemData modemData((char *)&hdr, sizeof(hdr));
    ProtocolIndAdapter testAdapter(&modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
    EXPECT_EQ(testAdapter.GetId(), SIT_IND_AIMS_CALL_MANAGE);
    EXPECT_EQ(testAdapter.GetLength(), length);
}
