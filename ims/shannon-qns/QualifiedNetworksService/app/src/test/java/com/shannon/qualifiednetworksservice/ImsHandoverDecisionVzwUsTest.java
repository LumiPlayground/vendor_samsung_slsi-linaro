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

import static org.junit.Assert.assertEquals;
import static org.mockito.ArgumentMatchers.any;
import static org.mockito.ArgumentMatchers.anyBoolean;
import static org.mockito.ArgumentMatchers.anyInt;
import static org.mockito.ArgumentMatchers.anyString;
import static org.mockito.Mockito.spy;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

@FixMethodOrder(MethodSorters.NAME_ASCENDING)
@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class ImsHandoverDecisionVzwUsTest {

    private ImsHandoverDecisionVzwUs mImsHandoverDecisionVzwUs;

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
        when(mockHandoverConditions.getProvisioningConditions()).thenReturn(mockProvisionedValues);
        when(mockHandoverConditions.getSlotIndex()).thenReturn(0);

        mImsHandoverDecisionVzwUs = new ImsHandoverDecisionVzwUs(mockHandoverConditions);
    }

    @After
    public void tearDown() {
        Mockito.validateMockitoUsage();
    }

    @Test
    public void getPreferredNetworkTest() {
        // Given
        // isWiFiTransportAvailable() returns true
        when(mockTransport.isTransportAvailable(anyInt(), any())).thenReturn(true);
        when(mockConfigurationManager.isEmcOverWiFiSupported()).thenReturn(true);
        when(mockConfigurationManager.isEmcHandoverAllowed()).thenReturn(true);

        // isRoaming = true
        when(mockCell.isRoaming()).thenReturn(true);

        // isCellReady = true iff 1 && 2 && 3
        // 1.
        when(mockCell.isRegistered()).thenReturn(true);

        // 2.
        //returns true isImsOnCell()---start
        when(mockUser.isVolteEnabled()).thenReturn(true);
        when(mockProvisionedValues.isVoLteProvisioned()).thenReturn(true);
        when(mockConfigurationManager.isVoLteVoPsRequired()).thenReturn(true);
        when(mockConfigurationManager.isVoLteVoPsNotRequiredActiveCall()).thenReturn(true);

        when(mockCell.isRoaming()).thenReturn(true);
        when(mockCell.isRoamedInternationally()).thenReturn(true);
        when(mockConfigurationManager.getAllowedRoamingNetworks()).thenReturn("LTE");
        when(mockCell.isAllowedNetwork(anyString())).thenReturn(true);
        when(mockConfigurationManager.isVoLteRoamingSupported()).thenReturn(true);
        when(mockConfigurationManager.getVoLteAllowedRat()).thenReturn("LTE");
        when(mockCell.isAllowedRat(anyString())).thenReturn(true);

        when(mockWiFi.isRoaming()).thenReturn(true);
        when(mockConfigurationManager.isWfcRoamingSupported()).thenReturn(true);
        // isImsOnCell()---end

        // 3.
        when(mockCell.isVoiceSupported()).thenReturn(true);

        // isWiFiReady = true iff 1 && 2
        // isWiFiConnected = true
        when(mockWiFi.isConnected()).thenReturn(true);
        //returns true isImsOnWiFi()---start
        when(mockCell.isRegistered()).thenReturn(true);
        when(mockUser.isWfcEnabledByPlatform()).thenReturn(true);
        when(mockProvisionedValues.isWfcProvisioned()).thenReturn(true);
        when(mockConfigurationManager.isWfcOemProvisioningRequired()).thenReturn(true);
        when(mockProvisionedValues.isMdnProvisioned()).thenReturn(true);
        // isImsOnWiFi()---end

        // isWiFiPreferred = true
        when(mockUser.isWfcEnabledByUser()).thenReturn(true);
        when(mockUser.isWiFiPreferred()).thenReturn(true);

        // isImsCallActive = true
        when(mockCall.isImsCallActive(anyBoolean())).thenReturn(true);

        // When
        mImsHandoverDecisionVzwUs.getPreferredNetwork(TelephonyManager.NETWORK_MODE_NR_LTE);
        // Then
        verify(mockCell, times(1)).getNetworkType();

        // When
        mImsHandoverDecisionVzwUs.getPreferredNetwork(TelephonyManager.NETWORK_TYPE_IWLAN);
        // Then
        verify(mockCell, times(1)).getNetworkType();

        // When
        mImsHandoverDecisionVzwUs.getPreferredNetwork(TelephonyManager.NETWORK_TYPE_UNKNOWN);
        // Then
        verify(mockCell, times(2)).getNetworkType();

        // Additional cases
        // 1.
        // Given
        // isRoaming = false
        when(mockCell.isRoaming()).thenReturn(false);
        // When
        mImsHandoverDecisionVzwUs.getPreferredNetwork(TelephonyManager.NETWORK_MODE_NR_LTE);
        // Then
        verify(mockCell, times(3)).getNetworkType();

        // 2.
        // Given
        // isRoaming = false
        when(mockCell.isRoaming()).thenReturn(false);
        // isWiFiReady = false
        // isWiFiConnected = false
        when(mockWiFi.isConnected()).thenReturn(false);

        // When
        mImsHandoverDecisionVzwUs.getPreferredNetwork(TelephonyManager.NETWORK_MODE_NR_LTE);
        // Then
        verify(mockCell, times(4)).getNetworkType();
        // When
        mImsHandoverDecisionVzwUs.getPreferredNetwork(TelephonyManager.NETWORK_TYPE_UNKNOWN);
        // Then
        verify(mockCell, times(5)).getNetworkType();
    }

    @Test
    public void getPreferredNetwork_isWiFiInCondition_Test() {
        // Given
        // isWiFiTransportAvailable() returns true
        when(mockTransport.isTransportAvailable(anyInt(), any())).thenReturn(true);
        when(mockConfigurationManager.isEmcOverWiFiSupported()).thenReturn(true);
        when(mockConfigurationManager.isEmcHandoverAllowed()).thenReturn(true);

        // isRoaming = true
        when(mockCell.isRoaming()).thenReturn(true);

        // isCellReady = true iff 1 && 2 && 3
        // 1.
        when(mockCell.isRegistered()).thenReturn(true);

        // 2.
        //returns true isImsOnCell()---start
        when(mockUser.isVolteEnabled()).thenReturn(true);
        when(mockProvisionedValues.isVoLteProvisioned()).thenReturn(true);
        when(mockConfigurationManager.isVoLteVoPsRequired()).thenReturn(true);
        when(mockConfigurationManager.isVoLteVoPsNotRequiredActiveCall()).thenReturn(true);

        when(mockCell.isRoaming()).thenReturn(true);
        when(mockCell.isRoamedInternationally()).thenReturn(true);
        when(mockConfigurationManager.getAllowedRoamingNetworks()).thenReturn("LTE");
        when(mockCell.isAllowedNetwork(anyString())).thenReturn(true);
        when(mockConfigurationManager.isVoLteRoamingSupported()).thenReturn(true);
        when(mockConfigurationManager.getVoLteAllowedRat()).thenReturn("LTE");
        when(mockCell.isAllowedRat(anyString())).thenReturn(true);

        when(mockWiFi.isRoaming()).thenReturn(true);
        when(mockConfigurationManager.isWfcRoamingSupported()).thenReturn(true);
        // isImsOnCell()---end

        // 3.
        when(mockCell.isVoiceSupported()).thenReturn(true);

        // isWiFiReady = true iff 1 && 2
        // isWiFiConnected = true
        when(mockWiFi.isConnected()).thenReturn(true);
        //returns true isImsOnWiFi()---start
        when(mockCell.isRegistered()).thenReturn(true);
        when(mockUser.isWfcEnabledByPlatform()).thenReturn(true);
        when(mockProvisionedValues.isWfcProvisioned()).thenReturn(true);
        when(mockConfigurationManager.isWfcOemProvisioningRequired()).thenReturn(true);
        when(mockProvisionedValues.isMdnProvisioned()).thenReturn(true);
        // isImsOnWiFi()---end

        // isWiFiPreferred = true
        when(mockUser.isWfcEnabledByUser()).thenReturn(true);
        when(mockUser.isWiFiPreferred()).thenReturn(true);

        // returns true isWiFiInCondition()---start
        when(mockCall.isVideoCallActive()).thenReturn(true);
        when(mockWiFi.isBetterThanThreshold(anyInt())).thenReturn(true);
        when(mockWiFi.isRoveIn()).thenReturn(true);
        when(mockCell.isRoveOut()).thenReturn(true);
        // isWiFiInCondition()---end

        // isImsCallActive = false
        when(mockCall.isImsCallActive(anyBoolean())).thenReturn(false);

        // When
        mImsHandoverDecisionVzwUs.getPreferredNetwork(TelephonyManager.NETWORK_MODE_NR_LTE);
        // Then
        verify(mockCell, times(0)).getNetworkType();

        // Additional case
        // 3. Set VoLTE as false
        when(mockCell.isVoiceSupported()).thenReturn(false);

        // When
        mImsHandoverDecisionVzwUs.getPreferredNetwork(TelephonyManager.NETWORK_MODE_NR_LTE);
        // Then
        verify(mockCell, times(0)).getNetworkType();
    }

    @Test
    public void getHysteresisTimerToLteTest() {
        // Default
        // When
        int test_default = mImsHandoverDecisionVzwUs.getHysteresisTimerToLte();
        // Then
        assertEquals(0, test_default);

        // Given
        when(mockUser.isWfcEnabledByUser()).thenReturn(false);
        when(mockUser.isWiFiPreferred()).thenReturn(false);
        when(mockCell.isRoaming()).thenReturn(true);
        when(mockConfigurationManager.getImsHysteresisTimerToLte()).thenReturn(1000);

        // When
        int test = mImsHandoverDecisionVzwUs.getHysteresisTimerToLte();
        // Then
        assertEquals(1000, test);
    }

    @Test
    public void getHysteresisTimerToWiFiTest() {
        // Default
        // When
        int test_default = mImsHandoverDecisionVzwUs.getHysteresisTimerToWiFi();
        // Then
        assertEquals(0, test_default);

        // Given
        when(mockUser.isWfcEnabledByUser()).thenReturn(true);
        when(mockUser.isWiFiPreferred()).thenReturn(true);
        when(mockCell.isRoaming()).thenReturn(true);
        when(mockConfigurationManager.getImsHysteresisTimerToWiFi()).thenReturn(1000);

        // When
        int test = mImsHandoverDecisionVzwUs.getHysteresisTimerToWiFi();
        // Then
        assertEquals(1000, test);
    }

    @Test
    public void getPreferredNetwork_isWiFiInCondition_getWiFiVideoIn_Test() {
        // Given
        spy(mockWiFi);
        when(mockTransport.isTransportAvailable(anyInt(), any())).thenReturn(true);
        when(mockConfigurationManager.isEmcOverWiFiSupported()).thenReturn(true);
        when(mockConfigurationManager.isEmcHandoverAllowed()).thenReturn(true);

        // isRoaming = true
        when(mockCell.isRoaming()).thenReturn(true);

        // isCellReady = true iff 1 && 2 && 3
        // 1.
        when(mockCell.isRegistered()).thenReturn(true);

        // 2.
        //returns true isImsOnCell()---start
        when(mockUser.isVolteEnabled()).thenReturn(true);
        when(mockProvisionedValues.isVoLteProvisioned()).thenReturn(true);
        when(mockConfigurationManager.isVoLteVoPsRequired()).thenReturn(true);
        when(mockConfigurationManager.isVoLteVoPsNotRequiredActiveCall()).thenReturn(true);

        when(mockCell.isRoaming()).thenReturn(true);
        when(mockCell.isRoamedInternationally()).thenReturn(true);
        when(mockConfigurationManager.getAllowedRoamingNetworks()).thenReturn("LTE");
        when(mockCell.isAllowedNetwork(anyString())).thenReturn(true);
        when(mockConfigurationManager.isVoLteRoamingSupported()).thenReturn(true);
        when(mockConfigurationManager.getVoLteAllowedRat()).thenReturn("LTE");
        when(mockCell.isAllowedRat(anyString())).thenReturn(true);

        when(mockWiFi.isRoaming()).thenReturn(true);
        when(mockConfigurationManager.isWfcRoamingSupported()).thenReturn(true);
        // isImsOnCell()---end

        // 3.
        when(mockCell.isVoiceSupported()).thenReturn(true);

        // isWiFiReady = true iff 1 && 2
        when(mockWiFi.isConnected()).thenReturn(true);
        //returns true isImsOnWiFi()---start
        when(mockUser.isWfcEnabledByPlatform()).thenReturn(true);
        when(mockProvisionedValues.isWfcProvisioned()).thenReturn(true);
        when(mockConfigurationManager.isWfcOemProvisioningRequired()).thenReturn(true);
        when(mockProvisionedValues.isMdnProvisioned()).thenReturn(true);
        // isImsOnWiFi()---end

        // isWiFiPreferred = true
        when(mockUser.isWfcEnabledByUser()).thenReturn(true);
        when(mockUser.isWiFiPreferred()).thenReturn(true);

        // isWiFiInCondition()---start
        when(mockCall.isVideoCallActive()).thenReturn(true);
        when(mockWiFi.isRoveIn()).thenReturn(false);
        when(mockCell.isRoveOut()).thenReturn(false);
        // isWiFiInCondition()---end

        // isImsCallActive = true
        when(mockCall.isImsCallActive(anyBoolean())).thenReturn(true);

        // When
        mImsHandoverDecisionVzwUs.getPreferredNetwork(TelephonyManager.NETWORK_MODE_NR_LTE);

        // Then
        verify(mockWiFi, times(1)).isBetterThanThreshold(anyInt());
    }

    @Test
    public void getPreferredNetwork_isWiFiInCondition_getWiFiVideoOut_Test() {
        // Given
        spy(mockWiFi);
        when(mockTransport.isTransportAvailable(anyInt(), any())).thenReturn(true);
        when(mockConfigurationManager.isEmcOverWiFiSupported()).thenReturn(true);
        when(mockConfigurationManager.isEmcHandoverAllowed()).thenReturn(true);

        when(mockCell.isRoaming()).thenReturn(false);

        // isCellReady = true iff 1 && 2 && 3
        // 1.
        when(mockCell.isRegistered()).thenReturn(true);

        // 2.
        //returns true isImsOnCell()---start
        when(mockUser.isVolteEnabled()).thenReturn(true);
        when(mockProvisionedValues.isVoLteProvisioned()).thenReturn(true);
        when(mockConfigurationManager.isVoLteVoPsRequired()).thenReturn(true);
        when(mockConfigurationManager.isVoLteVoPsNotRequiredActiveCall()).thenReturn(true);

        when(mockCell.isRoamedInternationally()).thenReturn(true);
        when(mockConfigurationManager.getAllowedRoamingNetworks()).thenReturn("LTE");
        when(mockCell.isAllowedNetwork(anyString())).thenReturn(true);
        when(mockConfigurationManager.isVoLteRoamingSupported()).thenReturn(true);
        when(mockConfigurationManager.getVoLteAllowedRat()).thenReturn("LTE");
        when(mockCell.isAllowedRat(anyString())).thenReturn(true);

        when(mockWiFi.isRoaming()).thenReturn(true);
        when(mockConfigurationManager.isWfcRoamingSupported()).thenReturn(true);
        // isImsOnCell()---end

        // 3.
        when(mockCell.isVoiceSupported()).thenReturn(true);

        // isWiFiReady = true iff 1 && 2
        when(mockWiFi.isConnected()).thenReturn(true);
        //returns true isImsOnWiFi()---start
        when(mockUser.isWfcEnabledByPlatform()).thenReturn(true);
        when(mockProvisionedValues.isWfcProvisioned()).thenReturn(true);
        when(mockConfigurationManager.isWfcOemProvisioningRequired()).thenReturn(true);
        when(mockProvisionedValues.isMdnProvisioned()).thenReturn(true);
        // isImsOnWiFi()---end

        // isWiFiPreferred = true
        when(mockUser.isWfcEnabledByUser()).thenReturn(true);
        when(mockUser.isWiFiPreferred()).thenReturn(true);

        // returns true isWiFiInCondition()---start
        when(mockCall.isVideoCallActive()).thenReturn(true);
        when(mockWiFi.isRoveIn()).thenReturn(false);
        when(mockCell.isRoveOut()).thenReturn(false);
        // isWiFiInCondition()---end

        // isImsCallActive = true
        when(mockCall.isImsCallActive(anyBoolean())).thenReturn(true);

        // When
        mImsHandoverDecisionVzwUs.getPreferredNetwork(TelephonyManager.NETWORK_TYPE_IWLAN);

        // Then
        verify(mockWiFi, times(1)).isLowerThanThreshold(anyInt());
    }

    @Test
    public void testRequestOrStopCellReports() {
        // Given
        when(mockUser.isWfcEnabled()).thenReturn(true);
        when(mockWiFi.isConnected()).thenReturn(true);

        // When
        mImsHandoverDecisionVzwUs.requestOrStopCellReports(TelephonyManager.NETWORK_TYPE_IWLAN,
                true);

        // Then
        verify(mockCell, times(1)).requestCellReports(anyBoolean(),
                anyBoolean(), anyBoolean(), anyBoolean(), anyBoolean());
    }

    @Test
    public void testRequestOrStopCellReports_whenWiFiPreferred() {
        // Given
        when(mockUser.isWfcEnabled()).thenReturn(true);
        when(mockUser.isWiFiPreferred()).thenReturn(true);
        when(mockWiFi.isConnected()).thenReturn(true);

        // When
        mImsHandoverDecisionVzwUs.requestOrStopCellReports(TelephonyManager.NETWORK_TYPE_IWLAN,
                true);

        // Then
        verify(mockCell, times(1)).requestCellReports(anyBoolean(),
                anyBoolean(), anyBoolean(), anyBoolean(), anyBoolean());
    }

    @Test
    public void testRequestOrStopCellReports_whenWiFiPreferred_and_current_is_cellular() {
        // Given
        when(mockUser.isWfcEnabled()).thenReturn(true);
        when(mockUser.isWiFiPreferred()).thenReturn(true);
        when(mockWiFi.isConnected()).thenReturn(true);

        // When
        mImsHandoverDecisionVzwUs.requestOrStopCellReports(TelephonyManager.NETWORK_TYPE_LTE,
                true);

        // Then
        verify(mockCell, times(1)).requestCellReports(anyBoolean(),
                anyBoolean(), anyBoolean(), anyBoolean(), anyBoolean());
    }

    @Test
    public void testRequestOrStopCellReports_whenCellularPreferred_and_current_is_cellular() {
        // Given
        when(mockUser.isWfcEnabled()).thenReturn(true);
        when(mockUser.isWiFiPreferred()).thenReturn(false);
        when(mockWiFi.isConnected()).thenReturn(true);

        // When
        mImsHandoverDecisionVzwUs.requestOrStopCellReports(TelephonyManager.NETWORK_TYPE_LTE,
                true);

        // Then
        verify(mockCell, times(1)).requestCellReports(anyBoolean(),
                anyBoolean(), anyBoolean(), anyBoolean(), anyBoolean());
    }

    @Test
    public void testRequestOrStopCellReports_ExceptionalCase() {
        // When
        mImsHandoverDecisionVzwUs.requestOrStopCellReports(TelephonyManager.NETWORK_TYPE_IWLAN,
                true);

        // Then
        verify(mockCell, times(1)).stopCellReports();
    }
}
