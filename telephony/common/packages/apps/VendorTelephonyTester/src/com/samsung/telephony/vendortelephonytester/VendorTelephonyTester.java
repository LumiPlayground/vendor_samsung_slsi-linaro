/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */
package com.samsung.telephony.vendortelephonytester;


import android.Manifest;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.ActivityNotFoundException;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.net.Uri;
import android.os.Bundle;
import android.preference.ListPreference;
import android.preference.Preference;
import android.preference.Preference.OnPreferenceChangeListener;
import android.preference.Preference.OnPreferenceClickListener;
import android.preference.PreferenceActivity;
import android.provider.Settings;
import android.telephony.data.DataProfile;
import android.util.Log;
import android.widget.Toast;

import androidx.core.app.ActivityCompat;

import java.util.ArrayList;
import java.util.List;

import com.android.internal.telephony.vendor.VendorTelephonyCallback;
import com.android.internal.telephony.vendor.VendorTelephonyManager;

public class VendorTelephonyTester extends PreferenceActivity {

    private final static String TAG = "VendorTelephonyTester";
    private VendorTelephonyManager mVendorTelephonyManager;

    private String[] permissions = {
        Manifest.permission.MODIFY_PHONE_STATE,
        Manifest.permission.READ_PHONE_STATE,
    };

    private ArrayList<String> permissionList;

    private final int MULTIPLE_PERMISSIONS = 1;

    private boolean checkPermission() {
        int result;
        permissionList = new ArrayList<String>();

        for(String pm : permissions) {
            result = getApplicationContext().checkSelfPermission(pm);
            if (result != PackageManager.PERMISSION_GRANTED) {
                permissionList.add(pm);
            }
        }

        if (!permissionList.isEmpty()) {
            return false;
        }

        return true;
    }

    private void requestPermission() {
        ActivityCompat.requestPermissions(this, permissionList.toArray(new String[permissionList.size()]), MULTIPLE_PERMISSIONS);
    }

    @Override
    public void onRequestPermissionsResult(int requestCode,
            String permissions[], int[] grantResults) {
        switch (requestCode) {
        case MULTIPLE_PERMISSIONS:
            if (!(grantResults.length > 0
                        && grantResults[0] == PackageManager.PERMISSION_GRANTED)) {
                showRequestAgainDialog();
            }
            return;
        }
    }

    private void showRequestAgainDialog() {
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setTitle("Warning: Permission required");
        builder.setMessage("You need to allow a phone permission to use this application without any problem."
                + "Please click a GO button and go to the App info to allow the permission.")
            .setCancelable(false).setPositiveButton("GO", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                try {
                    Intent intent = new Intent(Settings.ACTION_APPLICATION_DETAILS_SETTINGS)
                        .setData(Uri.parse("package:" + getApplicationContext().getPackageName()));
                    startActivity(intent);
                    dialog.cancel();
                } catch (ActivityNotFoundException e) {
                    e.printStackTrace();
                }
            }
        });
        builder.show();
    }

    @Override
    protected void onCreate(Bundle saveInstance) {
        super.onCreate(saveInstance);
        addPreferencesFromResource(R.layout.preference_main);

        if (!checkPermission()) {
            requestPermission();
        }

        mVendorTelephonyManager = VendorTelephonyManager.from(getApplicationContext());

        Preference getNrMode = getPreferenceScreen().findPreference("key_get_nr_mode");
        getNrMode.setOnPreferenceClickListener(new OnPreferenceClickListener() {

            @Override
            public boolean onPreferenceClick(Preference preference) {
                int mode = mVendorTelephonyManager.getNrMode();
                Log.d(TAG, "getNrMode=" + mode);
                preference.setSummary(NrModeToString(mode));
                return true;
            }
        });

        ListPreference setNrMode = (ListPreference) getPreferenceScreen().findPreference("key_set_nr_mode");
        setNrMode.setOnPreferenceChangeListener(new OnPreferenceChangeListener() {

            @Override
            public boolean onPreferenceChange(Preference preference, Object newValue) {
                int mode = Integer.parseInt((String) newValue);
                boolean res = mVendorTelephonyManager.setNrMode(mode);
                Log.d(TAG, "setNrMode=" + res);
                if (res) preference.setSummary(NrModeToString(mode));
                return res;
            }
        });

        ListPreference getSmsStorageOnSim = (ListPreference) getPreferenceScreen().findPreference("key_get_sms_storage_on_sim");
        getSmsStorageOnSim.setOnPreferenceChangeListener(new OnPreferenceChangeListener() {

            @Override
            public boolean onPreferenceChange(Preference preference, Object newValue) {
                int mode = Integer.parseInt((String) newValue);
                int res[] = mVendorTelephonyManager.getSmsStorageOnSim(mode);
                if (res != null && res.length > 0) {
                    String getSmsStorageOnSimRes = "";
                    getSmsStorageOnSimRes += res[0];
                    for (int i = 1; i < res.length; i++) {
                        getSmsStorageOnSimRes += (", " + res[i]) ;
                    }
                    preference.setSummary(getSmsStorageOnSimRes);
                }
                return true;
            }
        });

        Preference setActivateVsim = getPreferenceScreen().findPreference("key_set_activate_vsim");
        setActivateVsim.setOnPreferenceClickListener(new OnPreferenceClickListener() {

            @Override
            public boolean onPreferenceClick(Preference preference) {
                boolean res = mVendorTelephonyManager.setActivateVsim(0, "9082051508109352832F", "460010210321339", "46001", 1, 2);
                Log.d(TAG, "setActivateVsim=" + res);
                if (res) preference.setSummary(res + "");
                return true;
            }
        });

        Preference setupDataCall = getPreferenceScreen().findPreference("key_setup_data_call");
        setupDataCall.setOnPreferenceClickListener(new OnPreferenceClickListener() {

            @Override
            public boolean onPreferenceClick(Preference preference) {
                boolean res = mVendorTelephonyManager.setupDataCall_1_6();
                Log.d(TAG, "setupDataCall=" + res);
                if (res) preference.setSummary(res + "");
                return true;
            }
        });

        mVendorTelephonyManager.registerTelephonyCallback(getMainExecutor(), mCallback);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        mVendorTelephonyManager.unregisterTelephonyCallback(mCallback);
    }

    String NrModeToString(int mode) {
        switch (mode) {
        case 0:
            return "None";
        case 1:
            return "NSA Only";
        case 2:
            return "SA Only";
        case 3:
            return "NSA SA";
        case 4:
            return "Reset Default";
        }
        return "Error";
    }

    private final VendorTelephonyCallbackImpl mCallback = new VendorTelephonyCallbackImpl();
    class VendorTelephonyCallbackImpl extends VendorTelephonyCallback
            implements VendorTelephonyCallback.IccidInfoUpdateListener,
            VendorTelephonyCallback.NrDualConnectivityStateChangedListener,
            VendorTelephonyCallback.SimTrayStateChangedListener,
            VendorTelephonyCallback.NasTimerChangedListener,
            VendorTelephonyCallback.UnthrottleApnListener {

        @Override
        public void onIccidInfoUpdated(String iccid) {
            Log.d(TAG, "onIccidInfoUpdated:" + iccid);
            Toast.makeText(getApplicationContext(), "onIccidInfoUpdated:" + iccid, Toast.LENGTH_LONG).show();
        }

        @Override
        public void onSimTrayStateChanged(int state) {
            Log.d(TAG, "onSimTrayStateChanged:" + state);
            Toast.makeText(getApplicationContext(), "onSimTrayStateChanged:" + state, Toast.LENGTH_LONG).show();
        }

        @Override
        public void onNrDualConnectivityStateChanged(boolean enabled, int cause) {
            Log.d(TAG, "onNrDualConnectivityStateChanged:" + enabled + "," + cause);
            Toast.makeText(getApplicationContext(), "onNrDualConnectivityStateChanged:" + enabled + "," + cause, Toast.LENGTH_LONG).show();
        }

        @Override
        public void onNasTimerChanged(int type, int status, int value, String apn) {
            Log.d(TAG, "onNasTimerChanged:" + type + ", " + status + ", " + value + ", " + apn);
            Toast.makeText(getApplicationContext(), "onNasTimerChanged: " + type + ", " + status + ", " + value + ", " + apn, Toast.LENGTH_LONG).show();
        }

        @Override
        public void onUnthrottleApn(DataProfile dataProfile) {
            Log.d(TAG, "onUnthrottleApn:" + dataProfile.toString());
            Toast.makeText(getApplicationContext(), "onUnthrottleApn", Toast.LENGTH_LONG).show();
        }
    }
}
