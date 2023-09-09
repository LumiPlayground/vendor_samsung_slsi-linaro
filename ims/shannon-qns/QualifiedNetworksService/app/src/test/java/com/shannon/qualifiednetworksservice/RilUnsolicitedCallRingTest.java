package com.shannon.qualifiednetworksservice;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.FixMethodOrder;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.MethodSorters;
import org.mockito.Mockito;
import org.mockito.MockitoAnnotations;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.annotation.Config;

@FixMethodOrder(MethodSorters.NAME_ASCENDING)
@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class RilUnsolicitedCallRingTest {
    RilUnsolicitedMessage mRilUnsolicitedMessage;
    RilUnsolicitedCallRing mRilUnsolicitedCallRing;

    @BeforeClass
    public static void beforeClass() {
    }

    @AfterClass
    public static void afterClass() {
    }

    @Before
    public void setUp() {
        MockitoAnnotations.openMocks(this);
        byte[] mRawData = new byte[]{
                (byte) 0x01, (byte) 0x00, /* call type */
                (byte) 0x02,              /* call id */
                (byte) 0x00, (byte) 0x00, /* num len */
                (byte) 0x00, (byte) 0x00, /* sdp len */
                (byte) 0x00,              /* ussd language code */
                (byte) 0x00,              /* ussd len */
                (byte) 0x00,              /* ussd alerting pattern */
                (byte) 0x00,              /* ussd server initiated type */
                (byte) 0x00,              /* is conference call */
                (byte) 0x00,              /* display name len */
                (byte) 0x00,              /* verstat */
                (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00, /* sdp msg len */
                (byte) 0x00              /* mt call release cause */
                };
        mRilUnsolicitedMessage = new RilUnsolicitedMessage(RilUnsolicitedMessage.RIL_UNSOL_CALL_RING, mRawData);
        mRilUnsolicitedCallRing = new RilUnsolicitedCallRing(mRilUnsolicitedMessage);
    }

    @After
    public void tearDown() {
        Mockito.validateMockitoUsage();
    }

    @Test
    public void getCallId() {
        assertEquals(mRilUnsolicitedCallRing.getCallId(), 2);
    }

    @Test
    public void isAudioCall() {
        assertTrue(mRilUnsolicitedCallRing.isAudioCall());
    }

    @Test
    public void isVideoCall() {
        assertFalse(mRilUnsolicitedCallRing.isVideoCall());
    }

    @Test
    public void isEmergencyCall() {
        assertFalse(mRilUnsolicitedCallRing.isEmergencyCall());
    }

    @Test
    public void isValidCall() {
        assertTrue(mRilUnsolicitedCallRing.isValidCall());
    }

    @Test
    public void testToString() {
        mRilUnsolicitedCallRing.toString();
    }
}
