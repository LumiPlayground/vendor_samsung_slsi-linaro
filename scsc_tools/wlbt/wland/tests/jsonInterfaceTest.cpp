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
#include <gmock/gmock.h>

#include "../jsonInterface.h"

class JsonAdapterTest : public ::testing::Test {
 protected:
    jsonInterface* json;
    virtual void SetUp() {
        json = new jsonInterface();
    }
    virtual void TearDown() {
        delete json;
    }
};

TEST_F(JsonAdapterTest, emptyJsonString) {
    EXPECT_FALSE(json->parserJsonString(NULL));
    EXPECT_FALSE(json->parserJsonString(const_cast<char *>("")));
}

TEST_F(JsonAdapterTest, singleBracketJson) {
    EXPECT_FALSE(json->parserJsonString(const_cast<char *>("}")));
    EXPECT_FALSE(json->parserJsonString(const_cast<char *>("{")));
}

TEST_F(JsonAdapterTest, invalidJson) {
    EXPECT_FALSE(json->parserJsonString(const_cast<char *>("{:}")));
    EXPECT_FALSE(json->parserJsonString(const_cast<char *>("{\"key\":}")));
    EXPECT_FALSE(json->parserJsonString(const_cast<char *>("{:\"value\"}")));
}

TEST_F(JsonAdapterTest, invalidDoubleQuotesJson) {
    EXPECT_FALSE(json->parserJsonString(const_cast<char *>("{key:value}")));
    EXPECT_FALSE(json->parserJsonString(const_cast<char *>("{\"key\":value}")));
    EXPECT_FALSE(json->parserJsonString(const_cast<char *>("{key:\"value\"}")));
}

TEST_F(JsonAdapterTest, nomalJsonString) {
    EXPECT_TRUE(json->parserJsonString(const_cast<char *>("{\"key\":\"value\"}")));
    EXPECT_TRUE(json->parserJsonString(const_cast<char *>("{\"key\":\"value\", \"key\":\"value\"}")));
}

TEST_F(JsonAdapterTest, getStringTest) {
    EXPECT_TRUE(json->parserJsonString(const_cast<char *>("{\"key\":\"value\"}")));
    EXPECT_EQ(json->getStringValue("key"), "value");
}

TEST_F(JsonAdapterTest, wifiLogGetStringTest) {
    std::string jsonValue =
        std::string("{\"name\":\"wifilog\", \"exec\":\"start\", \"option\":\"udilog\", \"dir\":\"\"}");

    EXPECT_TRUE(json->parserJsonString(const_cast<char *>(jsonValue.c_str())));

    EXPECT_EQ(json->getStringValue("name"), "wifilog");
    EXPECT_EQ(json->getStringValue("exec"), "start");
    EXPECT_EQ(json->getStringValue("option"), "udilog");
    EXPECT_EQ(json->getStringValue("dir"), "");
}

TEST_F(JsonAdapterTest, btlogLogGetStringTest) {
    std::string jsonValue =
        std::string("{\"name\":\"btlog\", \"exec\":\"start\", \"option\":\"general\", \"dir\":\"\"}");

    EXPECT_TRUE(json->parserJsonString(const_cast<char *>(jsonValue.c_str())));

    EXPECT_EQ(json->getStringValue("name"), "btlog");
    EXPECT_EQ(json->getStringValue("exec"), "start");
    EXPECT_EQ(json->getStringValue("option"), "general");
    EXPECT_EQ(json->getStringValue("dir"), "");
}

TEST_F(JsonAdapterTest, getStringFailTest) {
    std::string jsonValue =
        std::string("{\"name\":\"btlog\", \"exec\":\"start\", \"option\":\"general\", \"dir\":\"\"}");

    EXPECT_TRUE(json->parserJsonString(const_cast<char *>(jsonValue.c_str())));
    EXPECT_EQ(json->getStringValue("invalid_name"), "");
}

TEST_F(JsonAdapterTest, worngOrderUseTest) {
    std::string jsonValue =
        std::string("{\"name\":\"wifilog\", \"exec\":\"start\", \"option\":\"udilog\", \"dir\":\"\"}");

    EXPECT_EQ(json->getStringValue("name"), "");
    EXPECT_TRUE(json->parserJsonString(const_cast<char *>(jsonValue.c_str())));
    EXPECT_EQ(json->getStringValue("name"), "wifilog");
}

