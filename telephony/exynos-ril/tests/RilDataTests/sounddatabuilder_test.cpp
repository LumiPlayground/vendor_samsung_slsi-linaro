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
 * sounddatabuilder_test.cpp
 *
 *  Created on: 2021. 8. 9.
 *      Author: sungwoo48.choi
 */
#include <gtest/gtest.h>
#include "sounddatabuilder.h"

TEST(SoundDataBuilder, BuildGetMuteResponse) {
    SoundDataBuilder testBuilder;
    const RilData *rildata = testBuilder.BuildGetMuteResponse(1);
    EXPECT_NE(rildata, nullptr);
    if (rildata) {
        EXPECT_NE(rildata->GetData(), nullptr);
        EXPECT_EQ(rildata->GetDataLength(), sizeof(int));
        EXPECT_EQ(((int *)rildata->GetData())[0], 1);
        delete rildata;
    }
}

TEST(SoundDataBuilder, BuildWBAMRReportUnsolResponse) {
    SoundDataBuilder testBuilder;
    const RilData *rildata = testBuilder.BuildWBAMRReportUnsolResponse(1, 2);
    EXPECT_NE(rildata, nullptr);
    if (rildata) {
        EXPECT_NE(rildata->GetData(), nullptr);
        EXPECT_EQ(rildata->GetDataLength(), sizeof(int) * 2);
        EXPECT_EQ(((int *)rildata->GetData())[0], 1);
        EXPECT_EQ(((int *)rildata->GetData())[1], 2);
        delete rildata;
    }
}
