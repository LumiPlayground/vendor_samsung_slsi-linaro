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
package com.shannon.dataservice;

import java.util.Arrays;

public class StrokeAuthenticationIndication extends StrokeResponseMessage {
    private static final int AKA_RAND_LENGTH = 16;
    private static final int AKA_AUTN_LENGTH = 16;

    private final String mName;
    private final byte[] mRand;
    private final byte[] mAutn;

    public StrokeAuthenticationIndication(StrokeResponseMessage from) {
        super(from);
        mName = getString();
        mRand = getBytesArray(AKA_RAND_LENGTH);
        mAutn = getBytesArray(AKA_AUTN_LENGTH);
    }

    public String getName() { return mName; }

    public byte[] getNonce() {
        byte[] bytes = new byte[AKA_RAND_LENGTH + AKA_AUTN_LENGTH];
        System.arraycopy(mRand, 0, bytes, 0, AKA_RAND_LENGTH);
        System.arraycopy(mAutn, 0, bytes, AKA_RAND_LENGTH, AKA_AUTN_LENGTH);
        return bytes;
    }

    @Override
    public String toString() {
        return ("" + getType() + " name=" + mName + " RAND=" + StringUtils.bytesToHexString(mRand) + " AUTN=" + StringUtils.bytesToHexString(mAutn));
    }
}
