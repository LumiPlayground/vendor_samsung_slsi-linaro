package com.shannon.dataservice;

import android.content.Context;
import android.os.SystemProperties;

import androidx.test.platform.app.InstrumentationRegistry;

import com.shannon.dataservice.StrokeTerminatedIndication.Cause;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.Mock;
import org.mockito.MockedConstruction;
import org.mockito.Mockito;
import org.mockito.MockitoAnnotations;
import org.mockito.MockitoSession;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.annotation.Config;

import java.net.InetAddress;
import java.net.UnknownHostException;
import java.util.HashMap;

import static com.shannon.dataservice.StrokeResponseMessageTest.TEST_BYTES_IND_SIM_AUTH;
import static com.shannon.dataservice.StrokeResponseMessageTest.TEST_BYTES_IND_TERMINATED;
import static org.junit.Assert.*;
import static org.mockito.ArgumentMatchers.any;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.mockitoSession;
import static org.mockito.Mockito.spy;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class IkeDaemonInterfaceTest {
    private static final String DAEMON_EXEC = "vendor.charon.exec";

    private static final String testConnectionName = "ims1";
    private static final String testAdapterName = "epdg0";

    @Mock private Context mContext;

    /* target class */
    private IkeDaemonInterface mIkeDaemonInterface;
    private IkeDaemonInterface.DataReceiver dataReceiver;

    @Before
    public void setUp() throws Exception {
        System.out.println("IkeDaemonInterfaceTest");
        MockitoAnnotations.openMocks(this);

        mIkeDaemonInterface = new IkeDaemonInterface(mContext);
        dataReceiver = mIkeDaemonInterface.new DataReceiver();
    }

    @After
    public void tearDown() throws Exception {

    }

    @Test
    public void openTunnel() {
        // given
        ConfigurationParameters configurationParameters = new ConfigurationParameters();
        ConnectionParameters parameters = new ConnectionParameters(configurationParameters);
        parameters.mName = testConnectionName;

        // when
        mIkeDaemonInterface.openTunnel(parameters, this.makeTestTunnelConnectionCallback());

        // then
        assertEquals(false, mIkeDaemonInterface.isIkeDaemonStarted());
        assertEquals(21, mIkeDaemonInterface.getRequestId());
    }

    @Test
    public void closeTunnel() {
        this.openTunnel();
        mIkeDaemonInterface.closeTunnel(testConnectionName);
        assertEquals(21, mIkeDaemonInterface.getRequestId());
    }

    @Test
    public void deleteTunnel() {
        this.openTunnel();
        mIkeDaemonInterface.deleteTunnel(testConnectionName);
        assertEquals(21, mIkeDaemonInterface.getRequestId());
    }

    @Test
    public void testStartAndStopIkeDaemon() {
        mIkeDaemonInterface.startIkeDaemon();

        assertEquals(false, mIkeDaemonInterface.isIkeDaemonStarted());
        assertEquals(SystemProperties.get(DAEMON_EXEC), "1");

        mIkeDaemonInterface.stopIkeDaemon();

        assertEquals( false, mIkeDaemonInterface.isIkeDaemonStarted());
        assertEquals(SystemProperties.get(DAEMON_EXEC), "0");
        assertEquals(false, mIkeDaemonInterface.isIkeDaemonStarted());
    }

    @Test
    public void getInstance() {
        assertNotNull(IkeDaemonInterface.getInstance());
    }

    @Test
    public void destroy() {
        // when
        mIkeDaemonInterface.destroy();

        // then
        assertEquals(false, mIkeDaemonInterface.getIsDaemonStarted());
    }

    @Test
    public void createRoute() {
        // given
        byte[] ipAddr1 = new byte[] {127, 0, 0, 1};
        byte[] ipAddr2 = new byte[] {10, 10, 10, 2};

        // when
        ConfigurationParameters configurationParameters = new ConfigurationParameters();
        ConnectionParameters mConnectionParameters = new ConnectionParameters(configurationParameters);
        try {
            mConnectionParameters.mLocalAddress = InetAddress.getByAddress(ipAddr1);
            mConnectionParameters.mRemoteAddress = InetAddress.getByAddress(ipAddr2);
        } catch (UnknownHostException e) {
            e.printStackTrace();
        }
        mIkeDaemonInterface.createRoute(mConnectionParameters);

        // then
        assertEquals(1, mIkeDaemonInterface.getRoutes().size());
    }

    @Test
    public void deleteRoute() {
        // given
        byte[] ipAddr1 = new byte[]{127, 0, 0, 1};
        byte[] ipAddr2 = new byte[]{10, 10, 10, 2};

        // when
        /* set default configurationParameters*/
        ConfigurationParameters configurationParameters = new ConfigurationParameters();
        ConnectionParameters mConnectionParameters = new ConnectionParameters(configurationParameters);
        try {
            mConnectionParameters.mLocalAddress = InetAddress.getByAddress(ipAddr1);
            mConnectionParameters.mRemoteAddress = InetAddress.getByAddress(ipAddr2);
            mConnectionParameters.mInterface = "test";
        } catch (UnknownHostException e) {
            e.printStackTrace();
        }
        mIkeDaemonInterface.createRoute(mConnectionParameters);
        mIkeDaemonInterface.deleteRoute(mConnectionParameters);

        // then
        assertEquals(0, mIkeDaemonInterface.getRoutes().size());
    }

    @Test
    public void setNewInterface() {
        mIkeDaemonInterface.setNewInterface("imsName", testAdapterName);
    }
    /* (Inner class) RouteEntry test */

    /* (Inner class) "DataReceiver" test */
    @Test
    public void testMessageOfSTR_IND_SIM_AUTH() {
        // given
        TunnelConnection mMockTunnelConnection = mock(TunnelConnection.class);

        // when
        try (MockedConstruction<StrokeAuthenticationIndication> mockedConstruction =
                     Mockito.mockConstruction(StrokeAuthenticationIndication.class)) {
            dataReceiver.onDataAvailable(TEST_BYTES_IND_SIM_AUTH);
        }

        // then
        verify(mMockTunnelConnection, times(0)).onAuthenticationRequest(any());
    }

    @Test
    public void testMessageOfSTR_IND_TERMINATED() {
        // given
        TunnelConnection mMockTunnelConnection = mock(TunnelConnection.class);

        // when
        try (MockedConstruction<StrokeAuthenticationIndication> mockedConstruction =
                     Mockito.mockConstruction(StrokeAuthenticationIndication.class)) {
            dataReceiver.onDataAvailable(TEST_BYTES_IND_TERMINATED);
        }

        // then
        verify(mMockTunnelConnection, times(0)).onAuthenticationRequest(any());
    }

    @Test
    public void loge() {
        mIkeDaemonInterface.loge("test error");
    }

    /* For Test, define internal function */
    public TunnelConnectionCallback makeTestTunnelConnectionCallback() {
        return new TunnelConnectionCallback() {
            @Override
            public void onConnected(String connectionName, String adapterName, int mtu, String tunnelIP, byte[] attributes, byte[] notifies) {
                System.out.println("onConnected " + connectionName + " on " + adapterName + " with mtu " + mtu + " IP:" + tunnelIP);
            }

            @Override
            public void onConnectionError(String connectionName, TunnelError error, byte[] notifies) {
                System.out.println("onConnectionError " + connectionName + " error: " + error);
            }

            @Override
            public void onDisconnected(String connectionName) {
                System.out.println("onDisconnected " + connectionName);
            }

            @Override
            public void onTerminated(String connectionName, Cause cause) {

            }

            @Override
            public byte[] onAuthenticationRequest(byte[] input) {
                System.out.println("onAuthenticationRequest " + input);
                return new byte[0];
            }
        };
    }
}