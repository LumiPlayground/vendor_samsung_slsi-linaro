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

import com.shannon.qualifiednetworksservice.util.LogUtils;

public class ImsHandoverDecisionTmoUs extends ImsHandoverDecision {
    ImsHandoverDecisionTmoUs(HandoverConditions conditions) {
        super(conditions);

        int slotIndex = mConditions.getSlotIndex();
        TAG = ShannonQualifiedNetworksService.getServiceTAG("-" + slotIndex + "-ims-ho-tmo");
    }

    @Override
    protected int getPreferredNetwork(int current) {
        int preferred = TelephonyManager.NETWORK_TYPE_UNKNOWN;

        boolean isWfcEnabled = isImsOnWiFi();
        boolean isCellRegistered = mCell.isRegistered();
        boolean isCellReady = isCellRegistered && isImsOnCell();
        if (mCell.isAirplaneMode()) {
            LogUtils.d(TAG, mApnType, "Airplane mode on and WiFiCalling: " + isWfcEnabled);
            if (isWfcEnabled && mWiFi.isConnected()) {
                preferred = TelephonyManager.NETWORK_TYPE_IWLAN;
            }
        } else if (isWfcEnabled) {
            boolean isWiFiPreferred = mUser.isWiFiPreferred();
            boolean isImsCall = isImsCallActive();
            boolean isWiFiConnected = mWiFi.isConnected();
            boolean isWiFiRoveOut = mWiFi.isRoveOut();
            boolean isDisconnectedFromNetwork = mCell.isDisconnectedFromNetwork();
            boolean isRtpEvent = isRtpEvent(mLastEvent);
            LogUtils.d(TAG, mApnType, "WiFiCalling on, preferred " + isWiFiPreferred);
            if (isWiFiPreferred) {
                if (current == TelephonyManager.NETWORK_TYPE_IWLAN) {
                    if (isCellReady && (!isWiFiConnected || isDisconnectedFromNetwork ||
                            (!mCell.isRoveOut() && (isWiFiRoveOut || isRtpEvent)))) {
                        preferred = mCell.getNetworkType();
                    } else if (isWiFiConnected && !isDisconnectedFromNetwork) {
                        if (!isWiFiRoveOut || isImsCall || !isCellRegistered || mCell.isRoveOut()) {
                            preferred = TelephonyManager.NETWORK_TYPE_IWLAN;
                        }
                    }
                } else {
                    if (isWiFiConnected && (mWiFi.isRoveIn() || (!isCellReady && !isWiFiRoveOut))) {
                        preferred = TelephonyManager.NETWORK_TYPE_IWLAN;
                    } else if (isCellReady) {
                        preferred = mCell.getNetworkType();
                    }
                }
            } else {
                if (current == TelephonyManager.NETWORK_TYPE_IWLAN) {
                    if (isCellRegistered && mCell.isRoveIn()) {
                        if (isCellReady) {
                            preferred = mCell.getNetworkType();
                        } else if (isImsCall) {
                            preferred = TelephonyManager.NETWORK_TYPE_IWLAN;
                        }
                    } else if (isCellReady && (!isWiFiConnected || isDisconnectedFromNetwork ||
                            ((isWiFiRoveOut || isRtpEvent) && !mCell.isRoveOut()))) {
                        preferred = mCell.getNetworkType();
                    } else if (isWiFiConnected && !isDisconnectedFromNetwork) {
                        if (!isWiFiRoveOut || isImsCall || !isCellRegistered || mCell.isRoveOut()) {
                            preferred = TelephonyManager.NETWORK_TYPE_IWLAN;
                        }
                    }
                } else {
                    if (isWiFiConnected && (!isCellRegistered || (mCell.isRoveOut() && mWiFi.isRoveIn()))) {
                        preferred = TelephonyManager.NETWORK_TYPE_IWLAN;
                    } else if (isCellReady) {
                        preferred = mCell.getNetworkType();
                    }
                }
            }
        } else {
            LogUtils.d(TAG, mApnType, "WiFiCalling off");
            if (isCellReady) {
                preferred = mCell.getNetworkType();
            }
        }
        return preferred;
    }

    @Override
    protected void requestOrStopCellReports(int current, boolean checkHandover) {
        if (mUser.isWfcEnabled() && mWiFi.isConnected()) {
            boolean isHysteresis = false;
            if (checkHandover) {
                isHysteresis = ((mState == mHandoverState) && (mTimer != null));
            }
            boolean isWiFiPreferred = mUser.isWiFiPreferred();
            boolean isImsCallActive = isImsCallActive();
            boolean canDisableCellIn = false;
            boolean canDisableCellOut = false;
            if (isWiFiPreferred) {
                canDisableCellIn = true;
                if (!isWiFiType(current) || (!isImsCallActive && !isWiFiOut(isImsCallActive))) {
                    canDisableCellOut = true;
                }
            } else {
                if (isValidCellularType(current)) {
                    canDisableCellIn = true;
                } else if (isWiFiType(current)) {
                    if (!isImsCallActive && !isWiFiOut(isImsCallActive)) {
                        canDisableCellOut = true;
                    }
                }
            }
            mCell.requestCellReports(isWiFiPreferred, isImsCallActive, isHysteresis,
                    canDisableCellIn, canDisableCellOut);
        } else {
            mCell.stopCellReports();
        }
    }
}
