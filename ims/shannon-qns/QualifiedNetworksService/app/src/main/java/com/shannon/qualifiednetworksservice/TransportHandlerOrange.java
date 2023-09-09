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
import android.telephony.TelephonyManager;
import android.telephony.data.ApnSetting;

import com.android.internal.annotations.VisibleForTesting;
import com.shannon.qualifiednetworksservice.Transport.TransportType;
import com.shannon.qualifiednetworksservice.util.LogUtils;

public class TransportHandlerOrange extends VendorTransportHandler {
    private static final int TIMEOUT_12H = (12 * 60 * 60 * 1000);
    private static final int TIMEOUT_24H = (24 * 60 * 60 * 1000);
    private static final int MAX_TIMEOUT_1024 = 1024;
    private static final int MAX_TIMEOUT_256 = 256;
    private static final int MAX_TIMEOUT_16 = 16;
    private static final int DEFAULT_MAX_RETRY_COUNT = 3;
    private static final int DEFAULT_TIMEOUT = (30 * 1000);

    private final int mApnType;
    private final int mCarrierId;
    private int mLastFailCause;
    private int mLastHandoverFailCause;
    private boolean mBlockedUntilWiFiOn;
    private boolean mHandoverBlockedUntilWiFiOn;

    public TransportHandlerOrange(int type, TelephonyManager tm) {
        mApnType = type;
        mLastFailCause = DataFailCause.NONE;
        mLastHandoverFailCause = DataFailCause.NONE;
        mBlockedUntilWiFiOn = false;
        mHandoverBlockedUntilWiFiOn = false;

        int carrierId = tm.getSimSpecificCarrierId();
        if (carrierId == TelephonyManager.UNKNOWN_CARRIER_ID) {
            carrierId = tm.getSimCarrierId();
            if (carrierId == TelephonyManager.UNKNOWN_CARRIER_ID) {
                carrierId = tm.getCarrierIdFromSimMccMnc();
            }
        }
        mCarrierId = carrierId;

        TAG = ShannonQualifiedNetworksService.getServiceTAG("-VTH-orange");
    }

    @Override
    public void onConnected(Transport transport) {
        TransportType type = transport.getType();
        if (type == TransportType.TRANSPORT_CELLULAR) {
            mLastFailCause = DataFailCause.NONE;
        } else if (type == TransportType.TRANSPORT_WIFI) {
            mBlockedUntilWiFiOn = false;
        }
    }

    @Override
    public void onHandoverCompleted(Transport transport) {
        TransportType type = transport.getType();
        if (type == TransportType.TRANSPORT_CELLULAR) {
            mLastHandoverFailCause = DataFailCause.NONE;
        } else if (type == TransportType.TRANSPORT_WIFI) {
            mHandoverBlockedUntilWiFiOn = false;
        }
    }

    @Override
    public boolean onConnectionFailure(Transport transport, int cause, int timeout) {
        if (timeout > 0) {
            /* default will handle it */
            return false;
        }
        TransportType type = transport.getType();
        if (type == TransportType.TRANSPORT_CELLULAR) {
            if (!isImsApn()) {
                /* default will handle it */
                return false;
            }
            TransportState state = transport.getInitialState();

            mLastFailCause = cause;
            switch (cause) {
                case DataFailCause.ACTIVATION_REJECT_GGSN:
                case DataFailCause.ACTIVATION_REJECT_UNSPECIFIED:
                case DataFailCause.SERVICE_OPTION_OUT_OF_ORDER:
                case DataFailCause.NETWORK_FAILURE: {
                    int retryTimeout = getRetryTimeout(state, 1, MAX_TIMEOUT_1024);
                    state.blockTemporary(retryTimeout);
                    LogUtils.w(TAG, "Block initial " + transport.getType() + " for " + retryTimeout + ". Fail cause: " + DataFailCause.toString(cause));
                    break;
                }
                case DataFailCause.MISSING_UNKNOWN_APN:
                case DataFailCause.SERVICE_OPTION_NOT_SUBSCRIBED: {
                    state.blockTemporary(TIMEOUT_24H);
                    LogUtils.w(TAG, "Block initial " + transport.getType() + " for 24H. Fail cause: " + DataFailCause.toString(cause));
                    break;
                }
                default: {
                    if (state.getRetryCount() >= DEFAULT_MAX_RETRY_COUNT) {
                        state.blockTemporary(DEFAULT_TIMEOUT);
                        LogUtils.w(TAG, "Block initial " + transport.getType() + " for " + DEFAULT_TIMEOUT + ". Fail cause: " + DataFailCause.toString(cause));
                    }
                }
            }
            return true;
        } else if (type == TransportType.TRANSPORT_WIFI) {
            mBlockedUntilWiFiOn = false;

            TransportState state = transport.getInitialState();
            switch (cause) {
                case DataFailCause.ERROR_UNSPECIFIED: {
                    int retryTimeout = getRetryTimeout(state, 1, MAX_TIMEOUT_256);
                    state.blockTemporary(retryTimeout);
                    LogUtils.w(TAG, "Block initial " + transport.getType() + " for " + retryTimeout + ". Fail cause: " + DataFailCause.toString(cause));
                    break;
                }
                case DataFailCause.NO_RESPONSE_FROM_BASE_STATION: {
                    int retryTimeout = getRetryTimeout(state, 4, MAX_TIMEOUT_1024);
                    state.blockTemporary(retryTimeout);
                    LogUtils.w(TAG, "Block initial " + transport.getType() + " for " + retryTimeout + ". Fail cause: " + DataFailCause.toString(cause));
                    break;
                }
                case DataFailCause.SERVICE_OPTION_NOT_SUPPORTED:
                case DataFailCause.MISSING_UNKNOWN_APN: {
                    state.blockTemporary(TIMEOUT_12H);
                    LogUtils.w(TAG, "Block initial " + transport.getType() + " for 12H. Fail cause: " + DataFailCause.toString(cause));
                    break;
                }
                case DataFailCause.ILLEGAL_ME: {
                    state.blockTemporary(TIMEOUT_24H);
                    LogUtils.w(TAG, "Block initial " + transport.getType() + " for 24H. Fail cause: " + DataFailCause.toString(cause));
                    break;
                }
                default: {
                    if (isOSP()) {
                        int retryTimeout = getRetryTimeout(state, 4, MAX_TIMEOUT_16);
                        if (retryTimeout == (MAX_TIMEOUT_16 * 1000)) {
                            retryTimeout = TIMEOUT_12H;
                        }
                        mBlockedUntilWiFiOn = true;
                        state.blockTemporary(retryTimeout);
                        LogUtils.w(TAG, "Block initial " + transport.getType() + " for " + retryTimeout + ". Fail cause: " + DataFailCause.toString(cause));
                    } else {
                        int retryTimeout = getRetryTimeout(state, 4, MAX_TIMEOUT_1024);
                        state.blockTemporary(retryTimeout);
                        LogUtils.w(TAG, "Block initial " + transport.getType() + " for " + retryTimeout + ". Fail cause: " + DataFailCause.toString(cause));
                    }
                }
            }
            return true;
        }
        return false;
    }

    @Override
    public boolean onHandoverFailure(Transport transport, int cause, int timeout) {
        if (timeout > 0) {
            /* default will handle it */
            return false;
        }
        TransportType type = transport.getType();
        if (type == TransportType.TRANSPORT_CELLULAR) {
            if (!isImsApn()) {
                /* default will handle it */
                return false;
            }
            TransportState state = transport.getHandoverState();

            mLastHandoverFailCause = cause;
            switch (cause) {
                case DataFailCause.ACTIVATION_REJECT_GGSN:
                case DataFailCause.ACTIVATION_REJECT_UNSPECIFIED:
                case DataFailCause.SERVICE_OPTION_OUT_OF_ORDER:
                case DataFailCause.NETWORK_FAILURE: {
                    int retryTimeout = getRetryTimeout(state, 1, MAX_TIMEOUT_1024);
                    state.blockTemporary(retryTimeout);
                    LogUtils.w(TAG, "Block handover " + transport.getType() + " for " + retryTimeout + ". Fail cause: " + DataFailCause.toString(cause));
                    break;
                }
                case DataFailCause.MISSING_UNKNOWN_APN:
                case DataFailCause.SERVICE_OPTION_NOT_SUBSCRIBED: {
                    state.blockTemporary(TIMEOUT_24H);
                    LogUtils.w(TAG, "Block handover " + transport.getType() + " for 24H. Fail cause: " + DataFailCause.toString(cause));
                    break;
                }
                default: {
                    if (state.getRetryCount() >= DEFAULT_MAX_RETRY_COUNT) {
                        state.blockTemporary(DEFAULT_TIMEOUT);
                        LogUtils.w(TAG, "Block handover " + transport.getType() + " for " + DEFAULT_TIMEOUT + ". Fail cause: " + DataFailCause.toString(cause));
                    }
                }
            }
            return true;
        } else if (type == TransportType.TRANSPORT_WIFI) {
            mHandoverBlockedUntilWiFiOn = false;

            TransportState state = transport.getHandoverState();
            switch (cause) {
                case DataFailCause.ERROR_UNSPECIFIED: {
                    int retryTimeout = getRetryTimeout(state, 1, MAX_TIMEOUT_256);
                    state.blockTemporary(retryTimeout);
                    LogUtils.w(TAG, "Block handover " + transport.getType() + " for " + retryTimeout + ". Fail cause: " + DataFailCause.toString(cause));
                    break;
                }
                case DataFailCause.NO_RESPONSE_FROM_BASE_STATION: {
                    int retryTimeout = getRetryTimeout(state, 4, MAX_TIMEOUT_1024);
                    state.blockTemporary(retryTimeout);
                    LogUtils.w(TAG, "Block handover " + transport.getType() + " for " + retryTimeout + ". Fail cause: " + DataFailCause.toString(cause));
                    break;
                }
                case DataFailCause.SERVICE_OPTION_NOT_SUPPORTED:
                case DataFailCause.MISSING_UNKNOWN_APN: {
                    state.blockTemporary(TIMEOUT_12H);
                    LogUtils.w(TAG, "Block handover " + transport.getType() + " for 12H. Fail cause: " + DataFailCause.toString(cause));
                    break;
                }
                case DataFailCause.ILLEGAL_ME: {
                    state.blockTemporary(TIMEOUT_24H);
                    LogUtils.w(TAG, "Block handover " + transport.getType() + " for 24H. Fail cause: " + DataFailCause.toString(cause));
                    break;
                }
                default: {
                    if (isOSP()) {
                        int retryTimeout = getRetryTimeout(state, 4, MAX_TIMEOUT_16);
                        if (retryTimeout == (MAX_TIMEOUT_16 * 1000)) {
                            retryTimeout = TIMEOUT_12H;
                        }
                        mHandoverBlockedUntilWiFiOn = true;
                        state.blockTemporary(retryTimeout);
                        LogUtils.w(TAG, "Block handover " + transport.getType() + " for " + retryTimeout + ". Fail cause: " + DataFailCause.toString(cause));
                    } else {
                        int retryTimeout = getRetryTimeout(state, 4, MAX_TIMEOUT_1024);
                        state.blockTemporary(retryTimeout);
                        LogUtils.w(TAG, "Block handover " + transport.getType() + " for " + retryTimeout + ". Fail cause: " + DataFailCause.toString(cause));
                    }
                }
            }
            return true;
        }
        return false;
    }

    @Override
    public boolean onAirplaneModeChanged(Transport transport) {
        TransportType type = transport.getType();
        if (isImsApn() && (type == TransportType.TRANSPORT_CELLULAR)) {
            switch (mLastFailCause) {
                case DataFailCause.MISSING_UNKNOWN_APN:
                case DataFailCause.SERVICE_OPTION_NOT_SUBSCRIBED: {
                    break;
                }
                default: {
                    mLastFailCause = DataFailCause.NONE;
                    transport.getInitialState().reset();
                }
            }
            switch (mLastHandoverFailCause) {
                case DataFailCause.MISSING_UNKNOWN_APN:
                case DataFailCause.SERVICE_OPTION_NOT_SUBSCRIBED: {
                    break;
                }
                default: {
                    mLastHandoverFailCause = DataFailCause.NONE;
                    transport.getHandoverState().reset();
                }
            }
            return true;
        } else if (type == TransportType.TRANSPORT_WIFI) {
            mBlockedUntilWiFiOn = false;
            mHandoverBlockedUntilWiFiOn = false;
        }
        return false;
    }

    @Override
    public boolean onWiFiStateChanged(Transport transport) {
        if (transport.getType() == TransportType.TRANSPORT_WIFI) {
            if (isOSP()) {
                if (mBlockedUntilWiFiOn) {
                    mBlockedUntilWiFiOn = false;
                    transport.getInitialState().reset();
                }
                if (mHandoverBlockedUntilWiFiOn) {
                    mHandoverBlockedUntilWiFiOn = false;
                    transport.getHandoverState().reset();
                }
                return true;
            }
        }
        return false;
    }

    private int getRetryTimeout(TransportState state, int start, int maxTimeout) {
        int timeout = MAX_TIMEOUT_16; // set safe default

        int count = state.getRetryCount();
        if (count > 1) {
            if (count > 10) { // 1024 is 2^10
                timeout = maxTimeout;
            } else {
                timeout = start << (count - 1);
            }
            if (timeout > maxTimeout) {
                timeout = maxTimeout;
            }
        }
        return timeout * 1000;
    }

    private boolean isImsApn() {
        return (mApnType == ApnSetting.TYPE_IMS);
    }

    private boolean isOSP() {
        // Orange Spain
        return (mCarrierId == 678);
    }

    @VisibleForTesting
    int getLastFailCause() { return mLastFailCause; }

    @VisibleForTesting
    void setLastFailCause(int cause) { mLastFailCause = cause; }

    @VisibleForTesting
    int getLastFailCauseHandover() { return mLastHandoverFailCause; }

    @VisibleForTesting
    void setLastFailCauseHandover(int cause) { mLastHandoverFailCause = cause; }

    @VisibleForTesting
    boolean getBlockedUntilWiFiOn() { return mBlockedUntilWiFiOn; }

    @VisibleForTesting
    void setBlockedUntilWiFiOn() { mBlockedUntilWiFiOn = true; }

    @VisibleForTesting
    boolean getBlockedUntilWiFiOnHandover() { return mHandoverBlockedUntilWiFiOn; }

    @VisibleForTesting
    void setBlockedUntilWiFiOnHandover() { mHandoverBlockedUntilWiFiOn = true; }
}
