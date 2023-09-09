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

import android.telephony.Rlog;
import android.text.TextUtils;

import com.android.internal.annotations.VisibleForTesting;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.LinkedList;

public class IkeNotifyProcessor {
    /* source 3GPP TS 24.302 8.1.2.3 */
    public static final int REACTIVATION_REQUESTED_CAUSE = 40961;
    public static final int BACKOFF_TIMER = 41041;
    public static final int PDN_TYPE_IPv4_ONLY_ALLOWED = 41050;
    public static final int PDN_TYPE_IPv6_ONLY_ALLOWED = 41051;
    public static final int DEVICE_IDENTITY = 41101;
    public static final int EMERGENCY_SUPPORT = 41112;
    public static final int EMERGENCY_CALL_NUMBERS = 41134;
    public static final int NBIFOM_GENERIC_CONTAINER = 41288;
    public static final int P_CSCF_RESELECTION_SUPPORT = 41304;
    public static final int PTI = 41501;
    public static final int IKEV2_MULTIPLE_BEARER_PDN_CONNECTIVITY = 42011;
    public static final int EPS_QOS = 42014;
    public static final int EXTENDED_EPS_QOS = 42015;
    public static final int TFT = 42017;
    public static final int MODIFIED_BEARER = 42020;
    public static final int APN_AMBR = 42094;
    public static final int EXTENDED_APN_AMBR = 42095;
    public static final int N1_MODE_CAPABILITY = 51015;
    public static final int N1_MODE_INFORMATION = 51115;
    public static final int N1_MODE_S_NSSAI_PLMN_ID = 52216;

    private static final boolean DBG = true;

    private final String TAG;
    private final LinkedList<IkeNotifyData> mSendList;
    private final LinkedList<Integer> mExpectedList;
    private final LinkedList<IkeNotifyData> mReceiveList;

    public IkeNotifyProcessor(String tag) {
        TAG = tag + "-ntfy";

        mSendList = new LinkedList<>();
        mExpectedList = new LinkedList<>();
        mReceiveList = new LinkedList<>();
    }

    @VisibleForTesting
    protected IkeNotifyProcessor(String tag, LinkedList sendList, LinkedList expectedList,
                                 LinkedList receiveList) {
        TAG = tag + "-ntfy";

        mSendList = sendList;
        mExpectedList = expectedList;
        mReceiveList = receiveList;
    }

    public void sendN1ModeCapability(int pduSessionId) {
        IkeNotifyN1ModeCapability capability = new IkeNotifyN1ModeCapability(pduSessionId);
        mSendList.add(capability);
    }

    public void sendDeviceIdentity(String Imei) {
        IkeNotifyDeviceIdentity identity = new IkeNotifyDeviceIdentity(Imei);
        mSendList.add(identity);
    }

    public void addExpectedNotify(int type) {
        mExpectedList.add(type);
    }

    public byte[] getOutgoingNotifyBytes() {
        if (mSendList.isEmpty()) {
            log("No notifies requested");
            return null;
        }

        /* create last, empty one */
        mSendList.add(new IkeNotifyData(0, null));

        int total = 0;
        for (IkeNotifyData data : mSendList) {
            total += data.getBytesSize();
        }

        ByteBuffer buffer = ByteBuffer.allocate(total);
        for (IkeNotifyData data : mSendList) {
            log("Send " + data);
            buffer.put(data.getBytes());
        }
        return buffer.array();
    }

    public byte[] getExpectedNotifyBytes() {
        if (mExpectedList.isEmpty()) {
            log("No notifies expected");
            return null;
        }
        int count = mExpectedList.size();

        ByteBuffer buffer = ByteBuffer.allocate(count * Short.BYTES + Integer.BYTES);
        buffer.order(ByteOrder.nativeOrder());
        buffer.putInt(count);
        for (Integer value : mExpectedList) {
            log("Expected " + value + " notify");
            buffer.putShort(value.shortValue());
        }
        return buffer.array();
    }

    public void processReceivedNotifies(byte[] bytes) {
        if (bytes == null) {
            return;
        }
        try {
            ByteBuffer buffer = ByteBuffer.wrap(bytes);
            buffer.order(ByteOrder.nativeOrder());

            IkeNotifyData data = new IkeNotifyData(buffer);
            while (data.getType() != 0) {
                log("Received " + data);
                mReceiveList.add(data);
                data = new IkeNotifyData(buffer);
            }
        } catch (Exception e) {
            loge("Failed to process received notify data: " + e);
        }
    }

    public IkeNotifyBackoffTimer getBackoffTimer() {
        if (mReceiveList.isEmpty()) {
            return new IkeNotifyBackoffTimer();
        }
        for (IkeNotifyData data : mReceiveList) {
            if (data.getType() == BACKOFF_TIMER) {
                return new IkeNotifyBackoffTimer(data);
            }
        }
        return new IkeNotifyBackoffTimer();
    }

    public IkeNotifyN1ModeInformation getN1ModeInformation() {
        if (mReceiveList.isEmpty()) {
            return null;
        }
        for (IkeNotifyData data : mReceiveList) {
            if (data.getType() == N1_MODE_INFORMATION) {
                return new IkeNotifyN1ModeInformation(data);
            }
        }
        return null;
    }

    public IkeNotifyN1ModeData getN1ModeData(int type) {
        if (mReceiveList.isEmpty()) {
            return null;
        }
        for (IkeNotifyData data : mReceiveList) {
            if (data.getType() == type) {
                return new IkeNotifyN1ModeData(data);
            }
        }
        return null;
    }

    private void log(String s) {
        if (DBG) {
            Rlog.d(TAG, s);
        }
    }

    private void loge(String s) {
        Rlog.e(TAG, s);
    }

    @VisibleForTesting
    LinkedList<IkeNotifyData> getSendList() {
        return mSendList;
    }

    @VisibleForTesting
    LinkedList<Integer> getExpectedList() {
        return mExpectedList;
    }
}
