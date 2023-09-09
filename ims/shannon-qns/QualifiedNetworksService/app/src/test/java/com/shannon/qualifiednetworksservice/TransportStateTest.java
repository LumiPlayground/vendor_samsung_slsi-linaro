package com.shannon.qualifiednetworksservice;

import android.content.Context;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;

import androidx.test.platform.app.InstrumentationRegistry;

import com.shannon.qualifiednetworksservice.TransportState.IStateChanged;
import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.Mock;
import org.mockito.MockitoAnnotations;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.annotation.Config;

import java.util.Timer;

import static org.junit.Assert.assertEquals;
import static org.mockito.Mockito.when;

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class TransportStateTest {
    private static final int testSlotId = 0;

    @Mock private SubscriptionManager mMockSubscriptionManager;
    @Mock private TelephonyManager mMockTelephonyManager;
    @Mock private IStateChanged mockStateChangedCallback;

    private TransportState mTransportState;
    private Context mContext;


    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.openMocks(this);

        mContext = InstrumentationRegistry.getInstrumentation().getContext();
        mTransportState = new TransportState(mockStateChangedCallback);

        when(mMockTelephonyManager.getNetworkSpecifier())
                .thenReturn(String.valueOf(testSlotId));
        when(mMockSubscriptionManager.getSubscriptionIds(testSlotId))
                .thenReturn(new int[]{TelephonyManager.SIM_STATE_READY});
    }

    @After
    public void tearDown() throws Exception {
    }

    @Test
    public void isAvailable() {
        assertEquals(true, mTransportState.isAvailable());
    }

    @Test
    public void reset() {
        // when
        mTransportState.reset();

        // then
        assertEquals(true, mTransportState.isAvailable());
    }

    @Test
    public void block() {
        // when
        mTransportState.block();

        // then
        assertEquals(0, mTransportState.getRetryCount());
        assertEquals(false, mTransportState.isConnecting());
    }

    @Test
    public void blockTemporary() {
        mTransportState.blockTemporary(300);
    }

    @Test
    public void onConnecting() {
        // when
        mTransportState.onConnecting();

        // then
        assertEquals(1, mTransportState.getRetryCount());
        assertEquals(true, mTransportState.isConnecting());
    }

    @Test
    public void onConnectingError() {
        // when
        mTransportState.onConnectingError();

        // then
        assertEquals(true, mTransportState.getRetryErrorCount() > 0);
    }

    @Test
    public void onConnected() {
        // when
        mTransportState.onConnected();

        // then
        assertEquals(0, mTransportState.getRetryCount());
    }

    @Test
    public void onDisconnected() {
        // when
        mTransportState.onDisconnected();

        // then
        assertEquals(true, !mTransportState.isConnecting());
    }

    @Test
    public void isConnectionError() {
        // when
        mTransportState.onConnecting();
        mTransportState.onConnectingError();

        // then
        assertEquals(true, mTransportState.isConnectionError());
    }
}
