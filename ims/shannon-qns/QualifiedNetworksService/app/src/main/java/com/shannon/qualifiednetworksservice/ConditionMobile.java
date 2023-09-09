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
package com.shannon.qualifiednetworksservice;

import android.content.Context;
import android.os.PersistableBundle;
import android.telephony.CarrierConfigManager;
import android.telephony.TelephonyManager;
import android.text.TextUtils;
import android.util.SparseArray;

import com.android.internal.annotations.VisibleForTesting;
import com.shannon.qualifiednetworksservice.HandoverConditions.ConditionEvents;
import com.shannon.qualifiednetworksservice.util.LogUtils;

public class ConditionMobile {
    private static final int COUNTRY_CODE_LENGTH = 3;

    protected class MobileCallbackEvent implements MobileNetworkCallback.IMobileCallbackEvent {
        public void onCallbackAdded(int slotId, MobileNetworkCallback callback) {
            if (slotId != mSlotId) {
                callback.registerForNetworkCallbackEvent(mMobileCallbackEvents);
                mMobileCallbacks.put(slotId, callback);
            }
        }

        public void onCallbackRemoved(int slotId) {
            if (slotId != mSlotId) {
                mMobileCallbacks.remove(slotId);
            }
        }

        public void onCallbackConditionsChanged(ConditionEvents event) {
            mChangedCallback.onConditionChanged(event);
        }
    }

    private final String TAG;
    private final int mSlotId;
    private final CarrierConfigManager mCarrierConfigManager;
    private final HandoverConditions.IConditionChanged mChangedCallback;
    private final ConfigurationManager mConfigurationManager;
    private final SimManager mSimManager;
    private final MobileCallbackEvent mMobileCallbackEvents;
    private final SparseArray<MobileNetworkCallback> mMobileCallbacks;

    public ConditionMobile(int slotId, Context context, ConfigurationManager configurationManager,
                           HandoverConditions.IConditionChanged callback) {
        mSlotId = slotId;
        mCarrierConfigManager = context.getSystemService(CarrierConfigManager.class);
        mConfigurationManager = configurationManager;
        mChangedCallback = callback;
        mSimManager = SimManager.getInstance();
        mMobileCallbackEvents = new MobileCallbackEvent();
        mMobileCallbacks = new SparseArray<>();

        TAG = ShannonQualifiedNetworksService.getServiceTAG("-" + mSlotId + "-HC-mobile");

        SparseArray<MobileNetworkCallback> array = mSimManager.registerForNetworkCallbackEvent(mMobileCallbackEvents);
        for (int i = 0; i < array.size(); i++) {
            int s = array.keyAt(i);
            /* copy all except current slot */
            if (s != mSlotId) {
                MobileNetworkCallback c = array.valueAt(i);
                c.registerForNetworkCallbackEvent(mMobileCallbackEvents);
                mMobileCallbacks.put(s, c);
            }
        }
    }

    public void destroy() {
        mSimManager.unregisterNetworkCallbackEvent(mMobileCallbackEvents);
        for (int i = 0; i < mMobileCallbacks.size(); i++) {
            MobileNetworkCallback callback = mMobileCallbacks.valueAt(i);
            callback.unregisterNetworkCallbackEvent(mMobileCallbackEvents);
        }
        mMobileCallbacks.clear();
    }

    public boolean isVoMobilePossible(boolean isRegistered, boolean isRoaming, boolean isVoMobileUsed) {
        for (int i = 0; i < mMobileCallbacks.size(); i++) {
            MobileNetworkCallback callback = mMobileCallbacks.valueAt(i);
            if (callback == null) {
                continue;
            }
            boolean connected = callback.isConnected();
            boolean supported = isVoMobileSupportedOnNetwork(callback.getNetworkType());
            boolean allowedOnRoamingNetwork = (mConfigurationManager.isVoMobileEnabledOnRoamingNetwork() || !callback.isRoaming());
            boolean forbiddenInRoaming = (isRoaming && isForbiddenNetworkInRoaming(callback.getNetworkOperator()));
            boolean signalThreshold;
            if (isVoMobileUsed) {
                signalThreshold = callback.isSignalBetterThanThreshold(mConfigurationManager.getVoMobileOtherSignalOut());
            } else {
                signalThreshold = callback.isSignalBetterThanThreshold(mConfigurationManager.getVoMobileOtherSignalIn());
            }
            LogUtils.d(TAG, LogUtils.LOG_CONDITIONS, "Mobile callback" + callback.getSlotId() +
                    " connected=" + connected + " RAT supported=" + supported +
                    " on roaming allowed=" + allowedOnRoamingNetwork + " forbidden=" + forbiddenInRoaming +
                    " threshold=" + (signalThreshold ? "ok" : "bad"));
            if (connected && supported && allowedOnRoamingNetwork && !forbiddenInRoaming && (!isRegistered || signalThreshold)) {
                return true;
            }
        }
        return false;
    }

    private boolean isOpportunisticNetworkPreferred(int subId) {
        PersistableBundle bundle = mCarrierConfigManager.getConfigForSubId(subId);
        if ((bundle != null) && !bundle.isEmpty()) {
            return CarrierConfigManager.isConfigForIdentifiedCarrier(bundle) &&
                    bundle.getBoolean(CarrierConfigManager.KEY_ENABLE_CROSS_SIM_CALLING_ON_OPPORTUNISTIC_DATA_BOOL);
        }
        return false;
    }

    public boolean isOpportunisticNetworkConnectedAndPreferred(boolean isInService) {
        for (int i = 0; i < mMobileCallbacks.size(); i++) {
            MobileNetworkCallback callback = mMobileCallbacks.valueAt(i);
            if (callback == null) {
                continue;
            }
            boolean preferred = isOpportunisticNetworkPreferred(mSimManager.getSubId(mSlotId));
            boolean connected = callback.isConnected();
            boolean opportunistic = mSimManager.isOpportunisticSubscription(callback.getSlotId(), callback.getSubId());
            LogUtils.d(TAG, LogUtils.LOG_CONDITIONS, "Mobile callback" + callback.getSlotId() +
                    " connected=" + connected + " opportunistic=" + opportunistic + " preferred=" + preferred +
                    " in_service=" + isInService);
            if (isInService && preferred && connected && opportunistic) {
                return true;
            }
        }
        return false;
    }

    private boolean isForbiddenNetworkInRoaming(String plmn) {
        String forbiddenList = mConfigurationManager.getVoMobileForbiddenNetworksInRoaming();
        /* default empty - all allowed */
        if (TextUtils.isEmpty(forbiddenList)) {
            return false;
        }

        /* Allow CST by default even invalid plmn */
        if (TextUtils.isEmpty(plmn)) {
            return false;
        }

        String[] list = forbiddenList.split(",");
        for (String s : list) {
            s = s.trim();
            int len = s.length();
            if (len < COUNTRY_CODE_LENGTH) {
                continue;
            } else if (len == COUNTRY_CODE_LENGTH) {
                /* MCC only */
                if (plmn.startsWith(s)) {
                    return true;
                }
            } else { // len > COUNTRY_CODE_LENGTH
                /* whole PLMN */
                if (plmn.equalsIgnoreCase(s)) {
                    return true;
                }
            }
        }
        return false;
    }

    private boolean isVoMobileSupportedOnNetwork(int networkType) {
        String ratList = mConfigurationManager.getVoMobileAllowedRatList();
        if (!TextUtils.isEmpty(ratList)) {
            String current = TelephonyManager.getNetworkTypeName(networkType);

            String[] tokens = ratList.split(",");
            for (String s : tokens) {
                if (current.equals(s.trim())) {
                    return true;
                }
            }
        }
        return false;
    }

    @VisibleForTesting
    MobileCallbackEvent getMobileCallbackEvents() { return mMobileCallbackEvents; }
}
