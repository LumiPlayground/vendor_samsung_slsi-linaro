package com.shannon.networkservice;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;

import androidx.test.platform.app.InstrumentationRegistry;

import com.android.internal.telephony.PhoneConstants;
import com.shannon.networkservice.ShannonNetworkService.ShannonNetworkServiceProvider;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.annotation.Config;

import java.text.SimpleDateFormat;
import java.util.Date;

import static org.junit.Assert.*;

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class ConfigurationManagerTest {
    private static int testSlotId = 0;
    private static int invalidSlotId = -1;

    /* Custom broadcast */
    private static final String BROADCAST_ACTION_VENDOR_CONFIG_CHANGED = "com.shannon.ACTION.VCC.vendor_config_changed";
    private static final String BROADCAST_EXTRA_VENDOR_CONFIG = "VENDOR_CONFIG";
    /* Vendor prefix */
    private static final String KEY_PREFIX = "vendor.";
    /* VoMobile */
    private static final String KEY_VOMOBILE_ENABLED = KEY_PREFIX + "vomobile_enabled";

    private ConfigurationManager mManager;
    private ShannonNetworkService mService;
    private ShannonNetworkService.ShannonNetworkServiceProvider mProvider;
    private ConfigurationManagerBoat mManagerBoat;
    Context mContext;

    @Before
    public void setUp() throws Exception {
        mContext = InstrumentationRegistry.getInstrumentation().getContext();

        mService = new ShannonNetworkService();
        mProvider = (ShannonNetworkService.ShannonNetworkServiceProvider) mService.onCreateNetworkServiceProvider(mContext, testSlotId);
        mManager = mProvider.getConfiguration();

        mManagerBoat = new ConfigurationManagerBoat(testSlotId,"TEST");
    }

    @Test
    public void testOnReceive() {
        ConfigurationBroadcastReceiver mReceiver = new ConfigurationBroadcastReceiver();

        Intent intent = new Intent(BROADCAST_ACTION_VENDOR_CONFIG_CHANGED);
        intent.putExtra(PhoneConstants.SLOT_KEY, testSlotId);

        mReceiver.onReceive(mContext, intent);
    }

    @Test
    public void testOnReceiveNull() {
        ConfigurationBroadcastReceiver mReceiver = new ConfigurationBroadcastReceiver();

        // when
        mManager.destroy();

        Intent intent = new Intent(BROADCAST_ACTION_VENDOR_CONFIG_CHANGED);
        intent.putExtra(PhoneConstants.SLOT_KEY, testSlotId);

        mReceiver.onReceive(mContext, intent);
    }

    @Test
    public void testCallback() {
        mManagerBoat = new ConfigurationManagerBoat(0,"TEST");
        ConfigurationManager.ConfigurationReceiverCallback callback = mManagerBoat.createConfigurationReceiverCallback();

        Intent intent = new Intent(BROADCAST_ACTION_VENDOR_CONFIG_CHANGED);
        Bundle bundle = new Bundle();
        bundle.putBoolean(KEY_VOMOBILE_ENABLED, true);
        intent.putExtra(PhoneConstants.SLOT_KEY, 0);
        intent.putExtra(BROADCAST_EXTRA_VENDOR_CONFIG, bundle);
        callback.onReceive(intent);
    }

    @After
    public void tearDown() throws Exception {
    }

    @Test
    public void testErrorlog() {
        mManagerBoat = new ConfigurationManagerBoat(0,"TEST");
        ConfigurationManager.ConfigurationReceiverCallback callback = mManagerBoat.createConfigurationReceiverCallback();

        ConfigurationBroadcastReceiver mReceiver = new ConfigurationBroadcastReceiver();
        Intent intent = new Intent(BROADCAST_ACTION_VENDOR_CONFIG_CHANGED);
        intent.putExtra(PhoneConstants.SLOT_KEY, invalidSlotId);

        mReceiver.onReceive(mContext, intent);
    }

    @Test
    public void testDestroy() {
        mManager.destroy();
    }

    @Test
    public void testIsVoMobileEnabled() {
        mManager.isVoMobileEnabled();
    }

    @Test
    public void testIsVoMobileEnabledOnRoaming() {
        mManager.isVoMobileEnabledOnRoaming();
    }

    @Test
    public void testIsVoMobileSupportedOnNetwork() {
        mManager.isVoMobileSupportedOnNetwork(0);
        mManager.isVoMobileSupportedOnNetwork(19);/*LTE_CA*/
    }
}
