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

import android.telephony.DataFailCause;
import android.telephony.TelephonyManager;
import android.telephony.data.ApnSetting;
import android.text.TextUtils;

import com.shannon.qualifiednetworksservice.HandoverConditions.ConditionEvents;
import com.shannon.qualifiednetworksservice.util.DeviceUtils;
import com.shannon.qualifiednetworksservice.util.LogUtils;

import java.util.Timer;
import java.util.TimerTask;

public class ImsHandoverDecision extends HandoverDecision {
    protected final ConditionCall mCall;
    protected final ConditionProvisioning mProvision;
    protected boolean mBlockHandoverByHysteresisOnCall;
    protected Timer mImsRegistrationTimer;
    protected ConditionEvents mLastEvent;

    public ImsHandoverDecision(HandoverConditions conditions) {
        super(ApnSetting.TYPE_IMS, conditions);

        mCall = mConditions.getCallConditions();
        mProvision = mConditions.getProvisioningConditions();
        mBlockHandoverByHysteresisOnCall = true;
        mLastEvent = ConditionEvents.EVENT_UNKNOWN;

        int slotIndex = mConditions.getSlotIndex();
        TAG = ShannonQualifiedNetworksService.getServiceTAG("-" + slotIndex + "-ims-ho-df");
    }

    @Override
    public PreferredList getPreferredList(ConditionEvents event) {
        int current = getCurrentNetworkType();
        if (current == TelephonyManager.NETWORK_TYPE_IWLAN) {
            mWiFi.startIpReachableTimer();
        } else {
            mWiFi.stopIpReachableTimer();
        }
        mLastEvent = event;
        boolean isHandover = isInHandoverState();
        boolean isVoMobileUsed = isVoMobileUsed();

        boolean isRtpEventOnVoMobile = false;
        if (isRtpEvent(event)) {
            LogUtils.v(TAG, mApnType, "Trigger WiFi reevaluation");
            mWiFi.requestNetworkValidation();
            isRtpEventOnVoMobile = isVoMobileUsed;
        }

        int preferred = preCheckEvent(current, event);
        if (preferred == TelephonyManager.NETWORK_TYPE_UNKNOWN) {
            preferred = getPreferredNetwork(current);

            boolean isCellRegistered = mCell.isRegistered();
            boolean isImsCallActive = isImsCallActive();
            boolean isWiFiApHandover = mWiFi.isWiFiApHandoverActive();

            if (preferred == TelephonyManager.NETWORK_TYPE_UNKNOWN) {
                LogUtils.d(TAG, mApnType, "No service");
                if (isImsCallActive) {
                    /* keep call */
                    if (isServiceSupported(current) ||
                            (canKeepPdnOnCell(current) && isCellRegistered)) {
                        LogUtils.v(TAG, mApnType, "Call active, keep call");
                        preferred = current;
                    }
                } else if (canKeepPdnOnCell(current)) {
                    if (mCell.isOutOfService()) {
                        /* out-of-service */
                        LogUtils.v(TAG, mApnType, "Out of service");
                        preferred = current;
                    } else if (isCellRegistered) {
                        /* keep registration */
                        LogUtils.v(TAG, mApnType, "Keep current PDN");
                        preferred = current;
                    }
                } else if (isWiFiType(current) && mCell.isAirplaneMode()) {
                    // framework could not handle empty list properly while cellular is still connected
                    // keep current selection until cell released
                    if (isCellRegistered) {
                        LogUtils.v(TAG, mApnType, "Keep current PDN when APM ON");
                        preferred = current;
                    }
                }
            } else {
                if (isWiFiType(preferred) && isWiFiApHandover) {
                    if (isCellRegistered && isImsOnCell()) {
                        int cellularNetworkType = mCell.getNetworkType();
                        LogUtils.i(TAG, mApnType, "VoLTE available. Handover to " +
                                TelephonyManager.getNetworkTypeName(cellularNetworkType) +
                                " needed");
                        preferred = cellularNetworkType;
                    } else if (mCell.isCsRegistered() && !isImsCallActive) {
                        LogUtils.d(TAG, mApnType, "WiFi to WiFi handover in progress with cellular available, reset");
                        preferred = TelephonyManager.NETWORK_TYPE_UNKNOWN;
                        /* stop timer if force disconnect requested */
                        mWiFi.stopWiFiApHandoverTimer();
                    }
                }
            }

            if (isHandover(current, preferred)) {
                LogUtils.d(TAG, mApnType, "Handover from " +
                        TelephonyManager.getNetworkTypeName(current) + " to " +
                        TelephonyManager.getNetworkTypeName(preferred));
                if (isHandoverNotAllowedByRat(current, preferred)) {
                    String log = "Handover is not allowed from " +
                            TelephonyManager.getNetworkTypeName(current) + " to " +
                            TelephonyManager.getNetworkTypeName(preferred) + ". ";
                    if (isImsCallActive) {
                        log += "Call is active, keep current network";
                        preferred = current;
                    } else if (isTransportNotAvailableForInitial(preferred)) {
                        log += "Target is blocked, keep current network";
                        preferred = current;
                    } else {
                        log += "No call, forward preferred network to framework";
                    }
                    LogUtils.i(TAG, mApnType, log);
                } else if (isHandoverNotAllowedByEvent(event)) {
                    LogUtils.d(TAG, mApnType, "Handover is not allowed by " + event +
                            ". Keep current");
                    preferred = current;
                }
            }

            if (!isWiFiType(preferred) && isImsOnWiFi()) {
                if (isHandover(current, TelephonyManager.NETWORK_TYPE_IWLAN) &&
                    isHandoverNotAllowedByRat(current, TelephonyManager.NETWORK_TYPE_IWLAN)) {
                    LogUtils.v(TAG, mApnType, "Handover to VoMobile is not allowed");
                } else if (isValidCellularType(preferred) && isRtpEventOnVoMobile) {
                    LogUtils.v(TAG, mApnType, "Try handover to VoLTE, RTP event on VoMobile");
                } else {
                    preferred = canUseVoMobile(preferred, isVoMobileUsed);
                }
            }

            if (isValidCellularType(preferred) && isWiFiApHandover) {
                /* stop timer if handover or initial attach to cellular was requested */
                mWiFi.stopWiFiApHandoverTimer();
            }
            postCheckDecision(current, preferred);
        }

        controlCellReports(event, current, isHandover);

        /* add */
        PreferredList preferredList = new PreferredList();
        addToList(preferredList, preferred);

        setLastReportedNetwork(preferred);
        return preferredList;
    }

    protected int getPreferredNoCellular(boolean isWfcEnabled, int current) {
        if (isWfcEnabled && mWiFi.isConnected()) {
            return TelephonyManager.NETWORK_TYPE_IWLAN;
        }
        return TelephonyManager.NETWORK_TYPE_UNKNOWN;
    }

    protected int getPreferredNetwork(int current) {
        int preferred = TelephonyManager.NETWORK_TYPE_UNKNOWN;

        boolean isWfcEnabled = isImsOnWiFi();
        boolean isCellReady = mCell.isRegistered() && isImsOnCell();
        if (mCell.isAirplaneMode()) {
            boolean isWfcSupported = mConfigurationManager.isWfcSupportedInAirplaneMode();
            LogUtils.d(TAG, mApnType, "Airplane mode on and WiFiCalling: " + isWfcEnabled +
                    ", supported: " + isWfcSupported);
            preferred = getPreferredNoCellular(isWfcEnabled && isWfcSupported, current);
        } else if (!isValidCellularType(current) && !isCellReady && !mCell.isCsRegistered()) {
            LogUtils.d(TAG, mApnType, "Cellular is not available and WiFiCalling: " + isWfcEnabled);
            preferred = getPreferredNoCellular(isWfcEnabled, current);
        } else if (isWfcEnabled) {
            boolean isWiFiPreferred = mUser.isWiFiPreferred();
            boolean isWiFiConnected = mWiFi.isConnected();
            boolean isImsCallActive = isImsCallActive();
            boolean isWiFiIn = isWiFiIn(isImsCallActive);
            boolean isWiFiOut = isWiFiOut(isImsCallActive);
            boolean isRtpEvent = isRtpEvent(mLastEvent);
            boolean isCellRegistered = mCell.isRegistered();
            boolean isCellIn = isCellIn(isImsCallActive);
            boolean isCellOut = isCellOut(isImsCallActive);
            boolean isUseDefault = mConfigurationManager.isProfileDefaultNetworkSet();

            LogUtils.d(TAG, mApnType, "WiFiCalling on, preferred " + isWiFiPreferred);
            if (isWiFiPreferred) {
                if (current != TelephonyManager.NETWORK_TYPE_IWLAN) {
                    if (isWiFiConnected) {
                        if (isWiFiIn || (isUseDefault && isCellOut)) {
                            preferred = TelephonyManager.NETWORK_TYPE_IWLAN;
                        } else if (isCellReady) {
                            preferred = mCell.getNetworkType();
                        }
                    } else if (isCellReady) {
                        preferred = mCell.getNetworkType();
                    }
                } else {
                    if (isWiFiConnected) {
                        if (isWiFiOut || isRtpEvent || !mWiFi.isIpReachable()) {
                            if (isCellReady && isCellIn) {
                                preferred = mCell.getNetworkType();
                            } else if (isImsCallActive || !isCellIn) {
                                preferred = TelephonyManager.NETWORK_TYPE_IWLAN;
                            }
                        } else {
                            preferred = TelephonyManager.NETWORK_TYPE_IWLAN;
                        }
                    } else if (isCellReady) {
                        preferred = mCell.getNetworkType();
                    }
                }
            } else {
                if (isCellRegistered && (isCellIn || (!isCellOut && !isWiFiType(current)) ||
                        (isUseDefault && isWiFiOut))) {
                    if (isImsOnCell()) {
                        preferred = mCell.getNetworkType();
                    } else if (isWiFiType(current) && isWiFiConnected && isImsCallActive) {
                        preferred = TelephonyManager.NETWORK_TYPE_IWLAN;
                    }
                } else if (isWiFiConnected) {
                    if (current != TelephonyManager.NETWORK_TYPE_IWLAN) {
                        if (isWiFiIn || !isCellRegistered) {
                            preferred = TelephonyManager.NETWORK_TYPE_IWLAN;
                        } else if (isCellReady) {
                            preferred = mCell.getNetworkType();
                        }
                    } else {
                        /* only for call */
                        if (isRtpEvent && isCellReady) {
                            preferred = mCell.getNetworkType();
                        } else {
                            preferred = TelephonyManager.NETWORK_TYPE_IWLAN;
                        }
                    }
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
        return preferred;
    }

    protected boolean isCellRoamedInternationally() {
        return (mCell.isRoaming() && mCell.isRoamedInternationally());
    }

    private boolean canKeepPdnOnCell(int networkType) {
        if (mCell.isAirplaneMode()) {
            return false;
        }
        boolean enabled = (mUser.isVolteEnabled() && mProvision.isVoLteProvisioned());
        if (isCellRoamedInternationally()) {
            enabled = (enabled && mConfigurationManager.isVoLteRoamingSupported());
        }
        return enabled && isValidCellularType(networkType);
    }

    protected boolean isImsOnCell() {
        boolean enabled = (mUser.isVolteEnabled() && mProvision.isVoLteProvisioned());
        boolean voiceRequired = mConfigurationManager.isVoLteVoPsRequired();
        boolean notVoiceRequiredImsActiveCall = mConfigurationManager.isVoLteVoPsNotRequiredActiveCall() && isImsCallActive();
        if (mCell.isRoaming()) {
            if (mCell.isRoamedInternationally()) {
                enabled = (enabled && mConfigurationManager.isVoLteRoamingSupported());
                enabled = (enabled && mCell.isAllowedNetwork(mConfigurationManager.getAllowedRoamingNetworks()));
                voiceRequired = mConfigurationManager.isVoLteVoPsRequiredRoaming();
            } else {
                enabled = (enabled && mCell.isAllowedNetwork(mConfigurationManager.getAllowedDomesticRoamingNetworks()));
            }
        }
        return (enabled && mCell.isAllowedRat(mConfigurationManager.getVoLteAllowedRat()) &&
               (mCell.isVoiceSupported() || !voiceRequired || notVoiceRequiredImsActiveCall) && isCellTransportAvailable());
    }

    protected boolean isImsOnWiFi() {
        boolean enabled = (mUser.isWfcEnabled() && mProvision.isWfcProvisioned());
        if (mConfigurationManager.isWfcOemProvisioningRequired()) {
            enabled = (enabled && mProvision.isOemProvisioned());
        }
        if (isCellRoamedInternationally() || mWiFi.isRoaming()) {
            enabled = (enabled && mConfigurationManager.isWfcRoamingSupported());
        }
        return enabled && isWiFiTransportAvailable();
    }

    @Override
    protected boolean isServiceSupported(int networkType) {
        if (networkType == TelephonyManager.NETWORK_TYPE_IWLAN) {
            return isImsOnWiFi() && (mWiFi.isConnected() || isVoMobileSupported());
        } else if (networkType != TelephonyManager.NETWORK_TYPE_UNKNOWN) {
            return mCell.isRegistered() && isImsOnCell();
        }
        return false;
    }

    private boolean isHandoverNotAllowedByEvent(ConditionEvents event) {
        switch (event) {
            case EVENT_AIRPLANE_MODE_CHANGED:
                return mCell.isAirplaneMode();
            default:;
        }
        return false;
    }

    protected boolean isImsCallActive() {
        int emergencyRat = getCurrentNetworkType(ApnSetting.TYPE_EMERGENCY);
        /* if emergency PDN established we suppose emergency call is on EIMS */
        return mCall.isImsCallActive(isValidType(emergencyRat));
    }

    @Override
    protected int getHysteresisTimerToLte() {
        return mConfigurationManager.getImsHysteresisTimerToLte();
    }

    @Override
    protected int getHysteresisTimerToWiFi() {
        return mConfigurationManager.getImsHysteresisTimerToWiFi();
    }

    protected boolean startImsRegistrationTimer() {
        if (mImsRegistrationTimer == null) {
            int timeout = mConfigurationManager.getImsRegistrationTimer();
            if (timeout > 0) {
                TimerTask task = new TimerTask() {
                    @Override
                    public void run() {
                        LogUtils.d(TAG, mApnType, "" + ConditionEvents.EVENT_IMS_REGISTRATION_TIMER_EXPIRED + " timer expired");
                        mImsRegistrationTimer = null;
                        mConditions.onConditionChanged(mApnType, ConditionEvents.EVENT_IMS_REGISTRATION_TIMER_EXPIRED);
                    }
                };

                LogUtils.i(TAG, mApnType, "Start ims registration timer for " + timeout + " seconds");
                mImsRegistrationTimer = new Timer();
                mImsRegistrationTimer.schedule(task, timeout * 1000);
                return true;
            }
        }
        return false;
    }

    protected void stopImsRegistrationTimer() {
        if (mImsRegistrationTimer != null) {
            mImsRegistrationTimer.cancel();
            mImsRegistrationTimer = null;
        }
    }

    protected boolean isWiFiIn(boolean callActive) {
        if (callActive) {
            return mWiFi.isHandIn();
        }
        return mWiFi.isRoveIn();
    }

    protected boolean isWiFiOut(boolean callActive) {
        if (callActive) {
            return mWiFi.isHandOut();
        }
        return mWiFi.isRoveOut();
    }

    protected boolean isCellIn(boolean callActive) {
        if (callActive) {
            return mCell.isHandIn();
        }
        return mCell.isRoveIn();
    }

    protected boolean isCellOut(boolean callActive) {
        if (callActive) {
            return mCell.isHandOut();
        }
        return mCell.isRoveOut();
    }

    @Override
    protected int processAttachState(int current, ConditionEvents event) {
        LogUtils.d(TAG, mApnType, "Process " + mState + " state. Event " + event +
                " (" + mEventInfo + ") Current=" + TelephonyManager.getNetworkTypeName(current) +
                ". Preferred=" + TelephonyManager.getNetworkTypeName(mLastPreferredNetwork));
        switch (event) {
            case EVENT_APN_CONNECTED: {
                if (mTimer == null) {
                    setNewState(mIdleState);
                }
                break;
            }
            case EVENT_IMS_REGISTRATION_COMPLETED:
            case EVENT_ATTACH_TIMER_EXPIRED: {
                setNewState(mIdleState);
                break;
            }
            case EVENT_APN_DISCONNECTED: {
                Transport transport = (Transport)mEventInfo;
                if (isTransportMatchedToPreferred(transport)) {
                    if (transport.getLastFailCause() != DataFailCause.NONE) {
                        setNewState(mIdleState);
                    } else {
                        if (mTimer == null) {
                            setNewState(mIdleState);
                        }
                    }
                }
                break;
            }
            case EVENT_APN_FAILURE: {
                Transport transport = (Transport)mEventInfo;
                if (isTransportMatchedToPreferred(transport)) {
                    setNewState(mIdleState);
                }
                break;
            }
            default:
                if (!isServiceSupported(mLastPreferredNetwork)) {
                    setNewState(mIdleState);
                }
        }
        return mLastPreferredNetwork;
    }

    @Override
    protected int processHandoverState(int current, ConditionEvents event) {
        LogUtils.d(TAG, mApnType, "Process " + mState + " state. Event " + event +
                " (" + mEventInfo + ") Current=" + TelephonyManager.getNetworkTypeName(current) +
                ". Preferred=" + TelephonyManager.getNetworkTypeName(mLastPreferredNetwork));
        switch (event) {
            case EVENT_WFC_MODE_CHANGED: {
                setNewState(mIdleState);
                break;
            }
            case EVENT_HYSTERESIS_TIMER_EXPIRED: {
                setNewState(mIdleState);
                break;
            }
            case EVENT_APN_DISCONNECTED:
            case EVENT_APN_FAILURE: {
                Transport transport = (Transport)mEventInfo;
                if (isTransportMatchedToPreferred(transport)) {
                    int timeout = transport.getHandoverRetryTimeout();
                    if (isValidType(current) && (timeout > 0)) {
                        setNewState(mHandoverFailedState, mLastPreferredNetwork, timeout);
                    } else {
                        setNewState(mIdleState);
                    }
                }
                break;
            }
            case EVENT_IMS_REGISTRATION_COMPLETED:
                if (mImsRegistrationTimer == null) {
                    break;
                }
                // fall through
            case EVENT_IMS_REGISTRATION_TIMER_EXPIRED: {
                if (mTimer == null) {
                    setNewState(mIdleState);
                } else {
                    if (!mBlockHandoverByHysteresisOnCall && isImsCallActive()) {
                        setNewState(mIdleState);
                    }
                }
                break;
            }
            case EVENT_APN_CONNECTED:
            case EVENT_APN_HANDOVER_COMPLETED: {
                if (current == mLastPreferredNetwork) {
                    if (mTimer == null) {
                        if (!startImsRegistrationTimer()) {
                            setNewState(mIdleState);
                        }
                    } else {
                        if (!mBlockHandoverByHysteresisOnCall && isImsCallActive()) {
                            if (!startImsRegistrationTimer()) {
                                setNewState(mIdleState);
                            }
                        }
                    }
                }
                break;
            }
            case EVENT_IMS_CALL_STARTED: {
                if (!mBlockHandoverByHysteresisOnCall && isImsCallActive()) {
                    setNewState(mIdleState);
                }
                break;
            }
            case EVENT_WIFI_DISCONNECTED: {
                if (isWiFiType(mLastPreferredNetwork) && isWiFiType(current)) {
                    setNewState(mIdleState);
                }
                break;
            }
            case EVENT_IMS_CALL_RTP_EVENT: {
                if (isWiFiType(mLastPreferredNetwork) && isWiFiType(current) && isImsCallActive()) {
                    setNewState(mIdleState);
                }
                break;
            }
            case EVENT_WFC_SETTINGS_BROADCAST: {
                if (current == mLastPreferredNetwork) {
                    /* If PDN is created on target RAT, exit handover state */
                    setNewState(mIdleState);
                } else {
                    /* If PDN is not created yet, just stop timer */
                    if (mTimer != null) {
                        mTimer.cancel();
                        mTimer = null;
                    }
                }
                break;
            }
            default:
                if (!isServiceSupported(mLastPreferredNetwork)) {
                    setNewState(mIdleState);
                }
        }
        return mLastPreferredNetwork;
    }

    protected void controlCellReports(ConditionEvents event, int current, boolean wasHandover) {
        switch (event) {
            case EVENT_WIFI_DISCONNECTED:
                mCell.stopCellReports();
                break;
            case EVENT_INIT:
                // For initialization
            case EVENT_CARRIER_CONFIG_CHANGED:
            case EVENT_CONFIGURATION_CHANGED:
                // For Configuration changes
            case EVENT_DEFAULT_LINK_PROPERTIES_CHANGED:
                // For Default network changes
            case EVENT_WIFI_CONNECTED:
            case EVENT_WIFI_CAPABILITIES_CHANGED:
                // For WiFi connection
            case EVENT_PHONE_SERVICE_STATE_CHANGED:
                // For Cellular Home/Roaming status
            case EVENT_IMS_CALL_STARTED:
            case EVENT_IMS_CALL_RELEASED:
                // For Call status
            case EVENT_WFC_SETTINGS_CHANGED:
                // For WiFi Calling Settings
            case EVENT_WFC_MODE_CHANGED:
                // For WiFi Calling Mode
            case EVENT_APN_HANDOVER_COMPLETED:
            case EVENT_HYSTERESIS_TIMER_EXPIRED:
                // For HO Hysteresis Timer status
            case EVENT_APN_CONNECTED:
            case EVENT_APN_DISCONNECTED:
                // For APN status
            case EVENT_WIFI_SIGNAL_STRENGTH_CHANGED:
                // For WiFi signal strength
            {
                requestOrStopCellReports(current, true);
                break;
            }
            default: {
                if (wasHandover && (mState != mHandoverState)) {
                    requestOrStopCellReports(current, false);
                }
            }
        }
    }

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
                if (isValidCellularType(current)) {
                    canDisableCellIn = true;
                    if (!mConfigurationManager.isProfileDefaultNetworkSet()) {
                        canDisableCellOut = true;
                    }
                } else if (!isWiFiOut(isImsCallActive)) {
                    if (!isImsCallActive) {
                        canDisableCellIn = true;
                    }
                    canDisableCellOut = true;
                }
            } else {
                if (isValidCellularType(current)) {
                    canDisableCellIn = true;
                } else if (isWiFiType(current)) {
                    canDisableCellOut = true;
                }
            }
            mCell.requestCellReports(isWiFiPreferred, isImsCallActive, isHysteresis,
                    canDisableCellIn, canDisableCellOut);
        } else {
            mCell.stopCellReports();
        }
    }

    @Override
    protected void onHandoverExit() {
        stopImsRegistrationTimer();
        super.onHandoverExit();
    }

    protected boolean isRtpEvent(ConditionEvents event) {
        return ((event == ConditionEvents.EVENT_IMS_CALL_RTP_EVENT) ||
                (event == ConditionEvents.EVENT_IMS_CALL_RTP_TIMEOUT));
    }
}
