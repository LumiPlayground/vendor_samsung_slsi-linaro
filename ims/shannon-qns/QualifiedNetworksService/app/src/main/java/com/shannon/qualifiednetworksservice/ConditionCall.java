/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or
 * distributed, transmitted, transcribed, stored in a retrieval system or
 * translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed to third parties
 * without the express written permission of Samsung Electronics.
 */
package com.shannon.qualifiednetworksservice;

import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;
import android.os.Message;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
import android.telephony.data.ApnSetting;
import android.util.SparseArray;

import com.android.internal.annotations.VisibleForTesting;
import com.shannon.qualifiednetworksservice.RilUnsolicitedLastCallFailCause.LastCallFailCause;

import com.android.internal.telephony.TelephonyIntents;
import com.shannon.qualifiednetworksservice.util.LogUtils;

import static com.shannon.qualifiednetworksservice.HandoverConditions.ConditionEvents.*;

public class ConditionCall {
    protected static class CallType {
        private boolean mAudio;
        private boolean mVideo;
        private final boolean mEmergency;
        private boolean mThresholdsConfigured;

        public CallType(RilUnsolicitedCallStatus callStatus) {
            mAudio = callStatus.isAudioCall();
            mVideo = callStatus.isVideoCall();
            mEmergency = callStatus.isEmergencyCall();
            mThresholdsConfigured = false;
        }

        public CallType(RilUnsolicitedCallRing callRing) {
            mAudio = callRing.isAudioCall();
            mVideo = callRing.isVideoCall();
            mEmergency = callRing.isEmergencyCall();
            mThresholdsConfigured = false;
        }

        public void setAudio(boolean value) {
            mAudio = value;
        }

        public void setVideo(boolean value) {
            mVideo = value;
        }

        public boolean isThresholdsConfiguredSetAfter() {
            boolean value = mThresholdsConfigured;
            mThresholdsConfigured = true;
            return value;
        }

        public boolean isAudioCall() { return mAudio; }
        public boolean isVideoCall() { return mVideo; }
        public boolean isEmergencyCall() { return mEmergency; }
    }

    protected class CallStateBroadcastReceiver extends BroadcastReceiver {
        @Override
        public void onReceive(Context context, Intent intent) {
            final String action = intent.getAction();
            if (action == null) {
                LogUtils.w(TAG, "NULL action for intent " + intent);
                return;
            }
            LogUtils.i(TAG, LogUtils.LOG_CONDITIONS, "Got intent " + intent);
            switch (action) {
                case TelephonyIntents.ACTION_EMERGENCY_CALLBACK_MODE_CHANGED: {
                    int slotId = intent.getIntExtra(SubscriptionManager.EXTRA_SLOT_INDEX, -1);
                    if (slotId != mSlotId) {
                        return;
                    }
                    boolean isInEcm = intent.getBooleanExtra(TelephonyManager.EXTRA_PHONE_IN_ECM_STATE, false);
                    if (isInEcm) {
                        LogUtils.i(TAG, LogUtils.LOG_CONDITIONS, "Entered to emergency callback mode");
                    } else {
                        clearLastCallFailCause();
                        mChangedCallback.onConditionChanged(ApnSetting.TYPE_EMERGENCY, EVENT_EMERGENCY_CALLBACK_END);
                    }
                    break;
                }
            }
        }
    }

    private final String TAG;
    private final int mSlotId;
    private final Context mContext;
    private final RilOemInterface mRilInterface;
    private final HandoverConditions.IConditionChanged mChangedCallback;
    private final HandoverConditions.IConditionHelper mConditionsHelper;
    private final Looper mLooper;
    private final Handler mHandler;
    private final HandlerThread mHandlerThread;
    private final ConfigurationManager mConfigurationManager;
    private final CallStateBroadcastReceiver mBroadcastReceiver;
    private final SparseArray<CallType> mCallList;
    private LastCallFailCause mLastCallFailCause;

    public ConditionCall(int slotIndex, Context context, RilOemInterface oemInterface,
            ConfigurationManager configurationManager, HandoverConditions.IConditionHelper helper,
            HandoverConditions.IConditionChanged callback) {
        mSlotId = slotIndex;
        mContext = context;
        mRilInterface = oemInterface;
        mConfigurationManager = configurationManager;
        mConditionsHelper = helper;
        mChangedCallback = callback;
        mBroadcastReceiver = new CallStateBroadcastReceiver();

        TAG = ShannonQualifiedNetworksService.getServiceTAG("-" + mSlotId + "-HC-call");

        mCallList = new SparseArray<>();
        clearLastCallFailCause();

        mHandlerThread = new HandlerThread(ConditionCall.class.getSimpleName());
        mHandlerThread.start();
        mLooper = mHandlerThread.getLooper();
        mHandler = new Handler(mLooper) {
            @Override
            public void handleMessage(Message message) {
                if (message.what == RilOemInterface.RIL_UNSOLICITED_MESSAGE) {
                    RilUnsolicitedMessage msg = (RilUnsolicitedMessage)message.obj;
                    switch(msg.getMessageId()) {
                        case RilUnsolicitedMessage.RIL_UNSOL_CALL_RING: {
                            RilUnsolicitedCallRing callRing = new RilUnsolicitedCallRing(msg);
                            LogUtils.d(TAG, LogUtils.LOG_CONDITIONS, "Received " + callRing);
                            if (callRing.isValidCall()) {
                                int callId = callRing.getCallId();
                                LogUtils.i(TAG, "Call " + callId + " is ringing");
                                mCallList.put(callId, new CallType(callRing));
                                mChangedCallback.onConditionChanged(
                                        ApnType.getApnBitmaskForCall(), EVENT_IMS_CALL_STARTED);
                            }
                            break;
                        }
                        case RilUnsolicitedMessage.RIL_UNSOL_CALL_STATUS: {
                            RilUnsolicitedCallStatus callStatus = new RilUnsolicitedCallStatus(msg);
                            LogUtils.d(TAG, LogUtils.LOG_CONDITIONS, "Received " + callStatus);
                            switch (callStatus.getCallState()) {
                                case CALL_STATE_DIALING: {
                                    int callId = callStatus.getCallId();
                                    LogUtils.i(TAG, "Call " + callId + " is dialing");
                                    mCallList.put(callId, new CallType(callStatus));
                                    mChangedCallback.onConditionChanged(
                                            ApnType.getApnBitmaskForCall(), EVENT_IMS_CALL_STARTED);
                                    break;
                                }
                                case CALL_STATE_ACTIVE: {
                                    int callId = callStatus.getCallId();
                                    LogUtils.i(TAG, "Call " + callId + " is active");
                                    CallType current = mCallList.get(callId);
                                    if (current == null) {
                                        // unusual case
                                        current = new CallType(callStatus);
                                        mCallList.put(callId, current);

                                        mChangedCallback.onConditionChanged(
                                                ApnType.getApnBitmaskForCall(), EVENT_IMS_CALL_STARTED);
                                    } else {
                                        // update as user may select different options
                                        current.setAudio(callStatus.isAudioCall());
                                        current.setVideo(callStatus.isVideoCall());
                                    }
                                    if (mConditionsHelper.isWfcEnabledByPlatform() &&
                                            !current.isThresholdsConfiguredSetAfter()) {
                                        setRtpThresholds(callId);
                                    }
                                    break;
                                }
                                case CALL_STATE_RELEASED: {
                                    int callId = callStatus.getCallId();
                                    LogUtils.i(TAG, "Call " + callId + " is released");
                                    if (callStatus.isEmergencyCall() && !callStatus.isECBM()) {
                                        clearLastCallFailCause();
                                    }
                                    mCallList.delete(callId);
                                    mChangedCallback.onConditionChanged(
                                            ApnType.getApnBitmaskForCall(), EVENT_IMS_CALL_RELEASED);
                                    break;
                                }
                                default:
                                    LogUtils.i(TAG, "Call " + callStatus.getCallId() +
                                            " state is " + callStatus.getCallState());
                            }
                            break;
                        }
                        case RilUnsolicitedMessage.RIL_UNSOL_CALL_MODIFIED: {
                            RilUnsolicitedCallModified modified = new RilUnsolicitedCallModified(msg);
                            LogUtils.d(TAG, LogUtils.LOG_CONDITIONS, "Received " + modified);
                            CallType current = mCallList.get(modified.getCallId());
                            if (current == null) {
                                LogUtils.d(TAG, LogUtils.LOG_CONDITIONS,
                                    "Received CALL_MODIFIED before call is active");
                                break;
                            }
                            boolean currentVideoState = current.isVideoCall();
                            if (currentVideoState != modified.isVideoCall()) {
                                if (currentVideoState) {
                                    // downgrade
                                    if (!mConfigurationManager.canIgnoreCallDowngrade()) {
                                        current.setVideo(false);
                                        mChangedCallback.onConditionChanged(
                                                ApnType.getApnBitmaskForCall(), EVENT_IMS_CALL_DOWNGRADED);
                                    }
                                } else {
                                    // upgrade
                                    if (!mConfigurationManager.canIgnoreCallUpgrade()) {
                                        current.setVideo(true);
                                        mChangedCallback.onConditionChanged(
                                                ApnType.getApnBitmaskForCall(), EVENT_IMS_CALL_UPGRADED);
                                    }
                                }
                            }
                            break;
                        }
                        case RilUnsolicitedMessage.RIL_UNSOL_SRVCC: {
                            RilUnsolicitedSrvcc indSrvcc = new RilUnsolicitedSrvcc(msg);
                            LogUtils.d(TAG, LogUtils.LOG_CONDITIONS, "Received " + indSrvcc);
                            switch (indSrvcc.getStatus()) {
                                case STATUS_COMPLETED: {
                                    /* how to handle for multiple calls? */
                                    if (mCallList.size() == 1) {
                                        mCallList.clear();
                                        mChangedCallback.onConditionChanged(
                                                ApnType.getApnBitmaskForCall(), EVENT_IMS_CALL_SRVCC);
                                    }
                                    break;
                                }
                                default:;
                            }
                            break;
                        }
                        case RilUnsolicitedMessage.RIL_UNSOL_RTP_EVENT: {
                            RilUnsolicitedRtpEvent rtpEvent = new RilUnsolicitedRtpEvent(msg);
                            LogUtils.d(TAG, LogUtils.LOG_CONDITIONS, "Received " + rtpEvent);
                            if (rtpEvent.getEventType() != RilUnsolicitedRtpEvent.RtpEvent.EVENT_UNKNOWN) {
                                mChangedCallback.onConditionChanged(
                                        ApnType.getApnBitmaskForCall(), EVENT_IMS_CALL_RTP_EVENT);
                            }
                            break;
                        }
                        case RilUnsolicitedMessage.RIL_UNSOL_LAST_CALL_FAIL_CAUSE: {
                            RilUnsolicitedLastCallFailCause failCause = new RilUnsolicitedLastCallFailCause(msg);
                            LogUtils.d(TAG, LogUtils.LOG_CONDITIONS, "Received " + failCause);
                            mLastCallFailCause = failCause.getLastCallFailCause();
                            mChangedCallback.onConditionChanged(ApnSetting.TYPE_EMERGENCY, EVENT_CALL_LAST_FAIL_CAUSE);
                            break;
                        }
                        default:;
                    }
                }
            }
        };

        mRilInterface.registerUnsolicitedListener(RilUnsolicitedMessage.RIL_UNSOL_CALL_RING, mHandler);
        mRilInterface.registerUnsolicitedListener(RilUnsolicitedMessage.RIL_UNSOL_CALL_STATUS, mHandler);
        mRilInterface.registerUnsolicitedListener(RilUnsolicitedMessage.RIL_UNSOL_CALL_MODIFIED, mHandler);
        mRilInterface.registerUnsolicitedListener(RilUnsolicitedMessage.RIL_UNSOL_SRVCC, mHandler);
        mRilInterface.registerUnsolicitedListener(RilUnsolicitedMessage.RIL_UNSOL_RTP_EVENT, mHandler);
        mRilInterface.registerUnsolicitedListener(RilUnsolicitedMessage.RIL_UNSOL_LAST_CALL_FAIL_CAUSE, mHandler);

        IntentFilter filter = new IntentFilter();
        filter.addAction(TelephonyIntents.ACTION_EMERGENCY_CALLBACK_MODE_CHANGED);
        mContext.registerReceiver(mBroadcastReceiver, filter);
    }

    public void destroy() {
        mContext.unregisterReceiver(mBroadcastReceiver);
        mRilInterface.unregisterUnsolicitedListener(RilUnsolicitedMessage.RIL_UNSOL_CALL_RING);
        mRilInterface.unregisterUnsolicitedListener(RilUnsolicitedMessage.RIL_UNSOL_CALL_STATUS);
        mRilInterface.unregisterUnsolicitedListener(RilUnsolicitedMessage.RIL_UNSOL_CALL_MODIFIED);
        mRilInterface.unregisterUnsolicitedListener(RilUnsolicitedMessage.RIL_UNSOL_SRVCC);
        mRilInterface.unregisterUnsolicitedListener(RilUnsolicitedMessage.RIL_UNSOL_RTP_EVENT);
        mRilInterface.unregisterUnsolicitedListener(RilUnsolicitedMessage.RIL_UNSOL_LAST_CALL_FAIL_CAUSE);
        mHandlerThread.quit();
    }

    public boolean isImsCallActive(boolean isEmcExcluded) {
        if (isEmcExcluded) {
            for (int i = 0; i < mCallList.size(); i++) {
                CallType type = mCallList.valueAt(i);
                if (!type.isEmergencyCall()) {
                    return true;
                }
            }
            return false;
        } else {
            return (mCallList.size() > 0);
        }
    }

    public boolean isCallFailEmcRedialToIms() {
        return (mLastCallFailCause == LastCallFailCause.REDIAL_EMERGENCY_TO_CELL);
    }

    public boolean isCallFailEmcRedialToWiFi() {
        return (mLastCallFailCause == LastCallFailCause.REDIAL_EMERGENCY_TO_WIFI);
    }

    public void clearLastCallFailCause() {
        mLastCallFailCause = LastCallFailCause.NONE;
    }

    public void clearLastCallFailCause(int networkType) {
        if (((networkType == TelephonyManager.NETWORK_TYPE_IWLAN) &&
                (mLastCallFailCause == LastCallFailCause.REDIAL_EMERGENCY_TO_WIFI)) ||
              ((networkType != TelephonyManager.NETWORK_TYPE_UNKNOWN) &&
               (networkType != TelephonyManager.NETWORK_TYPE_IWLAN) &&
                (mLastCallFailCause == LastCallFailCause.REDIAL_EMERGENCY_TO_CELL))) {
            LogUtils.d(TAG, "Clear last call fail cause");
            mLastCallFailCause = LastCallFailCause.NONE;
        }
    }

    public boolean isVideoCallActive() {
        for (int i = 0; i < mCallList.size(); i++) {
            CallType type = mCallList.valueAt(i);
            if (type.isVideoCall()) {
                return true;
            }
        }
        return false;
    }

    // audio only calls
    public boolean isAudioOnlyCallActive() {
        for (int i = 0; i < mCallList.size(); i++) {
            CallType type = mCallList.valueAt(i);
            if (type.isAudioCall() && !type.isVideoCall() && !type.isEmergencyCall()) {
                return true;
            }
        }
        return false;
    }

    private void setRtpThresholds(int callId) {
        RilSetRtpThreshold msg = new RilSetRtpThreshold(mRilInterface, callId,
                mConfigurationManager.getRtpThreshold(),
                mConfigurationManager.getRtpInterval(),
                mConfigurationManager.getRtpJitter(),
                mConfigurationManager.getNoRtpInterval());
        LogUtils.d(TAG, LogUtils.LOG_CONDITIONS, "Send " + msg);
        msg.request();
    }

    @Override
    public String toString() {
        return ("CALL" + mSlotId + ": calls=" + mCallList.size() + " last fail cause=" + mLastCallFailCause);
    }

    @VisibleForTesting
    SparseArray<CallType> getCallList() { return mCallList; }

    @VisibleForTesting
    void setLastFailCause(LastCallFailCause cause) { mLastCallFailCause = cause; }

    @VisibleForTesting
    CallStateBroadcastReceiver getBroadcastReceiver() { return mBroadcastReceiver; }

    @VisibleForTesting
    Handler getHandler() { return mHandler; }
}
