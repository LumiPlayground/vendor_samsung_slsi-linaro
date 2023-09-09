/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

package com.samsung.slsi.telephony.sstest;

import android.content.Context;
import android.content.DialogInterface;
import android.os.AsyncResult;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.preference.EditTextPreference;
import android.telephony.PhoneNumberUtils;
import android.telephony.SubscriptionInfo;
import android.util.Log;
import android.widget.Toast;

import com.android.internal.telephony.GsmCdmaPhone;
import com.android.internal.telephony.Phone;
import com.android.internal.telephony.PhoneFactory;
import com.android.internal.telephony.gsm.GsmMmiCode;

import com.samsung.slsi.telephony.oem.OemRil;
import com.samsung.slsi.telephony.oem.io.DataWriter;

import java.io.IOException;

public class SsEditTextPreference extends EditTextPreference {

    private static final String TAG = "SsEditTextPreference";

    private Phone mPhone;
    SubscriptionInfo mSsInfo;
    private int mPhoneId = -1;
    private OemRil[] mOemRil = new OemRil[2];

    private static final int EVENT_RIL_CONNECTED        = 100;
    private static final int EVENT_RIL_DISCONNECTED     = 101;

    private static final int EVENT_SET_COMPLETE = 1;

    private static final int RILC_REQ_CHANGE_BARRING_PASSWORD_OVER_MMI = 700;

    public SsEditTextPreference(Context context, SubscriptionInfo ssInfo) {
        super(context, null);
        mSsInfo = ssInfo;
        mPhoneId = ssInfo.getSimSlotIndex();
        connectToOemRilService();
    }

    // DOTO: disconnect Ril service

    @Override
    public void onClick(DialogInterface dialog, int which) {
        if (which == DialogInterface.BUTTON_POSITIVE) {
            Log.i(TAG, "click");
            String text = getEditText().getText().toString();
            Log.d(TAG, "text: " + text);

/*
            mPhone = (PhoneFactory.getPhones())[mPhoneId];
            String networkPortion = PhoneNumberUtils.extractNetworkPortionAlt(text);
            GsmMmiCode mmi = GsmMmiCode.newFromDialString(networkPortion, (GsmCdmaPhone)mPhone,
                                    ((GsmCdmaPhone)mPhone).getUiccCardApplication());

            if (mmi == null) {
                return;
            }
*/
            text = text.substring(2, text.length() - 1);
            Log.d(TAG, "text: " + text);

            String[] mmiCode = text.split("\\*");
            String[] str = {mmiCode[1], mmiCode[2], mmiCode[3], mmiCode[4]};

            DataWriter dr = new DataWriter();
            try {
                dr.writeInt(4);
                dr.writeStrings(str);
            } catch (IOException e) {
                Log.i(TAG, "SendData() IOException" + e);
            }

            if (mOemRil[mPhoneId] != null) {
                mOemRil[mPhoneId].invokeRequestRaw(RILC_REQ_CHANGE_BARRING_PASSWORD_OVER_MMI,
                        dr.toByteArray(), mHandler.obtainMessage(EVENT_SET_COMPLETE));
                Toast.makeText(getContext(), "Requesting call barring password change",
                                                Toast.LENGTH_LONG).show();
            }
        }
        super.onClick(dialog, which);
    }

    private Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            AsyncResult ar = (AsyncResult) msg.obj;
            switch (msg.what) {
                case EVENT_SET_COMPLETE:
                    if (ar.exception == null) {
                        Toast.makeText(getContext(), "Success response of call barring password change",
                                                        Toast.LENGTH_LONG).show();
                        Log.d(TAG, "Success");
                    } else {
                        Toast.makeText(getContext(), "Error: The password to be changed does not match.",
                                                        Toast.LENGTH_LONG).show();
                        Log.d(TAG, "Fail: " + msg.what);
                    }
                    break;
            }
        }
    };

    private void connectToOemRilService() {
        for (int i = 0; i < 2; i++) {
            mOemRil[i] = OemRil.init(getContext(), i);
            if (mOemRil[i] == null) {
                Log.d(TAG, "connectToOemRilService mOemRil"+i+" is null");
            } else {
                mOemRil[i].registerForOemRilConnected(mHandler, EVENT_RIL_CONNECTED);
                mOemRil[i].registerForOemRilDisconnected(mHandler, EVENT_RIL_DISCONNECTED);
            }
        }
    }
}
