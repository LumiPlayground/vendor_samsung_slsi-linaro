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

import android.content.Context;
import android.net.LinkProperties;
import android.net.Network;
import android.net.NetworkCapabilities;
import android.net.NetworkRequest;
import android.os.Handler;
import android.os.Message;
import android.telephony.DataFailCause;
import android.telephony.TelephonyManager;

import com.shannon.qualifiednetworksservice.ApnTransportHandler.DisconnectInfo;
import com.shannon.qualifiednetworksservice.Transport.TransportType;
import com.shannon.qualifiednetworksservice.util.DeviceUtils;

import junit.framework.TestCase;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.Mock;
import org.mockito.MockedStatic;
import org.mockito.Mockito;
import org.mockito.MockitoAnnotations;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.annotation.Config;

import java.util.HashMap;

import static androidx.test.core.app.ApplicationProvider.getApplicationContext;
import static com.shannon.qualifiednetworksservice.ApnTransportHandler.MSG_AIRPLANE_MODE_CHANGED;
import static com.shannon.qualifiednetworksservice.ApnTransportHandler.MSG_APN_CONNECTED;
import static com.shannon.qualifiednetworksservice.ApnTransportHandler.MSG_APN_CONNECTING;
import static com.shannon.qualifiednetworksservice.ApnTransportHandler.MSG_APN_DISCONNECTED;
import static com.shannon.qualifiednetworksservice.ApnTransportHandler.MSG_APN_UNKNOWN;
import static com.shannon.qualifiednetworksservice.ApnTransportHandler.MSG_APN_UNTHROTTLED;
import static com.shannon.qualifiednetworksservice.ApnTransportHandler.MSG_CARRIER_CONFIG_CHANGED;
import static com.shannon.qualifiednetworksservice.ApnTransportHandler.MSG_OPERATOR_CHANGED;
import static com.shannon.qualifiednetworksservice.ApnTransportHandler.MSG_RAT_CHANGED;
import static com.shannon.qualifiednetworksservice.ApnTransportHandler.MSG_REGISTRATION_COMPLETED;
import static com.shannon.qualifiednetworksservice.ApnTransportHandler.MSG_WFC_STATE_CHANGED;
import static com.shannon.qualifiednetworksservice.ApnTransportHandler.MSG_WIFI_ACCESS_POINT_CHANGED;
import static com.shannon.qualifiednetworksservice.ApnTransportHandler.MSG_WIFI_STATE_CHANGED;
import static com.shannon.qualifiednetworksservice.Transport.TransportType.TRANSPORT_CELLULAR;
import static com.shannon.qualifiednetworksservice.Transport.TransportType.TRANSPORT_WIFI;
import static org.mockito.ArgumentMatchers.any;
import static org.mockito.ArgumentMatchers.anyInt;
import static org.mockito.Mockito.doAnswer;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.mockStatic;
import static org.mockito.Mockito.spy;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class ApnTransportHandlerTest extends TestCase {
    @BeforeClass
    public static void beforeClass() {
        sSimManager =  mockStatic(SimManager.class);
        sDeviceUtils = mockStatic(DeviceUtils.class);
        sCarrierType = mockStatic(CarrierType.class);
    }

    @AfterClass
    public static void afterClass() {
        sSimManager.close();
        sDeviceUtils.close();
        sCarrierType.close();
    }

    private ApnTransportHandler mApnTransportHandler;
    private ApnTransportHandler.DisconnectInfo mDisconnectInfo;
    private ApnTransportHandler.NetworkCallback mNetworkCallback;
    private ApnTransportHandler.TransportStateChangedCallback mTransportStateChangedCallback;

    private static MockedStatic<SimManager> sSimManager;
    private static MockedStatic<DeviceUtils> sDeviceUtils;
    private static MockedStatic<CarrierType> sCarrierType;

    @Mock
    private ConfigurationManager mockConfigurationManager;

    @Mock
    ConditionCellular cellular;

    @Mock
    private HandoverConditions.IConditionChanged mockHandoverConditionChangedCallback;

    @Mock
    private HandoverConditions.IConditionHelper mockConditionHelper;

    int apnType = 64;  //TYPE_IMS
    int subId = 0;

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.openMocks(this);
        int slotId = 0;
        Context context = getApplicationContext();
        SimManager mockSimManager = mock(SimManager.class);

        sSimManager.when(() -> SimManager.getInstance()).thenReturn(mockSimManager);
        sDeviceUtils.when(() -> DeviceUtils.isConnectionCallbackFixed()).thenReturn(true);

        mApnTransportHandler = new ApnTransportHandler(apnType, slotId, context, mockConfigurationManager, mockConditionHelper, mockHandoverConditionChangedCallback);
        mNetworkCallback = mApnTransportHandler.new NetworkCallback(apnType, subId);
        mTransportStateChangedCallback = mApnTransportHandler.new TransportStateChangedCallback(TRANSPORT_WIFI);
    }

    @After
    public void tearDown() throws Exception {
        mApnTransportHandler.destroy();
    }

    @Test
    public void testDisconnectInfoWithoutTimeoutConstructor() {
        //Given
        int failCause = DataFailCause.NONE;
        int networkType = TelephonyManager.NETWORK_TYPE_IWLAN;
        int timeout = 0;

        //When
        mDisconnectInfo = new ApnTransportHandler.DisconnectInfo(networkType, failCause);

        //Then
        assertEquals(failCause, mDisconnectInfo.getCause());
        assertEquals(networkType, mDisconnectInfo.getNetworkType());
        assertEquals(timeout, mDisconnectInfo.getRetryTimeout());
    }

    @Test
    public void testDisconnectInfoWithTimeoutConstructor() {
        //Given
        int failCause = DataFailCause.NAS_SIGNALLING;
        int networkType = TelephonyManager.NETWORK_TYPE_NR;
        int timeout = 5;

        //When
        mDisconnectInfo = new ApnTransportHandler.DisconnectInfo(networkType, failCause, timeout);

        //Then
        assertEquals(failCause, mDisconnectInfo.getCause());
        assertEquals(networkType, mDisconnectInfo.getNetworkType());
        assertEquals(timeout, mDisconnectInfo.getRetryTimeout());
    }

    @Test
    public void testBuildRequest() {
        //Given
        NetworkRequest recvdNetworkRequest;

        //When
        recvdNetworkRequest = mNetworkCallback.buildRequest();

        //Then
        int[] transportType = recvdNetworkRequest.networkCapabilities.getTransportTypes();
        assertEquals(NetworkCapabilities.TRANSPORT_CELLULAR, transportType[0]);
    }

    @Test
    public void testOnLost() {
        //Given
        Network network = mock(Network.class);
        Transport mockTransport = mock(Transport.class);
        mApnTransportHandler.setCurrentTransport(mockTransport);
        when(mockTransport.getNetworkType()).thenReturn(TelephonyManager.NETWORK_TYPE_UNKNOWN);

        //When
        mNetworkCallback.onLost(network);

        //Then
        assertEquals(TelephonyManager.NETWORK_TYPE_UNKNOWN, mApnTransportHandler.getCurrentNetworkType());
    }

    @Test
    public void testOnLinkPropertiesChangedWifi() {
        //Given
        LinkProperties linkProperties =  mock(LinkProperties.class);
        when(linkProperties.getInterfaceName()).thenReturn("epdg0");
        when(mockConfigurationManager.getIwlanNetworkInterfaceName()).thenReturn("epdg");
        Message message = mock(Message.class);
        Network network = mock(Network.class);

        //When
        mNetworkCallback.onLinkPropertiesChanged(network, linkProperties);

        //Then
        assertEquals(0, message.arg1);
    }

    @Test
    public void testOnLinkPropertiesChangedCellular() {
        //Given
        LinkProperties linkProperties =  mock(LinkProperties.class);
        when(linkProperties.getInterfaceName()).thenReturn("rmnet0");
        when(mockConfigurationManager.getIwlanNetworkInterfaceName()).thenReturn("epdg");
        when(mockConfigurationManager.getCellularNetworkInterfaceName()).thenReturn("rmnet");
        Message message = mock(Message.class);
        Network network = mock(Network.class);

        //When
        mNetworkCallback.onLinkPropertiesChanged(network, linkProperties);

        //Then
        assertEquals(0, message.arg1);
    }

    @Test
    public void testOnUnblock() {
        //Given
        int expectedType = 64;
        doAnswer(invocation -> {
            int actualType = invocation.getArgument(0);
            assertEquals(expectedType, actualType);
            return null;
        }).when(mockHandoverConditionChangedCallback).onConditionChanged(anyInt(), any());

        //When
        mTransportStateChangedCallback.onUnblock();

        //Then
        verify(mockHandoverConditionChangedCallback, times(1)).onConditionChanged(anyInt(), any());
    }

    @Test
    public void testGetHandler() {
        //Given

        //When
        Handler handler = mApnTransportHandler.getHandler();

        //Then
        assertNotNull(handler);
    }

    @Test
    public void testIsTransportAvailable() {
        //Given
        Boolean expectedVal = true;
        TransportType transportType = TRANSPORT_WIFI;

        //When
        Boolean isTransportAvailable = mApnTransportHandler.isTransportAvailable(transportType);

        //Then
        assertEquals(expectedVal, isTransportAvailable);
    }

    @Test
    public void testIsTransportAvailable2() {
        //Given
        Boolean expectedVal = true;
        TransportType transportType = TRANSPORT_WIFI;

        Transport transport = mock(Transport.class);
        mApnTransportHandler.setCurrentTransport(transport);
        when(transport.getType()).thenReturn(TRANSPORT_CELLULAR);

        //When
        Boolean isTransportAvailable = mApnTransportHandler.isTransportAvailable(transportType);

        //Then
        assertEquals(expectedVal, isTransportAvailable);
    }

    @Test
    public void testIsTransportAvailableForInitial() {
        //Given
        Boolean expectedVal = true;
        TransportType transportType = TRANSPORT_WIFI;

        //When
        Boolean isTransportAvailable = mApnTransportHandler.isTransportAvailableForInitial(transportType);

        //Then
        assertEquals(expectedVal, isTransportAvailable);
    }

    @Test
    public void testIsHandoverFailed() {
        //Given
        Boolean expectedVal = false;
        TransportType transportType = TRANSPORT_WIFI;

        //When
        Boolean isTransportAvailable = mApnTransportHandler.isHandoverFailed(transportType);

        //Then
        assertEquals(expectedVal, isTransportAvailable);
    }

    public void testgetCurrentNetworkType() {
        //Given
        int expectedVal = 1;

        //When
        int currentNetwork = mApnTransportHandler.getCurrentNetworkType();

        //Then
        assertEquals(expectedVal, currentNetwork);
    }

    @Test
    public void testgetConnectingNetworkType() {
        //Given
        int expectedVal = 0;

        //When
        int connectingNetwork = mApnTransportHandler.getConnectingNetworkType();

        //Then
        assertEquals(expectedVal, connectingNetwork);
    }

    @Test
    public void testprocessMessage_MSG_APN_CONNECTING() {
        //Given
        Message message = mock(Message.class);
        message.what = MSG_APN_CONNECTING;
        message.arg1 = TelephonyManager.NETWORK_TYPE_IWLAN;
        Transport transport = mock(Transport.class);
        mApnTransportHandler.setCurrentTransport(transport);
        when(transport.getType()).thenReturn(TRANSPORT_CELLULAR);
        doAnswer(invocation -> {
            int recvType = invocation.getArgument(0);
            assertEquals(apnType, recvType);
            return null;
        }).when(mockHandoverConditionChangedCallback).onConditionChanged(anyInt(), any(), any());

        //When
        mApnTransportHandler.getHandler().handleMessage(message);

        //Then
        verify(mockHandoverConditionChangedCallback, times(1)).onConditionChanged(anyInt(), any(), any());
    }

    @Test
    public void testprocessMessage_MSG_APN_CONNECTED() {
        //Given
        Message message = mock(Message.class);
        message.what = MSG_APN_CONNECTED;
        message.arg1 = 2; //TRANSPORT_WIFI
        Transport transport = mock(Transport.class);
        mApnTransportHandler.setCurrentTransport(transport);
        when(transport.getType()).thenReturn(TRANSPORT_WIFI);
        doAnswer(invocation -> {
            int recvType = invocation.getArgument(0);
            assertEquals(apnType, recvType);
            return null;
        }).when(mockHandoverConditionChangedCallback).onConditionChanged(anyInt(), any(), any());

        //When
        mApnTransportHandler.getHandler().handleMessage(message);

        //Then
        verify(mockHandoverConditionChangedCallback, times(1)).onConditionChanged(anyInt(), any(), any());
    }

    @Test
    public void testprocessMessage_MSG_APN_CONNECTED_2() {
        //Given
        Message message = mock(Message.class);
        message.what = MSG_APN_CONNECTED;
        message.arg1 = 2; //TRANSPORT_WIFI
        Transport transport = mock(Transport.class);
        mApnTransportHandler.setCurrentTransport(transport);
        when(transport.getType()).thenReturn(TRANSPORT_CELLULAR);
        doAnswer(invocation -> {
            int recvType = invocation.getArgument(0);
            assertEquals(apnType, recvType);
            return null;
        }).when(mockHandoverConditionChangedCallback).onConditionChanged(anyInt(), any(), any());

        //When
        mApnTransportHandler.getHandler().handleMessage(message);

        //Then
        verify(mockHandoverConditionChangedCallback, times(1)).onConditionChanged(anyInt(), any(), any());
    }

    @Test
    public void testprocessMessage_MSG_APN_DISCONNECTED() {
        //Given
        Message message = mock(Message.class);
        message.what = MSG_APN_DISCONNECTED;
        DisconnectInfo info = new DisconnectInfo(TelephonyManager.NETWORK_TYPE_IWLAN, DataFailCause.OPERATOR_BARRED);
        message.obj = info;
        Transport transport = mock(Transport.class);
        mApnTransportHandler.setCurrentTransport(transport);
        when(transport.getType()).thenReturn(TRANSPORT_WIFI);
        doAnswer(invocation -> {
            int recvType = invocation.getArgument(0);
            assertEquals(apnType, recvType);
            return null;
        }).when(mockHandoverConditionChangedCallback).onConditionChanged(anyInt(), any(), any());

        //When
        mApnTransportHandler.getHandler().handleMessage(message);

        //Then
        verify(mockHandoverConditionChangedCallback, times(1)).onConditionChanged(anyInt(), any(), any());
    }

    @Test
    public void testprocessMessage_MSG_APN_UNKNOWN() {
        //Given
        Message message = mock(Message.class);
        message.what = MSG_APN_UNKNOWN;
        DisconnectInfo info = new DisconnectInfo(TelephonyManager.NETWORK_TYPE_IWLAN, DataFailCause.OPERATOR_BARRED);
        message.obj = info;
        Transport transport = mock(Transport.class);
        mApnTransportHandler.setCurrentTransport(transport);
        when(transport.getType()).thenReturn(TRANSPORT_WIFI);
        doAnswer(invocation -> {
            int recvType = invocation.getArgument(0);
            assertEquals(apnType, recvType);
            return null;
        }).when(mockHandoverConditionChangedCallback).onConditionChanged(anyInt(), any(), any());

        //When
        mApnTransportHandler.getHandler().handleMessage(message);

        //Then
        verify(mockHandoverConditionChangedCallback, times(1)).onConditionChanged(anyInt(), any(), any());
    }

    @Test
    public void testprocessMessage_MSG_APN_UNKNOWNSendHandoverFailure() {
        //Given
        Message message = mock(Message.class);
        message.what = MSG_APN_UNKNOWN;
        DisconnectInfo info = new DisconnectInfo(TelephonyManager.NETWORK_TYPE_IWLAN, DataFailCause.OPERATOR_BARRED);
        message.obj = info;
        Transport transport = mock(Transport.class);
        mApnTransportHandler.setCurrentTransport(transport);
        when(transport.getType()).thenReturn(TRANSPORT_CELLULAR);
        doAnswer(invocation -> {
            int recvType = invocation.getArgument(0);
            assertEquals(apnType, recvType);
            return null;
        }).when(mockHandoverConditionChangedCallback).onConditionChanged(anyInt(), any(), any());

        //When
        mApnTransportHandler.getHandler().handleMessage(message);

        //Then
        verify(mockHandoverConditionChangedCallback, times(1)).onConditionChanged(anyInt(), any(), any());
    }

    @Test
    public void testprocessMessage_MSG_AIRPLANE_MODE_CHANGED() {
        //Given
        Message message = mock(Message.class);
        message.what = MSG_AIRPLANE_MODE_CHANGED;
        HashMap<TransportType, Transport> transportList =  mApnTransportHandler.getTransportsList();
        spy(transportList);

        //When
        mApnTransportHandler.getHandler().handleMessage(message);

        //Then
        assertEquals(2, transportList.size());
    }

    @Test
    public void testprocessMessage_MSG_OPERATOR_CHANGED() {
        //Given
        Message message = mock(Message.class);
        message.what = MSG_OPERATOR_CHANGED;
        Transport transport = mock(Transport.class);
        mApnTransportHandler.setCurrentTransport(transport);
        doAnswer(invocation -> {
            int recvType = invocation.getArgument(0);
            assertEquals(apnType, recvType);
            return null;
        }).when(mockHandoverConditionChangedCallback).onConditionChanged(anyInt(), any());

        //When
        mApnTransportHandler.getHandler().handleMessage(message);

        //Then
        verify(mockHandoverConditionChangedCallback, times(1)).onConditionChanged(anyInt(), any());
    }

    @Test
    public void testprocessMessage_MSG_RAT_CHANGED() {
        //Given
        Message message = mock(Message.class);
        message.what = MSG_RAT_CHANGED;
        Transport transport = mock(Transport.class);
        mApnTransportHandler.setCurrentTransport(transport);
        doAnswer(invocation -> {
            int recvType = invocation.getArgument(0);
            assertEquals(apnType, recvType);
            return null;
        }).when(mockHandoverConditionChangedCallback).onConditionChanged(anyInt(), any());

        //When
        mApnTransportHandler.getHandler().handleMessage(message);

        //Then
        verify(mockHandoverConditionChangedCallback, times(1)).onConditionChanged(anyInt(), any());
    }

    @Test
    public void testprocessMessage_MSG_WIFI_STATE_CHANGED() {
        //Given
        Message message = mock(Message.class);
        message.what = MSG_WIFI_STATE_CHANGED;
        HashMap<TransportType, Transport> transportList =  mApnTransportHandler.getTransportsList();
        spy(transportList);

        //When
        mApnTransportHandler.getHandler().handleMessage(message);

        //Then
        assertEquals(2, transportList.size());
    }

    @Test
    public void testprocessMessage_MSG_WIFI_ACCESS_POINT_CHANGED() {
        //Given
        Message message = mock(Message.class);
        message.what = MSG_WIFI_ACCESS_POINT_CHANGED;
        Transport transport = mock(Transport.class);
        mApnTransportHandler.setCurrentTransport(transport);
        doAnswer(invocation -> {
            int recvType = invocation.getArgument(0);
            assertEquals(apnType, recvType);
            return null;
        }).when(mockHandoverConditionChangedCallback).onConditionChanged(anyInt(), any());

        //When
        mApnTransportHandler.getHandler().handleMessage(message);

        //Then
        verify(mockHandoverConditionChangedCallback, times(1)).onConditionChanged(anyInt(), any());
    }

    @Test
    public void testprocessMessage_MSG_WFC_STATE_CHANGED() {
        //Given
        Message message = mock(Message.class);
        message.what = MSG_WFC_STATE_CHANGED;
        HashMap<TransportType, Transport> transportList =  mApnTransportHandler.getTransportsList();
        spy(transportList);

        //When
        mApnTransportHandler.getHandler().handleMessage(message);

        //Then
        assertEquals(2, transportList.size());
    }

    @Test
    public void testprocessMessage_MSG_CARRIER_CONFIG_CHANGED() {
        //Given
        Message message = mock(Message.class);
        message.what = MSG_CARRIER_CONFIG_CHANGED;
        HashMap<TransportType, Transport> transportList =  mApnTransportHandler.getTransportsList();
        spy(transportList);

        //When
        mApnTransportHandler.getHandler().handleMessage(message);

        //Then
        assertEquals(2, transportList.size());
    }

    @Test
    public void testprocessMessage_MSG_APN_UNTHROTTLED() {
        //Given
        Message message = mock(Message.class);
        message.what = MSG_APN_UNTHROTTLED;
        Transport transport = mock(Transport.class);
        mApnTransportHandler.setCurrentTransport(transport);
        doAnswer(invocation -> {
            int recvType = invocation.getArgument(0);
            assertEquals(apnType, recvType);
            return null;
        }).when(mockHandoverConditionChangedCallback).onConditionChanged(anyInt(), any());

        //When
        mApnTransportHandler.getHandler().handleMessage(message);

        //Then
        verify(mockHandoverConditionChangedCallback, times(1))
                .onConditionChanged(anyInt(), any(), any());
    }

    @Test
    public void testprocessMessage_MSG_REGISTRATION_COMPLETED() {
        //Given
        Message message = mock(Message.class);
        message.what = MSG_REGISTRATION_COMPLETED;
        Transport transport = mock(Transport.class);
        mApnTransportHandler.setCurrentTransport(transport);
        doAnswer(invocation -> {
            int recvType = invocation.getArgument(0);
            assertEquals(apnType, recvType);
            return null;
        }).when(mockHandoverConditionChangedCallback).onConditionChanged(anyInt(), any());

        //When
        mApnTransportHandler.getHandler().handleMessage(message);

        //Then
        verify(mockHandoverConditionChangedCallback, times(1)).onConditionChanged(anyInt(), any());
    }

    @Test
    public void testSetCurrentTransport() {
        //Given
        Transport transport = mock(Transport.class);

        //When
        mApnTransportHandler.setCurrentTransport(transport);

        //Then
        assertEquals(transport, mApnTransportHandler.getCurrentTransport());
    }

    @Test
    public void testGetCurrentNetworkType() {
        //Given
        Transport mockTransport = mock(Transport.class);
        when(mockTransport.getNetworkType()).thenReturn(TelephonyManager.NETWORK_TYPE_UNKNOWN);
        mApnTransportHandler.setCurrentTransport(mockTransport);

        //When
        int retNetworkType = mApnTransportHandler.getCurrentNetworkType();

        //Then
        assertEquals(TelephonyManager.NETWORK_TYPE_UNKNOWN, retNetworkType);
    }

    @Test
    public void testGetVendorTransportHandler() {
        //Given
        int slotId = 0;
        SimManager mockSimManager = mock(SimManager.class);
        TelephonyManager mockTelephonyManager = mock(TelephonyManager.class);

        sSimManager.when(() -> SimManager.getInstance()).thenReturn(mockSimManager);
        when(mockSimManager.getTelephonyManager(slotId)).thenReturn(mockTelephonyManager);
        Transport.IVendorTransportHandler handler;

        /** Orange **/
        //When
        sCarrierType.when(() -> CarrierType.getEnum(mockTelephonyManager)).thenReturn(CarrierType.CARRIER_TYPE_ORANGE);
        handler = mApnTransportHandler.getVendorTransportHandler(mockSimManager);
        //Then
        assertTrue(handler instanceof TransportHandlerOrange);

        /** Swisscom **/
        //When
        sCarrierType.when(() -> CarrierType.getEnum(mockTelephonyManager)).thenReturn(CarrierType.CARRIER_TYPE_SWISSCOM);
        handler = mApnTransportHandler.getVendorTransportHandler(mockSimManager);
        //Then
        assertTrue(handler instanceof TransportHandlerSwisscom);

        /** Verizon **/
        //When
        sCarrierType.when(() -> CarrierType.getEnum(mockTelephonyManager)).thenReturn(CarrierType.CARRIER_TYPE_VZW);
        handler = mApnTransportHandler.getVendorTransportHandler(mockSimManager);
        //Then
        assertTrue(handler instanceof TransportHandlerVzw);

        /** Rogers **/
        //When
        sCarrierType.when(() -> CarrierType.getEnum(mockTelephonyManager)).thenReturn(CarrierType.CARRIER_TYPE_ROGERS);
        handler = mApnTransportHandler.getVendorTransportHandler(mockSimManager);
        //Then
        assertTrue(handler instanceof TransportHandlerRogers);

        /** EE **/
        //When
        sCarrierType.when(() -> CarrierType.getEnum(mockTelephonyManager)).thenReturn(CarrierType.CARRIER_TYPE_EE);
        handler = mApnTransportHandler.getVendorTransportHandler(mockSimManager);
        //Then
        assertTrue(handler instanceof TransportHandlerEE);

        /** Telstra **/
        //When
        sCarrierType.when(() -> CarrierType.getEnum(mockTelephonyManager)).thenReturn(CarrierType.CARRIER_TYPE_TELSTRA);
        handler = mApnTransportHandler.getVendorTransportHandler(mockSimManager);
        //Then
        assertTrue(handler instanceof TransportHandlerTelstra);

        /** TMO **/
        //When
        sCarrierType.when(() -> CarrierType.getEnum(mockTelephonyManager)).thenReturn(CarrierType.CARRIER_TYPE_TMO);
        handler = mApnTransportHandler.getVendorTransportHandler(mockSimManager);
        //Then
        assertTrue(handler instanceof TransportHandlerTmo);

    }
}

