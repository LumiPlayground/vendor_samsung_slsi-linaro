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

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class IkeVendorData {
    protected final int mType;
    protected final byte[] mData;

    public IkeVendorData(int type, byte[] bytes) {
        mType = type;
        mData = bytes;
    }

    public IkeVendorData(ByteBuffer buffer) {
        mType = buffer.getShort() & 0xFFFF;
        int length = buffer.getShort() & 0xFFFF;

        mData = new byte[length];
        buffer.get(mData, 0, length);
    }

    public int getType() { return mType; }
    public byte[] getData() { return mData; }

    public int getBytesSize() {
        return (Short.BYTES * 2 + getLength());
    }

    public byte[] getBytes() {
        ByteBuffer buffer = ByteBuffer.allocate(getBytesSize());
        buffer.order(ByteOrder.nativeOrder());
        buffer.putShort(Integer.valueOf(mType).shortValue());
        buffer.putShort(Integer.valueOf(getLength()).shortValue());
        if (mData != null) {
            buffer.put(mData);
        }
        return buffer.array();
    }

    private int getLength() {
        return ((mData != null) ? mData.length : 0);
    }
}
