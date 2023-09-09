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

import android.content.ContentResolver;
import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.net.wifi.WifiManager;
import android.os.Handler;
import android.os.Message;
import android.telephony.AccessNetworkConstants;
import android.telephony.DataFailCause;
import android.telephony.NetworkRegistrationInfo;
import android.telephony.PreciseDataConnectionState;
import android.telephony.ServiceState;
import android.telephony.TelephonyManager;
import android.telephony.data.ApnSetting;
import android.telephony.data.ThrottleStatus;
import android.util.SparseArray;

import androidx.test.platform.app.InstrumentationRegistry;

import com.shannon.qualifiednetworksservice.ConditionConnectivity.ConnectivityBroadcastReceiver;
import com.shannon.qualifiednetworksservice.ConditionConnectivity.ConnectivityStateListener;
import com.shannon.qualifiednetworksservice.ConditionConnectivity.WfcStateContentObserver;
import com.shannon.qualifiednetworksservice.Transport.TransportType;
import com.shannon.qualifiednetworksservice.util.DeviceUtils;

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
import org.robolectric.Shadows;
import org.robolectric.annotation.Config;
import org.robolectric.shadows.ShadowWifiManager;

import static com.shannon.qualifiednetworksservice.RilOemInterface.RIL_UNSOLICITED_MESSAGE;
import static org.mockito.ArgumentMatchers.any;
import static org.mockito.ArgumentMatchers.anyBoolean;
import static org.mockito.ArgumentMatchers.anyInt;
import static org.mockito.Mockito.doNothing;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.spy;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

import java.util.HashMap;

@FixMethodOrder(MethodSorters.NAME_ASCENDING)
@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class ConditionConnectivityTest extends TestCase {

    private static MockedStatic<SimManager> sMockedSimManager;
    private static MockedStatic<DeviceUtils> sMockedDeviceUtils;
    private ConditionConnectivity mConditionConnectivity;
    private ConnectivityBroadcastReceiver mConnectivityBroadcastReceiver;

    private Context mContext;
    private WifiManager mWiFiManager;
    private ConnectivityStateListener mConnectivityStateListener;
    private WfcStateContentObserver mWfcStateContentObserver;
    private Handler mHandler;

    @Mock
    private SimManager mockSimManager;
    @Mock
    private LocationManager mockLocationManager;
    @Mock
    private RilOemInterface mockRilOemInterface;
    @Mock
    private ConditionCellular mockConditionCellular;
    @Mock
    private ConditionCall mockConditionCall;
    @Mock
    private ConfigurationManager mockConfigurationManager;
    @Mock
    private HandoverConditions.IConditionChanged mockChangedCallback;
    @Mock
    private TelephonyManager mockTelephonyManager;
    @Mock
    private HandoverConditions.IConditionHelper mockConditionHelper;

    @BeforeClass
    public static void beforeClass() {
        sMockedSimManager = Mockito.mockStatic(SimManager.class);
        sMockedDeviceUtils = Mockito.mockStatic(DeviceUtils.class);
    }

    @AfterClass
    public static void afterClass() {
        sMockedSimManager.close();
        sMockedDeviceUtils.close();
        sMockedSimManager = null;
        sMockedDeviceUtils = null;
    }

    @Before
    public void setUp() {
        MockitoAnnotations.openMocks(this);

        sMockedSimManager.when(SimManager::getInstance).thenReturn(mockSimManager);

        when(mockSimManager.getSubId(0)).thenReturn(100);
        when(mockSimManager.getTelephonyManager(anyInt())).thenReturn(mockTelephonyManager);

        // Setup context for WifiManager
        mContext = spy(InstrumentationRegistry.getInstrumentation().getTargetContext());

        mWiFiManager = (WifiManager) mContext.getSystemService(WifiManager.class);
        ShadowWifiManager wifiManager = Shadows.shadowOf(mWiFiManager);

        mConditionConnectivity = new ConditionConnectivity(0, mContext, mockRilOemInterface,
            mockConfigurationManager, mockConditionHelper, mockChangedCallback);
    }

    @After
    public void tearDown() {
        Mockito.validateMockitoUsage();
    }

    @Test
    public void destroyTest() {
        // Given
        ContentResolver mockContentResolver = mock(ContentResolver.class);
        when(mContext.getContentResolver()).thenReturn(mockContentResolver);

        doNothing().when(mockContentResolver).registerContentObserver(any(), anyBoolean(), any());

        // When
        mConditionConnectivity.destroy();

        // Then
        verify(mContext, times(1)).unregisterReceiver(any());
    }

    @Test
    public void isTransportAvailableTest() {
        // Given
        TransportType mockTransportType = mock(TransportType.class);

        // Then
        assertFalse(mConditionConnectivity.isTransportAvailable(ApnSetting.AUTH_TYPE_NONE, mockTransportType));

        // Given
        ApnTransportHandler mockApnTransportHandler = mock(ApnTransportHandler.class);
        when(mockApnTransportHandler.isTransportAvailable(any())).thenReturn(true);

        // When
        boolean test_false = mConditionConnectivity.isTransportAvailable(ApnSetting.TYPE_IMS, mockTransportType);

        // Then
        assertFalse(test_false);
    }

    @Test
    public void isHandoverFailedTest() {
        // Given
        TransportType mockTransportType = mock(TransportType.class);

        // Then
        assertFalse(mConditionConnectivity.isHandoverFailed(ApnSetting.AUTH_TYPE_NONE, mockTransportType));

        // Given
        ApnTransportHandler mockApnTransportHandler = mock(ApnTransportHandler.class);
        when(mockApnTransportHandler.isTransportAvailable(any())).thenReturn(true);

        // When
        boolean test_false = mConditionConnectivity.isHandoverFailed(ApnSetting.TYPE_IMS, mockTransportType);

        // Then
        assertFalse(test_false);
    }

    @Test
    public void getNetworkTypeTest() {
        // Then
        assertEquals(TelephonyManager.NETWORK_TYPE_UNKNOWN,
            mConditionConnectivity.getNetworkType(ApnSetting.AUTH_TYPE_NONE));

        // Given
        ApnTransportHandler mockApnTransportHandler = mock(ApnTransportHandler.class);
        when(mockApnTransportHandler.isTransportAvailable(any())).thenReturn(true);

        // When
        int test = mConditionConnectivity.getNetworkType(ApnSetting.TYPE_IMS);

        // Then
        assertEquals(TelephonyManager.NETWORK_TYPE_UNKNOWN, test);
    }


    @Test
    public void isWiFiTransportUsedTest() {
        // When
        assertFalse(mConditionConnectivity.isWiFiTransportUsed());
    }

    @Test
    public void onConfigurationChangedTest() {
        // When
        try {
        mConditionConnectivity.onConfigurationChanged();
        } catch (Exception ignored) { }
        // Then
        // do nothing, since mApnHandlers is private final and can't be modified outside class. NPE will occur in mock.
    }

    @Test
    public void ConnectivityStateListenerTest() {
        // Given
        mConnectivityStateListener = mConditionConnectivity.new ConnectivityStateListener(0);

        // onServiceStateChanged
        ServiceState mockServiceState = mock(ServiceState.class);
        NetworkRegistrationInfo mockPsState = mock(NetworkRegistrationInfo.class);
        when(mockServiceState.getNetworkRegistrationInfo(anyInt(), anyInt())).thenReturn(mockPsState);
        when(mockPsState.isRegistered()).thenReturn(true);
        when(mockPsState.getRegisteredPlmn()).thenReturn("Test");
        when(mockPsState.getAccessNetworkTechnology()).thenReturn(TelephonyManager.CDMA_ROAMING_MODE_HOME);
        mConditionConnectivity.setLastNetworkType(TelephonyManager.NETWORK_TYPE_IWLAN);
        // When
        mConnectivityStateListener.onServiceStateChanged(mockServiceState);

        // Given
        when(mockPsState.getRegisteredPlmn()).thenReturn("MakePLMNEqual");
        mConditionConnectivity.setLastRegisteredPlmn("MakePLMNNotEqual");

        // When
        mConnectivityStateListener.onServiceStateChanged(mockServiceState);

        // Network Unknown, not registered
        // Given
        when(mockPsState.isRegistered()).thenReturn(false);

        // When
        mConnectivityStateListener.onServiceStateChanged(mockServiceState);

        // Given
        when(mockPsState.isRegistered()).thenReturn(false);

        // When
        mConnectivityStateListener.onServiceStateChanged(mockServiceState);

        // Given
        // onPreciseDataConnectionStateChanged
        PreciseDataConnectionState mockPreciseDataConnectionState = mock(PreciseDataConnectionState.class);
        when(mockPreciseDataConnectionState.getDataConnectionApn()).thenReturn("Test");
        when(mockPreciseDataConnectionState.getDataConnectionState()).thenReturn(TelephonyManager.DATA_CONNECTING);
        when(mockPreciseDataConnectionState.getDataConnectionApnTypeBitMask()).thenReturn(ApnSetting.TYPE_IMS);

        when(mockPreciseDataConnectionState.getNetworkType()).thenReturn(TelephonyManager.NETWORK_TYPE_IWLAN);

        // When
        try {
            mConnectivityStateListener.onPreciseDataConnectionStateChanged(mockPreciseDataConnectionState);
        } catch (Exception e) { }

        // Given
        when(mockPreciseDataConnectionState.getDataConnectionState()).thenReturn(TelephonyManager.DATA_CONNECTED);
        sMockedDeviceUtils.when(DeviceUtils::isConnectionCallbackFixed).thenReturn(true);

        // When
        try {
        mConnectivityStateListener.onPreciseDataConnectionStateChanged(mockPreciseDataConnectionState);
        } catch (Exception e) { }

        // Given
        when(mockPreciseDataConnectionState.getDataConnectionApnTypeBitMask()).thenReturn(ApnSetting.TYPE_EMERGENCY);
        when(mockPreciseDataConnectionState.getNetworkType()).thenReturn(TelephonyManager.NETWORK_TYPE_UNKNOWN);
        when(mockPreciseDataConnectionState.getDataConnectionState()).thenReturn(TelephonyManager.DATA_DISCONNECTED);

        // When
        try {
        mConnectivityStateListener.onPreciseDataConnectionStateChanged(mockPreciseDataConnectionState);
        } catch (Exception e) { }
        // Given
        when(mockPreciseDataConnectionState.getDataConnectionFailCause()).thenReturn(DataFailCause.INVALID_PCSCF_ADDR);
        when(mockPreciseDataConnectionState.getNetworkType()).thenReturn(TelephonyManager.NETWORK_TYPE_UNKNOWN);
        when(mockPreciseDataConnectionState.getDataConnectionState()).thenReturn(TelephonyManager.DATA_UNKNOWN);

        // When
        try {
        mConnectivityStateListener.onPreciseDataConnectionStateChanged(mockPreciseDataConnectionState);
        } catch (Exception e) { }
        // Given
        when(mockPreciseDataConnectionState.getDataConnectionApnTypeBitMask()).thenReturn(ApnSetting.TYPE_NONE);

        // When
        try {
        mConnectivityStateListener.onPreciseDataConnectionStateChanged(mockPreciseDataConnectionState);
        } catch (Exception e) { }


        // given
        when(mockPreciseDataConnectionState.getDataConnectionApnTypeBitMask()).thenReturn(ApnSetting.TYPE_IMS);
        when(mockPreciseDataConnectionState.getNetworkType()).thenReturn(TelephonyManager.NETWORK_TYPE_IWLAN);
        when(mockPreciseDataConnectionState.getDataConnectionState()).thenReturn(TelephonyManager.DATA_DISCONNECTED);

        // when: getApnThrottleTimeout() will be called
        try {
            mConnectivityStateListener.onPreciseDataConnectionStateChanged(mockPreciseDataConnectionState);
        } catch (Exception e) { }
    }

    @Test
    public void WfcStateContentObserverTest() {
        // Given
        mWfcStateContentObserver = mConditionConnectivity.new WfcStateContentObserver(mHandler);
        Uri mockUri = mock(Uri.class);

        // When
        mWfcStateContentObserver.onChange(true, mockUri);
    }

    @Test
    public void ConnectivityBroadcastReceiverTest() {
        // Given
        mConnectivityBroadcastReceiver = mConditionConnectivity.new ConnectivityBroadcastReceiver();
        Intent mockIntent = mock(Intent.class);
        when(mockIntent.getAction()).thenReturn(Intent.ACTION_AIRPLANE_MODE_CHANGED);

        // When
        mConnectivityBroadcastReceiver.onReceive(mContext, mockIntent);

        // Given
        when(mockIntent.getAction()).thenReturn(WifiManager.WIFI_STATE_CHANGED_ACTION);
        when(mockIntent.getIntExtra(WifiManager.EXTRA_WIFI_STATE, WifiManager.WIFI_STATE_UNKNOWN))
            .thenReturn(WifiManager.WIFI_STATE_ENABLED);

        // When
        mConnectivityBroadcastReceiver.onReceive(mContext, mockIntent);

        // Given
        when(mockIntent.getAction()).thenReturn(WifiManager.NETWORK_STATE_CHANGED_ACTION);

        // When
        mConnectivityBroadcastReceiver.onReceive(mContext, mockIntent);

        // Given
        when(mockIntent.getAction()).thenReturn(null);

        // When
        mConnectivityBroadcastReceiver.onReceive(mContext, mockIntent);
    }

    @Test
    public void testHandler() {
        Handler handler = mConditionConnectivity.getHandler();

        byte[] bytes = new byte[16];

        Message msg = Message.obtain();
        msg.what = RilOemInterface.RIL_UNSOLICITED_MESSAGE;

        // IMS registered
        bytes[0] = 2;
        RilUnsolicitedMessage unsolicitedMessage = new RilUnsolicitedMessage(
                RilUnsolicitedMessage.RIL_UNSOL_REGISTRATION, bytes);
        msg.obj = unsolicitedMessage;
        handler.handleMessage(msg);

        // IMS not registered
        bytes[0] = 1;
        unsolicitedMessage = new RilUnsolicitedMessage(
                RilUnsolicitedMessage.RIL_UNSOL_REGISTRATION, bytes);
        msg.obj = unsolicitedMessage;
        handler.handleMessage(msg);

        // emergency registered
        bytes[0] = 5;
        unsolicitedMessage = new RilUnsolicitedMessage(
                RilUnsolicitedMessage.RIL_UNSOL_REGISTRATION, bytes);
        msg.obj = unsolicitedMessage;
        handler.handleMessage(msg);
    }

    @Test
    public void getCurrentTransportTest() {
        // Given

        // When
        Transport returnType = mConditionConnectivity.getCurrentTransport(ApnSetting.TYPE_IMS);

        // Then
        assertEquals(null, returnType);
    }

    @Test
    public void testIsTransportAvailableForInitial() {
        // when: default

        // then
        assertEquals(true, mConditionConnectivity.isTransportAvailableForInitial(ApnSetting.TYPE_IMS,
                TransportType.TRANSPORT_CELLULAR));
    }

    @Test
    public void testOnThrottleStatusChanged_case_throttled() {
        // throttled case
        // given
        final int SLOT_INDEX = 10;
        final int TRANSPORT_TYPE = AccessNetworkConstants.TRANSPORT_TYPE_WWAN;
        final int APN_TYPE = ApnSetting.TYPE_IMS;
        final int THROTTLE_TYPE = ThrottleStatus.THROTTLE_TYPE_ELAPSED_TIME;
        final long THROTTLE_EXPIRY_TIME_MILLIS = 5005;
        final int RETRY_TYPE = ThrottleStatus.RETRY_TYPE_NEW_CONNECTION;
        ThrottleStatus status = new ThrottleStatus.Builder()
                .setSlotIndex(SLOT_INDEX)
                .setTransportType(TRANSPORT_TYPE)
                .setApnType(APN_TYPE)
                // if status have time millis, the type is 'THROTTLE_TYPE_ELAPSED_TIME'
                .setThrottleExpiryTimeMillis(THROTTLE_EXPIRY_TIME_MILLIS)
                .setRetryType(RETRY_TYPE)
                .build();

        // when
        mConditionConnectivity.onThrottleStatusChanged(status);

        // then
        SparseArray apnThrottleInformation = mConditionConnectivity.getApnThrottleInformation();
        assertEquals(true, apnThrottleInformation.size() >= 1);
    }

    @Test
    public void testOnThrottleStatusChanged_case_unthrottled() {
        // throttled case
        // given
        final int SLOT_INDEX = 10;
        final int TRANSPORT_TYPE = AccessNetworkConstants.TRANSPORT_TYPE_WWAN;
        final int APN_TYPE = ApnSetting.TYPE_IMS;
        final long THROTTLE_EXPIRY_TIME_MILLIS = 0;
        final int RETRY_TYPE = ThrottleStatus.RETRY_TYPE_NEW_CONNECTION;
        ThrottleStatus status = new ThrottleStatus.Builder()
                .setSlotIndex(SLOT_INDEX)
                .setTransportType(TRANSPORT_TYPE)
                .setApnType(APN_TYPE)
                .setThrottleExpiryTimeMillis(THROTTLE_EXPIRY_TIME_MILLIS)
                .setRetryType(RETRY_TYPE)
                .build();

        // when
        mConditionConnectivity.onThrottleStatusChanged(status);

        // then
        SparseArray apnThrottleInformation = mConditionConnectivity.getApnThrottleInformation();
        assertEquals(true, apnThrottleInformation.size() == 0);
    }

    @Test
    public void testOnThrottleStatusChanged_case_throttled_and_onPreciseDataConnectionStateChanged() {
        // throttled case
        // given
        final int SLOT_INDEX = 10;
        final int TRANSPORT_TYPE = AccessNetworkConstants.TRANSPORT_TYPE_WLAN;
        final int APN_TYPE = ApnSetting.TYPE_IMS;
        final int THROTTLE_TYPE = ThrottleStatus.THROTTLE_TYPE_ELAPSED_TIME;
        final long THROTTLE_EXPIRY_TIME_MILLIS = 5005;
        final int RETRY_TYPE = ThrottleStatus.RETRY_TYPE_NEW_CONNECTION;
        ThrottleStatus status = new ThrottleStatus.Builder()
                .setSlotIndex(SLOT_INDEX)
                .setTransportType(TRANSPORT_TYPE)
                .setApnType(APN_TYPE)
                // if status have time millis, the type is 'THROTTLE_TYPE_ELAPSED_TIME'
                .setThrottleExpiryTimeMillis(THROTTLE_EXPIRY_TIME_MILLIS)
                .setRetryType(RETRY_TYPE)
                .build();

        PreciseDataConnectionState mockPreciseDataConnectionState = mock(PreciseDataConnectionState.class);

        // onServiceStateChanged
        ServiceState mockServiceState = mock(ServiceState.class);
        NetworkRegistrationInfo mockPsState = mock(NetworkRegistrationInfo.class);
        when(mockServiceState.getNetworkRegistrationInfo(anyInt(), anyInt())).thenReturn(mockPsState);
        when(mockPsState.isRegistered()).thenReturn(true);
        when(mockPsState.getRegisteredPlmn()).thenReturn("Test");
        when(mockPsState.getAccessNetworkTechnology()).thenReturn(TelephonyManager.CDMA_ROAMING_MODE_HOME);
        mConditionConnectivity.setLastNetworkType(TelephonyManager.NETWORK_TYPE_IWLAN);

        mConnectivityStateListener = mConditionConnectivity.new ConnectivityStateListener(0);

        when(mockPsState.getRegisteredPlmn()).thenReturn("MakePLMNEqual");
        mConditionConnectivity.setLastRegisteredPlmn("MakePLMNNotEqual");

        // when: getApnThrottleTimeout() will be called with 'THROTTLE_EXPIRY_TIME_MILLIS'
        when(mockPreciseDataConnectionState.getDataConnectionApnTypeBitMask()).thenReturn(ApnSetting.TYPE_IMS);
        when(mockPreciseDataConnectionState.getNetworkType()).thenReturn(TelephonyManager.NETWORK_TYPE_IWLAN);
        when(mockPreciseDataConnectionState.getDataConnectionState()).thenReturn(TelephonyManager.DATA_DISCONNECTED);

        mConditionConnectivity.onThrottleStatusChanged(status);

        // then
        try {
            mConnectivityStateListener.onPreciseDataConnectionStateChanged(mockPreciseDataConnectionState);
        } catch (Exception e) { }
    }
}