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

import java.nio.ByteBuffer;

import static org.junit.Assert.*;
import static org.mockito.Mockito.spy;

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class IkeConfigurationAttributeTest {
    private static final int TEST_TYPE = 30;
    private static final byte[] bytes = new byte[]{1,1,1,2,2,2};

    @Mock
    ByteBuffer buffer;

    private IkeConfigurationAttribute mIkeConfigurationAttribute;

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.openMocks(this);

        mIkeConfigurationAttribute = new IkeConfigurationAttribute(TEST_TYPE, bytes);
    }

    @After
    public void tearDown() throws Exception {
    }

    @Test
    public void testToString2() {
        mIkeConfigurationAttribute = new IkeConfigurationAttribute(buffer);
    }

    @Test
    public void testToString() {
        String expectedString = "IKE_CFG_ATTRIBUTE type=" + TEST_TYPE
                + " data={" + StringUtils.bytesToHexString(mIkeConfigurationAttribute.mData) + "}";

        assertEquals(expectedString, mIkeConfigurationAttribute.toString());;
    }
}