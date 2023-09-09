/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or
 * distributed, transmitted, transcribed, stored in a retrieval system or
 * translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed to third parties
 * without the express written permission of Samsung Electronics.
 */
package com.shannon.dataservice.util;

import android.content.Context;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.os.Build.VERSION;
import android.telephony.Rlog;

import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;

import com.shannon.dataservice.ShannonDataService;

public class PackageVersion {
    private static final String TAG = ShannonDataService.getServiceTAG("-Version");

    private final String mVersion;
    private final String mLastUpdateTime;
    private final int mTargetSdkInt;
    private final int mBuildSdkInt;

    public PackageVersion(Context context) {
        String versionName = "N/A";
        String lastUpdateTime = "N/A";
        int targetSdk = VERSION.SDK_INT;
        try {
            PackageManager pm = context.getPackageManager();
            PackageInfo info = pm.getPackageInfo(context.getPackageName(), 0);

            versionName = info.versionName;
            SimpleDateFormat sdf = new SimpleDateFormat("dd MMM yyyy HH:mm:ss z", Locale.US);
            lastUpdateTime = sdf.format(new Date(info.lastUpdateTime));
            targetSdk = context.getApplicationInfo().targetSdkVersion;
        } catch (Exception e) {
            loge("Failed to get package information " + e);
        }
        mVersion = versionName;
        mLastUpdateTime = lastUpdateTime;
        mTargetSdkInt = targetSdk;
        mBuildSdkInt = VERSION.SDK_INT;
    }

    public void print() {
        logw("Change: " + mVersion + ", updated: " + mLastUpdateTime +
                ", target: " + mTargetSdkInt + ", built: " + mBuildSdkInt);
    }

    protected void loge(String s) {
        Rlog.e(TAG, s);
    }

    protected void logw(String s) {
        Rlog.w(TAG, s);
    }
}
