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

import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.os.Bundle;
import android.os.PersistableBundle;
import android.os.PowerManager;
import android.preference.ListPreference;
import android.preference.Preference;
import android.preference.PreferenceFragment;
import android.telephony.CarrierConfigManager;
import android.telephony.SubscriptionManager;
import android.util.Log;
import android.view.WindowManager;

import com.samsung.slsi.telephony.testmode.R;

public class UsageSettingPreferenceFragment extends PreferenceFragment {

    private static final String TAG = "CeModeTest";
    private ListPreference mUsageSettingPref;
    private SubscriptionManager mSubscriptionManager;
    private CarrierConfigManager mCarrierConfigManager;
    private int mPreferredUsageSetting = SubscriptionManager.USAGE_SETTING_UNKNOWN;
    private int mSubId = -1;
    int mPhoneId = -1;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        addPreferencesFromResource(R.layout.usage_setting_activity);

        mUsageSettingPref = (ListPreference) findPreference("key_usage_setting");
        mUsageSettingPref.setOnPreferenceChangeListener(new Preference.OnPreferenceChangeListener() {

            @Override
            public boolean onPreferenceChange(Preference preference, Object newValue) {
                int newPreferredUsageSetting = Integer.parseInt(newValue.toString());
                if (newPreferredUsageSetting != mPreferredUsageSetting) {
                    setPreferredUsageSettingByCarrierConfig(newPreferredUsageSetting);
                    mPreferredUsageSetting = newPreferredUsageSetting;
                    updatePreferenceSummary(mPreferredUsageSetting);
                }
                return true;
            }
        });

        mSubscriptionManager = getContext().getSystemService(SubscriptionManager.class);
        mCarrierConfigManager = getContext().getSystemService(CarrierConfigManager.class);
    }

    public static UsageSettingPreferenceFragment newInstance() {
        UsageSettingPreferenceFragment frag = new UsageSettingPreferenceFragment();
        return frag;
    }

    @Override
    public void onResume() {
        mSubId = getSubId();
        getPreferredUsageSetting();
        super.onResume();
    }

    private void setPreferredUsageSettingByCarrierConfig(int newUsageSetting) {
        PersistableBundle overrideBundle = new PersistableBundle();
        overrideBundle.putInt(CarrierConfigManager.KEY_CELLULAR_USAGE_SETTING_INT, newUsageSetting);
        if (overrideBundle == null) {
            Log.e(TAG, "overrideBundle is null");
            return;
        }

        mCarrierConfigManager.overrideConfig(getSubId(), overrideBundle, true);
    }

    private void setPreferredUsageSetting(String newUsageSetting) {
        Log.d(TAG, "setPreferredUsageSetting() newUsageSetting=" + newUsageSetting);
        mSubscriptionManager.setSubscriptionProperty(mSubId, SubscriptionManager.USAGE_SETTING, newUsageSetting);
        reboot();
    }

    private void getPreferredUsageSetting() {
        mPreferredUsageSetting = mSubscriptionManager.getIntegerSubscriptionProperty(mSubId, SubscriptionManager.USAGE_SETTING, SubscriptionManager.USAGE_SETTING_UNKNOWN, getContext());
        Log.d(TAG, "subId = " + mSubId + ", usageSetting=" + mPreferredUsageSetting);
        if (mPreferredUsageSetting == SubscriptionManager.USAGE_SETTING_VOICE_CENTRIC || mPreferredUsageSetting == SubscriptionManager.USAGE_SETTING_DATA_CENTRIC) {
            mUsageSettingPref.setValue(Integer.toString(mPreferredUsageSetting));
        } else {
            mUsageSettingPref.setValue("");
        }
        updatePreferenceSummary(mPreferredUsageSetting);
    }

    private void updatePreferenceSummary(int usageSetting) {

        switch(usageSetting) {
        case SubscriptionManager.USAGE_SETTING_VOICE_CENTRIC:
            mUsageSettingPref.setSummary(mUsageSettingPref.getEntries()[0]);
            break;
        case SubscriptionManager.USAGE_SETTING_DATA_CENTRIC:
            mUsageSettingPref.setSummary(mUsageSettingPref.getEntries()[1]);
            break;
        default:
            mUsageSettingPref.setSummary("Invalid usageSetting=" + usageSetting);
            break;
        }
    }

    private int getSubId() {
        if (mSubscriptionManager == null) {
            return SubscriptionManager.DEFAULT_SUBSCRIPTION_ID;
        }

        int[] subIds = mSubscriptionManager.getSubscriptionIds(mPhoneId);
        if (subIds == null || subIds.length == 0) {
            return SubscriptionManager.INVALID_SUBSCRIPTION_ID;
        }
        return subIds[0];
    }

    private void reboot() {
        final PowerManager pm = (PowerManager) getContext().getSystemService(Context.POWER_SERVICE);
        AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());
        builder.setMessage("Reboot the device")
                .setPositiveButton("OK", new DialogInterface.OnClickListener() {

                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        pm.reboot(null);
                    }
                }).setNegativeButton("Cancel", null);
        final AlertDialog resetDialog = builder.create();
        resetDialog.getWindow().setType(WindowManager.LayoutParams.TYPE_SYSTEM_ALERT);
        resetDialog.show();
    }

    public static UsageSettingPreferenceFragment newInstance(int phoneId) {
        Log.d(TAG, "phoneId=" + phoneId);
        UsageSettingPreferenceFragment frag = new UsageSettingPreferenceFragment();
        frag.mPhoneId = phoneId;
        return frag;
    }
}
