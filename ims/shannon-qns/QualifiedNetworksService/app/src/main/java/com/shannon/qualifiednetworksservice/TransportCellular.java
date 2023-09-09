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

import android.content.Context;
import android.telephony.TelephonyManager;

import java.util.Timer;

public class TransportCellular extends Transport {
    private final HandoverConditions.IConditionHelper mConditionsHelper;
    private boolean mRoaming;
    private int mNetworkType;

    public TransportCellular(Context context, int subId,
                             ConfigurationManager configurationManager,
                             HandoverConditions.IConditionHelper conditionHelper,
                             TransportState.IStateChanged callback,
                             IVendorTransportHandler vendorHandler) {
        super(TransportType.TRANSPORT_CELLULAR, context, subId, configurationManager,
                callback, vendorHandler);

        mConditionsHelper = conditionHelper;
        mRoaming = false;
        mNetworkType = TelephonyManager.NETWORK_TYPE_UNKNOWN;
    }

    @Override
    public int getNetworkType() { return mNetworkType; }

    @Override
    public boolean isRoaming() { return mRoaming; }

    @Override
    public void onConnected() {
        mRoaming = mConditionsHelper.isRoaming();
        mNetworkType = mConditionsHelper.getCellularNetworkType();
        super.onConnected();
    }

    @Override
    public void onHandoverCompleted() {
        mRoaming = mConditionsHelper.isRoaming();
        mNetworkType = mConditionsHelper.getCellularNetworkType();
        super.onHandoverCompleted();
    }

    @Override
    public void onRatChanged() {
        mNetworkType = mConditionsHelper.getCellularNetworkType();
        super.onRatChanged();
    }

    @Override
    public void onOperatorChanged() {
        mRoaming = mConditionsHelper.isRoaming();
        super.onOperatorChanged();
    }
}
