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
package com.shannon.qualifiednetworksservice.util;

import java.util.ArrayList;
import junit.framework.TestCase;
import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.Mockito;
import org.mockito.MockitoAnnotations;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.annotation.Config;

import static org.junit.Assert.assertArrayEquals;

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class StringUtilsTest extends TestCase {
    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.openMocks(this);
        StringUtils stringUtils = new StringUtils();
    }

    @After
    public void tearDown() throws Exception {
        Mockito.validateMockitoUsage();
    }

    @Test
    public void testbytesToHexString() {
        // Given
        byte[] inArray = {0x09, 0x01, 0x07, 0x04, 0x03, 0x0A, 0x06, 0x09, 0x0B};
        String test = "09 01 07 04 03 0A 06 09 0B ";

        // Then
        assertEquals(test, StringUtils.bytesToHexString(inArray));

        // Abnormal
        assertEquals("", StringUtils.bytesToHexString(null));
    }

    @Test
    public void testhexStringToByteArray() {
        // Given
        String input = "AA";
        byte[] test = {-86};

        // Then
        assertArrayEquals(test, StringUtils.hexStringToByteArray(input));
    }

    @Test
    public void testprimitiveArrayToArrayList() {
        // Given
        byte[] inArray = {0x01, 0x02};
        ArrayList<Byte> test = new ArrayList<>();
        test.add((byte) 0x01);
        test.add((byte) 0x02);

        // When
        StringUtils.primitiveArrayToArrayList(inArray);

        // Then
        assertTrue(StringUtils.primitiveArrayToArrayList(inArray).contains((byte) 0x01));
    }

    @Test
    public void testarrayListToPrimitiveArray() {
        // Given
        ArrayList<Byte> inArray = new ArrayList<>();
        inArray.add((byte) 0x01);
        inArray.add((byte) 0x02);

        // When
        StringUtils.arrayListToPrimitiveArray(inArray);

        // Then
        assertNotNull(StringUtils.arrayListToPrimitiveArray(inArray));
    }
}