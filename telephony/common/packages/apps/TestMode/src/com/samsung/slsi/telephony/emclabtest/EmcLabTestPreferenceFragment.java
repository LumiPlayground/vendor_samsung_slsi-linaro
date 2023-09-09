/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */
package com.samsung.slsi.telephony.emclabtest;

import android.content.Context;
import android.content.Intent;
import android.os.AsyncResult;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.os.SystemProperties;
import android.preference.Preference;
import android.preference.PreferenceFragment;
import android.preference.SwitchPreference;
import android.util.Log;

import com.samsung.slsi.telephony.oem.OemRil;
import com.samsung.slsi.telephony.oem.io.DataWriter;
import com.samsung.slsi.telephony.testmode.R;

import java.io.IOException;

public class EmcLabTestPreferenceFragment extends PreferenceFragment
    implements Preference.OnPreferenceChangeListener {

    private static final String TAG = "EmcLabTest";

    private static final String PROPERTY_EMC_LAB_TEST_ENABLED = "persist.vendor.radio.emc_lab_test_enabled";

    private SwitchPreference mEmcLabTestPref;
    private boolean mEnabled;
    private OemRil mOemRil;

    private static final int EVENT_RIL_CONNECTED        = 100;
    private static final int EVENT_RIL_DISCONNECTED     = 101;

    private static final int EVENT_SET_COMPLETE = 1;

    private static final int RILC_REQ_SET_EMC_LAB_TEST_MODE = 51;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        addPreferencesFromResource(R.layout.emc_lab_test_activity);
        connectToOemRilService();
    }

    public static EmcLabTestPreferenceFragment newInstance() {
        EmcLabTestPreferenceFragment frag = new EmcLabTestPreferenceFragment();
        return frag;
    }

    @Override
    public void onResume() {
        super.onResume();

        mEmcLabTestPref = (SwitchPreference) findPreference("key_emc_lab_test");
        mEmcLabTestPref.setOnPreferenceChangeListener(this);

        updateCurrentState();
    }

    private void updateCurrentState() {
        String mode = String.valueOf(SystemProperties.get(PROPERTY_EMC_LAB_TEST_ENABLED, "0"));
        if (mode.equals("1")) {
            mEmcLabTestPref.setChecked(true);
            mEmcLabTestPref.setSummary("On");
        } else {
            mEmcLabTestPref.setChecked(false);
            mEmcLabTestPref.setSummary("Off");
        }
    }

    private Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            AsyncResult ar = (AsyncResult) msg.obj;
            switch (msg.what) {
                case EVENT_SET_COMPLETE:
                    if (ar.exception == null) {
                        Log.d(TAG, "Success");
                    } else {
                        Log.d(TAG, "Fail: " + msg.what);
                    }
                    break;
            }
        }
    };

    @Override
    public boolean onPreferenceChange(Preference preference, Object newValue) {
        Log.d(TAG, "Changed to " + newValue);
        if (preference.getKey().equals("key_emc_lab_test")) {
            if (mOemRil != null) {
                DataWriter dr = new DataWriter();
                if (newValue.toString().equals("true")) {
                    try {
                        dr.writeInt(1);
                    } catch (IOException e) {
                        Log.i(TAG, "SendData() IOException" + e);
                    }
                    preference.setSummary("On");
                    SystemProperties.set(PROPERTY_EMC_LAB_TEST_ENABLED, "1");
                } else {
                    try {
                        dr.writeInt(0);
                    } catch (IOException e) {
                        Log.i(TAG, "SendData() IOException" + e);
                    }
                    preference.setSummary("Off");
                    SystemProperties.set(PROPERTY_EMC_LAB_TEST_ENABLED, "0");
                }
                mOemRil.invokeRequestRaw(RILC_REQ_SET_EMC_LAB_TEST_MODE,
                            dr.toByteArray(), mHandler.obtainMessage(EVENT_SET_COMPLETE));
                return true;
            }
        }
        return false;
    }

    private void connectToOemRilService() {
        mOemRil = OemRil.init(getContext(), 0);
        if (mOemRil == null) {
            Log.d(TAG, "connectToOemRilService mOemRil is null");
        } else {
            mOemRil.registerForOemRilConnected(mHandler, EVENT_RIL_CONNECTED);
            mOemRil.registerForOemRilDisconnected(mHandler, EVENT_RIL_DISCONNECTED);
        }
    }
}
