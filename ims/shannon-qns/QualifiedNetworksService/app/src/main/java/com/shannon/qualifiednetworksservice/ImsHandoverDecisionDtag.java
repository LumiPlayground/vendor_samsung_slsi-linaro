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

import com.shannon.qualifiednetworksservice.util.LogUtils;

import java.lang.reflect.Field;

import static android.telephony.ims.ImsMmTelManager.WIFI_MODE_CELLULAR_PREFERRED;
import static android.telephony.ims.ImsMmTelManager.WIFI_MODE_WIFI_PREFERRED;
import static com.shannon.qualifiednetworksservice.ConditionUserSetting.WIFI_MODE_IMS_PREFERRED;

public class ImsHandoverDecisionDtag extends ImsHandoverDecision {
    public ImsHandoverDecisionDtag(HandoverConditions conditions) {
        super(conditions);

        int slotIndex = mConditions.getSlotIndex();
        TAG = ShannonQualifiedNetworksService.getServiceTAG("-" + slotIndex + "-ims-ho-dtag");
    }

    protected boolean isImsPreferredNotDefined() {
        try {
            Class c = Class.forName("android.telephony.ims.ImsMmTelManager");
            Field f = c.getField("WIFI_MODE_IMS_PREFERRED");
            return ((int)f.get(null) != WIFI_MODE_IMS_PREFERRED);
        } catch (Exception e) {
            return true;
        }
    }

    @Override
    protected int getPreferredNetwork(int current) {
        int preferred = TelephonyManager.NETWORK_TYPE_UNKNOWN;

        boolean isWfcEnabled = isImsOnWiFi();
        boolean isWiFiConnected = mWiFi.isConnected();
        boolean isCellReady = mCell.isRegistered() && isImsOnCell();
        if (mCell.isAirplaneMode()) {
            LogUtils.d(TAG, mApnType, "Airplane mode on and WiFiCalling: " + isWfcEnabled);
            if (isWfcEnabled && isWiFiConnected) {
                preferred = TelephonyManager.NETWORK_TYPE_IWLAN;
            }
        } else if (isWfcEnabled) {
            int wfcMode = mUser.getWfcModeEnum();
            boolean isImsCallActive = isImsCallActive();
            boolean isWiFiIn = isWiFiIn(isImsCallActive, wfcMode);
            boolean isWiFiOut = isWiFiOut(isImsCallActive, wfcMode);
            boolean isCellIn = isCellIn(isImsCallActive, wfcMode);
            boolean isCellOut = isCellOut(isImsCallActive, wfcMode);
            boolean isRtpEvent = isRtpEvent(mLastEvent);

            /* IMS_PREFERRED is not defined, modify CELLULAR_PREFERRED and use IMS_PREFERRED */
            if (isImsPreferredNotDefined() && (wfcMode == WIFI_MODE_CELLULAR_PREFERRED)) {
                wfcMode = WIFI_MODE_IMS_PREFERRED;
            }

            LogUtils.d(TAG, mApnType, "WiFiCalling on");
            switch (wfcMode) {
                case WIFI_MODE_CELLULAR_PREFERRED: {
                    LogUtils.v(TAG, mApnType, "Wfc = CELLULAR_PREFERRED");
                    if (isValidCellularType(current)) {
                        if (isWiFiConnected && isWiFiIn && isCellOut) {
                            preferred = TelephonyManager.NETWORK_TYPE_IWLAN;
                        } else if (isCellReady) {
                            preferred = current;
                        }
                    } else if (isWiFiType(current)) {
                        if (mCell.isRegistered() && isCellIn && (isWiFiOut || isRtpEvent)) {
                            if (isImsOnCell()) {
                                preferred = mCell.getNetworkType();
                            } else if (isImsCallActive && isWiFiConnected) {
                                preferred = TelephonyManager.NETWORK_TYPE_IWLAN;
                            }
                        } else if (isWiFiConnected) {
                            preferred = TelephonyManager.NETWORK_TYPE_IWLAN;
                        }
                    } else {
                        if (mCell.isRegistered() && isCellIn) {
                            if (isImsOnCell()) {
                                preferred = mCell.getNetworkType();
                            }
                        } else if (isWiFiConnected && isWiFiIn && isCellOut) {
                            preferred = TelephonyManager.NETWORK_TYPE_IWLAN;
                        } else if (isCellReady) {
                            preferred = mCell.getNetworkType();
                        }
                    }
                    break;
                }
                case WIFI_MODE_WIFI_PREFERRED: {
                    LogUtils.v(TAG, mApnType, "Wfc = WIFI_PREFERRED");
                    if (isWiFiType(current)) {
                        if (isCellReady && isCellIn && (isWiFiOut || isRtpEvent || !isWiFiConnected)) {
                            preferred = mCell.getNetworkType();
                        } else if (isWiFiConnected) {
                            preferred = TelephonyManager.NETWORK_TYPE_IWLAN;
                        }
                    } else if (isValidCellularType(current)) {
                        if (isWiFiConnected && (!isImsCallActive || isWiFiIn)) {
                            preferred = TelephonyManager.NETWORK_TYPE_IWLAN;
                        } else if (isCellReady) {
                            preferred = current;
                        }
                    } else {
                        if (isWiFiConnected) {
                            preferred = TelephonyManager.NETWORK_TYPE_IWLAN;
                        } else if (isCellReady) {
                            preferred = mCell.getNetworkType();
                        }
                    }
                    break;
                }
                case WIFI_MODE_IMS_PREFERRED: {
                    LogUtils.v(TAG, mApnType, "Wfc = IMS_PREFERRED");
                    if (isValidCellularType(current)) {
                        if (isWiFiConnected && ((isWiFiIn && isCellOut) || !isCellReady)) {
                            preferred = TelephonyManager.NETWORK_TYPE_IWLAN;
                        } else if (isCellReady) {
                            preferred = current;
                        }
                    } else if (isWiFiType(current)) {
                        if (isCellReady && ((isCellIn && (isWiFiOut || isRtpEvent)) || !isWiFiConnected)) {
                            preferred = mCell.getNetworkType();
                        } else if (isWiFiConnected) {
                            preferred = TelephonyManager.NETWORK_TYPE_IWLAN;
                        }
                    } else {
                        if (isCellReady && (isCellIn || !isWiFiConnected || !isWiFiIn || !isCellOut)) {
                            preferred = mCell.getNetworkType();
                        } else if (isWiFiConnected) {
                            preferred = TelephonyManager.NETWORK_TYPE_IWLAN;
                        }
                    }
                    break;
                }
                default:;
            }
        } else {
            LogUtils.d(TAG, mApnType, "WiFiCalling off");
            if (isCellReady) {
                preferred = mCell.getNetworkType();
            }
        }
        return preferred;
    }

    private boolean isWiFiIn(boolean callActive, int mode) {
        if (callActive) {
            return mWiFi.isHandIn(mode);
        }
        return mWiFi.isRoveIn(mode);
    }

    private boolean isWiFiOut(boolean callActive, int mode) {
        if (callActive) {
            return mWiFi.isHandOut(mode);
        }
        /*
         *   no rove-out values defined
         *
         *   cellular preferred - always true
         *   wifi preferred - always false
         */
        return (mode != WIFI_MODE_WIFI_PREFERRED);
    }

    private boolean isCellIn(boolean callActive, int mode) {
        if (callActive) {
            return mCell.isHandIn(mode);
        }
        return mCell.isRoveIn(mode);
    }

    private boolean isCellOut(boolean callActive, int mode) {
        if (callActive) {
            return mCell.isHandOut(mode);
        }
        return mCell.isRoveOut(mode);
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
                if (isWiFiType(current)) {
                    canDisableCellOut = true;
                    if (!isImsCallActive && !isWiFiOut(isImsCallActive, WIFI_MODE_WIFI_PREFERRED)) {
                        canDisableCellIn = true;
                    }
                } else {
                    canDisableCellIn = true;
                    canDisableCellOut = true;
                }
            } else {
                if (isValidCellularType(current)) {
                    canDisableCellIn = true;
                    if (!isWiFiIn(isImsCallActive, WIFI_MODE_CELLULAR_PREFERRED)) {
                        canDisableCellOut = true;
                    }
                } else if (isWiFiType(current)) {
                    canDisableCellOut = true;
                    if (!isImsCallActive && !isWiFiOut(isImsCallActive, WIFI_MODE_CELLULAR_PREFERRED)) {
                        canDisableCellIn = true;
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
