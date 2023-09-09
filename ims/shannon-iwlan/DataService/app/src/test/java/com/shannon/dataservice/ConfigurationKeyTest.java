package com.shannon.dataservice;

import static com.shannon.dataservice.ConfigurationKey.CFG_IKE_ATTRIBUTE_PCSCF4;
import static com.shannon.dataservice.ConfigurationParameters.KEY_EPDG_IKE_ATTRIBUTE_PCSCFv4;

import junit.framework.TestCase;

import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.annotation.Config;

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class ConfigurationKeyTest extends TestCase {
    ConfigurationParameters mConfigurationParameters;
    private final static int P_CSCF_ATTRIBUTE_TYPE_IPV4 = 16384;

    @Before
    public void setUp() {
        mConfigurationParameters = new ConfigurationParameters();
    }

    @Test
    public void test001_getKeyName() {
        /* Check only one of the following as a representative */
        assertEquals(CFG_IKE_ATTRIBUTE_PCSCF4.getKeyName(), KEY_EPDG_IKE_ATTRIBUTE_PCSCFv4);
    }

    @Test
    public void test002_setToObject() {
        /* Check only one of the following as a representative */
        ConfigurationKey key = CFG_IKE_ATTRIBUTE_PCSCF4;
        key.setToObject(mConfigurationParameters, P_CSCF_ATTRIBUTE_TYPE_IPV4);
        assertEquals(mConfigurationParameters.mIkeAttrPcscf4, P_CSCF_ATTRIBUTE_TYPE_IPV4);
    }
}