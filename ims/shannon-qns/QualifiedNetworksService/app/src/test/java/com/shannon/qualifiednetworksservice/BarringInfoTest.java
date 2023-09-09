package com.shannon.qualifiednetworksservice;

import static android.hardware.radio.network.BarringInfo.BARRING_TYPE_UNCONDITIONAL;
import static android.hardware.radio.network.BarringInfo.SERVICE_TYPE_MMTEL_VOICE;
import static com.shannon.qualifiednetworksservice.ApnType.APN_IMS;

import com.shannon.qualifiednetworksservice.BarringInfo.BarringServiceType;
import com.shannon.qualifiednetworksservice.BarringInfo.BarringType;

import junit.framework.TestCase;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.MockitoAnnotations;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.annotation.Config;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class BarringInfoTest extends TestCase {
    private BarringInfo mBarringInfo;

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.openMocks(this);

        // RIL_UNSOL_BARRING
        ByteBuffer buffer = ByteBuffer.allocate(2048);
        buffer.order(ByteOrder.LITTLE_ENDIAN);
        buffer.putInt(Integer.valueOf(SERVICE_TYPE_MMTEL_VOICE).intValue());
        buffer.putInt(Integer.valueOf(BARRING_TYPE_UNCONDITIONAL).intValue());
        buffer.putInt(Integer.valueOf(100).intValue()); // barring factor
        buffer.putInt(Integer.valueOf(60).intValue()); // time seconds
        buffer.putInt(Integer.valueOf(1).intValue()); // isBarred

        mBarringInfo = new BarringInfo(buffer);
    }

    @After
    public void tearDown() throws Exception {
    }

    @Test
    public void testGetBarringServiceType() {
        assertNotNull(mBarringInfo.getBarringServiceType());
        assertEquals(BarringServiceType.SERVICE_TYPE_MMTEL_VOICE,
                     BarringServiceType.getEnum(SERVICE_TYPE_MMTEL_VOICE));
    }

    @Test
    public void testGetBarringType() {
        assertNotNull(mBarringInfo.getBarringType());
        assertEquals(BarringType.BARRING_TYPE_UNKNOWN, BarringType.getEnum(4/* invalid */));
    }

    @Test
    public void testGetBarringFactor() {
        assertNotNull(mBarringInfo.getBarringFactor());
    }

    @Test
    public void testIsBarred() {
        assertNotNull(mBarringInfo.isBarred());
    }

    @Test
    public void testTestEquals() {
        // RIL_UNSOL_BARRING
        ByteBuffer buffer = ByteBuffer.allocate(2048);
        buffer.order(ByteOrder.LITTLE_ENDIAN);
        buffer.putInt(Integer.valueOf(SERVICE_TYPE_MMTEL_VOICE).intValue());
        buffer.putInt(Integer.valueOf(4 /* invalid value */).intValue());
        buffer.putInt(Integer.valueOf(100).intValue()); // barring factor
        buffer.putInt(Integer.valueOf(60).intValue()); // time seconds
        buffer.putInt(Integer.valueOf(1).intValue()); // isBarred

        // Check not equal
        BarringInfo testBarringInfo = new BarringInfo(buffer);
        assertNotNull(mBarringInfo.equals(testBarringInfo));

        // Check equal
        assertNotNull(mBarringInfo.equals(mBarringInfo));

        // Check abnormal
        assertNotNull(mBarringInfo.equals(APN_IMS));
    }

    @Test
    public void testTestToString() {
        assertNotNull(mBarringInfo.toString());
    }
}