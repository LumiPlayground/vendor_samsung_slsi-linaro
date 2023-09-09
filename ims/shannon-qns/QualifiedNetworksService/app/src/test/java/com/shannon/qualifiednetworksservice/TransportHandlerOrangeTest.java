package com.shannon.qualifiednetworksservice;

import android.telephony.DataFailCause;
import android.telephony.TelephonyManager;
import android.telephony.data.ApnSetting;

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
public class TransportHandlerOrangeTest extends TestCase {
    private TransportHandlerOrange mHandler;
    private TransportHandlerOrange mHandler32;

    @Mock
    private TelephonyManager mockTelephonyManager;

    @Mock
    private Transport mockTransport;

    @Mock
    private TransportState mockTransportState;

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.openMocks(this);

        when(mockTelephonyManager.getSimSpecificCarrierId()).thenReturn(TelephonyManager.UNKNOWN_CARRIER_ID);
        when(mockTelephonyManager.getSimCarrierId()).thenReturn(TelephonyManager.UNKNOWN_CARRIER_ID);
        when(mockTelephonyManager.getCarrierIdFromSimMccMnc()).thenReturn(678);
        mHandler = new TransportHandlerOrange(ApnSetting.TYPE_IMS, mockTelephonyManager);

        when(mockTelephonyManager.getSimSpecificCarrierId()).thenReturn(32);
        mHandler32 = new TransportHandlerOrange(ApnSetting.TYPE_CBS, mockTelephonyManager);
    }

    @After
    public void tearDown() {
        Mockito.validateMockitoUsage();
    }

    @Test
    public void testOnConnected() {
        // unknown
        when(mockTransport.getType()).thenReturn(Transport.TransportType.TRANSPORT_UNKNOWN);
        mHandler.setBlockedUntilWiFiOn();
        mHandler.onConnected(mockTransport);
        assertTrue(mHandler.getBlockedUntilWiFiOn());

        // cellular
        when(mockTransport.getType()).thenReturn(Transport.TransportType.TRANSPORT_CELLULAR);
        mHandler.onConnected(mockTransport);
        assertEquals(DataFailCause.NONE, mHandler.getLastFailCause());

        // wifi
        when(mockTransport.getType()).thenReturn(Transport.TransportType.TRANSPORT_WIFI);
        mHandler.onConnected(mockTransport);
        assertFalse(mHandler.getBlockedUntilWiFiOn());
    }

    @Test
    public void testOnHandoverCompleted() {
        // unknown
        when(mockTransport.getType()).thenReturn(Transport.TransportType.TRANSPORT_UNKNOWN);
        mHandler.setBlockedUntilWiFiOnHandover();
        mHandler.onHandoverCompleted(mockTransport);
        assertTrue(mHandler.getBlockedUntilWiFiOnHandover());

        // cellular
        when(mockTransport.getType()).thenReturn(Transport.TransportType.TRANSPORT_CELLULAR);
        mHandler.onHandoverCompleted(mockTransport);
        assertEquals(DataFailCause.NONE, mHandler.getLastFailCauseHandover());

        // wifi
        when(mockTransport.getType()).thenReturn(Transport.TransportType.TRANSPORT_WIFI);
        mHandler.onHandoverCompleted(mockTransport);
        assertFalse(mHandler.getBlockedUntilWiFiOnHandover());
    }

    @Test
    public void testOnConnectionFailure() {
        // timeout defined
        assertFalse(mHandler.onConnectionFailure(null, DataFailCause.NONE, 10));

        // wrong transport
        when(mockTransport.getType()).thenReturn(Transport.TransportType.TRANSPORT_UNKNOWN);
        assertFalse(mHandler.onConnectionFailure(mockTransport, DataFailCause.NONE, 0));

        // cellular - not IMS
        when(mockTransport.getType()).thenReturn(Transport.TransportType.TRANSPORT_CELLULAR);
        assertFalse(mHandler32.onConnectionFailure(mockTransport, DataFailCause.NONE, 0));

        // cellular - NETWORK_FAILURE
        when(mockTransport.getType()).thenReturn(Transport.TransportType.TRANSPORT_CELLULAR);
        when(mockTransport.getInitialState()).thenReturn(mockTransportState);
        when(mockTransportState.getRetryCount()).thenReturn(2);
        assertTrue(mHandler.onConnectionFailure(mockTransport, DataFailCause.NETWORK_FAILURE, 0));
        // cellular - MISSING_UNKNOWN_APN
        when(mockTransport.getType()).thenReturn(Transport.TransportType.TRANSPORT_CELLULAR);
        when(mockTransport.getInitialState()).thenReturn(mockTransportState);
        assertTrue(mHandler.onConnectionFailure(mockTransport, DataFailCause.MISSING_UNKNOWN_APN, 0));
        // cellular - UNKNOWN, 1
        when(mockTransport.getType()).thenReturn(Transport.TransportType.TRANSPORT_CELLULAR);
        when(mockTransport.getInitialState()).thenReturn(mockTransportState);
        when(mockTransportState.getRetryCount()).thenReturn(1);
        assertTrue(mHandler.onConnectionFailure(mockTransport, DataFailCause.UNKNOWN, 0));
        // cellular - UNKNOWN, max count
        when(mockTransport.getType()).thenReturn(Transport.TransportType.TRANSPORT_CELLULAR);
        when(mockTransport.getInitialState()).thenReturn(mockTransportState);
        when(mockTransportState.getRetryCount()).thenReturn(3);
        assertTrue(mHandler.onConnectionFailure(mockTransport, DataFailCause.UNKNOWN, 0));

        // wifi - ERROR_UNSPECIFIED
        when(mockTransport.getType()).thenReturn(Transport.TransportType.TRANSPORT_WIFI);
        when(mockTransport.getInitialState()).thenReturn(mockTransportState);
        when(mockTransportState.getRetryCount()).thenReturn(9);
        assertTrue(mHandler.onConnectionFailure(mockTransport, DataFailCause.ERROR_UNSPECIFIED, 0));
        // wifi - NO_RESPONSE_FROM_BASE_STATION
        when(mockTransport.getType()).thenReturn(Transport.TransportType.TRANSPORT_WIFI);
        when(mockTransport.getInitialState()).thenReturn(mockTransportState);
        when(mockTransportState.getRetryCount()).thenReturn(11);
        assertTrue(mHandler.onConnectionFailure(mockTransport, DataFailCause.NO_RESPONSE_FROM_BASE_STATION, 0));
        // wifi - MISSING_UNKNOWN_APN
        when(mockTransport.getType()).thenReturn(Transport.TransportType.TRANSPORT_WIFI);
        when(mockTransport.getInitialState()).thenReturn(mockTransportState);
        assertTrue(mHandler.onConnectionFailure(mockTransport, DataFailCause.MISSING_UNKNOWN_APN, 0));
        // wifi - ILLEGAL_ME
        when(mockTransport.getType()).thenReturn(Transport.TransportType.TRANSPORT_WIFI);
        when(mockTransport.getInitialState()).thenReturn(mockTransportState);
        assertTrue(mHandler.onConnectionFailure(mockTransport, DataFailCause.ILLEGAL_ME, 0));
        // wifi - UNKNOWN
        when(mockTransport.getType()).thenReturn(Transport.TransportType.TRANSPORT_WIFI);
        when(mockTransport.getInitialState()).thenReturn(mockTransportState);
        when(mockTransportState.getRetryCount()).thenReturn(8);
        assertTrue(mHandler.onConnectionFailure(mockTransport, DataFailCause.UNKNOWN, 0));
        // wifi - UNKNOWN, not Orange Spain
        when(mockTransport.getType()).thenReturn(Transport.TransportType.TRANSPORT_WIFI);
        when(mockTransport.getInitialState()).thenReturn(mockTransportState);
        when(mockTransportState.getRetryCount()).thenReturn(4);
        assertTrue(mHandler32.onConnectionFailure(mockTransport, DataFailCause.UNKNOWN, 0));
    }

    @Test
    public void testOnHandoverFailure() {
        // timeout defined
        assertFalse(mHandler.onHandoverFailure(null, DataFailCause.NONE, 10));

        // wrong transport
        when(mockTransport.getType()).thenReturn(Transport.TransportType.TRANSPORT_UNKNOWN);
        assertFalse(mHandler.onHandoverFailure(mockTransport, DataFailCause.NONE, 0));

        // cellular - not IMS
        when(mockTransport.getType()).thenReturn(Transport.TransportType.TRANSPORT_CELLULAR);
        assertFalse(mHandler32.onHandoverFailure(mockTransport, DataFailCause.NONE, 0));

        // cellular - NETWORK_FAILURE
        when(mockTransport.getType()).thenReturn(Transport.TransportType.TRANSPORT_CELLULAR);
        when(mockTransport.getHandoverState()).thenReturn(mockTransportState);
        when(mockTransportState.getRetryCount()).thenReturn(1);
        assertTrue(mHandler.onHandoverFailure(mockTransport, DataFailCause.NETWORK_FAILURE, 0));
        // cellular - MISSING_UNKNOWN_APN
        when(mockTransport.getType()).thenReturn(Transport.TransportType.TRANSPORT_CELLULAR);
        when(mockTransport.getHandoverState()).thenReturn(mockTransportState);
        assertTrue(mHandler.onHandoverFailure(mockTransport, DataFailCause.MISSING_UNKNOWN_APN, 0));
        // cellular - UNKNOWN, 1
        when(mockTransport.getType()).thenReturn(Transport.TransportType.TRANSPORT_CELLULAR);
        when(mockTransport.getHandoverState()).thenReturn(mockTransportState);
        when(mockTransportState.getRetryCount()).thenReturn(1);
        assertTrue(mHandler.onHandoverFailure(mockTransport, DataFailCause.UNKNOWN, 0));
        // cellular - UNKNOWN, max count
        when(mockTransport.getType()).thenReturn(Transport.TransportType.TRANSPORT_CELLULAR);
        when(mockTransport.getHandoverState()).thenReturn(mockTransportState);
        when(mockTransportState.getRetryCount()).thenReturn(3);
        assertTrue(mHandler.onHandoverFailure(mockTransport, DataFailCause.UNKNOWN, 0));

        // wifi - ERROR_UNSPECIFIED
        when(mockTransport.getType()).thenReturn(Transport.TransportType.TRANSPORT_WIFI);
        when(mockTransport.getHandoverState()).thenReturn(mockTransportState);
        when(mockTransportState.getRetryCount()).thenReturn(9);
        assertTrue(mHandler.onHandoverFailure(mockTransport, DataFailCause.ERROR_UNSPECIFIED, 0));
        // wifi - NO_RESPONSE_FROM_BASE_STATION
        when(mockTransport.getType()).thenReturn(Transport.TransportType.TRANSPORT_WIFI);
        when(mockTransport.getHandoverState()).thenReturn(mockTransportState);
        when(mockTransportState.getRetryCount()).thenReturn(11);
        assertTrue(mHandler.onHandoverFailure(mockTransport, DataFailCause.NO_RESPONSE_FROM_BASE_STATION, 0));
        // wifi - MISSING_UNKNOWN_APN
        when(mockTransport.getType()).thenReturn(Transport.TransportType.TRANSPORT_WIFI);
        when(mockTransport.getHandoverState()).thenReturn(mockTransportState);
        assertTrue(mHandler.onHandoverFailure(mockTransport, DataFailCause.MISSING_UNKNOWN_APN, 0));
        // wifi - ILLEGAL_ME
        when(mockTransport.getType()).thenReturn(Transport.TransportType.TRANSPORT_WIFI);
        when(mockTransport.getHandoverState()).thenReturn(mockTransportState);
        assertTrue(mHandler.onHandoverFailure(mockTransport, DataFailCause.ILLEGAL_ME, 0));
        // wifi - UNKNOWN
        when(mockTransport.getType()).thenReturn(Transport.TransportType.TRANSPORT_WIFI);
        when(mockTransport.getHandoverState()).thenReturn(mockTransportState);
        when(mockTransportState.getRetryCount()).thenReturn(8);
        assertTrue(mHandler.onHandoverFailure(mockTransport, DataFailCause.UNKNOWN, 0));
        // wifi - UNKNOWN, not Orange Spain
        when(mockTransport.getType()).thenReturn(Transport.TransportType.TRANSPORT_WIFI);
        when(mockTransport.getHandoverState()).thenReturn(mockTransportState);
        when(mockTransportState.getRetryCount()).thenReturn(4);
        assertTrue(mHandler32.onHandoverFailure(mockTransport, DataFailCause.UNKNOWN, 0));
    }

    @Test
    public void testOnAirplaneModeChanged() {
        // cellular, not IMS
        when(mockTransport.getType()).thenReturn(Transport.TransportType.TRANSPORT_CELLULAR);
        assertFalse(mHandler32.onAirplaneModeChanged(mockTransport));

        // cellular, ignore airplane mode
        when(mockTransport.getType()).thenReturn(Transport.TransportType.TRANSPORT_CELLULAR);
        mHandler.setLastFailCause(DataFailCause.MISSING_UNKNOWN_APN);
        mHandler.setLastFailCauseHandover(DataFailCause.MISSING_UNKNOWN_APN);
        assertTrue(mHandler.onAirplaneModeChanged(mockTransport));
        // cellular, reset transport states
        when(mockTransport.getType()).thenReturn(Transport.TransportType.TRANSPORT_CELLULAR);
        when(mockTransport.getInitialState()).thenReturn(mockTransportState);
        when(mockTransport.getHandoverState()).thenReturn(mockTransportState);
        mHandler.setLastFailCause(DataFailCause.UNKNOWN);
        mHandler.setLastFailCauseHandover(DataFailCause.UNKNOWN);
        assertTrue(mHandler.onAirplaneModeChanged(mockTransport));
        assertEquals(DataFailCause.NONE, mHandler.getLastFailCause());
        assertEquals(DataFailCause.NONE, mHandler.getLastFailCauseHandover());

        // wifi
        when(mockTransport.getType()).thenReturn(Transport.TransportType.TRANSPORT_WIFI);
        mHandler.setBlockedUntilWiFiOn();
        mHandler.setBlockedUntilWiFiOnHandover();
        assertFalse(mHandler.onAirplaneModeChanged(mockTransport));
        assertFalse(mHandler.getBlockedUntilWiFiOn());
        assertFalse(mHandler.getBlockedUntilWiFiOnHandover());
    }

    @Test
    public void testOnWiFiStateChanged() {
        // cellular
        when(mockTransport.getType()).thenReturn(Transport.TransportType.TRANSPORT_CELLULAR);
        assertFalse(mHandler.onWiFiStateChanged(mockTransport));

        // wifi
        when(mockTransport.getType()).thenReturn(Transport.TransportType.TRANSPORT_WIFI);
        when(mockTransport.getInitialState()).thenReturn(mockTransportState);
        when(mockTransport.getHandoverState()).thenReturn(mockTransportState);
        mHandler.setBlockedUntilWiFiOn();
        mHandler.setBlockedUntilWiFiOnHandover();
        assertTrue(mHandler.onWiFiStateChanged(mockTransport));
        assertFalse(mHandler.getBlockedUntilWiFiOn());
        assertFalse(mHandler.getBlockedUntilWiFiOnHandover());

        // wifi
        when(mockTransport.getType()).thenReturn(Transport.TransportType.TRANSPORT_WIFI);
        when(mockTransport.getInitialState()).thenReturn(mockTransportState);
        when(mockTransport.getHandoverState()).thenReturn(mockTransportState);
        assertTrue(mHandler.onWiFiStateChanged(mockTransport));
        assertFalse(mHandler.getBlockedUntilWiFiOn());
        assertFalse(mHandler.getBlockedUntilWiFiOnHandover());

        // wifi
        when(mockTransport.getType()).thenReturn(Transport.TransportType.TRANSPORT_WIFI);
        assertFalse(mHandler32.onWiFiStateChanged(mockTransport));
    }
}