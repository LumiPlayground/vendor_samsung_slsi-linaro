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

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class RilSetRtpThreshold extends RilSolicitedMessage {
    private static final int LENGTH = 8;
    private final int mCallId;              // 1 byte
    private final int mJitter;              // 4 bytes
    private final int mRtpThreshold;       // 1 byte
    private final int mRtpInterval;        // 1 byte
    private final int mNoRtpInterval;      // 1 byte

    public RilSetRtpThreshold(RilOemInterface oemInterface, int callId, int rtpThreshold, int rtpInterval, int jitter, int noRtpInterval) {
        super(RIL_SET_RTP_THRESHOLD, oemInterface);
        mCallId = callId;
        mJitter = jitter;
        mRtpThreshold = rtpThreshold;
        mRtpInterval = rtpInterval;
        mNoRtpInterval = noRtpInterval;

        ByteBuffer buffer = ByteBuffer.allocate(LENGTH);
        buffer.order(ByteOrder.LITTLE_ENDIAN);
        buffer.put(Integer.valueOf(mCallId).byteValue());
        buffer.putInt(mJitter);
        buffer.put(Integer.valueOf(mRtpThreshold).byteValue());
        buffer.put(Integer.valueOf(mRtpInterval).byteValue());
        buffer.put(Integer.valueOf(mNoRtpInterval).byteValue());
        setData(buffer.array());
    }

    @Override
    public String toString() {
        return ("RIL_SET_RTP_THRESHOLD {" + mCallId + "," + mJitter + "," + mRtpThreshold + "," + mRtpInterval + "," + mNoRtpInterval + "}");
    }

    @VisibleForTesting int getCallId() { return mCallId; };
    @VisibleForTesting int getJitter() { return mJitter; };
    @VisibleForTesting int getRtpThreshold() { return mRtpThreshold; };
    @VisibleForTesting int getRtpInterval() { return mRtpInterval; };
    @VisibleForTesting int getNoRtpInterval() { return mNoRtpInterval; };
}
