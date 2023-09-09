/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or
 * distributed, transmitted, transcribed, stored in a retrieval system or
 * translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed to third parties
 * without the express written permission of Samsung Electronics.
 */
package com.shannon.qualifiednetworksservice;

import com.shannon.qualifiednetworksservice.BarringInfo.BarringServiceType;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.Arrays;

public class RilUnsolicitedBarring extends RilUnsolicitedMessage {
    private BarringInfo[] mBarringInfo;

    public RilUnsolicitedBarring(RilUnsolicitedMessage from) {
        super(from);
        ByteBuffer mBuffer = ByteBuffer.wrap(getData());
        mBuffer.order(ByteOrder.LITTLE_ENDIAN);
        int numOfBarringInfo = mBuffer.getInt();
        if (numOfBarringInfo > 0) {
            mBarringInfo = new BarringInfo[numOfBarringInfo];
            for (int i = 0; i < numOfBarringInfo; i++) {
                mBarringInfo[i] = new BarringInfo(mBuffer);
            }
        } else {
            mBarringInfo = null;
        }
    }

    public BarringInfo getBarringInfo(BarringServiceType serviceType) {
        if (mBarringInfo == null) return null;
        for (BarringInfo b : mBarringInfo) {
            if (b.getBarringServiceType() == serviceType) {
                return b;
            }
        }
        return null;
    }

    @Override
    public String toString() {
        return ("RIL_UNSOL_BARRING {" + Arrays.toString(mBarringInfo) + "}");
    }
}
