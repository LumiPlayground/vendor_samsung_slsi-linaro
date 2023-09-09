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
package com.shannon.qualifiednetworksservice.util;

import android.telephony.Rlog;
import android.telephony.data.ApnSetting;

import static android.util.Log.DEBUG;
import static android.util.Log.INFO;
import static android.util.Log.VERBOSE;

public class LogUtils {
    /* bits 0-15 allocated for APNs */
    public static final int LOG_CONDITIONS = (1 << 17);

    /* *
    * log tags for "setprop log.tag.<YOUR_LOG_TAG> <LEVEL>" or "setprop persist.log.tag.<YOUR_LOG_TAG> <LEVEL>"
    * Where level is either VERBOSE, DEBUG, INFO, WARN, ERROR, or ASSERT
    *
    * Log levels also could be set in /data/local.prop file that with the following in it: 'log.tag.<YOUR_LOG_TAG>=<LEVEL>'
    *
    * see https://developer.android.com/reference/android/util/Log#isLoggable(java.lang.String,%20int)
    * */
    private static final String LOG_TAG = "ShannonQNS";
    private static final String LOG_TAG_APN_IMS = "ShannonQNS-ims";
    private static final String LOG_TAG_APN_EMERGENCY = "ShannonQNS-emergency";
    private static final String LOG_TAG_APN_MMS = "ShannonQNS-mms";
    private static final String LOG_TAG_APN_XCAP = "ShannonQNS-xcap";
    private static final String LOG_TAG_APN_CBS = "ShannonQNS-cbs";
    private static final String LOG_TAG_CONDITIONS = "ShannonQNS-HC";

    private static boolean isLoggable(int type, int level) {
        switch (type) {
            case ApnSetting.TYPE_IMS:
                return Rlog.isLoggable(LOG_TAG_APN_IMS, level);
            case ApnSetting.TYPE_EMERGENCY:
                return Rlog.isLoggable(LOG_TAG_APN_EMERGENCY, level);
            case ApnSetting.TYPE_MMS:
                return Rlog.isLoggable(LOG_TAG_APN_MMS, level);
            case ApnSetting.TYPE_XCAP:
                return Rlog.isLoggable(LOG_TAG_APN_XCAP, level);
            case ApnSetting.TYPE_CBS:
                return Rlog.isLoggable(LOG_TAG_APN_CBS, level);
            case LOG_CONDITIONS:
                return Rlog.isLoggable(LOG_TAG_CONDITIONS, level);
        }
        return Rlog.isLoggable(LOG_TAG, level);
    }

    public static void v(String TAG, String msg) {
        if (isLoggable(0, VERBOSE)) {
            Rlog.v(TAG, msg);
        }
    }

    public static void v(String TAG, int type, String msg) {
        if (isLoggable(type, VERBOSE)) {
            Rlog.v(TAG, msg);
        }
    }

    public static void d(String TAG, String msg) {
        if (isLoggable(0, DEBUG)) {
            Rlog.d(TAG, msg);
        }
    }

    public static void d(String TAG, int type, String msg) {
        if (isLoggable(type, DEBUG)) {
            Rlog.d(TAG, msg);
        }
    }

    public static void i(String TAG, String msg) {
        if (isLoggable(0, INFO)) {
            Rlog.i(TAG, msg);
        }
    }

    public static void i(String TAG, int type, String msg) {
        if (isLoggable(type, INFO)) {
            Rlog.i(TAG, msg);
        }
    }

    public static void w(String TAG, String msg) {
        Rlog.w(TAG, msg);
    }

    public static void e(String TAG, String msg) {
        Rlog.e(TAG, msg);
    }
}
