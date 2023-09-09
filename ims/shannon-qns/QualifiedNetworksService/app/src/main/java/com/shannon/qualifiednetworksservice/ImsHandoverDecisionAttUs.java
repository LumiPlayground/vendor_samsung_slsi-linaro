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

public class ImsHandoverDecisionAttUs extends ImsHandoverDecision {
    private boolean mIsCallOffloaded;
    private boolean mBlockHandoverToIwlanOnCall;

    ImsHandoverDecisionAttUs(HandoverConditions conditions) {
        super(conditions);

        int slotIndex = mConditions.getSlotIndex();
        TAG = ShannonQualifiedNetworksService.getServiceTAG("-" + slotIndex + "-ims-ho-att");
        mBlockHandoverByHysteresisOnCall = false;
        mBlockHandoverToIwlanOnCall = false;
        mIsCallOffloaded = false;
    }

    @Override
    protected int getPreferredNetwork(int current) {
        int preferred = TelephonyManager.NETWORK_TYPE_UNKNOWN;

        boolean isWfcEnabled = isImsOnWiFi();
        boolean isCellReady = !mCell.isAirplaneMode() && mCell.isRegistered() && isImsOnCell();
        boolean isWifiRegistered = (current == TelephonyManager.NETWORK_TYPE_IWLAN);
        if (isWfcEnabled) {
            boolean isWiFiConnected = mWiFi.isConnected();
            boolean isWiFiRoveOut = mWiFi.isRoveOut();
            boolean isRtpEvent = isRtpEvent(mLastEvent);
            boolean isWiFiPreferred = mUser.isWiFiPreferred();
            boolean isCallActive = isImsCallActive();
            boolean isAudioCallActive = isCallActive && mCall.isAudioOnlyCallActive();

            LogUtils.d(TAG, mApnType, "WiFiCalling enabled, preferred: " + isWiFiPreferred);
            mBlockHandoverToIwlanOnCall =
                    mBlockHandoverToIwlanOnCall && isAudioCallActive && isCellReady;
            if (mBlockHandoverToIwlanOnCall) {
                LogUtils.v(TAG, mApnType, "Cell ready. Keep call on cell after handover");
                preferred = mCell.getNetworkType();
            } else if (mCell.isRoaming() || isWiFiPreferred) {
                if (isWifiRegistered) {
                    if (isCellReady && (!isWiFiConnected || (isWiFiRoveOut && mCell.isRoveIn()) || isRtpEvent)) {
                        LogUtils.v(TAG, mApnType, "Cell ready and WiFi rove-out. Return cell (R)");
                        preferred = mCell.getNetworkType();
                    } else if (isWiFiConnected) {
                        LogUtils.v(TAG, mApnType, "WiFi good, return wifi (R)");
                        preferred = current;
                    }
                } else {
                    if (isWiFiConnected && mWiFi.isRoveIn()) {
                        LogUtils.v(TAG, mApnType, "WiFi rove-in, return wifi (R)");
                        preferred = TelephonyManager.NETWORK_TYPE_IWLAN;
                    } else if (isCellReady && (!isWiFiConnected || mCell.isRoveIn() ||
                            (isValidCellularType(current) && !mCell.isRoveOut()))) {
                        LogUtils.v(TAG, mApnType, "Cell ready, return cell (R)");
                        preferred = mCell.getNetworkType();
                    } else if (isWiFiConnected && !isWiFiRoveOut) {
                        LogUtils.v(TAG, mApnType, "Cell not ready but WiFi available, return wifi (R)");
                        preferred = TelephonyManager.NETWORK_TYPE_IWLAN;
                    }
                }
            } else {
                boolean isCellRoveOut = mCell.isRoveOut();
                boolean isWiFiRoveIn = mWiFi.isRoveIn();
                if (isWiFiConnected && isWiFiRoveIn && mCall.isVideoCallActive()) {
                    LogUtils.v(TAG, mApnType, "Video call(s) active and WiFi is good. Return wifi.");
                    preferred = TelephonyManager.NETWORK_TYPE_IWLAN;
                    if (isValidCellularType(current)) {
                        LogUtils.v(TAG, mApnType, "Video call offloaded from " +
                                TelephonyManager.getNetworkTypeName(current));
                        mIsCallOffloaded = true;
                    }
                } else if (isCallActive && isWifiRegistered && isWiFiConnected && !isWiFiRoveOut && !isRtpEvent) {
                    LogUtils.v(TAG, mApnType, "Call active and WiFi is good. Return wifi.");
                    preferred = TelephonyManager.NETWORK_TYPE_IWLAN;
                } else if (isWiFiConnected && isCellReady) {
                    LogUtils.d(TAG, mApnType, "Cell and Wifi enabled");
                    if (mCell.isRoveIn() || isWiFiRoveOut || (mIsCallOffloaded && !isCallActive) ||
                            (isRtpEvent && !isCellRoveOut)) {
                        if (isCallActive && mCell.isNotLTE()) {
                            LogUtils.v(TAG, mApnType, "In call and non-LTE, return wifi.");
                            preferred = TelephonyManager.NETWORK_TYPE_IWLAN;
                        } else if (mIsCallOffloaded) {
                            LogUtils.v(TAG, mApnType, "Call was offloaded, back to previous RAT.");
                            preferred = mCell.getNetworkType();
                        } else {
                            LogUtils.v(TAG, mApnType, "Good cell conditions, return cell.");
                            preferred = mCell.getNetworkType();
                        }
                    } else if (isCellRoveOut && isWiFiRoveIn) {
                        LogUtils.v(TAG, mApnType, "Cell rove-out, return wifi.");
                        preferred = TelephonyManager.NETWORK_TYPE_IWLAN;
                    } else if (isValidType(current)) {
                        LogUtils.v(TAG, mApnType, "Keep current RAT:" +
                                TelephonyManager.getNetworkTypeName(current));
                        preferred = current;
                    } else {
                        LogUtils.v(TAG, mApnType, "Return cell.");
                        preferred = mCell.getNetworkType();
                    }
                } else if (isCellReady) {
                    LogUtils.v(TAG, mApnType, "Cell only, return cell.");
                    preferred = mCell.getNetworkType();
                } else if (isWiFiConnected) {
                    LogUtils.v(TAG, mApnType, "WiFi only, return wifi.");
                    preferred = TelephonyManager.NETWORK_TYPE_IWLAN;
                }
                /* reset */
                if (mIsCallOffloaded && (preferred != TelephonyManager.NETWORK_TYPE_IWLAN)) {
                    mIsCallOffloaded = false;
                }
            }
            if (isWifiRegistered && isAudioCallActive && isValidCellularType(preferred) && isRtpEvent) {
                mBlockHandoverToIwlanOnCall = true;
            }
        } else {
            LogUtils.d(TAG, mApnType, "WiFiCalling disabled");
            if (mUser.isWfcEnabledByPlatform() && mCall.isVideoCallActive()) {
                boolean isCellNotConnected = !mCell.isRegistered() ||
                        (mCell.isLowerThanThreshold(ConditionCellular.NOT_USABLE_LTE_RSRP));
                boolean isWiFiConnected = mWiFi.isConnected();
                if (isWiFiConnected &&
                        ((isWifiRegistered && !mWiFi.isRoveOut()) || mWiFi.isRoveIn())) {
                    if (isCellNotConnected) {
                        LogUtils.v(TAG, mApnType, "Video call(s) active, WiFi is good, but cell lost. Drop call.");
                    } else {
                        LogUtils.v(TAG, mApnType, "Video call(s) active and WiFi is good. Return wifi.");
                        preferred = TelephonyManager.NETWORK_TYPE_IWLAN;
                    }
                } else if (isCellReady && mCell.isLTE()) {
                    LogUtils.v(TAG, mApnType, "LTE available for video call, return cell.");
                    preferred = mCell.getNetworkType();
                } else if ((current == TelephonyManager.NETWORK_TYPE_IWLAN) &&
                        isWiFiConnected && !isCellNotConnected) {
                    LogUtils.v(TAG, mApnType, "Video call(s) active and WiFi is available. Return wifi.");
                    preferred = current;
                } else {
                    LogUtils.v(TAG, mApnType, "Video call(s) active, WiFi bad and not LTE. Drop call.");
                }
            } else if (isCellReady) {
                LogUtils.v(TAG, mApnType, "Cell only, return cell.");
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
            if (mCell.isRoaming() || isWiFiPreferred) {
                if (isWiFiType(current)) {
                    canDisableCellOut = true;
                    if (!isWiFiOut(isImsCallActive)) {
                        canDisableCellIn = true;
                    }
                } else if (isWiFiIn(isImsCallActive)) {
                    canDisableCellIn = true;
                    canDisableCellOut = true;
                } else if (isValidCellularType(current)) {
                    canDisableCellIn = true;
                }
            }
            mCell.requestCellReports(isWiFiPreferred, isImsCallActive, isHysteresis,
                    canDisableCellIn, canDisableCellOut);
        } else {
            mCell.stopCellReports();
        }
    }
}
