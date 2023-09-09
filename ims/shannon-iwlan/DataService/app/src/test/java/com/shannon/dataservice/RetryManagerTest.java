package com.shannon.dataservice;

import static org.mockito.Mockito.mockStatic;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;
import static org.junit.Assert.*;

import android.content.Context;
import android.content.Intent;

import com.shannon.dataservice.RetryManager.RetryBroadcastReceiver;

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

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class RetryManagerTest {
    private static final int testSlotId = 0;
    private static final int testPcoId = 0xff01;
    private static final ApnType testApnType = ApnType.APN_IMS;
    private static final String testApnIms = "IMS";
    private static final String testSupportedPcoList = "ff01,ff02";
    private static final String testRetryParameters = "name=TMO, N_fqdn=2, N_fast_try=3, N_max=6, "
                                                    + "T_fast=0, T_start=300";

    private AutoCloseable closeable;
    private RetryManager mRetryManager;

    @Mock private ConfigurationManager mMockConfigurationManager;
    @Mock private Context mMockContext;;
    @Mock private RetryHandler mMockRetryHandler;
    @Mock private SimManager mMockSimManager;

    private static MockedStatic<SimManager> mStaticSimManager;

    @BeforeClass
    public static void beforeClass() {
        mStaticSimManager = mockStatic(SimManager.class);
    }

    @AfterClass
    public static void afterClass() {
        mStaticSimManager.close();
    }

    @Before
    public void setUp() throws Exception {
        closeable = MockitoAnnotations.openMocks(this);
        when(SimManager.getInstance()).thenReturn(mMockSimManager);

        mRetryManager = new RetryManager(testSlotId, mMockContext, mMockConfigurationManager);
    }

    @After
    public void tearDown() throws Exception {
        closeable.close();
    }

    @Test
    public void test001_getApnRetryHandler() {
        //Given
        when(mMockConfigurationManager.getRetryParameters()).thenReturn(testRetryParameters);

        //When
        RetryHandler retryHandler = mRetryManager.getApnRetryHandler(testApnType,
                "ims-apn");

        //Then
        assertNotNull(retryHandler);
    }

    @Test
    public void test002_setPcoData() {
        //Given
        when(mMockConfigurationManager.getSupportedPcoIdList()).thenReturn(testSupportedPcoList);

        //When
        mRetryManager.setPcoData(testApnIms, testPcoId, new byte[10]);

        //Then
        verify(mMockConfigurationManager, times(1)).getSupportedPcoIdList();
    }

    @Test
    public void test003_RetryBroadcastReceiver() {
        //Given
        Intent intent = new Intent(Intent.ACTION_AIRPLANE_MODE_CHANGED);
        intent.putExtra("state", true);
        RetryBroadcastReceiver retryBroadcastReceiver = mRetryManager.getRetryBroadcastReceiver();
        mRetryManager.setApnMap(testApnType, mMockRetryHandler);

        //When
        retryBroadcastReceiver.onReceive(mMockContext, intent);

        //Then
        verify(mMockRetryHandler, times(1)).onAirplaneModeEnabled();
    }

    @Test
    public void test004_onWifiDisconnected() {
        //Given
        mRetryManager.setApnMap(testApnType, mMockRetryHandler);

        //When
        mRetryManager.onWifiDisconnected();

        //Then
        verify(mMockRetryHandler, times(1)).onWiFiDisconnected();
    }
}