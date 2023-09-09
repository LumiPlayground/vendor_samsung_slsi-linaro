/*
 * Copyright Samsung Electronics Co., LTD.
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
package com.samsung.slsi.telephony.oemril;

import com.samsung.slsi.telephony.oemril.multisim.OemMultiSimSettingController;

import android.content.Context;
import android.content.ContextWrapper;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
import android.telephony.data.DataProfile;
import android.util.Log;

public class OemRilGlobals extends ContextWrapper {

    public static final String TAG = "OemRilGlobal";
    private static OemRilGlobals sMe;
    private static VendorRadioContext[] sRadioContext = null;
    private static VendorCommandsInterface[] sCommandsInterfaces = null;

    private Context mContext;
    private TelephonyManager mTelephony;
    private VendorRadioInterfaceManager mRadioInterfaceManager;
    private VendorTelephonyNotifier mNotifier = new VendorTelephonyNotifier() {

        @Override
        public void notifySimTrayState(int phoneId, int state) {
            final VendorTelephonyRegistry radioRegistry = VendorTelephonyRegistry.getInstance();
            if (radioRegistry != null) {
                radioRegistry.notifySimTrayState(phoneId, state);
            }
        }

        @Override
        public void notifyNrDualConnectivityState(int phoneId, boolean enabled, int cause) {
            final VendorTelephonyRegistry radioRegistry = VendorTelephonyRegistry.getInstance();
            if (radioRegistry != null) {
                radioRegistry.notifyNrDualConnectivityState(phoneId, enabled, cause);
            }
        }

        @Override
        public void notifyIccidInfo(int phoneId, String iccid) {
            final VendorTelephonyRegistry radioRegistry = VendorTelephonyRegistry.getInstance();
            if (radioRegistry != null) {
                radioRegistry.notifyIccidInfo(phoneId, iccid);
            }
        }

        @Override
        public void notifyNasTimer(int phoneId, int type, int status, int value, String apn) {
            final VendorTelephonyRegistry radioRegistry = VendorTelephonyRegistry.getInstance();
            if (radioRegistry != null) {
                radioRegistry.notifyNasTimer(phoneId, type, status, value, apn);
            }
        }

        @Override
        public void notifyUnthrottleApn(int phoneId, DataProfile dataProfile) {
            final VendorTelephonyRegistry radioRegistry = VendorTelephonyRegistry.getInstance();
            if (radioRegistry != null) {
                radioRegistry.notifyUnthrottleApn(phoneId, dataProfile);
            }
        }
    };

    /**
     * Returns the singleton instance
     */
    public static OemRilGlobals getInstance() {
        if (sMe == null) {
            throw new IllegalStateException("No OemRilGlobals here!");
        }
        return sMe;
    }

    public OemRilGlobals(Context context) {
        super(context);
        sMe = this;
        mContext = context;
        mTelephony = mContext.getSystemService(TelephonyManager.class);
    }

    public void onCreate() {
        Log.d(TAG, "onCreate...");

        VendorTelephonyRegistry.init(this);

        makeRadioContext();

        mRadioInterfaceManager = VendorRadioInterfaceManager.init(this);

        // Init MultiSimSettingController
        OemMultiSimSettingController.init(this);
    }

    private void makeRadioContext() {
        int numPhones = getActiveModemCount();
        sRadioContext = new VendorRadioContext[numPhones];
        sCommandsInterfaces = new VendorRIL[numPhones];
        for (int i = 0; i < numPhones; i++) {
            sCommandsInterfaces[i] = new VendorRIL(mContext, i);
            sRadioContext[i] = new VendorRadioContext(mContext, sCommandsInterfaces[i], i, mNotifier);
        }
    }

    public int getActiveModemCount() {
        return mTelephony.getActiveModemCount();
    }

    public VendorRadioContext[] getRadioContexts() {
        return sRadioContext;
    }

    public VendorRadioContext getRadioContext(int phoneId) {
        VendorRadioContext radioContext;
        if (phoneId == SubscriptionManager.DEFAULT_PHONE_INDEX) {
            radioContext = sRadioContext[0];
        } else {
            radioContext = (phoneId >= 0 && phoneId < sRadioContext.length)
                        ? sRadioContext[phoneId] : null;
        }
        return radioContext;
    }
}
