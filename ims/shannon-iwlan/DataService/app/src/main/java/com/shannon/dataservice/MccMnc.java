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

import java.util.Objects;

public class MccMnc {
    private final int mMcc;
    private final int mMnc;

    MccMnc(int mcc, int mnc) {
        mMcc = mcc;
        mMnc = mnc;
    }

    MccMnc(String operator) {
        mMcc = Integer.parseInt(operator.substring(0, 3));
        mMnc = Integer.parseInt(operator.substring(3));
    }

    public int getMcc() { return mMcc; }
    public int getMnc() { return mMnc; }

    @Override
    public boolean equals(Object obj) {
        if (obj instanceof MccMnc) {
            MccMnc other = (MccMnc)obj;
            return (other == this) ||
                    ((other.mMcc == mMcc) && (other.mMnc == mMnc));
        }
        return false;
    }

    @Override
    public int hashCode() {
        return Objects.hash(mMcc, mMnc);
    }

    @Override
    public String toString() {
        return String.format("MCC=%d MNC=%d", mMcc, mMnc);
    }
}
