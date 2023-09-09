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

public class XcapHandoverDecision extends HandoverDecision {
    XcapHandoverDecision(HandoverConditions conditions) {
        super(ApnSetting.TYPE_XCAP, conditions);

        int slotIndex = mConditions.getSlotIndex();
        TAG = ShannonQualifiedNetworksService.getServiceTAG("-" + slotIndex + "-xcap-ho");
    }

    @Override
    public PreferredList getPreferredList(ConditionEvents event) {
        int current = getCurrentNetworkType();
        boolean isVoMobileUsed = isVoMobileUsed();

        int preferred = preCheckEvent(current, event);
        if (preferred == TelephonyManager.NETWORK_TYPE_UNKNOWN) {
            boolean isWiFiPreferred = mConfigurationManager.isWiFiPreferredForXcap();
            boolean isAllowedOnRat = mCell.isAllowedRat(mConfigurationManager.getXcapAllowedRatList());
            boolean isCellRegistered = mCell.isRegistered();
            boolean isCellTransportAvailable = isCellTransportAvailable();
            LogUtils.d(TAG, mApnType, "Current RAT: " + TelephonyManager.getNetworkTypeName(current) +
                    ", WiFi preferred: " + isWiFiPreferred + ", allowed on cellular RAT: " + isAllowedOnRat +
                    ", cellular registered: " + isCellRegistered + ", cell transport available: " + isCellTransportAvailable);

            boolean isXcapOverWiFiPossible = isXcapOverWiFiPossible();
            boolean isXcapOnCellularPossible = isCellRegistered && isAllowedOnRat && isCellTransportAvailable;

            if (isValidType(current)) {
                if (isWiFiPreferred) {
                    if (isWiFiType(current)) {
                        if (isXcapOnCellularPossible && (!isXcapOverWiFiPossible || mWiFi.isRoveOut())) {
                            preferred = mCell.getNetworkType();
                        } else if (isXcapOverWiFiPossible) {
                            preferred = TelephonyManager.NETWORK_TYPE_IWLAN;
                        }
                    } else {
                        if (isXcapOverWiFiPossible && (!isXcapOnCellularPossible || mWiFi.isRoveIn())) {
                            preferred = TelephonyManager.NETWORK_TYPE_IWLAN;
                        } else if (isXcapOnCellularPossible) {
                            preferred = current;
                        }
                    }
                } else {
                    if (isValidCellularType(current)) {
                        if (isXcapOverWiFiPossible &&
                                (!isXcapOnCellularPossible || (mCell.isRoveOut() && mWiFi.isRoveIn()))) {
                            preferred = TelephonyManager.NETWORK_TYPE_IWLAN;
                        } else if (isXcapOnCellularPossible) {
                            preferred = current;
                        }
                    } else {
                        if (isXcapOnCellularPossible &&
                                (!isXcapOverWiFiPossible || mCell.isRoveIn() || mWiFi.isRoveOut())) {
                            preferred = mCell.getNetworkType();
                        } else if (isXcapOverWiFiPossible) {
                            preferred = TelephonyManager.NETWORK_TYPE_IWLAN;
                        }
                    }
                }
                if (isHandoverNotAllowedByRat(current, preferred)) {
                    /* keep current if possible */
                    if ((isValidCellularType(current) && isXcapOnCellularPossible) ||
                            (isWiFiType(current) && isXcapOverWiFiPossible)) {
                        preferred = current;
                    }
                }
            } else {
                if (isWiFiPreferred) {
                    if (isXcapOverWiFiPossible) {
                        preferred = TelephonyManager.NETWORK_TYPE_IWLAN;
                    } else if (isXcapOnCellularPossible) {
                        preferred = mCell.getNetworkType();
                    }
                } else {
                    if (isXcapOnCellularPossible) {
                        preferred = mCell.getNetworkType();
                    } else if (isXcapOverWiFiPossible) {
                        preferred = TelephonyManager.NETWORK_TYPE_IWLAN;
                    }
                }
            }

            if (isXcapOverWiFiAllowed()) {
                if (isHandover(current, TelephonyManager.NETWORK_TYPE_IWLAN) &&
                        isHandoverNotAllowedByRat(current, TelephonyManager.NETWORK_TYPE_IWLAN)) {
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

    private boolean isXcapOverWiFiAllowed() {
        boolean isWiFiAllowed = mConfigurationManager.isXcapOverWiFiSupported() && mUser.isWfcEnabled();
        boolean isWiFiTransportAvailable = isWiFiTransportAvailable();

        LogUtils.d(TAG, mApnType, "WiFi allowed: " + isWiFiAllowed + " WiFi transport available: " + isWiFiTransportAvailable);
        if (isWiFiAllowed && mConfigurationManager.isXcapImsPreconditionForWiFi()) {
            return isImsPdnOnWiFi() && isWiFiTransportAvailable;
        }
        return (isWiFiAllowed && isWiFiTransportAvailable);
    }

    private boolean isXcapOverWiFiPossible() {
        boolean isWifiConnected = mWiFi.isConnected();
        LogUtils.v(TAG, mApnType, "WiFi connected: " + isWifiConnected);
        return isWifiConnected && isXcapOverWiFiAllowed();
    }

    @Override
    protected boolean isServiceSupported(int networkType) {
        if (networkType == TelephonyManager.NETWORK_TYPE_IWLAN) {
            return (mWiFi.isConnected() || isVoMobileSupported()) && isXcapOverWiFiAllowed();
        } else if (networkType != TelephonyManager.NETWORK_TYPE_UNKNOWN) {
            return mCell.isRegistered();
        }
        return false;
    }
}
