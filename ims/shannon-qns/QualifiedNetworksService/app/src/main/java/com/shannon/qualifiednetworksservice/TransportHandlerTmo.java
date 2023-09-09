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

import android.telephony.AccessNetworkConstants;
import android.telephony.CellSignalStrengthLte;
import android.telephony.DataFailCause;
import android.telephony.NetworkRegistrationInfo;
import android.telephony.ServiceState;
import android.telephony.TelephonyManager;
import android.telephony.data.ApnSetting;
import android.text.TextUtils;

import com.android.internal.annotations.VisibleForTesting;
import com.shannon.qualifiednetworksservice.Transport.TransportType;
import com.shannon.qualifiednetworksservice.util.LogUtils;

public class TransportHandlerTmo extends VendorTransportHandler {

    /*
    3.8.1 ePDG overload handling (Mandatory) ID: GID-MTRREQ-519288
    Note 3:
    N_fqdn      | ePDG_FQDN_Count           | 2
    N_fast_try  | Fast_Attempts_Count       | 3
    N_max       | Max_Attempts_Count        | 6
    T_fast      | Fast_Retry_Timer          | 0
    T_start     | BackOff_Start_Timer       | 300sec
     */
    private static int N_fqdn = 2;
    private static int N_fast_try = 3;
    private static int N_max = 6;
    private static int T_fast = 0; // fast retry
    private static int T_start = (300 * 1000); // 5 min
    private static int TIMEOUT_15M = (15 * 60 * 1000); // 15 min
    private static int TIMEOUT_1H = (60 * 60 * 1000); // 1 hour

    private static final int SUCCESS = 0;
    private static final int REG_FAIL_TRY_AFTER_WIFI_REASSOCIATE = 15; // 0x0F

    private final int mApnType;
    private final ConfigurationManager mConfigurationManager;
    private final TelephonyManager mTelephonyManager;

    private boolean mBlockedUntilHome;
    private boolean mBlockedUntilWifiAssociated;

    public TransportHandlerTmo(int type, TelephonyManager tm,
            ConfigurationManager configurationManager) {
        mApnType = type;
        TAG = ShannonQualifiedNetworksService.getServiceTAG("-VTH-tmo");
        mTelephonyManager = tm;
        mBlockedUntilHome = false;
        mBlockedUntilWifiAssociated = false;
        mConfigurationManager = configurationManager;
        /* load from configuration */
        loadParameters();
    }

    private boolean processConnectionFailure(TransportState state, TransportType type, String string, int cause) {
        if (type == TransportType.TRANSPORT_WIFI) {
            switch (cause) {
                case DataFailCause.NETWORK_FAILURE: {
                    state.blockTemporary(TIMEOUT_1H);
                    LogUtils.v(TAG, string + type +
                            " for 1H. Fail cause: " + DataFailCause.toString(cause));
                    break;
                }
                case DataFailCause.OEM_DCFAILCAUSE_1: { /* NETWORK_TOO_BUSY */
                    state.blockTemporary(TIMEOUT_15M);
                    LogUtils.v(TAG, string + type +
                            " for 15M. Fail cause: " + DataFailCause.toString(cause));
                    break;
                }
                case DataFailCause.SERVICE_OPTION_NOT_SUPPORTED: { /* RAT_DISALLOWED */
                    /* Never retry until power cycle or SIM change */
                    state.block();
                    LogUtils.v(TAG, string + type + " permanently. Fail cause: " + DataFailCause.toString(cause));
                    break;
                }
                case DataFailCause.APN_DISALLOWED_ON_ROAMING: {
                    /* No retry until return HOME */
                    mBlockedUntilHome = true;
                    state.block();
                    LogUtils.v(TAG, string + type + " until HOME. Fail cause: " + DataFailCause.toString(cause));
                    break;
                }
                case DataFailCause.CONGESTION: {
                    int retryTimeout = getRetryTimeout(state);
                    state.blockTemporary(retryTimeout);
                    LogUtils.e(TAG, string + type + " for " + retryTimeout +
                            ". Fail cause: " + DataFailCause.toString(cause));
                    break;
                }
                default:
                    break;
            }
            return true;
        }
        return false;
    }

    @Override
    public boolean onConnectionFailure(Transport transport, int cause, int timeout) {
        if (timeout > 0) {
            /* default will handle it */
            return false;
        }
        return processConnectionFailure(transport.getInitialState(), transport.getType(),
                "Block initial ", cause);
    }

    @Override
    public boolean onHandoverFailure(Transport transport, int cause, int timeout) {
        if (timeout > 0) {
            /* default will handle it */
            return false;
        }
        return processConnectionFailure(transport.getHandoverState(), transport.getType(),
                "Block handover ", cause);
    }

    @Override
    public boolean onAirplaneModeChanged(Transport transport) {
        if (mBlockedUntilWifiAssociated && isImsOnWiFi(transport)) {
            transport.getInitialState().reset();
            transport.getHandoverState().reset();
            mBlockedUntilWifiAssociated = false;
            return true;
        }
        return false;
    }

    @Override
    public boolean onOperatorChanged(Transport transport) {
        if (mBlockedUntilHome && isRegisteredHome() && isImsOnWiFi(transport)) {
            transport.getInitialState().reset();
            transport.getHandoverState().reset();
        }
        return false;
    }

    @Override
    public boolean onWiFiStateChanged(Transport transport) {
        if (mBlockedUntilWifiAssociated && isImsOnWiFi(transport)) {
            transport.getInitialState().reset();
            transport.getHandoverState().reset();
            mBlockedUntilWifiAssociated = false;
            return true;
        }
        return false;
    }

    @Override
    public boolean onWiFiAccessPointChanged(Transport transport) {
        if (mBlockedUntilWifiAssociated && isImsOnWiFi(transport)) {
            transport.getInitialState().reset();
            transport.getHandoverState().reset();
            mBlockedUntilWifiAssociated = false;
            return true;
        }
        return false;
    }

    @Override
    public boolean onWfcStateChanged(Transport transport) {
        if (mBlockedUntilWifiAssociated && isImsOnWiFi(transport)) {
            transport.getInitialState().reset();
            transport.getHandoverState().reset();
            mBlockedUntilWifiAssociated = false;
            return true;
        }
        return false;
    }

    @Override
    public boolean onCarrierConfigChanged(Transport transport) {
        loadParameters();
        return true;
    }

    @Override
    public boolean onRegistrationFailed(Transport transport, int statusCode) {
        if (isImsOnWiFi(transport) && (statusCode == REG_FAIL_TRY_AFTER_WIFI_REASSOCIATE)) {
            mBlockedUntilWifiAssociated = true;
            transport.getInitialState().block();
            transport.getHandoverState().block();
            return true;
        }
        return false;
    }

    private boolean isRegistered(NetworkRegistrationInfo ni) {
        return ((ni != null) && (ni.isRegistered()));
    }

    private boolean isRegisteredHome() {
        ServiceState serviceState = mTelephonyManager.getServiceState();
        NetworkRegistrationInfo psRegState = serviceState.getNetworkRegistrationInfo(
                NetworkRegistrationInfo.DOMAIN_PS, AccessNetworkConstants.TRANSPORT_TYPE_WWAN);
        NetworkRegistrationInfo csRegState = serviceState.getNetworkRegistrationInfo(
                NetworkRegistrationInfo.DOMAIN_CS, AccessNetworkConstants.TRANSPORT_TYPE_WWAN);

        return ((isRegistered(psRegState) || isRegistered(csRegState)) && (!mTelephonyManager.isNetworkRoaming()));
    }

    private int getParameter(String input, int defaultValue) {
        if (!TextUtils.isEmpty(input)) {
            try {
                String[] strings = input.trim().split("=");
                if (strings.length > 1) {
                    return Integer.parseInt(strings[1].trim());
                }
            } catch (Exception e) {
                // not used
            }
        }
        return defaultValue;
    }

    private void loadParameters() {
        String parametersString = mConfigurationManager.getRetryParameters();
        if (TextUtils.isEmpty(parametersString)) {
            return;
        }
        String[] parameters = parametersString.split(",");
        for (String p : parameters) {
            p = p.trim();
            if (p.startsWith("N_fqdn")) {
                N_fqdn = getParameter(p, N_fqdn);
            } else if (p.startsWith("N_fast_try")) {
                N_fast_try = getParameter(p, N_fast_try);
            } else if (p.startsWith("N_max")) {
                N_max = getParameter(p, N_max);
            } else if (p.startsWith("T_fast")) {
                T_fast = getParameter(p, T_fast);
            } else if (p.startsWith("T_start")) {
                T_start = getParameter(p, T_start);
            }
        }
        LogUtils.v(TAG, "Algorithm parameters: N_fqdn=" + N_fqdn + " N_fast_try=" + N_fast_try +
                " N_max=" + N_max + " T_fast=" + T_fast + " T_start=" + T_start);
    }

    /*
    3.8.1 ePDG overload handling (Mandatory) ID: GID-MTRREQ-519288
    Note 4:
    The back-off timer T_backoff is calculated as:
    T_backoff = T_start * 2 ^ (min[n, N_max -1] - N_fast_try)
    e.g.
    T_start = 5mins (default)
    N_max = 6 (default)
    N_fast_try = 3 (default)
    If n=3, T_backoff = 5 * 2^(min[3, 6-1] – 3) = 5mins
    If n=4, T_backoff = 5 * 2^(min[4, 6-1] – 3) = 10mins
    If n=5, T_backoff = 5 * 2^(min[5, 6-1] – 3) = 20mins
    If n=6, T_backoff = 5 * 2^(min[6, 6-1] – 3) = 20mins
    If n>6, T_backoff = 5 * 2^(5 – 3) = 20mins
     */
    private int getRetryTimeout(TransportState state) {
        int count = state.getRetryCount();
        int backoffTimeout = (count < N_fast_try) ?
                T_fast : (T_start * (1 << (Math.min(count, N_max - 1) - N_fast_try)));
        return backoffTimeout;
    }

    private boolean isImsApn() {
        return (mApnType == ApnSetting.TYPE_IMS);
    }

    private boolean isImsOnWiFi(Transport transport) {
        return (isImsApn() && (transport.getType() == TransportType.TRANSPORT_WIFI));
    }

    @VisibleForTesting
    protected void setBlockedUntilWifiAssociated(boolean blockedUntilWifiAssociated) {
        mBlockedUntilWifiAssociated = blockedUntilWifiAssociated;
    }

    @VisibleForTesting
    protected void setBlockedUntilHome(boolean blockedUntilHome) {
        mBlockedUntilHome = blockedUntilHome;
    }
}
