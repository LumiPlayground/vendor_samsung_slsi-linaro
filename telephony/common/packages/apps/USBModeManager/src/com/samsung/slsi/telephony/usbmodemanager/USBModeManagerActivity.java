/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */
package com.samsung.slsi.telephony.usbmodemanager;

import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.os.PowerManager;
import android.os.SystemProperties;
import android.preference.CheckBoxPreference;
import android.preference.Preference;
import android.preference.PreferenceActivity;
import android.text.TextUtils;
import android.util.Log;
import android.view.WindowManager;

public class USBModeManagerActivity extends PreferenceActivity implements Preference.OnPreferenceChangeListener {

    private static final String TAG = "USBModeManagerActivity";
    private static final String PROPERTY_USB_CONFIGFS = "sys.usb.configfs";
    private static final String PROPERTY_PERSIST_USB_CONFIG = "persist.vendor.usb.usbradio.config";
    private static final String PROPERTY_USB_CONFIG_DM = "dm";
    private CheckBoxPreference mDebugModePref;

    @Override
    protected void onCreate(Bundle saveInstance) {
        super.onCreate(saveInstance);
        addPreferencesFromResource(R.layout.preference_main);

        boolean isDebugMode = TextUtils.equals(SystemProperties.get(PROPERTY_USB_CONFIGFS), "2");
        if (isDebugMode == false) {
            Log.d(TAG, "start USB Mode Switch");
            Intent intent = new Intent("intent.action.START_USB_MODE_SWITCH");
            intent.addCategory(Intent.CATEGORY_DEFAULT);
            startActivity(intent);
            finish();
        }

        mDebugModePref = (CheckBoxPreference) getPreferenceScreen().findPreference("key_usb_debug_mode");
        mDebugModePref.setOnPreferenceChangeListener(this);
    }

    @Override
    protected void onResume() {
        super.onResume();

        boolean enable = TextUtils.equals(SystemProperties.get(PROPERTY_PERSIST_USB_CONFIG), PROPERTY_USB_CONFIG_DM);
        mDebugModePref.setChecked(enable);
    }

    @Override
    public boolean onPreferenceChange(Preference preference, Object newValue) {
        Log.d(TAG, "onPreferenceChange() " + newValue.toString());
        if (preference == mDebugModePref) {
            boolean mode = (Boolean) newValue;
            String config = "";
            if (mode) {
                config = PROPERTY_USB_CONFIG_DM;
            }
            SystemProperties.set(PROPERTY_PERSIST_USB_CONFIG, config);
        }
        reboot();
        return true;
    }

    public void reboot() {
        final PowerManager pm = (PowerManager) getApplicationContext().getSystemService(Context.POWER_SERVICE);
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setMessage("Reboot the device").setPositiveButton("OK", new DialogInterface.OnClickListener() {

                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        pm.reboot(null);
                    }
                }).setNegativeButton("Cancel", null);
        final AlertDialog resetDialog = builder.create();
        resetDialog.getWindow().setType(
                WindowManager.LayoutParams.TYPE_SYSTEM_ALERT);
        resetDialog.show();
    }
}