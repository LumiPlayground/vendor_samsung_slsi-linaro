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

public class IkeImeiConfigurationAttribute extends IkeConfigurationAttribute {
    public IkeImeiConfigurationAttribute(int type, String Imei) {
        super(type, convertToBytes(Imei));
    }

    private static byte[] convertToBytes(String string) {
        byte[] bytes = StringUtils.stringToNativeString(string, "US-ASCII");
        if (bytes != null && bytes.length > 1) {
            return Arrays.copyOf(bytes, bytes.length - 1);
        }
        return null;
    }
}
