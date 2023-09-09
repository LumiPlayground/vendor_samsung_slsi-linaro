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

import com.android.internal.annotations.VisibleForTesting;
import com.shannon.qualifiednetworksservice.HandoverConditions.ConditionEvents;
import com.shannon.qualifiednetworksservice.util.LogUtils;

class ImsHandoverDecisionSwisscom extends ImsHandoverDecision {
    private boolean mHandoverReselection;

    public ImsHandoverDecisionSwisscom(HandoverConditions conditions) {
        super(conditions);

        int slotIndex = mConditions.getSlotIndex();
        TAG = ShannonQualifiedNetworksService.getServiceTAG("-" + slotIndex + "-ims-ho-swc");

        mHandoverReselection = false;
    }

    @Override
    protected int getPreferredNetwork(int current) {
        int preferred = TelephonyManager.NETWORK_TYPE_UNKNOWN;

        boolean isWfcEnabled = isImsOnWiFi();
        boolean isCellReady = mCell.isRegistered() && isImsOnCell();
        if (mCell.isAirplaneMode()) {
            LogUtils.d(TAG, mApnType, "Airplane mode on and WiFiCalling: " + isWfcEnabled);
            if (isWfcEnabled && mWiFi.isConnected()) {
                preferred = TelephonyManager.NETWORK_TYPE_IWLAN;
            }
        } else if (!isValidCellularType(current) && !isCellReady && !mCell.isCsRegistered()) {
            LogUtils.d(TAG, mApnType, "Cellular is not available and WiFiCalling: " + isWfcEnabled);
            if (isWfcEnabled && mWiFi.isConnected()) {
                preferred = TelephonyManager.NETWORK_TYPE_IWLAN;
            }
        } else if (isWfcEnabled) {
            boolean isImsCallActive = isImsCallActive();
            boolean isWiFiConnected = mWiFi.isConnected();

            LogUtils.d(TAG, mApnType, "WiFiCalling on");
            if (isValidCellularType(current)) {
                if (isWiFiConnected && isWiFiIn(isImsCallActive) && isCellOut(isImsCallActive)) {
                    preferred = TelephonyManager.NETWORK_TYPE_IWLAN;
                } else if (isCellReady) {
                    preferred = mCell.getNetworkType();
                }
            } else if (isWiFiType(current)) {
                if (mCell.isRegistered() && isCellIn(isImsCallActive) &&
                        ((mTimer == null) || isWiFiOut(isImsCallActive))) {
                    if (isImsOnCell()) {
                        preferred = mCell.getNetworkType();
                    } else if (isWiFiConnected && isImsCallActive) {
                        preferred = TelephonyManager.NETWORK_TYPE_IWLAN;
                    }
                } else if (isWiFiConnected) {
                    preferred = TelephonyManager.NETWORK_TYPE_IWLAN;
                }
            } else {
                if (mCell.isRegistered() && (isCellIn(isImsCallActive) || !isWiFiConnected)) {
                    if (isImsOnCell()) {
                        preferred = mCell.getNetworkType();
                    }
                } else if (isWiFiConnected && isWiFiIn(isImsCallActive) && isCellOut(isImsCallActive)) {
                    preferred = TelephonyManager.NETWORK_TYPE_IWLAN;
                } else if (isCellReady) {
                    preferred = mCell.getNetworkType();
                }
            }
        } else {
            LogUtils.d(TAG, mApnType, "WiFiCalling off");
            if (isCellReady) {
                preferred = mCell.getNetworkType();
            }
        }
        mHandoverReselection = false;
        return preferred;
    }

    @Override
    protected boolean isWiFiIn(boolean callActive) {
        if (callActive) {
            return mWiFi.isHandIn(mCell.isRoaming(), mTimer != null);
        }
        return mWiFi.isRoveIn(mCell.isRoaming(), mTimer != null);
    }

    @Override
    protected boolean isWiFiOut(boolean callActive) {
        if (callActive) {
            return (mWiFi.isHandOut(mTimer != null) || isRtpEvent(mLastEvent));
        }
        return mWiFi.isRoveOut(mTimer != null);
    }

    @Override
    protected boolean isCellIn(boolean callActive) {
        if (callActive) {
            return mCell.isHandIn(mTimer != null);
        }
        return mCell.isRoveIn(mTimer != null);
    }

    @Override
    protected boolean isCellOut(boolean callActive) {
        if (callActive) {
            return mCell.isHandOut(mTimer != null);
        }
        return mCell.isRoveOut(mTimer != null);
    }

    @Override
    protected boolean isCellTransportAvailable() {
        if ((mState != mIdleState) && !mHandoverReselection) {
            return true;
        }
        return mTransport.isTransportAvailable(mApnType, Transport.TransportType.TRANSPORT_CELLULAR);
    }

    @Override
    protected boolean isWiFiTransportAvailable() {
        if ((mState != mIdleState) && !mHandoverReselection) {
            return true;
        }
        return mTransport.isTransportAvailable(mApnType, Transport.TransportType.TRANSPORT_WIFI);
    }

    @Override
    protected int processHandoverState(int current, ConditionEvents event) {
        int result = super.processHandoverState(current, event);
        /* re-select at handover state */
        if (isValidType(result) && (result == current)) {
            mHandoverReselection = true;

            int preferred = getPreferredNetwork(current);
            if (preferred != result) {
                result = preferred;
                setNewState(mIdleState);

                if (isHandover(current, preferred)) {
                    mLastPreferredNetwork = preferred;
                    setNewState(mHandoverState);
                }
            }
        }
        return result;
    }

    @VisibleForTesting
    protected void setHandoverReselection(boolean handoverReselection) {
        mHandoverReselection = handoverReselection;
    }

    @Override
    protected void requestOrStopCellReports(int current, boolean checkHandover) {
        if (mUser.isWfcEnabled() && mWiFi.isConnected()) {
            boolean isHysteresis = false;
            if (checkHandover) {
                isHysteresis = ((mState == mHandoverState) && (mTimer != null));
            }
            boolean canDisableCellIn = false;
            boolean canDisableCellOut = false;
            if (isValidCellularType(current)) {
                canDisableCellIn = true;
            } else if (isWiFiType(current)) {
                canDisableCellOut = true;
            }
            mCell.requestCellReports(mUser.isWiFiPreferred(), isImsCallActive(), isHysteresis,
                    canDisableCellIn, canDisableCellOut);
        } else {
            mCell.stopCellReports();
        }
    }
}
