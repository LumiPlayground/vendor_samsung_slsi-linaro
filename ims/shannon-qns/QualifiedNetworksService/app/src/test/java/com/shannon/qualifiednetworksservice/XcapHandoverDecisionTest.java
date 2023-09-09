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

import com.shannon.qualifiednetworksservice.HandoverConditions.ConditionEvents;
import java.util.ArrayList;
import junit.framework.TestCase;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.MockitoAnnotations;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.annotation.Config;

import static android.telephony.AccessNetworkConstants.AccessNetworkType.EUTRAN;
import static android.telephony.AccessNetworkConstants.AccessNetworkType.IWLAN;
import static android.telephony.TelephonyManager.NETWORK_TYPE_IWLAN;
import static android.telephony.TelephonyManager.NETWORK_TYPE_LTE;
import static android.telephony.TelephonyManager.NETWORK_TYPE_LTE_CA;
import static android.telephony.TelephonyManager.NETWORK_TYPE_NR;
import static android.telephony.TelephonyManager.NETWORK_TYPE_UNKNOWN;
import static com.shannon.qualifiednetworksservice.HandoverConditions.ConditionEvents.EVENT_APN_CONNECTED;
import static org.mockito.ArgumentMatchers.any;
import static org.mockito.ArgumentMatchers.anyBoolean;
import static org.mockito.ArgumentMatchers.anyInt;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class XcapHandoverDecisionTest extends TestCase {
    @BeforeClass
    public static void beforeClass() {
    }

    @AfterClass
    public static void afterClass() {
    }

    private XcapHandoverDecision mHandoverDecision;
    private ConditionConnectivity transport;
    private ConfigurationManager confMgr;
    private ConditionCellular cellular;
    private ConditionWiFi wifi;
    private ConditionUserSetting condUserSetting;
    private ConditionMobile condMobile;

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.openMocks(this);
        HandoverConditions handoverConditions =  mock(HandoverConditions.class);
        handoverConditions = mock(HandoverConditions.class);
        transport = mock(ConditionConnectivity.class);
        when(handoverConditions.getTransportCondition()).thenReturn(transport);
        confMgr = mock(ConfigurationManager.class);
        when(handoverConditions.getConfigurationManager()).thenReturn(confMgr);
        cellular = mock(ConditionCellular.class);
        when(handoverConditions.getCellularConditions()).thenReturn(cellular);
        wifi = mock(ConditionWiFi.class);
        when(handoverConditions.getWiFiConditions()).thenReturn(wifi);
        condUserSetting = mock(ConditionUserSetting.class);
        when(handoverConditions.getUserConditions()).thenReturn(condUserSetting);
        condMobile = mock(ConditionMobile.class);
        when(handoverConditions.getMobileConditions()).thenReturn(condMobile);

        mHandoverDecision = new XcapHandoverDecision(handoverConditions);
    }

    @After
    public void tearDown() throws Exception {
    }

    @Test
    public void testGetPreferredListDuringHandoverWifiRat() {
        //Given
        ConditionEvents event = EVENT_APN_CONNECTED;
        when(transport.isTransportAvailable(anyInt(), any())).thenReturn(true);
        when(confMgr.isWiFiPreferredForXcap()).thenReturn(true);
        when(confMgr.isXcapHandoverAllowed()).thenReturn(true);
        when(cellular.isAllowedRat(any())).thenReturn(true);
        when(cellular.isRegistered()).thenReturn(true);
        when(wifi.isConnected()).thenReturn(true);
        when(confMgr.isXcapOverWiFiSupported()).thenReturn(true);
        when(condUserSetting.isWfcEnabled()).thenReturn(true);
        when(transport.getNetworkType(anyInt())).thenReturn(NETWORK_TYPE_IWLAN);

        //When
        ArrayList<Integer> preferredList = mHandoverDecision.getPreferredList(event);

        //Then
        assertEquals(1, preferredList.size());
        assertEquals(true, preferredList.contains(IWLAN));
    }

    @Test
    public void testGetPreferredListHandoverCase() {
        //Given
        ConditionEvents event = EVENT_APN_CONNECTED;
        when(transport.isTransportAvailable(anyInt(), any())).thenReturn(true);
        when(confMgr.isWiFiPreferredForXcap()).thenReturn(true);
        when(confMgr.isXcapHandoverAllowed()).thenReturn(true);
        when(cellular.isAllowedRat(any())).thenReturn(true);
        when(cellular.isRegistered()).thenReturn(true);
        when(wifi.isConnected()).thenReturn(true);
        when(confMgr.isXcapOverWiFiSupported()).thenReturn(true);
        when(condUserSetting.isWfcEnabled()).thenReturn(true);
        when(transport.getNetworkType(anyInt())).thenReturn(NETWORK_TYPE_LTE);

        //When
        ArrayList<Integer> preferredList = mHandoverDecision.getPreferredList(event);

        //Then
        assertEquals(1, preferredList.size());
        assertEquals(true, preferredList.contains(EUTRAN));
    }

    @Test
    public void testGetPreferredListIdleCaseWifiRat() {
        //Given
        ConditionEvents event = EVENT_APN_CONNECTED;
        when(transport.isTransportAvailable(anyInt(), any())).thenReturn(true);
        when(confMgr.isWiFiPreferredForXcap()).thenReturn(true);
        when(confMgr.isXcapHandoverAllowed()).thenReturn(false);
        when(cellular.isAllowedRat(any())).thenReturn(true);
        when(cellular.isRegistered()).thenReturn(true);
        when(wifi.isConnected()).thenReturn(true);
        when(confMgr.isXcapOverWiFiSupported()).thenReturn(true);
        when(condUserSetting.isWfcEnabled()).thenReturn(true);
        when(transport.getNetworkType(anyInt())).thenReturn(NETWORK_TYPE_IWLAN);

        //When
        ArrayList<Integer> preferredList = mHandoverDecision.getPreferredList(event);

        //Then
        assertEquals(1, preferredList.size());
        assertEquals(true, preferredList.contains(IWLAN));
    }

    @Test
    public void testGetPreferredListIdleCaseCellularRat() {
        //Given
        ConditionEvents event = EVENT_APN_CONNECTED;
        when(transport.isTransportAvailable(anyInt(), any())).thenReturn(true);
        when(confMgr.isWiFiPreferredForXcap()).thenReturn(true);
        when(confMgr.isXcapHandoverAllowed()).thenReturn(false);
        when(cellular.isAllowedRat(any())).thenReturn(true);
        when(cellular.isRegistered()).thenReturn(true);
        when(wifi.isConnected()).thenReturn(true);
        when(confMgr.isXcapOverWiFiSupported()).thenReturn(true);
        when(condUserSetting.isWfcEnabled()).thenReturn(true);
        when(transport.getNetworkType(anyInt())).thenReturn(NETWORK_TYPE_LTE_CA);

        //When
        ArrayList<Integer> preferredList = mHandoverDecision.getPreferredList(event);

        //Then
        assertEquals(1, preferredList.size());
        assertEquals(true, preferredList.contains(EUTRAN));
    }

    @Test
    public void testGetPreferredListNotWifiPreferredWifiNotConnected() {
        //Given
        ConditionEvents event = EVENT_APN_CONNECTED;
        when(transport.isTransportAvailable(anyInt(), any())).thenReturn(true);
        when(confMgr.isWiFiPreferredForXcap()).thenReturn(true);
        when(confMgr.isXcapHandoverAllowed()).thenReturn(true);
        when(cellular.isAllowedRat(any())).thenReturn(true);
        when(cellular.isRegistered()).thenReturn(true);
        when(wifi.isConnected()).thenReturn(false);
        when(confMgr.isXcapOverWiFiSupported()).thenReturn(false);
        when(condUserSetting.isWfcEnabled()).thenReturn(true);
        when(transport.getNetworkType(anyInt())).thenReturn(NETWORK_TYPE_LTE);
        when(cellular.isRoveOut()).thenReturn(true);
        when(wifi.isRoveIn()).thenReturn(true);
        mHandoverDecision.setNewState(mHandoverDecision.mAttachState);

        //When
        ArrayList<Integer> preferredList = mHandoverDecision.getPreferredList(event);

        //Then
        assertEquals(1, preferredList.size());
        assertEquals(true, preferredList.contains(EUTRAN));
    }

    @Test
    public void testGetPreferredListNotValidRat() {
        //Given
        ConditionEvents event = EVENT_APN_CONNECTED;
        when(transport.isTransportAvailable(anyInt(), any())).thenReturn(true);
        when(confMgr.isWiFiPreferredForXcap()).thenReturn(false);
        when(confMgr.isXcapHandoverAllowed()).thenReturn(false);
        when(cellular.isAllowedRat(any())).thenReturn(true);
        when(cellular.isRegistered()).thenReturn(true);
        when(wifi.isConnected()).thenReturn(false);
        when(confMgr.isXcapOverWiFiSupported()).thenReturn(true);
        when(condUserSetting.isWfcEnabled()).thenReturn(true);
        when(transport.getNetworkType(anyInt())).thenReturn(NETWORK_TYPE_UNKNOWN);
        when(cellular.isRoveOut()).thenReturn(true);
        when(wifi.isRoveIn()).thenReturn(true);

        //When
        ArrayList<Integer> preferredList = mHandoverDecision.getPreferredList(event);

        //Then
        assertEquals(0, preferredList.size());
    }

    @Test
    public void testGetPreferredListVomobile() {
        //Given
        ConditionEvents event = EVENT_APN_CONNECTED;
        when(transport.isTransportAvailable(anyInt(), any())).thenReturn(true);
        when(confMgr.isWiFiPreferredForXcap()).thenReturn(true);
        when(confMgr.isXcapHandoverAllowed()).thenReturn(true);
        when(cellular.isAllowedRat(any())).thenReturn(true);
        when(cellular.isRegistered()).thenReturn(true);
        when(wifi.isConnected()).thenReturn(true);
        when(confMgr.isXcapOverWiFiSupported()).thenReturn(true);
        when(condUserSetting.isWfcEnabled()).thenReturn(true);
        when(transport.getNetworkType(anyInt())).thenReturn(NETWORK_TYPE_LTE);
        when(cellular.isRoveOut()).thenReturn(true);
        when(wifi.isRoveIn()).thenReturn(true);
        mHandoverDecision.setNewState(mHandoverDecision.mAttachState);
        when(condUserSetting.isVoMobileEnabled()).thenReturn(true);
        when(confMgr.isVoMobilePreferred()).thenReturn(true);
        when(condMobile.isVoMobilePossible(anyBoolean(), anyBoolean(), anyBoolean())).thenReturn(true);
        when(cellular.isLowerThanThreshold(anyInt())).thenReturn(true);
        when(condUserSetting.isVoMobileApnTypeEnabled(anyInt())).thenReturn(true);
        when(cellular.isAirplaneMode()).thenReturn(false);
        when(wifi.isConnected()).thenReturn(false);

        //When
        ArrayList<Integer> preferredList = mHandoverDecision.getPreferredList(event);

        //Then
        assertEquals(1, preferredList.size());
        assertEquals(true, preferredList.contains(IWLAN));
    }

    @Test
    public void testGetPreferredListNotWifiPreferredWifiRatAvailable() {
        //Given
        ConditionEvents event = EVENT_APN_CONNECTED;
        when(transport.isTransportAvailable(anyInt(), any())).thenReturn(true);
        when(confMgr.isWiFiPreferredForXcap()).thenReturn(false);
        when(confMgr.isXcapHandoverAllowed()).thenReturn(true);
        when(cellular.isAllowedRat(any())).thenReturn(true);
        when(cellular.isRegistered()).thenReturn(true);
        when(wifi.isConnected()).thenReturn(true);
        when(confMgr.isXcapOverWiFiSupported()).thenReturn(true);
        when(condUserSetting.isWfcEnabled()).thenReturn(true);
        when(transport.getNetworkType(anyInt())).thenReturn(NETWORK_TYPE_LTE);
        when(cellular.isRoveOut()).thenReturn(true);
        when(wifi.isRoveIn()).thenReturn(true);
        mHandoverDecision.setNewState(mHandoverDecision.mAttachState);

        //When
        ArrayList<Integer> preferredList = mHandoverDecision.getPreferredList(event);

        //Then
        assertEquals(1, preferredList.size());
        assertEquals(true, preferredList.contains(IWLAN));
    }

    @Test
    public void testGetPreferredListNotWifiPreferredCellularRat() {
        //Given
        ConditionEvents event = EVENT_APN_CONNECTED;
        when(transport.isTransportAvailable(anyInt(), any())).thenReturn(true);
        when(confMgr.isWiFiPreferredForXcap()).thenReturn(false);
        when(confMgr.isXcapHandoverAllowed()).thenReturn(true);
        when(cellular.isAllowedRat(any())).thenReturn(true);
        when(cellular.isRegistered()).thenReturn(true);
        when(wifi.isConnected()).thenReturn(true);
        when(confMgr.isXcapOverWiFiSupported()).thenReturn(false);
        when(condUserSetting.isWfcEnabled()).thenReturn(true);
        when(transport.getNetworkType(anyInt())).thenReturn(NETWORK_TYPE_LTE);
        when(cellular.isRoveOut()).thenReturn(true);
        when(wifi.isRoveIn()).thenReturn(true);
        mHandoverDecision.setNewState(mHandoverDecision.mAttachState);

        //When
        ArrayList<Integer> preferredList = mHandoverDecision.getPreferredList(event);

        //Then
        assertEquals(1, preferredList.size());
        assertEquals(true, preferredList.contains(EUTRAN));
    }

    @Test
    public void testGetPreferredListNotWifiPreferredNotValidCellularCase() {
        //Given
        ConditionEvents event = EVENT_APN_CONNECTED;
        when(transport.isTransportAvailable(anyInt(), any())).thenReturn(true);
        when(confMgr.isWiFiPreferredForXcap()).thenReturn(false);
        when(confMgr.isXcapHandoverAllowed()).thenReturn(true);
        when(cellular.isAllowedRat(any())).thenReturn(true);
        when(cellular.isRegistered()).thenReturn(true);
        when(wifi.isConnected()).thenReturn(true);
        when(confMgr.isXcapOverWiFiSupported()).thenReturn(true);
        when(condUserSetting.isWfcEnabled()).thenReturn(true);
        when(transport.getNetworkType(anyInt())).thenReturn(NETWORK_TYPE_IWLAN);
        when(cellular.isRoveOut()).thenReturn(true);
        when(wifi.isRoveIn()).thenReturn(true);
        mHandoverDecision.setNewState(mHandoverDecision.mAttachState);

        //When
        ArrayList<Integer> preferredList = mHandoverDecision.getPreferredList(event);

        //Then
        assertEquals(1, preferredList.size());
        assertEquals(true, preferredList.contains(IWLAN));
    }

    @Test
    public void testIsServiceSupportedIwlanRat() {
        //Given
        ConditionEvents event = EVENT_APN_CONNECTED;
        when(transport.isTransportAvailable(anyInt(), any())).thenReturn(true);
        when(confMgr.isWiFiPreferredForXcap()).thenReturn(false);
        when(confMgr.isXcapHandoverAllowed()).thenReturn(true);
        when(cellular.isAllowedRat(any())).thenReturn(true);
        when(cellular.isRegistered()).thenReturn(true);
        when(wifi.isConnected()).thenReturn(true);
        when(confMgr.isXcapOverWiFiSupported()).thenReturn(true);
        when(condUserSetting.isWfcEnabled()).thenReturn(true);
        when(transport.getNetworkType(anyInt())).thenReturn(NETWORK_TYPE_IWLAN);
        when(cellular.isRoveOut()).thenReturn(true);
        when(wifi.isRoveIn()).thenReturn(true);
        mHandoverDecision.setNewState(mHandoverDecision.mAttachState);

        //When
        boolean retValue = mHandoverDecision.isServiceSupported(NETWORK_TYPE_IWLAN);

        //Then
        assertEquals(true, retValue);
    }

    @Test
    public void testIsServiceSupportedCellulatRat() {
        //Given
        ConditionEvents event = EVENT_APN_CONNECTED;
        when(transport.isTransportAvailable(anyInt(), any())).thenReturn(true);
        when(confMgr.isWiFiPreferredForXcap()).thenReturn(false);
        when(confMgr.isXcapHandoverAllowed()).thenReturn(true);
        when(cellular.isAllowedRat(any())).thenReturn(true);
        when(cellular.isRegistered()).thenReturn(true);
        when(wifi.isConnected()).thenReturn(true);
        when(confMgr.isXcapOverWiFiSupported()).thenReturn(true);
        when(condUserSetting.isWfcEnabled()).thenReturn(true);
        when(transport.getNetworkType(anyInt())).thenReturn(NETWORK_TYPE_NR);
        when(cellular.isRoveOut()).thenReturn(true);
        when(wifi.isRoveIn()).thenReturn(true);
        mHandoverDecision.setNewState(mHandoverDecision.mAttachState);

        //When
        boolean retValue = mHandoverDecision.isServiceSupported(NETWORK_TYPE_NR);

        //Then
        assertEquals(true, retValue);
    }
}
