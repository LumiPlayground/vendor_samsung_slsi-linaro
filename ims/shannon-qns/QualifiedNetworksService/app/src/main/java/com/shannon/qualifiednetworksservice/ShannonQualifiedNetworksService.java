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
package com.shannon.qualifiednetworksservice;

import android.content.Context;
import android.os.Binder;
import android.os.Bundle;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;
import android.os.Message;
import android.telephony.TelephonyManager;
import android.telephony.data.ThrottleStatus;
import android.telephony.SubscriptionManager;
import android.telephony.data.ApnSetting;
import android.telephony.data.QualifiedNetworksService;
import android.util.SparseArray;

import com.android.internal.annotations.VisibleForTesting;
import com.android.internal.util.IndentingPrintWriter;

import com.shannon.qualifiednetworksservice.util.LogUtils;
import com.shannon.qualifiednetworksservice.util.PackageVersion;
import com.shannon.vendorcarrierconfig.ShannonVendorCarrierConfig;

import java.io.FileDescriptor;
import java.io.PrintWriter;
import java.util.List;

public class ShannonQualifiedNetworksService extends QualifiedNetworksService {
    private static final String TAG = "ShannonQNS";
    private static ShannonQualifiedNetworksService sMe = null;

    public static String getServiceTAG(String s) { return TAG.concat(s); }

    class LocalBinder extends Binder {
        ShannonQualifiedNetworksService getService() { return ShannonQualifiedNetworksService.this; }
    }

    private static final int SIM_STATE_CHANGED = 1;
    private static final int APN_NETWORK_LIST_UPDATED = 2;
    private static final int VENDOR_CONFIG_UPDATED = 3;

    private Context mContext = null;
    private PackageVersion mVersion;
    private ShannonVendorCarrierConfig mVendorConfig;
    private SimManager mSimManager;
    private LocationManager mLocationManager;
    private AlarmTimerManager mAlarmManager;
    private SparseArray<ShannonNetworkAvailabilityProvider> mNetworkProviders;

    protected class ShannonNetworkAvailabilityProvider extends NetworkAvailabilityProvider {
        private final Looper mLooper;
        private final Handler mHandler;
        private final HandlerThread mHandlerThread;
        private final ConfigurationManager mConfiguration;
        private HandoverConditions mConditions;
        private SparseArray<ApnQualifiedNetworksProvider> mApnProviders;

        ShannonNetworkAvailabilityProvider(int slotIndex) {
            super(slotIndex);

            mConditions = null;
            mApnProviders = new SparseArray<>();

            mHandlerThread = new HandlerThread(ShannonNetworkAvailabilityProvider.class.getSimpleName());
            mHandlerThread.start();
            mLooper = mHandlerThread.getLooper();
            mHandler = new Handler(mLooper) {
                @Override
                public void handleMessage(Message message) {
                    switch (message.what) {
                        case SIM_STATE_CHANGED:
                            SimInformation info = (SimInformation)message.obj;
                            if (info != null) {
                                if (mConditions == null) {
                                    try {
                                        LogUtils.i(TAG, "Create new APN providers, SIM inserted. Slot "
                                            + getSlotIndex() + ", subId " + info.getSubId());

                                        mConditions = new HandoverConditions(getSlotIndex(), mContext, mConfiguration);
                                        for (ApnType type : ApnType.values()) {
                                            if (type.isSupported()) {
                                                mApnProviders.put(type.getType(),
                                                        new ApnQualifiedNetworksProvider(type.getType(),
                                                                mConditions, mHandler, APN_NETWORK_LIST_UPDATED));
                                            }
                                        }
                                    } catch (Exception e) {
                                         LogUtils.e(TAG, "Failed to create HandoverConditions " + e);
                                    }
                                }
                            } else {
                                if (mConditions != null) {
                                    LogUtils.i(TAG, "Delete all APN providers, SIM removed. Slot " + getSlotIndex());
                                    removeProviders();
                                }
                            }
                            break;

                        case APN_NETWORK_LIST_UPDATED:
                            try {
                                PreferredList list = (PreferredList)message.obj;

                                updateQualifiedNetworkTypes(message.arg1, list);
                                LogUtils.d(TAG, "updateQualifiedNetworkTypes for "
                                        + ApnSetting.getApnTypeString(message.arg1) + " networks=" + list);
                            } catch (Exception e) {
                                LogUtils.e(TAG, "Failed to updateQualifiedNetworkTypes: " + e);
                            }
                            LogUtils.w(TAG, "" + mVersion);
                            break;

                        case VENDOR_CONFIG_UPDATED:
                            if (mConditions != null) {
                                mConditions.onConfigurationChanged();
                            }
                            break;

                        default:
                    }
                }
            };

            Message message = Message.obtain(mHandler, VENDOR_CONFIG_UPDATED);
            mConfiguration = new ConfigurationManager(slotIndex, mContext, message);
            /* register for SIM update */
            message = Message.obtain(mHandler, SIM_STATE_CHANGED);
            mSimManager.registerForSimEvent(getSlotIndex(), message);
        }

        private void removeProviders() {
            for (int i = 0; i < mApnProviders.size(); i++) {
                ApnQualifiedNetworksProvider p = mApnProviders.valueAt(i);
                p.destroy();
            }
            mApnProviders.clear();
            if (mConditions != null) {
                mConditions.destroy();
                mConditions = null;
            }
        }

        @Override
        public void reportThrottleStatusChanged(List<ThrottleStatus> statuses) {
            LogUtils.i(TAG, "reportThrottleStatusChanged: statuses size=" + statuses.size());
            if ((statuses != null) && (statuses.size() > 0)) {
                for (ThrottleStatus status : statuses) {
                    if ((mConditions != null) && (getSlotIndex() == status.getSlotIndex())) {
                        mConditions.onThrottleStatusChanged(status);
                    }
                }
            }
        }

        @Override
        public void close() {
            removeProviders();
            mConfiguration.destroy();
            mHandlerThread.quit();
            mNetworkProviders.remove(getSlotIndex());
            LogUtils.i(TAG, "Qualified network provider removed for slot " + getSlotIndex());
        }

        @VisibleForTesting
        protected void setHandoverCondition(HandoverConditions handoverConditions) {
            mConditions = handoverConditions;
        }

        @VisibleForTesting
        protected HandoverConditions getHandoverCondition() {
            return mConditions;
        }

        @VisibleForTesting
        protected Handler getHandler() {
            return mHandler;
        }

        public void dump(FileDescriptor fd, IndentingPrintWriter pw, String[] args) {
            pw.increaseIndent();
            for (int i = 0; i < mApnProviders.size(); i++) {
                ApnQualifiedNetworksProvider p = mApnProviders.valueAt(i);
                p.dump(fd, pw, args);
            }
            pw.decreaseIndent();
            pw.flush();
        }
    }

    @Override
    public NetworkAvailabilityProvider onCreateNetworkAvailabilityProvider(int slotIndex) {
        LogUtils.i(TAG, "Qualified network provider created for slot " + slotIndex);
        if (!SubscriptionManager.isValidSlotIndex(slotIndex)) {
            LogUtils.w(TAG, "Tried to network provider with invalid slotId " + slotIndex);
            return null;
        }
        ShannonNetworkAvailabilityProvider
                provider = new ShannonNetworkAvailabilityProvider(slotIndex);
        mNetworkProviders.put(slotIndex, provider);
        return provider;
    }

    @Override
    public void onCreate() {
        mContext = getApplicationContext();
        mVendorConfig = new ShannonVendorCarrierConfig(mContext);
        mSimManager = new SimManager(mContext);
        mLocationManager = new LocationManager(mContext);
        mVersion = new PackageVersion(getServiceTAG("-Version"), mContext);
        mNetworkProviders = new SparseArray<>();
        mAlarmManager = new AlarmTimerManager(mContext);

        LogUtils.i(TAG, "ShannonQualifiedNetworksService created");
        LogUtils.w(TAG, "" + mVersion);

        sMe = this;
    }

    @Override
    public void onDestroy() {
        LogUtils.i(TAG, "ShannonQualifiedNetworksService removed");
        mAlarmManager.destroy();
        mSimManager.destroy();
        mLocationManager.destroy();
        mVendorConfig.destroy();
        super.onDestroy();
    }

    public static void updateVendorCarrierConfigForSlot(int slotId, Bundle bundle) {
        if (sMe != null) {
            ShannonVendorCarrierConfig.updateVendorCarrierConfigForSlot(slotId, bundle);
        }
    }

    @VisibleForTesting
    protected void onCreate(Context context, ShannonVendorCarrierConfig config,
                            LocationManager locationManager, SimManager simManager) {
        mContext = context;
        mVendorConfig = config;
        mSimManager = simManager;
        mLocationManager = locationManager;
        mVersion = new PackageVersion(getServiceTAG("-Version"), mContext);
        mNetworkProviders = new SparseArray<>();

        sMe = this;
        LogUtils.d(TAG, "ShannonQualifiedNetworksService created");
    }

    @VisibleForTesting
    protected void setVersion(PackageVersion packageVersion) {
        mVersion = packageVersion;
    }

    @VisibleForTesting
    protected void setObject(ShannonQualifiedNetworksService shannonQualifiedNetworksServiceObj) {
        sMe = shannonQualifiedNetworksServiceObj;
    }

    @Override
    public void dump(FileDescriptor fd, PrintWriter printWriter, String[] args) {
        IndentingPrintWriter pw = new IndentingPrintWriter(printWriter, "  ");
        pw.println("Version = " + mVersion);
        pw.flush();

        for (int i = 0; i < mNetworkProviders.size(); i++) {
            int slotId = mNetworkProviders.keyAt(i);
            TelephonyManager tm = mSimManager.getTelephonyManager(slotId);
            if (tm == null) {
                continue;
            }
            String s = "SlotId: " + slotId + ", SubId: " + mSimManager.getSubId(slotId) + ", Carrier ";
            CarrierType carrier = CarrierType.getEnum(tm);
            if (carrier != null) {
                s += carrier;
            } else {
                int carrierId = tm.getSimSpecificCarrierId();
                if (carrierId == TelephonyManager.UNKNOWN_CARRIER_ID) {
                    carrierId = tm.getSimCarrierId();
                    if (carrierId == TelephonyManager.UNKNOWN_CARRIER_ID) {
                        carrierId = tm.getCarrierIdFromSimMccMnc();
                    }
                }
                s += carrierId;
            }
            s += " for MCC=" + mSimManager.getMcc(slotId) + " MNC=" + mSimManager.getMnc(slotId);
            pw.println("---------------------------------------------" +
                       "---------------------------------------------");
            pw.println(s);
            mNetworkProviders.valueAt(i).dump(fd, pw, args);
            pw.flush();
        }
    }
}
