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

import android.telephony.data.ApnSetting;

public enum ApnType {
    APN_DEFAULT(1, ApnSetting.TYPE_DEFAULT, "default", 0),
    APN_MMS(3, ApnSetting.TYPE_MMS, "mms", 0),
    APN_SUPL(5, ApnSetting.TYPE_SUPL, "supl", 0),
    APN_DUN(7, ApnSetting.TYPE_DUN, "dun", 0),
    APN_HIPRI(9, 0, "hipri", 0),
    APN_FOTA(11, ApnSetting.TYPE_FOTA, "fota", 0),
    APN_IMS(13, ApnSetting.TYPE_IMS, "ims", 2),
    APN_CBS(15, ApnSetting.TYPE_CBS, "cbs", 0),
    APN_IA(17, 0, "ia", 0),
    APN_EMERGENCY(19, ApnSetting.TYPE_EMERGENCY, "emergency", 1),
    APN_MCX(21, 0, "mcx", 0),
    APN_XCAP(23, ApnSetting.TYPE_XCAP, "xcap", 0);

    private final int mCode;
    private final int mBit;
    private final String mUserFriendlyName;
    private final int mType;

    ApnType(int code, int bit, String userName, int type) {
        mCode = code;
        mBit = bit;
        mUserFriendlyName = userName;
        mType = type;
    }

    public int getCode(int slotId) {
        return (mCode + slotId);
    }
    public int getBit() { return mBit; }
    public int getType() { return mType; }
    public String getUserFriendlyName() { return mUserFriendlyName; }

    public static ApnType getByCode(int code) {
        for (ApnType apn : ApnType.values()) {
            if (apn.mCode == code) {
                return apn;
            }
        }
        return null;
    }

    public static ApnType getByBitmask(int apnBitmask) {
        for (ApnType apn : ApnType.values()) {
            if ((apnBitmask & apn.mBit) != 0) {
                return apn;
            }
        }
        return null;
    }
}
