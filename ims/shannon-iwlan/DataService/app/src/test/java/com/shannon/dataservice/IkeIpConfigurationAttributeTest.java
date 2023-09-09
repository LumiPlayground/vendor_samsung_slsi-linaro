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
public class IkeIpConfigurationAttributeTest {

    @Mock
    IkeNotifyData mMockIkeNotifyData;

    IkeIpConfigurationAttribute IkeIpConfigurationAttribute;

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.openMocks(this);

        IkeIpConfigurationAttribute = new IkeIpConfigurationAttribute(0, null);

    }

    @After
    public void tearDown() throws Exception {
    }

    @Test
    public void getIp() {
        assertEquals(null, IkeIpConfigurationAttribute.getIp());;
    }
}