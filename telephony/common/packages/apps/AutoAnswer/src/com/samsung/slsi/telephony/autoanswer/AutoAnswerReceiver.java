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

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.util.Log;
import android.widget.Toast;

import androidx.preference.PreferenceManager;

public class AutoAnswerReceiver extends BroadcastReceiver {

    private static final String TAG = "AutoAnswerReceiver";

    @Override
    public void onReceive(Context context, Intent intent) {
        Log.d(TAG, intent.getAction());
        if (intent != null && "android.provider.Telephony.SECRET_CODE".equals(intent.getAction())) {
            Intent i = new Intent(Intent.ACTION_MAIN);
            i.setClass(context, AutoAnswerActivity.class);
            i.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
            context.startActivity(i);
        } else if (intent != null && "android.intent.action.BOOT_COMPLETED".equals(intent.getAction())) {
            SharedPreferences sharedPref = PreferenceManager.getDefaultSharedPreferences(context);
            Log.d(TAG, "shared: " +sharedPref.getBoolean(Constants.KEY_AUTO_MODE, false));
            if (sharedPref.getBoolean(Constants.KEY_AUTO_MODE, false)) {
                Toast.makeText(context, "Auto Answer: ON", Toast.LENGTH_LONG).show();
                context.startService(new Intent(context, CallService.class));
            }
        }
    }
}