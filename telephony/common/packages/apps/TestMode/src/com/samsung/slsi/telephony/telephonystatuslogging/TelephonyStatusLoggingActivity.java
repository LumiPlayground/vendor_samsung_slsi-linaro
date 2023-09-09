/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd All Rights Reserved
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

package com.samsung.slsi.telephony.telephonystatuslogging;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.os.AsyncResult;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.os.SystemProperties;
import android.util.Log;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.Switch;
import android.widget.Toast;

import java.io.IOException;
import java.lang.ref.WeakReference;
import java.util.Timer;
import java.util.TimerTask;

import com.samsung.slsi.telephony.oem.io.DataWriter;
import com.samsung.slsi.telephony.oem.OemRil;
import com.samsung.slsi.telephony.testmode.R;

import vendor.samsung_slsi.telephony.hardware.radioExternal.V1_0.RadioExternalError;

public class TelephonyStatusLoggingActivity extends Activity {
    private static final String TAG = "TelephonyStatusLoggingActivity";

    public static final String PROPERTY_TELEPHONY_STATUS_LOGGING_ONOFF = "persist.vendor.ril.telephonystatuslogging.onoff";
    public static final String PROPERTY_TELEPHONY_STATUS_LOGGING_DUMPALL = "persist.vendor.ril.telephonystatuslogging.dumpall";
    public static final String PROPERTY_TELEPHONY_STATUS_LOGGING_INTERVAL = "persist.vendor.ril.telephonystatuslogging.interval";

    public static final int EVENT_RIL_CONNECTED = 100;
    public static final int EVENT_RIL_DISCONNECTED = 101;
    public static final int EVENT_RIL_RESPONSE_TIMEOUT = 102;
    public static final int EVENT_SET_TELEPHONY_STATUS_LOGGING_DONE = 103;

    public static final int OEM_ERROR_SUCCESS = 0;
    public static final int OEM_ERROR_ALREADY_REQUESTED = 501;
    public static final int OEM_ERROR_UNSUPPORTED_BUILD_MODE = 502;
    public static final int OEM_ERROR_LOGGING_TURN_ON_FAILED = 503;
    public static final int OEM_ERROR_LOGGING_TURN_OFF_FAILED = 504;

    public static final int RILC_REQ_MISC_SET_TELEPHONY_STATUS_LOGGING = 989;

    public static final int OEMRIL_RESPONSE_TIMEOUT_MS = 5000;
    public static final int OEMRIL_DEFAULT_PHONE_ID = 0;

    public static final int SEND_TO_OEMRIL_ERROR_SUCCESS = 0;
    public static final int SEND_TO_OEMRIL_ERROR_START_TIMER_FAILED = 1;
    public static final int SEND_TO_OEMRIL_ERROR_OEMRIL_NULLPOINTER = 2;

    private Switch mSwitchOnOff;
    private CheckBox mCheckBoxDumpAll;
    private EditText mEditTextInterval;

    private AlertDialog.Builder mResetAlertBuilder;
    private AlertDialog.Builder mFinishAlertBuilder;
    private Timer mTimer;
    private OemRil mOemRil;
    private Handler mOemRilHandler = new OemRilHandler(this);

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.telephonystatuslogging_activity);

        mSwitchOnOff = (Switch)findViewById(R.id.switchOnOff);
        mCheckBoxDumpAll = (CheckBox)findViewById(R.id.checkboxDumpAll);
        mEditTextInterval = (EditText) findViewById(R.id.editTextInterval);
        mTimer = null;

        initAlertBuilders();
        connectToOemRilService();
    }

    @Override
    protected void onResume() {
        super.onResume();
        initUI();
    }

    @Override
    protected void onDestroy() {
        stopTimer();
        disconnectFromOemRilService();
        super.onDestroy();
    }

    private void initUI() {
        mSwitchOnOff.setOnCheckedChangeListener(null);
        String prop = String.valueOf(SystemProperties.get(PROPERTY_TELEPHONY_STATUS_LOGGING_ONOFF, "Off"));
        if (prop.equalsIgnoreCase("On")) {
            mSwitchOnOff.setChecked(true);
        } else {
            mSwitchOnOff.setChecked(false);
        }
        mSwitchOnOff.setOnCheckedChangeListener(new SwitchListener());

        prop = String.valueOf(SystemProperties.get(PROPERTY_TELEPHONY_STATUS_LOGGING_DUMPALL, "Off"));
        if (prop.equalsIgnoreCase("On")) {
            mCheckBoxDumpAll.setChecked(true);
        } else {
            mCheckBoxDumpAll.setChecked(false);
        }

        prop = String.valueOf(SystemProperties.get(PROPERTY_TELEPHONY_STATUS_LOGGING_INTERVAL, "5"));
        mEditTextInterval.setText(prop);

        if (mSwitchOnOff.isChecked()) {
            mCheckBoxDumpAll.setEnabled(false);
            mEditTextInterval.setEnabled(false);
        } else {
            mCheckBoxDumpAll.setEnabled(true);
            mEditTextInterval.setEnabled(true);
        }
    }

    private void initAlertBuilders() {
        mResetAlertBuilder = new AlertDialog.Builder(this);
        mResetAlertBuilder.setPositiveButton("Ok", new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog,int which) {
                initUI();
            }
        });
        mFinishAlertBuilder = new AlertDialog.Builder(this);
        mFinishAlertBuilder.setPositiveButton("Ok", new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog,int which) {
                finish();
            }
        });
    }

    public void makeToast(String text, int toastLength) {
        Toast.makeText(TelephonyStatusLoggingActivity.this, text, toastLength).show();
    }

    public void makeAlert(AlertDialog.Builder alertBuilder, String title, String content) {
        alertBuilder.setCancelable(false);
        alertBuilder.setTitle(title);
        alertBuilder.setMessage(content);
        alertBuilder.show();
    }

    private boolean startTimer(TimerTask task, long delay) {
        if (mTimer != null)
            return false;

        mTimer = new Timer(true);
        mTimer.schedule(task, delay);
        return true;
    }

    private boolean stopTimer() {
        if (mTimer == null)
            return false;

        mTimer.cancel();
        mTimer = null;
        return true;
    }

    private void connectToOemRilService() {
        mOemRil = OemRil.init(getApplicationContext(), OEMRIL_DEFAULT_PHONE_ID);
        if (mOemRil == null) {
            Log.e(TAG, "connectToOemRilService() mOemRil is null");
            return;
        }

        mOemRil.registerForOemRilConnected(mOemRilHandler, EVENT_RIL_CONNECTED);
        mOemRil.registerForOemRilDisconnected(mOemRilHandler, EVENT_RIL_DISCONNECTED);
    }

    private void disconnectFromOemRilService() {
        if (mOemRil == null) {
            Log.e(TAG, "disconnectFromOemRilService() mOemRil is null");
            return;
        }

        mOemRil.unregisterForOemRilConnected(mOemRilHandler);
        mOemRil.unregisterForOemRilDisconnected(mOemRilHandler);
        mOemRil.detach();
        mOemRil = null;
    }

    private int sendToOemRil(int onOff, int dumpAll, int interval) {
        if (!startTimer(new TimeoutTimerTask(), OEMRIL_RESPONSE_TIMEOUT_MS))
            return SEND_TO_OEMRIL_ERROR_START_TIMER_FAILED;
        if (mOemRil == null)
            return SEND_TO_OEMRIL_ERROR_OEMRIL_NULLPOINTER;

        Log.i(TAG, "sendToOemRil() onoff=" + onOff + ", dumpAll=" + dumpAll + ", interval=" + interval);
        DataWriter dr = new DataWriter();
        try {
            dr.writeInt(onOff);
            dr.writeInt(dumpAll);
            dr.writeInt(interval);
        } catch (IOException e) {
            Log.e(TAG, "sendToOemRil() IOException" + e);
        }

        mOemRil.invokeRequestRaw(RILC_REQ_MISC_SET_TELEPHONY_STATUS_LOGGING, dr.toByteArray(), mOemRilHandler.obtainMessage(EVENT_SET_TELEPHONY_STATUS_LOGGING_DONE));
        return SEND_TO_OEMRIL_ERROR_SUCCESS;
    }

    public class TimeoutTimerTask extends TimerTask {
        @Override
        public void run() {
            mOemRilHandler.sendEmptyMessageDelayed(EVENT_RIL_RESPONSE_TIMEOUT, 100);
            stopTimer();
        }
    }

    private class SwitchListener implements CompoundButton.OnCheckedChangeListener {
        @Override
        public void onCheckedChanged(CompoundButton compoundButton, boolean checked) {
            String intervalText = mEditTextInterval.getText().toString().trim();
            if (intervalText.equals("") || Integer.parseInt(intervalText) == 0) {
                makeToast("Please, set time interval first", Toast.LENGTH_SHORT);
                mSwitchOnOff.setChecked(!checked);
                return;
            }

            int onOff = checked ? 1 : 0;
            int dumpAll = mCheckBoxDumpAll.isChecked() ? 1 : 0;
            int interval = Integer.parseInt(intervalText);
            int sendError = sendToOemRil(onOff, dumpAll, interval);
            if (sendError == SEND_TO_OEMRIL_ERROR_START_TIMER_FAILED) {
                makeToast("Previous request didn't finish yet", Toast.LENGTH_SHORT);
                mSwitchOnOff.setChecked(!checked);
                return;
            } else if (sendError == SEND_TO_OEMRIL_ERROR_OEMRIL_NULLPOINTER) {
                makeAlert(mFinishAlertBuilder, "OEMRIL DISCONNECTED", "mOemRil is null");
                mSwitchOnOff.setChecked(!checked);
                return;
            }

            if (checked) {
                mCheckBoxDumpAll.setEnabled(false);
                mEditTextInterval.setEnabled(false);
            } else {
                mCheckBoxDumpAll.setEnabled(true);
                mEditTextInterval.setEnabled(true);
            }
        }
    }

    private static class OemRilHandler extends Handler {
        private final WeakReference<TelephonyStatusLoggingActivity> mActivityReference;

        public OemRilHandler(TelephonyStatusLoggingActivity activity) {
            mActivityReference = new WeakReference<TelephonyStatusLoggingActivity>(activity);
        }

        @Override
        public void handleMessage(Message msg) {
            TelephonyStatusLoggingActivity activity = mActivityReference.get();
            if (activity == null)
                return;

            switch (msg.what) {
                case EVENT_RIL_CONNECTED:
                    break;
                case EVENT_RIL_DISCONNECTED:
                    activity.makeAlert(activity.mFinishAlertBuilder, "OEMRIL DISCONNECTED", "OemRil is disconnected");
                    break;
                case EVENT_RIL_RESPONSE_TIMEOUT:
                    activity.makeAlert(activity.mResetAlertBuilder, "OEMRIL RESPONSE TIMEOUT", "Timeout(" + OEMRIL_RESPONSE_TIMEOUT_MS + "ms) occurs after request");
                    break;
                case EVENT_SET_TELEPHONY_STATUS_LOGGING_DONE:
                    if (!activity.stopTimer()) {
                        Log.e(TAG, "Unexcepted operation: mTimer is null!!");
                        activity.makeAlert(activity.mFinishAlertBuilder, "UNEXPECTED OPERATION", "Timer is null after event done");
                        return;
                    }
                    handleOemRilResponse(activity, msg.arg2, getIntOemError((AsyncResult)msg.obj));
                    break;
                default:
                    // received undefined message
                    break;
            }
        }

        private void handleOemRilResponse(TelephonyStatusLoggingActivity activity, int rilError, int oemError) {
            Log.i(TAG, "handleOemRilResponse() rilError=" + rilError + ", oemError=" + oemError);
            if (rilError != RadioExternalError.RADIO_EXTERNAL_NONE) {
                activity.makeAlert(activity.mFinishAlertBuilder, "RIL ERROR", "ril error(" + rilError + ") occurred");
                return;
            }

            if (oemError == OEM_ERROR_SUCCESS) {
                activity.makeToast((activity.mSwitchOnOff.isChecked() ? "Start" : "Stop") + " logging", Toast.LENGTH_SHORT);
            } else if (oemError == OEM_ERROR_ALREADY_REQUESTED) {
                activity.makeAlert(activity.mResetAlertBuilder, "DUPLICATED REQUEST", "Logging is already " + (activity.mSwitchOnOff.isChecked() ? "On" : "Off"));
            } else if (oemError == OEM_ERROR_UNSUPPORTED_BUILD_MODE) {
                activity.makeAlert(activity.mResetAlertBuilder, "UNSUPPORTED BUILD MODE", "Current build mode is unsupported");
            } else if (oemError == OEM_ERROR_LOGGING_TURN_ON_FAILED) {
                activity.makeAlert(activity.mFinishAlertBuilder, "TURN ON FAILURE", "Pipe open might be failed, please check the log");
            } else if (oemError == OEM_ERROR_LOGGING_TURN_OFF_FAILED) {
                activity.makeAlert(activity.mFinishAlertBuilder, "TURN OFF FAILURE", "Pipe notification might be failed, please check the log");
            } else {
                activity.makeAlert(activity.mFinishAlertBuilder, "UNDEFINED ERROR", "Please check ap log");
            }
        }

        private int getIntOemError(AsyncResult ar) {
            if (ar == null || ar.result == null)
                return -1;

            byte[] errorBytes = (byte[])(ar.result);
            int errorInt = 0;

            for (int i = 0; i < Integer.BYTES; i++)
                errorInt += (errorBytes[i] & 0xFF) << (i * 8);
            return errorInt;
        }
    }
}
