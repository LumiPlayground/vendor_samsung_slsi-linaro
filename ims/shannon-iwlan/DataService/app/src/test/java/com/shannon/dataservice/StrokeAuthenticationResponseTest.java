package com.shannon.dataservice;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.Mock;
import org.mockito.MockitoAnnotations;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.annotation.Config;

import static com.shannon.dataservice.StrokeAuthenticationResponse.AuthenticationStatus.AUTH_STATUS_ERROR;
import static org.junit.Assert.*;

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class StrokeAuthenticationResponseTest {

    private StrokeAuthenticationResponse strokeAuthenticationResponse;

    @Mock
    StrokeAuthenticationResponse.AuthenticationStatus authenticationStatus;

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.openMocks(this);

        strokeAuthenticationResponse = new StrokeAuthenticationResponse(authenticationStatus,
                new byte[] {0x00, 0x00}, new byte[] {0x00, 0x00}, new byte[] {0x00, 0x00});
    }

    @After
    public void tearDown() throws Exception {
    }

    /* inner class test */
    @Test
    public void getCode() {
        StrokeAuthenticationResponse.AuthenticationStatus anEnum = StrokeAuthenticationResponse.AuthenticationStatus.getEnum(1);

        assertEquals(1, anEnum.getCode());
    }

    @Test
    public void getEnum() {
        assertEquals(AUTH_STATUS_ERROR, StrokeAuthenticationResponse.AuthenticationStatus.getEnum(1));;
    }
}