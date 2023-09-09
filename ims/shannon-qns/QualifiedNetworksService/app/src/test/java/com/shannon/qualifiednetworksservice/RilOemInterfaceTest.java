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

import android.os.Handler;
import android.os.Message;
import android.util.SparseArray;
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

import vendor.samsung_slsi.telephony.hardware.radioExternal.V1_0.IOemSlsiRadioExternal;

import static com.shannon.qualifiednetworksservice.RilOemInterface.RIL_UNSOLICITED_MESSAGE;
import static org.mockito.ArgumentMatchers.any;
import static org.mockito.ArgumentMatchers.anyInt;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.mockStatic;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

@FixMethodOrder(MethodSorters.NAME_ASCENDING)
@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class RilOemInterfaceTest {
    private RilOemInterface mRilInterface;

    @Mock
    IOemSlsiRadioExternal mockOemRil;

    @BeforeClass
    public static void beforeClass() {
    }

    @AfterClass
    public static void afterClass() {
    }

    @Before
    public void setUp() {
        MockitoAnnotations.openMocks(this);

        mRilInterface = new RilOemInterface(0, true);
    }

    @After
    public void tearDown() {
        Mockito.validateMockitoUsage();
    }

    @Test
    public void destroyTest() {
        // When
        mRilInterface.destroy();
    }

    @Test
    public void onResponseTest() {
        // Given
        RilSetCellularThresholds mockedMsg = mock(RilSetCellularThresholds.class);
        int tid = mRilInterface.putSolicitedMessageToList(mockedMsg);

        byte[] data = {0x1, 0x2};
        // When
        mRilInterface.onResponse(tid, 0, data, data.length);
        // Then
        verify(mockedMsg, times(1)).onResponse(0, data, data.length);
    }

    @Test
    public void sendRequestTest() {
        MockedStatic<IOemSlsiRadioExternal> mockedStatic = mockStatic(IOemSlsiRadioExternal.class);
        try {
            // When
            when(IOemSlsiRadioExternal.getService("rilExternal")).thenReturn(mockOemRil);
            when(mockOemRil.setResponseFunctions(null, null)).thenReturn(1);

            RilSetCellularThresholds msg = new RilSetCellularThresholds(mRilInterface);
            mRilInterface.sendRequest(msg);

            // Then
            verify(mockOemRil, times(1)).sendRequestRaw(anyInt(), 1, anyInt(), 0, anyInt(), any());
        } catch (Exception e) {
            // nothing
        }
        mockedStatic.close();
    }

    @Test
    public void registerUnsolicitedListenerTest() {
        // Given
        Handler mockHandler = mock(Handler.class);
        // When
        mRilInterface.registerUnsolicitedListener(0, mockHandler);
        // Then
        // since mUnsolicitedListeners is private final SparseArray<Handler>, it can't be verified.
    }

    @Test
    public void unregisterUnsolicitedListenerTest() {
        // Given
        // When
        mRilInterface.unregisterUnsolicitedListener(0);
        // Then
        // since mUnsolicitedListeners is private final SparseArray<Handler>, it can't be verified.
    }

    @Test
    public void onIndicationTest() {
        // Given
        byte[] data = {0x1, 0x2};

        // When
        mRilInterface.onIndication(3, 0, data, 2);

        Handler mockHandler = mock(Handler.class);
        mRilInterface.registerUnsolicitedListener(1, mockHandler);

        MockedStatic<Message> mockedStatic = mockStatic(Message.class);
        Message mockedMessage = mock(Message.class);
        when(Message.obtain(mockHandler, RIL_UNSOLICITED_MESSAGE)).thenReturn(mockedMessage);

        // When
        mRilInterface.onIndication(0, 1, data, data.length);
        // Then
        verify(mockedMessage, times(1)).sendToTarget();

        mockedStatic.close();
    }

    @Test
    public void serviceDiedTest() {
        RilOemInterface.RadioProxyDeathRecipient deathRecipient = mRilInterface.getDeathLink();
        deathRecipient.serviceDied(0);
        deathRecipient.serviceDied(1);
    }
}
