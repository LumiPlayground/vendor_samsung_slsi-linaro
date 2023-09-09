/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or
 * distributed, transmitted, transcribed, stored in a retrieval system or
 * translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed to third parties
 * without the express written permission of Samsung Electronics.
 */

package com.shannon.qualifiednetworksservice;

import com.shannon.qualifiednetworksservice.RilUnsolicitedLastCallFailCause.LastCallFailCause;
import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.FixMethodOrder;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.MethodSorters;
import org.mockito.Mockito;
import org.mockito.MockitoAnnotations;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.annotation.Config;

import static org.junit.Assert.assertEquals;

@FixMethodOrder(MethodSorters.NAME_ASCENDING)
@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class RilUnsolicitedLastCallFailCauseTest {
    RilUnsolicitedMessage mRilUnsolicitedMessage;
    RilUnsolicitedLastCallFailCause mRilUnsolicitedLastCallFailCause;

    @BeforeClass
    public static void beforeClass() {
    }

    @AfterClass
    public static void afterClass() {
    }

    @Before
    public void setUp() {
        MockitoAnnotations.openMocks(this);
        byte[] mRawData = new byte[]{(byte) 0x00, (byte) 0x01, (byte) 0x02, (byte) 0x00};
        mRilUnsolicitedMessage = new RilUnsolicitedMessage(RilUnsolicitedMessage.RIL_UNSOL_LAST_CALL_FAIL_CAUSE,
            mRawData);
        mRilUnsolicitedLastCallFailCause = new RilUnsolicitedLastCallFailCause(mRilUnsolicitedMessage);
    }

    @After
    public void tearDown() {
        Mockito.validateMockitoUsage();
    }

    @Test
    public void getEnumTest() {
        assertEquals(LastCallFailCause.REDIAL_EMERGENCY_TO_CELL, LastCallFailCause.getEnum(0x0BB9));
    }

    @Test
    public void getLastCallFailCauseTest() {
        assertEquals(LastCallFailCause.NONE,
            mRilUnsolicitedLastCallFailCause.getLastCallFailCause());
    }

    @Test
    public void toStringTest() {
        mRilUnsolicitedLastCallFailCause.toString();
    }
}
