package com.shannon.dataservice;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.MockitoAnnotations;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.annotation.Config;

import static org.junit.Assert.*;
import static org.mockito.Mockito.mock;

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class DnsResourceRecordTest {
    private static final int TEST_TTL = 100;

    private DnsResourceRecord defaultDnsResourceRecord;

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.openMocks(this);

        defaultDnsResourceRecord = new DnsResourceRecord(new TestDnsRecordExpiredCallback(), TEST_TTL);
    }

    @After
    public void tearDown() throws Exception {
    }

    @Test
    public void destroy() {
        defaultDnsResourceRecord.destroy();
    }

    private class TestDnsRecordExpiredCallback implements DnsResourceRecord.DnsResourceCallback {
        public void onResourceExpired(DnsResourceRecord record) {
        }
    }
}