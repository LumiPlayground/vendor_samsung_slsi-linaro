package com.shannon.qualifiednetworksservice;

import com.shannon.qualifiednetworksservice.BarringInfo.BarringServiceType;

import junit.framework.TestCase;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.MockitoAnnotations;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.annotation.Config;

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class RilUnsolicitedBarringTest extends TestCase {
    RilUnsolicitedMessage mRilUnsolicitedMessage;
    RilUnsolicitedBarring mRilUnsolicitedBarring;

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.openMocks(this);

        byte[] mRawData = new byte[]{
                (byte) 0x01, (byte) 0x00, (byte) 0x00, (byte) 0x00, /* number of info : 1 */
                (byte) 0x06, (byte) 0x00, (byte) 0x00, (byte) 0x00, /* SERVICE_TYPE_MMTEL_VOICE */
                (byte) 0x02, (byte) 0x00, (byte) 0x00, (byte) 0x00,
                (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00,
                (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00,
                (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00};
        mRilUnsolicitedMessage = new RilUnsolicitedMessage(RilUnsolicitedMessage.RIL_UNSOL_BARRING,
                mRawData);
        mRilUnsolicitedBarring = new RilUnsolicitedBarring(mRilUnsolicitedMessage);

    }

    @After
    public void tearDown() throws Exception {
    }

    @Test
    public void testGetBarringInfo() {
        assertNotNull(mRilUnsolicitedBarring.getBarringInfo(BarringServiceType.SERVICE_TYPE_MMTEL_VOICE));
    }

    @Test
    public void testGetBarringInfoNull() {
        byte[] mRawData = new byte[]{
                (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00, /* number of info : 0 */
                (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00,
                (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00,
                (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00,
                (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00,
                (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00};
        mRilUnsolicitedMessage = new RilUnsolicitedMessage(RilUnsolicitedMessage.RIL_UNSOL_BARRING,
                mRawData);
        mRilUnsolicitedBarring = new RilUnsolicitedBarring(mRilUnsolicitedMessage);

        assertNull(mRilUnsolicitedBarring.getBarringInfo(BarringServiceType.SERVICE_TYPE_MMTEL_VOICE));
    }

    @Test
    public void testGetBarringInfoAbnormal() {
        byte[] mRawData = new byte[]{
                (byte) 0x01, (byte) 0x00, (byte) 0x00, (byte) 0x00, /* number of info : 0 */
                (byte) 0x02, (byte) 0x00, (byte) 0x00, (byte) 0x00, /* Invalid service type */
                (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00,
                (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00,
                (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00,
                (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00};
        mRilUnsolicitedMessage = new RilUnsolicitedMessage(RilUnsolicitedMessage.RIL_UNSOL_BARRING,
                mRawData);
        mRilUnsolicitedBarring = new RilUnsolicitedBarring(mRilUnsolicitedMessage);

        assertNull(mRilUnsolicitedBarring.getBarringInfo(BarringServiceType.SERVICE_TYPE_MMTEL_VOICE));
    }

    @Test
    public void testTestToString() {
        mRilUnsolicitedBarring.toString();
    }
}