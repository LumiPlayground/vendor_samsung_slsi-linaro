package com.shannon.dataservice;

import android.content.Context;
import android.telephony.TelephonyManager;

import androidx.test.platform.app.InstrumentationRegistry;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.Mock;
import org.mockito.MockitoAnnotations;
import org.mockito.MockitoSession;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.annotation.Config;

import java.net.InetAddress;

import static org.junit.Assert.*;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.mockitoSession;
import static org.mockito.Mockito.when;

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class IkeConfigurationTest {
    private static final int TEST_TYPE = 123;
    private static final String TEST_IMEI = "100410041004";

    private MockitoSession mStaticMockSession;

    private IkeConfiguration mIkeConfiguration;
    private String testString = "test";

    private Context mContext;

    @Before
    public void setUp() throws Exception {
        System.out.println("IkeDaemonInterfaceTest");

        MockitoAnnotations.openMocks(this);
        mStaticMockSession = mockitoSession().startMocking();
        mContext = InstrumentationRegistry.getInstrumentation().getContext();

        mIkeConfiguration = new IkeConfiguration(testString);
    }

    @After
    public void tearDown() throws Exception {
        mStaticMockSession.finishMocking();
    }

    @Test
    public void addPcscfAttribute() {
        InetAddress mInetAddress = mock(InetAddress.class);
        when(mInetAddress.getAddress()).thenReturn(new byte[] {1,1,1,2,2,2});

        mIkeConfiguration.addPcscfAttribute(TEST_TYPE, mInetAddress);
    }

    @Test
    public void addImeiAttribute() {
        // when
        mIkeConfiguration.addImeiAttribute(TEST_TYPE, TEST_IMEI);

        // then
        assertEquals(1, mIkeConfiguration.getSendList().size());

        // after
        mIkeConfiguration.getSendList().clear();
    }

    @Test
    public void addDeviceIdentityAttribute() {
        // when
        mIkeConfiguration.addDeviceIdentityAttribute(TEST_IMEI);

        // then
        assertEquals(1, mIkeConfiguration.getSendList().size());

        // after
        mIkeConfiguration.getSendList().clear();
    }

    @Test
    public void addLivenessCheckAttribute() {
        // when
        mIkeConfiguration.addLivenessCheckAttribute();

        // then
        assertEquals(1, mIkeConfiguration.getSendList().size());

        // after
        mIkeConfiguration.getSendList().clear();
    }

    @Test
    public void addExpectedAttribute() {
        // when
        mIkeConfiguration.addExpectedAttribute(TEST_TYPE);

        // then
        assertEquals(1, mIkeConfiguration.getExpectedList().size());

        // after
        mIkeConfiguration.getExpectedList().clear();
    }

    @Test
    public void getOutgoingAttributesBytesWithEmpty() {
        assertEquals(null, mIkeConfiguration.getOutgoingAttributesBytes());
    }

    @Test
    public void getOutgoingAttributesBytes() {
        // when
        mIkeConfiguration.addLivenessCheckAttribute();

        // then
        assertNotNull(mIkeConfiguration.getOutgoingAttributesBytes());

        // clear
        mIkeConfiguration.getSendList().clear();
    }

    @Test
    public void getExpectedAttributesBytesWithEmpty() {
        assertEquals(null, mIkeConfiguration.getExpectedAttributesBytes());
    }

    @Test
    public void getExpectedAttributesBytes() {
        // when
        mIkeConfiguration.addExpectedAttribute(TEST_TYPE);

        // then
        assertNotNull(mIkeConfiguration.getExpectedAttributesBytes());

        // after
        mIkeConfiguration.getExpectedList().clear();
    }

    @Test
    public void processReceivedAttributesWithNull() {
        mIkeConfiguration.processReceivedAttributes(null);
    }

    @Test
    public void processReceivedAttributes() {
        mIkeConfiguration.processReceivedAttributes(new byte[]{1,1,1,2,2,2});
    }

    @Test
    public void getIpConfigurationAttributesWithEmpty() {
        // then
        assertEquals(0, mIkeConfiguration.getIpConfigurationAttributes(TEST_TYPE).size());
    }

    @Test
    public void getIpConfigurationAttributes() {
        // when
        mIkeConfiguration.getReceiveList().add(new IkeConfigurationAttribute(
                TEST_TYPE, new byte[] {1,2,3})
        );

        // then
        assertEquals(1, mIkeConfiguration.getIpConfigurationAttributes(TEST_TYPE).size());

        // after
        mIkeConfiguration.getReceiveList().clear();
    }
}