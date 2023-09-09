/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd All Rights Reserved
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
#include "imsdatabuilder.h"
#include "librilutils/textutils.h"

TEST(ImsDataBuilder, BuildImsDataStartImsTrafficResponse_test) {
    typedef struct {
        int32_t reason;
        int32_t causeCode;
        int32_t waitTime;
    } TestVector;

    std::initializer_list<TestVector> testVector = {
        {0, 0, 0}, {0, 1, 2}, {1, 1, 1}, {2, 1, 0}
    };

    for (auto t : testVector) {
        ImsDataStartImsTrafficBuilder builder;
        auto rilData = std::unique_ptr<const RilData>(
                builder.BuildImsDataStartImsTrafficResponse(t.reason, t.causeCode, t.waitTime));
        if (rilData != nullptr) {
            auto data = *(reinterpret_cast<RilImsV2_1::RIL_ConnectionFailureInfo *>(rilData->GetData()));
            EXPECT_TRUE(data.failureReason == t.reason);
            EXPECT_TRUE(data.causeCode == t.causeCode);
            EXPECT_TRUE(data.waitTimeMillis == t.waitTime);
        }
    }
}

TEST(ImsDataBuilder, BuildImsDataConnectionSetupFailure_test) {
    typedef struct {
        int32_t token;
        int32_t reason;
        int32_t causeCode;
        int32_t waitTime;
    } TestVector;

    std::initializer_list<TestVector> testVector = {
        {0, 0, 0, 0}, {0, 1, 2, 3}, {1, 1, 1, 1}, {3, 2, 1, 0}
    };

    for (auto t : testVector) {
        ImsDataConnectionSetupFailureBuilder builder;
        auto rilData = std::unique_ptr<const RilData>(
                builder.BuildImsDataConnectionSetupFailure(t.token, t.reason, t.causeCode, t.waitTime));
        if (rilData != nullptr) {
            auto data = *(reinterpret_cast<RilImsV2_1::RIL_OnConnectionSetupFailure *>(rilData->GetData()));
            EXPECT_TRUE(data.token == t.token);
            EXPECT_TRUE(data.info.failureReason == t.reason);
            EXPECT_TRUE(data.info.causeCode == t.causeCode);
            EXPECT_TRUE(data.info.waitTimeMillis == t.waitTime);
        }
    }
}

TEST(ImsDataBuilder, BuildImsDataTriggerEpsFallbackInd_test) {
    typedef struct {
        int32_t result;
        int32_t rat;
    } TestVector;

    std::initializer_list<TestVector> testVector = {
        {0, 0}, {0, 1}, {1, 0}, {1, 1}
    };

    for (auto t : testVector) {
        ImsDataTriggerEpsFallbackIndBuilder builder;
        auto rilData = std::unique_ptr<const RilData>(
                builder.BuildImsDataTriggerEpsFallbackInd(t.result, t.rat));
        if (rilData != nullptr) {
            auto data = *(reinterpret_cast<RilImsV2_1::RIL_TriggerFpsFallbackInd *>(rilData->GetData()));
            EXPECT_TRUE(data.result == t.result);
            EXPECT_TRUE(data.rat == t.rat);
        }
    }
}