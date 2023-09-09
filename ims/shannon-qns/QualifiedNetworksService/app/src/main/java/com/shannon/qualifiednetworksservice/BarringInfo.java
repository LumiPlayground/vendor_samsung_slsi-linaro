/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or
 * distributed, transmitted, transcribed, stored in a retrieval system or
 * translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed to third parties
 * without the express written permission of Samsung Electronics.
 */
package com.shannon.qualifiednetworksservice;

import java.nio.ByteBuffer;

public class BarringInfo {
    protected enum BarringServiceType {
        SERVICE_TYPE_MO_SIGNALLING(3),
        SERVICE_TYPE_MO_DATA(4),
        SERVICE_TYPE_MMTEL_VOICE(6),
        SERVICE_TYPE_MMTEL_VIDEO(7),
        SERVICE_TYPE_EMERGENCY(8),
        SERVICE_TYPE_SMS(9),
        SERVICE_TYPE_OPERATOR_1(0x1001), // real VOICE_SIB2 for VZW
        SERVICE_TYPE_OPERATOR_2(0x1002), // real VOIDE_SIB2 for VZW
        SERVICE_TYPE_UNKNOWN(-1);

        private int mServiceType;

        BarringServiceType(int type) {
            mServiceType = type;
        }

        public static BarringServiceType getEnum(int type) {
            for (BarringServiceType e : values()) {
                if (e.mServiceType == type) {
                    return e;
                }
            }
            return SERVICE_TYPE_UNKNOWN;
        }
    }

    protected enum BarringType {
        /** Device is not barred for the given service */
        BARRING_TYPE_NONE(0),
        /** Device may be barred based on time and probability factors */
        BARRING_TYPE_CONDITIONAL(1),
        /* Device is unconditionally barred */
        BARRING_TYPE_UNCONDITIONAL(2),
        BARRING_TYPE_UNKNOWN(3);

        private int mBarringType;

        BarringType(int type) { mBarringType = type; }

        public static BarringType getEnum(int type) {
            for (BarringType e : values()) {
                if (e.mBarringType == type) {
                    return e;
                }
            }
            return BARRING_TYPE_UNKNOWN;
        }
    }

    private BarringServiceType mBarringServiceType;
    private BarringType mBarringType;
    private int mBarringFactor;
    private int mBarringTimeSeconds;
    private boolean mIsBarred;

    BarringInfo(ByteBuffer buffer) {
        mBarringServiceType = BarringServiceType.getEnum(buffer.getInt());
        mBarringType = BarringType.getEnum(buffer.getInt());
        mBarringFactor = buffer.getInt();
        mBarringTimeSeconds = buffer.getInt();
        mIsBarred = (buffer.getInt() == 1);
    }

    BarringServiceType getBarringServiceType() { return mBarringServiceType; }
    BarringType getBarringType() { return mBarringType; }
    int getBarringFactor() { return mBarringFactor; }
    boolean isBarred() { return mIsBarred; }

    @Override
    public boolean equals(Object o) {
        if (this == o) {
            return true;
        }
        if (!(o instanceof BarringInfo)) {
            return false;
        }

        BarringInfo info = (BarringInfo) o;
        return (this.mBarringServiceType == info.mBarringServiceType)
            && (this.mBarringType == info.mBarringType)
            && (this.mBarringFactor == info.mBarringFactor)
            && (this.mBarringTimeSeconds == info.mBarringTimeSeconds)
            && (this.mIsBarred == info.mIsBarred);
    }

    @Override
    public String toString() {
        return ("BarringInfo {" + mBarringServiceType + ", " + mBarringType + ", " + mBarringFactor +
            ", " + mBarringTimeSeconds + ", " + mIsBarred + "}");
    }
}
