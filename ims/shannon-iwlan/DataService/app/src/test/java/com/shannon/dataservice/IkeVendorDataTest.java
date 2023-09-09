package com.shannon.dataservice;

import android.content.Context;
import android.telephony.TelephonyManager;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.Mock;
import org.mockito.MockitoAnnotations;
import org.mockito.MockitoSession;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.annotation.Config;

import static org.junit.Assert.*;
import static org.mockito.ArgumentMatchers.eq;
import static org.mockito.Mockito.mockitoSession;
import static org.mockito.Mockito.spy;
import static org.mockito.Mockito.when;

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class IkeVendorDataTest {
    private static final int testType = 5;
    private static final byte[] testBytes = new byte[] {1,2,3,4,5};
    private static final String testImei = "100410041004";

    private IkeVendorData mIkeVendorData;

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.openMocks(this);

        mIkeVendorData = new IkeVendorData(testType, testBytes);
    }

    @After
    public void tearDown() throws Exception {
    }

    @Test
    public void getType() {
        assertEquals(testType, mIkeVendorData.getType());
    }

    @Test
    public void getData() {
        assertEquals(testBytes, mIkeVendorData.getData());
    }

    @Test
    public void getBytesSize() {
        assertEquals(Short.BYTES * 2 + testBytes.length, mIkeVendorData.getBytesSize());
    }

    @Test
    public void getBytes() {
        byte[] resultBytes = new byte[] {0,1,1,1,1};
        mIkeVendorData.getBytes();
    }
}