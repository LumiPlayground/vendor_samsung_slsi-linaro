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
import android.os.Bundle;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
import android.text.TextUtils;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.navigation.NavController;
import androidx.navigation.Navigation;
import androidx.preference.Preference;
import androidx.preference.PreferenceFragmentCompat;
import androidx.preference.PreferenceScreen;

public class SelectSimFragment extends PreferenceFragmentCompat implements Preference.OnPreferenceClickListener {

    private static final String TAG = "SelectSimFragment";
    private PreferenceScreen mScreen;
    private int mPhoneCnt = 0;

    class PreferenceExt extends Preference {

        private int subId;
        private int phoneId;

        private void setTitle() {
            this.setTitle("SIM" + (this.phoneId+1));
        }

        private void setSummary() {
            TelephonyManager telephonyManager = getContext().getSystemService(TelephonyManager.class);
            String summary = telephonyManager.getSimOperatorNameForPhone(phoneId);
            if (TextUtils.isEmpty(summary)) {
                summary = "No SIM";
                this.setEnabled(false);
            } else {
                summary += " (phoneId:" + phoneId + ", subId:" + subId + ")";
                this.setEnabled(true);
            }
            this.setSummary(summary);
        }

        private int getSubId(int phoneId) {
            SubscriptionManager subscriptionManager = getContext().getSystemService(SubscriptionManager.class);
            if (subscriptionManager == null) {
                return SubscriptionManager.DEFAULT_SUBSCRIPTION_ID;
            }

            int[] subIds = subscriptionManager.getSubscriptionIds(phoneId);
            if (subIds == null || subIds.length == 0) {
                return SubscriptionManager.INVALID_SUBSCRIPTION_ID;
            }
            return subIds[0];
        }


        public PreferenceExt(Context context, int phoneId) {
            super(context);
            this.phoneId = phoneId;
            this.subId = getSubId(phoneId);
            this.getExtras().putInt("subId", this.subId);
        }

        void update() {
            setTitle();
            setSummary();
        }
    }

    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        Log.d(TAG, "onCreateView()");
        mScreen = this.getPreferenceScreen();
        mScreen.removeAll();

        TelephonyManager telephonyManager = getContext().getSystemService(TelephonyManager.class);
        mPhoneCnt = telephonyManager.getActiveModemCount();

        for (int i = 0; i < mPhoneCnt; i++) {
            PreferenceExt preference = new PreferenceExt(getContext(), i);
            preference.setIconSpaceReserved(false);
            preference.setOnPreferenceClickListener(this);
            mScreen.addPreference(preference);
        }
        return super.onCreateView(inflater, container, savedInstanceState);
    }

    @Override
    public void onCreatePreferences(@Nullable Bundle savedInstanceState, @Nullable String rootKey) {
        setPreferencesFromResource(R.xml.preference_main, rootKey);
    }

    @Override
    public void onResume() {
        for (int i = 0; i < mPhoneCnt; i++) {
            PreferenceExt pref = (PreferenceExt) mScreen.getPreference(i);
            pref.update();
        }

        super.onResume();
    }

    @Override
    public boolean onPreferenceClick(Preference preference) {
        Log.d(TAG, getActivity().getLocalClassName());
        NavController navController = Navigation.findNavController(getActivity(), R.id.nav_host_fragment);
        navController.navigate(R.id.action_SelectSimFragment_to_MainFragment, preference.getExtras());
        return true;
    }
}