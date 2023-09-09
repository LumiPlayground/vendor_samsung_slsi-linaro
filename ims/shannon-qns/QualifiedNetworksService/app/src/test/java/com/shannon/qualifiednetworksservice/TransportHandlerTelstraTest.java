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

import android.telephony.DataFailCause;
import android.telephony.TelephonyManager;
import android.telephony.data.ApnSetting;
import com.shannon.qualifiednetworksservice.Transport.TransportType;
import junit.framework.TestCase;
import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.Mock;
import org.mockito.Mockito;
import org.mockito.MockitoAnnotations;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.annotation.Config;

import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class TransportHandlerTelstraTest extends TestCase {
    private TransportHandlerTelstra mTransportHandlerTelstra;

    @Mock
    private TelephonyManager mockTelephonyManager;

    @Mock
    private Transport mockTransport;

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.openMocks(this);
        mTransportHandlerTelstra = new TransportHandlerTelstra(ApnSetting.TYPE_IMS);
    }

    @After
    public void tearDown() {
        Mockito.validateMockitoUsage();
    }

    @Test
    public void onConnectionFailureFalseTest() {
        // Given

        // When
        boolean retVal = mTransportHandlerTelstra.onConnectionFailure(mockTransport, DataFailCause.NETWORK_FAILURE, 2);

        // Then
        assertEquals(false, retVal);
    }

    @Test
    public void onConnectionFailureTrueTest() {
        // Given
        TransportState transportState = mock(TransportState.class);
        when(mockTransport.getInitialState()).thenReturn(transportState);
        when(transportState.getRetryCount()).thenReturn(7);

        // When
        boolean retVal = mTransportHandlerTelstra.onConnectionFailure(mockTransport, DataFailCause.NETWORK_FAILURE, 0);

        // Then
        assertEquals(true, retVal);
    }

    @Test
    public void onHandoverFailureFalseTest() {
        // Given

        // When
        boolean retVal = mTransportHandlerTelstra.onHandoverFailure(mockTransport, DataFailCause.NETWORK_FAILURE, 2);

        // Then
        assertEquals(false, retVal);
    }

    @Test
    public void onHandoverFailureTrueTest() {
        // Given
        TransportState transportState = mock(TransportState.class);
        when(mockTransport.getHandoverState()).thenReturn(transportState);
        when(transportState.getRetryCount()).thenReturn(7);

        // When
        boolean retVal = mTransportHandlerTelstra.onHandoverFailure(mockTransport, DataFailCause.NETWORK_FAILURE, 0);

        // Then
        assertEquals(true, retVal);
    }

    @Test
    public void processErrorXcapTest() {
        // Given
        TransportHandlerTelstra mTransportHandlerXcap;
        mTransportHandlerXcap = new TransportHandlerTelstra(ApnSetting.TYPE_XCAP);
        TransportState transportState = mock(TransportState.class);
        when(mockTransport.getHandoverState()).thenReturn(transportState);
        when(mockTransport.getType()).thenReturn(TransportType.TRANSPORT_CELLULAR);
        when(transportState.getRetryCount()).thenReturn(7);

        // When
        boolean retVal = mTransportHandlerXcap.onHandoverFailure(mockTransport, DataFailCause.SERVICE_OPTION_NOT_SUBSCRIBED, 0);

        // Then
        assertEquals(true, retVal);
    }
}
