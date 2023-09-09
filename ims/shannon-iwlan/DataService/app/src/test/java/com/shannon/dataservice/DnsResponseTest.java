package com.shannon.dataservice;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.MockitoAnnotations;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.annotation.Config;

import java.util.Arrays;

import static org.junit.Assert.*;

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class DnsResponseTest {
    private static final int DNS_COMPRESSED = 0xc0;

    // DNS_HEADER_LENGTH = 12;
    public static byte[] TEST_INPUT = {
            0x00, 0x01, // id
            0x00, 0x00, // empty
            0x00, 0x03, // questions
            0x00, 0x02, // answers
            0x00, 0x00, // + answers
            0x00, 0x00, // + answers
            // the above is DNS_HEADER_LENGTH (12)
            // the below is specific value.
            (byte)0xc0, (byte)0xc0, (byte)0xc0, (byte)0xc0, (byte)0xc0, (byte)0xc0, (byte)0xc0, (byte)0xc0,
            (byte)0xc0, (byte)0xc0, (byte)0xc0, (byte)0xc0, (byte)0x10, (byte)0x10, (byte)0x10, (byte)0x10,
            (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00,
            (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00,
            (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00,
            (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00
    };

    private DnsResponse defaultDnsResponse;

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.openMocks(this);
        defaultDnsResponse = new DnsResponse(TEST_INPUT);
    }

    @After
    public void tearDown() throws Exception {
    }

    @Test
    public void getNextAnswer() {
        assertEquals(new String(new byte[] {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}),
                new String(defaultDnsResponse.getNextAnswer()));
    }

    @Test
    public void getDomainName() {
        byte[] input = {
                (byte)0x01, (byte)0x02, (byte)0x03, (byte)0x04
        };

        // when & then
        assertEquals("X", defaultDnsResponse.getDomainName(input, 0));
    }


    @Test
    public void getDomainNameWithDNS_COMPRESSED() {
        byte[] input = {
                (byte)0x01, (byte)0x02, (byte)0x03, (byte)DNS_COMPRESSED
        };

        // when & then
        assertEquals("X", defaultDnsResponse.getDomainName(input, 0));
    }

    @Test
    public void getTypeOffset() {
        byte[] input = {
                (byte)0x01, (byte)0x02, (byte)0x03, (byte)0x04,
                (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00,
                (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00
        };

        // when & then
        assertEquals(7, defaultDnsResponse.getTypeOffset(input, 0));
    }

    @Test
    public void isExpected() {
        assertEquals(true, defaultDnsResponse.isExpected(1));
    }
}