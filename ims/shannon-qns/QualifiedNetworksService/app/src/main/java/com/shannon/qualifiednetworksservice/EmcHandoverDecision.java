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
import android.telephony.data.ApnSetting;
import android.text.TextUtils;

import com.shannon.qualifiednetworksservice.HandoverConditions.ConditionEvents;
import com.shannon.qualifiednetworksservice.util.LogUtils;

public class EmcHandoverDecision extends HandoverDecision {
    protected final ConditionCall mCall;

    EmcHandoverDecision(HandoverConditions conditions) {
        super(ApnSetting.TYPE_EMERGENCY, conditions);

        int slotIndex = mConditions.getSlotIndex();
        TAG = ShannonQualifiedNetworksService.getServiceTAG("-" + slotIndex + "-emc-ho");

        mCall = mConditions.getCallConditions();
    }

    @Override
    public PreferredList getPreferredList(ConditionEvents event) {
        int current = getCurrentNetworkType();
        boolean isVoMobileUsed = isVoMobileUsed();

        int preferred = preCheckEvent(current, event);
        if (preferred == TelephonyManager.NETWORK_TYPE_UNKNOWN) {
            boolean isEmcOverWiFiAllowed = mConfigurationManager.isEmcOverWiFiSupported();
            boolean isEmcRedialOverWiFi = mCall.isCallFailEmcRedialToWiFi();
            if (isEmcOverWiFiAllowed) {
                if (isValidType(current)) {
                    preferred = current;

                    /* handover supported between 4G/5G and IWLAN only */
                    if (mCell.isLTE()) {
                        /* check handover conditions */
                        if (current == TelephonyManager.NETWORK_TYPE_IWLAN) {
                            if (isEmcOverCellSupported() && isWiFiOutCondition()) {
                                preferred = mCell.getNetworkType();
                            }
                        } else {
                            if (isEmcOverWiFiSupported(true) && isWiFiInCondition()) {
                                preferred = TelephonyManager.NETWORK_TYPE_IWLAN;
                            }
                        }
                    }
                    if (isHandover(current, preferred) &&
                            isHandoverNotAllowedByRat(current, preferred)) {
                        LogUtils.i(TAG, mApnType,
                                "Handover not supported from " + TelephonyManager.getNetworkTypeName(current) +
                                        " to " + TelephonyManager.getNetworkTypeName(preferred));
                        /* keep previous selection */
                        preferred = current;
                    }
                } else {
                    if (isEmcRedialOverWiFi && isEmcOverWiFiSupported(false)) {
                        preferred = TelephonyManager.NETWORK_TYPE_IWLAN;
                    }
                    if (preferred == TelephonyManager.NETWORK_TYPE_UNKNOWN) {
                        if (isEmcOverCellSupported()) {
                            preferred = mCell.getNetworkType();
                        }
                    }
                }
            } else {
                if (isEmcOverCellSupported()) {
                    preferred = mCell.getNetworkType();
                }
            }

            boolean isHandover = isHandover(current, TelephonyManager.NETWORK_TYPE_IWLAN);
            if ((isEmcRedialOverWiFi || isValidType(current)) &&
                    isEmcOverWiFiAllowed && isEmcOverWiFiEnabled(isHandover)) {
                if (isHandover && isHandoverNotAllowedByRat(current, TelephonyManager.NETWORK_TYPE_IWLAN)) {
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

    private boolean isEmcOverCellSupported() {
        return (mCell.isConnected() &&
               (mCell.isEmergencySupported() || mCall.isCallFailEmcRedialToIms()) &&
                isCellTransportAvailable());
    }

    private boolean isEmcOverWiFiEnabled(boolean isHandover) {
        boolean enabled;
        if (isHandover) {
            enabled = mConfigurationManager.canIgnoreWfcSettingForEmcHandover() ?
                    mUser.isWfcEnabledByPlatform() : mUser.isWfcEnabled();
        } else {
            enabled = mConfigurationManager.canIgnoreWfcSettingForEmc() ?
                    mUser.isWfcEnabledByPlatform() : mUser.isWfcEnabled();
        }
        if (mConfigurationManager.isEmcOemProvisioningRequired()) {
            enabled = (enabled && mConditions.getProvisioningConditions().isOemProvisioned());
        }
        if ((mCell.isRoaming() && mCell.isRoamedInternationally()) || mWiFi.isRoaming()) {
            enabled = (enabled && mConfigurationManager.isEmcOverWiFiSupportedRoaming());
        }
        return enabled && isWiFiTransportAvailable();
    }

    private boolean isEmcOverWiFiSupported(boolean isHandover) {
        return  (mWiFi.isConnected() && isEmcOverWiFiEnabled(isHandover));
    }

    private boolean isWiFiOutCondition() {
        int currentRsrp = mCell.getRsrp();
        int midRsrp = mConfigurationManager.getCellEmcRsrpMid();

        return (((mWiFi.getRssi() < mConfigurationManager.getWiFiEmcRoveOut()) &&
                ((midRsrp != 0) && (currentRsrp > midRsrp))) ||
                (currentRsrp >= mConfigurationManager.getCellEmcRoveIn()));
    }

    private boolean isWiFiInCondition() {
        return ((mWiFi.getRssi() >= mConfigurationManager.getWiFiEmcRoveIn()) &&
                (mCell.getRsrp() < mConfigurationManager.getCellEmcRoveOut()));
    }

    @Override
    protected boolean isServiceSupported(int networkType) {
        if (networkType == TelephonyManager.NETWORK_TYPE_IWLAN) {
            return isEmcOverWiFiEnabled(false) && (mWiFi.isConnected() || isVoMobileSupported());
        } else if (networkType != TelephonyManager.NETWORK_TYPE_UNKNOWN) {
            return isEmcOverCellSupported();
        }
        return false;
    }
}
