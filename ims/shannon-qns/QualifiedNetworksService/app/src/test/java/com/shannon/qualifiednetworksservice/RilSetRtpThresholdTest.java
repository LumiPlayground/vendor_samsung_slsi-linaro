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
import org.mockito.Mock;
import org.mockito.Mockito;
import org.mockito.MockitoAnnotations;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.annotation.Config;

import static org.junit.Assert.assertEquals;

@FixMethodOrder(MethodSorters.NAME_ASCENDING)
@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class RilSetRtpThresholdTest {
    RilSetRtpThreshold mRilSetRtpThreshold;

    @Mock
    RilOemInterface mockOemInterface;

    @BeforeClass
    public static void beforeClass() {
    }

    @AfterClass
    public static void afterClass() {
    }

    @Before
    public void setUp() {
        MockitoAnnotations.openMocks(this);
        mRilSetRtpThreshold = new RilSetRtpThreshold(mockOemInterface, 0, -100, 10, 5, 2);
    }

    @After
    public void tearDown() {
        Mockito.validateMockitoUsage();
    }

    @Test
    public void constructorTest() {
        assertEquals(0, mRilSetRtpThreshold.getCallId());
        assertEquals(5, mRilSetRtpThreshold.getJitter());
        assertEquals(-100, mRilSetRtpThreshold.getRtpThreshold());
        assertEquals(10, mRilSetRtpThreshold.getRtpInterval());
        assertEquals(2, mRilSetRtpThreshold.getNoRtpInterval());
    }

    @Test
    public void toStringTest() {
        mRilSetRtpThreshold.toString();
    }
}
