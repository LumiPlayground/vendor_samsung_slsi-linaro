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

import com.shannon.qualifiednetworksservice.RilUnsolicitedRegistration.StatusCode;
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

import static com.shannon.qualifiednetworksservice.RilUnsolicitedRegistration.StatusCode.REG_FAIL_RETRY;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;

@FixMethodOrder(MethodSorters.NAME_ASCENDING)
@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class RilUnsolicitedRegistrationTest {
    RilUnsolicitedMessage mRilUnsolicitedMessage;
    RilUnsolicitedRegistration mRilUnsolicitedRegistration;

    @BeforeClass
    public static void beforeClass() {
    }

    @AfterClass
    public static void afterClass() {
    }

    @Before
    public void setUp() {
        MockitoAnnotations.openMocks(this);
        byte[] mRawData = new byte[]{(byte) 0x00,
            (byte) 0x01,
            (byte) 0x02,
            (byte) 0x00,
            (byte) 0x00,
            (byte) 0x00,
            (byte) 0x00,
            (byte) 0x00,
        };
        mRilUnsolicitedMessage = new RilUnsolicitedMessage(RilUnsolicitedMessage.RIL_UNSOL_REGISTRATION, mRawData);
        mRilUnsolicitedRegistration = new RilUnsolicitedRegistration(mRilUnsolicitedMessage);
    }

    @After
    public void tearDown() {
        Mockito.validateMockitoUsage();
    }

    @Test
    public void getEnumTest() {
        assertEquals(REG_FAIL_RETRY, StatusCode.getEnum(8));
    }

    @Test
    public void getStatusCodeAsIntTest() {
        assertEquals(0, mRilUnsolicitedRegistration.getStatusCodeAsInt());
    }

    @Test
    public void isImsIndicationTest() {
        assertFalse(mRilUnsolicitedRegistration.isImsIndication());
    }

    @Test
    public void isImsRegisteredTest() {
        assertFalse(mRilUnsolicitedRegistration.isImsRegistered());
    }

    @Test
    public void isEmergencyIndicationTest() {
        assertFalse(mRilUnsolicitedRegistration.isEmergencyIndication());
    }

    @Test
    public void isEmergencyRegisteredTest() {
        assertFalse(mRilUnsolicitedRegistration.isEmergencyRegistered());
    }

    @Test
    public void toStringTest() {
        mRilUnsolicitedRegistration.toString();
    }
}
