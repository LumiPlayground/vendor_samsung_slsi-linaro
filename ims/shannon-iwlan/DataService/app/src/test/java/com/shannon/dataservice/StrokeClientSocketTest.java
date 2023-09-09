package com.shannon.dataservice;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.Mock;
import org.mockito.MockitoAnnotations;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.annotation.Config;

import static org.junit.Assert.*;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class StrokeClientSocketTest {

    @Mock
    StrokeClientSocketCallback mMockStrokeClientSocketCallback;

    private StrokeClientSocket strokeClientSocket;

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.openMocks(this);
        strokeClientSocket = new StrokeClientSocket(mMockStrokeClientSocketCallback);
    }

    @After
    public void tearDown() throws Exception {
    }

    @Test
    public void send() {
        // in test enviornment, socke open fail
        assertFalse(strokeClientSocket.send(new byte[] {0x01, 0x01, 0x01, 0x01}));;
    }

    @Test
    public void log() {
        strokeClientSocket.log("Test log");
    }

    @Test
    public void loge() {
        strokeClientSocket.loge("Test error");
    }

    /* inner class */
    @Test
    public void run() {
        // given
        StrokeClientSocket.ResponseThread responseThread
                = strokeClientSocket.new ResponseThread();

        // when
        responseThread.run();

        // then
        verify(mMockStrokeClientSocketCallback, times(1)).onFailure();
    }
}