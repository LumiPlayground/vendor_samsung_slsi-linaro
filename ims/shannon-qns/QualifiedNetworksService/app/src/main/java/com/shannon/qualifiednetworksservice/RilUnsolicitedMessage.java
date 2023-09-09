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

public class RilUnsolicitedMessage {
    public static final int RIL_UNSOL_LAST_CALL_FAIL_CAUSE = 1005;
    public static final int RIL_UNSOL_BARRING = 2015;
    public static final int RIL_UNSOL_SRVCC = 3003;
    public static final int RIL_UNSOL_CALL_RING = 3010;
    public static final int RIL_UNSOL_CALL_STATUS = 3011;
    public static final int RIL_UNSOL_REGISTRATION = 3012;
    public static final int RIL_UNSOL_CALL_MODIFIED = 3026;
    public static final int RIL_UNSOL_RTP_TIMEOUT = 3049;
    public static final int RIL_UNSOL_RTP_EVENT = 3052;

    protected static final int AUDIO = 0x01;
    protected static final int VIDEO = 0x02;
    protected static final int TEXT = 0x04;
    protected static final int EMERGENCY = 0x08;
    protected static final int USSD = 0x10;
    protected static final int INVALID = 0xFFFF;

    public enum CallType {
        CALL_TYPE_NONE(0),
        CALL_TYPE_AUDIO(AUDIO),
        CALL_TYPE_VIDEO(VIDEO),
        CALL_TYPE_AUDIO_VIDEO(AUDIO | VIDEO),
        CALL_TYPE_TEXT(TEXT),
        CALL_TYPE_AUDIO_TEXT(AUDIO | TEXT),
        CALL_TYPE_VIDEO_TEXT(VIDEO | TEXT),
        CALL_TYPE_AUDIO_VIDEO_TEXT(AUDIO | VIDEO | TEXT),
        CALL_TYPE_EMERGENCY(EMERGENCY),
        CALL_TYPE_EMERGENCY_AUDIO(EMERGENCY | AUDIO),
        CALL_TYPE_EMERGENCY_VIDEO(EMERGENCY | VIDEO),
        CALL_TYPE_EMERGENCY_AUDIO_VIDEO(EMERGENCY | AUDIO | VIDEO),
        CALL_TYPE_EMERGENCY_TEXT(EMERGENCY | TEXT),
        CALL_TYPE_EMERGENCY_AUDIO_TEXT(EMERGENCY | AUDIO | TEXT),
        CALL_TYPE_EMERGENCY_VIDEO_TEXT(EMERGENCY | VIDEO | TEXT),
        CALL_TYPE_EMERGENCY_AUDIO_VIDEO_TEXT(EMERGENCY | AUDIO | VIDEO | TEXT),
        CALL_TYPE_USSD(USSD),
        CALL_TYPE_UNKNOWN(INVALID);

        private int mType;

        CallType(int type) {
            mType = type;
        }

        public static CallType getEnum(int type) {
            for (CallType e : values()) {
                if (e.mType == type) {
                    return e;
                }
            }
            return CALL_TYPE_UNKNOWN;
        }

        public boolean hasAudio() {
            if (mType == INVALID) {
                return false;
            }
            return ((mType & AUDIO) != 0);
        }

        public boolean hasVideo() {
            if (mType == INVALID) {
                return false;
            }
            return ((mType & VIDEO) != 0);
        }

        public boolean isEmergency() {
            if (mType == INVALID) {
                return false;
            }
            return ((mType & EMERGENCY) != 0);
        }

        public boolean isUssd() {
            if (mType == INVALID) {
                return false;
            }
            return ((mType & USSD) != 0);
        }
    }

    protected final int mMessageId;
    private final byte[] mRawData;

    RilUnsolicitedMessage(int msgId, byte[] data) {
        mMessageId = msgId;
        mRawData = data;
    }

    public RilUnsolicitedMessage(RilUnsolicitedMessage from) {
        mMessageId = from.mMessageId;
        mRawData = from.mRawData;
    }

    public int getMessageId() { return mMessageId; }

    protected byte[] getData() {
        return mRawData;
    }

    protected short getShort(byte b1, byte b2) {
        return (short)(b1 & 0xFF | (b2 & 0xFF) << 8);
    }

    protected int getInt(byte b1, byte b2, byte b3, byte b4) {
        return (int)(b1 & 0xFF | ((b2 & 0xFF) << 8) | ((b3 & 0xFF) << 16) | ((b4 & 0xFF) << 24));
    }
}
