/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or
 * distributed, transmitted, transcribed, stored in a retrieval system or
 * translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed to third parties
 * without the express written permission of Samsung Electronics.
 */
package com.shannon.qualifiednetworksservice;

public class RilUnsolicitedRtpTimeout extends RilUnsolicitedMessage {
    public enum RtpEvent {
        EVENT_UNKNOWN(0),
        EVENT_RTP_TIMEOUT(1),
        EVENT_RTCP_TIMEOUT(2);

        private int mType;
        RtpEvent(int type) { mType = type; }

        public static RtpEvent getEnum(int type) {
            for (RtpEvent e : values()) {
                if (e.mType == type) {
                    return e;
                }
            }
            return EVENT_UNKNOWN;
        }
    }

    private final int mCallId;
    private final RtpEvent mType;

    public RilUnsolicitedRtpTimeout(RilUnsolicitedMessage from) {
        super(from);

        byte[] bytes = getData();
        if (bytes.length == 2) {
            mCallId = bytes[0];
            mType = RtpEvent.getEnum(bytes[1]);
        } else {
            mCallId = 0;
            mType = RtpEvent.EVENT_UNKNOWN;
        }
    }

    public RtpEvent getTimeoutType() { return mType; }

    @Override
    public String toString() {
        return ("RIL_UNSOLICITED_RTP_TIMEOUT {" + mCallId + "," + mType + "}");
    }
}
