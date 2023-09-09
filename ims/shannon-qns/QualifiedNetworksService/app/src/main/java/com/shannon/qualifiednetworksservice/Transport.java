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
import android.telephony.DataFailCause;
import android.telephony.TelephonyManager;

import com.shannon.qualifiednetworksservice.util.DeviceUtils;
import com.shannon.qualifiednetworksservice.util.LogUtils;

public class Transport {
    public enum TransportType {
        TRANSPORT_UNKNOWN,
        TRANSPORT_CELLULAR,
        TRANSPORT_WIFI
    }

    public interface IVendorTransportHandler {
        /* initialization */
        public void doInitialization(Transport transport);

        /* state machine work */
        public void tryConnecting(Transport transport);
        public void tryHandover(Transport transport);
        public void onConnected(Transport transport);
        public void onHandoverCompleted(Transport transport);
        public void onDisconnected(Transport transport);

        /* error handling */
        public boolean onConnectionFailure(Transport transport, int cause, int timeout);
        public boolean onHandoverFailure(Transport transport, int cause, int timeout);

        /* events */
        public boolean onAirplaneModeChanged(Transport transport);
        public boolean onOperatorChanged(Transport transport);
        public boolean onRatChanged(Transport transport);
        public boolean onWiFiStateChanged(Transport transport);
        public boolean onWiFiAccessPointChanged(Transport transport);
        public boolean onWfcStateChanged(Transport transport);
        public boolean onCarrierConfigChanged(Transport transport);
        public boolean onRegistrationFailed(Transport transport, int statusCode);
        public boolean onRegistrationCompleted(Transport transport);
    }

    private final TransportType mType;
    protected final TransportState mState;
    protected final TransportState mHandoverState;
    protected final Context mContext;
    protected final int mSubId;
    private final IVendorTransportHandler mVendorHandler;
    private final ConfigurationManager mConfigurationManager;
    private int mLastFailCause;
    private int mHandoverRetryTimeout;
    private final String TAG;

    public Transport(TransportType type, Context context, int subId,
                     ConfigurationManager configurationManager,
                     TransportState.IStateChanged callback,
                     IVendorTransportHandler vendorHandler) {
        mSubId = subId;
        mType = type;
        mContext = context;
        mState = new TransportState(callback);
        mHandoverState = new TransportState(callback);
        mVendorHandler = vendorHandler;
        mConfigurationManager = configurationManager;
        mLastFailCause = DataFailCause.NONE;
        mHandoverRetryTimeout = -1;

        TAG = ShannonQualifiedNetworksService.getServiceTAG("-transport");

        if (mVendorHandler != null) {
            mVendorHandler.doInitialization(this);
        }
    }

    public TransportType getType() { return mType; }
    public TransportState getInitialState() { return mState; }
    public TransportState getHandoverState() { return mHandoverState; }
    public boolean isAvailable() { return mState.isAvailable(); }
    public boolean isAvailableForHandover() { return mHandoverState.isAvailable(); }

    public void onAirplaneModeChanged() {
        boolean handled = false;
        if (mVendorHandler != null) {
            handled = mVendorHandler.onAirplaneModeChanged(this);
        }
        if (handled) {
            return;
        }
        mState.reset();
        mHandoverState.reset();
    }

    public void onApnUnthrottled() {
        if (mState.isTemporaryBlocked()) mState.reset();
        if (mHandoverState.isTemporaryBlocked()) mHandoverState.reset();
    }

    public void onRatChanged() {
        if (mVendorHandler != null) {
            mVendorHandler.onRatChanged(this);
        }
    }

    public void onOperatorChanged() {
        if (mVendorHandler != null) {
            mVendorHandler.onOperatorChanged(this);
        }
    }

    public void onWiFiStateChanged() {
        if (mVendorHandler != null) {
            mVendorHandler.onWiFiStateChanged(this);
        }
    }

    public void onWiFiAccessPointChanged() {
        if (mVendorHandler != null) {
            mVendorHandler.onWiFiAccessPointChanged(this);
        }
    }

    public void onWfcStateChanged() {
        if (mVendorHandler != null) {
            mVendorHandler.onWfcStateChanged(this);
        }
    }

    public void onCarrierConfigChanged() {
        if (mVendorHandler != null) {
            mVendorHandler.onCarrierConfigChanged(this);
        }
    }

    public void onConnectionFailure(int cause, int timeout) {
        mLastFailCause = cause;
        if (cause == DataFailCause.NONE) {
            return;
        }
        mState.onConnectingError();

        boolean handled = false;
        if (mVendorHandler != null) {
            handled = mVendorHandler.onConnectionFailure(this, cause, timeout);
        }
        if (handled) {
            return;
        }
        if (timeout > 0) {
            LogUtils.w(TAG, "onConnectionFailure timeout: " + timeout + " for " + mType);
            mState.blockTemporary(timeout);
        } else {
            int retryCount = 1;
            int retryTimeout = 0;

            if (DataFailCause.isPermanentFailure(mContext, cause, mSubId)) {
                if (mType == TransportType.TRANSPORT_CELLULAR) {
                    retryCount = mConfigurationManager.getCellularPermanentErrorRetryCount();
                    retryTimeout = mConfigurationManager.getCellularPermanentErrorRetryTimeout();
                } else if (mType == TransportType.TRANSPORT_WIFI) {
                    retryCount = mConfigurationManager.getIwlanPermanentErrorRetryCount();
                    retryTimeout = mConfigurationManager.getIwlanPermanentErrorRetryTimeout();
                }
                if ((mState.getRetryCount() < retryCount) && (retryTimeout > 0)) {
                    mState.blockTemporary(retryTimeout * 1000);
                } else {
                    /* block permanently */
                    mState.block();
                }
            } else {
                retryCount = mConfigurationManager.getInitialNonPermanentErrorRetryCount();
                if (mState.getRetryCount() >= retryCount) {
                    retryTimeout = mConfigurationManager.getInitialNonPermanentErrorRetryTimeout();
                    mState.blockTemporary(retryTimeout * 1000);
                }
            }
        }
    }

    public void onHandoverFailure(int cause, int timeout) {
        mLastFailCause = cause;
        if (cause == DataFailCause.NONE) {
            return;
        }
        if (!DeviceUtils.isConnectionCallbackFixed()) {
            mHandoverState.onConnecting();
        }
        mHandoverState.onConnectingError();
        mHandoverRetryTimeout = timeout;

        boolean handled = false;
        if (mVendorHandler != null) {
            handled = mVendorHandler.onHandoverFailure(this, cause, timeout);
        }
        if (handled) {
            return;
        }
        if (timeout > 0) {
            LogUtils.w(TAG, "onHandoverFailure timeout: " + timeout + " for " + mType);
            // framework will retry but we also need to block transport
            mHandoverState.blockTemporary(timeout);
        } else {
            int retryCount = 1;
            int retryTimeout = 0;

            if (DataFailCause.isPermanentFailure(mContext, cause, mSubId)) {
                if (mType == TransportType.TRANSPORT_CELLULAR) {
                    retryCount = mConfigurationManager.getCellularPermanentErrorRetryCount();
                    retryTimeout = mConfigurationManager.getCellularPermanentErrorHandoverRetryTimeout();
                } else if (mType == TransportType.TRANSPORT_WIFI) {
                    retryCount = mConfigurationManager.getIwlanPermanentErrorRetryCount();
                    retryTimeout = mConfigurationManager.getIwlanPermanentErrorHandoverRetryTimeout();
                }
                if ((mHandoverState.getRetryCount() < retryCount) && (retryTimeout > 0)) {
                    mHandoverState.blockTemporary(retryTimeout * 1000);
                } else {
                    /* block permanently */
                    mHandoverState.block();
                }
            } else {
                retryCount = mConfigurationManager.getHandoverNonPermanentErrorRetryCount();
                if (mHandoverState.getRetryCount() >= retryCount) {
                    retryTimeout = mConfigurationManager.getHandoverNonPermanentErrorRetryTimeout();
                    mHandoverState.blockTemporary(retryTimeout * 1000);
                }
            }
        }
    }

    public boolean isHandoverFailed() {
        return mHandoverState.isConnectionError();
    }

    public boolean isConnecting() {
        return mState.isConnecting();
    }

    public void tryConnecting() {
        mState.onConnecting();

        if (mVendorHandler != null) {
            mVendorHandler.tryConnecting(this);
        }
    }

    public void tryHandover() {
        mHandoverState.onConnecting();

        if (mVendorHandler != null) {
            mVendorHandler.tryHandover(this);
        }
    }

    public void onConnected() {
        mLastFailCause = DataFailCause.NONE;
        mState.onConnected();

        if (mVendorHandler != null) {
            mVendorHandler.onConnected(this);
        }
    }

    public void onHandoverCompleted() {
        mLastFailCause = DataFailCause.NONE;
        mHandoverState.onConnected();

        if (mVendorHandler != null) {
            mVendorHandler.onHandoverCompleted(this);
        }
    }

    public void onDisconnected() {
        mState.onDisconnected();
        mHandoverState.onDisconnected();

        if (mVendorHandler != null) {
            mVendorHandler.onDisconnected(this);
        }
    }

    public int getLastFailCause() { return mLastFailCause; }

    public void onRegistrationFailed(int statusCode) {
        if (mVendorHandler != null) {
            mVendorHandler.onRegistrationFailed(this, statusCode);
        }
    }

    public void onRegistrationCompleted() {
        if (mVendorHandler != null) {
            mVendorHandler.onRegistrationCompleted(this);
        }
    }

    public int getHandoverRetryTimeout() { return mHandoverRetryTimeout; }
    public int getNetworkType() { return TelephonyManager.NETWORK_TYPE_UNKNOWN; }
    public boolean isRoaming() { return false; }

    @Override
    public String toString() {
        return (mType.toString() + " " + mState + " handover " + mHandoverState);
    }
}
