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

import android.telephony.DataFailCause;

import android.telephony.TelephonyManager;
import android.telephony.data.ApnSetting;
import com.shannon.qualifiednetworksservice.HandoverConditions.ConditionEvents;
import com.shannon.qualifiednetworksservice.Transport.TransportType;
import junit.framework.TestCase;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.Mock;
import org.mockito.MockitoAnnotations;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.annotation.Config;

import static android.hardware.radio.Announcement.TYPE_EMERGENCY;
import static android.telephony.DataFailCause.NAS_SIGNALLING;
import static android.telephony.TelephonyManager.NETWORK_TYPE_IWLAN;
import static android.telephony.TelephonyManager.NETWORK_TYPE_LTE;
import static android.telephony.TelephonyManager.NETWORK_TYPE_LTE_CA;
import static android.telephony.TelephonyManager.NETWORK_TYPE_UMTS;
import static android.telephony.TelephonyManager.NETWORK_TYPE_UNKNOWN;
import static com.shannon.qualifiednetworksservice.HandoverConditions.ConditionEvents.*;
import static com.shannon.qualifiednetworksservice.Transport.TransportType.TRANSPORT_CELLULAR;
import static com.shannon.qualifiednetworksservice.Transport.TransportType.TRANSPORT_WIFI;
import static org.mockito.ArgumentMatchers.anyBoolean;
import static org.mockito.ArgumentMatchers.anyInt;
import static org.mockito.ArgumentMatchers.eq;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class HandoverDecisionTest extends TestCase {
    @BeforeClass
    public static void beforeClass() {
    }

    @AfterClass
    public static void afterClass() {
    }

    private HandoverDecision mHandoverDecision;

    @Mock
    ConditionConnectivity mockConnectivity;

    @Mock
    HandoverConditions mockHandoverCond;

    @Mock
    ConfigurationManager mockConfigurationMgr;

    @Mock
    ConditionUserSetting mockConditionUserSetting;

    @Mock
    ConditionCellular mockConditionCellular;

    @Mock
    ConditionWiFi mockConditionWifi;

    @Mock
    ConditionMobile mockConditionMobile;

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.openMocks(this);
        mockConnectivity = mock(ConditionConnectivity.class);
        mockHandoverCond = mock(HandoverConditions.class);
        mockConfigurationMgr = mock(ConfigurationManager.class);
        when(mockHandoverCond.getConfigurationManager()).thenReturn(mockConfigurationMgr);
        when(mockHandoverCond.getTransportCondition()).thenReturn(mockConnectivity);
        when(mockHandoverCond.getUserConditions()).thenReturn(mockConditionUserSetting);
        when(mockHandoverCond.getCellularConditions()).thenReturn(mockConditionCellular);
        when(mockHandoverCond.getWiFiConditions()).thenReturn(mockConditionWifi);
        when(mockHandoverCond.getMobileConditions()).thenReturn(mockConditionMobile);
        mHandoverDecision = new HandoverDecision(ApnSetting.TYPE_IMS, mockHandoverCond);
    }

    @After
    public void tearDown() throws Exception {
    }

    @Test
    public void testIsValidCellularType() {
        //Given
        int networkType = NETWORK_TYPE_UMTS;

        //When
        boolean isCellular = mHandoverDecision.isValidCellularType(networkType);

        //Then
        assertEquals(true, isCellular);
    }

    @Test
    public void testIsValidType() {
        //Given
        int networkType = NETWORK_TYPE_UMTS;

        //When
        boolean isValidType = mHandoverDecision.isValidType(networkType);

        //Then
        assertEquals(true, isValidType);
    }

    @Test
    public void testIsWiFiType() {
        //Given
        int networkType = NETWORK_TYPE_IWLAN;

        //When
        boolean isValidType = mHandoverDecision.isWiFiType(networkType);

        //Then
        assertEquals(true, isValidType);
    }

    @Test
    public void testIsHandover() {
        //Given
        int toNetworkType = NETWORK_TYPE_IWLAN;
        int fromNetworkType = NETWORK_TYPE_UMTS;

        //When
        boolean isHandover = mHandoverDecision.isHandover(fromNetworkType, toNetworkType);

        //Then
        assertEquals(true, isHandover);
    }

    @Test
    public void testIsImsPdnOnWiFi() {
        //Given
        int networkType = NETWORK_TYPE_IWLAN;
        when(mockConnectivity.getNetworkType(anyInt())).thenReturn(NETWORK_TYPE_IWLAN);

        //When
        boolean isHandover = mHandoverDecision.isImsPdnOnWiFi();

        //Then
        assertEquals(true, isHandover);
    }

    @Test
    public void testGetCurrentNetworkType() {
        //Given
        when(mockConnectivity.getNetworkType(anyInt())).thenReturn(NETWORK_TYPE_IWLAN);

        //When
        int networkType = mHandoverDecision.getCurrentNetworkType(TYPE_EMERGENCY);

        //Then
        assertEquals(NETWORK_TYPE_IWLAN, networkType);
    }

    @Test
    public void testGetCurrentNetworkTypeBasedOnApn() {
        //Given
        when(mockConnectivity.getNetworkType(anyInt())).thenReturn(NETWORK_TYPE_IWLAN);

        //When
        int networkType = mHandoverDecision.getCurrentNetworkType();

        //Then
        assertEquals(NETWORK_TYPE_IWLAN, networkType);
    }

    @Test
    public void testIsCellTransportAvailable() {
        //Given
        when(mockConnectivity.isTransportAvailable(anyInt(), eq(TransportType.TRANSPORT_CELLULAR))).thenReturn(true);

        //When
        boolean isCellTrans = mHandoverDecision.isCellTransportAvailable();

        //Then
        assertEquals(true, isCellTrans);
    }

    @Test
    public void testIsWifiTransportAvailable() {
        //Given
        when(mockConnectivity.isTransportAvailable(anyInt(), eq(TransportType.TRANSPORT_WIFI))).thenReturn(true);

        //When
        boolean isCellTrans = mHandoverDecision.isWiFiTransportAvailable();

        //Then
        assertEquals(true, isCellTrans);
    }

    @Test
    public void testIsWifiHandoverFailed() {
        //Given
        when(mockConnectivity.isHandoverFailed(anyInt(), eq(TransportType.TRANSPORT_WIFI))).thenReturn(true);
        int networkType = NETWORK_TYPE_IWLAN;

        //When
        boolean isWifiHandoverFailed = mHandoverDecision.isHandoverFailed(networkType);

        //Then
        assertEquals(true, isWifiHandoverFailed);
    }

    @Test
    public void testIsCellularHandoverFailed() {
        //Given
        when(mockConnectivity.isHandoverFailed(anyInt(), eq(TRANSPORT_CELLULAR))).thenReturn(true);
        int networkType = NETWORK_TYPE_LTE;

        //When
        boolean isCellularHandoverFailed = mHandoverDecision.isHandoverFailed(networkType);

        //Then
        assertEquals(true, isCellularHandoverFailed);
    }

    @Test
    public void testDecisionTimerTask() {
        //Given
        ConditionEvents condEvents = EVENT_APN_CONNECTED;
        HandoverDecision.DecisionTimerTask decisionTimerTask = mHandoverDecision.new DecisionTimerTask(condEvents);

        //When
        decisionTimerTask.run();

        //Then
        verify(mockHandoverCond, times(1)).onConditionChanged(anyInt(), eq(EVENT_APN_CONNECTED));
    }

    @Test
    public void testsetIdleState() {
        //Given


        //When
        mHandoverDecision.setNewState(mHandoverDecision.mIdleState);

        //Then
        assertEquals(false, mHandoverDecision.isVoMobileUsed());
    }

    @Test
    public void testsetIdleState_withPreferredAndTimeout() {
        // Given

        // When
        mHandoverDecision.setNewState(mHandoverDecision.mIdleState,
                TelephonyManager.NETWORK_TYPE_IWLAN, 10);

        // Then
        assertEquals(false, mHandoverDecision.isVoMobileUsed());
    }

    @Test
    public void testsetAttachState() {
        //Given
        when(mockConfigurationMgr.getInitialAttachTimeout()).thenReturn(10);

        //When
        mHandoverDecision.setNewState(mHandoverDecision.mAttachState);

        //Then
        verify(mockConfigurationMgr, times(1)).getInitialAttachTimeout();
    }


    @Test
    public void testSetEventInfo() {
        //Given
        Object obj = mock(Object.class);

        //When
        mHandoverDecision.setEventInfo(obj);

        //Then
        assertEquals(obj, mHandoverDecision.getEventInfo());
    }

    @Test
    public void testIsServiceSupported() {
        //Given
        int networkType = NETWORK_TYPE_IWLAN;

        //When
        boolean isServiceSupp = mHandoverDecision.isServiceSupported(networkType);

        //Then
        assertEquals(true, isServiceSupp);
    }

    @Test
    public void testIsVoMobileSupported() {
        //Given

        //When
        boolean isVomobilePossible = mHandoverDecision.isVoMobileSupported();

        //Then
        assertEquals(false, isVomobilePossible);
    }

    @Test
    public void testCanUseVoMobile() {
        //Given
        int networkType = NETWORK_TYPE_LTE;
        boolean isVoMobileUsed = true;
        when(mockConditionUserSetting.isVoMobileEnabled()).thenReturn(true);
        when(mockConditionUserSetting.isVoMobileApnTypeEnabled(anyInt())).thenReturn(true);
        when(mockConditionCellular.isAirplaneMode()).thenReturn(false);
        when(mockConditionWifi.isConnected()).thenReturn(false);
        when(mockConditionMobile.isVoMobilePossible(anyBoolean(), anyBoolean(), anyBoolean())).thenReturn(true);
        when(mockConfigurationMgr.isVoMobilePreferred()).thenReturn(true);
        when(mockConditionMobile.isVoMobilePossible(anyBoolean(), anyBoolean(), anyBoolean())).thenReturn(true);
        when(mockConditionCellular.isLowerThanThreshold(anyInt())).thenReturn(true);

        //When
        int preferredRat = mHandoverDecision.canUseVoMobile(networkType, isVoMobileUsed);

        //Then
        assertEquals(NETWORK_TYPE_IWLAN, preferredRat);
    }

    @Test
    public void testProcessAttachStateForApnConnected() {
        //Given
        int networkType = NETWORK_TYPE_IWLAN;

        //When
        mHandoverDecision.processAttachState(networkType, EVENT_APN_CONNECTED);

        //Then
        assertEquals(mHandoverDecision.mIdleState, mHandoverDecision.mState);
    }

    @Test
    public void testProcessAttachStateForAttachedTimerExpired() {
        //Given
        int networkType = NETWORK_TYPE_IWLAN;

        //When
        mHandoverDecision.processAttachState(networkType, EVENT_ATTACH_TIMER_EXPIRED);

        //Then
        assertEquals(mHandoverDecision.mIdleState, mHandoverDecision.mState);
    }

    @Test
    public void testProcessAttachStateForApnDisconnected() {
        //Given
        int networkType = NETWORK_TYPE_IWLAN;
        Transport mockTransport = mock(Transport.class);
        mHandoverDecision.setEventInfo((Object) mockTransport);
        when(mockTransport.getType()).thenReturn(TRANSPORT_WIFI);
        when(mockTransport.getLastFailCause()).thenReturn(NAS_SIGNALLING);
        mHandoverDecision.mLastPreferredNetwork = NETWORK_TYPE_IWLAN;

        //When
        mHandoverDecision.processAttachState(networkType, EVENT_APN_DISCONNECTED);

        //Then
        assertEquals(mHandoverDecision.mIdleState, mHandoverDecision.mState);
    }

    @Test
    public void testProcessAttachStateForApnDisconnectedDataFailCause_NONE() {
        //Given
        int networkType = NETWORK_TYPE_IWLAN;
        Transport mockTransport = mock(Transport.class);
        mHandoverDecision.setEventInfo((Object) mockTransport);
        when(mockTransport.getType()).thenReturn(TRANSPORT_WIFI);
        when(mockTransport.getLastFailCause()).thenReturn(DataFailCause.NONE);
        mHandoverDecision.mLastPreferredNetwork = NETWORK_TYPE_IWLAN;

        //When
        mHandoverDecision.processAttachState(networkType, EVENT_APN_DISCONNECTED);

        //Then
        assertEquals(mHandoverDecision.mIdleState, mHandoverDecision.mState);
    }

    @Test
    public void testProcessAttachStateForApnFailure() {
        //Given
        int networkType = NETWORK_TYPE_IWLAN;
        Transport mockTransport = mock(Transport.class);
        mHandoverDecision.setEventInfo((Object) mockTransport);
        when(mockTransport.getType()).thenReturn(TRANSPORT_WIFI);

        //When
        mHandoverDecision.processAttachState(networkType, EVENT_APN_FAILURE);

        //Then
        assertEquals(mHandoverDecision.mIdleState, mHandoverDecision.mState);
    }

    @Test
    public void testProcessAttachStateForDefaultCase() {
        //Given
        int networkType = NETWORK_TYPE_IWLAN;

        //When
        mHandoverDecision.processAttachState(networkType, EVENT_CALL_STARTED);

        //Then
        assertEquals(mHandoverDecision.mIdleState, mHandoverDecision.mState);
    }

    @Test
    public void testProcessHandoverState() {
        //Given
        int networkType = NETWORK_TYPE_IWLAN;

        //When
        int lastPrefNw = mHandoverDecision.processHandoverState(networkType, EVENT_APN_CONNECTED);

        //Then
        assertEquals(mHandoverDecision.mIdleState, mHandoverDecision.mState);
    }

    @Test
    public void testProcessHandoverStateApnDisconnected() {
        //Given
        int networkType = NETWORK_TYPE_IWLAN;
        Transport mockTransport = mock(Transport.class);
        mHandoverDecision.setEventInfo((Object) mockTransport);
        when(mockTransport.getType()).thenReturn(TRANSPORT_WIFI);

        //When
        int lastPrefNw = mHandoverDecision.processHandoverState(networkType, EVENT_APN_DISCONNECTED);

        //Then
        assertEquals(mHandoverDecision.mIdleState, mHandoverDecision.mState);
    }

    @Test
    public void testProcessHandoverStateHysteresisTimerExpired() {
        //Given
        int networkType = NETWORK_TYPE_IWLAN;

        //When
        int lastPrefNw = mHandoverDecision.processHandoverState(networkType, EVENT_HYSTERESIS_TIMER_EXPIRED);

        //Then
        assertEquals(mHandoverDecision.mIdleState, mHandoverDecision.mState);
    }

    @Test
    public void testProcessHandoverFailed_EVENT_APN_CONNECTING() {
        //Given
        int lastPrefNw;
        int networkType = NETWORK_TYPE_IWLAN;

        //When
        lastPrefNw = mHandoverDecision.processHandoverFailedState(networkType,
                EVENT_APN_CONNECTING);
    }

    @Test
    public void testProcessHandoverFailed_EVENT_APN_DISCONNECTED() {
        //Given
        int lastPrefNw;
        int networkType = NETWORK_TYPE_IWLAN;
        Transport mockTransport = mock(Transport.class);
        mHandoverDecision.setEventInfo((Object) mockTransport);
        when(mockTransport.getType()).thenReturn(TRANSPORT_WIFI);

        //When
        lastPrefNw = mHandoverDecision.processHandoverFailedState(networkType,
                EVENT_APN_DISCONNECTED);
    }

    @Test
    public void testProcessHandoverFailed_EVENT_APN_FAILURE() {
        //Given
        int lastPrefNw;
        Transport mockTransport = mock(Transport.class);
        mHandoverDecision.setEventInfo((Object) mockTransport);
        when(mockTransport.getType()).thenReturn(TRANSPORT_WIFI);
        int networkType = TelephonyManager.NETWORK_TYPE_UNKNOWN;

        //When
        lastPrefNw = mHandoverDecision.processHandoverFailedState(networkType,
                EVENT_APN_FAILURE);
    }

    @Test
    public void testProcessHandoverFailed_EVENT_THROTTLE_TIMER_EXPIRED() {
        //Given
        int lastPrefNw;
        int networkType = NETWORK_TYPE_IWLAN;

        //When
        lastPrefNw = mHandoverDecision.processHandoverFailedState(networkType,
                EVENT_THROTTLE_TIMER_EXPIRED);
    }

    @Test
    public void testProcessHandoverFailed_EVENT_APN_UNTHROTTLED() {
        //Given
        int lastPrefNw;
        int networkType = NETWORK_TYPE_IWLAN;

        //When
        lastPrefNw = mHandoverDecision.processHandoverFailedState(networkType,
                EVENT_APN_UNTHROTTLED);
    }

    @Test
    public void testProcessHandoverFailed_default() {
        //Given
        int lastPrefNw;
        int networkType = NETWORK_TYPE_IWLAN;

        //When
        lastPrefNw = mHandoverDecision.processHandoverFailedState(networkType,
                EVENT_IMS_CALL_STARTED);
    }

    @Test
    public void testProcessHandoverStateDefaultCase() {
        //Given
        int networkType = NETWORK_TYPE_IWLAN;

        //When
        int lastPrefNw = mHandoverDecision.processHandoverState(networkType, EVENT_CALL_STARTED);

        //Then
        assertEquals(mHandoverDecision.mIdleState, mHandoverDecision.mState);
    }

    @Test
    public void testPreCheckEvent() {
        //Given
        int networkType = NETWORK_TYPE_IWLAN;

        //When
        int nwType = mHandoverDecision.preCheckEvent(networkType, EVENT_APN_CONNECTED);

        //Then
        assertEquals(NETWORK_TYPE_UNKNOWN, nwType);
    }

    @Test
    public void testPostCheckEvent() {
        //Given
        int networkType = NETWORK_TYPE_IWLAN;
        int preferredType = NETWORK_TYPE_LTE_CA;

        //When
        mHandoverDecision.postCheckDecision(networkType, preferredType);

        //Then
        assertEquals(mHandoverDecision.mHandoverState, mHandoverDecision.mState);
    }

    @Test
    public void testAddToList() {
        //Given
        PreferredList preferredList = mHandoverDecision.getPreferredList(EVENT_APN_CONNECTED);
        int network = NETWORK_TYPE_UMTS;
        int lteNetwork = NETWORK_TYPE_LTE;

        //When
        mHandoverDecision.addToList(preferredList, network);
        mHandoverDecision.addCellular(preferredList, lteNetwork);
        mHandoverDecision.addWiFi(preferredList);

        //Then
        assertEquals(true, preferredList.contains(2)); //UTRAN = AccessNetwork.UTRAN
        assertEquals(true, preferredList.contains(3)); //EUTRAN = AccessNetwork.EUTRAN
        assertEquals(true, preferredList.contains(5)); //IWLAN = AccessNetwork.IWLAN
    }

    @Test
    public void testIsTransportNotAvailableForInitial() {
        assertEquals(true, mHandoverDecision
                .isTransportNotAvailableForInitial(NETWORK_TYPE_UNKNOWN));

        assertEquals(true, mHandoverDecision
                .isTransportNotAvailableForInitial(NETWORK_TYPE_IWLAN));

        assertEquals(true, mHandoverDecision
                .isTransportNotAvailableForInitial(NETWORK_TYPE_LTE));
    }

    @Test
    public void testIsHandoverNotAllowedByRat_on_expectional_case() {
        // We don't handle "ApnSetting.TYPE_ENTERPRISE",
        // Need to handle "ApnSetting.TYPE_ENTERPRISE" of APN type.
        mHandoverDecision = new HandoverDecision(ApnSetting.TYPE_ENTERPRISE, mockHandoverCond);

        assertEquals(false, mHandoverDecision
                .isHandoverNotAllowedByRat(NETWORK_TYPE_LTE, NETWORK_TYPE_IWLAN));
    }

    @Test
    public void testIsRoaming() {
        Transport mockWiFiTransport;
        Transport mockCellularTransport;

        // when
        when(mockConditionCellular.isRegistered()).thenReturn(true);
        when(mockConditionCellular.isRoaming()).thenReturn(true);
        // then
        assertEquals(true, mHandoverDecision.isRoaming());

        // when
        mockCellularTransport = mock(Transport.class);
        when(mockCellularTransport.getType()).thenReturn(TransportType.TRANSPORT_CELLULAR);
        when(mockCellularTransport.isRoaming()).thenReturn(true);
        when(mockConditionCellular.isRegistered()).thenReturn(false);
        when(mockConnectivity.getCurrentTransport(anyInt())).thenReturn(mockCellularTransport);
        // then
        assertEquals(true, mHandoverDecision.isRoaming());

        // when
        mockWiFiTransport = mock(Transport.class);
        when(mockWiFiTransport.getType()).thenReturn(TransportType.TRANSPORT_WIFI);
        when(mockWiFiTransport.isRoaming()).thenReturn(true);
        when(mockConditionCellular.isCsRegistered()).thenReturn(true);
        when(mockConditionCellular.isCsRoaming()).thenReturn(true);
        when(mockConnectivity.getCurrentTransport(anyInt())).thenReturn(mockWiFiTransport);
        // then
        assertEquals(true, mHandoverDecision.isRoaming());

        // when
        mockWiFiTransport = mock(Transport.class);
        when(mockWiFiTransport.getType()).thenReturn(TransportType.TRANSPORT_WIFI);
        when(mockWiFiTransport.isRoaming()).thenReturn(true);
        when(mockConditionCellular.isCsRegistered()).thenReturn(false);
        when(mockWiFiTransport.isRoaming()).thenReturn(false);
        when(mockConnectivity.getCurrentTransport(anyInt())).thenReturn(mockWiFiTransport);
        // then
        assertEquals(false, mHandoverDecision.isRoaming());
    }
}
