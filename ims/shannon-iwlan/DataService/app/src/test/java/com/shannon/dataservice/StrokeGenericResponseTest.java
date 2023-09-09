package com.shannon.dataservice;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.MockitoAnnotations;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.annotation.Config;

import static com.shannon.dataservice.StrokeResponseMessage.ResponseErrorStatus.STR_ERROR_UNKNOWN;
import static org.junit.Assert.*;

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class StrokeGenericResponseTest {
    private StrokeGenericResponse mMessage;

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.openMocks(this);
        mMessage = new StrokeGenericResponse(
                new StrokeResponseMessage(StrokeResponseMessageTest.TEST_BYTES_IND_SIM_AUTH)
        );
    }

    @After
    public void tearDown() throws Exception {
    }

    @Test
    public void getName() {
        assertEquals("", mMessage.getName());
    }

    @Test
    public void getStatus() {
        assertEquals(STR_ERROR_UNKNOWN, mMessage.getStatus());
    }

    @Test
    public void getNotify() {
        assertEquals(null, mMessage.getNotify());
    }

    @Test
    public void testToString() {
        assertEquals("STR_IND_SIM_AUTH name= status=STR_ERROR_UNKNOWN notify=null", mMessage.toString());

    }
}