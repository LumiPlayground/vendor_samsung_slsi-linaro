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

import com.shannon.qualifiednetworksservice.RilUnsolicitedRtpTimeout.RtpEvent;
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
public class RilUnsolicitedRtpTimeoutTest {
    RilUnsolicitedMessage mRilUnsolicitedMessage;
    RilUnsolicitedRtpTimeout mRilUnsolicitedRtpTimeout;

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
        mRilUnsolicitedMessage = new RilUnsolicitedMessage(RilUnsolicitedMessage.RIL_UNSOL_RTP_TIMEOUT, mRawData);
        mRilUnsolicitedRtpTimeout = new RilUnsolicitedRtpTimeout(mRilUnsolicitedMessage);
    }

    @After
    public void tearDown() {
        Mockito.validateMockitoUsage();
    }

    @Test
    public void getEnumTest() {
        assertEquals(RtpEvent.EVENT_RTP_TIMEOUT, RtpEvent.getEnum(1));
        assertEquals(RtpEvent.EVENT_UNKNOWN, RtpEvent.getEnum(0));
    }

    @Test
    public void getTimeoutTypeTest() {
        assertEquals(mRilUnsolicitedRtpTimeout.getTimeoutType(), RtpEvent.EVENT_UNKNOWN);
    }

    @Test
    public void toStringTest() {
        mRilUnsolicitedRtpTimeout.toString();
    }
}
