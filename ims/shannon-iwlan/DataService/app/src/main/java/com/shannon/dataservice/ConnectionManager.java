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
import android.net.LinkProperties;
import android.net.Network;
import android.net.NetworkCapabilities;
import android.net.NetworkRequest;
import android.os.AsyncResult;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;
import android.os.Message;
import android.provider.Settings;
import android.telephony.DataFailCause;
import android.telephony.Rlog;
import android.telephony.data.ApnSetting;
import android.telephony.data.DataCallResponse;
import android.telephony.data.DataProfile;
import android.telephony.data.NetworkSliceInfo;
import android.util.SparseArray;

import com.android.net.module.util.LinkPropertiesUtils;

import com.android.internal.annotations.VisibleForTesting;

import java.nio.channels.NotYetConnectedException;
import java.util.ArrayList;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;

import static android.telephony.data.DataService.REQUEST_REASON_HANDOVER;

public class ConnectionManager {
    private static final boolean DBG = true;
    private static final int WIFI_DISABLED                      = 0;
    private static final int WIFI_ENABLED                       = 1;
    private static final int WIFI_ENABLED_AIRPLANE_OVERRIDE     = 2;
    private static final int WIFI_DISABLED_AIRPLANE_ON          = 3;

    private final String TAG;
    private final int mSlotId;
    private final Context mContext;
    private final RilOemInterface mRilInterface;
    private final ConnectionNetworkCallback mRequestCallback;
    private final ConnectivityManager mConnectivityManager;
    private final HandlerThread mHandlerThread;
    private final DataConnectionCallback mDataConnectionCallback;
    private final ConfigurationManager mConfiguration;
    private final RetryManager mRetryManager;
    private SparseArray<DataConnection> mPendingConnections;
    private SparseArray<DataConnection> mEstablishedConnections;
    private Network mWiFiNetwork;
    private LinkProperties mWiFiLinkProperties;
    private final Timer mTimer;
    private ReleaseTimerTask mReleaseTimerTask;
    private Message mDataCallListChangedMessage;

    private class ReleaseTimerTask extends TimerTask {
        public void run() {
            mReleaseTimerTask = null;
            updateEmptyDataCallListToFramework();
            deactivateEstablishedConnections();
        }
    }

    private void startReleaseTimer(int value) {
        if ((mReleaseTimerTask == null) && (value > 0)) {
            log("Start Release timer for " + value * 1000 + " ms");

            mReleaseTimerTask = new ReleaseTimerTask();
            mTimer.schedule(mReleaseTimerTask, value * 1000);
        }
    }

    protected class ConnectionNetworkCallback extends ConnectivityManager.NetworkCallback {
        @Override
        public void onLinkPropertiesChanged(Network network, LinkProperties linkProperties) {
            log("Network " + network + " available {LP:" + linkProperties + "}");
            mWiFiNetwork = network;
            if (mWiFiLinkProperties == null) {
                mWiFiLinkProperties = linkProperties;
                /* cancel release timer if wifi is connected again*/
                if (mReleaseTimerTask != null) {
                    mReleaseTimerTask.cancel();
                }
                /* update all */
                updateDataConnectionRoutes(network, linkProperties);
            } else if (!mWiFiLinkProperties.equals(linkProperties)) {
                /* update routes for new link addresses */
                if (!LinkPropertiesUtils.isIdenticalAddresses(
                        linkProperties, mWiFiLinkProperties)) {
                    updateDataConnectionRoutes(linkProperties);
                }
                mWiFiLinkProperties = linkProperties;
            }
        }

        @Override
        public void onLost(Network network) {
            // TODO: check that network is the same as mWiFiNetwork, or this code will be incorrect
            // in STA+STA scenarios where there is more than one wifi network connected.
            log("Network " + network + " lost");
            mWiFiNetwork = null;
            mWiFiLinkProperties = null;

            closePendingConnections();

            mRetryManager.onWifiDisconnected();

            MobileNetworkCallback callback = SimManager.getInstance().getMobileNetwork(mSlotId, mConfiguration);
            if (callback != null) {
                log("WiFi lost, try handover to VoMobile");
                updateDataConnectionRoutes(callback.getNetwork(), callback.getLinkProperties());
            }
            /*
             * Start release timer to deactivate all PDNs.
             * Note: WiFi AP HO should be considered only in case wifi button is enabled.
             */
             else if (mEstablishedConnections.size() > 0) {
                int timeout = 3;
                if (isWiFiEnabled()) {
                    timeout += mConfiguration.getWifiApHandoverTimeout();
                }
                startReleaseTimer(timeout);
            }
        }
    }

    private boolean isWiFiEnabled() {
        try {
            int wifiState = Settings.Global.getInt(mContext.getContentResolver(), Settings.Global.WIFI_ON);
            return (wifiState == WIFI_ENABLED) || (wifiState == WIFI_ENABLED_AIRPLANE_OVERRIDE);
        } catch (Exception e) {
            // nothing
        }
        return false;
    }

    protected class DataConnectionCallback implements DataConnection.ConnectionCallback {
        public void onConnectionCompleted(ApnType type, Message message, DataCallResponse response) {
            int key = type.getCode(mSlotId);

            DataConnection c = mPendingConnections.get(key);
            if (c != null) {
                log("Connection type " + type + " completed");
                mPendingConnections.remove(key);
                if (response.getCause() != DataFailCause.NONE) {
                    /* error, check timer */
                    if (response.getSuggestedRetryTime() > 0) {
                        log("Connection type " + type + " rejected with timer " + response.getSuggestedRetryTime());
                    }
                } else {
                    mEstablishedConnections.put(key, c);
                }
                if (message != null) {
                    AsyncResult.forMessage(message, response, null);
                    message.sendToTarget();
                }
                updateDataCallListToFramework();
            }
        }

        public void onConnectionTerminated(ApnType type, Message message) {
            int key = type.getCode(mSlotId);

            DataConnection c = mEstablishedConnections.get(key);
            if (c != null) {
                log("Connection type " + type + " terminated");
                mEstablishedConnections.remove(key);
            } else {
                c = mPendingConnections.get(key);
                if (c != null) {
                    log("Connection type " + type + " terminated during initialization");
                    mPendingConnections.remove(key);
                }
            }
            /* it was requested */
            if (message != null) {
                AsyncResult.forMessage(message);
                message.sendToTarget();
            }
            updateDataCallListToFramework();
        }

        public void onConnectionTerminated(ApnType type, Message message, StrokeTerminatedIndication.Cause cause) {
            if (cause == StrokeTerminatedIndication.Cause.CAUSE_TIMEOUT) {
                if (mWiFiNetwork != null) {
                    loge("Connection " + type + " terminated by timeout, re-evaluate network " + mWiFiNetwork);
                    mConnectivityManager.reportNetworkConnectivity(mWiFiNetwork, false);
                }
            }
            onConnectionTerminated(type, message);
        }
    }

    public ConnectionManager(int slotId, Context context, ConfigurationManager configuration) {
        mSlotId = slotId;
        mContext = context;
        mWiFiLinkProperties = null;
        mWiFiNetwork = null;
        mRilInterface = new RilOemInterface(mSlotId);
        mDataConnectionCallback = new DataConnectionCallback();
        mConnectivityManager = mContext.getSystemService(ConnectivityManager.class);
        mConfiguration = configuration;
        mRetryManager = new RetryManager(slotId, context, configuration);

        mPendingConnections = new SparseArray<>();
        mEstablishedConnections = new SparseArray<>();
        mTimer = new Timer();
        mReleaseTimerTask = null;

        mDataCallListChangedMessage = null;

        TAG = ShannonDataService.getServiceTAG("-CM-" + mSlotId);

        /* network request */
        NetworkRequest.Builder builder = new NetworkRequest.Builder();
        builder.addCapability(NetworkCapabilities.NET_CAPABILITY_INTERNET);
        builder.addCapability(NetworkCapabilities.NET_CAPABILITY_NOT_RESTRICTED);
        builder.addTransportType(NetworkCapabilities.TRANSPORT_WIFI);
        NetworkRequest request = builder.build();

        /* request handler */
        mHandlerThread = new HandlerThread(ConnectionManager.class.getSimpleName());
        mHandlerThread.start();
        Looper looper = mHandlerThread.getLooper();
        Handler handler = new Handler(looper);

        /* callback */
        mRequestCallback = new ConnectionNetworkCallback();
        try {
            mConnectivityManager.registerNetworkCallback(request, mRequestCallback, handler);
        } catch (Exception e) {
            loge("Failed to request network callback");
        }
    }

    public void destroy() {
        mTimer.cancel();
        mTimer.purge();
        mDataCallListChangedMessage = null;
        removeEstablishedConnections();
        removePendingConnections();
        mRilInterface.destroy();
        mRetryManager.destroy();
        mConnectivityManager.unregisterNetworkCallback(mRequestCallback);
        mHandlerThread.quit();
    }

    public void registerForDataCallListChanged(Message message) {
        mDataCallListChangedMessage = message;
    }

    public void registerForApnUnthrottled(Message message) {
        mRetryManager.registerForApnUnthrottled(message);
    }

    public void setupDataCall(DataProfile dataProfile, int reason, LinkProperties linkProperties,
                              int pduSessionId, NetworkSliceInfo sliceInfo, Message message) {
        ApnType apnType = ApnType.getByBitmask(dataProfile.getSupportedApnTypesBitmask());
        /* unsupported APN */
        if (apnType == null) {
            loge("Unknown APN requested with " + dataProfile.getSupportedApnTypesBitmask());
            if (message != null) {
                DataCallResponse r = mRetryManager.createFailureResponse(DataFailCause.MISSING_UNKNOWN_APN,
                        null, reason);
                AsyncResult.forMessage(message, r, null);
                message.sendToTarget();
            }
            return;
        }
        int key = apnType.getCode(mSlotId);
        /* currently pending */
        DataConnection c = mPendingConnections.get(key);
        if (c != null) {
            loge("APN " + apnType +  " activation already pended");
            if (message != null) {
                DataCallResponse r = mRetryManager.createFailureResponse(DataFailCause.MULTI_CONN_TO_SAME_PDN_NOT_ALLOWED,
                        apnType, reason);
                AsyncResult.forMessage(message, r, null);
                message.sendToTarget();
            }
            return;
        }
        /* backoff timer is ongoing */
        if (mRetryManager.isApnThrottled(apnType)) {
            loge("APN " + apnType +  " was rejected with backoff timer, timer not expired");
            if (message != null) {
                DataCallResponse r = mRetryManager.createFailureResponse(apnType, reason);
                AsyncResult.forMessage(message, r, null);
                message.sendToTarget();
            }
            return;
        }
        /* we do not support connection without SIM information */
        if (!SimManager.getInstance().isSimInformationAvailable(mSlotId)) {
            loge("SIM information is not available for slot " + mSlotId);
            if (message != null) {
                DataCallResponse r = mRetryManager.createFailureResponse(DataFailCause.SIM_CARD_CHANGED,
                        apnType, reason);
                AsyncResult.forMessage(message, r, null);
                message.sendToTarget();
            }
            return;
        }
        c = new DataConnection(mSlotId, mContext, apnType, dataProfile, reason, linkProperties,
                pduSessionId, sliceInfo, mEstablishedConnections.size() == 0,
                mRilInterface, mConfiguration, mRetryManager, mDataConnectionCallback);
        mPendingConnections.put(key, c);
        c.activate(mWiFiNetwork, mWiFiLinkProperties, message);
    }

    public void deactivateDataCall(int cid, int reason, Message message) {
        DataConnection c = mEstablishedConnections.get(cid);
        if (c == null) {
            log("No connection found with " + cid + " for " + mSlotId);
            if (message != null) {
                AsyncResult.forMessage(message, null, new NotYetConnectedException());
                message.sendToTarget();
            }
            return;
        }
        c.deactivate(message);
    }

    public void getDataCallList(Message message) {
        if (mEstablishedConnections.size() == 0) {
            log("No established connections found");
            if (message != null) {
                List<DataCallResponse> responseList = new ArrayList<DataCallResponse>();
                AsyncResult.forMessage(message, responseList, null);
                message.sendToTarget();
            }
            return;
        }
        if (message != null) {
            List<DataCallResponse> responseList = new ArrayList<DataCallResponse>();
            for (int i = 0; i < mEstablishedConnections.size(); i++) {
                DataConnection c = mEstablishedConnections.valueAt(i);
                responseList.add(c.getDataCallResponse());
            }
            AsyncResult.forMessage(message, responseList, null);
            message.sendToTarget();
        }
    }

    public void setPcoData(String apn, int pcoId, byte[] pcoData) {
        mRetryManager.setPcoData(apn, pcoId, pcoData);
    }

    private void updateDataCallListToFramework() {
        log("Update data call list for " + mSlotId);
        if (mDataCallListChangedMessage != null) {
            Message message = Message.obtain(mDataCallListChangedMessage);
            getDataCallList(message);
        }
    }

    private void updateEmptyDataCallListToFramework() {
        log("Update empty data call list for " + mSlotId);
        if ((mDataCallListChangedMessage != null) && (mEstablishedConnections.size() > 0)) {
            List<DataCallResponse> responseList = new ArrayList<DataCallResponse>();

            Message message = Message.obtain(mDataCallListChangedMessage);
            AsyncResult.forMessage(message, responseList, null);
            message.sendToTarget();
        }
    }

    private void removeEstablishedConnections() {
        for (int i = 0; i < mEstablishedConnections.size(); i++) {
            DataConnection c = mEstablishedConnections.valueAt(i);
            c.destroyEstablished();
        }
    }

    private void removePendingConnections() {
        for (int i = 0; i < mPendingConnections.size(); i++) {
            DataConnection c = mPendingConnections.valueAt(i);
            c.destroyPending();
        }
    }

    private void closePendingConnections() {
        for (int i = 0; i < mPendingConnections.size(); i++) {
            DataConnection c = mPendingConnections.valueAt(i);
            c.closePending();
        }
    }

    private void updateDataConnectionRoutes(LinkProperties linkProperties) {
        for (int i = 0; i < mEstablishedConnections.size(); i++) {
            DataConnection c = mEstablishedConnections.valueAt(i);
            c.updateLocalAddresses(linkProperties);
        }
    }

    private void updateDataConnectionRoutes(Network network, LinkProperties linkProperties) {
        for (int i = 0; i < mEstablishedConnections.size(); i++) {
            DataConnection c = mEstablishedConnections.valueAt(i);
            c.updateLocalAddresses(linkProperties);
            c.updateNetwork(network);
        }
    }

    private void deactivateEstablishedConnections() {
        for (int i = 0; i < mEstablishedConnections.size(); i++) {
            DataConnection c = mEstablishedConnections.valueAt(i);
            c.deactivate(null);
        }
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
    protected ConnectionNetworkCallback getRequestCallback() { return mRequestCallback; }

    @VisibleForTesting
    protected DataConnectionCallback getDataConnectionCallback() { return mDataConnectionCallback; }

    @VisibleForTesting
    protected SparseArray<DataConnection> getEstablishedConnections() { return mEstablishedConnections; }
}
