package com.samsung.slsi.logdump;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.lang.ref.WeakReference;
import java.nio.file.Files;
import java.nio.file.attribute.PosixFilePermission;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import java.util.zip.ZipEntry;
import java.util.zip.ZipOutputStream;

import android.app.Service;
import android.content.Intent;
import android.os.AsyncResult;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.util.Log;

public class LogDumpService extends Service {

    private static final String TAG = "LogDumpService";
    private boolean mRunning;
    private MyHandler mHandler;
    private static final int EVENT_LOG_DUMP_DONE = 0;
    private String mBaseDirectory;

    ZipArchiveThread.OnZipArchiveListener mCallback;

    private static class MyHandler extends Handler {
        private final WeakReference<LogDumpService> mService;

        MyHandler(LogDumpService service) {
            mService = new WeakReference<LogDumpService>(service);
        }

        @Override
        public void handleMessage(Message msg) {
            LogDumpService service = mService.get();
            if (service != null) {
                service.handleMessage(msg);
            }
        }
    }

    public static class ZipArchiveThread extends Thread {
        public static final String TAG = "LogDumpService.ZipArchiveThread";
        private String archiveName;
        private List<String> fileList;
        private OnZipArchiveListener listener;

        ZipArchiveThread(String archiveName, List<String> fileList, OnZipArchiveListener listener) {
            this.fileList = new ArrayList<String>(fileList);
            this.archiveName = archiveName;
            this.listener = listener;
        }

        @Override
        public void run() {
            Log.d(TAG, "ZipArchiveThread start");

            if (fileList == null || fileList.isEmpty()) {
                Log.d(TAG, "No log files.");
                return;
            }

            if (listener != null) {
                listener.onZipArchiveStarted();
            }

            String targetPath = archiveName;
            ZipOutputStream zos = null;
            try {
                zos = new ZipOutputStream(new FileOutputStream(new File(targetPath)));
                Log.d(TAG, "target name:" + targetPath);
                for (String file : fileList) {
                    String entryName = new File(file).getName();
                    ZipEntry entry = new ZipEntry(entryName);
                    zos.putNextEntry(entry);
                    Log.d(TAG, "add ZipEntery: " + entryName);

                    String sourcePath = file;
                    FileInputStream fis = null;
                    try {
                        fis = new FileInputStream(sourcePath);
                        Log.d(TAG, "Open FileInputStream: " + sourcePath);
                        byte[] buffer = new byte[4096];
                        int ret = 0;
                        while ((ret = fis.read(buffer)) > 0) {
                            zos.write(buffer, 0, ret);
                        } // end while ~
                    } catch (IOException e) {
                        Log.e(TAG, "", e);
                    } finally {
                        try {
                            if (fis != null)
                                fis.close();
                        } catch (IOException e) {
                        }
                    }
                    zos.closeEntry();
                } // end for ~
                zos.close();

                File targetFile = new File(targetPath);
                Set<PosixFilePermission> perms = new HashSet<PosixFilePermission>();
                perms.add(PosixFilePermission.OWNER_READ);
                perms.add(PosixFilePermission.OWNER_WRITE);
                perms.add(PosixFilePermission.GROUP_READ);
                perms.add(PosixFilePermission.GROUP_WRITE);
                perms.add(PosixFilePermission.OTHERS_READ);
                Files.setPosixFilePermissions(targetFile.toPath(), perms);

            } catch (FileNotFoundException e) {
                Log.e(TAG, "", e);
            } catch (IOException e) {
                Log.e(TAG, "", e);
            } finally {
                try {
                    if (zos != null)
                        zos.close();
                } catch (IOException e) {
                }
            }
            Log.d(TAG, "ZipArchiveThread finished");

            if (listener != null) {
                listener.onZipArchiveComplete();
            }
        }

        public interface OnZipArchiveListener {
            public void onZipArchiveStarted();
            public void onZipArchiveComplete();
        }
    }


    public LogDumpService() {
        mHandler = new MyHandler(this);
    }

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        if (!mRunning) {
            mRunning = true;

            mBaseDirectory= "/data/vendor/log/cbd";
            Message msg = mHandler.obtainMessage(EVENT_LOG_DUMP_DONE);
            new LogDump().dump(mBaseDirectory, msg);
            Log.d(TAG, "start dump AP logs");
        }
        else {
            stopSelf();
        }
        return START_NOT_STICKY;
    }

    public void handleMessage(Message msg) {
        switch (msg.what) {
        case EVENT_LOG_DUMP_DONE:
            mRunning = false;
            AsyncResult ar = (AsyncResult) msg.obj;
            if (ar.exception != null ||
                !doZipArchive((List<String>)ar.result)) {
                Log.d(TAG, "dump AP logs done but errors.");
                stopSelf();
            }
            break;
        }
    }

    private boolean doZipArchive(List<String> fileList) {
        if (fileList == null || fileList.size() == 0) {
            return false;
        }

        final List<String> list = fileList;
        SimpleDateFormat dateFormat = new SimpleDateFormat("yyyyMMddHHmmss");
        String currTime = dateFormat.format(new Date()).toString();
        String archiveName = mBaseDirectory + "/dump_logcat_" + currTime + ".zip";
        new ZipArchiveThread(archiveName, fileList, new ZipArchiveThread.OnZipArchiveListener() {
            @Override
            public void onZipArchiveStarted() {
                Log.d(TAG, "onZipArchiveStarted");
            }

            @Override
            public void onZipArchiveComplete() {
                Log.d(TAG, "onZipArchiveComplete. Dump AP logs done.");
                for (String name : list) {
                    File f = new File(name);
                    if (f.exists()) {
                        f.delete();
                    }
                } // end for ~
                stopSelf();
            }
        }).start();

        return true;
    }
}
