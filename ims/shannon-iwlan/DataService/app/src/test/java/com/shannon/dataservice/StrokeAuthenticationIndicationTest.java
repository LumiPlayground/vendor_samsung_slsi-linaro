package com.shannon.dataservice;

import static org.junit.Assert.*;
import static org.mockito.Mockito.mockitoSession;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.MockitoAnnotations;
import org.mockito.MockitoSession;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.annotation.Config;

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class StrokeAuthenticationIndicationTest {
    private static final byte testNum = 0x0F;
    // Native Order (Big Endian)
    public static final byte[] TEST_BYTES_IND_SIM_AUTH = new byte[] {
            (byte) 0x0E, (byte) 0x00, (byte) 0x00, (byte) 0x00, // values
            (byte) 0x05, (byte) 0x00, (byte) 0x00, (byte) 0x00, // STR_IND_SIM_AUTH (response type)
            testNum,     (byte) 0x00, (byte) 0x00, (byte) 0x00, // Int value 0xFF
            (byte) 0x08, (byte) 0x00 // length
    };

    private AutoCloseable closeable;
    private MockitoSession mStaticMockSession;
    private StrokeAuthenticationIndication mMessage;

    @Before
    public void setUp() throws Exception {
        closeable = MockitoAnnotations.openMocks(this);
        mStaticMockSession = mockitoSession().startMocking();

        mMessage = new StrokeAuthenticationIndication(new StrokeResponseMessage(TEST_BYTES_IND_SIM_AUTH));
    }

    @After
    public void tearDown() throws Exception {
        mStaticMockSession.finishMocking();
        closeable.close();
    }

    @Test
    public void getName() {
        assertEquals("", mMessage.getName());
    }

    @Test
    public void getNonce() {
        assertNotNull(mMessage.getNonce());
    }

    @Test
    public void testToString() {
        assertNotNull(mMessage.toString());
    }
}