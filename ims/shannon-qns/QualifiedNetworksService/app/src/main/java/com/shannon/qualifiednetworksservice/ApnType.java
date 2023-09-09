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

import android.net.NetworkCapabilities;
import android.telephony.data.ApnSetting;

import com.android.internal.telephony.PhoneConstants;

public enum ApnType {
    APN_DEFAULT(ApnSetting.TYPE_DEFAULT, NetworkCapabilities.NET_CAPABILITY_INTERNET, PhoneConstants.APN_TYPE_DEFAULT, false, "INTERNET"),
    APN_MMS(ApnSetting.TYPE_MMS, NetworkCapabilities.NET_CAPABILITY_MMS, PhoneConstants.APN_TYPE_MMS, true, "MMS"),
    APN_SUPL(ApnSetting.TYPE_SUPL, NetworkCapabilities.NET_CAPABILITY_SUPL, PhoneConstants.APN_TYPE_SUPL, false, "SUPL"),
    APN_DUN(ApnSetting.TYPE_DUN, NetworkCapabilities.NET_CAPABILITY_DUN, PhoneConstants.APN_TYPE_DUN, false, "DUN"),
    APN_HIPRI(ApnSetting.TYPE_HIPRI, -1, PhoneConstants.APN_TYPE_HIPRI, false, "HIPRI"),
    APN_FOTA(ApnSetting.TYPE_FOTA, NetworkCapabilities.NET_CAPABILITY_FOTA, PhoneConstants.APN_TYPE_FOTA, false, "FOTA"),
    APN_IMS(ApnSetting.TYPE_IMS, NetworkCapabilities.NET_CAPABILITY_IMS, PhoneConstants.APN_TYPE_IMS, true, "IMS"),
    APN_CBS(ApnSetting.TYPE_CBS, NetworkCapabilities.NET_CAPABILITY_CBS, PhoneConstants.APN_TYPE_CBS, true, "CBS"),
    APN_IA(ApnSetting.TYPE_IA, -1, PhoneConstants.APN_TYPE_IA, false, "IA"),
    APN_EMERGENCY(ApnSetting.TYPE_EMERGENCY, NetworkCapabilities.NET_CAPABILITY_EIMS, PhoneConstants.APN_TYPE_EMERGENCY, true, "EIMS"),
    APN_MCX(ApnSetting.TYPE_MCX, NetworkCapabilities.NET_CAPABILITY_MCX, PhoneConstants.APN_TYPE_MCX, false, "MCX"),
    APN_XCAP(ApnSetting.TYPE_XCAP, NetworkCapabilities.NET_CAPABILITY_XCAP, PhoneConstants.APN_TYPE_XCAP, true, "XCAP");

    private static final int MODEM_TYPE_DEFAULT = 0;
    private static final int MODEM_TYPE_EMERGENCY = 1;
    private static final int MODEM_TYPE_IMS = 2;

    private final int mType;
    private final int mCapability;
    private final String mName;
    private final boolean mSupported;
    private final String mCapabilityName;

    ApnType(int type, int capability, String name, boolean supported, String capabilityName) {
        mType = type;
        mCapability = capability;
        mName = name;
        mSupported = supported;
        mCapabilityName = capabilityName;
    }

    public int getType() { return mType; }
    public int getCapability() { return mCapability; }
    public boolean isSupported() { return mSupported; }

    public int getModemType() {
        if (mType == ApnSetting.TYPE_IMS) {
            return MODEM_TYPE_IMS;
        }
        if (mType == ApnSetting.TYPE_EMERGENCY) {
            return MODEM_TYPE_EMERGENCY;
        }
        return MODEM_TYPE_DEFAULT;
    }

    private boolean isImsOrEmergency() {
        return (mType == ApnSetting.TYPE_IMS) || (mType == ApnSetting.TYPE_EMERGENCY);
    }

    public static ApnType getEnum(int type) {
        for (ApnType e : ApnType.values()) {
            if (e.mType == type) {
                return e;
            }
        }
        return null;
    }

    public static String getBroadcastType(int type) {
        for (ApnType e : ApnType.values()) {
            if (e.mType == type) {
                return e.mName;
            }
        }
        return "UNKNOWN";
    }

    public static int getApnBitmask() {
        int bitmask = 0;

        for (ApnType e : ApnType.values()) {
            if (e.mSupported) {
                bitmask |= e.mType;
            }
        }
        return bitmask;
    }

    public static int getApnBitmaskWithoutImsAndEmergency() {
        int bitmask = 0;

        for (ApnType e : ApnType.values()) {
            if (e.mSupported && !e.isImsOrEmergency()) {
                bitmask |= e.mType;
            }
        }
        return bitmask;
    }

    public static int getSupportedApnType(int bitmask) {
        for (ApnType e : ApnType.values()) {
            if (e.mSupported && ((bitmask & e.mType) != 0)) {
                return e.mType;
            }
        }
        return ApnSetting.TYPE_NONE;
    }

    public static int getApnBitmaskForCall() {
        int bitmask = 0;

        if (APN_IMS.isSupported()) {
            bitmask |= ApnSetting.TYPE_IMS;
        }
        if (APN_EMERGENCY.isSupported()) {
            bitmask |= ApnSetting.TYPE_EMERGENCY;
        }
        return bitmask;
    }

    public static String getCapabilityNameOf(int type) {
        for (ApnType e : ApnType.values()) {
            if (e.mType == type) {
                return e.mCapabilityName;
            }
        }
        return "";
    }
}
