package com.shannon.dataservice;

import static org.mockito.ArgumentMatchers.any;
import static org.mockito.ArgumentMatchers.anyInt;
import static org.mockito.Mockito.doNothing;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.mockStatic;
import static org.mockito.Mockito.mockitoSession;
import static org.mockito.Mockito.spy;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

import android.content.Context;
import android.net.ConnectivityManager;
import android.net.LinkProperties;
import android.telephony.AccessNetworkConstants.AccessNetworkType;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
import android.telephony.data.DataProfile;
import android.telephony.data.DataService;
import android.telephony.data.DataServiceCallback;
import android.util.SparseArray;

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
import org.mockito.MockitoSession;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.annotation.Config;

import vendor.samsung_slsi.telephony.hardware.radioExternal.V1_0.IOemSlsiRadioExternal;

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class ShannonDataServiceTest extends TestCase {
    private static int testCid = 0;

    private ShannonDataService mShannonDataService;
    private ShannonDataService mSpyShannonDataService;

    private ShannonDataService.ShannonDataServiceProvider mShannonDataServiceProvider;
    private MockitoSession mStaticMockSession;

    // static
    private static MockedStatic<SubscriptionManager> mStaticSubscriptionManager;
    private static MockedStatic<RilOemInterface> mStaticRilOemInterface;

    @Mock private Context mMockContext;;
    @Mock private DataServiceCallback mMockDataServiceCallback;
    @Mock private IkeDaemonInterface mMockIkeDaemonInterface;
    @Mock private IOemSlsiRadioExternal mMockOemRil;
    @Mock private SimManager mMockSimManager;
    @Mock private SparseArray mMockServiceProviders;

    @BeforeClass
    public static void beforeClass() {
        mStaticSubscriptionManager = mockStatic(SubscriptionManager.class);
        mStaticRilOemInterface = mockStatic(RilOemInterface.class);
    }

    @AfterClass
    public static void afterClass() {
        mStaticSubscriptionManager.close();
        mStaticRilOemInterface.close();
    }

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.openMocks(this);
        mStaticMockSession = mockitoSession().startMocking();

        mShannonDataService = new ShannonDataService();
        mSpyShannonDataService = spy(mShannonDataService);

        when(SubscriptionManager.getSimStateForSlotIndex(TEST_VALUES.SLOT_ID_1))
                .thenReturn(TelephonyManager.SIM_STATE_READY);
        when(SubscriptionManager.isValidSubscriptionId(anyInt()))
                .thenReturn(true);
        when(SubscriptionManager.isValidSlotIndex(anyInt()))
                .thenReturn(true);
        when(SubscriptionManager.getSlotIndex(anyInt()))
                .thenReturn(TEST_VALUES.SLOT_ID_1);

        // sets Mocks members
        mMockContext = mock(Context.class);
        mMockIkeDaemonInterface = mock(IkeDaemonInterface.class);;
        mMockServiceProviders = new SparseArray<>();
        mMockSimManager = mock(SimManager.class);;

        // ShannonDataService create using mock keyword.
        mSpyShannonDataService.onCreate(mMockContext, mMockIkeDaemonInterface,
                mMockServiceProviders, mMockSimManager);

        ShannonDataService.getInstance();
    }

    @After
    public void cleanUp() throws Exception {
        if (mSpyShannonDataService != null) {
            mSpyShannonDataService.onDestroy();
        }
        mStaticMockSession.finishMocking();
    }

    @Test
    public void testSendPcoData() {
        // given
        DataService.DataServiceProvider dataServiceProvider = setMockOemInterface();

        // when
        mSpyShannonDataService.sendPcoData(TEST_VALUES.SLOT_ID_1, TEST_VALUES.CONNECTION_NAME,
                                            TEST_VALUES.PCO_ID_IPV4, new byte[10]);

        // then
        assertNotNull(dataServiceProvider);
    }

    @Test
    public void testOnCreateDataServiceProviderWithNull() {
        // given
        DataService.DataServiceProvider dataServiceProvider = setMockOemInterface();

        when(SubscriptionManager.isValidSlotIndex(anyInt()))
                .thenReturn(false);

        // when/then
        assertNull(mSpyShannonDataService.onCreateDataServiceProvider(TEST_VALUES.SLOT_ID_1));
    }

    @Test
    public void testLocalBinder() {
        ShannonDataService.LocalBinder localBinder = mSpyShannonDataService.new LocalBinder();

        // then
        assertNotNull(localBinder.getService());
    }

    /* inner class of "ShannonDataServiceProvider" */
    @Test
    public void testIwlanSetupDataCallWithValidArg() {
        // given
        mShannonDataServiceProvider = (ShannonDataService.ShannonDataServiceProvider) setMockOemInterface();

        // when
        mShannonDataServiceProvider.setupDataCall(
                AccessNetworkType.IWLAN,
                mock(DataProfile.class),
                true,
                true,
                DataService.REQUEST_REASON_NORMAL,
                new LinkProperties(),
                mMockDataServiceCallback
        );

        // then
        verify(mMockContext, times(1)).sendBroadcast(any());
    }

    @Test
    public void testIwlanSetupDataCallWithInvalidArg() {
        // given
        mShannonDataServiceProvider = (ShannonDataService.ShannonDataServiceProvider) setMockOemInterface();

        // when
        mShannonDataServiceProvider.setupDataCall(
                AccessNetworkType.EUTRAN, // EUTRAN is sent instead of IWLAN
                mock(DataProfile.class),
                true,
                true,
                DataService.REQUEST_REASON_NORMAL,
                new LinkProperties(),
                mMockDataServiceCallback
        );

        // then
        verify(mMockDataServiceCallback, times(1))
                .onSetupDataCallComplete(DataServiceCallback.RESULT_ERROR_UNSUPPORTED, null);
    }

    @Test
    public void testIwlanDeactivateDataCallWithValidArg() {
        // given
        mShannonDataServiceProvider = (ShannonDataService.ShannonDataServiceProvider) setMockOemInterface();

        // when
        mShannonDataServiceProvider.deactivateDataCall(
                testCid,
                DataService.REQUEST_REASON_NORMAL,
                mMockDataServiceCallback
        );

        // then
        verify(mMockContext, times(1)).sendBroadcast(any());
    }

    @Test
    public void testRequestDataCallList() {
        // given
        mShannonDataServiceProvider = (ShannonDataService.ShannonDataServiceProvider) setMockOemInterface();

        // when
        mShannonDataServiceProvider.setPcoData("ims", TEST_VALUES.PCO_ID_IPV4, new byte[10]);
        mShannonDataServiceProvider.requestDataCallList(mMockDataServiceCallback);

        // then
        verify(mMockContext, times(1)).sendBroadcast(any());
    }

    @Test
    public void onClose() {
        // given
        ConnectivityManager mMockConnectivityManager = mock(ConnectivityManager.class);
        doNothing().when(mMockConnectivityManager).unregisterNetworkCallback(any(ConnectionManager.ConnectionNetworkCallback.class));
        when(mMockContext.getSystemService(ConnectivityManager.class))
                .thenReturn(mMockConnectivityManager);

        mShannonDataServiceProvider = (ShannonDataService.ShannonDataServiceProvider) setMockOemInterface();

        mShannonDataServiceProvider.close();
    }

    public DataService.DataServiceProvider setMockOemInterface() {
        DataService.DataServiceProvider dataServiceProvider;

        try (MockedConstruction<RilOemInterface> mockedConstruction =
                     Mockito.mockConstruction(RilOemInterface.class);) {
            dataServiceProvider = mSpyShannonDataService.onCreateDataServiceProvider(TEST_VALUES.SLOT_ID_1);
        }
        return dataServiceProvider;
    }
}
