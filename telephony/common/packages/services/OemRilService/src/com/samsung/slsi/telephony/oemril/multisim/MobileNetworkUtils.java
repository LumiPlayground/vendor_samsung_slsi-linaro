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

import static android.telephony.RadioAccessFamily.RAF_1xRTT;
import static android.telephony.RadioAccessFamily.RAF_EDGE;
import static android.telephony.RadioAccessFamily.RAF_EHRPD;
import static android.telephony.RadioAccessFamily.RAF_EVDO_0;
import static android.telephony.RadioAccessFamily.RAF_EVDO_A;
import static android.telephony.RadioAccessFamily.RAF_EVDO_B;
import static android.telephony.RadioAccessFamily.RAF_GPRS;
import static android.telephony.RadioAccessFamily.RAF_GSM;
import static android.telephony.RadioAccessFamily.RAF_HSDPA;
import static android.telephony.RadioAccessFamily.RAF_HSPA;
import static android.telephony.RadioAccessFamily.RAF_HSPAP;
import static android.telephony.RadioAccessFamily.RAF_HSUPA;
import static android.telephony.RadioAccessFamily.RAF_IS95A;
import static android.telephony.RadioAccessFamily.RAF_IS95B;
import static android.telephony.RadioAccessFamily.RAF_LTE;
import static android.telephony.RadioAccessFamily.RAF_LTE_CA;
import static android.telephony.RadioAccessFamily.RAF_NR;
import static android.telephony.RadioAccessFamily.RAF_UMTS;
import static android.telephony.RadioAccessFamily.RAF_TD_SCDMA;

import android.content.Context;
import android.os.PersistableBundle;
import android.provider.Settings;
import android.telephony.CarrierConfigManager;
import android.telephony.RadioAccessFamily;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;

public class MobileNetworkUtils {

    // Grouping of RAFs
    // 2G
    public static final int GSM = RAF_GSM | RAF_GPRS | RAF_EDGE;
    public static final int CDMA = RAF_IS95A | RAF_IS95B | RAF_1xRTT;
    // 3G
    public static final int EVDO = RAF_EVDO_0 | RAF_EVDO_A | RAF_EVDO_B | RAF_EHRPD;
    public static final int HS = RAF_HSUPA | RAF_HSDPA | RAF_HSPA | RAF_HSPAP;
    public static final int WCDMA = HS | RAF_UMTS;
    // 4G
    public static final int LTE = RAF_LTE | RAF_LTE_CA;
    // 5G
    public static final int NR = RAF_NR;

    public static int getPreferredNetworkMode(Context context, int subId) {
        return RadioAccessFamily.getNetworkTypeFromRaf(
                (int)getPreferredNetworkTypeBitmask(context, subId));
    }

    public static long getPreferredNetworkTypeBitmask(Context context, int subId) {
        TelephonyManager telephonyManager =
            context.getSystemService(TelephonyManager.class).createForSubscriptionId(subId);

        return telephonyManager.getAllowedNetworkTypesForReason(
                    TelephonyManager.ALLOWED_NETWORK_TYPES_REASON_USER);
    }

    public static boolean isWorldMode(Context context, int subId) {
        final PersistableBundle carrierConfig = context.getSystemService(
                CarrierConfigManager.class).getConfigForSubId(subId);
        return carrierConfig == null
                ? false
                : carrierConfig.getBoolean(CarrierConfigManager.KEY_WORLD_MODE_ENABLED_BOOL);
    }

    public static boolean isCdmaSupported(Context context, int subId) {
        return isCdmaSupported(context,
                context.getSystemService(TelephonyManager.class).createForSubscriptionId(subId));
    }

    private static boolean isCdmaSupported(Context context, TelephonyManager telephonyManager) {
        return telephonyManager.isApplicationOnUicc(TelephonyManager.APPTYPE_RUIM) ||
                telephonyManager.isApplicationOnUicc(TelephonyManager.APPTYPE_CSIM);
    }

    public static boolean isTdscdmaSupported(Context context, int subId) {
        if (context == null || !SubscriptionManager.isValidSubscriptionId(subId)) {
            return false;
        }

        final PersistableBundle carrierConfig = context.getSystemService(
                CarrierConfigManager.class).getConfigForSubId(subId);

        if (carrierConfig == null) {
            return false;
        }

        if (carrierConfig.getBoolean(CarrierConfigManager.KEY_SUPPORT_TDSCDMA_BOOL)) {
            return true;
        }

        TelephonyManager telephonyManager =
                context.getSystemService(TelephonyManager.class).createForSubscriptionId(subId);
        final String operatorNumeric = telephonyManager.getServiceState().getOperatorNumeric();
        final String[] numericArray = carrierConfig.getStringArray(
                CarrierConfigManager.KEY_SUPPORT_TDSCDMA_ROAMING_NETWORKS_STRING_ARRAY);
        if (numericArray == null || operatorNumeric == null) {
            return false;
        }
        for (String numeric : numericArray) {
            if (operatorNumeric.equals(numeric)) {
                return true;
            }
        }
        return false;
    }

    public static boolean checkSupportedRadioBitmask(long supportedRadioBitmask, long targetBitmask) {
        return (targetBitmask & supportedRadioBitmask) > 0;
    }

    public static long getAdjustedRaf(long raf) {
        raf = ((GSM & raf) > 0) ? (GSM | raf) : raf;
        raf = ((WCDMA & raf) > 0) ? (WCDMA | raf) : raf;
        raf = ((CDMA & raf) > 0) ? (CDMA | raf) : raf;
        raf = ((EVDO & raf) > 0) ? (EVDO | raf) : raf;
        raf = ((LTE & raf) > 0) ? (LTE | raf) : raf;
        raf = ((NR & raf) > 0) ? (NR | raf) : raf;

        return raf;
    }
}
