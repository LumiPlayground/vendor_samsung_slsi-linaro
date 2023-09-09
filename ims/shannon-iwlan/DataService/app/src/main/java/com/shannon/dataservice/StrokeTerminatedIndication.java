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

public class StrokeTerminatedIndication extends StrokeResponseMessage {
    private final String mName;
    private final Cause mTerminationCause;

    public enum Cause {
        CAUSE_NORMAL(0),
        CAUSE_TIMEOUT(1);

        private int mEnumCause;

        Cause(int cause) {
            mEnumCause = cause;
        }

        public static Cause getEnum(int cause) {
            for (Cause e : values()) {
                if (e.mEnumCause == cause) {
                    return e;
                }
            }
            return CAUSE_NORMAL;
        }
    }

    public StrokeTerminatedIndication(StrokeResponseMessage from) {
        super(from);
        mName = getString();
        mTerminationCause = Cause.getEnum(getInt());
    }

    public String getName() { return mName; }
    public Cause getCause() { return mTerminationCause; }

    @Override
    public String toString() {
        return ("" + getType() + " name=" + mName + " cause=" + mTerminationCause);
    }
}
