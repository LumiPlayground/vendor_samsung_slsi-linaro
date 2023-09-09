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

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class RilUnsolicitedCallStatus extends RilUnsolicitedMessage {
    public enum CallState {
        CALL_STATE_ACTIVE(0),
        CALL_STATE_ON_HOLD(1),
        CALL_STATE_DIALING(2),
        CALL_STATE_ALERTING(3),
        CALL_STATE_INCOMING(4),
        CALL_STATE_WAITING(5),
        CALL_STATE_RELEASED(6),
        CALL_STATE_HOLD_MO(7),
        CALL_STATE_HOLD_MT(8),
        CALL_STATE_UNKNOWN(0xFF);

        private int mState;

        CallState(int state) {
            mState = state;
        }

        public static CallState getEnum(int state) {
            for (CallState e : values()) {
                if (e.mState == state) {
                    return e;
                }
            }
            return CALL_STATE_UNKNOWN;
        }
    }

    public enum IsMultiParty {
        MP_STATE_SINGLE_CALL(0),
        MP_STATE_MULTI_PARTY_CALL(1),
        MP_STATE_UNKNOWN(0xFF);

        private int mMultiPartyState;

        IsMultiParty(int value) {
            mMultiPartyState = value;
        }

        public static IsMultiParty getEnum(int value) {
            for (IsMultiParty e : values()) {
                if (e.mMultiPartyState == value) {
                    return e;
                }
            }
            return MP_STATE_UNKNOWN;
        }
    }

    public enum CallStateReason {
        CS_REASON_NORMAL(0),
        CS_REASON_RETRY(0x20),
        CS_REASON_STOP_RINGING(0x21),
        CS_REASON_UNKNOWN(0xFF);

        private int mReason;

        CallStateReason(int value) {
            mReason = value;
        }

        public static CallStateReason getEnum(int value) {
            for (CallStateReason e : values()) {
                if (e.mReason == value) {
                    return e;
                }
            }
            return CS_REASON_UNKNOWN;
        }
    }

    public enum CallEndCause {
        CE_CAUSE_UNALLOCATED_NUMBER(1),
        CE_CAUSE_NO_ROUTE_TO_DESTINATION(3),
        CE_CAUSE_CHANNLE_UNACCEPTABLE(6),
        CE_CAUSE_UNKNOWN(0xFF);

        private int mCause;

        CallEndCause(int value) {
            mCause = value;
        }

        public static CallEndCause getEnum(int value) {
            for (CallEndCause e : values()) {
                if (e.mCause == value) {
                    return e;
                }
            }
            return CE_CAUSE_UNKNOWN;
        }
    }

    public enum EMCCallCategory {
        EMCC_DEFAULT(0x00),
        EMCC_POLICE(0x01),
        EMCC_AMBULANCE(0x02),
        EMCC_FIRE(0x04),
        EMCC_MARIN(0x08),
        EMCC_MOUNTAIN(0x10),
        EMCC_TRAFFIC(0x20),
        EMCC_CAUSE_UNKNOWN(0xFF);

        private int mCategory;

        EMCCallCategory(int value) {
            mCategory = value;
        }

        public static EMCCallCategory getEnum(int value) {
            for (EMCCallCategory e : values()) {
                if (e.mCategory == value) {
                    return e;
                }
            }
            return EMCC_CAUSE_UNKNOWN;
        }
    }

    public enum MediaDirection {
        MEDIA_DIR_NULL(0x0000),
        MEDIA_DIR_AUDIO_SEND_RECV(0x0001),
        MEDIA_DIR_AUDIO_SEND_ONLY(0x0002),
        MEDIA_DIR_AUDIO_RECV_ONLY(0x0004),
        MEDIA_DIR_AUDIO_NO_SEND_RECV(0x0008),
        MEDIA_DIR_VIDEO_SEND_RECV(0x0010),
        MEDIA_DIR_VIDEO_SEND_ONLY(0x0020),
        MEDIA_DIR_VIDEO_RECV_ONLY(0x0040),
        MEDIA_DIR_VIDEO_NO_SEND_RECV(0x0080),
        MEDIA_DIR_TEXT_SEND_RECV(0x0100),
        MEDIA_DIR_TEXT_SEND_ONLY(0x0200),
        MEDIA_DIR_TEXT_RECV_ONLY(0x0400),
        MEDIA_DIR_TEXT_NO_SEND_RECV(0x0800),
        MEDIA_DIR_UNKNOWN(0xFFFF);

        private int mMediaDirection;

        MediaDirection(int value) {
            mMediaDirection = value;
        }

        public static MediaDirection getEnum(int value) {
            for (MediaDirection e : values()) {
                if (e.mMediaDirection == value) {
                    return e;
                }
            }
            return MEDIA_DIR_UNKNOWN;
        }
    }

    public enum RemoteSupportedMedia {
        REMOTE_MEDIA_AUDIO(0x0001),
        REMOTE_MEDIA_VIDEO(0x0002),
        REMOTE_MEDIA_TEXT(0x0004),
        REMOTE_MEDIA_EMERGENCY(0x0008),
        REMOTE_MEDIA_USSD(0x0010),
        REMOTE_MEDIA_UNKNOWN(INVALID);

        private int mRemoteMedia;

        RemoteSupportedMedia(int value) {
            mRemoteMedia = value;
        }

        public static RemoteSupportedMedia getEnum(int value) {
            for (RemoteSupportedMedia e : values()) {
                if (e.mRemoteMedia == value) {
                    return e;
                }
            }
            return REMOTE_MEDIA_UNKNOWN;
        }
    }

    public enum EmergencyCallBackMode {
        ECBM_NONE(0x00),
        ECBM_ENTER(0x01),
        ECBM_UNKNOWN(0xFF);

        private int mMode;

        EmergencyCallBackMode(int value) {
            mMode = value;
        }

        public static EmergencyCallBackMode getEnum(int value) {
            for (EmergencyCallBackMode e : values()) {
                if (e.mMode == value) {
                    return e;
                }
            }
            return ECBM_UNKNOWN;
        }

        boolean isECBM() { return (mMode == ECBM_ENTER.mMode); }
    }

    private static final int MAX_USER_COUNT = 6;
    private static final int MAX_SDP_INFO_SIZE = 1750;
    private static final int MAX_NUMBER_SIZE = 513;

    private final int mCallId;
    private final CallType mCallType;
    private final CallState mCallState;
    private final EmergencyCallBackMode mECBM;

    public RilUnsolicitedCallStatus(RilUnsolicitedMessage from) {
        super(from);

        getByteBuffer();
        mCallType = CallType.getEnum(getShort());
        mCallId = getByte();
        mCallState = CallState.getEnum(getByte());
        boolean isMultiParty = (IsMultiParty.getEnum(getByte()) == IsMultiParty.MP_STATE_MULTI_PARTY_CALL);
        int numOfUsers = getByte(); if (numOfUsers > MAX_USER_COUNT) numOfUsers = MAX_USER_COUNT;
        getConfCallInfo(isMultiParty, numOfUsers);
        CallStateReason callStateReason = CallStateReason.getEnum(getByte());
        CallEndCause callEndCause = CallEndCause.getEnum(getByte());
        EMCCallCategory emcCallCategory = EMCCallCategory.getEnum(getByte());
        int SDPLength = getShort(); if (SDPLength > MAX_SDP_INFO_SIZE) SDPLength = MAX_SDP_INFO_SIZE;
        String SDPInfo = ((SDPLength > 0) ? getString(SDPLength) : "");
        MediaDirection mediaDir = MediaDirection.getEnum(getShort());
        int connPartyNumberLength = getShort(); if (connPartyNumberLength > MAX_NUMBER_SIZE) connPartyNumberLength = MAX_NUMBER_SIZE;
        String connPartyNumber =((connPartyNumberLength > 0) ? getString(connPartyNumberLength) : "");
        RemoteSupportedMedia remoteSupportedMedia = RemoteSupportedMedia.getEnum(getShort());
        mECBM = EmergencyCallBackMode.getEnum(getByte());

    }

    private void getConfCallInfo(boolean isMultiParty, int numOfUsers) {
        int callId;
        int callState;
        int uriLength;
        String uriString;
        if (isMultiParty) {
            for(int i = 0; i < numOfUsers ; i++) {
                callId = getByte();
                callState = getByte();
                uriLength = getShort(); if (uriLength > MAX_NUMBER_SIZE) uriLength = MAX_NUMBER_SIZE;
                uriString = ((uriLength > 0) ? getString(uriLength) : "");
            }
        }
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

    protected long getLong() {
        if ((mBufferOffset + Long.BYTES) > mBufferLength) {
            return 0;
        }
        mBufferOffset += Long.BYTES;
        return mBuffer.getLong();
    }

    protected String getString(int length) {
        if ((mBufferOffset + length) > mBufferLength) {
            return "Invalid Length";
        }
        int offset = mBufferOffset;
        mBufferOffset += length;
        return new String(mBuffer.array(), offset, length);
    }

    public int getCallId() { return mCallId; }
    public CallState getCallState() { return mCallState; }
    public boolean isAudioCall() { return mCallType.hasAudio(); }
    public boolean isVideoCall() { return mCallType.hasVideo(); }
    public boolean isEmergencyCall() { return mCallType.isEmergency(); }
    public boolean isECBM() { return mECBM.isECBM(); }

    @Override
    public String toString() {
        return ("RIL_UNSOLICITED_CALL_STATUS {" + mCallId + "," + mCallType + "," + mCallState + "," + mECBM +"}");
    }
}
