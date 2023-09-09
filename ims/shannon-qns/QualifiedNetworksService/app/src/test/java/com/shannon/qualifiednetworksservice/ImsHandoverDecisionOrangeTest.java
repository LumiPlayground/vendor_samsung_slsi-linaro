package com.shannon.qualifiednetworksservice;

import android.telephony.TelephonyManager;

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

import static org.mockito.Mockito.when;

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class ImsHandoverDecisionOrangeTest extends TestCase {
    private ImsHandoverDecisionOrange mDecision;

    @Mock
    HandoverConditions mockConditions;

    @Mock
    ConditionWiFi mockWiFi;

    @Mock
    ConditionUserSetting mockUser;

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.openMocks(this);

        when(mockConditions.getWiFiConditions()).thenReturn(mockWiFi);
        when(mockConditions.getUserConditions()).thenReturn(mockUser);
        when(mockUser.isWfcEnabled()).thenReturn(false);
        mDecision = new ImsHandoverDecisionOrange(mockConditions);
    }

    @After
    public void tearDown() throws Exception {
        Mockito.validateMockitoUsage();
    }

    @Test
    public void testGetPreferredNoCellular() {
        when(mockWiFi.isConnected()).thenReturn(false);
        assertEquals(TelephonyManager.NETWORK_TYPE_UNKNOWN,
                mDecision.getPreferredNoCellular(false, TelephonyManager.NETWORK_TYPE_UNKNOWN));
        assertEquals(TelephonyManager.NETWORK_TYPE_UNKNOWN,
                mDecision.getPreferredNoCellular(true, TelephonyManager.NETWORK_TYPE_UNKNOWN));

        when(mockWiFi.isConnected()).thenReturn(true);
        when(mockWiFi.isBetterThanThreshold(ImsHandoverDecisionOrange.RSSI_IN)).thenReturn(false);
        when(mockWiFi.isBetterThanThreshold(ImsHandoverDecisionOrange.RSSI_OUT)).thenReturn(false);
        assertEquals(TelephonyManager.NETWORK_TYPE_UNKNOWN,
                mDecision.getPreferredNoCellular(true, TelephonyManager.NETWORK_TYPE_UNKNOWN));

        when(mockWiFi.isBetterThanThreshold(ImsHandoverDecisionOrange.RSSI_OUT)).thenReturn(true);
        assertEquals(TelephonyManager.NETWORK_TYPE_UNKNOWN,
                mDecision.getPreferredNoCellular(true, TelephonyManager.NETWORK_TYPE_UNKNOWN));
        assertEquals(TelephonyManager.NETWORK_TYPE_IWLAN,
                mDecision.getPreferredNoCellular(true, TelephonyManager.NETWORK_TYPE_IWLAN));

        when(mockWiFi.isBetterThanThreshold(ImsHandoverDecisionOrange.RSSI_IN)).thenReturn(true);
        assertEquals(TelephonyManager.NETWORK_TYPE_IWLAN,
                mDecision.getPreferredNoCellular(true, TelephonyManager.NETWORK_TYPE_IWLAN));
    }
}