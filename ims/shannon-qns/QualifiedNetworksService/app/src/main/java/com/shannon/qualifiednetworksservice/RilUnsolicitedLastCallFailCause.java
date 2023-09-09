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

public class RilUnsolicitedLastCallFailCause extends RilUnsolicitedMessage {
    public enum LastCallFailCause {
        NONE(0),
        REDIAL_EMERGENCY_TO_CELL(0x0BB9),
        REDIAL_EMERGENCY_TO_WIFI(0x0BBA);

        private final int mType;

        LastCallFailCause(int type) { mType = type; }

        public static LastCallFailCause getEnum(int type) {
            for (LastCallFailCause e : values()) {
                if (e.mType == type) {
                    return e;
                }
            }
            return NONE;
        }
    }

    private final LastCallFailCause mLastCallFailCause;

    public RilUnsolicitedLastCallFailCause(RilUnsolicitedMessage from) {
        super(from);

        byte[] bytes = getData();
        if (bytes.length == 4) {
            mLastCallFailCause = LastCallFailCause.getEnum(getInt(bytes[0], bytes[1], bytes[2], bytes[3]));
        } else {
            mLastCallFailCause = LastCallFailCause.NONE;
        }
    }

    public LastCallFailCause getLastCallFailCause() { return mLastCallFailCause; }

    @Override
    public String toString() {
        return ("RIL_UNSOL_LAST_CALL_FAIL_CAUSE {" + mLastCallFailCause + "}");
    }
}
