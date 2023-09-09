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
 * sdmdoc_test.cpp
 *
 *  Created on: 2021. 9. 3.
 *      Author: sungwoo48.choi
 */
#include <gtest/gtest.h>
#include <time.h>
#include "sdmdoc.h"

#include "dmd_test_util.h"
#include "systemproperty.h"

TEST(SdmDoc, SdmDoc_interface) {
    class SdmDocImpl : public SdmDoc {
    public:
        bool onCreated = false;
        bool onClosed = false;

        SdmDocImpl() = default;
        virtual ~SdmDocImpl() = default;

        bool onCreate() {
            onCreated = true;
            return true;
        }
        void onClose() {
            onClosed = true;
        }
        int write(const void *data, size_t datalen) {
            if (data) {
                return datalen;
            }
            return 0;
        }
        size_t size() { return 0; }
    };

    SdmDocImpl test;
    std::string filepath = "/data/vendor/slog/test.sdm";
    EXPECT_TRUE(test.create(filepath));
    EXPECT_TRUE(test.getFilepath() == filepath);
    EXPECT_TRUE(test.onCreated);
    EXPECT_EQ(test.write(nullptr, 10), 0);
    EXPECT_EQ(test.write("test", 4), 4);
    EXPECT_EQ(test.size(), 0);
    EXPECT_TRUE(test.toString().length() > 0);
    test.close();
    EXPECT_TRUE(test.onClosed);
}

TEST(SdmDoc, SdmDocV2HeaderBuilder) {
    SdmDocV2HeaderBuilder test;
    test.tag("SDM2:")
        .version("1234")
        .uid(1, 0)
        .codebook("1234")
        .timestamp(1234UL)
        .tzid("Asia/Seoul")
        .tzoffset(1234567UL);
    size_t datalen = test.size();
    EXPECT_EQ(datalen, 29 + 2 + 2 + 8 + 2 + std::string("Asia/Seoul").length() + 2 + 2 + 8);
}

TEST(SdmDoc, SdmDocV2) {
    srand(time(0));

    uint32_t uid = 100, seq = 0;
    MemoryBufferOutputStream hook; // hook
    SdmDocV2 *testDoc = SdmDocV2::newInstance(uid, seq);
    EXPECT_NE(testDoc, nullptr);

    if (testDoc) {
        testDoc->setOutputStream(&hook);
        std::string filepath = "/data/vendor/slog/test.sdm";
        EXPECT_TRUE(testDoc->create(filepath));
        EXPECT_TRUE(testDoc->getFilepath().compare(filepath) == 0);
        EXPECT_EQ(testDoc->uid(), uid);
        EXPECT_EQ(testDoc->seq(), seq);


        TestVector testVector[2];
        size_t size = sizeof(testVector) / sizeof(testVector[0]);
        for (size_t i = 0; i < size; i++) {
            int ret = testDoc->write(testVector[i].data, testVector[i].datalen);
            EXPECT_EQ(ret, (int)testVector[i].datalen);
            std::string testHookStr = ::toHexdump((uint8_t *)hook.cur() - ret, ret);
            std::string strData = ::toHexdump(testVector[i].data, testVector[i].datalen);
            EXPECT_TRUE(testHookStr. compare(strData) == 0);
        }

        testDoc->close();
        delete testDoc;

        //dlog("%s", hook.toHexdump().c_str());
    }
}
