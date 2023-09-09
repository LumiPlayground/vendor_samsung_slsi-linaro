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
/*
 * simcardstatus_test.cpp
 *
 *  Created on: 2021. 10. 22.
 */
#include <gtest/gtest.h>
#include <slsi/radio_v1_4.h>
#include "simcardstatus.h"
#include "test_api.h"

TEST(SimCardStatus, SimCardStatus) {
    RIL_CardStatus_V1_4 testVector[] = {
        { .card_state = RIL_CARDSTATE_ABSENT,
          .physicalSlotId = 0,
        },
        { .card_state = RIL_CARDSTATE_PRESENT,
          .physicalSlotId = 1,
          .atr = (char *)"3BDE9600803FC7A0FFFFE073FE215B64534304308105F2",
          .iccid = (char *)"898201236304310894F",
          .eid = 0,
        },
        { .card_state = RIL_CARDSTATE_PRESENT,
          .physicalSlotId = 1,
          .atr = (char *)"3BDE9600803FC7A0FFFFE073FE215B64534304308105F2",
          .iccid = (char *)"8982051506304310894F",
          .eid = (char *)"89033023426200000330025426265873",
        },
        { .card_state = RIL_CARDSTATE_ERROR,
          .physicalSlotId = 0,
        },
    };
    size_t size = NUM_OF_ELEMENTS(testVector);
    for (size_t i = 0; i < size; i++) {
        const RIL_CardStatus_V1_4& cardStatus = testVector[i];
        SimCardStatus test = SimCardStatus::create(cardStatus);
        EXPECT_EQ(test.getCardState(), cardStatus.card_state);
        EXPECT_EQ((unsigned int)/* NEED TO CHECK */ test.getPhysicalSlotId(), cardStatus.physicalSlotId);
        EXPECT_EQ(test.getAtr(), cardStatus.atr ? cardStatus.atr : "");
        EXPECT_EQ(test.getIccid(), cardStatus.iccid ? cardStatus.iccid : "");
        EXPECT_EQ(test.getEid(), cardStatus.eid ? cardStatus.eid : "");
    }
}

TEST(SimCardStatus, SimCardStatus_copy) {
    RIL_CardStatus_V1_4 testVector[] = {
        { .card_state = RIL_CARDSTATE_ABSENT,
          .physicalSlotId = 0,
        },
        { .card_state = RIL_CARDSTATE_PRESENT,
          .physicalSlotId = 1,
          .atr = (char *)"3BDE9600803FC7A0FFFFE073FE215B64534304308105F2",
          .iccid = (char *)"898201236304310894F",
          .eid = 0,
        },
        { .card_state = RIL_CARDSTATE_PRESENT,
          .physicalSlotId = 1,
          .atr = (char *)"3BDE9600803FC7A0FFFFE073FE215B64534304308105F2",
          .iccid = (char *)"8982051506304310894F",
          .eid = (char *)"89033023426200000330025426265873",
        },
        { .card_state = RIL_CARDSTATE_ERROR,
          .physicalSlotId = 0,
        },
    };
    size_t size = NUM_OF_ELEMENTS(testVector);
    for (size_t i = 0; i < size; i++) {
        const RIL_CardStatus_V1_4& cardStatus = testVector[i];
        SimCardStatus test = SimCardStatus::create(cardStatus);
        SimCardStatus copy = test;
        EXPECT_TRUE(copy == test);
        test.setNoSim();
        copy = test;
        EXPECT_TRUE(copy == test);
    }
}

TEST(SimCardStatus, SimCardStatus_move) {
    RIL_CardStatus_V1_4 testVector[] = {
        { .card_state = RIL_CARDSTATE_ABSENT,
          .physicalSlotId = 0,
        },
        { .card_state = RIL_CARDSTATE_PRESENT,
          .physicalSlotId = 1,
          .atr = (char *)"3BDE9600803FC7A0FFFFE073FE215B64534304308105F2",
          .iccid = (char *)"898201236304310894F",
          .eid = 0,
        },
        { .card_state = RIL_CARDSTATE_PRESENT,
          .physicalSlotId = 1,
          .atr = (char *)"3BDE9600803FC7A0FFFFE073FE215B64534304308105F2",
          .iccid = (char *)"8982051506304310894F",
          .eid = (char *)"89033023426200000330025426265873",
        },
        { .card_state = RIL_CARDSTATE_ERROR,
          .physicalSlotId = 0,
        },
    };
    size_t size = NUM_OF_ELEMENTS(testVector);
    for (size_t i = 0; i < size; i++) {
        const RIL_CardStatus_V1_4& cardStatus = testVector[i];
        SimCardStatus test = SimCardStatus::create(cardStatus);
        std::string str = test.toString();
        SimCardStatus move = std::move(test);
        EXPECT_TRUE(move.toString() == str);

        test = SimCardStatus::create(cardStatus);
        move = std::move(test);
        EXPECT_TRUE(move.toString() == str);
    }
}
