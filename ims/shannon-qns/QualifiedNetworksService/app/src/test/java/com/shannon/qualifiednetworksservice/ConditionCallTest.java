package com.shannon.qualifiednetworksservice;

import android.content.Context;
import android.content.Intent;
import android.os.Handler;
import android.os.Message;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
import android.util.SparseArray;

import com.android.internal.telephony.TelephonyIntents;

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

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import static com.shannon.qualifiednetworksservice.RilOemInterface.RIL_UNSOLICITED_MESSAGE;
import static com.shannon.qualifiednetworksservice.RilUnsolicitedMessage.RIL_UNSOL_CALL_MODIFIED;
import static com.shannon.qualifiednetworksservice.RilUnsolicitedMessage.RIL_UNSOL_CALL_STATUS;
import static com.shannon.qualifiednetworksservice.RilUnsolicitedMessage.RIL_UNSOL_LAST_CALL_FAIL_CAUSE;
import static com.shannon.qualifiednetworksservice.RilUnsolicitedMessage.RIL_UNSOL_RTP_EVENT;
import static com.shannon.qualifiednetworksservice.RilUnsolicitedMessage.RIL_UNSOL_SRVCC;
import static org.mockito.ArgumentMatchers.any;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class ConditionCallTest extends TestCase {
    private ConditionCall mConditions;

    @Mock
    Context mockContext;

    @Mock
    RilOemInterface mockRil;

    @Mock
    ConfigurationManager mockConfiguration;

    @Mock
    ConditionUserSetting mockUserSettings;

    @Mock
    HandoverConditions.IConditionChanged mockCallback;

    @Mock
    SimManager mockSimManager;

    @Mock
    TelephonyManager mockTelephonyManager;

    @Mock
    Intent mockIntent;

    @Mock
    RilUnsolicitedCallStatus mockCallStatus;

    @Mock
    HandoverConditions.IConditionHelper mockConditionHelper;

    private static MockedStatic<SimManager> mockedStaticSimManager;

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.openMocks(this);

        mockedStaticSimManager = Mockito.mockStatic(SimManager.class);
        when(SimManager.getInstance()).thenReturn(mockSimManager);
        when(mockSimManager.getTelephonyManager(0)).thenReturn(mockTelephonyManager);
        when(mockSimManager.getSubId(0)).thenReturn(1);
        when(mockTelephonyManager.getCallState()).thenReturn(TelephonyManager.CALL_STATE_IDLE);

        mConditions = new ConditionCall(0, mockContext, mockRil,
                mockConfiguration, mockConditionHelper, mockCallback);
    }

    @After
    public void tearDown() throws Exception {
        mockedStaticSimManager.close();
        Mockito.validateMockitoUsage();
    }

    @Test
    public void testDestroy() {
        mConditions.destroy();
        verify(mockContext, times(1)).unregisterReceiver(any());
    }

    @Test
    public void testIsImsCallActive() {
        SparseArray<ConditionCall.CallType> callList = mConditions.getCallList();

        assertFalse(mConditions.isImsCallActive(false));

        when(mockCallStatus.isAudioCall()).thenReturn(true);
        when(mockCallStatus.isVideoCall()).thenReturn(false);
        when(mockCallStatus.isEmergencyCall()).thenReturn(false);
        callList.append(1, new ConditionCall.CallType(mockCallStatus));
        assertTrue(mConditions.isImsCallActive(false));
        assertTrue(mConditions.isImsCallActive(true));

        callList.clear();
        when(mockCallStatus.isEmergencyCall()).thenReturn(true);
        callList.append(1, new ConditionCall.CallType(mockCallStatus));
        assertFalse(mConditions.isImsCallActive(true));
    }

    @Test
    public void testIsCallFailEmcRedialToIms() {
        assertFalse(mConditions.isCallFailEmcRedialToIms());
        mConditions.setLastFailCause(RilUnsolicitedLastCallFailCause.
                LastCallFailCause.REDIAL_EMERGENCY_TO_CELL);
        assertTrue(mConditions.isCallFailEmcRedialToIms());
    }

    @Test
    public void testIsCallFailEmcRedialToWiFi() {
        assertFalse(mConditions.isCallFailEmcRedialToWiFi());
        mConditions.setLastFailCause(RilUnsolicitedLastCallFailCause.
                LastCallFailCause.REDIAL_EMERGENCY_TO_WIFI);
        assertTrue(mConditions.isCallFailEmcRedialToWiFi());
    }

    @Test
    public void testClearLastCallFailCause() {
        mConditions.clearLastCallFailCause();
        assertTrue(true);
    }

    @Test
    public void testTestClearLastCallFailCause() {
        mConditions.clearLastCallFailCause(TelephonyManager.NETWORK_TYPE_IWLAN);
        mConditions.setLastFailCause(RilUnsolicitedLastCallFailCause.
                LastCallFailCause.REDIAL_EMERGENCY_TO_WIFI);
        mConditions.clearLastCallFailCause(TelephonyManager.NETWORK_TYPE_IWLAN);
        assertTrue(true);
    }

    @Test
    public void testIsVideoCallActive() {
        assertFalse(mConditions.isVideoCallActive());

        SparseArray<ConditionCall.CallType> callList = mConditions.getCallList();
        when(mockCallStatus.isAudioCall()).thenReturn(true);
        when(mockCallStatus.isVideoCall()).thenReturn(false);
        when(mockCallStatus.isEmergencyCall()).thenReturn(true);
        callList.append(1, new ConditionCall.CallType(mockCallStatus));
        when(mockCallStatus.isVideoCall()).thenReturn(true);
        when(mockCallStatus.isEmergencyCall()).thenReturn(false);
        callList.append(2, new ConditionCall.CallType(mockCallStatus));
        assertTrue(mConditions.isVideoCallActive());
    }

    @Test
    public void testIsAudioOnlyCallActive() {
        assertFalse(mConditions.isAudioOnlyCallActive());

        SparseArray<ConditionCall.CallType> callList = mConditions.getCallList();
        when(mockCallStatus.isAudioCall()).thenReturn(true);
        when(mockCallStatus.isVideoCall()).thenReturn(false);
        when(mockCallStatus.isEmergencyCall()).thenReturn(true);
        callList.append(1, new ConditionCall.CallType(mockCallStatus));
        when(mockCallStatus.isVideoCall()).thenReturn(true);
        when(mockCallStatus.isEmergencyCall()).thenReturn(false);
        callList.append(2, new ConditionCall.CallType(mockCallStatus));
        when(mockCallStatus.isVideoCall()).thenReturn(false);
        callList.append(3, new ConditionCall.CallType(mockCallStatus));
        assertTrue(mConditions.isAudioOnlyCallActive());
    }

    @Test
    public void testTestToString() {
        assertNotNull(mConditions.toString());
    }

    @Test
    public void testCallStateBroadcastReceiver() {
        ConditionCall.CallStateBroadcastReceiver receiver = mConditions.getBroadcastReceiver();

        when(mockIntent.getAction()).thenReturn(null);
        receiver.onReceive(mockContext, mockIntent);
        when(mockIntent.getAction()).thenReturn(TelephonyIntents.ACTION_ANY_DATA_CONNECTION_STATE_CHANGED);
        receiver.onReceive(mockContext, mockIntent);

        when(mockIntent.getAction()).thenReturn(TelephonyIntents.ACTION_EMERGENCY_CALLBACK_MODE_CHANGED);
        when(mockIntent.getIntExtra(SubscriptionManager.EXTRA_SLOT_INDEX, -1)).thenReturn(1);
        receiver.onReceive(mockContext, mockIntent);

        when(mockIntent.getIntExtra(SubscriptionManager.EXTRA_SLOT_INDEX, -1)).thenReturn(0);
        when(mockIntent.getBooleanExtra(TelephonyManager.EXTRA_PHONE_IN_ECM_STATE, false)).thenReturn(true);
        receiver.onReceive(mockContext, mockIntent);

        when(mockIntent.getBooleanExtra(TelephonyManager.EXTRA_PHONE_IN_ECM_STATE, false)).thenReturn(false);
        receiver.onReceive(mockContext, mockIntent);

        assertTrue(true);
    }

    @Test
    public void testHandler() {
        Handler handler = mConditions.getHandler();
        Message message = Message.obtain();

        message.what = 0;
        handler.handleMessage(message);

        RilUnsolicitedMessage rilUnsolicitedMessage = new RilUnsolicitedMessage(1000, null);
        message.what = RIL_UNSOLICITED_MESSAGE;
        message.obj = rilUnsolicitedMessage;
        handler.handleMessage(message);

        // RIL_UNSOL_CALL_STATUS
        ByteBuffer buffer = ByteBuffer.allocate(2048);
        buffer.order(ByteOrder.LITTLE_ENDIAN);
        buffer.putShort(Integer.valueOf(9).shortValue());
        buffer.put(Integer.valueOf(1).byteValue());
        buffer.put(Integer.valueOf(0).byteValue()); // active
        when(mockUserSettings.isWfcEnabledByPlatform()).thenReturn(true);
        rilUnsolicitedMessage = new RilUnsolicitedMessage(RIL_UNSOL_CALL_STATUS, buffer.array());
        message.what = RIL_UNSOLICITED_MESSAGE;
        message.obj = rilUnsolicitedMessage;
        handler.handleMessage(message);

        buffer.clear();
        buffer.putShort(Integer.valueOf(9).shortValue());
        buffer.put(Integer.valueOf(1).byteValue());
        buffer.put(Integer.valueOf(6).byteValue()); // released
        rilUnsolicitedMessage = new RilUnsolicitedMessage(RIL_UNSOL_CALL_STATUS, buffer.array());
        message.what = RIL_UNSOLICITED_MESSAGE;
        message.obj = rilUnsolicitedMessage;
        handler.handleMessage(message);

        // RIL_UNSOL_CALL_MODIFIED
        buffer.clear();
        buffer.put(Integer.valueOf(1).byteValue());
        buffer.putShort(Integer.valueOf(0x10).shortValue());
        buffer.put(Integer.valueOf(0).byteValue());
        rilUnsolicitedMessage = new RilUnsolicitedMessage(RIL_UNSOL_CALL_MODIFIED, buffer.array());
        message.what = RIL_UNSOLICITED_MESSAGE;
        message.obj = rilUnsolicitedMessage;
        handler.handleMessage(message);

        SparseArray<ConditionCall.CallType> callList = mConditions.getCallList();
        when(mockCallStatus.isAudioCall()).thenReturn(true);
        when(mockCallStatus.isVideoCall()).thenReturn(false);
        when(mockCallStatus.isEmergencyCall()).thenReturn(false);
        callList.append(1, new ConditionCall.CallType(mockCallStatus));
        when(mockConfiguration.canIgnoreCallUpgrade()).thenReturn(false);
        handler.handleMessage(message);

        buffer.clear();
        buffer.put(Integer.valueOf(1).byteValue());
        buffer.putShort(Integer.valueOf(0).shortValue());
        buffer.put(Integer.valueOf(0).byteValue());
        when(mockConfiguration.canIgnoreCallDowngrade()).thenReturn(false);
        rilUnsolicitedMessage = new RilUnsolicitedMessage(RIL_UNSOL_CALL_MODIFIED, buffer.array());
        message.what = RIL_UNSOLICITED_MESSAGE;
        message.obj = rilUnsolicitedMessage;
        handler.handleMessage(message);

        // RIL_UNSOL_SRVCC
        byte[] bytes = new byte[2];
        bytes[0] = 0;
        bytes[1] = 1;
        rilUnsolicitedMessage = new RilUnsolicitedMessage(RIL_UNSOL_SRVCC, bytes);
        message.what = RIL_UNSOLICITED_MESSAGE;
        message.obj = rilUnsolicitedMessage;
        callList.clear();
        callList.append(1, new ConditionCall.CallType(mockCallStatus));
        handler.handleMessage(message);

        // RIL_UNSOL_RTP_EVENT
        bytes[0] = 1;
        bytes[1] = 2;
        rilUnsolicitedMessage = new RilUnsolicitedMessage(RIL_UNSOL_RTP_EVENT, bytes);
        message.what = RIL_UNSOLICITED_MESSAGE;
        message.obj = rilUnsolicitedMessage;
        handler.handleMessage(message);

        // RIL_UNSOL_LAST_CALL_FAIL_CAUSE
        rilUnsolicitedMessage = new RilUnsolicitedMessage(RIL_UNSOL_LAST_CALL_FAIL_CAUSE, new byte[1]);
        message.what = RIL_UNSOLICITED_MESSAGE;
        message.obj = rilUnsolicitedMessage;
        handler.handleMessage(message);

        assertTrue(true);
    }
}