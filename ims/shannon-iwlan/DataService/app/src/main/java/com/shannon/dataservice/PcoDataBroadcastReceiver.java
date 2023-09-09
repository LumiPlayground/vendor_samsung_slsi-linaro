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
package com.shannon.dataservice;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.telephony.Rlog;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;

import java.util.Arrays;

public class PcoDataBroadcastReceiver extends BroadcastReceiver {
    private static final boolean DBG = true;
    private static final String TAG = "ShannonPBR-DS";

    @Override
    public void onReceive(Context context, Intent intent) {
        final String action = intent.getAction();
        if (action == null) {
            loge("NULL action for intent " + intent);
            return;
        }
        log("Intent received " + intent);
        switch (action) {
            case TelephonyManager.ACTION_CARRIER_SIGNAL_PCO_VALUE:
                int subId = intent.getIntExtra(SubscriptionManager.EXTRA_SUBSCRIPTION_INDEX, SubscriptionManager.INVALID_SUBSCRIPTION_ID);
                if (SubscriptionManager.isValidSubscriptionId(subId)) {
                    int slotIndex = SubscriptionManager.getSlotIndex(subId);
                    if (slotIndex > SubscriptionManager.INVALID_SIM_SLOT_INDEX) {
                        String apn = intent.getStringExtra(TelephonyManager.EXTRA_APN_TYPE);
                        int pcoId = intent.getIntExtra(TelephonyManager.EXTRA_PCO_ID, 0);
                        byte[] pcoData = intent.getByteArrayExtra(TelephonyManager.EXTRA_PCO_VALUE);
                        log("PCO " + String.format("%X", pcoId) + " [" + Arrays.toString(pcoData) + "] for " + apn +  " on slot " + slotIndex);

                        ShannonDataService ds = ShannonDataService.getInstance();
                        if (ds != null) {
                            ds.sendPcoData(slotIndex, apn, pcoId, pcoData);
                        }
                    }
                }
                break;
            default:;
        }
    }

    private static void log(String s) {
        if (DBG) {
            Rlog.d(TAG, s);
        }
    }

    private void loge(String s) {
        Rlog.e(TAG, s);
    }
}
