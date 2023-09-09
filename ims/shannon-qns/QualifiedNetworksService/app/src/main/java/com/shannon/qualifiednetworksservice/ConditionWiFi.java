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
import android.net.ConnectivityManager.NetworkCallback;
import android.net.LinkAddress;
import android.net.LinkProperties;
import android.net.Network;
import android.net.NetworkCapabilities;
import android.net.NetworkRequest;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.Handler;
import android.os.HandlerThread;
import android.provider.Settings;
import android.telephony.AccessNetworkConstants;
import android.telephony.NetworkRegistrationInfo;
import android.telephony.ServiceState;
import android.telephony.TelephonyCallback;
import android.telephony.TelephonyManager;
import android.telephony.data.ApnSetting;
import android.text.TextUtils;

import com.android.internal.annotations.VisibleForTesting;
import com.shannon.qualifiednetworksservice.util.LogUtils;
import com.shannon.qualifiednetworksservice.util.ping.IcmpEcho;

import java.net.Inet4Address;
import java.net.Inet6Address;
import java.net.InetAddress;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Locale;
import java.util.Timer;
import java.util.TimerTask;
import java.util.concurrent.ConcurrentLinkedDeque;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.concurrent.Executors;
import java.util.concurrent.atomic.AtomicReference;

import static android.net.NetworkCapabilities.SIGNAL_STRENGTH_UNSPECIFIED;
import static android.telephony.ims.ImsMmTelManager.WIFI_MODE_WIFI_PREFERRED;
import static com.shannon.qualifiednetworksservice.HandoverConditions.ConditionEvents.*;
import static com.shannon.qualifiednetworksservice.MapType.*;

public class ConditionWiFi {
    private static final int MAX_SIGNAL_STRENGTH_SIZE = 3;
    private static final int SIGNAL_STRENGTH_VALIDATION_PERIOD = 30;
    private static final int MIN_RSSI = -100;
    private static final int DEFAULT_RSSI_IN = -70;
    private static final int DEFAULT_RSSI_OUT = -80;

    private enum AirplaneModeState {
        APM_ON,
        APM_OFF,
        APM_TURNED_ON,
        APM_TURNED_OFF
    }

    protected class WiFiBroadcastReceiver extends BroadcastReceiver {
        @Override
        public void onReceive(Context context, Intent intent) {
            final String action = intent.getAction();
            if (action == null) {
                LogUtils.w(TAG, "NULL action for intent " + intent);
                return;
            }
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "Got intent " + intent);
            switch (action) {
                case WifiManager.RSSI_CHANGED_ACTION: {
                    int signalStrength = intent.getIntExtra(WifiManager.EXTRA_NEW_RSSI, 0);
                    if (signalStrength != 0) {
                        updateSignalStrength(signalStrength);
                        if (mNetwork != null) {
                            mChangedCallback.onConditionChanged(EVENT_WIFI_SIGNAL_STRENGTH_CHANGED);
                        }
                    }
                    break;
                }
                case WifiManager.WIFI_STATE_CHANGED_ACTION: {
                    int state = intent.getIntExtra(WifiManager.EXTRA_WIFI_STATE, WifiManager.WIFI_STATE_UNKNOWN);
                    if (mWiFiState != state) {
                        LogUtils.i(TAG, LogUtils.LOG_CONDITIONS,
                                "WiFi state changed from " + getWiFiStateString(mWiFiState) + " to " + getWiFiStateString(state));

                        mWiFiState = state;
                        mChangedCallback.onConditionChanged(EVENT_WIFI_BUTTON_STATE_CHANGED);
                    }
                    break;
                }
                case Intent.ACTION_AIRPLANE_MODE_CHANGED: {
                    setAirplaneModeState(mAirplaneMode);
                    break;
                }
                default:;
            }
        }
    }

    @VisibleForTesting
    protected class WiFiStrengthTimerTask extends TimerTask {
        @Override
        public void run() {
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "WiFi signal validation period expired");
            mStrengthTimer.set(null);
            mChangedCallback.onConditionChanged(EVENT_WIFI_SIGNAL_STRENGTH_CHANGED);
        }
    }

    @VisibleForTesting
    protected class WiFiHandoverTimerTask extends TimerTask {
        @Override
        public void run() {
            LogUtils.d(TAG, LogUtils.LOG_CONDITIONS, "WiFi to WiFi handover timer expired");
            mWiFiHandoverTimer = null;
            mChangedCallback.onConditionChanged(ApnSetting.TYPE_IMS, EVENT_WIFI_AP_HANDOVER_TIMER_EXPIRED);
        }
    }

    @VisibleForTesting
    protected class IpReachableTimerTask extends TimerTask {
        @Override
        public void run() {
            if ((mNetwork == null) || (mLinkProperties == null)) {
                LogUtils.w(TAG, "Network unavailable: " + mNetwork + " , " + mLinkProperties);
                return;
            }
            if (mIpReachableList == null) {
                LogUtils.w(TAG, "Destination unknown");
                return;
            }
            int interval = mConfigurationManager.getPingInterval();
            int count = mConfigurationManager.getPingCount();
            int size = mConfigurationManager.getPingPacketSize();
            int rtt = mConfigurationManager.getPingMaxRtt();
            if (rtt == 0) {
                LogUtils.w(TAG, "RTT condition not provided");
                return;
            }
            if (count == 0) {
                count = 1;
            }
            if (interval < rtt) {
                interval = rtt;
            }
            InetAddress srcIP = null;
            InetAddress dstIP = null;
            for (InetAddress inetAddress : mIpReachableList) {
                if (inetAddress instanceof Inet4Address) {
                    List<LinkAddress> addresses = mLinkProperties.getLinkAddresses();
                    for (LinkAddress a : addresses) {
                        InetAddress ia = a.getAddress();
                        if (ia instanceof Inet4Address) {
                            srcIP = ia;
                            dstIP = inetAddress;
                            break;
                        }
                    }
                }
                if (dstIP != null) {
                    break;
                }
                if (inetAddress instanceof Inet6Address) {
                    List<LinkAddress> addresses = mLinkProperties.getLinkAddresses();
                    for (LinkAddress a : addresses) {
                        InetAddress ia = a.getAddress();
                        if (ia instanceof Inet6Address) {
                            srcIP = ia;
                            dstIP = inetAddress;
                            break;
                        }
                    }
                }
                if (dstIP != null) {
                    break;
                }
            }
            if (dstIP == null) {
                LogUtils.w(TAG, "Destination IP not found");
                return;
            }
            long total = 0;
            for (int i = 0; i < count; i++) {
                long timeout = IcmpEcho.isHostReachable(srcIP, dstIP, size, interval);
                long delay = interval - timeout; total += timeout;
                if (delay > 0) {
                    try {
                        Thread.sleep(delay);
                    } catch (InterruptedException e) {
                        /* not required */
                    }
                }
            }
            int average = (int)(total / count);
            if (average > rtt) {
                LogUtils.d(TAG, LogUtils.LOG_CONDITIONS,
                        "Average RTT=" + average + " exceeds criteria " + rtt + " ms");
                mIpReachable = false;
                mChangedCallback.onConditionChanged(ApnSetting.TYPE_IMS, EVENT_DESTINATION_UNREACHABLE);
            }
        }
    }

    @VisibleForTesting
    protected class WiFiNetworkCallback extends ConnectivityManager.NetworkCallback {
        @Override
        public void onAvailable(Network network) {
            LogUtils.i(TAG, LogUtils.LOG_CONDITIONS, "onAvailable for " + network);
            mNetwork = network;
            addSignalStrengthCallbacks();
            setAirplaneModeState(null);
            updateLocationOnNetworkConnected();
            if (!mWiFiValidationRequired) {
                stopWiFiApHandoverTimer();
            }
            mChangedCallback.onConditionChanged(EVENT_WIFI_CONNECTED);
        }

        @Override
        public void onLost(Network network) {
            LogUtils.i(TAG, LogUtils.LOG_CONDITIONS, "onLost for " + network);
            mNetwork = null;
            mLinkProperties = null;
            mValidated = false;
            removeSignalStrengthCallbacks();
            mSignalStrength.clear();
            stopIpReachableTimer();
            startWiFiApHandoverTimer();
            mChangedCallback.onConditionChanged(EVENT_WIFI_DISCONNECTED);
        }

        @Override
        public void onCapabilitiesChanged(Network network, NetworkCapabilities networkCapabilities) {
            boolean updated = false;
            LogUtils.i(TAG, LogUtils.LOG_CONDITIONS,
                    "onCapabilitiesChanged for " + network + " NC:" + networkCapabilities);

            boolean validated = networkCapabilities.hasCapability(NetworkCapabilities.NET_CAPABILITY_VALIDATED);
            if (mValidated != validated) {
                mValidated = validated; updated = true;
                if (validated) {
                    stopWiFiApHandoverTimer();
                }
            }
            int ss = networkCapabilities.getSignalStrength();
            if (ss > SIGNAL_STRENGTH_UNSPECIFIED) {
                updateSignalStrength(ss); updated = true;
            }
            /* notify if required */
            if (updated) {
                mChangedCallback.onConditionChanged(EVENT_WIFI_CAPABILITIES_CHANGED);
            }
        }

        @Override
        public void onLinkPropertiesChanged(Network network, LinkProperties linkProperties) {
            LogUtils.i(TAG, LogUtils.LOG_CONDITIONS,
                    "onLinkPropertiesChanged for " + network + " LP:" + linkProperties);
            boolean updated = isLinkAddressChanged(linkProperties);
            mLinkProperties = linkProperties;
            if (updated) {
                mChangedCallback.onConditionChanged(EVENT_WIFI_LINK_PROPERTIES_CHANGED);
            }
        }
    }

    private void updateLocationOnNetworkConnected() {
        LocationManager.getInstance().updateLocationOnNetworkConnected();
    }

    @VisibleForTesting
    protected class DefaultNetworkCallback extends ConnectivityManager.NetworkCallback {
        @Override
        public void onLost(Network network) {
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "onLost for default " + network);
            mDefault = false;
        }

        @Override
        public void onLinkPropertiesChanged(Network network, LinkProperties linkProperties) {
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS,
                    "onLinkPropertiesChanged for default " + network + " LP:" + linkProperties);

            String name = linkProperties.getInterfaceName();
            mDefault = (name != null && !name.isEmpty() && name.startsWith("wlan"));

            mChangedCallback.onConditionChanged(EVENT_DEFAULT_LINK_PROPERTIES_CHANGED);
        }
    }

    protected class WlanPsStateListener extends TelephonyCallback
            implements TelephonyCallback.ServiceStateListener {
        @Override
        public void onServiceStateChanged(ServiceState serviceState) {
            if (serviceState == null) {
                return;
            }
            NetworkRegistrationInfo psState = serviceState.getNetworkRegistrationInfo(
                    NetworkRegistrationInfo.DOMAIN_PS, AccessNetworkConstants.TRANSPORT_TYPE_WLAN);
            if ((psState != null) && !mPsRegState.equals(psState)) {
                LogUtils.i(TAG, LogUtils.LOG_CONDITIONS, "IWLAN PS registration state changed to " + psState);

                mPsRegState = psState;
                mChangedCallback.onConditionChanged(EVENT_WLAN_PS_STATE_CHANGED);
            }
        }
    }

    private final String TAG;
    private final int mSlotId;
    private final Context mContext;
    private final ConnectivityManager mConnectivityManager;
    private final TelephonyManager mTelephonyManager;
    private final HandoverConditions.IConditionChanged mChangedCallback;
    private final HandoverConditions.IConditionHelper mConditionsHelper;
    private final WiFiBroadcastReceiver mBroadcastReceiver;
    private final WiFiNetworkCallback mNetworkCallback;
    private final ConfigurationManager mConfigurationManager;
    private final String mFqdnEnding;
    private Network mNetwork;
    private LinkProperties mLinkProperties;
    private boolean mValidated;
    private boolean mDefault;
    private boolean mIpReachable;
    private final ConcurrentLinkedDeque<Integer> mSignalStrength;
    private long mLastStrengthUpdateTimestamp;
    private final AtomicReference<Timer> mStrengthTimer;
    private final ConcurrentLinkedQueue<NetworkCallback> mCallbackList;
    private Timer mIpReachableTimer;
    private InetAddress[] mIpReachableList;
    private DefaultNetworkCallback mDefaultCallback;
    private Timer mWiFiHandoverTimer;
    private AirplaneModeState mAirplaneMode;
    private int mWiFiState;
    private boolean mWiFiValidationRequired;
    private HashMap<MapType, Integer> mRssiIn;
    private HashMap<MapType, Integer> mRssiOut;
    private boolean mDestroyed;
    private NetworkRegistrationInfo mPsRegState;
    private final WlanPsStateListener mPsStateListener;
    private boolean mRoaming;
    private final HandlerThread mHandlerThread;

    public ConditionWiFi(int slotIndex, Context context, ConfigurationManager configurationManager,
                         HandoverConditions.IConditionHelper helper,
                         HandoverConditions.IConditionChanged callback) {
        mDestroyed = false;
        mSlotId = slotIndex;
        mContext = context;
        mConfigurationManager = configurationManager;
        mConditionsHelper = helper;
        mChangedCallback = callback;
        mConnectivityManager = mContext.getSystemService(ConnectivityManager.class);
        mBroadcastReceiver = new WiFiBroadcastReceiver();
        mNetworkCallback = new WiFiNetworkCallback();
        SimManager simManager = SimManager.getInstance();
        mFqdnEnding = String.format(Locale.US, "mnc%03d.mcc%03d.pub.3gppnetwork.org",
                simManager.getMnc(mSlotId), simManager.getMcc(mSlotId));
        mTelephonyManager = simManager.getTelephonyManager(mSlotId);

        TAG = ShannonQualifiedNetworksService.getServiceTAG("-" + mSlotId + "-HC-wifi");

        mNetwork = null;
        mLinkProperties = null;
        mValidated = false;
        mDefault = true;
        mIpReachable = true;
        mSignalStrength = new ConcurrentLinkedDeque<>();
        mLastStrengthUpdateTimestamp = System.nanoTime();
        mStrengthTimer = new AtomicReference<>();
        mCallbackList = new ConcurrentLinkedQueue<>();
        mIpReachableTimer = null;
        mIpReachableList = null;
        mDefaultCallback = null;
        mWiFiHandoverTimer = null;
        setAirplaneModeState(null);
        mWiFiState = getWiFiState();
        buildRssiHashMaps();
        mRoaming = mConfigurationManager.getUnknownLocationAsRoaming();

        /* get current RSSI */
        WifiManager manager = mContext.getSystemService(WifiManager.class);
        if (manager != null) {
            WifiInfo info = manager.getConnectionInfo();
            if (info != null) {
                updateSignalStrength(info.getRssi());
            }
        }

        /* get last service state */
        mPsStateListener = new WlanPsStateListener();
        mPsRegState = null;
        try {
            ServiceState serviceState = mTelephonyManager.getServiceState();
            mPsRegState = serviceState.getNetworkRegistrationInfo(
                    NetworkRegistrationInfo.DOMAIN_PS, AccessNetworkConstants.TRANSPORT_TYPE_WLAN);
            mTelephonyManager.registerTelephonyCallback(
                    Executors.newSingleThreadExecutor(), mPsStateListener);
        } catch (Exception e) {
            LogUtils.e(TAG, "Failed to get service state: " + e);
        }
        if (mPsRegState == null) {
            mPsRegState = new NetworkRegistrationInfo.Builder()
                    .setAccessNetworkTechnology(TelephonyManager.NETWORK_TYPE_UNKNOWN)
                    .setRegistrationState(NetworkRegistrationInfo.REGISTRATION_STATE_UNKNOWN)
                    .build();
        }

        mHandlerThread = new HandlerThread(ConditionWiFi.class.getSimpleName());
        mHandlerThread.start();
        Handler handler = new Handler(mHandlerThread.getLooper());

        NetworkRequest.Builder builder = new NetworkRequest.Builder();
        builder.addCapability(NetworkCapabilities.NET_CAPABILITY_INTERNET);
        builder.addCapability(NetworkCapabilities.NET_CAPABILITY_NOT_RESTRICTED);
        builder.addCapability(NetworkCapabilities.NET_CAPABILITY_NOT_VCN_MANAGED);
        builder.addCapability(NetworkCapabilities.NET_CAPABILITY_NOT_VPN);
        builder.addCapability(NetworkCapabilities.NET_CAPABILITY_TRUSTED);
        builder.addTransportType(NetworkCapabilities.TRANSPORT_WIFI);
        mConnectivityManager.registerBestMatchingNetworkCallback (builder.build(),
                mNetworkCallback, handler);

        mWiFiValidationRequired = mConfigurationManager.isWiFiValidationRequired();
        if (!mWiFiValidationRequired) {
            addDefaultNetworkCallback();
        }

        IntentFilter filter = new IntentFilter();
        filter.addAction(WifiManager.RSSI_CHANGED_ACTION);
        filter.addAction(WifiManager.WIFI_STATE_CHANGED_ACTION);
        filter.addAction(Intent.ACTION_AIRPLANE_MODE_CHANGED);
        mContext.registerReceiver(mBroadcastReceiver, filter);
    }

    public void destroy() {
        mDestroyed = true;
        stopIpReachableTimer();
        removeSignalStrengthCallbacks();
        removeDefaultNetworkCallback();
        mTelephonyManager.unregisterTelephonyCallback(mPsStateListener);
        mContext.unregisterReceiver(mBroadcastReceiver);
        mConnectivityManager.unregisterNetworkCallback(mNetworkCallback);
        mHandlerThread.quitSafely();
    }

    public boolean isConnected() {
        if (isWiFiDisabled()) {
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "WiFi button off");
            return false;
        }
        if (isWiFiHandoverActive()) {
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "WiFi to WiFi handover in progress, connected = true");
            return true;
        }
        boolean connected = (mNetwork != null) && isIpAddressSet() && mPsRegState.isRegistered();
        if (mWiFiValidationRequired) {
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "WiFi connected: " + connected + " validated: " + mValidated);
            return (connected && mValidated);
        }
        LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "WiFi connected: " + connected + " default: " + mDefault);
        return connected && (mDefault || mConditionsHelper.isImsConnectedOnWiFi());
    }

    public boolean isWiFiApHandoverActive() {
        boolean connected = (mNetwork != null) && isIpAddressSet() && mPsRegState.isRegistered();
        boolean handover = isWiFiHandoverActive();
        LogUtils.v(TAG, LogUtils.LOG_CONDITIONS,
                "WiFi handover: " + handover + " connected: " + connected + " validated: " + mValidated);
        return handover && !(connected && (!mWiFiValidationRequired || mValidated));
    }

    private boolean isBetterThanThreshold(int threshold, String outStr) {
        int lastStrength = mSignalStrength.getFirst();
        LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "WiFi strength: " + lastStrength + outStr + threshold);

        /* signal much better than threshold */
        if (lastStrength >= (threshold + 5)) {
            return true;
        }
        /* stable signal, no updates */
        long diff = (System.nanoTime() - mLastStrengthUpdateTimestamp) / 1000000000;
        if ((lastStrength >= threshold) &&
                (diff > SIGNAL_STRENGTH_VALIDATION_PERIOD)) {
            return true;
        }
        /* calculate average */
        int avg = 0;
        Integer[] strengths = mSignalStrength.toArray(new Integer[0]);
        for (int i = 0; i < MAX_SIGNAL_STRENGTH_SIZE; i++) {
            if (i >= strengths.length) {
                avg += MIN_RSSI;
            } else {
                avg += strengths[i];
            }
        }
        avg = avg / MAX_SIGNAL_STRENGTH_SIZE;
        LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "WiFi average strength: " + avg + outStr + threshold);
        return  (avg >= threshold);
    }

    public boolean isRoveIn() {
        if (isRoveOut()) {
            return false;
        }
        if (!mSignalStrength.isEmpty()) {
            return isBetterThanThreshold(
                    getRssiRoveIn(), " good: ");
        }
        return false;
    }

    public boolean isRoveIn(boolean roaming, boolean hysteresis) {
        if (isRoveOut(hysteresis)) {
            return false;
        }
        if (!mSignalStrength.isEmpty()) {
            if (hysteresis) {
                return isBetterThanThreshold(getRssiRoveInHysteresisOn(), " good: ");
            }
            return isBetterThanThreshold(
                    roaming ? getRssiRoveInRoaming() : getRssiRoveIn(),
                    " good: ");
        }
        return false;
    }

    public boolean isRoveIn(int mode) {
        if (isRoveOut(mode)) {
            return false;
        }
        if (!mSignalStrength.isEmpty()) {
            return isBetterThanThreshold(
                    (mode == WIFI_MODE_WIFI_PREFERRED) ?
                    getRssiRoveInWiFiPreferred() : getRssiRoveInCellPreferred(), " good: ");
        }
        return false;
    }

    public int getRssi() {
        if (!mSignalStrength.isEmpty()) {
            return mSignalStrength.getFirst();
        }
        return MIN_RSSI;
    }

    private boolean isRoveOutInternal(int strengthOut) {
        if (!mSignalStrength.isEmpty()) {
            int lastStrength = mSignalStrength.getFirst();
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "WiFi strength: " + lastStrength + " bad: " + strengthOut);
            return (lastStrength <= strengthOut);
        }
        return true;
    }

    public boolean isRoveOut() {
        return isRoveOutInternal(getRssiRoveOut());
    }

    public boolean isRoveOut(boolean hysteresis) {
        return isRoveOutInternal(hysteresis ? getRssiRoveOutHysteresisOn() : getRssiRoveOut());
    }

    public boolean isRoveOut(int mode) {
        return isRoveOutInternal((mode == WIFI_MODE_WIFI_PREFERRED) ?
                getRssiRoveOutWiFiPreferred() : getRssiRoveOutCellPreferred());
    }

    /* in condition with active call */
    public boolean isHandIn() {
        if (isHandOut()) {
            return false;
        }
        if (!mSignalStrength.isEmpty()) {
            return isBetterThanThreshold(
                    getRssiHandIn(), " good_in_call: ");
        }
        return false;
    }

    public boolean isHandIn(boolean roaming, boolean hysteresis) {
        if (isHandOut(hysteresis)) {
            return false;
        }
        if (!mSignalStrength.isEmpty()) {
            if (hysteresis) {
                return isBetterThanThreshold(getRssiHandInHysteresisOn(), " good_in_call: ");
            }
            return isBetterThanThreshold(
                    roaming ? getRssiHandInRoaming() : getRssiHandIn(),
                    " good_in_call: ");
        }
        return false;
    }

    public boolean isHandIn(int mode) {
        if (isHandOut(mode)) {
            return false;
        }
        if (!mSignalStrength.isEmpty()) {
            return isBetterThanThreshold(
                    (mode == WIFI_MODE_WIFI_PREFERRED) ?
                    getRssiHandInWiFiPreferred() : getRssiHandInCellPreferred(), " good_in_call: ");
        }
        return false;
    }

    /* out condition with active call */
    private boolean isHandOutInternal(int strengthOut) {
        if (!mSignalStrength.isEmpty()) {
            int lastStrength = mSignalStrength.getFirst();
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "WiFi strength: " + lastStrength + " bad_in_call: " + strengthOut);
            return (lastStrength <= strengthOut);
        }
        return true;
    }

    public boolean isHandOut() {
        return isHandOutInternal(getRssiHandOut());
    }

    public boolean isHandOut(boolean hysteresis) {
        return isHandOutInternal(hysteresis ? getRssiHandOutHysteresisOn() : getRssiHandOut());
    }

    public boolean isHandOut(int mode) {
        return isHandOutInternal((mode == WIFI_MODE_WIFI_PREFERRED) ?
                getRssiHandOutWiFiPreferred() : getRssiHandOutCellPreferred());
    }

    public boolean isLowerThanThreshold(int threshold) {
        if (!mSignalStrength.isEmpty()) {
            int lastStrength = mSignalStrength.getFirst();
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "WiFi strength: " + lastStrength + " threshold: " + threshold);
            return (lastStrength <= threshold);
        }
        return true;
    }

    public boolean isBetterThanThreshold(int threshold) {
        if (!mSignalStrength.isEmpty()) {
            int lastStrength = mSignalStrength.getFirst();
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "WiFi strength: " + lastStrength + " threshold: " + threshold);
            return (lastStrength > threshold);
        }
        return false;
    }

    public boolean isIpReachable() {
        return mIpReachable;
    }

    private synchronized void updateSignalStrength(int strength) {
        if (strength >= 0) {
            /* ignore invalid strength */
            return;
        }
        mSignalStrength.addFirst(strength);
        if (mSignalStrength.size() > MAX_SIGNAL_STRENGTH_SIZE) {
            mSignalStrength.removeLast();
        }
        mLastStrengthUpdateTimestamp = System.nanoTime();
        startStrengthTimer();
    }

    private void startStrengthTimer() {
        if (!mDestroyed) {
            Timer timer = new Timer();
            timer.schedule(new WiFiStrengthTimerTask(), SIGNAL_STRENGTH_VALIDATION_PERIOD * 1000);
            Timer prevTimer = mStrengthTimer.getAndSet(timer);
            if (prevTimer != null) {
                prevTimer.cancel();
            }
        }
    }

    private void stopStrengthTimer() {
        Timer timer = mStrengthTimer.getAndSet(null);
        if (timer != null) {
            timer.cancel();
        }
    }

    private void addSignalStrengthCallbacks() {
        /* In */
        ConnectivityManager.NetworkCallback cb = new ConnectivityManager.NetworkCallback();
        mCallbackList.add(cb);
        NetworkRequest.Builder builder = new NetworkRequest.Builder();
        builder.addCapability(NetworkCapabilities.NET_CAPABILITY_INTERNET);
        builder.addCapability(NetworkCapabilities.NET_CAPABILITY_NOT_RESTRICTED);
        builder.addTransportType(NetworkCapabilities.TRANSPORT_WIFI);
        builder.setSignalStrength(getRssiRoveIn());
        mConnectivityManager.registerNetworkCallback(builder.build(), cb);

        /* Out */
        cb = new ConnectivityManager.NetworkCallback();
        mCallbackList.add(cb);
        builder = new NetworkRequest.Builder();
        builder.addCapability(NetworkCapabilities.NET_CAPABILITY_INTERNET);
        builder.addCapability(NetworkCapabilities.NET_CAPABILITY_NOT_RESTRICTED);
        builder.addTransportType(NetworkCapabilities.TRANSPORT_WIFI);
        builder.setSignalStrength(getRssiRoveOut() + 1);
        mConnectivityManager.registerNetworkCallback(builder.build(), cb);
    }

    private void removeSignalStrengthCallbacks() {
        if (!mCallbackList.isEmpty()) {
            for (ConnectivityManager.NetworkCallback cb : mCallbackList) {
                mConnectivityManager.unregisterNetworkCallback(cb);
            }
            mCallbackList.clear();
        }
        stopStrengthTimer();
    }

    private void addDefaultNetworkCallback() {
        mDefault = false;

        if (mDefaultCallback == null) {
            mDefaultCallback = new DefaultNetworkCallback();
            mConnectivityManager.registerDefaultNetworkCallback(mDefaultCallback);
        }
    }

    private void removeDefaultNetworkCallback() {
        if (mDefaultCallback != null) {
            mConnectivityManager.unregisterNetworkCallback(mDefaultCallback);
            mDefaultCallback = null;
        }
        mDefault = true;
    }

    public void startIpReachableTimer() {
        if (mIpReachableTimer == null) {
            /* WiFi not connected */
            if (mNetwork == null) {
                return;
            }
            int period = mConfigurationManager.getPingPeriod();
            if (period > 0) {
                mIpReachableList = getIpListToReach();
                /* could not resolve IP address */
                if (mIpReachableList == null) {
                    return;
                }
                LogUtils.d(TAG, LogUtils.LOG_CONDITIONS, "Start IP reachable timer task for " +
                        period + " seconds");
                mIpReachableTimer = new Timer();
                mIpReachableTimer.schedule(new IpReachableTimerTask(),
                        period * 1000, period * 1000);
            }
        }
    }

    public void stopIpReachableTimer() {
        if (mIpReachableTimer != null) {
            mIpReachableTimer.cancel();
            mIpReachableTimer = null;
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "Stop IP reachable timer");
        }
        mIpReachable = true;
    }

    private InetAddress[] getIpListToReach() {
        String destination = mConfigurationManager.getPingDestination();
        if ((destination == null) || destination.isEmpty()) {
            LogUtils.w(TAG, "No destination provided for IP reach");
            return null;
        }
        if (destination.equalsIgnoreCase("epdg.epc.mnc.mcc")) {
            destination = "epdg.epc." + mFqdnEnding;
        } else if (destination.equalsIgnoreCase("epdg.ims.mnc.mcc")) {
            destination = "epdg.ims." + mFqdnEnding;
        }
        InetAddress[] result = null;
        try {
            result = mNetwork.getAllByName(destination);
        } catch (Exception e) {
            /* not required */
        }
        LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "IP to reach " + Arrays.toString(result) + " FQDN: " + destination);
        return result;
    }

    public void onConfigurationChanged() {
        boolean oldValue = mWiFiValidationRequired;
        mWiFiValidationRequired = mConfigurationManager.isWiFiValidationRequired();

        buildRssiHashMaps();

        removeSignalStrengthCallbacks();
        addSignalStrengthCallbacks();

        if (oldValue != mWiFiValidationRequired) {
            if (mWiFiValidationRequired) {
                /* transition false -> true */
                removeDefaultNetworkCallback();
            } else {
                /* transition true -> false */
                addDefaultNetworkCallback();
            }
        }
    }

    private int getWiFiState() {
        int result = WifiManager.WIFI_STATE_UNKNOWN;
        try {
            WifiManager manager = mContext.getSystemService(WifiManager.class);
            result = manager.getWifiState();
        } catch (Exception e) {
            LogUtils.e(TAG, "Failed to read WiFi state: " + e);
        }
        LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "WiFi state is " + getWiFiStateString(result));
        return result;
    }

    private boolean isWiFiDisabled() {
        return (mWiFiState != WifiManager.WIFI_STATE_ENABLED);
    }

    private String getWiFiStateString(int state) {
        switch (state) {
            case WifiManager.WIFI_STATE_ENABLING: return "ENABLING";
            case WifiManager.WIFI_STATE_ENABLED: return "ENABLED";
            case WifiManager.WIFI_STATE_DISABLING: return "DISABLING";
            case WifiManager.WIFI_STATE_DISABLED: return "DISABLED";
        }
        return "UNKNOWN";
    }

    private void startWiFiApHandoverTimer() {
        if (mConfigurationManager.isWiFiApHandoverSupported()) {
            if (isAirplaneModeTurnedOn()) {
                LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "Airplane mode changed to ON");
                return;
            }
            if (isWiFiDisabled()) {
                LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "WiFi button off, WiFi handover not possible");
                return;
            }
            if (mConditionsHelper.isWiFiTransportNotUsed()) {
                LogUtils.d(TAG, LogUtils.LOG_CONDITIONS, "WiFi transport not used by any APN, do not start timer");
                return;
            }
            if (mWiFiHandoverTimer != null) {
                LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "WiFi to WiFi handover timer already started");
            } else {
                int timeout = mConfigurationManager.getWiFiApHandoverTimeout();
                if (timeout > 0) {
                    LogUtils.d(TAG, LogUtils.LOG_CONDITIONS, "Start WiFi to WiFi handover timer for " +
                            timeout + " seconds");
                    mWiFiHandoverTimer = new Timer();
                    mWiFiHandoverTimer.schedule(new WiFiHandoverTimerTask(), timeout * 1000);
                }
            }
        }
    }

    private boolean isWiFiHandoverActive() {
        return (mWiFiHandoverTimer != null);
    }

    public void stopWiFiApHandoverTimer() {
        if (mWiFiHandoverTimer != null) {
            mWiFiHandoverTimer.cancel();
            mWiFiHandoverTimer = null;
            LogUtils.d(TAG, LogUtils.LOG_CONDITIONS, "Stop WiFi to WiFi handover timer");
        }
    }

    private boolean isAirplaneModeEnabled() {
        boolean result = false;
        try {
            result = (Settings.Global.getInt(mContext.getContentResolver(), Settings.Global.AIRPLANE_MODE_ON, 0) != 0);
        } catch (Exception e) {
            LogUtils.e(TAG, "Failed to read airplane mode setting " + e);
        }
        return result;
    }

    private void setAirplaneModeState(AirplaneModeState state) {
        boolean isApmOn = isAirplaneModeEnabled();

        /* initial setting */
        if (state == null) {
            if (isApmOn) {
                mAirplaneMode = AirplaneModeState.APM_ON;
            } else {
                mAirplaneMode = AirplaneModeState.APM_OFF;
            }
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "Airplane mode: " + mAirplaneMode);
            return;
        }
        /* changed */
        switch (state) {
            case APM_ON:
            case APM_TURNED_ON: {
                if (!isApmOn) {
                    mAirplaneMode = AirplaneModeState.APM_TURNED_OFF;
                    LogUtils.d(TAG, LogUtils.LOG_CONDITIONS,
                            "Airplane mode changed: " + mAirplaneMode);
                }
                break;
            }
            case APM_OFF:
            case APM_TURNED_OFF: {
                if (isApmOn) {
                    mAirplaneMode = AirplaneModeState.APM_TURNED_ON;
                    LogUtils.d(TAG, LogUtils.LOG_CONDITIONS,
                            "Airplane mode changed: " + mAirplaneMode);
                }
                break;
            }
            default:;
        }
    }

    private boolean isAirplaneModeTurnedOn() {
        switch (mAirplaneMode) {
            case APM_ON:
            case APM_OFF: {
                break;
            }
            case APM_TURNED_ON: {
                mAirplaneMode = AirplaneModeState.APM_ON;
                return true;
            }
            case APM_TURNED_OFF: {
                mAirplaneMode = AirplaneModeState.APM_OFF;
                break;
            }
            default:;
        }
        return false;
    }

    private String getParametersString(String input) {
        if (!TextUtils.isEmpty(input)) {
            String[] strings = input.split("\\{");
            if (strings.length > 1 && !TextUtils.isEmpty(strings[1])) {
                return strings[1].trim().substring(0, strings[1].length() - 1).trim();
            }
        }
        return "";
    }

    private int processConfigurationString(String string) {
        if (TextUtils.isEmpty(string)) {
            return 0;
        }
        String[] strings = string.split(";");
        for (String s : strings) {
            s = s.trim();

            if (s.startsWith("iwlan") || s.startsWith("IWLAN")) {
                String[] params = getParametersString(s).split("=");
                if (params.length > 1 && params[0].trim().equalsIgnoreCase("rssi")) {
                    try {
                        return Integer.parseInt(params[1].trim());
                    } catch (Exception e) {
                        LogUtils.e(TAG, "Failed to parse configuration RSSI: " + e);
                    }
                }
            }
        }
        return 0;
    }

    private void buildRssiHashMaps() {
        mRssiIn = new HashMap<>();
        int value = processConfigurationString(mConfigurationManager.getDefaultImsHandoverIn());
        if (value != 0) {
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "Default RSSI in: " + value);
            mRssiIn.put(MAP_DEFAULT, value);
        }
        value = processConfigurationString(mConfigurationManager.getImsRoveIn());
        if (value != 0) {
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "RSSI rove_in: " + value);
            mRssiIn.put(MAP_IDLE, value);
        }
        value = processConfigurationString(mConfigurationManager.getImsRoveInWiFiPreferred());
        if (value != 0) {
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "RSSI rove_in WiFi preferred: " + value);
            mRssiIn.put(MAP_IDLE_WIFI, value);
        }
        value = processConfigurationString(mConfigurationManager.getImsRoveInCellPreferred());
        if (value != 0) {
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "RSSI rove_in cellular preferred: " + value);
            mRssiIn.put(MAP_IDLE_CELL, value);
        }
        value = processConfigurationString(mConfigurationManager.getImsHandIn());
        if (value != 0) {
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "RSSI hand_in: " + value);
            mRssiIn.put(MAP_CALL, value);
        }
        value = processConfigurationString(mConfigurationManager.getImsHandInWiFiPreferred());
        if (value != 0) {
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "RSSI hand_in WiFi preferred: " + value);
            mRssiIn.put(MAP_CALL_WIFI, value);
        }
        value = processConfigurationString(mConfigurationManager.getImsHandInCellPreferred());
        if (value != 0) {
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "RSSI hand_in cellular preferred: " + value);
            mRssiIn.put(MAP_CALL_CELL, value);
        }
        value = processConfigurationString(mConfigurationManager.getImsRoveInRoaming());
        if (value != 0) {
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "RSSI rove_in in roaming: " + value);
            mRssiIn.put(MAP_IDLE_ROAM, value);
        }
        value = processConfigurationString(mConfigurationManager.getImsRoveInHysteresisOn());
        if (value != 0) {
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "RSSI rove_in with hysteresis timer on: " + value);
            mRssiIn.put(MAP_IDLE_TIMER, value);
        }
        value = processConfigurationString(mConfigurationManager.getImsHandInRoaming());
        if (value != 0) {
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "RSSI hand_in in roaming: " + value);
            mRssiIn.put(MAP_CALL_ROAM, value);
        }
        value = processConfigurationString(mConfigurationManager.getImsHandInHysteresisOn());
        if (value != 0) {
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "RSSI hand_in with hysteresis timer on: " + value);
            mRssiIn.put(MAP_CALL_TIMER, value);
        }

        mRssiOut = new HashMap<>();
        value = processConfigurationString(mConfigurationManager.getDefaultImsHandoverOut());
        if (value != 0) {
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "Default RSSI out: " + value);
            mRssiOut.put(MAP_DEFAULT, value);
        }
        value = processConfigurationString(mConfigurationManager.getImsRoveOut());
        if (value != 0) {
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "RSSI rove_out: " + value);
            mRssiOut.put(MAP_IDLE, value);
        }
        value = processConfigurationString(mConfigurationManager.getImsRoveOutWiFiPreferred());
        if (value != 0) {
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "RSSI rove_out WiFi preferred: " + value);
            mRssiOut.put(MAP_IDLE_WIFI, value);
        }
        value = processConfigurationString(mConfigurationManager.getImsRoveOutCellPreferred());
        if (value != 0) {
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "RSSI rove_out cellular preferred: " + value);
            mRssiOut.put(MAP_IDLE_CELL, value);
        }
        value = processConfigurationString(mConfigurationManager.getImsHandOut());
        if (value != 0) {
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "RSSI hand_out: " + value);
            mRssiOut.put(MAP_CALL, value);
        }
        value = processConfigurationString(mConfigurationManager.getImsHandOutWiFiPreferred());
        if (value != 0) {
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "RSSI hand_out WiFi preferred: " + value);
            mRssiOut.put(MAP_CALL_WIFI, value);
        }
        value = processConfigurationString(mConfigurationManager.getImsHandOutCellPreferred());
        if (value != 0) {
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "RSSI hand_out cellular preferred: " + value);
            mRssiOut.put(MAP_CALL_CELL, value);
        }
        value = processConfigurationString(mConfigurationManager.getImsRoveOutHysteresisOn());
        if (value != 0) {
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "RSSI rove_out with hysteresis timer on: " + value);
            mRssiOut.put(MAP_IDLE_TIMER, value);
        }
        value = processConfigurationString(mConfigurationManager.getImsHandOutHysteresisOn());
        if (value != 0) {
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "RSSI hand_out with hysteresis timer on: " + value);
            mRssiOut.put(MAP_CALL_TIMER, value);
        }
    }

    public int getRssiRoveIn() {
        Integer res = mRssiIn.get(MAP_IDLE);
        if (res != null && res != 0) {
            return res;
        }
        res = mRssiIn.get(MAP_DEFAULT);
        if (res != null) {
            return res;
        }
        return DEFAULT_RSSI_IN;
    }

    private int getRssiRoveInRoaming() {
        Integer res = mRssiIn.get(MAP_IDLE_ROAM);
        if (res != null && res != 0) {
            return res;
        }
        return getRssiRoveIn();
    }

    private int getRssiRoveInHysteresisOn() {
        Integer res = mRssiIn.get(MAP_IDLE_TIMER);
        if (res != null && res != 0) {
            return res;
        }
        return getRssiRoveIn();
    }

    private int getRssiRoveInWiFiPreferred() {
        Integer res = mRssiIn.get(MAP_IDLE_WIFI);
        if (res != null && res != 0) {
            return res;
        }
        return getRssiRoveIn();
    }

    private int getRssiRoveInCellPreferred() {
        Integer res = mRssiIn.get(MAP_IDLE_CELL);
        if (res != null && res != 0) {
            return res;
        }
        return getRssiRoveIn();
    }

    public int getRssiRoveOut() {
        Integer res = mRssiOut.get(MAP_IDLE);
        if (res != null && res != 0) {
            return res;
        }
        res = mRssiOut.get(MAP_DEFAULT);
        if (res != null) {
            return res;
        }
        return DEFAULT_RSSI_OUT;
    }

    private int getRssiRoveOutHysteresisOn() {
        Integer res = mRssiOut.get(MAP_IDLE_TIMER);
        if (res != null && res != 0) {
            return res;
        }
        return getRssiRoveOut();
    }

    private int getRssiRoveOutWiFiPreferred() {
        Integer res = mRssiOut.get(MAP_IDLE_WIFI);
        if (res != null && res != 0) {
            return res;
        }
        return getRssiRoveOut();
    }

    private int getRssiRoveOutCellPreferred() {
        Integer res = mRssiOut.get(MAP_IDLE_CELL);
        if (res != null && res != 0) {
            return res;
        }
        return getRssiRoveOut();
    }

    private int getRssiHandIn() {
        Integer res = mRssiIn.get(MAP_CALL);
        if (res != null && res != 0) {
            return res;
        }
        return getRssiRoveIn();
    }

    private int getRssiHandInRoaming() {
        Integer res = mRssiIn.get(MAP_CALL_ROAM);
        if (res != null && res != 0) {
            return res;
        }
        return getRssiHandIn();
    }

    private int getRssiHandInHysteresisOn() {
        Integer res = mRssiIn.get(MAP_CALL_TIMER);
        if (res != null && res != 0) {
            return res;
        }
        return getRssiRoveInHysteresisOn();
    }

    private int getRssiHandInWiFiPreferred() {
        Integer res = mRssiIn.get(MAP_CALL_WIFI);
        if (res != null && res != 0) {
            return res;
        }
        return getRssiRoveInWiFiPreferred();
    }

    private int getRssiHandInCellPreferred() {
        Integer res = mRssiIn.get(MAP_CALL_CELL);
        if (res != null && res != 0) {
            return res;
        }
        return getRssiRoveInCellPreferred();
    }

    private int getRssiHandOut() {
        Integer res = mRssiOut.get(MAP_CALL);
        if (res != null && res != 0) {
            return res;
        }
        return getRssiRoveOut();
    }

    private int getRssiHandOutHysteresisOn() {
        Integer res = mRssiOut.get(MAP_CALL_TIMER);
        if (res != null && res != 0) {
            return res;
        }
        return getRssiRoveOutHysteresisOn();
    }

    private int getRssiHandOutWiFiPreferred() {
        Integer res = mRssiOut.get(MAP_CALL_WIFI);
        if (res != null && res != 0) {
            return res;
        }
        return getRssiRoveOutWiFiPreferred();
    }

    private int getRssiHandOutCellPreferred() {
        Integer res = mRssiOut.get(MAP_CALL_CELL);
        if (res != null && res != 0) {
            return res;
        }
        return getRssiRoveOutCellPreferred();
    }

    public void setRssiRoveIn(int rssi) {
        mRssiIn.put(MAP_IDLE, rssi);
    }

    public void setRssiRoveOut(int rssi) {
        mRssiOut.put(MAP_IDLE, rssi);
    }

    public boolean isRoaming() {
        String curCountry = LocationManager.getInstance().getCountryCode();
        String homeCountry = mTelephonyManager.getSimCountryIso();

        if (TextUtils.isEmpty(curCountry) || TextUtils.isEmpty(homeCountry)) {
            boolean unknownLocationAsRoaming = mConfigurationManager.getUnknownLocationAsRoaming();
            if (mRoaming != unknownLocationAsRoaming) {
                LogUtils.d(TAG, LogUtils.LOG_CONDITIONS,
                        "Use unknownLocationAsRoaming = " + unknownLocationAsRoaming);
                mRoaming = unknownLocationAsRoaming;
            }
        } else {
            boolean result = !curCountry.equalsIgnoreCase(homeCountry);
            if (mRoaming != result) {
                LogUtils.i(TAG, LogUtils.LOG_CONDITIONS,
                        "WiFi roaming changed. Home=" + homeCountry.toUpperCase() +
                                " current=" + curCountry.toUpperCase());
                mRoaming = result;
            }
        }
        return mRoaming;
    }

    public void requestNetworkValidation() {
        /* If mNetwork is null, default network will be selected */
        try {
            mConnectivityManager.reportNetworkConnectivity(mNetwork, false);
        } catch (Exception e) {
            LogUtils.e(TAG, "Failed to request network validation with error " + e);
        }
    }

    private boolean isIpAddressSet() {
        if (mLinkProperties == null) {
            return false;
        }
        boolean canUseIPv6 = mConfigurationManager.canUseWiFiIPv6();
        LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "WiFi attached, use IPv6: " + canUseIPv6);
        for (LinkAddress address : mLinkProperties.getLinkAddresses()) {
            InetAddress inetAddress = address.getAddress();
            if (canUseIPv6 && (inetAddress instanceof Inet6Address) &&
                    !((Inet6Address) inetAddress).isLinkLocalAddress()) {
                return true;
            } else if (inetAddress instanceof Inet4Address) {
                return true;
            }
        }
        return false;
    }

    private boolean isLinkAddressChanged(LinkProperties newProperties) {
        if ((mLinkProperties == null) || (newProperties == null)) {
            return false;
        }
        List<LinkAddress> oldAddresses = mLinkProperties.getLinkAddresses();
        List<LinkAddress> newAddresses = newProperties.getLinkAddresses();
        return (newAddresses.size() != oldAddresses.size()) ||
                !newAddresses.containsAll(oldAddresses);
    }

    @Override
    public String toString() {
        return ("WIFI" + mSlotId + ": state=" + getWiFiStateString(mWiFiState) + " network=" + mNetwork +
                " valid=" + mValidated + " reachable=" + mIpReachable + " roaming=" + mRoaming +
                " strength=" + mSignalStrength);
    }

    @VisibleForTesting
    protected void setValidated(boolean validated) {
        mValidated = validated;
    }

    @VisibleForTesting
    protected void setDefault(boolean aDefault) {
        mDefault = aDefault;
    }

    @VisibleForTesting
    protected boolean getDefault() {
        return mDefault;
    }

    @VisibleForTesting
    protected void setIpReachable(boolean ipReachable) {
        mIpReachable = ipReachable;
    }

    @VisibleForTesting
    protected ConcurrentLinkedDeque<Integer> getSignalStrength() {
        return mSignalStrength;
    }

    @VisibleForTesting
    protected void setWiFiHandoverTimer(Timer wiFiHandoverTimer) {
        mWiFiHandoverTimer = wiFiHandoverTimer;
    }

    @VisibleForTesting
    protected void setWiFiState(int wiFiState) {
        mWiFiState = wiFiState;
    }

    @VisibleForTesting
    protected void setWiFiValidationRequired(boolean wiFiValidationRequired) {
        mWiFiValidationRequired = wiFiValidationRequired;
    }

    @VisibleForTesting
    protected void setRssiIn(HashMap<MapType, Integer> rssiIn) {
        mRssiIn = rssiIn;
    }

    @VisibleForTesting
    protected void setRssiOut(HashMap<MapType, Integer> rssiOut) {
        mRssiOut = rssiOut;
    }

    @VisibleForTesting
    protected void setNetwork(Network network) {
        mNetwork = network;
    }

    @VisibleForTesting
    protected void setLinkProperties(LinkProperties linkProperties) {
        mLinkProperties = linkProperties;
    }

    @VisibleForTesting
    protected void setAirplaneModeByTurning(boolean turnedOn) {
        if (turnedOn) {
            mAirplaneMode = AirplaneModeState.APM_TURNED_ON;
        } else {
            mAirplaneMode = AirplaneModeState.APM_TURNED_OFF;
        }
    }

    @VisibleForTesting
    protected void setIpReachableList(InetAddress[] ipReachableList) {
        mIpReachableList = ipReachableList;
    }

    @VisibleForTesting
    protected ConcurrentLinkedQueue<ConnectivityManager.NetworkCallback> getCallbackList() {
        return mCallbackList;
    }

    @VisibleForTesting
    protected void setPsState(NetworkRegistrationInfo nri) {
        mPsRegState = nri;
    }

    @VisibleForTesting
    protected WiFiBroadcastReceiver getBroadcastReceiver() {
        return mBroadcastReceiver;
    }

    @VisibleForTesting
    protected HandoverConditions.IConditionHelper getConditionsHelper() {
        return mConditionsHelper;
    }
}
