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

import java.io.IOException;

import android.content.Context;
import android.os.AsyncResult;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;
import android.os.Message;
import android.util.Log;

import com.samsung.slsi.telephony.oem.OemRilBinderManager;
import com.samsung.slsi.telephony.oem.OemRilConstants;
import com.samsung.slsi.telephony.oem.io.DataReader;
import com.samsung.slsi.telephony.oem.io.DataWriter;

public class OemCommandsHelper implements OemCommandsHelperInterface {

    private static final String TAG = "OemRilCommandsHelper";
    private final Context mContext;
    private final int mPhoneId;
    private HandlerThread mHandlerThread;
    private OemRilBinderManager mOemRilManager;
    private CommandsHelperHandler mHandler;

    public static final int EVENT_GET_VOICE_OPERATION_DONE = 0;

    private class CommandsHelperHandler extends Handler {
        public CommandsHelperHandler(Looper looper) {
            super(looper);
        }

        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
            case EVENT_GET_VOICE_OPERATION_DONE:
                onGetVoiceOperationDone((AsyncResult) msg.obj);
                break;
            default:
                break;
            } // end switch ~
        }

        private void onGetVoiceOperationDone(AsyncResult ar) {
            Message onComplete = (Message) ar.userObj;
            if (onComplete != null) {
                if (ar.exception != null) {
                    AsyncResult.forMessage(onComplete, null, ar.exception);
                }
                else {
                    DataReader dr = new DataReader((byte[]) ar.result);
                    try {
                        boolean result = dr.getInt() == 0 ? false: true;
                        AsyncResult.forMessage(onComplete, result, null);
                    } catch (IOException e) {
                        Log.w(TAG, "onGetPreferredCallCapabilityDone: ", e);
                    }
                }
                onComplete.sendToTarget();
            }
        }
    }

    public static OemCommandsHelper from(Context context, Integer phoneId) {
        return new OemCommandsHelper(context, phoneId);
    }

    OemCommandsHelper(Context context, Integer phoneId) {
        mContext = context;
        mPhoneId = (phoneId == null) ? 0 : phoneId;
        mOemRilManager = OemRilBinderManager.from(context);

        mHandlerThread = new HandlerThread(OemCommandsHelper.class.getSimpleName() + mPhoneId);
        mHandlerThread.start();
        mHandler = new CommandsHelperHandler(mHandlerThread.getLooper());
    }

    @Override
    public void setVoiceOperation(boolean enable, Message result) {
        try {
            DataWriter dr = new DataWriter();
            dr.writeInt((enable ? 1 : 0));
            mOemRilManager.invokeOemRilRequestRaw(
                    OemRilConstants.RILC_REQ_SET_VOICE_OPERATION,
                    dr.toByteArray(), result, mPhoneId);
        } catch (IOException e) {
            Log.w(TAG, "setVoiceOperation: ", e);
        }
    }

    @Override
    public void getVoiceOperation(Message result) {
        Message msg = mHandler.obtainMessage(EVENT_GET_VOICE_OPERATION_DONE, result);
        mOemRilManager.invokeOemRilRequestRaw(
                OemRilConstants.RILC_REQ_GET_VOICE_OPERATION,
                null, msg, mPhoneId);
    }

    @Override
    public void setMobileDataState(boolean mobileData, boolean roamingData, Message result) {
        try {
            DataWriter dr = new DataWriter();
            dr.writeInt((mobileData ? 1 : 0));
            dr.writeInt((roamingData ? 1 : 0));
            mOemRilManager.invokeOemRilRequestRaw(
                    OemRilConstants.RILC_REQ_SET_MOBILE_DATA_STATE,
                    dr.toByteArray(), result, mPhoneId);
        } catch (IOException e) {
            Log.w(TAG, "setMobileDataState: ", e);
        }
    }
}
