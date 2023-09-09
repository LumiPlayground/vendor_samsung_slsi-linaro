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
import android.hardware.radio.V1_5.AccessNetwork;
import android.net.LinkProperties;
import android.net.Network;
import android.net.NetworkCapabilities;
import android.net.NetworkRequest;
import android.os.Handler;
import android.os.Message;
import android.telephony.DataFailCause;
import android.telephony.TelephonyManager;

import android.telephony.data.ApnSetting;
import com.shannon.qualifiednetworksservice.ApnTransportHandler.DisconnectInfo;
import com.shannon.qualifiednetworksservice.HandoverConditions.ConditionEvents;
import com.shannon.qualifiednetworksservice.HandoverDecision.DecisionTimerTask;
import com.shannon.qualifiednetworksservice.Transport.TransportType;
import com.shannon.qualifiednetworksservice.TransportHandlerSwisscom.State;
import com.shannon.qualifiednetworksservice.util.DeviceUtils;
import com.shannon.qualifiednetworksservice.util.LogUtils;
import java.util.HashMap;
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
import org.mockito.Mockito;
import org.mockito.MockitoAnnotations;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.annotation.Config;

import static android.hardware.radio.Announcement.TYPE_EMERGENCY;
import static android.telephony.DataFailCause.NAS_SIGNALLING;
import static android.telephony.ServiceState.RIL_RADIO_TECHNOLOGY_IWLAN;
import static android.telephony.ServiceState.RIL_RADIO_TECHNOLOGY_LTE;
import static android.telephony.ServiceState.RIL_RADIO_TECHNOLOGY_UMTS;
import static android.telephony.TelephonyManager.NETWORK_TYPE_IWLAN;
import static android.telephony.TelephonyManager.NETWORK_TYPE_LTE;
import static android.telephony.TelephonyManager.NETWORK_TYPE_LTE_CA;
import static android.telephony.TelephonyManager.NETWORK_TYPE_UMTS;
import static android.telephony.TelephonyManager.NETWORK_TYPE_UNKNOWN;
import static androidx.test.core.app.ApplicationProvider.getApplicationContext;
import static com.shannon.qualifiednetworksservice.ApnTransportHandler.MSG_AIRPLANE_MODE_CHANGED;
import static com.shannon.qualifiednetworksservice.ApnTransportHandler.MSG_APN_CONNECTED;
import static com.shannon.qualifiednetworksservice.ApnTransportHandler.MSG_APN_CONNECTING;
import static com.shannon.qualifiednetworksservice.ApnTransportHandler.MSG_APN_DISCONNECTED;
import static com.shannon.qualifiednetworksservice.ApnTransportHandler.MSG_APN_UNKNOWN;
import static com.shannon.qualifiednetworksservice.ApnTransportHandler.MSG_CARRIER_CONFIG_CHANGED;
import static com.shannon.qualifiednetworksservice.ApnTransportHandler.MSG_OPERATOR_CHANGED;
import static com.shannon.qualifiednetworksservice.ApnTransportHandler.MSG_RAT_CHANGED;
import static com.shannon.qualifiednetworksservice.ApnTransportHandler.MSG_REGISTRATION_COMPLETED;
import static com.shannon.qualifiednetworksservice.ApnTransportHandler.MSG_WFC_STATE_CHANGED;
import static com.shannon.qualifiednetworksservice.ApnTransportHandler.MSG_WIFI_ACCESS_POINT_CHANGED;
import static com.shannon.qualifiednetworksservice.ApnTransportHandler.MSG_WIFI_STATE_CHANGED;
import static com.shannon.qualifiednetworksservice.HandoverConditions.ConditionEvents.EVENT_APN_CONNECTED;
import static com.shannon.qualifiednetworksservice.HandoverConditions.ConditionEvents.EVENT_APN_DISCONNECTED;
import static com.shannon.qualifiednetworksservice.HandoverConditions.ConditionEvents.EVENT_APN_FAILURE;
import static com.shannon.qualifiednetworksservice.HandoverConditions.ConditionEvents.EVENT_ATTACH_TIMER_EXPIRED;
import static com.shannon.qualifiednetworksservice.HandoverConditions.ConditionEvents.EVENT_CALL_STARTED;
import static com.shannon.qualifiednetworksservice.HandoverConditions.ConditionEvents.EVENT_HYSTERESIS_TIMER_EXPIRED;
import static com.shannon.qualifiednetworksservice.Transport.TransportType.TRANSPORT_CELLULAR;
import static com.shannon.qualifiednetworksservice.Transport.TransportType.TRANSPORT_WIFI;
import static com.shannon.qualifiednetworksservice.TransportHandlerSwisscom.State.STATE_RETRY_1;
import static com.shannon.qualifiednetworksservice.TransportHandlerSwisscom.State.STATE_RETRY_2;
import static com.shannon.qualifiednetworksservice.TransportHandlerSwisscom.State.STATE_RETRY_3;
import static com.shannon.qualifiednetworksservice.TransportHandlerSwisscom.State.STATE_RETRY_4;
import static com.shannon.qualifiednetworksservice.TransportHandlerSwisscom.State.STATE_RETRY_5;
import static org.mockito.ArgumentMatchers.any;
import static org.mockito.ArgumentMatchers.anyBoolean;
import static org.mockito.ArgumentMatchers.anyInt;
import static org.mockito.ArgumentMatchers.eq;
import static org.mockito.Mockito.doAnswer;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.mockStatic;
import static org.mockito.Mockito.spy;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class TransportHandlerSwisscomTest extends TestCase {

    @BeforeClass
    public static void beforeClass() {
    }

    @AfterClass
    public static void afterClass() {
    }

    private TransportHandlerSwisscom mTransHdlrSwisscom;

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

        mTransHdlrSwisscom = new TransportHandlerSwisscom();
    }

    @After
    public void tearDown() throws Exception {
        //mStaticMockSession.finishMocking();
    }

    @Test
    public void testOnConnected() {
        //Given
        Transport transport = mock(Transport.class);

        //When
        mTransHdlrSwisscom.onConnected(transport);

        //Then
        assertEquals(State.STATE_NONE, mTransHdlrSwisscom.getState());
    }

    @Test
    public void testOnHandoverCompleted() {
        //Given
        Transport transport = mock(Transport.class);

        //When
        mTransHdlrSwisscom.onHandoverCompleted(transport);

        //Then
        assertEquals(State.STATE_NONE, mTransHdlrSwisscom.getHandoverState());
    }

    @Test
    public void testOnAirplaneModeChanged() {
        //Given
        Transport transport = mock(Transport.class);

        //When
        boolean isAirplaneModeChanged = mTransHdlrSwisscom.onAirplaneModeChanged(transport);

        //Then
        assertEquals(State.STATE_NONE, mTransHdlrSwisscom.getState());
        assertEquals(State.STATE_NONE, mTransHdlrSwisscom.getHandoverState());
        assertEquals(false, isAirplaneModeChanged);
    }

    /*@Test
    public void testProcessState() {
        //Given
        TransportState transportState = mock(TransportState.class);
        State nextState = STATE_RETRY_1;
        int interStateTimeout = 10;
        int expectedRetry = 2;
        when(transportState.getRetryCount()).thenReturn(2);

        //When
        int retryTimeout = mTransHdlrSwisscom.processState(transportState, nextState, interStateTimeout);

        //Then
        verify(transportState, times(1)).blockTemporary(anyInt());
    }*/

    /*@Test
    public void testProcessStateMoreRetries() {
        //Given
        TransportState transportState = mock(TransportState.class);
        State nextState = STATE_RETRY_1;
        int interStateTimeout = 10;
        int expectedRetry = 2;
        when(transportState.getRetryCount()).thenReturn(4);

        //When
        int retryTimeout = mTransHdlrSwisscom.processState(transportState, nextState, interStateTimeout);

        //Then
        verify(transportState, times(1)).onDisconnected();
        verify(transportState, times(1)).blockTemporary(anyInt());
    }*/

   /* @Test
    public void testProcessHandoverState() {
        //Given
        TransportState transportState = mock(TransportState.class);
        State nextState = STATE_RETRY_1;
        int interStateTimeout = 10;
        int expectedRetry = 2;
        when(transportState.getRetryCount()).thenReturn(2);

        //When
        int retryTimeout = mTransHdlrSwisscom.processHandoverState(transportState, nextState, interStateTimeout);

        //Then
        verify(transportState, times(1)).blockTemporary(anyInt());
    }*/

    /*@Test
    public void testProcessHandoverStateMoreRetries() {
        //Given
        TransportState transportState = mock(TransportState.class);
        State nextState = STATE_RETRY_1;
        int interStateTimeout = 10;
        int expectedRetry = 2;
        when(transportState.getRetryCount()).thenReturn(4);

        //When
        int retryTimeout = mTransHdlrSwisscom.processHandoverState(transportState, nextState, interStateTimeout);

        //Then
        verify(transportState, times(1)).onDisconnected();
        verify(transportState, times(1)).blockTemporary(anyInt());
    }*/

    @Test
    public void testOnConnectionFailure() {
        //Given
        Transport transport = mock(Transport.class);
        TransportState transportState = mock(TransportState.class);
        int cause = DataFailCause.NO_RESPONSE_FROM_BASE_STATION;
        int timeout = 0;
        when(transport.getInitialState()).thenReturn(transportState);
        when(transportState.getRetryCount()).thenReturn(3);
        when(transport.getType()).thenReturn(TransportType.TRANSPORT_WIFI);

        //When
        mTransHdlrSwisscom.onConnectionFailure(transport, cause, timeout);

        //Then
        assertEquals(State.STATE_RETRY_2, mTransHdlrSwisscom.getState());
    }

    @Test
    public void testOnConnectionFailureStateRetry2() {
        //Given
        Transport transport = mock(Transport.class);
        TransportState transportState = mock(TransportState.class);
        int cause = DataFailCause.NO_RESPONSE_FROM_BASE_STATION;
        int timeout = 0;
        mTransHdlrSwisscom.setState(STATE_RETRY_2);
        boolean expVal = true;
        when(transport.getType()).thenReturn(TransportType.TRANSPORT_WIFI);
        when(transport.getInitialState()).thenReturn(transportState);
        when(transportState.getRetryCount()).thenReturn(3);
        when(transport.getInitialState()).thenReturn(transportState);

        //When
        boolean returnVal = mTransHdlrSwisscom.onConnectionFailure(transport, cause, timeout);

        //Then
        assertEquals(expVal, returnVal);
    }

    @Test
    public void testOnConnectionFailureStateRetry3() {
        //Given
        Transport transport = mock(Transport.class);
        TransportState transportState = mock(TransportState.class);
        int cause = DataFailCause.NO_RESPONSE_FROM_BASE_STATION;
        int timeout = 0;
        mTransHdlrSwisscom.setState(STATE_RETRY_3);
        boolean expVal = true;
        when(transport.getType()).thenReturn(TransportType.TRANSPORT_WIFI);
        when(transport.getInitialState()).thenReturn(transportState);
        when(transportState.getRetryCount()).thenReturn(3);
        when(transport.getInitialState()).thenReturn(transportState);

        //When
        boolean returnVal = mTransHdlrSwisscom.onConnectionFailure(transport, cause, timeout);

        //Then
        assertEquals(expVal, returnVal);
    }

    @Test
    public void testOnConnectionFailureStateRetry4() {
        //Given
        Transport transport = mock(Transport.class);
        TransportState transportState = mock(TransportState.class);
        int cause = DataFailCause.NO_RESPONSE_FROM_BASE_STATION;
        int timeout = 0;
        mTransHdlrSwisscom.setState(STATE_RETRY_4);
        boolean expVal = true;
        when(transport.getType()).thenReturn(TransportType.TRANSPORT_WIFI);
        when(transport.getInitialState()).thenReturn(transportState);
        when(transportState.getRetryCount()).thenReturn(3);

        //When
        boolean returnVal = mTransHdlrSwisscom.onConnectionFailure(transport, cause, timeout);

        //Then
        assertEquals(expVal, returnVal);
    }

    @Test
    public void testOnConnectionFailureStateRetry4_whenRetryCountIsLowerThanRETRY_IN_STATE() {
        //Given
        Transport transport = mock(Transport.class);
        TransportState transportState = mock(TransportState.class);
        int cause = DataFailCause.NO_RESPONSE_FROM_BASE_STATION;
        int timeout = 0;
        mTransHdlrSwisscom.setState(STATE_RETRY_4);
        boolean expVal = true;
        when(transport.getType()).thenReturn(TransportType.TRANSPORT_WIFI);
        when(transport.getInitialState()).thenReturn(transportState);
        when(transportState.getRetryCount()).thenReturn(2); // RETRY_IN_STATE - 1

        //When
        boolean returnVal = mTransHdlrSwisscom.onConnectionFailure(transport, cause, timeout);

        //Then
        assertEquals(expVal, returnVal);
    }

    @Test
    public void testOnConnectionFailureStateRetry5() {
        //Given
        Transport transport = mock(Transport.class);
        TransportState transportState = mock(TransportState.class);
        int cause = DataFailCause.NO_RESPONSE_FROM_BASE_STATION;
        int timeout = 0;
        mTransHdlrSwisscom.setState(STATE_RETRY_5);
        boolean expVal = true;
        when(transport.getInitialState()).thenReturn(transportState);
        when(transportState.getRetryCount()).thenReturn(3);
        when(transport.getType()).thenReturn(TransportType.TRANSPORT_WIFI);

        //When
        boolean returnVal = mTransHdlrSwisscom.onConnectionFailure(transport, cause, timeout);

        //Then
        assertEquals(expVal, returnVal);
    }

    @Test
    public void testOnConnectionFailureStateRetry5MoreRetires() {
        //Given
        Transport transport = mock(Transport.class);
        TransportState transportState = mock(TransportState.class);
        int cause = DataFailCause.NO_RESPONSE_FROM_BASE_STATION;
        int timeout = 0;
        mTransHdlrSwisscom.setState(STATE_RETRY_5);
        boolean expVal = true;
        when(transport.getInitialState()).thenReturn(transportState);
        when(transportState.getRetryCount()).thenReturn(5);
        when(transport.getType()).thenReturn(TransportType.TRANSPORT_WIFI);

        //When
        boolean returnVal = mTransHdlrSwisscom.onConnectionFailure(transport, cause, timeout);

        //Then
        assertEquals(expVal, returnVal);
    }

    @Test
    public void testOnConnectionFailureStateRetry5_whenRetryCountIsLowerThanRETRY_IN_STATE() {
        //Given
        Transport transport = mock(Transport.class);
        TransportState transportState = mock(TransportState.class);
        int cause = DataFailCause.NO_RESPONSE_FROM_BASE_STATION;
        int timeout = 0;
        mTransHdlrSwisscom.setState(STATE_RETRY_5);
        boolean expVal = true;
        when(transport.getInitialState()).thenReturn(transportState);
        when(transportState.getRetryCount()).thenReturn(2); // RETRY_IN_STATE - 1
        when(transport.getType()).thenReturn(TransportType.TRANSPORT_WIFI);

        //When
        boolean returnVal = mTransHdlrSwisscom.onConnectionFailure(transport, cause, timeout);

        //Then
        assertEquals(expVal, returnVal);
    }

    @Test
    public void testOnConnectionFailureStateRetry1() {
        //Given
        Transport transport = mock(Transport.class);
        TransportState transportState = mock(TransportState.class);
        int cause = DataFailCause.NO_RESPONSE_FROM_BASE_STATION;
        int timeout = 0;
        mTransHdlrSwisscom.setState(STATE_RETRY_1);
        boolean expVal = true;
        when(transport.getType()).thenReturn(TransportType.TRANSPORT_WIFI);
        when(transport.getInitialState()).thenReturn(transportState);
        when(transportState.getRetryCount()).thenReturn(3);

        //When
        boolean returnVal = mTransHdlrSwisscom.onConnectionFailure(transport, cause, timeout);

        //Then
        assertEquals(expVal, returnVal);
    }

    @Test
    public void testOnConnectionFailureRetryCountLessThanOne() {
        //Given
        Transport transport = mock(Transport.class);
        TransportState transportState = mock(TransportState.class);
        int cause = DataFailCause.REJECTED_BY_BASE_STATION;
        int timeout = 0;
        mTransHdlrSwisscom.setState(STATE_RETRY_1);
        boolean expVal = true;
        when(transport.getInitialState()).thenReturn(transportState);
        when(transportState.getRetryCount()).thenReturn(0);
        when(transport.getType()).thenReturn(TransportType.TRANSPORT_WIFI);

        //When
        boolean returnVal = mTransHdlrSwisscom.onConnectionFailure(transport, cause, timeout);

        //Then
        verify(transport.getInitialState(), times(1)).blockTemporary(anyInt());
    }

    @Test
    public void testOnConnectionFailureRetryCountMoreThanTwelve() {
        //Given
        Transport transport = mock(Transport.class);
        TransportState transportState = mock(TransportState.class);
        int cause = DataFailCause.NO_CDMA_SERVICE;
        int timeout = 0;
        mTransHdlrSwisscom.setState(STATE_RETRY_1);
        boolean expVal = true;
        when(transport.getInitialState()).thenReturn(transportState);
        when(transportState.getRetryCount()).thenReturn(14);
        when(transport.getType()).thenReturn(TransportType.TRANSPORT_WIFI);

        //When
        boolean returnVal = mTransHdlrSwisscom.onConnectionFailure(transport, cause, timeout);

        //Then
        verify(transport.getInitialState(), times(1)).blockTemporary(anyInt());
    }

    @Test
    public void testOnHandoverFailure() {
        //Given
        Transport transport = mock(Transport.class);
        TransportState transportState = mock(TransportState.class);
        int cause = DataFailCause.NO_RESPONSE_FROM_BASE_STATION;
        int timeout = 0;
        when(transport.getHandoverState()).thenReturn(transportState);
        when(transportState.getRetryCount()).thenReturn(3);
        when(transport.getType()).thenReturn(TransportType.TRANSPORT_WIFI);

        //When
        mTransHdlrSwisscom.onHandoverFailure(transport, cause, timeout);

        //Then
        assertEquals(State.STATE_NONE, mTransHdlrSwisscom.getState());
    }

    @Test
    public void testOnHandoverFailureStateRetry2() {
        //Given
        Transport transport = mock(Transport.class);
        TransportState transportState = mock(TransportState.class);
        int cause = DataFailCause.NO_RESPONSE_FROM_BASE_STATION;
        int timeout = 0;
        mTransHdlrSwisscom.setHandoverState(STATE_RETRY_2);
        boolean expVal = true;
        when(transport.getHandoverState()).thenReturn(transportState);
        when(transportState.getRetryCount()).thenReturn(3);
        when(transport.getType()).thenReturn(TransportType.TRANSPORT_WIFI);

        //When
        boolean returnVal = mTransHdlrSwisscom.onHandoverFailure(transport, cause, timeout);

        //Then
        assertEquals(expVal, returnVal);
    }

    @Test
    public void testOnHandoverFailureStateRetry3() {
        //Given
        Transport transport = mock(Transport.class);
        TransportState transportState = mock(TransportState.class);
        int cause = DataFailCause.NO_RESPONSE_FROM_BASE_STATION;
        int timeout = 0;
        mTransHdlrSwisscom.setHandoverState(STATE_RETRY_3);
        boolean expVal = true;
        when(transportState.getRetryCount()).thenReturn(3);
        when(transport.getHandoverState()).thenReturn(transportState);
        when(transport.getType()).thenReturn(TransportType.TRANSPORT_WIFI);

        //When
        boolean returnVal = mTransHdlrSwisscom.onHandoverFailure(transport, cause, timeout);

        //Then
        assertEquals(expVal, returnVal);
    }

    @Test
    public void testOnHandoverFailureStateRetry4() {
        //Given
        Transport transport = mock(Transport.class);
        TransportState transportState = mock(TransportState.class);
        int cause = DataFailCause.NO_RESPONSE_FROM_BASE_STATION;
        int timeout = 0;
        mTransHdlrSwisscom.setHandoverState(STATE_RETRY_4);
        boolean expVal = true;
        when(transport.getHandoverState()).thenReturn(transportState);
        when(transportState.getRetryCount()).thenReturn(3);
        when(transport.getType()).thenReturn(TransportType.TRANSPORT_WIFI);

        //When
        boolean returnVal = mTransHdlrSwisscom.onHandoverFailure(transport, cause, timeout);

        //Then
        assertEquals(expVal, returnVal);
    }

    @Test
    public void testOnHandoverFailureStateRetry4_whenRetryCountIsLowerThanRETRY_IN_STATE() {
        //Given
        Transport transport = mock(Transport.class);
        TransportState transportState = mock(TransportState.class);
        int cause = DataFailCause.NO_RESPONSE_FROM_BASE_STATION;
        int timeout = 0;
        mTransHdlrSwisscom.setHandoverState(STATE_RETRY_4);
        boolean expVal = true;
        when(transport.getHandoverState()).thenReturn(transportState);
        when(transportState.getRetryCount()).thenReturn(2); // RETRY_IN_STATE - 1
        when(transport.getType()).thenReturn(TransportType.TRANSPORT_WIFI);

        //When
        boolean returnVal = mTransHdlrSwisscom.onHandoverFailure(transport, cause, timeout);

        //Then
        assertEquals(expVal, returnVal);

    }

    @Test
    public void testOnHandoverFailureStateRetry5() {
        //Given
        Transport transport = mock(Transport.class);
        int cause = DataFailCause.NO_RESPONSE_FROM_BASE_STATION;
        TransportState transportState = mock(TransportState.class);
        int timeout = 0;
        mTransHdlrSwisscom.setHandoverState(STATE_RETRY_5);
        boolean expVal = true;
        when(transport.getHandoverState()).thenReturn(transportState);
        when(transportState.getRetryCount()).thenReturn(3);
        when(transport.getType()).thenReturn(TransportType.TRANSPORT_WIFI);

        //When
        boolean returnVal = mTransHdlrSwisscom.onHandoverFailure(transport, cause, timeout);

        //Then
        assertEquals(expVal, returnVal);
    }

    @Test
    public void testOnHandoverFailureStateRetry5MoreRetires() {
        //Given
        Transport transport = mock(Transport.class);
        TransportState transportState = mock(TransportState.class);
        int cause = DataFailCause.NO_RESPONSE_FROM_BASE_STATION;
        int timeout = 0;
        mTransHdlrSwisscom.setHandoverState(STATE_RETRY_5);
        when(transportState.getRetryCount()).thenReturn(5);
        when(transport.getHandoverState()).thenReturn(transportState);
        when(transport.getType()).thenReturn(TransportType.TRANSPORT_WIFI);
        boolean expVal = true;

        //When
        boolean returnVal = mTransHdlrSwisscom.onHandoverFailure(transport, cause, timeout);

        //Theno
        assertEquals(expVal, returnVal);
    }

    @Test
    public void testOnHandoverFailureStateRetry5MoreRetires_whenRetryCountIsLowerThanRETRY_IN_STATE() {
        //Given
        Transport transport = mock(Transport.class);
        TransportState transportState = mock(TransportState.class);
        int cause = DataFailCause.NO_RESPONSE_FROM_BASE_STATION;
        int timeout = 0;
        mTransHdlrSwisscom.setHandoverState(STATE_RETRY_5);
        when(transportState.getRetryCount()).thenReturn(2); // RETRY_IN_STATE - 1
        when(transport.getHandoverState()).thenReturn(transportState);
        when(transport.getType()).thenReturn(TransportType.TRANSPORT_WIFI);
        boolean expVal = true;

        //When
        boolean returnVal = mTransHdlrSwisscom.onHandoverFailure(transport, cause, timeout);

        //Theno
        assertEquals(expVal, returnVal);
    }

    @Test
    public void testOnHandoverFailureStateRetry1() {
        //Given
        Transport transport = mock(Transport.class);
        TransportState transportState = mock(TransportState.class);
        int cause = DataFailCause.NO_RESPONSE_FROM_BASE_STATION;
        int timeout = 0;
        mTransHdlrSwisscom.setHandoverState(STATE_RETRY_1);
        boolean expVal = true;
        when(transport.getHandoverState()).thenReturn(transportState);
        when(transportState.getRetryCount()).thenReturn(3);
        when(transport.getType()).thenReturn(TransportType.TRANSPORT_WIFI);

        //When
        boolean returnVal = mTransHdlrSwisscom.onHandoverFailure(transport, cause, timeout);

        //Then
        assertEquals(expVal, returnVal);
    }

    @Test
    public void testOnHandoverFailureRetryCountLessThanOne() {
        //Given
        Transport transport = mock(Transport.class);
        TransportState transportState = mock(TransportState.class);
        int cause = DataFailCause.REJECTED_BY_BASE_STATION;
        int timeout = 0;
        mTransHdlrSwisscom.setHandoverState(STATE_RETRY_1);
        boolean expVal = false;
        when(transport.getHandoverState()).thenReturn(transportState);
        when(transportState.getRetryCount()).thenReturn(0);
        when(transport.getType()).thenReturn(TransportType.TRANSPORT_WIFI);

        //When
        boolean returnVal = mTransHdlrSwisscom.onHandoverFailure(transport, cause, timeout);

        //Then
        verify(transport.getHandoverState(), times(1)).blockTemporary(anyInt());
    }

    @Test
    public void testOnHandoverFailureRetryCountMoreThanTwelve() {
        //Given
        Transport transport = mock(Transport.class);
        TransportState transportState = mock(TransportState.class);
        int cause = DataFailCause.REJECTED_BY_BASE_STATION;
        int timeout = 0;
        mTransHdlrSwisscom.setState(STATE_RETRY_1);
        boolean expVal = false;
        when(transport.getHandoverState()).thenReturn(transportState);
        when(transportState.getRetryCount()).thenReturn(14);
        when(transport.getType()).thenReturn(TransportType.TRANSPORT_WIFI);

        //When
        boolean returnVal = mTransHdlrSwisscom.onHandoverFailure(transport, cause, timeout);

        //Then
        verify(transport.getHandoverState(), times(1)).blockTemporary(anyInt());
    }
}




