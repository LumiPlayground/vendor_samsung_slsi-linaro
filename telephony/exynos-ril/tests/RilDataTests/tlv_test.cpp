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
 * tlv_test.cpp
 *
 *  Created on: 2021. 8. 3.
 */
#include <gtest/gtest.h>
#include "stk/tlvparser.h"
#include "librilutils/textutils.h"

TEST(STK, CTLV_tags) {
    struct {
        int tag;
        const char *tagString;
    } testVector[] = {
        { TAG_COMMAND_DETAIL, "TAG_COMMAND_DETAIL" },
        { TAG_DEVICE_IDENTITY, "TAG_DEVICE_IDENTITY" },
        { TAG_RESULT, "TAG_RESULT" },
        { TAG_DURATION, "TAG_DURATION" },
        { TAG_ALPHA_IDENTIFIER, "TAG_ALPHA_IDENTIFIER" },
        { TAG_ADDRESS, "TAG_ADDRESS" },
        { TAG_SUB_ADDRESS, "TAG_SUB_ADDRESS" },
        { TAG_TEXT_STRING, "TAG_TEXT_STRING" },
        { TAG_EVENT_LIST, "TAG_EVENT_LIST" },
        { TAG_ICON_IDENTIFIER, "TAG_ICON_IDENTIFIER" },
        { TAG_BEARER_DESCRIPTION, "TAG_BEARER_DESCRIPTION" },
        { TAG_CHANNEL_DATA, "TAG_CHANNEL_DATA" },
        { TAG_CHANNEL_DATA_LENGTH, "TAG_CHANNEL_DATA_LENGTH" },
        { TAG_CHANNEL_STATUS, "TAG_CHANNEL_STATUS" },
        { TAG_BUFFER_SIZE, "TAG_BUFFER_SIZE" },
        { TAG_UICC_TERMINAL_INTERFACE_TRANSPORT_LEVEL, "TAG_UICC_TERMINAL_INTERFACE_TRANSPORT_LEVEL" },
        { TAG_OTHER_ADDRESS, "TAG_OTHER_ADDRESS" },
        { TAG_NETWORK_ACCESS_NAME, "TAG_NETWORK_ACCESS_NAME" },
        { TAG_REMOTE_ENTITY_ADDRESS, "TAG_REMOTE_ENTITY_ADDRESS" },
        { TAG_TEXT_ATTRIBUTE, "TAG_TEXT_ATTRIBUTE" },
        { TAG_FRAME_IDENTIFIER, "TAG_FRAME_IDENTIFIER" },
        { TAG_UTRAN_EUTRAN_MEASUREMENT_QUALIFIER, "TAG_UTRAN_EUTRAN_MEASUREMENT_QUALIFIER" },
        { TAG_CSG_ID_LIST, "TAG_CSG_ID_LIST" },
        { TAG_NONE, "Unknown" },
    };
    size_t size = sizeof(testVector) / sizeof(testVector[0]);

    for (size_t i = 0; i < size; i++) {
        BYTE val[1];
        CTLV test((BYTE)testVector[i].tag, val, sizeof(val));
        EXPECT_EQ(test.GetTag(), (BYTE)testVector[i].tag);
        EXPECT_TRUE(TextUtils::Equals(test.GetTagString(), testVector[i].tagString));
        EXPECT_NE(test.GetValue(), nullptr);
        EXPECT_EQ((unsigned int)/* NEED TO CHECK */ test.GetLength(), sizeof(val));
        if (test.GetValue()) {
            EXPECT_TRUE(memcmp(test.GetValue(), val, sizeof(val)) == 0);
        }
        BYTE *rawdata = test.GetRawData();
        EXPECT_NE(rawdata, nullptr);
        EXPECT_EQ(test.GetRawDataLength(), 3);
        if (rawdata) {
            BYTE tmp[3] = { (BYTE)testVector[i].tag, 1, val[0] } ;
            EXPECT_TRUE(memcmp(rawdata, tmp, sizeof(tmp)) == 0);
            delete[] rawdata;
        }
    }
}

TEST(STK, CTLV_three_bytes) {
    BYTE val[3] = { 0x7F, 1, 2 };
    CTLV test;
    test.Set(0, 0, 0);  // invalid
    test.Set(TAG_NONE, val, sizeof(val));
    EXPECT_NE(test.GetValue(), nullptr);
    EXPECT_EQ(test.GetLength(), 3);
    if (test.GetValue()) {
        EXPECT_TRUE(memcmp(test.GetValue(), val, sizeof(val)) == 0);
    }
    // !!!test.GetRawData() always returns NULL in case of three-bytes test.
    EXPECT_EQ(test.GetRawData(), nullptr);
}

TEST(STK, CTLV_variable_size) {
    int testVector[] = {
            127, 30000, 70000,
    };
    size_t size = sizeof(testVector) / sizeof(testVector[0]);
    for (size_t i = 0; i < size; i++) {
        BYTE *buf = new BYTE[testVector[i]];
        if (buf) {
            CTLV test(TAG_COMMAND_DETAIL, buf, testVector[i]);
            EXPECT_TRUE(test.GetRawDataLength() > testVector[i]);
            delete[] buf;
        }
    }
}
