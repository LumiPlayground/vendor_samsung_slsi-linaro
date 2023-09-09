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

import static android.telephony.ims.ImsMmTelManager.WIFI_MODE_CELLULAR_PREFERRED;
import static android.telephony.ims.ImsMmTelManager.WIFI_MODE_WIFI_PREFERRED;
import static com.shannon.qualifiednetworksservice.ConditionUserSetting.WIFI_MODE_IMS_PREFERRED;
import static org.mockito.ArgumentMatchers.any;
import static org.mockito.ArgumentMatchers.anyBoolean;
import static org.mockito.ArgumentMatchers.anyInt;
import static org.mockito.ArgumentMatchers.anyString;
import static org.mockito.Mockito.doReturn;
import static org.mockito.Mockito.spy;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

@FixMethodOrder(MethodSorters.NAME_ASCENDING)
@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class ImsHandoverDecisionDtagTest {
    private ImsHandoverDecisionDtag mImsHandoverDecisionDtag, spyImsHandoverDecisionDtag;

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

        mImsHandoverDecisionDtag = new ImsHandoverDecisionDtag(mockHandoverConditions);
        spyImsHandoverDecisionDtag = spy(mImsHandoverDecisionDtag);
    }

    @After
    public void tearDown() {
        Mockito.validateMockitoUsage();
    }

    @Test
    public void getPreferredNetwork_APM_ON_current_LTE_preferred_IWLAN_Test() {
        // Given
        // isWiFiTransportAvailable() returns true
        when(mockTransport.isTransportAvailable(anyInt(), any())).thenReturn(true);
        when(mockConfigurationManager.isEmcOverWiFiSupported()).thenReturn(true);
        when(mockConfigurationManager.isEmcHandoverAllowed()).thenReturn(true);

        // isWfcEnabled = true
        //returns true isImsOnWiFi()---start
        when(mockUser.isWfcEnabled()).thenReturn(true);
        when(mockProvisionedValues.isWfcProvisioned()).thenReturn(true);
        when(mockConfigurationManager.isWfcOemProvisioningRequired()).thenReturn(true);
        when(mockProvisionedValues.isOemProvisioned()).thenReturn(true);
        when(mockCell.isRoaming()).thenReturn(true);
        when(mockCell.isRoamedInternationally()).thenReturn(true);

        when(mockWiFi.isRoaming()).thenReturn(true);
        when(mockConfigurationManager.isWfcRoamingSupported()).thenReturn(true);
        // isImsOnWiFi()---end

        // isWiFiConnected = true
        when(mockWiFi.isConnected()).thenReturn(true);

        // isCellReady = true iff 1 && 2
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
        when(mockConfigurationManager.isVoLteRoamingSupported()).thenReturn(true);
        when(mockConfigurationManager.getAllowedRoamingNetworks()).thenReturn("LTE");
        when(mockCell.isAllowedNetwork(anyString())).thenReturn(true);
        when(mockConfigurationManager.getVoLteAllowedRat()).thenReturn("LTE");
        when(mockCell.isAllowedRat(anyString())).thenReturn(true);

        when(mockWiFi.isRoaming()).thenReturn(true);
        when(mockConfigurationManager.isWfcRoamingSupported()).thenReturn(true);
        // isImsOnCell()---end

        // APM ON
        when(mockCell.isAirplaneMode()).thenReturn(true);

        // When
        mImsHandoverDecisionDtag.getPreferredNetwork(TelephonyManager.NETWORK_MODE_NR_LTE);

        // Then
        verify(mockCell, times(0)).getNetworkType();

        // APM OFF
        // Given
        when(mockCell.isAirplaneMode()).thenReturn(false);

        // When
        mImsHandoverDecisionDtag.getPreferredNetwork(TelephonyManager.NETWORK_MODE_NR_LTE);

        // Then
        verify(mockCell, times(0)).getNetworkType();
    }

    @Test
    public void getPreferredNetwork_APM_OFF_current_LTE_Test() {
        // Given
        // isWiFiTransportAvailable() returns true
        when(mockTransport.isTransportAvailable(anyInt(), any())).thenReturn(true);
        when(mockConfigurationManager.isEmcOverWiFiSupported()).thenReturn(true);
        when(mockConfigurationManager.isEmcHandoverAllowed()).thenReturn(true);

        // isCellReady = true iff 1 && 2
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

        // APM OFF and isWfcEnabled = false and isCellReady = true
        // Given
        when(mockCell.isAirplaneMode()).thenReturn(false);

        // When
        mImsHandoverDecisionDtag.getPreferredNetwork(TelephonyManager.NETWORK_MODE_NR_LTE);

        // Then
        verify(mockCell, times(1)).getNetworkType();
    }

    @Test
    public void getPreferredNetwork_WIFI_MODE_CELLULAR_PREFERRED_ImsPreferredNotDefined_Test() {
        // Given
        // isWiFiTransportAvailable() returns true
        when(mockTransport.isTransportAvailable(anyInt(), any())).thenReturn(true);
        when(mockConfigurationManager.isEmcOverWiFiSupported()).thenReturn(true);
        when(mockConfigurationManager.isEmcHandoverAllowed()).thenReturn(true);

        // isWfcEnabled = true
        //returns true isImsOnWiFi()---start
        when(mockUser.isWfcEnabled()).thenReturn(true);
        when(mockProvisionedValues.isWfcProvisioned()).thenReturn(true);
        when(mockConfigurationManager.isWfcOemProvisioningRequired()).thenReturn(true);
        when(mockProvisionedValues.isOemProvisioned()).thenReturn(true);
        when(mockCell.isRoaming()).thenReturn(true);
        when(mockCell.isRoamedInternationally()).thenReturn(true);

        when(mockWiFi.isRoaming()).thenReturn(true);
        when(mockConfigurationManager.isWfcRoamingSupported()).thenReturn(true);
        // isImsOnWiFi()---end

        // isWiFiConnected = true
        when(mockWiFi.isConnected()).thenReturn(true);

        // isCellReady = true iff 1 && 2
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

        // wfcMode = WIFI_MODE_CELLULAR_PREFERRED
        when(mockUser.getWfcModeEnum()).thenReturn(WIFI_MODE_CELLULAR_PREFERRED);

        // isImsCallActive = true
        when(mockCall.isImsCallActive(anyBoolean())).thenReturn(true);

        // isWiFiIn = true
        when(mockWiFi.isHandIn(anyInt())).thenReturn(true);

        // isWiFiOut = true
        when(mockWiFi.isHandOut(anyInt())).thenReturn(true);

        // isCellIn = true
        when(mockCell.isHandIn(anyInt())).thenReturn(true);

        // isCellOut = true
        when(mockCell.isHandOut(anyInt())).thenReturn(true);

        // When
        mImsHandoverDecisionDtag.getPreferredNetwork(TelephonyManager.NETWORK_MODE_NR_LTE);
        // Then
        verify(mockCell, times(0)).getNetworkType();

        // When
        mImsHandoverDecisionDtag.getPreferredNetwork(TelephonyManager.NETWORK_TYPE_IWLAN);
        // Then
        verify(mockCell, times(1)).getNetworkType();

        // When
        mImsHandoverDecisionDtag.getPreferredNetwork(TelephonyManager.NETWORK_TYPE_UNKNOWN);
        // Then
        verify(mockCell, times(2)).getNetworkType();
    }

    @Test
    public void getPreferredNetwork_WIFI_MODE_CELLULAR_PREFERRED_Test() {
        // Given
        // isWiFiTransportAvailable() returns true
        when(mockTransport.isTransportAvailable(anyInt(), any())).thenReturn(true);
        when(mockConfigurationManager.isEmcOverWiFiSupported()).thenReturn(true);
        when(mockConfigurationManager.isEmcHandoverAllowed()).thenReturn(true);

        // isWfcEnabled = true
        //returns true isImsOnWiFi()---start
        when(mockUser.isWfcEnabled()).thenReturn(true);
        when(mockProvisionedValues.isWfcProvisioned()).thenReturn(true);
        when(mockConfigurationManager.isWfcOemProvisioningRequired()).thenReturn(true);
        when(mockProvisionedValues.isOemProvisioned()).thenReturn(true);
        when(mockCell.isRoaming()).thenReturn(true);
        when(mockCell.isRoamedInternationally()).thenReturn(true);

        when(mockWiFi.isRoaming()).thenReturn(true);
        when(mockConfigurationManager.isWfcRoamingSupported()).thenReturn(true);
        // isImsOnWiFi()---end

        // isWiFiConnected = true
        when(mockWiFi.isConnected()).thenReturn(true);

        // isCellReady = true iff 1 && 2
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

        // wfcMode = WIFI_MODE_CELLULAR_PREFERRED
        when(mockUser.getWfcModeEnum()).thenReturn(WIFI_MODE_CELLULAR_PREFERRED);

        // isImsCallActive = true
        when(mockCall.isImsCallActive(anyBoolean())).thenReturn(true);

        // isWiFiIn = true
        when(mockWiFi.isHandIn(anyInt())).thenReturn(true);

        // isWiFiOut = true
        when(mockWiFi.isHandOut(anyInt())).thenReturn(true);

        // isCellIn = true
        when(mockCell.isHandIn(anyInt())).thenReturn(true);

        // isCellOut = true
        when(mockCell.isHandOut(anyInt())).thenReturn(true);

        when(spyImsHandoverDecisionDtag.isImsPreferredNotDefined()).thenReturn(false);

        // When
        spyImsHandoverDecisionDtag.getPreferredNetwork(TelephonyManager.NETWORK_MODE_NR_LTE);
        // Then
        verify(mockCell, times(0)).getNetworkType();

        // When
        spyImsHandoverDecisionDtag.getPreferredNetwork(TelephonyManager.NETWORK_TYPE_IWLAN);
        // Then
        verify(mockCell, times(1)).getNetworkType();

        // When
        spyImsHandoverDecisionDtag.getPreferredNetwork(TelephonyManager.NETWORK_TYPE_UNKNOWN);
        // Then
        verify(mockCell, times(2)).getNetworkType();
    }

    @Test
    public void getPreferredNetwork_WIFI_MODE_WIFI_PREFERRED_Test() {
        // Given
        // isWiFiTransportAvailable() returns true
        when(mockTransport.isTransportAvailable(anyInt(), any())).thenReturn(true);
        when(mockConfigurationManager.isEmcOverWiFiSupported()).thenReturn(true);
        when(mockConfigurationManager.isEmcHandoverAllowed()).thenReturn(true);

        // isWfcEnabled = true
        //returns true isImsOnWiFi()---start
        when(mockUser.isWfcEnabled()).thenReturn(true);
        when(mockProvisionedValues.isWfcProvisioned()).thenReturn(true);
        when(mockConfigurationManager.isWfcOemProvisioningRequired()).thenReturn(true);
        when(mockProvisionedValues.isOemProvisioned()).thenReturn(true);
        when(mockCell.isRoaming()).thenReturn(true);
        when(mockCell.isRoamedInternationally()).thenReturn(true);

        when(mockWiFi.isRoaming()).thenReturn(true);
        when(mockConfigurationManager.isWfcRoamingSupported()).thenReturn(true);
        // isImsOnWiFi()---end

        // isWiFiConnected = true
        when(mockWiFi.isConnected()).thenReturn(true);

        // isCellReady = true iff 1 && 2
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

        // isImsCallActive = true
        when(mockCall.isImsCallActive(anyBoolean())).thenReturn(true);

        // isWiFiIn = true
        when(mockWiFi.isHandIn(anyInt())).thenReturn(true);

        // isWiFiOut = true
        when(mockWiFi.isHandOut(anyInt())).thenReturn(true);

        // isCellIn = true
        when(mockCell.isHandIn(anyInt())).thenReturn(true);

        // isCellOut = true
        when(mockCell.isHandOut(anyInt())).thenReturn(true);

        // wfcMode = WIFI_MODE_WIFI_PREFERRED
        when(mockUser.getWfcModeEnum()).thenReturn(WIFI_MODE_WIFI_PREFERRED);

        // When
        mImsHandoverDecisionDtag.getPreferredNetwork(TelephonyManager.NETWORK_MODE_NR_LTE);
        // Then
        verify(mockCell, times(0)).getNetworkType();

        // When
        mImsHandoverDecisionDtag.getPreferredNetwork(TelephonyManager.NETWORK_TYPE_IWLAN);
        // Then
        verify(mockCell, times(1)).getNetworkType();

        // When
        mImsHandoverDecisionDtag.getPreferredNetwork(TelephonyManager.NETWORK_TYPE_UNKNOWN);
        // Then
        verify(mockCell, times(1)).getNetworkType();

        // Additional coverage with if-else conditions
        // Given
        // isCellIn = false
        when(mockCell.isHandIn(anyInt())).thenReturn(false);
        // When
        mImsHandoverDecisionDtag.getPreferredNetwork(TelephonyManager.NETWORK_TYPE_IWLAN);
        // Then
        verify(mockCell, times(1)).getNetworkType();

        // Given
        // isWiFiConnected = false
        when(mockWiFi.isConnected()).thenReturn(false);

        // When
        mImsHandoverDecisionDtag.getPreferredNetwork(TelephonyManager.NETWORK_MODE_NR_LTE);
        // Then
        verify(mockCell, times(1)).getNetworkType();
    }

    @Test
    public void getPreferredNetwork_WIFI_MODE_IMS_PREFERRED_Test() {
        // Given
        // isWiFiTransportAvailable() returns true
        when(mockTransport.isTransportAvailable(anyInt(), any())).thenReturn(true);
        when(mockConfigurationManager.isEmcOverWiFiSupported()).thenReturn(true);
        when(mockConfigurationManager.isEmcHandoverAllowed()).thenReturn(true);

        // isWfcEnabled = true
        //returns true isImsOnWiFi()---start
        when(mockUser.isWfcEnabled()).thenReturn(true);
        when(mockProvisionedValues.isWfcProvisioned()).thenReturn(true);
        when(mockConfigurationManager.isWfcOemProvisioningRequired()).thenReturn(true);
        when(mockProvisionedValues.isOemProvisioned()).thenReturn(true);
        when(mockCell.isRoaming()).thenReturn(true);
        when(mockCell.isRoamedInternationally()).thenReturn(true);

        when(mockWiFi.isRoaming()).thenReturn(true);
        when(mockConfigurationManager.isWfcRoamingSupported()).thenReturn(true);
        // isImsOnWiFi()---end

        // isWiFiConnected = true
        when(mockWiFi.isConnected()).thenReturn(true);

        // isCellReady = true iff 1 && 2
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

        // isImsCallActive = true
        when(mockCall.isImsCallActive(anyBoolean())).thenReturn(true);

        // isWiFiIn = true
        when(mockWiFi.isHandIn(anyInt())).thenReturn(true);

        // isWiFiOut = true
        when(mockWiFi.isHandOut(anyInt())).thenReturn(true);

        // isCellIn = true
        when(mockCell.isHandIn(anyInt())).thenReturn(true);

        // isCellOut = true
        when(mockCell.isHandOut(anyInt())).thenReturn(true);

        // wfcMode = WIFI_MODE_IMS_PREFERRED
        when(mockUser.getWfcModeEnum()).thenReturn(WIFI_MODE_IMS_PREFERRED);

        // When
        mImsHandoverDecisionDtag.getPreferredNetwork(TelephonyManager.NETWORK_MODE_NR_LTE);
        // Then
        verify(mockCell, times(0)).getNetworkType();

        // When
        mImsHandoverDecisionDtag.getPreferredNetwork(TelephonyManager.NETWORK_TYPE_IWLAN);
        // Then
        verify(mockCell, times(1)).getNetworkType();

        // When
        mImsHandoverDecisionDtag.getPreferredNetwork(TelephonyManager.NETWORK_TYPE_UNKNOWN);
        // Then
        verify(mockCell, times(2)).getNetworkType();
    }

    @Test
    public void testRequestOrStopCellReports() {
        // Given
        when(mockUser.isWfcEnabled()).thenReturn(true);
        when(mockWiFi.isConnected()).thenReturn(true);

        // When
        spyImsHandoverDecisionDtag.requestOrStopCellReports(TelephonyManager.NETWORK_TYPE_IWLAN,
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
        spyImsHandoverDecisionDtag.requestOrStopCellReports(TelephonyManager.NETWORK_TYPE_IWLAN,
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
        spyImsHandoverDecisionDtag.requestOrStopCellReports(TelephonyManager.NETWORK_TYPE_LTE,
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
        spyImsHandoverDecisionDtag.requestOrStopCellReports(TelephonyManager.NETWORK_TYPE_LTE,
                true);

        // Then
        verify(mockCell, times(1)).requestCellReports(anyBoolean(),
                anyBoolean(), anyBoolean(), anyBoolean(), anyBoolean());
    }

    @Test
    public void testRequestOrStopCellReports_ExceptionalCase() {
        // When
        spyImsHandoverDecisionDtag.requestOrStopCellReports(TelephonyManager.NETWORK_TYPE_IWLAN,
                true);

        // Then
        verify(mockCell, times(1)).stopCellReports();
    }
}
