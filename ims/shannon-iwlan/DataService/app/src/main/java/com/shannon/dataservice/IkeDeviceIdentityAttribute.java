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

public class IkeDeviceIdentityAttribute extends IkeConfigurationAttribute {
    private static final int ATTRIBUTE_TYPE = 41101;
    private static final int IDENTITY_TYPE_IMEI = 1;
    private static final int IDENTITY_TYPE_IMEISV = 2;

    public IkeDeviceIdentityAttribute(String imei) {
        super(ATTRIBUTE_TYPE, buildAttributeData(imei));
    }

    private static byte[] buildAttributeData(String imei) {
        if (TextUtils.isEmpty(imei)) {
            return null;
        }
        byte[] identity = StringUtils.stringToTBCD(imei);
        byte[] bytes = new byte[identity.length + 1];
        bytes[0] = Integer.valueOf(IDENTITY_TYPE_IMEI).byteValue();
        System.arraycopy(identity, 0, bytes, 1, identity.length);
        return bytes;
    }
}
