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

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.ConnectivityManager;
import android.os.Message;
import android.os.PersistableBundle;
import android.telephony.CarrierConfigManager;
import android.telephony.SubscriptionInfo;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
import android.util.SparseArray;

import com.android.internal.annotations.VisibleForTesting;
import com.android.internal.telephony.PhoneConstants;
import com.android.internal.telephony.TelephonyIntents;
import com.shannon.qualifiednetworksservice.util.LogUtils;

import java.util.LinkedList;

public class SimManager {
    private static SimManager sMe = null;
    public static SimManager getInstance() { return sMe; }

    protected class SimBroadcastReceiver extends BroadcastReceiver {
        @Override
        public void onReceive(Context context, Intent intent) {
            final String action = intent.getAction();
            if (action == null) {
                LogUtils.w(TAG, "NULL action for intent " + intent);
                return;
            }
            switch (action) {
                case CarrierConfigManager.ACTION_CARRIER_CONFIG_CHANGED: {
                    int slotId = intent.getIntExtra(CarrierConfigManager.EXTRA_SLOT_INDEX, -1);
                    if (slotId < 0) {
                        return;
                    }
                    if (!isSimLoaded(slotId)) {
                        LogUtils.w(TAG, "SIM " + slotId + " got spurious CARRIER_CONFIG_CHANGED");
                        return;
                    }
                    SimInformation simInformation = mSimInformationArray.get(slotId);
                    if ((simInformation != null) && isCarrierConfigLoaded(simInformation.getSubId())) {
                        LogUtils.d(TAG, "SIM " + slotId + " carrier config loaded, notify");

                        Message message = mNotificationsMap.get(slotId);
                        if (message != null) {
                            Message clonedMessage = Message.obtain(message);
                            clonedMessage.obj = simInformation;
                            clonedMessage.sendToTarget();
                        }
                    }
                    break;
                }
                case TelephonyIntents.ACTION_SIM_STATE_CHANGED:
                    int slotId = intent.getIntExtra(PhoneConstants.PHONE_KEY, -1);
                    if (slotId < 0) {
                        return;
                    }
                    /* check SIM state */
                    int state = SubscriptionManager.getSimStateForSlotIndex(slotId);
                    if (isSimPresent(state)) {
                        int subId = intent.getIntExtra(PhoneConstants.SUBSCRIPTION_KEY, -1);
                        if (subId < 0) {
                            LogUtils.d(TAG, "Invalid subId received " + subId + " for slot " + slotId);
                            return;
                        }
                        SimInformation simInformation = mSimInformationArray.get(slotId);
                        if (simInformation != null) {
                            if (simInformation.getSubId() != subId) {
                                LogUtils.i(TAG, "SIM subId changed " + simInformation.getSubId() + "->" + subId +
                                        " for slot " + slotId);
                                deleteSimInformationForSlot(slotId);
                            } else {
                                LogUtils.d(TAG, "SIM information already available for slot " + slotId);
                                return;
                            }
                        }
                        simInformation = SimInformation.createSimInformation(slotId, mSubscriptionManager, mTelephonyManager);
                        if (simInformation != null) {
                            mSimInformationArray.put(slotId, simInformation);
                            LogUtils.i(TAG, "SIM loaded. Created " + simInformation);

                            MobileNetworkCallback callback = new MobileNetworkCallback(slotId, simInformation);
                            mNetworkCallbacks.put(slotId, callback);
                            mConnectivityManager.registerNetworkCallback(callback.buildRequest(), callback);
                            notifyForNetworkCallbackAdded(slotId, callback);
                        }
                    } else {
                        if (mSimInformationArray.contains(slotId)) {
                            deleteSimInformationForSlot(slotId);
                        }
                    }
                    break;
                default:;
            }
        }
    }

    private final String TAG;
    private final Context mContext;
    private final TelephonyManager mTelephonyManager;
    private final SubscriptionManager mSubscriptionManager;
    private final ConnectivityManager mConnectivityManager;
    private final SparseArray<SimInformation> mSimInformationArray;
    private final SparseArray<MobileNetworkCallback> mNetworkCallbacks;
    private final SparseArray<Message> mNotificationsMap;
    private final LinkedList<MobileNetworkCallback.IMobileCallbackEvent> mNetworkCallbackEvents;
    private final SimBroadcastReceiver mReceiver;

    public SimManager(Context context) {
        mContext = context;
        mSimInformationArray = new SparseArray<>();
        mNetworkCallbacks = new SparseArray<>();
        mNotificationsMap = new SparseArray<>();
        mNetworkCallbackEvents = new LinkedList<>();

        TAG = ShannonQualifiedNetworksService.getServiceTAG("-SIM");

        mReceiver = new SimBroadcastReceiver();
        mTelephonyManager = mContext.getSystemService(TelephonyManager.class);
        mSubscriptionManager = mContext.getSystemService(SubscriptionManager.class);
        mConnectivityManager = mContext.getSystemService(ConnectivityManager.class);

        sMe = this;

        for(int slot = 0; slot < mTelephonyManager.getPhoneCount(); slot++) {
            int state = SubscriptionManager.getSimStateForSlotIndex(slot);
            if (isSimPresent(state)) {
                SimInformation simInformation = SimInformation.createSimInformation(slot, mSubscriptionManager, mTelephonyManager);
                if (simInformation != null) {
                    mSimInformationArray.put(slot, simInformation);
                    LogUtils.i(TAG, "SIM available. Created " + simInformation);

                    MobileNetworkCallback callback = new MobileNetworkCallback(slot, simInformation);
                    mNetworkCallbacks.put(slot, callback);
                    mConnectivityManager.registerNetworkCallback(callback.buildRequest(), callback);
                }
            }
        }

        IntentFilter filter = new IntentFilter();
        filter.addAction(CarrierConfigManager.ACTION_CARRIER_CONFIG_CHANGED);
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

    private boolean isSimLoaded(int slotId) {
        return (SubscriptionManager.getSimStateForSlotIndex(slotId) == TelephonyManager.SIM_STATE_LOADED);
    }

    public int getMcc(int slotId) {
        SimInformation info = mSimInformationArray.get(slotId);
        if (info != null) {
            return info.getMcc();
        }
        return -1;
    }

    public int getMnc(int slotId) {
        SimInformation info = mSimInformationArray.get(slotId);
        if (info != null) {
            return info.getMnc();
        }
        return -1;
    }

    public int getSubId(int slotId) {
        SimInformation info = mSimInformationArray.get(slotId);
        if (info != null) {
            return info.getSubId();
        }
        return -1;
    }

    public TelephonyManager getTelephonyManager(int slotId) {
        SimInformation info = mSimInformationArray.get(slotId);
        if (info != null) {
            return info.getTelephonyManager();
        }
        return null;
    }

    public void registerForSimEvent(int slotId, Message message) {
        mNotificationsMap.put(slotId, message);

        SimInformation info = mSimInformationArray.get(slotId);
        if ((info != null) && isCarrierConfigLoaded(info.getSubId())) {
            LogUtils.d(TAG, "SIM information already available, send message for " + slotId);
            Message clonedMessage = Message.obtain(message);
            clonedMessage.obj = info;
            clonedMessage.sendToTarget();
        }
    }

    public SparseArray<MobileNetworkCallback> registerForNetworkCallbackEvent(MobileNetworkCallback.IMobileCallbackEvent callback) {
        mNetworkCallbackEvents.add(callback);
        return mNetworkCallbacks;
    }

    public void unregisterNetworkCallbackEvent(MobileNetworkCallback.IMobileCallbackEvent callback) {
        mNetworkCallbackEvents.remove(callback);
    }

    private void notifyForNetworkCallbackAdded(int slotId, MobileNetworkCallback callback) {
        for (MobileNetworkCallback.IMobileCallbackEvent event : mNetworkCallbackEvents) {
            event.onCallbackAdded(slotId, callback);
        }
    }

    private void notifyForNetworkCallbackRemoved(int slotId) {
        for (MobileNetworkCallback.IMobileCallbackEvent event : mNetworkCallbackEvents) {
            event.onCallbackRemoved(slotId);
        }
    }

    private boolean isCarrierConfigLoaded(int subId) {
        CarrierConfigManager ccm = mContext.getSystemService(CarrierConfigManager.class);
        if (ccm != null) {
            PersistableBundle bundle = ccm.getConfigForSubId(subId);
            if (bundle != null) {
                return CarrierConfigManager.isConfigForIdentifiedCarrier(bundle);
            }
        }
        return false;
    }

    private void deleteSimInformationForSlot(int slotId) {
        mSimInformationArray.remove(slotId);
        LogUtils.i(TAG, "SIM information cleared for slot " + slotId);

        MobileNetworkCallback callback = mNetworkCallbacks.get(slotId);
        if (callback != null) {
            callback.destroy();
            mNetworkCallbacks.remove(slotId);
            mConnectivityManager.unregisterNetworkCallback(callback);
            notifyForNetworkCallbackRemoved(slotId);
        }

        Message message = mNotificationsMap.get(slotId);
        if (message != null) {
            Message clonedMessage = Message.obtain(message);
            clonedMessage.sendToTarget();
        }
    }

    public boolean isOpportunisticSubscription(int slotId, int subId) {
        try {
            int preferredSubId = mTelephonyManager.getPreferredOpportunisticDataSubscription();
            if ((preferredSubId == SubscriptionManager.DEFAULT_SUBSCRIPTION_ID) || (preferredSubId != subId)) {
                return false;
            }
            SubscriptionInfo info = mSubscriptionManager.getActiveSubscriptionInfoForSimSlotIndex(slotId);
            if (info != null) {
                return info.isOpportunistic();
            }
        } catch (Exception e) {
            LogUtils.e(TAG, "Failed to check opportunistic subscription. " + e);
        }
        return false;
    }

    @VisibleForTesting
    SimBroadcastReceiver getReceiver() { return mReceiver; }
}
