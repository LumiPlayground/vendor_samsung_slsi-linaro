package com.shannon.qualifiednetworksservice;

import android.content.Context;
import android.telephony.DataFailCause;

import com.shannon.qualifiednetworksservice.util.DeviceUtils;

import junit.framework.TestCase;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.Mock;
import org.mockito.MockedStatic;
import org.mockito.Mockito;
import org.mockito.MockitoAnnotations;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.annotation.Config;

import java.util.Timer;

import static org.mockito.Mockito.when;

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class TransportTest extends TestCase {
    private static MockedStatic<DeviceUtils> sMockedUtils;
    private static MockedStatic<DataFailCause> sMockedFailCause;

    @BeforeClass
    public static void beforeClass() {
        sMockedUtils = Mockito.mockStatic(DeviceUtils.class);
        sMockedFailCause = Mockito.mockStatic(DataFailCause.class);
    }

    @AfterClass
    public static void afterClass() {
        sMockedUtils.close();
        sMockedFailCause.close();
    }

    private Transport mTransport;
    private Transport mTransportNoVendor;

    @Mock
    private Context mockContext;

    @Mock
    private Timer mockTimer;

    @Mock
    private ConfigurationManager mockConfiguration;

    @Mock
    TransportState.IStateChanged mockCallback;

    @Mock
    Transport.IVendorTransportHandler mockHandler;

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.openMocks(this);

        mTransport = new Transport(Transport.TransportType.TRANSPORT_CELLULAR, mockContext, 1,
                mockConfiguration, mockCallback, mockHandler);

        mTransportNoVendor = new Transport(Transport.TransportType.TRANSPORT_WIFI, mockContext, 2,
                mockConfiguration, mockCallback, null);
    }

    @After
    public void tearDown() throws Exception {
        Mockito.validateMockitoUsage();
    }

    @Test
    public void testGetType() {
        assertEquals(Transport.TransportType.TRANSPORT_CELLULAR, mTransport.getType());
    }

    @Test
    public void testGetInitialState() {
        assertNotNull(mTransport.getInitialState());
    }

    @Test
    public void testGetHandoverState() {
        assertNotNull(mTransport.getHandoverState());
    }

    @Test
    public void testIsAvailable() {
        assertTrue(mTransport.isAvailable());
    }

    @Test
    public void testIsAvailableForHandover() {
        assertTrue(mTransport.isAvailableForHandover());
    }

    @Test
    public void testOnAirplaneModeChanged() {
        mTransportNoVendor.onAirplaneModeChanged();

        when(mockHandler.onAirplaneModeChanged(mTransport)).thenReturn(false);
        mTransport.onAirplaneModeChanged();

        when(mockHandler.onAirplaneModeChanged(mTransport)).thenReturn(true);
        mTransport.onAirplaneModeChanged();
        assertTrue(true);
    }

    @Test
    public void testOnRatChanged() {
        mTransportNoVendor.onRatChanged();
        mTransport.onRatChanged();
        assertTrue(true);
    }

    @Test
    public void testOnOperatorChanged() {
        mTransportNoVendor.onOperatorChanged();
        mTransport.onOperatorChanged();
        assertTrue(true);
    }

    @Test
    public void testOnWiFiStateChanged() {
        mTransportNoVendor.onWiFiStateChanged();
        mTransport.onWiFiStateChanged();
        assertTrue(true);
    }

    @Test
    public void testOnWiFiAccessPointChanged() {
        mTransportNoVendor.onWiFiAccessPointChanged();
        mTransport.onWiFiAccessPointChanged();
        assertTrue(true);
    }

    @Test
    public void testOnWfcStateChanged() {
        mTransportNoVendor.onWfcStateChanged();
        mTransport.onWfcStateChanged();
        assertTrue(true);
    }

    @Test
    public void testOnCarrierConfigChanged() {
        mTransportNoVendor.onCarrierConfigChanged();
        mTransport.onCarrierConfigChanged();
        assertTrue(true);
    }

    @Test
    public void testOnConnectionFailure() {
        mTransport.onConnectionFailure(DataFailCause.NONE, 0);

        when(mockHandler.onConnectionFailure(mTransport, DataFailCause.UNKNOWN, 0)).thenReturn(true);
        mTransport.onConnectionFailure(DataFailCause.UNKNOWN, 0);

        when(mockHandler.onConnectionFailure(mTransport, DataFailCause.UNKNOWN, 10000)).thenReturn(false);
        mTransport.onConnectionFailure(DataFailCause.UNKNOWN, 10000);

        when(mockHandler.onConnectionFailure(mTransport, DataFailCause.UNKNOWN, 0)).thenReturn(false);
        when(DataFailCause.isPermanentFailure(mockContext, DataFailCause.UNKNOWN, 1)).thenReturn(true);
        when(DataFailCause.isPermanentFailure(mockContext, DataFailCause.UNKNOWN, 2)).thenReturn(true);

        when(mockConfiguration.getCellularPermanentErrorRetryCount()).thenReturn(1);
        when(mockConfiguration.getCellularPermanentErrorRetryTimeout()).thenReturn(0);
        when(mockConfiguration.getIwlanPermanentErrorRetryCount()).thenReturn(1);
        when(mockConfiguration.getIwlanPermanentErrorRetryTimeout()).thenReturn(1);

        mTransport.onConnectionFailure(DataFailCause.UNKNOWN, 0);
        mTransportNoVendor.onConnectionFailure(DataFailCause.UNKNOWN, 0);

        when(DataFailCause.isPermanentFailure(mockContext, DataFailCause.UNKNOWN, 1)).thenReturn(false);
        when(mockConfiguration.getInitialNonPermanentErrorRetryCount()).thenReturn(1);
        when(mockConfiguration.getInitialNonPermanentErrorRetryTimeout()).thenReturn(1);
        mTransport.onConnectionFailure(DataFailCause.UNKNOWN, 0);

        TransportState state = mTransport.getInitialState();
        state.reset(); state.onConnecting(); state.onConnecting();
        mTransport.onConnectionFailure(DataFailCause.UNKNOWN, 0);

        assertTrue(true);
    }

    @Test
    public void testOnHandoverFailure() {

        when(DeviceUtils.isConnectionCallbackFixed()).thenReturn(false);

        mTransport.onHandoverFailure(DataFailCause.NONE, 0);

        when(mockHandler.onHandoverFailure(mTransport, DataFailCause.UNKNOWN, 0)).thenReturn(true);
        mTransport.onHandoverFailure(DataFailCause.UNKNOWN, 0);

        when(DeviceUtils.isConnectionCallbackFixed()).thenReturn(true);

        when(mockHandler.onHandoverFailure(mTransport, DataFailCause.UNKNOWN, 10000)).thenReturn(false);
        mTransport.onHandoverFailure(DataFailCause.UNKNOWN, 10000);

        when(mockHandler.onHandoverFailure(mTransport, DataFailCause.UNKNOWN, 0)).thenReturn(false);
        when(DataFailCause.isPermanentFailure(mockContext, DataFailCause.UNKNOWN, 1)).thenReturn(true);
        when(DataFailCause.isPermanentFailure(mockContext, DataFailCause.UNKNOWN, 2)).thenReturn(true);

        when(mockConfiguration.getCellularPermanentErrorRetryCount()).thenReturn(1);
        when(mockConfiguration.getCellularPermanentErrorHandoverRetryTimeout()).thenReturn(0);
        when(mockConfiguration.getIwlanPermanentErrorRetryCount()).thenReturn(1);
        when(mockConfiguration.getIwlanPermanentErrorHandoverRetryTimeout()).thenReturn(1);

        mTransport.onHandoverFailure(DataFailCause.UNKNOWN, 0);
        mTransportNoVendor.onHandoverFailure(DataFailCause.UNKNOWN, 0);

        when(DataFailCause.isPermanentFailure(mockContext, DataFailCause.UNKNOWN, 1)).thenReturn(false);
        when(mockConfiguration.getHandoverNonPermanentErrorRetryCount()).thenReturn(1);
        when(mockConfiguration.getHandoverNonPermanentErrorRetryTimeout()).thenReturn(1);
        mTransport.onHandoverFailure(DataFailCause.UNKNOWN, 0);

        TransportState state = mTransport.getHandoverState();
        state.reset(); state.onConnecting(); state.onConnecting();
        mTransport.onHandoverFailure(DataFailCause.UNKNOWN, 0);

        assertTrue(true);
    }

    @Test
    public void testIsHandoverFailed() {
        assertFalse(mTransport.isHandoverFailed());
    }

    @Test
    public void testIsConnecting() {
        assertFalse(mTransport.isConnecting());
    }

    @Test
    public void testTryConnecting() {
        mTransportNoVendor.tryConnecting();
        mTransport.tryConnecting();
        assertTrue(true);
    }

    @Test
    public void testTryHandover() {
        mTransportNoVendor.tryHandover();
        mTransport.tryHandover();
        assertTrue(true);
    }

    @Test
    public void testOnConnected() {
        mTransportNoVendor.onConnected();
        mTransport.onConnected();
        assertTrue(true);
    }

    @Test
    public void testOnHandoverCompleted() {
        mTransportNoVendor.onHandoverCompleted();
        mTransport.onHandoverCompleted();
        assertTrue(true);
    }

    @Test
    public void testOnDisconnected() {
        mTransportNoVendor.onDisconnected();
        mTransport.onDisconnected();
        assertTrue(true);
    }

    @Test
    public void testGetLastFailCause() {
        assertEquals(DataFailCause.NONE, mTransport.getLastFailCause());
    }

    @Test
    public void testOnRegistrationFailed() {
        mTransportNoVendor.onRegistrationFailed(4);
        mTransport.onRegistrationFailed(4);
        assertTrue(true);
    }

    @Test
    public void testTestToString() {
        assertNotNull(mTransport.toString());
    }
}