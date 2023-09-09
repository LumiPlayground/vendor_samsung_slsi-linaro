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

import com.shannon.qualifiednetworksservice.Transport.TransportType;
import com.shannon.qualifiednetworksservice.util.LogUtils;

public class TransportHandlerEE extends VendorTransportHandler {
    private static final int[] TIMEOUT_LIST =
            {0, (10 * 1000), (30 * 1000), (60 * 1000), (120 * 1000), (300 * 1000)};

    public TransportHandlerEE() {
        TAG = ShannonQualifiedNetworksService.getServiceTAG("-VTH-EE");
    }

    private boolean process(Transport transport, TransportState state, int cause, int timeout) {
        if (transport.getType() == TransportType.TRANSPORT_WIFI) {
            if (timeout > 0) {
                return false;
            }
            int count = state.getRetryCount();
            if (count > 5) count = 5;
            state.blockTemporary(TIMEOUT_LIST[count]);
            LogUtils.w(TAG, "Block " + transport.getType() + " for " + TIMEOUT_LIST[count]
                    + ". Fail cause: " + DataFailCause.toString(cause));
            return true;
        }
        return false;
    }

    @Override
    public boolean onConnectionFailure(Transport transport, int cause, int timeout) {
        TransportState initialState = transport.getInitialState();
        return process(transport, initialState, cause, timeout);
    }

    @Override
    public boolean onHandoverFailure(Transport transport, int cause, int timeout) {
        TransportState handoverState = transport.getHandoverState();
        return process(transport, handoverState, cause, timeout);
    }
}
