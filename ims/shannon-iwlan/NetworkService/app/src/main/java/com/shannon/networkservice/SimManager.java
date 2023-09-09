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
package com.shannon.networkservice;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.ConnectivityManager;
import android.net.Network;
import android.net.NetworkCapabilities;
import android.net.NetworkRequest;
import android.telephony.AccessNetworkConstants;
import android.telephony.NetworkRegistrationInfo;
import android.telephony.Rlog;
import android.telephony.ServiceState;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
import android.util.SparseArray;

import com.android.internal.annotations.VisibleForTesting;
import com.android.internal.telephony.PhoneConstants;
import com.android.internal.telephony.TelephonyIntents;

class SimManager {
    private static final boolean DBG = true;
    private static SimManager sMe = null;

    public static SimManager getInstance(Context context) {
        if (sMe == null) {
            sMe = new SimManager(context);
        }
        return sMe;
    }

    @VisibleForTesting
    class MobileNetworkCallback extends ConnectivityManager.NetworkCallback {
        private final int mSlotId;
        private final int mSubId;
        private final TelephonyManager mTelephonyManager;
        private boolean mConnected;

        MobileNetworkCallback(int slotId, int subId) {
            mSlotId = slotId;
            mSubId = subId;
            mConnected = false;

            TelephonyManager tm = mContext.getSystemService(TelephonyManager.class);
            mTelephonyManager = tm.createForSubscriptionId(mSubId);
        }

        @Override
        public void onAvailable(Network network) {
            mConnected = true;
            log("Mobile network " + network + " connected for slot = " + mSlotId);
            ShannonNetworkService.onConnectivityChanged(mSlotId);
        }

        @Override
        public void onLost(Network network) {
            mConnected = false;
            log("Mobile network " + network + " disconnected for slot = " + mSlotId);
            ShannonNetworkService.onConnectivityChanged(mSlotId);
        }

        public NetworkRequest buildRequest() {
            return new NetworkRequest.Builder()
                    .addCapability(NetworkCapabilities.NET_CAPABILITY_INTERNET)
                    .addTransportType(NetworkCapabilities.TRANSPORT_CELLULAR)
                    .setNetworkSpecifier(mTelephonyManager.getNetworkSpecifier())
                    .build();
        }

        public boolean isConnected() { return mConnected; }
        public boolean isRoaming() { return mTelephonyManager.isNetworkRoaming(); }

        public int getNetworkType() {
            try {
                ServiceState ss = mTelephonyManager.getServiceState();
                if (ss != null) {
                    NetworkRegistrationInfo nri = ss.getNetworkRegistrationInfo(
                            NetworkRegistrationInfo.DOMAIN_PS,
                            AccessNetworkConstants.TRANSPORT_TYPE_WWAN);
                    if (nri != null) {
                        return nri.getAccessNetworkTechnology();
                    }
                }
            } catch (Exception e) {
                // nothing
            }
            return TelephonyManager.NETWORK_TYPE_UNKNOWN;
        }

        @Override
        public String toString() {
            return ("Mobile callback for " + mSlotId + " (subId=" + mSubId + ") connected: " + mConnected
                    + " roaming: " + isRoaming() + " on " + TelephonyManager.getNetworkTypeName(getNetworkType()));
        }
    }

    @VisibleForTesting
    class SimStateBroadcastReceiver extends BroadcastReceiver {
        @Override
        public void onReceive(Context context, Intent intent) {
            final String action = intent.getAction();
            if (action == null) {
                loge("NULL action for intent " + intent);
                return;
            }
            switch (action) {
                case TelephonyIntents.ACTION_SIM_STATE_CHANGED:
                    int slotId = intent.getIntExtra(PhoneConstants.PHONE_KEY, -1);
                    if (slotId < 0) {
                        return;
                    }
                    log("Received " + intent + " for slot " + slotId);

                    int state = SubscriptionManager.getSimStateForSlotIndex(slotId);
                    switch (state) {
                        case TelephonyManager.SIM_STATE_READY:
                        case TelephonyManager.SIM_STATE_LOADED:
                        case TelephonyManager.SIM_STATE_PRESENT: {
                            MobileNetworkCallback callback = mNetworkCallbacks.get(slotId);
                            if (callback != null) {
                                log("Callback already created for slot " + slotId);
                                return;
                            }
                            int[] subIds = mSubscriptionManager.getSubscriptionIds(slotId);
                            int subId = (subIds != null && subIds.length > 0) ? subIds[0] : -1;
                            if (subId < 0) {
                                return;
                            }
                            log("SIM inserted for slot " + slotId + " subId = " + subId);

                            callback = new MobileNetworkCallback(slotId, subId);
                            mNetworkCallbacks.put(slotId, callback);
                            mConnectivityManager.registerNetworkCallback(callback.buildRequest(), callback);
                            break;
                        }
                        default: {
                            MobileNetworkCallback callback = mNetworkCallbacks.get(slotId);
                            if (callback != null) {
                                mConnectivityManager.unregisterNetworkCallback(callback);
                                mNetworkCallbacks.remove(slotId);

                                log("SIM removed for slot " + slotId);
                            }
                        }
                    }
                    break;
                default:;
            }
        }
    }

    private static final String TAG = ShannonNetworkService.getTag() + "-SIM";
    private final Context mContext;
    private final SimStateBroadcastReceiver mReceiver;
    private final ConnectivityManager mConnectivityManager;
    private final SubscriptionManager mSubscriptionManager;
    private final SparseArray<MobileNetworkCallback> mNetworkCallbacks;

    @VisibleForTesting
    SimManager(Context context) {
        mContext = context;
        mNetworkCallbacks = new SparseArray<>();
        mConnectivityManager = mContext.getSystemService(ConnectivityManager.class);
        mSubscriptionManager = mContext.getSystemService(SubscriptionManager.class);

        TelephonyManager tm = mContext.getSystemService(TelephonyManager.class);
        for (int slot = 0; slot < tm.getPhoneCount(); slot++) {
            int state = SubscriptionManager.getSimStateForSlotIndex(slot);
            if (isSimPresent(state)) {
                int[] subIds = mSubscriptionManager.getSubscriptionIds(slot);
                int subId = (subIds != null && subIds.length > 0) ? subIds[0] : -1;
                if (subId < 0) {
                    continue;
                }
                log("SIM available for slot " + slot + " subId = " + subId);
                MobileNetworkCallback callback = new MobileNetworkCallback(slot, subId);
                mNetworkCallbacks.put(slot, callback);
                mConnectivityManager.registerNetworkCallback(callback.buildRequest(), callback);
            }
        }

        mReceiver = new SimStateBroadcastReceiver();
        IntentFilter filter = new IntentFilter();
        filter.addAction(TelephonyIntents.ACTION_SIM_STATE_CHANGED);
        mContext.registerReceiver(mReceiver, filter);
    }

    public void destroy() {
        mContext.unregisterReceiver(mReceiver);

        for (int i = 0; i < mNetworkCallbacks.size(); i++) {
            MobileNetworkCallback callback = mNetworkCallbacks.valueAt(i);
            if (callback != null) {
                mConnectivityManager.unregisterNetworkCallback(callback);
            }
        }
        mNetworkCallbacks.clear();

        sMe = null;
    }

    public boolean isVoMobilePossible(int notUsedSlotId, ConfigurationManager config) {
        for (int i = 0; i < mNetworkCallbacks.size(); i++) {
            int slotId = mNetworkCallbacks.keyAt(i);
            if (slotId != notUsedSlotId) {
                MobileNetworkCallback callback = mNetworkCallbacks.valueAt(i);
                log("Found. " + callback);
                if ((callback != null) && callback.isConnected() &&
                     config.isVoMobileSupportedOnNetwork(callback.getNetworkType()) &&
                    (config.isVoMobileEnabledOnRoaming() || !callback.isRoaming())) {
                    return true;
                }
            }
        }
        return false;
    }

    private boolean isSimPresent(int state) {
        switch (state) {
            case TelephonyManager.SIM_STATE_READY:
            case TelephonyManager.SIM_STATE_LOADED:
            case TelephonyManager.SIM_STATE_PRESENT:
                return true;
            default:;
        }
        return false;
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
    final SimStateBroadcastReceiver getSimStateBroadcastReceiver() {
        return mReceiver;
    }

    @VisibleForTesting
    protected SimManager getMe() { return sMe; }
}
