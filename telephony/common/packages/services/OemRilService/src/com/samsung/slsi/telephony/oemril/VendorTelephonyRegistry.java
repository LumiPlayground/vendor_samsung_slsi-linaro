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

import java.util.ArrayList;
import java.util.Arrays;
import java.util.NoSuchElementException;
import java.util.Set;
import java.util.stream.Collectors;

import com.android.internal.telephony.vendor.IVendorTelephonyListener;
import com.android.internal.telephony.vendor.VendorTelephonyCallback;
import com.android.telephony.Rlog;

import android.content.Context;
import android.os.Binder;
import android.os.IBinder;
import android.os.RemoteException;
import android.os.UserHandle;
import android.telephony.data.DataProfile;
import android.telephony.SubscriptionManager;

public class VendorTelephonyRegistry implements VendorTelephonyNotifier {
    private static final String TAG = "VendorTelephonyRegistry";
    private static final boolean DBG = true;
    private static final boolean VDBG = true;

    private static VendorTelephonyRegistry sIntance;

    private Context mContext;
    private int mNumPhones;
    private String[] mIccidInfo;
    private int mSimTrayState;
    private int mNasTimerType, mNasTimerStatus, mNasTimerValue;
    private String mNasTimerApn;
    private final ArrayList<IBinder> mRemoveList = new ArrayList<IBinder>();
    private final ArrayList<Record> mRecords = new ArrayList<Record>();


    private static class Record {
        Context context;
        IBinder binder;
        IVendorTelephonyListener callback;
        String callingPackage;
        int phoneId = SubscriptionManager.INVALID_PHONE_INDEX;
        Set<Integer> eventList;
        VendorTelephonyRegistryDeathRecipient deathRecipient;

        int callerUid;
        int callerPid;

        boolean matchEvent(int event) {
            return (callback != null) && (this.eventList.contains(event));
        }
    }

    private class VendorTelephonyRegistryDeathRecipient implements IBinder.DeathRecipient {
        private final IBinder binder;

        VendorTelephonyRegistryDeathRecipient(IBinder binder) {
            this.binder = binder;
        }

        @Override
        public void binderDied() {
            loge("binderDied: binder=" + binder);
            remove(binder);
        }
    }

    public static VendorTelephonyRegistry init(OemRilGlobals globals) {
        if (sIntance == null) {
            sIntance = new VendorTelephonyRegistry(globals);
        } else {

        }
        return sIntance;
    }

    public static VendorTelephonyRegistry getInstance() {
        return sIntance;
    }

    private VendorTelephonyRegistry(OemRilGlobals globals) {
        mContext = globals.getApplicationContext();
        int numPhones = globals.getActiveModemCount();
        mNumPhones = numPhones;
        mIccidInfo = new String[numPhones];
        for (int i = 0; i < numPhones; i++) {
            mIccidInfo[i] = "";
        }
        mSimTrayState = VendorRadioInterfaceManager.SIM_TRAY_STATE_UNKNOWN;
        mNasTimerType = 0;
        mNasTimerStatus = 0;
        mNasTimerValue = 0;
        mNasTimerApn = "";
    }

    public void listenWithEventList(int phoneId, String callingPackage,
            IVendorTelephonyListener callback, int[] events, boolean notifyNow) {
        Set<Integer> eventList = Arrays.stream(events).boxed().collect(Collectors.toSet());
        listen(callingPackage, callback, eventList, notifyNow, phoneId);
    }

    private void listen(String callingPackage, IVendorTelephonyListener callback,
            Set<Integer> events, boolean notifyNow, int phoneId) {
        int callerUserId = UserHandle.getCallingUserId();
        //mAppOps.checkPackage(Binder.getCallingUid(), callingPackage);
        if (events.isEmpty()) {
            if (DBG) {
                log("listen: Unregister");
            }
            events.clear();
            remove(callback.asBinder());
            return;
        }

        synchronized (mRecords) {
            // register
            IBinder b = callback.asBinder();
            Record r = add(b, Binder.getCallingUid(), Binder.getCallingPid());
            if (r == null) {
                return;
            }

            r.context = mContext;
            r.callback = callback;
            r.callingPackage = callingPackage;
            r.callerUid = Binder.getCallingUid();
            r.callerPid = Binder.getCallingPid();
            r.phoneId = phoneId;
            r.eventList = events;
            if (DBG) {
                log("listen:  Register r=" + r + " r.subId=" + r.phoneId + " phoneId=" + phoneId);
            }

            if (notifyNow && SubscriptionManager.isValidPhoneId(phoneId)) {
                if (events.contains(VendorTelephonyCallback.EVENT_ICCID_INFO_UPDATED)) {
                    try {
                        String iccid = mIccidInfo[phoneId];
                        r.callback.onIccidInfoUpdated(iccid);
                    } catch (RemoteException ex) {
                        remove(r.binder);
                    }
                }

                if (events.contains(VendorTelephonyCallback.EVENT_SIM_TRAY_STATE_CHANGED)) {
                    try {
                        r.callback.onSimTrayStatusChanged(mSimTrayState);
                    } catch (RemoteException ex) {
                        remove(r.binder);
                    }
                }

                if (events.contains(VendorTelephonyCallback.EVENT_NAS_TIMER_STATUS_CHANGED)) {
                    try {
                        r.callback.onNasTimerChanged(mNasTimerType, mNasTimerStatus, mNasTimerValue, mNasTimerApn);
                    } catch (RemoteException ex) {
                        remove(r.binder);
                    }
                }
            }
        }
    }

    private Record add(IBinder binder, int callingUid, int callingPid) {
        Record r;
        synchronized (mRecords) {
            final int N = mRecords.size();
            for (int i = 0; i < N; i++) {
                r = mRecords.get(i);
                if (binder == r.binder) {
                    return r;
                }
            }

            r = new Record();
            r.binder = binder;
            r.deathRecipient = new VendorTelephonyRegistryDeathRecipient(binder);

            try {
                binder.linkToDeath(r.deathRecipient, 0);
            } catch (RemoteException e) {
                if (VDBG) log("LinkToDeath remote exception sending to r=" + r + " e=" + e);
                // Binder already died. Return null.
                return null;
            }

            mRecords.add(r);
            if (DBG) log("add new record");
        }
        return r;
    }

    private void remove(IBinder binder) {
        synchronized (mRecords) {
            final int recordCount = mRecords.size();
            for (int i = 0; i < recordCount; i++) {
                Record r = mRecords.get(i);
                if (r.binder == binder) {
                    if (DBG) {
                        log("remove: binder=" + binder + " r.callingPackage " + r.callingPackage
                                + " r.callback " + r.callback);
                    }

                    if (r.deathRecipient != null) {
                        try {
                            binder.unlinkToDeath(r.deathRecipient, 0);
                        } catch (NoSuchElementException e) {
                            if (VDBG) log("UnlinkToDeath NoSuchElementException sending to r="
                                    + r + " e=" + e);
                        }
                    }

                    mRecords.remove(i);
                    return;
                }
            }
        }
    }

    private void handleRemoveListLocked() {
        int size = mRemoveList.size();
        if (VDBG) log("handleRemoveListLocked: mRemoveList.size()=" + size);
        if (size > 0) {
            for (IBinder b : mRemoveList) {
                remove(b);
            }
            mRemoveList.clear();
        }
    }

    @Override
    public void notifyIccidInfo(int phoneId, String iccid) {
        synchronized (mRecords) {
            if (SubscriptionManager.isValidPhoneId(phoneId)) {
                mIccidInfo[phoneId] = iccid;
                for (Record r : mRecords) {
                    if (r.matchEvent(VendorTelephonyCallback.EVENT_ICCID_INFO_UPDATED)
                        && r.phoneId == phoneId) {
                        try {
                            r.callback.onIccidInfoUpdated(iccid);
                        } catch (RemoteException ex) {
                            mRemoveList.add(r.binder);
                        }
                    }
                }
            }
            handleRemoveListLocked();
        }
    }

    @Override
    public void notifyNrDualConnectivityState(int phoneId, boolean enabled, int cause) {
        synchronized (mRecords) {
            if (SubscriptionManager.isValidPhoneId(phoneId)) {
                for (Record r : mRecords) {
                    if (r.matchEvent(VendorTelephonyCallback.EVENT_NR_DUAL_CONNECTIVITY_STATE_CHANGED)
                        && r.phoneId == phoneId) {
                        try {
                            r.callback.onNrDualConnectivityStateChanged(enabled, cause);
                        } catch (RemoteException ex) {
                            mRemoveList.add(r.binder);
                        }
                    }
                }
            }
            handleRemoveListLocked();
        }
    }

    @Override
    public void notifySimTrayState(int phoneId, int state) {
        // a single SIM tray
        synchronized (mRecords) {
            if (SubscriptionManager.isValidPhoneId(phoneId)) {
                mSimTrayState = state;
                for (Record r : mRecords) {
                    if (r.matchEvent(VendorTelephonyCallback.EVENT_SIM_TRAY_STATE_CHANGED)) {
                        try {
                            r.callback.onSimTrayStatusChanged(state);
                        } catch (RemoteException ex) {
                            mRemoveList.add(r.binder);
                        }
                    }
                }
            }
            handleRemoveListLocked();
        }
    }

    @Override
    public void notifyNasTimer(int phoneId, int type, int status, int value, String apn) {
        synchronized (mRecords) {
            if (SubscriptionManager.isValidPhoneId(phoneId)) {
                mNasTimerType = type;
                mNasTimerStatus = status;
                mNasTimerValue = value;
                mNasTimerApn = apn;
                for (Record r : mRecords) {
                    if (r.matchEvent(VendorTelephonyCallback.EVENT_NAS_TIMER_STATUS_CHANGED)) {
                        try {
                            r.callback.onNasTimerChanged(type, status, value, apn);
                        } catch (RemoteException ex) {
                            mRemoveList.add(r.binder);
                        }
                    }
                }
            }
            handleRemoveListLocked();
        }
    }

    @Override
    public void notifyUnthrottleApn(int phoneId, DataProfile dataProfile) {
        synchronized (mRecords) {
            if (SubscriptionManager.isValidPhoneId(phoneId)) {
                for (Record r : mRecords) {
                    if (r.matchEvent(VendorTelephonyCallback.EVENT_UNTHROTTLE_APN)) {
                        try {
                            r.callback.onUnthrottleApn(dataProfile);
                        } catch (RemoteException ex) {
                            mRemoveList.add(r.binder);
                        }
                    }
                }
            }
            handleRemoveListLocked();
        }
    }

    private static void log(String s) {
        Rlog.d(TAG, s);
    }

    private static void loge(String s) {
        Rlog.e(TAG, s);
    }
}
