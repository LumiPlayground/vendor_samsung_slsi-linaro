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

import static org.junit.Assert.assertArrayEquals;
import static org.junit.Assert.assertEquals;

@FixMethodOrder(MethodSorters.NAME_ASCENDING)
@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class RilUnsolicitedMessageTest {
    RilUnsolicitedMessage mRilUnsolicitedMessage;
    RilUnsolicitedMessage mRilUnsolicitedMessage_ctr;

    @BeforeClass
    public static void beforeClass() {
    }

    @AfterClass
    public static void afterClass() {
    }

    @Before
    public void setUp() {
        MockitoAnnotations.openMocks(this);
        byte[] mRawData = new byte[]{(byte) 0x00, (byte) 0x01, (byte) 0x02};
        mRilUnsolicitedMessage = new RilUnsolicitedMessage(RilUnsolicitedMessage.RIL_UNSOL_SRVCC, mRawData);
        mRilUnsolicitedMessage_ctr = new RilUnsolicitedMessage(mRilUnsolicitedMessage);
    }

    @After
    public void tearDown() {
        Mockito.validateMockitoUsage();
    }

    @Test
    public void getMessageIdTest() {
        assertEquals(mRilUnsolicitedMessage.getMessageId(), RilUnsolicitedMessage.RIL_UNSOL_SRVCC);
    }

    @Test
    public void getDataTest() {
        byte[] testRawData = new byte[]{(byte) 0x00, (byte) 0x01, (byte) 0x02};
        assertArrayEquals(testRawData, mRilUnsolicitedMessage.getData());
    }

    @Test
    public void getShortTest() {
        int testRawData = 513;
        byte b1 = 0x01;
        byte b2 = 0x02;
        assertEquals(testRawData, mRilUnsolicitedMessage.getShort(b1, b2));
    }

    @Test
    public void getIntTest() {
        int testRawData = 16842752;
        byte b1 = 0x00;
        byte b2 = 0x00;
        byte b3 = 0x01;
        byte b4 = 0x01;
        assertEquals(testRawData, mRilUnsolicitedMessage.getInt(b1, b2, b3, b4));
    }
}
