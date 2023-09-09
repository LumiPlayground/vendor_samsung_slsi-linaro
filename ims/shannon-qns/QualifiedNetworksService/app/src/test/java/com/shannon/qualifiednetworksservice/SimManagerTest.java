package com.shannon.qualifiednetworksservice;

import android.content.Context;
import android.content.Intent;
import android.net.ConnectivityManager;
import android.os.Message;
import android.os.PersistableBundle;
import android.telephony.CarrierConfigManager;
import android.telephony.SubscriptionInfo;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;

import com.android.internal.telephony.PhoneConstants;
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

import static org.mockito.ArgumentMatchers.anyInt;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class SimManagerTest extends TestCase {
    private SimManager mSimManager;

    @Mock
    Context mockContext;

    @Mock
    TelephonyManager mockTelephonyManager;

    @Mock
    SubscriptionManager mockSubscriptionManager;

    @Mock
    ConnectivityManager mockConnectivityManager;

    @Mock
    SimInformation mockSimInformation;

    @Mock
    MobileNetworkCallback.IMobileCallbackEvent mockMobileCallback;

    @Mock
    Message mockMessage;

    @Mock
    CarrierConfigManager mockCarrierConfigManager;

    @Mock
    PersistableBundle mockBundle;

    @Mock
    Intent mockIntent;

    private MockedStatic<SubscriptionManager> mockedStaticSubscriptionManager;
    private MockedStatic<SimInformation> mockedStaticSimInformation;
    private MockedStatic<CarrierConfigManager> mockedStaticCarrierConfigManager;
    private MockedStatic<Message> mockedStaticMessage;

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.openMocks(this);

        mockedStaticMessage = Mockito.mockStatic(Message.class);

        mockedStaticSubscriptionManager = Mockito.mockStatic(SubscriptionManager.class);

        when(mockContext.getSystemService(TelephonyManager.class)).thenReturn(mockTelephonyManager);
        when(mockContext.getSystemService(SubscriptionManager.class)).thenReturn(mockSubscriptionManager);
        when(mockContext.getSystemService(ConnectivityManager.class)).thenReturn(mockConnectivityManager);
        when(mockTelephonyManager.getPhoneCount()).thenReturn(1);
        when(SubscriptionManager.getSimStateForSlotIndex(0)).thenReturn(TelephonyManager.SIM_STATE_ABSENT);
        SimManager manager = new SimManager(mockContext);

        mockedStaticSimInformation = Mockito.mockStatic(SimInformation.class);
        when(SubscriptionManager.getSimStateForSlotIndex(0)).thenReturn(TelephonyManager.SIM_STATE_PRESENT);
        when(SimInformation.createSimInformation(0, mockSubscriptionManager, mockTelephonyManager)).thenReturn(null);
        manager = new SimManager(mockContext);

        mockedStaticCarrierConfigManager = Mockito.mockStatic(CarrierConfigManager.class);

        when(SimInformation.createSimInformation(0, mockSubscriptionManager, mockTelephonyManager)).thenReturn(mockSimInformation);
        when(mockSimInformation.getTelephonyManager()).thenReturn(mockTelephonyManager);
        when(mockSimInformation.getSubId()).thenReturn(1);
        mSimManager = new SimManager(mockContext);
    }

    @After
    public void tearDown() throws Exception {
        mockedStaticMessage.close();
        mockedStaticSubscriptionManager.close();
        mockedStaticSimInformation.close();
        mockedStaticCarrierConfigManager.close();
        Mockito.validateMockitoUsage();
    }

    @Test
    public void testGetInstance() {
        assertEquals(mSimManager, SimManager.getInstance());
    }

    @Test
    public void testDestroy() {
        mSimManager.destroy();
        assertTrue(true);
    }

    @Test
    public void testGetMcc() {
        when(mockSimInformation.getMcc()).thenReturn(310);
        assertEquals(310, mSimManager.getMcc(0));
        assertEquals(-1, mSimManager.getMcc(1));
    }

    @Test
    public void testGetMnc() {
        when(mockSimInformation.getMnc()).thenReturn(260);
        assertEquals(260, mSimManager.getMnc(0));
        assertEquals(-1, mSimManager.getMnc(1));
    }

    @Test
    public void testGetSubId() {
        when(mockSimInformation.getSubId()).thenReturn(1);
        assertEquals(1, mSimManager.getSubId(0));
        assertEquals(-1, mSimManager.getSubId(1));
    }

    @Test
    public void testGetTelephonyManager() {
        when(mockSimInformation.getTelephonyManager()).thenReturn(mockTelephonyManager);
        assertEquals(mockTelephonyManager, mSimManager.getTelephonyManager(0));
        assertNull(mSimManager.getTelephonyManager(1));
    }

    @Test
    public void testRegisterForSimEvent() {
        mSimManager.registerForSimEvent(1, mockMessage);

        when(mockSimInformation.getSubId()).thenReturn(1);
        when(mockContext.getSystemService(CarrierConfigManager.class)).thenReturn(null);
        mSimManager.registerForSimEvent(0, mockMessage);

        when(mockContext.getSystemService(CarrierConfigManager.class)).thenReturn(mockCarrierConfigManager);
        when(mockCarrierConfigManager.getConfigForSubId(1)).thenReturn(null);
        mSimManager.registerForSimEvent(0, mockMessage);

        when(mockCarrierConfigManager.getConfigForSubId(1)).thenReturn(mockBundle);
        when(CarrierConfigManager.isConfigForIdentifiedCarrier(mockBundle)).thenReturn(false);
        mSimManager.registerForSimEvent(0, mockMessage);

        when(CarrierConfigManager.isConfigForIdentifiedCarrier(mockBundle)).thenReturn(true);
        when(Message.obtain(mockMessage)).thenReturn(mockMessage);
        mSimManager.registerForSimEvent(0, mockMessage);

        assertTrue(true);
    }

    @Test
    public void testRegisterForNetworkCallbackEvent() {
        assertNotNull(mSimManager.registerForNetworkCallbackEvent(mockMobileCallback));
    }

    @Test
    public void testUnregisterNetworkCallbackEvent() {
        mSimManager.unregisterNetworkCallbackEvent(mockMobileCallback);
        assertTrue(true);
    }

    @Test
    public void testReceiver_onReceive() {
        SimManager.SimBroadcastReceiver receiver = mSimManager.getReceiver();
        when(mockIntent.getAction()).thenReturn(null);
        receiver.onReceive(mockContext, mockIntent);

        when(mockIntent.getAction()).thenReturn(TelephonyIntents.ACTION_ANY_DATA_CONNECTION_STATE_CHANGED);
        receiver.onReceive(mockContext, mockIntent);

        when(mockIntent.getAction()).thenReturn(TelephonyIntents.ACTION_SIM_STATE_CHANGED);
        when(mockIntent.getIntExtra(PhoneConstants.PHONE_KEY, -1)).thenReturn(-1);
        receiver.onReceive(mockContext, mockIntent);

        when(mockIntent.getIntExtra(PhoneConstants.PHONE_KEY, -1)).thenReturn(0);
        receiver.onReceive(mockContext, mockIntent);

        mSimManager.registerForNetworkCallbackEvent(mockMobileCallback);
        when(Message.obtain(mockMessage)).thenReturn(mockMessage);
        when(SubscriptionManager.getSimStateForSlotIndex(0)).thenReturn(TelephonyManager.SIM_STATE_ABSENT);
        mSimManager.registerForSimEvent(0, mockMessage);
        receiver.onReceive(mockContext, mockIntent);

        receiver.onReceive(mockContext, mockIntent);

        when(SimInformation.createSimInformation(0, mockSubscriptionManager, mockTelephonyManager)).thenReturn(mockSimInformation);
        when(SubscriptionManager.getSimStateForSlotIndex(0)).thenReturn(TelephonyManager.SIM_STATE_PRESENT);
        receiver.onReceive(mockContext, mockIntent);

        when(mockIntent.getAction()).thenReturn(CarrierConfigManager.ACTION_CARRIER_CONFIG_CHANGED);
        when(mockIntent.getIntExtra(CarrierConfigManager.EXTRA_SLOT_INDEX, -1)).thenReturn(-1);
        receiver.onReceive(mockContext, mockIntent);

        when(mockIntent.getIntExtra(CarrierConfigManager.EXTRA_SLOT_INDEX, -1)).thenReturn(0);
        when(SubscriptionManager.getSimStateForSlotIndex(0)).thenReturn(TelephonyManager.SIM_STATE_PRESENT);
        receiver.onReceive(mockContext, mockIntent);

        when(mockSimInformation.getSubId()).thenReturn(1);
        when(mockContext.getSystemService(CarrierConfigManager.class)).thenReturn(mockCarrierConfigManager);
        when(mockCarrierConfigManager.getConfigForSubId(1)).thenReturn(mockBundle);
        when(CarrierConfigManager.isConfigForIdentifiedCarrier(mockBundle)).thenReturn(true);
        when(SubscriptionManager.getSimStateForSlotIndex(0)).thenReturn(TelephonyManager.SIM_STATE_LOADED);
        receiver.onReceive(mockContext, mockIntent);

        assertTrue(true);
    }

    @Test
    public void testIsOpportunisticSubscription() {
        when(mockTelephonyManager.getPreferredOpportunisticDataSubscription())
                .thenReturn(SubscriptionManager.DEFAULT_SUBSCRIPTION_ID);
        assertEquals(false, mSimManager.isOpportunisticSubscription(0, 1));

        when(mockTelephonyManager.getPreferredOpportunisticDataSubscription())
                .thenReturn(1);
        assertEquals(false, mSimManager.isOpportunisticSubscription(0, 1));

        when(mockTelephonyManager.getPreferredOpportunisticDataSubscription())
                .thenReturn(1);
        when(mockSubscriptionManager.getActiveSubscriptionInfoForSimSlotIndex(anyInt()))
                .thenReturn(null);
        assertEquals(false, mSimManager.isOpportunisticSubscription(0, 1));

        SubscriptionInfo mockSubscriptionInfo = mock(SubscriptionInfo.class);
        when(mockTelephonyManager.getPreferredOpportunisticDataSubscription())
                .thenReturn(1);

        when(mockSubscriptionInfo.isOpportunistic()).thenReturn(true);
        when(mockSubscriptionManager.getActiveSubscriptionInfoForSimSlotIndex(anyInt()))
                .thenReturn(mockSubscriptionInfo);
        assertEquals(true, mSimManager.isOpportunisticSubscription(0, 1));
    }

    @Test
    public void testIsOpportunisticSubscription_exception() {
        when(mockTelephonyManager.getPreferredOpportunisticDataSubscription())
                .thenThrow(new NullPointerException());
        mSimManager.isOpportunisticSubscription(0, 1);
    }
}