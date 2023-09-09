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
public class TransportHandlerVzwTest extends TestCase {
    private TransportHandlerVzw mHandler;

    @Mock
    private Transport mockTransport;

    @Mock
    private TransportState mockTransportState;

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.openMocks(this);

        mHandler = new TransportHandlerVzw(false);
    }

    @After
    public void tearDown() throws Exception {
        Mockito.validateMockitoUsage();
    }

    @Test
    public void testDoInitialization() {
        when(mockTransport.getType()).thenReturn(Transport.TransportType.TRANSPORT_CELLULAR);
        mHandler.doInitialization(mockTransport);

        when(mockTransport.getType()).thenReturn(Transport.TransportType.TRANSPORT_WIFI);
        when(mockTransport.getInitialState()).thenReturn(mockTransportState);
        when(mockTransport.getHandoverState()).thenReturn(mockTransportState);
        mHandler.doInitialization(mockTransport);

        assertTrue(true);
    }

    @Test
    public void testOnConnectionFailure() {
        when(mockTransport.getType()).thenReturn(Transport.TransportType.TRANSPORT_CELLULAR);
        when(mockTransport.getInitialState()).thenReturn(mockTransportState);
        when(mockTransportState.getRetryCount()).thenReturn(1);
        assertFalse(mHandler.onConnectionFailure(mockTransport, DataFailCause.UNKNOWN, 10));
        assertTrue(mHandler.onConnectionFailure(mockTransport, DataFailCause.UNKNOWN, 0));
        when(mockTransportState.getRetryCount()).thenReturn(3);
        assertTrue(mHandler.onConnectionFailure(mockTransport, DataFailCause.UNKNOWN, 0));
        when(mockTransportState.getRetryCount()).thenReturn(4);
        assertTrue(mHandler.onConnectionFailure(mockTransport, DataFailCause.UNKNOWN, 0));
        when(mockTransportState.getRetryCount()).thenReturn(5);
        assertTrue(mHandler.onConnectionFailure(mockTransport, DataFailCause.UNKNOWN, 0));
        when(mockTransportState.getRetryCount()).thenReturn(6);
        assertTrue(mHandler.onConnectionFailure(mockTransport, DataFailCause.UNKNOWN, 0));

        when(mockTransport.getType()).thenReturn(Transport.TransportType.TRANSPORT_WIFI);
        assertTrue(mHandler.onConnectionFailure(mockTransport, DataFailCause.UNKNOWN, 0));
        assertTrue(mHandler.onConnectionFailure(mockTransport, DataFailCause.NETWORK_FAILURE, 0));
        when(mockTransportState.getRetryCount()).thenReturn(1);
        assertTrue(mHandler.onConnectionFailure(mockTransport, DataFailCause.SERVICE_OPTION_NOT_SUPPORTED, 0));
        when(mockTransportState.getRetryCount()).thenReturn(3);
        assertTrue(mHandler.onConnectionFailure(mockTransport, DataFailCause.USER_AUTHENTICATION, 0));
    }

    @Test
    public void testOnHandoverFailure() {
        when(mockTransport.getType()).thenReturn(Transport.TransportType.TRANSPORT_CELLULAR);
        when(mockTransport.getHandoverState()).thenReturn(mockTransportState);
        when(mockTransportState.getRetryCount()).thenReturn(1);
        assertFalse(mHandler.onHandoverFailure(mockTransport, DataFailCause.UNKNOWN, 10));
        assertTrue(mHandler.onHandoverFailure(mockTransport, DataFailCause.UNKNOWN, 0));
    }

    @Test
    public void testOnWiFiAccessPointChanged() {
        when(mockTransport.getType()).thenReturn(Transport.TransportType.TRANSPORT_CELLULAR);
        mHandler.onWiFiAccessPointChanged(mockTransport);

        when(mockTransport.getType()).thenReturn(Transport.TransportType.TRANSPORT_WIFI);
        when(mockTransport.getInitialState()).thenReturn(mockTransportState);
        when(mockTransport.getHandoverState()).thenReturn(mockTransportState);
        mHandler.onWiFiAccessPointChanged(mockTransport);
        assertTrue(true);
    }

    @Test
    public void testOnRegistrationCompleted() {
        when(mockTransport.getType()).thenReturn(Transport.TransportType.TRANSPORT_CELLULAR);
        mHandler.onRegistrationCompleted(mockTransport);

        when(mockTransport.getType()).thenReturn(Transport.TransportType.TRANSPORT_WIFI);
        when(mockTransport.getInitialState()).thenReturn(mockTransportState);
        when(mockTransport.getHandoverState()).thenReturn(mockTransportState);
        when(mockTransportState.isTemporaryBlocked()).thenReturn(true);
        mHandler.doInitialization(mockTransport);
        mHandler.onRegistrationCompleted(mockTransport);
        assertTrue(true);
    }
}