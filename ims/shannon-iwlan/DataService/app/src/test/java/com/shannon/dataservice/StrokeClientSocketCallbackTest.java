package com.shannon.dataservice;

import static org.junit.Assert.*;

import com.shannon.dataservice.StrokeRequestMessage.RequestMessageType;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.MockitoAnnotations;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.annotation.Config;

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class StrokeClientSocketCallbackTest {
    private StrokeClientSocketCallback mCallback;

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.openMocks(this);
        mCallback = new StrokeClientSocketCallback(RequestMessageType.STR_REQ_ADD_CONN);
    }

    @After
    public void tearDown() throws Exception {
    }

    @Test
    public void onResponse() {
        mCallback.onResponse(new byte[10]);
    }

    @Test
    public void onFailure() {
        mCallback.onFailure();
    }

    @Test
    public void log() {
        mCallback.log("log");
    }

    @Test
    public void loge() {
        mCallback.loge("loge");
    }
}