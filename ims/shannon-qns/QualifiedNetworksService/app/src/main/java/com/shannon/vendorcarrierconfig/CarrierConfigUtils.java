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
package com.shannon.vendorcarrierconfig;

import android.os.Bundle;
import android.os.PersistableBundle;
import android.telephony.TelephonyManager;

import com.android.internal.annotations.VisibleForTesting;

import java.lang.reflect.Field;

import static com.shannon.vendorcarrierconfig.ShannonVendorCarrierConfig.*;

public class CarrierConfigUtils {
    @VisibleForTesting
    protected static final String KEY_SUPPORTED_RAT_LIST = "KEY_SUPPORTED_RAT_LIST";
    private static final String KEY_CARRIER_VOLTE_ROAMING_AVAILABLE_BOOL = "KEY_CARRIER_VOLTE_ROAMING_AVAILABLE_BOOL";
    private static final String KEY_CARRIER_USE_VOPS_FOR_VOLTE_ENABLE_BOOL = "KEY_CARRIER_USE_VOPS_FOR_VOLTE_ENABLE_BOOL";
    private static final String KEY_MMS_RAT_SUPPORTED_LIST = "KEY_MMS_RAT_SUPPORTED_LIST";
    private static final String KEY_CARRIER_WFC_ROAMING_ENABLED_BOOL = "KEY_CARRIER_WFC_ROAMING_ENABLED_BOOL";
    private static final String KEY_VOWIFI_EMERGENCY_CALL_OVER_EMERGENCY_PDN_BOOL = "KEY_VOWIFI_EMERGENCY_CALL_OVER_EMERGENCY_PDN_BOOL";
    private static final String KEY_IDLE_HANDOVER_SUPPORTED_IN_HOME = "KEY_IDLE_HANDOVER_SUPPORTED_IN_HOME";
    private static final String KEY_INCALL_HANDOVER_SUPPORTED_IN_HOME = "KEY_INCALL_HANDOVER_SUPPORTED_IN_HOME";
    private static final String KEY_IDLE_HANDOVER_SUPPORTED_ON_ROAMING = "KEY_IDLE_HANDOVER_SUPPORTED_ON_ROAMING";
    private static final String KEY_INCALL_HANDOVER_SUPPORTED_ON_ROAMING = "KEY_INCALL_HANDOVER_SUPPORTED_ON_ROAMING";
    private static final String KEY_HANDOVER_ALLOWED_IN_EMERGENCY_CALL_BOOL = "KEY_HANDOVER_ALLOWED_IN_EMERGENCY_CALL_BOOL";
    private static final String KEY_SUPPORTED_RAT_LIST_FOR_IMS_PDN_HANDOVER_STRING = "KEY_SUPPORTED_RAT_LIST_FOR_IMS_PDN_HANDOVER_STRING";
    private static final String KEY_SUPPORTED_RAT_LIST_FOR_EMERGENCY_PDN_HANDOVER_STRING = "KEY_SUPPORTED_RAT_LIST_FOR_EMERGENCY_PDN_HANDOVER_STRING";
    private static final String KEY_LTE_HANDOVER_HYSTERSIS_TIMER_INT_ARRAY = "KEY_LTE_HANDOVER_HYSTERSIS_TIMER_INT_ARRAY";
    private static final String KEY_WIFI_HANDOVER_HYSTERSIS_TIMER_INT_ARRAY = "KEY_WIFI_HANDOVER_HYSTERSIS_TIMER_INT_ARRAY";
    @VisibleForTesting
    protected static final String KEY_LTE_ROVE_IN_IDLE_CELLPREF_INT_ARRAY = "KEY_LTE_ROVE_IN_IDLE_CELLPREF_INT_ARRAY";
    private static final String KEY_LTE_ROVE_OUT_IDLE_CELLPREF_INT_ARRAY = "KEY_LTE_ROVE_OUT_IDLE_CELLPREF_INT_ARRAY";
    private static final String KEY_LTE_ROVE_IN_INCALL_CELLPREF_INT_ARRAY = "KEY_LTE_ROVE_IN_INCALL_CELLPREF_INT_ARRAY";
    private static final String KEY_LTE_ROVE_OUT_INCALL_CELLPREF_INT_ARRAY = "KEY_LTE_ROVE_OUT_INCALL_CELLPREF_INT_ARRAY";
    private static final String KEY_LTE_ROVE_IN_IDLE_WIFIPREF_INT_ARRAY = "KEY_LTE_ROVE_IN_IDLE_WIFIPREF_INT_ARRAY";
    private static final String KEY_LTE_ROVE_OUT_IDLE_WIFIPREF_INT_ARRAY = "KEY_LTE_ROVE_OUT_IDLE_WIFIPREF_INT_ARRAY";
    private static final String KEY_LTE_ROVE_IN_INCALL_WIFIPREF_INT_ARRAY = "KEY_LTE_ROVE_IN_INCALL_WIFIPREF_INT_ARRAY";
    private static final String KEY_LTE_ROVE_OUT_INCALL_WIFIPREF_INT_ARRAY = "KEY_LTE_ROVE_OUT_INCALL_WIFIPREF_INT_ARRAY";
    @VisibleForTesting
    protected static final String KEY_WIFI_RSSI_ROVE_IN_IDLE_CELLPREF_INT = "KEY_WIFI_RSSI_ROVE_IN_IDLE_CELLPREF_INT";
    private static final String KEY_WIFI_RSSI_ROVE_OUT_IDLE_CELLPREF_INT = "KEY_WIFI_RSSI_ROVE_OUT_IDLE_CELLPREF_INT";
    private static final String KEY_WIFI_RSSI_ROVE_IN_INCALL_CELLPREF_INT = "KEY_WIFI_RSSI_ROVE_IN_INCALL_CELLPREF_INT";
    private static final String KEY_WIFI_RSSI_ROVE_OUT_INCALL_CELLPREF_INT = "KEY_WIFI_RSSI_ROVE_OUT_INCALL_CELLPREF_INT";
    private static final String KEY_WIFI_RSSI_ROVE_IN_IDLE_WIFIPREF_INT = "KEY_WIFI_RSSI_ROVE_IN_IDLE_WIFIPREF_INT";
    private static final String KEY_WIFI_RSSI_ROVE_OUT_IDLE_WIFIPREF_INT = "KEY_WIFI_RSSI_ROVE_OUT_IDLE_WIFIPREF_INT";
    private static final String KEY_WIFI_RSSI_ROVE_IN_INCALL_WIFIPREF_INT = "KEY_WIFI_RSSI_ROVE_IN_INCALL_WIFIPREF_INT";
    private static final String KEY_WIFI_RSSI_ROVE_OUT_INCALL_WIFIPREF_INT = "KEY_WIFI_RSSI_ROVE_OUT_INCALL_WIFIPREF_INT";
    @VisibleForTesting
    protected static final String KEY_NR_ROVE_IN_IDLE_CELLPREF_INT_ARRAY = "KEY_NR_ROVE_IN_IDLE_CELLPREF_INT_ARRAY";
    private static final String KEY_NR_ROVE_OUT_IDLE_CELLPREF_INT_ARRAY = "KEY_NR_ROVE_OUT_IDLE_CELLPREF_INT_ARRAY";
    private static final String KEY_NR_ROVE_IN_INCALL_CELLPREF_INT_ARRAY = "KEY_NR_ROVE_IN_INCALL_CELLPREF_INT_ARRAY";
    private static final String KEY_NR_ROVE_OUT_INCALL_CELLPREF_INT_ARRAY = "KEY_NR_ROVE_OUT_INCALL_CELLPREF_INT_ARRAY";
    private static final String KEY_NR_ROVE_IN_IDLE_WIFIPREF_INT_ARRAY = "KEY_NR_ROVE_IN_IDLE_WIFIPREF_INT_ARRAY";
    private static final String KEY_NR_ROVE_OUT_IDLE_WIFIPREF_INT_ARRAY = "KEY_NR_ROVE_OUT_IDLE_WIFIPREF_INT_ARRAY";
    private static final String KEY_NR_ROVE_IN_INCALL_WIFIPREF_INT_ARRAY = "KEY_NR_ROVE_IN_INCALL_WIFIPREF_INT_ARRAY";
    private static final String KEY_NR_ROVE_OUT_INCALL_WIFIPREF_INT_ARRAY = "KEY_NR_ROVE_OUT_INCALL_WIFIPREF_INT_ARRAY";
    private static final String KEY_UT_RAT_SUPPORTED_LIST = "KEY_UT_RAT_SUPPORTED_LIST";
    private static final String KEY_UT_SUPPORTED_WHEN_ROAMING_BOOL = "KEY_UT_SUPPORTED_WHEN_ROAMING_BOOL";
    @VisibleForTesting
    protected static final String KEY_IMS_PDN_ENABLED_IN_NO_VOPS_SUPPORT_INT_ARRAY = "KEY_IMS_PDN_ENABLED_IN_NO_VOPS_SUPPORT_INT_ARRAY";

    private static final String RAT_TYPE_NR = "NR";
    private static final String RAT_TYPE_LTE = "LTE";
    private static final String RAT_TYPE_3G = "3G";
    private static final String RAT_TYPE_2G = "2G";
    private static final String RAT_TYPE_WIFI = "WIFI";

    private final PersistableBundle mBundle;

    public CarrierConfigUtils(PersistableBundle bundle) {
        mBundle = bundle;
    }

    @VisibleForTesting
    protected String getKey(String key) {
        try {
            Class c = Class.forName("android.telephony.CarrierConfigManager");
            Field f = c.getField(key);
            return (String)f.get(null);
        } catch (Exception e) {
            return null;
        }
    }

    private String getString(String key) {
        String convertedKey = getKey(key);
        if (convertedKey != null && mBundle.containsKey(convertedKey)) {
            return mBundle.getString(convertedKey);
        }
        return null;
    }

    private boolean getBoolean(String key, boolean defaultValue) {
        String convertedKey = getKey(key);
        if (convertedKey != null && mBundle.containsKey(convertedKey)) {
            return mBundle.getBoolean(convertedKey, defaultValue);
        }
        return defaultValue;
    }

    private int getInt(String key, int defaultValue) {
        String convertedKey = getKey(key);
        if (convertedKey != null && mBundle.containsKey(convertedKey)) {
            return mBundle.getInt(convertedKey, defaultValue);
        }
        return defaultValue;
    }

    private int[] getIntArray(String key) {
        String convertedKey = getKey(key);
        if (convertedKey != null && mBundle.containsKey(convertedKey)) {
            return mBundle.getIntArray(convertedKey);
        }
        return null;
    }

    private String convertToNetworkTypes(String input) {
        if ((input == null) || input.length() == 0) {
            return "";
        }
        StringBuilder result = new StringBuilder(256);

        String[] types = input.split(",");
        for (String s: types) {
            if (RAT_TYPE_2G.equalsIgnoreCase(s.trim())) {
                if (result.length() != 0) {
                    result.append(",");
                }
                result.append(TelephonyManager.getNetworkTypeName(TelephonyManager.NETWORK_TYPE_GSM)).
                        append(",").append(TelephonyManager.getNetworkTypeName(TelephonyManager.NETWORK_TYPE_GPRS)).
                        append(",").append(TelephonyManager.getNetworkTypeName(TelephonyManager.NETWORK_TYPE_EDGE));
            } else if (RAT_TYPE_3G.equalsIgnoreCase(s.trim())) {
                if (result.length() != 0) {
                    result.append(",");
                }
                result.append(TelephonyManager.getNetworkTypeName(TelephonyManager.NETWORK_TYPE_UMTS)).
                        append(",").append(TelephonyManager.getNetworkTypeName(TelephonyManager.NETWORK_TYPE_HSPA)).
                        append(",").append(TelephonyManager.getNetworkTypeName(TelephonyManager.NETWORK_TYPE_HSDPA)).
                        append(",").append(TelephonyManager.getNetworkTypeName(TelephonyManager.NETWORK_TYPE_HSPAP)).
                        append(",").append(TelephonyManager.getNetworkTypeName(TelephonyManager.NETWORK_TYPE_HSUPA));
            } else if (RAT_TYPE_LTE.equalsIgnoreCase(s.trim())) {
                if (result.length() != 0) {
                    result.append(",");
                }
                result.append(TelephonyManager.getNetworkTypeName(TelephonyManager.NETWORK_TYPE_LTE)).
                        append(",").append(TelephonyManager.getNetworkTypeName(TelephonyManager.NETWORK_TYPE_LTE_CA));
            } else if (RAT_TYPE_NR.equalsIgnoreCase(s.trim())) {
                if (result.length() != 0) {
                    result.append(",");
                }
                result.append(TelephonyManager.getNetworkTypeName(TelephonyManager.NETWORK_TYPE_NR));
            } else if (RAT_TYPE_WIFI.equalsIgnoreCase(s.trim())) {
                if (result.length() != 0) {
                    result.append(",");
                }
                result.append(TelephonyManager.getNetworkTypeName(TelephonyManager.NETWORK_TYPE_IWLAN));
            }
        }
        return result.toString();
    }

    private String buildThresholdsString(String thresholdsLte, String thresholdsNr, String thresholdsWiFi) {
        StringBuilder result = new StringBuilder(256);

        if (thresholdsLte != null) {
            int[] intArray = getIntArray(thresholdsLte);
            if (intArray != null) {
                // 0 - RSRP, 1 - RSRQ, 2 - RSSNR
                int rsrp = 0;
                int rsrq = 0;
                int rssnr = 0;
                if (intArray.length > 0) {
                    rsrp = intArray[0];
                }
                if (intArray.length > 1) {
                    rsrq = intArray[1];
                    if (intArray.length > 2) {
                        rssnr = intArray[2];
                    }
                }
                if ((rsrp != 0) || (rsrq != 0) || (rssnr != 0)) {
                    result.append("eutran{");
                    if (rsrp != 0) {
                        result.append("rsrp=").append(rsrp);
                    }
                    if (rsrq != 0) {
                        if (rsrp != 0) {
                            result.append(",");
                        }
                        result.append("rsrq=").append(rsrq);
                    }
                    if (rssnr != 0) {
                        if ((rsrp != 0) || (rsrq != 0)) {
                            result.append(",");
                        }
                        result.append("rssnr=").append(rssnr);
                    }
                    result.append("}");
                }
            }
        }
        if (thresholdsNr != null) {
            int[] intArray = getIntArray(thresholdsNr);
            if (intArray != null) {
                // 0 - RSRP, 1 - RSRQ, 2 - RSSNR
                int rsrp = 0;
                int rsrq = 0;
                int rssnr = 0;
                if (intArray.length > 0) {
                    rsrp = intArray[0];
                }
                if (intArray.length > 1) {
                    rsrq = intArray[1];
                    if (intArray.length > 2) {
                        rssnr = intArray[2];
                    }
                }
                if ((rsrp != 0) || (rsrq != 0) || (rssnr != 0)) {
                    result.append("ngran{");
                    if (rsrp != 0) {
                        result.append("rsrp=").append(rsrp);
                    }
                    if (rsrq != 0) {
                        if (rsrp != 0) {
                            result.append(",");
                        }
                        result.append("rsrq=").append(rsrq);
                    }
                    if (rssnr != 0) {
                        if ((rsrp != 0) || (rsrq != 0)) {
                            result.append(",");
                        }
                        result.append("rssnr=").append(rssnr);
                    }
                    result.append("}");
                }
            }
        }
        if (thresholdsWiFi != null) {
            int value = getInt(thresholdsWiFi, 0);
            if (value != 0) {
                if (result.length() > 0) {
                    result.append(";");
                }
                result.append("iwlan{rssi=").append(value).append("}");
            }
        }
        return (result.length() > 0) ? result.toString() : null;
    }

    public Bundle convertToLocal() {
        Bundle bundle = new Bundle(mBundle);
        String string = getString(KEY_SUPPORTED_RAT_LIST);
        if (string != null) {
            // KEY_VOLTE_SUPPORTED_RADIO_TECHNOLOGY
            bundle.putString(KEY_VOLTE_SUPPORTED_RADIO_TECHNOLOGY, convertToNetworkTypes(string));
            // KEY_VOLTE_ROAMING_ENABLED
            bundle.putBoolean(KEY_VOLTE_ROAMING_ENABLED,
                    getBoolean(KEY_CARRIER_VOLTE_ROAMING_AVAILABLE_BOOL, true));
            // KEY_VOLTE_VOPS_REQUIRED
            bundle.putBoolean(KEY_VOLTE_VOPS_REQUIRED,
                    getBoolean(KEY_CARRIER_USE_VOPS_FOR_VOLTE_ENABLE_BOOL, true));
            // KEY_MMS_SUPPORTED_RADIO_TECHNOLOGY
            string = getString(KEY_MMS_RAT_SUPPORTED_LIST);
            if (string != null) {
                bundle.putString(KEY_MMS_SUPPORTED_RADIO_TECHNOLOGY, convertToNetworkTypes(string));
            }
            // KEY_VOWIFI_ROAMING_ENABLED
            bundle.putBoolean(KEY_VOWIFI_ROAMING_ENABLED,
                    getBoolean(KEY_CARRIER_WFC_ROAMING_ENABLED_BOOL, false));
            // KEY_EMERGENCY_OVER_WIFI_SUPPORTED
            bundle.putBoolean(KEY_EMERGENCY_OVER_WIFI_SUPPORTED,
                    getBoolean(KEY_VOWIFI_EMERGENCY_CALL_OVER_EMERGENCY_PDN_BOOL, true));
            // KEY_IMS_HANDOVER_SUPPORTED
            bundle.putBoolean(KEY_IMS_HANDOVER_SUPPORTED,
                    getBoolean(KEY_IDLE_HANDOVER_SUPPORTED_IN_HOME, true));
            // KEY_IMS_HANDOVER_SUPPORTED_IN_CALL
            bundle.putBoolean(KEY_IMS_HANDOVER_SUPPORTED_IN_CALL,
                    getBoolean(KEY_INCALL_HANDOVER_SUPPORTED_IN_HOME, true));
            // KEY_IMS_HANDOVER_SUPPORTED_ROAMING
            bundle.putBoolean(KEY_IMS_HANDOVER_SUPPORTED_ROAMING,
                    getBoolean(KEY_IDLE_HANDOVER_SUPPORTED_ON_ROAMING, true));
            // KEY_IMS_HANDOVER_SUPPORTED_IN_CALL_ROAMING
            bundle.putBoolean(KEY_IMS_HANDOVER_SUPPORTED_IN_CALL_ROAMING,
                    getBoolean(KEY_INCALL_HANDOVER_SUPPORTED_ON_ROAMING, true));
            // KEY_EMERGENCY_HANDOVER_ENABLED
            bundle.putBoolean(KEY_EMERGENCY_HANDOVER_ENABLED,
                    getBoolean(KEY_HANDOVER_ALLOWED_IN_EMERGENCY_CALL_BOOL, false));
            // KEY_IMS_HANDOVER_SUPPORTED_RADIO_TECHNOLOGY
            string = getString(KEY_SUPPORTED_RAT_LIST_FOR_IMS_PDN_HANDOVER_STRING);
            if (string != null) {
                bundle.putString(KEY_IMS_HANDOVER_SUPPORTED_RADIO_TECHNOLOGY,
                        convertToNetworkTypes(string));
            }
            // KEY_EMERGENCY_HANDOVER_SUPPORTED_RADIO_TECHNOLOGY
            string = getString(KEY_SUPPORTED_RAT_LIST_FOR_EMERGENCY_PDN_HANDOVER_STRING);
            if (string != null) {
                bundle.putString(KEY_EMERGENCY_HANDOVER_SUPPORTED_RADIO_TECHNOLOGY,
                        convertToNetworkTypes(string));
            }
            // KEY_IMS_HANDOVER_HYSTERESIS_TIMER_VOLTE
            int[] intArray = getIntArray(KEY_LTE_HANDOVER_HYSTERSIS_TIMER_INT_ARRAY);
            if (intArray != null && (intArray.length > 0)) {
                bundle.putInt(KEY_IMS_HANDOVER_HYSTERESIS_TIMER_VOLTE, intArray[0]);
            }
            // KEY_IMS_HANDOVER_HYSTERESIS_TIMER_VOWIFI
            intArray = getIntArray(KEY_WIFI_HANDOVER_HYSTERSIS_TIMER_INT_ARRAY);
            if (intArray != null && (intArray.length > 0)) {
                bundle.putInt(KEY_IMS_HANDOVER_HYSTERESIS_TIMER_VOWIFI, intArray[0]);
            }
            // KEY_IMS_HANDOVER_ROVE_IN_CELL_PREFERRED
            string = buildThresholdsString(KEY_LTE_ROVE_IN_IDLE_CELLPREF_INT_ARRAY,
                    KEY_NR_ROVE_IN_IDLE_CELLPREF_INT_ARRAY, KEY_WIFI_RSSI_ROVE_IN_IDLE_CELLPREF_INT);
            if (string != null) {
                bundle.putString(KEY_IMS_HANDOVER_ROVE_IN_CELL_PREFERRED, string);
            }
            // KEY_IMS_HANDOVER_ROVE_OUT_CELL_PREFERRED
            string = buildThresholdsString(KEY_LTE_ROVE_OUT_IDLE_CELLPREF_INT_ARRAY,
                    KEY_NR_ROVE_OUT_IDLE_CELLPREF_INT_ARRAY, KEY_WIFI_RSSI_ROVE_OUT_IDLE_CELLPREF_INT);
            if (string != null) {
                bundle.putString(KEY_IMS_HANDOVER_ROVE_OUT_CELL_PREFERRED, string);
            }
            // KEY_IMS_HANDOVER_HAND_IN_CELL_PREFERRED
            string = buildThresholdsString(KEY_LTE_ROVE_IN_INCALL_CELLPREF_INT_ARRAY,
                    KEY_NR_ROVE_IN_INCALL_CELLPREF_INT_ARRAY, KEY_WIFI_RSSI_ROVE_IN_INCALL_CELLPREF_INT);
            if (string != null) {
                bundle.putString(KEY_IMS_HANDOVER_HAND_IN_CELL_PREFERRED, string);
            }
            // KEY_IMS_HANDOVER_HAND_OUT_CELL_PREFERRED
            string = buildThresholdsString(KEY_LTE_ROVE_OUT_INCALL_CELLPREF_INT_ARRAY,
                    KEY_NR_ROVE_OUT_INCALL_CELLPREF_INT_ARRAY, KEY_WIFI_RSSI_ROVE_OUT_INCALL_CELLPREF_INT);
            if (string != null) {
                bundle.putString(KEY_IMS_HANDOVER_HAND_OUT_CELL_PREFERRED, string);
            }
            // KEY_IMS_HANDOVER_ROVE_IN_WIFI_PREFERRED
            string = buildThresholdsString(KEY_LTE_ROVE_IN_IDLE_WIFIPREF_INT_ARRAY,
                    KEY_NR_ROVE_IN_IDLE_WIFIPREF_INT_ARRAY, KEY_WIFI_RSSI_ROVE_IN_IDLE_WIFIPREF_INT);
            if (string != null) {
                bundle.putString(KEY_IMS_HANDOVER_ROVE_IN_WIFI_PREFERRED, string);
            }
            // KEY_IMS_HANDOVER_ROVE_OUT_WIFI_PREFERRED
            string = buildThresholdsString(KEY_LTE_ROVE_OUT_IDLE_WIFIPREF_INT_ARRAY,
                    KEY_NR_ROVE_OUT_IDLE_WIFIPREF_INT_ARRAY, KEY_WIFI_RSSI_ROVE_OUT_IDLE_WIFIPREF_INT);
            if (string != null) {
                bundle.putString(KEY_IMS_HANDOVER_ROVE_OUT_WIFI_PREFERRED, string);
            }
            // KEY_IMS_HANDOVER_HAND_IN_WIFI_PREFERRED
            string = buildThresholdsString(KEY_LTE_ROVE_IN_INCALL_WIFIPREF_INT_ARRAY,
                    KEY_NR_ROVE_IN_INCALL_WIFIPREF_INT_ARRAY, KEY_WIFI_RSSI_ROVE_IN_INCALL_WIFIPREF_INT);
            if (string != null) {
                bundle.putString(KEY_IMS_HANDOVER_HAND_IN_WIFI_PREFERRED, string);
            }
            // KEY_IMS_HANDOVER_HAND_OUT_WIFI_PREFERRED
            string = buildThresholdsString(KEY_LTE_ROVE_OUT_INCALL_WIFIPREF_INT_ARRAY,
                    KEY_NR_ROVE_OUT_INCALL_WIFIPREF_INT_ARRAY, KEY_WIFI_RSSI_ROVE_OUT_INCALL_WIFIPREF_INT);
            if (string != null) {
                bundle.putString(KEY_IMS_HANDOVER_HAND_OUT_WIFI_PREFERRED, string);
            }
            // KEY_XCAP_SUPPORTED_RADIO_TECHNOLOGY
            string = getString(KEY_UT_RAT_SUPPORTED_LIST);
            if (string != null) {
                bundle.putString(KEY_XCAP_SUPPORTED_RADIO_TECHNOLOGY, convertToNetworkTypes(string));
            }
        }
        updateVopsRules(bundle);
        return bundle;
    }

    private void updateVopsRules(Bundle bundle) {
        String[] keyNames = {KEY_VOLTE_VOPS_REQUIRED, KEY_VOLTE_VOPS_REQUIRED_ROAMING};

        // KEY_VOLTE_VOPS_REQUIRED[_ROAMING]
        int[] intArray = getIntArray(KEY_IMS_PDN_ENABLED_IN_NO_VOPS_SUPPORT_INT_ARRAY);
        if ((intArray != null) && (intArray.length > 0)) {
            for (int type : intArray) {
                if (type < keyNames.length) {
                    bundle.putBoolean(keyNames[type], false);
                }
            }
        }
    }
}
