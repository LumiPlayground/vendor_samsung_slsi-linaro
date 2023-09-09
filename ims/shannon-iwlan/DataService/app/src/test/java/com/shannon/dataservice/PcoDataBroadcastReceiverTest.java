package com.shannon.dataservice;

import static org.mockito.ArgumentMatchers.any;
import static org.mockito.ArgumentMatchers.anyInt;
import static org.mockito.ArgumentMatchers.eq;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.mockStatic;
import static org.mockito.Mockito.mockitoSession;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;

import android.content.Context;
import android.content.Intent;
import android.net.Network;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
import android.telephony.data.ApnSetting;

import com.shannon.dataservice.ShannonDataService.ShannonDataServiceProvider;

import junit.framework.TestCase;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.Mock;
import org.mockito.MockedStatic;
import org.mockito.Mockito;
import org.mockito.MockitoAnnotations;
import org.mockito.MockitoSession;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.annotation.Config;

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class PcoDataBroadcastReceiverTest extends TestCase {
    private PcoDataBroadcastReceiver mPcoDataBroadcastReceiver;

    private static final String ACTION_CARRIER_SIGNAL_PCO_VALUE =
    TelephonyManager.ACTION_CARRIER_SIGNAL_PCO_VALUE;
    private static final String EXTRA_APN_TYPE_INT_KEY = TelephonyManager.EXTRA_APN_TYPE;
    private static final String EXTRA_PCO_ID_KEY = TelephonyManager.EXTRA_PCO_ID;
    private static final String EXTRA_PCO_VALUE_KEY = TelephonyManager.EXTRA_PCO_VALUE;

    private static final String TEST_PCO_STRING = "testPcoData";
    private byte[] pcoData = TEST_PCO_STRING.getBytes();

    MockitoSession mStaticMockSession;

    @Mock private ShannonDataService mMockShannonDataService = mock(ShannonDataService.class);
    @Mock private ShannonDataServiceProvider mShannonDataServiceProvider =
            mock(ShannonDataServiceProvider.class);
    @Mock private Context mMockContext;

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.openMocks(this);

        mStaticMockSession = mockitoSession().startMocking();

        mPcoDataBroadcastReceiver = new PcoDataBroadcastReceiver();
    }

    @After
    public void celanUp() {
        mStaticMockSession.finishMocking();
    }

    @Test
    public void testOnReceiveIPv6Pass() throws Exception {
        try (MockedStatic<ShannonDataService> mock = Mockito.mockStatic(ShannonDataService.class)) {
            mock.when(ShannonDataService::getInstance).thenReturn(mMockShannonDataService);

            try (MockedStatic<SubscriptionManager> mock1 =
                    Mockito.mockStatic(SubscriptionManager.class)) {
                mock1.when(() -> SubscriptionManager.isValidSubscriptionId(anyInt())).thenReturn(true);
                mock1.when(() -> SubscriptionManager.getSlotIndex(anyInt())).thenReturn(TEST_VALUES.SLOT_ID_1);

                // Create intent object
                final Intent mIntent = new Intent(ACTION_CARRIER_SIGNAL_PCO_VALUE);
                mIntent.putExtra(SubscriptionManager.EXTRA_SUBSCRIPTION_INDEX, TEST_VALUES.SUB_ID_1);
                mIntent.putExtra(EXTRA_APN_TYPE_INT_KEY, ApnSetting.TYPE_IMS);
                mIntent.putExtra(TelephonyManager.EXTRA_APN_TYPE, ApnSetting.getApnTypeString(ApnSetting.TYPE_IMS));
                mIntent.putExtra(EXTRA_PCO_ID_KEY, TEST_VALUES.PCO_ID_IPv6);
                mIntent.putExtra(EXTRA_PCO_VALUE_KEY, pcoData);

                // Trigger onReceive method
                mPcoDataBroadcastReceiver.onReceive(mMockContext, mIntent);

                verify(mMockShannonDataService, times(1))
                        .sendPcoData(TEST_VALUES.SLOT_ID_1, ApnSetting.TYPE_IMS_STRING, TEST_VALUES.PCO_ID_IPv6,
                                pcoData);
            }
        }
    }

    @Test
    public void testOnReceiveFail() throws Exception {
        final Intent mIntent = new Intent();
        mPcoDataBroadcastReceiver.onReceive(mMockContext, mIntent);
    }
}
