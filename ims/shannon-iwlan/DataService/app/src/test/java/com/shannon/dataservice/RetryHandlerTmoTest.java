package com.shannon.dataservice;

import static com.shannon.dataservice.RetryHandler.DEFAULT_RETRY_VALUE;
import static org.junit.Assert.assertEquals;
import static org.mockito.ArgumentMatchers.any;
import static org.mockito.ArgumentMatchers.anyInt;
import static org.mockito.ArgumentMatchers.anyString;
import static org.mockito.ArgumentMatchers.eq;
import static org.mockito.Mockito.doReturn;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.mockStatic;
import static org.mockito.Mockito.spy;
import static org.mockito.Mockito.when;

import android.content.Context;
import android.net.ConnectivityManager;
import android.net.Network;
import android.os.Message;
import android.telephony.DataFailCause;
import android.telephony.TelephonyManager;
import android.telephony.data.DataCallResponse;

import junit.framework.TestCase;

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
import java.util.LinkedList;

import androidx.test.platform.app.InstrumentationRegistry;

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class RetryHandlerTmoTest extends TestCase {
    private static int TEST_SLOT_ID = 0;
    private static ApnType TEST_APN_IMS = ApnType.APN_IMS;
    private static final String TEST_RETRY_PARAMETERS = "name=TMO, N_fqdn=2, N_fast_try=3, " +
                                                        "N_max=6, T_fast=0, T_start=300";
    private static final String TEST_IPV4_ADDRESS_1 = "127.0.0.1";
    private static final String TEST_IPV4_ADDRESS_2 = "127.0.0.2";
    private static final String TEST_IPV6_ADDRESS_1 = "0000:0000:0000:0000:0000:0000:0000:0001";
    private static final String TEST_IPV6_ADDRESS_2 = "0000:0000:0000:0000:0000:0000:0000:0002";

    private static MockedStatic<SimManager> mStaticSimManager;
    private static MockedStatic<DataFailCause> mStaticDataFailCause;

    private AutoCloseable closeable;
    private RetryHandlerTmo mRetryHandlerTmo;
    private RetryHandlerTmo mSpyRetryHandlerTmo;

    @Mock private ConfigurationManager mMockConfigurationManager;
    @Mock private ConfigurationParameters mMockConfigurationParameters;
    @Mock private ConnectivityManager mMockConnectivityManager;
    @Mock private Context mMockContext;
    @Mock private DnsResolver mMockResolver;
    @Mock private Network mMockNetwork;
    @Mock private SimInformation mMockSimInformation;
    @Mock private SimManager mMockSimManager;
    @Mock private TelephonyManager mMockTelephonyManager;
    @Mock private Message mMockMessage;

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
        closeable = MockitoAnnotations.openMocks(this);

        when(mMockConfigurationManager.getRetryParameters()).thenReturn(TEST_RETRY_PARAMETERS);
        when(SimManager.getInstance()).thenReturn(mMockSimManager);
        when(mMockSimManager.getSimInformation(anyInt()))
                .thenReturn(mMockSimInformation);
        when(mMockSimInformation.getTelephonyManager()).thenReturn(mMockTelephonyManager);
        when(mMockConfigurationManager.getConnectionConfiguration())
                .thenReturn(mMockConfigurationParameters);

        // Set Conetext
        mMockContext = InstrumentationRegistry.getInstrumentation().getContext();


        mRetryHandlerTmo = new RetryHandlerTmo(TEST_SLOT_ID, TEST_APN_IMS,"ims-apn",
                mMockContext, mMockConfigurationManager, mMockResolver, mMockMessage);
        mSpyRetryHandlerTmo = spy(mRetryHandlerTmo);
    }

    @After
    public void tearDown() throws Exception {
        closeable.close();
    }

    @Test
    public void test001_getAddress() throws UnknownHostException {
        // Given
        ArrayList<InetAddress> localAddresses =
                new ArrayList<InetAddress>();
        localAddresses.add(InetAddress.getByName(TEST_IPV4_ADDRESS_1));
        mRetryHandlerTmo.mServerAddresses.add(InetAddress.getByName(TEST_IPV4_ADDRESS_2));

        // When
        InetAddress inetAddress = mRetryHandlerTmo.getAddress(localAddresses);

        // Then
        assertEquals(mRetryHandlerTmo.mServerAddresses.getFirst(), inetAddress);
    }

    @Test
    public void test002_clearAddresses() {
        // iven
        Inet6Address mInet6Address = mock(Inet6Address.class);
        Inet4Address mInet4Address = mock(Inet4Address.class);

        // When
        mSpyRetryHandlerTmo.mServerAddresses.add(mInet6Address);
        mSpyRetryHandlerTmo.mServerAddresses.add(mInet4Address);
        mSpyRetryHandlerTmo.clearAddresses();

        // Then
        assertEquals(0, mSpyRetryHandlerTmo.mServerAddresses.size());
    }

    @Test
    public void test003_getNextAddress_congestion() {
        // Given
        ArrayList<InetAddress> localAddresses = new ArrayList<InetAddress>();
        mRetryHandlerTmo.mLastError = TunnelError.ERR_CONGESTION;

        // When
        InetAddress ret = mRetryHandlerTmo.getNextAddress(localAddresses);

        // Then
        assertNull(ret);
    }

    @Test
    public void test004_getNextAddress_ipv4() throws UnknownHostException {
        // Given
        ArrayList<InetAddress> localAddresses = new ArrayList<InetAddress>();
        localAddresses.add(InetAddress.getByName(TEST_IPV4_ADDRESS_1));
        mRetryHandlerTmo.mServerAddresses.add(InetAddress.getByName(TEST_IPV4_ADDRESS_2));

        // When
        InetAddress inetAddress = mRetryHandlerTmo.getNextAddress(localAddresses);

        // Then
        assertEquals(mRetryHandlerTmo.mServerAddresses.getFirst(), inetAddress);
    }

    @Test
    public void test005_getNextAddress_ipv6() throws UnknownHostException {
        // Given
        ArrayList<InetAddress> localAddresses =
                new ArrayList<InetAddress>();
        localAddresses.add(InetAddress.getByName(TEST_IPV6_ADDRESS_1));
        mRetryHandlerTmo.mServerAddresses.add(InetAddress.getByName(TEST_IPV6_ADDRESS_2));

        // When
        InetAddress inetAddress = mRetryHandlerTmo.getNextAddress(localAddresses);

        // Then
        assertEquals(mRetryHandlerTmo.mServerAddresses.getFirst(), inetAddress);
    }

    @Test
    public void test006_getRetryTimeout() {
        mRetryHandlerTmo.mLastError = TunnelError.ERR_INTERNAL_ADDR_FAILED;
        mMockConfigurationParameters.mDefaultRetryTimeout = "36:180";
        mRetryHandlerTmo.buildDefaultRetryTimerMap();
        assertEquals(180000, mRetryHandlerTmo.getRetryTimeout(true));

        mRetryHandlerTmo.mLastError = TunnelError.ERR_CONGESTION;
        mMockConfigurationParameters.mDefaultRetryTimeout = "";
        assertEquals(0, mRetryHandlerTmo.getRetryTimeout(true));

        mRetryHandlerTmo.mLastError = TunnelError.ERR_SUCCESS;
        when(DataFailCause.isPermanentFailure(eq(mMockContext), anyInt(), anyInt())).thenReturn(true);
        assertEquals(DEFAULT_RETRY_VALUE, mRetryHandlerTmo.getRetryTimeout(false));

        when(DataFailCause.isPermanentFailure(eq(mMockContext), anyInt(), anyInt())).thenReturn(false);
        assertEquals(DataCallResponse.RETRY_DURATION_UNDEFINED, mRetryHandlerTmo.getRetryTimeout(false));
    }
}