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

import java.io.File;
import java.io.IOException;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.nio.ByteBuffer;
import java.nio.charset.StandardCharsets;
import java.util.Arrays;

import android.bluetooth.BluetoothAdapter;
import android.content.Context;
import android.net.LocalSocket;
import android.net.LocalSocketAddress;
import android.os.Environment;
import android.util.Log;

import org.json.JSONObject;

class CmdRunner {
    private final String TAG = "CmdRunner";

    private CNNTUtils mUtils;

    private LoggingJSONValue mLoggingJSONValue;

    private LocalSocket mWlandSocket;

    private boolean mIsLoggingSuccess = true;

    private static Process mProcess;

    boolean startScript(Context context, String scriptName, String time, String logType, String dirName) {
        Log.d(TAG, "startScript " + scriptName + ", dirname : " + dirName);

        mUtils = new CNNTUtils(context);
        mLoggingJSONValue = new LoggingJSONValue();
        mIsLoggingSuccess = true;
        if (scriptName.equals(CmdDefine.StartScript)) {
            startLogging(time, logType, dirName);
            mUtils.sendWifiLoggingResult(mIsLoggingSuccess);
        } else if (scriptName.equals(CmdDefine.StopScript)) {
            killLogcatProcess();

            JSONObject json = mLoggingJSONValue.getJSonValue(CmdDefine.WIFI_LOG_ALL_STOP, "", null);
            execShellCmd(json);
            mUtils.sendWifiLoggingResult(mIsLoggingSuccess);
        } else if (scriptName.equals(CmdDefine.StartScriptBT)) {
            BluetoothAdapter adapter = BluetoothAdapter.getDefaultAdapter();
            try {
                Method method = adapter.getClass().getMethod("cmdDBFW",
                        int.class, int.class, int.class, int.class);
                if (CmdDefine.btAudioFilter.equals(
                    mUtils.getPreference(CmdDefine.KEY_BT_FILTER, CmdDefine.btAudioFilter))) {
                    method.invoke(adapter,
                        CmdDefine.VSC_OPCODE_DBFW,
                        CmdDefine.DBFW_SCO_DUMP,
                        CmdDefine.SCO_PCM_TX_RX_DUMP,
                        65535);
                }
                method.invoke(adapter, CmdDefine.VSC_OPCODE_LINK_LAYER, 0, getLinkLayerMode(), 0);
            } catch (NoSuchMethodException | IllegalAccessException | InvocationTargetException e) {
                Log.e(TAG, "can not find method : " + e.toString());
            }
            startLogging(time, logType, dirName);
            mUtils.sendBtLoggingResult(mIsLoggingSuccess);
        } else if (scriptName.equals(CmdDefine.StopScriptBT)) {
            BluetoothAdapter adapter = BluetoothAdapter.getDefaultAdapter();
            try {
                Method method = adapter.getClass().getMethod("cmdDBFW",
                        int.class, int.class, int.class, int.class);
                if (CmdDefine.btAudioFilter.equals(
                    mUtils.getPreference(CmdDefine.KEY_BT_FILTER, CmdDefine.btAudioFilter))) {
                    method.invoke(adapter,
                        CmdDefine.VSC_OPCODE_DBFW,
                        CmdDefine.DBFW_SCO_DUMP,
                        CmdDefine.SCO_DUMP_DISABLE,
                        0);
                }
                method.invoke(adapter, CmdDefine.VSC_OPCODE_LINK_LAYER, 0, 0, 0);
            } catch (NoSuchMethodException | IllegalAccessException | InvocationTargetException e) {
                Log.e(TAG, "can not find method : " + e.toString());
            }
            killLogcatProcess();

            JSONObject json = mLoggingJSONValue.getJSonValue(CmdDefine.BT_LOG_STOP, "", null);
            execShellCmd(json);
            mUtils.sendBtLoggingResult(mIsLoggingSuccess);
        } else if (scriptName.equals(CmdDefine.wifiHip4)) {
            createDirectory(CmdDefine.dirPath + CmdDefine.mHip4Path);
            JSONObject json = mLoggingJSONValue.getJSonValue(CmdDefine.WIFI_LOG_GET_HIP4, dirName, null);
            execShellCmd(json);

            //Copy to external storage code
            /*if (mIsLoggingSuccess) {
                File hip4 = new File(CmdDefine.dirPath + CmdDefine.mHip4Path);
                final File internalDirectory =
                    new File(Environment.getExternalStorageDirectory().toString() + "/" + CmdDefine.mHip4Path);
                boolean loggingCopyResult = mUtils.copyFile(hip4, internalDirectory);
            }*/
        } else { //CmdDefine.DeleteLog
            boolean isSuccess = deleteAllLogs();
            mUtils.sendLoggingResultBroadcast(CmdDefine.CLEAR_LOG_RESULT_INTENT, isSuccess);
        }

        Log.d(CmdDefine.LOGTAG, "mIsLoggingSuccess " + mIsLoggingSuccess);
        return mIsLoggingSuccess;
    }

    private int getLinkLayerMode() {
        String[] modes = mUtils.getStringArr(R.array.bt_ll_modes);
        int mode = 0;

        if (mUtils.getPreference(CmdDefine.KEY_BT_USE_DEFAULT_MODE, true)) {
            mode = 0x0011;
        } else {
            for (int i = 0; i < modes.length; i++) {
                if (mUtils.getPreference(modes[i], false)) {
                    if (i == 0) {
                        mode = 1;
                    } else {
                        mode += 1 << i;
                    }
                }
            }
        }

        Log.d(TAG, "link layer mode=" + mode);
        return mode;
    }

    private void killLogcatProcess() {
        if (mProcess != null) {
            Log.d(TAG, "process information " + mProcess.toString());

            if (mProcess.isAlive()) {
                mProcess.destroy();
                mProcess = null;
                Log.d(TAG, "kill process ");
            }
        }
    }

    private void cmdExecute(String cmd) throws RuntimeException {
        Runtime runtime = Runtime.getRuntime();
        try {
            mProcess = runtime.exec(cmd);

            Log.d(TAG, "pid : " + mProcess.toString());
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private void multiCmdExecute(String[] cmd) throws RuntimeException {
        Runtime runtime = Runtime.getRuntime();
        try {
            mProcess = runtime.exec(cmd);
            mProcess.waitFor();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private boolean isValidFile(String filePath) {
        if (filePath == null) return false;

        File file = new File(filePath);
        return file.exists();
    }

    private void doStartLogging(String type, String time, String dirName) {
        Log.d(CmdDefine.LOGTAG, type + " logging start >>>");

        String buffer;
        String logdir;
        String filePath;
        JSONObject jsonObject;

        if (dirName.equals("")) {
            logdir = mUtils.getSystemLoggingPath() + "Log_" + time + "/";
        } else {
            logdir = mUtils.getSystemLoggingPath() + dirName + "_" + time + "/";
        }

        if (type.equals(CmdDefine.TYPE_AP)) {
            createDirectory(mUtils.getSystemLoggingPath());
            createDirectory(logdir);

            buffer = checkLoggingOption("AP");
            String shellCmd = CmdDefine.lcLog + buffer + " -f " + logdir + type + "_" + time + ".log" + " -r 51200";
            cmdExecute(shellCmd);
            Log.d(CmdDefine.LOGTAG, "[AP] shell CMD >>> " + shellCmd);
            return;
        } else if (type.equals(CmdDefine.TYPE_MX)) {
            filePath = logdir + "mxlog_" + time;
            jsonObject = mLoggingJSONValue.getJSonValue(CmdDefine.WIFI_LOG_MXLOG_START, filePath, null);
        } else if (type.equals(CmdDefine.TYPE_UDI)) {
            filePath = logdir + "udi_decode_" + time;
            jsonObject = mLoggingJSONValue.getJSonValue(CmdDefine.WIFI_LOG_UDILOG_START, filePath, null);
        } else {
            Log.e(CmdDefine.LOGTAG, "Not supported logtype: " + type);
            mIsLoggingSuccess = false;
            return;
        }

        execShellCmd(jsonObject);
    }

    private void startBTLogging(String cmd, String type, String time, String dirName) {
        Log.d(CmdDefine.LOGTAG, type + " startBTLogging " + cmd);

        String logdir;
        String filePath;
        JSONObject jsonObject;

        if (dirName.equals("")) {
            logdir = mUtils.getSystemLoggingPath() + "Log_" + time;
        } else {
            logdir = mUtils.getSystemLoggingPath() + dirName + "_" + time;
        }

        if (type.equals(CmdDefine.btNormalFilter)) {
            filePath = logdir + "/bt_general";
            jsonObject = mLoggingJSONValue.getJSonValue(CmdDefine.BT_LOG_NORMAL, filePath, null);
        } else if (type.equals(CmdDefine.btAudioFilter)) {
            filePath = logdir + "/bt_audio";
            jsonObject = mLoggingJSONValue.getJSonValue(CmdDefine.BT_LOG_AUDIO, filePath, null);
        } else if (type.startsWith(CmdDefine.btCustomFilter)) {
            String customFilter = type.substring(CmdDefine.btCustomFilter.length());
            mUtils.setPreference(CmdDefine.KEY_BT_FILTER_PREV_VAL, customFilter);
            filePath = logdir + "/bt_custom";
            jsonObject = mLoggingJSONValue.getJSonValue(CmdDefine.BT_LOG_CUSTOM, filePath, customFilter);
        } else {
            Log.e(CmdDefine.LOGTAG, "Not supported logtype: " + type);
            mIsLoggingSuccess = false;
            return;
        }

        execShellCmd(jsonObject);
    }

    private void startLogging(String timestamp, String logtype, String dirName) {
        String[] logs = logtype.split(" ");

        for (String type : logs) {
            if (isBtLoggingFilter(type)) {
                startBTLogging(CmdDefine.StartScriptBT, type, timestamp, dirName);
            } else {
                doStartLogging(type, timestamp, dirName);
            }
        }
    }

    private boolean isBtLoggingFilter(String filter) {
        return CmdDefine.btNormalFilter.equals(filter)
                || CmdDefine.btAudioFilter.equals(filter)
                || filter.startsWith(CmdDefine.btCustomFilter);
    }

    private void execShellCmd(JSONObject jsonObject) {
        if (jsonObject == null) {
            Log.e(CmdDefine.LOGTAG, "jsonObject is null");
            mIsLoggingSuccess = false;
            return;
        }

        try {
            mWlandSocket = new LocalSocket();
            mWlandSocket.connect(new LocalSocketAddress(CmdDefine.SOCKET_NAME, LocalSocketAddress.Namespace.ABSTRACT));

            Log.d(CmdDefine.LOGTAG, "<<< jsonObject: " + jsonObject);
            byte[] bytes = ByteBuffer.allocate(4).putInt(jsonObject.toString().length()).array();
            mWlandSocket.getOutputStream().write(bytes);
            mWlandSocket.getOutputStream().write(jsonObject.toString().getBytes());

            String execKey = (String) jsonObject.get(LoggingJSONValue.EXEC_KEY);
            boolean loggingStarted = CmdDefine.StartScriptBT.equals(execKey)
                    || CmdDefine.wifiHip4.equals(execKey);
            if (loggingStarted) {
                byte[] recvBuf = new byte[10];
                int byteRead = mWlandSocket.getInputStream().read(recvBuf);
                byte[] response = new byte[byteRead];
                System.arraycopy(recvBuf, 0, response, 0, byteRead);
                String pid = new String(response, StandardCharsets.US_ASCII);

                Log.d(CmdDefine.LOGTAG, ">>> pid: " + pid);
                if (Integer.parseInt(pid) == -1) {
                    mIsLoggingSuccess = false;
                }
            }

            Thread.sleep(100);
            mWlandSocket.close();

            if (loggingStarted) {
                String filePath = (String) jsonObject.get(LoggingJSONValue.DIR_KEY);
                String option = (String) jsonObject.get(LoggingJSONValue.OPTION_KEY);
                Log.d(TAG, "filePath : " + filePath);

                if (CmdDefine.wifiCommand.equals(jsonObject.get(LoggingJSONValue.NAME_KEY))
                        && !CmdDefine.wifiHip4.equals(option)) {
                    if (CmdDefine.TYPE_MX.equals(option)) {
                        filePath = filePath + "_0.log";
                    } else if (CmdDefine.TYPE_UDI.equals(option)) {
                        filePath = filePath + ".log.00";
                    } else { // wifiLog all - check mx log
                        filePath = filePath + "_0.log";
                    }
                } else if (CmdDefine.btCommand.equals(jsonObject.get(LoggingJSONValue.NAME_KEY))) {
                    filePath = filePath + "_mxlog_0.log";
                }

                if (filePath != null && !isValidFile(filePath)) {
                    mIsLoggingSuccess = false;
                }
            } else {
                // if total log file size is over 2GB, remove old directory.
                if (!mUtils.mIsSystemPartition) {
                    deleteOldDirectory(CmdDefine.dirPath);
                }
            }
        } catch (IOException ex) {
            Log.e(CmdDefine.LOGTAG, "'" + CmdDefine.SOCKET_NAME + "' socket closed", ex);
            mIsLoggingSuccess = false;
        } catch (InterruptedException ie) {
            Log.e(CmdDefine.LOGTAG, ie.getLocalizedMessage());
            mIsLoggingSuccess = false;
        } catch (Throwable tr) {
            Log.e(CmdDefine.LOGTAG, "Uncaught exception", tr);
            mIsLoggingSuccess = false;
        }
    }

    private void createDirectory(String path) {
        File directory = new File(path);
        if (!directory.exists()) {
            Log.d(TAG, path + " dir is created");
            directory.mkdirs();
            directory.setReadable(true, false);
            directory.setWritable(true, false);
            directory.setExecutable(true, false);
        } else {
            Log.d(TAG, path + " is already created");
        }
    }

    private void deleteOldDirectory(String path) {
        File rootDir = new File(path);
        if (rootDir == null) {
            Log.e(TAG, path + " dir is does't exist");
            return;
        }

        File[] logDir = rootDir.listFiles();
        long[] folderSize;
        long totalStorage = 0; // KB
        int i = 0;

        if (logDir != null && logDir.length > 0) {
            folderSize = new long[logDir.length];
        } else {
            Log.e(TAG, "Log Folder doesn't exist");
            return;
        }

        Arrays.sort(logDir);

        for (i = 0; i < logDir.length; i++) {
            if (logDir[i].isDirectory()) {
                File[] logFiles = logDir[i].listFiles();
                if (logFiles == null || (logFiles != null && logFiles.length == 0)) {
                    continue;
                }

                for (int j = 0; j < logFiles.length; j++) {
                    folderSize[i] += (logFiles[j].length() / 1024); // byte / 1024 = KB
                }
            } else {
                folderSize[i] += (logDir[i].length() / 1024);
            }
            totalStorage += folderSize[i];
        }

        i = 0;
        //2GB = 2097152KB
        while (totalStorage > CmdDefine.STORAGE_SIZE_2GB) { // KB
            if (i >= logDir.length) {
                break;
            }
            if (logDir[i].isDirectory()) {
                mUtils.removeDirectory(logDir[i].toString(), true);
            } else {
                mUtils.removeFile(logDir[i].toString());
            }

            totalStorage = totalStorage - folderSize[i];
            Log.d(TAG, "DELETE : " + logDir[i].toString()
                  + ", remain storage : " + totalStorage + "KB");
            i++;
        }
    }

    private String checkLoggingOption(String type) {
        String buffer = "";

        if (type.equalsIgnoreCase("AP")) {
            String[] apList;
            apList = mUtils.getStringArr(R.array.ap_logtype); //c.getResources().getStringArray(R.array.ap_logtype);

            for (int i = 0; i < apList.length; i++) {
                if (mUtils.getPreference(apList[i], true)) {
                    buffer = buffer + " -b " + apList[i];
                    if (i == 0) break;
                }
            }
        } else if (type.equalsIgnoreCase("TCP")) {
            buffer = mUtils.getSettingPreference("key_tcp_logging_type", "any");
            Log.i(TAG, "prefSettings tcp type : " + buffer);
        }

        return buffer;
    }

    private boolean deleteAllLogs() {
        boolean isSuccess = false;
        File wlbtDir = new File(mUtils.getSystemLoggingPath());
        String[] shellCmd = { "/bin/sh", "-c", "" };

        if (mUtils.mIsSystemPartition) {
            shellCmd[2] = "rm -rf " + mUtils.getSystemLoggingPath() + "Log_*";
        } else {
            shellCmd[2] = "rm -rf " + mUtils.getSystemLoggingPath() + "* " + CmdDefine.SABLE_LOG_DIR + "*";
        }

        multiCmdExecute(shellCmd);

        File[] wlbtList = wlbtDir.listFiles();
        if (mUtils.mIsSystemPartition) {
            if (wlbtList == null || (wlbtList != null && wlbtList.length == 0)) {
                isSuccess = true;
            } else {
                for (int i = 0; i < wlbtList.length; i++) {
                    Log.d(TAG, "file name : " + wlbtList[i].getName());
                    if (wlbtList[i].getName().startsWith("Log_")) {
                        isSuccess = false;
                        break;
                    }
                    isSuccess = true;
                }
            }
        } else {
            File sableDir = new File(CmdDefine.SABLE_LOG_DIR);
            File[] sableList = sableDir.listFiles();
            if ((wlbtList == null || (wlbtList != null && wlbtList.length == 0))
                 && (sableList == null || (sableList != null && sableList.length == 0))) {
                isSuccess = true;
            }
        }

        mUtils.sendLoggingResultBroadcast(CmdDefine.CLEAR_LOG_RESULT_INTENT, isSuccess);
        killLogcatProcess();
        return isSuccess;
    }
}
