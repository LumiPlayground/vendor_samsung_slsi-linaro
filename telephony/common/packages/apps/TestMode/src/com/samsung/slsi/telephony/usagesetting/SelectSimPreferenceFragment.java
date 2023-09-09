/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

package com.samsung.slsi.telephony.usagesetting;

import android.os.Bundle;
import android.preference.Preference;
import android.preference.PreferenceFragment;
import android.preference.PreferenceScreen;
import android.telephony.TelephonyManager;
import android.text.TextUtils;
import android.util.Log;

import com.samsung.slsi.telephony.testmode.R;

public class SelectSimPreferenceFragment extends PreferenceFragment {

    private static final String TAG = "SelectSimPreferenceFragment";
    private static final int SIM1 = 0;
    private static final int SIM2 = 1;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        addPreferencesFromResource(R.layout.preference_select_sim);

        initUi();
    }

    private void initUi() {
        TelephonyManager telephonyManager = getContext().getSystemService(TelephonyManager.class);
        Preference sim1 = findPreference("key_sim1");
        Preference sim2 = findPreference("key_sim2");
        String simOperator1 = telephonyManager.getSimOperatorNumericForPhone(SIM1);
        String simOperator2 = telephonyManager.getSimOperatorNumericForPhone(SIM2);
        String operatorName1 = telephonyManager.getSimOperatorNameForPhone(SIM1);
        String operatorName2 = telephonyManager.getSimOperatorNameForPhone(SIM2);
        Log.d(TAG, "sim1: " + simOperator1 + ", sim2: " + simOperator2);
        if (TextUtils.isEmpty(simOperator1)) {
            setNoSimPreference(sim1);
        } else {
            sim1.setSummary(operatorName1);
        }
        if (TextUtils.isEmpty(simOperator2)) {
            setNoSimPreference(sim2);
        } else {
            sim2.setSummary(operatorName2);
        }
    }

    private void setNoSimPreference(Preference preference) {
        preference.setEnabled(false);
        preference.setSummary("No SIM");
    }

    @Override
    public boolean onPreferenceTreeClick(PreferenceScreen preferenceScreen,
            Preference preference) {
        UsageSettingActivity activity = (UsageSettingActivity) getActivity();
        if (preference.getKey().equals("key_sim1")) {
            activity.onNextFragment(SIM1);
        } else if (preference.getKey().equals("key_sim2")) {
            activity.onNextFragment(SIM2);
        }
        return super.onPreferenceTreeClick(preferenceScreen, preference);
    }

    public static SelectSimPreferenceFragment newInstance() {
        SelectSimPreferenceFragment frag = new SelectSimPreferenceFragment();
        return frag;
    }
}
