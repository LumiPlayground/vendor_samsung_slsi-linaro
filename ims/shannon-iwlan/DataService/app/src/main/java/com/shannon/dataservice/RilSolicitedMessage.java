/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or
 * distributed, transmitted, transcribed, stored in a retrieval system or
 * translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed to third parties
 * without the express written permission of Samsung Electronics.
 */
package com.shannon.dataservice;

import java.util.concurrent.CountDownLatch;
import java.util.concurrent.TimeUnit;

import static com.shannon.dataservice.RilOemInterface.RIL_RESPONSE_SUCCESS;

public class RilSolicitedMessage {
    public static final int RIL_GET_AUTHENTICATION_RESPONSE = 32;

    private final RilOemInterface mRilInterface;
    private final int mMessageId;
    private final CountDownLatch mOnResponseSignal;
    protected byte[] mRawData;
    private byte[] mResponseData;

    RilSolicitedMessage(int id, RilOemInterface oemInterface) {
        mMessageId = id;
        mRilInterface = oemInterface;
        mOnResponseSignal = new CountDownLatch(1);
        mRawData = new byte[0];
        mResponseData = null;
    }

    public byte[] request() {
        mRilInterface.sendRequest(this);
        try {
            mOnResponseSignal.await(5, TimeUnit.SECONDS);
        } catch (Exception e) {
            // nothing
        }
        return mResponseData;
    }

    public void requestNoResponse() {
        mRilInterface.sendRequest(this);
    }

    public void onResponse(int error, byte[] data, int dataLen) {
        if (error == RIL_RESPONSE_SUCCESS) {
            mResponseData = data;
        }
        mOnResponseSignal.countDown();
    }

    protected void setData(byte ... params) {
        mRawData = new byte[params.length];
        for (int i = 0; i < params.length; i++) {
            mRawData[i] = params[i];
        }
    }

    protected byte toByte(int value) {
        return (byte)(value & 0xFF);
    }

    public int getMessageId() { return mMessageId; }
    public byte[] getData() {  return mRawData; }
    public int getDataLength() { return mRawData.length; }
}
