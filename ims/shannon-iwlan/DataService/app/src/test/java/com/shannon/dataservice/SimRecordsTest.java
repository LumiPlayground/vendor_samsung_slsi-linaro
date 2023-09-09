package com.shannon.dataservice;

import static org.junit.Assert.*;
import static org.mockito.ArgumentMatchers.any;
import static org.mockito.ArgumentMatchers.anyInt;
import static org.mockito.ArgumentMatchers.anyString;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.mockStatic;
import static org.mockito.Mockito.mockitoSession;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

import android.telephony.TelephonyManager;

import com.android.internal.telephony.uicc.PlmnActRecord;
import com.shannon.dataservice.SimRecords.PreferredString;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.Mock;
import org.mockito.MockedStatic;
import org.mockito.MockitoAnnotations;
import org.mockito.MockitoSession;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.annotation.Config;

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class SimRecordsTest {
    private final byte[] TEST_BYTES = new byte[] {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
            0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E};
    private MockitoSession mStaticMockSession;
    private SimRecords mSimRecords;

    private static MockedStatic<PlmnActRecord> mStaticPlmnActRecord;
    private AutoCloseable closeable;

    @Mock private TelephonyManager mMockTelephonyManager;

    @BeforeClass
    public static void beforeClass() {
        mStaticPlmnActRecord = mockStatic(PlmnActRecord.class);
    }

    @AfterClass
    public static void afterClass() {
        mStaticPlmnActRecord.close();
    }

    @Before
    public void setUp() throws Exception {
        closeable = MockitoAnnotations.openMocks(this);
        mStaticMockSession = mockitoSession().startMocking();

        mMockTelephonyManager = mock(TelephonyManager.class);
        when(mMockTelephonyManager.iccExchangeSimIO(anyInt(), anyInt(),
                anyInt(), anyInt(), anyInt(), anyString())).thenReturn(TEST_BYTES);

        mSimRecords = new SimRecords(TEST_VALUES.SLOT_ID_1, mMockTelephonyManager);
    }

    @After
    public void tearDown() throws Exception {
        mStaticMockSession.finishMocking();
        closeable.close();
    }

    @Test
    public void getEfEhplmn() {
        //When
        String[] strings = mSimRecords.getEfEhplmn();

        //Then
        assertNotNull(strings);
    }


    @Test
    public void getEfOplmn() {
        //When
        PlmnActRecord[] plmnActRecords = new PlmnActRecord[10];
        for(int i = 0; i < 10; i++) {
            plmnActRecords[i] = new PlmnActRecord(TEST_BYTES, 0);
        }
        when(PlmnActRecord.getRecords(any())).thenReturn(plmnActRecords);
        mSimRecords.getEfOplmn();

        //Then
        verify(mMockTelephonyManager, times(10)).iccExchangeSimIO(anyInt(), anyInt(), anyInt(),
                anyInt(), anyInt(), anyString());
    }

    @Test
    public void getEfEpdgId() {
        //When
        mSimRecords.getEfEpdgId();

        //Then
        verify(mMockTelephonyManager, times(10)).iccExchangeSimIO(anyInt(), anyInt(), anyInt(),
                anyInt(), anyInt(), anyString());
    }

    @Test
    public void getEfEpdgSelection() {
        //When
        mSimRecords.getEfEpdgSelection();

        //Then
        verify(mMockTelephonyManager, times(10)).iccExchangeSimIO(anyInt(), anyInt(), anyInt(),
                anyInt(), anyInt(), anyString());
    }

    @Test
    public void getEfEpdgIdEmergency() {
        //When
        mSimRecords.getEfEpdgIdEmergency();

        //Then
        verify(mMockTelephonyManager, times(10)).iccExchangeSimIO(anyInt(), anyInt(), anyInt(),
                anyInt(), anyInt(), anyString());
    }

    @Test
    public void getEfEpdgSelectionEmergency() {
        //When
        mSimRecords.getEfEpdgSelectionEmergency();

        //Then
        verify(mMockTelephonyManager, times(10)).iccExchangeSimIO(anyInt(), anyInt(), anyInt(),
                anyInt(), anyInt(), anyString());
    }

    @Test
    public void getIwlanPlmn() {
        //When
        String string = mSimRecords.getIwlanPlmn();

        //Then
        assertNotNull(string);
    }

    @Test
    public void testPreferredString() {
        PreferredString preferredString =
                mSimRecords.new PreferredString(TEST_VALUES.ADAPTER_NAME, TEST_VALUES.NETWORK_ID);

        PreferredString preferredStringNew =
                mSimRecords.new PreferredString(TEST_VALUES.ADAPTER_NAME, TEST_VALUES.NETWORK_ID);
        assertEquals(TEST_VALUES.ADAPTER_NAME, preferredString.getValue());
        assertEquals(0, preferredString.compareTo(preferredStringNew));
    }
}