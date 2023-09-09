package com.shannon.dataservice;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.MockitoAnnotations;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.annotation.Config;

import static org.junit.Assert.*;

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class DnsNaptrRecordTest {
    private static final int TEST_TTL = 100;
    private static final int TEST_ORDER = 0x01;
    private static final int TEST_PREFER = 0x02;

    public static final byte[] input = new byte[] {
            0x00, TEST_ORDER, // Order
            0x00, TEST_PREFER, // Preference
            5, // length of flags
            'F', 'L', 'A', 'G', 'S', // test string
            0, // length of 'services'
            6, // length of 'regexp'
            'R', 'E', 'G', 'E', 'X', 'P', // test string
            8, // length of 'domain'
            'D', 'O', 'M', 'A', '.', 'c', 'o', 'm', // test string
            0
    };

    DnsNaptrRecord defaultDnsNaptrRecord;

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.openMocks(this);

        defaultDnsNaptrRecord = new DnsNaptrRecord(makeDnsResourceCallback(), TEST_TTL, input);
    }

    @After
    public void tearDown() throws Exception {
    }

    @Test
    public void compareTo() {
        // given
        byte[] otherInput = new byte[] {
                0x00, TEST_ORDER + 1, // Order
                0x00, TEST_PREFER, // Preference
                5, // length of flags
                'F', 'L', 'A', 'G', 'S', // test string
                0, // length of 'services'
                6, // length of 'regexp'
                'R', 'E', 'G', 'E', 'X', 'P', // test string
                8, // length of 'domain'
                'D', 'O', 'M', 'A', '.', 'c', 'o', 'm', // test string
                0
        };
        DnsNaptrRecord dnsNaptrRecord = new DnsNaptrRecord(makeDnsResourceCallback(), TEST_TTL, otherInput);

        assertEquals(false, defaultDnsNaptrRecord.compareTo(dnsNaptrRecord) > 0);
    }

    @Test
    public void getOrder() {
        assertEquals(TEST_ORDER, defaultDnsNaptrRecord.getOrder());
    }

    @Test
    public void getPreference() {
        assertEquals(TEST_PREFER, defaultDnsNaptrRecord.getPreference());
    }

    @Test
    public void getReplacement() {
        assertEquals("DOMA.com", defaultDnsNaptrRecord.getReplacement());
    }

    @Test
    public void testToString() {
        assertEquals("NAPTR. Order=1 Pref=2 " +
                "Flags=" + "FLAGS" +
                " Regexp=" + "REGEXP" +
                " Replace=" + "DOMA.com",
                 defaultDnsNaptrRecord.toString());
    }

    /* for test */
    DnsResourceRecord.DnsResourceCallback makeDnsResourceCallback() {
        return new TestDnsRecordExpiredCallback();
    }

    private class TestDnsRecordExpiredCallback implements DnsResourceRecord.DnsResourceCallback {
        public void onResourceExpired(DnsResourceRecord record) {

        }
    }
}