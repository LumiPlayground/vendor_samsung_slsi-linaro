package com.shannon.dataservice;

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

import static com.shannon.dataservice.StrokeTerminatedIndication.Cause.CAUSE_NORMAL;
import static com.shannon.dataservice.TunnelConnection.*;
import static org.junit.Assert.*;
import static org.mockito.ArgumentMatchers.any;
import static org.mockito.ArgumentMatchers.anyInt;
import static org.mockito.ArgumentMatchers.anyString;
import static org.mockito.Mockito.doNothing;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.mockStatic;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;


@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class TunnelConnectionTest {
    private static MockedStatic<IkeDaemonInterface> mStaticIkeDaemonInterface;

    @Mock private ConnectionParameters mMockConnectionParameters;
    @Mock private TunnelConnectionCallback mMockTunnelConnectionCallback;
    @Mock private IkeDaemonInterface mMockIkeDaemonInterface;

    private TunnelConnection tunnelConnection;

    @BeforeClass
    public static void beforeClass() {
        mStaticIkeDaemonInterface = mockStatic(IkeDaemonInterface.class);
    }

    @AfterClass
    public static void afterClass() {
        mStaticIkeDaemonInterface.close();
    }

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.openMocks(this);
        when(IkeDaemonInterface.getInstance()).thenReturn(mMockIkeDaemonInterface);
    }

    @After
    public void tearDown() throws Exception {
    }

    @Test
    public void addCallback() {
        // given
        doNothing().when(mMockTunnelConnectionCallback).onConnected(anyString(), anyString(), anyInt(),
                anyString(), any(), any());
        tunnelConnection = new TunnelConnection(mMockConnectionParameters, mMockTunnelConnectionCallback);
        tunnelConnection.setState(STATE_OPENED);

        // when
        tunnelConnection.addCallback(mMockTunnelConnectionCallback);

        // then
        verify(mMockTunnelConnectionCallback, times(2)).onConnected(null, null,
                0,
                null, null, null);
    }

    @Test
    public void connect() {
        // when
        tunnelConnection = new TunnelConnection(mMockConnectionParameters, mMockTunnelConnectionCallback);
        tunnelConnection.connect();

        // then
        // because, ADD is failure (socket not open)
        assertEquals(STATE_TERMINATED, tunnelConnection.getState());
    }

    @Test
    public void disconnectWithSTATE_OPENED_onDisconnectFailure() {
        // given
        tunnelConnection = new TunnelConnection(mMockConnectionParameters, mMockTunnelConnectionCallback);
        tunnelConnection.setState(STATE_OPENED);

        tunnelConnection.disconnect();

        // then
        assertEquals(0, tunnelConnection.getCallbackList().size());
    }

    @Test
    public void disconnectWithSTATE_OPENING_onDisconnectFailure() {
        // given
        tunnelConnection = new TunnelConnection(mMockConnectionParameters, mMockTunnelConnectionCallback);
        tunnelConnection.setState(STATE_OPENING);

        tunnelConnection.disconnect();

        // then
        assertEquals(0, tunnelConnection.getCallbackList().size());
    }

    @Test
    public void disconnectWithSTATE_ADD_onDisconnectFailure() {
        // given
        // we expected "deleteConnection"
        tunnelConnection = new TunnelConnection(mMockConnectionParameters, mMockTunnelConnectionCallback);
        tunnelConnection.setState(STATE_TERMINATED);

        tunnelConnection.disconnect();

        // then
        // not clear, because, current state is ADD.
        assertEquals(1, tunnelConnection.getCallbackList().size());
    }

    @Test
    public void close() {
        // given
        doNothing().when(mMockTunnelConnectionCallback).onTerminated(any(), any());
        tunnelConnection = new TunnelConnection(mMockConnectionParameters, mMockTunnelConnectionCallback);

        tunnelConnection.setState(STATE_OPENED);
        StrokeTerminatedIndication mMockIndication = mock(StrokeTerminatedIndication.class);
        when(mMockIndication.getCause()).thenReturn(CAUSE_NORMAL);


        tunnelConnection.addCallback(mMockTunnelConnectionCallback);

        // when
        tunnelConnection.close(mMockIndication);

        // then
        assertEquals(STATE_TERMINATED, tunnelConnection.getState());
        assertEquals(0, tunnelConnection.getCallbackList().size());
    }

    @Test
    public void closeWithGetcuase() {
        // given
        doNothing().when(mMockTunnelConnectionCallback).onTerminated(any(), any());
        tunnelConnection = new TunnelConnection(mMockConnectionParameters, mMockTunnelConnectionCallback);

        tunnelConnection.setState(STATE_OPENED);

        StrokeResponseMessage mMockStrokeResponseMessage = mock(StrokeResponseMessage.class);
        StrokeTerminatedIndication indication = new StrokeTerminatedIndication(mMockStrokeResponseMessage);

        tunnelConnection.addCallback(mMockTunnelConnectionCallback);

        // when
        tunnelConnection.close(indication);

        // then
        assertEquals(STATE_TERMINATED, tunnelConnection.getState());
        assertEquals(0, tunnelConnection.getCallbackList().size());
    }

    @Test
    public void onAuthenticationRequest() {
        // given
        when(mMockTunnelConnectionCallback.onAuthenticationRequest(any()))
                .thenReturn("TEST_SUCCESS".getBytes());
        tunnelConnection = new TunnelConnection(mMockConnectionParameters, mMockTunnelConnectionCallback);

        // when
        tunnelConnection.addCallback(mMockTunnelConnectionCallback);

        // then
        assertEquals(new String("TEST_SUCCESS"), new String(tunnelConnection.onAuthenticationRequest(new byte[] {1})));
    }

    @Test
    public void onConnectFailure() {
        tunnelConnection = new TunnelConnection(mMockConnectionParameters, mMockTunnelConnectionCallback);
        tunnelConnection.setState(STATE_OPENED);

        TunnelError tunnelError = TunnelError.ERR_SEMANTIC_ERROR_IN_THE_TFT_OPERATION;
        tunnelConnection.onConnectFailure(TEST_VALUES.CONNECTION_NAME, tunnelError, new byte[] {0x00, 0x00});
    }

    @Test
    public void onConnected() {
        tunnelConnection = new TunnelConnection(mMockConnectionParameters, mMockTunnelConnectionCallback);
        tunnelConnection.setState(STATE_OPENED);

        // protected void onConnected(String name, String adapter, int mtu, String tunnelIP, byte[] attributes, byte[] notifies) {
        tunnelConnection.onConnected(TEST_VALUES.CONNECTION_NAME, TEST_VALUES.ADAPTER_NAME,
                TEST_VALUES.MTU, TEST_VALUES.TUUNEL_IP_V4,
                TEST_VALUES.ATTRIBUTES, TEST_VALUES.NOTIFIES);
    }

    @Test
    public void initiateConnection() {
        tunnelConnection = new TunnelConnection(mMockConnectionParameters, mMockTunnelConnectionCallback);
        tunnelConnection.setState(STATE_OPENED);

        // ePDG name
        tunnelConnection.initiateConnection("epdg0");
    }

    @Test
    public void onDisconnected() {
        tunnelConnection = new TunnelConnection(mMockConnectionParameters, mMockTunnelConnectionCallback);
        tunnelConnection.setState(STATE_OPENED);

        tunnelConnection.onDisconnected("epdg0");
    }

    @Test
    public void onAdded() {
        tunnelConnection = new TunnelConnection(mMockConnectionParameters, mMockTunnelConnectionCallback);
        tunnelConnection.setState(STATE_OPENED);

        tunnelConnection.onAdded("epdg0");
    }

    @Test
    public void onDeleted() {
        tunnelConnection = new TunnelConnection(mMockConnectionParameters, mMockTunnelConnectionCallback);
        tunnelConnection.setState(STATE_OPENED);

        tunnelConnection.onDeleted("epdg0");
    }
}
