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
 * embmsdata_test.cpp
 *
 *  Created on: 2021. 8. 9.
 *      Author: sungwoo48.choi
 */
#include <gtest/gtest.h>
#include "embmsdata.h"

TEST(EmbmsData, EmbmsSessionData) {
    EmbmsSessionData test(RIL_REQUEST_OEM_EMBMS_SET_SESSION, 0);
    EXPECT_EQ(test.encode(0, 0), -1);
    EXPECT_EQ(test.encode(0, sizeof(RIL_EmbmsSessionData)), -1);

    RIL_EmbmsSessionData testVector = {
        1, 2, 3ULL,
        { 1, {}, 2, {} },
        4,
    };
    EXPECT_EQ(test.encode((char *)&testVector, sizeof(testVector)), 0);
    EXPECT_EQ((unsigned int)/* NEED TO CHECK */ test.GetState(), testVector.state);
    EXPECT_EQ(test.GetPriority(), testVector.priority);
    EXPECT_TRUE(test.GetTmgi() == testVector.tmgi);
    EXPECT_EQ((unsigned int)/* NEED TO CHECK */ test.GetInfoBindCount(), testVector.infobindcount);
    RIL_InfoBinding testInfoBinding = test.GetInfoBind();
    EXPECT_TRUE(memcmp(&testInfoBinding, &testVector.infobind, sizeof(RIL_InfoBinding)) == 0);
}
