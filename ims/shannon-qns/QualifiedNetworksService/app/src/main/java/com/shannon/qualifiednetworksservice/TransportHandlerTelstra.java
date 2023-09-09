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

import android.telephony.DataFailCause;
import android.telephony.data.ApnSetting;

import com.shannon.qualifiednetworksservice.Transport.TransportType;
import com.shannon.qualifiednetworksservice.util.LogUtils;

public class TransportHandlerTelstra extends VendorTransportHandler {
    private static final int[] TIMEOUT_LIST =
            {0, (45 * 1000), (90 * 1000), (180 * 1000), (360 * 1000), (720 * 1000), (1440 * 1000)};

    private final int mApnType;

    public TransportHandlerTelstra(int apnType) {
        mApnType = apnType;
        TAG = ShannonQualifiedNetworksService.getServiceTAG("-VTH-TLRA");
    }

    private boolean processError(Transport transport, TransportState state, int cause, int timeout) {
        if ((mApnType == ApnSetting.TYPE_IMS) ||
            (transport.getType() == TransportType.TRANSPORT_WIFI)) {
            if (timeout > 0) {
                return false;
            }
            int count = state.getRetryCount();
            if (count > 6) count = 6;
            state.blockTemporary(TIMEOUT_LIST[count]);
            LogUtils.w(TAG, "Block " + transport.getType() + ". APN = " + ApnSetting.getApnTypeString(mApnType) +
                    " for " + TIMEOUT_LIST[count] + ". Fail cause: " + DataFailCause.toString(cause));
            return true;
        } else if ((mApnType == ApnSetting.TYPE_XCAP) && (cause == DataFailCause.SERVICE_OPTION_NOT_SUBSCRIBED)) {
            LogUtils.w(TAG, "Block " + transport.getType() + " for XCAP permanently. Fail cause: " + DataFailCause.toString(cause));
            state.block();
            return true;
        }
        return false;
    }

    @Override
    public boolean onConnectionFailure(Transport transport, int cause, int timeout) {
        TransportState initialState = transport.getInitialState();
        return processError(transport, initialState, cause, timeout);
    }

    @Override
    public boolean onHandoverFailure(Transport transport, int cause, int timeout) {
        TransportState handoverState = transport.getHandoverState();
        return processError(transport, handoverState, cause, timeout);
    }
}
