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

import com.android.internal.annotations.VisibleForTesting;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class RilSetCellularThresholds extends RilSolicitedMessage {
    private static final int THRESHOLDS_COUNT = 9;
    private static final int BUFFER_LENGTH = (1 + 4 + ((THRESHOLDS_COUNT * Short.BYTES) * 2));
    private static final int BUFFER_LENGTH_NO_DATA = (1 + 4);
    private static final byte MODE_STOP          = 0;
    private static final int MODE_CELL_OUT      = 1;
    private static final int MODE_CELL_IN       = 2;
    private static final int MODE_CELL_OUT_EXT  = 3;
    private static final int MODE_CELL_IN_EXT   = 4;
    private static final byte MODE_CELL_RANGE    = 5;

    private static final byte NOT_USED_BYTE = 0x7F;
    private static final short NOT_USED_SHORT = 0x7FFF;

    private int mMode;
    private CellularParameters mNgranThresholdsIn;
    private CellularParameters mEutranThresholdsIn;
    private CellularParameters mUtranThresholdsIn;
    private CellularParameters mNgranThresholdsOut;
    private CellularParameters mEutranThresholdsOut;
    private CellularParameters mUtranThresholdsOut;
    private CellularParameters mGeranThresholdsIn;
    private CellularParameters mGeranThresholdsOut;

    public RilSetCellularThresholds(RilOemInterface oemInterface) {
        super(RIL_SET_CELL_THRESHOLD, oemInterface);
        mNgranThresholdsIn = null;
        mEutranThresholdsIn = null;
        mUtranThresholdsIn = null;
        mNgranThresholdsOut = null;
        mEutranThresholdsOut = null;
        mUtranThresholdsOut = null;
        mGeranThresholdsIn = null;
        mGeranThresholdsOut = null;
    }

    public RilSetCellularThresholds(RilSetCellularThresholds from) {
        super(from);
        mNgranThresholdsIn = from.mNgranThresholdsIn;
        mNgranThresholdsOut = from.mNgranThresholdsOut;
        mEutranThresholdsIn = from.mEutranThresholdsIn;
        mEutranThresholdsOut = from.mEutranThresholdsOut;
        mUtranThresholdsIn = from.mUtranThresholdsIn;
        mUtranThresholdsOut = from.mUtranThresholdsOut;
        mGeranThresholdsIn = from.mGeranThresholdsIn;
        mGeranThresholdsOut = from.mGeranThresholdsOut;
    }

    public void setNgranThresholdsIn(CellularParameters parameters) {
        if (parameters == null) {
            return;
        }
        mNgranThresholdsIn = parameters;
    }

    public void setNgranThresholdsOut(CellularParameters parameters) {
        if (parameters == null) {
            return;
        }
        mNgranThresholdsOut = parameters;
    }

    public void setEutranThresholdsIn(CellularParameters parameters) {
        if (parameters == null) {
            return;
        }
        mEutranThresholdsIn = parameters;
    }

    public void setEutranThresholdsOut(CellularParameters parameters) {
        if (parameters == null) {
            return;
        }
        mEutranThresholdsOut = parameters;
    }

    public void setUtranThresholdsIn(CellularParameters parameters) {
        if (parameters == null) {
            return;
        }
        mUtranThresholdsIn = parameters;
    }

    public void setUtranThresholdsOut(CellularParameters parameters) {
        if (parameters == null) {
            return;
        }
        mUtranThresholdsOut = parameters;
    }

    public void setGeranThresholdsIn(CellularParameters parameters) {
        if (parameters == null) {
            return;
        }
        mGeranThresholdsIn = parameters;
    }

    public void setGeranThresholdsOut(CellularParameters parameters) {
        if (parameters == null) {
            return;
        }
        mGeranThresholdsOut = parameters;
    }

    public void startReports() {
        ByteBuffer buffer = ByteBuffer.allocate(BUFFER_LENGTH);
        buffer.order(ByteOrder.LITTLE_ENDIAN);

        mMode = MODE_CELL_RANGE;
        buffer.put(MODE_CELL_RANGE);

        buffer.put(NOT_USED_BYTE);
        buffer.put(NOT_USED_BYTE);
        buffer.put(NOT_USED_BYTE);
        buffer.put(NOT_USED_BYTE);

        writeThresholdsWithoutQuality(buffer, mUtranThresholdsIn);
        writeThresholds(buffer, mEutranThresholdsIn);
        writeThresholds(buffer, mNgranThresholdsIn);
        writeThresholdsWithoutQuality(buffer, mUtranThresholdsOut);
        writeThresholds(buffer, mEutranThresholdsOut);
        writeThresholds(buffer, mNgranThresholdsOut);
        writeThresholdsSignalOnly(buffer, mGeranThresholdsIn);
        writeThresholdsSignalOnly(buffer, mGeranThresholdsOut);

        setData(buffer.array());
        request();
    }

    public void stopReports() {
        ByteBuffer buffer = ByteBuffer.allocate(BUFFER_LENGTH_NO_DATA);
        buffer.order(ByteOrder.LITTLE_ENDIAN);

        mMode = MODE_STOP;
        buffer.put(MODE_STOP);

        buffer.put(NOT_USED_BYTE);
        buffer.put(NOT_USED_BYTE);
        buffer.put(NOT_USED_BYTE);
        buffer.put(NOT_USED_BYTE);

        setData(buffer.array());
        request();
    }

    private short IntegerToShort(int value) {
        return (value == 0) ? NOT_USED_SHORT :
                Integer.valueOf(value).shortValue();
    }

    private void writeThresholds(ByteBuffer buffer, CellularParameters parameters) {
        if (parameters == null) {
            buffer.putShort(NOT_USED_SHORT);
            buffer.putShort(NOT_USED_SHORT);
            buffer.putShort(NOT_USED_SHORT);
            return;
        }
        buffer.putShort(IntegerToShort(parameters.getSignal()));
        buffer.putShort(IntegerToShort(parameters.getNoise()));
        buffer.putShort(IntegerToShort(parameters.getQuality()));
    }

    private void writeThresholdsWithoutQuality(ByteBuffer buffer, CellularParameters parameters) {
        if (parameters == null) {
            buffer.putShort(NOT_USED_SHORT);
            buffer.putShort(NOT_USED_SHORT);
            return;
        }
        buffer.putShort(IntegerToShort(parameters.getSignal()));
        buffer.putShort(IntegerToShort(parameters.getNoise()));
    }

    private void writeThresholdsSignalOnly(ByteBuffer buffer, CellularParameters parameters) {
        if (parameters == null) {
            buffer.putShort(NOT_USED_SHORT);
            return;
        }
        buffer.putShort(IntegerToShort(parameters.getSignal()));
    }

    private boolean isParametersSame(CellularParameters first, CellularParameters second) {
        return ((first == null) && (second == null)) || ((first != null) && first.equals(second));
    }

    @Override
    public boolean equals(Object obj) {
        if (obj instanceof RilSetCellularThresholds) {
            if (obj == this) {
                return true;
            }
            RilSetCellularThresholds other = (RilSetCellularThresholds)obj;
            return (isParametersSame(mEutranThresholdsIn, other.mEutranThresholdsIn) &&
                    isParametersSame(mEutranThresholdsOut, other.mEutranThresholdsOut) &&
                    isParametersSame(mNgranThresholdsIn, other.mNgranThresholdsIn) &&
                    isParametersSame(mNgranThresholdsOut,other.mNgranThresholdsOut) &&
                    isParametersSame(mUtranThresholdsIn, other.mUtranThresholdsIn) &&
                    isParametersSame(mUtranThresholdsOut, other.mUtranThresholdsOut) &&
                    isParametersSame(mGeranThresholdsIn, other.mGeranThresholdsIn) &&
                    isParametersSame(mGeranThresholdsOut, other.mGeranThresholdsOut));
        }
        return false;
    }

    private String parameterToString(CellularParameters parameters) {
        if (parameters == null) {
            return "[null]";
        }
        return "[" + parameters.getSignal() + "," + parameters.getNoise() + "," + parameters.getQuality() + "]";
    }

    @Override
    public String toString() {
        if (mMode == MODE_STOP) {
            return ("RIL_SET_CELL_THRESHOLDS { STOP }");
        }
        return ("RIL_SET_CELL_THRESHOLDS { " +
                " 5G=" + parameterToString(mNgranThresholdsIn) + "/" + parameterToString(mNgranThresholdsOut) +
                " 4G=" + parameterToString(mEutranThresholdsIn) + "/" + parameterToString(mEutranThresholdsOut) +
                " 3G=" + parameterToString(mUtranThresholdsIn) + "/" + parameterToString(mUtranThresholdsOut) +
                " 2G=" + parameterToString(mGeranThresholdsIn) + "/" + parameterToString(mGeranThresholdsOut) + " }");
    }

    @VisibleForTesting int getMode() { return mMode; };
}
