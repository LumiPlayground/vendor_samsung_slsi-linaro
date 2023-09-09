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

import android.content.Context;
import android.telephony.TelephonyManager;
import android.telephony.data.ApnSetting;
import android.telephony.ims.ProvisioningManager;
import android.text.TextUtils;

import com.android.ims.ImsManager;
import com.android.internal.annotations.VisibleForTesting;
import com.shannon.qualifiednetworksservice.util.LogUtils;

import java.util.Timer;
import java.util.TimerTask;
import java.util.concurrent.Executors;

import static com.shannon.qualifiednetworksservice.HandoverConditions.ConditionEvents.*;
import static com.shannon.vendorcarrierconfig.ShannonVendorCarrierConfig.KEY_IMS_HANDOVER_HYSTERESIS_TIMER_VOLTE;
import static com.shannon.vendorcarrierconfig.ShannonVendorCarrierConfig.KEY_IMS_HANDOVER_HYSTERESIS_TIMER_VOWIFI;
import static com.shannon.vendorcarrierconfig.ShannonVendorCarrierConfig.KEY_VOMOBILE_ENABLED;

public class ConditionProvisioning {
    private static final int IMS_CONFIG_READ_RETRY_TIMER = 1000;
    private static final int IMS_CONFIG_VOMOBILE_ENABLE = 802;

    protected class ProvisioningCallBack extends ProvisioningManager.Callback {
        public void onProvisioningIntChanged(int item, int value) {
            boolean updated = false;

            LogUtils.i(TAG, LogUtils.LOG_CONDITIONS,
                    "onProvisioningIntChanged item=" + item + " value=" + value);
            switch (item) {
                case ProvisioningManager.KEY_VOLTE_PROVISIONING_STATUS: {
                    boolean newValue = mImsManager.isVolteProvisionedOnDevice();

                    updated = (newValue != mVoLteProvisioned);
                    if (updated) {
                        mVoLteProvisioned = newValue;
                        LogUtils.d(TAG, LogUtils.LOG_CONDITIONS,
                                "VoLTE provisioned changed to " + mVoLteProvisioned);
                    }
                    break;
                }
                case ProvisioningManager.KEY_VOICE_OVER_WIFI_ENABLED_OVERRIDE: {
                    boolean newValue = mImsManager.isWfcProvisionedOnDevice();

                    updated = (newValue != mWfcProvisioned);
                    if (updated) {
                        mWfcProvisioned = newValue;
                        LogUtils.d(TAG, LogUtils.LOG_CONDITIONS,
                                "WFC provisioned changed to " + mWfcProvisioned);
                    }
                    break;
                }
                case ProvisioningManager.KEY_LTE_THRESHOLD_1: {
                    updated = (value != mConditionsHelper.getRsrpRoveOut());
                    if (updated) {
                        LogUtils.d(TAG, LogUtils.LOG_CONDITIONS,
                                "TH_LTE1 changed to " + value);
                        mConditionsHelper.setRsrpRoveOut(value);
                    }
                    break;
                }
                case ProvisioningManager.KEY_LTE_THRESHOLD_2: {
                    updated = (value != mConditionsHelper.getRsrpMid());
                    if (updated) {
                        LogUtils.d(TAG, LogUtils.LOG_CONDITIONS,
                                "TH_LTE2 changed to " + value);
                        mConditionsHelper.setRsrpMid(value);
                    }
                    break;
                }
                case ProvisioningManager.KEY_LTE_THRESHOLD_3: {
                    updated = (value != mConditionsHelper.getRsrpRoveIn());
                    if (updated) {
                        LogUtils.d(TAG, LogUtils.LOG_CONDITIONS,
                                "TH_LTE3 changed to " + value);
                        mConditionsHelper.setRsrpRoveIn(value);
                    }
                    break;
                }
                case ProvisioningManager.KEY_WIFI_THRESHOLD_A: {
                    updated = (value != mConditionsHelper.getWiFiRssiRoveIn());
                    if (updated) {
                        LogUtils.d(TAG, LogUtils.LOG_CONDITIONS,
                                "VOWT_A changed to " + value);
                        mConditionsHelper.setWiFiRssiRoveIn(value);
                    }
                    break;
                }
                case ProvisioningManager.KEY_WIFI_THRESHOLD_B: {
                    updated = (value != mConditionsHelper.getWiFiRssiRoveOut());
                    if (updated) {
                        LogUtils.d(TAG, LogUtils.LOG_CONDITIONS,
                                "VOWT_B changed to " + value);
                        mConditionsHelper.setWiFiRssiRoveOut(value);
                    }
                    break;
                }
                case ProvisioningManager.KEY_LTE_EPDG_TIMER_SEC: {
                    updated = (value != mConfigurationManager.getInt(KEY_IMS_HANDOVER_HYSTERESIS_TIMER_VOLTE, -1));
                    if (updated) {
                        LogUtils.d(TAG, LogUtils.LOG_CONDITIONS,
                                "T_EPDG_LTE changed to " + value);
                        mConfigurationManager.putInt(KEY_IMS_HANDOVER_HYSTERESIS_TIMER_VOLTE, value);
                    }
                    break;
                }
                case ProvisioningManager.KEY_WIFI_EPDG_TIMER_SEC: {
                    updated = (value != mConfigurationManager.getInt(KEY_IMS_HANDOVER_HYSTERESIS_TIMER_VOWIFI, -1));
                    if (updated) {
                        LogUtils.d(TAG, LogUtils.LOG_CONDITIONS,
                                "T_EPDG_WIFI changed to " + value);
                        mConfigurationManager.putInt(KEY_IMS_HANDOVER_HYSTERESIS_TIMER_VOWIFI, value);
                    }
                    break;
                }
                case ProvisioningManager.KEY_VOICE_OVER_WIFI_ROAMING_ENABLED_OVERRIDE: {
                    LogUtils.d(TAG, LogUtils.LOG_CONDITIONS,
                            "VOICE_OVER_WIFI_ROAMING_ENABLED changed to " + value);
                    mConditionsHelper.setWfcEnabledRoaming(toBoolean(value));
                    break;
                }
                case IMS_CONFIG_VOMOBILE_ENABLE: {
                    updated = (toBoolean(value) != mConfigurationManager.getBoolean(KEY_VOMOBILE_ENABLED, mVoMobileEnabled));
                    if (updated) {
                        LogUtils.d(TAG, LogUtils.LOG_CONDITIONS,
                                "VoMobile switch changed to " + value);
                        mConfigurationManager.putBooleanPersisted(KEY_VOMOBILE_ENABLED, toBoolean(value));
                    }
                    return;
                }
                default:;
            }
            /* update if required */
            if (updated) {
                mChangedCallback.onConditionChanged(EVENT_PROVISIONED_VALUE_CHANGED);
            }
        }

        public void onProvisioningStringChanged(int item, String value) {
            boolean updated = false;

            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS,
                    "onProvisioningStringChanged item=" + item + " value=" + value);
            switch (item) {
                case ProvisioningManager.KEY_VOICE_OVER_WIFI_ENTITLEMENT_ID: {
                    if (value != null) {
                        updated = !value.equals(mOemWfcSettings);
                    } else if (mOemWfcSettings != null) {
                        updated = true;
                    }
                    if (updated) {
                        mOemWfcSettings = value;
                        LogUtils.d(TAG, LogUtils.LOG_CONDITIONS,
                                "OEM provisioned value changed to " + getOemWfcSettingsString());
                    }
                    break;
                }
                default:;
            }
            /* update if required */
            if (updated) {
                mChangedCallback.onConditionChanged(EVENT_PROVISIONED_VALUE_CHANGED);
            }
        }
    }

    private final String TAG;
    private final int mSlotId;
    private final TelephonyManager mTelephonyManager;
    private final HandoverConditions.IConditionChanged mChangedCallback;
    private final HandoverConditions.IConditionHelper mConditionsHelper;
    private final ImsManager mImsManager;
    private final ProvisioningManager mProvisioningManager;
    private final ProvisioningCallBack mProvisioningCallback;
    private final ConfigurationManager mConfigurationManager;
    private Timer mRetryTimer;

    /* standard provisioned parameters */
    private boolean mVoLteProvisioned;
    private boolean mWfcProvisioned;

    /* OEM provisioning parameter */
    private String mOemWfcSettings;

    /* VoMobile enabled */
    private boolean mVoMobileEnabled;

    public ConditionProvisioning(int slotIndex, Context context, ConfigurationManager configurationManager,
                          HandoverConditions.IConditionHelper helper, HandoverConditions.IConditionChanged callback) {
        mSlotId = slotIndex;
        SimManager mSimManager = SimManager.getInstance();
        mTelephonyManager = mSimManager.getTelephonyManager(mSlotId);
        mConditionsHelper = helper;
        mChangedCallback = callback;
        mConfigurationManager = configurationManager;
        mImsManager = ImsManager.getInstance(context, slotIndex);
        int subId = mSimManager.getSubId(mSlotId);
        mProvisioningManager = ProvisioningManager.createForSubscriptionId(subId);
        mProvisioningCallback = new ProvisioningCallBack();
        mRetryTimer = null;

        TAG = ShannonQualifiedNetworksService.getServiceTAG("-" + mSlotId + "-HC-prv");

        if (isImsConfigValid()) {
            getDefaultProvisioningValues();
        } else {
            mVoLteProvisioned = mImsManager.isVolteProvisionedOnDevice();
            mWfcProvisioned = mImsManager.isWfcProvisionedOnDevice();
            mOemWfcSettings = null;
            mVoMobileEnabled = mConfigurationManager.getBoolean(KEY_VOMOBILE_ENABLED, false);
            retryAttach(IMS_CONFIG_READ_RETRY_TIMER);
        }
    }

    public void destroy() {
        if (mRetryTimer != null) {
            mRetryTimer.cancel();
        }
        removeProvisioningCallback();
    }

    public boolean isVoLteProvisioned() { return mVoLteProvisioned; }
    public boolean isWfcProvisioned() { return mWfcProvisioned; }
    public boolean isOemProvisioned() { return !TextUtils.isEmpty(mOemWfcSettings); }

    public boolean isMdnProvisioned() {
        try {
            String mdn = mTelephonyManager.getLine1Number();
            if (mdn != null && mOemWfcSettings != null) {
                boolean res = mdn.equals(mOemWfcSettings);
                LogUtils.v(TAG, LogUtils.LOG_CONDITIONS,
                        "MDN is " + (res ? "same" : "not same") + " to VoWiFi_MDN");
                return res;
            }
        } catch (Exception e) {
            LogUtils.e(TAG, "Failed to check MDN");
        }
        return false;
    }

    private boolean toBoolean(int value) {
        return (value == ProvisioningManager.PROVISIONING_VALUE_ENABLED);
    }

    private int getProvisioningInt(int item) {
        int result = ProvisioningManager.PROVISIONING_RESULT_UNKNOWN;
        try {
            result = mProvisioningManager.getProvisioningIntValue(item);
        } catch (Exception e) {
            LogUtils.e(TAG, "Failed to read item " + item + " : " + e);
        }
        return result;
    }

    private String getProvisioningString(int item) {
        String result = null;
        try {
            result = mProvisioningManager.getProvisioningStringValue(item);
        } catch (Exception e) {
            LogUtils.e(TAG, "Failed to read item " + item + " : " + e);
        }
        return result;
    }

    private void addProvisioningCallback() {
        try {
            mProvisioningManager.registerProvisioningChangedCallback(
                    Executors.newSingleThreadExecutor(), mProvisioningCallback);
        } catch (Exception e) {
            LogUtils.e(TAG, "Unable to add ProvisioningCallback : " + e);
        }
    }

    private void removeProvisioningCallback() {
        try {
            mProvisioningManager.unregisterProvisioningChangedCallback(mProvisioningCallback);
        } catch (Exception e) {
            LogUtils.e(TAG, "Unable to remove ProvisioningCallback : " + e);
        }
    }

    private boolean isImsConfigValid() {
        boolean result = false;
        try {
            result = (mImsManager.getConfigInterface() != null);
        } catch (Exception e) {
            LogUtils.e(TAG, "Could not get ImsConfig : " + e);
        }
        return result;
    }

    private void getDefaultProvisioningValues() {
        mVoLteProvisioned = mImsManager.isVolteProvisionedOnDevice();
        mWfcProvisioned = mImsManager.isWfcProvisionedOnDevice();
        mOemWfcSettings = getProvisioningString(ProvisioningManager.KEY_VOICE_OVER_WIFI_ENTITLEMENT_ID);
        mVoMobileEnabled = toBoolean(getProvisioningInt(IMS_CONFIG_VOMOBILE_ENABLE));

        /* update configuration */
        int value = getProvisioningInt(ProvisioningManager.KEY_LTE_THRESHOLD_1);
        if (value != ProvisioningManager.PROVISIONING_RESULT_UNKNOWN &&
                value != ProvisioningManager.PROVISIONING_VALUE_DISABLED) {
            mConditionsHelper.setRsrpRoveOut(value);
            LogUtils.d(TAG, LogUtils.LOG_CONDITIONS, "TH_LTE1 updated to " + value);
        }
        value = getProvisioningInt(ProvisioningManager.KEY_LTE_THRESHOLD_2);
        if (value != ProvisioningManager.PROVISIONING_RESULT_UNKNOWN &&
                value != ProvisioningManager.PROVISIONING_VALUE_DISABLED) {
            mConditionsHelper.setRsrpMid(value);
            LogUtils.d(TAG, LogUtils.LOG_CONDITIONS, "TH_LTE2 updated to " + value);
        }
        value = getProvisioningInt(ProvisioningManager.KEY_LTE_THRESHOLD_3);
        if (value != ProvisioningManager.PROVISIONING_RESULT_UNKNOWN &&
                value != ProvisioningManager.PROVISIONING_VALUE_DISABLED) {
            mConditionsHelper.setRsrpRoveIn(value);
            LogUtils.d(TAG, LogUtils.LOG_CONDITIONS, "TH_LTE3 updated to " + value);
        }
        value = getProvisioningInt(ProvisioningManager.KEY_WIFI_THRESHOLD_A);
        if (value != ProvisioningManager.PROVISIONING_RESULT_UNKNOWN &&
            value != ProvisioningManager.PROVISIONING_VALUE_DISABLED) {
            mConditionsHelper.setWiFiRssiRoveIn(value);
            LogUtils.d(TAG, LogUtils.LOG_CONDITIONS, "VOWT_A updated to " + value);
        }
        value = getProvisioningInt(ProvisioningManager.KEY_WIFI_THRESHOLD_B);
        if (value != ProvisioningManager.PROVISIONING_RESULT_UNKNOWN &&
                value != ProvisioningManager.PROVISIONING_VALUE_DISABLED) {
            mConditionsHelper.setWiFiRssiRoveOut(value);
            LogUtils.d(TAG, LogUtils.LOG_CONDITIONS, "VOWT_B updated to " + value);
        }
        value = getProvisioningInt(ProvisioningManager.KEY_VOICE_OVER_WIFI_ROAMING_ENABLED_OVERRIDE);
        if (value != ProvisioningManager.PROVISIONING_RESULT_UNKNOWN) {
            mConditionsHelper.setWfcEnabledRoaming(toBoolean(value));
            LogUtils.d(TAG, LogUtils.LOG_CONDITIONS,
                    "VOICE_OVER_WIFI_ROAMING_ENABLED updated to " + value);
        }
        setProvisioningIntValue(ProvisioningManager.KEY_LTE_EPDG_TIMER_SEC,
                KEY_IMS_HANDOVER_HYSTERESIS_TIMER_VOLTE, "T_EPDG_LTE updated to ");
        setProvisioningIntValue(ProvisioningManager.KEY_WIFI_EPDG_TIMER_SEC,
                KEY_IMS_HANDOVER_HYSTERESIS_TIMER_VOWIFI, "T_EPDG_WIFI updated to ");

        addProvisioningCallback();
    }

    private void setProvisioningIntValue(int item, String key, String outString) {
        int value = getProvisioningInt(item);
        if (value != ProvisioningManager.PROVISIONING_RESULT_UNKNOWN &&
            value != ProvisioningManager.PROVISIONING_VALUE_DISABLED) {
            mConfigurationManager.putInt(key, value);
            LogUtils.d(TAG, LogUtils.LOG_CONDITIONS, outString + value);
        }
    }

    private void retryAttach(final int timerValue) {
        LogUtils.d(TAG, LogUtils.LOG_CONDITIONS,
                "ImsConfig not available, retry after " + timerValue + " ms");

        mRetryTimer = new Timer();
        mRetryTimer.schedule(new TimerTask() {
            @Override
            public void run() {
                if (isImsConfigValid()) {
                    LogUtils.v(TAG, LogUtils.LOG_CONDITIONS,
                            "ImsConfig available, read provisioning values");
                    getDefaultProvisioningValues();
                    // notify IMS decision
                    mChangedCallback.onConditionChanged(ApnSetting.TYPE_IMS, EVENT_IMS_CONFIG_CONNECTED);
                } else {
                    retryAttach(timerValue);
                }
            }
        }, timerValue);
    }

    private String getOemWfcSettingsString() {
        if (TextUtils.isEmpty(mOemWfcSettings)) {
            return "null";
        }
        return "****";
    }

    @Override
    public String toString() {
        return ("PRVSN" + mSlotId + ": VoLTE=" + mVoLteProvisioned + " WFC=" + mWfcProvisioned + " OEM=" + getOemWfcSettingsString());
    }

    @VisibleForTesting
    ProvisioningCallBack getProvisioningCallback() { return mProvisioningCallback; }
}
