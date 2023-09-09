package com.samsung.slsi.audiologging;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;
import android.os.RemoteException;


public class LogActionReceiver extends BroadcastReceiver {
    private static final String TAG = "LogActionReceiver";
    private static final boolean DEBUG = true;

    static final String AUDIO_LOGGING_START_INTENT = "com.samsung.slsi.audio.action.START_LOGGING";
    static final String AUDIO_LOGGING_STOP_INTENT = "com.samsung.slsi.audio.action.STOP_LOGGING";
    static final String AUDIO_LOGGING_RESULT = "com.samsung.slsi.audio.action.LOGGING_RESULT";

    Context mContext;

    @Override
    public void onReceive(Context context, Intent intent) {
        String action = intent.getAction();
        Log.d(TAG, "onReceive : " + action);

        if (action == null) {
            return;
        }
        mContext = context;
        Intent newintent = new Intent(context, LogActionService.class);
        newintent.setAction(action);
        context.startService(newintent);
      }
}
