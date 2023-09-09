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

public class ImsHandoverDecisionVzwUs extends ImsHandoverDecision {
    private int mRtpRetryCount;

    public ImsHandoverDecisionVzwUs(HandoverConditions conditions) {
        super(conditions);

        int slotIndex = mConditions.getSlotIndex();
        TAG = ShannonQualifiedNetworksService.getServiceTAG("-" + slotIndex + "-ims-ho-vzw");

        mBlockHandoverByHysteresisOnCall = false;
        mRtpRetryCount = 0;
    }

    @Override
    protected int getPreferredNetwork(int current) {
        int preferred = TelephonyManager.NETWORK_TYPE_UNKNOWN;

        boolean isRoaming = mCell.isRoaming();
        boolean isCellReady = !mCell.isAirplaneMode() && mCell.isRegistered() && isImsOnCell();
        boolean isWiFiReady = mWiFi.isConnected() && isImsOnWiFi();
        boolean isWiFiPreferred = (mUser.isWfcEnabledByUser() && mUser.isWiFiPreferred());
        boolean isImsCallActive = isImsCallActive();
        if (!isImsCallActive) {
            mRtpRetryCount = 0;
        }
        /* if VoPS is 0 or 100% SSAC Barring (VZ_REQ_VOWIFI_6247024), check WiFi state */
        if (isCellReady && isVoiceNotSupportedOrBarred() && isWiFiReady) {
            LogUtils.d(TAG, mApnType, "Set VoLTE as false (VoPS=0 and VoWiFi=true)");
            isCellReady = false;
        }
        LogUtils.d(TAG, mApnType,
                "VoWiFi=" + isWiFiReady + " preferred=" + isWiFiPreferred + " VoLTE=" + isCellReady + " 4G button=" + mUser.isVolteEnabled());
        if (isRoaming) {
            if (isWiFiReady && (isWiFiPreferred || !isCellReady) && isWiFiInCondition(current, isCellReady, isRoaming)) {
                preferred = TelephonyManager.NETWORK_TYPE_IWLAN;
            } else if (isCellReady) {
                preferred = mCell.getNetworkType();
            }
        } else if (isCellReady && isCellInCondition(current, isWiFiReady, isRoaming)) {
            preferred = mCell.getNetworkType();
        } else if (isWiFiReady) {
            preferred = TelephonyManager.NETWORK_TYPE_IWLAN;
        }
        return preferred;
    }

    @Override
    protected boolean isImsOnWiFi() {
        boolean isCellReady = mCell.isRegistered() && isImsOnCell();
        boolean enabled = (mUser.isWfcEnabledByPlatform() && mProvision.isWfcProvisioned());
        if(mConfigurationManager.isWfcOemProvisioningRequired()) {
            enabled = (enabled && mProvision.isMdnProvisioned());
        }
        if (isCellReady) {
            enabled = (enabled && (mUser.isWfcEnabledByUser() ||
                    (mCell.isRoaming() && isVoiceNotSupportedOrBarred()) ||
                    (mCall.isVideoCallActive() && !mCall.isAudioOnlyCallActive())));
        }
        if (isCellRoamedInternationally() || mWiFi.isRoaming()) {
            enabled = (enabled && mConfigurationManager.isWfcRoamingSupported());
        }
        return enabled && isWiFiTransportAvailable();
    }

    private boolean isCellInCondition(int current, boolean isWiFiReady, boolean isRoaming) {
        if (!isWiFiReady) {
            return true;
        }
        boolean isImsCallActive = isImsCallActive();
        if (isRoaming && !isImsCallActive &&
                (!mUser.isWfcEnabledByUser() || !mUser.isWiFiPreferred())) {
            // cellular preferred
            return true;
        }
        if (!isValidCellularType(current)) {
            boolean isVideoCallActive = mCall.isVideoCallActive();
            return (!isImsCallActive && (!isRoaming && !isValidType(current))) ||
                    (!isVideoCallActive && (mCell.isRoveIn() ||
                            ((isRtpEvent(isImsCallActive) || mWiFi.isRoveOut()) && !mCell.isRoveOutMid()))) ||
                    (isVideoCallActive && mWiFi.isLowerThanThreshold(getWiFiVideoOut()));

        } else {
            return !isWiFiInCondition(current, true, isRoaming);
        }
    }

    private boolean isWiFiInCondition(int current, boolean isCellReady, boolean isRoaming) {
        if (!isCellReady) {
            return true;
        }
        if (isRoaming && !isImsCallActive() &&
                (mUser.isWfcEnabledByUser() && mUser.isWiFiPreferred())) {
            // WiFi preferred
            return true;
        }
        if (current != TelephonyManager.NETWORK_TYPE_IWLAN) {
            boolean isVideoCallActive = mCall.isVideoCallActive();
            return (isWiFiHandoverAllowed(current) &&
                    ((!isVideoCallActive && mWiFi.isRoveIn() && mCell.isRoveOut()) ||
                     (isVideoCallActive && mWiFi.isBetterThanThreshold(getWiFiVideoIn()))));
        } else {
            return !isCellInCondition(current, true, isRoaming);
        }
    }

    private int getWiFiVideoIn() { return mWiFi.getRssiRoveOut() + 10; }
    private int getWiFiVideoOut() { return mWiFi.getRssiRoveOut() + 5; }

    private boolean isRtpEvent(boolean isImsCallActive) {
        boolean event = false;
        if (isImsCallActive) {
            event = isRtpEvent(mLastEvent);
            if (event) {
                mRtpRetryCount++;
            }
        }
        return event;
    }

    private boolean isWiFiHandoverAllowed(int current) {
        boolean result;
        if (current == TelephonyManager.NETWORK_TYPE_UNKNOWN) {
            result = true;
        } else {
            result = (mRtpRetryCount <= 3);
        }
        LogUtils.v(TAG, mApnType, "Handover to WiFi " + (result ? "allowed" : "disallowed"));
        return result;
    }

    private boolean isVoiceNotSupportedOrBarred() {
        return (!mCell.isVoiceSupported() || mCell.isVoiceBarred());
    }

    @Override
    protected int getHysteresisTimerToLte() {
        boolean isCellularPreferred = !mUser.isWfcEnabledByUser() || !mUser.isWiFiPreferred();
        if (mCell.isRoaming() && isCellularPreferred) {
            return mConfigurationManager.getImsHysteresisTimerToLte();
        }
        return 0;
    }

    @Override
    protected int getHysteresisTimerToWiFi() {
        boolean isWiFiPreferred = mUser.isWfcEnabledByUser() && mUser.isWiFiPreferred();
        if (mCell.isRoaming() && isWiFiPreferred) {
            return mConfigurationManager.getImsHysteresisTimerToWiFi();
        }
        return 0;
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
            boolean isCellReady = mCell.isRegistered() && isImsOnCell();
            boolean isRoaming = mCell.isRoaming();
            boolean canDisableCellIn = false;
            boolean canDisableCellOut = false;
            if (!isCellReady || (isRoaming && !isImsCallActive)) {
                canDisableCellIn = true;
                canDisableCellOut = true;
            } else if (isRoaming && isWiFiPreferred) {
                if (!isWiFiType(current)) {
                    canDisableCellIn = true;
                } else {
                    canDisableCellOut = true;
                }
            } else {
                if (!isValidCellularType(current)) {
                    canDisableCellOut = true;
                } else {
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
