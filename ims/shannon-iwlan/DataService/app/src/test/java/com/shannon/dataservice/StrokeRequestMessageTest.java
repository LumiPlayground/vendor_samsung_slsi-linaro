package com.shannon.dataservice;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.MockitoAnnotations;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.annotation.Config;

import static com.shannon.dataservice.StrokeRequestMessage.RequestMessageType.STR_REQ_ADD_CA;
import static org.junit.Assert.*;

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class StrokeRequestMessageTest {

    private StrokeRequestMessage defaultStrokeRequestMessage;

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.openMocks(this);

        /* default setting */
        defaultStrokeRequestMessage = new StrokeRequestMessage(STR_REQ_ADD_CA);
    }

    @After
    public void tearDown() throws Exception {
    }

    @Test
    public void getBytes() {
        defaultStrokeRequestMessage.getBytes();
    }

    @Test
    public void setMinimumLength() {
        defaultStrokeRequestMessage.setMinimumLength();
    }

    @Test
    public void align() {
        defaultStrokeRequestMessage.align();
    }

    @Test
    public void alignWithParameter() {
        StrokeRequestMessage requestMessage = new StrokeRequestMessage(STR_REQ_ADD_CA);
        int previous = requestMessage.getTotalLength();

        //when
        requestMessage.align(8);

        // then
        // although, align as 8,
        // result is added by 4. because, that
        assertEquals(previous + 4, requestMessage.getTotalLength());
    }

    @Test
    public void putByte() {
        // given
        StrokeRequestMessage requestMessage = new StrokeRequestMessage(STR_REQ_ADD_CA);
        int previous = requestMessage.getTotalLength();

        // when
        requestMessage.putByte("BLABLA_ATTRIBUTE", 100);

        // then
        assertEquals(previous + 1, requestMessage.getTotalLength());
    }

    @Test
    public void putShort() {
        // given
        StrokeRequestMessage requestMessage = new StrokeRequestMessage(STR_REQ_ADD_CA);
        int previous = requestMessage.getTotalLength();

        // when
        requestMessage.putShort("BLABLA_ATTRIBUTE", 0x000F);

        // then
        assertEquals(previous + 2, requestMessage.getTotalLength());
    }

    @Test
    public void putInt() {
        // given
        StrokeRequestMessage requestMessage = new StrokeRequestMessage(STR_REQ_ADD_CA);
        int previous = requestMessage.getTotalLength();

        // when
        requestMessage.putInt("BLABLA_ATTRIBUTE", 0x0000000F);

        // then
        assertEquals(previous + 4, requestMessage.getTotalLength());
    }

    @Test
    public void putLong() {
        // given
        StrokeRequestMessage requestMessage = new StrokeRequestMessage(STR_REQ_ADD_CA);
        int previous = requestMessage.getTotalLength();

        // when
        requestMessage.putLong("BLABLA_ATTRIBUTE", (long) 0x0F);

        // then
        // because of padding, should be added by 4
        assertEquals(previous + 8 + 4, requestMessage.getTotalLength());
    }

    @Test
    public void putLongOrInt() {
        // given
        StrokeRequestMessage requestMessage = new StrokeRequestMessage(STR_REQ_ADD_CA);
        int previous = requestMessage.getTotalLength();

        // when
        requestMessage.putLongOrInt("BLABLA_ATTRIBUTE", 0x0F);

        // then
        // current env is PROTO64
        // because of padding, should be added by 4
        assertEquals(previous + 8 + 4, requestMessage.getTotalLength());
    }

    @Test
    public void loge() {
        defaultStrokeRequestMessage.loge("test loge");
    }
}