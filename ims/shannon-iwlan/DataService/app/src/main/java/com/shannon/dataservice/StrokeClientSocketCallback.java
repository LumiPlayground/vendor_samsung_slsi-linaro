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

import com.shannon.dataservice.StrokeRequestMessage.RequestMessageType;

public class StrokeClientSocketCallback {
    protected boolean DBG = true;
    private final RequestMessageType mType;

    private final String TAG;

    StrokeClientSocketCallback(RequestMessageType type) {
        TAG = "" + type + "-callback";

        mType = type;
    }

    public void onResponse(byte[] bytes) {
        log("Successful response for " + mType);
    }

    public void onFailure() {
        log("Failure response for " + mType);
    }

    protected void log(String s) {
        if (DBG) {
            Rlog.d(TAG, s);
        }
    }

    protected void loge(String s) {
        Rlog.e(TAG, s);
    }
}
