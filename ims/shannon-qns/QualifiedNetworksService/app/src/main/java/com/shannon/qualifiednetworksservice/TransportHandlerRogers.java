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

public class TransportHandlerRogers extends VendorTransportHandler {
    public TransportHandlerRogers() {
        TAG = ShannonQualifiedNetworksService.getServiceTAG("-VTH-rgs");
    }

    @Override
    public boolean onOperatorChanged(Transport transport) {
        if (transport.getType() == TransportType.TRANSPORT_CELLULAR) {
            transport.getInitialState().reset();
            transport.getHandoverState().reset();
        }
        return false;
    }

    @Override
    public boolean onWfcStateChanged(Transport transport) {
        if (transport.getType() == TransportType.TRANSPORT_WIFI) {
            transport.getInitialState().reset();
            transport.getHandoverState().reset();
        }
        return false;
    }

    @Override
    public boolean onConnectionFailure(Transport transport, int cause, int timeout) {
        if (timeout > 0) {
            return false;
        }
        if ((transport.getType() == TransportType.TRANSPORT_CELLULAR) &&
                (cause == DataFailCause.INSUFFICIENT_RESOURCES)) {
            transport.getInitialState().block();
            return true;
        }
        return false;
    }

    @Override
    public boolean onHandoverFailure(Transport transport, int cause, int timeout) {
        if (timeout > 0) {
            return false;
        }
        if ((transport.getType() == TransportType.TRANSPORT_CELLULAR) &&
                (cause == DataFailCause.INSUFFICIENT_RESOURCES)) {
            transport.getHandoverState().block();
            return true;
        }
        return false;
    }
}
