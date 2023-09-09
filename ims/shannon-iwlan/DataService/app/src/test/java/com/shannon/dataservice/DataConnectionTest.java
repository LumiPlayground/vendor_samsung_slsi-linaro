package com.shannon.dataservice;

import android.content.Context;
import android.net.ConnectivityManager;
import android.net.IpSecManager;
import android.net.IpSecManager.ResourceUnavailableException;
import android.net.LinkAddress;
import android.net.LinkProperties;
import android.net.Network;
import android.net.SocketKeepalive;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.Message;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
import android.telephony.data.ApnSetting;
import android.telephony.data.DataCallResponse;
import android.telephony.data.DataProfile;
import android.telephony.data.NetworkSliceInfo;

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

import static android.telephony.data.ApnSetting.PROTOCOL_IPV6;
import static android.telephony.data.DataService.REQUEST_REASON_HANDOVER;
import static android.telephony.data.DataService.REQUEST_REASON_NORMAL;
import static com.shannon.dataservice.RetryHandler.DEFAULT_RETRY_VALUE;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertNull;
import static org.mockito.ArgumentMatchers.any;
import static org.mockito.ArgumentMatchers.eq;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.mockStatic;
import static org.mockito.Mockito.mockitoSession;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

import com.shannon.dataservice.DataConnection.ConnectionCallback;
import com.shannon.dataservice.StrokeTerminatedIndication.Cause;

import java.io.IOException;
import java.net.InetAddress;
import java.net.UnknownHostException;
import java.util.ArrayList;

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class DataConnectionTest {
    private static final int testSlotId = 0;
    private static final int testSubId = 1;
    private static final int testMtu = 1380;
    private static final int testNetId = 100;
    private static final ApnType testApnType = ApnType.APN_IMS;
    private static final String TEST_IPV4_ADDRESS_1 = "127.0.0.1";
    private static InetAddress TEST_INET_ADDRV4;
    private static LinkAddress TEST_LINK_ADDRV4;

    private static MockedStatic<SimManager> mStaticSimManager;
    private static MockedStatic<IkeDaemonInterface> mStaticIkeDaemonInterface;

    MockitoSession mStaticMockSession;
    private DataConnection mDataConnection;

    @Mock private Context mMockContext;
    @Mock private ConfigurationManager mMockConfigurationManager;
    @Mock private ConfigurationParameters mMockConfigurationParameters;
    @Mock private ConnectionCallback mMockConnectionCallback;
    @Mock private ConnectivityManager mMockConnectivityManager;
    @Mock private IkeDaemonInterface mMockIkeDaemonInterface;
    @Mock private IpSecManager mMockIpSecManager;
    @Mock private LinkProperties mMockLinkProperties;
    @Mock private Network mMockNetwork;
    @Mock private RetryHandler mMockRetryHandler;
    @Mock private RetryManager mMockRetryManager;
    @Mock private RilOemInterface mMockRilInterface;
    @Mock private SimInformation mMockSimInformation;
    @Mock private SimManager mMockSimManager;
    @Mock private SubscriptionManager mMockSubscriptionManager;
    @Mock private TelephonyManager mMockTelephonyManager;
    @Mock private DataProfile mMockDataProfile;
    @Mock private WifiInfo mMockWifiInfo;
    @Mock private WifiManager mMockWifiManager;
    @Mock private NetworkSliceInfo mMockNetworkSliceInfo;

    @BeforeClass
    public static void beforeClass() {
        mStaticSimManager = mockStatic(SimManager.class);
        mStaticIkeDaemonInterface = mockStatic(IkeDaemonInterface.class);
    }

    @AfterClass
    public static void afterClass() {
        mStaticSimManager.close();
        mStaticIkeDaemonInterface.close();
    }

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.openMocks(this);
        mStaticMockSession = mockitoSession().startMocking();

        TEST_INET_ADDRV4 = InetAddress.getByName(TEST_IPV4_ADDRESS_1);
        TEST_LINK_ADDRV4 = new LinkAddress(TEST_INET_ADDRV4, 32);

        when(SimManager.getInstance()).thenReturn(mMockSimManager);
        when(IkeDaemonInterface.getInstance()).thenReturn(mMockIkeDaemonInterface);

        when(mMockContext.getSystemService(eq(ConnectivityManager.class)))
                .thenReturn(mMockConnectivityManager);
        when(mMockContext.getSystemService(IpSecManager.class)).thenReturn(mMockIpSecManager);
        when(mMockContext.getSystemService(eq(SubscriptionManager.class)))
                .thenReturn(mMockSubscriptionManager);
        when(mMockContext.getSystemService(eq(TelephonyManager.class)))
                .thenReturn(mMockTelephonyManager);
        when(mMockTelephonyManager.createForSubscriptionId(eq(testSubId)))
                .thenReturn(mMockTelephonyManager);
        when(mMockDataProfile.getApn()).thenReturn(ApnType.APN_IMS.getUserFriendlyName());
        when(mMockDataProfile.getSupportedApnTypesBitmask()).thenReturn(ApnSetting.TYPE_IMS);
        when(mMockRetryManager.getApnRetryHandler(testApnType, mMockDataProfile.getApn())).thenReturn(mMockRetryHandler);
        when(mMockConfigurationManager.getConnectionConfiguration()).thenReturn(mMockConfigurationParameters);
        when(mMockSimManager.getSimInformation(eq(testSlotId))).thenReturn(mMockSimInformation);
        when(mMockSimInformation.getTelephonyManager()).thenReturn(mMockTelephonyManager);

        mDataConnection = new DataConnection(
                        testSlotId,
                        mMockContext,
                        ApnType.APN_IMS,
                        mMockDataProfile,
                        REQUEST_REASON_NORMAL,
                        mock(LinkProperties.class),
                        0,
                        mMockNetworkSliceInfo,
                        true,
                        mMockRilInterface,
                        mMockConfigurationManager,
                        mMockRetryManager,
                        mMockConnectionCallback
                );
    }

    @After
    public void tearDown() throws Exception {
        mStaticMockSession.finishMocking();
    }

    private void activate() throws UnknownHostException {
        ArrayList<InetAddress> arrayList = new ArrayList<>();
        InetAddress testInetAddress = InetAddress.getByName(TEST_IPV4_ADDRESS_1);
        arrayList.add(testInetAddress);
        mMockLinkProperties.addDnsServer(TEST_INET_ADDRV4);
        mMockLinkProperties.addLinkAddress(TEST_LINK_ADDRV4);
        ArrayList<LinkAddress> linkAddresses = new ArrayList<>();
        linkAddresses.add(TEST_LINK_ADDRV4);

        when(mMockLinkProperties.getAddresses()).thenReturn(arrayList);
        when(mMockLinkProperties.getLinkAddresses()).thenReturn(linkAddresses);
        when(mMockRetryHandler.getAddress(any())).thenReturn(testInetAddress);
        when(mMockIkeDaemonInterface.isIkeDaemonStarted()).thenReturn(true);
        mDataConnection.activate(mMockNetwork, mMockLinkProperties, new Message());
    }

    @Test
    public void test001_activate() throws UnknownHostException {
        //When
        activate();

        //Then
        assertNotNull(mDataConnection.getRequestMessage());
    }

    @Test
    public void test002_activate_network_null() {
        //When
        mDataConnection.activate(new Network(
                        testNetId),
                null,
                new Message()
        );

        //Then
        verify(mMockSimManager, times(1)).getMobileNetwork(testSlotId, mMockConfigurationManager);
    }

    @Test
    public void test004_destroyEstablished() throws UnknownHostException {
        //Without activation
        mDataConnection.destroyEstablished();

        //Wifi activation
        activate();
        mDataConnection.destroyEstablished();

        //Then
        verify(mMockIkeDaemonInterface, times(1)).deleteTunnel(any());
    }

    @Test
    public void test005_destroyPending() throws UnknownHostException {
        //When
        activate();
        mDataConnection.destroyPending();

        //Then
        verify(mMockIkeDaemonInterface, times(1)).deleteTunnel(any());
    }

    @Test
    public void test006_closePending() throws UnknownHostException {
        //When
        activate();
        mDataConnection.closePending();

        //Then
        verify(mMockIkeDaemonInterface, times(1)).closeTunnel(any());
    }

    @Test
    public void test007_getDataCallResponse() throws NoSuchFieldError {
        ////Fail - default
        //When
        mDataConnection.setLinkProperties(mMockLinkProperties);
        DataCallResponse resp = mDataConnection.getDataCallResponse();

        int linkStatus = resp.getLinkStatus();
        //Then
        assertEquals(linkStatus, DataCallResponse.LINK_STATUS_ACTIVE);

        ////Success
        //Given
        ArrayList<LinkAddress> linkAddresses = new ArrayList<>();
        ArrayList<InetAddress> listInetAddress = new ArrayList<>();

        //When
        when(mMockLinkProperties.getInterfaceName()).thenReturn("ims");
        when(mMockLinkProperties.getLinkAddresses()).thenReturn(linkAddresses);
        when(mMockLinkProperties.getDnsServers()).thenReturn(listInetAddress);
        when(mMockLinkProperties.getPcscfServers()).thenReturn(listInetAddress);
        when(mMockLinkProperties.getMtu()).thenReturn(testMtu);
        mDataConnection.setLinkProperties(mMockLinkProperties);
        resp = mDataConnection.getDataCallResponse();
        linkStatus = resp.getLinkStatus();

        //Then
       assertEquals(linkStatus, DataCallResponse.LINK_STATUS_ACTIVE);
    }

    @Test
    public void test008_updateNetwork() throws IOException, ResourceUnavailableException {
        activate();
        Network testNetwork = new Network(456);
        mMockConfigurationParameters.mUseOffloadedKeepalive = true;
        mDataConnection.setKeepAliveInterval(20);
        mDataConnection.updateNetwork(testNetwork);

        verify(mMockIpSecManager, times(1)).openUdpEncapsulationSocket(4500);
    }

    @Test
    public void test011_deactivate() throws UnknownHostException {
        activate();
        mDataConnection.deactivate(new Message());
        verify(mMockIkeDaemonInterface, times(1)).closeTunnel(any());
    }

    @Test
    public void test012_updateLocalAddresses() {
        // given
        mMockConfigurationParameters.mUseMacForNai = true;
        mMockLinkProperties = new LinkProperties();
        mMockLinkProperties.addLinkAddress(TEST_LINK_ADDRV4);

        // when
        mDataConnection.updateLocalAddresses(mMockLinkProperties);
    }

    @Test
    public void test013_daemonCallback() throws UnknownHostException {
        //Given
        DataConnection.DaemonCallback daemonCallback = mDataConnection.getDaemonCallback();
        activate();

        //onConnected
        daemonCallback.onConnected("ims0", "wlan0", testMtu,
                null, new byte[10], new byte[10]);
        assertNotNull(mDataConnection.getRequestMessage());

        //onConnectionError
        when(mMockRetryHandler.getNextAddress(any())).thenReturn(TEST_INET_ADDRV4);
        daemonCallback.onConnectionError("ims0",
                TunnelError.ERR_NETWORK_AUTH_FAILED, new byte[10]);
        assertNotNull(mDataConnection.getRequestMessage());

        //onTerminated
        daemonCallback.onTerminated("ims0", Cause.CAUSE_NORMAL);
        assertNull(mDataConnection.getRequestMessage());

        //onAuthenticationRequest
        byte[] bytes = new byte[10];
        daemonCallback.onAuthenticationRequest(bytes);
        verify(mMockRilInterface, times(1)).getAuthenticationResponse(bytes, true);

        //onDisconnected
        daemonCallback.onDisconnected("ims0");
        assertNull(mDataConnection.getRequestMessage());
    }

    @Test
    public void test014_keepAliveCallback() {
        //Just invoke callbacks
        DataConnection.KeepAliveCallback keepAliveCallback = mDataConnection.new KeepAliveCallback();
        keepAliveCallback.onError(SocketKeepalive.SUCCESS);
        keepAliveCallback.onError(SocketKeepalive.NO_KEEPALIVE);
        keepAliveCallback.onError(SocketKeepalive.DATA_RECEIVED);
        keepAliveCallback.onError(SocketKeepalive.BINDER_DIED);
        keepAliveCallback.onError(SocketKeepalive.ERROR_INVALID_NETWORK);
        keepAliveCallback.onError(SocketKeepalive.ERROR_INVALID_IP_ADDRESS);
        keepAliveCallback.onError(SocketKeepalive.ERROR_INVALID_PORT);
        keepAliveCallback.onError(SocketKeepalive.ERROR_INVALID_LENGTH);
        keepAliveCallback.onError(SocketKeepalive.ERROR_INVALID_INTERVAL);
        keepAliveCallback.onError(SocketKeepalive.ERROR_INVALID_SOCKET);
        keepAliveCallback.onError(SocketKeepalive.ERROR_SOCKET_NOT_IDLE);
        keepAliveCallback.onError(SocketKeepalive.ERROR_UNSUPPORTED);
        keepAliveCallback.onError(SocketKeepalive.ERROR_HARDWARE_ERROR);
        keepAliveCallback.onError(SocketKeepalive.ERROR_INSUFFICIENT_RESOURCES);
        keepAliveCallback.onStarted();
        keepAliveCallback.onStopped();
    }
}

