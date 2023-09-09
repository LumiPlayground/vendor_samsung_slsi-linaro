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

import android.os.Build;
import android.telephony.Rlog;

import com.android.internal.annotations.VisibleForTesting;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.Arrays;
import java.util.LinkedList;

public class StrokeRequestMessage {
    protected boolean DBG = true;
    public static final boolean PROTO64 = (Build.SUPPORTED_64_BIT_ABIS.length > 0);

    public enum RequestMessageType {
        /* original */
        STR_REQ_INITIATE(0),
        STR_REQ_ROUTE(1),
        STR_REQ_UNROUTE(2),
        STR_REQ_ADD_CONN(3),
        STR_REQ_DEL_CONN(4),
        STR_REQ_TERMINATE(5),
        STR_REQ_TERMINATE_SRCIP(6),
        STR_REQ_REKEY(7),
        STR_REQ_STATUS(8),
        STR_REQ_STATUS_ALL(9),
        STR_REQ_STATUS_ALL_NOBLK(10),
        STR_REQ_ADD_CA(11),
        STR_REQ_DEL_CA(12),
        STR_REQ_LOGLEVEL(13),
        STR_REQ_CONFIG(14),
        STR_REQ_LIST(15),
        STR_REQ_REREAD(16),
        STR_REQ_PURGE(17),
        STR_REQ_LEASES(18),
        STR_REQ_EXPORT(19),
        STR_REQ_MEMUSAGE(20),
        STR_REQ_USER_CREDS(21),
        STR_REQ_COUNTERS(22),
        /* added */
        STR_REQ_ADD_ROUTE(23),
        STR_REQ_DEL_ROUTE(24),
        STR_REQ_SET_INTERFACE(25);

        private int mCode;

        RequestMessageType(int code) { mCode = code; }
        public int getCode() { return mCode; }
    }

    private class PackedField {
        final String mName;
        final int mLength;
        final int mPadLength;
        Object mValue;

        PackedField(String name) {
            mName = name;
            mLength = 0;
            mValue = null;
            mPadLength = ((mStringsOffset + (getOffsetSize() - 1)) & ~(getOffsetSize() - 1)) - mStringsOffset;
        }

        PackedField(String name, int length, byte value) {
            mName = name;
            mLength = 0;
            mValue = null;
            mPadLength = ((mStringsOffset + (length - 1)) & ~(length - 1)) - mStringsOffset;
        }

        PackedField(String name, byte value) {
            mName = name;
            mLength = Byte.BYTES;
            mValue = value;
            mPadLength = 0;
        }

        PackedField(String name, short value) {
            mName = name;
            mLength = Short.BYTES;
            mValue = value;
            mPadLength = ((mStringsOffset + (mLength - 1)) & ~(mLength - 1)) - mStringsOffset;
        }

        PackedField(String name, int value) {
            mName = name;
            mLength = Integer.BYTES;
            mValue = value;
            mPadLength = ((mStringsOffset + (mLength - 1)) & ~(mLength - 1)) - mStringsOffset;
        }

        PackedField(String name, long value) {
            mName = name;
            mLength = Long.BYTES;
            mValue = value;
            mPadLength = ((mStringsOffset + (mLength - 1)) & ~(mLength - 1)) - mStringsOffset;
        }

        PackedField(String name, float value) {
            mName = name;
            mLength = Float.BYTES;
            mValue = value;
            mPadLength = ((mStringsOffset + (mLength - 1)) & ~(mLength - 1)) - mStringsOffset;
        }

        PackedField(String name, int length, String value) {
            mName = name;
            mLength = length;
            mValue = value;
            mPadLength = ((mStringsOffset + (getOffsetSize() - 1)) & ~(getOffsetSize() - 1)) - mStringsOffset;
        }

        PackedField(String name, byte[] value) {
            mName = name;
            mLength = (value != null) ? value.length : 0;
            mValue = value;
            mPadLength = ((mStringsOffset + (getOffsetSize() - 1)) & ~(getOffsetSize() - 1)) - mStringsOffset;
        }

        protected int getOffsetSize() {
            return (PROTO64 ? Long.BYTES : Integer.BYTES);
        }

        public int getSize() { return mLength + mPadLength; }

        public void pack(ByteBuffer buffer) {
            if (mPadLength > 0) {
                buffer.put(new byte[mPadLength]);
            }
            if (mValue instanceof Byte) {
                buffer.put((byte)mValue);
            } else if (mValue instanceof Short) {
                buffer.putShort((short)mValue);
            } else if (mValue instanceof Integer) {
                buffer.putInt((int)mValue);
            } else if (mValue instanceof Long) {
                buffer.putLong((long)mValue);
            } else if (mValue instanceof Float) {
                buffer.putFloat((float)mValue);
            }
        }
    }

    private class StringPackedField extends PackedField {
        StringPackedField(String name, String value) {
            super(name, (value != null) ? (value.length() + 1) : 0, value);
        }

        public int getPtrSize() {
            return (mPadLength + getOffsetSize());
        }

        @Override
        public int getSize() { return (mLength + mPadLength + getOffsetSize()); }

        private void putOffset(ByteBuffer buffer, int offset) {
            if (PROTO64) {
                buffer.putLong(offset);
            } else {
                buffer.putInt(offset);
            }
        }

        @Override
        public void pack(ByteBuffer buffer) {
            if (mPadLength > 0) {
                buffer.put(new byte[mPadLength]);
            }

            String value = (String)mValue;
            if (value != null && !value.isEmpty()) {
                try {
                    putOffset(buffer, mStringsOffset);
                    for (byte b : value.getBytes("UTF-8")) {
                        buffer.put(mStringsOffset++, b);
                    }
                    buffer.put(mStringsOffset++, (byte) 0);
                } catch (Exception e) {
                    loge("Failed to put " + value + " to message: " + e);
                    buffer.put(mStringsOffset++, (byte) 0);
                }
            } else {
                putOffset(buffer, 0);
            }
        }
    }

    private class ArrayPackedField extends PackedField {
        ArrayPackedField(String name, byte[] value) {
            super(name, value);
        }

        public int getPtrSize() {
            return (mPadLength + getOffsetSize());
        }

        @Override
        public int getSize() { return (mLength + mPadLength + getOffsetSize()); }

        private void putOffset(ByteBuffer buffer, int offset) {
            if (PROTO64) {
                buffer.putLong(offset);
            } else {
                buffer.putInt(offset);
            }
        }

        @Override
        public void pack(ByteBuffer buffer) {
            if (mPadLength > 0) {
                buffer.put(new byte[mPadLength]);
            }

            byte[] value = (byte[])mValue;
            if (value != null) {
                try {
                    putOffset(buffer, mStringsOffset);
                    for (byte b : value) {
                        buffer.put(mStringsOffset++, b);
                    }
                } catch (Exception e) {
                    loge("Failed to put " + mName + " to message: " + e);
                }
            } else {
                putOffset(buffer, 0);
            }
        }
    }

    private final String TAG;
    private int mTotalLength;
    private int mStringsOffset;
    private final LinkedList<PackedField> mPackingList;

    private static int mMinimumOffset = 0;

    public StrokeRequestMessage(RequestMessageType request) {
        TAG = request.toString();

        mTotalLength = 0;
        mStringsOffset = 0;
        mPackingList = new LinkedList<>();

        putShort("length", 0);
        putInt("type", request.getCode());
        putInt("output_verbosity", 0);
        log("64bit: " + PROTO64 + ", Build.SUPPORTED_64_BIT_ABIS: " + Arrays.toString(Build.SUPPORTED_64_BIT_ABIS));
    }

    public byte[] getBytes() {
        if (PROTO64) {
            putLong("out", 0);
        } else {
            putInt("out", 0);
        }
        putShort("buflen", 0);

        if (mMinimumOffset > mStringsOffset) {
            int oldLength = mTotalLength;

            mTotalLength += (mMinimumOffset - mStringsOffset);
            mStringsOffset = mMinimumOffset;
            log("Adjust message size to " + mTotalLength + " bytes from " + oldLength);
        }

        setLength();

        ByteBuffer buffer = ByteBuffer.allocate(mTotalLength);
        buffer.order(ByteOrder.nativeOrder());
        for (PackedField p : mPackingList) {
            log("pack " + p.mName + "=" + p.mValue);
            p.pack(buffer);
        }
        return buffer.array();
    }

    /* should be called once for add_conn as longest message */
    public void setMinimumLength() {
        if (PROTO64) {
            putLong("out", 0);
        } else {
            putInt("out", 0);
        }
        putShort("buflen", 0);

        mMinimumOffset = mStringsOffset;
        log("Set minimum message size to " + mMinimumOffset);
    }

    protected void align() {
        PackedField s = new PackedField("align");

        int padLen = s.getSize();
        if (padLen > 0) {
            mPackingList.add(s);

            mTotalLength += padLen;
            mStringsOffset += padLen;
        }
    }

    protected void align(int length) {
        PackedField s = new PackedField("align" + length, length, (byte)0);

        int padLen = s.getSize();
        if (padLen > 0) {
            mPackingList.add(s);

            mTotalLength += padLen;
            mStringsOffset += padLen;
        }
    }

    protected void putByte(String name, int value) {
        PackedField s = new PackedField(name, Integer.valueOf(value).byteValue());
        mPackingList.add(s);

        mTotalLength += s.getSize();
        mStringsOffset += s.getSize();
    }

    protected void putShort(String name, int value) {
        PackedField s = new PackedField(name, Integer.valueOf(value).shortValue());
        mPackingList.add(s);

        mTotalLength += s.getSize();
        mStringsOffset += s.getSize();
    }

    protected void putInt(String name, int value) {
        PackedField s = new PackedField(name, value);
        mPackingList.add(s);

        mTotalLength += s.getSize();
        mStringsOffset += s.getSize();
    }

    protected void putLong(String name, long value) {
        PackedField s = new PackedField(name, value);
        mPackingList.add(s);

        mTotalLength += s.getSize();
        mStringsOffset += s.getSize();
    }

    protected void putLongOrInt(String name, long value) {
        if (PROTO64) {
            putLong(name, value);
        } else {
            putInt(name, Long.valueOf(value).intValue());
        }
    }

    protected void putFloat(String name, float value) {
        PackedField s = new PackedField(name, value);
        mPackingList.add(s);

        mTotalLength += s.getSize();
        mStringsOffset += s.getSize();
    }

    protected void putTime(String name, long value) {
        putLongOrInt(name, value);
    }

    protected void putString(String name, String value) {
        StringPackedField s = new StringPackedField(name, value);
        mPackingList.add(s);

        mTotalLength += s.getSize();
        mStringsOffset += s.getPtrSize();
    }

    protected void putBytes(String name, byte[] value) {
        ArrayPackedField s = new ArrayPackedField(name, value);
        mPackingList.add(s);

        mTotalLength += s.getSize();
        mStringsOffset += s.getPtrSize();
    }

    private void setLength() {
        PackedField s = mPackingList.peekFirst();
        if (s != null) {
            s.mValue = Integer.valueOf(mTotalLength).shortValue();
        }
    }

    protected void log(String s) {
        if (DBG) {
            Rlog.d(TAG, s);
        }
    }

    protected void loge(String s) {
        Rlog.e(TAG, s);
    }

    @VisibleForTesting
    protected int getTotalLength(){
        return mTotalLength;
    }
}
