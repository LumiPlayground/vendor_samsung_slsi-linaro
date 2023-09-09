/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */
#include <gtest/gtest.h>
#include "smsdata.h"
#include "librilutils/textutils.h"

inline std::string toHexString(uint8_t *data, size_t datalen) {
    const char *HEX = "0123456789ABCDEF";
    std::stringstream ss;
    if (data) {
        for (size_t i = 0; i < datalen; i++) {
            ss << HEX[(data[i] >> 4) & 0xF];
            ss << HEX[data[i] & 0xF];
        }
    }
    return ss.str();
}

TEST(SmsData, Pdu) {
    char testVector[MAX_GSM_SMS_TPDU_SIZE];
    size_t size = 140;
    Pdu();
    Pdu test(testVector, size);
    EXPECT_EQ(test.mLen, (int)size);
    EXPECT_TRUE(memcmp(test.mData, testVector, size) == 0);
    EXPECT_TRUE(TextUtils::Equals(test.ToHexString(), toHexString((uint8_t *)testVector, size)));
    test = Pdu((char *)toHexString((uint8_t *)testVector, size).c_str());
    EXPECT_EQ(test.mLen, (int)size);
    test.SetRawData((char *)toHexString((uint8_t *)testVector, size/2).c_str());
    EXPECT_EQ(test.mLen, (int)size / 2);
    EXPECT_TRUE(TextUtils::Equals(test.ToHexString(), toHexString((uint8_t *)testVector, size/2)));

    Pdu copy = test;
    EXPECT_TRUE(TextUtils::Equals(copy.ToHexString(), toHexString((uint8_t *)testVector, size/2)));
    EXPECT_EQ(copy.mLen, test.mLen);
    EXPECT_TRUE(memcmp(copy.mData, test.mData, copy.mLen) == 0);
}
