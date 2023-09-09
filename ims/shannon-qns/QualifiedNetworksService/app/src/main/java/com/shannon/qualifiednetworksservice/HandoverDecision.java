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

import android.telephony.AccessNetworkConstants;
import android.telephony.DataFailCause;
import android.telephony.data.ApnSetting;
import android.telephony.ServiceState;
import android.telephony.TelephonyManager;
import android.text.TextUtils;
import android.util.LocalLog;
import android.util.SparseArray;

import com.android.internal.annotations.VisibleForTesting;
import com.android.internal.util.IndentingPrintWriter;
import com.shannon.qualifiednetworksservice.Transport.TransportType;
import com.shannon.qualifiednetworksservice.HandoverConditions.ConditionEvents;
import com.shannon.qualifiednetworksservice.util.LogUtils;

import java.util.List;

public class HandoverDecision {
    public static abstract class State {
        public void enter() {};
        public void enter(int timeout) {};
        public abstract void exit();
        public abstract int process(int current, ConditionEvents event);
    }

    protected class DecisionTimerTask implements Runnable {
        private final ConditionEvents mEvent;

        public DecisionTimerTask(ConditionEvents event) {
            mEvent = event;
        }

        @Override
        public void run() {
            LogUtils.d(TAG, mApnType, "" + mEvent + " timer expired");
            mTimer = null;
            mLastPreferredNetwork = TelephonyManager.NETWORK_TYPE_UNKNOWN;
            mConditions.onConditionChanged(mApnType, mEvent);
        }
    }

    protected String TAG;
    protected final int mApnType;
    protected final HandoverConditions mConditions;
    protected final ConfigurationManager mConfigurationManager;
    protected final ConditionCellular mCell;
    protected final ConditionWiFi mWiFi;
    protected final ConditionUserSetting mUser;
    protected final ConditionConnectivity mTransport;
    protected final ConditionMobile mMobile;

    /* timers */
    protected AlarmTimer mTimer;

    /* network */
    protected int mLastPreferredNetwork;
    private boolean mVoMobileUsed;
    private int mLastReportedNetwork;

    protected Object mEventInfo;

    /* log dump */
    private final static int LOG_DUMP_CURRENT = 1;
    private final static int LOG_DUMP_EVENT = 2;
    private final static int LOG_DUMP_CONDITION = 3;

    private final SparseArray<Object> mLogDumpMap;
    private final LocalLog mLogBuffer;

    /* state */
    protected State mState;

    protected State mIdleState = new State() {
        @Override
        public void enter() {
            onIdleEnter();
        }

        @Override
        public void exit() {
            onIdleExit();
        }

        @Override
        public int process(int current, ConditionEvents event) {
            return processIdleState(current, event);
        }

        @Override
        public String toString() {
            return "IDLE";
        }
    };

    protected State mAttachState = new State() {
        @Override
        public void enter() {
            onAttachEnter();
        }

        @Override
        public void exit() {
            onAttachExit();
        }

        @Override
        public int process(int current, ConditionEvents event) {
            return processAttachState(current, event);
        }

        @Override
        public String toString() {
            return "ATTACH";
        }
    };

    protected State mHandoverState = new State() {
        @Override
        public void enter() {
            onHandoverEnter();
        }

        @Override
        public void exit() {
            onHandoverExit();
        }

        @Override
        public int process(int current, ConditionEvents event) {
            return processHandoverState(current, event);
        }

        @Override
        public String toString() {
            return "HANDOVER";
        }
    };

    protected State mHandoverFailedState = new State() {
        @Override
        public void enter(int timeout) {
            onHandoverFailedEnter(timeout);
        }

        @Override
        public void exit() {
            onHandoverFailedExit();
        }

        @Override
        public int process(int current, ConditionEvents event) {
            return processHandoverFailedState(current, event);
        }

        @Override
        public String toString() {
            return "HANDOVER_FAILED";
        }
    };

    public HandoverDecision(int apnType, HandoverConditions conditions) {
        mConditions = conditions;
        mApnType = apnType;

        mConfigurationManager = mConditions.getConfigurationManager();
        mCell = mConditions.getCellularConditions();
        mWiFi = mConditions.getWiFiConditions();
        mUser = mConditions.getUserConditions();
        mTransport = mConditions.getTransportCondition();
        mMobile = mConditions.getMobileConditions();

        mTimer = null;
        mLastPreferredNetwork = TelephonyManager.NETWORK_TYPE_UNKNOWN;
        mVoMobileUsed = false;
        mLastReportedNetwork = TelephonyManager.NETWORK_TYPE_UNKNOWN;

        mEventInfo = null;

        mLogDumpMap = new SparseArray<>();
        mLogBuffer = new LocalLog(100);

        mState = mIdleState;
    }

    public PreferredList getPreferredList(ConditionEvents event) { return new PreferredList(); }

    protected boolean isValidCellularType(int networkType) {
        return (networkType != TelephonyManager.NETWORK_TYPE_UNKNOWN) &&
                (networkType != TelephonyManager.NETWORK_TYPE_IWLAN);
    }

    protected boolean isValidType(int networkType) {
        return (networkType != TelephonyManager.NETWORK_TYPE_UNKNOWN);
    }

    protected boolean isWiFiType(int networkType) {
        return (networkType == TelephonyManager.NETWORK_TYPE_IWLAN);
    }

    protected void addCellular(List<Integer> list, int networkType) {
        list.add(getAccessTypeFromTelephony(networkType));
    }

    protected void addWiFi(List<Integer> list) {
        list.add(AccessNetworkConstants.AccessNetworkType.IWLAN);
    }

    protected void addToList(List<Integer> list, int networkType) {
        if (networkType == TelephonyManager.NETWORK_TYPE_IWLAN) {
            list.add(AccessNetworkConstants.AccessNetworkType.IWLAN);
        } else if (isValidType(networkType)) {
            list.add(getAccessTypeFromTelephony(networkType));
        }
    }

    protected boolean isHandover(int from, int to) {
        return ((from != to) &&
                ((from != TelephonyManager.NETWORK_TYPE_UNKNOWN) && (to != TelephonyManager.NETWORK_TYPE_UNKNOWN)) &&
                ((from == TelephonyManager.NETWORK_TYPE_IWLAN) || (to == TelephonyManager.NETWORK_TYPE_IWLAN)));
    }

    protected int getCurrentNetworkType(int type) {
        return mTransport.getNetworkType(type);
    }

    protected int getCurrentNetworkType() {
        return mTransport.getNetworkType(mApnType);
    }

    protected boolean isCellTransportAvailable() {
        if (mState != mIdleState) {
            return true;
        }
        return mTransport.isTransportAvailable(mApnType, TransportType.TRANSPORT_CELLULAR);
    }

    protected boolean isImsPdnOnWiFi() {
        return (getCurrentNetworkType(ApnSetting.TYPE_IMS) == TelephonyManager.NETWORK_TYPE_IWLAN);
    }

    protected boolean isWiFiTransportAvailable() {
        if (mState != mIdleState) {
            return true;
        }
        return mTransport.isTransportAvailable(mApnType, TransportType.TRANSPORT_WIFI);
    }

    protected boolean isTransportNotAvailableForInitial(int networkType) {
        if (networkType == TelephonyManager.NETWORK_TYPE_UNKNOWN) {
            return true;
        } else if (networkType == TelephonyManager.NETWORK_TYPE_IWLAN) {
            return !mTransport.isTransportAvailableForInitial(mApnType,
                    TransportType.TRANSPORT_WIFI);
        } else {
            return !mTransport.isTransportAvailableForInitial(mApnType,
                    TransportType.TRANSPORT_CELLULAR);
        }
    }

    protected boolean isHandoverFailed(int networkType) {
        if (isWiFiType(networkType)) {
            return mTransport.isHandoverFailed(mApnType, TransportType.TRANSPORT_WIFI);
        }
        if (isValidCellularType(networkType)) {
            return mTransport.isHandoverFailed(mApnType, TransportType.TRANSPORT_CELLULAR);
        }
        return false;
    }

    protected boolean isHandoverNotAllowedByRat(int current, int preferred) {
        String capabilityName = ApnType.getCapabilityNameOf(mApnType);

        if (TextUtils.isEmpty(capabilityName)) {
            LogUtils.e(TAG, "Not found capability name of ApnType:" + mApnType);
            return false;
        }

        return mConfigurationManager.isHandoverNotAllowed(capabilityName,
                getAccessTypeFromTelephony(current), getAccessTypeFromTelephony(preferred),
                mCell.isRoaming());
    }

    private int getAccessTypeFromTelephony(int networkType) {
        return ServiceState.rilRadioTechnologyToAccessNetworkType(
                ServiceState.networkTypeToRilRadioTechnology(networkType));
    }

    protected int getHysteresisTimerToLte() {
        return mConfigurationManager.getHysteresisTimerToLte();
    }

    protected int getHysteresisTimerToWiFi() {
        return mConfigurationManager.getHysteresisTimerToWiFi();
    }

    protected void startHysteresisTimer() {
        if (mTimer == null) {
            int timeout;
            if (mLastPreferredNetwork == TelephonyManager.NETWORK_TYPE_IWLAN) {
                timeout = getHysteresisTimerToLte();
            } else {
                timeout = getHysteresisTimerToWiFi();
            }
            if (timeout != 0) {
                LogUtils.i(TAG, mApnType, "Start hysteresis timer for " + timeout + " seconds");

                mTimer = new AlarmTimer();
                mTimer.schedule(new DecisionTimerTask(ConditionEvents.EVENT_HYSTERESIS_TIMER_EXPIRED),
                        timeout * 1000);
            }
        }
    }

    private void startAttachTimer() {
        if (mTimer == null) {
            int timeout = mConfigurationManager.getInitialAttachTimeout();
            if (timeout != 0) {
                LogUtils.d(TAG, mApnType, "Start initial attach timer for " + timeout + " seconds");

                mTimer = new AlarmTimer();
                mTimer.schedule(new DecisionTimerTask(ConditionEvents.EVENT_ATTACH_TIMER_EXPIRED),
                        timeout * 1000);
            }
        }
    }

    private void startThrottleTimer(int timeout) {
        if (mTimer == null) {
            if (timeout > 0) {
                LogUtils.d(TAG, mApnType, "Start throttle timer for " + timeout);

                mTimer = new AlarmTimer();
                mTimer.schedule(new DecisionTimerTask(ConditionEvents.EVENT_THROTTLE_TIMER_EXPIRED),
                        timeout);
            }
        }
    }

    protected void stopTimer() {
        if (mTimer != null) {
            mTimer.cancel();
            mTimer = null;
            mLastPreferredNetwork = TelephonyManager.NETWORK_TYPE_UNKNOWN;
        }
    }

    protected boolean isServiceSupported(int networkType) { return true; }

    private boolean isVoMobilePossible(int preferred, boolean isVoMobileUsed) {
        if (isWiFiType(preferred) || mWiFi.isConnected() || mCell.isAirplaneMode()) {
            return false;
        }
        boolean isApnEnabledOverMobile = mUser.isVoMobileApnTypeEnabled(mApnType);
        boolean isPsRegistered = mCell.isRegistered();
        boolean isCsRegistered = mCell.isCsRegistered();
        if (isApnEnabledOverMobile &&
                mMobile.isOpportunisticNetworkConnectedAndPreferred(isPsRegistered || isCsRegistered)) {
            return true;
        }
        if (mUser.isVoMobileEnabled() && isApnEnabledOverMobile) {
            boolean isVoMobilePreferred = mConfigurationManager.isVoMobilePreferred();
            boolean isVoMobileRoamingOverrideSignalStrength =
                    mConfigurationManager.isVoMobileRoamingOverrideSignalStrength();
            boolean isPsRoaming = mCell.isRoaming();
            boolean isCsRoaming = mCell.isCsRoaming();
            boolean canIgnorePsSignalStrength = (isVoMobileRoamingOverrideSignalStrength && isPsRoaming);
            boolean canIgnoreCsSignalStrength = (isVoMobileRoamingOverrideSignalStrength && isCsRoaming);
            boolean canIgnoreCsRegistration = mConfigurationManager.isVoMobileIgnoreCsState();
            boolean badSignalStrength = mCell.isLowerThanThreshold(mConfigurationManager.getVoMobileSignalOut());
            boolean isServicePossible = isValidCellularType(preferred) || isPsRegistered || (isCsRegistered && !canIgnoreCsRegistration);
            boolean isHandover = isInHandoverState();
            boolean isCellRoveIn = mCell.isBetterThanThreshold(mConfigurationManager.getVoMobileSignalIn());
            LogUtils.v(TAG, mApnType,
                    "VoMobile RAT=" + TelephonyManager.getNetworkTypeName(preferred) +
                            " PS=" + isPsRegistered + " CS=" + isCsRegistered + " R=" + (isPsRoaming || isCsRoaming) +
                            " noPsSS=" + canIgnorePsSignalStrength + " noCs=" + canIgnoreCsRegistration +
                            " noCsSS=" + canIgnoreCsSignalStrength + " preferred=" + isVoMobilePreferred +
                            " used=" + isVoMobileUsed + " handover=" + isHandover + " cell-in=" + isCellRoveIn);
                    // VoMobile preferred is set, it overrides PS and CS checks
            return ((isVoMobilePreferred ||
                    // if VoMobile was already selected try to keep it and return back to LTE at rove-in condition
                    (isVoMobileUsed && (isHandover || !isCellRoveIn)) ||
                    // device is not connected to PS network or it is not good for IMS services
                    ((!isPsRegistered || canIgnorePsSignalStrength || badSignalStrength) &&
                    // CS connection can be used or not?
                    (canIgnoreCsRegistration || !isCsRegistered || canIgnoreCsSignalStrength || badSignalStrength))) &&
                    // last one, check default SIM network callback
                    mMobile.isVoMobilePossible(isServicePossible, isPsRoaming || isCsRoaming, isVoMobileUsed));
        }
        return false;
    }

    protected int canUseVoMobile(int preferred, boolean isVoMobileUsed) {
        mVoMobileUsed = isVoMobilePossible(preferred, isVoMobileUsed);
        if (mVoMobileUsed) {
            LogUtils.d(TAG, mApnType, "VoMobile connection");
            return TelephonyManager.NETWORK_TYPE_IWLAN;
        }
        return preferred;
    }

    protected boolean isVoMobileSupported() {
        return (mVoMobileUsed && isVoMobilePossible(TelephonyManager.NETWORK_TYPE_UNKNOWN, true));
    }

    protected void setNewState(State state) {
        LogUtils.d(TAG, mApnType, "State changed. From " + mState + " to " + state);
        saveToLocalLog(state);

        mState.exit();
        mState = state;
        mState.enter();
    }

    protected void setNewState(State state, int preferred, int timeout) {
        LogUtils.d(TAG, mApnType, "State changed. From " + mState + " to " + state + " with timeout=" + timeout);
        saveToLocalLog(state, preferred, timeout);

        mState.exit();
        mLastPreferredNetwork = preferred;
        mState = state;
        mState.enter(timeout);
    }

    /* state machine handlers */
    protected void onIdleEnter() { mVoMobileUsed = false; }
    protected void onIdleExit() {}
    protected void onAttachEnter() { startAttachTimer(); }
    protected void onAttachExit() { stopTimer(); }
    protected void onHandoverEnter() { startHysteresisTimer(); }
    protected void onHandoverExit() { stopTimer(); }
    protected void onHandoverFailedEnter(int timeout) { startThrottleTimer(timeout); }
    protected void onHandoverFailedExit() { stopTimer(); }

    protected int processIdleState(int current, ConditionEvents event) {
        mVoMobileUsed = false;

        LogUtils.d(TAG, mApnType, "Process " + mState + " state. Event " + event +
                " (" + mEventInfo + "). Current=" + TelephonyManager.getNetworkTypeName(current));
        return TelephonyManager.NETWORK_TYPE_UNKNOWN;
    }

    protected boolean isTransportTypeMatched(int networkType, TransportType transportType) {
        return (((transportType == TransportType.TRANSPORT_WIFI) && isWiFiType(networkType)) ||
                ((transportType == TransportType.TRANSPORT_CELLULAR) && isValidCellularType(networkType)));
    }

    protected boolean isTransportMatchedToPreferred(Transport transport) {
        return isTransportTypeMatched(mLastPreferredNetwork, transport.getType());
    }

    protected int processAttachState(int current, ConditionEvents event) {
        LogUtils.d(TAG, mApnType, "Process " + mState + " state. Event " + event +
                " (" + mEventInfo + "). Current=" + TelephonyManager.getNetworkTypeName(current) +
                ". Preferred=" + TelephonyManager.getNetworkTypeName(mLastPreferredNetwork));
        switch (event) {
            case EVENT_APN_CONNECTED: {
                setNewState(mIdleState);
                break;
            }
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

    protected int processHandoverState(int current, ConditionEvents event) {
        LogUtils.d(TAG, mApnType, "Process " + mState + " state. Event " + event +
                " (" + mEventInfo + "). Current=" + TelephonyManager.getNetworkTypeName(current) +
                ". Preferred=" + TelephonyManager.getNetworkTypeName(mLastPreferredNetwork));
        switch (event) {
            case EVENT_APN_CONNECTED:
            case EVENT_APN_HANDOVER_COMPLETED: {
                if (current == mLastPreferredNetwork) {
                    if (mTimer == null) {
                        setNewState(mIdleState);
                    }
                }
                break;
            }
            case EVENT_APN_DISCONNECTED:
            case EVENT_APN_FAILURE: {
                Transport transport = (Transport)mEventInfo;
                if (isTransportMatchedToPreferred(transport)) {
                    int timeout = transport.getHandoverRetryTimeout();
                    if (timeout > 0) {
                        setNewState(mHandoverFailedState, mLastPreferredNetwork, timeout);
                    } else {
                        setNewState(mIdleState);
                    }
                }
                break;
            }
            case EVENT_HYSTERESIS_TIMER_EXPIRED: {
                setNewState(mIdleState);
                break;
            }
            default:
                if (!isServiceSupported(mLastPreferredNetwork)) {
                    setNewState(mIdleState);
                }
        }
        return mLastPreferredNetwork;
    }

    protected int processHandoverFailedState(int current, ConditionEvents event) {
        LogUtils.d(TAG, mApnType, "Process " + mState + " state. Event " + event +
                " (" + mEventInfo + "). Current=" + TelephonyManager.getNetworkTypeName(current) +
                ". Preferred=" + TelephonyManager.getNetworkTypeName(mLastPreferredNetwork));
        switch (event) {
            case EVENT_APN_CONNECTING: {
                TransportType transportType = (TransportType)mEventInfo;
                if (isTransportTypeMatched(mLastPreferredNetwork, transportType)) {
                    setNewState(mHandoverState);
                }
                break;
            }
            case EVENT_APN_DISCONNECTED:
            case EVENT_APN_FAILURE: {
                Transport transport = (Transport)mEventInfo;
                if (isTransportTypeMatched(current, transport.getType())) {
                    setNewState(mIdleState);
                }
                break;
            }
            case EVENT_THROTTLE_TIMER_EXPIRED: {
                setNewState(mIdleState);
                break;
            }
            case EVENT_APN_UNTHROTTLED: {
                TransportType transportType = (TransportType)mEventInfo;
                if (isTransportTypeMatched(mLastPreferredNetwork, transportType)) {
                    setNewState(mIdleState);
                }
                break;
            }
            default:
                if (!isServiceSupported(current)) {
                    setNewState(mIdleState);
                }
        }
        return mLastPreferredNetwork;
    }

    /* on enter pre-check, return network type UNKNOWN if new decision possible */
    protected int preCheckEvent(int current, ConditionEvents event) {
        mLogDumpMap.put(LOG_DUMP_CURRENT, current);
        mLogDumpMap.put(LOG_DUMP_EVENT, event);
        mLogDumpMap.put(LOG_DUMP_CONDITION, mConditions.toString());
        return mState.process(current, event);
    }

    /* on decision post-check, update state machine if required */
    protected void postCheckDecision(int current, int preferred) {
        mLastPreferredNetwork = preferred;

        if (!isValidType(current) && isValidType(preferred) && (mLastReportedNetwork != preferred)) {
            setNewState(mAttachState);
        } else if (isHandover(current, preferred)) {
            setNewState(mHandoverState);
        }
    }

    protected void setLastReportedNetwork(int network) { mLastReportedNetwork = network; }

    public void setEventInfo(Object object) { mEventInfo = object; }

    protected boolean isVoMobileUsed() { return mVoMobileUsed; }

    protected boolean isRoaming() {
        if (mCell.isRegistered()) {
            return mCell.isRoaming();
        }
        Transport transport = mTransport.getCurrentTransport(mApnType);
        if ((transport != null) && (transport.getType() == TransportType.TRANSPORT_CELLULAR)) {
            return transport.isRoaming();
        }
        if (mCell.isCsRegistered()) {
            return mCell.isCsRoaming();
        }
        return mWiFi.isRoaming();
    }

    public boolean isInHandoverState() { return (mState == mHandoverState); }

    private void saveToLocalLog(State state) {
        int current = TelephonyManager.NETWORK_TYPE_UNKNOWN;
        if (mLogDumpMap.contains(LOG_DUMP_CURRENT)) {
            current = (int)mLogDumpMap.get(LOG_DUMP_CURRENT);
        }
        String s = "" + mState + "->" + state + ", " +
                mLogDumpMap.get(LOG_DUMP_EVENT) +
                ", info=" + mEventInfo +
                ", current=" + TelephonyManager.getNetworkTypeName(current) +
                ", preferred=" + TelephonyManager.getNetworkTypeName(mLastPreferredNetwork) +
                ", mobile=" + mVoMobileUsed + ", " + mLogDumpMap.get(LOG_DUMP_CONDITION);
        mLogBuffer.log(s);
    }

    private void saveToLocalLog(State state, int preferred, int timeout) {
        int current = TelephonyManager.NETWORK_TYPE_UNKNOWN;
        if (mLogDumpMap.contains(LOG_DUMP_CURRENT)) {
            current = (int)mLogDumpMap.get(LOG_DUMP_CURRENT);
        }
        String s = "" + mState + "->" + state + ", " +
                mLogDumpMap.get(LOG_DUMP_EVENT) +
                ", info=" + mEventInfo +
                ", timeout=" + timeout +
                ", current=" + TelephonyManager.getNetworkTypeName(current) +
                ", preferred=" + TelephonyManager.getNetworkTypeName(preferred) +
                ", mobile=" + mVoMobileUsed + ", " + mLogDumpMap.get(LOG_DUMP_CONDITION);
        mLogBuffer.log(s);
    }

    public void saveToLocalLog(String s) {
        mLogBuffer.log(s);
    }

    public void dump(IndentingPrintWriter printWriter) {
        IndentingPrintWriter pw = new IndentingPrintWriter(printWriter, "  ");
        pw.increaseIndent();
        mLogBuffer.dump(pw);
        pw.decreaseIndent();
        pw.flush();
    }

    @VisibleForTesting
    protected Object getEventInfo() { return mEventInfo; }
}
