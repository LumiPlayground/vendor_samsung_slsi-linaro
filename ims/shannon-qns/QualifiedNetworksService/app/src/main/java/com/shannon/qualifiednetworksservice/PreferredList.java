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
package com.shannon.qualifiednetworksservice;

import android.telephony.AccessNetworkConstants;

import java.util.ArrayList;

public class PreferredList extends ArrayList<Integer> {
    @Override
    public boolean equals(Object o) {
        if (o instanceof PreferredList) {
            PreferredList list = (PreferredList)o;

            int size = size();
            if (size == list.size()) {
                /* compare each */
                for (int i = 0; i < size; i++) {
                    if (!get(i).equals(list.get(i))) {
                        return false;
                    }
                }
                return true;
            }
        }
        return false;
    }

    @Override
    public String toString() {
        if (isEmpty()) {
            return "[empty]";
        }
        String s = "[";
        for (Integer i : this) {
            s += (AccessNetworkConstants.AccessNetworkType.toString(i) + ",");
        }
        return (s.substring(0, s.length() - 1) + "]");
    }
}
