package com.shannon.dataservice;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.telephony.TelephonyManager;

import androidx.test.platform.app.InstrumentationRegistry;

import com.android.internal.telephony.PhoneConstants;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.MockitoAnnotations;
import org.mockito.MockitoSession;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.annotation.Config;

import static com.shannon.dataservice.ConfigurationManager.BROADCAST_EXTRA_VENDOR_CONFIG;
import static com.shannon.dataservice.ConfigurationParameters.KEY_EPDG_CONNECTION_RETRY_PARAMETERS;
import static com.shannon.dataservice.ConfigurationParameters.KEY_VOMOBILE_ENABLED;
import static com.shannon.dataservice.ConfigurationParameters.KEY_VOMOBILE_ONROAMING_ENABLED;
import static org.junit.Assert.*;
import static org.mockito.Mockito.mockitoSession;

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class ConfigurationManagerTest {
    private ConfigurationBroadcastReceiver mConfigurationBroadcastReceiver;
    private ConfigurationManager mConfigurationManager;
    private MockitoSession mStaticMockSession;
    private Context mContext;

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.openMocks(this);
        mStaticMockSession = mockitoSession().startMocking();
        mContext = InstrumentationRegistry.getInstrumentation().getContext();

        // Initial Setting
        mConfigurationBroadcastReceiver = new ConfigurationBroadcastReceiver();
        mConfigurationManager = new ConfigurationManager(mContext, TEST_VALUES.SLOT_ID_1);
    }

    @After
    public void tearDown() throws Exception {
        mStaticMockSession.finishMocking();
    }

    @Test
    public void destroy() {
        // given
        int preSize;
        Intent intent = new Intent(ConfigurationBroadcastReceiver.BROADCAST_ACTION_VENDOR_CONFIG_CHANGED);
        Bundle bundle = new Bundle();
        bundle.putString("name", "oliver");
        intent.putExtra(PhoneConstants.SLOT_KEY, TEST_VALUES.SLOT_ID_1);
        intent.putExtra(BROADCAST_EXTRA_VENDOR_CONFIG, bundle);

        // when
        mConfigurationBroadcastReceiver.onReceive(mContext, intent);
        preSize = ConfigurationBroadcastReceiver.getmCallbacks().size();
        mConfigurationManager.destroy();

        // then
        assertEquals(preSize - 1, ConfigurationBroadcastReceiver.getmCallbacks().size());
    }

    @Test
    public void testInitVendorConfigChange () {
        // given
        Intent intent = new Intent(ConfigurationBroadcastReceiver.BROADCAST_ACTION_VENDOR_CONFIG_CHANGED);
        Bundle bundle = new Bundle();
        bundle.putString("name", "oliver");
        intent.putExtra(PhoneConstants.SLOT_KEY, TEST_VALUES.SLOT_ID_1);
        intent.putExtra(BROADCAST_EXTRA_VENDOR_CONFIG, bundle);

        // when, broadcast received
        mConfigurationBroadcastReceiver.onReceive(mContext, intent);

        // then, check if the name is initialized.
        assertEquals(bundle.getString("name"), mConfigurationManager.getBundle().getString("name"));
    }

    @Test
    public void testUpdateVendorConfigChange() {
        // given
        Intent intent = new Intent(ConfigurationBroadcastReceiver.BROADCAST_ACTION_VENDOR_CONFIG_CHANGED);
        Bundle bundle = new Bundle();
        bundle.putString("name", "oliver");
        bundle.putString(ConfigurationParameters.KEY_EPDG_SUPPORTED_PCO_ID_LIST, "testPcoId_001");
        bundle.putInt(ConfigurationParameters.KEY_WIFI_ACCESS_POINTS_HANDOVER_TIMEOUT, 30);
        bundle.putBoolean(ConfigurationParameters.KEY_WIFI_ACCESS_POINTS_HANDOVER_SUPPORTED, true);
        intent.putExtra(PhoneConstants.SLOT_KEY, TEST_VALUES.SLOT_ID_1);
        intent.putExtra(BROADCAST_EXTRA_VENDOR_CONFIG, bundle);
        
        // when
        mConfigurationBroadcastReceiver.onReceive(mContext, intent);

        // then
        assertEquals("testPcoId_001", mConfigurationManager.getSupportedPcoIdList());
        assertEquals(30, mConfigurationManager.getWifiApHandoverTimeout());
        assertEquals(bundle.getString("name"), mConfigurationManager.getBundle().getString("name"));
    }

    @Test
    public void getConnectionConfiguration() {
        ConfigurationParameters mConnectionConfiguration = mConfigurationManager.getConnectionConfiguration();
    }

    @Test
    public void updateBundleNonOverlap() {
        // given
        setBundle(mConfigurationManager, BROADCAST_EXTRA_VENDOR_CONFIG, true);
        mConfigurationManager.handleVendorConfigChanged(makeIntent("don't care", "don't care"));
    }

    @Test
    public void updateBundleNonOverlapWithError() {
        // given
        setBundle(mConfigurationManager, BROADCAST_EXTRA_VENDOR_CONFIG, true);
        mConfigurationManager.handleVendorConfigChanged(makeIntent(null,"don't care", "don't care"));
    }

    @Test
    public void isVoMobileSupportedOnNetwork() {
        assertEquals(false, mConfigurationManager.isVoMobileSupportedOnNetwork(TelephonyManager.NETWORK_TYPE_IWLAN));
    }

    @Test
    public void isVoMobileEnabled() {
        // given
        setBundle(mConfigurationManager, KEY_VOMOBILE_ENABLED, true);

        // when/then
        assertEquals(true, mConfigurationManager.isVoMobileEnabled());
    }

    @Test
    public void isVoMobileEnabledOnRoamingNetwork() {
        // given
        setBundle(mConfigurationManager, KEY_VOMOBILE_ONROAMING_ENABLED, true);

        // when/then
        assertEquals(true, mConfigurationManager.isVoMobileEnabledOnRoamingNetwork());
    }

    @Test
    public void getRetryParameters() {
        // given
        setBundleWithString(mConfigurationManager, KEY_EPDG_CONNECTION_RETRY_PARAMETERS, "any");

        // when/then
        assertEquals("any", mConfigurationManager.getRetryParameters());
    }


    /* Not for test */
    /* For easy testing, only use the below function in test code */
    public void setBundle(ConfigurationManager mConfigurationManager, String key, boolean value) {
        Intent intent = new Intent(ConfigurationBroadcastReceiver.BROADCAST_ACTION_VENDOR_CONFIG_CHANGED);
        Bundle bundle = new Bundle();
        bundle.putBoolean(key, value);
        intent.putExtra(PhoneConstants.SLOT_KEY, TEST_VALUES.SLOT_ID_1);
        intent.putExtra(BROADCAST_EXTRA_VENDOR_CONFIG, bundle);

        mConfigurationManager.handleVendorConfigChanged(intent);
    }

    public void setBundleWithString(ConfigurationManager mConfigurationManager, String key, String value) {
        Intent intent = new Intent(ConfigurationBroadcastReceiver.BROADCAST_ACTION_VENDOR_CONFIG_CHANGED);
        Bundle bundle = new Bundle();
        bundle.putString(key, value);
        intent.putExtra(PhoneConstants.SLOT_KEY, TEST_VALUES.SLOT_ID_1);
        intent.putExtra(BROADCAST_EXTRA_VENDOR_CONFIG, bundle);

        mConfigurationManager.handleVendorConfigChanged(intent);
    }

    public Intent makeIntent(String key, String value) {
        Intent intent = new Intent(ConfigurationBroadcastReceiver.BROADCAST_ACTION_VENDOR_CONFIG_CHANGED);
        Bundle bundle = new Bundle();
        bundle.putString(key, value);
        intent.putExtra(PhoneConstants.SLOT_KEY, TEST_VALUES.SLOT_ID_1);
        intent.putExtra(BROADCAST_EXTRA_VENDOR_CONFIG, bundle);
        return intent;
    }

    public Intent makeIntent(String action, String key, String value) {
        Intent intent;
        if (action != null) {
            intent = new Intent();
        } else {
            intent = new Intent(action);
        }
        Bundle bundle = new Bundle();
        bundle.putString(key, value);
        intent.putExtra(PhoneConstants.SLOT_KEY, TEST_VALUES.SLOT_ID_1);
        intent.putExtra(BROADCAST_EXTRA_VENDOR_CONFIG, bundle);
        return intent;
    }
}