package com.shannon.networkservice;

import android.content.Context;
import android.net.LinkProperties;
import android.net.Network;
import android.net.NetworkCapabilities;
import android.telephony.AccessNetworkConstants;
import android.telephony.NetworkRegistrationInfo;
import android.telephony.NetworkService;
import android.telephony.NetworkServiceCallback;
import android.telephony.TelephonyManager;

import androidx.test.platform.app.InstrumentationRegistry;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.MockitoAnnotations;
import org.mockito.MockitoSession;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.annotation.Config;

import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;

import static org.junit.Assert.*;
import static org.mockito.ArgumentMatchers.any;
import static org.mockito.ArgumentMatchers.eq;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.mockitoSession;
import static org.mockito.Mockito.spy;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

import com.shannon.networkservice.ShannonNetworkService.ShannonNetworkServiceProvider;


@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class ShannonNetworkServiceTest {
    private static final int testSlotId = 0;
    private static final int testNetId = 100;

    MockitoSession mStaticMockSession;

    private ShannonNetworkService mService;
    private ShannonNetworkService mSpyService;
    private ShannonNetworkService.ShannonNetworkServiceProvider mProvider;
    private ShannonNetworkService.ShannonNetworkServiceProvider.ShannonNetworkProviderCallback callback;
    private Context mContext;

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.openMocks(this);
        mStaticMockSession = mockitoSession().startMocking();

        mContext = InstrumentationRegistry.getInstrumentation().getContext();

        mService = new ShannonNetworkService();
        mSpyService = spy(new ShannonNetworkService());
        mService.onCreateNetworkServiceProvider(mContext, testSlotId);
        mProvider = ShannonNetworkService.getProviders().get(testSlotId);
        callback = mProvider.getRequestCallback();
    }

    @After
    public void tearDown() throws Exception {
        mStaticMockSession.finishMocking();
        mSpyService.onDestroy();
    }

    @Test
    public void testOnNetworkRegistrationInfo_PS() {
        NetworkCapabilities nc = buildNetworkCapabilities();
        NetworkServiceCallback networkCallback = mock(NetworkServiceCallback.class);
        callback.onAvailable(
                new Network(testNetId),
                nc,
                mock(LinkProperties.class),
                false
        );
        nc.setCapability(NetworkCapabilities.NET_CAPABILITY_VALIDATED, true);
        callback.onCapabilitiesChanged(new Network(testNetId), nc);

        mProvider.requestNetworkRegistrationInfo(NetworkRegistrationInfo.DOMAIN_PS, networkCallback);
        List<Integer> resultArr = new ArrayList<>();
        resultArr.add(NetworkRegistrationInfo.SERVICE_TYPE_DATA);

        verify(networkCallback, times(1)).onRequestNetworkRegistrationInfoComplete(NetworkServiceCallback.RESULT_SUCCESS, new NetworkRegistrationInfo.Builder()
                .setDomain(NetworkRegistrationInfo.DOMAIN_PS)
                .setTransportType(AccessNetworkConstants.TRANSPORT_TYPE_WLAN)
                .setRegistrationState(NetworkRegistrationInfo.REGISTRATION_STATE_HOME)
                .setAccessNetworkTechnology(TelephonyManager.NETWORK_TYPE_IWLAN)
                .setRejectCause(0)
                .setEmergencyOnly(false)
                .setAvailableServices(resultArr)
                .setCellIdentity(null)
                .build()
        );
    }

    @Test
    public void testGetConfiguration() {
        ConfigurationManager mConfig = mProvider.getConfigurationManager();

        assertNotNull(mConfig);
    }

    @Test
    public void testOnClose() {
        mProvider.close();

        assertEquals(mService.getProviders().size(), 0);
    }

    @Test
    public void testOnNetworkSuspended() {
        NetworkCapabilities nc = buildNetworkCapabilities();
        callback.onAvailable(
                new Network(testNetId),
                nc,
                mock(LinkProperties.class),
                false
        );

        callback.onNetworkSuspended(new Network(testNetId));

        assertEquals(false, mProvider.getShannonNetworkInfo().mIsSuspended);
    }

    @Test
    public void testIsVoMobilePossible() {
        assertEquals(mProvider.isVoMobilePossible(), false);
    }

    @Test
    public void testIsVoMobileEnable() {
        assertEquals(mProvider.isVoMobileEnabled(), false);
    }

    @Test
    public void testOnResumed() {
        NetworkCapabilities nc = buildNetworkCapabilities();
        callback.onAvailable(
                new Network(testNetId),
                nc,
                mock(LinkProperties.class),
                false
        );

        callback.onNetworkResumed(new Network(testNetId));

        assertEquals(mProvider.getShannonNetworkInfo().mIsSuspended, false);
    }

    @Test
    public void testOnBlockedStatusChanged() {
        NetworkCapabilities nc = buildNetworkCapabilities();
        callback.onAvailable(
                new Network(testNetId),
                nc,
                mock(LinkProperties.class),
                false
        );

        callback.onBlockedStatusChanged(new Network(testNetId), false);

        assertEquals(mProvider.getShannonNetworkInfo().mIsBlocked, false);
    }

    @Test
    public void testOnAvailable() {
        NetworkCapabilities nc = buildNetworkCapabilities();

        callback.onAvailable(
                new Network(testNetId),
                nc,
                mock(LinkProperties.class),
                false
        );

        assertEquals(mProvider.getShannonNetworkInfo().isNetworkReady(), true);
    }

    @Test
    public void testOnLinkPropertiesChanged() {
        NetworkCapabilities nc = buildNetworkCapabilities();
        LinkProperties mOldLinkProperties = mock(LinkProperties.class);
        LinkProperties mNewLinkProperties = mock(LinkProperties.class);
        callback.onAvailable(
                new Network(testNetId),
                nc,
                mOldLinkProperties,
                false
        );

        callback.onLinkPropertiesChanged(new Network(testNetId), mNewLinkProperties);

        assertEquals(mProvider.getShannonNetworkInfo().mLinkProperties, mNewLinkProperties);
    }

    @Test
    public void testIsVoMobile() {
        assertEquals(mProvider.isVoMobileEnabled(), false);
    }

    @Test
    public void testOnLost() {
        NetworkCapabilities nc = buildNetworkCapabilities();

        callback.onAvailable(
                new Network(testNetId),
                nc,
                mock(LinkProperties.class),
                false
        );

        callback.onLost(new Network(testNetId));

        assertEquals(mProvider.getShannonNetworkInfo(), null);
    }

    public static NetworkCapabilities buildNetworkCapabilities() {
        NetworkCapabilities nc = new NetworkCapabilities();
        nc.setTransportType(1,true);
        nc.setLinkUpstreamBandwidthKbps(37000);
        nc.setLinkDownstreamBandwidthKbps(37000);
        nc.setSignalStrength(-49);
        nc.setRequestorUid(-1);
        nc.setCapability(NetworkCapabilities.NET_CAPABILITY_NOT_RESTRICTED, true);
        nc.setCapability(NetworkCapabilities.NET_CAPABILITY_NOT_METERED, true);
        nc.setCapability(NetworkCapabilities.NET_CAPABILITY_INTERNET, true);
        nc.setCapability(NetworkCapabilities.NET_CAPABILITY_NOT_RESTRICTED, true);
        nc.setCapability(NetworkCapabilities.NET_CAPABILITY_TRUSTED, true);
        nc.setCapability(NetworkCapabilities.NET_CAPABILITY_NOT_VPN, true);
        nc.setCapability(NetworkCapabilities.NET_CAPABILITY_NOT_ROAMING, true);
        nc.setCapability(NetworkCapabilities.NET_CAPABILITY_FOREGROUND, true);
        nc.setCapability(NetworkCapabilities.NET_CAPABILITY_NOT_CONGESTED, true);
        nc.setCapability(NetworkCapabilities.NET_CAPABILITY_NOT_SUSPENDED, true);
        return nc;
    }
}
