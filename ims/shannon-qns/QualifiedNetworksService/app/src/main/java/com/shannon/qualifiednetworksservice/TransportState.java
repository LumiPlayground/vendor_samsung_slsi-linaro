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

import com.android.internal.annotations.VisibleForTesting;

public class TransportState {
    private enum State {
        TRANSPORT_AVAILABLE,
        TRANSPORT_TEMPORARY_BLOCKED,
        TRANSPORT_PERMANENTLY_BLOCKED
    }

    private enum ConnectionState {
        DISCONNECTED,
        CONNECTING,
        CONNECTED
    }

    public interface IStateChanged {
        public void onUnblock();
    }

    private State mState;
    private ConnectionState mConnectionState;
    private int mRetryCount;
    private int mRetryErrorCount;
    protected AlarmTimer mTimer;
    private final IStateChanged mStateChangedCallback;

    public TransportState(IStateChanged callback) {
        mState = State.TRANSPORT_AVAILABLE;
        mConnectionState = ConnectionState.DISCONNECTED;
        mRetryCount = 0;
        mRetryErrorCount = 0;
        mTimer = null;
        mStateChangedCallback = callback;
    }

    public boolean isAvailable() {
        return (mState == State.TRANSPORT_AVAILABLE);
    }
    public boolean isTemporaryBlocked() {
        return (mState == State.TRANSPORT_TEMPORARY_BLOCKED);
    }
    public int getRetryCount() {
        return mRetryCount;
    }
    public boolean isConnecting() { return (mConnectionState == ConnectionState.CONNECTING); }

    public void reset() {
        mState = State.TRANSPORT_AVAILABLE;
        if (mTimer != null) {
            mTimer.cancel();
            mTimer = null;
        }
        mRetryCount = 0;
        mRetryErrorCount = 0;
    }

    public void block() {
        mState = State.TRANSPORT_PERMANENTLY_BLOCKED;
        mConnectionState = ConnectionState.DISCONNECTED;
        mRetryCount = 0;
        mRetryErrorCount = 0;
    }

    public void blockTemporary(int timeout) {
        if (timeout > 0) {
            if (mTimer != null) {
                mTimer.cancel();
            }
            Runnable task = new Runnable() {
                @Override
                public void run() {
                    mTimer = null;
                    mRetryCount = 0;
                    mRetryErrorCount = 0;
                    mState = State.TRANSPORT_AVAILABLE;
                    mStateChangedCallback.onUnblock();
                }
            };
            mState = State.TRANSPORT_TEMPORARY_BLOCKED;
            mConnectionState = ConnectionState.DISCONNECTED;
            mTimer = new AlarmTimer();
            mTimer.schedule(task, timeout);
        }
    }

    public void onConnecting() {
        mRetryCount++;
        mConnectionState = ConnectionState.CONNECTING;
    }

    public void onConnectingError() {
        mRetryErrorCount++;
        mConnectionState = ConnectionState.DISCONNECTED;
    }

    public void onConnected() {
        reset();
        mConnectionState = ConnectionState.CONNECTED;
    }

    public void onDisconnected() {
        mRetryCount = 0;
        mRetryErrorCount = 0;
        mConnectionState = ConnectionState.DISCONNECTED;
    }

    public boolean isConnectionError() {
        return ((mRetryErrorCount > 0) && (mRetryCount == mRetryErrorCount));
    }

    @Override
    public String toString() {
        String state;
        switch (mState) {
            case TRANSPORT_TEMPORARY_BLOCKED: state = "temporary_blocked"; break;
            case TRANSPORT_PERMANENTLY_BLOCKED: state = "permanently_blocked"; break;
            default:
                state = "available";
        }
        return ("state = " + state + " retry count = " + mRetryCount + " connection state = " + mConnectionState);
    }

    @VisibleForTesting
    int getRetryErrorCount() {
        return mRetryErrorCount;
    }
}
