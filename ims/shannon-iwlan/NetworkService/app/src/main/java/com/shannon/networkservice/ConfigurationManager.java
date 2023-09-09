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
package com.shannon.networkservice;

import android.content.Intent;
import android.os.Bundle;
import android.telephony.Rlog;
import android.telephony.TelephonyManager;
import android.text.TextUtils;

import com.android.internal.annotations.VisibleForTesting;
import com.android.internal.telephony.PhoneConstants;

class ConfigurationManager {
    private static final boolean DBG = true;

    /* Custom broadcast */
    private static final String BROADCAST_ACTION_VENDOR_CONFIG_CHANGED = "com.shannon.ACTION.VCC.vendor_config_changed";
    private static final String BROADCAST_EXTRA_VENDOR_CONFIG = "VENDOR_CONFIG";
    /* Vendor prefix */
    private static final String KEY_PREFIX = "vendor.";
    /* VoMobile */
    private static final String KEY_VOMOBILE_ENABLED = KEY_PREFIX + "vomobile_enabled";
    private static final String KEY_VOMOBILE_SUPPORTED_RADIO_TECHNOLOGY = KEY_PREFIX + "vomobile_supported_radio_technology";
    private static final String KEY_VOMOBILE_ONROAMING_ENABLED = KEY_PREFIX + "vomobile_onroaming_enabled";
    private static final String DEFAULT_VOMOBILE_RAT_LIST = "LTE,LTE_CA,NR";

    private final String TAG;
    private final int mSlotId;
    private boolean mVoMobileEnabled;
    private boolean mVoMobileEnabledOnRoaming;
    private String mVoMobileAllowedRat;

    @VisibleForTesting
    class ConfigurationReceiverCallback implements ConfigurationBroadcastReceiver.IConfigurationReceiverCallback {
        @Override
        public void onReceive(Intent intent) {
            final String action = intent.getAction();
            if (action == null) {
                loge("NULL action for intent " + intent);
                return;
            }
            log("Got intent " + intent);
            switch (action) {
                case BROADCAST_ACTION_VENDOR_CONFIG_CHANGED: {
                    int slotId = intent.getIntExtra(PhoneConstants.SLOT_KEY, -1);
                    if (slotId == mSlotId) {
                        Bundle bundle = intent.getBundleExtra(BROADCAST_EXTRA_VENDOR_CONFIG);
                        if (bundle != null) {
                            if (bundle.containsKey(KEY_VOMOBILE_ENABLED)) {
                                mVoMobileEnabled = bundle.getBoolean(KEY_VOMOBILE_ENABLED, false);
                            }
                            if (bundle.containsKey(KEY_VOMOBILE_ONROAMING_ENABLED)) {
                                mVoMobileEnabledOnRoaming = bundle.getBoolean(KEY_VOMOBILE_ONROAMING_ENABLED, false);
                            }
                            if (bundle.containsKey(KEY_VOMOBILE_SUPPORTED_RADIO_TECHNOLOGY)) {
                                mVoMobileAllowedRat = bundle.getString(KEY_VOMOBILE_SUPPORTED_RADIO_TECHNOLOGY, DEFAULT_VOMOBILE_RAT_LIST);
                            }
                            log("Vendor config received for slot " + mSlotId +
                                    ". VoMobile enabled: " + mVoMobileEnabled +
                                    " onRAT: " + mVoMobileAllowedRat +
                                    " enabled on roaming network: " + mVoMobileEnabledOnRoaming);
                            ShannonNetworkService.onConfigurationChanged(mSlotId);
                        }
                    } else {
                        log("slotId : " + slotId + " from bundle is not matched, ignore");
                    }
                    break;
                }
            }
        }
    }

    public ConfigurationManager(int slotId, String tag) {
        mSlotId = slotId;
        TAG = tag + "-CFG";
        mVoMobileEnabled = false;
        mVoMobileEnabledOnRoaming = false;
        mVoMobileAllowedRat = DEFAULT_VOMOBILE_RAT_LIST;

        ConfigurationBroadcastReceiver.registerConfigurationReceiverCallback(mSlotId, new ConfigurationReceiverCallback());
    }

    public void destroy() {
        ConfigurationBroadcastReceiver.unregisterConfigurationReceiverCallback(mSlotId);
    }

    public boolean isVoMobileEnabled() { return mVoMobileEnabled; }
    public boolean isVoMobileEnabledOnRoaming() { return mVoMobileEnabledOnRoaming; }

    public boolean isVoMobileSupportedOnNetwork(int networkType) {
        if (!TextUtils.isEmpty(mVoMobileAllowedRat)) {
            String current = TelephonyManager.getNetworkTypeName(networkType);

            String[] tokens = mVoMobileAllowedRat.split(",");
            for (String s : tokens) {
                if (current.equals(s.trim())) {
                    return true;
                }
            }
        }
        return false;
    }

    private void log(String s) {
        if (DBG) {
            Rlog.d(TAG, s);
        }
    }

    private void loge(String s) {
        Rlog.e(TAG, s);
    }
}
