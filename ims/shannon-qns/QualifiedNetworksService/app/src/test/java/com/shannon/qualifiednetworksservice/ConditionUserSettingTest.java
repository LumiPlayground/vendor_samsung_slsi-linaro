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
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
import androidx.test.platform.app.InstrumentationRegistry;
import com.android.ims.ImsManager;
import com.shannon.qualifiednetworksservice.ConditionUserSetting.UserSettingBroadcastReceiver;
import com.shannon.qualifiednetworksservice.ConditionUserSetting.UserSettingContentObserver;
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
import org.mockito.MockedStatic.Verification;
import org.mockito.Mockito;
import org.mockito.MockitoAnnotations;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.Shadows;
import org.robolectric.annotation.Config;
import org.robolectric.shadows.ShadowWifiManager;

import static android.provider.Telephony.SimInfo.COLUMN_CROSS_SIM_CALLING_ENABLED;
import static android.telephony.ims.ImsMmTelManager.WIFI_MODE_CELLULAR_PREFERRED;
import static android.telephony.ims.ImsMmTelManager.WIFI_MODE_WIFI_ONLY;
import static android.telephony.ims.ImsMmTelManager.WIFI_MODE_WIFI_PREFERRED;
import static com.shannon.qualifiednetworksservice.HandoverConditions.ConditionEvents.EVENT_CARRIER_CONFIG_CHANGED;
import static com.shannon.qualifiednetworksservice.HandoverConditions.ConditionEvents.EVENT_CST_SETTINGS_CHANGED;
import static com.shannon.qualifiednetworksservice.HandoverConditions.ConditionEvents.EVENT_VLT_SETTINGS_CHANGED;
import static com.shannon.qualifiednetworksservice.HandoverConditions.ConditionEvents.EVENT_WFC_MODE_CHANGED;
import static com.shannon.qualifiednetworksservice.HandoverConditions.ConditionEvents.EVENT_WFC_SETTINGS_BROADCAST;
import static com.shannon.qualifiednetworksservice.HandoverConditions.ConditionEvents.EVENT_WFC_SETTINGS_CHANGED;
import static org.mockito.ArgumentMatchers.any;
import static org.mockito.ArgumentMatchers.anyBoolean;
import static org.mockito.ArgumentMatchers.anyInt;
import static org.mockito.Mockito.doNothing;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.spy;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

@FixMethodOrder(MethodSorters.NAME_ASCENDING)
@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class ConditionUserSettingTest extends TestCase {
    private ConditionUserSetting mConditionUserSetting;
    private UserSettingBroadcastReceiver mUserSettingBroadcastReceiver;
    private UserSettingContentObserver mUserSettingContentObserver;

    private Context mContext;
    private WifiManager mWiFiManager;

    private static MockedStatic<SimManager> sMockedSimManager;
    private static MockedStatic<ImsManager> sMockedImsManager;

    @Mock
    private SimManager mockSimManager;
    @Mock
    private ImsManager mockImsManager;
    @Mock
    private ConfigurationManager mockConfigurationManager;
    @Mock
    private HandoverConditions.IConditionChanged mockChangedCallback;
    @Mock
    private TelephonyManager mockTelephonyManager;
    @Mock
    private ContentResolver mockContentResolver;
    @Mock
    private HandoverConditions.IConditionHelper mockConditionHelper;

    @BeforeClass
    public static void beforeClass() {
        sMockedSimManager = Mockito.mockStatic(SimManager.class);
        sMockedImsManager = Mockito.mockStatic(ImsManager.class);
    }

    @AfterClass
    public static void afterClass() {
        sMockedSimManager.close();
        sMockedImsManager.close();
        sMockedSimManager = null;
        sMockedImsManager = null;
    }

    @Before
    public void setUp() {
        MockitoAnnotations.openMocks(this);

        sMockedSimManager.when(SimManager::getInstance).thenReturn(mockSimManager);
        when(mockSimManager.getSubId(0)).thenReturn(100);
        when(mockSimManager.getTelephonyManager(anyInt())).thenReturn(mockTelephonyManager);
        when(mockTelephonyManager.isNetworkRoaming()).thenReturn(true);

        // Setup context for WifiManager
        mContext = spy(InstrumentationRegistry.getInstrumentation().getTargetContext());

        mWiFiManager = (WifiManager) mContext.getSystemService(WifiManager.class);
        ShadowWifiManager wifiManager = Shadows.shadowOf(mWiFiManager);

        sMockedImsManager.when((Verification) ImsManager.getInstance(mContext, 0)).thenReturn(mockImsManager);
        when(mockImsManager.isEnhanced4gLteModeSettingEnabledByUser()).thenReturn(true);
        when(mockImsManager.isVolteEnabledByPlatform()).thenReturn(true);
        when(mockImsManager.isWfcEnabledByUser()).thenReturn(true);
        when(mockImsManager.isWfcRoamingEnabledByUser()).thenReturn(true);
        when(mockImsManager.isWfcEnabledByPlatform()).thenReturn(true);
        when(mockImsManager.getWfcMode(anyBoolean())).thenReturn(WIFI_MODE_WIFI_PREFERRED);
        when(mockImsManager.isCrossSimCallingEnabledByUser()).thenReturn(true);

        when(mContext.getContentResolver()).thenReturn(mockContentResolver);
        doNothing().when(mockContentResolver).registerContentObserver(any(), anyBoolean(), any());

        mConditionUserSetting = new ConditionUserSetting(0, mContext, mockConfigurationManager,
                mockConditionHelper, mockChangedCallback);
    }

    @After
    public void tearDown() {
        Mockito.validateMockitoUsage();
    }

    @Test
    public void destroyTest() {
        // When
        mConditionUserSetting.destroy();
        // Then
        verify(mContext, times(1)).unregisterReceiver(any());
    }

    @Test
    public void isWfcEnabledTest() {
        // Given
        mConditionUserSetting.setWfcEnabledByUserOverride(true);
        // When
        boolean test = mConditionUserSetting.isWfcEnabled();
        // Then
        assertTrue(test);
    }

    @Test
    public void isWiFiPreferredTest() {
        // Given
        mConditionUserSetting.setWfcEnabledByUserOverride(true);
        // When
        boolean test = mConditionUserSetting.isWiFiPreferred();
        // Then
        assertTrue(test);
    }

    @Test
    public void isWfcEnabledByPlatformTest() {
        // When
        boolean test = mConditionUserSetting.isWfcEnabledByPlatform();
        // Then
        assertTrue(test);
    }

    @Test
    public void isWfcEnabledByUserTest() {
        // When
        boolean test = mConditionUserSetting.isWfcEnabledByUser();
        // Then
        assertTrue(test);
    }

    @Test
    public void isVolteEnabledTest() {
        // When
        boolean test = mConditionUserSetting.isVolteEnabled();
        // Then
        assertTrue(test);
    }

    @Test
    public void isVoMobileEnabledTest() {
        // Given
        mConditionUserSetting.setVoMobileEnabledByPlatform(true);
        // When
        boolean test = mConditionUserSetting.isVoMobileEnabled();
        // Then
        assertTrue(test);
    }

    @Test
    public void getWfcModeEnumTest() {
        // When
        int test = mConditionUserSetting.getWfcModeEnum();
        // Then
        assertEquals(test, WIFI_MODE_WIFI_PREFERRED);
    }

    @Test
    public void getWfcModeTest() {
        // When
        String test = mConditionUserSetting.getWfcMode(1);
        // Then
        assertEquals(test, "CELLULAR_PREFERRED");
    }

    @Test
    public void getWfcModeTest_WIFI_MODE_IMS_PREFERRED() {
        // When
        String test = mConditionUserSetting.getWfcMode(3);
        // Then
        assertEquals(test, "IMS_PREFERRED");
    }

    @Test
    public void testSetWfcEnabledRoaming() {
        // When
        mConditionUserSetting.setWfcEnabledRoaming(true);
        // Then
        verify(mockChangedCallback, times(0))
                .onConditionChanged(EVENT_WFC_SETTINGS_CHANGED);
    }

    @Test
    public void testSetWfcEnabledRoamingFalse() {
        // When
        mConditionUserSetting.setWfcEnabledRoaming(false);
        // Then
        verify(mockChangedCallback, times(1))
                .onConditionChanged(EVENT_WFC_SETTINGS_CHANGED);
    }

    @Test
    public void isVoMobileApnTypeEnabledTest() {
        // Given
        mConditionUserSetting.setVoMobileEnabledByPlatform(true);
        // When
        boolean test = mConditionUserSetting.isVoMobileApnTypeEnabled(0);
        // Then
        assertFalse(test);
    }

    @Test
    public void toStringTest() {
        mConditionUserSetting.toString();
    }

    @Test
    public void ConnectivityStateListenerTest() {
        // Given
        Intent mockIntent = mock(Intent.class);
        when(mockIntent.getAction()).thenReturn(android.telephony.CarrierConfigManager.ACTION_CARRIER_CONFIG_CHANGED);

        when(mockImsManager.isEnhanced4gLteModeSettingEnabledByUser()).thenReturn(false);
        when(mockImsManager.isVolteEnabledByPlatform()).thenReturn(false);
        when(mockImsManager.isWfcEnabledByUser()).thenReturn(false);
        when(mockImsManager.isWfcRoamingEnabledByUser()).thenReturn(false);
        when(mockImsManager.isWfcEnabledByPlatform()).thenReturn(false);
        when(mockImsManager.getWfcMode(anyBoolean())).thenReturn(WIFI_MODE_CELLULAR_PREFERRED);

        mUserSettingBroadcastReceiver = mConditionUserSetting.new UserSettingBroadcastReceiver();

        // When
        mUserSettingBroadcastReceiver.onReceive(mContext, mockIntent);

        // Then
        verify(mockChangedCallback, times(1)).onConditionChanged(EVENT_CARRIER_CONFIG_CHANGED);

        // Given
        when(mockIntent.getAction()).thenReturn("com.google.android.wfcactivation.TRY_WFC_CONNECTION");
        when(mockIntent.getIntExtra("SUB_ID", -1)).thenReturn(100);
        when(mockIntent.getIntExtra("TRY_STATUS", 0)).thenReturn(1);
        mConditionUserSetting.setWfcEnabledByUserOverride(false);

        // When
        mUserSettingBroadcastReceiver.onReceive(mContext, mockIntent);

        // Then
        verify(mockChangedCallback, times(1)).onConditionChanged(EVENT_WFC_SETTINGS_BROADCAST);
    }

    @Test
    public void UserSettingContentObserverTest() {
        // Given
        Handler mockHandler = mock(Handler.class);
        String subId = String.valueOf(100);
        Uri mUri = Uri.withAppendedPath(SubscriptionManager.WFC_ENABLED_CONTENT_URI, subId);

        when(mockImsManager.isWfcEnabledByUser()).thenReturn(false);

        mUserSettingContentObserver = mConditionUserSetting.new UserSettingContentObserver(mockHandler);

        // When
        mUserSettingContentObserver.onChange(true, mUri);
        // Then
        verify(mockChangedCallback, times(1)).onConditionChanged(EVENT_WFC_SETTINGS_CHANGED);

        // Given
        mUri = Uri.withAppendedPath(SubscriptionManager.WFC_ROAMING_ENABLED_CONTENT_URI, subId);

        when(mockImsManager.isWfcRoamingEnabledByUser()).thenReturn(false);
        when(mockImsManager.isWfcEnabledByUser()).thenReturn(true);

        // When
        mUserSettingContentObserver.onChange(true, mUri);
        // Then
        verify(mockChangedCallback, times(2)).onConditionChanged(EVENT_WFC_SETTINGS_CHANGED);

        // Given
        mUri = Uri.withAppendedPath(SubscriptionManager.WFC_MODE_CONTENT_URI, subId);
        when(mockImsManager.getWfcMode(anyBoolean())).thenReturn(WIFI_MODE_WIFI_ONLY);

        // When
        mUserSettingContentObserver.onChange(true, mUri);

        // Then
        verify(mockChangedCallback, times(1)).onConditionChanged(EVENT_WFC_MODE_CHANGED);

        // Given
        mUri = Uri.withAppendedPath(SubscriptionManager.ADVANCED_CALLING_ENABLED_CONTENT_URI, subId);
        when(mockImsManager.isEnhanced4gLteModeSettingEnabledByUser()).thenReturn(false);

        // When
        mUserSettingContentObserver.onChange(true, mUri);

        // Then
        verify(mockChangedCallback, times(1)).onConditionChanged(EVENT_VLT_SETTINGS_CHANGED);

        // Given
        mUri = Uri.withAppendedPath(Uri.withAppendedPath(SubscriptionManager.CONTENT_URI, COLUMN_CROSS_SIM_CALLING_ENABLED),
            subId);
        when(mockImsManager.isCrossSimCallingEnabledByUser()).thenReturn(false);

        // When
        mUserSettingContentObserver.onChange(true, mUri);

        // Then
        verify(mockChangedCallback, times(1)).onConditionChanged(EVENT_CST_SETTINGS_CHANGED);
    }
}