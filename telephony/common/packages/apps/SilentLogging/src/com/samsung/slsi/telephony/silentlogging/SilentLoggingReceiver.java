/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

package com.samsung.slsi.telephony.silentlogging;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.os.SystemProperties;
import android.util.Log;

/**
 * Created by jin-h.shin on 2015-12-07.
 */
public class SilentLoggingReceiver extends BroadcastReceiver {

    private static final String TAG = "SilentLoggingReceiver";

    @Override
    public void onReceive(Context context, Intent intent) {

        if (intent.getAction().equals("android.intent.action.BOOT_COMPLETED")) {
            Log.d(TAG, "BOOT_COMPLETED");
            String prop = String.valueOf(SystemProperties.get(SilentLoggingConstant.PROPERTY_SLOG_MODE));
            if (SilentLoggingControlInterface.isEnabled() && prop.equalsIgnoreCase("On")) {
                SilentLoggingControlInterface.getInstance().setContext(context);
                SilentLoggingControlInterface.getInstance().autoStartAPTCP();
            }
            context.startService(new Intent(context, LogdConfigService.class));
        }
    }
}
