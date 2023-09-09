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
package com.samsung.slsi.telephony.autoanswer;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.preference.ListPreference;
import androidx.preference.Preference;
import androidx.preference.PreferenceFragmentCompat;
import androidx.preference.SwitchPreference;

public class AutoAnswerPreferenceFragment extends PreferenceFragmentCompat implements Preference.OnPreferenceChangeListener {

    private static final String TAG = "AutoAnswer";

    @Override
    public void onCreatePreferences(@Nullable Bundle savedInstanceState, @Nullable String rootKey) {
        setPreferencesFromResource(R.xml.preference_main, rootKey);

        SwitchPreference autoModePref = (SwitchPreference) findPreference(Constants.KEY_AUTO_MODE);
        autoModePref.setOnPreferenceChangeListener(this);

        ListPreference answerTimePref = (ListPreference) findPreference(Constants.KEY_ANSWER_TIME);
        answerTimePref.setOnPreferenceChangeListener(this);
        answerTimePref.setSummary(answerTimePref.getValue());
    }

    @Override
    public boolean onPreferenceChange(@NonNull Preference preference, Object newValue) {
        Log.d(TAG, "onPreferenceChange() : " + preference.getKey() + " changed to " + newValue);
        if (Constants.KEY_AUTO_MODE.equals(preference.getKey())) {
            Intent intent = new Intent(getContext(), CallService.class);
            if ((Boolean) newValue) {
                getContext().startForegroundService(intent);
            } else {
                getContext().stopService(intent);
            }
            return true;
        } else if (Constants.KEY_ANSWER_TIME.equals(preference.getKey())) {
            preference.setSummary(newValue.toString());
            return true;
        }
        return false;
    }
}