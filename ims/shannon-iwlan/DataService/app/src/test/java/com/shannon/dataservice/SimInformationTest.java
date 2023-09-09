package com.shannon.dataservice;

import android.content.Context;
import android.telephony.SubscriptionInfo;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;

import androidx.test.platform.app.InstrumentationRegistry;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.Mock;
import org.mockito.MockedStatic;
import org.mockito.Mockito;
import org.mockito.MockitoAnnotations;
import org.mockito.MockitoSession;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.annotation.Config;

import static org.junit.Assert.*;
import static org.junit.Assert.assertEquals;
import static org.mockito.ArgumentMatchers.any;
import static org.mockito.ArgumentMatchers.anyInt;
import static org.mockito.ArgumentMatchers.eq;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.mockitoSession;
import static org.mockito.Mockito.spy;
import static org.mockito.Mockito.when;

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class SimInformationTest {
    private SimInformation mSimInformation;
    private SimInformation mSpySimInformation;

    @Mock private Context mMockContext;
    @Mock private SubscriptionManager mMockSubscriptionManager;
    @Mock private TelephonyManager mMockTelephonyManager;
    @Mock private SubscriptionInfo mMockSubscriptionInfo;

    MockitoSession mStaticMockSession;

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.openMocks(this);
        mStaticMockSession = mockitoSession().startMocking();

        when(mMockContext.getSystemService(eq(TelephonyManager.class)))
                .thenReturn(mMockTelephonyManager);
        when(mMockTelephonyManager.createForSubscriptionId(eq(TEST_VALUES.SUB_ID_1)))
                .thenReturn(mMockTelephonyManager);
        when(mMockSubscriptionManager.getSubscriptionIds(TEST_VALUES.SLOT_ID_1))
                .thenReturn(new int[] {TEST_VALUES.SUB_ID_1, 0, 0});
        when(mMockSubscriptionManager.getActiveSubscriptionInfo(TEST_VALUES.SUB_ID_1))
                .thenReturn(mMockSubscriptionInfo);
        when(mMockSubscriptionInfo.getMcc()).thenReturn(TEST_VALUES.MCC);
        when(mMockSubscriptionInfo.getMnc()).thenReturn(TEST_VALUES.MNC);

        mSimInformation = SimInformation.createSimInformation(TEST_VALUES.SLOT_ID_1,
                mMockSubscriptionManager,
                mMockTelephonyManager
        );

        mSpySimInformation = spy(mSimInformation);

        when(mMockSubscriptionManager.getSubscriptionIds(TEST_VALUES.SLOT_ID_1))
                .thenReturn(new int[]{TelephonyManager.SIM_STATE_READY});
    }

    @After
    public void tearDown() throws Exception {
        mStaticMockSession.finishMocking();
    }

    @Test
    public void getMcc() {
        int mcc = mSimInformation.getMcc();
        assertEquals(mcc, TEST_VALUES.MCC);
    }

    @Test
    public void getMnc() {
        int mnc = mSimInformation.getMnc();
        assertEquals(mnc, TEST_VALUES.MNC);
    }

    @Test
    public void getSubId() {
        mSpySimInformation.getSubId();
    }

    @Test
    public void getlmsi() {
        assertNull(mSpySimInformation.getImsi());
    }

    @Test
    public void testToString() {
        assertEquals("SIM information for slot 0 subId=3, MCC=310 MNC=80 IMSI:null", mSpySimInformation.toString());
    }

    @Test
    public void getTelephonyManager() {
        // when
        TelephonyManager telephonyManager = mSpySimInformation.getTelephonyManager();

        // then
        assertNotNull(telephonyManager);
    }

    @Test
    public void  getRecords() {
        // when
        SimRecords records = mSpySimInformation.getRecords();

        // then
        assertNotNull(records);
    }

    @Test
    public void isIsimCard() {
        // when
        boolean isimCard = mSpySimInformation.isIsimCard();

        // then
        assertEquals(false, isimCard);
    }
}