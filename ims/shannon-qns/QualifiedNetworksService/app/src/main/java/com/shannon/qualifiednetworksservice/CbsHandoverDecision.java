/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
import android.telephony.data.ApnSetting;

import com.shannon.qualifiednetworksservice.HandoverConditions.ConditionEvents;
import com.shannon.qualifiednetworksservice.util.LogUtils;

public class CbsHandoverDecision extends HandoverDecision {
    public CbsHandoverDecision(HandoverConditions conditions) {
        super(ApnSetting.TYPE_CBS, conditions);

        int slotIndex = mConditions.getSlotIndex();
        TAG = ShannonQualifiedNetworksService.getServiceTAG("-" + slotIndex + "-cbs-ho");
    }

    @Override
    public PreferredList getPreferredList(ConditionEvents event) {
        int current = getCurrentNetworkType();

        int preferred = preCheckEvent(current, event);
        if (preferred == TelephonyManager.NETWORK_TYPE_UNKNOWN) {
            boolean isOverWiFiSupported = mConfigurationManager.isCbsOverWiFiSupported();
            LogUtils.d(TAG, mApnType, "Current RAT: " + TelephonyManager.getNetworkTypeName(current) +
                    " , WiFi supported: " + isOverWiFiSupported);

            /* handover is not supported */
            if (isValidType(current)) {
                preferred = current;
            } else {
                /* cellular is always preferred */
                if (!mCell.isAirplaneMode() && isCbsOnCellularPossible()) {
                    preferred = mCell.getNetworkType();
                }
                if (preferred == TelephonyManager.NETWORK_TYPE_UNKNOWN) {
                    if (isCbsOverWiFiPossible(isOverWiFiSupported)) {
                        preferred = TelephonyManager.NETWORK_TYPE_IWLAN;
                    }
                }
            }
            /* VoMobile is not supported */
            postCheckDecision(current, preferred);
        }

        PreferredList preferredList = new PreferredList();
        if (preferred != TelephonyManager.NETWORK_TYPE_UNKNOWN) {
            addToList(preferredList, preferred);
        }
        setLastReportedNetwork(preferred);
        return preferredList;
    }

    private boolean isCbsOverWiFiPossible(boolean isSupported) {
        return (isSupported && mWiFi.isConnected() && isWiFiTransportAvailable());
    }

    private  boolean isCbsOnCellularPossible() {
        return (mCell.isRegistered() && isCellTransportAvailable());
    }

    @Override
    protected boolean isServiceSupported(int networkType) {
        if (networkType == TelephonyManager.NETWORK_TYPE_IWLAN) {
            return mWiFi.isConnected();
        } else if (networkType != TelephonyManager.NETWORK_TYPE_UNKNOWN) {
            return mCell.isRegistered();
        }
        return false;
    }
}
