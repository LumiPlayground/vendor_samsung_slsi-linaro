package com.samsung.slsi.audiologging;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;

public class KeyStringReceiver extends BroadcastReceiver {
    private static final String TAG = "KeyStringReceiver";
    private static final boolean DEBUG = true;
    protected static final String KEY_NUM_INTENT = "android.provider.Telephony.SECRET_CODE";

    @Override
    public void onReceive(Context context, Intent intent) {
        if (intent != null && intent.getAction().equals(KEY_NUM_INTENT)) {
            Intent i = new Intent(Intent.ACTION_MAIN);
            i.setClass(context, MainActivity.class);
            i.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
            context.startActivity(i);
        }
    }
}
