package com.samsung.slsi.audiologging;

import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.os.IBinder;
import android.util.Log;
import android.os.HwBinder;
import android.os.RemoteException;
import android.os.SystemProperties;





import vendor.samsung_slsi.hardware.audio_dump.V1_0.*;

public class LogActionService extends Service {
    private static final String TAG = "LogActionService";
    private static final boolean DEBUG = true;

    static final String AUDIO_LOGGING_START_INTENT = "com.samsung.slsi.audio.action.START_LOGGING";
    static final String AUDIO_LOGGING_STOP_INTENT = "com.samsung.slsi.audio.action.STOP_LOGGING";
    static final String AUDIO_LOGGING_RESULT = "com.samsung.slsi.audio.action.LOGGING_RESULT";
    IAudioDump mService;
    DeathRecipient mDeathRecipient;
    Context mContext;


    private void initAudioDumpService() {
        Log.d(TAG, "initAudioDumpService: enter");
        try {
            mService = IAudioDump.getService(true);
            if (mService == null) {
                Log.d(TAG, "initAudioDumpService: service is null.");
            }else {
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
    private void sendActionResult(boolean result){
        Intent intent = new Intent(AUDIO_LOGGING_RESULT);
        Log.d(TAG, "sendActionResult : " + result);
        intent.putExtra("result",result);
        mContext.sendBroadcast(intent);
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

    @Override
    public void onCreate() {
        super.onCreate();
        mDeathRecipient = new DeathRecipient();
        mContext = getApplicationContext();
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        initAudioDumpService();
        if(mService == null){
            sendActionResult(false);
            releaseAudioDumpService();
            stopSelf();
            return super.onStartCommand(intent, flags, startId);
        }
        String action = intent.getAction();
        String value = SystemProperties.get("ro.boot.hardware");
        int logcmd = (value.equals("s5e9815"))? 0:2;
        switch(action) {
            case AUDIO_LOGGING_START_INTENT:
                try {
                    mService.startDump(logcmd);
                    sendActionResult(true);
                    } catch (RemoteException e) {
                        e.printStackTrace();
                        sendActionResult(false);
                }
                break;
            case AUDIO_LOGGING_STOP_INTENT:
                try {
                mService.stopDump();
                sendActionResult(true);
                } catch (RemoteException e) {
                    e.printStackTrace();
                    sendActionResult(false);
                }
                break;
            default :
                break;
        }
        releaseAudioDumpService();
        stopSelf();
        return super.onStartCommand(intent, flags, startId);
    }

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
    }
}
