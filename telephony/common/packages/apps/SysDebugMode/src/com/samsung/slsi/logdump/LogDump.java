package com.samsung.slsi.logdump;

import java.io.File;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;

import android.os.AsyncResult;
import android.os.Message;
import android.util.Log;

public class LogDump {
    private static final String TAG = "LogDump";

    public void dump(String directory, Message result) {
        final String baseDir = directory;
        final Message onComplete = result;
        Thread job = new Thread(new Runnable() {
            @Override
            public void run() {

                ArrayList<String> list = new ArrayList<>();
                File dir = new File(baseDir);
                try {
                    if (dir.exists() && dir.canWrite()) {
                        SimpleDateFormat dateFormat = new SimpleDateFormat("yyyyMMddHHmmss");
                        String currTime = dateFormat.format(new Date()).toString();
                        list.add("" + baseDir + "/dump_radio_" + currTime + ".log");
                        list.add("" + baseDir + "/dump_main_" + currTime + ".log");
                        list.add("" + baseDir + "/dump_system_" + currTime + ".log");
                        list.add("" + baseDir + "/dump_kernel_" + currTime + ".log");

                        ArrayList<String> cmds = new ArrayList<>();
                        cmds.add("logcat -b radio -d -f ");
                        cmds.add("logcat -b main -d -f ");
                        cmds.add("logcat -b system -d -f ");
                        cmds.add("logcat -b kernel -d -f ");

                        for (int i = 0; i < list.size(); i++) {
                            runShellCMD(cmds.get(i) + list.get(i));
                        }
                    }
                    else {
                        Log.i(TAG, "The log directory " + directory + " cannot be accessible.");
                    }
                }
                catch (SecurityException e) {
                    Log.w(TAG, "The log directory " + directory + " cannot be accessible.", e);
                }

                if (onComplete != null) {
                    AsyncResult.forMessage(onComplete, list, null);
                    onComplete.sendToTarget();
                }
            }
        });
        job.start();
    }

    public static void runShellCMD(String cmd) {
        String[] shellCmd = {"/system/bin/sh", "-c", cmd};
        Log.i(TAG, "Execute shell command >> " + cmd);

        try {
            Process proc = Runtime.getRuntime().exec(shellCmd);
            proc.waitFor();
        } catch (IOException e) {
            Log.i(TAG, "runShellCMD() IOException" + e);
        } catch (SecurityException e) {
            Log.i(TAG, "runShellCMD() SecurityException" + e);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }
}
