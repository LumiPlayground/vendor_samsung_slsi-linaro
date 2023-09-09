/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package com.samsung.slsi.telephony.oemril;

import android.os.AsyncResult;
import android.os.Message;
import android.os.SystemClock;
import android.os.WorkSource;
import android.os.WorkSource.WorkChain;

import com.android.internal.telephony.CommandException;
import com.android.telephony.Rlog;

import java.util.List;
import java.util.Random;
import java.util.concurrent.atomic.AtomicInteger;

/**
 * {@hide}
 */

public class RILRequest {
    static final String LOG_TAG = "RilRequest";

    //***** Class Variables
    static Random sRandom = new Random();
    static AtomicInteger sNextSerial = new AtomicInteger(0);
    private static Object sPoolSync = new Object();
    private static RILRequest sPool = null;
    private static int sPoolSize = 0;
    private static final int MAX_POOL_SIZE = 4;

    //***** Instance Variables
    int mSerial;
    int mRequest;
    Message mResult;
    RILRequest mNext;
    WorkSource mWorkSource;
    String mClientId;
    // time in ms when RIL request was made
    long mStartTimeMs;
    /** Argument list for radio HAL fallback method call */
    Object[] mArguments;

    public int getSerial() {
        return mSerial;
    }

    public int getRequest() {
        return mRequest;
    }

    public Message getResult() {
        return mResult;
    }

    /**
     * Retrieves a new RILRequest instance from the pool.
     *
     * @param request RIL_REQUEST_*
     * @param result sent when operation completes
     * @return a RILRequest instance from the pool.
     */
    private static RILRequest obtain(int request, Message result) {
        RILRequest rr = null;

        synchronized (sPoolSync) {
            if (sPool != null) {
                rr = sPool;
                sPool = rr.mNext;
                rr.mNext = null;
                sPoolSize--;
            }
        }

        if (rr == null) {
            rr = new RILRequest();
        }

        // Increment serial number. Wrap to 0 when reaching Integer.MAX_VALUE.
        rr.mSerial = sNextSerial.getAndUpdate(n -> ((n + 1) % Integer.MAX_VALUE));

        rr.mRequest = request;
        rr.mResult = result;

        rr.mWorkSource = null;
        rr.mStartTimeMs = SystemClock.elapsedRealtime();
        if (result != null && result.getTarget() == null) {
            throw new NullPointerException("Message target must not be null");
        }

        return rr;
    }


    /**
     * Retrieves a new RILRequest instance from the pool and sets the clientId
     *
     * @param request RIL_REQUEST_*
     * @param result sent when operation completes
     * @param workSource WorkSource to track the client
     * @return a RILRequest instance from the pool.
     */
    public static RILRequest obtain(int request, Message result, WorkSource workSource) {
        RILRequest rr = obtain(request, result);

        if (workSource != null) {
            rr.mWorkSource = workSource;
            rr.mClientId = rr.getWorkSourceClientId();
        } else {
            Rlog.e(LOG_TAG, "null workSource " + request);
        }

        return rr;
    }

    /**
     * Generate a String client ID from the WorkSource.
     */
    public String getWorkSourceClientId() {
        if (mWorkSource == null || mWorkSource.isEmpty()) {
            return null;
        }

        if (mWorkSource.size() > 0) {
            return mWorkSource.getUid(0) + ":" + mWorkSource.getPackageName(0);
        }

        final List<WorkChain> workChains = mWorkSource.getWorkChains();
        if (workChains != null && !workChains.isEmpty()) {
            final WorkChain workChain = workChains.get(0);
            return workChain.toString();
        }

        return null;
    }

    /**
     * Returns a RILRequest instance to the pool.
     *
     * Note: This should only be called once per use.
     */
    void release() {
        synchronized (sPoolSync) {
            if (sPoolSize < MAX_POOL_SIZE) {
                mNext = sPool;
                sPool = this;
                sPoolSize++;
                mResult = null;
                mArguments = null;
            }
        }
    }

    private RILRequest() {
    }

    static void resetSerial() {
        // Use a non-negative random number so that on recovery we probably don't mix old requests
        // with new.
        sNextSerial.set(sRandom.nextInt(Integer.MAX_VALUE));
    }

    String serialString() {
        //Cheesy way to do %04d
        StringBuilder sb = new StringBuilder(8);
        String sn;

        // Truncate mSerial to a number with maximum 4 digits.
        int adjustedSerial = mSerial % 10000;
        sn = Integer.toString(adjustedSerial);

        //sb.append("J[");
        sb.append('[');
        for (int i = 0, s = sn.length(); i < 4 - s; i++) {
            sb.append('0');
        }

        sb.append(sn);
        sb.append(']');
        return sb.toString();
    }

    void onError(final int error, final Object ret) {
        final CommandException ex = CommandException.fromRilErrno(error);

        final Message result = mResult;
        if (VendorRIL.RILJ_LOGD) {
            Rlog.d(LOG_TAG, serialString() + "< "
                    + VendorRIL.requestToString(mRequest)
                    + " error: " + ex + " ret=" + VendorRIL.retToString(mRequest, ret)
                    + " result=" + result);
        }

        if (result != null && result.getTarget() != null) {
            AsyncResult.forMessage(result, ret, ex);
            result.sendToTarget();
        }
    }
}
