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

package com.shannon.vendorcarrierconfig;

import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.res.AssetManager;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.telephony.CarrierConfigManager;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
import android.util.SparseArray;
import com.android.internal.telephony.PhoneConstants;
import com.android.internal.telephony.TelephonyIntents;
import com.shannon.vendorcarrierconfig.ShannonVendorCarrierConfig.ShannonVendorCarrierConfigProvider;
import com.shannon.vendorcarrierconfig.ShannonVendorCarrierConfig.SimStateBroadcastReceiver;

import java.io.ByteArrayInputStream;
import java.io.File;
import java.io.InputStream;
import java.util.HashMap;
import java.util.Map;

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

import static junit.framework.TestCase.assertEquals;
import static junit.framework.TestCase.assertFalse;
import static junit.framework.TestCase.assertNotNull;
import static junit.framework.TestCase.assertNull;
import static junit.framework.TestCase.assertTrue;
import static junit.framework.TestCase.fail;
import static org.mockito.ArgumentMatchers.any;
import static org.mockito.ArgumentMatchers.anyInt;
import static org.mockito.ArgumentMatchers.anyObject;
import static org.mockito.ArgumentMatchers.anyString;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.mockStatic;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

@FixMethodOrder(MethodSorters.NAME_ASCENDING)
@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class ShannonVendorCarrierConfigTest {
    private static MockedStatic<SubscriptionManager> sMockedSubscriptionManager;

    private ShannonVendorCarrierConfig mShannonVendorCarrierConfig;
    private ShannonVendorCarrierConfigProvider mShannonVendorCarrierConfigProvider;
    private SimStateBroadcastReceiver mSimStateBroadcastReceiver;

    @Mock
    private Bundle mockBundle;
    @Mock
    private Context mockContext;
    @Mock
    private TelephonyManager mockTelephonyManager;

    @BeforeClass
    public static void beforeClass() {
        sMockedSubscriptionManager = Mockito.mockStatic(SubscriptionManager.class);
    }

    @AfterClass
    public static void afterClass() {
        sMockedSubscriptionManager.close();
    }

    @Before
    public void setUp() {
        MockitoAnnotations.openMocks(this);
        when(mockContext.createDeviceProtectedStorageContext()).thenReturn(mockContext);
        when(mockContext.getSystemService(TelephonyManager.class)).thenReturn(mockTelephonyManager);
        when(mockTelephonyManager.createForSubscriptionId(anyInt())).thenReturn(mockTelephonyManager);
        when(mockTelephonyManager.getActiveModemCount()).thenReturn(1);

        when(SubscriptionManager.getSimStateForSlotIndex(anyInt())).thenReturn(TelephonyManager.SIM_STATE_ABSENT);

        mShannonVendorCarrierConfig = new ShannonVendorCarrierConfig(mockContext);
        mShannonVendorCarrierConfigProvider = mShannonVendorCarrierConfig.new ShannonVendorCarrierConfigProvider(0, 0);
        mSimStateBroadcastReceiver = mShannonVendorCarrierConfig.new SimStateBroadcastReceiver();
    }

    @After
    public void tearDown() {
        Mockito.validateMockitoUsage();
    }

    @Test
    public void destroyTest() {
        // When
        mShannonVendorCarrierConfig.destroy();

        // Then
        verify(mockContext, times(1)).unregisterReceiver(any());
    }

    @Test
    public void getVendorCarrierConfigForSlotTest() {
        assertNotNull(mShannonVendorCarrierConfig.getVendorCarrierConfigForSlot(0));
    }

    @Test
    public void updateVendorCarrierConfigForSlotTest() {
        // When
        mShannonVendorCarrierConfig.updateVendorCarrierConfigForSlot(0, mockBundle);
    }

    @Test
    public void deleteSharedPreferencesFilesTest() {
        // Given
        File mockFile_1 = mock(File.class);
        File mockFile_2 = mock(File.class);
        File mockFile_3 = mock(File.class);

        File[] dirFilesList = new File[3];
        dirFilesList[0] = mockFile_1;
        dirFilesList[1] = mockFile_2;
        dirFilesList[2] = mockFile_3;

        when(mockContext.getSharedPreferencesPath(anyString())).thenReturn(mockFile_1);
        when(mockFile_1.getParentFile()).thenReturn(mockFile_2);
        when(mockFile_2.listFiles()).thenReturn(dirFilesList);

        when(mockFile_1.getName()).thenReturn("com.shannon.vendor_config_carrierid_");
        when(mockFile_2.getName()).thenReturn("com.shannon.vendor_config_carrierid_");
        when(mockFile_3.getName()).thenReturn("com.shannon.vendor_config_carrierid_");

        when(mockFile_1.delete()).thenReturn(true);
        when(mockFile_2.delete()).thenReturn(true);
        when(mockFile_3.delete()).thenReturn(true);

        when(mockContext.deleteSharedPreferences(anyString())).thenReturn(true);

        // When
        mShannonVendorCarrierConfig.deleteSharedPreferencesFiles();

        // Then
        verify(mockFile_1, times(1)).delete();
        verify(mockFile_2, times(1)).delete();
        verify(mockFile_3, times(1)).delete();

        // Given
        when(mockFile_1.getParentFile()).thenReturn(null);

        // When
        mShannonVendorCarrierConfig.deleteSharedPreferencesFiles();
        // Then
        verify(mockFile_1, times(1)).delete();

        // Given
        when(mockFile_1.getParentFile()).thenReturn(mockFile_2);
        when(mockFile_2.listFiles()).thenReturn(null);

        // When
        mShannonVendorCarrierConfig.deleteSharedPreferencesFiles();
        // Then
        verify(mockFile_1, times(1)).delete();
    }

    @Test
    public void getBundleTest() {
        assertNotNull(mShannonVendorCarrierConfigProvider.getBundle());
    }

    @Test
    public void SimStateBroadcastReceiverTest() {
        Intent mockIntent = mock(Intent.class);
        when(mockIntent.getAction()).thenReturn(null);
        mSimStateBroadcastReceiver.onReceive(mockContext, mockIntent);

        when(mockIntent.getAction()).thenReturn(TelephonyIntents.ACTION_SIM_STATE_CHANGED);
        when(mockIntent.getIntExtra(PhoneConstants.PHONE_KEY, -1)).thenReturn(-1);
        mSimStateBroadcastReceiver.onReceive(mockContext, mockIntent);

        // Given
        Intent intent = new Intent(TelephonyIntents.ACTION_SIM_STATE_CHANGED);
        intent.putExtra(PhoneConstants.PHONE_KEY, 1);
        intent.putExtra(PhoneConstants.SUBSCRIPTION_KEY, 1);
        when(SubscriptionManager.isValidSubscriptionId(anyInt())).thenReturn(true);

        // When
        mSimStateBroadcastReceiver.onReceive(mockContext, intent);
        // Then
        verify(mockContext, times(0)).unregisterReceiver(any());

        // values for getSimStateString()
        // Given
        when(SubscriptionManager.getSimStateForSlotIndex(anyInt())).thenReturn(TelephonyManager.SIM_STATE_ABSENT);
        // When
        mSimStateBroadcastReceiver.onReceive(mockContext, intent);
        // Given
        when(SubscriptionManager.getSimStateForSlotIndex(anyInt())).thenReturn(TelephonyManager.SIM_STATE_PIN_REQUIRED);
        // When
        mSimStateBroadcastReceiver.onReceive(mockContext, intent);
        // Given
        when(SubscriptionManager.getSimStateForSlotIndex(anyInt())).thenReturn(TelephonyManager.SIM_STATE_PUK_REQUIRED);
        // When
        mSimStateBroadcastReceiver.onReceive(mockContext, intent);
        // Given
        when(SubscriptionManager.getSimStateForSlotIndex(anyInt()))
            .thenReturn(TelephonyManager.SIM_STATE_NETWORK_LOCKED);
        // When
        mSimStateBroadcastReceiver.onReceive(mockContext, intent);
        // Given
        when(SubscriptionManager.getSimStateForSlotIndex(anyInt())).thenReturn(TelephonyManager.SIM_STATE_READY);
        // When
        mSimStateBroadcastReceiver.onReceive(mockContext, intent);
        // Given
        when(SubscriptionManager.getSimStateForSlotIndex(anyInt())).thenReturn(TelephonyManager.SIM_STATE_NOT_READY);
        // When
        mSimStateBroadcastReceiver.onReceive(mockContext, intent);
        // Given
        when(SubscriptionManager.getSimStateForSlotIndex(anyInt()))
            .thenReturn(TelephonyManager.SIM_STATE_PERM_DISABLED);
        // When
        mSimStateBroadcastReceiver.onReceive(mockContext, intent);
        // Given
        when(SubscriptionManager.getSimStateForSlotIndex(anyInt()))
            .thenReturn(TelephonyManager.SIM_STATE_CARD_IO_ERROR);
        // When
        mSimStateBroadcastReceiver.onReceive(mockContext, intent);
        // Given
        when(SubscriptionManager.getSimStateForSlotIndex(anyInt()))
            .thenReturn(TelephonyManager.SIM_STATE_CARD_RESTRICTED);
        // When
        mSimStateBroadcastReceiver.onReceive(mockContext, intent);
        // Given
        when(SubscriptionManager.getSimStateForSlotIndex(anyInt())).thenReturn(TelephonyManager.SIM_STATE_PRESENT);
        // When
        mSimStateBroadcastReceiver.onReceive(mockContext, intent);
        // Given
        when(SubscriptionManager.getSimStateForSlotIndex(anyInt())).thenReturn(TelephonyManager.SIM_STATE_UNKNOWN);
        // When
        mSimStateBroadcastReceiver.onReceive(mockContext, intent);

        // Then
        verify(mockContext, times(2)).unregisterReceiver(any());
    }

    @Test
    public void testVendorConfigBroadcastReceiver() {
        MockedStatic<Message> messageMockedStatic = mockStatic(Message.class);
        Message message = mock(Message.class);
        when(Message.obtain(anyObject(), anyInt())).thenReturn(message);

        Intent intent = mock(Intent.class);
        ShannonVendorCarrierConfigProvider.VendorConfigBroadcastReceiver receiver =
                mShannonVendorCarrierConfigProvider.getBroadcastReceiver();

        when(intent.getAction()).thenReturn(null);
        receiver.onReceive(mockContext, intent);

        when(intent.getAction()).thenReturn(CarrierConfigManager.ACTION_CARRIER_CONFIG_CHANGED);
        when(intent.getIntExtra(CarrierConfigManager.EXTRA_SLOT_INDEX, -1)).thenReturn(-1);
        receiver.onReceive(mockContext, intent);

        when(intent.getIntExtra(CarrierConfigManager.EXTRA_SLOT_INDEX, -1)).thenReturn(0);
        receiver.onReceive(mockContext, intent);

        when(intent.getAction()).thenReturn(ShannonVendorCarrierConfig.BROADCAST_ACTION_VENDOR_CONFIG_DATA_CHANGED);
        receiver.onReceive(mockContext, intent);

        when(intent.getAction()).thenReturn(ShannonVendorCarrierConfig.BROADCAST_ACTION_VENDOR_CONFIG_MEDIA_MOUNTED);
        receiver.onReceive(mockContext, intent);

        when(intent.getAction()).thenReturn(ShannonVendorCarrierConfig.BROADCAST_ACTION_REQUEST_VENDOR_CONFIG);
        when(intent.getIntExtra(PhoneConstants.SLOT_KEY, -1)).thenReturn(-1);
        receiver.onReceive(mockContext, intent);

        when(intent.getAction()).thenReturn(ShannonVendorCarrierConfig.BROADCAST_ACTION_REQUEST_VENDOR_CONFIG);
        when(intent.getIntExtra(PhoneConstants.SLOT_KEY, -1)).thenReturn(0);
        receiver.onReceive(mockContext, intent);

        verify(message, times(4)).sendToTarget();

        messageMockedStatic.close();
    }

    @Test
    public void testShannonVendorCarrierConfigProviderDestroy() {
        mShannonVendorCarrierConfigProvider.destroy();
        verify(mockContext, times(1)).unregisterReceiver(any());
    }

    @Test
    public void testShannonVendorCarrierConfigProviderGetSubId() {
        assertEquals(0, mShannonVendorCarrierConfigProvider.getSubId());
    }

    @Test
    public void testShannonVendorCarrierConfigProviderHandler() {
        sMockedSubscriptionManager.when(() -> SubscriptionManager.getSimStateForSlotIndex(anyInt()))
                .thenReturn(TelephonyManager.SIM_STATE_ABSENT);

        try {
            AssetManager assets = mock(AssetManager.class);
            File file = mock(File.class);

            when(mockContext.getAssets()).thenReturn(assets);
            when(mockContext.getSharedPreferencesPath(anyString())).thenReturn(file);
            when(mockTelephonyManager.createForSubscriptionId(anyInt())).thenReturn(mockTelephonyManager);
            when(mockTelephonyManager.getSimSpecificCarrierId()).thenReturn(1);
            when(mockTelephonyManager.getSimOperator()).thenReturn("310260");
            when(file.exists()).thenReturn(false);

            String[] strings = new String[1];
            strings[0] = "default.xml";
            when(assets.list("")).thenReturn(strings);
            String test = "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"yes\"?>\n" +
                    "<vendor_config>\n" +
                    "<boolean name=\"test\" value=\"false\"/>\n" +
                    "</vendor_config>\n";
            InputStream stream = new ByteArrayInputStream(test.getBytes("utf-8"));
            when(assets.open("default.xml")).thenReturn(stream);

            ShannonVendorCarrierConfigProvider provider = new ShannonVendorCarrierConfig(mockContext).
                    new ShannonVendorCarrierConfigProvider(0, 0);
            Handler handler = provider.getMessageHandler();
            Message message = Message.obtain();

            // SIM_INSERTED
            message.what = 1;
            handler.handleMessage(message);
            assertFalse(provider.getBundle().isEmpty());

            // CARRIER_CONFIG_CHANGED
            message.what = 2;
            handler.handleMessage(message);

            // VENDOR_CONFIG_CHANGED
            message.what = 3;
            handler.handleMessage(message);

            // USER_CONFIG_CHANGED
            message.what = 4;
            message.obj = null;
            handler.handleMessage(message);

            Bundle userBundle = new Bundle();
            userBundle.putBoolean("test", true);
            message.obj = userBundle;
            handler.handleMessage(message);

            // REQUEST_CONFIG
            message.what = 6;
            message.obj = null;
            handler.handleMessage(message);

            assertNotNull(provider.getBundle());
        } catch (Exception e) {
            assertNull(e);
        }
    }

    @Test
    public void testShannonVendorCarrierConfigProvider_getSharedPreferencesBundle() {
        File file = mock(File.class);
        when(mockContext.getSharedPreferencesPath(anyString())).thenReturn(file);
        when(file.exists()).thenReturn(true);
        SharedPreferences preferences = mock(SharedPreferences.class);
        when(mockContext.getSharedPreferences(anyString(), anyInt())).thenReturn(preferences);
        Map<String, ? super Object> map = new HashMap<>();
        map.put("test_int", 1);
        map.put("test_boolean", true);
        map.put("test_string", "string");
        map.put("test_float", (float) 0.42);
        when(preferences.getAll()).thenAnswer(x -> map);

        Bundle bundle = mShannonVendorCarrierConfigProvider.getSharedPreferencesBundle(1);
        assertFalse(bundle.isEmpty());
    }

    @Test
    public void testShannonVendorCarrierConfigProvider_saveBundleToSharedPreferences() {
        when(mockTelephonyManager.getSimSpecificCarrierId()).thenReturn(TelephonyManager.UNKNOWN_CARRIER_ID);
        when(mockTelephonyManager.getSimCarrierId()).thenReturn(TelephonyManager.UNKNOWN_CARRIER_ID);
        when(mockTelephonyManager.getCarrierIdFromSimMccMnc()).thenReturn(1);
        SharedPreferences preferences = mock(SharedPreferences.class);
        when(mockContext.getSharedPreferences(anyString(), anyInt())).thenReturn(preferences);
        SharedPreferences.Editor editor = mock(SharedPreferences.Editor.class);
        when(preferences.edit()).thenReturn(editor);

        Bundle bundle = new Bundle();
        bundle.putInt("test_int", 1);
        bundle.putBoolean("test_boolean", true);
        bundle.putString("test_string", "string");
        bundle.putFloat("test_float", (float)0.30);

        mShannonVendorCarrierConfigProvider.setTelephonyManager(mockTelephonyManager);
        mShannonVendorCarrierConfigProvider.saveBundleToSharedPreferences(bundle);
        verify(editor, times(1)).apply();
    }

    @Test
    public void testShannonVendorCarrierConfigProvider_getConfigFromExternal() {
        MockedStatic<Environment> mockedStaticEnv = mockStatic(Environment.class);

        when(Environment.getExternalStorageState()).thenReturn(Environment.MEDIA_MOUNTED);
        File dir = mock(File.class);
        when(mockContext.getExternalFilesDir(null)).thenReturn(dir);
        File file = mock(File.class);
        File[] files = {file};
        when(dir.listFiles()).thenReturn(files);
        when(file.getName()).thenReturn("default.xml");
        when(file.getPath()).thenReturn("default.xml");

        Bundle bundle = mShannonVendorCarrierConfigProvider.getConfigFromExternal("default");
        assertTrue(bundle.isEmpty());

        mockedStaticEnv.close();
    }

    @Test
    public void testSimStateBroadcastReceiver_onServiceLoaded() {
        MockedStatic<Message> mockedStaticMsg = mockStatic(Message.class);

        Message message = mock(Message.class);
        when(Message.obtain(any(), anyInt())).thenReturn(message);
        when(SubscriptionManager.getSimStateForSlotIndex(anyInt())).thenReturn(TelephonyManager.SIM_STATE_LOADED);
        int[] subIds = {5};
        when(SubscriptionManager.getSubId(0)).thenReturn(subIds);
        when(SubscriptionManager.isValidSubscriptionId(5)).thenReturn(true);

        mSimStateBroadcastReceiver.onServiceLoaded();
        verify(message, times(1)).sendToTarget();

        mockedStaticMsg.close();
    }
}

