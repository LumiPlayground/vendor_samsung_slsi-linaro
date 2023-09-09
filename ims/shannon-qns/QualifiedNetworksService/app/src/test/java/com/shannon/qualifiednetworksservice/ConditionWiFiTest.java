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

import android.content.Context;
import android.content.Intent;
import android.net.ConnectivityManager;
import android.net.ConnectivityManager.NetworkCallback;
import android.net.LinkAddress;
import android.net.LinkProperties;
import android.net.Network;
import android.net.NetworkCapabilities;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.telephony.AccessNetworkConstants;
import android.telephony.NetworkRegistrationInfo;
import android.telephony.ServiceState;
import android.telephony.TelephonyManager;
import android.telephony.data.ApnSetting;

import com.android.internal.telephony.metrics.AirplaneModeStats;
import com.shannon.qualifiednetworksservice.ConditionWiFi.DefaultNetworkCallback;
import com.shannon.qualifiednetworksservice.ConditionWiFi.IpReachableTimerTask;
import com.shannon.qualifiednetworksservice.ConditionWiFi.WiFiNetworkCallback;

import junit.framework.TestCase;

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

import java.net.InetAddress;
import java.net.UnknownHostException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Timer;
import java.util.concurrent.ConcurrentLinkedDeque;
import java.util.concurrent.ConcurrentLinkedQueue;

import static android.telephony.ims.ImsMmTelManager.WIFI_MODE_WIFI_PREFERRED;
import static com.shannon.qualifiednetworksservice.HandoverConditions.ConditionEvents.EVENT_DEFAULT_LINK_PROPERTIES_CHANGED;
import static com.shannon.qualifiednetworksservice.HandoverConditions.ConditionEvents.EVENT_DESTINATION_UNREACHABLE;
import static com.shannon.qualifiednetworksservice.HandoverConditions.ConditionEvents.EVENT_WIFI_AP_HANDOVER_TIMER_EXPIRED;
import static com.shannon.qualifiednetworksservice.HandoverConditions.ConditionEvents.EVENT_WIFI_BUTTON_STATE_CHANGED;
import static com.shannon.qualifiednetworksservice.HandoverConditions.ConditionEvents.EVENT_WIFI_CAPABILITIES_CHANGED;
import static com.shannon.qualifiednetworksservice.HandoverConditions.ConditionEvents.EVENT_WIFI_CONNECTED;
import static com.shannon.qualifiednetworksservice.HandoverConditions.ConditionEvents.EVENT_WIFI_DISCONNECTED;
import static com.shannon.qualifiednetworksservice.HandoverConditions.ConditionEvents.EVENT_WIFI_SIGNAL_STRENGTH_CHANGED;
import static com.shannon.qualifiednetworksservice.HandoverConditions.ConditionEvents.EVENT_WLAN_PS_STATE_CHANGED;
import static com.shannon.qualifiednetworksservice.MapType.MAP_DEFAULT;
import static com.shannon.qualifiednetworksservice.MapType.MAP_IDLE;
import static org.mockito.ArgumentMatchers.any;
import static org.mockito.ArgumentMatchers.anyInt;
import static org.mockito.ArgumentMatchers.eq;
import static org.mockito.Mockito.doThrow;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

@FixMethodOrder(MethodSorters.NAME_ASCENDING)
@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class ConditionWiFiTest extends TestCase {
    private static MockedStatic<SimManager> sMockedSimManager;
    private static MockedStatic<LocationManager> sMockedLocationManager;
    private ConditionWiFi mConditionWiFi;
    private WiFiNetworkCallback mWiFiNetworkCallback;

    @Mock
    private SimManager mockSimManager;
    @Mock
    private LocationManager mockLocationManager;
    @Mock
    private Context mockContext;
    @Mock
    private ConfigurationManager mockConfigurationManager;
    @Mock
    private HandoverConditions.IConditionChanged mockChangedCallback;
    @Mock
    private TelephonyManager mockTelephonyManager;
    @Mock
    private WifiManager mockWifiManager;
    @Mock
    private WifiInfo mockInfo;
    @Mock
    private ConnectivityManager mockConnectivityManager;
    @Mock
    private Network mockNetwork;
    @Mock
    private Timer mockWiFiHandoverTimer;
    @Mock
    private ConditionConnectivity mockConditionConnectivity;
    @Mock
    private HandoverConditions.IConditionHelper mockConditionHelper;
    @Mock
    private NetworkRegistrationInfo mockNetworkRegistrationInfo;
    @Mock
    private Intent mockIntent;

    private HashMap<MapType, Integer> mRssiIn = new HashMap<>();
    private HashMap<MapType, Integer> mRssiOut = new HashMap<>();
    private String mHomeCountry;
    private boolean mValidated;
    private boolean mDefault;
    private boolean mIpReachable;
    private List<ConnectivityManager.NetworkCallback> mCallbackList;
    private int mWiFiState;
    private boolean mWiFiValidationRequired;
    private Network mNetwork;
    private LinkProperties mLinkProperties;
    private InetAddress[] mIpReachableList;
    private IpReachableTimerTask mIpReachableTimerTask;

    @BeforeClass
    public static void beforeClass() {
        sMockedSimManager = Mockito.mockStatic(SimManager.class);
        sMockedLocationManager = Mockito.mockStatic(LocationManager.class);
    }

    @AfterClass
    public static void afterClass() {
        sMockedSimManager.close();
        sMockedLocationManager.close();
        sMockedSimManager = null;
        sMockedLocationManager = null;
    }

    @Before
    public void setUp() {
        MockitoAnnotations.openMocks(this);

        sMockedSimManager.when(SimManager::getInstance).thenReturn(mockSimManager);
        sMockedLocationManager.when(LocationManager::getInstance).thenReturn(mockLocationManager);

        when(mockSimManager.getMcc(anyInt())).thenReturn(100);
        when(mockSimManager.getMnc(anyInt())).thenReturn(200);
        when(mockSimManager.getTelephonyManager(anyInt())).thenReturn(mockTelephonyManager);
        when(mockTelephonyManager.getSimCountryIso()).thenReturn("Test-Country");
        when(mockContext.getSystemService(WifiManager.class)).thenReturn(mockWifiManager);
        when(mockWifiManager.getConnectionInfo()).thenReturn(mockInfo);
        when(mockInfo.getRssi()).thenReturn(-100);

        when(mockConfigurationManager.getDefaultImsHandoverIn()).thenReturn("iwlan{rssi=-70}");
        when(mockConfigurationManager.getImsRoveIn()).thenReturn("iwlan{rssi=-75}");
        when(mockConfigurationManager.getImsRoveInWiFiPreferred()).thenReturn("iwlan{rssi=-80}");
        when(mockConfigurationManager.getImsRoveInCellPreferred()).thenReturn("iwlan{rssi=-85}");
        when(mockConfigurationManager.getImsHandIn()).thenReturn("iwlan{rssi=-90}");
        when(mockConfigurationManager.getImsHandInWiFiPreferred()).thenReturn("iwlan{rssi=-95}");
        when(mockConfigurationManager.getImsHandInCellPreferred()).thenReturn("iwlan{rssi=-100}");
        when(mockConfigurationManager.getImsRoveInRoaming()).thenReturn("iwlan{rssi=-105}");
        when(mockConfigurationManager.getImsRoveInHysteresisOn()).thenReturn("iwlan{rssi=-105}");
        when(mockConfigurationManager.getImsHandInRoaming()).thenReturn("iwlan{rssi=-110}");
        when(mockConfigurationManager.getImsHandInHysteresisOn()).thenReturn("iwlan{rssi=-105}");
        when(mockConfigurationManager.getDefaultImsHandoverOut()).thenReturn("iwlan{rssi=-110}");
        when(mockConfigurationManager.getImsRoveOut()).thenReturn("iwlan{rssi=-80}");
        when(mockConfigurationManager.getImsRoveOutWiFiPreferred()).thenReturn("iwlan{rssi=-80}");
        when(mockConfigurationManager.getImsRoveOutCellPreferred()).thenReturn("iwlan{rssi=-80}");
        when(mockConfigurationManager.getImsHandOut()).thenReturn("iwlan{rssi=-130}");
        when(mockConfigurationManager.getImsHandOutWiFiPreferred()).thenReturn("iwlan{rssi=-135}");
        when(mockConfigurationManager.getImsHandOutCellPreferred()).thenReturn("iwlan{rssi=-140}");
        when(mockConfigurationManager.getImsRoveOutHysteresisOn()).thenReturn("iwlan{rssi=-80}");
        when(mockConfigurationManager.getImsHandOutHysteresisOn()).thenReturn("iwlan{rssi=-150}");

        when(mockContext.getSystemService(ConnectivityManager.class)).thenReturn(mockConnectivityManager);

        mConditionWiFi = new ConditionWiFi(0, mockContext, mockConfigurationManager, mockConditionHelper,
                mockChangedCallback);
        mNetwork = mockNetwork;
        mConditionWiFi.setNetwork(mNetwork);
    }

    @After
    public void tearDown() {
        Mockito.validateMockitoUsage();
    }

    @Test
    public void destroyTest() {
        // Given
        // stopIpReachableTimer()
        IpReachableTimerTask mockIpReachableTimerTask = mock(ConditionWiFi.IpReachableTimerTask.class);
        mIpReachableTimerTask = mockIpReachableTimerTask;
        //removeSignalStrengthCallbacks()
        ConnectivityManager.NetworkCallback mockNetworkCallback = new ConnectivityManager.NetworkCallback();
        List<NetworkCallback> mockCallbackList = mock(List.class);
        mockCallbackList.add(mockNetworkCallback);
        mCallbackList = mockCallbackList;

        ConcurrentLinkedQueue<NetworkCallback> mCallbackList = mConditionWiFi.getCallbackList();
        mCallbackList.add(mockNetworkCallback);

        // When
        mConditionWiFi.destroy();

        // Then
        // 3 invocations at removeSignalStrengthCallbacks() removeDefaultNetworkCallback() and destroy()
        verify(mockConnectivityManager, times(3))
            .unregisterNetworkCallback(any(ConnectivityManager.NetworkCallback.class));
    }

    @Test
    public void isConnected_isConnected_WIFI_STATE_DISABLED_Test() {
        // Given
        // isWiFiDisabled
        mWiFiState = WifiManager.WIFI_STATE_DISABLED;
        mConditionWiFi.setWiFiState(mWiFiState);

        // isWiFiHandoverActive

        // When
        boolean test = mConditionWiFi.isConnected();

        // Then
        assertFalse(test);
    }

    @Test
    public void isConnected_true_with_IPv4_Test() throws UnknownHostException {
        // Given
        // isWiFiDisabled
        mWiFiState = WifiManager.WIFI_STATE_ENABLED;
        mConditionWiFi.setWiFiState(mWiFiState);

        // isWiFiHandoverActive
        mDefault = true;
        mConditionWiFi.setDefault(mDefault);

        // isIpAddressSet returns true with IPv4 address.
        LinkProperties mockLinkProperties = mock(LinkProperties.class);
        List<LinkAddress> linkAddressList = new ArrayList<>();

        InetAddress ipv4address1 = InetAddress.getByName("30.40.246.108");
        InetAddress ipv4address2 = InetAddress.getByName("30.40.246.108");

        linkAddressList.add(new LinkAddress(ipv4address1, 0));
        linkAddressList.add(new LinkAddress(ipv4address2, 0));
        when(mockLinkProperties.getLinkAddresses()).thenReturn(linkAddressList);
        mLinkProperties = mockLinkProperties;
        mConditionWiFi.setLinkProperties(mLinkProperties);
        mConditionWiFi.setPsState(mockNetworkRegistrationInfo);
        when(mockNetworkRegistrationInfo.isRegistered()).thenReturn(true);

        // When
        boolean test = mConditionWiFi.isConnected();

        // Then
        assertTrue(test);

        // Given

        // When
        boolean test_WiFiHandoverActive = mConditionWiFi.isConnected();

        // Then
        assertTrue(test_WiFiHandoverActive);
    }

    @Test
    public void isConnected_true_with_IPv6_Test() throws UnknownHostException {
        // Given
        // isWiFiDisabled
        mWiFiState = WifiManager.WIFI_STATE_ENABLED;
        mConditionWiFi.setWiFiState(mWiFiState);

        // isWiFiHandoverActive

        mDefault = true;
        mConditionWiFi.setDefault(mDefault);

        // isIpAddressSet returns true with IPv6 address.
        LinkProperties mockLinkProperties = mock(LinkProperties.class);
        List<LinkAddress> linkAddressList = new ArrayList<>();

        InetAddress ipv6address1 = InetAddress.getByName("fc01:bbbb:cdcd:efe1::1");
        InetAddress ipv6address2 = InetAddress.getByName("fc01:bbbb:cdcd:efe1::1");

        linkAddressList.add(new LinkAddress(ipv6address1, 0));
        linkAddressList.add(new LinkAddress(ipv6address2, 0));
        when(mockLinkProperties.getLinkAddresses()).thenReturn(linkAddressList);
        mLinkProperties = mockLinkProperties;
        mConditionWiFi.setLinkProperties(mLinkProperties);
        mConditionWiFi.setPsState(mockNetworkRegistrationInfo);

        when(mockConfigurationManager.canUseWiFiIPv6()).thenReturn(true);
        when(mockNetworkRegistrationInfo.isRegistered()).thenReturn(true);

        // When
        boolean test = mConditionWiFi.isConnected();

        // Then
        assertTrue(test);
    }

    @Test
    public void isConnected_WiFiValidation_True_Test() throws UnknownHostException {
        // Given
        // isWiFiDisabled
        mWiFiState = WifiManager.WIFI_STATE_ENABLED;
        mConditionWiFi.setWiFiState(mWiFiState);

        // isWiFiHandoverActive

        mDefault = false;
        mConditionWiFi.setDefault(mDefault);

        // WiFiValidation true
        mWiFiValidationRequired = true;
        mConditionWiFi.setWiFiValidationRequired(mWiFiValidationRequired);
        mValidated = true;
        mConditionWiFi.setValidated(mValidated);

        // isIpAddressSet returns true with IPv6 address.
        LinkProperties mockLinkProperties = mock(LinkProperties.class);
        List<LinkAddress> linkAddressList = new ArrayList<>();

        InetAddress ipv6address1 = InetAddress.getByName("fc01:bbbb:cdcd:efe1::1");
        InetAddress ipv6address2 = InetAddress.getByName("fc01:bbbb:cdcd:efe1::1");

        linkAddressList.add(new LinkAddress(ipv6address1, 0));
        linkAddressList.add(new LinkAddress(ipv6address2, 0));
        when(mockLinkProperties.getLinkAddresses()).thenReturn(linkAddressList);
        mLinkProperties = mockLinkProperties;
        mConditionWiFi.setLinkProperties(mLinkProperties);
        mConditionWiFi.setPsState(mockNetworkRegistrationInfo);

        when(mockConfigurationManager.canUseWiFiIPv6()).thenReturn(true);
        when(mockNetworkRegistrationInfo.isRegistered()).thenReturn(true);

        // When
        boolean test = mConditionWiFi.isConnected();

        // Then
        assertTrue(test);
    }

    @Test
    public void isWiFiApHandoverActive_handover_true_Test() {
        // Given
        // WiFiValidation true
        mWiFiValidationRequired = false;
        mConditionWiFi.setWiFiValidationRequired(mWiFiValidationRequired);
        mValidated = true;
        mConditionWiFi.setValidated(mValidated);
        // isWiFiHandoverActive
        mConditionWiFi.setWiFiHandoverTimer(mockWiFiHandoverTimer);

        mConditionWiFi.setPsState(mockNetworkRegistrationInfo);
        when(mockNetworkRegistrationInfo.isRegistered()).thenReturn(false);

        // When
        boolean test = mConditionWiFi.isWiFiApHandoverActive();

        // Then
        assertTrue(test);
    }

    @Test
    public void isWiFiApHandoverActive_handover_false_Test() throws UnknownHostException {
        // Given
        // WiFiValidation true
        mWiFiValidationRequired = false;
        mConditionWiFi.setWiFiValidationRequired(mWiFiValidationRequired);
        mValidated = true;
        mConditionWiFi.setValidated(mValidated);

        // isIpAddressSet returns true with IPv6 address.
        LinkProperties mockLinkProperties = mock(LinkProperties.class);
        List<LinkAddress> linkAddressList = new ArrayList<>();

        InetAddress ipv6address1 = InetAddress.getByName("fc01:bbbb:cdcd:efe1::1");
        InetAddress ipv6address2 = InetAddress.getByName("fc01:bbbb:cdcd:efe1::1");

        linkAddressList.add(new LinkAddress(ipv6address1, 0));
        linkAddressList.add(new LinkAddress(ipv6address2, 0));
        when(mockLinkProperties.getLinkAddresses()).thenReturn(linkAddressList);
        mLinkProperties = mockLinkProperties;
        mConditionWiFi.setLinkProperties(mLinkProperties);

        when(mockConfigurationManager.canUseWiFiIPv6()).thenReturn(true);

        // When
        boolean test = mConditionWiFi.isWiFiApHandoverActive();

        // Then
        assertFalse(test);
    }

    @Test
    public void isRoveInTest() {
        // Given
        ConcurrentLinkedDeque<Integer> mSignalStrength = mConditionWiFi.getSignalStrength();
        mSignalStrength.clear();
        mSignalStrength.add(-70);
        mSignalStrength.add(-60);
        mSignalStrength.add(-50);
        // isBetterThanThreshold() and getRssiRoveIn()

        // When
        boolean testisRoveIn = mConditionWiFi.isRoveIn();

        // Then
        assertTrue(testisRoveIn);
    }

    @Test
    public void isRoveInTest_exceptional_case() {
        // When
        boolean testisRoveIn = mConditionWiFi.isRoveIn();

        // Then
        assertFalse(testisRoveIn);
    }

    @Test
    public void isRoveInTest_withParameter_exceptional_case() {
        // When
        boolean testisRoveIn = mConditionWiFi.isRoveIn(false, false);

        // Then
        assertFalse(testisRoveIn);
    }

    @Test
    public void getRssiRoveInTest() {
        // Given
        mRssiIn.put(MAP_IDLE, null);
        mRssiIn.put(MAP_DEFAULT, null);
        mConditionWiFi.setRssiIn(mRssiIn);

        // When
        int test_DEFAULT_RSSI_IN = mConditionWiFi.getRssiRoveIn();
        // Then
        assertEquals(-70, test_DEFAULT_RSSI_IN);

        // Given
        mRssiIn.put(MAP_IDLE, -100);
        mRssiIn.put(MAP_DEFAULT, null);
        mConditionWiFi.setRssiIn(mRssiIn);

        // When
        int test_MAP_IDLE = mConditionWiFi.getRssiRoveIn();
        // Then
        assertEquals(-100, test_MAP_IDLE);

        // Given
        mRssiIn.put(MAP_IDLE, null);
        mRssiIn.put(MAP_DEFAULT, -110);
        mConditionWiFi.setRssiIn(mRssiIn);

        // When
        int test_MAP_DEFAULT = mConditionWiFi.getRssiRoveIn();
        // Then
        assertEquals(-110, test_MAP_DEFAULT);
    }

    @Test
    public void getRssiRoveOutTest() {
        // Given
        mRssiOut.put(MAP_IDLE, null);
        mRssiOut.put(MAP_DEFAULT, null);
        mConditionWiFi.setRssiOut(mRssiOut);

        // When
        int test_DEFAULT_RSSI_OUT = mConditionWiFi.getRssiRoveOut();
        // Then
        assertEquals(-80, test_DEFAULT_RSSI_OUT);

        // Given
        mRssiOut.put(MAP_IDLE, -100);
        mRssiOut.put(MAP_DEFAULT, null);
        mConditionWiFi.setRssiOut(mRssiOut);

        // When
        int test_MAP_IDLE = mConditionWiFi.getRssiRoveOut();
        // Then
        assertEquals(-100, test_MAP_IDLE);

        // Given
        mRssiOut.put(MAP_IDLE, null);
        mRssiOut.put(MAP_DEFAULT, -110);
        mConditionWiFi.setRssiOut(mRssiOut);

        // When
        int test_MAP_DEFAULT = mConditionWiFi.getRssiRoveOut();
        // Then
        assertEquals(-110, test_MAP_DEFAULT);
    }

    @Test
    public void isRoveIn_default_false_Test() {
        // When
        boolean testisRoveIn = mConditionWiFi.isRoveIn();

        // Then
        assertFalse(testisRoveIn);
    }

    @Test
    public void isRoveIn_with_args_default_Test() {
        // Given
        ConcurrentLinkedDeque<Integer> mSignalStrength = mConditionWiFi.getSignalStrength();
        mSignalStrength.clear();

        // When
        boolean testisRoveIn = mConditionWiFi.isRoveIn(true, true);

        // Then
        assertFalse(testisRoveIn);
    }

    @Test
    public void isRoveIn_with_args_Test() {
        // Given
        ConcurrentLinkedDeque<Integer> mSignalStrength = mConditionWiFi.getSignalStrength();
        mSignalStrength.clear();
        mSignalStrength.add(-70);
        mSignalStrength.add(-60);
        mSignalStrength.add(-50);

        // When
        boolean testisRoveIn = mConditionWiFi.isRoveIn(true, false);

        // Then
        assertTrue(testisRoveIn);
    }

    @Test
    public void isRoveIn_with_int_arg_default_Test() {
        // When
        boolean testisRoveIn = mConditionWiFi.isRoveIn(WIFI_MODE_WIFI_PREFERRED);

        // Then
        assertFalse(testisRoveIn);
    }

    @Test
    public void isRoveIn_with_int_arg_Test() {
        // Given
        ConcurrentLinkedDeque<Integer> mSignalStrength = mConditionWiFi.getSignalStrength();
        mSignalStrength.clear();
        mSignalStrength.add(-70);
        mSignalStrength.add(-60);
        mSignalStrength.add(-50);

        // When
        boolean testisRoveIn = mConditionWiFi.isRoveIn(WIFI_MODE_WIFI_PREFERRED);

        // Then
        assertTrue(testisRoveIn);
    }

    @Test
    public void getRssiTest() {
        // Given
        ConcurrentLinkedDeque<Integer> mSignalStrength = mConditionWiFi.getSignalStrength();
        mSignalStrength.clear();
        mSignalStrength.add(-70);

        // When
        int testgetRssi = mConditionWiFi.getRssi();

        // Then
        assertEquals(-70, testgetRssi);
    }

    @Test
    public void isRoveOutTest() {
        // When
        boolean testisRoveOut = mConditionWiFi.isRoveOut(WIFI_MODE_WIFI_PREFERRED);

        // Then
        assertTrue(testisRoveOut);
    }

    @Test
    public void isHandIn_default_false_Test() {
        // When
        boolean testisHandIn = mConditionWiFi.isHandIn();

        // Then
        assertFalse(testisHandIn);
    }

    @Test
    public void isHandIn_true_Test() {
        // Given
        ConcurrentLinkedDeque<Integer> mSignalStrength = mConditionWiFi.getSignalStrength();
        mSignalStrength.clear();
        mSignalStrength.add(-70);
        mSignalStrength.add(-60);
        mSignalStrength.add(-50);

        // When
        boolean testisHandIn = mConditionWiFi.isHandIn();

        // Then
        assertTrue(testisHandIn);
    }

    @Test
    public void isHandIn_default_2_args_Test() {
        // When
        boolean testisHandIn = mConditionWiFi.isHandIn(true, true);

        // Then
        assertTrue(testisHandIn);
    }

    @Test
    public void isHandIn_default_2_args_true_Test() {
        // Given
        ConcurrentLinkedDeque<Integer> mSignalStrength = mConditionWiFi.getSignalStrength();
        mSignalStrength.clear();
        mSignalStrength.add(-70);
        mSignalStrength.add(-60);
        mSignalStrength.add(-50);

        // When
        boolean testisHandIn = mConditionWiFi.isHandIn(true, false);

        // Then
        assertTrue(testisHandIn);
    }

    @Test
    public void isHandIn_with_int_arg_default_Test() {
        // When
        boolean testisHandIn = mConditionWiFi.isHandIn(WIFI_MODE_WIFI_PREFERRED);

        // Then
        assertFalse(testisHandIn);
    }

    @Test
    public void isHandIn_with_int_arg_Test() {
        // Given
        ConcurrentLinkedDeque<Integer> mSignalStrength = mConditionWiFi.getSignalStrength();
        mSignalStrength.clear();
        mSignalStrength.add(-70);
        mSignalStrength.add(-60);
        mSignalStrength.add(-50);

        // When
        boolean testisHandIn = mConditionWiFi.isHandIn(WIFI_MODE_WIFI_PREFERRED);

        // Then
        assertTrue(testisHandIn);
    }

    @Test
    public void isLowerThanThresholdTest() {
        // When
        boolean testisLowerThanThreshold_empty_strengths = mConditionWiFi.isLowerThanThreshold(-80);

        // Then
        assertTrue(testisLowerThanThreshold_empty_strengths);

        // Given
        ConcurrentLinkedDeque<Integer> mSignalStrength = mConditionWiFi.getSignalStrength();
        mSignalStrength.clear();
        mSignalStrength.add(-70);
        mSignalStrength.add(-60);
        mSignalStrength.add(-50);

        // When
        boolean testisLowerThanThreshold = mConditionWiFi.isLowerThanThreshold(-80);

        // Then
        assertFalse(testisLowerThanThreshold);
    }

    @Test
    public void isBetterThanThresholdTest() {
        // When
        boolean isBetterThanThreshold_empty_strengths = mConditionWiFi.isBetterThanThreshold(-80);

        // Then
        assertFalse(isBetterThanThreshold_empty_strengths);

        // Given
        ConcurrentLinkedDeque<Integer> mSignalStrength = mConditionWiFi.getSignalStrength();
        mSignalStrength.clear();
        mSignalStrength.add(-70);
        mSignalStrength.add(-60);
        mSignalStrength.add(-50);

        // When
        boolean testisBetterThanThreshold = mConditionWiFi.isBetterThanThreshold(-80);

        // Then
        assertTrue(testisBetterThanThreshold);
    }

    @Test
    public void isBetterThanThresholdTest_exceptional_case() {
        // When
        boolean testisBetterThanThreshold = mConditionWiFi.isBetterThanThreshold(-80);

        // Then
        // because there is no signal strength.
        assertFalse(testisBetterThanThreshold);
    }

    @Test
    public void isIpReachableTest() {
        // Given
        mIpReachable = true;
        mConditionWiFi.setIpReachable(mIpReachable);
        // When
        boolean testisIpReachable = mConditionWiFi.isIpReachable();

        // Then
        assertTrue(testisIpReachable);
    }

    @Test
    public void startIpReachableTimerTest() throws UnknownHostException {
        // Given
        when(mockConfigurationManager.getPingPeriod()).thenReturn(10);

        // getIpListToReach
        when(mockConfigurationManager.getPingDestination()).thenReturn("epdg.epc.mnc.mcc");
        InetAddress[] inetAddressArray = InetAddress.getAllByName("127.0.0.1");
        when(mockNetwork.getAllByName(any())).thenReturn(inetAddressArray);

        // When
        mConditionWiFi.startIpReachableTimer();

        // Then
        // do nothing, mTimer modification can't be verified.
    }

    @Test
    public void stopIpReachableTimer() {
        // Given
        when(mockConfigurationManager.getPingPeriod()).thenReturn(10);

        // getIpListToReach
        try {
            when(mockConfigurationManager.getPingDestination()).thenReturn("epdg.epc.mnc.mcc");
            InetAddress[] inetAddressArray = InetAddress.getAllByName("127.0.0.1");
            when(mockNetwork.getAllByName(any())).thenReturn(inetAddressArray);
        } catch (UnknownHostException e) {
            e.printStackTrace();
        }

        // When
        mConditionWiFi.startIpReachableTimer();
        mConditionWiFi.stopIpReachableTimer();
    }

    @Test
    public void onConfigurationChangedTest() {
        // Given
        mWiFiValidationRequired = true;
        mConditionWiFi.setWiFiValidationRequired(mWiFiValidationRequired);
        when(mockConfigurationManager.isWiFiValidationRequired()).thenReturn(false);

        // When
        mConditionWiFi.onConfigurationChanged();

        // Then
        verify(mockConnectivityManager, times(1)).registerDefaultNetworkCallback(any());

        // Given
        mWiFiValidationRequired = false;
        mConditionWiFi.setWiFiValidationRequired(mWiFiValidationRequired);
        when(mockConfigurationManager.isWiFiValidationRequired()).thenReturn(true);

        // When
        mConditionWiFi.onConfigurationChanged();

        // Then
        verify(mockConnectivityManager, times(1)).unregisterNetworkCallback(any(DefaultNetworkCallback.class));
    }

    @Test
    public void stopWiFiApHandoverTimerTest() {
        // Given
        mConditionWiFi.setWiFiHandoverTimer(mockWiFiHandoverTimer);

        // When
        mConditionWiFi.stopWiFiApHandoverTimer();

        // Then
        verify(mockWiFiHandoverTimer, times(1)).cancel();
    }

    @Test
    public void setRssiRoveInTest() {
        // When
        mConditionWiFi.setRssiRoveIn(-100);

        // Then
        // do nothing
    }

    @Test
    public void setRssiRoveOutTest() {
        // When
        mConditionWiFi.setRssiRoveOut(-100);

        // Then
        // do nothing
    }

    @Test
    public void isRoamingTest() {
        // Given
        when(mockLocationManager.getCountryCode()).thenReturn("Test-Country-different");

        // When
        boolean testisRoaming_different = mConditionWiFi.isRoaming();

        // Then
        assertTrue(testisRoaming_different);

        // Given
        mHomeCountry = "Test-Country";

        when(mockLocationManager.getCountryCode()).thenReturn("");
        when(mockConfigurationManager.getUnknownLocationAsRoaming()).thenReturn(true);

        // When
        boolean testisRoaming_true = mConditionWiFi.isRoaming();

        // Then
        assertTrue(testisRoaming_true);
    }

    @Test
    public void requestNetworkValidationTest() {
        // When
        mConditionWiFi.requestNetworkValidation();

        // Then
        verify(mockConnectivityManager, times(1)).reportNetworkConnectivity(mockNetwork, false);

        // Given

        // When Trigger Exception
        mConditionWiFi.requestNetworkValidation();

        // Then
        verify(mockConnectivityManager, times(2)).reportNetworkConnectivity(mockNetwork, false);
    }

    @Test
    public void IpReachableTimerTaskTest() throws UnknownHostException {
        // Given
        mNetwork = new Network(123);
        mConditionWiFi.setNetwork(mNetwork);

        when(mockConfigurationManager.getPingInterval()).thenReturn(50);
        when(mockConfigurationManager.getPingCount()).thenReturn(5);
        when(mockConfigurationManager.getPingPacketSize()).thenReturn(5);
        when(mockConfigurationManager.getPingMaxRtt()).thenReturn(1);

        LinkProperties mockLinkProperties = mock(LinkProperties.class);
        List<LinkAddress> linkAddressList = new ArrayList<>();

        InetAddress ipv6address1 = InetAddress.getByName("fc01:bbbb:cdcd:efe1::1");
        InetAddress ipv6address2 = InetAddress.getByName("fc01:bbbb:cdcd:efe1::1");

        linkAddressList.add(new LinkAddress(ipv6address1, 0));
        linkAddressList.add(new LinkAddress(ipv6address2, 0));
        when(mockLinkProperties.getLinkAddresses()).thenReturn(linkAddressList);
        mLinkProperties = mockLinkProperties;
        mConditionWiFi.setLinkProperties(mLinkProperties);

        mIpReachableList = InetAddress.getAllByName("127.0.0.1");
        mConditionWiFi.setIpReachableList(mIpReachableList);

        mIpReachableTimerTask = mConditionWiFi.new IpReachableTimerTask();

        // When
        mIpReachableTimerTask.run();

        // Then
        verify(mockChangedCallback, times(0)).onConditionChanged(ApnSetting.TYPE_IMS, EVENT_DESTINATION_UNREACHABLE);

        // Given
        // IPv6 test
        mIpReachableList = InetAddress.getAllByName("fc01:bbbb:aadd:efe1::1");
        mConditionWiFi.setIpReachableList(mIpReachableList);

        //When
        mIpReachableTimerTask.run();

        // Then
        verify(mockChangedCallback, times(1)).onConditionChanged(ApnSetting.TYPE_IMS, EVENT_DESTINATION_UNREACHABLE);
    }

    @Test
    public void WiFiNetworkCallbackTest() throws UnknownHostException {
        // Given
        Network network = new Network(123);
        mWiFiNetworkCallback = new ConditionWiFi(0, mockContext, mockConfigurationManager,
                mockConditionHelper, mockChangedCallback).new WiFiNetworkCallback();

        // When
        mWiFiNetworkCallback.onAvailable(network);

        // Then
        verify(mockChangedCallback, times(1)).onConditionChanged(EVENT_WIFI_CONNECTED);

        // When
        mWiFiNetworkCallback.onLost(network);

        // Then
        verify(mockChangedCallback, times(1)).onConditionChanged(EVENT_WIFI_DISCONNECTED);

        // Given
        NetworkCapabilities mockNetworkCapabilities = mock(NetworkCapabilities.class);
        when(mockNetworkCapabilities.hasCapability(NetworkCapabilities.NET_CAPABILITY_VALIDATED)).thenReturn(true);

        // When
        mWiFiNetworkCallback.onCapabilitiesChanged(network, mockNetworkCapabilities);

        // Then
        verify(mockChangedCallback, times(1)).onConditionChanged(EVENT_WIFI_CAPABILITIES_CHANGED);

        // Given
        LinkProperties mockLinkProperties = mock(LinkProperties.class);
        List<LinkAddress> linkAddressList = new ArrayList<>();

        InetAddress ipv4address1 = InetAddress.getByName("30.40.246.108");
        InetAddress ipv4address2 = InetAddress.getByName("30.40.246.108");

        linkAddressList.add(new LinkAddress(ipv4address1, 0));
        linkAddressList.add(new LinkAddress(ipv4address2, 0));
        when(mockLinkProperties.getLinkAddresses()).thenReturn(linkAddressList);
        mLinkProperties = mockLinkProperties;
        mConditionWiFi.setLinkProperties(mLinkProperties);

        LinkProperties mLinkProperties_for_mWiFiNetworkCallback = new LinkProperties();
        List<LinkAddress> linkAddressList_for_mWiFiNetworkCallback = new ArrayList<>();

        InetAddress ipv4address1_for_mWiFiNetworkCallback = InetAddress.getByName("100.60.246.125");
        InetAddress ipv4address2_for_mWiFiNetworkCallback = InetAddress.getByName("100.60.246.125");

        linkAddressList_for_mWiFiNetworkCallback.add(new LinkAddress(ipv4address1_for_mWiFiNetworkCallback, 0));
        linkAddressList_for_mWiFiNetworkCallback.add(new LinkAddress(ipv4address2_for_mWiFiNetworkCallback, 9));
        mLinkProperties_for_mWiFiNetworkCallback.setLinkAddresses(linkAddressList_for_mWiFiNetworkCallback);

        // When
        mWiFiNetworkCallback.onLinkPropertiesChanged(network, mLinkProperties_for_mWiFiNetworkCallback);

        // Then
        verify(mockChangedCallback, times(1)).onConditionChanged(EVENT_WIFI_CAPABILITIES_CHANGED);
    }

    @Test
    public void toStringTest() {
        mConditionWiFi.toString();
    }

    @Test
    public void wifiStrengthTimerTaskTest() {
        // Given
        ConditionWiFi.WiFiStrengthTimerTask wifiStrengthTimerTask = mConditionWiFi. new WiFiStrengthTimerTask();
        wifiStrengthTimerTask.run();

        // When
        wifiStrengthTimerTask.run();

        // Then
        verify(mockChangedCallback, times(2)).onConditionChanged(EVENT_WIFI_SIGNAL_STRENGTH_CHANGED);
    }

    @Test
    public void testWifiHandoverTimerTask() {
        // Given
        ConditionWiFi.WiFiHandoverTimerTask wiFiHandoverTimerTask =
                mConditionWiFi.new WiFiHandoverTimerTask();

        // When
        wiFiHandoverTimerTask.run();

        // Then
        verify(mockChangedCallback, times(1)).onConditionChanged(ApnSetting.TYPE_IMS,
                EVENT_WIFI_AP_HANDOVER_TIMER_EXPIRED);
    }

    @Test
    public void testWiFiBroadcastReceiverNull() {
        // Given
        ConditionWiFi.WiFiBroadcastReceiver receiver = mConditionWiFi.getBroadcastReceiver();

        // When
        when(mockIntent.getAction()).thenReturn(null);
        receiver.onReceive(mockContext, mockIntent);

        // Then
        assertTrue(true);
    }

    @Test
    public void testWiFiBroadcastReceiverRssiChanged() {
        // Given
        ConditionWiFi.WiFiBroadcastReceiver receiver = mConditionWiFi.getBroadcastReceiver();

        // When
        mConditionWiFi.setNetwork(mockNetwork);
        when(mockIntent.getAction()).thenReturn(WifiManager.RSSI_CHANGED_ACTION);
        when(mockIntent.getIntExtra(WifiManager.EXTRA_NEW_RSSI, 0)).thenReturn(-80);
        receiver.onReceive(mockContext, mockIntent);

        // Then
        verify(mockChangedCallback, times(1)).onConditionChanged(EVENT_WIFI_SIGNAL_STRENGTH_CHANGED);
    }

    @Test
    public void testWiFiBroadcastReceiverWifiStateChanged() {
        // Given
        ConditionWiFi.WiFiBroadcastReceiver receiver = mConditionWiFi.getBroadcastReceiver();

        // When
        when(mockIntent.getAction()).thenReturn(WifiManager.WIFI_STATE_CHANGED_ACTION);
        when(mockIntent.getIntExtra(WifiManager.EXTRA_WIFI_STATE, WifiManager.WIFI_STATE_UNKNOWN)).thenReturn(WifiManager.WIFI_STATE_ENABLED);
        receiver.onReceive(mockContext, mockIntent);

        // Then
        verify(mockChangedCallback, times(1)).onConditionChanged(EVENT_WIFI_BUTTON_STATE_CHANGED);
    }

    @Test
    public void testWiFiBroadcastReceiverAirplaneModeChanged() {
        // Given
        ConditionWiFi.WiFiBroadcastReceiver receiver = mConditionWiFi.getBroadcastReceiver();

        // When
        when(mockIntent.getAction()).thenReturn(Intent.ACTION_AIRPLANE_MODE_CHANGED);
        when(mockIntent.getIntExtra(WifiManager.EXTRA_WIFI_STATE, WifiManager.WIFI_STATE_UNKNOWN)).thenReturn(WifiManager.WIFI_STATE_ENABLED);
        receiver.onReceive(mockContext, mockIntent);

        // Then
        assertTrue(true);
    }

    @Test
    public void testDefaultNetworkCallbackOnLost() {
        // Given
        ConditionWiFi.DefaultNetworkCallback defaultNetworkCallback =
                mConditionWiFi.new DefaultNetworkCallback();

        // When
        defaultNetworkCallback.onLost(mockNetwork);

        // Then
        assertFalse(mConditionWiFi.getDefault());
    }

    @Test
    public void testDefaultNetworkCallbackOnLinkPropertiesChanged() {
        // Given
        ConditionWiFi.DefaultNetworkCallback defaultNetworkCallback =
                mConditionWiFi.new DefaultNetworkCallback();
        LinkProperties mockLinkProperties = mock(LinkProperties.class);

        // When
        defaultNetworkCallback.onLinkPropertiesChanged(mockNetwork, mockLinkProperties);

        // Then
        verify(mockChangedCallback, times(1)).onConditionChanged(EVENT_DEFAULT_LINK_PROPERTIES_CHANGED);
    }

    @Test
    public void testWlanPsStateListener() {
        // Given
        ConditionWiFi.WlanPsStateListener wlanPsStateListener =
                mConditionWiFi.new WlanPsStateListener();
        ServiceState mockServiceState = mock(ServiceState.class);
        NetworkRegistrationInfo mockNetworkRegistrationInfo = mock(NetworkRegistrationInfo.class);

        // When
        when(mockServiceState.getNetworkRegistrationInfo(NetworkRegistrationInfo.DOMAIN_PS,
                AccessNetworkConstants.TRANSPORT_TYPE_WLAN)).thenReturn(mockNetworkRegistrationInfo);
        wlanPsStateListener.onServiceStateChanged(mockServiceState);

        // Then
        verify(mockChangedCallback, times(1)).onConditionChanged(EVENT_WLAN_PS_STATE_CHANGED);

        /* Exception */
        // When
        wlanPsStateListener.onServiceStateChanged(null);

        // Then
        assertTrue(true);
    }

    @Test
    public void testIsAirplaneModeTurnedOn() {
        // Given
        ConditionWiFi.WiFiNetworkCallback wiFiNetworkCallback = mConditionWiFi.new WiFiNetworkCallback();

        // When
        mRssiOut.put(MAP_DEFAULT, -110);
        mConditionWiFi.setRssiOut(mRssiOut);
        when(mockConfigurationManager.isWiFiApHandoverSupported()).thenReturn(true);
        wiFiNetworkCallback.onLost(mockNetwork);
    }

    @Test
    public void testIsAirplaneModeTurnedOnByForce() {
        // Given
        ConditionWiFi.WiFiNetworkCallback wiFiNetworkCallback =
                mConditionWiFi.new WiFiNetworkCallback();

        // When
        mRssiOut.put(MAP_DEFAULT, -110);
        mConditionWiFi.setRssiOut(mRssiOut);
        when(mockConfigurationManager.isWiFiApHandoverSupported()).thenReturn(true);
        mConditionWiFi.setAirplaneModeByTurning(true);
        wiFiNetworkCallback.onLost(mockNetwork);
    }

    @Test
    public void testStartWiFiApHandoverTimer_whenIsWiFiTransportNotUsed() {
        // Given
        ConditionWiFi.WiFiNetworkCallback wiFiNetworkCallback =
                mConditionWiFi.new WiFiNetworkCallback();

        // When
        when(mockConfigurationManager.isWiFiApHandoverSupported()).thenReturn(true);
        mConditionWiFi.setWiFiState(WifiManager.WIFI_STATE_ENABLED);
        when(mockConditionHelper.isWiFiTransportNotUsed()).thenReturn(true);
        wiFiNetworkCallback.onLost(mockNetwork);
    }

    @Test
    public void testStartWiFiApHandoverTimer_whenIsWiFiTransportUsed() {
        // Given
        ConditionWiFi.WiFiNetworkCallback wiFiNetworkCallback =
                mConditionWiFi.new WiFiNetworkCallback();

        // When
        when(mockConfigurationManager.isWiFiApHandoverSupported()).thenReturn(true);
        mConditionWiFi.setWiFiState(WifiManager.WIFI_STATE_ENABLED);
        when(mockConditionHelper.isWiFiTransportNotUsed()).thenReturn(false);
        wiFiNetworkCallback.onLost(mockNetwork);
    }

    @Test
    public void testStartWiFiApHandoverTimer_whenIsWiFiTransportUsedAndWiFiTimerIsRunning() {
        // Given
        ConditionWiFi.WiFiNetworkCallback wiFiNetworkCallback =
                mConditionWiFi.new WiFiNetworkCallback();

        // When
        mConditionWiFi.setWiFiHandoverTimer(mockWiFiHandoverTimer);
        when(mockConfigurationManager.isWiFiApHandoverSupported()).thenReturn(true);
        mConditionWiFi.setWiFiState(WifiManager.WIFI_STATE_ENABLED);
        when(mockConditionHelper.isWiFiTransportNotUsed()).thenReturn(false);

        wiFiNetworkCallback.onLost(mockNetwork);
    }

    @Test
    public void testStartWiFiApHandoverTimer_whenIsWiFiTransportUsedAndTimeoutConfigured()
    {
        // Given
        ConditionWiFi.WiFiNetworkCallback wiFiNetworkCallback =
                mConditionWiFi.new WiFiNetworkCallback();
        ConditionWiFi.WiFiHandoverTimerTask wiFiHandoverTimerTask =
                mConditionWiFi.new WiFiHandoverTimerTask();

        // When
        when(mockConfigurationManager.isWiFiApHandoverSupported()).thenReturn(true);
        mConditionWiFi.setWiFiState(WifiManager.WIFI_STATE_ENABLED);
        when(mockConditionHelper.isWiFiTransportNotUsed()).thenReturn(false);
        when(mockConfigurationManager.getWiFiApHandoverTimeout()).thenReturn(100);

        wiFiNetworkCallback.onLost(mockNetwork);
    }

    @Test
    public void testMethodRequestNetworkValidation() {
        // When
        doThrow(new NullPointerException()).when(mockConnectivityManager).
                reportNetworkConnectivity(any(Network.class), eq(false));

        mConditionWiFi.requestNetworkValidation();
    }

    @Test
    public void testStartIpReachableTimer() {
        mConditionWiFi.setNetwork(null);
        mConditionWiFi.startIpReachableTimer();
    }

    @Test
    public void testStartIpReachableTimer_ExceptionalCase_when_destination_is_null() {
        mConditionWiFi.setNetwork(null);
        mConditionWiFi.startIpReachableTimer();

        when(mockConfigurationManager.getPingDestination()).thenReturn("");
    }
}
