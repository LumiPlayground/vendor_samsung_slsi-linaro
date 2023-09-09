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

import android.telephony.SubscriptionInfo;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
import android.text.TextUtils;

public class SimInformation {
    private final int mSlotId;
    private final int mMcc;
    private final int mMnc;
    private String mImsi;
    private final int mSubId;
    private final TelephonyManager mTelephonyManager;
    private final SimRecords mSimRecords;

    public static SimInformation createSimInformation(int slotId, SubscriptionManager sm, TelephonyManager tm) {
        int[] subIds = sm.getSubscriptionIds(slotId);
        int subId = (subIds != null && subIds.length > 0) ? subIds[0] : -1;
        if (subId < 0) {
            return null;
        }
        try {
            SubscriptionInfo info = sm.getActiveSubscriptionInfo(subId);
            if (info != null) {
                int mcc = info.getMcc();
                int mnc = info.getMnc();
                if ((mcc > 0) && (mnc >= 0)) {
                    return new SimInformation(slotId, mcc, mnc, subId, tm);
                }
            }
        } catch(Exception e) {
            // nothing
        }
        return null;
    }

    private SimInformation(int slotId, int mcc, int mnc, int subId, TelephonyManager tm) {
        mSlotId = slotId;
        mMcc = mcc;
        mMnc = mnc;
        mSubId = subId;

        mTelephonyManager = tm.createForSubscriptionId(subId);
        mSimRecords = new SimRecords(mSlotId, mTelephonyManager);
        try {
            mImsi = mTelephonyManager.getSubscriberId();
        } catch (Exception e) {
            mImsi = null;
        }
    }

    public int getMcc() { return mMcc; }
    public int getMnc() { return mMnc; }
    public int getSubId() { return mSubId; }
    public TelephonyManager getTelephonyManager() { return mTelephonyManager; }
    public SimRecords getRecords() { return mSimRecords; }
    public boolean isIsimCard() { return
            mTelephonyManager.isApplicationOnUicc(TelephonyManager.APPTYPE_ISIM); }

    public String getImsi() {
        if (TextUtils.isEmpty(mImsi)) {
            try {
                mImsi = mTelephonyManager.getSubscriberId();
            }
            catch (Exception e) {
                // nothing
            }
        }
        return mImsi;
    }

    @Override
    public String toString() {
        return "SIM information for slot " + mSlotId + " subId=" + mSubId + ", MCC=" + mMcc + " MNC=" + mMnc + " IMSI:" + mImsi;
    }
}
