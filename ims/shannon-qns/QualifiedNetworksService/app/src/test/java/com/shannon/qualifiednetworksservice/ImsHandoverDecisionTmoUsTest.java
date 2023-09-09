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
import com.shannon.qualifiednetworksservice.Transport.TransportType;
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
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertThat;
import static org.junit.Assert.assertTrue;
import static org.mockito.ArgumentMatchers.any;
import static org.mockito.ArgumentMatchers.anyBoolean;
import static org.mockito.ArgumentMatchers.anyInt;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

@FixMethodOrder(MethodSorters.NAME_ASCENDING)
@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class ImsHandoverDecisionTmoUsTest {
    private ImsHandoverDecisionTmoUs mImsHandoverDecisionTmoUs;

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

        mImsHandoverDecisionTmoUs = new ImsHandoverDecisionTmoUs(mockHandoverConditions);
    }

    @After
    public void tearDown() {
        Mockito.validateMockitoUsage();
    }

    @Test
    public void getPreferredNetwork_Cellular_Test() {
        // Given
        when(mockTransport.isTransportAvailable(anyInt(), any())).thenReturn(true);
        when(mockConfigurationManager.isEmcOverWiFiSupported()).thenReturn(true);
        when(mockConfigurationManager.isEmcHandoverAllowed()).thenReturn(true);

        //returns true isImsOnWiFi()---start
        when(mockUser.isWfcEnabled()).thenReturn(true);
        when(mockProvisionedValues.isWfcProvisioned()).thenReturn(true);
        when(mockConfigurationManager.isEmcOemProvisioningRequired()).thenReturn(true);
        when(mockProvisionedValues.isOemProvisioned()).thenReturn(true);
        when(mockWiFi.isRoaming()).thenReturn(true);
        when(mockConfigurationManager.isWfcRoamingSupported()).thenReturn(true);
        // isImsOnWiFi()---end

        // returns true isImsOnCell()---start
        when(mockUser.isVolteEnabled()).thenReturn(true);
        when(mockProvisionedValues.isVoLteProvisioned()).thenReturn(true);
        when(mockConfigurationManager.isVoLteVoPsRequired()).thenReturn(false);
        when(mockConfigurationManager.isVoLteVoPsNotRequiredActiveCall()).thenReturn(false);
        // isImsOnCell()---end

        when(mockTransport.getNetworkType(anyInt())).thenReturn(TelephonyManager.NETWORK_TYPE_IWLAN);

        when(mockCell.isVoiceSupported()).thenReturn(true);
        when(mockConfigurationManager.getVoLteAllowedRat()).thenReturn("LTE");
        when(mockCell.isAllowedRat(any())).thenReturn(true);

        when(mockCell.isRegistered()).thenReturn(true);
        // Set airplane mode false
        when(mockCell.isAirplaneMode()).thenReturn(false);

        when(mockUser.isWiFiPreferred()).thenReturn(true);
        // Set WiFi off
        when(mockWiFi.isConnected()).thenReturn(false);
        when(mockWiFi.isRoveOut()).thenReturn(true);

        // When
        mImsHandoverDecisionTmoUs.getPreferredNetwork(TelephonyManager.NETWORK_TYPE_IWLAN);

        // Then
        verify(mockCell, times(1)).getNetworkType();
    }

    @Test
    public void getPreferredNetwork_APM_ON_IWLAN_Test() {
        // Given
        when(mockTransport.isTransportAvailable(anyInt(), any())).thenReturn(true);
        when(mockConfigurationManager.isEmcOverWiFiSupported()).thenReturn(true);
        when(mockConfigurationManager.isEmcHandoverAllowed()).thenReturn(true);

        when(mockTransport.getNetworkType(anyInt())).thenReturn(TelephonyManager.NETWORK_TYPE_IWLAN);

        //returns true isImsOnWiFi()---start
        when(mockUser.isWfcEnabled()).thenReturn(true);
        when(mockProvisionedValues.isWfcProvisioned()).thenReturn(true);
        when(mockConfigurationManager.isEmcOemProvisioningRequired()).thenReturn(true);
        when(mockProvisionedValues.isOemProvisioned()).thenReturn(true);
        when(mockWiFi.isRoaming()).thenReturn(true);
        when(mockConfigurationManager.isWfcRoamingSupported()).thenReturn(true);
        // isImsOnWiFi()---end

        // Set airplane mode true
        when(mockCell.isAirplaneMode()).thenReturn(true);
        // Set WiFi ON
        when(mockWiFi.isConnected()).thenReturn(true);

        // When
        mImsHandoverDecisionTmoUs.getPreferredNetwork(TelephonyManager.NETWORK_TYPE_IWLAN);

        // Then
        verify(mockCell, times(0)).getNetworkType();
    }

    @Test
    public void getPreferredNetwork_current_UNKNOWN_Preferred_Cellular_Test() {
        // Given
        when(mockTransport.isTransportAvailable(anyInt(), any())).thenReturn(true);
        when(mockConfigurationManager.isEmcOverWiFiSupported()).thenReturn(true);
        when(mockConfigurationManager.isEmcHandoverAllowed()).thenReturn(true);

        //returns true isImsOnWiFi()---start
        when(mockUser.isWfcEnabled()).thenReturn(true);
        when(mockProvisionedValues.isWfcProvisioned()).thenReturn(true);
        when(mockConfigurationManager.isEmcOemProvisioningRequired()).thenReturn(true);
        when(mockProvisionedValues.isOemProvisioned()).thenReturn(true);
        when(mockWiFi.isRoaming()).thenReturn(true);
        when(mockConfigurationManager.isWfcRoamingSupported()).thenReturn(true);
        // isImsOnWiFi()---end

        // returns true isImsOnCell()---start
        when(mockUser.isVolteEnabled()).thenReturn(true);
        when(mockProvisionedValues.isVoLteProvisioned()).thenReturn(true);
        when(mockConfigurationManager.isVoLteVoPsRequired()).thenReturn(false);
        when(mockConfigurationManager.isVoLteVoPsNotRequiredActiveCall()).thenReturn(false);
        // isImsOnCell()---end

        when(mockTransport.getNetworkType(anyInt())).thenReturn(TelephonyManager.NETWORK_TYPE_IWLAN);

        when(mockCell.isVoiceSupported()).thenReturn(true);
        when(mockConfigurationManager.getVoLteAllowedRat()).thenReturn("LTE");
        when(mockCell.isAllowedRat(any())).thenReturn(true);

        when(mockCell.isRegistered()).thenReturn(true);
        // Set airplane mode false
        when(mockCell.isAirplaneMode()).thenReturn(false);

        when(mockUser.isWiFiPreferred()).thenReturn(true);

        // isDisconnectedFromNetwork false
        //when(mockCell.isDisconnectedFromNetwork()).thenReturn(false);

        // Set WiFi on
        // isWiFiConnected
        when(mockWiFi.isConnected()).thenReturn(true);
        when(mockWiFi.isRoveOut()).thenReturn(true);

        // When
        mImsHandoverDecisionTmoUs.getPreferredNetwork(TelephonyManager.NETWORK_MODE_NR_LTE);

        // Then
        verify(mockCell, times(1)).getNetworkType();
    }

    @Test
    public void getPreferredNetwork_current_IWLAN_Preferred_LTE_Test() {
        // Given
        when(mockTransport.isTransportAvailable(anyInt(), any())).thenReturn(true);
        when(mockConfigurationManager.isEmcOverWiFiSupported()).thenReturn(true);
        when(mockConfigurationManager.isEmcHandoverAllowed()).thenReturn(true);

        //returns true isImsOnWiFi()---start
        when(mockUser.isWfcEnabled()).thenReturn(true);
        when(mockProvisionedValues.isWfcProvisioned()).thenReturn(true);
        when(mockConfigurationManager.isEmcOemProvisioningRequired()).thenReturn(true);
        when(mockProvisionedValues.isOemProvisioned()).thenReturn(true);
        when(mockWiFi.isRoaming()).thenReturn(true);
        when(mockConfigurationManager.isWfcRoamingSupported()).thenReturn(true);
        // isImsOnWiFi()---end

        // returns true isImsOnCell()---start
        when(mockUser.isVolteEnabled()).thenReturn(true);
        when(mockProvisionedValues.isVoLteProvisioned()).thenReturn(true);
        when(mockConfigurationManager.isVoLteVoPsRequired()).thenReturn(false);
        when(mockConfigurationManager.isVoLteVoPsNotRequiredActiveCall()).thenReturn(false);
        // isImsOnCell()---end

        when(mockTransport.getNetworkType(anyInt())).thenReturn(TelephonyManager.NETWORK_TYPE_IWLAN);

        when(mockCell.isVoiceSupported()).thenReturn(true);
        when(mockConfigurationManager.getVoLteAllowedRat()).thenReturn("LTE");
        when(mockCell.isAllowedRat(any())).thenReturn(true);

        when(mockCell.isRegistered()).thenReturn(false);
        // Set airplane mode false
        when(mockCell.isAirplaneMode()).thenReturn(false);

        when(mockUser.isWiFiPreferred()).thenReturn(true);

        // isDisconnectedFromNetwork false
        when(mockCell.isDisconnectedFromNetwork()).thenReturn(false);

        // Set WiFi on
        // isWiFiConnected
        when(mockWiFi.isConnected()).thenReturn(true);
        when(mockWiFi.isRoveOut()).thenReturn(true);

        // When
        mImsHandoverDecisionTmoUs.getPreferredNetwork(TelephonyManager.NETWORK_TYPE_IWLAN);

        // Then
        verify(mockCell, times(0)).getNetworkType();
    }

    @Test
    public void getPreferredNetwork_current_NR_Preferred_IWLAN_Test() {
        // Given
        when(mockTransport.isTransportAvailable(anyInt(), any())).thenReturn(true);
        when(mockConfigurationManager.isEmcOverWiFiSupported()).thenReturn(true);
        when(mockConfigurationManager.isEmcHandoverAllowed()).thenReturn(true);

        //returns true isImsOnWiFi()---start
        when(mockUser.isWfcEnabled()).thenReturn(true);
        when(mockProvisionedValues.isWfcProvisioned()).thenReturn(true);
        when(mockConfigurationManager.isEmcOemProvisioningRequired()).thenReturn(true);
        when(mockProvisionedValues.isOemProvisioned()).thenReturn(true);
        when(mockWiFi.isRoaming()).thenReturn(true);
        when(mockConfigurationManager.isWfcRoamingSupported()).thenReturn(true);
        // isImsOnWiFi()---end

        // returns true isImsOnCell()---start
        when(mockUser.isVolteEnabled()).thenReturn(true);
        when(mockProvisionedValues.isVoLteProvisioned()).thenReturn(true);
        when(mockConfigurationManager.isVoLteVoPsRequired()).thenReturn(false);
        when(mockConfigurationManager.isVoLteVoPsNotRequiredActiveCall()).thenReturn(false);
        // isImsOnCell()---end

        when(mockTransport.getNetworkType(anyInt())).thenReturn(TelephonyManager.NETWORK_TYPE_IWLAN);

        when(mockCell.isVoiceSupported()).thenReturn(true);
        when(mockConfigurationManager.getVoLteAllowedRat()).thenReturn("LTE");
        when(mockCell.isAllowedRat(any())).thenReturn(true);

        // isCellReady false
        when(mockCell.isRegistered()).thenReturn(false);
        // Set airplane mode false
        when(mockCell.isAirplaneMode()).thenReturn(false);

        when(mockUser.isWiFiPreferred()).thenReturn(true);

        // isDisconnectedFromNetwork false
        when(mockCell.isDisconnectedFromNetwork()).thenReturn(false);

        // Set WiFi on
        // isWiFiConnected
        when(mockWiFi.isConnected()).thenReturn(true);
        when(mockWiFi.isRoveIn()).thenReturn(true);

        // When
        mImsHandoverDecisionTmoUs.getPreferredNetwork(TelephonyManager.NETWORK_MODE_NR_LTE);

        // Then
        verify(mockCell, times(0)).getNetworkType();
    }

    @Test
    public void getPreferredNetwork_current_IWLAN_Preferred_IWLAN_Test() {
        // Given
        when(mockTransport.isTransportAvailable(anyInt(), any())).thenReturn(true);
        when(mockConfigurationManager.isEmcOverWiFiSupported()).thenReturn(true);
        when(mockConfigurationManager.isEmcHandoverAllowed()).thenReturn(true);

        //returns true isImsOnWiFi()---start
        when(mockUser.isWfcEnabled()).thenReturn(true);
        when(mockProvisionedValues.isWfcProvisioned()).thenReturn(true);
        when(mockConfigurationManager.isEmcOemProvisioningRequired()).thenReturn(true);
        when(mockProvisionedValues.isOemProvisioned()).thenReturn(true);
        when(mockWiFi.isRoaming()).thenReturn(true);
        when(mockConfigurationManager.isWfcRoamingSupported()).thenReturn(true);
        // isImsOnWiFi()---end

        // returns true isImsOnCell()---start
        when(mockUser.isVolteEnabled()).thenReturn(true);
        when(mockProvisionedValues.isVoLteProvisioned()).thenReturn(true);
        when(mockConfigurationManager.isVoLteVoPsRequired()).thenReturn(false);
        when(mockConfigurationManager.isVoLteVoPsNotRequiredActiveCall()).thenReturn(false);
        // isImsOnCell()---end

        when(mockTransport.getNetworkType(anyInt())).thenReturn(TelephonyManager.NETWORK_TYPE_IWLAN);

        when(mockCell.isVoiceSupported()).thenReturn(true);
        when(mockConfigurationManager.getVoLteAllowedRat()).thenReturn("LTE");
        when(mockCell.isAllowedRat(any())).thenReturn(true);

        // isCellReady false
        when(mockCell.isRegistered()).thenReturn(true);
        // Set airplane mode false
        when(mockCell.isAirplaneMode()).thenReturn(false);

        when(mockUser.isWiFiPreferred()).thenReturn(false);

        // isDisconnectedFromNetwork false
        when(mockCell.isDisconnectedFromNetwork()).thenReturn(false);

        // Set WiFi on
        // isWiFiConnected
        when(mockWiFi.isConnected()).thenReturn(true);
        when(mockWiFi.isRoveIn()).thenReturn(true);

        // When
        mImsHandoverDecisionTmoUs.getPreferredNetwork(TelephonyManager.NETWORK_TYPE_IWLAN);

        // Then
        verify(mockCell, times(0)).getNetworkType();
    }

    @Test
    public void getPreferredNetwork_NR_Preferred_NR_Test() {
        // Given
        when(mockTransport.isTransportAvailable(anyInt(), any())).thenReturn(true);
        when(mockConfigurationManager.isEmcOverWiFiSupported()).thenReturn(true);
        when(mockConfigurationManager.isEmcHandoverAllowed()).thenReturn(true);

        //returns true isImsOnWiFi()---start
        when(mockUser.isWfcEnabled()).thenReturn(true);
        when(mockProvisionedValues.isWfcProvisioned()).thenReturn(true);
        when(mockConfigurationManager.isEmcOemProvisioningRequired()).thenReturn(true);
        when(mockProvisionedValues.isOemProvisioned()).thenReturn(true);
        when(mockWiFi.isRoaming()).thenReturn(true);
        when(mockConfigurationManager.isWfcRoamingSupported()).thenReturn(true);
        // isImsOnWiFi()---end

        // returns true isImsOnCell()---start
        when(mockUser.isVolteEnabled()).thenReturn(true);
        when(mockProvisionedValues.isVoLteProvisioned()).thenReturn(true);
        when(mockConfigurationManager.isVoLteVoPsRequired()).thenReturn(false);
        when(mockConfigurationManager.isVoLteVoPsNotRequiredActiveCall()).thenReturn(false);
        // isImsOnCell()---end

        when(mockTransport.getNetworkType(anyInt())).thenReturn(TelephonyManager.NETWORK_TYPE_IWLAN);

        when(mockCell.isVoiceSupported()).thenReturn(true);
        when(mockConfigurationManager.getVoLteAllowedRat()).thenReturn("LTE");
        when(mockCell.isAllowedRat(any())).thenReturn(true);

        // isCellReady true
        when(mockCell.isRegistered()).thenReturn(true);
        // Set airplane mode false
        when(mockCell.isAirplaneMode()).thenReturn(false);

        when(mockUser.isWiFiPreferred()).thenReturn(true);

        // isDisconnectedFromNetwork false
        when(mockCell.isDisconnectedFromNetwork()).thenReturn(false);

        // Set WiFi on
        // isWiFiConnected
        when(mockWiFi.isRoveIn()).thenReturn(false);
        when(mockWiFi.isRoveOut()).thenReturn(true);

        // When
        mImsHandoverDecisionTmoUs.getPreferredNetwork(TelephonyManager.NETWORK_MODE_NR_LTE);

        // Then
        verify(mockCell, times(1)).getNetworkType();
    }

    @Test
    public void getPreferredNetwork_Cellular_WiFi_notPreferred_Test() {
        // Given
        when(mockTransport.isTransportAvailable(anyInt(), any())).thenReturn(true);
        when(mockConfigurationManager.isEmcOverWiFiSupported()).thenReturn(true);
        when(mockConfigurationManager.isEmcHandoverAllowed()).thenReturn(true);

        //returns true isImsOnWiFi()---start
        when(mockUser.isWfcEnabled()).thenReturn(true);
        when(mockProvisionedValues.isWfcProvisioned()).thenReturn(true);
        when(mockConfigurationManager.isEmcOemProvisioningRequired()).thenReturn(true);
        when(mockProvisionedValues.isOemProvisioned()).thenReturn(true);
        when(mockWiFi.isRoaming()).thenReturn(true);
        when(mockConfigurationManager.isWfcRoamingSupported()).thenReturn(true);
        // isImsOnWiFi()---end

        // returns true isImsOnCell()---start
        when(mockUser.isVolteEnabled()).thenReturn(true);
        when(mockProvisionedValues.isVoLteProvisioned()).thenReturn(true);
        when(mockConfigurationManager.isVoLteVoPsRequired()).thenReturn(false);
        when(mockConfigurationManager.isVoLteVoPsNotRequiredActiveCall()).thenReturn(false);
        // isImsOnCell()---end

        when(mockTransport.getNetworkType(anyInt())).thenReturn(TelephonyManager.NETWORK_TYPE_IWLAN);

        when(mockCell.isVoiceSupported()).thenReturn(true);
        when(mockConfigurationManager.getVoLteAllowedRat()).thenReturn("LTE");
        when(mockCell.isAllowedRat(any())).thenReturn(true);

        when(mockCell.isRegistered()).thenReturn(true);
        // Set airplane mode false
        when(mockCell.isAirplaneMode()).thenReturn(false);

        when(mockUser.isWiFiPreferred()).thenReturn(false);
        // Set WiFi off
        when(mockWiFi.isConnected()).thenReturn(false);
        when(mockWiFi.isRoveIn()).thenReturn(true);
        when(mockWiFi.isRoveOut()).thenReturn(true);

        // When
        mImsHandoverDecisionTmoUs.getPreferredNetwork(TelephonyManager.NETWORK_TYPE_IWLAN);

        // Then
        verify(mockCell, times(1)).getNetworkType();
    }


    @Test
    public void getPreferredNetwork_Cellular_WiFi_notPreferredTest() {
        // Given
        when(mockTransport.isTransportAvailable(anyInt(), any())).thenReturn(true);
        when(mockConfigurationManager.isEmcOverWiFiSupported()).thenReturn(true);
        when(mockConfigurationManager.isEmcHandoverAllowed()).thenReturn(true);

        //returns true isImsOnWiFi()---start
        when(mockUser.isWfcEnabled()).thenReturn(true);
        when(mockProvisionedValues.isWfcProvisioned()).thenReturn(true);
        when(mockConfigurationManager.isEmcOemProvisioningRequired()).thenReturn(true);
        when(mockProvisionedValues.isOemProvisioned()).thenReturn(true);
        when(mockWiFi.isRoaming()).thenReturn(true);
        when(mockConfigurationManager.isWfcRoamingSupported()).thenReturn(true);
        // isImsOnWiFi()---end

        // returns true isImsOnCell()---start
        when(mockUser.isVolteEnabled()).thenReturn(true);
        when(mockProvisionedValues.isVoLteProvisioned()).thenReturn(true);
        when(mockConfigurationManager.isVoLteVoPsRequired()).thenReturn(false);
        when(mockConfigurationManager.isVoLteVoPsNotRequiredActiveCall()).thenReturn(false);
        // isImsOnCell()---end

        when(mockTransport.getNetworkType(anyInt())).thenReturn(TelephonyManager.NETWORK_TYPE_IWLAN);

        when(mockCell.isVoiceSupported()).thenReturn(true);
        when(mockConfigurationManager.getVoLteAllowedRat()).thenReturn("LTE");
        when(mockCell.isAllowedRat(any())).thenReturn(true);

        when(mockCell.isRegistered()).thenReturn(true);
        // Set airplane mode false
        when(mockCell.isAirplaneMode()).thenReturn(false);

        when(mockUser.isWiFiPreferred()).thenReturn(false);
        // Set WiFi off
        when(mockWiFi.isConnected()).thenReturn(false);
        when(mockWiFi.isRoveIn()).thenReturn(true);
        when(mockWiFi.isRoveOut()).thenReturn(true);

        // When
        mImsHandoverDecisionTmoUs.getPreferredNetwork(TelephonyManager.NETWORK_MODE_NR_LTE);

        // Then
        verify(mockCell, times(1)).getNetworkType();

        // Given
        when(mockCell.isRegistered()).thenReturn(false);

        // When
        mImsHandoverDecisionTmoUs.getPreferredNetwork(TelephonyManager.NETWORK_MODE_NR_LTE);

        // Then
        verify(mockCell, times(1)).getNetworkType();
    }

    @Test
    public void getPreferredNetwork_WiFi_Calling_Off_Test() {
        // Given
        when(mockTransport.isTransportAvailable(anyInt(), any())).thenReturn(true);
        when(mockConfigurationManager.isEmcOverWiFiSupported()).thenReturn(true);
        when(mockConfigurationManager.isEmcHandoverAllowed()).thenReturn(true);

        // returns true isImsOnCell()---start
        when(mockUser.isVolteEnabled()).thenReturn(true);
        when(mockProvisionedValues.isVoLteProvisioned()).thenReturn(true);
        when(mockConfigurationManager.isVoLteVoPsRequired()).thenReturn(false);
        when(mockConfigurationManager.isVoLteVoPsNotRequiredActiveCall()).thenReturn(false);
        // isImsOnCell()---end

        when(mockTransport.getNetworkType(anyInt())).thenReturn(TelephonyManager.NETWORK_TYPE_IWLAN);

        when(mockCell.isVoiceSupported()).thenReturn(true);
        when(mockConfigurationManager.getVoLteAllowedRat()).thenReturn("LTE");
        when(mockCell.isAllowedRat(any())).thenReturn(true);

        when(mockCell.isRegistered()).thenReturn(true);
        // Set airplane mode false
        when(mockCell.isAirplaneMode()).thenReturn(false);

        when(mockUser.isWiFiPreferred()).thenReturn(true);
        // Set WiFi off
        when(mockWiFi.isConnected()).thenReturn(false);
        when(mockWiFi.isRoveOut()).thenReturn(true);

        // When
        mImsHandoverDecisionTmoUs.getPreferredNetwork(TelephonyManager.NETWORK_TYPE_IWLAN);

        // Then
        verify(mockCell, times(1)).getNetworkType();
    }

    @Test
    public void getPreferredNetwork_ID_MTRREQ_480604() {
        /**
         *  Cellular and Wi-Fi Preferred Logic - NR and LTE (Mandatory) ID: GID-MTRREQ-480604
         *  Condition: 1) Cellular Preferred, 2) WiFi & LTE is rove out, 3) VoWiFi registered
         *  Behavior: select(keep) IWLAN network as preferred.
         */
        // Given
        int current = TelephonyManager.NETWORK_TYPE_IWLAN;
        when(mockUser.isWiFiPreferred()).thenReturn(false);
        when(mockWiFi.isConnected()).thenReturn(true);
        when(mockWiFi.isRoveIn()).thenReturn(false);
        when(mockWiFi.isRoveOut()).thenReturn(true);
        when(mockCell.isRegistered()).thenReturn(false);
        when(mockCell.isDisconnectedFromNetwork()).thenReturn(false);

        when(mockCell.isVoiceSupported()).thenReturn(true);
        when(mockConfigurationManager.getVoLteAllowedRat()).thenReturn("LTE");
        when(mockCell.isAllowedRat(any())).thenReturn(true);
        when(mockCell.isRegistered()).thenReturn(true);
        when(mockCell.isRoveOut()).thenReturn(true);
        when(mockCell.isRoveIn()).thenReturn(false);

        when(mockTransport.isTransportAvailable(anyInt(), any())).thenReturn(true);
        when(mockConfigurationManager.isEmcOverWiFiSupported()).thenReturn(true);
        when(mockConfigurationManager.isEmcHandoverAllowed()).thenReturn(true);

        //returns true isImsOnWiFi()---start
        when(mockUser.isWfcEnabled()).thenReturn(true);
        when(mockProvisionedValues.isWfcProvisioned()).thenReturn(true);
        when(mockConfigurationManager.isEmcOemProvisioningRequired()).thenReturn(true);
        when(mockProvisionedValues.isOemProvisioned()).thenReturn(true);
        when(mockWiFi.isRoaming()).thenReturn(true);
        when(mockConfigurationManager.isWfcRoamingSupported()).thenReturn(true);
        // isImsOnWiFi()---end

        // returns true isImsOnCell()---start
        when(mockUser.isVolteEnabled()).thenReturn(true);
        when(mockProvisionedValues.isVoLteProvisioned()).thenReturn(true);
        when(mockConfigurationManager.isVoLteVoPsRequired()).thenReturn(false);
        when(mockConfigurationManager.isVoLteVoPsNotRequiredActiveCall()).thenReturn(false);
        // isImsOnCell()---end
        when(mockTransport.getNetworkType(anyInt())).thenReturn(TelephonyManager.NETWORK_TYPE_IWLAN);

        // then
        assertEquals(TelephonyManager.NETWORK_TYPE_IWLAN,
                mImsHandoverDecisionTmoUs.getPreferredNetwork(current));
    }
    public void testRequestOrStopCellReports() {
        // Given
        when(mockUser.isWfcEnabled()).thenReturn(true);
        when(mockWiFi.isConnected()).thenReturn(true);

        // When
        mImsHandoverDecisionTmoUs.requestOrStopCellReports(TelephonyManager.NETWORK_TYPE_IWLAN,
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
        mImsHandoverDecisionTmoUs.requestOrStopCellReports(TelephonyManager.NETWORK_TYPE_IWLAN,
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
        mImsHandoverDecisionTmoUs.requestOrStopCellReports(TelephonyManager.NETWORK_TYPE_LTE,
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
        mImsHandoverDecisionTmoUs.requestOrStopCellReports(TelephonyManager.NETWORK_TYPE_LTE,
                true);

        // Then
        verify(mockCell, times(1)).requestCellReports(anyBoolean(),
                anyBoolean(), anyBoolean(), anyBoolean(), anyBoolean());
    }

    @Test
    public void testRequestOrStopCellReports_ExceptionalCase() {
        // When
        mImsHandoverDecisionTmoUs.requestOrStopCellReports(TelephonyManager.NETWORK_TYPE_IWLAN,
                true);

        // Then
        verify(mockCell, times(1)).stopCellReports();
    }
}
