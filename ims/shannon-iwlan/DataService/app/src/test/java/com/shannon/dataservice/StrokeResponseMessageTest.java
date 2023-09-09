package com.shannon.dataservice;

import android.content.Context;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.Mock;
import org.mockito.MockitoAnnotations;
import org.mockito.MockitoSession;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.annotation.Config;

import static com.shannon.dataservice.StrokeResponseMessage.ProcessedNotifyType.STR_NOTIFY_NETWORK_AUTH_FAILED;
import static com.shannon.dataservice.StrokeResponseMessage.ResponseErrorStatus.STR_ERROR_UNKNOWN;
import static com.shannon.dataservice.StrokeResponseMessage.ResponseMessageType.STR_RES_INITIATE;
import static com.shannon.dataservice.StrokeResponseMessage.ResponseMessageType.STR_RES_SET_INTERFACE;
import static org.junit.Assert.*;

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class StrokeResponseMessageTest {
    private static final byte testNum = 0x0F;


    // Native Order (Big Endian)
    public static final byte[] TEST_BYTES_INT = new byte[] {
            (byte) 0x0E, (byte) 0x00, (byte) 0x00, (byte) 0x00, // values
            (byte) 0x09, (byte) 0x00, (byte) 0x00, (byte) 0x00, // STR_RES_SET_INTERFACE (response type)
            testNum,     (byte) 0x00, (byte) 0x00, (byte) 0x00, // Int value 0xFF
            (byte) 0x08, (byte) 0x00 // length
    };

    public static final byte[] TEST_BYTES_IND_SIM_AUTH = new byte[] {
            (byte) 0x0E, (byte) 0x00, (byte) 0x00, (byte) 0x00, // values
            (byte) 0x05, (byte) 0x00, (byte) 0x00, (byte) 0x00, // STR_IND_SIM_AUTH (response type)
            testNum,     (byte) 0x00, (byte) 0x00, (byte) 0x00, // Int value 0xFF
            (byte) 0x08, (byte) 0x00 // length
    };

    public static final byte[] TEST_BYTES_IND_TERMINATED = new byte[] {
            (byte) 0x0E, (byte) 0x00, (byte) 0x00, (byte) 0x00, // values
            (byte) 0x04, (byte) 0x00, (byte) 0x00, (byte) 0x00, // STR_IND_TERMINATED (response type)
            testNum,     (byte) 0x00, (byte) 0x00, (byte) 0x00, // Int value 0xFF
            (byte) 0x08, (byte) 0x00 // length
    };

    @Mock
    StrokeClientSocket mMockStrokeClientSocket;
    private Context mContext;

    /* target class for test */
    private StrokeResponseMessage strokeResponseMessage;

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.openMocks(this);

        /* default */
        strokeResponseMessage = new StrokeResponseMessage(TEST_BYTES_INT);
    }

    @After
    public void tearDown() throws Exception {
    }

    @Test
    public void StrokeResponseMessageTest() {
        StrokeResponseMessage message = new StrokeResponseMessage(STR_RES_INITIATE, 30);

        assertEquals(STR_RES_INITIATE, message.getType());
    }

    @Test
    public void getType() {
        assertEquals(STR_RES_SET_INTERFACE, strokeResponseMessage.getType());
    }

    @Test
    public void getShort() {
        assertEquals(0x0201, strokeResponseMessage.getShort());
    }

    @Test
    public void getInt() {
        assertEquals(testNum, strokeResponseMessage.getInt());
    }

    @Test
    public void putInt() {
        StrokeResponseMessage mMessage = new StrokeResponseMessage(TEST_BYTES_INT);
        int previous = mMessage.getOffset();

        // when
        mMessage.putInt(4);

        // then
        assertEquals(previous + 4, mMessage.getOffset());
    }

    @Test
    public void getLong() {
        // given
        final byte[] TEST_BYTES_LONG = new byte[] {
                (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00, // value
                (byte) 0x09, (byte) 0x00, (byte) 0x00, (byte) 0x00, // STR_RES_SET_INTERFACE (response type)
                (byte) testNum, (byte) 0x00, (byte) 0x00, (byte) 0x00, // Int value 0xFF
                (byte) 0x0A, (byte) 0x00 // length
        };

        // when
        StrokeResponseMessage mMessage = new StrokeResponseMessage(TEST_BYTES_LONG);

        // then
        assertEquals((long)0, mMessage.getLong());
    }

    @Test
    public void getString() {
        assertEquals("", strokeResponseMessage.getString());
    }

    @Test
    public void getArray() {
        assertNull(strokeResponseMessage.getArray());
    }

    @Test
    public void getBytesArray() {
        assertEquals( 1, strokeResponseMessage.getBytesArray(6)[0]);
    }

    @Test
    public void putBytesArray() {
        StrokeResponseMessage mMessage = new StrokeResponseMessage(TEST_BYTES_INT);
        int previous = mMessage.getOffset();

        // when
        mMessage.putBytesArray(new byte[] {1, 2, 3, 4});

        // then
        assertEquals(previous + 4, mMessage.getOffset());
    }

    @Test
    public void getBytes() {
        assertEquals(14, strokeResponseMessage.getBytes()[0]);
    }

    @Test
    public void getEnum() {
        // when
        assertEquals(STR_ERROR_UNKNOWN, StrokeResponseMessage.ResponseErrorStatus.getEnum(1));
    }

    @Test
    public void getEnumWithProcessedNotifyType () {
        // when
        assertEquals(STR_NOTIFY_NETWORK_AUTH_FAILED, StrokeResponseMessage.ProcessedNotifyType.getEnum(24));
    }
}