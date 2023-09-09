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

import android.telephony.TelephonyManager;

public class ImsHandoverDecisionOrange extends ImsHandoverDecision {
    protected static final int RSSI_IN = -85;
    protected static final int RSSI_OUT = -90;

    public ImsHandoverDecisionOrange(HandoverConditions conditions) {
        super(conditions);

        int slotIndex = mConditions.getSlotIndex();
        TAG = ShannonQualifiedNetworksService.getServiceTAG("-" + slotIndex + "-ims-ho-orange");
    }

    @Override
    protected int getPreferredNoCellular(boolean isWfcEnabled, int current) {
        if (isWfcEnabled && mWiFi.isConnected() &&
                (mWiFi.isBetterThanThreshold(RSSI_IN) ||
                        (isWiFiType(current) && mWiFi.isBetterThanThreshold(RSSI_OUT)))) {
            return TelephonyManager.NETWORK_TYPE_IWLAN;
        }
        return TelephonyManager.NETWORK_TYPE_UNKNOWN;
    }
}
