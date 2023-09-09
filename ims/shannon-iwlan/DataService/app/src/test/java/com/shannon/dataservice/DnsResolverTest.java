package com.shannon.dataservice;

import android.annotation.CallbackExecutor;
import android.annotation.NonNull;
import android.annotation.Nullable;
import android.net.Network;
import android.os.CancellationSignal;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.Mock;
import org.mockito.MockedStatic;
import org.mockito.MockitoAnnotations;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.annotation.Config;

import java.util.concurrent.CountDownLatch;
import java.util.concurrent.PriorityBlockingQueue;

import static org.junit.Assert.*;
import static org.mockito.Mockito.doNothing;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.mockStatic;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class DnsResolverTest {
    private static int TEST_SLOT = 1;
    private static int TEST_TTL = 100;
    private static String TEST_DOMAIN = "oliverll.com";

    @Mock private DnsQuery mMockQuery;
    @Mock private CountDownLatch mMockLatch;
    @Mock private static android.net.DnsResolver mMockDnsResolver;

    private static MockedStatic<android.net.DnsResolver> fwDnsResolver;

    /* target class of test */
    private DnsResolver defaultDnsResolver;
    DnsResolver.DnsResolverCallback dnsResolverCallback; // inner class

    @BeforeClass
    public static void beforeClass() {
        fwDnsResolver = mockStatic(android.net.DnsResolver.class);
    }

    @AfterClass
    public static void afterClass() {
        fwDnsResolver.close();
    }

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.openMocks(this);

        when(android.net.DnsResolver.getInstance()).thenReturn(mMockDnsResolver);

        defaultDnsResolver = new DnsResolver(TEST_SLOT);
        dnsResolverCallback = defaultDnsResolver
                .new DnsResolverCallback(mMockQuery, mMockLatch);
        doNothing().when(mMockLatch).countDown();
    }

    @After
    public void tearDown() throws Exception {
    }

    @Test
    public void destroy() {
        PriorityBlockingQueue<DnsNaptrRecord> naptrRecords = defaultDnsResolver.getNaptrRecords();
        naptrRecords.add(new DnsNaptrRecord(new TestDnsRecordExpiredCallback(), TEST_TTL, DnsNaptrRecordTest.input));

        // when
        defaultDnsResolver.destroy();

        // then
        assertEquals(0, naptrRecords.size());
    }

    @Test
    public void doReplacementLookup() {
        // given
        PriorityBlockingQueue<DnsNaptrRecord> naptrRecords = defaultDnsResolver.getNaptrRecords();
        naptrRecords.add(new DnsNaptrRecord(new TestDnsRecordExpiredCallback(), TEST_TTL, DnsNaptrRecordTest.input));

        // when
        defaultDnsResolver.doReplacementLookup(TEST_DOMAIN);
    }


    /* inner class of DnsResolverCallback */
    @Test
    public void onAnswer_with_DNS_RCODE_SUCCESS_withException() {
        // when
        dnsResolverCallback.onAnswer(new byte[] {1, 2, 3}, 0); // 0 is DNS_RCODE_SUCCESS

        // then
    }

    @Test
    public void isSuccessful() {
        assertEquals(true, dnsResolverCallback.isSuccessful());
    }

    @Test
    public void isSuccessfulWithErrorCode() {
        // given
        DnsResolver.DnsResolverCallback mCallback = defaultDnsResolver
                .new DnsResolverCallback(mMockQuery, mMockLatch);

        // when
        mCallback.onAnswer(new byte[] {1, 2, 3}, 10); // 10 is DNS_RCODE_NOT_ZONE

        // then
        assertEquals(false, mCallback.isSuccessful());
    }

    /* inner class */
    @Test
    public void execute() {
        // given
        DnsResolver.DnsResolverExecutor dnsResolverExecutor =
                defaultDnsResolver.new DnsResolverExecutor();
        Runnable mMockRunnable = mock(Runnable.class);

        // when
        dnsResolverExecutor.execute(mMockRunnable);

        // then
        verify(mMockRunnable, times(1)).run();
    }

    @Test
    public void onResourceExpired() {
        // given
        DnsResolver.DnsRecordExpiredCallback dnsRecordExpiredCallback =
                defaultDnsResolver.new DnsRecordExpiredCallback();
        DnsResourceRecord mMockDnsResourceRecord = mock(DnsResourceRecord.class);

        // when
        dnsRecordExpiredCallback.onResourceExpired(mMockDnsResourceRecord);
    }

    @Test
    public void onError() {
        android.net.DnsResolver.DnsException dnsException = mock(android.net.DnsResolver.DnsException.class);
        dnsResolverCallback.onError(dnsException);
    }


    /* for test */
    private class TestDnsRecordExpiredCallback implements DnsResourceRecord.DnsResourceCallback {
        public void onResourceExpired(DnsResourceRecord record) {

        }
    }
}