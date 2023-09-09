package com.shannon.dataservice;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.MockitoAnnotations;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.annotation.Config;

import static org.junit.Assert.*;

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class StrokeInitiateResponseTest {
    private StrokeInitiateResponse mMessage;

    private static final byte testNum = 0x0F;
    public static final byte[] TEST_BYTES_IND_SIM_AUTH = new byte[] {
            (byte) 0x0E, (byte) 0x00, (byte) 0x00, (byte) 0x00, // values
            (byte) 0x05, (byte) 0x00, (byte) 0x00, (byte) 0x00, // STR_IND_SIM_AUTH (response type)
            testNum,     (byte) 0x00, (byte) 0x00, (byte) 0x00, // Int value 0xFF
            (byte) 0x08, (byte) 0x00, // length
            (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00,
            (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00
    };

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.openMocks(this);
        mMessage = new StrokeInitiateResponse(
                new StrokeResponseMessage(TEST_BYTES_IND_SIM_AUTH)
        );
    }

    @After
    public void tearDown() throws Exception {
    }

    @Test
    public void getAdapter() {
        assertEquals("", mMessage.getAdapter());;
    }

    @Test
    public void getIpAddresses() {
        assertEquals("", mMessage.getIpAddresses());
    }

    @Test
    public void getAttributes() {
        assertEquals(null, mMessage.getAttributes());
    }

    @Test
    public void getNotifies() {
        assertEquals(null, mMessage.getNotifies());
    }

    @Test
    public void getMtu() {
        assertEquals(0, mMessage.getMtu());
    }

    @Test
    public void testToString() {
        assertEquals("STR_IND_SIM_AUTH name= status=STR_ERROR_UNKNOWN notify=null adapter= address= MTU=0", mMessage.toString());
    }
}