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

import java.net.InetAddress;
import java.util.Arrays;

public class IkeIpConfigurationAttribute extends IkeConfigurationAttribute {
    private static final int INET6_ADDR_LENGTH = 16;

    public IkeIpConfigurationAttribute(int type, InetAddress address) {
        super(type, (address != null) ? address.getAddress() : null);
    }

    public IkeIpConfigurationAttribute(IkeConfigurationAttribute from) {
        super(from.getType(), from.getData());
    }

    public InetAddress getIp() {
        try {
            byte[] data = getData();
            if (data == null) {
                return null;
            }
            if (data.length > INET6_ADDR_LENGTH) {
                data = Arrays.copyOf(data, INET6_ADDR_LENGTH);
            }
            return InetAddress.getByAddress(data);
        } catch (Exception e) {
            return null;
        }
    }
}
