package com.shannon.dataservice;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.MockedStatic;
import org.mockito.MockitoAnnotations;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.annotation.Config;

import java.util.ArrayList;

import vendor.samsung_slsi.telephony.hardware.radioExternal.V1_0.IOemSlsiRadioExternal;

import static org.junit.Assert.*;
import static org.mockito.Mockito.spy;

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class StringUtilsTest {

    private StringUtils mStringUtils;

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.openMocks(this);

        mStringUtils = new StringUtils();
    }

    @After
    public void tearDown() throws Exception {
    }

    @Test
    public void bytesToHexString() {
        // given
        byte[] data = {'1', 'B', '2', '3'};

        // when

        // then
        mStringUtils.bytesToHexString(data);
    }

    @Test
    public void hexStringToByteArray() {
        // given
        String hexString = "6B1C2D";

        // then
        mStringUtils.hexStringToByteArray(hexString);
    }

    @Test
    public void stringToNativeString() {
        // given
        String input = "test_input";
        String charset = "test_charset";

        // then
        mStringUtils.stringToNativeString(input, charset);
    }

    @Test
    public void stringToTBCD() {
        // TBCD means Telephony Binary-Coded Decimal.

        // given
        String examleTBCD = "0149293004885";

        String s = new String(mStringUtils.stringToTBCD(examleTBCD));
        for (int i = 0; i < s.length(); i++) {
            System.out.println("oliver4885 data : " + s.charAt(i));
        }
        // then
        // assertEquals("1", );
    }

    @Test
    public void primitiveArrayToArrayList() {
        // given
        byte[] data = {1, 1, 1};

        // then
        mStringUtils.primitiveArrayToArrayList(data);
    }

    @Test
    public void arrayListToPrimitiveArray() {
        // given
        ArrayList<Byte> arrayList = new ArrayList<>();
        arrayList.add(new Byte("1"));
        arrayList.add(new Byte("2"));
        arrayList.add(new Byte("3"));

        // then
        mStringUtils.arrayListToPrimitiveArray(arrayList);
    }

    @Test
    public void getByteAsInt() {
        // given
        byte[] data = {0x03};
        int offset = 0;

        // then
        assertEquals(3, mStringUtils.getByteAsInt(data, offset));
    }

    @Test
    public void getShortAsInt() {
        // given
        byte[] data = {0x00, 0x04};
        int offset = 0;

        // then
        assertEquals(4, mStringUtils.getShortAsInt(data, offset));
    }

    @Test
    public void getInt() {
        // given
        byte[] data = {0x00, 0x00, 0x00, 0x05};
        int offset = 0;

        // then
        assertEquals(5, mStringUtils.getInt(data, offset));
    }
}