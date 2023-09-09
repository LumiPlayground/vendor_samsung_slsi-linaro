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

import android.telephony.Rlog;

public class IkeNotifyBackoffTimer {
    private static final int DEACTIVATED = -1;
    private static final int INVALID = -2;

    private static final boolean DBG = true;

    private final String TAG;
    private final int mTimerValue;

    public IkeNotifyBackoffTimer(IkeNotifyData from) {
        TAG = "IKE_BACKOFF_TIMER";

        /* skip first byte, it is length */
        mTimerValue = getBackoffTimerValue(StringUtils.getByteAsInt(from.getData(), 1));
    }

    public IkeNotifyBackoffTimer() {
        TAG = "";
        mTimerValue = INVALID;
    }

    public int getTimerValue() {
        /* return value in msecs */
        if (mTimerValue > 0) {
            return (mTimerValue * 1000);
        }
        return 0;
    }

    public boolean isDeactivatedOrInvalid() {
        return (mTimerValue == INVALID || mTimerValue == DEACTIVATED);
    }

    /**
     *       -1 = timer deactivated, block retry
     *       0 = use implementation specific algorithm
     *       other value - timer in seconds
     * */
    private int getBackoffTimerValue(int backoffTimerIE) {
        /* 3GPP TS 24.008 10.5.7.4a GPRS timer 3 */
        int value = backoffTimerIE & 0x1F;
        int cfg = (backoffTimerIE & 0xE0) >> 5;
        if (cfg == 7) { // 1 1 1
            log("Backoff timer deactivated");
            return DEACTIVATED;
        }
        if (value == 0) {
            log("Backoff timer 0 or not provided");
            return 0;
        }
        switch (cfg) {
            case 0:
                log("10 min * " + value);
                value *= (10 * 60);
                break;

            case 1:
            case 6:
                log("1 hour * " + value);
                value *= (60 * 60);
                break;

            case 2:
                log("10 hours * " + value);
                value *= (60 * 60 * 10);
                break;

            case 3:
                log("2 sec * " + value);
                value *= 2;
                break;

            case 4:
                log("30 sec * " + value);
                value *= 30;
                break;

            case 5:
                log("1 min * " + value);
                value *= 60;
                break;

            default:
                /* should not be here */
                loge("Backoff timer does not have valid IE");
                return INVALID;
        }
        log("Calculated backoff timer in sec " + value);
        return value;
    }

    private void log(String s) {
        if (DBG) {
            Rlog.d(TAG, s);
        }
    }

    protected void loge(String s) {
        Rlog.e(TAG, s);
    }
}
