/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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

public class MmsHandoverDecision extends HandoverDecision {
    MmsHandoverDecision(HandoverConditions conditions) {
        super(ApnSetting.TYPE_MMS, conditions);

        int slotIndex = mConditions.getSlotIndex();
        TAG = ShannonQualifiedNetworksService.getServiceTAG("-" + slotIndex + "-mms-ho");
    }

    @Override
    public PreferredList getPreferredList(ConditionEvents event) {
        int current = getCurrentNetworkType();
        boolean isVoMobileUsed = isVoMobileUsed();

        int preferred = preCheckEvent(current, event);
        if (preferred == TelephonyManager.NETWORK_TYPE_UNKNOWN) {
            boolean isWiFiSupported = mConfigurationManager.isMmsOverWiFiSupported();
            boolean isWiFiPreferred = mConfigurationManager.isWiFiPreferredForMms();
            LogUtils.d(TAG, mApnType, "Current RAT: " + TelephonyManager.getNetworkTypeName(current) +
                    ", WiFi supported: " + isWiFiSupported + ", preferred: " + isWiFiPreferred);

            if (isWiFiSupported) {
                if (isValidType(current)) {
                    preferred = current;
                } else {
                    if (mUser.isWfcEnabled() && isWiFiPreferred) {
                        if (isMmsOverWiFiPossible()) {
                            preferred = TelephonyManager.NETWORK_TYPE_IWLAN;
                        }
                        if (preferred == TelephonyManager.NETWORK_TYPE_UNKNOWN) {
                            if (isMmsOnCellularPossible()) {
                                preferred = mCell.getNetworkType();
                            }
                        }
                    } else {
                        if (mCell.isAirplaneMode()) {
                            if (isMmsOverWiFiPossible()) {
                                preferred = TelephonyManager.NETWORK_TYPE_IWLAN;
                            }
                        } else {
                            if (isMmsOnCellularPossible()) {
                                preferred = mCell.getNetworkType();
                            }
                            if (preferred == TelephonyManager.NETWORK_TYPE_UNKNOWN) {
                                if (isMmsOverWiFiPossible()) {
                                    preferred = TelephonyManager.NETWORK_TYPE_IWLAN;
                                }
                            }
                        }
                    }
                }
            } else {
                if (isMmsOnCellularPossible()) {
                    preferred = mCell.getNetworkType();
                }
            }

            if (isWiFiSupported && isMmsOverWiFiPossible()) {
                if (isHandover(current, TelephonyManager.NETWORK_TYPE_IWLAN)) {
                    LogUtils.v(TAG, mApnType, "Handover to VoMobile is not allowed");
                } else {
                    preferred = canUseVoMobile(preferred, isVoMobileUsed);
                }
            }
            postCheckDecision(current, preferred);
        }

        PreferredList preferredList = new PreferredList();
        if (preferred != TelephonyManager.NETWORK_TYPE_UNKNOWN) {
            addToList(preferredList, preferred);
        }
        setLastReportedNetwork(preferred);
        return preferredList;
    }

    private boolean isMmsOverWiFiPossible() {
        return (isImsPdnOnWiFi() && isWiFiTransportAvailable());
    }

    private boolean isMmsOnCellularPossible() {
        boolean isAllowedOnRat = mCell.isAllowedRat(mConfigurationManager.getMmsAllowedRatList());
        boolean isCellRegistered = mCell.isRegistered();
        boolean isCellTransportAvailable = isCellTransportAvailable();

        LogUtils.d(TAG, mApnType,
                "allowed on cellular RAT: " + isAllowedOnRat + ", cellular registered: " + isCellRegistered
                + " cellular transport available: " + isCellTransportAvailable);
        return (isCellRegistered && isAllowedOnRat && isCellTransportAvailable);
    }

    @Override
    protected boolean isServiceSupported(int networkType) {
        if (networkType == TelephonyManager.NETWORK_TYPE_IWLAN) {
            return isMmsOverWiFiPossible();
        } else if (networkType != TelephonyManager.NETWORK_TYPE_UNKNOWN) {
            return mCell.isRegistered();
        }
        return false;
    }
}
