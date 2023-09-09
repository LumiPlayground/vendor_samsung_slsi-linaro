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
package com.shannon.dataservice;

import android.net.ConnectivityManager;
import android.net.LinkProperties;
import android.net.Network;
import android.net.NetworkCapabilities;
import android.net.NetworkRequest;
import android.telephony.AccessNetworkConstants;
import android.telephony.NetworkRegistrationInfo;
import android.telephony.Rlog;
import android.telephony.ServiceState;
import android.telephony.TelephonyManager;

public class MobileNetworkCallback extends ConnectivityManager.NetworkCallback {
    private static final boolean DBG = true;

    private final String TAG;
    private final int mSlotId;
    private final TelephonyManager mTelephonyManager;
    private Network mNetwork;
    private LinkProperties mLinkProperties;

    public MobileNetworkCallback(int slotId, TelephonyManager tm) {
        mSlotId = slotId;
        mTelephonyManager = tm;
        mNetwork = null;
        mLinkProperties = null;

        TAG = ShannonDataService.getServiceTAG("-NC-" + mSlotId);
    }

    @Override
    public void onLost(Network network) {
        log("Mobile network " + network + " lost for " + mSlotId);
        mNetwork = null;
        mLinkProperties = null;
    }

    @Override
    public void onLinkPropertiesChanged(Network network, LinkProperties linkProperties) {
        log("Network " + network + " onLinkPropertiesChanged {LP:" + linkProperties + "} for " + mSlotId);
        mNetwork = network;
        mLinkProperties = linkProperties;
    }

    public NetworkRequest buildRequest() {
        return new NetworkRequest.Builder()
                .addCapability(NetworkCapabilities.NET_CAPABILITY_INTERNET)
                .addTransportType(NetworkCapabilities.TRANSPORT_CELLULAR)
                .setNetworkSpecifier(mTelephonyManager.getNetworkSpecifier())
                .build();
    }

    public boolean isConnected() { return mNetwork != null; }
    public boolean isRoaming() { return mTelephonyManager.isNetworkRoaming(); }
    public Network getNetwork() { return mNetwork; }
    public LinkProperties getLinkProperties() { return mLinkProperties; }

    public int getNetworkType() {
        try {
            ServiceState ss = mTelephonyManager.getServiceState();
            if (ss != null) {
                NetworkRegistrationInfo nri = ss.getNetworkRegistrationInfo(
                        NetworkRegistrationInfo.DOMAIN_PS,
                        AccessNetworkConstants.TRANSPORT_TYPE_WWAN);
                if (nri != null) {
                    return nri.getAccessNetworkTechnology();
                }
            }
        } catch (Exception e) {
            // nothing
        }
        return TelephonyManager.NETWORK_TYPE_UNKNOWN;
    }

    private void log(String s) {
        if (DBG) {
            Rlog.d(TAG, s);
        }
    }
}
