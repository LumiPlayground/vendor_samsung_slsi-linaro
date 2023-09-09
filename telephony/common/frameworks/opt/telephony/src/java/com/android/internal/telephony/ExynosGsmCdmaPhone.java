/*
 * Copyright (C) 2015 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.internal.telephony.vendor;

import android.compat.annotation.UnsupportedAppUsage;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.os.Bundle;
import android.os.Message;
import android.os.PowerManager;
import android.os.Registrant;
import android.os.RegistrantList;
import android.os.ResultReceiver;
import android.provider.Telephony;
import android.telephony.ServiceState;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
import android.telephony.UssdResponse;
import android.text.TextUtils;
import android.util.Log;

import com.android.ims.ImsManager;
import com.android.internal.telephony.imsphone.ImsPhone;
import com.android.telephony.Rlog;

import com.android.internal.telephony.CommandsInterface;
import com.android.internal.telephony.GsmCdmaPhone;
import com.android.internal.telephony.PhoneNotifier;
import com.android.internal.telephony.TelephonyComponentFactory;

/**
 * {@hide}
 */
public class ExynosGsmCdmaPhone extends GsmCdmaPhone {
    // NOTE that LOG_TAG here is "GsmCdma", which means that log messages
    // from this file will go into the radio log rather than the main
    // log.  (Use "adb logcat -b radio" to see them.)
    public static final String LOG_TAG = "ExynosGsmCdmaPhone";
    private static final boolean DBG = true;
    private static final boolean VDBG = false; /* STOPSHIP if true */

    // Constructors

    public ExynosGsmCdmaPhone(Context context, CommandsInterface ci, PhoneNotifier notifier, int phoneId,
                        int precisePhoneType, TelephonyComponentFactory telephonyComponentFactory) {
        this(context, ci, notifier, false, phoneId, precisePhoneType, telephonyComponentFactory);
    }

    public ExynosGsmCdmaPhone(Context context, CommandsInterface ci, PhoneNotifier notifier,
                        boolean unitTestMode, int phoneId, int precisePhoneType,
                        TelephonyComponentFactory telephonyComponentFactory) {
        super(context, ci, notifier, unitTestMode, phoneId, precisePhoneType, telephonyComponentFactory);
    }

    private void logd(String msg) {
        Rlog.d(LOG_TAG, "[" + mPhoneId +" ] " + msg);
    }

    private void loge(String msg) {
        Rlog.e(LOG_TAG, "[" + mPhoneId +" ] " + msg);
    }
}

/* Injection Strategy : Add Changes */
