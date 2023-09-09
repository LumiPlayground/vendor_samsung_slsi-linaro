/*
 * Copyright (C) 2013 The Android Open Source Project
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

package com.android.internal.telephony.vendor.imsphone;

import static com.android.internal.annotations.VisibleForTesting.Visibility.PRIVATE;
import static com.android.internal.telephony.Phone.CS_FALLBACK;

import android.compat.annotation.UnsupportedAppUsage;
import android.content.Context;
import android.content.SharedPreferences;
import android.os.Build;
import android.telephony.DisconnectCause;
import android.telephony.ims.ImsReasonInfo;
import android.util.Log;

import com.android.ims.FeatureConnector;
import com.android.ims.ImsCall;
import com.android.internal.annotations.VisibleForTesting;
import com.android.internal.telephony.Call;
import com.android.internal.telephony.Phone;
import com.android.internal.telephony.imsphone.ImsPhone;
import com.android.internal.telephony.imsphone.ImsPhone.ImsDialArgs;
import com.android.internal.telephony.imsphone.ImsPhoneCallTracker;
import com.android.internal.telephony.imsphone.ImsPhoneCallTracker.ConnectorFactory;
import com.android.internal.telephony.imsphone.ImsPhoneConnection;
import com.android.internal.telephony.metrics.TelephonyMetrics;
import com.android.telephony.Rlog;

import java.util.concurrent.Executor;

/**
 * {@hide}
 */
public class ExynosImsPhoneCallTracker extends ImsPhoneCallTracker {
    static final String LOG_TAG = "ExynosImsPhoneCallTracker";
    private static final boolean DBG = true;

    private TelephonyMetrics mMetrics;

    @UnsupportedAppUsage(maxTargetSdk = Build.VERSION_CODES.R, trackingBug = 170729553)
    ImsPhone mPhone;

    //***** Constructors
    public ExynosImsPhoneCallTracker(ImsPhone phone, ConnectorFactory factory) {
        this(phone, factory, phone.getContext().getMainExecutor());
    }

    @VisibleForTesting
    public ExynosImsPhoneCallTracker(ImsPhone phone, ConnectorFactory factory, Executor executor) {
        super(phone, factory, executor);
        this.mPhone = phone;

        mMetrics = TelephonyMetrics.getInstance();
    }

    /**
     * Maps an {@link ImsReasonInfo} reason code to a {@link DisconnectCause} cause code.
     * The {@link Call.State} provided is the state of the call prior to disconnection.
     * @param reasonInfo the {@link ImsReasonInfo} for the disconnection.
     * @param callState The {@link Call.State} prior to disconnection.
     * @return The {@link DisconnectCause} code.
     */
    @VisibleForTesting
    public int getDisconnectCauseFromReasonInfo(ImsReasonInfo reasonInfo, Call.State callState) {
        int cause = super.getDisconnectCauseFromReasonInfo(reasonInfo, callState);

        int code = maybeRemapReasonCode(reasonInfo);
        switch (code) {
            case ImsReasonInfo.CODE_SIP_FORBIDDEN:
                return DisconnectCause.CALL_FAILURE;
        }
        return cause;
    }
}
