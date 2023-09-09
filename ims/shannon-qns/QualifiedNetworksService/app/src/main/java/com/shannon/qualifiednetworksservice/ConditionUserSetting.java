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

import com.android.ims.ImsManager;
import com.android.internal.annotations.VisibleForTesting;
import com.shannon.qualifiednetworksservice.util.LogUtils;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.database.ContentObserver;
import android.net.Uri;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;
import android.os.PersistableBundle;
import android.telephony.CarrierConfigManager;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
import android.telephony.data.ApnSetting;
import android.telephony.ims.ProvisioningManager;

import java.lang.reflect.Method;

import static android.Manifest.permission.MODIFY_PHONE_STATE;
import static android.telephony.ims.ImsMmTelManager.WIFI_MODE_WIFI_ONLY;
import static android.telephony.ims.ImsMmTelManager.WIFI_MODE_CELLULAR_PREFERRED;
import static android.telephony.ims.ImsMmTelManager.WIFI_MODE_WIFI_PREFERRED;

import static com.shannon.qualifiednetworksservice.HandoverConditions.ConditionEvents.*;

public class ConditionUserSetting {
    public static final int WIFI_MODE_IMS_PREFERRED = (WIFI_MODE_WIFI_PREFERRED + 1);

    private static final String ACTION_OVERWRITE_WFC_SETTINGS = "com.google.android.wfcactivation.TRY_WFC_CONNECTION";
    private static final String EXTRA_SUB_ID = "SUB_ID";
    private static final String EXTRA_REQUEST_TYPE = "TRY_STATUS";
    private static final int OVERWRITE_WFC_REQUEST = 1;

    private static final String KEY_CARRIER_CROSS_SIM_ENABLED_BOOL = "cross_sim_calling_feature_enabled_bool";
    private static final String KER_CARRIER_CROSS_SIM_REMOVED_APNS_ARRAY = "cross_sim_calling_exempt_apn_types_int_array";
    private static final String COLUMN_CROSS_SIM_CALLING_ENABLED = "cross_sim_calling_enabled";

    protected class UserSettingBroadcastReceiver extends BroadcastReceiver {
        @Override
        public void onReceive(Context context, Intent intent) {
            final String action = intent.getAction();
            if (action == null) {
                LogUtils.w(TAG, "NULL action for intent " + intent);
                return;
            }
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "Got intent " + intent);
            switch (action) {
                case CarrierConfigManager.ACTION_CARRIER_CONFIG_CHANGED: {
                    int slotId = intent.getIntExtra(CarrierConfigManager.EXTRA_SLOT_INDEX, -1);
                    if (mSlotId != slotId) {
                        break;
                    }
                    boolean isVolteEnabled = mImsManager.isEnhanced4gLteModeSettingEnabledByUser();
                    boolean isVolteSupported = mImsManager.isVolteEnabledByPlatform();
                    boolean isWfcEnabled = mImsManager.isWfcEnabledByUser();
                    boolean isWfcEnabledRoaming = mImsManager.isWfcRoamingEnabledByUser();
                    boolean isWfcSupported = mImsManager.isWfcEnabledByPlatform();
                    int mode = getWfcMode();
                    LogUtils.i(TAG, LogUtils.LOG_CONDITIONS,
                            "Carrier config changed. VoLTE supported: " + isVolteSupported + " enabled: " + isVolteEnabled +
                        " VoWIFI supported: " + isWfcSupported + " enabled: " + isWfcEnabled + " (roam: " + isWfcEnabledRoaming +
                            ") mode: " + getWfcMode(mode));

                    boolean updated = updateVoMobileSettings();
                    if (mVolteEnabledByPlatform != isVolteSupported) {
                        mVolteEnabledByPlatform = isVolteSupported; updated = true;
                    }
                    if (mVolteEnabledByUser != isVolteEnabled) {
                        mVolteEnabledByUser = isVolteEnabled; updated = true;
                    }
                    if (mWfcEnabledByPlatform != isWfcSupported) {
                        mWfcEnabledByPlatform = isWfcSupported; updated = true;
                    }
                    if (mWfcEnabledByUser != isWfcEnabled) {
                        mWfcEnabledByUser = isWfcEnabled; updated = true;
                    }
                    if (mWfcEnabledByUserRoaming != isWfcEnabledRoaming) {
                        mWfcEnabledByUserRoaming = isWfcEnabledRoaming; updated = true;
                    }
                    if (mWfcMode != mode) {
                        mWfcMode = mode; updated = true;
                    }
                    /* update if required */
                    if (updated) {
                        mChangedCallback.onConditionChanged(EVENT_CARRIER_CONFIG_CHANGED);
                    }
                    break;
                }
                case ACTION_OVERWRITE_WFC_SETTINGS: {
                    int subId = intent.getIntExtra(EXTRA_SUB_ID, -1);
                    if (mSubId != subId) {
                        break;
                    }
                    int request = intent.getIntExtra(EXTRA_REQUEST_TYPE, 0);
                    boolean newState = (request == OVERWRITE_WFC_REQUEST);
                    if (newState != mWfcEnabledByUserOverride) {
                        mWfcEnabledByUserOverride = newState;
                        mChangedCallback.onConditionChanged(EVENT_WFC_SETTINGS_BROADCAST);
                    }
                    break;
                }
                default:;
            }
        }
    }

    protected class UserSettingContentObserver extends ContentObserver {
        UserSettingContentObserver(Handler h) {
            super(h);
        }

        @Override
        public void onChange(boolean selfChange, Uri uri) {
            LogUtils.i(TAG, LogUtils.LOG_CONDITIONS, "onChange with " + uri);
            if (mUriWfcEnabled.equals(uri)) {
                boolean isWfcEnabled = mImsManager.isWfcEnabledByUser();
                if (mWfcEnabledByUser != isWfcEnabled) {
                    LogUtils.d(TAG, LogUtils.LOG_CONDITIONS,
                            "Change for VoWIFI enabled: " + isWfcEnabled);
                    mWfcEnabledByUser = isWfcEnabled;
                    mChangedCallback.onConditionChanged(EVENT_WFC_SETTINGS_CHANGED);
                }
            } else if (mUriWfcEnabledRoaming.equals(uri)) {
                boolean isWfcEnabled = mImsManager.isWfcRoamingEnabledByUser();
                /* do not update roaming value here, it should be received via provisioning */
                LogUtils.d(TAG, LogUtils.LOG_CONDITIONS,
                        "Change for VoWIFI enabled (roam): " + isWfcEnabled);
                mChangedCallback.onConditionChanged(EVENT_WFC_SETTINGS_CHANGED);
            } else if (mUriWfcMode.equals(uri) || mUriWfcModeRoaming.equals(uri)) {
                int mode = getWfcMode();
                if (mWfcMode != mode) {
                    LogUtils.d(TAG, LogUtils.LOG_CONDITIONS,
                            "Change for WFC mode: " + getWfcMode(mode));
                    mWfcMode = mode;
                    mChangedCallback.onConditionChanged(EVENT_WFC_MODE_CHANGED);
                }
            } else if (mUriButton4G.equals(uri)) {
                boolean isVolteEnabled = mImsManager.isEnhanced4gLteModeSettingEnabledByUser();
                if (mVolteEnabledByUser != isVolteEnabled) {
                    LogUtils.d(TAG, LogUtils.LOG_CONDITIONS,
                            "Change for VoLTE enabled: " + isVolteEnabled);
                    mVolteEnabledByUser = isVolteEnabled;
                    mChangedCallback.onConditionChanged(EVENT_VLT_SETTINGS_CHANGED);
                }
            } else if (mUriButtonVoMobile.equals(uri)) {
                boolean isVoMobileEnabledByUser = isVoMobileEnabledByUser();
                if (mVoMobileEnabledByUser != isVoMobileEnabledByUser) {
                    LogUtils.d(TAG, LogUtils.LOG_CONDITIONS,
                            "Change for VoMobile enabled: " + isVoMobileEnabledByUser);
                    mVoMobileEnabledByUser = isVoMobileEnabledByUser;
                    mChangedCallback.onConditionChanged(EVENT_CST_SETTINGS_CHANGED);
                }
            }
        }
    }

    private final String TAG;
    private final int mSlotId;
    private final int mSubId;
    private final Context mContext;
    private final ConfigurationManager mConfigurationManager;
    private final HandoverConditions.IConditionChanged mChangedCallback;
    private final HandoverConditions.IConditionHelper mConditionsHelper;
    private final ImsManager mImsManager;
    private final UserSettingBroadcastReceiver mBroadcastReceiver;
    private final HandlerThread mHandlerThread;
    private final UserSettingContentObserver mContentObserver;
    private final Uri mUriWfcEnabled;
    private final Uri mUriWfcEnabledRoaming;
    private final Uri mUriWfcMode;
    private final Uri mUriWfcModeRoaming;
    private final Uri mUriButton4G;
    private final Uri mUriButtonVoMobile;

    private boolean mVolteEnabledByUser;
    private boolean mVolteEnabledByPlatform;
    private boolean mWfcEnabledByUser; /* home */
    private boolean mWfcEnabledByUserOverride;
    private boolean mWfcEnabledByUserRoaming;
    private boolean mWfcEnabledByPlatform;
    private int mWfcMode;
    private boolean mVoMobileEnabledByUser;
    private boolean mVoMobileEnabledByPlatform;
    private int mVoMobileEnabledApnBitmask;

    public ConditionUserSetting(int slotIndex, Context context,
                                ConfigurationManager configurationManager,
                                HandoverConditions.IConditionHelper helper,
                                HandoverConditions.IConditionChanged callback) {
        SimManager simManager = SimManager.getInstance();

        mSlotId = slotIndex;
        mSubId = simManager.getSubId(mSlotId);
        mContext = context;
        mConfigurationManager = configurationManager;
        mConditionsHelper = helper;
        mChangedCallback = callback;
        mImsManager = ImsManager.getInstance(mContext, mSlotId);
        mBroadcastReceiver = new UserSettingBroadcastReceiver();

        TAG = ShannonQualifiedNetworksService.getServiceTAG("-" + mSlotId + "-HC-user");

        mHandlerThread = new HandlerThread(ConditionUserSetting.class.getSimpleName());
        mHandlerThread.start();
        Looper looper = mHandlerThread.getLooper();
        Handler handler = new Handler(looper);
        mContentObserver = new UserSettingContentObserver(handler);

        mVolteEnabledByUser = mImsManager.isEnhanced4gLteModeSettingEnabledByUser();
        mVolteEnabledByPlatform = mImsManager.isVolteEnabledByPlatform();
        mWfcEnabledByUser = mImsManager.isWfcEnabledByUser();
        mWfcEnabledByPlatform = mImsManager.isWfcEnabledByPlatform();
        mWfcEnabledByUserOverride = false;
        mWfcEnabledByUserRoaming = mImsManager.isWfcRoamingEnabledByUser();
        mWfcMode = getWfcMode();
        mVoMobileEnabledByPlatform = true;
        mVoMobileEnabledApnBitmask = ApnType.getApnBitmask();
        mVoMobileEnabledByUser = isVoMobileEnabledByUser();

        IntentFilter filter = new IntentFilter();
        filter.addAction(CarrierConfigManager.ACTION_CARRIER_CONFIG_CHANGED);
        filter.addAction(ACTION_OVERWRITE_WFC_SETTINGS);
        mContext.registerReceiver(mBroadcastReceiver, filter, MODIFY_PHONE_STATE, null);

        String subId = String.valueOf(mSubId);
        mUriWfcEnabled = Uri.withAppendedPath(SubscriptionManager.WFC_ENABLED_CONTENT_URI, subId);
        mUriWfcEnabledRoaming = Uri.withAppendedPath(SubscriptionManager.WFC_ROAMING_ENABLED_CONTENT_URI, subId);
        mUriWfcMode = Uri.withAppendedPath(SubscriptionManager.WFC_MODE_CONTENT_URI, subId);
        mUriWfcModeRoaming = Uri.withAppendedPath(SubscriptionManager.WFC_ROAMING_MODE_CONTENT_URI, subId);
        mUriButton4G = Uri.withAppendedPath(SubscriptionManager.ADVANCED_CALLING_ENABLED_CONTENT_URI, subId);
        mUriButtonVoMobile = Uri.withAppendedPath(Uri.withAppendedPath(SubscriptionManager.CONTENT_URI, COLUMN_CROSS_SIM_CALLING_ENABLED), subId);
        mContext.getContentResolver().registerContentObserver(mUriWfcEnabled, true, mContentObserver);
        mContext.getContentResolver().registerContentObserver(mUriWfcEnabledRoaming, true, mContentObserver);
        mContext.getContentResolver().registerContentObserver(mUriWfcMode, true, mContentObserver);
        mContext.getContentResolver().registerContentObserver(mUriWfcModeRoaming, true, mContentObserver);
        mContext.getContentResolver().registerContentObserver(mUriButton4G, true, mContentObserver);
        mContext.getContentResolver().registerContentObserver(mUriButtonVoMobile, true, mContentObserver);
    }

    public void destroy() {
        mContext.getContentResolver().unregisterContentObserver(mContentObserver);
        mContext.unregisterReceiver(mBroadcastReceiver);
        mHandlerThread.quit();
    }

    public String getWfcMode(int mode) {
        switch (mode) {
            case WIFI_MODE_WIFI_ONLY: return "WIFI_ONLY";
            case WIFI_MODE_CELLULAR_PREFERRED: return "CELLULAR_PREFERRED";
            case WIFI_MODE_WIFI_PREFERRED: return "WIFI_PREFERRED";
            case WIFI_MODE_IMS_PREFERRED: return "IMS_PREFERRED";
            default:;
        }
        return "UNKNOWN";
    }

    public boolean isWfcEnabled() { return ((getWfcEnabled() || mWfcEnabledByUserOverride) && mWfcEnabledByPlatform); }
    public boolean isWiFiPreferred() { return (mWfcEnabledByUserOverride || (mWfcMode == WIFI_MODE_WIFI_PREFERRED)); }
    public int getWfcModeEnum() { return mWfcMode; }
    public boolean isVolteEnabled() { return (mVolteEnabledByUser && mVolteEnabledByPlatform); }
    public boolean isWfcEnabledByPlatform() { return mWfcEnabledByPlatform; }
    public boolean isWfcEnabledByUser() { return getWfcEnabled(); }

    private boolean getWfcEnabled() {
        if (mConfigurationManager.canUseWfcRoamingSettings() && mConditionsHelper.isRoaming()) {
            /* *
            * Android roaming WFC settings implementation is not proper,
            * isWfcRoamingEnabledByUser will return same (default) value,
            * regardless of user settings.
            *
            * Use provisioning value, it will be updated on user changes.
            * */
            return mWfcEnabledByUserRoaming;
        }
        return mWfcEnabledByUser;
    }

    private int getWfcMode() {
        return mImsManager.getWfcMode(mConditionsHelper.isRoaming());
    }

    public void setWfcEnabledRoaming(boolean value) {
        if (mWfcEnabledByUserRoaming != value) {
            mWfcEnabledByUserRoaming = value;
            mChangedCallback.onConditionChanged(EVENT_WFC_SETTINGS_CHANGED);
        }
    }

    public boolean isVoMobileApnTypeEnabled(int type) {
        boolean enabled = ((mVoMobileEnabledApnBitmask & type) != 0);
        LogUtils.v(TAG, type,
                "VoMobile " + (enabled ? "enabled" : "disabled") + " for APN " + ApnSetting.getApnTypeString(type));
        return enabled;
    }


    public boolean isVoMobileEnabledByUser() {
        boolean isEnabledByUser;
        try {
            Method method = ImsManager.class.getMethod("isCrossSimCallingEnabledByUser", (Class<?>[]) null);
            isEnabledByUser = (boolean)method.invoke(mImsManager, (Object[]) null);
        } catch (Exception e) {
            isEnabledByUser = mConfigurationManager.isVoMobileEnabled();
        }
        LogUtils.d(TAG, LogUtils.LOG_CONDITIONS, "VoMobile enabled by user: " + isEnabledByUser);
        return isEnabledByUser;
    }

    public boolean isVoMobileEnabled() {
        return (mVoMobileEnabledByPlatform & isVoMobileEnabledByUser());
    }

    private boolean updateVoMobileSettings() {
        boolean updated = false;

        CarrierConfigManager ccm = mContext.getSystemService(CarrierConfigManager.class);
        if (ccm != null) {
            // If an invalid subId is used, this bundle will contain default values.
            PersistableBundle bundle = ccm.getConfigForSubId(mSubId);
            if ((bundle != null) &&
                    CarrierConfigManager.isConfigForIdentifiedCarrier(bundle)) {
                boolean boolValue = bundle.getBoolean(KEY_CARRIER_CROSS_SIM_ENABLED_BOOL, true);
                if (boolValue != mVoMobileEnabledByPlatform) {
                    mVoMobileEnabledByPlatform = boolValue; updated = true;
                }
                int[] intArray = bundle.getIntArray(KER_CARRIER_CROSS_SIM_REMOVED_APNS_ARRAY);
                if (intArray != null && intArray.length > 0) {
                    int bitmask = 0;
                    for (int i : intArray) {
                        bitmask |= i;
                    }
                    int bitmaskValue = (mVoMobileEnabledApnBitmask & ~bitmask);
                    if (bitmaskValue != mVoMobileEnabledApnBitmask) {
                        mVoMobileEnabledApnBitmask = bitmaskValue; updated = true;
                    }
                }
            }
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS,
                    "VoMobile enabled by platform: " + mVoMobileEnabledByPlatform +
                            " APNs bitmask = " + mVoMobileEnabledApnBitmask);
        }
        return updated;
    }

    @Override
    public String toString() {
        return ("USER" + mSlotId + ". VoLTE supported: " + mVolteEnabledByPlatform + " enabled: " + mVolteEnabledByUser +
                " VoWIFI supported: " + mWfcEnabledByPlatform + " enabled: " + getWfcEnabled() + " mode: " + getWfcMode(mWfcMode) +
                " override: " + mWfcEnabledByUserOverride + " VoMobile supported: " + mVoMobileEnabledByPlatform);
    }

    @VisibleForTesting
    protected void setVoMobileEnabledByPlatform(boolean voMobileEnabledByPlatform) {
        mVoMobileEnabledByPlatform = voMobileEnabledByPlatform;
    }

    @VisibleForTesting
    protected void setWfcEnabledByUserOverride(boolean wfcEnabledByUserOverride) {
        mWfcEnabledByUserOverride = wfcEnabledByUserOverride;
    }
}
