package com.shannon.dataservice;

import android.content.Context;
import android.net.ConnectivityManager;
import android.net.Network;
import android.os.AsyncResult;
import android.os.Message;
import android.os.PersistableBundle;
import android.telephony.CarrierConfigManager;
import android.telephony.DataFailCause;
import android.telephony.TelephonyManager;
import android.telephony.data.ApnSetting;
import android.telephony.data.DataCallResponse;

import androidx.test.platform.app.InstrumentationRegistry;

import org.bouncycastle.util.test.FixedSecureRandom;
import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.Mock;
import org.mockito.MockedStatic;
import org.mockito.MockitoAnnotations;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.annotation.Config;

import java.net.Inet4Address;
import java.net.Inet6Address;
import java.net.InetAddress;
import java.net.UnknownHostException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.LinkedList;

import static com.shannon.dataservice.RetryHandler.DEFAULT_RETRY_VALUE;
import static com.shannon.dataservice.StrokeResponseMessage.ProcessedNotifyType.STR_NOTIFY_NETWORK_AUTH_FAILED;
import static org.junit.Assert.*;
import static org.mockito.ArgumentMatchers.any;
import static org.mockito.ArgumentMatchers.anyInt;
import static org.mockito.ArgumentMatchers.eq;
import static org.mockito.Mockito.doNothing;
import static org.mockito.Mockito.doReturn;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.mockStatic;
import static org.mockito.Mockito.spy;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class RetryHandlerTest {
    private static int testSlotId = 0;
    private static ApnType testApnType = ApnType.APN_IMS;
    private static final String TEST_IPV4_ADDRESS_1 = "127.0.0.1";
    private static final String TEST_IPV4_ADDRESS_2 = "127.0.0.2";
    private static final String TEST_IPV6_ADDRESS_1 = "0000:0000:0000:0000:0000:0000:0000:0001";
    private static final String TEST_IPV6_ADDRESS_2 = "0000:0000:0000:0000:0000:0000:0000:0002";
    private static final int TEST_RETRY_TIMER_VALUE = 20;

    private RetryHandler mRetryHandler;
    private RetryHandler mSpyRetryHandler;

    @Mock private SimManager mMockSimManager;
    @Mock private SimInformation mMockSimInformation;
    @Mock private CarrierConfigManager mMockCarrierConfigManager;
    @Mock private Context mMockContext;
    @Mock private ConfigurationManager mMockConfigurationManager;
    @Mock private DnsResolver mMockResolver;
    @Mock private ConfigurationParameters mMockConfigurationParameters;
    @Mock private ConnectivityManager mMockConnectivityManager;
    @Mock private Network mMockNetwork;
    @Mock private TelephonyManager mMockTelephonyManager;

    private static MockedStatic<SimManager> mStaticSimManager;
    private static MockedStatic<DataFailCause> mStaticDataFailCause;

    private AutoCloseable closeable;

    @BeforeClass
    public static void beforeClass() {
        mStaticSimManager = mockStatic(SimManager.class);
        mStaticDataFailCause = mockStatic(DataFailCause.class);
    }

    @AfterClass
    public static void afterClass() {
        mStaticSimManager.close();
        mStaticDataFailCause.close();
    }

    @Before
    public void setUp() throws Exception {
        // initial setting complete

        // initializes objects annotated with Mockito annotations for given testClass: @Mock, @Spy, @Captor, @InjectMocks
        closeable = MockitoAnnotations.openMocks(this);

        // set static method
        when(SimManager.getInstance()).thenReturn(mMockSimManager);
        when(mMockSimManager.getSimInformation(anyInt()))
            .thenReturn(mMockSimInformation);
        when(mMockSimInformation.getTelephonyManager()).thenReturn(mMockTelephonyManager);
        when(mMockConfigurationManager.getConnectionConfiguration())
                .thenReturn(mMockConfigurationParameters);

        when(mMockContext.getSystemService(ConnectivityManager.class))
                .thenReturn(mMockConnectivityManager);
        when(mMockContext.getSystemService(CarrierConfigManager.class))
                .thenReturn(mMockCarrierConfigManager);

        // handler = new RetryHandler(mSlotId, apnType, mContext, mConfigurationManager, mDnsResolver);
        mRetryHandler = new RetryHandler(testSlotId, testApnType, "ims-apn", mMockContext,
                mMockConfigurationManager, mMockResolver, null);
        mSpyRetryHandler = spy(mRetryHandler);
    }

    @After
    public void tearDown() throws Exception {
        closeable.close();
        //mMockSimInformation.close();
    }

    @Test
    public void test001_updatePcoServerAddresses() {
        // given
        LinkedList<InetAddress> pcoAddrersses = new LinkedList<>();
        InetAddress mInetAddress = mock(InetAddress.class);
        pcoAddrersses.add(mInetAddress);

        // when
        mSpyRetryHandler.updatePcoServerAddresses(pcoAddrersses);

        // then
        assertEquals(true, mSpyRetryHandler.mPcoServerAddresses.size() == 1);
    }

    @Test
    public void test002_getFirstRemoteIpv4() {
        // given
        Inet6Address mInet6Address = mock(Inet6Address.class);
        Inet4Address mInet4Address = mock(Inet4Address.class);

        // when
        mSpyRetryHandler.mServerAddresses.add(mInet6Address);
        mSpyRetryHandler.mServerAddresses.add(mInet4Address);

        // then
        assertEquals(true, mSpyRetryHandler.getFirstRemoteIpv4().equals(mInet4Address));
    }

    @Test
    public void test003_getFirstRemoteIpv6() {
        // given
        Inet6Address mInet6Address = mock(Inet6Address.class);
        Inet4Address mInet4Address = mock(Inet4Address.class);

        // when
        mSpyRetryHandler.mServerAddresses.add(mInet6Address);
        mSpyRetryHandler.mServerAddresses.add(mInet4Address);

        // then
        assertEquals(true, mSpyRetryHandler.getFirstRemoteIpv6().equals(mInet6Address));
    }

    @Test
    public void test004_hasIpv4() {
        // given
        Inet6Address mInet6Address = mock(Inet6Address.class);
        Inet4Address mInet4Address = mock(Inet4Address.class);

        // when
        ArrayList<InetAddress> mList = new ArrayList<>();

        mList.add(mInet6Address);
        mList.add(mInet4Address);

        // then
        assertEquals(true, mSpyRetryHandler.hasIpv4(mList));
        assertEquals(true, mSpyRetryHandler.hasIpv6(mList));
    }

    @Test
    public void test005_clearAddresses() {
        // given
        Inet6Address mInet6Address = mock(Inet6Address.class);
        Inet4Address mInet4Address = mock(Inet4Address.class);

        // when
        mSpyRetryHandler.mServerAddresses.add(mInet6Address);
        mSpyRetryHandler.mServerAddresses.add(mInet4Address);
        mSpyRetryHandler.clearAddresses();

        // then
        assertEquals(0, mSpyRetryHandler.mServerAddresses.size());
    }

    @Test
    public void test006_removeAddress() {
        // given
        Inet6Address mInet6Address = mock(Inet6Address.class);
        Inet4Address mInet4Address = mock(Inet4Address.class);

        // when
        mSpyRetryHandler.mServerAddresses.add(mInet6Address);
        mSpyRetryHandler.mServerAddresses.add(mInet4Address);
        mSpyRetryHandler.removeAddress(TunnelError.ERR_CERTIFICATE_NO_ISSUER, mInet6Address);

        // then
        assertEquals(1, mSpyRetryHandler.mServerAddresses.size());
    }

    @Test
    public void test007_getNextAddress_empty() throws UnknownHostException {
        //Given
        mRetryHandler.mServerNames =
                new LinkedList<String>(Arrays.asList("epdg.epc.mnc088.mcc888.pub.3gppnetwork.org"));
        when(mMockConnectivityManager.getActiveNetwork()).thenReturn(mMockNetwork);
        when(mMockNetwork.getAllByName(mRetryHandler.mServerNames.getFirst()))
                .thenReturn(new InetAddress[] {InetAddress.getByName(TEST_IPV4_ADDRESS_1)});
        ArrayList<InetAddress> localAddresses =
                new ArrayList<InetAddress>();
        localAddresses.add(InetAddress.getByName(TEST_IPV4_ADDRESS_1));

        //When
        InetAddress inetAddress = mRetryHandler.getNextAddress(localAddresses);

        //Then
        assertEquals(mRetryHandler.mServerAddresses.getFirst(), inetAddress);
    }
    @Test
    public void test008_getNextAddress_ipv4() throws UnknownHostException {
        //Given
        ArrayList<InetAddress> localAddresses =
                new ArrayList<InetAddress>();
        localAddresses.add(InetAddress.getByName(TEST_IPV4_ADDRESS_1));
        mRetryHandler.mServerAddresses.add(InetAddress.getByName(TEST_IPV4_ADDRESS_2));

        //When
        InetAddress inetAddress = mRetryHandler.getNextAddress(localAddresses);

        //Then
        assertEquals(mRetryHandler.mServerAddresses.getFirst(), inetAddress);
    }

    @Test
    public void test009_getNextAddress_ipv6() throws UnknownHostException {
        //Given
        ArrayList<InetAddress> localAddresses =
                new ArrayList<InetAddress>();
        localAddresses.add(InetAddress.getByName(TEST_IPV6_ADDRESS_1));
        mRetryHandler.mServerAddresses.add(InetAddress.getByName(TEST_IPV6_ADDRESS_2));

        //When
        InetAddress inetAddress = mRetryHandler.getNextAddress(localAddresses);

        //Then
        assertEquals(mRetryHandler.mServerAddresses.getFirst(), inetAddress);
    }

    @Test
    public void test013_resolveServer_valid() throws UnknownHostException {
        //Given
        mRetryHandler.mServerNames =
                new LinkedList<String>(Arrays.asList("epdg.epc.mnc088.mcc888.pub.3gppnetwork.org"));
        when(mMockConnectivityManager.getActiveNetwork()).thenReturn(mMockNetwork);
        when(mMockNetwork.getAllByName(mRetryHandler.mServerNames.getFirst()))
                .thenReturn(new InetAddress[] {InetAddress.getByName(TEST_IPV4_ADDRESS_1)});
        //When
        mRetryHandler.resolveServer();

        //Then
        assertNotEquals(0, mRetryHandler.mServerAddresses.size());
    }

    @Test
    public void test014_getAddress_valid() throws UnknownHostException {
        //Given
        ArrayList<InetAddress> localAddresses =
                new ArrayList<InetAddress>();
        localAddresses.add(InetAddress.getByName(TEST_IPV4_ADDRESS_1));
        mRetryHandler.mServerAddresses.add(InetAddress.getByName(TEST_IPV4_ADDRESS_2));

        //When
        InetAddress inetAddress = mRetryHandler.getAddress(localAddresses);

        //Then
        assertEquals(mRetryHandler.mServerAddresses.getFirst(), inetAddress);
    }

    @Test
    public void test015_getAddress_invalid() {
        //Given
        ArrayList<InetAddress> localAddresses = new ArrayList<>();

        //When
        InetAddress inetAddress = mRetryHandler.getAddress(localAddresses);

        //Then
        assertNull(inetAddress);
    }

    @Test
    public void test016_getLastError() {
        assertEquals(mRetryHandler.getLastError(), TunnelError.ERR_SUCCESS);
    }

    @Test
    public void test017_onWiFiDisconnected() {
        mRetryHandler.onWiFiDisconnected();
        assertTrue(mRetryHandler.mServerAddresses.isEmpty());
    }

    @Test
    public void test018_onAirplaneModeEnabled() {
        mRetryHandler.onAirplaneModeEnabled();
        assertTrue(mRetryHandler.mPcoServerAddresses.isEmpty());
    }

    @Test
    public void test019_getRetryTimeout_1() {
        mRetryHandler.mLastError = TunnelError.ERR_INTERNAL_ADDR_FAILED;
        mMockConfigurationParameters.mDefaultRetryTimeout = "36:180";
        mRetryHandler.buildDefaultRetryTimerMap();
        assertEquals(180000, mRetryHandler.getRetryTimeout(true));

        mRetryHandler.mLastError = TunnelError.ERR_SUCCESS;
        mMockConfigurationParameters.mDefaultRetryTimeout = "";
        when(DataFailCause.isPermanentFailure(eq(mMockContext), anyInt(), anyInt())).thenReturn(true);
        assertEquals(DEFAULT_RETRY_VALUE, mRetryHandler.getRetryTimeout(true));

        when(DataFailCause.isPermanentFailure(eq(mMockContext), anyInt(), anyInt())).thenReturn(false);
        assertEquals(DataCallResponse.RETRY_DURATION_UNDEFINED, mRetryHandler.getRetryTimeout(false));

        mRetryHandler.mRetryForever = false;
        mRetryHandler.mRetryCount = 3;
        mRetryHandler.mMaxRetryCount = 3;
        assertEquals(DataCallResponse.RETRY_DURATION_UNDEFINED, mRetryHandler.getRetryTimeout(true));
    }

    @Test
    public void test020_backoffTimer() {
        mRetryHandler.mBackoffTimerTask = null;
        mRetryHandler.startBackoffTimer(DEFAULT_RETRY_VALUE);
        assertTrue(mRetryHandler.isBackoffTimerActive());

        mRetryHandler.stopBackoffTimer();

        mRetryHandler.mBackoffTimerTask = mRetryHandler.new BackoffTimerTask();
        mRetryHandler.mBackoffTimerTask.run();

        assertFalse(mRetryHandler.isBackoffTimerActive());
    }

    @Test
    public void test021_onCarrierConfigChanged() {
        PersistableBundle mockBundle = mock(PersistableBundle.class);

        when(mMockCarrierConfigManager.getConfigForSubId(anyInt())).thenReturn(mockBundle);
        when(CarrierConfigManager.isConfigForIdentifiedCarrier(mockBundle)).thenReturn(true);
        when(mockBundle.getStringArray(CarrierConfigManager.KEY_CARRIER_DATA_CALL_RETRY_CONFIG_STRINGS))
                .thenReturn(new String[]{
                        "default:default_randomization=2000,5000,10000,20000,40000,80000:5000,"
                                + "160000:5000,320000:5000,640000:5000,1280000:5000,1800000:5000",
                        "mms:default_randomization=2000,5000,10000,20000,40000,80000:5000,"
                                + "160000:5000,320000:5000,640000:5000,1280000:5000,1800000:5000",
                        "ims:max_retries=10, 5000, 5000, 5000",
                        "others:max_retries=3, 5000, 5000, 5000"});

        mRetryHandler.onCarrierConfigChanged();

        assertEquals(mRetryHandler.mMaxRetryCount, 10);

        when(mockBundle.getStringArray(CarrierConfigManager.KEY_CARRIER_DATA_CALL_RETRY_CONFIG_STRINGS))
                .thenReturn(new String[]{
                        "default:default_randomization=2000,5000,10000,20000,40000,80000:5000,"
                                + "160000:5000,320000:5000,640000:5000,1280000:5000,1800000:5000",
                        "ims:default_randomization=2000,5000,10000,20000,40000,80000:5000,"
                                + "160000:5000,320000:5000,640000:5000,1280000:5000,1800000:5000",
                        "ims:max_retries=5, 5000, 5000, 5000",
                        "others:max_retries=3, 5000, 5000, 5000"});

        mRetryHandler.onCarrierConfigChanged();

        assertEquals(mRetryHandler.mMaxRetryCount, 13);

    }

    @Test
    public void test022_getNextRetryTimeout() {
        assertEquals(DataCallResponse.RETRY_DURATION_UNDEFINED, mRetryHandler.getNextRetryTimeout(false));

        mRetryHandler.reset();
        test021_onCarrierConfigChanged();
        assertNotEquals(DataCallResponse.RETRY_DURATION_UNDEFINED, mRetryHandler.getNextRetryTimeout(true));
    }

    @Test
    public void test023_createFailureResponse() {
        assertNotEquals(DataCallResponse.RETRY_DURATION_UNDEFINED,
                mRetryHandler.createFailureResponse(true));
        assertNotEquals(DataCallResponse.RETRY_DURATION_UNDEFINED,
                mRetryHandler.createFailureResponse(mRetryHandler.mLastError.toDataFailCause(),
                        ApnSetting.PROTOCOL_IP, true));

        assertNotEquals(DataCallResponse.RETRY_DURATION_UNDEFINED,
                mRetryHandler.createFailureResponse(mRetryHandler.mLastError.toDataFailCause(),
                        ApnSetting.PROTOCOL_IP, true));

        mRetryHandler.startBackoffTimer(DEFAULT_RETRY_VALUE);
        assertNotEquals(DataCallResponse.RETRY_DURATION_UNDEFINED,
                mRetryHandler.createFailureResponse(mRetryHandler.mLastError.toDataFailCause(),
                        5, 1, ApnSetting.PROTOCOL_IP, true));
    }

    @Test
    public void test024_buildServersBlackList() {
        mMockConfigurationParameters.mServersBlacklist = TEST_IPV4_ADDRESS_1;
        mRetryHandler.buildServersBlackList();
    }

    @Test
    public void test025_destroy() {
        mRetryHandler.destroy();
        assertTrue(mRetryHandler.mServerAddresses.isEmpty());
    }
}