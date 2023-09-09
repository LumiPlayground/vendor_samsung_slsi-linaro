/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or
 * distributed, transmitted, transcribed, stored in a retrieval system or
 * translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed to third parties
 * without the express written permission of Samsung Electronics.
 */
package com.shannon.qualifiednetworksservice;

public class ImsHandoverDecisionRJio extends ImsHandoverDecision {
    public ImsHandoverDecisionRJio(HandoverConditions conditions) {
        super(conditions);

        int slotIndex = mConditions.getSlotIndex();
        TAG = ShannonQualifiedNetworksService.getServiceTAG("-" + slotIndex + "-ims-ho-jio");
    }

    @Override
    protected int getHysteresisTimerToWiFi() {
        switch (mLastEvent) {
            case EVENT_IMS_CALL_RTP_EVENT:
            case EVENT_DESTINATION_UNREACHABLE:
                return mConfigurationManager.getImsHysteresisTimerToWiFi();
            default:
                /* same as LTE */
                return mConfigurationManager.getImsHysteresisTimerToLte();
        }
    }
}
