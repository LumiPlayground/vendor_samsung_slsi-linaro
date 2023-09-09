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

import com.android.internal.annotations.VisibleForTesting;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.Arrays;

public class StrokeResponseMessage {
    private static final boolean PROTO64 = StrokeRequestMessage.PROTO64;

    public enum ResponseMessageType {
        STR_RES_INITIATE(0),
        STR_RES_ADD_CONN(1),
        STR_RES_DEL_CONN(2),
        STR_RES_TERMINATE(3),
        STR_IND_TERMINATED(4),
        STR_IND_SIM_AUTH(5),
        STR_RES_SIM_AUTH(6),
        STR_RES_ADD_ROUTE(7),
        STR_RES_DEL_ROUTE(8),
        STR_RES_SET_INTERFACE(9);

        private int mCode;

        ResponseMessageType(int code) { mCode = code; }
        public int getCode() { return mCode; }

        public static ResponseMessageType getEnum(int code) {
            for (ResponseMessageType e : ResponseMessageType.values()) {
                if (e.mCode == code) {
                    return e;
                }
            }
            return null;
        }
    }

    public enum ResponseErrorStatus {
        STR_ERROR_SUCCESS(0),
        STR_ERROR_UNKNOWN(1),                            /* IPSec tunnel fail for any other reasons */
        STR_ERROR_PEER_INIT_UNREACHABLE(2),            /* ALERT_PEER_INIT_UNREACHABLE */
        STR_ERROR_CERTIFICATE_EXPIRED(3),	            /* ALERT_CERT_EXPIRED */
        STR_ERROR_CERTIFICATE_REVOKED(4),	            /* ALERT_CERT_REVOKED */
        STR_ERROR_CERTIFICATE_VALIDATION_FAILED(5),	/* ALERT_CERT_VALIDATION_FAILED */
        STR_ERROR_CERTIFICATE_NO_ISSUER(6),	        /* ALERT_CERT_NO_ISSUER */
        STR_ERROR_CERTIFICATE_UNTRUSTED_ROOT(7),	    /* ALERT_CERT_UNTRUSTED_ROOT */
        STR_ERROR_CERTIFICATE_EXCEEDED_PATH_LEN(8),	/* ALERT_CERT_EXCEEDED_PATH_LEN*/
        STR_ERROR_CERTIFICATE_POLICY_VIOLATION(9),	/* ALERT_CERT_POLICY_VIOLATION */
        STR_ERROR_CERTIFICATE_AUTH_FAILED(10),	        /* ALERT_PEER_AUTH_FAILED */
        STR_ERROR_NETWORK_FAILURE(11);                  /* NETWORK FAILURE NOTIFY */

        private int mCode;

        ResponseErrorStatus(int code) { mCode = code; }
        public int getCode() { return mCode; }

        public static ResponseErrorStatus getEnum(int code) {
            for (ResponseErrorStatus e : ResponseErrorStatus.values()) {
                if (e.mCode == code) {
                    return e;
                }
            }
            return STR_ERROR_UNKNOWN;
        }
    }

    public enum ProcessedNotifyType {
        STR_NOTIFY_NETWORK_AUTH_FAILED(24),
        STR_NOTIFY_INTERNAL_ADDR_FAILED(36),
        STR_NOTIFY_PDN_CONNECTION_REJECTION(8192),
        STR_NOTIFY_MAX_CONNECTION_REACHED(8193),
        STR_NOTIFY_SEMANTIC_ERROR_IN_THE_TFT_OPERATION(8241),
        STR_NOTIFY_SYNTACTICAL_ERROR_IN_THE_TFT_OPERATION(8242),
        STR_NOTIFY_SEMANTIC_ERRORS_IN_PACKET_FILTERS(8244),
        STR_NOTIFY_SYNTACTICAL_ERRORS_IN_PACKET_FILTERS(8245),
        STR_NOTIFY_NON_3GPP_ACCESS_NOT_ALLOWED(9000),
        STR_NOTIFY_USER_UNKNOWN(9001),
        STR_NOTIFY_NO_APN_SUBSCRIPTION(9002),
        STR_NOTIFY_AUTHORIZATION_REJECTED(9003),
        STR_NOTIFY_ILLEGAL_ME(9006),
        STR_NOTIFY_MISSING_OR_UNKNOWN_APN(9048),
        STR_NOTIFY_NO_SUBSCRIPTION(9049),
        STR_NOTIFY_NETWORK_TOO_BUSY(10000),
        STR_NOTIFY_NETWORK_FAILURE(10500),
        STR_NOTIFY_ROAMING_NOT_ALLOWED(11000),
        STR_NOTIFY_RAT_TYPE_NOT_ALLOWED(11001),
        STR_NOTIFY_PERMANENT_FAILURE(11002),
        STR_NOTIFY_RAT_DISALLOWED(11003),
        STR_NOTIFY_IMEI_NOT_ACCEPTED(11005),
        STR_NOTIFY_PLMN_NOT_ALLOWED(11011),
        STR_NOTIFY_UNSUPPORTED_PDN_TYPE(11049),
        STR_NOTIFY_UNAUTHENTICATED_EMERGENCY_NOT_SUPPORTED(11055),
        STR_NOTIFY_CONGESTION(15500);

        private int mCode;

        ProcessedNotifyType(int code) { mCode = code; }
        public int getCode() { return mCode; }

        public static ProcessedNotifyType getEnum(int code) {
            for (ProcessedNotifyType e : ProcessedNotifyType.values()) {
                if (e.mCode == code) {
                    return e;
                }
            }
            return null;
        }
    }

    private final ByteBuffer mBuffer;
    private final int mLength;
    private final ResponseMessageType mType;
    private int mOffset;

    public StrokeResponseMessage(byte[] bytes) {
        mBuffer = ByteBuffer.wrap(bytes);
        mBuffer.order(ByteOrder.nativeOrder());

        mLength = mBuffer.getShort() & 0xFFFF;
        mOffset = Short.BYTES;
        mType = ResponseMessageType.getEnum(getInt());
    }

    public StrokeResponseMessage(StrokeResponseMessage from) {
        mLength = from.mLength;
        mBuffer = from.mBuffer;
        mType = from.mType;
        mOffset = from.mOffset;
    }

    public StrokeResponseMessage(ResponseMessageType type, int length) {
        mLength = length + 16; // add some padding here
        mType = type;
        mBuffer = ByteBuffer.allocate(mLength);
        mBuffer.order(ByteOrder.nativeOrder());

        mOffset = 0;
        putShort(mLength);
        putInt(mType.getCode());
    }

    public ResponseMessageType getType() { return mType; }

    private int align(int length) {
        int padLen = ((mOffset + (length - 1)) & ~(length - 1)) - mOffset;
        if (padLen > 0) {
            byte[] dummy = new byte[padLen];
            mBuffer.get(dummy);
        }
        return padLen;
    }

    protected int getShort() {
        mOffset += align(Short.BYTES);
        if ((mOffset + Short.BYTES) > mLength) {
            return 0;
        }
        mOffset += Short.BYTES;
        return (mBuffer.getShort() & 0xFFFF);
    }

    private void putShort(int value) {
        int padLen = ((mOffset + (Short.BYTES - 1)) & ~(Short.BYTES - 1)) - mOffset;
        if (padLen > 0) {
            mBuffer.put(new byte[padLen]);
        }
        mBuffer.putShort(Integer.valueOf(value).shortValue());
        mOffset += (Short.BYTES + padLen);
    }

    protected int getInt() {
        mOffset += align(Integer.BYTES);
        if ((mOffset + Integer.BYTES) > mLength) {
            return 0;
        }
        mOffset += Integer.BYTES;
        return mBuffer.getInt();
    }

    protected void putInt(int value) {
        int padLen = ((mOffset + (Integer.BYTES - 1)) & ~(Integer.BYTES - 1)) - mOffset;
        if (padLen > 0) {
            mBuffer.put(new byte[padLen]);
        }
        mBuffer.putInt(value);
        mOffset += (Integer.BYTES + padLen);
    }

    protected long getLong() {
        mOffset += align(Long.BYTES);
        if ((mOffset + Long.BYTES) > mLength) {
            return 0;
        }
        mOffset += Long.BYTES;
        return mBuffer.getLong();
    }

    protected String getString() {
        int offset;

        if (PROTO64) {
            offset = Long.valueOf(getLong()).intValue();
        } else {
            offset = getInt();
        }
        if (offset == 0) {
            return "";
        }
        int length = 0;
        while (mBuffer.get(offset + length) != 0) {
            length++;
        }
        return new String(mBuffer.array(), offset, length);
    }

    protected byte[] getArray() {
        int offset;

        if (PROTO64) {
            offset = Long.valueOf(getLong()).intValue();
        } else {
            offset = getInt();
        }
        if (offset == 0) {
            return null;
        }
        int length = mBuffer.getInt(offset);
        if (length > 0) {
            byte[] res = new byte[length];
            for (int i = 0; i < length; i++) {
                res[i] = mBuffer.get(offset + Integer.BYTES + i);
            }
            return res;
        }
        return null;
    }

    protected byte[] getBytesArray(int size) {
        byte[] bytes = new byte[size];
        if ((mOffset + size) > mLength) {
             return bytes;
        }
        mBuffer.get(bytes);
        return bytes;
    }

    protected void putBytesArray(byte[] bytes) {
        if (bytes == null || bytes.length == 0) {
            return;
        }
        mBuffer.put(bytes);
        mOffset += bytes.length;
    }

    public byte[] getBytes() {
        return mBuffer.array();
    }

    @VisibleForTesting
    protected int getOffset() {
        return mOffset;
    }
}
