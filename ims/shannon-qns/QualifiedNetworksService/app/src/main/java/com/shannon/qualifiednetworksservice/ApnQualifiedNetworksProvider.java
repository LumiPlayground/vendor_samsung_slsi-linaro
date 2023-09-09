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
import android.telephony.data.ApnSetting;

import com.android.internal.annotations.VisibleForTesting;
import com.android.internal.util.IndentingPrintWriter;
import com.shannon.qualifiednetworksservice.HandoverConditions.ConditionEvents;
import com.shannon.qualifiednetworksservice.HandoverConditions.EventInfo;
import com.shannon.qualifiednetworksservice.util.LogUtils;

import java.io.FileDescriptor;

public class ApnQualifiedNetworksProvider {
    public class ApnConditionsHandler extends Handler {
        ApnConditionsHandler(Looper looper) {
            super(looper);
        }

        @Override
        public void handleMessage(Message message) {
            if (message.what == HandoverConditions.CONDITIONS_CHANGED) {
                ConditionEvents event = ConditionEvents.EVENT_UNKNOWN;
                if (message.obj != null) {
                    if (message.obj instanceof ConditionEvents) {
                        event = (ConditionEvents)message.obj;
                    } else if (message.obj instanceof EventInfo) {
                        EventInfo info = (EventInfo)message.obj;
                        onConditionsChanged(info.getEvent(), info.getInfo());
                        return;
                    }
                }
                onConditionsChanged(event);
            } else if (message.what == HandoverConditions.CONFIGURATION_CHANGED) {
                onConditionsChanged(ConditionEvents.EVENT_CONFIGURATION_CHANGED);
            }
        }

        public boolean isHandledApnType(int bitmask) {
            return ((bitmask & mType) != 0);
        }
    }

    protected final String TAG;
    private final int mSlotId;
    private final ApnConditionsHandler mHandler;
    private final HandlerThread mHandlerThread;
    private final Handler mMessageHandler;
    private final int mMessageId;
    private final int mType;
    private final HandoverConditions mConditions;
    private final HandoverDecision mDecision;
    private PreferredList mQualifiedNetworks;

    public ApnQualifiedNetworksProvider(int type, HandoverConditions conditions, Handler handler, int what) {
        mType = type;
        mConditions = conditions;
        mMessageHandler = handler;
        mMessageId = what;
        mQualifiedNetworks = new PreferredList();
        mSlotId = mConditions.getSlotIndex();

        TAG = ShannonQualifiedNetworksService.getServiceTAG("-" + mSlotId + "-" + ApnSetting.getApnTypeString(mType));

        mHandlerThread = new HandlerThread(ApnQualifiedNetworksProvider.class.getSimpleName());
        mHandlerThread.start();
        Looper looper = mHandlerThread.getLooper();
        mHandler = new ApnConditionsHandler(looper);

        mDecision = mConditions.getHandoverDecisionForApn(mType);
        mConditions.registerListener(mHandler);
    }

    public void destroy() {
        mQualifiedNetworks.clear();
        updateQualifiedNetworks();
        mConditions.unregisterListener(mHandler);
        mHandlerThread.quitSafely();
    }

    private void onConditionsChanged(ConditionEvents event, Object object) {
        mDecision.setEventInfo(object);
        onConditionsChanged(event);
        mDecision.setEventInfo(null);
    }

    private void onConditionsChanged(ConditionEvents event) {
        LogUtils.i(TAG, mType, "Conditions changed for " + ApnSetting.getApnTypeString(mType) + ". " + event);

        PreferredList preferredList = mDecision.getPreferredList(event);
        if (preferredList.isEmpty() && mQualifiedNetworks.isEmpty()) {
            LogUtils.v(TAG, mType, "preferredList & QualifiedNetworks are empty, nothing to do");
        } else if (!mQualifiedNetworks.equals(preferredList)) {
            LogUtils.d(TAG, mType,
                    "preferredList is changed " + mQualifiedNetworks + "->" + preferredList);
            mDecision.saveToLocalLog("PreferredList is changed: " + mQualifiedNetworks + "->" + preferredList + ". " + event);
            mQualifiedNetworks.clear();
            if (!preferredList.isEmpty()) {
                mQualifiedNetworks.addAll(preferredList);
            }
            updateQualifiedNetworks();
        } else {
            LogUtils.v(TAG, mType, "preferredList is not changed " + mQualifiedNetworks + ", noting to do ");
        }
    }

    private PreferredList getCopiedList() {
        PreferredList list = new PreferredList();
        if (!mQualifiedNetworks.isEmpty()) {
            list.addAll(mQualifiedNetworks);
        }
        return list;
    }

    private void updateQualifiedNetworks() {
        Message msg = Message.obtain(mMessageHandler, mMessageId);
        msg.arg1 = mType;
        msg.obj = getCopiedList();
        msg.sendToTarget();
    }

    public void dump(FileDescriptor fd, IndentingPrintWriter printWriter, String[] args) {
        IndentingPrintWriter pw = new IndentingPrintWriter(printWriter, "  ");
        pw.println("APN : " + ApnSetting.getApnTypeString(mType) +
                ", last = " +  mQualifiedNetworks.toString());
        pw.increaseIndent();
        mDecision.dump(pw);
        pw.decreaseIndent();
        pw.flush();
    }

    @VisibleForTesting
    protected void setQualifiedNetworks(PreferredList qualifiedNetworks) {
        mQualifiedNetworks = qualifiedNetworks;
    }
}
