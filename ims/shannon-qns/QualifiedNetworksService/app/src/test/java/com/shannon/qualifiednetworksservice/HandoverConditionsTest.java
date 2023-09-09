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

import static android.telephony.AccessNetworkConstants.AccessNetworkType.IWLAN;
import static com.shannon.qualifiednetworksservice.HandoverConditions.ConditionEvents.EVENT_APN_CONNECTED;
import static com.shannon.qualifiednetworksservice.HandoverConditions.ConditionEvents.EVENT_OPERATOR_CHANGED;
import static org.mockito.ArgumentMatchers.any;
import static org.mockito.ArgumentMatchers.anyInt;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.mockStatic;
import static org.mockito.Mockito.spy;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

import android.content.Context;
import android.net.Network;
import android.telephony.SubscriptionInfo;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
import android.telephony.data.ApnSetting;
import android.telephony.data.QualifiedNetworksService;
import android.telephony.data.ThrottleStatus;

import com.shannon.qualifiednetworksservice.ApnQualifiedNetworksProvider.ApnConditionsHandler;

import com.shannon.qualifiednetworksservice.HandoverConditions.ConditionEvents;
import com.shannon.qualifiednetworksservice.HandoverConditions.EventInfo;
import java.util.List;
import junit.framework.TestCase;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.Mock;
import org.mockito.MockedStatic;
import org.mockito.MockitoAnnotations;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.annotation.Config;

import androidx.test.platform.app.InstrumentationRegistry;

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class HandoverConditionsTest extends TestCase {
    @BeforeClass
    public static void beforeClass() {
        sCarrierType = mockStatic(CarrierType.class);
        sRilOemInterface =  mockStatic(RilOemInterface.class);
    }

    @AfterClass
    public static void afterClass() {
        sRilOemInterface.close();
        sCarrierType.close();
    }

    private static MockedStatic<RilOemInterface> sRilOemInterface;
    private static MockedStatic<CarrierType> sCarrierType;

    private static final int testSlotId = 0;
    private static final int testSubId = 1;
    private static final int testNetId = 100;
    private static final int testMcc = 310;
    private static final int testMnc = 260;


    @Mock
    private ShannonQualifiedNetworksService mMockService;
    @Mock private ApnQualifiedNetworksProvider mMockApnQualifiedNetworksProvider = mock(ApnQualifiedNetworksProvider.class);
    @Mock private QualifiedNetworksService.NetworkAvailabilityProvider mMockShannonNetworkAvailabilityProvider = mock(QualifiedNetworksService.NetworkAvailabilityProvider.class);
    @Mock private Network mMockNetwork;
    @Mock private SubscriptionManager mMockSubscriptionManager;
    @Mock private SubscriptionInfo mMockSubscriptionInfo;
    @Mock private TelephonyManager mMockTelephonyManager;
    @Mock SimManager.SimBroadcastReceiver mSimBroadcastReceiver;
    @Mock ConfigurationManager   mockConfigurationManager;

    @Mock private RilOemInterface mockRilOemInterface;
    @Mock private ConditionUserSetting mockConditionUserSetting;
    @Mock private ConditionCellular mockConditionCellular;
    @Mock private ConditionWiFi mockConditionWiFi;
    @Mock private ConditionProvisioning mockConditionProvisioning;
    @Mock private ConditionCall mockConditionCall;
    @Mock private ConditionMobile mockConditionMobile;
    @Mock private ConditionConnectivity mockConditionConnectivity;

    private Context mContext = InstrumentationRegistry.getInstrumentation().getContext();
    private HandoverConditions mConditions;
    private HandoverConditions.HandoverConditionsHelper mHandoverConditionsHelper;

    private SimManager mSimManager;
    private ApnQualifiedNetworksProvider mApnQualifiedNetworksProvider;
    private QualifiedNetworksService.NetworkAvailabilityProvider mShannonNetworkAvailabilityProvider;
    private ShannonQualifiedNetworksService mShannonQualifiedNetworksService;
    SimInformation mTestSimInformation;

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.openMocks(this);
        mSimManager = spy(new SimManager(mContext));
        mSimBroadcastReceiver = mSimManager.getReceiver();

        when(mMockTelephonyManager.getNetworkSpecifier())
            .thenReturn(String.valueOf(testSlotId));
        when(mMockSubscriptionManager.getSubscriptionIds(testSlotId))
            .thenReturn(new int[]{TelephonyManager.SIM_STATE_READY});

        // For test, HandoverCondition as a default
        mConditions = new HandoverConditions(testSlotId, mContext,
            mockConfigurationManager, mockRilOemInterface, mockConditionUserSetting, mockConditionCellular,
            mockConditionWiFi, mockConditionProvisioning, mockConditionCall, mockConditionMobile, mockConditionConnectivity);
        mHandoverConditionsHelper = mConditions.new HandoverConditionsHelper();
    }

    @After
    public void tearDown() throws Exception {
        mConditions.destroy();
    }

    @Test
    public void testGetSlotIndex() {
        //Given


        //When
        int returnVal = mConditions.getSlotIndex();

        //Then
        assertEquals(testSlotId, returnVal);
    }

    @Test
    public void testGetContext() {
        //Given


        //When
        Context returnVal = mConditions.getContext();

        //Then
        assertEquals(mContext, returnVal);
    }

    @Test
    public void testGetCellularConditions() {
        //Given


        //When
        ConditionCellular returnVal = mConditions.getCellularConditions();

        //Then
        assertNotNull(returnVal);
    }

    @Test
    public void testGetWiFiConditions() {
        //Given


        //When
        ConditionWiFi returnVal = mConditions.getWiFiConditions();

        //Then
        assertNotNull(returnVal);
    }

    @Test
    public void testGetCallConditions() {
        //Given


        //When
        ConditionCall returnVal = mConditions.getCallConditions();

        //Then
        assertNotNull(returnVal);
    }

    @Test
    public void testGetUserConditions() {
        //Given


        //When
        ConditionUserSetting returnVal = mConditions.getUserConditions();

        //Then
        assertNotNull(returnVal);
    }

    @Test
    public void testGetProvisioningConditions() {
        //Given


        //When
        ConditionProvisioning returnVal = mConditions.getProvisioningConditions();

        //Then
        assertNotNull(returnVal);
    }

    @Test
    public void testGetMobileConditions() {
        //Given


        //When
        ConditionMobile returnVal = mConditions.getMobileConditions();

        //Then
        assertNotNull(returnVal);
    }

    @Test
    public void testGetConfigurationManager() {
        //Given


        //When
        ConfigurationManager returnVal = mConditions.getConfigurationManager();

        //Then
        assertNotNull(returnVal);
    }

    @Test
    public void testGetTransportCondition() {
        //Given


        //When
        ConditionConnectivity returnVal = mConditions.getTransportCondition();

        //Then
        assertNotNull(returnVal);
    }

    @Test
    public void testGetHandoverDecisionForApn() {
        //Given


        //When
        HandoverDecision mmsHandoverObject = mConditions.getHandoverDecisionForApn(ApnSetting.TYPE_MMS);
        HandoverDecision imsHandoverObject = mConditions.getHandoverDecisionForApn(ApnSetting.TYPE_IMS);
        HandoverDecision xcapHandoverObject = mConditions.getHandoverDecisionForApn(ApnSetting.TYPE_XCAP);
        HandoverDecision emcHandoverObject = mConditions.getHandoverDecisionForApn(ApnSetting.TYPE_EMERGENCY);
        HandoverDecision cbsHandoverObject = mConditions.getHandoverDecisionForApn(ApnSetting.TYPE_CBS);
        HandoverDecision mcxHandoverObject = mConditions.getHandoverDecisionForApn(ApnSetting.TYPE_MCX);

        //Then
        assertTrue(mmsHandoverObject instanceof MmsHandoverDecision);
        assertTrue(imsHandoverObject instanceof ImsHandoverDecision);
        assertTrue(xcapHandoverObject instanceof XcapHandoverDecision);
        assertTrue(emcHandoverObject instanceof EmcHandoverDecision);
        assertTrue(cbsHandoverObject instanceof CbsHandoverDecision);
        assertTrue(mcxHandoverObject instanceof HandoverDecision);
    }

    @Test
    public void testGetHandoverDecisionForApnIMS() {
        HandoverDecision imsHandoverObject;

        //Given
        //When
        when(mSimManager.getTelephonyManager(anyInt())).thenReturn(mMockTelephonyManager);
        when(CarrierType.getEnum(any())).thenReturn(CarrierType.CARRIER_TYPE_ATT);
        imsHandoverObject = mConditions.getHandoverDecisionForApn(ApnSetting.TYPE_IMS);

        //Then
        assertTrue(imsHandoverObject instanceof ImsHandoverDecisionAttUs);

        //When
        when(mSimManager.getTelephonyManager(anyInt())).thenReturn(mMockTelephonyManager);
        when(CarrierType.getEnum(any())).thenReturn(CarrierType.CARRIER_TYPE_TMO);
        imsHandoverObject = mConditions.getHandoverDecisionForApn(ApnSetting.TYPE_IMS);

        //Then
        assertTrue(imsHandoverObject instanceof ImsHandoverDecisionTmoUs);

        //When
        when(mSimManager.getTelephonyManager(anyInt())).thenReturn(mMockTelephonyManager);
        when(CarrierType.getEnum(any())).thenReturn(CarrierType.CARRIER_TYPE_VZW);
        imsHandoverObject = mConditions.getHandoverDecisionForApn(ApnSetting.TYPE_IMS);

        //Then
        assertTrue(imsHandoverObject instanceof ImsHandoverDecisionVzwUs);

        //When
        when(mSimManager.getTelephonyManager(anyInt())).thenReturn(mMockTelephonyManager);
        when(CarrierType.getEnum(any())).thenReturn(CarrierType.CARRIER_TYPE_ORANGE);
        imsHandoverObject = mConditions.getHandoverDecisionForApn(ApnSetting.TYPE_IMS);

        //Then
        assertTrue(imsHandoverObject instanceof ImsHandoverDecisionOrange);

        //When
        when(mSimManager.getTelephonyManager(anyInt())).thenReturn(mMockTelephonyManager);
        when(CarrierType.getEnum(any())).thenReturn(CarrierType.CARRIER_TYPE_SWISSCOM);
        imsHandoverObject = mConditions.getHandoverDecisionForApn(ApnSetting.TYPE_IMS);

        //Then
        assertTrue(imsHandoverObject instanceof ImsHandoverDecisionSwisscom);

        //When
        when(mSimManager.getTelephonyManager(anyInt())).thenReturn(mMockTelephonyManager);
        when(CarrierType.getEnum(any())).thenReturn(CarrierType.CARRIER_TYPE_DTAG);
        imsHandoverObject = mConditions.getHandoverDecisionForApn(ApnSetting.TYPE_IMS);

        //Then
        assertTrue(imsHandoverObject instanceof ImsHandoverDecisionDtag);

        //When
        when(mSimManager.getTelephonyManager(anyInt())).thenReturn(mMockTelephonyManager);
        when(CarrierType.getEnum(any())).thenReturn(CarrierType.CARRIER_TYPE_RJIO);
        imsHandoverObject = mConditions.getHandoverDecisionForApn(ApnSetting.TYPE_IMS);

        //Then
        assertTrue(imsHandoverObject instanceof ImsHandoverDecisionRJio);
    }

    @Test
    public void testEventInfo() {
        //Given
        ConditionEvents event = EVENT_OPERATOR_CHANGED;
        Object object = mock(Object.class);

        //When
        HandoverConditions.EventInfo eventInfo = new EventInfo(event, object);

        //Then
        assertEquals(event, eventInfo.getEvent());
        assertEquals(object, eventInfo.getInfo());
    }

    @Test
    public void testonConditionChangedOneParam() {
        //Given
        ConditionEvents condEvents = EVENT_APN_CONNECTED;
        ApnConditionsHandler handler = mock(ApnConditionsHandler.class);
        HandoverConditions.HandoverConditionsCallback handoverConditionsCallback = mConditions.new HandoverConditionsCallback();
        mConditions.registerListener(handler);

        //When
        handoverConditionsCallback.onConditionChanged(condEvents);

        //Then
        List<ApnConditionsHandler> list =  mConditions.getlistenersList();
        assertEquals(1, list.size());
        mConditions.unregisterListener(handler);
    }

    @Test
    public void testonConditionChangedTwoParams() {
        //Given
        ConditionEvents condEvents = EVENT_APN_CONNECTED;
        ApnConditionsHandler handler = mock(ApnConditionsHandler.class);
        int apnBitmask = 15;
        HandoverConditions.HandoverConditionsCallback handoverConditionsCallback = mConditions.new HandoverConditionsCallback();
        mConditions.registerListener(handler);

        //When
        handoverConditionsCallback.onConditionChanged(apnBitmask, condEvents);

        //Then
        List<ApnConditionsHandler> list =  mConditions.getlistenersList();
        assertEquals(1, list.size());
        mConditions.unregisterListener(handler);
    }

    @Test
    public void testonConditionChangedThreeParams() {
        //Given
        ConditionEvents condEvents = EVENT_APN_CONNECTED;
        ApnConditionsHandler handler = mock(ApnConditionsHandler.class);
        int apnBitmask = 15;
        Object obj = mock(Object.class);
        HandoverConditions.HandoverConditionsCallback handoverConditionsCallback = mConditions.new HandoverConditionsCallback();
        mConditions.registerListener(handler);

        //When
        handoverConditionsCallback.onConditionChanged(apnBitmask, condEvents, obj);

        //Then
        List<ApnConditionsHandler> list =  mConditions.getlistenersList();
        assertEquals(1, list.size());
        mConditions.unregisterListener(handler);
    }

    @Test
    public void testonConfigurationChanged() {
        //Given
        ConditionEvents condEvents = EVENT_APN_CONNECTED;
        ApnConditionsHandler handler = mock(ApnConditionsHandler.class);
        int apnBitmask = 15;
        Object obj = mock(Object.class);
        HandoverConditions.HandoverConditionsCallback handoverConditionsCallback = mConditions.new HandoverConditionsCallback();
        mConditions.registerListener(handler);

        //When
        mConditions.onConfigurationChanged();

        //Then
        List<ApnConditionsHandler> list =  mConditions.getlistenersList();
        assertEquals(1, list.size());
        mConditions.unregisterListener(handler);
    }

    @Test
    public void testIsBroadcastAllowedApn() {
        //Given
        ConditionEvents condEvents = EVENT_APN_CONNECTED;
        ApnConditionsHandler handler = mock(ApnConditionsHandler.class);
        int apnBitmask = 15;
        Object obj = mock(Object.class);
        HandoverConditions.HandoverConditionsCallback handoverConditionsCallback = mConditions.new HandoverConditionsCallback();
        mConditions.registerListener(handler);

        //Then
        List<ApnConditionsHandler> list =  mConditions.getlistenersList();
        assertEquals(1, list.size());
        mConditions.unregisterListener(handler);
    }

    @Test
    public void testonConditionChanged() {
        //Given
        ConditionEvents condEvents = EVENT_APN_CONNECTED;
        ApnConditionsHandler handler = mock(ApnConditionsHandler.class);
        int apnBitmask = 15;
        Object obj = mock(Object.class);
        HandoverConditions.HandoverConditionsCallback handoverConditionsCallback = mConditions.new HandoverConditionsCallback();
        mConditions.registerListener(handler);

        //When
        mConditions.onConditionChanged(IWLAN, condEvents);

        //Then
        List<ApnConditionsHandler> list =  mConditions.getlistenersList();
        assertEquals(1, list.size());
        mConditions.unregisterListener(handler);
    }

    @Test
    public void testIsAirplaneMode_IsRoaming() {
        // given
        when(mockConditionCellular.isAirplaneMode())
                .thenReturn(true);
        when(mockConditionCellular.isRegistered())
                .thenReturn(true);
        when(mockConditionCellular.isRoaming())
                .thenReturn(true);

        // when
        mHandoverConditionsHelper = mConditions.new HandoverConditionsHelper();

        // then
        assertEquals(true, mHandoverConditionsHelper.isAirplaneMode());
        assertEquals(true, mHandoverConditionsHelper.isRoaming());
    }

    @Test
    public void testIsRoaming() {
        // when
        when(mockConditionCellular.isAirplaneMode())
                .thenReturn(true);
        when(mockConditionCellular.isRegistered())
                .thenReturn(false);
        when(mockConditionCellular.isRoaming())
                .thenReturn(true);
        // then
        assertEquals(false, mHandoverConditionsHelper.isRoaming());

        // when
        when(mockConditionCellular.isAirplaneMode())
                .thenReturn(true);
        when(mockConditionCellular.isRegistered())
                .thenReturn(false);
        when(mockConditionCellular.isCsRegistered())
                .thenReturn(true);
        when(mockConditionCellular.isCsRoaming())
                .thenReturn(true);
        // then
        assertEquals(true, mHandoverConditionsHelper.isRoaming());
    }

    @Test
    public void testGetRsrpRoveInAndOut() {
        // when
        when(mockConditionCellular.getRsrpRoveIn()).thenReturn(90);
        when(mockConditionCellular.getRsrpRoveOut()).thenReturn(91);

        mHandoverConditionsHelper = mConditions.new HandoverConditionsHelper();

        // then
        assertEquals(90, mHandoverConditionsHelper.getRsrpRoveIn());
        assertEquals(91, mHandoverConditionsHelper.getRsrpRoveOut());
    }

    @Test
    public void testSetRsrpRoveInAndOut() {
        // when
        mHandoverConditionsHelper = mConditions.new HandoverConditionsHelper();

        mHandoverConditionsHelper.setRsrpRoveIn(50);
        mHandoverConditionsHelper.setRsrpRoveOut(51);

        // then
        verify(mockConditionCellular, times(1)).setRsrpRoveIn(50);
        verify(mockConditionCellular, times(1)).setRsrpRoveOut(51);
    }

    @Test
    public void testGetRsrpMid() {
        // when
        when(mockConditionCellular.getRsrpMid()).thenReturn(32);

        mHandoverConditionsHelper = mConditions.new HandoverConditionsHelper();

        // then
        assertEquals(32, mHandoverConditionsHelper.getRsrpMid());
    }

    @Test
    public void testSetRsrpMid() {
        // when
        mHandoverConditionsHelper = mConditions.new HandoverConditionsHelper();
        mHandoverConditionsHelper.setRsrpMid(25);

        // then
        verify(mockConditionCellular, times(1)).setRsrpMid(25);
    }

    @Test
    public void testGetCellularNetworkType() {
        // when
        when(mockConditionCellular.getNetworkType()).thenReturn(TelephonyManager.NETWORK_TYPE_LTE);
        mHandoverConditionsHelper = mConditions.new HandoverConditionsHelper();

        // then
        assertEquals(TelephonyManager.NETWORK_TYPE_LTE,
                mHandoverConditionsHelper.getCellularNetworkType());
    }

    @Test
    public void testSetDisconnectedFromNetwork() {
        // when
        mHandoverConditionsHelper = mConditions.new HandoverConditionsHelper();
        mHandoverConditionsHelper.setDisconnectedFromNetwork(true);

        // then
        verify(mockConditionCellular, times(1)).
                setDisconnectedFromNetwork(true);
    }

    @Test
    public void testSetWiFiRssiRoveInAndOut() {
        // when
        mHandoverConditionsHelper = mConditions.new HandoverConditionsHelper();
        mHandoverConditionsHelper.setWiFiRssiRoveIn(77);
        mHandoverConditionsHelper.setWiFiRssiRoveOut(78);

        // then
        verify(mockConditionWiFi, times(1)).
                setRssiRoveIn(77);
        verify(mockConditionWiFi, times(1)).
                setRssiRoveOut(78);
    }

    @Test
    public void testGetWiFiRssiRoveInAndOut() {
        // when
        when(mockConditionWiFi.getRssiRoveIn()).thenReturn(26);
        when(mockConditionWiFi.getRssiRoveOut()).thenReturn(28);

        mHandoverConditionsHelper = mConditions.new HandoverConditionsHelper();

        // then
        assertEquals(26, mHandoverConditionsHelper.getWiFiRssiRoveIn());
        assertEquals(28, mHandoverConditionsHelper.getWiFiRssiRoveOut());
    }

    @Test
    public void testSetWfcEnabledRoaming() {
        // when
        when(mockConditionUserSetting.isWfcEnabledByPlatform()).thenReturn(true);
        mHandoverConditionsHelper = mConditions.new HandoverConditionsHelper();

        mHandoverConditionsHelper.setWfcEnabledRoaming(true);

        // then
        verify(mockConditionUserSetting, times(1)).
                setWfcEnabledRoaming(true);
        assertEquals(true, mHandoverConditionsHelper.isWfcEnabledByPlatform());
    }

    @Test
    public void testClearLastCallFailCause() {
        // when
        mHandoverConditionsHelper = mConditions.new HandoverConditionsHelper();
        mHandoverConditionsHelper.clearLastCallFailCause(TelephonyManager.NETWORK_TYPE_IWLAN);

        // then
        verify(mockConditionCall, times(1)).
                clearLastCallFailCause(TelephonyManager.NETWORK_TYPE_IWLAN);
    }

    @Test
    public void testIsWiFiTransportNotUsed() {
        // when
        when(mockConditionConnectivity.isWiFiTransportUsed()).thenReturn(false);

        mHandoverConditionsHelper = mConditions.new HandoverConditionsHelper();

        // then
        assertEquals(true, mHandoverConditionsHelper.isWiFiTransportNotUsed());
    }

    @Test
    public void testIsImsConnectedOnWiFi() {
        // when
        when(mockConditionConnectivity.getNetworkType(ApnSetting.TYPE_IMS))
                .thenReturn(TelephonyManager.NETWORK_TYPE_IWLAN);
        mHandoverConditionsHelper = mConditions.new HandoverConditionsHelper();

        // then
        assertEquals(true, mHandoverConditionsHelper.isImsConnectedOnWiFi());
    }

    @Test
    public void testIsInHandoverState() {
        // when
        mHandoverConditionsHelper = mConditions.new HandoverConditionsHelper();
        mConditions.getHandoverDecisionForApn(ApnSetting.TYPE_IMS);

        // then
        assertEquals(false, mHandoverConditionsHelper
                .isInHandoverState(ApnSetting.TYPE_IMS));
    }

    @Test
    public void testOnThrottleStatusChanged() {
        // when
        mHandoverConditionsHelper = mConditions.new HandoverConditionsHelper();
        mConditions.onThrottleStatusChanged(any());

        // then
        verify(mockConditionConnectivity, times(1)).
                onThrottleStatusChanged(any());
    }
}
