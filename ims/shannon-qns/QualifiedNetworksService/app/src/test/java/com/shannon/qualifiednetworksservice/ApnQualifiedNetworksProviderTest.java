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

import android.content.Context;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;
import android.os.Message;

import com.android.internal.util.FastPrintWriter;
import com.android.internal.util.IndentingPrintWriter;
import com.shannon.qualifiednetworksservice.ApnQualifiedNetworksProvider.ApnConditionsHandler;
import com.shannon.qualifiednetworksservice.HandoverConditions.ConditionEvents;
import com.shannon.qualifiednetworksservice.HandoverConditions.EventInfo;

import junit.framework.TestCase;
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

import static com.shannon.qualifiednetworksservice.Transport.TransportType.TRANSPORT_CELLULAR;
import static org.mockito.ArgumentMatchers.any;
import static org.mockito.ArgumentMatchers.anyInt;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.spy;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.PrintWriter;

@FixMethodOrder(MethodSorters.NAME_ASCENDING)
@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class ApnQualifiedNetworksProviderTest extends TestCase {
    private ApnQualifiedNetworksProvider mApnQualifiedNetworksProvider;
    private ApnConditionsHandler mApnConditionsHandler;

    @Mock
    private HandlerThread mockHandlerThread;
    @Mock
    private Context mockContext;
    @Mock
    private Handler mockMessageHandler;
    @Mock
    private HandoverConditions mockHandoverConditions;
    @Mock
    private HandoverDecision mockHandoverDecision;

    @BeforeClass
    public static void beforeClass() {
    }

    @AfterClass
    public static void afterClass() {
    }

    @Before
    public void setUp() {
        MockitoAnnotations.openMocks(this);

        when(mockHandoverConditions.getSlotIndex()).thenReturn(0);
        when(mockHandoverConditions.getContext()).thenReturn(mockContext);
        when(mockHandoverConditions.getHandoverDecisionForApn(anyInt())).thenReturn(mockHandoverDecision);

        mApnQualifiedNetworksProvider = new ApnQualifiedNetworksProvider(0, mockHandoverConditions,
            mockMessageHandler,
            1);
    }

    @After
    public void tearDown() {
        Mockito.validateMockitoUsage();
    }

    @Test
    public void destroyTest() {
        // When
        mApnQualifiedNetworksProvider.destroy();

        // Then
        verify(mockHandoverConditions, times(1)).unregisterListener(any());
    }

    @Test
    public void ApnConditionsHandlerTest() {
        // Given
        Looper mockLooper = mock(Looper.class);
        PreferredList mockPreferredList = spy(PreferredList.class);
        when(mockHandlerThread.getLooper()).thenReturn(mockLooper);

        Message mockMessage = mock(Message.class);
        mockMessage.what = HandoverConditions.CONDITIONS_CHANGED;
        mockMessage.obj = ConditionEvents.EVENT_UNKNOWN;

        mockPreferredList.add(1);
        mockPreferredList.add(2);
        mockPreferredList.add(3);

        when(mockHandoverDecision.getPreferredList(any())).thenReturn(mockPreferredList);

        PreferredList mPreferredList = new PreferredList();
        mPreferredList.add(10);
        mPreferredList.add(20);
        mPreferredList.add(30);

        mApnConditionsHandler = mApnQualifiedNetworksProvider.new ApnConditionsHandler(mockLooper);
        mApnQualifiedNetworksProvider.setQualifiedNetworks(mPreferredList);

        // When
        mApnConditionsHandler.handleMessage(mockMessage);

        // Then
        assertFalse(mPreferredList.isEmpty());

        // Given
        mockPreferredList.clear();

        // When
        mApnConditionsHandler.handleMessage(mockMessage);

        // Then
        assertTrue(mPreferredList.isEmpty());
    }

    @Test
    public void ApnConditionsHandlerTest2() {
        // Given
        Looper mockLooper = mock(Looper.class);
        PreferredList mockPreferredList = spy(PreferredList.class);
        when(mockHandlerThread.getLooper()).thenReturn(mockLooper);

        Message mockMessage = mock(Message.class);
        mockMessage.what = HandoverConditions.CONFIGURATION_CHANGED;
        mockMessage.obj = ConditionEvents.EVENT_UNKNOWN;

        mockPreferredList.add(1);
        mockPreferredList.add(2);
        mockPreferredList.add(3);

        when(mockHandoverDecision.getPreferredList(any())).thenReturn(mockPreferredList);

        PreferredList mPreferredList = new PreferredList();
        mPreferredList.add(10);
        mPreferredList.add(20);
        mPreferredList.add(30);

        mApnConditionsHandler = mApnQualifiedNetworksProvider.new ApnConditionsHandler(mockLooper);
        mApnQualifiedNetworksProvider.setQualifiedNetworks(mPreferredList);

        // When
        mApnConditionsHandler.handleMessage(mockMessage);

        // Then
        assertFalse(mPreferredList.isEmpty());

        // Given
        mockPreferredList.clear();

        // When
        mApnConditionsHandler.handleMessage(mockMessage);

        // Then
        assertTrue(mPreferredList.isEmpty());
    }

    @Test
    public void ApnConditionsHandlerTest3() {
        // Given
        Looper mockLooper = mock(Looper.class);
        PreferredList mockPreferredList = spy(PreferredList.class);
        when(mockHandlerThread.getLooper()).thenReturn(mockLooper);

        Message mockMessage = mock(Message.class);
        mockMessage.what = HandoverConditions.CONDITIONS_CHANGED;
        mockMessage.obj = new EventInfo(ConditionEvents.EVENT_APN_CONNECTED, TRANSPORT_CELLULAR);

        mockPreferredList.add(1);
        mockPreferredList.add(2);
        mockPreferredList.add(3);

        when(mockHandoverDecision.getPreferredList(any())).thenReturn(mockPreferredList);

        PreferredList mPreferredList = new PreferredList();
        mPreferredList.add(1);
        mPreferredList.add(2);
        mPreferredList.add(3);

        mApnConditionsHandler = mApnQualifiedNetworksProvider.new ApnConditionsHandler(mockLooper);
        mApnQualifiedNetworksProvider.setQualifiedNetworks(mPreferredList);

        // When
        mApnConditionsHandler.handleMessage(mockMessage);

        // Then
        assertFalse(mPreferredList.isEmpty());

        // Given
        mockPreferredList.clear();

        // When
        mApnConditionsHandler.handleMessage(mockMessage);

        // Then
        assertTrue(mPreferredList.isEmpty());
    }

    @Test
    public void IsHandledApnTypeTest() {
       //Given
       Looper mockLooper = mock(Looper.class);
       mApnConditionsHandler = mApnQualifiedNetworksProvider.new ApnConditionsHandler(mockLooper);

       // When
       boolean retVal = mApnConditionsHandler.isHandledApnType(0);

       //Then
        assertFalse(retVal);
    }

    @Test
    public void dumpTest() throws FileNotFoundException {
        // Given
        FileOutputStream fout = new FileOutputStream(new File("./test.txt"));
        PrintWriter pw = new FastPrintWriter(fout);

        // When
        mApnQualifiedNetworksProvider.dump(null,
                                            new IndentingPrintWriter(new FastPrintWriter(pw), " "),
                                            null);
    }

}