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
public class TransportHandlerRogersTest extends TestCase {
    private TransportHandlerRogers mHandler;

    @Mock
    Transport mockTransport;

    @Mock
    TransportState mockTransportState;

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.openMocks(this);

        mHandler = new TransportHandlerRogers();
    }

    @After
    public void tearDown() throws Exception {
        Mockito.validateMockitoUsage();
    }

    @Test
    public void testOnOperatorChanged() {
        when(mockTransport.getType()).thenReturn(Transport.TransportType.TRANSPORT_WIFI);
        assertFalse(mHandler.onOperatorChanged(mockTransport));

        when(mockTransport.getType()).thenReturn(Transport.TransportType.TRANSPORT_CELLULAR);
        when(mockTransport.getInitialState()).thenReturn(mockTransportState);
        when(mockTransport.getHandoverState()).thenReturn(mockTransportState);
        assertFalse(mHandler.onOperatorChanged(mockTransport));
    }

    @Test
    public void testOnWfcStateChanged() {
        when(mockTransport.getType()).thenReturn(Transport.TransportType.TRANSPORT_CELLULAR);
        assertFalse(mHandler.onWfcStateChanged(mockTransport));

        when(mockTransport.getType()).thenReturn(Transport.TransportType.TRANSPORT_WIFI);
        when(mockTransport.getInitialState()).thenReturn(mockTransportState);
        when(mockTransport.getHandoverState()).thenReturn(mockTransportState);
        assertFalse(mHandler.onWfcStateChanged(mockTransport));
    }

    @Test
    public void testOnConnectionFailure() {
        assertFalse(mHandler.onConnectionFailure(mockTransport, DataFailCause.UNKNOWN, 10));

        when(mockTransport.getType()).thenReturn(Transport.TransportType.TRANSPORT_WIFI);
        assertFalse(mHandler.onConnectionFailure(mockTransport, DataFailCause.UNKNOWN, 0));

        when(mockTransport.getType()).thenReturn(Transport.TransportType.TRANSPORT_CELLULAR);
        assertFalse(mHandler.onConnectionFailure(mockTransport, DataFailCause.UNKNOWN, 0));

        when(mockTransport.getInitialState()).thenReturn(mockTransportState);
        assertTrue(mHandler.onConnectionFailure(mockTransport, DataFailCause.INSUFFICIENT_RESOURCES, 0));
    }

    @Test
    public void testOnHandoverFailure() {
        assertFalse(mHandler.onHandoverFailure(mockTransport, DataFailCause.UNKNOWN, 10));

        when(mockTransport.getType()).thenReturn(Transport.TransportType.TRANSPORT_WIFI);
        assertFalse(mHandler.onHandoverFailure(mockTransport, DataFailCause.UNKNOWN, 0));

        when(mockTransport.getType()).thenReturn(Transport.TransportType.TRANSPORT_CELLULAR);
        assertFalse(mHandler.onHandoverFailure(mockTransport, DataFailCause.UNKNOWN, 0));

        when(mockTransport.getHandoverState()).thenReturn(mockTransportState);
        assertTrue(mHandler.onHandoverFailure(mockTransport, DataFailCause.INSUFFICIENT_RESOURCES, 0));
    }
}