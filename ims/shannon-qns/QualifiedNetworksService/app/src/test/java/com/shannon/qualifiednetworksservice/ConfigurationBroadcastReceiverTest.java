package com.shannon.qualifiednetworksservice;

import android.content.Context;
import android.content.Intent;

import com.android.internal.telephony.PhoneConstants;

import junit.framework.TestCase;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.Mock;
import org.mockito.Mockito;
import org.mockito.MockitoAnnotations;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.annotation.Config;

import static com.shannon.vendorcarrierconfig.ShannonVendorCarrierConfig.BROADCAST_ACTION_VENDOR_CONFIG_CHANGED;
import static org.mockito.Mockito.when;

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class ConfigurationBroadcastReceiverTest extends TestCase {
    private ConfigurationBroadcastReceiver mReceiver;

    @Mock
    private Context mockedContext;

    @Mock
    private Intent mockedIntent;

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.openMocks(this);

        mReceiver = new ConfigurationBroadcastReceiver();
    }

    @After
    public void tearDown() throws Exception {
        Mockito.validateMockitoUsage();
    }

    @Test
    public void testOnReceive() {
        when(mockedIntent.getAction()).thenReturn(BROADCAST_ACTION_VENDOR_CONFIG_CHANGED);
        when(mockedIntent.getIntExtra(PhoneConstants.SLOT_KEY, -1)).thenReturn(0);

        mReceiver.onReceive(mockedContext, mockedIntent);
        assertTrue(mReceiver.getIntentsArray().size() > 0);
    }

    @Test
    public void testRegisterConfigurationReceiverCallback() {
        when(mockedIntent.getAction()).thenReturn(BROADCAST_ACTION_VENDOR_CONFIG_CHANGED);
        when(mockedIntent.getIntExtra(PhoneConstants.SLOT_KEY, -1)).thenReturn(0);
        mReceiver.onReceive(mockedContext, mockedIntent);

        ConfigurationBroadcastReceiver.registerConfigurationReceiverCallback(0,
                new ConfigurationBroadcastReceiver.IConfigurationReceiverCallback() {
            @Override
            public void onReceive(Intent intent) {
                // empty
            }
        });
        assertEquals(0, mReceiver.getIntentsArray().size());
    }

    @Test
    public void testUnregisterConfigurationReceiverCallback() {
        ConfigurationBroadcastReceiver.unregisterConfigurationReceiverCallback(0);
        assertTrue(true);
    }
}