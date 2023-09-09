/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
import java.nio.ByteOrder;

public class RilUnsolicitedCallRing extends RilUnsolicitedMessage {
    public enum MtCallEndCause {
        MCE_CAUSE_INVALID(0),
        MCE_CAUSE_QOS_FAIL(1),
        MCE_CAUSE_REJECT_ONGOING_E9911_CALL(2),
        MCE_CAUSE_REJECT_ONGING_CALLSETUP(3),
        MCE_CAUSE_MAX_CALL_LIMIT(4),
        MCE_CAUSE_REJECT_ONGOING_UPGRADE(5),
        MCE_CAUSE_BAD_REQUEST(6),
        MCE_CAUSE_ONGOING_TRANSFER(7),
        MCE_CAUSE_ONGOING_CONF_CALL(8),
        MCE_CAUSE_ONGOING_CLEARED_BY_NW(9),
        MCE_CAUSE_TIMEOUT(10),
        MCE_CAUSE_REQ_TERMINATED(11),
        MCE_CAUSE_BUSY(12),
        MCE_CAUSE_UNKNOWN(13),
        MCE_CAUSE_MAX(0xFF);

        private int mCause;

        MtCallEndCause(int value) {
            mCause = value;
        }

        public static MtCallEndCause getEnum(int value) {
            for (MtCallEndCause e : values()) {
                if (e.mCause == value) {
                    return e;
                }
            }
            return MCE_CAUSE_MAX;
        }
    }

    private final int mCallId;
    private final CallType mCallType;
    private final MtCallEndCause mMtCallEndCause;

    public RilUnsolicitedCallRing(RilUnsolicitedMessage from) {
        super(from);
        getByteBuffer();
        mCallType = CallType.getEnum(getShort());
        mCallId = getByte();
        /* Drop not used fields */
        int numLen = getShort();
        if (numLen > 0) mBuffer.get(new byte[numLen]); //NUMBER
        int sdpLen = getShort();
        if (sdpLen > 0) mBuffer.get(new byte[sdpLen]); // SDP
        getByte(); // USSD_LANGUAGE_CODE
        int ussdLen = getByte();
        if (ussdLen > 0) mBuffer.get(new byte[ussdLen]); // USSD
        getByte(); // USSD_ALERTING_PATTERN
        getByte(); // USSD_SERVER_INITIATED_TYPE
        getByte(); // IS_CONFERENCE_CALL
        int displayNameLen = getByte();
        if (displayNameLen > 0) mBuffer.get(new byte[displayNameLen]); //DISPLAY_NAME
        getByte(); // VERSTAT
        int sipMsgLen = getInt();
        if (sipMsgLen > 0) mBuffer.get(new byte[sipMsgLen]); // SIP_MSG
        mMtCallEndCause = MtCallEndCause.getEnum(getByte());
    }

    private ByteBuffer mBuffer;
    private int mBufferOffset;
    private int mBufferLength;

    protected void getByteBuffer() {
        mBuffer = ByteBuffer.wrap(getData());
        mBuffer.order(ByteOrder.LITTLE_ENDIAN);
        mBufferOffset = 0;
        mBufferLength = getData().length;
    }

    protected int getByte() {
        if ((mBufferOffset + Byte.BYTES) > mBufferLength) {
            return 0;
        }
        mBufferOffset += Byte.BYTES;
        return (mBuffer.get() & 0xFF);
    }

    protected int getShort() {
        if ((mBufferOffset + Short.BYTES) > mBufferLength) {
            return 0;
        }
        mBufferOffset += Short.BYTES;
        return (mBuffer.getShort() & 0xFFFF);
    }

    protected int getInt() {
        if ((mBufferOffset + Integer.BYTES) > mBufferLength) {
            return 0;
        }
        mBufferOffset += Integer.BYTES;
        return mBuffer.getInt();
    }

    public int getCallId() { return mCallId; }
    public boolean isAudioCall() { return mCallType.hasAudio(); }
    public boolean isVideoCall() { return mCallType.hasVideo(); }
    public boolean isEmergencyCall() { return mCallType.isEmergency(); }
    public boolean isValidCall() { return (mMtCallEndCause == MtCallEndCause.MCE_CAUSE_INVALID); }

    @Override
    public String toString() {
        return ("RIL_UNSOLICITED_CALL_RING {" + mCallId + "," + mCallType + "," + mMtCallEndCause +"}");
    }
}
