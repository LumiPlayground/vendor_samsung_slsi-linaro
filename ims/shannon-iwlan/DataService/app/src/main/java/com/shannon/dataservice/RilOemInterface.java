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
package com.shannon.dataservice;

import android.os.HwBinder;
import android.telephony.Rlog;
import android.util.SparseArray;

import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.atomic.AtomicLong;

import com.android.internal.annotations.VisibleForTesting;
import vendor.samsung_slsi.telephony.hardware.radioExternal.V1_0.IOemSlsiRadioExternal;
import vendor.samsung_slsi.telephony.hardware.radioExternal.V1_0.IOemSlsiRadioExternalInd;
import vendor.samsung_slsi.telephony.hardware.radioExternal.V1_0.IOemSlsiRadioExternalRes;
import vendor.samsung_slsi.telephony.hardware.radioExternal.V1_0.RadioExternalResponseInfo;

import com.android.internal.annotations.VisibleForTesting;

public class RilOemInterface {
    private static final boolean DBG = true;

    public static final int RIL_RESPONSE_SUCCESS  = 0;
    public static final int RIL_RESPONSE_FAIL = 1;
    public static final int RIL_RESPONSE_NOT_CONNECTED = 2;
    public static final int RIL_RESPONSE_INVALID_PARAM = 3;
    public static final int RIL_RESPONSE_IO_ERROR = 4;
    public static final int RIL_RESPONSE_NO_RESOURCES = 5;

    private final String TAG;
    private final int mSlotId;
    private final AtomicInteger mTidGenerator;
    private final AtomicLong mLinkCookie;
    private final RilOemResponseCallback mResponseCallback;
    private final RadioProxyDeathRecipient mDeathLink;
    private final SparseArray<RilSolicitedMessage> mRequestList;
    private IOemSlsiRadioExternal mOemRil;
    private int mClientId;

    final class RadioProxyDeathRecipient implements HwBinder.DeathRecipient {
        @Override
        public void serviceDied(long cookie) {
            // Deal with service going away
            if (mLinkCookie.get() == cookie) {
                loge("RIL service died");
                mOemRil = null;
                mLinkCookie.incrementAndGet();
            }
        }
    }

    public RilOemInterface(int slotId) {
        mSlotId = slotId;
        mTidGenerator = new AtomicInteger(1);
        mLinkCookie = new AtomicLong(0);
        mRequestList = new SparseArray<>();
        mDeathLink = new RadioProxyDeathRecipient();
        mResponseCallback = new RilOemResponseCallback(this);

        TAG = ShannonDataService.getServiceTAG("-RIL-" + mSlotId);

        attach();
    }

    @VisibleForTesting
    protected RilOemInterface(int slotId, SparseArray sparseArray,
            RilOemResponseCallback rilOemResponseCallback) {
        mSlotId = slotId;
        mTidGenerator = new AtomicInteger(1);
        mLinkCookie = new AtomicLong(0);
        mRequestList = sparseArray;
        mDeathLink = new RadioProxyDeathRecipient();
        mResponseCallback = rilOemResponseCallback;

        TAG = ShannonDataService.getServiceTAG("-RIL-" + mSlotId);

        attach();
    }

    public void destroy() {
        detach();
    }

    private void attach() {
        log("attach to RIL, slot " + mSlotId);
        try {
            mOemRil = IOemSlsiRadioExternal.getService("rilExternal");
            if (mOemRil != null) {
                mClientId = mOemRil.setResponseFunctions(mResponseCallback, null);
                mOemRil.linkToDeath(mDeathLink, mLinkCookie.incrementAndGet());
            }
        } catch (Exception e) {
            loge("Failed to attach to RIL " + e);
        }
    }

    private void detach() {
        log("detach from RIL, slot " + mSlotId);
        try {
            mOemRil.clearResponseFunctions(mClientId);
            mOemRil = null;
        } catch (Exception e) {
            loge("Failed to detach from RIL " + e);
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
                loge("Not recoverable error. Could not re-attach to RIL");
                return;
            }
        }
        int tid = mTidGenerator.getAndIncrement();
        synchronized (mRequestList) {
            mRequestList.put(tid, msg);
        }
        try {
            mOemRil.sendRequestRaw(tid, mClientId, msg.getMessageId(), mSlotId, msg.getDataLength(), StringUtils.primitiveArrayToArrayList(msg.getData()));
        } catch (Exception e) {
            loge("Failed to send message to RIL " + e);
        }
    }

    public byte[] getAuthenticationResponse(byte[] nonce, boolean isUsim) {
        RilGetAuthenticationResponse msg = new RilGetAuthenticationResponse(this, nonce, isUsim);
        byte[] bRes = msg.request();
        if (bRes != null) {
            String sRes = new String(bRes);
            log("Authentication response: " + sRes);
            return StringUtils.hexStringToByteArray(sRes);
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
    protected RadioProxyDeathRecipient getDeathLink() { return mDeathLink; }
}
