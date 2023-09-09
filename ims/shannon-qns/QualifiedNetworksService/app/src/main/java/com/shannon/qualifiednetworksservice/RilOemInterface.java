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

import android.os.Handler;
import android.os.HwBinder;
import android.os.Message;
import android.util.SparseArray;

import com.android.internal.annotations.VisibleForTesting;
import com.shannon.qualifiednetworksservice.util.LogUtils;
import com.shannon.qualifiednetworksservice.util.StringUtils;

import java.util.ArrayList;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.atomic.AtomicLong;

import vendor.samsung_slsi.telephony.hardware.radioExternal.V1_0.IOemSlsiRadioExternal;
import vendor.samsung_slsi.telephony.hardware.radioExternal.V1_0.IOemSlsiRadioExternalInd;
import vendor.samsung_slsi.telephony.hardware.radioExternal.V1_0.IOemSlsiRadioExternalRes;
import vendor.samsung_slsi.telephony.hardware.radioExternal.V1_0.RadioExternalResponseInfo;

public class RilOemInterface {
    public static final int RIL_RESPONSE_SUCCESS  = 0;
    public static final int RIL_RESPONSE_FAIL = 1;
    public static final int RIL_RESPONSE_NOT_CONNECTED = 2;
    public static final int RIL_RESPONSE_INVALID_PARAM = 3;
    public static final int RIL_RESPONSE_IO_ERROR = 4;
    public static final int RIL_RESPONSE_NO_RESOURCES = 5;

    public static final int RIL_UNSOLICITED_MESSAGE = 1;

    final class RadioProxyDeathRecipient implements HwBinder.DeathRecipient {
        @Override
        public void serviceDied(long cookie) {
            // Deal with service going away
            if (mLinkCookie.get() == cookie) {
                LogUtils.w(TAG, "RIL service died");
                mOemRil = null;
                mLinkCookie.incrementAndGet();
            }
        }
    }

    private static class RilOemSegmentedMessage {
        private final SparseArray<ArrayList<Byte>> mMessage;
        private final int mTotalLength;
        private int mLength;

        public RilOemSegmentedMessage(int totalLength) {
            mLength = 0;
            mTotalLength = totalLength;
            mMessage = new SparseArray<>();
        }

        public boolean isLast() { return (mLength >= mTotalLength); }

        public void addSegment(ArrayList<Byte> data, int index) {
            mMessage.put(index, data);
            mLength += data.size();
        }

        public byte[] getBytes() {
            byte[] data = new byte[mTotalLength];
            for (int i = 0, offs = 0; i < mMessage.size(); i++) {
                ArrayList<Byte> segment = mMessage.get(i);
                if (segment != null) {
                    for (int j = 0; j < segment.size(); j++) {
                        if (offs > mTotalLength) {
                            return data;
                        }
                        data[offs++] = segment.get(j);
                    }
                }
            }
            return data;
        }
    }

    private final String TAG;
    private final int mSlotId;
    private final AtomicInteger mTidGenerator;
    private final AtomicLong mLinkCookie;
    private final RilOemResponseCallback mResponseCallback;
    private final RilOemIndicationCallback mIndicationCallback;
    private final RadioProxyDeathRecipient mDeathLink;
    private final SparseArray<RilSolicitedMessage> mRequestList;
    private final SparseArray<Handler> mUnsolicitedListeners;
    private final SparseArray<RilOemSegmentedMessage> mSegmentedMessages;
    private IOemSlsiRadioExternal mOemRil;
    private int mClientId;

    public RilOemInterface(int slotId) {
        this(slotId, false);
    }

    @VisibleForTesting
    protected RilOemInterface(int slotId, boolean doNotSetupCallbacks) {
        mSlotId = slotId;
        mTidGenerator = new AtomicInteger(1);
        mLinkCookie = new AtomicLong(0);
        mRequestList = new SparseArray<>();
        mUnsolicitedListeners = new SparseArray<>();
        mSegmentedMessages = new SparseArray<>();
        mDeathLink = new RadioProxyDeathRecipient();

        // need to separate for testing
        if (doNotSetupCallbacks) {
            mResponseCallback = null;
            mIndicationCallback = null;
        } else {
            mResponseCallback = new RilOemResponseCallback(this);
            mIndicationCallback = new RilOemIndicationCallback(this);
        }

        TAG = ShannonQualifiedNetworksService.getServiceTAG("-" + mSlotId + "-RIL");

        attach();
    }

    public void destroy() {
        detach();
    }

    private void attach() {
        LogUtils.d(TAG, "attach to RIL, slot " + mSlotId);
        try {
            mOemRil = IOemSlsiRadioExternal.getService("rilExternal");
            if (mOemRil != null) {
                mClientId = mOemRil.setResponseFunctions(mResponseCallback, mIndicationCallback);
                mOemRil.linkToDeath(mDeathLink, mLinkCookie.incrementAndGet());
            }
        } catch (Exception e) {
            LogUtils.e(TAG, "Failed to attach to RIL " + e);
        }
    }

    private void detach() {
        LogUtils.d(TAG, "detach from RIL, slot " + mSlotId);
        try {
            mOemRil.clearResponseFunctions(mClientId);
            mOemRil = null;
        } catch (Exception e) {
            LogUtils.e(TAG, "Failed to detach from RIL " + e);
        }
    }

    public void onResponse(int tid, int error, byte[] data, int len) {
        RilSolicitedMessage msg = mRequestList.get(tid);
        if (msg != null) {
            synchronized (mRequestList) {
                mRequestList.remove(tid);
            }
            msg.onResponse(error, data, len);
        }
    }

    public void sendRequest(RilSolicitedMessage msg) {
        if (mOemRil == null) {
            attach();

            if (mOemRil == null) {
                LogUtils.e(TAG, "Not recoverable error. Could not re-attach to RIL");
                return;
            }
        }
        int tid = mTidGenerator.getAndIncrement();
        synchronized (mRequestList) {
            mRequestList.put(tid, msg);
        }
        try {
            mOemRil.sendRequestRaw(tid, mClientId, msg.getMessageId(), mSlotId,
                    msg.getDataLength(), StringUtils.primitiveArrayToArrayList(msg.getData()));
        } catch (Exception e) {
            LogUtils.e(TAG, "Failed to send message to RIL " + e);
        }
    }

    public void registerUnsolicitedListener(int msgId, Handler h) {
        mUnsolicitedListeners.put(msgId, h);
    }

    public void unregisterUnsolicitedListener(int msgId) {
        mUnsolicitedListeners.remove(msgId);
    }

    public void onIndication(int slotId, int msgId, byte[] data, int len) {
        if (slotId != mSlotId) {
            return;
        }
        LogUtils.v(TAG, "Received unsolicited message " + msgId);
        Handler h = mUnsolicitedListeners.get(msgId);
        if (h != null) {
            RilUnsolicitedMessage ind = new RilUnsolicitedMessage(msgId, data);

            Message msg = Message.obtain(h, RIL_UNSOLICITED_MESSAGE);
            msg.obj = ind;

            msg.sendToTarget();
        }
    }

    public void onIndication(int slotId, int msgId, ArrayList<Byte> data, int index, int totalLength) {
        if (slotId != mSlotId) {
            return;
        }
        LogUtils.v(TAG, "Received unsolicited segmented message " + msgId + " length=" + totalLength);
        Handler h = mUnsolicitedListeners.get(msgId);
        if (h != null) {
            LogUtils.d(TAG, "Message " + msgId + ". Segment " + index + " data length=" + data.size());
            RilOemSegmentedMessage segmentedMessage = mSegmentedMessages.get(msgId);
            if (segmentedMessage == null) {
                segmentedMessage = new RilOemSegmentedMessage(totalLength);
                mSegmentedMessages.put(msgId, segmentedMessage);
            }
            segmentedMessage.addSegment(data, index);
            if (segmentedMessage.isLast()) {
                LogUtils.d(TAG, "Last segment for " + msgId);
                mSegmentedMessages.remove(msgId);

                RilUnsolicitedMessage ind = new RilUnsolicitedMessage(msgId, segmentedMessage.getBytes());

                Message msg = Message.obtain(h, RIL_UNSOLICITED_MESSAGE);
                msg.obj = ind;

                msg.sendToTarget();
            }
        }
    }

    @VisibleForTesting
    protected int putSolicitedMessageToList(RilSolicitedMessage message) {
        int tid = mTidGenerator.getAndIncrement();
        synchronized (mRequestList) {
            mRequestList.put(tid, message);
        }
        return tid;
    }

    @VisibleForTesting
    protected RadioProxyDeathRecipient getDeathLink() { return mDeathLink; }
}
