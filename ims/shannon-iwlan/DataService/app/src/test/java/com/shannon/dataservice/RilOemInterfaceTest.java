package com.shannon.dataservice;

import static org.junit.Assert.*;
import static org.mockito.Mockito.mockStatic;
import static org.mockito.Mockito.when;

import android.util.SparseArray;

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

import vendor.samsung_slsi.telephony.hardware.radioExternal.V1_0.IOemSlsiRadioExternal;

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class RilOemInterfaceTest {
    private static final int testSlotId = 0;

    private AutoCloseable mCloseable;
    private RilOemInterface mRilOemInterface;
    private RilOemResponseCallback mRilOemResponseCallback;
    private RilSolicitedMessage mRilSolicitedMessage;
    private SparseArray mSparseArray;

    @Mock private IOemSlsiRadioExternal mMockOemRil;
    @Mock private RilOemInterface mMockRilOemInterface;

    @BeforeClass
    public static void beforeClass() {
        sIOemSlsiRadioExternalMockedStatic = mockStatic(IOemSlsiRadioExternal.class);
    }

    @AfterClass
    public static void afterClass() {
        sIOemSlsiRadioExternalMockedStatic.close();
    }

    private static MockedStatic<IOemSlsiRadioExternal> sIOemSlsiRadioExternalMockedStatic;

    @Before
    public void setUp() throws Exception {
        mCloseable = MockitoAnnotations.openMocks(this);
        mRilSolicitedMessage = new RilGetAuthenticationResponse(mMockRilOemInterface,
                new byte[] {0x01},
                true
        );
        mSparseArray = new SparseArray<RilSolicitedMessage>();
        when(IOemSlsiRadioExternal.getService("rilExternal")).thenReturn(mMockOemRil);
        mRilOemInterface = new RilOemInterface(testSlotId, mSparseArray, mRilOemResponseCallback);
    }

    @After
    public void tearDown() throws Exception {
        mCloseable.close();
    }

    @Test
    public void test001_destroy() {
        mRilOemInterface.destroy();
    }

    @Test
    public void test002_onResponse() {
        //Given
        byte[] data = {0x01, 0x02};
        mRilOemInterface.onResponse(0, 0, data, data.length);
    }

    @Test
    public void test003_sendRequest() {
        mRilSolicitedMessage.onResponse(1, new byte[] {0x00, 0x00, 0x00, 0x00}, 4);
        mRilOemInterface.sendRequest(mRilSolicitedMessage);
    }

    @Test
    public void test004_getAuthenticationResponse() {
        mRilOemInterface.getAuthenticationResponse(new byte[10], false);
    }

    /* inner class */
    @Test
    public void serviceDied() {
        mRilOemInterface.getDeathLink().serviceDied(2L);
    }
}