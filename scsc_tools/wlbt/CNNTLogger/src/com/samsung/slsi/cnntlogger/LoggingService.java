/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

package com.samsung.slsi.cnntlogger;

import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.AsyncTask;
import android.os.IBinder;
import android.os.SystemProperties;
import android.util.Log;
import android.support.v4.content.LocalBroadcastManager;

import java.util.Arrays;
import java.util.Comparator;
import java.io.File;

public class LoggingService extends Service {
    private final String TAG = "LoggingService";

    private LoggingManager mLoggingManager;

    private ConcurrentTask.ConcurrentTaskListener mConcurrentTask =
        new ConcurrentTask.ConcurrentTaskListener() {
            @Override
            public void taskCompleted(boolean result) {
            }
        };

    private boolean deleteDirectory(File directoryToBeDeleted) {
        File[] allContents = directoryToBeDeleted.listFiles();
        if (allContents != null) {
            for (File file : allContents) {
                deleteDirectory(file);
            }
        }
        return directoryToBeDeleted.delete();
    }

    private void handleBTStart(Context context, Intent intent) {
        CNNTUtils utils = new CNNTUtils(context);

        new AsyncTask<Void, Void, Void>(){
            @Override
            protected Void doInBackground(Void... params) {
                File dir = new File(CmdDefine.dirPath);
                if (dir == null)
                    return null;

                File[] files = dir.listFiles();
                if (files == null || (files != null && files.length < 4))
                    return null;

                Arrays.sort(files, Comparator.comparingLong(File::lastModified));
                for (int i = 0; i < files.length - 4; i++) {
                    if (files[i].isDirectory()) {
                        if (deleteDirectory(files[i]))
                            Log.d(TAG, files[i] + " was removed");
                    }
                }
                return null;
            }
        }.execute();

        if (!utils.getPreference(CmdDefine.KEY_BTN_STATUS, true)) {
            new ConcurrentTask(context, mConcurrentTask).execute(
                    CmdDefine.StopScriptBT, "", "", "");
        }

        String filterType = intent.getStringExtra("filterType");
        String filter = "";
        if (filterType.equals("custom")) {
            filter  = intent.getStringExtra("filter");
        }
        new ConcurrentTask(context, mConcurrentTask).execute(
                CmdDefine.StartScriptBT, utils.getCurrentTimeString(), filterType + filter, "");

        utils.setPreference(CmdDefine.KEY_BTN_STATUS, false);
        utils.setPreference(CmdDefine.KEY_CHECK_LOGCAT, false);
        utils.setPreference(CmdDefine.KEY_CHECK_MXLOG, true);
        utils.setPreference(CmdDefine.KEY_CHECK_UDILOG, true);
        utils.setPreference(CmdDefine.KEY_IS_WIFI_LOG, false);
        utils.setPreference(CmdDefine.KEY_BT_FILTER, filterType);

        mLoggingManager.updateLoggingNotification(true, filterType);
        sendLocalBroadcast(context, CmdDefine.UPDATE_LOGGING_OPTION);
    }

    private void handleBTStop(Context context, Intent intent) {
        CNNTUtils utils = new CNNTUtils(context);

        new ConcurrentTask(context, mConcurrentTask).execute(
                CmdDefine.StopScriptBT, utils.getCurrentTimeString(), "", "");
        utils.setPreference(CmdDefine.KEY_BTN_STATUS, true);

        mLoggingManager.updateLoggingNotification(false, "");
        sendLocalBroadcast(context, CmdDefine.UPDATE_LOGGING_OPTION);
    }

    private BroadcastReceiver mLoggingStartReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            Log.d(TAG, "onReceive : " + action);

            if (action == null) {
                return;
            }

            CNNTUtils utils = new CNNTUtils(context);
            String time = utils.getCurrentTimeString();

            switch (action) {
                case CmdDefine.WIFI_LOGGING_START_INTENT:
                    // logging started status
                    if (!utils.getPreference(CmdDefine.KEY_BTN_STATUS, true)) {
                        new ConcurrentTask(context, mConcurrentTask).execute(
                                CmdDefine.StopScript, "", "", "");
                    }

                    boolean isCheckedUdiLog = intent.getBooleanExtra("wifi_udilog", true);
                    boolean isCheckedMxLog = intent.getBooleanExtra("wifi_mxlog", true);

                    String logType = "";
                    if (isCheckedUdiLog) {
                        logType += "udilog ";
                    }

                    if (isCheckedMxLog) {
                        logType += "mxlog ";
                    }
                    new ConcurrentTask(context, mConcurrentTask).execute(
                            CmdDefine.StartScript, time, logType, "");

                    utils.setPreference(CmdDefine.KEY_BTN_STATUS, false);
                    utils.setPreference(CmdDefine.KEY_CHECK_LOGCAT, false);
                    utils.setPreference(CmdDefine.KEY_CHECK_MXLOG, isCheckedMxLog);
                    utils.setPreference(CmdDefine.KEY_CHECK_UDILOG, isCheckedUdiLog);
                    utils.setPreference(CmdDefine.KEY_IS_WIFI_LOG, true);
                    mLoggingManager.updateLoggingNotification(true, logType);
                    sendLocalBroadcast(context, CmdDefine.UPDATE_LOGGING_OPTION);
                    break;
                case CmdDefine.WIFI_LOGGING_STOP_INTENT:
                    new ConcurrentTask(context, mConcurrentTask).execute(
                            CmdDefine.StopScript, "", "", "");
                    utils.setPreference(CmdDefine.KEY_BTN_STATUS, true);
                    mLoggingManager.updateLoggingNotification(false, "");
                    sendLocalBroadcast(context, CmdDefine.UPDATE_LOGGING_OPTION);
                    break;
                case CmdDefine.BT_LOGGING_START_INTENT:
                    handleBTStart(context, intent);
                    break;
                case CmdDefine.BT_LOGGING_STOP_INTENT:
                    handleBTStop(context, intent);
                    break;
                case CmdDefine.COPY_LOG_INTENT:
                    String loggingPath = utils.getSystemLoggingPath();
                    String sdcardPath = utils.getSystemSdcardPath();
                    final File loggingDirectory = new File(loggingPath);
                    final File sdcardDirectory = new File(sdcardPath + "/wlbt/");
                    boolean loggingCopyResult = true;
                    if (loggingDirectory.exists()) {
                        loggingCopyResult = utils.copyFile(loggingDirectory, sdcardDirectory);
                    } else {
                        Log.d(TAG, loggingDirectory + " doesn't exist.");
                    }

                    final File sableLogDirectory = new File(CmdDefine.SABLE_LOG_DIR);
                    final File sableSdcardDirectory = new File(sdcardPath + "/wifi/");
                    boolean sableCopyResult = true;
                    if (sableLogDirectory.exists()) {
                        sableCopyResult = utils.copyFile(sableLogDirectory, sableSdcardDirectory);
                    } else {
                        Log.d(TAG, sableLogDirectory + " doesn't exist.");
                    }

                    utils.sendLoggingResultBroadcast(CmdDefine.COPY_LOG_RESULT_INTENT,
                        loggingCopyResult && sableCopyResult);
                    break;
                case CmdDefine.CLEAR_LOG_INTENT:
                    new ConcurrentTask(context, mConcurrentTask).execute(
                            CmdDefine.DeleteLog, "", "", "");
                    /* sendBroadcast clear result Code in CmdRunner.class */
                    break;
                default :
                    break;
            }
        }
    };

    private void sendLocalBroadcast(Context context, String intentName) {
        Intent intent = new Intent(intentName);
        LocalBroadcastManager.getInstance(context).sendBroadcast(intent);
    }

    @Override
    public void onCreate() {
        super.onCreate();
        mLoggingManager = new LoggingManager(getApplicationContext(), false);

        IntentFilter filter = new IntentFilter();
        filter.addAction(CmdDefine.WIFI_LOGGING_START_INTENT);
        filter.addAction(CmdDefine.WIFI_LOGGING_STOP_INTENT);
        filter.addAction(CmdDefine.BT_LOGGING_START_INTENT);
        filter.addAction(CmdDefine.BT_LOGGING_STOP_INTENT);
        filter.addAction(CmdDefine.COPY_LOG_INTENT);
        filter.addAction(CmdDefine.CLEAR_LOG_INTENT);
        registerReceiver(mLoggingStartReceiver, filter);
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        return super.onStartCommand(intent, flags, startId);
    }

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        unregisterReceiver(mLoggingStartReceiver);
    }
}
