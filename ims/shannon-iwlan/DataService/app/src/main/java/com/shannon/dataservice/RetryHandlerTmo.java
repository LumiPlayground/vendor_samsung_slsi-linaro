/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or
 * distributed, transmitted, transcribed, stored in a retrieval system or
 * translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed to third parties
 * without the express written permission of Samsung Electronics.
 */
package com.shannon.dataservice;

import android.content.Context;
import android.os.Message;
import android.telephony.TelephonyManager;
import android.text.TextUtils;

import java.net.InetAddress;
import java.util.ArrayList;

public class RetryHandlerTmo extends RetryHandler {
    /* same as defined at Evolved_Packet_Data_Gateway_ePDG_TRD */
    private int k;
    private int n;
    private int m;
    private int N_fqdn;
    private int N_fast_try;
    private int N_max;
    private int T_fast;
    private int T_start;

    public RetryHandlerTmo(int slotId, ApnType apnType, String apnName, Context context,
                           ConfigurationManager configuration, DnsResolver resolver, Message message) {
        super(slotId, apnType, apnName, context, configuration, resolver, message);

        TAG = ShannonDataService.getServiceTAG("-RH-" + apnType.getUserFriendlyName() + "-tmo-" + slotId);

        k = n = m = 0;
        /* set default */
        N_fqdn = 2;
        N_fast_try = 3;
        N_max = 6;
        T_fast = 0;
        T_start = 300;
        /* load from configuration */
        loadParameters();
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
        log("Algorithm parameters: N_fqdn=" + N_fqdn + " N_fast_try=" + N_fast_try +
                " N_max=" + N_max + " T_fast=" + T_fast + " T_start=" + T_start);
    }

    @Override
    public InetAddress getAddress(ArrayList<InetAddress> localAddresses) {
        mConfigurationParameters = mConfigurationManager.getConnectionConfiguration();
        buildDefaultRetryTimerMap();
        buildServersBlackList();
        loadParameters();

        if (mServerNames.isEmpty()) {
            TelephonyManager tm = mSimInformation.getTelephonyManager();

            FqdnBuilder builder = new FqdnBuilder(mConfigurationParameters, tm, mDnsResolver, mSimInformation.getRecords());
            mServerNames = builder.build(mApnType.getBit(), tm.isNetworkRoaming());
            if (mApnType == ApnType.APN_IMS && !mPcoServerAddresses.isEmpty()) {
                for (InetAddress a : mPcoServerAddresses) {
                    mServerNames.addFirst(a.getHostName());
                }
            }
        }
        if (mLastError == TunnelError.ERR_CONGESTION) {
            if (m >= N_max) {
                String last = mServerNames.removeFirst();
                mServerNames.addLast(last);
                String next = mServerNames.getFirst();
                if (!last.equals(next)) {
                    if (++k <= N_fqdn) {
                        n = 0;
                    }
                }
                m = 0;

                mServerAddresses.clear();
            }
            if (mServerAddresses.isEmpty()) {
                String current = mServerNames.getFirst();
                mLastError = resolveServer();
                if (mLastError != TunnelError.ERR_SUCCESS) {
                    k = n = m = 0; mServerNames.clear();
                    return null;
                }
                /* return FQDN back to queue */
                mServerNames.addFirst(current);
            }
            m++; n++;
        } else {
            mLastError = TunnelError.ERR_SUCCESS;
            if (mServerAddresses.isEmpty()) {
                do {
                    mLastError = resolveServer();
                } while ((mLastError != TunnelError.ERR_SUCCESS) && !mServerNames.isEmpty());
                if (mServerAddresses.isEmpty()) {
                    loge("No server addresses found");
                    return null;
                }
            }
        }
        InetAddress result = null;
        if (hasIpv4(localAddresses)) {
            result = getFirstRemoteIpv4();
        }
        if ((result == null) && hasIpv6(localAddresses)) {
            result = getFirstRemoteIpv6();
        }
        return result;
    }

    @Override
    public void clearAddresses() {
        if (mLastError != TunnelError.ERR_CONGESTION) {
            mServerAddresses.clear();
        }
    }

    @Override
    public InetAddress getNextAddress(ArrayList<InetAddress> localAddresses) {
        if (mLastError == TunnelError.ERR_CONGESTION) {
            if (mServerNames.size() != N_fqdn) {
                mServerNames.clear(); mServerAddresses.clear();
            }
            return null;
        }
        /* reset */
        k = n = m = 0;
        /* use default */
        return super.getNextAddress(localAddresses);
    }

    @Override
    protected int getRetryTimeout(boolean handover) {
        if (mDefaultRetryTimerMap.containsKey(mLastError)) {
            return getDefaultRetryTimeout(mLastError);
        }
        if (mLastError == TunnelError.ERR_CONGESTION) {
            if (n < N_fast_try) {
                if (T_fast > 0) {
                    return (T_fast * 1000);
                }
                return 0;
            } else {
                return (T_start * (1 << (Math.min(n, N_max - 1) - N_fast_try)));
            }
        }
        if (isPermanentFailure(mLastError)) {
            return DEFAULT_RETRY_VALUE;
        }
        return getNextRetryTimeout(handover);
    }
}
