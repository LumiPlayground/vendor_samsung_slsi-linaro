package com.shannon.qualifiednetworksservice;

import static org.mockito.ArgumentMatchers.any;
import static org.mockito.ArgumentMatchers.anyInt;
import static org.mockito.Mockito.when;

import android.telephony.TelephonyManager;

import com.shannon.qualifiednetworksservice.HandoverConditions.ConditionEvents;

import junit.framework.TestCase;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.Mock;
import org.mockito.MockitoAnnotations;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.annotation.Config;

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class CbsHandoverDecisionTest extends TestCase {
    private CbsHandoverDecision mCbsHandoverDecision;

    @Mock
    private HandoverConditions mockHandoverConditions;

    @Mock
    private ConditionCall mockCall;

    @Mock
    private ConditionCellular mockCell;

    @Mock
    private ConditionConnectivity mockTransport;

    @Mock
    private ConditionEvents mockConditionEvents;

    @Mock
    private ConditionUserSetting mockUser;

    @Mock
    private ConditionWiFi mockWiFi;

    @Mock
    private ConfigurationManager mockConfigurationManager;

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.openMocks(this);
        when(mockHandoverConditions.getSlotIndex()).thenReturn(1);
        when(mockHandoverConditions.getCallConditions()).thenReturn(mockCall);
        when(mockHandoverConditions.getTransportCondition()).thenReturn(mockTransport);
        when(mockHandoverConditions.getConfigurationManager()).thenReturn(mockConfigurationManager);
        when(mockHandoverConditions.getCellularConditions()).thenReturn(mockCell);
        when(mockHandoverConditions.getWiFiConditions()).thenReturn(mockWiFi);
        when(mockHandoverConditions.getUserConditions()).thenReturn(mockUser);

        mCbsHandoverDecision = new CbsHandoverDecision(mockHandoverConditions);
    }

    @After
    public void tearDown() throws Exception {
    }

    @Test
    public void testGetPreferredList() {
        assertNotNull(mCbsHandoverDecision.getPreferredList(mockConditionEvents));
    }

    @Test
    public void testGetPreferredListValid() {
        when(mockTransport.getNetworkType(anyInt())).thenReturn(TelephonyManager.NETWORK_TYPE_IWLAN);
        assertNotNull(mCbsHandoverDecision.getPreferredList(mockConditionEvents));
    }

    @Test
    public void testGetPreferredListCellular() {
        when(mockCell.isAirplaneMode()).thenReturn(false);
        when(mockCell.isRegistered()).thenReturn(true);
        when(mockTransport.isTransportAvailable(anyInt(), any())).thenReturn(true);
        assertNotNull(mCbsHandoverDecision.getPreferredList(mockConditionEvents));
    }

    @Test
    public void testGetPreferredListWiFi() {
        when(mockConfigurationManager.isCbsOverWiFiSupported()).thenReturn(true);
        when(mockWiFi.isConnected()).thenReturn(true);
        when(mockTransport.isTransportAvailable(anyInt(), any())).thenReturn(true);
        assertNotNull(mCbsHandoverDecision.getPreferredList(mockConditionEvents));
    }

    @Test
    public void testIsServiceSupported() {
        assertFalse(mCbsHandoverDecision.isServiceSupported(TelephonyManager.NETWORK_TYPE_IWLAN));
        assertFalse(mCbsHandoverDecision.isServiceSupported(TelephonyManager.NETWORK_TYPE_LTE));
        assertFalse(mCbsHandoverDecision.isServiceSupported(TelephonyManager.NETWORK_TYPE_UNKNOWN));
    }
}