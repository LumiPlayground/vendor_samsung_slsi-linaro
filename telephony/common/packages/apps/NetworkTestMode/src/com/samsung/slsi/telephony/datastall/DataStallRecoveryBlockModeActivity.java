/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */
package com.samsung.slsi.telephony.datastall;

import android.os.Bundle;
import android.os.SystemProperties;
import android.preference.Preference;
import android.preference.PreferenceActivity;
import android.preference.PreferenceScreen;
import android.preference.SwitchPreference;
import android.provider.Settings;
import android.util.Log;
import android.widget.Toast;

import com.samsung.slsi.telephony.networktestmode.R;

public class DataStallRecoveryBlockModeActivity extends PreferenceActivity implements Preference.OnPreferenceChangeListener {

    private static final String TAG = "DataStallRecoveryBlockModeActivity";
    private static final String PROPERTY_DATA_STALL_RECOVERY_BLOCK = "persist.vendor.ril.datastallrecoveryblock";

    private static final String CAPTIVE_PORTAL_MODE = "captive_portal_mode";
    private static final String APP_DATA_STALL_RECOVERY_ON_BAD_NETWORK = "data_stall_recovery_on_bad_network";
    private static final String APP_DATA_STALL_ALARM_NON_AGGRESSIVE_DELAY_IN_MS_DEFAULT = "data_stall_alarm_non_aggressive_delay_in_ms";
    private static final String APP_DATA_STALL_ALARM_AGGRESSIVE_DELAY_IN_MS_DEFAULT = "data_stall_alarm_aggressive_delay_in_ms";

    protected static final int MAX_DELAY = 6000000;
    // Default for the data stall alarm while non-aggressive stall detection
    protected static final int DEFAULT_DATA_STALL_ALARM_NON_AGGRESSIVE_DELAY_IN_MS_DEFAULT = 1000 * 60 * 6;
    // Default for the data stall alarm for aggressive stall detection
    protected static final int DEFAULT_DATA_STALL_ALARM_AGGRESSIVE_DELAY_IN_MS_DEFAULT = 1000 * 60;


    private SwitchPreference mDataStallRecoveryBlockBtn;

    private boolean mFirstTime = true;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        addPreferencesFromResource(R.layout.preference);

        mDataStallRecoveryBlockBtn = new SwitchPreference(this);
        if (mDataStallRecoveryBlockBtn != null) {
            mDataStallRecoveryBlockBtn.setTitle("DataStallRecoveryBlockMode");
            mDataStallRecoveryBlockBtn.setKey("key_data_stall_recovery_block");
            mDataStallRecoveryBlockBtn.setOnPreferenceChangeListener(this);
        }

        fillList();
    }

    private void fillList() {
        PreferenceScreen root = (PreferenceScreen) findPreference("key_pref_screen");

        if (root != null) {
            root.removeAll();
            root.addPreference(mDataStallRecoveryBlockBtn);
        }
    }

    @Override
    public void onResume() {
        super.onResume();
        if (mFirstTime) {
            updateCurrentMode();
            mFirstTime = false;
        }
    }

    private void updateCurrentMode() {
        String mode = String.valueOf(SystemProperties.get(PROPERTY_DATA_STALL_RECOVERY_BLOCK, "0"));
        if (mode.equals("1")) {
            Toast.makeText(getApplicationContext(), "DataStallRecovery: Block", Toast.LENGTH_SHORT).show();
            mDataStallRecoveryBlockBtn.setChecked(true);
            mDataStallRecoveryBlockBtn.setSummary("on");
        } else {
            mDataStallRecoveryBlockBtn.setChecked(false);
            mDataStallRecoveryBlockBtn.setSummary("off");
        }
    }

    @Override
    public boolean onPreferenceChange(Preference preference, Object newValue) {
        Log.d(TAG, "Changed to " + newValue);
        if (preference.getKey().equals("key_data_stall_recovery_block")) {
            if (newValue.toString().equals("true")) {
                preference.setSummary("on");
                SystemProperties.set(PROPERTY_DATA_STALL_RECOVERY_BLOCK, "1");
                Settings.Global.putInt(getApplicationContext().getContentResolver(), CAPTIVE_PORTAL_MODE, 0);
                Settings.Global.putInt(getApplicationContext().getContentResolver(), APP_DATA_STALL_RECOVERY_ON_BAD_NETWORK, 0);
                Settings.Global.putInt(getApplicationContext().getContentResolver(), APP_DATA_STALL_ALARM_NON_AGGRESSIVE_DELAY_IN_MS_DEFAULT, MAX_DELAY);
                Settings.Global.putInt(getApplicationContext().getContentResolver(), APP_DATA_STALL_ALARM_AGGRESSIVE_DELAY_IN_MS_DEFAULT, MAX_DELAY);
                //Log
                Log.d(TAG, "ON");
                Log.d(TAG, "(1)DATA_STALL_RECOVERY_ON_BAD_NETWORK: " + Settings.Global.getInt(getApplicationContext().getContentResolver(), APP_DATA_STALL_RECOVERY_ON_BAD_NETWORK,-1));
                Log.d(TAG, "(2)DATA_STALL_ALARM_NON_AGGRESSIVE_DELAY_IN_MS_DEFAULT: " + Settings.Global.getInt(getApplicationContext().getContentResolver(), APP_DATA_STALL_ALARM_NON_AGGRESSIVE_DELAY_IN_MS_DEFAULT,-1));
                Log.d(TAG, "(3)DATA_STALL_ALARM_AGGRESSIVE_DELAY_IN_MS_DEFAULT: " + Settings.Global.getInt(getApplicationContext().getContentResolver(), APP_DATA_STALL_ALARM_AGGRESSIVE_DELAY_IN_MS_DEFAULT,-1));
                Log.d(TAG, "(4)CAPTIVE_PORTAL_MODE: " + Settings.Global.getInt(getApplicationContext().getContentResolver(), CAPTIVE_PORTAL_MODE,-1));
            } else {
                preference.setSummary("off");
                SystemProperties.set(PROPERTY_DATA_STALL_RECOVERY_BLOCK, "0");
                Settings.Global.putInt(getApplicationContext().getContentResolver(), APP_DATA_STALL_RECOVERY_ON_BAD_NETWORK, 1);
                Settings.Global.putInt(getApplicationContext().getContentResolver(), APP_DATA_STALL_ALARM_NON_AGGRESSIVE_DELAY_IN_MS_DEFAULT, DEFAULT_DATA_STALL_ALARM_NON_AGGRESSIVE_DELAY_IN_MS_DEFAULT);
                Settings.Global.putInt(getApplicationContext().getContentResolver(), APP_DATA_STALL_ALARM_AGGRESSIVE_DELAY_IN_MS_DEFAULT, DEFAULT_DATA_STALL_ALARM_AGGRESSIVE_DELAY_IN_MS_DEFAULT);
                //Log
                Log.d(TAG, "OFF");
                Log.d(TAG, "(1)DATA_STALL_RECOVERY_ON_BAD_NETWORK: " + Settings.Global.getInt(getApplicationContext().getContentResolver(), APP_DATA_STALL_RECOVERY_ON_BAD_NETWORK,-1));
                Log.d(TAG, "(2)DATA_STALL_ALARM_NON_AGGRESSIVE_DELAY_IN_MS_DEFAULT: " + Settings.Global.getInt(getApplicationContext().getContentResolver(), APP_DATA_STALL_ALARM_NON_AGGRESSIVE_DELAY_IN_MS_DEFAULT,-1));
                Log.d(TAG, "(3)DATA_STALL_ALARM_AGGRESSIVE_DELAY_IN_MS_DEFAULT: " + Settings.Global.getInt(getApplicationContext().getContentResolver(), APP_DATA_STALL_ALARM_AGGRESSIVE_DELAY_IN_MS_DEFAULT,-1));
                Log.d(TAG, "(4)CAPTIVE_PORTAL_MODE: " + Settings.Global.getInt(getApplicationContext().getContentResolver(), CAPTIVE_PORTAL_MODE,-1));
            }
            return true;
        }
        return false;
    }
}