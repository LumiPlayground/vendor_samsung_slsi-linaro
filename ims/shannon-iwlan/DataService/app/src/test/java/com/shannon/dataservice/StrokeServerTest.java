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

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class StrokeServerTest {
    @Mock
    StrokeServer.SocketDataReceiver mMockSocketDataReceiver;

    private StrokeServer strokeServer;

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.openMocks(this);
        strokeServer = new StrokeServer(mMockSocketDataReceiver);
    }

    @After
    public void tearDown() throws Exception {
    }

    @Test
    public void log() {
        strokeServer.log("test");
    }
}