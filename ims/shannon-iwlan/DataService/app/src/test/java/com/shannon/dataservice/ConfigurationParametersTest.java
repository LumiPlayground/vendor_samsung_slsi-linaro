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
public class ConfigurationParametersTest {
    ConfigurationParameters mConfigurationParameters;

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.openMocks(this);

        mConfigurationParameters = new ConfigurationParameters();
    }

    @After
    public void tearDown() throws Exception {
    }

    @Test
    public void testToString() {
        assertEquals("Configuration parameters homeFQDNs: 3gpp-operator:sim " +
                "IkeAttrPcscf4: 20 IkeAttrPcscf6: 21 " +
                "IkeDscp: 0 IKE lifetime: 64800 ESP " +
                "lifetime: 64800 DPD interval: 60 " +
                "Keep-alive: 20 IKE retrans timeout: 1 " +
                "IKE retrans base: 2 " +
                "IKE algorithms: aes128-aes256-aes128ctr-aes256" +
                "ctr-des-3des-null-md5-sha1-aesxcbc-sha256-prfmd5-prfsha1-prfaesxcbc-prfsha256-modp1024-modp1536-modp2048!" +
                " ESP algorithms: aes128-aes192-aes256-aes128ctr-aes192ctr-aes256ctr-des-3des-null-md5-sha1-aesxcbc-sha256! " +
                "EMC APN name: sos PMTU Discovery: true", mConfigurationParameters.toString());
    }
}