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
package com.shannon.dataservice;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.telephony.Rlog;
import android.telephony.TelephonyManager;
import android.text.TextUtils;

import com.android.internal.annotations.VisibleForTesting;
import com.android.internal.telephony.PhoneConstants;

import com.shannon.dataservice.ConfigurationBroadcastReceiver.IConfigurationReceiverCallback;

import static com.shannon.dataservice.ConfigurationParameters.KEY_EPDG_CONNECTION_RETRY_PARAMETERS;
import static com.shannon.dataservice.ConfigurationParameters.KEY_EPDG_SUPPORTED_PCO_ID_LIST;
import static com.shannon.dataservice.ConfigurationParameters.KEY_VOMOBILE_ENABLED;
import static com.shannon.dataservice.ConfigurationParameters.KEY_VOMOBILE_ONROAMING_ENABLED;
import static com.shannon.dataservice.ConfigurationParameters.KEY_VOMOBILE_SUPPORTED_RADIO_TECHNOLOGY;
import static com.shannon.dataservice.ConfigurationParameters.KEY_WIFI_ACCESS_POINTS_HANDOVER_SUPPORTED;
import static com.shannon.dataservice.ConfigurationParameters.KEY_WIFI_ACCESS_POINTS_HANDOVER_TIMEOUT;

public class ConfigurationManager {
    private static final boolean DBG = true;

    /* custom broadcasts */
    public static final String BROADCAST_ACTION_REQUEST_VENDOR_CONFIG = "com.shannon.ACTION.VCC.request_vendor_config";
    public static final String BROADCAST_ACTION_VENDOR_CONFIG_CHANGED = "com.shannon.ACTION.VCC.vendor_config_changed";
    public static final String BROADCAST_EXTRA_VENDOR_CONFIG = "VENDOR_CONFIG";

    private static final String DEFAULT_VOMOBILE_RAT_LIST = "LTE,LTE_CA,NR";

    private final String TAG;
    private final int mSlotId;
    private Context mContext;
    private Bundle mBundle;
    private final ConfigurationReceiverCallback mConfigurationReceiverCallback;

    private class ConfigurationReceiverCallback implements IConfigurationReceiverCallback {
        @Override
        public void onReceive(Intent intent) {
            handleVendorConfigChanged(intent);
        }
    }

    public ConfigurationManager(Context context, int slotId) {
        mContext = context;
        mSlotId = slotId;
        mBundle = null;

        TAG = ShannonDataService.getServiceTAG("-" + slotId + "-CFG");
        mConfigurationReceiverCallback = new ConfigurationReceiverCallback();
        ConfigurationBroadcastReceiver.registerConfigurationReceiverCallback(mSlotId, mConfigurationReceiverCallback);

        try {
            Intent intent = new Intent();
            intent.setPackage("com.shannon.qualifiednetworksservice");
            intent.setAction(BROADCAST_ACTION_REQUEST_VENDOR_CONFIG);
            intent.putExtra(PhoneConstants.SLOT_KEY, mSlotId);
            mContext.sendBroadcast(intent);
            log("Send intent=" + intent + "to QNS");
        } catch (Exception e) {
            loge("Failed to send a broadcast to QNS" + e);
        }
    }

    public void destroy() {
        ConfigurationBroadcastReceiver.unregisterConfigurationReceiverCallback(mSlotId);
    }

    public void handleVendorConfigChanged(Intent intent) {
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
                        log("Vendor config received for slot " + mSlotId);
                        if (mBundle == null) {
                            mBundle = new Bundle(bundle);
                        } else {
                            updateBundleNonOverlap(mBundle, bundle);
                        }
                    }
                }
                break;
            }
        }
    }

    public ConfigurationParameters getConnectionConfiguration() {
        ConfigurationParameters params = new ConfigurationParameters();
        if (mBundle != null) {
            for (ConfigurationKey key : ConfigurationKey.values()) {
                if (mBundle.containsKey(key.getKeyName())) {
                    Object obj = mBundle.get(key.getKeyName());
                    if (obj != null) {
                        key.setToObject(params, obj);
                    }
                }
            }
        }
        else {
            log("Bundle is null!");
        }
        return params;
    }

    private void updateBundleNonOverlap(Bundle to, Bundle from) {
        if (to == null || from == null || to.isEmpty() || from.isEmpty()) {
            return;
        }
        Bundle bundle = new Bundle(from);
        for (String key : from.keySet()) {
            if (to.containsKey(key)) {
                Object objTo = to.get(key);
                Object objFrom = bundle.get(key);

                if ((objTo == null) || !objTo.equals(objFrom)) {
                    to.remove(key);
                } else {
                    bundle.remove(key);
                }
            } else {
                bundle.remove(key);
            }
        }
        if (!bundle.isEmpty()) {
            /* overwrite all changed values */
            to.putAll(bundle);
        }
    }

    public String getSupportedPcoIdList() {
        if (mBundle != null) {
            return mBundle.getString(KEY_EPDG_SUPPORTED_PCO_ID_LIST);
        }
        return null;
    }

    public int getWifiApHandoverTimeout() {
        if (mBundle != null) {
            if (mBundle.getBoolean(KEY_WIFI_ACCESS_POINTS_HANDOVER_SUPPORTED)) {
                return mBundle.getInt(KEY_WIFI_ACCESS_POINTS_HANDOVER_TIMEOUT);
            }
            return 0;
        }
        return 10;
    }

    public boolean isVoMobileEnabled() {
        if (mBundle != null) {
            return mBundle.getBoolean(KEY_VOMOBILE_ENABLED, false);
        }
        return false;
    }

    public boolean isVoMobileSupportedOnNetwork(int networkType) {
        String allowedRatList = DEFAULT_VOMOBILE_RAT_LIST;
        if (mBundle != null) {
            allowedRatList = mBundle.getString(KEY_VOMOBILE_SUPPORTED_RADIO_TECHNOLOGY, DEFAULT_VOMOBILE_RAT_LIST);
        }
        if (!TextUtils.isEmpty(allowedRatList)) {
            String current = TelephonyManager.getNetworkTypeName(networkType);

            String[] tokens = allowedRatList.split(",");
            for (String s : tokens) {
                if (current.equals(s.trim())) {
                    return true;
                }
            }
        }
        return false;
    }

    public boolean isVoMobileEnabledOnRoamingNetwork() {
        if (mBundle != null) {
            return mBundle.getBoolean(KEY_VOMOBILE_ONROAMING_ENABLED, false);
        }
        return false;
    }

    public String getRetryParameters() {
        if (mBundle != null) {
            return mBundle.getString(KEY_EPDG_CONNECTION_RETRY_PARAMETERS);
        }
        return null;
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
    protected Bundle getBundle() { return mBundle; }
}
