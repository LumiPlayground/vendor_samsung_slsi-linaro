/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

package com.samsung.slsi.exynos.memlogcontroller;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.IOException;
import java.nio.channels.FileChannel;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.ProgressDialog;
import android.app.Dialog;
import android.content.ComponentName;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.graphics.Color;
import android.media.MediaScannerConnection;
import android.net.Uri;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.RadioButton;
import android.widget.TextView;
import android.widget.Toast;

public class MemlogControllerSettings extends Activity {

    private static final String TAG = "MemlogControllerSettings";
    private static final String TARGET_PACKAGE_CP = "com.samsung.slsi.telephony.silentlogging";
    private static final String TARGET_COMPONENT_CP = "com.samsung.slsi.telephony.silentlogging.SilentLoggingService";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        Log.i(TAG, "onCreate");
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_settings);
        getActionBar().setDisplayHomeAsUpEnabled(true);

        Button button_CP_SilentLogging_Start = (Button) findViewById(R.id.Button_CP_SilentLogging_Start);
        Button button_CP_SilentLogging_Stop = (Button) findViewById(R.id.Button_CP_SilentLogging_Stop);
        Button button_CP_DSP_Start = (Button) findViewById(R.id.Button_DSP_Enable);
        Button button_CP_DSP_Stop = (Button) findViewById(R.id.Button_DSP_Disable);
        Button button_GNSS_LOG_Start = (Button) findViewById(R.id.Button_GNSS_SilentLogging_Start);
        Button button_GNSS_LOG_Stop = (Button) findViewById(R.id.Button_GNSS_SilentLogging_Stop);
        Button btnCopy = (Button) findViewById(R.id.btnCopy);
        Button btnRemove = (Button) findViewById(R.id.btnRemove);
        Button btnDump = (Button) findViewById(R.id.btnDump);

        button_CP_SilentLogging_Start.setOnClickListener(new Button.OnClickListener() {
           @Override
           public void onClick(View view) {
               Log.i(TAG, "CP START_SILENT_LOGGING");
               Intent intent = new Intent("com.samsung.slsi.telephony.action.START_SILENT_LOGGING");
               intent.setComponent(new ComponentName(TARGET_PACKAGE_CP, TARGET_COMPONENT_CP));
               startService(intent);
               //Toast.makeText(MemlogControllerSettings.this, "Intent \"START_SILENT_LOGGING\"\n was sent to CP", Toast.LENGTH_LONG).show();
           }
        });

        button_CP_SilentLogging_Stop.setOnClickListener(new Button.OnClickListener() {
            @Override
            public void onClick(View view) {
                Log.i(TAG, "CP STOP_SILENT_LOGGING");
                Intent intent = new Intent("com.samsung.slsi.telephony.action.STOP_SILENT_LOGGING");
                intent.setComponent(new ComponentName(TARGET_PACKAGE_CP, TARGET_COMPONENT_CP));
                startService(intent);
                //Toast.makeText(MemlogControllerSettings.this, "Intent \"STOP_SILENT_LOGGING\"\n was sent to CP", Toast.LENGTH_LONG).show();
            }
        });

        button_CP_DSP_Start.setOnClickListener(new Button.OnClickListener() {
            @Override
            public void onClick(View view) {
                Log.i(TAG, "CP DSP_ENABLE");
                Intent intent = new Intent("com.samsung.slsi.telephony.action.DSP_ENABLE");
                intent.setComponent(new ComponentName(TARGET_PACKAGE_CP, TARGET_COMPONENT_CP));
                startService(intent);
                //Toast.makeText(MemlogControllerSettings.this, "Intent \"DSP_ENABLE\" was sent to CP", Toast.LENGTH_LONG).show();
            }
        });

        button_CP_DSP_Stop.setOnClickListener(new Button.OnClickListener() {
            @Override
            public void onClick(View view) {
                Log.i(TAG, "CP DSP_DISABLE");
                Intent intent = new Intent("com.samsung.slsi.telephony.action.DSP_DISABLE");
                intent.setComponent(new ComponentName(TARGET_PACKAGE_CP, TARGET_COMPONENT_CP));
                startService(intent);
                //Toast.makeText(MemlogControllerSettings.this, "Intent \"DSP_DISABLE\" was sent to CP", Toast.LENGTH_LONG).show();
            }
        });

        button_GNSS_LOG_Start.setOnClickListener(new Button.OnClickListener() {
            @Override
            public void onClick(View view) {
                final int default_level = 4;
                final int[] level = {default_level};
                AlertDialog.Builder builder = new AlertDialog.Builder(MemlogControllerSettings.this);
                builder.setTitle("Select Log Level")
                        .setSingleChoiceItems(R.array.gnss_levels, default_level,
                                new DialogInterface.OnClickListener() {
                                    @Override
                                    public void onClick(DialogInterface dialog, int which) {
                                        level[0] = which;
                                    }
                                })
                        .setPositiveButton("OK", new DialogInterface.OnClickListener() {
                            public void onClick(DialogInterface dialog, int id) {
                                level[0]++;
                                Log.i(TAG, "level select ok " + level[0]);
                                if (level[0] >= 1 && level[0] <= 5) {
                                    Log.i(TAG, "GNSS START_LOGGING");
                                    Intent intent = new Intent("com.samsung.slsi.gnss.action.START_LOGGING");
                                    intent.putExtra("level", level[0]);
                                    sendBroadcast(intent);
                                    //Toast.makeText(MemlogControllerSettings.this, "Intent \"START_LOGGING\" was sent to GNSS", Toast.LENGTH_LONG).show();
                                } else
                                    Log.i(TAG, "level invalid!");
                            }
                        })
                        .setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
                            public void onClick(DialogInterface dialog, int id) {
                                Log.i(TAG, "level select cancel");
                            }
                        });
                if (!isFinishing()) {
                    AlertDialog dialog = builder.create();
                    dialog.show();
                }
            }
        });

        button_GNSS_LOG_Stop.setOnClickListener(new Button.OnClickListener() {
            @Override
            public void onClick(View view) {
                Log.i(TAG, "GNSS STOP_LOGGING");
                Intent intent = new Intent("com.samsung.slsi.gnss.action.STOP_LOGGING");
                sendBroadcast(intent);
                //Toast.makeText(MemlogControllerSettings.this, "Intent STOP_LOGGING was sent to GNSS", Toast.LENGTH_LONG).show();
            }
        });

        btnCopy.setOnClickListener(new Button.OnClickListener() {
            @Override
            public void onClick(View view) {
                Log.i(TAG, "Copy to Storage");
                copyToStorage();
            }
        });

        btnRemove.setOnClickListener(new Button.OnClickListener() {
            @Override
            public void onClick(View view) {
                Log.i(TAG, "Remove all items in Storage");
                btnRemove.setEnabled(false);
                removeStorage();

                final Handler handler = new Handler();
                handler.postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        btnRemove.setEnabled(true);
                    }
                }, 3000);
            }
        });

        btnDump.setOnClickListener(new Button.OnClickListener() {
            @Override
            public void onClick(View view) {
                Log.i(TAG, "Save dumpstate in Storage");
                Thread thread = new Thread(new Runnable() {
                    @Override
                    public void run() {
                        // TODO Auto-generated method stub
                        runOnUiThread(new Runnable() {
                            public void run() {
                                btnDump.setEnabled(false);
                            }
                        });
                        saveDumpstate();
                    }
                });
                thread.start();

                final Handler handler = new Handler();
                handler.postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        // Do something after 3s = 3000ms
                        final File src = new File("/data/vendor/log");
                        final File dst = new File(Environment.getExternalStorageDirectory().toString() + "/Memlogger");
                        String fileName = "dumpstate.txt";
                        if (!dst.exists()) {
                            dst.mkdir();
                        }
                        copyDirectory(new File(src, fileName), new File(dst, fileName));
                        Toast.makeText(MemlogControllerSettings.this, "Memlogger dumpstate is\nsaved in /storage/emulated/0/Memlogger", Toast.LENGTH_LONG).show();
                        btnDump.setEnabled(true);
                    }
                }, 3000);
            }
        });
    }
/*
    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_silent_logging, menu);
        return true;
    }
*/
    private void copyToStorage() {
        LayoutInflater inflater = LayoutInflater.from(this);
        final View dialogView = inflater.inflate(R.layout.dialog_storage, null);
        final RadioButton dialog_rb_internal = (RadioButton) dialogView.findViewById(R.id.dialog_rb_internal);
        final RadioButton dialog_rb_sdcard = (RadioButton) dialogView.findViewById(R.id.dialog_rb_sdcard);
        Button btnCopy = (Button) findViewById(R.id.btnCopy);
        final File sdcardDirectory = new File("/storage/sdcard/log/Memlogger");
        final File dataDirectory = new File("/data/vendor/log");
        final File internalDirectory = new File(Environment.getExternalStorageDirectory().toString() + "/Memlogger");
        AlertDialog.Builder mBuilder;
        mBuilder = new AlertDialog.Builder(this);
        mBuilder.setTitle("Setting storage");
        mBuilder.setView(dialogView);
        mBuilder.setPositiveButton(android.R.string.ok, new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                Thread thread = new Thread(new Runnable() {
                    @Override
                    public void run() {
                        // TODO Auto-generated method stub
                        String result = "";
                        if (dialog_rb_internal.isChecked()) {
                            Log.d(TAG, "internel storage is checked");
                            result = copyDirectory(dataDirectory, internalDirectory);
                        } else {
                            Log.d(TAG, "sdcard storage is checked");
                            result = copyDirectory(dataDirectory, sdcardDirectory);
                        }

                        Log.i(TAG, result);
                    }
                });
                thread.start();
                btnCopy.setEnabled(false);
                final Handler handler = new Handler();
                handler.postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        btnCopy.setEnabled(true);
                    }
                }, 3000);
                //Toast.makeText(MemlogControllerSettings.this, "Memlog files are copied", Toast.LENGTH_LONG).show();
            }
        });
        mBuilder.setNegativeButton(android.R.string.cancel, null);
        if (!Environment.getExternalStorageState(sdcardDirectory).equals(android.os.Environment.MEDIA_MOUNTED)) {
            Log.d(TAG, "getExternalStorageState : no sdcard directory");
            dialog_rb_sdcard.setClickable(false);
            dialog_rb_sdcard.setTextColor(Color.GRAY);
        }
        mBuilder.show();
    }

    private String copyDirectory(File src, File dst) {
        Log.i(TAG, "Copy Directory : To " + dst + " from " + src);
        FileInputStream srcFile = null;
        FileOutputStream dstFile = null;
        FileChannel srcChannel = null;
        FileChannel dstChannel = null;

        String result = "";

        if (src.isDirectory()) {
            if (!dst.exists()) {
                dst.mkdir();
            }

            String[] fileList = src.list();

            if (fileList == null || fileList.length <= 0)
                return "No file in " + src.getPath() + " folder.\n";

            for (int i = 0; i < fileList.length; i++) {
                if ((new File(src, fileList[i])).isDirectory())
                {
                    Log.i(TAG, "found subdirectory : " + fileList[i]);
                    result += copyDirectory(new File(src, fileList[i]), new File(dst, fileList[i]));
                }
                if (fileList[i].endsWith(".memlog")) // support only memlog file
                {
                    //Log.i(TAG, ".memlog file found : " + fileList[i]);
                    result += copyDirectory(new File(src, fileList[i]), new File(dst, fileList[i]));
                }
            }
        } else {
            try {
                if (src.exists() == false) {
                    return src.getName() + " doesn't exist!!\n";
                }
                if (src.length() == 0) {
                    return src.getName() + " skip empty files!\n";
                }
                srcFile = new FileInputStream(src);
                dstFile = new FileOutputStream(dst);
                srcChannel = srcFile.getChannel();
                dstChannel = dstFile.getChannel();
                long size = srcChannel.size();

                srcChannel.transferTo(0, size, dstChannel);
                result += src.getPath() + " was copied (" + src.length() +").\n";
            } catch (Exception e) {
                e.printStackTrace();
                Log.i(TAG, "Exception : " + e);
            } finally {
                try {
                    if (src.exists()) {
                        if (srcChannel != null)
                            srcChannel.close();
                        if (srcFile != null)
                            srcFile.close();
                        if (dstChannel != null)
                            dstChannel.close();
                        if (dstFile != null)
                            dstFile.close();
                    }
                    /*
                    if (src.length() > 0) {
                        Intent intent = new Intent(Intent.ACTION_MEDIA_SCANNER_SCAN_FILE);
                        intent.setData(Uri.fromFile(dst));
                        sendBroadcast(intent);
                    }
                     */
                } catch (IOException e) {
                    e.printStackTrace();
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }
        return result;
    }

    private void removeStorage() {
        LayoutInflater inflater = LayoutInflater.from(this);
        final View dialogView = inflater.inflate(R.layout.dialog_storage, null);
        Button btnRemove = (Button) findViewById(R.id.btnRemove);
        final RadioButton dialog_rb_internal = (RadioButton) dialogView.findViewById(R.id.dialog_rb_internal);
        final RadioButton dialog_rb_sdcard = (RadioButton) dialogView.findViewById(R.id.dialog_rb_sdcard);
        final File sdcardDirectory = new File("/storage/sdcard/log/Memlogger");
        final File internalDirectory = new File(Environment.getExternalStorageDirectory().toString() + "/Memlogger");
        AlertDialog.Builder mBuilder;
        mBuilder = new AlertDialog.Builder(this);
        mBuilder.setTitle("Select storage to remove");
        mBuilder.setView(dialogView);
        mBuilder.setPositiveButton(android.R.string.ok, new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                Thread thread = new Thread(new Runnable() {
                    @Override
                    public void run() {
                        // TODO Auto-generated method stub
                        File dir;
                        if (dialog_rb_internal.isChecked()) {
                            Log.d(TAG, "internel storage is checked");
                            //result = removeDirectory(dataDirectory, internalDirectory);
                            dir = internalDirectory;
                        } else {
                            Log.d(TAG, "sdcard storage is checked");
                            //result = copyDirectory(dataDirectory, sdcardDirectory);
                            dir = sdcardDirectory;
                        }
                        int count = 0;
                        if (dir.isDirectory()) {
                            for (File f : dir.listFiles()) {
                                if (f.delete()) {
                                    Log.d(TAG, "Deleted file: " + f.getName());
                                    count++;
                                }
                            }
                            Log.d(TAG, "Total " + count + "files have been deleted");
                        }
                    }
                });
                thread.start();
                btnRemove.setEnabled(false);
                final Handler handler = new Handler();
                handler.postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        btnRemove.setEnabled(true);
                    }
                }, 3000);
                //Toast.makeText(MemlogControllerSettings.this, "Memlog files are removed", Toast.LENGTH_LONG).show();
            }
        });
        mBuilder.setNegativeButton(android.R.string.cancel, null);
        if (!Environment.getExternalStorageState(sdcardDirectory).equals(android.os.Environment.MEDIA_MOUNTED)) {
            Log.d(TAG, "getExternalStorageState : no sdcard directory");
            dialog_rb_sdcard.setClickable(false);
            dialog_rb_sdcard.setTextColor(Color.GRAY);
        }
        mBuilder.show();
    }

    private void saveDumpstate() {
        MemlogControllerControlInterface.getInstance().saveDumpstate();
    }
}