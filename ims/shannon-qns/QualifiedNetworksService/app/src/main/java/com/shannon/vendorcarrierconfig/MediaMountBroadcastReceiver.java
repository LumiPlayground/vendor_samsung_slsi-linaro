/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or
 * distributed, transmitted, transcribed, stored in a retrieval system or
 * translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed to third parties
 * without the express written permission of Samsung Electronics.
 */
package com.shannon.vendorcarrierconfig;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.telephony.Rlog;

public class MediaMountBroadcastReceiver extends BroadcastReceiver {
    private static final boolean DBG = true;
    private static final String TAG = "ShannonMBR";

    @Override
    public void onReceive(Context context, Intent intent) {
        final String action = intent.getAction();
        if (action == null) {
            return;
        }
        log("Received : " + action);
        try {
            Intent vccIntent = new Intent(ShannonVendorCarrierConfig.BROADCAST_ACTION_VENDOR_CONFIG_MEDIA_MOUNTED);
            context.sendBroadcast(vccIntent);
        } catch (Exception e) {
            loge("Failed to send broadcast " + e);
        }
    }

    private void log(String s) {
        if (DBG) {
            Rlog.d(TAG, s);
        }
    }

    private void loge(String s) {
        Rlog.e(TAG, s);
    }
}
