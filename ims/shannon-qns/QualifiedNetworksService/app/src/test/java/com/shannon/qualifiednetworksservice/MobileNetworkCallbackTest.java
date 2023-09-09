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

import android.net.LinkProperties;
import android.net.Network;
import android.net.NetworkCapabilities;
import android.telephony.CellSignalStrength;
import android.telephony.CellSignalStrengthCdma;
import android.telephony.CellSignalStrengthGsm;
import android.telephony.CellSignalStrengthLte;
import android.telephony.CellSignalStrengthNr;
import android.telephony.CellSignalStrengthTdscdma;
import android.telephony.CellSignalStrengthWcdma;
import android.telephony.NetworkRegistrationInfo;
import android.telephony.ServiceState;
import android.telephony.SignalStrength;
import android.telephony.TelephonyManager;
import com.shannon.qualifiednetworksservice.MobileNetworkCallback.IMobileCallbackEvent;
import com.shannon.qualifiednetworksservice.MobileNetworkCallback.MobileSignalStrengthListener;
import java.util.ArrayList;
import java.util.List;
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

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertTrue;
import static org.mockito.ArgumentMatchers.any;
import static org.mockito.ArgumentMatchers.anyInt;
import static org.mockito.Mockito.doNothing;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

@FixMethodOrder(MethodSorters.NAME_ASCENDING)
@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class MobileNetworkCallbackTest {
    private MobileNetworkCallback mMobileNetworkCallback;
    private MobileSignalStrengthListener mMobileSignalStrengthListener;

    @Mock
    private SimInformation mockSimInformation;
    @Mock
    private TelephonyManager mockTelephonyManager;
    @Mock
    private CellSignalStrength mockCellSignalStrength;
    @Mock
    private SignalStrength mockSignalStrength;
    @Mock
    CellSignalStrengthCdma mockStrengthCdma;
    @Mock
    CellSignalStrengthTdscdma mockStrengthTdscdma;
    @Mock
    CellSignalStrengthGsm mockStrengthGsm;
    @Mock
    CellSignalStrengthWcdma mockStrengthWcdma;
    @Mock
    CellSignalStrengthLte mockStrengthLte;
    @Mock
    CellSignalStrengthNr mockStrengthNr;

    @BeforeClass
    public static void beforeClass() {
    }

    @AfterClass
    public static void afterClass() {
    }

    @Before
    public void setUp() {
        MockitoAnnotations.openMocks(this);
        when(mockSimInformation.getTelephonyManager()).thenReturn(mockTelephonyManager);
        doNothing().when(mockTelephonyManager).listen(any(), anyInt());

        mMobileNetworkCallback = new MobileNetworkCallback(0, mockSimInformation);
    }

    @After
    public void tearDown() {
        Mockito.validateMockitoUsage();
    }

    @Test
    public void destroyTest() {
        // When
        mMobileNetworkCallback.destroy();
        // Then
        verify(mockTelephonyManager, times(2)).listen(any(), anyInt());
    }

    @Test
    public void isSignalBetterThanThresholdTest() {
        // Given
        mMobileNetworkCallback.setSignalStrength(mockCellSignalStrength);
        when(mockCellSignalStrength.isValid()).thenReturn(true);

        // When
        boolean test = mMobileNetworkCallback.isSignalBetterThanThreshold(-100);

        // Then
        assertTrue(test);

        // Given
        when(mockCellSignalStrength.isValid()).thenReturn(false);

        // When
        boolean test_false = mMobileNetworkCallback.isSignalBetterThanThreshold(-100);

        // Then
        assertFalse(test_false);
    }

    @Test
    public void getNetworkTypeTest() {
        // Default case
        assertEquals(TelephonyManager.NETWORK_TYPE_UNKNOWN, mMobileNetworkCallback.getNetworkType());

        // Given
        ServiceState mockServiceState = mock(ServiceState.class);
        when(mockTelephonyManager.getServiceState()).thenReturn(mockServiceState);

        NetworkRegistrationInfo mockNetworkRegistrationInfo = mock(NetworkRegistrationInfo.class);
        when(mockServiceState.getNetworkRegistrationInfo(anyInt(), anyInt())).thenReturn(mockNetworkRegistrationInfo);

        when(mockNetworkRegistrationInfo.getAccessNetworkTechnology()).thenReturn(TelephonyManager.NETWORK_TYPE_IWLAN);

        // When
        int test = mMobileNetworkCallback.getNetworkType();

        // Then
        assertEquals(test, TelephonyManager.NETWORK_TYPE_IWLAN);
    }

    @Test
    public void getNetworkOperatorTest() {
        // Given
        when(mockTelephonyManager.getNetworkOperator()).thenReturn("Test");

        // When
        String test = mMobileNetworkCallback.getNetworkOperator();

        // Then
        assertEquals(test, "Test");
    }

    @Test
    public void getSlotIdTest() {
        // When
        int test = mMobileNetworkCallback.getSlotId();

        // Then
        assertEquals(test, 0);
    }

    @Test
    public void isRoamingTest() {
        // Given
        when(mockTelephonyManager.isNetworkRoaming()).thenReturn(true);

        // When
        boolean test = mMobileNetworkCallback.isRoaming();

        // Then
        assertTrue(test);
    }

    @Test
    public void isConnectedTest() {
        // When
        boolean test = mMobileNetworkCallback.isConnected();

        // Then
        assertFalse(test);
    }

    @Test
    public void registerForNetworkCallbackEventTest() {
        // Given
        IMobileCallbackEvent mockIMobileCallbackEvent = mock(IMobileCallbackEvent.class);

        // When
        mMobileNetworkCallback.registerForNetworkCallbackEvent(mockIMobileCallbackEvent);

        // Then
        // do nothing, mCallbackEvents is private final member, not accessible.
    }

    @Test
    public void unregisterNetworkCallbackEventTest() {
        // Given
        IMobileCallbackEvent mockIMobileCallbackEvent = mock(IMobileCallbackEvent.class);

        // When
        mMobileNetworkCallback.unregisterNetworkCallbackEvent(mockIMobileCallbackEvent);

        // Then
        // do nothing, mCallbackEvents is private final member, not accessible.
    }

    @Test
    public void buildRequestTest() {
        // Given
        when(mockTelephonyManager.getNetworkSpecifier()).thenReturn("Test");
        // When
        mMobileNetworkCallback.buildRequest();

        // Then
        // invoke a new NetworkRequest.Builder()
    }

    @Test
    public void onAvailableTest() {
        // Given
        Network network = new Network(123);
        NetworkCapabilities mockNetworkCapabilities = mock(NetworkCapabilities.class);
        when(mockNetworkCapabilities.hasCapability(anyInt())).thenReturn(true);

        LinkProperties mockLinkProperties = mock(LinkProperties.class);

        // When
        mMobileNetworkCallback.onAvailable(network, mockNetworkCapabilities, mockLinkProperties, true);

        // Then
        // invoke notifyForCallbackEvent
    }

    @Test
    public void onCapabilitiesChangedTest() {
        // Given
        Network network = new Network(123);
        NetworkCapabilities mockNetworkCapabilities = mock(NetworkCapabilities.class);
        when(mockNetworkCapabilities.hasCapability(anyInt())).thenReturn(true);

        // When
        mMobileNetworkCallback.onCapabilitiesChanged(network, mockNetworkCapabilities);

        // Then
        // invoke notifyForCallbackEvent
    }

    @Test
    public void onLostTest() {
        // Given
        Network network = new Network(123);

        // When
        mMobileNetworkCallback.onLost(network);

        // Then
        // invoke notifyForCallbackEvent
    }

    @Test
    public void MobileSignalStrengthListenerTest() {
        // Given
        ArrayList<CellSignalStrength> list = new ArrayList<>();
        list.add(mockStrengthCdma);
        when(mockStrengthCdma.isValid()).thenReturn(false);
        list.add(mockStrengthTdscdma);
        when(mockStrengthTdscdma.isValid()).thenReturn(false);
        list.add(mockStrengthGsm);
        when(mockStrengthGsm.isValid()).thenReturn(true);
        list.add(mockStrengthWcdma);
        when(mockStrengthWcdma.isValid()).thenReturn(true);
        list.add(mockStrengthLte);
        when(mockStrengthLte.isValid()).thenReturn(true);
        list.add(mockStrengthNr);
        when(mockStrengthNr.isValid()).thenReturn(true);
        when(mockSignalStrength.getCellSignalStrengths()).thenReturn(list);

        mMobileSignalStrengthListener = mMobileNetworkCallback.new MobileSignalStrengthListener(0);

        // When
        mMobileSignalStrengthListener.onSignalStrengthsChanged(mockSignalStrength);

        // Then
        // invoke notifyForCallbackEvent

        // Default case for exception
        // When
        mMobileSignalStrengthListener.onSignalStrengthsChanged(null);
    }
}
