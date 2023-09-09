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

public class RilUnsolicitedSrvcc extends RilUnsolicitedMessage {
    public enum SrvccType {
        /**
         * Type of handover
         * 0x00 = Not specified
         * 0x01 = SRVCC handover
         * 0x02 = PS handover
         */
        TYPE_NOT_SPECIFIED(0x00),
        TYPE_SRVCC(0x01),
        TYPE_PS(0x02);

        private int mType;

        SrvccType (int type) {
            mType = type;
        }

        public static SrvccType getEnum(int type) {
            for (SrvccType e : values()) {
                if (e.mType == type) {
                    return e;
                }
            }
            return TYPE_NOT_SPECIFIED;
        }
    }

    public enum SrvccStatus {
        /**
         * HO status
         * 0x00 = Started
         * 0x01 = Completed
         * 0x02 = Reinvited_requested
         * 0x03 = Disconnect requested
         * 0x04 = Failed
         */
        STATUS_STARTED(0x00),
        STATUS_COMPLETED(0x01),
        STATUS_REINVITE_REQUESTED(0x02),
        STATUS_DISCONNECT_REQUESTED(0x03),
        STATUS_FAILED(0x04);

        private int mStatus;

        SrvccStatus (int status) {
            mStatus = status;
        }

        public static SrvccStatus getEnum(int status) {
            for (SrvccStatus e : values()) {
                if (e.mStatus == status) {
                    return e;
                }
            }
            return STATUS_FAILED;
        }
    }

    private final SrvccType mType;
    private final SrvccStatus mStatus;

    public RilUnsolicitedSrvcc(RilUnsolicitedMessage from) {
        super(from);

        byte[] bytes = getData();
        if (bytes.length == 2) {
            mType = SrvccType.getEnum(bytes[0]);
            mStatus = SrvccStatus.getEnum(bytes[1]);
        } else {
            mType = SrvccType.TYPE_NOT_SPECIFIED;
            mStatus = SrvccStatus.STATUS_FAILED;
        }
    }

    public SrvccType getType() { return mType; }
    public SrvccStatus getStatus() { return mStatus; }

    @Override
    public String toString() {
        return ("RIL_UNSOLICITED_SRVCC {" + mType + "," + mStatus + "}");
    }
}
