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
public class IkeNotifyN1ModeDataTest {

    @Mock
    IkeNotifyData mMockIkeNotifyData;

    IkeNotifyN1ModeData ikeNotifyN1ModeData;

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.openMocks(this);

        ikeNotifyN1ModeData = new IkeNotifyN1ModeData(mMockIkeNotifyData);
    }

    @After
    public void tearDown() throws Exception {
    }

    @Test
    public void getData() {
        assertEquals(null, ikeNotifyN1ModeData.getData());
    }
}