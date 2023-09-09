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
 * nvitemdata_test.cpp
 *
 *  Created on: 2021. 8. 9.
 *      Author: sungwoo48.choi
 */
#include <gtest/gtest.h>
#include "nvitemdata.h"
#include "librilutils/textutils.h"

TEST(NvItemData, NvReadItemRequestData) {
    RIL_NV_ReadItem testVector = { RIL_NV_Item::RIL_NV_CDMA_MEID };
    NvReadItemRequestData test(RIL_REQUEST_NV_READ_ITEM, 0);
    EXPECT_EQ(test.encode(0, 0), -1);
    EXPECT_EQ(test.encode((char *)&testVector, 0), -1);
    EXPECT_EQ(test.encode((char *)&testVector, sizeof(RIL_NV_ReadItem) + 1), -1);
    EXPECT_EQ(test.encode((char *)&testVector, sizeof(RIL_NV_ReadItem)), 0);
    EXPECT_EQ(test.GetNvItemID(), (int)testVector.itemID);

    NvReadItemRequestData *clone = test.Clone();
    EXPECT_NE(clone, nullptr);
    if (clone) {
        EXPECT_EQ(clone->GetNvItemID(), test.GetNvItemID());
        delete clone;
    }
}

TEST(NvItemData, NvWriteItemRequestData) {
    RIL_NV_WriteItem testVector = { RIL_NV_Item::RIL_NV_CDMA_MEID, (char *)"12345678" };
    NvWriteItemRequestData test(RIL_REQUEST_NV_READ_ITEM, 0);
    EXPECT_EQ(test.encode(0, 0), -1);
    EXPECT_EQ(test.encode((char *)&testVector, 0), -1);
    EXPECT_EQ(test.encode((char *)&testVector, sizeof(RIL_NV_WriteItem) + 1), -1);
    EXPECT_EQ(test.encode((char *)&testVector, sizeof(RIL_NV_WriteItem)), 0);
    EXPECT_EQ(test.GetNvItemID(), (int)testVector.itemID);
    EXPECT_TRUE(TextUtils::Equals(test.GetValue(), testVector.value));
    EXPECT_EQ(test.GetValueLength(), (int)strlen(testVector.value));

    NvWriteItemRequestData *clone = test.Clone();
    EXPECT_NE(clone, nullptr);
    if (clone) {
        EXPECT_EQ(clone->GetNvItemID(), test.GetNvItemID());
        EXPECT_TRUE(TextUtils::Equals(clone->GetValue(), test.GetValue()));
        delete clone;
    }
}

