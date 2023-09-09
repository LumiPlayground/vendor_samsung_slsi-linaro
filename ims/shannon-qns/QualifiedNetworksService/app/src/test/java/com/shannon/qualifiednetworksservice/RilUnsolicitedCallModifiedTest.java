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
import static org.junit.Assert.assertFalse;

@FixMethodOrder(MethodSorters.NAME_ASCENDING)
@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class RilUnsolicitedCallModifiedTest {
    RilUnsolicitedMessage mRilUnsolicitedMessage;
    RilUnsolicitedCallModified mRilUnsolicitedCallModified;

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
        mRilUnsolicitedMessage = new RilUnsolicitedMessage(RilUnsolicitedMessage.RIL_UNSOL_CALL_MODIFIED, mRawData);
        mRilUnsolicitedCallModified = new RilUnsolicitedCallModified(mRilUnsolicitedMessage);
    }

    @After
    public void tearDown() {
        Mockito.validateMockitoUsage();
    }

    @Test
    public void getCallIdTest() {
        assertEquals(0, mRilUnsolicitedCallModified.getCallId());
    }

    @Test
    public void isVideoCallTest() {
        assertFalse(mRilUnsolicitedCallModified.isVideoCall());
    }

    @Test
    public void toStringTest() {
        mRilUnsolicitedCallModified.toString();
    }
}
