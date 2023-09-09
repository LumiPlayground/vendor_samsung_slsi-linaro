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
package com.shannon.networkservice;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.telephony.Rlog;
import android.util.SparseArray;

import com.android.internal.telephony.PhoneConstants;

import java.util.LinkedList;

public class ConfigurationBroadcastReceiver extends BroadcastReceiver {
    private static final boolean DBG = true;
    private static final String TAG = "ShannonCBR-NS";
    private static final String BROADCAST_ACTION_VENDOR_CONFIG_CHANGED = "com.shannon.ACTION.VCC.vendor_config_changed";

    public interface IConfigurationReceiverCallback {
        public void onReceive(Intent intent);
    }

    private static SparseArray<LinkedList<Intent>> mIntents = new SparseArray<>();
    private static SparseArray<IConfigurationReceiverCallback> mCallbacks = new SparseArray<>();

    @Override
    public void onReceive(Context context, Intent intent) {
        final String action = intent.getAction();
        int slotId = intent.getIntExtra(PhoneConstants.SLOT_KEY, -1);
        if (slotId < 0 || action == null) {
            loge("Invalid slotId or action null");
            return;
        }
        log("Receive " + intent + " for slotId: " + slotId);

        switch (action) {
            case BROADCAST_ACTION_VENDOR_CONFIG_CHANGED: {
                IConfigurationReceiverCallback callback = mCallbacks.get(slotId);
                if (callback != null) {
                    callback.onReceive(intent);
                } else {
                    LinkedList<Intent> intents = mIntents.get(slotId);
                    if (intents == null) {
                        intents = new LinkedList<>();
                    }
                    intents.add(intent);
                    mIntents.put(slotId, intents);
                }
                break;
            }
            default: ;
        }
    }

    public static void registerConfigurationReceiverCallback(int slotId, IConfigurationReceiverCallback callback) {
        log("registerConfigurationReceiverCallback for slotId: " + slotId + ", callback: " + callback);
        mCallbacks.put(slotId, callback);

        LinkedList<Intent> intents = mIntents.get(slotId);
        if (intents != null && !intents.isEmpty()) {
            for (Intent intent : intents) {
                callback.onReceive(intent);
            }
            mIntents.put(slotId, null);
        }
    }

    public static void unregisterConfigurationReceiverCallback(int slotId) {
        mCallbacks.remove(slotId);
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
