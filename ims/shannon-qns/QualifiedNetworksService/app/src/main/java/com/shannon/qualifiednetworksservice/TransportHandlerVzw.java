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

import com.shannon.qualifiednetworksservice.Transport.TransportType;
import com.shannon.qualifiednetworksservice.util.LogUtils;

import android.telephony.DataFailCause;

import java.lang.reflect.Field;
import java.util.Random;

public class TransportHandlerVzw extends VendorTransportHandler {
    private static final int TIMEOUT_15 = (15 * 1000);
    private static final int TIMEOUT_45 = (45 * 1000);
    private static final int TIMEOUT_1M = (60 * 1000);
    private static final int TIMEOUT_2M = (2 * 60 * 1000);
    private static final int TIMEOUT_8M = (8 * 60 * 1000);
    private static final int TIMEOUT_15M = (15 * 60 * 1000);
    private static final int TIMEOUT_1H = (60 * 60 * 1000);
    private static final int COUNT_3 = 3;
    private static final int COUNT_4 = 4;
    private static final int COUNT_5 = 5;

    private final boolean mAirplaneMode;
    private boolean mWiFiBlocked;

    public TransportHandlerVzw(boolean isAirplaneMode) {
        mAirplaneMode = isAirplaneMode;
        mWiFiBlocked = false;

        TAG = ShannonQualifiedNetworksService.getServiceTAG("-VTH-vzw");
    }

    @Override
    public void doInitialization(Transport transport) {
        if ((transport.getType() == TransportType.TRANSPORT_WIFI) && !mAirplaneMode) {
            /* block transport for 45 seconds */
            mWiFiBlocked = true;
            transport.getInitialState().blockTemporary(TIMEOUT_45);
            transport.getHandoverState().blockTemporary(TIMEOUT_45);
        }
    }

    private boolean isAuthenticationFailure(int cause) {
        return (cause == DataFailCause.IWLAN_IKEV2_AUTH_FAILURE) ||
                (cause == DataFailCause.USER_AUTHENTICATION);
    }

    private boolean processConnectionFailure(TransportState state, TransportType type, String string, int cause) {
        int count = state.getRetryCount();
        if (type == TransportType.TRANSPORT_WIFI) {
            boolean permanentError = false;

            switch (cause) {
                case DataFailCause.NETWORK_FAILURE: {
                    state.blockTemporary(TIMEOUT_1H);
                    LogUtils.w(TAG, string + type + " for 1 hour. Fail cause: " + DataFailCause.toString(cause));
                    return true;
                }
                case DataFailCause.SERVICE_OPTION_NOT_SUPPORTED:
                case DataFailCause.MISSING_UNKNOWN_APN:
                case DataFailCause.SERVICE_OPTION_NOT_SUBSCRIBED:
                case DataFailCause.APN_DISALLOWED_ON_ROAMING: {
                    permanentError = true;
                }
                default: {
                    if (permanentError || isAuthenticationFailure(cause)) {
                        if (count >= COUNT_3) {
                            state.block();
                            LogUtils.w(TAG, string + type + " permanently. Fail cause: " + DataFailCause.toString(cause));
                        }
                        return true;
                    }
                }
            }
        }
        if (count < COUNT_3) {
            /* no delay */
            return true;
        } else if (count == COUNT_3) {
            Random r = new Random();
            int retryTimeout = TIMEOUT_1M + r.nextInt(TIMEOUT_15);
            state.blockTemporary(retryTimeout);
            LogUtils.w(TAG, string + type + " for " +
                    retryTimeout + ". Fail cause: " + DataFailCause.toString(cause));
        } else if (count == COUNT_4) {
            state.blockTemporary(TIMEOUT_2M);
            LogUtils.w(TAG, string + type + " for " +
                    TIMEOUT_2M + ". Fail cause: " + DataFailCause.toString(cause));
        } else if (count == COUNT_5) {
            state.blockTemporary(TIMEOUT_8M);
            LogUtils.w(TAG, string + type + " for " +
                    TIMEOUT_8M + ". Fail cause: " + DataFailCause.toString(cause));
        } else {
            state.blockTemporary(TIMEOUT_15M);
            LogUtils.w(TAG, string + type + " for " +
                    TIMEOUT_15M + ". Fail cause: " + DataFailCause.toString(cause));
        }
        return true;
    }

    @Override
    public boolean onConnectionFailure(Transport transport, int cause, int timeout) {
        if (timeout > 0) {
            return false;
        }
        return processConnectionFailure(transport.getInitialState(), transport.getType(),
                "Block initial ", cause);
    }

    @Override
    public boolean onHandoverFailure(Transport transport, int cause, int timeout) {
        if (timeout > 0) {
            return false;
        }
        return processConnectionFailure(transport.getHandoverState(), transport.getType(),
                "Block handover ", cause);
    }

    @Override
    public boolean onWiFiAccessPointChanged(Transport transport) {
        if (transport.getType() == TransportType.TRANSPORT_WIFI) {
            /* reset throttling counters */
            transport.getInitialState().reset();
            transport.getHandoverState().reset();
        }
        return true;
    }

    @Override
    public boolean onRegistrationCompleted(Transport transport) {
        if (transport.getType() == TransportType.TRANSPORT_WIFI) {
            if (mWiFiBlocked) {
                mWiFiBlocked = false;

                TransportState initialState = transport.getInitialState();
                TransportState handoverState = transport.getHandoverState();
                if (initialState.isTemporaryBlocked() && handoverState.isTemporaryBlocked()) {
                    initialState.reset();
                    handoverState.reset();
                }
            }
        }
        return false;
    }
}
