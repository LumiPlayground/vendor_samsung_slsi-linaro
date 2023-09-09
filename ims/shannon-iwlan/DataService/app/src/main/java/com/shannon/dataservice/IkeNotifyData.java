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

public class IkeNotifyData extends IkeVendorData {
    public IkeNotifyData(int type, byte[] bytes) {
        super(type, bytes);
    }

    public IkeNotifyData(ByteBuffer buffer) {
        super(buffer);
    }

    @Override
    public String toString() {
        return ("IKE_NOTIFY type=" + mType + " data={" + StringUtils.bytesToHexString(mData) + "}");
    }
}
