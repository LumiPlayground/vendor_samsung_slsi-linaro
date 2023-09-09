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

import static org.junit.Assert.assertArrayEquals;
import static org.junit.Assert.assertEquals;
import static org.mockito.ArgumentMatchers.any;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;

@FixMethodOrder(MethodSorters.NAME_ASCENDING)
@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class RilSolicitedMessageTest {
    RilSolicitedMessage mRilSolicitedMessage;

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
        mRilSolicitedMessage = new RilSolicitedMessage(10, mockOemInterface);
    }

    @After
    public void tearDown() {
        Mockito.validateMockitoUsage();
    }

    @Test
    public void getMessageIdTest() {
        assertEquals(10, mRilSolicitedMessage.getMessageId());
    }

    @Test
    public void getDataTest() {
        byte[] testRawData = new byte[]{(byte) 0x00, (byte) 0x01, (byte) 0x02};
        mRilSolicitedMessage.setRawData(testRawData);
        assertArrayEquals(testRawData, mRilSolicitedMessage.getData());
    }

    @Test
    public void getDataLengthTest() {
        byte[] testRawData = new byte[]{(byte) 0x00, (byte) 0x01, (byte) 0x02};
        mRilSolicitedMessage.setRawData(testRawData);
        assertEquals(testRawData.length, mRilSolicitedMessage.getDataLength());
    }

    @Test
    public void setDataTest() {
        // Given
        byte[] testRawData = new byte[]{(byte) 0x00, (byte) 0x01, (byte) 0x02};

        // When
        mRilSolicitedMessage.setData(testRawData);

        // Then
        assertArrayEquals(testRawData, mRilSolicitedMessage.getRawData());
    }

    @Test
    public void requestTest() {
        // When
        mRilSolicitedMessage.request();

        // Verify
        verify(mockOemInterface, times(1)).sendRequest(any());
    }

    @Test
    public void onResponseTest() {
        // Given
        byte[] testRawData = new byte[]{(byte) 0x00, (byte) 0x01, (byte) 0x02};

        // When
        mRilSolicitedMessage.onResponse(0, testRawData, 10);
        // do nothing
    }
}
