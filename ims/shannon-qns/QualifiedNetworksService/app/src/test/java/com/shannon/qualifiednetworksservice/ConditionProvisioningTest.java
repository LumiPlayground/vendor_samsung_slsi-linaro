package com.shannon.qualifiednetworksservice;

import android.content.Context;
import android.telephony.TelephonyManager;
import android.telephony.ims.ProvisioningManager;

import com.android.ims.ImsConfig;
import com.android.ims.ImsManager;

import junit.framework.TestCase;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.Mock;
import org.mockito.MockedStatic;
import org.mockito.Mockito;
import org.mockito.MockitoAnnotations;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.annotation.Config;

import static com.shannon.vendorcarrierconfig.ShannonVendorCarrierConfig.KEY_IMS_HANDOVER_HYSTERESIS_TIMER_VOLTE;
import static com.shannon.vendorcarrierconfig.ShannonVendorCarrierConfig.KEY_IMS_HANDOVER_HYSTERESIS_TIMER_VOWIFI;
import static org.mockito.Mockito.when;

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class ConditionProvisioningTest extends TestCase {
    private ConditionProvisioning mConditions;
    private ConditionProvisioning mConditions1;

    @Mock
    Context mockContext;

    @Mock
    ConfigurationManager mockConfiguration;

    @Mock
    ConditionCellular mockCell;

    @Mock
    ConditionWiFi mockWiFi;

    @Mock
    ConditionUserSetting mockUser;

    @Mock
    HandoverConditions.IConditionChanged mockCallback;

    @Mock
    SimManager mockSimManager;

    @Mock
    TelephonyManager mockTelephonyManager;

    @Mock
    ImsManager mockImsManager;

    @Mock
    ProvisioningManager mockProvisioningManager;

    @Mock
    private HandoverConditions.IConditionHelper mockConditionHelper;

    @Mock
    ImsConfig mockImsConfig;

    // static mocks
    private MockedStatic<SimManager> mockedStaticSimManager;
    private MockedStatic<ImsManager> mockedStaticImsManager;
    private MockedStatic<ProvisioningManager> mockedStaticProvisioningManager;

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.openMocks(this);

        mockedStaticSimManager = Mockito.mockStatic(SimManager.class);
        when(SimManager.getInstance()).thenReturn(mockSimManager);
        when(mockSimManager.getTelephonyManager(0)).thenReturn(mockTelephonyManager);
        mockedStaticImsManager = Mockito.mockStatic(ImsManager.class);
        when(ImsManager.getInstance(mockContext, 0)).thenReturn(mockImsManager);
        when(mockSimManager.getSubId(0)).thenReturn(1);
        mockedStaticProvisioningManager = Mockito.mockStatic(ProvisioningManager.class);
        when(ProvisioningManager.createForSubscriptionId(1)).thenReturn(mockProvisioningManager);
        when(mockImsManager.getConfigInterface()).thenReturn(mockImsConfig);
        when(mockImsManager.isVolteProvisionedOnDevice()).thenReturn(true);
        when(mockImsManager.isWfcProvisionedOnDevice()).thenReturn(true);
        when(mockProvisioningManager.getProvisioningStringValue(801)).thenReturn("");
        when(mockProvisioningManager.getProvisioningIntValue(802)).thenReturn(ProvisioningManager.PROVISIONING_VALUE_ENABLED);
        when(mockProvisioningManager.getProvisioningIntValue(ProvisioningManager.KEY_LTE_THRESHOLD_1)).thenReturn(-100);
        when(mockProvisioningManager.getProvisioningIntValue(ProvisioningManager.KEY_LTE_THRESHOLD_2)).thenReturn(-105);
        when(mockProvisioningManager.getProvisioningIntValue(ProvisioningManager.KEY_LTE_THRESHOLD_3)).thenReturn(-110);
        when(mockProvisioningManager.getProvisioningIntValue(ProvisioningManager.KEY_WIFI_THRESHOLD_A)).thenReturn(-70);
        when(mockProvisioningManager.getProvisioningIntValue(ProvisioningManager.KEY_WIFI_THRESHOLD_B)).thenReturn(-80);
        when(mockProvisioningManager.getProvisioningIntValue(
                ProvisioningManager.KEY_VOICE_OVER_WIFI_ROAMING_ENABLED_OVERRIDE)).thenReturn(1);
        when(mockProvisioningManager.getProvisioningIntValue(ProvisioningManager.KEY_LTE_EPDG_TIMER_SEC)).thenReturn(10);
        when(mockProvisioningManager.getProvisioningIntValue(ProvisioningManager.KEY_WIFI_EPDG_TIMER_SEC)).thenReturn(10);

        mConditions = new ConditionProvisioning(0, mockContext, mockConfiguration, mockConditionHelper, mockCallback);

        when(mockImsManager.getConfigInterface()).thenReturn(null);
        ConditionProvisioning conditions =
                new ConditionProvisioning(0, mockContext, mockConfiguration, mockConditionHelper, mockCallback);
        when(mockImsManager.getConfigInterface()).thenReturn(mockImsConfig);

        when(mockProvisioningManager.getProvisioningStringValue(ProvisioningManager.KEY_VOICE_OVER_WIFI_ENTITLEMENT_ID)).thenReturn("test");
        mConditions1 = new ConditionProvisioning(0, mockContext, mockConfiguration, mockConditionHelper, mockCallback);
    }

    @After
    public void tearDown() throws Exception {
        mockedStaticSimManager.close();
        mockedStaticImsManager.close();
        mockedStaticProvisioningManager.close();
        Mockito.validateMockitoUsage();
    }

    @Test
    public void testDestroy() {
        mConditions.destroy();
        assertTrue(true);
    }

    @Test
    public void testIsVoLteProvisioned() {
        assertTrue(mConditions.isVoLteProvisioned());
    }

    @Test
    public void testIsWfcProvisioned() {
        assertTrue(mConditions.isWfcProvisioned());
    }

    @Test
    public void testIsOemProvisioned() {
        assertFalse(mConditions.isOemProvisioned());
        assertTrue(mConditions1.isOemProvisioned());
    }

    @Test
    public void testIsMdnProvisioned() {
        when(mockTelephonyManager.getLine1Number()).thenReturn(null);
        assertFalse(mConditions.isMdnProvisioned());
        when(mockTelephonyManager.getLine1Number()).thenReturn("test");
        assertFalse(mConditions.isMdnProvisioned());
    }

    @Test
    public void testTestToString() {
        assertNotNull(mConditions.toString());
        assertNotNull(mConditions1.toString());
    }

    @Test
    public void testCallback_onProvisioningStringChanged() {
        ConditionProvisioning.ProvisioningCallBack callBack = mConditions.getProvisioningCallback();
        callBack.onProvisioningStringChanged(1, "");
        callBack.onProvisioningStringChanged(ProvisioningManager.KEY_VOICE_OVER_WIFI_ENTITLEMENT_ID, "test");
        callBack.onProvisioningStringChanged(ProvisioningManager.KEY_VOICE_OVER_WIFI_ENTITLEMENT_ID, null);
        callBack.onProvisioningStringChanged(ProvisioningManager.KEY_VOICE_OVER_WIFI_ENTITLEMENT_ID, null);
        assertTrue(true);
    }

    @Test
    public void testCallback_onProvisioningIntChanged() {
        ConditionProvisioning.ProvisioningCallBack callBack = mConditions.getProvisioningCallback();
        callBack.onProvisioningIntChanged(1, 0);

        callBack.onProvisioningIntChanged(ProvisioningManager.KEY_VOLTE_PROVISIONING_STATUS, 0);
        when(mockImsManager.isVolteProvisionedOnDevice()).thenReturn(false);
        callBack.onProvisioningIntChanged(ProvisioningManager.KEY_VOLTE_PROVISIONING_STATUS, 0);

        callBack.onProvisioningIntChanged(ProvisioningManager.KEY_VOICE_OVER_WIFI_ENABLED_OVERRIDE, 0);
        when(mockImsManager.isWfcProvisionedOnDevice()).thenReturn(false);
        callBack.onProvisioningIntChanged(ProvisioningManager.KEY_VOICE_OVER_WIFI_ENABLED_OVERRIDE, 0);

        when(mockCell.getRsrpRoveOut()).thenReturn(-100);
        callBack.onProvisioningIntChanged(ProvisioningManager.KEY_LTE_THRESHOLD_1, -100);
        callBack.onProvisioningIntChanged(ProvisioningManager.KEY_LTE_THRESHOLD_1, -115);

        when(mockCell.getRsrpMid()).thenReturn(-105);
        callBack.onProvisioningIntChanged(ProvisioningManager.KEY_LTE_THRESHOLD_2, -105);
        callBack.onProvisioningIntChanged(ProvisioningManager.KEY_LTE_THRESHOLD_2, -110);

        when(mockCell.getRsrpRoveIn()).thenReturn(-110);
        callBack.onProvisioningIntChanged(ProvisioningManager.KEY_LTE_THRESHOLD_3, -110);
        callBack.onProvisioningIntChanged(ProvisioningManager.KEY_LTE_THRESHOLD_3, -100);

        when(mockWiFi.getRssiRoveIn()).thenReturn(-70);
        callBack.onProvisioningIntChanged(ProvisioningManager.KEY_WIFI_THRESHOLD_A, -70);
        callBack.onProvisioningIntChanged(ProvisioningManager.KEY_WIFI_THRESHOLD_A, -75);

        when(mockWiFi.getRssiRoveOut()).thenReturn(-80);
        callBack.onProvisioningIntChanged(ProvisioningManager.KEY_WIFI_THRESHOLD_B, -80);
        callBack.onProvisioningIntChanged(ProvisioningManager.KEY_WIFI_THRESHOLD_B, -85);

        when(mockConfiguration.getInt(KEY_IMS_HANDOVER_HYSTERESIS_TIMER_VOLTE, -1)).thenReturn(10);
        callBack.onProvisioningIntChanged(ProvisioningManager.KEY_LTE_EPDG_TIMER_SEC, 10);
        callBack.onProvisioningIntChanged(ProvisioningManager.KEY_LTE_EPDG_TIMER_SEC, 20);

        when(mockConfiguration.getInt(KEY_IMS_HANDOVER_HYSTERESIS_TIMER_VOWIFI, -1)).thenReturn(10);
        callBack.onProvisioningIntChanged(ProvisioningManager.KEY_WIFI_EPDG_TIMER_SEC, 10);
        callBack.onProvisioningIntChanged(ProvisioningManager.KEY_WIFI_EPDG_TIMER_SEC, 20);

        callBack.onProvisioningIntChanged(ProvisioningManager.KEY_VOICE_OVER_WIFI_ROAMING_ENABLED_OVERRIDE, 0);

        when(mockConfiguration.getBoolean("vendor.vomobile_enabled", false)).thenReturn(false);
        callBack.onProvisioningIntChanged(802, 0);
        callBack.onProvisioningIntChanged(802, 1);

        assertTrue(true);
    }
}