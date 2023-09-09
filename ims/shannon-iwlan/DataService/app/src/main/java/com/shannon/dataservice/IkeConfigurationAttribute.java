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

public class IkeConfigurationAttribute extends IkeVendorData {
    public IkeConfigurationAttribute(int type, byte[] bytes) {
        super(type, bytes);
    }

    public IkeConfigurationAttribute(ByteBuffer buffer) {
        super(buffer);
    }

    @Override
    public String toString() {
        return ("IKE_CFG_ATTRIBUTE type=" + mType + " data={" + StringUtils.bytesToHexString(mData) + "}");
    }
}
