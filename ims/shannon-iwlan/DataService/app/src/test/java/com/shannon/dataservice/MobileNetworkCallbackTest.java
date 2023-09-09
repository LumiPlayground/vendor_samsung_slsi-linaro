package com.shannon.dataservice;

import static org.junit.Assert.*;
import static org.mockito.ArgumentMatchers.anyInt;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;

import android.net.LinkProperties;
import android.net.Network;
import android.net.NetworkRequest;
import android.telephony.NetworkRegistrationInfo;
import android.telephony.ServiceState;
import android.telephony.TelephonyManager;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.Mock;
import org.mockito.MockitoAnnotations;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.annotation.Config;

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class MobileNetworkCallbackTest {
    private static final int testSlotId = 0;

    private AutoCloseable closeable;
    private MobileNetworkCallback mMobileNetworkCallback;

    @Mock private TelephonyManager mMockTelephonyManager;

    @Before
    public void setUp() throws Exception {
        closeable = MockitoAnnotations.openMocks(this);

        mMobileNetworkCallback = new MobileNetworkCallback(testSlotId, mMockTelephonyManager);
    }

    @After
    public void tearDown() throws Exception {
        closeable.close();
    }

    @Test
    public void onLost() {
        //Given
        Network network = new Network(456);

        //When
        mMobileNetworkCallback.onLost(network);

        //Then
        assertNull(mMobileNetworkCallback.getNetwork());
    }

    @Test
    public void onLinkPropertiesChanged() {
        //Given
        Network network = new Network(456);
        LinkProperties mockLinkProperties = mock(LinkProperties.class);

        //When
        mMobileNetworkCallback.onLinkPropertiesChanged(network, mockLinkProperties);

        //Then
        assertEquals(mMobileNetworkCallback.getNetwork(), network);
    }

    @Test
    public void buildRequest() {
        //Given
        when(mMockTelephonyManager.getNetworkSpecifier()).thenReturn("0");

        //When
        NetworkRequest networkRequest = mMobileNetworkCallback.buildRequest();

        //Then
        assertNotNull(networkRequest);
    }

    @Test
    public void isConnected() {
        //When
        onLinkPropertiesChanged();

        //Then
        assertTrue(mMobileNetworkCallback.isConnected());
    }

    @Test
    public void isRoaming() {
        //When
        when(mMockTelephonyManager.isNetworkRoaming()).thenReturn(true);

        //Then
        assertTrue(mMobileNetworkCallback.isRoaming());
    }

    @Test
    public void getLinkProperties() {
        //Given
        Network network = new Network(456);
        LinkProperties mockLinkProperties = mock(LinkProperties.class);

        //When
        mMobileNetworkCallback.onLinkPropertiesChanged(network, mockLinkProperties);

        //Then
        assertEquals(mMobileNetworkCallback.getLinkProperties(), mockLinkProperties);
    }

    @Test
    public void getNetworkType() {
        // Default case
        assertEquals(TelephonyManager.NETWORK_TYPE_UNKNOWN, mMobileNetworkCallback.getNetworkType());

        // Given
        ServiceState mockServiceState = mock(ServiceState.class);
        when(mMockTelephonyManager.getServiceState()).thenReturn(mockServiceState);

        NetworkRegistrationInfo mockNetworkRegistrationInfo = mock(NetworkRegistrationInfo.class);
        when(mockServiceState.getNetworkRegistrationInfo(anyInt(), anyInt())).thenReturn(mockNetworkRegistrationInfo);

        when(mockNetworkRegistrationInfo.getAccessNetworkTechnology()).thenReturn(TelephonyManager.NETWORK_TYPE_IWLAN);

        // When
        int test = mMobileNetworkCallback.getNetworkType();

        // Then
        assertEquals(test, TelephonyManager.NETWORK_TYPE_IWLAN);
    }
}