/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or
 * distributed, transmitted, transcribed, stored in a retrieval system or
 * translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed to third parties
 * without the express written permission of Samsung Electronics.
 */
package com.shannon.qualifiednetworksservice;

public class RilUnsolicitedCallModified extends RilUnsolicitedMessage {
    private final int mId;
    private final boolean mVideoEnabled;

    public RilUnsolicitedCallModified(RilUnsolicitedMessage from) {
        super(from);

        byte[] bytes = getData();
        mId = bytes[0];
        if (bytes[3] == 0) {
            int direction = getShort(bytes[1], bytes[2]);
            mVideoEnabled = ((direction & 0xF0) != 0);
        } else {
            mVideoEnabled = false;
        }
    }

    public int getCallId() { return mId; }
    public boolean isVideoCall() { return mVideoEnabled; }

    @Override
    public String toString() {
        return ("RIL_UNSOLICITED_CALL_MODIFIED {" + mId + ", v:" + mVideoEnabled + "}");
    }
}
