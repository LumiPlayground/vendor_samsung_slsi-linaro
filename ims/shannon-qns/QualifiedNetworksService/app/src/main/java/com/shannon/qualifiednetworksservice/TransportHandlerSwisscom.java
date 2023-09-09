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

import android.telephony.DataFailCause;

import com.android.internal.annotations.VisibleForTesting;
import com.shannon.qualifiednetworksservice.Transport.TransportType;
import com.shannon.qualifiednetworksservice.util.LogUtils;

import java.util.Random;

public class TransportHandlerSwisscom extends VendorTransportHandler {
    private static final int TIMEOUT_5 = (5 * 1000);
    private static final int TIMEOUT_30 = (30 * 1000);
    private static final int RETRY_IN_STATE = 3;
    private static final int TIMEOUT_1_2 = (5 * 60 * 1000);
    private static final int TIMEOUT_2_3 = (15 * 60 * 1000);
    private static final int TIMEOUT_3_4 = (60 * 60 * 1000);
    private static final int TIMEOUT_4_5 = (24 * 60 * 60 * 1000);

    protected enum State {
        STATE_RETRY_1,
        STATE_RETRY_2,
        STATE_RETRY_3,
        STATE_RETRY_4,
        STATE_RETRY_5,
        STATE_NONE
    }

    private State mState;
    private State mHandoverState;

    public TransportHandlerSwisscom() {
        mState = State.STATE_NONE;
        mHandoverState = State.STATE_NONE;

        TAG = ShannonQualifiedNetworksService.getServiceTAG("-VTH-swisscom");
    }

    @Override
    public void onConnected(Transport transport) { mState = State.STATE_NONE; }

    @Override
    public void onHandoverCompleted(Transport transport) { mHandoverState = State.STATE_NONE; }

    @Override
    public boolean onAirplaneModeChanged(Transport transport) {
        mState = State.STATE_NONE;
        mHandoverState = State.STATE_NONE;
        return false;
    }

    private int processState(TransportState transportState, State nextState, int interStateTimeout) {
        int retryTimeout;
        if (transportState.getRetryCount() < RETRY_IN_STATE) {
            Random r = new Random();
            retryTimeout = r.nextInt(TIMEOUT_30 - TIMEOUT_5) + TIMEOUT_5;
        } else {
            mState = nextState;
            transportState.onDisconnected();
            retryTimeout = interStateTimeout;
        }
        transportState.blockTemporary(retryTimeout);
        return retryTimeout;
    }

    private int processHandoverState(TransportState transportState, State nextState, int interStateTimeout) {
        int retryTimeout;
        if (transportState.getRetryCount() < RETRY_IN_STATE) {
            Random r = new Random();
            retryTimeout = r.nextInt(TIMEOUT_30 - TIMEOUT_5) + TIMEOUT_5;
        } else {
            mHandoverState = nextState;
            transportState.onDisconnected();
            retryTimeout = interStateTimeout;
        }
        transportState.blockTemporary(retryTimeout);
        return retryTimeout;
    }

    @Override
    public boolean onConnectionFailure(Transport transport, int cause, int timeout) {
        if (transport.getType() == TransportType.TRANSPORT_WIFI) {
            if (timeout > 0) {
                return false;
            }
            TransportState state = transport.getInitialState();
            if (cause == DataFailCause.NO_RESPONSE_FROM_BASE_STATION) {
                if (mState == State.STATE_NONE) {
                    mState = State.STATE_RETRY_1;
                }
                switch (mState) {
                    case STATE_RETRY_2: {
                        LogUtils.w(TAG, "Block initial " + transport.getType() + " for " +
                                processState(state, State.STATE_RETRY_3, TIMEOUT_2_3) +
                                ". Fail cause: " + DataFailCause.toString(cause));
                        break;
                    }
                    case STATE_RETRY_3: {
                        LogUtils.w(TAG, "Block initial " + transport.getType() + " for " +
                                processState(state, State.STATE_RETRY_4, TIMEOUT_3_4) +
                                ". Fail cause: " + DataFailCause.toString(cause));
                        break;
                    }
                    case STATE_RETRY_4: {
                        LogUtils.w(TAG, "Block initial " + transport.getType() + " for " +
                                processState(state, State.STATE_RETRY_4, TIMEOUT_4_5) +
                                ". Fail cause: " + DataFailCause.toString(cause));
                        break;
                    }
                    case STATE_RETRY_5: {
                        if (state.getRetryCount() < RETRY_IN_STATE) {
                            Random r = new Random();
                            int retryTimeout = r.nextInt(TIMEOUT_30 - TIMEOUT_5) + TIMEOUT_5;
                            state.blockTemporary(retryTimeout);
                            LogUtils.w(TAG, "Block initial " + transport.getType() + " for " + retryTimeout +
                                    ". Fail cause: " + DataFailCause.toString(cause));
                        } else {
                            mState = State.STATE_NONE;
                            state.block();
                            LogUtils.w(TAG, "Block initial " + transport.getType() + " permanently " +
                                    ". Fail cause: " + DataFailCause.toString(cause));
                        }
                        break;
                    }
                    case STATE_RETRY_1:
                    default: {
                        LogUtils.w(TAG, "Block initial " + transport.getType() + " for " +
                                processState(state, State.STATE_RETRY_2, TIMEOUT_1_2) +
                                ". Fail cause: " + DataFailCause.toString(cause));
                    }
                }
            } else {
                int retryCount = state.getRetryCount();
                if (retryCount < 1) {
                    retryCount = 1;
                } else if (retryCount > 12) {
                    retryCount = 12;
                }
                int retryTimeout =  (1 << (retryCount - 1)) * TIMEOUT_30;
                state.blockTemporary(retryTimeout);
                LogUtils.w(TAG, "Block initial " + transport.getType() + " for " + retryTimeout + ". Fail cause: " + DataFailCause.toString(cause));
            }
            return true;
        }
        return false;
    }

    @Override
    public boolean onHandoverFailure(Transport transport, int cause, int timeout) {
        if (transport.getType() == TransportType.TRANSPORT_WIFI) {
            if (timeout > 0) {
                return false;
            }
            TransportState state = transport.getHandoverState();
            if (cause == DataFailCause.NO_RESPONSE_FROM_BASE_STATION) {
                if (mHandoverState == State.STATE_NONE) {
                    mHandoverState = State.STATE_RETRY_1;
                }
                switch (mHandoverState) {
                    case STATE_RETRY_2: {
                        LogUtils.w(TAG, "Block handover " + transport.getType() + " for " +
                                processHandoverState(state, State.STATE_RETRY_3, TIMEOUT_2_3) +
                                ". Fail cause: " + DataFailCause.toString(cause));
                        break;
                    }
                    case STATE_RETRY_3: {
                        LogUtils.w(TAG, "Block handover " + transport.getType() + " for " +
                                processHandoverState(state, State.STATE_RETRY_4, TIMEOUT_3_4) +
                                ". Fail cause: " + DataFailCause.toString(cause));
                        break;
                    }
                    case STATE_RETRY_4: {
                        LogUtils.w(TAG, "Block handover " + transport.getType() + " for " +
                                processHandoverState(state, State.STATE_RETRY_5, TIMEOUT_4_5) +
                                ". Fail cause: " + DataFailCause.toString(cause));
                        break;
                    }
                    case STATE_RETRY_5: {
                        if (state.getRetryCount() < RETRY_IN_STATE) {
                            Random r = new Random();
                            int retryTimeout = r.nextInt(TIMEOUT_30 - TIMEOUT_5) + TIMEOUT_5;
                            state.blockTemporary(retryTimeout);
                            LogUtils.w(TAG, "Block handover " + transport.getType() + " for " + retryTimeout +
                                    ". Fail cause: " + DataFailCause.toString(cause));
                        } else {
                            mHandoverState = State.STATE_NONE;
                            state.block();
                            LogUtils.w(TAG, "Block handover " + transport.getType() + " permanently " +
                                    ". Fail cause: " + DataFailCause.toString(cause));
                        }
                        break;
                    }
                    case STATE_RETRY_1:
                    default: {
                        LogUtils.w(TAG, "Block handover " + transport.getType() + " for " +
                                processHandoverState(state, State.STATE_RETRY_2, TIMEOUT_1_2) +
                                ". Fail cause: " + DataFailCause.toString(cause));
                    }
                }
            } else {
                int retryCount = state.getRetryCount();
                if (retryCount < 1) {
                    retryCount = 1;
                } else if (retryCount > 12) {
                    retryCount = 12;
                }
                int retryTimeout =  (1 << (retryCount - 1)) * TIMEOUT_30;
                state.blockTemporary(retryTimeout);
                LogUtils.w(TAG, "Block handover " + transport.getType() + " for " + retryTimeout + ". Fail cause: " + DataFailCause.toString(cause));
            }
            return true;
        }
        return false;
    }

    @VisibleForTesting
    protected State getState() {
        return mState;
    }

    @VisibleForTesting
    protected State getHandoverState() {
        return mHandoverState;
    }

    @VisibleForTesting
    protected void setState(State state) {
        mState = state;
    }

    @VisibleForTesting
    protected void setHandoverState(State handoverState) {
        mHandoverState = handoverState;
    }
}
