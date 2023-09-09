package com.shannon.dataservice;

import android.content.Context;
import android.content.Intent;
import android.net.ConnectivityManager;
import android.net.LinkProperties;
import android.net.Network;
import android.net.NetworkCapabilities;
import android.os.Message;
import android.telephony.data.DataCallResponse;
import android.telephony.data.DataProfile;
import android.telephony.data.DataService;
import android.telephony.data.NetworkSliceInfo;
import android.util.SparseArray;

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

import static org.junit.Assert.assertNull;
import static org.mockito.ArgumentMatchers.any;
import static org.mockito.ArgumentMatchers.anyInt;
import static org.mockito.ArgumentMatchers.eq;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.mockStatic;
import static org.mockito.Mockito.mockitoSession;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class ConnectionManagerTest {
    private static final int testPcoId = 10;

    @Mock private Context mMockContext;
    @Mock private ConfigurationManager mMockConfiguratoinManager;
    @Mock private ConnectivityManager mMockConnectivityManager;

    @Mock private SimManager mMockSimManger;

    @Mock private NetworkSliceInfo mMockNetworkSliceInfo;

    ConnectionManager mConnectionManager;

    private static MockedStatic<SimManager> mStaticSimManager;

    @BeforeClass
    public static void beforeClass() {
        mStaticSimManager = mockStatic(SimManager.class);
    }

    @AfterClass
    public static void afterClass() {
        mStaticSimManager.close();
    }

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.openMocks(this);
        MobileNetworkCallback mMockMobileNetworkCallback = mock(MobileNetworkCallback.class);

        when(mMockContext.getSystemService(eq(ConnectivityManager.class)))
                .thenReturn(mMockConnectivityManager);
        mConnectionManager = createConnectionManager(TEST_VALUES.SLOT_ID_1, mMockContext,
                mMockConfiguratoinManager);

        when(mMockSimManger.getMobileNetwork(anyInt(), any(ConfigurationManager.class)))
            .thenReturn(mMockMobileNetworkCallback);
        when(SimManager.getInstance()).thenReturn(mMockSimManger);

    }

    @After
    public void tearDown() throws Exception {
    }

    @Test
    public void setupDataCall() {
        // given
        DataProfile mMockDataProfile = mock(DataProfile.class);
        Message message = mock(Message.class);
        LinkProperties linkProperties = mock(LinkProperties.class);

        when(mMockDataProfile.getSupportedApnTypesBitmask())
                .thenReturn(0b1101); // APN_IMS


        // when
        mConnectionManager.setupDataCall(
                mMockDataProfile,
                DataService.REQUEST_REASON_NORMAL,
                linkProperties,
                0,
                mMockNetworkSliceInfo,
                message
        );

        // then
        verify(message, times(1)).sendToTarget();
    }

    @Test
    public void testSetPcoData() {
        mConnectionManager.setPcoData("IMS", testPcoId, new byte[10]);
    }

    @Test
    public void testDestroy() {
        mConnectionManager.destroy();

        // then
        verify(mMockContext, times(1)).unregisterReceiver(any());
    }


    @Test
    public void getDataCallList() {
        // given;
        Message mMockMessage = mock(Message.class);

        // when
        mConnectionManager.getDataCallList(mMockMessage);

        // then
        verify(mMockMessage, times(1)).sendToTarget();
    }

    @Test
    public void setPcoData() {
    }


    /* inner class "DataConnectionCallback" */
    @Test
    public void onConnectionCompleted() {
        // given
        ApnType mMockType = mock(ApnType.class);
        Message mMockMessage = mock(Message.class);
        DataCallResponse mMockResponse = mock(DataCallResponse.class);

        // when
        mConnectionManager.getDataConnectionCallback().onConnectionCompleted(mMockType, mMockMessage,
                mMockResponse);

        // then
        verify(mMockMessage, times(0)).sendToTarget();

    }

    @Test
    public void onConnectionTerminated() {
        // given
        ApnType mMockType = mock(ApnType.class);
        Message mMockMessage = mock(Message.class);

        // when
        mConnectionManager.getDataConnectionCallback().onConnectionTerminated(mMockType, mMockMessage);

        // then
        verify(mMockMessage, times(1)).sendToTarget();
    }

    @Test
    public void onConnectionTerminatedWithThree() {
        // given
        ApnType mMockType = mock(ApnType.class);
        Message mMockMessage = mock(Message.class);
        StrokeTerminatedIndication.Cause mMockCause = mock(StrokeTerminatedIndication.Cause.class);

        // when
        mConnectionManager.getDataConnectionCallback().onConnectionTerminated(mMockType, mMockMessage,
                mMockCause);

        // then
        verify(mMockMessage, times(1)).sendToTarget();
    }

    /* inner class "ConnectionNetworkCallback" */
    @Test
    public void onAvailable() {
        Network network = mock(Network.class);
        NetworkCapabilities networkCapabilities = mock(NetworkCapabilities.class);
        LinkProperties linkProperties = mock(LinkProperties.class);

        mConnectionManager.getRequestCallback().onAvailable(network, networkCapabilities,
                linkProperties, true);
    }

    @Test
    public void onLost() {
        Network network = mock(Network.class);

        mConnectionManager.getRequestCallback().onLost(network);
    }

    @Test
    public void onLinkPropertiesChanged() {
        // given
        Network network = mock(Network.class);
        LinkProperties linkProperties = mock(LinkProperties.class);

        // when
        mConnectionManager.getRequestCallback().onLinkPropertiesChanged(network, linkProperties);
    }

    @Test
    public void startReleaseTimer() {
        // given
        when(mMockConfiguratoinManager.isVoMobileEnabled())
                .thenReturn(false);
        when(mMockSimManger.getMobileNetwork(anyInt(), any()))
                .thenReturn(null);
        Network network = mock(Network.class);
        DataConnection dataConnection = mock(DataConnection.class);
        SparseArray<DataConnection> establishedConnections = mConnectionManager.getEstablishedConnections();
        establishedConnections.put(1, dataConnection);

        // when
        mConnectionManager.getRequestCallback().onLost(network);
    }

    @Test
    public void updateDataConnectionRoutesWithLinkProperties() {
        // given
        Network network = mock(Network.class);
        LinkProperties linkProperties1 = mock(LinkProperties.class);
        when(linkProperties1.isIdenticalAddresses(any()))
                .thenReturn(false);
        LinkProperties linkProperties2 = mock(LinkProperties.class);
        NetworkCapabilities networkCapabilities = mock(NetworkCapabilities.class);
        mConnectionManager.getRequestCallback().onAvailable(network, networkCapabilities,
                linkProperties1, true);

        // when
        mConnectionManager.getRequestCallback().onLinkPropertiesChanged(network, linkProperties2);
    }

    /* for test */
    private ConnectionManager createConnectionManager(int slotId, Context mContext, ConfigurationManager mManager) {
        ConnectionManager connectionManager;

        try (MockedConstruction<RilOemInterface> mockedConstruction =
                     Mockito.mockConstruction(RilOemInterface.class);) {
            connectionManager = new ConnectionManager(slotId, mContext, mManager);
        }
        return connectionManager;
    }
}
