package com.shannon.dataservice;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.MockitoAnnotations;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.annotation.Config;

import static com.shannon.dataservice.DnsResolver.DNS_TYPE_NAPTR;
import static org.junit.Assert.*;

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class DnsQueryTest {
    private static String query = "oliver.com";

    private DnsQuery defaultDnsQuery;

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.openMocks(this);

        defaultDnsQuery = new DnsQuery();
    }

    @After
    public void tearDown() throws Exception {
    }

    @Test
    public void getNaptrQuery() {
        assertEquals(true, (new String(defaultDnsQuery.getNaptrQuery(query))).contains("oliver"));
    }

    @Test
    public void getRequestId() {
        DnsQuery dnsQuery = new DnsQuery();
        dnsQuery.getRequestId();
    }

    @Test
    public void getType() {
        // when
        defaultDnsQuery.getNaptrQuery(query);

        // then
        assertEquals(DNS_TYPE_NAPTR, defaultDnsQuery.getType());
    }

    @Test
    public void getDomain() {
        // when
        defaultDnsQuery.getNaptrQuery(query);

        // then
        assertEquals("oliver.com", defaultDnsQuery.getDomain());
    }
}