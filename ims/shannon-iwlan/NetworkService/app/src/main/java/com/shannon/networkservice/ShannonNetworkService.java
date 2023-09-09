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
package com.shannon.networkservice;

import android.content.Context;
import android.net.ConnectivityManager;
import android.net.LinkProperties;
import android.net.Network;
import android.net.NetworkCapabilities;
import android.net.NetworkRequest;
import android.os.Binder;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;
import android.telephony.AccessNetworkConstants;
import android.telephony.LteVopsSupportInfo;
import android.telephony.NetworkRegistrationInfo;
import android.telephony.NetworkService;
import android.telephony.NetworkServiceCallback;
import android.telephony.Rlog;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
import android.util.SparseArray;

import com.android.internal.annotations.VisibleForTesting;

import java.util.ArrayList;
import java.util.List;

public class ShannonNetworkService extends NetworkService {
    private static final boolean DBG = true;
    private static final String TAG = "ShannonNS";
    public static String getTag() { return TAG; }

    private static SparseArray<ShannonNetworkServiceProvider> mProviders = new SparseArray<>();

    class LocalBinder extends Binder {
        ShannonNetworkService getService() { return ShannonNetworkService.this; }
    }

    private SimManager mSimManager = null;

    @VisibleForTesting
    class ShannonNetworkServiceProvider extends NetworkServiceProvider {
        @VisibleForTesting
        class ShannonNetworkInfo {
            Network mNetwork = null;
            LinkProperties mLinkProperties = null;
            boolean mIsSuspended = false;
            boolean mIsBlocked = false;
            // Because onBlockedStatusChanged is called after onAvailable, onCapabilitiesChanged,
            // onLinkPropertiesChanged, all other members should be initialized when it is true.
            boolean mInitialBlockStateReceived = false;
            boolean isNetworkReady() {
                return (!mIsSuspended && !mIsBlocked);
            }

        }

        private final String TAG1;
        private final Context mContext;
        private final ConnectivityManager mConnectivityManager;
        private final ShannonNetworkProviderCallback mRequestCallback;
        private final HandlerThread mHandlerThread;
        private final ConfigurationManager mConfiguration;
        private ShannonNetworkInfo mNetworkInfo;

        @VisibleForTesting
        class ShannonNetworkProviderCallback extends ConnectivityManager.NetworkCallback {
            @Override
            public void onAvailable(Network network) {
                log("Network " + network + " available");
                if (mNetworkInfo == null) {
                    mNetworkInfo = new ShannonNetworkInfo();
                    // Update only network. onAvailable will always immediately be followed by a
                    // call to onCapabilitiesChanged, then by a call to onLinkPropertiesChanged, and
                    // a call onBlockedStatusChanged. The other members of ShannonNetworkInfo will
                    // be updated there.
                    mNetworkInfo.mNetwork = network;
                }
            }

            @Override
            public void onLost(Network network) {
                log("Network " + network + " lost");
                if (mNetworkInfo != null) {
                    mNetworkInfo = null;
                    /* send notification to listeners */
                    notifyNetworkRegistrationInfoChanged();
                }
            }

            @Override
            public void onCapabilitiesChanged(Network network, NetworkCapabilities networkCapabilities) {
                log("Network " + network + " onCapabilitiesChanged {NC:" + networkCapabilities + "}");
                if (mNetworkInfo == null) return;

                final boolean wasSuspended = mNetworkInfo.mIsSuspended;
                mNetworkInfo.mIsSuspended = !networkCapabilities.hasCapability(
                        NetworkCapabilities.NET_CAPABILITY_NOT_SUSPENDED);

                // The initial notification will be sent in onBlockedStatusChanged, called
                // immediately after the initial onCapabilitiesChanged
                if (mNetworkInfo.mInitialBlockStateReceived
                        && wasSuspended != mNetworkInfo.mIsSuspended) {
                    /* send notification to listeners */
                    notifyNetworkRegistrationInfoChanged();
                }
            }

            @Override
            public void onLinkPropertiesChanged(Network network, LinkProperties linkProperties) {
                log("Network " + network + " onLinkPropertiesChanged {LP:" + linkProperties + "}");
                if (mNetworkInfo != null) {
                    if (!mNetworkInfo.mInitialBlockStateReceived) {
                        mNetworkInfo.mLinkProperties = linkProperties;
                        return;
                    }

                    if (!linkProperties.equals(mNetworkInfo.mLinkProperties)) {
                        mNetworkInfo.mLinkProperties = linkProperties;
                        /* send notification to listeners */
                        notifyNetworkRegistrationInfoChanged();
                    }
                }
            }

            @Override
            public void onBlockedStatusChanged(Network network, boolean blocked) {
                log("Network " + network + " blocked:" + blocked);
                if (mNetworkInfo == null) return;

                final boolean isFirstStatus = !mNetworkInfo.mInitialBlockStateReceived;
                mNetworkInfo.mInitialBlockStateReceived = true;
                final boolean wasBlocked = mNetworkInfo.mIsBlocked;
                mNetworkInfo.mIsBlocked = blocked;
                // Skip only the first notification if the network is not ready as per historical
                // behavior.
                if ((isFirstStatus && mNetworkInfo.isNetworkReady())
                        || (!isFirstStatus && wasBlocked != blocked)) {
                    notifyNetworkRegistrationInfoChanged();
                }
            }
        }

        ShannonNetworkServiceProvider(Context context, int slotIndex) {
            super(slotIndex);
            mNetworkInfo = null;

            /* required */
            mContext = context;
            mConnectivityManager = mContext.getSystemService(ConnectivityManager.class);

            TAG1 = TAG.concat("-" + slotIndex);

            /* handler */
            mHandlerThread = new HandlerThread(ShannonNetworkService.class.getSimpleName());
            mHandlerThread.start();
            Looper looper = mHandlerThread.getLooper();
            Handler handler = new Handler(looper);

            mConfiguration = new ConfigurationManager(slotIndex, TAG1);
            mSimManager = SimManager.getInstance(mContext);

            /* network request */
            NetworkRequest.Builder builder = new NetworkRequest.Builder();
            builder.addCapability(NetworkCapabilities.NET_CAPABILITY_INTERNET);
            builder.addCapability(NetworkCapabilities.NET_CAPABILITY_NOT_RESTRICTED);
            builder.addTransportType(NetworkCapabilities.TRANSPORT_WIFI);
            NetworkRequest request = builder.build();

            /* callback */
            mRequestCallback = new ShannonNetworkProviderCallback();
            mConnectivityManager.registerNetworkCallback(request, mRequestCallback, handler);
        }

        @Override
        public void requestNetworkRegistrationInfo(int domain, NetworkServiceCallback callback) {
            log("requestNetworkRegistrationInfo for domain " + domain);
            if (domain == NetworkRegistrationInfo.DOMAIN_CS) {
                log("requestNetworkRegistrationInfo CS domain is not supported");
                callback.onRequestNetworkRegistrationInfoComplete(NetworkServiceCallback.RESULT_ERROR_UNSUPPORTED, null);
            } else if (domain == NetworkRegistrationInfo.DOMAIN_PS) {
                int registrationState = NetworkRegistrationInfo.REGISTRATION_STATE_NOT_REGISTERED_OR_SEARCHING;
                int accessNetwork = TelephonyManager.NETWORK_TYPE_UNKNOWN;
                List<Integer> availableServices = null;
                if ((mNetworkInfo != null && mNetworkInfo.isNetworkReady()) || isVoMobilePossible()) {
                    registrationState = NetworkRegistrationInfo.REGISTRATION_STATE_HOME;
                    accessNetwork = TelephonyManager.NETWORK_TYPE_IWLAN;

                    availableServices = new ArrayList<>();
                    availableServices.add(NetworkRegistrationInfo.SERVICE_TYPE_DATA);
                }
                log("requestNetworkRegistrationInfo PS domain state is " + registrationState);

                NetworkRegistrationInfo nri = new NetworkRegistrationInfo.Builder()
                        .setDomain(NetworkRegistrationInfo.DOMAIN_PS)
                        .setTransportType(AccessNetworkConstants.TRANSPORT_TYPE_WLAN)
                        .setRegistrationState(registrationState)
                        .setAccessNetworkTechnology(accessNetwork)
                        .setRejectCause(0)
                        .setEmergencyOnly(false)
                        .setAvailableServices(availableServices)
                        .setCellIdentity(null)
                        .build();
                callback.onRequestNetworkRegistrationInfoComplete(NetworkServiceCallback.RESULT_SUCCESS, nri);
            } else {
                loge("requestNetworkRegistrationInfo invalid domain " + domain);
                callback.onRequestNetworkRegistrationInfoComplete(NetworkServiceCallback.RESULT_ERROR_INVALID_ARG, null);
            }
        }

        @Override
        public void close() {
            log("WLAN network service deleted for slot " + getSlotIndex());
            mConnectivityManager.unregisterNetworkCallback(mRequestCallback);
            mConfiguration.destroy();
            mHandlerThread.quit();

            mProviders.remove(getSlotIndex());
        }

        @VisibleForTesting
        boolean isVoMobilePossible() {
            if (mConfiguration.isVoMobileEnabled()) {
                return mSimManager.isVoMobilePossible(getSlotIndex(), mConfiguration);
            }
            return false;
        }

        public boolean isVoMobileEnabled() {
            return mConfiguration.isVoMobileEnabled();
        }

        private void log(String s) {
            if (DBG) {
               Rlog.d(TAG1, s);
            }
        }

        private void loge(String s) {
           Rlog.e(TAG1, s);
        }

        @VisibleForTesting
        ConfigurationManager getConfigurationManager() {
            return mConfiguration;
        }

        @VisibleForTesting
        ShannonNetworkProviderCallback getRequestCallback() {
            return mRequestCallback;
        }

        @VisibleForTesting
        ConfigurationManager getConfiguration() {
            return mConfiguration;
        }

        @VisibleForTesting
        ShannonNetworkInfo getShannonNetworkInfo() {
                return mNetworkInfo;
        }
    }

    @Override
    public NetworkServiceProvider onCreateNetworkServiceProvider(int slotIndex) {
        return this.onCreateNetworkServiceProvider(getApplicationContext(), slotIndex);
    }

    @VisibleForTesting
    NetworkServiceProvider onCreateNetworkServiceProvider(Context context, int slotIndex) {
        log("WLAN network service created for slot " + slotIndex);
        if (!SubscriptionManager.isValidSlotIndex(slotIndex)) {
            loge("Tried to WLAN network service with invalid slotId " + slotIndex);
            return null;
        }
        ShannonNetworkServiceProvider provider = new ShannonNetworkServiceProvider(context, slotIndex);
        mProviders.put(slotIndex, provider);
        return provider;
    }

    public static void onConfigurationChanged(int slotIndex) {
        ShannonNetworkServiceProvider provider = mProviders.get(slotIndex);
        if (provider != null) {
            provider.notifyNetworkRegistrationInfoChanged();
        }
    }

    public static void onConnectivityChanged(int notUsedSlotId) {
        for (int i = 0; i < mProviders.size(); i++) {
            /* notify other slots */
            int slot = mProviders.keyAt(i);
            if (slot != notUsedSlotId) {
                ShannonNetworkServiceProvider provider = mProviders.valueAt(i);
                if (provider.isVoMobileEnabled()) {
                    provider.notifyNetworkRegistrationInfoChanged();
                }
            }
        }
    }

    @Override
    public void onDestroy() {
        if (mSimManager != null) {
            mSimManager.destroy();
            mSimManager = null;
        }
        for (int i = 0; i < mProviders.size(); i++) {
            ShannonNetworkServiceProvider provider = mProviders.valueAt(i);
            provider.close();
        }
        super.onDestroy();
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
    static SparseArray<ShannonNetworkServiceProvider> getProviders() {
        return mProviders;
    }
}
