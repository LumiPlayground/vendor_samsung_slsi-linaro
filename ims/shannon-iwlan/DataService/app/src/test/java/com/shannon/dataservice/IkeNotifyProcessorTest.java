package com.shannon.dataservice;

import android.content.Context;

import androidx.test.platform.app.InstrumentationRegistry;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.MockitoAnnotations;
import org.mockito.MockitoSession;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.annotation.Config;

import java.util.LinkedList;

import static org.junit.Assert.*;
import static org.mockito.Mockito.mockitoSession;

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class IkeNotifyProcessorTest {
    private static final int testType = 10;
    public static final int N1_MODE_INFORMATION = 51115;
    private static final int testPduSessionId = 5;
    private static final String testImei = "100410041004";

    private MockitoSession mStaticMockSession;
    private IkeNotifyProcessor mIkeNotifyProcessor;
    private Context mContext;

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.openMocks(this);
        mStaticMockSession = mockitoSession().startMocking();

        mContext = InstrumentationRegistry.getInstrumentation().getContext();

        LinkedList<IkeNotifyData> mSendList = new LinkedList<>();
        LinkedList<Integer> mExpectedList = new LinkedList<>();
        LinkedList<IkeNotifyData> mReceiveList = new LinkedList<>();

        mIkeNotifyProcessor = new IkeNotifyProcessor(
                new String("Test"),
                mSendList,
                mExpectedList,
                mReceiveList
        );
    }

    @After
    public void tearDown() throws Exception {
        mStaticMockSession.finishMocking();
    }

    @Test
    public void sendN1ModeCapability() {
        mIkeNotifyProcessor.sendN1ModeCapability(testPduSessionId);

        assertEquals(1, mIkeNotifyProcessor.getSendList().size());
    }

    @Test
    public void sendDeviceIdentity() {
        mIkeNotifyProcessor.sendDeviceIdentity(testImei);

        assertEquals(1, mIkeNotifyProcessor.getSendList().size());
    }

    @Test
    public void addExpectedNotify() {
        mIkeNotifyProcessor.addExpectedNotify(IkeNotifyProcessor.BACKOFF_TIMER);
    }

    @Test
    public void getOutgoingNotifyBytesWithNull() {
        assertEquals(null, mIkeNotifyProcessor.getOutgoingNotifyBytes());
    }

    @Test
    public void getOutgoingNotifyBytes() {
        // given
        LinkedList<IkeNotifyData> mSendList = new LinkedList<>();
        LinkedList<Integer> mExpectedList = new LinkedList<>();
        LinkedList<IkeNotifyData> mReceiveList = new LinkedList<>();
        mSendList.add(new IkeNotifyData(testType, new byte[] {1, 2, 3, 4, 5, 6}));
        IkeNotifyProcessor mTestIkeNotifyProcessor;

        // when
        mTestIkeNotifyProcessor = new IkeNotifyProcessor(
                new String("Test"),
                mSendList,
                mExpectedList,
                mReceiveList
        );

        // then
        assertEquals(10, mTestIkeNotifyProcessor.getOutgoingNotifyBytes()[0]);
    }

    @Test
    public void getExpectedNotifyBytesWithNull() {
        // when
        mIkeNotifyProcessor.getExpectedNotifyBytes();

        // then
        assertEquals(0, mIkeNotifyProcessor.getExpectedList().size());
    }

    @Test
    public void getExpectedNotifyBytes() {
        // given
        LinkedList<IkeNotifyData> mSendList = new LinkedList<>();
        LinkedList<Integer> mExpectedList = new LinkedList<>();
        LinkedList<IkeNotifyData> mReceiveList = new LinkedList<>();
        mExpectedList.add(100); // want allocate size
        IkeNotifyProcessor mTestIkeNotifyProcessor;

        // when
        mTestIkeNotifyProcessor = new IkeNotifyProcessor(
                new String("Test"),
                mSendList,
                mExpectedList,
                mReceiveList
        );

        // then
        assertEquals(1, mTestIkeNotifyProcessor.getExpectedNotifyBytes()[0]);
    }

    @Test
    public void processReceivedNotifiesWithNull() {
        mIkeNotifyProcessor.processReceivedNotifies(null);
    }


    @Test
    public void processReceivedNotifies() {
        mIkeNotifyProcessor.processReceivedNotifies(new byte[] {1, 0, 0});
    }

    @Test
    public void getBackoffTimerWithNull() {
        assertNotNull(mIkeNotifyProcessor.getBackoffTimer());
    }

    @Test
    public void getBackoffTimer() {
        // given
        LinkedList<IkeNotifyData> mSendList = new LinkedList<>();
        LinkedList<Integer> mExpectedList = new LinkedList<>();
        LinkedList<IkeNotifyData> mReceiveList = new LinkedList<>();
        mReceiveList.add(new IkeNotifyData(testType, new byte[] {1, 2, 3, 4, 5, 6}));
        IkeNotifyProcessor mTestIkeNotifyProcessor;

        // when
        mTestIkeNotifyProcessor = new IkeNotifyProcessor(
                new String("Test"),
                mSendList,
                mExpectedList,
                mReceiveList
        );

        // then
        assertNotNull( mTestIkeNotifyProcessor.getBackoffTimer());
    }

    @Test
    public void getN1ModeDataWithNull() {
        assertEquals(null, mIkeNotifyProcessor
                .getN1ModeData(IkeNotifyProcessor.N1_MODE_INFORMATION));
    }

    @Test
    public void getN1ModeData() {
        // given
        LinkedList<IkeNotifyData> mSendList = new LinkedList<>();
        LinkedList<Integer> mExpectedList = new LinkedList<>();
        LinkedList<IkeNotifyData> mReceiveList = new LinkedList<>();
        mReceiveList.add(new IkeNotifyData(IkeNotifyProcessor.N1_MODE_INFORMATION, new byte[] {1, 2, 3, 4, 5, 6}));
        IkeNotifyProcessor mTestIkeNotifyProcessor;

        // when
        mTestIkeNotifyProcessor = new IkeNotifyProcessor(
                new String("Test"),
                mSendList,
                mExpectedList,
                mReceiveList
        );

        // then
        assertNotNull(mTestIkeNotifyProcessor
                .getN1ModeData(IkeNotifyProcessor.N1_MODE_INFORMATION));
    }
}