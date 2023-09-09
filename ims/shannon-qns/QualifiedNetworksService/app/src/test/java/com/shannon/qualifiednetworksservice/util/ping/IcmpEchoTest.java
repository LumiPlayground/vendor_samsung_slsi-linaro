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

package com.shannon.qualifiednetworksservice.util.ping;

import java.io.FileDescriptor;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.UnknownHostException;
import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.FixMethodOrder;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.MethodSorters;
import org.mockito.Mock;
import org.mockito.MockedStatic;
import org.mockito.Mockito;
import org.mockito.MockitoAnnotations;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.annotation.Config;

import static org.junit.Assert.assertEquals;
import static org.mockito.ArgumentMatchers.any;
import static org.mockito.ArgumentMatchers.anyBoolean;
import static org.mockito.ArgumentMatchers.anyInt;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.mockStatic;
import static org.mockito.Mockito.when;

import android.system.Os;

@FixMethodOrder(MethodSorters.NAME_ASCENDING)
@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class IcmpEchoTest {
    private static MockedStatic<Os> sOs;
    private static MockedStatic<IcmpPacket> sIcmpPacket;

    @Mock
    FileDescriptor mockFileDescriptor = mock(FileDescriptor.class);

    @Mock
    IcmpPacket mockIcmpPacket = mock(IcmpPacket.class);

    @Mock
    InetSocketAddress mockInetSocketAddress = mock(InetSocketAddress.class);

    @BeforeClass
    public static void beforeClass() {
        sIcmpPacket = mockStatic(IcmpPacket.class);
        sOs = mockStatic(Os.class);
    }

    @AfterClass
    public static void afterClass() {
        sIcmpPacket.close();
        sOs.close();
    }

    @Before
    public void setUp() {
        MockitoAnnotations.openMocks(this);

        IcmpEcho icmpEcho = new IcmpEcho();
        byte[] testPacket = new byte[]{(byte) 30, (byte) 0x01, (byte) 0x02, (byte) 0x03};


        sOs.when(() -> Os.getsockname(any())).thenReturn(mockInetSocketAddress);
        sOs.when(()-> Os.socket(anyInt(), anyInt(), anyInt())).thenReturn(mockFileDescriptor);
        sIcmpPacket.when(()->IcmpPacket.IcmpEchoRequest(anyBoolean(), anyInt(), anyInt())).thenReturn(mockIcmpPacket);
        sIcmpPacket.when(()->IcmpPacket.isResponse(anyBoolean(), (byte) anyInt())).thenReturn(true);

        when(mockIcmpPacket.getBytes()).thenReturn(testPacket);
        when(mockInetSocketAddress.getPort()).thenReturn(80);
        when(mockFileDescriptor.valid()).thenReturn(true);
    }

    @After
    public void tearDown() {
        Mockito.validateMockitoUsage();
    }

    @Test
    public void testisHostReachable() throws UnknownHostException {
        // Given
        InetAddress src = InetAddress.getByName("30.40.246.108");
        InetAddress dst = InetAddress.getByName("100.60.246.125");

        // Then
        assertEquals(2, IcmpEcho.isHostReachable(src, dst, 2, 2));
    }

    // Since fd from isHostReachable() is always null, can't be tested
}
