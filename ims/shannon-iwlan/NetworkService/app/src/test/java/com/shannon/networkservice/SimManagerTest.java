package com.shannon.networkservice;

import android.content.Context;
import android.content.Intent;
import android.net.ConnectivityManager;
import android.net.LinkProperties;
import android.net.Network;
import android.net.NetworkCapabilities;
import android.telephony.NetworkRegistrationInfo;
import android.telephony.ServiceState;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;

import com.android.internal.telephony.PhoneConstants;
import com.android.internal.telephony.TelephonyIntents;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.Mock;
import org.mockito.MockedStatic;
import org.mockito.MockitoAnnotations;
import org.mockito.MockitoSession;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.annotation.Config;

import static org.junit.Assert.*;
import static org.mockito.ArgumentMatchers.any;
import static org.mockito.ArgumentMatchers.anyInt;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.mockStatic;
import static org.mockito.Mockito.mockitoSession;
import static org.mockito.Mockito.when;

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class SimManagerTest {
    private static final int testSlotId_0 = 0;
    private static final int testSlotId_1 = 1;
    private static final int testSubId = 1;
    private static final int testNetId = 100;
    private static final int phoneCount = 2;

    @Mock private ConfigurationManager mMockConfigurationManager;
    @Mock private ConnectivityManager mMockConnectivityManager;
    @Mock private Context mMockContext;
    @Mock private SubscriptionManager mMockSubscriptionManager;
    @Mock private TelephonyManager mMockTelephonyManager;
    MockitoSession mStaticMockSession;

    private static MockedStatic<SubscriptionManager> mSubscriptionManager;

    SimManager mSimManager;
    SimManager.MobileNetworkCallback mMobileNetworkCallback;

    @BeforeClass
    public static void beforeClass() {
        mSubscriptionManager = mockStatic(SubscriptionManager.class);
    }

    @AfterClass
    public static void afterClass() {
        mSubscriptionManager.close();
    }

    @Before
    public void setUp() {
        MockitoAnnotations.openMocks(this);
        mStaticMockSession = mockitoSession().startMocking();

        when(mMockTelephonyManager.getNetworkSpecifier()).thenReturn(String.valueOf(testSlotId_0));
        when(mMockTelephonyManager.getPhoneCount()).thenReturn(phoneCount);
        when(mMockContext.getSystemService(SubscriptionManager.class))
                .thenReturn(mMockSubscriptionManager);
        when(mMockContext.getSystemService(TelephonyManager.class))
                .thenReturn(mMockTelephonyManager);
        when(mMockTelephonyManager.createForSubscriptionId(testSubId))
                .thenReturn(mMockTelephonyManager);
        when(mMockContext.getSystemService(ConnectivityManager.class))
                .thenReturn(mMockConnectivityManager);

        mSimManager = new SimManager(mMockContext);
    }

    @After
    public void tearDown() {
        mStaticMockSession.finishMocking();
    }

    @Test
    public void test001_receiverSimReady() {
        Intent intent = new Intent(TelephonyIntents.ACTION_SIM_STATE_CHANGED);
        intent.putExtra(PhoneConstants.PHONE_KEY, testSlotId_0);

        when(SubscriptionManager.getSimStateForSlotIndex(testSlotId_0))
                .thenReturn(TelephonyManager.SIM_STATE_READY);
        when(mMockSubscriptionManager.getSubscriptionIds(testSlotId_0))
                .thenReturn(new int[]{testSubId});

        mSimManager.getSimStateBroadcastReceiver().onReceive(mMockContext, intent);
    }

    @Test
    public void test002_receiverSimLoaded() {
        test001_receiverSimReady();

        Intent intent = new Intent(TelephonyIntents.ACTION_SIM_STATE_CHANGED);
        intent.putExtra(PhoneConstants.PHONE_KEY, testSlotId_0);

        when(SubscriptionManager.getSimStateForSlotIndex(testSlotId_0))
                .thenReturn(TelephonyManager.SIM_STATE_LOADED);
        when(mMockSubscriptionManager.getSubscriptionIds(testSlotId_0))
                .thenReturn(new int[]{testSubId});

        mSimManager.getSimStateBroadcastReceiver().onReceive(mMockContext, intent);
    }

    @Test
    public void test003_receiverDefault() {
        test001_receiverSimReady();

        Intent intent = new Intent(TelephonyIntents.ACTION_SIM_STATE_CHANGED);
        intent.putExtra(PhoneConstants.PHONE_KEY, testSlotId_0);

        when(SubscriptionManager.getSimStateForSlotIndex(testSlotId_0))
                .thenReturn(TelephonyManager.SIM_STATE_ABSENT);
        when(mMockSubscriptionManager.getSubscriptionIds(testSlotId_0))
                .thenReturn(new int[]{testSubId});

        mSimManager.getSimStateBroadcastReceiver().onReceive(mMockContext, intent);
    }

    @Test
    public void test004_receiverActionNull() {
        Intent intent = new Intent();
        mSimManager.getSimStateBroadcastReceiver().onReceive(mMockContext, intent);
    }

    @Test
    public void test005_4eceiverInvalidSoltId() {
        Intent intent = new Intent(TelephonyIntents.ACTION_SIM_STATE_CHANGED);
        intent.putExtra(PhoneConstants.PHONE_KEY, -1);

        mSimManager.getSimStateBroadcastReceiver().onReceive(mMockContext, intent);
    }

    @Test
    public void test006_mobileNetworkCallback() {
        mMobileNetworkCallback = mSimManager.new MobileNetworkCallback(testSlotId_0, testSubId);
        NetworkCapabilities nc = ShannonNetworkServiceTest.buildNetworkCapabilities();

        //buildRequest
        assertNotNull(mMobileNetworkCallback.buildRequest());

        //onLost
        mMobileNetworkCallback.onLost(new Network(testNetId));
        assertFalse(mMobileNetworkCallback.isConnected());

        //onAvailable
        mMobileNetworkCallback.onAvailable(
                new Network(testNetId),
                nc,
                mock(LinkProperties.class),
                false
        );
        assertTrue(mMobileNetworkCallback.isConnected());
    }

    @Test
    public void test007_mobileNetworkCallbackGetNetworkType() {
        mMobileNetworkCallback = mSimManager.new MobileNetworkCallback(testSlotId_0, testSubId);

        ServiceState mockServiceState = mock(ServiceState.class);
        when(mMockTelephonyManager.getServiceState()).thenReturn(mockServiceState);

        NetworkRegistrationInfo mockNetworkRegistrationInfo = mock(NetworkRegistrationInfo.class);
        when(mockServiceState.getNetworkRegistrationInfo(anyInt(), anyInt())).thenReturn(mockNetworkRegistrationInfo);
        when(mockNetworkRegistrationInfo.getAccessNetworkTechnology()).thenReturn(TelephonyManager.NETWORK_TYPE_IWLAN);
        assertEquals(TelephonyManager.NETWORK_TYPE_IWLAN, mMobileNetworkCallback.getNetworkType());
    }

    @Test
    public void test008_toString() {
        mMobileNetworkCallback = mSimManager.new MobileNetworkCallback(testSlotId_0, testSubId);
        assertNotNull(mMobileNetworkCallback.toString());
    }

    @Test
    public void test009_isVoMobilePossible() {
        test001_receiverSimReady();

        NetworkCapabilities nc = ShannonNetworkServiceTest.buildNetworkCapabilities();
        mMobileNetworkCallback = mSimManager.new MobileNetworkCallback(testSlotId_0, testSubId);
        mMobileNetworkCallback.onAvailable(
                new Network(testNetId),
                nc,
                mock(LinkProperties.class),
                false
        );

        ServiceState mockServiceState = mock(ServiceState.class);
        when(mMockTelephonyManager.getServiceState()).thenReturn(mockServiceState);

        NetworkRegistrationInfo mockNetworkRegistrationInfo = mock(NetworkRegistrationInfo.class);
        when(mockServiceState.getNetworkRegistrationInfo(anyInt(), anyInt())).thenReturn(mockNetworkRegistrationInfo);
        when(mockNetworkRegistrationInfo.getAccessNetworkTechnology()).thenReturn(TelephonyManager.NETWORK_TYPE_IWLAN);

        mMockConfigurationManager = mock(ConfigurationManager.class);

        assertFalse(mSimManager.isVoMobilePossible(testSlotId_1, mMockConfigurationManager));
    }

    @Test
    public void test010_destroy() {
        test001_receiverSimReady();
        mSimManager.destroy();
        assertNull(mSimManager.getMe());
    }

    @Test
    public void test011_registerCallback() {
        SimManager testSimManager = SimManager.getInstance(mMockContext);
        Intent intent = new Intent(TelephonyIntents.ACTION_SIM_STATE_CHANGED);
        intent.putExtra(PhoneConstants.PHONE_KEY, testSlotId_0);

        when(SubscriptionManager.getSimStateForSlotIndex(testSlotId_0))
                .thenReturn(TelephonyManager.SIM_STATE_LOADED);
        when(mMockSubscriptionManager.getSubscriptionIds(testSlotId_0))
                .thenReturn(new int[]{testSubId});

        testSimManager.getSimStateBroadcastReceiver().onReceive(mMockContext, intent);
    }

}