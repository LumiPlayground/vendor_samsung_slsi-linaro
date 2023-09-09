/*
 * Copyright (C) 2018 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package com.samsung.slsi.telephony.oemril.multisim;

import android.content.Context;
import android.os.PersistableBundle;
import android.telephony.CarrierConfigManager;
import android.telephony.RadioAccessFamily;
import android.telephony.TelephonyManager;
import android.util.Log;

public final class NetworkModeSelector {
    private static final String LOG_TAG = "NetworkModeSelector";
    private CarrierConfigManager mCarrierConfigManager;
    private Context mContext;
    private TelephonyManager mTelephonyManager;
    private boolean mAllowed5gNetworkType;
    private boolean mIsGlobalCdma;
    private boolean mSupported5gRadioAccessFamily;
    private int mSubId;

    enum EnabledNetworks {
        ENABLED_NETWORKS_UNKNOWN,
        ENABLED_NETWORKS_CDMA_CHOICES,
        ENABLED_NETWORKS_CDMA_NO_LTE_CHOICES,
        ENABLED_NETWORKS_CDMA_ONLY_LTE_CHOICES,
        ENABLED_NETWORKS_TDSCDMA_CHOICES,
        ENABLED_NETWORKS_EXCEPT_GSM_LTE_CHOICES,
        ENABLED_NETWORKS_EXCEPT_GSM_4G_CHOICES,
        ENABLED_NETWORKS_EXCEPT_GSM_CHOICES,
        ENABLED_NETWORKS_EXCEPT_LTE_CHOICES,
        ENABLED_NETWORKS_4G_CHOICES,
        ENABLED_NETWORKS_CHOICES,
        PREFERRED_NETWORK_MODE_CHOICES_WORLD_MODE,
        ENABLED_NETWORKS_EXCEPT_GSM_WCDMA_CHOICES

    }

    NetworkModeSelector(Context context, int subId) {
        this.mContext = context;
        this.mSubId = subId;

        mCarrierConfigManager = mContext.getSystemService(CarrierConfigManager.class);
        mTelephonyManager = mContext.getSystemService(TelephonyManager.class)
                .createForSubscriptionId(mSubId);

        final PersistableBundle carrierConfig = mCarrierConfigManager.getConfigForSubId(mSubId);
        mIsGlobalCdma = mTelephonyManager.isLteCdmaEvdoGsmWcdmaEnabled()
                && carrierConfig.getBoolean(CarrierConfigManager.KEY_SHOW_CDMA_CHOICES_BOOL);
        refresh();
    }

    private void refresh() {
        mAllowed5gNetworkType = MobileNetworkUtils.checkSupportedRadioBitmask(
                getAllowedNetworkTypes(),
                TelephonyManager.NETWORK_TYPE_BITMASK_NR);
        mSupported5gRadioAccessFamily = MobileNetworkUtils.checkSupportedRadioBitmask(
                mTelephonyManager.getSupportedRadioAccessFamily(),
                TelephonyManager.NETWORK_TYPE_BITMASK_NR);
    }

    public int getPreferredNetworkMode() {
        return MobileNetworkUtils.getPreferredNetworkMode(mContext, mSubId);
    }

    public long getPreferredNetworkTypeBitmask() {
        return MobileNetworkUtils.getPreferredNetworkTypeBitmask(mContext, mSubId);
    }

    private EnabledNetworks getEnabledNetworkType() {
        EnabledNetworks enabledNetworkType = EnabledNetworks.ENABLED_NETWORKS_UNKNOWN;
        final int phoneType = mTelephonyManager.getPhoneType();
        final PersistableBundle carrierConfig = mCarrierConfigManager.getConfigForSubId(mSubId);

        if (phoneType == TelephonyManager.PHONE_TYPE_CDMA) {
            final int lteForced = android.provider.Settings.Global.getInt(
                    mContext.getContentResolver(),
                    android.provider.Settings.Global.LTE_SERVICE_FORCED + mSubId,
                    0);
            final int settingsNetworkMode = getPreferredNetworkMode();
            Log.d(LOG_TAG, "settingsNetworkMode("+settingsNetworkMode+")");
            if (mTelephonyManager.isLteCdmaEvdoGsmWcdmaEnabled()) {
                if (lteForced == 0) {
                    enabledNetworkType = EnabledNetworks.ENABLED_NETWORKS_CDMA_CHOICES;
                } else {
                    switch (settingsNetworkMode) {
                        case TelephonyManager.NETWORK_MODE_CDMA_EVDO:
                        case TelephonyManager.NETWORK_MODE_CDMA_NO_EVDO:
                        case TelephonyManager.NETWORK_MODE_EVDO_NO_CDMA:
                            enabledNetworkType =
                                    EnabledNetworks.ENABLED_NETWORKS_CDMA_NO_LTE_CHOICES;
                            break;
                        case TelephonyManager.NETWORK_MODE_GLOBAL:
                        case TelephonyManager.NETWORK_MODE_LTE_CDMA_EVDO:
                        case TelephonyManager.NETWORK_MODE_LTE_CDMA_EVDO_GSM_WCDMA:
                        case TelephonyManager.NETWORK_MODE_LTE_ONLY:
                            enabledNetworkType =
                                    EnabledNetworks.ENABLED_NETWORKS_CDMA_ONLY_LTE_CHOICES;
                            break;
                        default:
                            enabledNetworkType = EnabledNetworks.ENABLED_NETWORKS_CDMA_CHOICES;
                            break;
                    }
                }
            }
        } else if (phoneType == TelephonyManager.PHONE_TYPE_GSM) {
            if (MobileNetworkUtils.isTdscdmaSupported(mContext, mSubId)) {
                enabledNetworkType = EnabledNetworks.ENABLED_NETWORKS_TDSCDMA_CHOICES;
            } else if (carrierConfig != null
                    && !carrierConfig.getBoolean(CarrierConfigManager.KEY_PREFER_2G_BOOL)
                    && !carrierConfig.getBoolean(CarrierConfigManager.KEY_LTE_ENABLED_BOOL)) {
                enabledNetworkType = EnabledNetworks.ENABLED_NETWORKS_EXCEPT_GSM_LTE_CHOICES;
            } else if (carrierConfig != null
                    && !carrierConfig.getBoolean(CarrierConfigManager.KEY_PREFER_2G_BOOL)) {
                enabledNetworkType = EnabledNetworks.ENABLED_NETWORKS_EXCEPT_GSM_CHOICES;
            } else if (carrierConfig != null
                    && !carrierConfig.getBoolean(CarrierConfigManager.KEY_LTE_ENABLED_BOOL)) {
                enabledNetworkType = EnabledNetworks.ENABLED_NETWORKS_EXCEPT_LTE_CHOICES;
            } else if (mIsGlobalCdma) {
                enabledNetworkType = EnabledNetworks.ENABLED_NETWORKS_CDMA_CHOICES;
            } else {
                enabledNetworkType = EnabledNetworks.ENABLED_NETWORKS_CHOICES;
            }
        }
        if (MobileNetworkUtils.isWorldMode(mContext, mSubId)) {
            enabledNetworkType = EnabledNetworks.PREFERRED_NETWORK_MODE_CHOICES_WORLD_MODE;
        }

        Log.d(LOG_TAG, "enabledNetworkType: " + enabledNetworkType);
        return enabledNetworkType;
    }

    public int getDesiredNetworkType(boolean isNrSupported) {
        refresh();

        int networkType = TelephonyManager.DEFAULT_PREFERRED_NETWORK_MODE;
        switch (getEnabledNetworkType()) {
            case ENABLED_NETWORKS_CDMA_CHOICES:
                networkType = getDesiredNetworkType(
                        TelephonyManager.NETWORK_MODE_LTE_CDMA_EVDO, isNrSupported);
                break;
            case ENABLED_NETWORKS_CDMA_NO_LTE_CHOICES:
                networkType = TelephonyManager.NETWORK_MODE_CDMA_EVDO;
                break;
            case ENABLED_NETWORKS_CDMA_ONLY_LTE_CHOICES:
                networkType = TelephonyManager.NETWORK_MODE_LTE_CDMA_EVDO;
                break;
            case ENABLED_NETWORKS_TDSCDMA_CHOICES:
                networkType = getDesiredNetworkType(
                        TelephonyManager.NETWORK_MODE_LTE_TDSCDMA_CDMA_EVDO_GSM_WCDMA, isNrSupported);
                break;
            case ENABLED_NETWORKS_EXCEPT_GSM_LTE_CHOICES:
                networkType = TelephonyManager.NETWORK_MODE_WCDMA_PREF;
                break;
            case ENABLED_NETWORKS_EXCEPT_GSM_4G_CHOICES:
                networkType = getDesiredNetworkType(
                        TelephonyManager.NETWORK_MODE_LTE_GSM_WCDMA, isNrSupported);
                break;
            case ENABLED_NETWORKS_EXCEPT_GSM_CHOICES:
                networkType = getDesiredNetworkType(
                        TelephonyManager.NETWORK_MODE_LTE_GSM_WCDMA, isNrSupported);
                break;
            case ENABLED_NETWORKS_EXCEPT_LTE_CHOICES:
                networkType = TelephonyManager.NETWORK_MODE_WCDMA_PREF;
                break;
            case ENABLED_NETWORKS_4G_CHOICES:
            case ENABLED_NETWORKS_CHOICES:
                networkType = getDesiredNetworkType(
                        TelephonyManager.NETWORK_MODE_LTE_GSM_WCDMA, isNrSupported);
                break;
            case PREFERRED_NETWORK_MODE_CHOICES_WORLD_MODE:
                getDesiredNetworkType(TelephonyManager.NETWORK_MODE_LTE_CDMA_EVDO_GSM_WCDMA, isNrSupported);
                break;
            default:
                Log.w(LOG_TAG,"Not supported enabled network types.");
                break;
        }
        return networkType;
    }

    public int getDesiredNetworkTypeBitmask(boolean isNrSupported) {
        return RadioAccessFamily.getRafFromNetworkType(getDesiredNetworkType(isNrSupported));
    }

    private int getDesiredNetworkType(int value, boolean isNrSupported) {
        if (isNrSupported && mSupported5gRadioAccessFamily && mAllowed5gNetworkType) {
            return addNrToLteNetworkType(value);
        } else {
            Log.d(LOG_TAG, "Disable 5G. "
                    + " isNrSupported: " + isNrSupported
                    + " supported5GRadioAccessFamily: " + mSupported5gRadioAccessFamily
                    + " allowed5GNetworkType: " + mAllowed5gNetworkType);
            return value;
        }
    }

    public long getDesiredAllowedNetworkTypes(boolean isNrSupported, boolean isNrDsSupported) {
        Log.d(LOG_TAG, "getDesiredAllowedNetworkTypes() isNrSupported=" + isNrSupported + ", isNrDsSupported=" + isNrDsSupported);
        long allowedNetworkTypes = getAllowedNetworkTypes();
        if (isNrSupported || isNrDsSupported) {
            allowedNetworkTypes |= TelephonyManager.NETWORK_TYPE_BITMASK_NR;
        }
        else {
            allowedNetworkTypes &= ~TelephonyManager.NETWORK_TYPE_BITMASK_NR;
        }
        return allowedNetworkTypes;
    }

    public long getAllowedNetworkTypes() {
        return mTelephonyManager.getAllowedNetworkTypesForReason(
                        TelephonyManager.ALLOWED_NETWORK_TYPES_REASON_CARRIER);
    }

    /**
     * Transform LTE network mode to 5G network mode.
     *
     * @param networkType an LTE network mode without 5G.
     * @return the corresponding network mode with 5G.
     */
    private static int addNrToLteNetworkType(int networkType) {
        switch(networkType) {
            case TelephonyManager.NETWORK_MODE_LTE_ONLY:
                return TelephonyManager.NETWORK_MODE_NR_LTE;
            case TelephonyManager.NETWORK_MODE_LTE_CDMA_EVDO:
                return TelephonyManager.NETWORK_MODE_NR_LTE_CDMA_EVDO;
            case TelephonyManager.NETWORK_MODE_LTE_GSM_WCDMA:
                return TelephonyManager.NETWORK_MODE_NR_LTE_GSM_WCDMA;
            case TelephonyManager.NETWORK_MODE_LTE_CDMA_EVDO_GSM_WCDMA:
                return TelephonyManager.NETWORK_MODE_NR_LTE_CDMA_EVDO_GSM_WCDMA;
            case TelephonyManager.NETWORK_MODE_LTE_WCDMA:
                return TelephonyManager.NETWORK_MODE_NR_LTE_WCDMA;
            case TelephonyManager.NETWORK_MODE_LTE_TDSCDMA:
                return TelephonyManager.NETWORK_MODE_NR_LTE_TDSCDMA;
            case TelephonyManager.NETWORK_MODE_LTE_TDSCDMA_GSM:
                return TelephonyManager.NETWORK_MODE_NR_LTE_TDSCDMA_GSM;
            case TelephonyManager.NETWORK_MODE_LTE_TDSCDMA_WCDMA:
                return TelephonyManager.NETWORK_MODE_NR_LTE_TDSCDMA_WCDMA;
            case TelephonyManager.NETWORK_MODE_LTE_TDSCDMA_GSM_WCDMA:
                return TelephonyManager.NETWORK_MODE_NR_LTE_TDSCDMA_GSM_WCDMA;
            case TelephonyManager.NETWORK_MODE_LTE_TDSCDMA_CDMA_EVDO_GSM_WCDMA:
                return TelephonyManager.NETWORK_MODE_NR_LTE_TDSCDMA_CDMA_EVDO_GSM_WCDMA;
            default:
                return networkType; // not LTE
        }
    }
}
