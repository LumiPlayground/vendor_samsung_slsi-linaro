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

public class IkeNotifyN1ModeData {
    private final byte[] mData;

    public IkeNotifyN1ModeData(IkeNotifyData from) {
        byte[] notifyData = from.getData();

        if (notifyData != null && notifyData[0] > 0) {
            int length = StringUtils.getByteAsInt(notifyData, 0);

            mData = new byte[length];
            System.arraycopy(notifyData, 1, mData, 0, length);
        } else {
            mData = null;
        }
    }

    public byte[] getData() { return mData; }
}
