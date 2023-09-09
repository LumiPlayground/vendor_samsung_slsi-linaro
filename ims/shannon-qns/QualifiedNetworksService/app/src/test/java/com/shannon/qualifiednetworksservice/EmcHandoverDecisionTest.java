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

import android.telephony.TelephonyManager;
import com.shannon.qualifiednetworksservice.HandoverConditions.ConditionEvents;
import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.FixMethodOrder;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.MethodSorters;
import org.mockito.Mock;
import org.mockito.Mockito;
import org.mockito.MockitoAnnotations;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.annotation.Config;

import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;
import static org.mockito.ArgumentMatchers.any;
import static org.mockito.ArgumentMatchers.anyInt;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

@FixMethodOrder(MethodSorters.NAME_ASCENDING)
@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class EmcHandoverDecisionTest {
    private EmcHandoverDecision mEmcHandoverDecision;

    @Mock
    private HandoverConditions mockHandoverConditions;
    @Mock
    private ConditionCall mockCall;
    @Mock
    private ConditionEvents mockConditionEvents;
    @Mock
    private ConditionConnectivity mockTransport;
    @Mock
    private ConfigurationManager mockConfigurationManager;

    @Mock
    private ConditionCellular mockCell;
    @Mock
    private ConditionWiFi mockWiFi;
    @Mock
    private ConditionUserSetting mockUser;
    @Mock
    private ConditionProvisioning mockProvisionedValues;

    @BeforeClass
    public static void beforeClass() {
    }

    @AfterClass
    public static void afterClass() {
    }

    @Before
    public void setUp() {
        MockitoAnnotations.openMocks(this);
        when(mockHandoverConditions.getSlotIndex()).thenReturn(1);
        when(mockHandoverConditions.getCallConditions()).thenReturn(mockCall);
        when(mockHandoverConditions.getTransportCondition()).thenReturn(mockTransport);
        when(mockHandoverConditions.getConfigurationManager()).thenReturn(mockConfigurationManager);
        when(mockHandoverConditions.getCellularConditions()).thenReturn(mockCell);
        when(mockHandoverConditions.getWiFiConditions()).thenReturn(mockWiFi);
        when(mockHandoverConditions.getUserConditions()).thenReturn(mockUser);

        mEmcHandoverDecision = new EmcHandoverDecision(mockHandoverConditions);

    }

    @After
    public void tearDown() {
        Mockito.validateMockitoUsage();
    }

    @Test
    public void getPreferredList_Cellular_Test() {
        // Given
        when(mockTransport.getNetworkType(anyInt())).thenReturn(TelephonyManager.NETWORK_TYPE_IWLAN);
        when(mockConfigurationManager.isEmcOverWiFiSupported()).thenReturn(true);
        when(mockConfigurationManager.isEmcHandoverAllowed()).thenReturn(true);

        //returns true isEmcOverWiFiEnabled()---start
        when(mockUser.isWfcEnabled()).thenReturn(true);
        when(mockConfigurationManager.isEmcOemProvisioningRequired()).thenReturn(true);
        when(mockHandoverConditions.getProvisioningConditions()).thenReturn(mockProvisionedValues);
        when(mockProvisionedValues.isOemProvisioned()).thenReturn(true);
        when(mockCell.isRoaming()).thenReturn(true);
        when(mockCell.isRoamedInternationally()).thenReturn(true);
        when(mockWiFi.isRoaming()).thenReturn(true);
        when(mockConfigurationManager.isEmcOverWiFiSupportedRoaming()).thenReturn(true);
        when(mockTransport.isTransportAvailable(anyInt(), any())).thenReturn(true);
        // isEmcOverWiFiEnabled()---end

        when(mockConfigurationManager.isEmcHandoverAllowedAtRoaming()).thenReturn(true);
        when(mockCell.isLTE()).thenReturn(true);

        // isEmcOverCellSupported()
        when(mockCell.isConnected()).thenReturn(true);
        when(mockCell.isEmergencySupported()).thenReturn(true);
        when(mockCall.isCallFailEmcRedialToIms()).thenReturn(true);

        // returns true isWiFiOutCondition()---start
        when(mockCell.getRsrp()).thenReturn(-100);
        when(mockConfigurationManager.getCellEmcRsrpMid()).thenReturn(-120);
        when(mockWiFi.getRssi()).thenReturn(-100);
        when(mockConfigurationManager.getWiFiEmcRoveOut()).thenReturn(-80);
        when(mockConfigurationManager.getCellEmcRoveIn()).thenReturn(-100);
        // isWiFiOutCondition()---end

        // When
        mEmcHandoverDecision.getPreferredList(mockConditionEvents);

        // Then
        verify(mockCell, times(1)).getNetworkType();

        // Given
        when(mockTransport.getNetworkType(anyInt())).thenReturn(TelephonyManager.NETWORK_TYPE_LTE);
        when(mockWiFi.isConnected()).thenReturn(true);

        // returns true isWiFiInCondition()---start
        when(mockWiFi.getRssi()).thenReturn(-100);
        when(mockConfigurationManager.getWiFiEmcRoveIn()).thenReturn(-120);
        when(mockCell.getRsrp()).thenReturn(-100);
        when(mockConfigurationManager.getCellEmcRoveOut()).thenReturn(-80);
        // isWiFiInCondition()---end

        // returns true isEmcHandoverAllowedToRat()---start
        when(mockConfigurationManager.getEmcHandoverAllowedRatList()).thenReturn("NR, LTE, IWLAN");
        // isEmcHandoverAllowedToRat()---end

        // When
        mEmcHandoverDecision.getPreferredList(mockConditionEvents);

        // Then
        verify(mockCell, times(1)).getNetworkType();
    }

    @Test
    public void getPreferredList_WiFi_Test() {
        // Given
        when(mockTransport.getNetworkType(anyInt())).thenReturn(TelephonyManager.NETWORK_TYPE_IWLAN);
        when(mockConfigurationManager.isEmcOverWiFiSupported()).thenReturn(true);
        // isHandoverAllowed false
        when(mockConfigurationManager.isEmcHandoverAllowed()).thenReturn(false);

        when(mockConfigurationManager.isEmcHandoverAllowedAtRoaming()).thenReturn(false);
        when(mockCell.isLTE()).thenReturn(false);

        // returns false isEmcHandoverAllowedToRat()---start
        when(mockConfigurationManager.getEmcHandoverAllowedRatList()).thenReturn("");
        // isEmcHandoverAllowedToRat()---end

        // When
        mEmcHandoverDecision.getPreferredList(mockConditionEvents);

        // Then
        verify(mockCell, times(0)).getNetworkType();

        // Given
        //returns true isEmcOverWiFiEnabled()---start
        when(mockUser.isWfcEnabled()).thenReturn(true);
        when(mockConfigurationManager.isEmcOemProvisioningRequired()).thenReturn(true);
        when(mockHandoverConditions.getProvisioningConditions()).thenReturn(mockProvisionedValues);
        when(mockProvisionedValues.isOemProvisioned()).thenReturn(true);
        when(mockCell.isRoaming()).thenReturn(true);
        when(mockCell.isRoamedInternationally()).thenReturn(true);
        when(mockWiFi.isRoaming()).thenReturn(true);
        when(mockConfigurationManager.isEmcOverWiFiSupportedRoaming()).thenReturn(true);
        when(mockTransport.isTransportAvailable(anyInt(), any())).thenReturn(true);
        // isEmcOverWiFiEnabled()---end

        // isEmcRedialOverWiFi
        when(mockCall.isCallFailEmcRedialToWiFi()).thenReturn(true);

        when(mockWiFi.isConnected()).thenReturn(true);

        // When
        mEmcHandoverDecision.getPreferredList(mockConditionEvents);

        // Then
        verify(mockCell, times(0)).getNetworkType();
    }

    @Test
    public void isServiceSupportedTest(){
        // Given
        when(mockCell.isConnected()).thenReturn(false);

        // When
        boolean test_false = mEmcHandoverDecision.isServiceSupported(TelephonyManager.NETWORK_TYPE_LTE);

        // Then
        assertFalse(test_false);

        // Given
        when(mockTransport.getNetworkType(anyInt())).thenReturn(TelephonyManager.NETWORK_TYPE_IWLAN);

        //returns true isEmcOverWiFiEnabled()---start
        when(mockUser.isWfcEnabled()).thenReturn(true);
        when(mockConfigurationManager.isEmcOemProvisioningRequired()).thenReturn(true);
        when(mockHandoverConditions.getProvisioningConditions()).thenReturn(mockProvisionedValues);
        when(mockProvisionedValues.isOemProvisioned()).thenReturn(true);
        when(mockCell.isRoaming()).thenReturn(true);
        when(mockCell.isRoamedInternationally()).thenReturn(true);
        when(mockWiFi.isRoaming()).thenReturn(true);
        when(mockConfigurationManager.isEmcOverWiFiSupportedRoaming()).thenReturn(true);
        when(mockTransport.isTransportAvailable(anyInt(), any())).thenReturn(true);
        // isEmcOverWiFiEnabled()---end

        when(mockWiFi.isConnected()).thenReturn(true);

        // When
        boolean test_true = mEmcHandoverDecision.isServiceSupported(TelephonyManager.NETWORK_TYPE_IWLAN);

        // Then
        assertTrue(test_true);
    }

}
