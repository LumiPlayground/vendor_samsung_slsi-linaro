package com.shannon.qualifiednetworksservice;

import android.telephony.SubscriptionInfo;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;

import junit.framework.TestCase;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.Mock;
import org.mockito.Mockito;
import org.mockito.MockitoAnnotations;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.annotation.Config;

import static org.mockito.Mockito.when;

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class SimInformationTest extends TestCase {
    private SimInformation mInformation;

    @Mock
    SubscriptionManager mockSubscriptionManager;

    @Mock
    TelephonyManager mockTelephonyManager;

    @Mock
    SubscriptionInfo mockSubscriptionInfo;

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.openMocks(this);

        when(mockSubscriptionManager.getActiveSubscriptionInfoForSimSlotIndex(0)).thenReturn(mockSubscriptionInfo);
        when(mockSubscriptionInfo.getSubscriptionId()).thenReturn(5);
        when(mockSubscriptionInfo.getMcc()).thenReturn(310);
        when(mockSubscriptionInfo.getMnc()).thenReturn(260);
        when(mockTelephonyManager.createForSubscriptionId(5)).thenReturn(mockTelephonyManager);
        mInformation = SimInformation.createSimInformation(0, mockSubscriptionManager, mockTelephonyManager);
    }

    @After
    public void tearDown() throws Exception {
        Mockito.validateMockitoUsage();
    }

    @Test
    public void testCreateSimInformation() {
        when(mockSubscriptionManager.getActiveSubscriptionInfoForSimSlotIndex(0)).thenReturn(null);
        assertNull(SimInformation.createSimInformation(0, mockSubscriptionManager, mockTelephonyManager));

        when(mockSubscriptionManager.getActiveSubscriptionInfoForSimSlotIndex(0)).thenReturn(mockSubscriptionInfo);
        when(mockSubscriptionInfo.getSubscriptionId()).thenReturn(1);
        when(mockSubscriptionInfo.getMcc()).thenReturn(0);
        when(mockSubscriptionInfo.getMnc()).thenReturn(0);
        assertNull(SimInformation.createSimInformation(0, mockSubscriptionManager, mockTelephonyManager));

        when(mockSubscriptionInfo.getMcc()).thenReturn(1);
        when(mockTelephonyManager.createForSubscriptionId(1)).thenReturn(mockTelephonyManager);
        assertNotNull(SimInformation.createSimInformation(0, mockSubscriptionManager, mockTelephonyManager));
    }

    @Test
    public void testGetMcc() {
        assertEquals(310, mInformation.getMcc());
    }

    @Test
    public void testGetMnc() {
        assertEquals(260, mInformation.getMnc());
    }

    @Test
    public void testGetSubId() {
        assertEquals(5, mInformation.getSubId());
    }

    @Test
    public void testGetTelephonyManager() {
        assertEquals(mockTelephonyManager, mInformation.getTelephonyManager());
    }

    @Test
    public void testTestToString() {
        assertNotNull(mInformation.toString());
    }
}