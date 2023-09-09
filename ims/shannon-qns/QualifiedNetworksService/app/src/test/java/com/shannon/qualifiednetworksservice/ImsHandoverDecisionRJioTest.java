package com.shannon.qualifiednetworksservice;

import static org.mockito.Mockito.when;

import android.telephony.TelephonyManager;
import android.telephony.data.ApnSetting;

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
public class ImsHandoverDecisionRJioTest extends TestCase {
    private ImsHandoverDecisionRJio mDecision;

    @Mock
    private HandoverConditions mockConditions;

    @Mock
    private ConfigurationManager mockConfiguration;

    @Mock
    private ConditionCellular mockCellularConditions;

    @Mock
    private ConditionWiFi mockWiFiCondition;

    @Mock
    private ConditionUserSetting mockUserConditions;

    @Mock
    private ConditionConnectivity mockTransportConditions;

    @Mock
    private ConditionMobile mockMobileConditions;

    @Mock
    private ConditionCall mockCallConditions;

    @Mock
    private ConditionProvisioning mockProvisioningConditions;

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.openMocks(this);

        when(mockConditions.getConfigurationManager()).thenReturn(mockConfiguration);
        when(mockConditions.getCellularConditions()).thenReturn(mockCellularConditions);
        when(mockConditions.getWiFiConditions()).thenReturn(mockWiFiCondition);
        when(mockConditions.getUserConditions()).thenReturn(mockUserConditions);
        when(mockConditions.getTransportCondition()).thenReturn(mockTransportConditions);
        when(mockConditions.getMobileConditions()).thenReturn(mockMobileConditions);
        when(mockConditions.getCallConditions()).thenReturn(mockCallConditions);
        when(mockConditions.getProvisioningConditions()).thenReturn(mockProvisioningConditions);
        when(mockConditions.getSlotIndex()).thenReturn(0);
        mDecision = new ImsHandoverDecisionRJio(mockConditions);
    }

    @After
    public void tearDown() throws Exception {
    }

    @Test
    public void testGetHysteresisTimerToWiFi() {
        when(mockConfiguration.getHysteresisTimerToWiFi()).thenReturn(30);
        assertEquals(0, mDecision.getHysteresisTimerToWiFi());
    }
}