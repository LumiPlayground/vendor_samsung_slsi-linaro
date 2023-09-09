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

import static android.telephony.PhoneStateListener.LISTEN_NONE;
import static android.telephony.PhoneStateListener.LISTEN_PRECISE_DATA_CONNECTION_STATE;
import static android.telephony.PhoneStateListener.LISTEN_SERVICE_STATE;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.database.ContentObserver;
import android.net.Uri;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;
import android.os.Message;
import android.os.SystemClock;
import android.telephony.AccessNetworkConstants;
import android.telephony.DataFailCause;
import android.telephony.NetworkRegistrationInfo;
import android.telephony.PhoneStateListener;
import android.telephony.PreciseDataConnectionState;
import android.telephony.ServiceState;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
import android.telephony.data.ApnSetting;
import android.telephony.data.ThrottleStatus;
import android.text.TextUtils;
import android.util.SparseArray;

import com.android.internal.annotations.VisibleForTesting;
import com.shannon.qualifiednetworksservice.Transport.TransportType;
import com.shannon.qualifiednetworksservice.util.DeviceUtils;
import com.shannon.qualifiednetworksservice.util.LogUtils;

import java.lang.reflect.Method;
import java.util.HashMap;

public class ConditionConnectivity {

    private class ApnThrottleInfo {
        private final static long MARGIN = 25; //ms

        private final Long mTimeout;

        ApnThrottleInfo(ThrottleStatus status) {
            if ((status.getThrottleType() != ThrottleStatus.THROTTLE_TYPE_NONE) &&
                    (status.getRetryType() != ThrottleStatus.RETRY_TYPE_NONE)) {
                mTimeout = status.getThrottleExpiryTimeMillis();
            } else {
                mTimeout = (long)0;
            }
        }

        public long getTimeout() {
            if (mTimeout > 0) {
                long ret = (mTimeout - SystemClock.elapsedRealtime() - MARGIN);
                if (ret > 0) {
                    return ret;
                }
            }
            return 0;
        }

        public boolean isUnthrottled() { return (mTimeout == 0); }
    }

    protected class ConnectivityBroadcastReceiver extends BroadcastReceiver {
        @Override
        public void onReceive(Context context, Intent intent) {
            final String action = intent.getAction();
            if (action == null) {
                LogUtils.w(TAG, "NULL action for intent " + intent);
                return;
            }
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "Got intent " + intent);
            switch (action) {
                case Intent.ACTION_AIRPLANE_MODE_CHANGED: {
                    sendMessageToAllHandlers(ApnTransportHandler.MSG_AIRPLANE_MODE_CHANGED);
                    break;
                }
                case WifiManager.WIFI_STATE_CHANGED_ACTION: {
                    int currentState = intent.getIntExtra(WifiManager.EXTRA_WIFI_STATE, WifiManager.WIFI_STATE_UNKNOWN);
                    boolean state = (currentState == WifiManager.WIFI_STATE_ENABLED);
                    if (mWiFiState != state) {
                        LogUtils.d(TAG, LogUtils.LOG_CONDITIONS, "WiFi state changed");

                        mWiFiState = state;
                        sendMessageToAllHandlers(ApnTransportHandler.MSG_WIFI_STATE_CHANGED);
                    }
                    break;
                }
                case WifiManager.NETWORK_STATE_CHANGED_ACTION: {
                    if (mWiFiManager == null) {
                        break;
                    }
                    WifiInfo info = mWiFiManager.getConnectionInfo();
                    if (info != null) {
                        String ssid = info.getSSID();
                        if (!TextUtils.isEmpty(ssid)) {
                            if (TextUtils.isEmpty(mLastWiFiSsid)) {
                                mLastWiFiSsid = ssid;
                            } else if (!ssid.equals(mLastWiFiSsid)) {
                                LogUtils.i(TAG, LogUtils.LOG_CONDITIONS, "WiFi SSID changed");

                                mLastWiFiSsid = ssid;
                                sendMessageToAllHandlers(ApnTransportHandler.MSG_WIFI_ACCESS_POINT_CHANGED);
                            }
                        }
                    }
                    break;
                }
            }
        }
    }

    protected class ConnectivityStateListener extends PhoneStateListener {
        ConnectivityStateListener(int subId) {
            super();
            mSubId = subId;
        }

        @Override
        public void onServiceStateChanged(ServiceState serviceState) {
            if (serviceState == null) {
                return;
            }
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "onServiceStateChanged: " + serviceState);

            NetworkRegistrationInfo psState = serviceState.getNetworkRegistrationInfo(
                    NetworkRegistrationInfo.DOMAIN_PS, AccessNetworkConstants.TRANSPORT_TYPE_WWAN);
            if (psState == null) {
                return;
            }
            if (psState.isRegistered()) {
                int networkType = psState.getAccessNetworkTechnology();
                if (mLastNetworkType != networkType) {
                    LogUtils.d(TAG, LogUtils.LOG_CONDITIONS,
                            "RAT changed from " + TelephonyManager.getNetworkTypeName(mLastNetworkType) +
                            " to " + TelephonyManager.getNetworkTypeName(networkType));
                    mLastNetworkType = networkType;
                    sendMessageToAllHandlers(ApnTransportHandler.MSG_RAT_CHANGED);
                }

                String plmn = psState.getRegisteredPlmn();
                if (!TextUtils.isEmpty(plmn)) {
                    if (TextUtils.isEmpty(mLastRegisteredPlmn) || !plmn.equals(mLastRegisteredPlmn)) {
                        LogUtils.d(TAG, LogUtils.LOG_CONDITIONS,
                                "PLMN changed from " + mLastRegisteredPlmn + " to " + plmn);
                        mLastRegisteredPlmn = plmn;
                        sendMessageToAllHandlers(ApnTransportHandler.MSG_OPERATOR_CHANGED);
                    }
                }
            } else {
                mLastNetworkType = TelephonyManager.NETWORK_TYPE_UNKNOWN;
                mLastRegisteredPlmn = "";
            }
        }

        private int getTransportTypeFromDataConnectionState(PreciseDataConnectionState dataConnectionState) {
            try {
                Method method = PreciseDataConnectionState.class.getMethod(
                        "getTransportType", (Class<?>[]) null);
                return ((int) method.invoke(dataConnectionState, (Object[]) null));
            } catch (Exception e) {
                return AccessNetworkConstants.TRANSPORT_TYPE_INVALID;
            }
        }

        @Override
        public void onPreciseDataConnectionStateChanged(PreciseDataConnectionState dataConnectionState) {
            LogUtils.i(TAG, LogUtils.LOG_CONDITIONS, "onPreciseDataConnectionStateChanged " + dataConnectionState);
            if (dataConnectionState != null) {
                String apnName = dataConnectionState.getDataConnectionApn();
                int connectionState = dataConnectionState.getDataConnectionState();
                int networkType = dataConnectionState.getNetworkType();
                int transportType = getTransportTypeFromDataConnectionState(dataConnectionState);
                int bitmask = dataConnectionState.getDataConnectionApnTypeBitMask();
                do {
                    int apnType = ApnType.getSupportedApnType(bitmask);
                    if (apnType == ApnSetting.TYPE_NONE) {
                        return;
                    }
                    bitmask &= ~apnType;

                    switch (connectionState) {
                        case TelephonyManager.DATA_HANDOVER_IN_PROGRESS:
                        case TelephonyManager.DATA_CONNECTING: {
                            LogUtils.d(TAG, LogUtils.LOG_CONDITIONS,
                                    "APN: " + apnName + " TYPE: " + ApnSetting.getApnTypeString(apnType) +
                                    " connecting on " + TelephonyManager.getNetworkTypeName(networkType));
                            if (networkType != TelephonyManager.NETWORK_TYPE_UNKNOWN) {
                                ApnTransportHandler handler = mApnHandlers.get(apnType);
                                if (handler != null) {
                                    Message message = Message.obtain(handler.getHandler(),
                                            ApnTransportHandler.MSG_APN_CONNECTING);
                                    message.arg1 = networkType;
                                    message.sendToTarget();
                                }
                            }
                            break;
                        }
                        case TelephonyManager.DATA_CONNECTED: {
                            LogUtils.d(TAG, LogUtils.LOG_CONDITIONS,
                                    "APN: " + apnName + " TYPE: " + ApnSetting.getApnTypeString(apnType) +
                                    " connected on " + TelephonyManager.getNetworkTypeName(networkType));
                            if (DeviceUtils.isConnectionCallbackFixed() && (networkType != TelephonyManager.NETWORK_TYPE_UNKNOWN)) {
                                ApnTransportHandler handler = mApnHandlers.get(apnType);
                                if (handler != null) {
                                    Message message = Message.obtain(handler.getHandler(),
                                            ApnTransportHandler.MSG_APN_CONNECTED);
                                    message.arg1 = networkType;
                                    message.sendToTarget();
                                }
                            }
                            break;
                        }
                        case TelephonyManager.DATA_DISCONNECTED: {
                            int failCause = dataConnectionState.getDataConnectionFailCause();
                            LogUtils.d(TAG, LogUtils.LOG_CONDITIONS,
                                    "APN: " + apnName + " TYPE: " + ApnSetting.getApnTypeString(apnType) +
                                    " disconnected with " + DataFailCause.toString(failCause) +
                                    " on " + TelephonyManager.getNetworkTypeName(networkType));
                            if (!DeviceUtils.isConnectionCallbackFixed() && (failCause == DataFailCause.NONE)) {
                                continue;
                            }
                            ApnTransportHandler handler = mApnHandlers.get(apnType);
                            if (handler != null) {
                                if (apnType == ApnSetting.TYPE_EMERGENCY) {
                                    mConditionsHelper.clearLastCallFailCause(
                                            (networkType != TelephonyManager.NETWORK_TYPE_UNKNOWN) ?
                                            networkType : handler.getCurrentNetworkType());
                                }
                                /* transportType can be used in case networkType is UNKNOWN */
                                if (networkType == TelephonyManager.NETWORK_TYPE_UNKNOWN) {
                                    if (transportType == AccessNetworkConstants.TRANSPORT_TYPE_WWAN) {
                                        networkType = TelephonyManager.NETWORK_TYPE_LTE;
                                    } else if (transportType == AccessNetworkConstants.TRANSPORT_TYPE_WLAN) {
                                        networkType = TelephonyManager.NETWORK_TYPE_IWLAN;
                                    } else {
                                        int current = handler.getCurrentNetworkType();
                                        if (current == TelephonyManager.NETWORK_TYPE_IWLAN) {
                                            networkType = mConditionsHelper.getCellularNetworkType();
                                        } else if (current != TelephonyManager.NETWORK_TYPE_UNKNOWN) {
                                            networkType = TelephonyManager.NETWORK_TYPE_IWLAN;
                                        } else {
                                            continue;
                                        }
                                    }
                                }
                                Message message = Message.obtain(handler.getHandler(),
                                        ApnTransportHandler.MSG_APN_DISCONNECTED,
                                        new ApnTransportHandler.DisconnectInfo(networkType,
                                            failCause, getApnThrottleTimeout(apnType, networkType)));
                                message.sendToTarget();
                            }
                            break;
                        }
                        case TelephonyManager.DATA_UNKNOWN: {
                            int failCause = dataConnectionState.getDataConnectionFailCause();
                            LogUtils.d(TAG, LogUtils.LOG_CONDITIONS,
                                    "APN: " + apnName + " TYPE: " + ApnSetting.getApnTypeString(apnType) +
                                    " failed with " + DataFailCause.toString(failCause) + " on " + TelephonyManager.getNetworkTypeName(networkType));
                            if (failCause == DataFailCause.NONE) {
                                continue;
                            }
                            ApnTransportHandler handler = mApnHandlers.get(apnType);
                            if (handler != null) {
                                /* fix needed if network unknown */
                                if (networkType == TelephonyManager.NETWORK_TYPE_UNKNOWN) {
                                    int current = handler.getCurrentNetworkType();
                                    if (current == TelephonyManager.NETWORK_TYPE_IWLAN) {
                                        networkType = mConditionsHelper.getCellularNetworkType();
                                    } else if (current != TelephonyManager.NETWORK_TYPE_UNKNOWN) {
                                        networkType = TelephonyManager.NETWORK_TYPE_IWLAN;
                                    } else {
                                        networkType = handler.getConnectingNetworkType();
                                        if (networkType == TelephonyManager.NETWORK_TYPE_UNKNOWN) {
                                            continue;
                                        }
                                    }
                                }
                                Message message = Message.obtain(handler.getHandler(),
                                        ApnTransportHandler.MSG_APN_UNKNOWN,
                                        new ApnTransportHandler.DisconnectInfo(networkType,
                                            failCause, getApnThrottleTimeout(apnType, networkType)));
                                message.sendToTarget();
                            }
                            break;
                        }
                        default:;
                    }
                } while (bitmask != 0);
            }
        }
    }

    protected class WfcStateContentObserver extends ContentObserver {
        WfcStateContentObserver(Handler h) {
            super(h);
        }

        @Override
        public void onChange(boolean selfChange, Uri uri) {
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "onChange with " + uri);
            sendMessageToAllHandlers(ApnTransportHandler.MSG_WFC_STATE_CHANGED);
        }
      }

    private final String TAG;
    private final int mSlotId;
    private final Context mContext;
    private final HandoverConditions.IConditionHelper mConditionsHelper;
    private final SparseArray<ApnTransportHandler> mApnHandlers;
    private final ConnectivityBroadcastReceiver mBroadcastReceiver;
    private final TelephonyManager mTelephonyManager;
    private final ConnectivityStateListener mStateListener;
    private final WifiManager mWiFiManager;
    private final HandlerThread mHandlerThread;
    private final Handler mHandler;
    private final RilOemInterface mRilInterface;
    private final WfcStateContentObserver mContentObserver;
    private final SparseArray<HashMap<TransportType, ApnThrottleInfo>> mApnThrottleInformation;
    private int mLastNetworkType;
    private String mLastRegisteredPlmn;
    private boolean mWiFiState;
    private String mLastWiFiSsid;

    public ConditionConnectivity(int slotId, Context context, RilOemInterface rilOemInterface,
                                 ConfigurationManager configurationManager,
                                 HandoverConditions.IConditionHelper helper,
                                 HandoverConditions.IConditionChanged callback) {
        mSlotId = slotId;
        mContext = context;
        SimManager simManager = SimManager.getInstance();
        mTelephonyManager = simManager.getTelephonyManager(mSlotId);
        int subId = simManager.getSubId(mSlotId);
        mStateListener = new ConnectivityStateListener(subId);
        mConditionsHelper = helper;
        mWiFiManager = mContext.getSystemService(WifiManager.class);
        mRilInterface = rilOemInterface;

        mLastNetworkType = TelephonyManager.NETWORK_TYPE_UNKNOWN;
        mLastRegisteredPlmn = "";
        mWiFiState = getWiFiState();
        mLastWiFiSsid = "";
        if (mWiFiManager != null) {
            WifiInfo info = mWiFiManager.getConnectionInfo();
            if (info != null) {
                mLastWiFiSsid = info.getSSID();
            }
        }
        mApnThrottleInformation = new SparseArray<>();

        TAG = ShannonQualifiedNetworksService.getServiceTAG("-" + mSlotId + "-HC-cc");

        mApnHandlers = new SparseArray<>();
        for (ApnType type : ApnType.values()) {
            if (type.isSupported()) {
                int apnType = type.getType();
                ApnTransportHandler handler = new ApnTransportHandler(apnType, mSlotId,
                        mContext, configurationManager, helper, callback);
                mApnHandlers.put(apnType, handler);
            }
        }

        mHandlerThread = new HandlerThread(ConditionConnectivity.class.getSimpleName());
        mHandlerThread.start();
        mHandler = new Handler(mHandlerThread.getLooper()) {
            @Override
            public void handleMessage(Message message) {
                if (message.what == RilOemInterface.RIL_UNSOLICITED_MESSAGE) {
                    RilUnsolicitedMessage msg = (RilUnsolicitedMessage)message.obj;
                    switch (msg.getMessageId()) {
                        case RilUnsolicitedMessage.RIL_UNSOL_REGISTRATION: {
                            RilUnsolicitedRegistration registration = new RilUnsolicitedRegistration(msg);
                            LogUtils.d(TAG, LogUtils.LOG_CONDITIONS, "Received " + registration);
                            if (registration.isImsIndication()) {
                                ApnTransportHandler handler = mApnHandlers.get(ApnSetting.TYPE_IMS);
                                if (handler != null) {
                                    boolean registered = registration.isImsRegistered();
                                    if (registered) {
                                        Message newMsg = Message.obtain(handler.getHandler(),
                                                ApnTransportHandler.MSG_REGISTRATION_COMPLETED);
                                        newMsg.sendToTarget();
                                    } else {
                                        Message newMsg = Message.obtain(handler.getHandler(),
                                                ApnTransportHandler.MSG_REGISTRATION_FAILED);
                                        newMsg.arg1 = registration.getStatusCodeAsInt();
                                        newMsg.sendToTarget();
                                    }
                                }
                                break;
                            }
                            break;
                        }
                        default:;
                    }
                }
            }
        };

        mRilInterface.registerUnsolicitedListener(RilUnsolicitedMessage.RIL_UNSOL_REGISTRATION, mHandler);

        mBroadcastReceiver = new ConnectivityBroadcastReceiver();
        IntentFilter filter = new IntentFilter();
        filter.addAction(Intent.ACTION_AIRPLANE_MODE_CHANGED);
        filter.addAction(WifiManager.WIFI_STATE_CHANGED_ACTION);
        filter.addAction(WifiManager.NETWORK_STATE_CHANGED_ACTION);
        mContext.registerReceiver(mBroadcastReceiver, filter);

        String stringSubId = String.valueOf(subId);
        Uri uriWfcEnabled = Uri.withAppendedPath(SubscriptionManager.WFC_ENABLED_CONTENT_URI, stringSubId);
        mContentObserver = new WfcStateContentObserver(new Handler(Looper.getMainLooper()));
        mContext.getContentResolver().registerContentObserver(uriWfcEnabled, true, mContentObserver);

        mTelephonyManager.listen(mStateListener, LISTEN_PRECISE_DATA_CONNECTION_STATE | LISTEN_SERVICE_STATE);
    }

    public void destroy() {
        mRilInterface.unregisterUnsolicitedListener(RilUnsolicitedMessage.RIL_UNSOL_REGISTRATION);
        mContext.getContentResolver().unregisterContentObserver(mContentObserver);
        mContext.unregisterReceiver(mBroadcastReceiver);
        mTelephonyManager.listen(mStateListener, LISTEN_NONE);
        for (int i = 0; i < mApnHandlers.size(); i++) {
            ApnTransportHandler handler = mApnHandlers.valueAt(i);
            handler.destroy();
        }
        mApnHandlers.clear();
        mHandlerThread.quit();
    }

    public boolean isTransportAvailableForInitial(int apnType, TransportType transportType) {
        ApnTransportHandler handler = mApnHandlers.get(apnType);
        if (handler != null) {
            boolean result = handler.isTransportAvailableForInitial(transportType);
            LogUtils.v(TAG, apnType,
                    "Initial Transport " + transportType + " for " + ApnSetting.getApnTypeString(apnType) +
                            " is " + (result ? "available" : "not available"));
            return result;
        }
        return false;
    }

    public boolean isTransportAvailable(int apnType, TransportType transportType) {
        ApnTransportHandler handler = mApnHandlers.get(apnType);
        if (handler != null) {
            boolean result = handler.isTransportAvailable(transportType);
            LogUtils.v(TAG, apnType,
                    "Transport " + transportType + " for " + ApnSetting.getApnTypeString(apnType) +
                    " is " + (result ? "available" : "not available"));
            return result;
        }
        return false;
    }

    public Transport getCurrentTransport(int apnType) {
        ApnTransportHandler handler = mApnHandlers.get(apnType);
        if (handler != null) {
            return handler.getCurrentTransport();
        }
        return null;
    }

    public boolean isHandoverFailed(int apnType, TransportType transportType) {
        ApnTransportHandler handler = mApnHandlers.get(apnType);
        if (handler != null) {
            return handler.isHandoverFailed(transportType);
        }
        return false;
    }

    public int getNetworkType(int apnType) {
        ApnTransportHandler handler = mApnHandlers.get(apnType);
        if (handler != null) {
            return handler.getCurrentNetworkType();
        }
        return TelephonyManager.NETWORK_TYPE_UNKNOWN;
    }

    public boolean isWiFiTransportUsed() {
        for (int i = 0; i < mApnHandlers.size(); i++) {
            ApnTransportHandler handler = mApnHandlers.valueAt(i);
            if (handler.getCurrentNetworkType() == TelephonyManager.NETWORK_TYPE_IWLAN) {
                return true;
            }
        }
        return false;
    }

    private void sendMessageToAllHandlers(int what) {
        for (int i = 0; i < mApnHandlers.size(); i++) {
            ApnTransportHandler handler = mApnHandlers.valueAt(i);

            Message message = Message.obtain(handler.getHandler(), what);
            message.sendToTarget();
        }
    }

    private boolean getWiFiState() {
        boolean result = false;
        try {
            result = (mWiFiManager.getWifiState() == WifiManager.WIFI_STATE_ENABLED);
        } catch (Exception e) {
            LogUtils.e(TAG, "Failed to read WiFi state: " + e);
        }
        return result;
    }

    public void onConfigurationChanged() {
        sendMessageToAllHandlers(ApnTransportHandler.MSG_CARRIER_CONFIG_CHANGED);
    }

    public void onThrottleStatusChanged(ThrottleStatus status) {
        LogUtils.v(TAG, "onThrottleStatusChanged: " + status);
        int apnType = status.getApnType();
        HashMap<TransportType, ApnThrottleInfo> map = mApnThrottleInformation.get(apnType);
        if (map == null) {
            map = new HashMap<>();
        }
        TransportType transportType = TransportType.TRANSPORT_UNKNOWN;
        switch (status.getTransportType()) {
            case AccessNetworkConstants.TRANSPORT_TYPE_WLAN:
                transportType = TransportType.TRANSPORT_WIFI;
                break;
            case AccessNetworkConstants.TRANSPORT_TYPE_WWAN:
                transportType = TransportType.TRANSPORT_CELLULAR;
                break;
            default:
                return;
        }
        ApnThrottleInfo info = new ApnThrottleInfo(status);
        if (info.isUnthrottled()) {
            ApnTransportHandler handler = mApnHandlers.get(apnType);
            if (handler != null) {
                Message message = Message.obtain(handler.getHandler(),
                        ApnTransportHandler.MSG_APN_UNTHROTTLED, transportType);
                message.sendToTarget();
            }
        } else {
            map.put(transportType, info);
            mApnThrottleInformation.put(apnType, map);
        }
    }

    private long getApnThrottleTimeout(int apnType, int networkType) {
        HashMap<TransportType, ApnThrottleInfo> map = mApnThrottleInformation.get(apnType);
        if (map != null) {
            TransportType transportType = TransportType.TRANSPORT_CELLULAR;
            switch (networkType) {
                case TelephonyManager.NETWORK_TYPE_IWLAN:
                    transportType = TransportType.TRANSPORT_WIFI;
                    break;
                case TelephonyManager.NETWORK_TYPE_UNKNOWN:
                    return 0;
                default:;
            }
            ApnThrottleInfo info = map.remove(transportType);
            if (info != null) {
                long timeout = info.getTimeout();
                LogUtils.v(TAG, "Throttling timeout: " + timeout +
                        " for APN: " + ApnSetting.getApnTypeString(apnType) +
                        ", networkType: " + TelephonyManager.getNetworkTypeName(networkType));
                return timeout;
            }
        }
        return 0;
    }

    @VisibleForTesting
    protected void setLastRegisteredPlmn(String lastRegisteredPlmn) {
        mLastRegisteredPlmn = lastRegisteredPlmn;
    }

    @VisibleForTesting
    protected void setLastNetworkType(int lastNetworkType) {
        mLastNetworkType = lastNetworkType;
    }

    @VisibleForTesting
    protected Handler getHandler() { return mHandler; }

    @VisibleForTesting
    protected SparseArray getApnThrottleInformation() {
        return mApnThrottleInformation;
    }
}
