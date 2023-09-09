package com.shannon.dataservice;

import static org.mockito.ArgumentMatchers.eq;
import static org.mockito.Mockito.mockitoSession;
import static org.mockito.Mockito.when;

import android.content.Context;
import android.telephony.TelephonyManager;

import junit.framework.TestCase;

import org.junit.After;
import org.junit.Assert;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.Mock;
import org.mockito.Mockito;
import org.mockito.MockitoAnnotations;
import org.mockito.MockitoSession;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.annotation.Config;

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class MccMncTest extends TestCase {

    @Mock private Context mMockContext;
    @Mock private TelephonyManager mMockTelephonyManager;
    MockitoSession mStaticMockSession;

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.openMocks(this);
        mStaticMockSession = mockitoSession().startMocking();

        when(mMockContext.getSystemService(eq(TelephonyManager.class)))
                .thenReturn(mMockTelephonyManager);
        when(mMockTelephonyManager.getSimOperator())
                .thenReturn("310410");

    }

    @After
    public void tearDown() throws Exception {
        mStaticMockSession.finishMocking();
    }

    @Test
    public void testMcc() {
        MccMnc mccMnc = new MccMnc(mMockTelephonyManager.getSimOperator());
        int mcc = mccMnc.getMcc();
        Assert.assertEquals(mcc, 310);
    }

    @Test
    public void testMnc() {
        MccMnc mccMnc = new MccMnc(mMockTelephonyManager.getSimOperator());
        int mnc = mccMnc.getMnc();
        Assert.assertEquals(mnc, 410);
    }

    @Test
    public void testEquals() {
        MccMnc mMccMnc1 = new MccMnc(380, 160);
        MccMnc mMccMnc2 = new MccMnc(380, 160);

        assertEquals(true, mMccMnc1.equals(mMccMnc2));
    }

    @Test
    public void testHashCode() {
        MccMnc mMccMnc1 = new MccMnc(380, 160);
        MccMnc mMccMnc2 = new MccMnc(380, 160);

        assertEquals(true, mMccMnc1.hashCode() == mMccMnc2.hashCode());
    }

    @Test
    public void testToString() {
        MccMnc mMccMnc1 = new MccMnc(380, 160);

        assertEquals(true, mMccMnc1.toString().equals("MCC=380 MNC=160"));
    }
}