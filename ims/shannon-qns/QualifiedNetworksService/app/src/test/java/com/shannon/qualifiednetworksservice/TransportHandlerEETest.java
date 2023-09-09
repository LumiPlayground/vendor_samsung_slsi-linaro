package com.shannon.qualifiednetworksservice;

import android.telephony.DataFailCause;

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

import static org.mockito.Mockito.when;

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class TransportHandlerEETest extends TestCase {
    private TransportHandlerEE mHandler;

    @Mock
    Transport mockTransport;

    @Mock
    TransportState mockTransportState;

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.openMocks(this);

        mHandler = new TransportHandlerEE();
    }

    @After
    public void tearDown() throws Exception {
        Mockito.validateMockitoUsage();
    }

    @Test
    public void testOnConnectionFailure() {
        when(mockTransport.getInitialState()).thenReturn(mockTransportState);
        when(mockTransport.getType()).thenReturn(Transport.TransportType.TRANSPORT_CELLULAR);
        assertFalse(mHandler.onConnectionFailure(mockTransport, DataFailCause.UNKNOWN, 0));

        when(mockTransport.getType()).thenReturn(Transport.TransportType.TRANSPORT_WIFI);
        assertFalse(mHandler.onConnectionFailure(mockTransport, DataFailCause.UNKNOWN, 10));

        when(mockTransportState.getRetryCount()).thenReturn(1);
        assertTrue(mHandler.onConnectionFailure(mockTransport, DataFailCause.UNKNOWN, 0));

        when(mockTransportState.getRetryCount()).thenReturn(6);
        assertTrue(mHandler.onConnectionFailure(mockTransport, DataFailCause.UNKNOWN, 0));
    }

    @Test
    public void testOnHandoverFailure() {
        when(mockTransport.getHandoverState()).thenReturn(mockTransportState);
        when(mockTransport.getType()).thenReturn(Transport.TransportType.TRANSPORT_CELLULAR);
        assertFalse(mHandler.onHandoverFailure(mockTransport, DataFailCause.UNKNOWN, 0));
    }
}