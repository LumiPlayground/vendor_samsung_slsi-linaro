/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */
package com.samsung.slsi.telephony.oemril;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

import com.samsung.slsi.telephony.oemril.service.OemRilService;

public class OemRilServiceReceiver extends BroadcastReceiver {

    private static final String TAG = "OemRilServiceReceiver";

    @Override
    public void onReceive(Context context, Intent intent) {
        Log.d(TAG, intent.getAction());
        if (intent != null && "android.intent.action.BOOT_COMPLETED".equals(intent.getAction())) {
            context.startService(new Intent(context, OemRilService.class));
        }
    }
}
