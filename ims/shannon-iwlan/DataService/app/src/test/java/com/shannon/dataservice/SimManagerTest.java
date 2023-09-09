package com.shannon.dataservice;

import android.content.Context;
import android.content.Intent;
import android.net.ConnectivityManager;
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
import org.mockito.MockedConstruction;
import org.mockito.MockedStatic;
import org.mockito.MockitoAnnotations;
import org.mockito.MockitoSession;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.annotation.Config;

import static com.android.internal.telephony.PhoneConstants.DEFAULT_SLOT_INDEX;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertNull;
import static org.mockito.ArgumentMatchers.any;
import static org.mockito.ArgumentMatchers.anyInt;
import static org.mockito.Mockito.doNothing;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.mockConstruction;
import static org.mockito.Mockito.mockStatic;
import static org.mockito.Mockito.mockitoSession;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class SimManagerTest {
    MockitoSession mStaticMockSession;

    @Mock private TelephonyManager mMockTelephonyManager;
    @Mock private Context mMockContext;
    @Mock private SubscriptionManager mMockSubscriptionManager;
    @Mock private SimInformation mMockSimInformation;
    @Mock private ConnectivityManager mMockConnectivityManager;

    private static MockedStatic<SubscriptionManager> mSubscriptionManager;
    private static MockedStatic<SimInformation> mSimInformation;

    SimManager mSimManager;

    @BeforeClass
    public static void beforeClass() {
        mSubscriptionManager = mockStatic(SubscriptionManager.class);
        mSimInformation = mockStatic(SimInformation.class);
    }

    @AfterClass
    public static void afterClass() {
        mSubscriptionManager.close();
        mSimInformation.close();
    }

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.openMocks(this);
        mStaticMockSession = mockitoSession().startMocking();

        when(mMockTelephonyManager.getNetworkSpecifier())
                .thenReturn(String.valueOf(TEST_VALUES.SLOT_ID_1));

        // mocking method for mMockTelephonyManager
        when(mMockTelephonyManager.getPhoneCount())
                .thenReturn(0);

        // mocking method for mMockContext
        when(mMockContext.getSystemService(SubscriptionManager.class))
                .thenReturn(mMockSubscriptionManager);
        when(mMockContext.getSystemService(TelephonyManager.class))
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
    public void testGetSimInformation() {
        when(SubscriptionManager.getSimStateForSlotIndex(TEST_VALUES.SLOT_ID_1)).thenReturn(TelephonyManager.SIM_STATE_READY);

        Intent intent = new Intent(TelephonyIntents.ACTION_SIM_STATE_CHANGED);
        intent.putExtra(PhoneConstants.PHONE_KEY, DEFAULT_SLOT_INDEX);
    }

    @Test
    public void destroy() {
        mSimManager.destroy();

        verify(mMockContext, times(1)).unregisterReceiver(any());
    }

    @Test
    public void getSimInformation() {
        // given
        addSim();

        // when/then
        assertNotNull(mSimManager.getSimInformation(TEST_VALUES.SLOT_ID_1));
    }

    @Test
    public void isSimInformationAvailable() {
        // given
        addSim();

        // when/then
        assertNotNull(mSimManager.isSimInformationAvailable(TEST_VALUES.SLOT_ID_1));
    }

    @Test
    public void getMobileNetwork() {
        // given
        int notUsedSlotId = 0;
        ConfigurationManager config = mock(ConfigurationManager.class);
        when(config.isVoMobileEnabled()).thenReturn(false);

        // when/then
        assertNull(mSimManager.getMobileNetwork(notUsedSlotId, config));
    }

    /* Not for test */
    /* For easy testing, only use the below function in test code */
    public void addSim() {
        when(SimInformation.createSimInformation(anyInt(), any(), any()))
                .thenReturn(mMockSimInformation);
        SimManager.CreateSimInformationJob createSimInformationJob =
                mSimManager.new CreateSimInformationJob(TEST_VALUES.SLOT_ID_1);
        doNothing().when(mMockConnectivityManager).registerNetworkCallback(any(),
                any(MobileNetworkCallback.class));

        try (MockedConstruction<MobileNetworkCallback> mockedConstruction =
                     mockConstruction(MobileNetworkCallback.class)) {
            createSimInformationJob.run();
        }
    }

    @Test
    public void getInstance() {
        assertNotNull(SimManager.getInstance());
    }

    /* inner class test */
    @Test
    public void onReceiveWithNoAction() {
        // given
        SimManager.SimStateBroadcastReceiver simStateBroadcastReceiver = mSimManager.new SimStateBroadcastReceiver();

        // when
        simStateBroadcastReceiver.onReceive(mMockContext, new Intent());
    }
}
