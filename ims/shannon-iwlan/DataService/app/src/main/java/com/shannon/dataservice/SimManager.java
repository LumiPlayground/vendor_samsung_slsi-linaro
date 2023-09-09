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

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.ConnectivityManager;
import android.os.Handler;
import android.os.HandlerThread;
import android.telephony.Rlog;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;

import com.android.internal.annotations.VisibleForTesting;
import com.android.internal.telephony.PhoneConstants;
import com.android.internal.telephony.TelephonyIntents;

import java.util.concurrent.ConcurrentHashMap;

public class SimManager {
    private static final boolean DBG = true;

    private static SimManager sMe = null;
    public static SimManager getInstance() { return sMe; }

    protected class CreateSimInformationJob implements Runnable {
        private final int mSlotId;

        public CreateSimInformationJob(int slotId) {
            mSlotId = slotId;
        }

        @Override
        public void run() {
            SimInformation simInformation = mSimInformationArray.get(mSlotId);
            if (simInformation != null) {
                log("SIM information already available for slot " + mSlotId);
                return;
            }
            simInformation = SimInformation.createSimInformation(mSlotId, mSubscriptionManager, mTelephonyManager);
            if (simInformation == null) {
                log("SIM information not ready for slot " + mSlotId);
                return;
            }
            mSimInformationArray.put(mSlotId, simInformation);
            log("SIM loaded. Created " + simInformation);

            MobileNetworkCallback callback = new MobileNetworkCallback(mSlotId, simInformation.getTelephonyManager());
            mNetworkCallbacks.put(mSlotId, callback);
            mConnectivityManager.registerNetworkCallback(callback.buildRequest(), callback);
        }
    }

    protected class SimStateBroadcastReceiver extends BroadcastReceiver {
        @Override
        public void onReceive(Context context, Intent intent) {
            final String action = intent.getAction();
            if (action == null) {
                loge("NULL action for intent " + intent);
                return;
            }
            log("Got intent " + intent);

            switch (action) {
                case TelephonyIntents.ACTION_SIM_STATE_CHANGED:
                    int slotId = intent.getIntExtra(PhoneConstants.PHONE_KEY, -1);
                    if (slotId < 0) {
                        return;
                    }
                    /* check SIM state */
                    int state = SubscriptionManager.getSimStateForSlotIndex(slotId);
                    switch (state) {
                        case TelephonyManager.SIM_STATE_READY:
                        case TelephonyManager.SIM_STATE_PRESENT:
                            break;
                        case TelephonyManager.SIM_STATE_LOADED: {
                            /* create may take time, use separated thread */
                            log("Request to add SIM information for slot " + slotId);
                            mHandler.post(new CreateSimInformationJob(slotId));
                            break;
                        }
                        default: {
                            SimInformation simInformation = mSimInformationArray.get(slotId);
                            if (simInformation != null) {
                                mSimInformationArray.remove(slotId);
                                log("SIM information cleared for slot " + slotId);

                                MobileNetworkCallback callback = mNetworkCallbacks.get(slotId);
                                if (callback != null) {
                                    mNetworkCallbacks.remove(slotId);
                                    mConnectivityManager.unregisterNetworkCallback(callback);
                                }
                            }
                            break;
                        }
                    }
                    break;
                default:;
            }
        }
    }

    private final String TAG;
    private final SimStateBroadcastReceiver mReceiver;
    private final Context mContext;
    private final SubscriptionManager mSubscriptionManager;
    private final TelephonyManager mTelephonyManager;
    private final ConnectivityManager mConnectivityManager;
    private final ConcurrentHashMap<Integer, SimInformation> mSimInformationArray;
    private final ConcurrentHashMap<Integer, MobileNetworkCallback> mNetworkCallbacks;
    private final HandlerThread mHandlerThread;
    private final Handler mHandler;

    public SimManager(Context context) {
        mContext = context;

        TAG = ShannonDataService.getServiceTAG("-SIM");

        mReceiver = new SimStateBroadcastReceiver();
        mSimInformationArray = new ConcurrentHashMap<>();
        mNetworkCallbacks = new ConcurrentHashMap<>();
        mSubscriptionManager = mContext.getSystemService(SubscriptionManager.class);
        mTelephonyManager = mContext.getSystemService(TelephonyManager.class);
        mConnectivityManager = mContext.getSystemService(ConnectivityManager.class);

        mHandlerThread = new HandlerThread(SimManager.class.getSimpleName());
        mHandlerThread.start();
        mHandler = new Handler(mHandlerThread.getLooper());

        sMe = this;

        for (int slot = 0; slot < mTelephonyManager.getPhoneCount(); slot++) {
            int state = SubscriptionManager.getSimStateForSlotIndex(slot);
            if (state == TelephonyManager.SIM_STATE_LOADED) {
                /* create may take time, use separated thread */
                log("SIM available for slot " + slot);
                mHandler.post(new CreateSimInformationJob(slot));
            }
        }

        IntentFilter filter = new IntentFilter();
        filter.addAction(TelephonyIntents.ACTION_SIM_STATE_CHANGED);
        mContext.registerReceiver(mReceiver, filter);
    }

    public void destroy() {
        mContext.unregisterReceiver(mReceiver);
        mHandlerThread.quit();

        for (MobileNetworkCallback callback : mNetworkCallbacks.values()) {
            mConnectivityManager.unregisterNetworkCallback(callback);
        }
        mNetworkCallbacks.clear();
    }

    public SimInformation getSimInformation(int slotId) {
        return mSimInformationArray.get(slotId);
    }

    public boolean isSimInformationAvailable(int slotId) {
        return (mSimInformationArray.get(slotId) != null);
    }

    public MobileNetworkCallback getMobileNetwork(int notUsedSlotId, ConfigurationManager config) {
        if (config.isVoMobileEnabled()) {
            for (int i : mNetworkCallbacks.keySet()) {
                if (i != notUsedSlotId) {
                    MobileNetworkCallback callback = mNetworkCallbacks.get(i);
                    if ((callback != null) && callback.isConnected() &&
                        config.isVoMobileSupportedOnNetwork(callback.getNetworkType()) &&
                        (config.isVoMobileEnabledOnRoamingNetwork() || !callback.isRoaming())) {
                        return callback;
                    }
                }
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
}
