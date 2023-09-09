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

import android.annotation.NonNull;
import android.os.AsyncResult;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.telephony.Rlog;
import android.telephony.data.DataProfile;
import vendor.samsung_slsi.telephony.hardware.radio.V1_0.NasTimerStatus;

public class VendorTelephonyTracker extends Handler {

    private static final String LOG_TAG = "VendorTelephonyTracker";
    private VendorRadioContext mRadioContext;
    private int mPhoneId;
    private VendorCommandsInterface mCi;
    private String mIccid;
    private int mSimTrayState = VendorRadioInterfaceManager.SIM_TRAY_STATE_UNKNOWN;

    public static final int EVENT_ICCID_INFO_UPDATED =
            VendorRadioInterfaceManager.EVENT_ICCID_INFO_UPDATED;
    public static final int EVENT_NR_DUAL_CONNECTIVITY_STATE_CHANGED =
            VendorRadioInterfaceManager.EVENT_NR_DUAL_CONNECTIVITY_STATE_CHANGED ;
    public static final int EVENT_SIM_TRAY_STATE_CHANGED =
            VendorRadioInterfaceManager.EVENT_SIM_TRAY_STATE_CHANGED;
    public static final int EVENT_NAS_TIMER_STATUS_CHANGED =
            VendorRadioInterfaceManager.EVENT_NAS_TIMER_STATUS_CHANGED;
    public static final int EVENT_UNTHROTTLE_APN =
            VendorRadioInterfaceManager.EVENT_UNTHROTTLE_APN;

    public VendorTelephonyTracker(@NonNull VendorRadioContext radioContext,
            @NonNull VendorCommandsInterface ci) {
        super(Looper.getMainLooper());
        mRadioContext = radioContext;
        mPhoneId = radioContext.getPhoneId();
        mCi = ci;

        mCi.registerForIccIdInfo(this, EVENT_ICCID_INFO_UPDATED, null);
        mCi.registerForNrDualConnectivityState(this, EVENT_NR_DUAL_CONNECTIVITY_STATE_CHANGED,
                null);
        if (mRadioContext.getPhoneId() == 0) {
            mCi.registerForSimTrayStatus(this, EVENT_SIM_TRAY_STATE_CHANGED, null);
        }
        mCi.registerForDataNasTimerStatusChanged(this, EVENT_NAS_TIMER_STATUS_CHANGED, null);
        mCi.registerForUnthrottleApn(this, EVENT_UNTHROTTLE_APN, null);
    }

    @Override
    public void handleMessage(Message msg) {
        AsyncResult ar;

        switch (msg.what) {
        case EVENT_ICCID_INFO_UPDATED:
            logv("EVENT_ICCID_INFO_UPDATED");
            ar = (AsyncResult) msg.obj;
            if (ar.exception != null) {
                // error
                loge("error");
            } else {
                mIccid = (String) ar.result;
                logv("ICCID=" + mIccid);
                mRadioContext.notifyIccidInfo(mIccid);
            }
            break;

        case EVENT_NR_DUAL_CONNECTIVITY_STATE_CHANGED: {
            logv("EVENT_NR_DUAL_CONNECTIVITY_STATE_CHANGED");
            ar = (AsyncResult) msg.obj;
            if (ar.exception != null) {
                // error
                loge("error");
            } else {
                int[] state = (int[]) ar.result;
                if (state != null && state.length >= 2) {
                    boolean enabled = !(state[0] == 0);
                    int cause = state[1];
                    logv("notifyNrDualConnectivityState(" + enabled + ", " + cause + ")");
                    mRadioContext.notifyNrDualConnectivityState(enabled, cause);
                } else {
                    logv("invalid state " + state);
                }
            }
            break;
        }

        case EVENT_SIM_TRAY_STATE_CHANGED:
            logv("EVENT_SIM_TRAY_STATE_CHANGED");
            ar = (AsyncResult) msg.obj;
            if (ar.exception != null) {
                // error
                loge("error");
            } else {
                mSimTrayState = (int) ar.result;
                logv("mSimTrayState=" + mSimTrayState);
                mRadioContext.notifySimTrayState(mSimTrayState);
            }
            break;

        case EVENT_NAS_TIMER_STATUS_CHANGED:
            logv("EVENT_NAS_TIMER_STATUS_CHANGED");
            ar = (AsyncResult) msg.obj;
            if (ar.exception != null) {
                // error
                loge("error");
            } else {
                NasTimerStatus DataNasTimer = (NasTimerStatus)(ar.result);
                logv("NasTimerStatus ("+DataNasTimer+")");
                mRadioContext.notifyNasTimer(DataNasTimer.type, DataNasTimer.status, DataNasTimer.value, DataNasTimer.apn);
            }
            break;

        case EVENT_UNTHROTTLE_APN:
            logv("EVENT_UNTHROTTLE_APN");
            ar = (AsyncResult) msg.obj;
            if (ar.exception != null) {
                // error
                loge("error");
            } else {
                DataProfile dataProfile = (DataProfile)(ar.result);
                mRadioContext.notifyUnthrottleApn(dataProfile);
            }
            break;

        default:
            break;
        }  // end switch ~
    }

    public String getIccid() { return mIccid; }
    public int getSimTrayState() { return mSimTrayState; }

    protected final void log(String msg) {
        Rlog.d(LOG_TAG + mPhoneId, msg);
    }

    protected final void loge(String msg) {
        Rlog.e(LOG_TAG + mPhoneId, msg);
    }

    protected final void logv(String msg) {
        Rlog.v(LOG_TAG + mPhoneId, msg);
    }
}
