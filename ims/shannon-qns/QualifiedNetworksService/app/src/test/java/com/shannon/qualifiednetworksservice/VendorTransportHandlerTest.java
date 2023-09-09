package com.shannon.qualifiednetworksservice;

import junit.framework.TestCase;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.Mockito;
import org.mockito.MockitoAnnotations;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.annotation.Config;

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class VendorTransportHandlerTest extends TestCase {
    private VendorTransportHandler mHandler;

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.openMocks(this);

        mHandler = new VendorTransportHandler();
    }

    @After
    public void tearDown() throws Exception {
        Mockito.validateMockitoUsage();
    }

    @Test
    public void testDoInitialization() {
        mHandler.doInitialization(null);
        assertTrue(true);
    }

    @Test
    public void testTryConnecting() {
        mHandler.tryConnecting(null);
        assertTrue(true);
    }

    @Test
    public void testTryHandover() {
        mHandler.tryHandover(null);
        assertTrue(true);
    }

    @Test
    public void testOnConnected() {
        mHandler.onConnected(null);
        assertTrue(true);
    }

    @Test
    public void testOnHandoverCompleted() {
        mHandler.onHandoverCompleted(null);
        assertTrue(true);
    }

    @Test
    public void testOnDisconnected() {
        mHandler.onDisconnected(null);
        assertTrue(true);
    }

    @Test
    public void testOnConnectionFailure() {
        assertFalse(mHandler.onConnectionFailure(null, 0, 0));
    }

    @Test
    public void testOnHandoverFailure() {
        assertFalse(mHandler.onHandoverFailure(null, 0, 0));
    }

    @Test
    public void testOnAirplaneModeChanged() {
        assertFalse(mHandler.onAirplaneModeChanged(null));
    }

    @Test
    public void testOnOperatorChanged() {
        assertFalse(mHandler.onOperatorChanged(null));
    }

    @Test
    public void testOnRatChanged() {
        assertFalse(mHandler.onRatChanged(null));
    }

    @Test
    public void testOnWiFiStateChanged() {
        assertFalse(mHandler.onWiFiStateChanged(null));
    }

    @Test
    public void testOnWiFiAccessPointChanged() {
        assertFalse(mHandler.onWiFiAccessPointChanged(null));
    }

    @Test
    public void testOnWfcStateChanged() {
        assertFalse(mHandler.onWfcStateChanged(null));
    }

    @Test
    public void testOnCarrierConfigChanged() {
        assertFalse(mHandler.onCarrierConfigChanged(null));
    }

    @Test
    public void testOnRegistrationFailed() {
        assertFalse(mHandler.onRegistrationFailed(null, 0));
    }
}