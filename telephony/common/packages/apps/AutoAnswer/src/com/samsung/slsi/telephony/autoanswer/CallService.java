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

import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.app.Service;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.IBinder;
import android.telephony.PhoneStateListener;
import android.telecom.TelecomManager;
import android.telephony.TelephonyManager;
import android.util.Log;

import androidx.core.app.NotificationCompat;
import androidx.preference.PreferenceManager;

public class CallService extends Service {

    private static final String TAG = "AutoAnswer.CallService";
    private static final String CHANNEL_ID = "channel_call_service";
    private static final int NOTIFICATION_ID = 100;
    private SharedPreferences mSharedPref;
    private TelephonyManager mTelephonyManager;
    private TelecomManager mTelecomManager;
    private int mTime = 1;
    private boolean mIsChecked = false;
    private PhoneStateListener mPhoneStateListener = new PhoneStateListener() {

        @Override
        public void onCallStateChanged(int state, String incomingNumber) {
            super.onCallStateChanged(state, incomingNumber);
            Log.d(TAG, "onCallStateChanged() state=" + state);
            if (state == TelephonyManager.CALL_STATE_RINGING) {
                Log.d(TAG, "CallService CALL_STATE_RINGING: " + incomingNumber);
                if (mIsChecked) AcceptCall();
            }
        }
    };

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        Log.d(TAG, "onStartCommand()");

        // Foreground Service
        initNotification();

        mTelephonyManager = this.getSystemService(TelephonyManager.class);
        mTelephonyManager.listen(mPhoneStateListener, PhoneStateListener.LISTEN_CALL_STATE);
        mTelecomManager = this.getSystemService(TelecomManager.class);
        mSharedPref = PreferenceManager.getDefaultSharedPreferences(getApplicationContext());
        mIsChecked = mSharedPref.getBoolean(Constants.KEY_AUTO_MODE, false);

        return super.onStartCommand(intent, flags, startId);
    }

    @Override
    public IBinder onBind(Intent arg0) {
        // TODO Auto-generated method stub
        return null;
    }

    @Override
    public void onDestroy() {
        Log.d(TAG, "onDestroy()");
        mTelephonyManager.listen(mPhoneStateListener, PhoneStateListener.LISTEN_NONE);
        stopForeground(true);
        super.onDestroy();
    }

    public void AcceptCall() {
        Log.d(TAG, "AcceptCall()");
        mTime = Integer.parseInt(mSharedPref.getString(Constants.KEY_ANSWER_TIME, "1"));
        new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    Thread.sleep(mTime * 1000);
                } catch (InterruptedException e) {
                    // TODO Auto-generated catch block
                    e.printStackTrace();
                }
                Log.d(TAG, "Accept in thread!!");
                mTelecomManager.acceptRingingCall();
            }
        }).start();
    }


    private void initNotification() {
        Intent notificationIntent = new Intent(this, AutoAnswerActivity.class);
        PendingIntent pendingIntent = PendingIntent.getActivity(this, 0, notificationIntent, PendingIntent.FLAG_IMMUTABLE);

        NotificationChannel notificationChannel = new NotificationChannel(
                CHANNEL_ID,
                "Auto Answer",
                NotificationManager.IMPORTANCE_LOW
        );

        NotificationManager nm = this.getSystemService(NotificationManager.class);
        nm.createNotificationChannel(notificationChannel);

        NotificationCompat.Builder notification = new NotificationCompat.Builder(getApplicationContext(), CHANNEL_ID)
                .setContentTitle("Auto Answer")
                .setContentIntent(pendingIntent)
                .setContentText("Auto Answer is running")
                .setSmallIcon(R.drawable.ic_launcher_foreground);
        nm.notify(NOTIFICATION_ID, notification.build());
        startForeground(NOTIFICATION_ID, notification.build());
    }
}
