/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd All Rights Reserved
 * PROPRIETARY/CONFIDENTIAL
 *
 * This software is the confidential and proprietary information of
 * SAMSUNG ELECTRONICS ("Confidential Information").
 *
 * You shall not disclose such Confidential Information and shall use it
 * only in accordance with the terms of the license agreement
 * you entered into with SAMSUNG ELECTRONICS.
 *
 * SAMSUNG make no representations or warranties about the suitability of
 * the software, either express or implied, including but not limited to
 * the implied warranties of merchantability, fitness for a particular
 * purpose, or non-infringement. SAMSUNG shall not be liable for any
 * damages suffered by license as a result of using, modifying or
 * distributing this software or its derivatives.
 */
package com.samsung.slsi.telephony.carrierconfigoverride;

import android.content.Context;
import android.os.PersistableBundle;
import android.telephony.CarrierConfigManager;
import android.util.Log;

public class CarrierConfigHelper {

    private Context mContext = null;
    private int mSubId = -1;
    private CarrierConfigManager mCarrierConfigManager;
    private static final String TAG = "CarrierConfigHelper";

    public static CarrierConfigHelper from(Context context, int subId) {
        return new CarrierConfigHelper(context, subId);
    }

    CarrierConfigHelper(Context context, int subId) {
        mContext = context;
        mSubId = subId;
        mCarrierConfigManager = mContext.getSystemService(CarrierConfigManager.class);
    }

    public void restoreConfig() {
        Log.d(TAG, "restoreConfig()");
        mCarrierConfigManager.overrideConfig(mSubId, null, true);
    }

    public boolean getConfigForBoolean(String key) {
        PersistableBundle bundle = mCarrierConfigManager.getConfigForSubId(mSubId);
        if (bundle != null) {
            return bundle.getBoolean(key, false);
        }
        return false;
    }

    public void overrideConfigForBoolean(String key, boolean value) {
        PersistableBundle overrideBundle = new PersistableBundle();
        overrideBundle.putBoolean(key, value);
        mCarrierConfigManager.overrideConfig(mSubId, overrideBundle, true);
    }

    public String[] getConfigForStringArray(String key) {
        PersistableBundle bundle = mCarrierConfigManager.getConfigForSubId(mSubId);
        if (bundle != null) {
            return bundle.getStringArray(key);
        }
        return null;
    }

    public void overrideConfigForStringArray(String key, String[] value) {
        PersistableBundle overrideBundle = new PersistableBundle();
        overrideBundle.putStringArray(key, value);
        mCarrierConfigManager.overrideConfig(mSubId, overrideBundle, true);
    }
}
