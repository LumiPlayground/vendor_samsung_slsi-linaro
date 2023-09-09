
package com.shannon.dataservice;

import android.telephony.data.ApnSetting;

import junit.framework.TestCase;

import org.junit.Before;
import org.junit.Test;

public class ApnTypeTest extends TestCase {
    private static final int DEFAULT_SLOT_INDEX = 0;

    /*
    APN_DEFAULT(1, ApnSetting.TYPE_DEFAULT, "default", 0),
    APN_MMS(3, ApnSetting.TYPE_MMS, "mms", 0),
    APN_SUPL(5, ApnSetting.TYPE_SUPL, "supl", 0),
    APN_DUN(7, ApnSetting.TYPE_DUN, "dun", 0),
    APN_HIPRI(9, 0, "hipri", 0),
    APN_FOTA(11, ApnSetting.TYPE_FOTA, "fota", 0),
    APN_IMS(13, ApnSetting.TYPE_IMS, "ims", 2),
    APN_CBS(15, ApnSetting.TYPE_CBS, "cbs", 0),
    APN_IA(17, 0, "ia", 0),
    APN_EMERGENCY(19, ApnSetting.TYPE_EMERGENCY, "emergency", 1),
    APN_MCX(21, 0, "mcx", 0),
    APN_XCAP(23, ApnSetting.TYPE_XCAP, "xcap", 0);
 */
    ApnType mApnTypeDefault;
    ApnType mApnTypeIms;
    ApnType mApnTypeEmergency;
    ApnType mApnTypeXcap;

    @Before
    public void setUp() throws Exception {
        mApnTypeDefault = ApnType.getByBitmask(ApnSetting.TYPE_DEFAULT);
        mApnTypeIms = ApnType.getByBitmask(ApnSetting.TYPE_IMS);
        mApnTypeEmergency = ApnType.getByBitmask(ApnSetting.TYPE_EMERGENCY);
        mApnTypeXcap = ApnType.getByBitmask(ApnSetting.TYPE_XCAP);
    }

    @Test
    public void test001_getCode() {
        assertEquals(1, mApnTypeDefault.getCode(DEFAULT_SLOT_INDEX));
        assertEquals(13, mApnTypeIms.getCode(DEFAULT_SLOT_INDEX));
        assertEquals(19, mApnTypeEmergency.getCode(DEFAULT_SLOT_INDEX));
        assertEquals(23, mApnTypeXcap.getCode(DEFAULT_SLOT_INDEX));
    }

    @Test
    public void test002_getBit() {
        assertEquals(ApnSetting.TYPE_DEFAULT, mApnTypeDefault.getBit());
        assertEquals(ApnSetting.TYPE_IMS, mApnTypeIms.getBit());
        assertEquals(ApnSetting.TYPE_EMERGENCY, mApnTypeEmergency.getBit());
        assertEquals(ApnSetting.TYPE_XCAP, mApnTypeXcap.getBit());
    }

    @Test
    public void test003_getType() {
        assertEquals(0, mApnTypeDefault.getType());
        assertEquals(2, mApnTypeIms.getType());
        assertEquals(1, mApnTypeEmergency.getType());
        assertEquals(0, mApnTypeXcap.getType());
    }

    @Test
    public void test004_getByCode() {
        assertEquals(mApnTypeDefault, ApnType.getByCode(1));
        assertEquals(mApnTypeIms, ApnType.getByCode(13));
        assertEquals(mApnTypeEmergency, ApnType.getByCode(19));
        assertEquals(mApnTypeXcap, ApnType.getByCode(23));
    }
}