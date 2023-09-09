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

import android.os.Bundle;
import android.os.Message;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
import android.telephony.data.QualifiedNetworksService.NetworkAvailabilityProvider;
import android.telephony.data.ThrottleStatus;

import com.android.internal.util.FastPrintWriter;
import com.android.internal.util.IndentingPrintWriter;
import com.shannon.qualifiednetworksservice.util.PackageVersion;
import com.shannon.vendorcarrierconfig.ShannonVendorCarrierConfig;
import junit.framework.TestCase;
import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.Mock;
import org.mockito.MockedConstruction;
import org.mockito.MockedStatic;
import org.mockito.Mockito;
import org.mockito.MockitoAnnotations;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.annotation.Config;
import static org.mockito.ArgumentMatchers.any;
import static org.mockito.ArgumentMatchers.anyInt;
import static org.mockito.Mockito.doNothing;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.mockStatic;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.List;

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class ShannonQualifiedNetworksServiceTest extends TestCase {
    private static final int SIM_STATE_CHANGED = 1;
    private static final int APN_NETWORK_LIST_UPDATED = 2;
    private static final int VENDOR_CONFIG_UPDATED = 3;

    private ShannonQualifiedNetworksService mShannonQualifiedNetworksService;
    private ShannonQualifiedNetworksService.ShannonNetworkAvailabilityProvider mShannonNetworkAvailabilityProvider;
    android.content.Context context;

    ShannonQualifiedNetworksService.LocalBinder mLocalBinder;
    ShannonVendorCarrierConfig vendorConfig;

    private static MockedStatic<CarrierType> sCarrierType;
    private static MockedStatic<SubscriptionManager> sSubscriptionManager;
    private static MockedStatic<SimManager> sSimManagerStatic;

    @Mock
    SimManager mockSimManager = mock(SimManager.class);

    @Mock
    TelephonyManager mockTelephonyManager = mock(TelephonyManager.class);

    @BeforeClass
    public static void beforeClass() {
        sCarrierType = mockStatic(CarrierType.class);
        sSubscriptionManager =  mockStatic(SubscriptionManager.class);
        sSimManagerStatic = mockStatic(SimManager.class);
    }

    @AfterClass
    public static void afterClass() {
        sCarrierType.close();
        sSubscriptionManager.close();
        sSimManagerStatic.close();
    }

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.openMocks(this);

        context = mock(android.content.Context.class);
        vendorConfig = mock(ShannonVendorCarrierConfig.class);
        LocationManager locationManager = mock(LocationManager.class);

        when(SimManager.getInstance()).thenReturn(mockSimManager);
        when(mockSimManager.getSubId(anyInt())).thenReturn(-1);
        doNothing().when(vendorConfig).destroy();

        mShannonQualifiedNetworksService = new ShannonQualifiedNetworksService();
        mShannonQualifiedNetworksService.onCreate(context, vendorConfig, locationManager,
                mockSimManager);
        mShannonNetworkAvailabilityProvider = mShannonQualifiedNetworksService.new ShannonNetworkAvailabilityProvider(0);
    }

    @After
    public void tearDown() throws Exception {
        mShannonQualifiedNetworksService.onDestroy();
        mShannonNetworkAvailabilityProvider.close();
    }

    @Test
    public void testReportThrottleStatusChanged() {
        List<ThrottleStatus> statuses = new ArrayList<>();
        ThrottleStatus mockThrottleStatus = mock(ThrottleStatus.class);
        statuses.add(mockThrottleStatus);
        when(mockThrottleStatus.getSlotIndex()).thenReturn(0);
        HandoverConditions mockHandoverConditions = mock(HandoverConditions.class);

        mShannonNetworkAvailabilityProvider.setHandoverCondition(mockHandoverConditions);
        mShannonNetworkAvailabilityProvider.reportThrottleStatusChanged(statuses);
    }

    @Test
    public void testHandleVENDOR_CONFIG_UPDATEDMessage() throws Exception {
        //Given
        Message mockMessage = mock(Message.class);
        mockMessage.what = VENDOR_CONFIG_UPDATED;
        HandoverConditions mockHandoverConditions = mock(HandoverConditions.class);
        mShannonNetworkAvailabilityProvider.setHandoverCondition(mockHandoverConditions);

        //When
        mShannonNetworkAvailabilityProvider.getHandler().handleMessage(mockMessage);

        //Then
        verify(mockHandoverConditions, times(1)).onConfigurationChanged();
    }

    @Test
    public void testServiceDump() throws FileNotFoundException {
        //Given
        FileOutputStream fout = new FileOutputStream(new File("./test.txt"));
        PrintWriter pw = new FastPrintWriter(fout);

        when(mockSimManager.getTelephonyManager(0)).thenReturn(null);

        //When
        mShannonQualifiedNetworksService.onCreateNetworkAvailabilityProvider(0);

        mShannonQualifiedNetworksService.dump(null,
                new android.util.IndentingPrintWriter(new FastPrintWriter(pw), ""), null);
    }

    @Test
    public void testServiceDump1() throws FileNotFoundException {
        //Given
        FileOutputStream fout = new FileOutputStream(new File("./test.txt"));
        PrintWriter pw = new FastPrintWriter(fout);
        CarrierType mockCarrierType = mock(CarrierType.class);

        //When
        when(SimManager.getInstance()).thenReturn(mockSimManager);
        when(mockSimManager.getSubId(anyInt())).thenReturn(-1);
        when(mockSimManager.getTelephonyManager(0)).thenReturn(mockTelephonyManager);
        when(mockSimManager.getTelephonyManager(1)).thenReturn(mockTelephonyManager);
        sCarrierType.when(() -> CarrierType.getEnum(any())).thenReturn(mockCarrierType);
        when(mockSimManager.getMcc(0)).thenReturn(310);
        when(mockSimManager.getMcc(1)).thenReturn(310);
        when(mockSimManager.getMnc(0)).thenReturn(260);
        when(mockSimManager.getMnc(1)).thenReturn(260);

        context = mock(android.content.Context.class);
        vendorConfig = mock(ShannonVendorCarrierConfig.class);
        LocationManager locationManager = mock(LocationManager.class);

        mShannonQualifiedNetworksService = new ShannonQualifiedNetworksService();
        mShannonQualifiedNetworksService.onCreate(context, vendorConfig, locationManager,
                mockSimManager);

        mShannonQualifiedNetworksService.onCreateNetworkAvailabilityProvider(0);
        mShannonQualifiedNetworksService.onCreateNetworkAvailabilityProvider(1);


        mShannonQualifiedNetworksService.dump(null,
                new android.util.IndentingPrintWriter(new FastPrintWriter(pw), ""), null);
    }

    @Test
    public void testServiceDump2() throws FileNotFoundException {
        //Given
        FileOutputStream fout = new FileOutputStream(new File("./test.txt"));
        PrintWriter pw = new FastPrintWriter(fout);

        //When
        when(SimManager.getInstance()).thenReturn(mockSimManager);
        when(mockSimManager.getSubId(anyInt())).thenReturn(-1);
        when(mockSimManager.getTelephonyManager(0)).thenReturn(mockTelephonyManager);
        when(mockSimManager.getTelephonyManager(1)).thenReturn(mockTelephonyManager);
        sCarrierType.when(() -> CarrierType.getEnum(any())).thenReturn(null);
        when(mockTelephonyManager.getSimSpecificCarrierId()).thenReturn(TelephonyManager.UNKNOWN_CARRIER_ID);
        when(mockTelephonyManager.getSimCarrierId()).thenReturn(TelephonyManager.UNKNOWN_CARRIER_ID);
        when(mockTelephonyManager.getCarrierIdFromSimMccMnc()).thenReturn(1 /* TMO */);
        when(mockSimManager.getMcc(0)).thenReturn(310);
        when(mockSimManager.getMcc(1)).thenReturn(310);
        when(mockSimManager.getMnc(0)).thenReturn(260);
        when(mockSimManager.getMnc(1)).thenReturn(260);

        context = mock(android.content.Context.class);
        vendorConfig = mock(ShannonVendorCarrierConfig.class);
        LocationManager locationManager = mock(LocationManager.class);

        mShannonQualifiedNetworksService = new ShannonQualifiedNetworksService();
        mShannonQualifiedNetworksService.onCreate(context, vendorConfig, locationManager,
                mockSimManager);

        mShannonQualifiedNetworksService.onCreateNetworkAvailabilityProvider(0);
        mShannonQualifiedNetworksService.onCreateNetworkAvailabilityProvider(1);


        mShannonQualifiedNetworksService.dump(null,
                new android.util.IndentingPrintWriter(new FastPrintWriter(pw), ""), null);
    }

    @Test
    public void testProviderDump() throws FileNotFoundException {
        //Given
        FileOutputStream fout = new FileOutputStream(new File("./test.txt"));
        PrintWriter pw = new FastPrintWriter(fout);

        //Given
        Message mockMessage = mock(Message.class);
        mockMessage.what = SIM_STATE_CHANGED;
        SimInformation simInformation = mock(SimInformation.class);
        mockMessage.obj = simInformation;
        mockMessage.arg1 = 10;
        PackageVersion mockVersion = mock(PackageVersion.class);
        mShannonQualifiedNetworksService.setVersion(mockVersion);

        //mShannonNetworkAvailabilityProvider.setHandoverCondition(null);

        //When
        try (MockedConstruction<RilOemInterface> mockedConstruction =
                Mockito.mockConstruction(RilOemInterface.class);
                MockedConstruction<ConditionUserSetting> mockedCondUser =
                        Mockito.mockConstruction(ConditionUserSetting.class);
                MockedConstruction<ConditionCellular> mockedCellular =
                        Mockito.mockConstruction(ConditionCellular.class);
                MockedConstruction<ConditionWiFi> mockedWifi =
                        Mockito.mockConstruction(ConditionWiFi.class);
                MockedConstruction<ConditionProvisioning> mockedCondProvisioning =
                        Mockito.mockConstruction(ConditionProvisioning.class);
                MockedConstruction<ConditionCall> mockConditionCall =
                        Mockito.mockConstruction(ConditionCall.class);
                MockedConstruction<ConditionMobile> mockConditionMobile =
                        Mockito.mockConstruction(ConditionMobile.class);
                MockedConstruction<ConditionConnectivity> mockConditionConnectivity =
                        Mockito.mockConstruction(ConditionConnectivity.class)) {
            mShannonNetworkAvailabilityProvider.getHandler().handleMessage(mockMessage);
        }

        //When
        mShannonNetworkAvailabilityProvider.dump(null,
                new IndentingPrintWriter(new FastPrintWriter(pw), ""), null);
    }

    @Test
    public void testHandleAPN_NETWORK_LIST_UPDATEDMessage() throws Exception {
        //Given
        Message mockMessage = mock(Message.class);
        mockMessage.what = APN_NETWORK_LIST_UPDATED;
        PreferredList preferredList = mock(PreferredList.class);
        mockMessage.obj = preferredList;
        mockMessage.arg1 = 10;
        PackageVersion mockVersion = mock(PackageVersion.class);
        mShannonQualifiedNetworksService.setVersion(mockVersion);

        HandoverConditions mockHandoverConditions = mock(HandoverConditions.class);
        mShannonNetworkAvailabilityProvider.setHandoverCondition(mockHandoverConditions);

        //When
        mShannonNetworkAvailabilityProvider.getHandler().handleMessage(mockMessage);

        //Then
    }

    @Test
    public void testHandleAPN_NETWORK_LIST_UPDATEDMessageException() throws Exception {
        //Given
        Message mockMessage = mock(Message.class);
        mockMessage.what = APN_NETWORK_LIST_UPDATED;
        mockMessage.obj = null;
        mockMessage.arg1 = -1;
        PackageVersion mockVersion = mock(PackageVersion.class);
        mShannonQualifiedNetworksService.setVersion(mockVersion);

        //When
        mShannonNetworkAvailabilityProvider.getHandler().handleMessage(mockMessage);

        //Then
    }

    @Test
    public void testHandleSIM_STATE_CHANGEDMessage() throws Exception {
        //Given
        Message mockMessage = mock(Message.class);
        mockMessage.what = SIM_STATE_CHANGED;
        mockMessage.obj = null;
        mockMessage.arg1 = 10;
        PackageVersion mockVersion = mock(PackageVersion.class);
        mShannonQualifiedNetworksService.setVersion(mockVersion);

        HandoverConditions mockHandoverConditions = mock(HandoverConditions.class);
        mShannonNetworkAvailabilityProvider.setHandoverCondition(mockHandoverConditions);

        //When
        mShannonNetworkAvailabilityProvider.getHandler().handleMessage(mockMessage);

        //Then
        verify(mockHandoverConditions, times(1)).destroy();
    }

    @Test
    public void testHandleSIM_STATE_CHANGEDMessageHandoverCondNull() throws Exception {
        //Given
        Message mockMessage = mock(Message.class);
        mockMessage.what = SIM_STATE_CHANGED;
        SimInformation simInformation = mock(SimInformation.class);
        mockMessage.obj = simInformation;
        mockMessage.arg1 = 10;
        PackageVersion mockVersion = mock(PackageVersion.class);
        mShannonQualifiedNetworksService.setVersion(mockVersion);

        //When
        try (MockedConstruction<RilOemInterface> mockedConstruction =
            Mockito.mockConstruction(RilOemInterface.class);
            MockedConstruction<ConditionUserSetting> mockedCondUser =
                Mockito.mockConstruction(ConditionUserSetting.class);
            MockedConstruction<ConditionCellular> mockedCellular =
                Mockito.mockConstruction(ConditionCellular.class);
            MockedConstruction<ConditionWiFi> mockedWifi =
                Mockito.mockConstruction(ConditionWiFi.class);
            MockedConstruction<ConditionProvisioning> mockedCondProvisioning =
                Mockito.mockConstruction(ConditionProvisioning.class);
            MockedConstruction<ConditionCall> mockConditionCall =
                Mockito.mockConstruction(ConditionCall.class);
            MockedConstruction<ConditionMobile> mockConditionMobile =
                Mockito.mockConstruction(ConditionMobile.class);
            MockedConstruction<ConditionConnectivity> mockConditionConnectivity =
                Mockito.mockConstruction(ConditionConnectivity.class)) {
            mShannonNetworkAvailabilityProvider.getHandler().handleMessage(mockMessage);
        }

        //Then
        assertNotNull(mShannonNetworkAvailabilityProvider.getHandoverCondition());
    }

    @Test
    public void testHandleSIM_STATE_CHANGEDNullInfoMessage() throws Exception {
        //Given
        Message mockMessage = mock(Message.class);
        mockMessage.what = SIM_STATE_CHANGED;
        mockMessage.arg1 = 10;
        PackageVersion mockVersion = mock(PackageVersion.class);
        mShannonQualifiedNetworksService.setVersion(mockVersion);

        HandoverConditions mockHandoverConditions = mock(HandoverConditions.class);
        mShannonNetworkAvailabilityProvider.setHandoverCondition(mockHandoverConditions);

        //When
        mShannonNetworkAvailabilityProvider.getHandler().handleMessage(mockMessage);

        //Then
        verify(mockHandoverConditions, times(1)).destroy();
    }

    @Test
    public void testOnCreateNetworkAvailabilityProvider() throws Exception {
        //Given
        int slot  = 0;
        SubscriptionManager mockSubscriptionManager = mock(SubscriptionManager.class);


        sSubscriptionManager.when(() -> SubscriptionManager.isValidSlotIndex(anyInt())).thenReturn(true);

        //When
        NetworkAvailabilityProvider returnVal = mShannonQualifiedNetworksService.onCreateNetworkAvailabilityProvider(slot);

        //Then
        assertNotNull(returnVal);
    }

    @Test
    public void testOnCreateNetworkAvailabilityProviderNotValidSlot() throws Exception {
        //Given
        int slot = 0;
        sSubscriptionManager.when(() -> SubscriptionManager.isValidSlotIndex(anyInt())).thenReturn(false);

        //When
        NetworkAvailabilityProvider returnVal = mShannonQualifiedNetworksService.onCreateNetworkAvailabilityProvider(slot);

        //Then
        assertNull(returnVal);
    }

    @Test
    public void testGetService() throws Exception {
        //Given
        mLocalBinder = mShannonQualifiedNetworksService.new LocalBinder();

        //When
        ShannonQualifiedNetworksService revdObj = mLocalBinder.getService();

        //Then
        assertEquals(mShannonQualifiedNetworksService, revdObj);
    }

    @Test
    public void testHandleSIM_STATE_CHANGEDMessageHandoverCondNullException() throws Exception {
        //Given
        Message mockMessage = mock(Message.class);
        mockMessage.what = SIM_STATE_CHANGED;
        SimInformation simInformation = mock(SimInformation.class);
        mockMessage.obj = simInformation;
        mockMessage.arg1 = 10;

        when(simInformation.getSubId()).thenThrow(NullPointerException.class);

        try (MockedConstruction<RilOemInterface> mockedConstruction =
                Mockito.mockConstruction(RilOemInterface.class);
                MockedConstruction<ConditionUserSetting> mockedCondUser =
                        Mockito.mockConstruction(ConditionUserSetting.class);
                MockedConstruction<ConditionCellular> mockedCellular =
                        Mockito.mockConstruction(ConditionCellular.class);
                MockedConstruction<ConditionWiFi> mockedWifi =
                        Mockito.mockConstruction(ConditionWiFi.class);
                MockedConstruction<ConditionProvisioning> mockedCondProvisioning =
                        Mockito.mockConstruction(ConditionProvisioning.class);
                MockedConstruction<ConditionCall> mockConditionCall =
                        Mockito.mockConstruction(ConditionCall.class);
                MockedConstruction<ConditionMobile> mockConditionMobile =
                        Mockito.mockConstruction(ConditionMobile.class);
                MockedConstruction<ConditionConnectivity> mockConditionConnectivity =
                        Mockito.mockConstruction(ConditionConnectivity.class)) {
            mShannonNetworkAvailabilityProvider.getHandler().handleMessage(mockMessage);
        }

        //Then
        assertNull(mShannonNetworkAvailabilityProvider.getHandoverCondition());
    }
}
