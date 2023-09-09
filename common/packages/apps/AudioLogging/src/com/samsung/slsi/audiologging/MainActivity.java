package com.samsung.slsi.audiologging;

import android.Manifest;
import android.app.Activity;
import android.content.Context;
import android.content.SharedPreferences;
import android.content.pm.PackageManager;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Environment;
import android.os.HwBinder;
import android.os.RemoteException;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;

import vendor.samsung_slsi.hardware.audio_dump.V1_0.*;

/*
supported Logging types
AUDIO_DBG = 0
GENERAL_DBG = 1
SILENT_DBG = 2
*/

public class MainActivity extends Activity {
    private static final String TAG = "AudioLogging";
    private static final boolean DEBUG = true;

    private static final int PERMISSION_REQUEST_CODE = 1;
    private static final String PREF_PERMISSION_ALLOWED = "permission";

    SharedPreferences mSharedPref;

    IAudioDump mService;
    DeathRecipient mDeathRecipient;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        mSharedPref = getPreferences(Context.MODE_PRIVATE);
        SharedPreferences.Editor editor = mSharedPref.edit();

        if (checkSelfPermission(Manifest.permission.READ_EXTERNAL_STORAGE) !=
                PackageManager.PERMISSION_GRANTED ||
                checkSelfPermission(Manifest.permission.WRITE_EXTERNAL_STORAGE) !=
                PackageManager.PERMISSION_GRANTED) {
            editor.putInt(PREF_PERMISSION_ALLOWED, -1);
            if (DEBUG) Log.d(TAG,
                    "Required permission is not currently allowed.");
            requestPermissions(new String[]{Manifest.permission.READ_EXTERNAL_STORAGE,
                            Manifest.permission.WRITE_EXTERNAL_STORAGE}, PERMISSION_REQUEST_CODE);
        } else {
            editor.putInt(PREF_PERMISSION_ALLOWED, 0);
        }
        editor.commit();

        setContentView(R.layout.activity_main);

        mDeathRecipient = new DeathRecipient();

        Button audioButton = (Button) findViewById(R.id.button_audio);
        Button silentButton = (Button) findViewById(R.id.button_silent);
        Button generalButton = (Button) findViewById(R.id.button_general);
        Button offButton = (Button) findViewById(R.id.button_off);
        Button copyButton = (Button) findViewById(R.id.button_copy);

        audioButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                try {
                    mService.startDump(0);
                } catch (RemoteException e) {
                    e.printStackTrace();
                }
                Toast.makeText(MainActivity.this,
                        "\'on\' AudioDump Enable message sent to audiodumpd",
                        Toast.LENGTH_SHORT).show();

                audioButton.setEnabled(false);
                silentButton.setEnabled(false);
                generalButton.setEnabled(false);
                offButton.setEnabled(true);
            }
        });
        generalButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                try {
                    mService.startDump(1);
                } catch (RemoteException e) {
                    e.printStackTrace();
                }
                Toast.makeText(MainActivity.this,
                        "\'on\' generalDump Enable message sent to audiodumpd",
                        Toast.LENGTH_SHORT).show();
                audioButton.setEnabled(false);
                silentButton.setEnabled(false);
                generalButton.setEnabled(false);
                offButton.setEnabled(true);
            }
        });
        silentButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                try {
                    mService.startDump(2);
                } catch (RemoteException e) {
                    e.printStackTrace();
                }
                Toast.makeText(MainActivity.this,
                        "\'on\' SilentDump Enable message sent to audiodumpd",
                        Toast.LENGTH_SHORT).show();

                audioButton.setEnabled(false);
                silentButton.setEnabled(false);
                generalButton.setEnabled(false);
                offButton.setEnabled(true);
            }
        });
        offButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                try {
                    mService.stopDump();
                } catch (RemoteException e) {
                    e.printStackTrace();
                }
                Toast.makeText(MainActivity.this,
                        "\'off\' message was sent to the audiodumpd.",
                        Toast.LENGTH_SHORT).show();

                audioButton.setEnabled(true);
                silentButton.setEnabled(true);
                generalButton.setEnabled(true);
                offButton.setEnabled(false);
                copyButton.setEnabled(true);
            }
        });
        copyButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                int permissionAllowedPrefValue =
                        mSharedPref.getInt(PREF_PERMISSION_ALLOWED, -1);
                Log.d(TAG, "\'COPY\' button was clicked. (permissionAllowedPrefValue = " +
                        permissionAllowedPrefValue + ")");
                if (permissionAllowedPrefValue < 0) {
                    Toast.makeText(MainActivity.this,
                            "The \'COPY\' function requires the necessary permission grant.",
                            Toast.LENGTH_LONG).show();
                } else {
                    new DumpFileCopyTask(MainActivity.this).execute();
                    copyButton.setEnabled(false);
                }
            }
        });

        audioButton.setEnabled(true);
        silentButton.setEnabled(true);
        generalButton.setEnabled(true);
        offButton.setEnabled(false);
        copyButton.setEnabled(false);
    }

    @Override
    protected void onResume() {
        super.onResume();
        initAudioDumpService();
    }

    @Override
    protected void onPause() {
        super.onPause();
        releaseAudioDumpService();
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions,
                                           int[] grantResults) {
        if (grantResults.length > 0) {
            for (int i = 0; i < grantResults.length; i++)
                Log.d(TAG, "grantResults[" + i + "] = " + grantResults[i]);
            SharedPreferences.Editor editor = mSharedPref.edit();
            if (grantResults[0] == PackageManager.PERMISSION_DENIED ||
                    grantResults[1] == PackageManager.PERMISSION_DENIED) {
                Toast.makeText(this,
                        "The \'COPY\' function requires the necessary permission grant.",
                        Toast.LENGTH_LONG).show();
                editor.putInt(PREF_PERMISSION_ALLOWED, -1);
            } else if (grantResults[0] == PackageManager.PERMISSION_GRANTED &&
                    grantResults[1] == getPackageManager().PERMISSION_GRANTED) {
                editor.putInt(PREF_PERMISSION_ALLOWED, 0);
            }
            editor.commit();
        }
    }

    private void initAudioDumpService() {
        Log.d(TAG, "initAudioDumpService: enter");
        try {
            mService = IAudioDump.getService(true);
            if (mService == null) {
                Log.d(TAG, "initAudioDumpService: service is null.");
            } else {
                Log.d(TAG, "initAudioDumpService: service is not null.");
                mService.linkToDeath(mDeathRecipient, 0);
            }
        } catch (RemoteException e) {
            e.printStackTrace();
        }
    }

    private void releaseAudioDumpService() {
        Log.d(TAG, "releaseAudioDumpService");
        try {
            if (mService != null) {
                mService.unlinkToDeath(mDeathRecipient);
                mService = null;
            }
        } catch (RemoteException e) {
            e.printStackTrace();
        }
    }

    class DeathRecipient implements HwBinder.DeathRecipient {
        private static final String TAG = "AudioLogging";

        @Override
        public void serviceDied(long cookie) {
            Log.d(TAG, "serviceDied");
            releaseAudioDumpService();
            initAudioDumpService();
        }
    }


    private class DumpFileCopyTask extends AsyncTask<Void, Void, Void> {
        private static final String DUMP_PATH = "/data/vendor/log/abox";

        Context mContext;

        public DumpFileCopyTask(Context context) {
            mContext = context;
        }

        @Override
        protected Void doInBackground(Void... voids) {
            File dumpDir = null;
            try {
                dumpDir = new File(DUMP_PATH);
                if (dumpDir.exists()) {
                    if (DEBUG) Log.d(TAG, "dumpDir.canRead() = " + dumpDir.canRead());
                    File[] dumpDirFiles = dumpDir.listFiles();
                    if (DEBUG) Log.d(TAG, "dumpDirFiles.length = " + dumpDirFiles.length);

                    File targetDir = mContext.getExternalFilesDir(null);

                    for (File file : dumpDirFiles) {
                        File targetFile =
                            new File(targetDir.getAbsolutePath() + File.separator + file.getName());

                        if (DEBUG) Log.d(TAG, "targetFile.getAbsolutePath() = " +
                                targetFile.getAbsolutePath());
                        FileInputStream fis = new FileInputStream(file);
                        FileOutputStream fos = new FileOutputStream(targetFile);;
                        byte[] buffer = new byte[4096];
                        int byteRead;
                        while ((byteRead = fis.read(buffer)) != -1) {
                            fos.write(buffer, 0, byteRead);
                        }
                        if (fis != null) {
                            fis.close();
                            fis = null;
                        }
                        if (fos != null) {
                            fos.close();
                            fos = null;
                        }
                    }
                } else {
                    Log.e(TAG, "There is no dump directory.");
                }
            } catch (Exception e) {
                e.printStackTrace();
            }
            return null;
        }

        @Override
        protected void onPostExecute(Void aVoid) {
            Toast.makeText(MainActivity.this, "Dump file copy is complete.",
                    Toast.LENGTH_SHORT).show();
        }
    }
}
