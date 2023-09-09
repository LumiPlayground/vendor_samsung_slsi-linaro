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

import android.text.TextUtils;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class IkeNotifyDeviceIdentity extends IkeNotifyData {
    private static final int IDENTITY_TYPE_IMEI = 1;
    private static final int IDENTITY_TYPE_IMEISV = 2;

    public IkeNotifyDeviceIdentity(String Imei) {
        super(IkeNotifyProcessor.DEVICE_IDENTITY, convertToBytes(Imei));
    }

    private static byte[] convertToBytes(String string) {
        /*
         * Length (2 bytes)
         * Type (IMEI or IMEISV)
         * Value
         */
        if (TextUtils.isEmpty(string)) {
            return null;
        }
        byte[] identity = StringUtils.stringToTBCD(string);
        int length = identity.length;

        ByteBuffer buffer = ByteBuffer.allocate(length + Short.BYTES + 1);
        buffer.order(ByteOrder.BIG_ENDIAN);
        buffer.putShort(Integer.valueOf(length + 1).shortValue());
        buffer.put((byte)IDENTITY_TYPE_IMEI);
        buffer.put(identity);
        return buffer.array();
    }
}
