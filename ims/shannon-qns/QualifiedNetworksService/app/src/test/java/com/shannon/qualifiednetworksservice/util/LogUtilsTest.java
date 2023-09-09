package com.shannon.qualifiednetworksservice.util;

import static com.shannon.qualifiednetworksservice.util.LogUtils.LOG_CONDITIONS;
import static org.mockito.ArgumentMatchers.anyInt;
import static org.mockito.ArgumentMatchers.anyString;
import static org.mockito.Mockito.mockStatic;

import android.telephony.Rlog;
import android.telephony.data.ApnSetting;

import junit.framework.TestCase;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.MockedStatic;
import org.mockito.MockitoAnnotations;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.annotation.Config;

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class LogUtilsTest extends TestCase {
    private static final String TAG = "LogUtilsTest";
    private static final String MSG = "testMsg";

    private static MockedStatic<Rlog> sRlog;

    @BeforeClass
    public static void beforeClass() {
        sRlog = mockStatic(Rlog.class);
    }

    @AfterClass
    public static void afterClass() {
        sRlog.close();
    }

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.openMocks(this);
        sRlog.when(()->Rlog.isLoggable(anyString(), anyInt())).thenReturn(true);

        LogUtils logUtils = new LogUtils();
    }

    @After
    public void tearDown() throws Exception {
    }

    @Test
    public void testV() {
        LogUtils.v(TAG, MSG);
        LogUtils.v(TAG, ApnSetting.TYPE_IMS, MSG);
        LogUtils.v(TAG, ApnSetting.TYPE_EMERGENCY, MSG);
        LogUtils.v(TAG, ApnSetting.TYPE_MMS, MSG);
        LogUtils.v(TAG, ApnSetting.TYPE_XCAP, MSG);
        LogUtils.v(TAG, ApnSetting.TYPE_CBS, MSG);
        LogUtils.v(TAG, LOG_CONDITIONS, MSG);
        LogUtils.v(TAG, ApnSetting.TYPE_BIP, MSG);
    }

    @Test
    public void testD() {
        LogUtils.d(TAG, MSG);
        LogUtils.d(TAG, ApnSetting.TYPE_IMS, MSG);
    }

    @Test
    public void testI() {
        LogUtils.i(TAG, MSG);
        LogUtils.i(TAG, ApnSetting.TYPE_IMS, MSG);
    }

    @Test
    public void testW() {
        LogUtils.w(TAG, MSG);
    }

    @Test
    public void testE() {
        LogUtils.e(TAG, MSG);
    }
}