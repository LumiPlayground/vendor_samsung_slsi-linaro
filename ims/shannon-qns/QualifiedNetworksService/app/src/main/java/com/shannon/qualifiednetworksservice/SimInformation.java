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
package com.shannon.qualifiednetworksservice;

import android.telephony.SubscriptionInfo;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;

public class SimInformation {
    private final int mSlotId;
    private final int mMcc;
    private final int mMnc;
    private final int mSubId;
    private final TelephonyManager mTelephonyManager;

    public static SimInformation createSimInformation(int slotId, SubscriptionManager sm, TelephonyManager tm) {
        try {
            SubscriptionInfo info = sm.getActiveSubscriptionInfoForSimSlotIndex(slotId);
            if (info != null) {
                int subId = info.getSubscriptionId();
                int mcc = info.getMcc();
                int mnc = info.getMnc();
                if ((mcc > 0) && (mnc >= 0)) {
                    return new SimInformation(slotId, mcc, mnc, subId, tm);
                }
            }
        }
        catch(Exception e) {
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
    }

    public int getMcc() { return mMcc; }
    public int getMnc() { return mMnc; }
    public int getSubId() { return mSubId; }
    public TelephonyManager getTelephonyManager() { return mTelephonyManager; }

    @Override
    public String toString() {
        return "SIM information for slot " + mSlotId + " subId=" + mSubId + ", MCC=" + mMcc + " MNC=" + mMnc;
    }
}
