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

import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.widget.Toast;

import androidx.preference.Preference;
import androidx.preference.PreferenceFragmentCompat;
import androidx.preference.PreferenceScreen;

import java.util.ArrayList;
import java.util.Arrays;

import static com.samsung.slsi.telephony.carrierconfigoverride.CarrierConfigList.ENTRY_CONFIGS;

public class MainFragment extends PreferenceFragmentCompat {

    private static final String TAG = "MainFragment";
    CarrierConfigHelper mCi;
    ArrayList<BaseSwitchPreference> mPrefList = new ArrayList<>();

    @Override
    public void onCreateOptionsMenu(Menu menu, MenuInflater inflater) {
        super.onCreateOptionsMenu(menu, inflater);
        inflater.inflate(R.menu.options_menu, menu);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        Log.d(TAG, "onOptionsItemSelected");
        if (item.getItemId() == R.id.action_menu) {
            restoreConfig();
        }
        return super.onOptionsItemSelected(item);
    }

    @Override
    public void onCreatePreferences(Bundle savedInstanceState, String rootKey) {
        setPreferencesFromResource(R.xml.preference_main, rootKey);
        setHasOptionsMenu(true);

        Bundle bundle = getArguments();
        if (bundle != null) {

            int subId = bundle.getInt("subId", -1);
            mCi = new CarrierConfigHelper(getContext(), subId);
            BaseSwitchPreference.BooleanTypeManager booleanTypeManager = new BaseSwitchPreference.BooleanTypeManager() {

                @Override
                public boolean getBooleanValue(String key) {
                    return mCi.getConfigForBoolean(key);
                }

                @Override
                public void setBooleanValue(String key, boolean value) {
                    mCi.overrideConfigForBoolean(key, value);
                }
            };

            BaseSwitchPreference.BooleanTypeManager allowToEditApnManager = new BaseSwitchPreference.BooleanTypeManager() {

                String[] allReadOnly = new String[] { "*" };

                @Override
                public boolean getBooleanValue(String key) {
                    String res[] = mCi.getConfigForStringArray(key);
                    return !Arrays.equals(res, allReadOnly);
                }

                @Override
                public void setBooleanValue(String key, boolean value) {
                    String[] strings = null;
                    if (!value) strings = allReadOnly;
                    mCi.overrideConfigForStringArray(key, strings);
                }
            };

            PreferenceScreen screen = this.getPreferenceScreen();
            screen.removeAll();

            for (int i = 0; i < ENTRY_CONFIGS.size(); i++) {
                CarrierConfig config = ENTRY_CONFIGS.get(i);
                if (config.type == ValueType.TYPE_BASE_SWITCH_PREFERENCE) {
                    BaseSwitchPreference preference = new BaseSwitchPreference(getContext(), booleanTypeManager);
                    mPrefList.add(preference);
                    addPreference(screen, preference, config);
                } else if (config.type == ValueType.TYPE_ALLOW_TO_ADD_APNS_PREFERENCE) {
                    BaseSwitchPreference preference = new BaseSwitchPreference(getContext(), allowToEditApnManager);
                    mPrefList.add(preference);
                    addPreference(screen, preference, config);
                }
            }
        } else {
            Log.e(TAG, "Invalid Sub Id");
        }
    }

    private void addPreference(PreferenceScreen screen, Preference preference, CarrierConfig config) {
        preference.setTitle(config.title);
        preference.setSummary(config.summary);
        preference.setKey(config.key);
        screen.addPreference(preference);
    }

    private void restoreConfig() {
        mCi.restoreConfig();
        Toast.makeText(getContext(), "Success to Restore Config", Toast.LENGTH_LONG).show();

        for (BaseSwitchPreference preference : mPrefList) {
            preference.update();
        }
    }
}