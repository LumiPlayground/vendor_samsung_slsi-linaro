package com.shannon.qualifiednetworksservice.util;

import junit.framework.TestCase;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.Mockito;
import org.mockito.MockitoAnnotations;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.annotation.Config;

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class DeviceUtilsTest extends TestCase {

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.openMocks(this);
    }

    @After
    public void tearDown() throws Exception {
        Mockito.validateMockitoUsage();
    }

    @Test
    public void testIsConnectionCallbackFixed() {
        assertTrue(DeviceUtils.isConnectionCallbackFixed());
    }
}