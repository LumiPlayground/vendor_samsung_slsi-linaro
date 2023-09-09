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

import android.annotation.NonNull;
import android.net.ConnectivityManager;
import android.net.Network;
import android.net.NetworkCapabilities;
import android.net.NetworkRequest;
import android.telephony.AccessNetworkConstants;
import android.telephony.CellSignalStrength;
import android.telephony.CellSignalStrengthLte;
import android.telephony.NetworkRegistrationInfo;
import android.telephony.PhoneStateListener;
import android.telephony.ServiceState;
import android.telephony.SignalStrength;
import android.telephony.TelephonyManager;

import com.android.internal.annotations.VisibleForTesting;
import java.util.LinkedList;
import java.util.List;

import static android.telephony.PhoneStateListener.LISTEN_NONE;
import static android.telephony.PhoneStateListener.LISTEN_SIGNAL_STRENGTHS;

import com.shannon.qualifiednetworksservice.HandoverConditions.ConditionEvents;
import com.shannon.qualifiednetworksservice.util.LogUtils;

import static com.shannon.qualifiednetworksservice.HandoverConditions.ConditionEvents.*;

class MobileNetworkCallback extends ConnectivityManager.NetworkCallback {
    public interface IMobileCallbackEvent {
        public void onCallbackAdded(int slotId, MobileNetworkCallback callback);
        public void onCallbackRemoved(int slotId);
        public void onCallbackConditionsChanged(ConditionEvents events);
    }

    private final String TAG;
    private final int mSlotId;
    private final int mSubscriptionId;
    private final TelephonyManager mTelephonyManager;
    private final MobileSignalStrengthListener mSignalStrengthListener;
    private final LinkedList<IMobileCallbackEvent> mCallbackEvents;
    private Network mNetwork;
    private CellSignalStrength mSignalStrength;
    private boolean mValidated;

    protected class MobileSignalStrengthListener extends PhoneStateListener {
        MobileSignalStrengthListener(int subId) {
            super();
            mSubId = subId;
        }

        @Override
        public void onSignalStrengthsChanged(SignalStrength signalStrength) {
            try {
                List<CellSignalStrength> signalStrengthList = signalStrength.getCellSignalStrengths();
                LogUtils.d(TAG, LogUtils.LOG_CONDITIONS, "onSignalStrengthsChanged " + signalStrengthList);
                if (signalStrengthList != null && !signalStrengthList.isEmpty()) {
                    for (CellSignalStrength cellStrength : signalStrengthList) {
                        if (!cellStrength.isValid()) {
                            continue;
                        }
                        LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "Received " + cellStrength);

                        boolean updated = !mSignalStrength.equals(cellStrength);
                        if (updated) {
                            mSignalStrength = cellStrength;
                            notifyForCallbackEvent(EVENT_MOBILE_SIGNAL_STRENGTH_CHANGED);
                        }
                    }
                }
            } catch (Exception e) {
                LogUtils.e(TAG, "Exception in onSignalStrengthsChanged " + e);
            }
        }
    }

    public MobileNetworkCallback(int slotId, SimInformation simInformation) {
        mSlotId = slotId;
        mSubscriptionId = simInformation.getSubId();
        mTelephonyManager = simInformation.getTelephonyManager();
        mNetwork = null;
        mSignalStrength = new CellSignalStrengthLte();
        mSignalStrengthListener = new MobileSignalStrengthListener(mSubscriptionId);
        mCallbackEvents = new LinkedList<>();
        mValidated = false;

        TAG = ShannonQualifiedNetworksService.getServiceTAG("-NC-" + mSlotId);

        mTelephonyManager.listen(mSignalStrengthListener, LISTEN_SIGNAL_STRENGTHS);
    }

    @Override
    public void onAvailable(@NonNull Network network) {
        LogUtils.i(TAG, LogUtils.LOG_CONDITIONS,"Network " + network + " available for " + mSlotId);
        mNetwork = network;
    }

    @Override
    public void onCapabilitiesChanged(@NonNull Network network,
            @NonNull NetworkCapabilities networkCapabilities) {
        LogUtils.i(TAG, LogUtils.LOG_CONDITIONS,
                "onCapabilitiesChanged for " + network + " NC:" + networkCapabilities);
        boolean validated = networkCapabilities.hasCapability(NetworkCapabilities.NET_CAPABILITY_VALIDATED);
        if (mValidated != validated) {
            mValidated = validated;
            notifyForCallbackEvent(mValidated ? EVENT_MOBILE_CONNECTED : EVENT_MOBILE_DISCONNECTED);
        }
    }

    @Override
    public void onLost(Network network) {
        LogUtils.i(TAG, LogUtils.LOG_CONDITIONS, "Mobile network " + network + " lost for " + mSlotId);
        mNetwork = null;
        mValidated = false;
        notifyForCallbackEvent(EVENT_MOBILE_DISCONNECTED);
    }

    public NetworkRequest buildRequest() {
        return new NetworkRequest.Builder()
                .addCapability(NetworkCapabilities.NET_CAPABILITY_INTERNET)
                .addTransportType(NetworkCapabilities.TRANSPORT_CELLULAR)
                .setNetworkSpecifier(mTelephonyManager.getNetworkSpecifier())
                .build();
    }

    public boolean isConnected() { return ((mNetwork != null) && mValidated); }
    public boolean isRoaming() { return mTelephonyManager.isNetworkRoaming(); }
    public int getSlotId() { return mSlotId; }
    public int getSubId() { return mSubscriptionId; }
    public String getNetworkOperator() { return mTelephonyManager.getNetworkOperator(); }

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

    public boolean isSignalBetterThanThreshold(int threshold) {
        if (!mSignalStrength.isValid()) {
            return false;
        }
        int current = mSignalStrength.getDbm();

        LogUtils.v(TAG, LogUtils.LOG_CONDITIONS,
                "Current signal=" + current + " (threshold=" + threshold + ")");
        return (current > threshold);
    }

    public void destroy() {
        mTelephonyManager.listen(mSignalStrengthListener, LISTEN_NONE);
    }

    public void registerForNetworkCallbackEvent(IMobileCallbackEvent callback) {
        mCallbackEvents.add(callback);
    }

    public void unregisterNetworkCallbackEvent(IMobileCallbackEvent callback) {
        mCallbackEvents.remove(callback);
    }

    private void notifyForCallbackEvent(ConditionEvents conditionEvent) {
        for (IMobileCallbackEvent event : mCallbackEvents) {
            event.onCallbackConditionsChanged(conditionEvent);
        }
    }

    @VisibleForTesting
    protected void setSignalStrength(CellSignalStrength signalStrength) {
        mSignalStrength = signalStrength;
    }
}
