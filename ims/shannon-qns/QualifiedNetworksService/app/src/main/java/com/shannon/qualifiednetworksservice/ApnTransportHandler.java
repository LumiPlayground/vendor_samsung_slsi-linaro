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
import android.net.ConnectivityManager;
import android.net.LinkProperties;
import android.net.Network;
import android.net.NetworkCapabilities;
import android.net.NetworkRequest;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Message;
import android.telephony.DataFailCause;
import android.telephony.TelephonyManager;
import android.telephony.data.ApnSetting;

import com.android.internal.annotations.VisibleForTesting;
import com.shannon.qualifiednetworksservice.util.DeviceUtils;
import com.shannon.qualifiednetworksservice.Transport.TransportType;
import com.shannon.qualifiednetworksservice.util.LogUtils;

import java.util.HashMap;

import static com.shannon.qualifiednetworksservice.HandoverConditions.ConditionEvents.*;
import static com.shannon.qualifiednetworksservice.Transport.TransportType.TRANSPORT_CELLULAR;
import static com.shannon.qualifiednetworksservice.Transport.TransportType.TRANSPORT_UNKNOWN;
import static com.shannon.qualifiednetworksservice.Transport.TransportType.TRANSPORT_WIFI;

public class ApnTransportHandler {
    public static final int MSG_APN_CONNECTING = 1;
    public static final int MSG_APN_CONNECTED = 2;
    public static final int MSG_APN_DISCONNECTED = 3;
    public static final int MSG_APN_UNKNOWN = 4;
    public static final int MSG_AIRPLANE_MODE_CHANGED = 5;
    public static final int MSG_RAT_CHANGED = 6;
    public static final int MSG_OPERATOR_CHANGED = 7;
    public static final int MSG_WIFI_STATE_CHANGED = 8;
    public static final int MSG_CARRIER_CONFIG_CHANGED = 9;
    public static final int MSG_WIFI_ACCESS_POINT_CHANGED = 10;
    public static final int MSG_WFC_STATE_CHANGED = 11;
    public static final int MSG_REGISTRATION_FAILED = 12;
    public static final int MSG_APN_UNTHROTTLED = 13;
    public static final int MSG_REGISTRATION_COMPLETED = 14;

    public static class DisconnectInfo {
        private int mType;
        private int mCause;
        private int mTimeout;

        public DisconnectInfo(int type, int cause) {
            mType = type;
            mCause = cause;
            mTimeout = 0;
        }

        public DisconnectInfo(int type, int cause, long timeout) {
            mType = type;
            mCause = cause;
            mTimeout = (int)timeout;
        }

        public int getNetworkType() { return mType; }
        public int getCause() { return mCause; }
        public int getRetryTimeout() { return mTimeout; }
    }

    protected class NetworkCallback extends ConnectivityManager.NetworkCallback {
        private final ApnType mApnType;
        private final int mSubId;

        NetworkCallback(int type, int subId) {
            mApnType = ApnType.getEnum(type);
            mSubId = subId;
        }

        public NetworkRequest buildRequest() {
            NetworkRequest.Builder builder = new NetworkRequest.Builder();
            builder.addCapability(mApnType.getCapability());
            builder.addTransportType(NetworkCapabilities.TRANSPORT_CELLULAR);
            builder.setNetworkSpecifier(Integer.toString(mSubId));
            return builder.build();
        }

        private int getNetworkType(LinkProperties linkProperties) {
            int type = TelephonyManager.NETWORK_TYPE_UNKNOWN;
            if (linkProperties != null) {
                String interfaceName = linkProperties.getInterfaceName();
                if (interfaceName != null && !interfaceName.isEmpty()) {
                    if (interfaceName.startsWith(mConfigurationManager.getIwlanNetworkInterfaceName())) {
                        type = TelephonyManager.NETWORK_TYPE_IWLAN;
                    } else if (interfaceName.startsWith(mConfigurationManager.getCellularNetworkInterfaceName())) {
                        type = mConditionsHelper.getCellularNetworkType();
                    }
                }
            }
            return type;
        }

        @Override
        public void onLost(Network network) {
            int networkType = TelephonyManager.NETWORK_TYPE_UNKNOWN;
            if (mCurrentTransport != null) {
                networkType = mCurrentTransport.getNetworkType();
            }
            LogUtils.d(TAG, mType, "APN " + mApnType + " network " + network +
                            " disconnected on " + TelephonyManager.getNetworkTypeName(networkType));

            Message message = Message.obtain(mHandler, MSG_APN_DISCONNECTED,
                    new DisconnectInfo(networkType, DataFailCause.NONE));
            message.sendToTarget();
        }

        @Override
        public void onLinkPropertiesChanged(Network network, LinkProperties linkProperties) {
            int networkType = getNetworkType(linkProperties);

            LogUtils.d(TAG, mType, "APN " + mApnType + " connected on " + network +
                    ", RAT=" + TelephonyManager.getNetworkTypeName(networkType));

            Message message = Message.obtain(mHandler, MSG_APN_CONNECTED);
            message.arg1 = networkType;
            message.sendToTarget();
        }
    }

    protected class TransportStateChangedCallback implements TransportState.IStateChanged {
        private final TransportType mTransportType;

        public TransportStateChangedCallback(TransportType type) {
            mTransportType = type;
        }

        public void onUnblock() {
            LogUtils.i(TAG, mType, "Unblocked " + mTransports.get(mTransportType));
            mChangedCallback.onConditionChanged(mType, EVENT_APN_TRANSPORT_AVAILABLE);
        }
    }

    private final String TAG;
    private final int mType;
    private final int mSlotId;
    private final Handler mHandler;
    private final HandlerThread mHandlerThread;
    private final HashMap<TransportType, Transport> mTransports;
    private final HandoverConditions.IConditionChanged mChangedCallback;
    private final HandoverConditions.IConditionHelper mConditionsHelper;
    private final ConfigurationManager mConfigurationManager;
    private final ConnectivityManager mConnectivityManager;
    private final NetworkCallback mNetworkCallback;
    private Transport mCurrentTransport;

    public ApnTransportHandler(int type, int slotId, Context context,
                               ConfigurationManager configurationManager,
                               HandoverConditions.IConditionHelper helper,
                               HandoverConditions.IConditionChanged callback) {
        mType = type;
        mSlotId = slotId;
        mConditionsHelper = helper;
        mChangedCallback = callback;
        mConfigurationManager = configurationManager;
        mConnectivityManager = context.getSystemService(ConnectivityManager.class);

        SimManager simManager = SimManager.getInstance();
        int subId = simManager.getSubId(mSlotId);

        TAG = ShannonQualifiedNetworksService.getServiceTAG("-" + mSlotId + "-" + ApnSetting.getApnTypeString(mType) + "-th");

        mTransports = new HashMap<>();
        mTransports.put(TRANSPORT_CELLULAR,
                new TransportCellular(context, subId, mConfigurationManager, mConditionsHelper,
                        new TransportStateChangedCallback(TRANSPORT_CELLULAR), getVendorTransportHandler(simManager)));
        mTransports.put(TRANSPORT_WIFI,
                new TransportWiFi(context, subId, mConfigurationManager,
                        new TransportStateChangedCallback(TRANSPORT_WIFI), getVendorTransportHandler(simManager)));
        mCurrentTransport = null;

        mHandlerThread = new HandlerThread(ApnTransportHandler.class.getSimpleName());
        mHandlerThread.start();
        mHandler = new Handler(mHandlerThread.getLooper()) {
            @Override
            public void handleMessage(Message message) {
                processMessage(message);
            }
        };

        mNetworkCallback = new NetworkCallback(mType, subId);
        if (!DeviceUtils.isConnectionCallbackFixed()) {
            mConnectivityManager.registerNetworkCallback(mNetworkCallback.buildRequest(), mNetworkCallback);
        }
    }

    public void destroy() {
        if (!DeviceUtils.isConnectionCallbackFixed()) {
            mConnectivityManager.unregisterNetworkCallback(mNetworkCallback);
        }
        mHandlerThread.quitSafely();
    }

    public Handler getHandler() { return mHandler; }

    public boolean isTransportAvailableForInitial(TransportType type) {
        Transport transport = mTransports.get(type);
        if (transport == null) {
            return false;
        }
        return transport.isAvailable();
    }

    public boolean isTransportAvailable(TransportType type) {
        Transport transport = mTransports.get(type);
        if (transport == null) {
            return false;
        }
        if (mCurrentTransport != null) {
            if (mCurrentTransport.getType() != type) {
                return transport.isAvailableForHandover();
            }
            return true;
        }
        return transport.isAvailable();
    }

    public boolean isHandoverFailed(TransportType type) {
        Transport transport = mTransports.get(type);
        if (transport == null) {
            return false;
        }
        return transport.isHandoverFailed();
    }

    public int getCurrentNetworkType() {
        if (mCurrentTransport == null) {
            return TelephonyManager.NETWORK_TYPE_UNKNOWN;
        }
        return mCurrentTransport.getNetworkType();
    }

    public Transport getCurrentTransport() { return mCurrentTransport; }

    /* *
    * Fix for framework network type UNKNOWN issue
    * */
    public int getConnectingNetworkType() {
        Transport transportCell = mTransports.get(TRANSPORT_CELLULAR);
        Transport transportWiFi = mTransports.get(TRANSPORT_WIFI);
        if ((transportCell != null) && (transportWiFi != null)) {
            /* both connecting, we could not make decision */
            if (transportCell.isConnecting() && transportWiFi.isConnecting()) {
                return TelephonyManager.NETWORK_TYPE_UNKNOWN;
            }
            /* check one by one */
            if (transportCell.isConnecting()) {
                return transportCell.getNetworkType();
            }
            if (transportWiFi.isConnecting()) {
                return TelephonyManager.NETWORK_TYPE_IWLAN;
            }
        }
        return TelephonyManager.NETWORK_TYPE_UNKNOWN;
    }

    private TransportType getTransportType(int networkType) {
        if (networkType == TelephonyManager.NETWORK_TYPE_IWLAN) {
            return TRANSPORT_WIFI;
        }
        if (networkType != TelephonyManager.NETWORK_TYPE_UNKNOWN) {
            return TRANSPORT_CELLULAR;
        }
        return TRANSPORT_UNKNOWN;
    }

    private void processMessage(Message message) {
        switch (message.what) {
            case MSG_APN_CONNECTING: {
                TransportType type = getTransportType(message.arg1);
                if (type == TRANSPORT_UNKNOWN) {
                    return;
                }
                Transport transport = mTransports.get(type);
                if (transport == null) {
                    return;
                }
                if (mCurrentTransport != null) {
                    if (mCurrentTransport.getType() != type) {
                        transport.tryHandover();
                    }
                } else {
                    transport.tryConnecting();
                }
                LogUtils.d(TAG, mType, "Connecting to " + transport);
                mChangedCallback.onConditionChanged(mType, EVENT_APN_CONNECTING, transport.getType());
                break;
            }
            case MSG_APN_CONNECTED: {
                TransportType type = getTransportType(message.arg1);
                if (type == TRANSPORT_UNKNOWN) {
                    return;
                }
                Transport transport = mTransports.get(type);
                if (transport == null) {
                    return;
                }
                if (mCurrentTransport == null) {
                    LogUtils.d(TAG, mType, "Connected to " + transport);
                    transport.onConnected();
                    mCurrentTransport = transport;
                    if (mType == ApnSetting.TYPE_IMS) {
                        mConditionsHelper.setDisconnectedFromNetwork(false);
                    }
                    mChangedCallback.onConditionChanged(mType, EVENT_APN_CONNECTED, transport.getType());
                } else if (mCurrentTransport.getType() != type) {
                    LogUtils.d(TAG, mType, "Handover completed to " + transport);
                    transport.onHandoverCompleted();
                    mCurrentTransport = transport;
                    mChangedCallback.onConditionChanged(mType, EVENT_APN_HANDOVER_COMPLETED, transport.getType());
                } else if (mCurrentTransport.getType() == type) {
                    Transport otherTransport = mTransports.get(
                            (type == TRANSPORT_CELLULAR) ? (TRANSPORT_WIFI) : (TRANSPORT_CELLULAR));
                    LogUtils.d(TAG, mType, "Handover failed to " + otherTransport);
                    otherTransport.onHandoverFailure(DataFailCause.UNKNOWN, 0);
                    mChangedCallback.onConditionChanged(mType, EVENT_APN_DISCONNECTED, otherTransport);
                }
                if (mType == ApnSetting.TYPE_IMS) {
                    mChangedCallback.onConditionChanged(ApnType.getApnBitmaskWithoutImsAndEmergency(), EVENT_APN_IMS_CONNECTION_CHANGED);
                }
                break;
            }
            case MSG_APN_DISCONNECTED: {
                DisconnectInfo info = (DisconnectInfo)message.obj;
                if (info == null) {
                    return;
                }
                TransportType type = getTransportType(info.getNetworkType());
                if (type == TRANSPORT_UNKNOWN) {
                    return;
                }
                Transport transport = mTransports.get(type);
                if (transport == null) {
                    return;
                }
                int cause = info.getCause();

                LogUtils.d(TAG, mType, "Disconnect with " + DataFailCause.toString(cause) + " received for " + transport);
                if (mCurrentTransport != null) {
                    if (mCurrentTransport.getType() == type) {
                        mCurrentTransport = null;
                        if ((mType == ApnSetting.TYPE_IMS) && (type == TRANSPORT_WIFI) &&
                                (cause == DataFailCause.NORMAL_RELEASE)) {
                            mConditionsHelper.setDisconnectedFromNetwork(true);
                        } else if (cause != DataFailCause.NONE) {
                            transport.onConnectionFailure(cause, info.getRetryTimeout());
                        }
                        /* APN disconnected, reset all retry values for all transports */
                        for (Transport t : mTransports.values()) {
                            t.onDisconnected();
                        }
                    } else {
                        if (cause != DataFailCause.NONE) {
                            transport.onHandoverFailure(cause, info.getRetryTimeout());
                        }
                    }
                } else {
                    if (cause != DataFailCause.NONE) {
                        if (mConditionsHelper.isInHandoverState(mType)) {
                            transport.onHandoverFailure(cause, info.getRetryTimeout());
                        } else {
                            transport.onConnectionFailure(cause, info.getRetryTimeout());
                        }
                    }
                }
                LogUtils.v(TAG, mType, "Disconnect processed for " + transport);
                mChangedCallback.onConditionChanged(mType, EVENT_APN_DISCONNECTED, transport);
                if (mType == ApnSetting.TYPE_IMS) {
                    mChangedCallback.onConditionChanged(ApnType.getApnBitmaskWithoutImsAndEmergency(), EVENT_APN_IMS_CONNECTION_CHANGED);
                }
                break;
            }
            case MSG_APN_UNKNOWN: {
                DisconnectInfo info = (DisconnectInfo)message.obj;
                if (info == null) {
                    return;
                }
                TransportType type = getTransportType(info.getNetworkType());
                if (type == TRANSPORT_UNKNOWN) {
                    return;
                }
                Transport transport = mTransports.get(type);
                if (transport == null) {
                    return;
                }
                LogUtils.d(TAG, mType, "Unknown state with " + DataFailCause.toString(info.getCause()) + " received for " + transport);
                if (mCurrentTransport != null) {
                    if (mCurrentTransport.getType() == type) {
                        /* should not be here */
                        LogUtils.w(TAG, "Current transport is same to failed");
                        mCurrentTransport = null;

                        transport.onConnectionFailure(info.getCause(), info.getRetryTimeout());
                    } else {
                        transport.onHandoverFailure(info.getCause(), info.getRetryTimeout());
                    }
                } else {
                    if (mConditionsHelper.isInHandoverState(mType)) {
                        transport.onHandoverFailure(info.getCause(), info.getRetryTimeout());
                    } else {
                        transport.onConnectionFailure(info.getCause(), info.getRetryTimeout());
                    }
                }
                LogUtils.v(TAG, mType, "Failure processed for " + transport);
                mChangedCallback.onConditionChanged(mType, EVENT_APN_FAILURE, transport);
                break;
            }
            case MSG_AIRPLANE_MODE_CHANGED: {
                LogUtils.v(TAG, mType, "Airplane mode changed");
                for (Transport transport : mTransports.values()) {
                    transport.onAirplaneModeChanged();
                }
                break;
            }
            case MSG_OPERATOR_CHANGED: {
                LogUtils.v(TAG, mType, "PLMN changed");
                for (Transport transport : mTransports.values()) {
                    transport.onOperatorChanged();
                }
                mChangedCallback.onConditionChanged(mType, EVENT_OPERATOR_CHANGED);
                break;
            }
            case MSG_RAT_CHANGED: {
                LogUtils.v(TAG, mType, "RAT changed");
                for (Transport transport : mTransports.values()) {
                    transport.onRatChanged();
                }
                mChangedCallback.onConditionChanged(mType, EVENT_RAT_CHANGED);
                break;
            }
            case MSG_WIFI_STATE_CHANGED: {
                LogUtils.v(TAG, mType, "WiFi button state changed");
                for (Transport transport : mTransports.values()) {
                    transport.onWiFiStateChanged();
                }
                break;
            }
            case MSG_WIFI_ACCESS_POINT_CHANGED: {
                LogUtils.v(TAG, mType, "WiFi Access Point changed");
                for (Transport transport : mTransports.values()) {
                    transport.onWiFiAccessPointChanged();
                }
                mChangedCallback.onConditionChanged(mType, EVENT_WIFI_AP_CHANGED);
                break;
            }
            case MSG_WFC_STATE_CHANGED: {
                LogUtils.v(TAG, mType, "WFC state changed");
                for (Transport transport : mTransports.values()) {
                    transport.onWfcStateChanged();
                }
                break;
            }
            case MSG_CARRIER_CONFIG_CHANGED: {
                LogUtils.v(TAG, mType, "Carrier config changed");
                for (Transport transport : mTransports.values()) {
                    transport.onCarrierConfigChanged();
                }
                break;
            }
            case MSG_REGISTRATION_FAILED: {
                LogUtils.v(TAG, mType, "Registration failed");
                if (mCurrentTransport != null) {
                    mCurrentTransport.onRegistrationFailed(message.arg1);
                }
                break;
            }
            case MSG_APN_UNTHROTTLED: {
                TransportType transportType = (TransportType)message.obj;
                LogUtils.d(TAG, mType, "Unthrottle APN for " + transportType);
                Transport transport = mTransports.get(transportType);
                if (transport != null) {
                    transport.onApnUnthrottled();
                }
                mChangedCallback.onConditionChanged(mType, EVENT_APN_UNTHROTTLED, transportType);
                break;
            }
            case MSG_REGISTRATION_COMPLETED: {
                LogUtils.v(TAG, mType, "Registration completed");
                for (Transport transport : mTransports.values()) {
                    transport.onRegistrationCompleted();
                }
                mChangedCallback.onConditionChanged(mType, EVENT_IMS_REGISTRATION_COMPLETED);
                break;
            }
            default:;
        }
    }

    @VisibleForTesting
    protected Transport.IVendorTransportHandler getVendorTransportHandler(SimManager simManager) {
        TelephonyManager tm = simManager.getTelephonyManager(mSlotId);

        CarrierType carrier = CarrierType.getEnum(tm);
        if (carrier != null) {
            if (carrier == CarrierType.CARRIER_TYPE_ORANGE) {
                return new TransportHandlerOrange(mType, tm);
            } else if (carrier == CarrierType.CARRIER_TYPE_SWISSCOM) {
                return new TransportHandlerSwisscom();
            } else if (carrier == CarrierType.CARRIER_TYPE_VZW) {
                return new TransportHandlerVzw(mConditionsHelper.isAirplaneMode());
            } else if (carrier == CarrierType.CARRIER_TYPE_ROGERS) {
                return new TransportHandlerRogers();
            } else if (carrier == CarrierType.CARRIER_TYPE_EE) {
                return new TransportHandlerEE();
            } else if (carrier == CarrierType.CARRIER_TYPE_TELSTRA) {
                return new TransportHandlerTelstra(mType);
            } else if (carrier == CarrierType.CARRIER_TYPE_TMO) {
                return new TransportHandlerTmo(mType, tm, mConfigurationManager);
            }
        }
        return null;
    }

    @VisibleForTesting
    protected HashMap<TransportType, Transport> getTransportsList() {
        return mTransports;
    }

    @VisibleForTesting
    protected void setCurrentTransport(Transport transport) {
        mCurrentTransport = transport;
    }
}
