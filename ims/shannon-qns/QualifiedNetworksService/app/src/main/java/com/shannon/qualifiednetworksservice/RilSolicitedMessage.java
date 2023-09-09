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
package com.shannon.qualifiednetworksservice;

import com.android.internal.annotations.VisibleForTesting;

public class RilSolicitedMessage {
    public static final int RIL_SET_RTP_THRESHOLD = 148;
    public static final int RIL_SET_CELL_THRESHOLD = 153;

    private final RilOemInterface mRilInterface;
    private final int mMessageId;
    private byte[] mRawData;

    public RilSolicitedMessage(int id, RilOemInterface oemInterface) {
        mMessageId = id;
        mRilInterface = oemInterface;
        mRawData = new byte[0];
    }

    public RilSolicitedMessage(RilSolicitedMessage from) {
        mMessageId = from.mMessageId;
        mRilInterface = from.mRilInterface;
        mRawData = new byte[0];
    }

    public void request() {
        mRilInterface.sendRequest(this);
    }

    public void onResponse(int error, byte[] data, int dataLen) {
    }

    protected void setData(byte[] bytes) {
        mRawData = bytes;
    }

    public int getMessageId() { return mMessageId; }
    public byte[] getData() {  return mRawData; }
    public int getDataLength() { return mRawData.length; }

    @VisibleForTesting byte[] getRawData() { return mRawData; };
    @VisibleForTesting void setRawData(byte[] rawData) { mRawData = rawData; };
}