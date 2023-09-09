package com.shannon.qualifiednetworksservice;

import android.content.Context;
import android.telephony.CarrierConfigManager;
import android.telephony.TelephonyManager;
import android.util.SparseArray;

import junit.framework.TestCase;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.Mock;
import org.mockito.MockedStatic;
import org.mockito.Mockito;
import org.mockito.MockitoAnnotations;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.annotation.Config;

import static org.mockito.ArgumentMatchers.anyObject;
import static org.mockito.Mockito.when;

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class ConditionMobileTest extends TestCase {
    private ConditionMobile mConditions;

    @Mock
    Context mockContext;

    @Mock
    ConfigurationManager mockConfiguration;

    @Mock
    HandoverConditions.IConditionChanged mockCallback;

    @Mock
    SimManager mockSimManager;

    @Mock
    MobileNetworkCallback mockNetworkCallback;

    @Mock
    CarrierConfigManager mockCarrierConfigManager;

    private static MockedStatic<SimManager> mockedStaticSimManager;

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.openMocks(this);

        mockedStaticSimManager = Mockito.mockStatic(SimManager.class);
        when(SimManager.getInstance()).thenReturn(mockSimManager);

        SparseArray<MobileNetworkCallback> array = new SparseArray<>();
        when(mockSimManager.registerForNetworkCallbackEvent(anyObject())).thenReturn(array);

        ConditionMobile conditions = new ConditionMobile(0, mockContext, mockConfiguration, mockCallback);

        array.append(0, mockNetworkCallback);
        array.append(1, mockNetworkCallback);

        when(mockContext.getSystemService(CarrierConfigManager.class)).thenReturn(mockCarrierConfigManager);
        mConditions = new ConditionMobile(0, mockContext, mockConfiguration, mockCallback);
    }

    @After
    public void tearDown() throws Exception {
        mockedStaticSimManager.close();
        Mockito.validateMockitoUsage();
    }

    @Test
    public void testTestDestroy() {
        mConditions.destroy();
        assertTrue(true);
    }

    @Test
    public void testTestIsVoMobilePossible() {
        when(mockNetworkCallback.isConnected()).thenReturn(false);
        when(mockNetworkCallback.getNetworkType()).thenReturn(TelephonyManager.NETWORK_TYPE_UNKNOWN);
        when(mockNetworkCallback.isRoaming()).thenReturn(false);
        when(mockNetworkCallback.getNetworkOperator()).thenReturn("");
        when(mockConfiguration.getVoMobileAllowedRatList()).thenReturn("");
        when(mockConfiguration.isVoMobileEnabledOnRoamingNetwork()).thenReturn(false);
        when(mockConfiguration.getVoMobileForbiddenNetworksInRoaming()).thenReturn("");
        when(mockConfiguration.getVoMobileOtherSignalIn()).thenReturn(-105);
        when(mockConfiguration.getVoMobileOtherSignalOut()).thenReturn(-115);
        when(mockNetworkCallback.isSignalBetterThanThreshold(-110)).thenReturn(false);
        when(mockNetworkCallback.isSignalBetterThanThreshold(-115)).thenReturn(false);
        assertFalse(mConditions.isVoMobilePossible(false, false, false));
        assertFalse(mConditions.isVoMobilePossible(false, true, true));

        when(mockNetworkCallback.isConnected()).thenReturn(true);
        when(mockNetworkCallback.getNetworkType()).thenReturn(TelephonyManager.NETWORK_TYPE_LTE);
        when(mockConfiguration.getVoMobileAllowedRatList()).thenReturn("NR,LTE");
        when(mockConfiguration.getVoMobileForbiddenNetworksInRoaming()).thenReturn("10,254,310,311480,310260");
        assertTrue(mConditions.isVoMobilePossible(false, false, false));
        assertFalse(mConditions.isVoMobilePossible(true, true, true));

        when(mockConfiguration.isVoMobileEnabledOnRoamingNetwork()).thenReturn(true);
        when(mockNetworkCallback.getNetworkOperator()).thenReturn("310260");
        assertTrue(mConditions.isVoMobilePossible(false, false, false));
        assertFalse(mConditions.isVoMobilePossible(true, true, true));

        when(mockConfiguration.getVoMobileForbiddenNetworksInRoaming()).thenReturn("10,254,311480,310260");
        assertTrue(mConditions.isVoMobilePossible(false, false, false));
        assertFalse(mConditions.isVoMobilePossible(true, true, true));

        when(mockConfiguration.getVoMobileForbiddenNetworksInRoaming()).thenReturn("10");
        when(mockNetworkCallback.isSignalBetterThanThreshold(-110)).thenReturn(true);
        when(mockNetworkCallback.isSignalBetterThanThreshold(-115)).thenReturn(true);
        assertTrue(mConditions.isVoMobilePossible(false, false, false));
        assertTrue(mConditions.isVoMobilePossible(true, true, true));
    }

    @Test
    public void testMobileCallbackEvent() {
        ConditionMobile.MobileCallbackEvent eventCallback = mConditions.getMobileCallbackEvents();

        eventCallback.onCallbackConditionsChanged(HandoverConditions.ConditionEvents.EVENT_UNKNOWN);

        eventCallback.onCallbackRemoved(0);
        eventCallback.onCallbackRemoved(1);

        eventCallback.onCallbackAdded(0, mockNetworkCallback);
        eventCallback.onCallbackAdded(1, mockNetworkCallback);
        assertTrue(true);
    }

    @Test
    public void testIsOpportunisticNetworkConnectedAndPreferred() {
        when(mockSimManager.getSubId(0)).thenReturn(1);
        when(mockCarrierConfigManager.getConfigForSubId(1)).thenReturn(null);
        when(mockNetworkCallback.isConnected()).thenReturn(true);
        when(mockNetworkCallback.getSlotId()).thenReturn(1);
        when(mockNetworkCallback.getSubId()).thenReturn(2);
        when(mockSimManager.isOpportunisticSubscription(1, 2)).thenReturn(true);
        assertFalse(mConditions.isOpportunisticNetworkConnectedAndPreferred(true));
    }
}