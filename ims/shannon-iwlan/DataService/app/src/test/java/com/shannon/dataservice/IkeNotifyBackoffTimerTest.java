package com.shannon.dataservice;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.Mock;
import org.mockito.MockitoAnnotations;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.annotation.Config;

import static org.junit.Assert.assertEquals;
import static org.mockito.Mockito.when;

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class IkeNotifyBackoffTimerTest {
    private IkeNotifyBackoffTimer mIkeNotifyBackoffTimer;

    @Mock IkeNotifyData mMockIkeNotifyData;

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.openMocks(this);

        when(mMockIkeNotifyData.getData())
                .thenReturn(new byte[] {0, 0});

        mIkeNotifyBackoffTimer = new IkeNotifyBackoffTimer(mMockIkeNotifyData);
    }

    @After
    public void tearDown() throws Exception {
    }

    @Test
    public void getTimerValue() {
        assertEquals(0, mIkeNotifyBackoffTimer.getTimerValue());;
    }

    @Test
    public void getTimerValueUsingSpecificValue() {
        when(mMockIkeNotifyData.getData())
                .thenReturn(new byte[] {(byte)0, (byte)0xE0});

        IkeNotifyBackoffTimer mTestIkeNotifyBackoffTimer =
                new IkeNotifyBackoffTimer(mMockIkeNotifyData);
    }

    @Test
    public void getTimerValueUsingSpecificValue1() {
        // 1st
        when(mMockIkeNotifyData.getData())
                .thenReturn(new byte[]{(byte) 0, (byte) 0x1F});

        IkeNotifyBackoffTimer mTestIkeNotifyBackoffTimer =
                new IkeNotifyBackoffTimer(mMockIkeNotifyData);

        assertEquals(18600000, mTestIkeNotifyBackoffTimer.getTimerValue());
    }

    @Test
    public void getTimerValueUsingSpecificValue2() {
        // 2nd
        when(mMockIkeNotifyData.getData())
                .thenReturn(new byte[]{(byte) 0, (byte) 0x3F});

        IkeNotifyBackoffTimer mTestIkeNotifyBackoffTimer = new IkeNotifyBackoffTimer(mMockIkeNotifyData);

        assertEquals(111600000, mTestIkeNotifyBackoffTimer.getTimerValue());
    }

    @Test
    public void getTimerValueUsingSpecificValue3() {
        // 3rd
        when(mMockIkeNotifyData.getData())
                .thenReturn(new byte[]{(byte) 0, (byte) 0x5F});

        IkeNotifyBackoffTimer mTestIkeNotifyBackoffTimer = new IkeNotifyBackoffTimer(mMockIkeNotifyData);

        assertEquals(1116000000, mTestIkeNotifyBackoffTimer.getTimerValue());
    }

    @Test
    public void getTimerValueUsingSpecificValue4() {
        // 4th
        when(mMockIkeNotifyData.getData())
                .thenReturn(new byte[]{(byte) 0, (byte) 0x7F});

        IkeNotifyBackoffTimer mTestIkeNotifyBackoffTimer = new IkeNotifyBackoffTimer(mMockIkeNotifyData);

        assertEquals(62000, mTestIkeNotifyBackoffTimer.getTimerValue());
    }

    @Test
    public void getTimerValueUsingSpecificValue5() {
        // 5th
        when(mMockIkeNotifyData.getData())
                .thenReturn(new byte[]{(byte) 0, (byte) 0x8F});


        IkeNotifyBackoffTimer mTestIkeNotifyBackoffTimer = new IkeNotifyBackoffTimer(mMockIkeNotifyData);

        assertEquals(450000, mTestIkeNotifyBackoffTimer.getTimerValue());
    }

    @Test
    public void getTimerValueUsingSpecificValue6() {
        // 6th
        when(mMockIkeNotifyData.getData())
                .thenReturn(new byte[]{(byte) 0, (byte) 0x9F});

        IkeNotifyBackoffTimer mTestIkeNotifyBackoffTimer = new IkeNotifyBackoffTimer(mMockIkeNotifyData);

        assertEquals(930000, mTestIkeNotifyBackoffTimer.getTimerValue());
    }

    @Test
    public void getTimerValueUsingSpecificValue7() {
        // 7th
        when(mMockIkeNotifyData.getData())
                .thenReturn(new byte[]{(byte) 0, (byte) 0x11F});

        IkeNotifyBackoffTimer mTestIkeNotifyBackoffTimer = new IkeNotifyBackoffTimer(mMockIkeNotifyData);

        assertEquals(18600000, mTestIkeNotifyBackoffTimer.getTimerValue());
    }

    @Test
    public void getTimerValueUsingSpecificValue8() {
        // 8th
        when(mMockIkeNotifyData.getData())
                .thenReturn(new byte[]{(byte) 0, (byte) 0x13F});

        IkeNotifyBackoffTimer mTestIkeNotifyBackoffTimer = new IkeNotifyBackoffTimer(mMockIkeNotifyData);

        assertEquals(111600000, mTestIkeNotifyBackoffTimer.getTimerValue());
    }

    @Test
    public void getTimerValueUsingSpecificValue9() {
        // 9th
        when(mMockIkeNotifyData.getData())
                .thenReturn(new byte[]{(byte) 0, (byte) 0x15F});

        IkeNotifyBackoffTimer mTestIkeNotifyBackoffTimer = new IkeNotifyBackoffTimer(mMockIkeNotifyData);

        assertEquals(1116000000, mTestIkeNotifyBackoffTimer.getTimerValue());
    }

    @Test
    public void getTimerValueUsingSpecificValue10() {
        // 10th
        when(mMockIkeNotifyData.getData())
                .thenReturn(new byte[] {(byte)0, (byte)0x4F});

        IkeNotifyBackoffTimer mTestIkeNotifyBackoffTimer = new IkeNotifyBackoffTimer(mMockIkeNotifyData);

        assertEquals(540000000, mTestIkeNotifyBackoffTimer.getTimerValue());
    }

    @Test
    public void isDeactivatedOrInvalid() {
        assertEquals(false, mIkeNotifyBackoffTimer.isDeactivatedOrInvalid());;
    }

    @Test
    public void loge() {
        mIkeNotifyBackoffTimer.loge("loge");
    }
}