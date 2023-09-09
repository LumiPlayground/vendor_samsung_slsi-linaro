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

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Message;
import android.provider.Settings;
import android.telephony.AccessNetworkConstants;
import android.telephony.AccessNetworkConstants.AccessNetworkType;
import android.telephony.CellSignalStrength;
import android.telephony.CellSignalStrengthCdma;
import android.telephony.CellSignalStrengthGsm;
import android.telephony.CellSignalStrengthLte;
import android.telephony.CellSignalStrengthNr;
import android.telephony.CellSignalStrengthTdscdma;
import android.telephony.CellSignalStrengthWcdma;
import android.telephony.DataSpecificRegistrationInfo;
import android.telephony.NrVopsSupportInfo;
import android.telephony.VopsSupportInfo;
import android.telephony.NetworkRegistrationInfo;
import android.telephony.PhoneStateListener;
import android.telephony.ServiceState;
import android.telephony.SignalStrength;
import android.telephony.TelephonyManager;

import android.text.TextUtils;
import android.util.SparseArray;

import com.android.internal.annotations.VisibleForTesting;
import com.shannon.qualifiednetworksservice.BarringInfo.BarringServiceType;
import com.shannon.qualifiednetworksservice.BarringInfo.BarringType;
import com.shannon.qualifiednetworksservice.util.LogUtils;

import java.util.HashMap;
import java.util.List;

import static android.telephony.ims.ImsMmTelManager.WIFI_MODE_WIFI_PREFERRED;
import static android.telephony.PhoneStateListener.LISTEN_NONE;
import static android.telephony.PhoneStateListener.LISTEN_SERVICE_STATE;
import static android.telephony.PhoneStateListener.LISTEN_SIGNAL_STRENGTHS;
import static com.shannon.qualifiednetworksservice.HandoverConditions.ConditionEvents.*;
import static com.shannon.qualifiednetworksservice.MapType.*;

public class ConditionCellular {
    public static final int MIN_LTE_RSRP = -140;
    public static final int NOT_USABLE_LTE_RSRP = -130;
    public static final int GOOD_LTE_RSRP = -95;
    private static final int COUNTRY_CODE_LENGTH = 3;

    protected class CellBroadcastReceiver extends BroadcastReceiver {
        @Override
        public void onReceive(Context context, Intent intent) {
            final String action = intent.getAction();
            if (action == null) {
                LogUtils.w(TAG, "NULL action for intent " + intent);
                return;
            }
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "Got intent " + intent);
            switch (action) {
                case Intent.ACTION_AIRPLANE_MODE_CHANGED: {
                    int mode = getAirplaneMode();

                    LogUtils.i(TAG, LogUtils.LOG_CONDITIONS, "Airplane mode changed to " + mode);
                    mAirplaneMode = mode;

                    mChangedCallback.onConditionChanged(EVENT_AIRPLANE_MODE_CHANGED);
                    break;
                }
            }
        }
    }

    protected class CellularStateListener extends PhoneStateListener {
        CellularStateListener(int subId) {
            super();
            mSubId = subId;
        }

        private boolean isNotInService(NetworkRegistrationInfo registrationInfo) {
            int state = registrationInfo.getRegistrationState();
            return (state == NetworkRegistrationInfo.REGISTRATION_STATE_NOT_REGISTERED_OR_SEARCHING ||
                    state == NetworkRegistrationInfo.REGISTRATION_STATE_NOT_REGISTERED_SEARCHING ||
                    state == NetworkRegistrationInfo.REGISTRATION_STATE_UNKNOWN);
        }

        @Override
        public void onServiceStateChanged(ServiceState serviceState) {
            if (serviceState == null) {
                return;
            }
            LogUtils.i(TAG, LogUtils.LOG_CONDITIONS, "onServiceStateChanged: " + serviceState);

            NetworkRegistrationInfo psRegState = serviceState.getNetworkRegistrationInfo(
                    NetworkRegistrationInfo.DOMAIN_PS, AccessNetworkConstants.TRANSPORT_TYPE_WWAN);
            NetworkRegistrationInfo csRegState = serviceState.getNetworkRegistrationInfo(
                    NetworkRegistrationInfo.DOMAIN_CS, AccessNetworkConstants.TRANSPORT_TYPE_WWAN);

            boolean disconnected = false;
            if (((csRegState == null) || isNotInService(csRegState)) &&
                ((psRegState == null) || isNotInService(psRegState))) {
                // reset signal strength if both not registered
                mSignalStrength = new CellSignalStrengthLte();
                disconnected = true;
            }
            // update if state is changed
            if ((psRegState != null) && !mPsRegState.equals(psRegState)) {
                LogUtils.d(TAG, LogUtils.LOG_CONDITIONS, "PS registration state changed to " + psRegState);

                mPsRegState = psRegState;
                mChangedCallback.onConditionChanged(EVENT_PHONE_SERVICE_STATE_CHANGED);
            }
            if ((csRegState != null) && !mCsRegState.equals(csRegState)) {
                LogUtils.d(TAG, LogUtils.LOG_CONDITIONS, "CS registration state changed to " + csRegState);

                mCsRegState = csRegState;
                mChangedCallback.onConditionChanged(EVENT_PHONE_SERVICE_STATE_CHANGED);
            }
            // update signal strength
            if (!disconnected) {
                CellSignalStrength strength = getLastSignalStrength();
                if ((strength != null) && !mSignalStrength.equals(strength)) {
                    mSignalStrength = strength;
                    mChangedCallback.onConditionChanged(EVENT_CELL_SIGNAL_STRENGTH_CHANGED);
                }
            }
        }

        @Override
        public void onSignalStrengthsChanged(SignalStrength signalStrength) {
            try {
                List<CellSignalStrength> signalStrengthList = signalStrength.getCellSignalStrengths();
                LogUtils.i(TAG, LogUtils.LOG_CONDITIONS, "onSignalStrengthsChanged " + signalStrengthList);

                if (isAirplaneMode()) {
                    if (mSignalStrength.isValid()) {
                        // reset valid mSignalStrength to default if APM on
                        mSignalStrength = new CellSignalStrengthLte();
                    }
                    return;
                }
                if (signalStrengthList.isEmpty()) {
                    // do we need to clear signal strength here?
                    return;
                }
                for (CellSignalStrength cellStrength : signalStrengthList) {
                    if (!cellStrength.isValid() || !isSignalStrengthMatchedToCurrentRat(cellStrength)) {
                        continue;
                    }
                    LogUtils.d(TAG, LogUtils.LOG_CONDITIONS, "Received " + cellStrength);
                    if (!cellStrength.equals(mSignalStrength)) {
                        mSignalStrength = cellStrength;
                        mChangedCallback.onConditionChanged(EVENT_CELL_SIGNAL_STRENGTH_CHANGED);
                    }
                }
            } catch (Exception e) {
                LogUtils.e(TAG, "Exception in onSignalStrengthsChanged " + e);
            }
        }
    }

    private final String TAG;
    private final int mSlotId;
    private final Context mContext;
    private final Handler mHandler;
    private final HandlerThread mHandlerThread;
    private final TelephonyManager mTelephonyManager;
    private final RilOemInterface mRilInterface;
    private final CellularStateListener mStateListener;
    private final HandoverConditions.IConditionChanged mChangedCallback;
    private final ConfigurationManager mConfigurationManager;
    private final CellBroadcastReceiver mBroadcastReceiver;
    private NetworkRegistrationInfo mPsRegState;
    private NetworkRegistrationInfo mCsRegState;
    private BarringInfo mVoiceBarringInfo;
    private CellSignalStrength mSignalStrength;
    private int mAirplaneMode;
    private boolean mDisconnectedFromNetwork;
    private SparseArray<HashMap<MapType, CellularParameters>> mSignalIn;
    private SparseArray<HashMap<MapType, CellularParameters>> mSignalOut;
    private SparseArray<CellularParameters> mSignalMid;

    private RilSetCellularThresholds mLastCellularThresholds;

    public ConditionCellular(int slotIndex, Context context, RilOemInterface oemInterface,
                             ConfigurationManager configurationManager, HandoverConditions.IConditionChanged callback) {
        mSlotId = slotIndex;
        mContext = context;
        mRilInterface = oemInterface;
        mConfigurationManager = configurationManager;
        mChangedCallback = callback;
        SimManager simManager = SimManager.getInstance();
        mTelephonyManager = simManager.getTelephonyManager(mSlotId);

        TAG = ShannonQualifiedNetworksService.getServiceTAG("-" + mSlotId + "-HC-cell");

        mStateListener = new CellularStateListener(simManager.getSubId(mSlotId));
        mBroadcastReceiver = new CellBroadcastReceiver();
        mAirplaneMode = getAirplaneMode();
        mDisconnectedFromNetwork = false;
        buildSignalHashMaps();
        mLastCellularThresholds = null;

        /* get last service state */
        mPsRegState = null;
        mCsRegState = null;
        try {
            ServiceState serviceState = mTelephonyManager.getServiceState();
            mPsRegState = serviceState.getNetworkRegistrationInfo(
                    NetworkRegistrationInfo.DOMAIN_PS, AccessNetworkConstants.TRANSPORT_TYPE_WWAN);
            mCsRegState = serviceState.getNetworkRegistrationInfo(
                    NetworkRegistrationInfo.DOMAIN_CS, AccessNetworkConstants.TRANSPORT_TYPE_WWAN);
        } catch (Exception e) {
            // nothing
        }
        if (mPsRegState == null) {
            mPsRegState = new NetworkRegistrationInfo.Builder()
                    .setAccessNetworkTechnology(TelephonyManager.NETWORK_TYPE_UNKNOWN)
                    .setRegistrationState(NetworkRegistrationInfo.REGISTRATION_STATE_UNKNOWN)
                    .build();
        }
        if (mCsRegState == null) {
            mCsRegState = new NetworkRegistrationInfo.Builder()
                    .setAccessNetworkTechnology(TelephonyManager.NETWORK_TYPE_UNKNOWN)
                    .setRegistrationState(NetworkRegistrationInfo.REGISTRATION_STATE_UNKNOWN)
                    .build();
        }
        mVoiceBarringInfo = null;

        /* get last signal strength */
        mSignalStrength = getLastSignalStrength();
        if (mSignalStrength == null) {
            mSignalStrength = new CellSignalStrengthLte();
        }

        IntentFilter filter = new IntentFilter();
        filter.addAction(Intent.ACTION_AIRPLANE_MODE_CHANGED);
        mContext.registerReceiver(mBroadcastReceiver, filter);

        mHandlerThread = new HandlerThread(ConditionCellular.class.getSimpleName());
        mHandlerThread.start();
        mHandler = new Handler(mHandlerThread.getLooper()) {
            @Override
            public void handleMessage(Message message) {
                if (message.what == RilOemInterface.RIL_UNSOLICITED_MESSAGE) {
                    RilUnsolicitedMessage msg = (RilUnsolicitedMessage) message.obj;
                    switch (msg.getMessageId()) {
                        case RilUnsolicitedMessage.RIL_UNSOL_BARRING: {
                            RilUnsolicitedBarring barringInfo = new RilUnsolicitedBarring(msg);
                            LogUtils.d(TAG, LogUtils.LOG_CONDITIONS, "Received " + barringInfo);

                            BarringInfo voiceBarringInfo = barringInfo.getBarringInfo(
                                    BarringServiceType.SERVICE_TYPE_MMTEL_VOICE);
                            if ((voiceBarringInfo != null) &&
                                    !voiceBarringInfo.equals(mVoiceBarringInfo)) {
                                mVoiceBarringInfo = voiceBarringInfo;
                                LogUtils.d(TAG, LogUtils.LOG_CONDITIONS, "Voice barring info "
                                                + "changed " + mVoiceBarringInfo.getBarringFactor());
                                mChangedCallback.onConditionChanged(EVENT_BARRING_INFO_CHANGED);
                            }
                        }
                    }
                }
            }
        };

        mRilInterface.registerUnsolicitedListener(RilUnsolicitedMessage.RIL_UNSOL_BARRING, mHandler);
        mTelephonyManager.listen(mStateListener, LISTEN_SIGNAL_STRENGTHS | LISTEN_SERVICE_STATE);
    }

    public void destroy() {
        stopCellReports();
        mContext.unregisterReceiver(mBroadcastReceiver);
        mTelephonyManager.listen(mStateListener, LISTEN_NONE);
        mRilInterface.unregisterUnsolicitedListener(RilUnsolicitedMessage.RIL_UNSOL_BARRING);
        mHandlerThread.quit();
    }

    /*
     REGISTRATION_STATE_NOT_REGISTERED_OR_SEARCHING = 0;
     REGISTRATION_STATE_HOME = 1;
     REGISTRATION_STATE_NOT_REGISTERED_SEARCHING = 2;
     REGISTRATION_STATE_DENIED = 3;
     REGISTRATION_STATE_UNKNOWN = 4;
     REGISTRATION_STATE_ROAMING = 5;
     */
    public boolean isConnected() {
        return ((mPsRegState.isRegistered()) ||
                (mPsRegState.getRegistrationState() == NetworkRegistrationInfo.REGISTRATION_STATE_DENIED));
    }

    public boolean isOutOfService() { return !isRegistered(); } // NOT_REGI + SEARCH + DENIED + UNKNOWN
    public boolean isRegistered() { return mPsRegState.isRegistered(); } // HOME + ROAM
    public int getNetworkType() { return mPsRegState.getAccessNetworkTechnology(); }
    public boolean isAirplaneMode() { return (mAirplaneMode == 1); }
    public boolean isDisconnectedFromNetwork() { return mDisconnectedFromNetwork; }
    public void setDisconnectedFromNetwork(boolean value) { mDisconnectedFromNetwork = value; }
    public boolean isNotLTE() { return !isLTE(); }

    public boolean isRoaming() {
        return (mPsRegState.getRegistrationState() == NetworkRegistrationInfo.REGISTRATION_STATE_ROAMING);
    }

    public boolean isRoamedInternationally() {
        return (mPsRegState.getRoamingType() == ServiceState.ROAMING_TYPE_INTERNATIONAL);
    }

    public boolean isLTE() {
        return ServiceState.isPsOnlyTech(
                ServiceState.networkTypeToRilRadioTechnology(getNetworkType()));
    }

    public boolean isVoiceSupported() {
        try {
            DataSpecificRegistrationInfo regInfo = mPsRegState.getDataSpecificInfo();
            if (regInfo != null) {
                VopsSupportInfo supportInfo = regInfo.getVopsSupportInfo();
                if (supportInfo != null) {
                    if (supportInfo instanceof NrVopsSupportInfo) {
                        NrVopsSupportInfo nr = (NrVopsSupportInfo)supportInfo;
                        return ((nr.getVopsSupport() & NrVopsSupportInfo.NR_STATUS_VOPS_3GPP_SUPPORTED) != 0);
                    } else { // supportInfo instanceof LteVopsSupportInfo
                        return supportInfo.isVopsSupported();
                    }
                }
            }
        } catch (Exception e) {
            // nothing
        }
        return false;
    }

    public boolean isEmergencySupported() {
        try {
            DataSpecificRegistrationInfo regInfo = mPsRegState.getDataSpecificInfo();
            if (regInfo != null) {
                VopsSupportInfo supportInfo = regInfo.getVopsSupportInfo();
                if (supportInfo != null) {
                    return supportInfo.isEmergencyServiceSupported();
                }
            }
        } catch (Exception e) {
            // nothing
        }
        return false;
    }

    public boolean isVoiceBarred() {
        if (mVoiceBarringInfo == null) return false;

        BarringType type = mVoiceBarringInfo.getBarringType();
        return (type == BarringType.BARRING_TYPE_UNCONDITIONAL);
    }

    public boolean isAllowedRat(String allowedRatList) {
        return isAllowedRat(getNetworkType(), allowedRatList);
    }

    public boolean isAllowedRat(int networkType, String allowedRatList) {
        String current = TelephonyManager.getNetworkTypeName(networkType);
        String [] list = allowedRatList.split(",");
        for (String s : list) {
            if (current.equalsIgnoreCase(s.trim())) {
                return true;
            }
        }
        return false;
    }

    public boolean isAllowedNetwork(String allowedNetworksList) {
        /* default - all allowed */
        if (TextUtils.isEmpty(allowedNetworksList)) {
            return true;
        }
        String current = mPsRegState.getRegisteredPlmn();
        /* current PLMN should be valid */
        if (TextUtils.isEmpty(current)) {
            return false;
        }
        LogUtils.v(TAG, LogUtils.LOG_CONDITIONS,
                "Allowed networks list: " + allowedNetworksList + " current: " + current);

        String[] list = allowedNetworksList.split(",");
        for (String s : list) {
            s = s.trim();
            if (s.length() < COUNTRY_CODE_LENGTH) {
                continue;
            }
            if (s.length() == COUNTRY_CODE_LENGTH) {
                /* MCC only */
                if (current.startsWith(s)) {
                    return true;
                }
            } else {
                /* whole PLMN */
                if (current.equalsIgnoreCase(s)) {
                    return true;
                }
            }
        }
        return false;
    }

    private int getAccessTechFromSignalStrength() {
        if (mSignalStrength instanceof CellSignalStrengthTdscdma ||
                mSignalStrength instanceof CellSignalStrengthCdma) {
            return AccessNetworkType.CDMA2000;
        }

        if (mSignalStrength instanceof CellSignalStrengthNr) {
            return AccessNetworkType.NGRAN;
        }
        if (mSignalStrength instanceof CellSignalStrengthLte) {
            return AccessNetworkType.EUTRAN;
        }
        if (mSignalStrength instanceof CellSignalStrengthWcdma) {
            return AccessNetworkType.UTRAN;
        }
        if (mSignalStrength instanceof CellSignalStrengthGsm) {
            return AccessNetworkType.GERAN;
        }
        return AccessNetworkType.UNKNOWN;
    }

    private CellularParameters getCurrentFromSignalStrength() {
        if (mSignalStrength instanceof CellSignalStrengthTdscdma) {
            CellSignalStrengthTdscdma strength = (CellSignalStrengthTdscdma)mSignalStrength;
            return new CellularParameters(strength.getDbm());
        } else if (mSignalStrength instanceof CellSignalStrengthCdma) {
            CellSignalStrengthCdma strength = (CellSignalStrengthCdma)mSignalStrength;
            return new CellularParameters(strength.getDbm());
        } else if (mSignalStrength instanceof CellSignalStrengthNr) {
            CellSignalStrengthNr strength = (CellSignalStrengthNr)mSignalStrength;
            return new CellularParameters(strength.getDbm(), strength.getSsSinr(), strength.getSsRsrq());
        } else if (mSignalStrength instanceof CellSignalStrengthLte) {
            CellSignalStrengthLte strength = (CellSignalStrengthLte)mSignalStrength;
            return new CellularParameters(strength.getRsrp(), strength.getRssnr(), strength.getRsrq());
        } else if (mSignalStrength instanceof CellSignalStrengthWcdma) {
            CellSignalStrengthWcdma strength = (CellSignalStrengthWcdma)mSignalStrength;
            return new CellularParameters(strength.getDbm(), strength.getEcNo(), 0);
        } else if (mSignalStrength instanceof CellSignalStrengthGsm) {
            CellSignalStrengthGsm strength = (CellSignalStrengthGsm)mSignalStrength;
            return new CellularParameters(strength.getDbm());
        }
        return null;
    }

    private boolean isRoveIn(CellularParameters parameters) {
        if (!mSignalStrength.isValid()) {
            return false;
        }
        CellularParameters current = getCurrentFromSignalStrength();
        if (current == null) {
            return false;
        }
        LogUtils.v(TAG, LogUtils.LOG_CONDITIONS,
                "Current " + TelephonyManager.getNetworkTypeName(getNetworkType()) + " signal=" + current.getSignal() +
                " (good=" + parameters.getSignalThreshold() + ") snr=" + current.getNoise() + " (good=" + parameters.getNoiseThreshold() +
                ") rq=" + current.getQuality() + " (good=" + parameters.getQualityThreshold() + ")");
        return current.isBetter(parameters);
    }

    public boolean isRoveIn() {
        return isRoveIn(getSignalRoveIn(getAccessTechFromSignalStrength()));
    }

    public boolean isRoveIn(boolean hysteresis) {
        if (hysteresis) {
            return isRoveIn(getSignalRoveInHysteresisOn(getAccessTechFromSignalStrength()));
        } else {
            return isRoveIn();
        }
    }

    public boolean isRoveIn(int mode) {
        if (mode == WIFI_MODE_WIFI_PREFERRED) {
            return isRoveIn(getSignalRoveInWiFiPreferred(getAccessTechFromSignalStrength()));
        }
        return isRoveIn(getSignalRoveInCellPreferred(getAccessTechFromSignalStrength()));
    }

    /* in condition with call */
    private boolean isHandIn(CellularParameters parameters) {
        if (!mSignalStrength.isValid()) {
            return false;
        }
        CellularParameters current = getCurrentFromSignalStrength();
        if (current == null) {
            return false;
        }
        LogUtils.v(TAG, LogUtils.LOG_CONDITIONS,
                "Current " + TelephonyManager.getNetworkTypeName(getNetworkType()) + " signal=" + current.getSignal() +
                " (good_in_call=" + parameters.getSignalThreshold() + ") snr=" + current.getNoise() + " (good_in_call=" + parameters.getNoiseThreshold() +
                ") rq=" + current.getQuality() + " (good_in_call=" + parameters.getQualityThreshold() + ")");
        return current.isBetter(parameters);
    }

    public boolean isHandIn() {
        return isHandIn(getSignalHandIn(getAccessTechFromSignalStrength()));
    }

    public boolean isHandIn(boolean hysteresis) {
        if (hysteresis) {
            return isHandIn(getSignalHandInHysteresisOn(getAccessTechFromSignalStrength()));
        } else {
            return isHandIn();
        }
    }

    public boolean isHandIn(int mode) {
        if (mode == WIFI_MODE_WIFI_PREFERRED) {
            return isHandIn(getSignalHandInWiFiPreferred(getAccessTechFromSignalStrength()));
        }
        return isHandIn(getSignalHandInCellPreferred(getAccessTechFromSignalStrength()));
    }

    private boolean isRoveOut(CellularParameters parameters) {
        if (!mSignalStrength.isValid()) {
            return true;
        }
        CellularParameters current = getCurrentFromSignalStrength();
        if (current == null) {
            return true;
        }
        LogUtils.v(TAG, LogUtils.LOG_CONDITIONS,
                "Current " + TelephonyManager.getNetworkTypeName(getNetworkType()) + " signal=" + current.getSignal() +
                " (bad=" + parameters.getSignalThreshold() + ") snr=" + current.getNoise() + " (bad=" + parameters.getNoiseThreshold() +
                ") rq=" + current.getQuality() + " (bad=" + parameters.getQualityThreshold() + ")");
        return current.isWorse(parameters);
    }

    public boolean isRoveOut() {
        return isRoveOut(getSignalRoveOut(getAccessTechFromSignalStrength()));
    }

    public boolean isRoveOut(boolean hysteresis) {
        if (hysteresis) {
            return isRoveOut(getSignalRoveOutHysteresisOn(getAccessTechFromSignalStrength()));
        } else {
            return isRoveOut();
        }
    }

    public boolean isRoveOut(int mode) {
        if (mode == WIFI_MODE_WIFI_PREFERRED) {
            return isRoveOut(getSignalRoveOutWiFiPreferred(getAccessTechFromSignalStrength()));
        }
        return isRoveOut(getSignalRoveOutCellPreferred(getAccessTechFromSignalStrength()));
    }

    /* out condition with call */
    private boolean isHandOut(CellularParameters parameters) {
        if (!mSignalStrength.isValid()) {
            return true;
        }
        CellularParameters current = getCurrentFromSignalStrength();
        if (current == null) {
            return true;
        }
        LogUtils.v(TAG, LogUtils.LOG_CONDITIONS,
                "Current " + TelephonyManager.getNetworkTypeName(getNetworkType()) + " signal=" + current.getSignal() +
                " (bad_in_call=" + parameters.getSignalThreshold() + ") snr=" + current.getNoise() + " (bad_in_call=" + parameters.getNoiseThreshold() +
                ") rq=" + current.getQuality() + " (bad_in_call=" + parameters.getQualityThreshold() + ")");
        return current.isWorse(parameters);
    }

    public boolean isHandOut() {
        return isHandOut(getSignalHandOut(getAccessTechFromSignalStrength()));
    }

    public boolean isHandOut(boolean hysteresis) {
        if (hysteresis) {
            return isHandOut(getSignalHandOutHysteresisOn(getAccessTechFromSignalStrength()));
        } else {
            return isHandOut();
        }
    }

    public boolean isHandOut(int mode) {
        if (mode == WIFI_MODE_WIFI_PREFERRED) {
            return isHandOut(getSignalHandOutWiFiPreferred(getAccessTechFromSignalStrength()));
        }
        return isHandOut(getSignalHandOutCellPreferred(getAccessTechFromSignalStrength()));
    }

    public boolean isRoveOutMid() {
        if (!mSignalStrength.isValid()) {
            return true;
        }
        int strengthCurrent = 0;
        int qualityCurrent = 0;
        CellularParameters parameters = getSignalMid(getAccessTechFromSignalStrength());
        if (mSignalStrength instanceof CellSignalStrengthNr) {
            CellSignalStrengthNr strength = (CellSignalStrengthNr)mSignalStrength;
            strengthCurrent = strength.getDbm();
            qualityCurrent = strength.getSsRsrq();
        } else if (mSignalStrength instanceof CellSignalStrengthLte) {
            CellSignalStrengthLte strength = (CellSignalStrengthLte)mSignalStrength;
            strengthCurrent = strength.getRsrp();
            qualityCurrent = strength.getRsrq();
        } else {
            return false;
        }
        LogUtils.v(TAG, LogUtils.LOG_CONDITIONS,
                "Current " + TelephonyManager.getNetworkTypeName(getNetworkType()) + " signal=" + strengthCurrent +
                " (bad=" + parameters.getSignalThreshold() + ") rq=" + qualityCurrent + " (bad=" + parameters.getQualityThreshold() + ")");

        return (strengthCurrent <= parameters.getSignalThreshold()) ||
                (parameters.getQualityThreshold() != 0 && qualityCurrent != 0 && qualityCurrent <= parameters.getQualityThreshold());
    }

    public int getRsrp() {
        if (!mSignalStrength.isValid()) {
            return MIN_LTE_RSRP;
        }
        return mSignalStrength.getDbm();
    }

    public boolean isLowerThanThreshold(int threshold) {
        if (!mSignalStrength.isValid()) {
            return true;
        }
        return (mSignalStrength.getDbm() <= threshold);
    }

    public boolean isBetterThanThreshold(int threshold) {
        if (!mSignalStrength.isValid()) {
            return false;
        }
        return (mSignalStrength.getDbm() >= threshold);
    }

    public void stopCellReports() {
        if (mLastCellularThresholds != null) {
            mLastCellularThresholds.stopReports();
            LogUtils.d(TAG, LogUtils.LOG_CONDITIONS, "Sent " + mLastCellularThresholds);
            mLastCellularThresholds = null;
        }
    }

    private CellularParameters getSignalThresholds(boolean isIn, int accessTech, MapType mapType) {
        if (isIn) {
            switch (mapType) {
                case MAP_CALL: return getSignalHandIn(accessTech);
                case MAP_IDLE_WIFI: return getSignalRoveInWiFiPreferred(accessTech);
                case MAP_CALL_WIFI: return getSignalHandInWiFiPreferred(accessTech);
                case MAP_IDLE_CELL: return getSignalRoveInCellPreferred(accessTech);
                case MAP_CALL_CELL: return getSignalHandInCellPreferred(accessTech);
                case MAP_IDLE_ROAM: {
                    HashMap<MapType, CellularParameters> map = mSignalIn.get(accessTech);
                    if (map != null) {
                        CellularParameters cp = map.get(mapType);
                        if (cp != null) return cp;
                    }
                    return getSignalRoveIn(accessTech);
                }
                case MAP_CALL_ROAM: {
                    HashMap<MapType, CellularParameters> map = mSignalIn.get(accessTech);
                    if (map != null) {
                        CellularParameters cp = map.get(mapType);
                        if (cp != null) return cp;
                    }
                    return getSignalHandIn(accessTech);
                }
                case MAP_IDLE_TIMER: return getSignalRoveInHysteresisOn(accessTech);
                case MAP_CALL_TIMER: return getSignalHandInHysteresisOn(accessTech);
                case MAP_IDLE:
                    /* fall through */
                default:
                    return getSignalRoveIn(accessTech);
            }
        } else {
            switch (mapType) {
                case MAP_CALL: return getSignalHandOut(accessTech);
                case MAP_IDLE_WIFI: return getSignalRoveOutWiFiPreferred(accessTech);
                case MAP_CALL_WIFI: return getSignalHandOutWiFiPreferred(accessTech);
                case MAP_IDLE_CELL: return getSignalRoveOutCellPreferred(accessTech);
                case MAP_CALL_CELL: return getSignalHandOutCellPreferred(accessTech);
                case MAP_IDLE_ROAM: {
                    HashMap<MapType, CellularParameters> map = mSignalOut.get(accessTech);
                    if (map != null) {
                        CellularParameters cp = map.get(mapType);
                        if (cp != null) return cp;
                    }
                    return getSignalRoveOut(accessTech);
                }
                case MAP_CALL_ROAM: {
                    HashMap<MapType, CellularParameters> map = mSignalOut.get(accessTech);
                    if (map != null) {
                        CellularParameters cp = map.get(mapType);
                        if (cp != null) return cp;
                    }
                    return getSignalHandOut(accessTech);
                }
                case MAP_IDLE_TIMER: return getSignalRoveOutHysteresisOn(accessTech);
                case MAP_CALL_TIMER: return getSignalHandOutHysteresisOn(accessTech);
                case MAP_IDLE:
                    /* fall through */
                default:
                    return getSignalRoveOut(accessTech);
            }
        }
    }

    private class CellReportsRequest {
        final CellularParameters mUtranIn;
        final CellularParameters mUtranOut;
        final CellularParameters mEutranIn;
        final CellularParameters mEutranOut;
        final CellularParameters mNgranIn;
        final CellularParameters mNgranOut;
        final CellularParameters mGeranIn;
        final CellularParameters mGeranOut;

        CellReportsRequest(MapType type, boolean disableCellIn, boolean disableCellOut) {
            mUtranIn = disableCellIn ? null :
                    getSignalThresholds(true, AccessNetworkType.UTRAN, type);
            mUtranOut = disableCellOut ? null :
                    getSignalThresholds(false, AccessNetworkType.UTRAN, type);
            mEutranIn = disableCellIn ? null :
                    getSignalThresholds(true, AccessNetworkType.EUTRAN, type);
            mEutranOut = disableCellOut ? null :
                    getSignalThresholds(false, AccessNetworkType.EUTRAN, type);
            mNgranIn = disableCellIn ? null :
                    getSignalThresholds(true, AccessNetworkType.NGRAN, type);
            mNgranOut = disableCellOut ? null :
                    getSignalThresholds(false, AccessNetworkType.NGRAN, type);
            mGeranIn = disableCellIn ? null :
                    getSignalThresholds(true, AccessNetworkType.GERAN, type);
            mGeranOut = disableCellOut ? null :
                    getSignalThresholds(false, AccessNetworkType.GERAN, type);
        }

        boolean isValid() {
            return ((mUtranIn != null) || (mUtranOut != null) ||
                    (mEutranIn != null) || (mEutranOut != null) ||
                    (mNgranIn != null) || (mNgranOut != null) ||
                    (mGeranIn != null) || (mGeranOut != null));
        }
    }

    public void requestCellReports(boolean isWiFiPreferred, boolean isCall, boolean isHysteresis,
                                   boolean disableCellIn, boolean disableCellOut) {
        if (disableCellIn && disableCellOut) {
            stopCellReports();
            return;
        }
        CellReportsRequest request = null;
        do {
            if (isCall) {
                if (isHysteresis) {
                    request = new CellReportsRequest(MAP_CALL_TIMER, disableCellIn, disableCellOut);
                }
                if ((request != null) && request.isValid()) {
                    break;
                }
                if (isRoaming()) {
                    request = new CellReportsRequest(MAP_CALL_ROAM, disableCellIn, disableCellOut);
                }
                if ((request != null) && request.isValid()) {
                    break;
                }
                if (isWiFiPreferred) {
                    request = new CellReportsRequest(MAP_CALL_WIFI, disableCellIn, disableCellOut);
                } else {
                    request = new CellReportsRequest(MAP_CALL_CELL, disableCellIn, disableCellOut);
                }
                if (request.isValid()) {
                    break;
                }
                request = new CellReportsRequest(MAP_CALL, disableCellIn, disableCellOut);
            } else {
                if (isHysteresis) {
                    request = new CellReportsRequest(MAP_IDLE_TIMER, disableCellIn, disableCellOut);
                }
                if ((request != null) && request.isValid()) {
                    break;
                }
                if (isRoaming()) {
                    request = new CellReportsRequest(MAP_IDLE_ROAM, disableCellIn, disableCellOut);
                }
                if ((request != null) && request.isValid()) {
                    break;
                }
                if (isWiFiPreferred) {
                    request = new CellReportsRequest(MAP_IDLE_WIFI, disableCellIn, disableCellOut);
                } else {
                    request = new CellReportsRequest(MAP_IDLE_CELL, disableCellIn, disableCellOut);
                }
            }
            if (request.isValid()) {
                break;
            }
            request = new CellReportsRequest(MAP_IDLE, disableCellIn, disableCellOut);
        } while (false);

        if (mLastCellularThresholds != null) {
            if (!request.isValid()) {
                return;
            }
            RilSetCellularThresholds newThresholds = new RilSetCellularThresholds(mRilInterface);
            newThresholds.setUtranThresholdsIn(request.mUtranIn);
            newThresholds.setUtranThresholdsOut(request.mUtranOut);
            newThresholds.setEutranThresholdsIn(request.mEutranIn);
            newThresholds.setEutranThresholdsOut(request.mEutranOut);
            newThresholds.setNgranThresholdsIn(request.mNgranIn);
            newThresholds.setNgranThresholdsOut(request.mNgranOut);
            newThresholds.setGeranThresholdsIn(request.mGeranIn);
            newThresholds.setGeranThresholdsOut(request.mGeranOut);
            if (mLastCellularThresholds.equals(newThresholds)) {
                return;
            }
            mLastCellularThresholds = newThresholds;
            mLastCellularThresholds.startReports();
            LogUtils.d(TAG, LogUtils.LOG_CONDITIONS, "Sent " + mLastCellularThresholds);
        } else {
            mLastCellularThresholds = new RilSetCellularThresholds(mRilInterface);
            int accessTech = AccessNetworkType.UTRAN;
            if ((request.mUtranIn == null) && !disableCellIn) {
                mLastCellularThresholds.setUtranThresholdsIn(getSignalRoveIn(accessTech));
            } else {
                mLastCellularThresholds.setUtranThresholdsIn(request.mUtranIn);
            }
            if ((request.mUtranOut == null) && !disableCellOut) {
                mLastCellularThresholds.setUtranThresholdsOut(getSignalRoveOut(accessTech));
            } else {
                mLastCellularThresholds.setUtranThresholdsOut(request.mUtranOut);
            }
            accessTech = AccessNetworkType.EUTRAN;
            if ((request.mEutranIn == null) && !disableCellIn) {
                mLastCellularThresholds.setEutranThresholdsIn(getSignalRoveIn(accessTech));
            } else {
                mLastCellularThresholds.setEutranThresholdsIn(request.mEutranIn);
            }
            if ((request.mEutranOut == null) && !disableCellOut) {
                mLastCellularThresholds.setEutranThresholdsOut(getSignalRoveOut(accessTech));
            } else {
                mLastCellularThresholds.setEutranThresholdsOut(request.mEutranOut);
            }
            accessTech = AccessNetworkType.NGRAN;
            if ((request.mNgranIn == null) && !disableCellIn) {
                mLastCellularThresholds.setNgranThresholdsIn(getSignalRoveIn(accessTech));
            } else {
                mLastCellularThresholds.setNgranThresholdsIn(request.mNgranIn);
            }
            if ((request.mNgranOut == null) && !disableCellOut) {
                mLastCellularThresholds.setNgranThresholdsOut(getSignalRoveOut(accessTech));
            } else {
                mLastCellularThresholds.setNgranThresholdsOut(request.mNgranOut);
            }
            accessTech = AccessNetworkType.GERAN;
            if ((request.mGeranIn == null) && !disableCellIn) {
                mLastCellularThresholds.setGeranThresholdsIn(getSignalRoveIn(accessTech));
            } else {
                mLastCellularThresholds.setGeranThresholdsIn(request.mGeranIn);
            }
            if ((request.mGeranOut == null) && !disableCellOut) {
                mLastCellularThresholds.setGeranThresholdsOut(getSignalRoveOut(accessTech));
            } else {
                mLastCellularThresholds.setGeranThresholdsOut(request.mGeranOut);
            }
            mLastCellularThresholds.startReports();
            LogUtils.d(TAG, LogUtils.LOG_CONDITIONS, "Sent " + mLastCellularThresholds);
        }
    }

    private int getAirplaneMode() {
        int result = 0;
        try {
            result = Settings.Global.getInt(mContext.getContentResolver(), Settings.Global.AIRPLANE_MODE_ON, 0);
        } catch (Exception e) {
            LogUtils.e(TAG, "Failed to read airplane mode setting " + e);
        }
        return result;
    }

    public boolean isCsRegistered() {
        try {
            return mCsRegState.isInService();
        } catch (Exception e) {
            // nothing
        }
        return false;
    }

    public boolean isCsRoaming() {
        return (mCsRegState.getRegistrationState() == NetworkRegistrationInfo.REGISTRATION_STATE_ROAMING);
    }

    private String getParametersString(String input) {
        if (!TextUtils.isEmpty(input)) {
            String[] strings = input.split("\\{");
            if (strings.length > 1 && !TextUtils.isEmpty(strings[1])) {
                return strings[1].trim().substring(0, strings[1].length() - 1).trim();
            }
        }
        return "";
    }

    private CellularParameters processCdmaConfigurationString(String input) {
        if (TextUtils.isEmpty(input)) {
            return null;
        }
        String[] strings = input.split(";");
        for (String s : strings) {
            s = s.trim();

            if (s.startsWith("cdma") || s.startsWith("CDMA")) {
                String[] params = getParametersString(s).split("=");
                if (params.length > 1 && params[0].trim().equalsIgnoreCase("rssi")) {
                    try {
                        return new CellularParameters(Integer.parseInt(params[1].trim()));
                    } catch (Exception e) {
                        LogUtils.e(TAG, "Failed to parse CDMA configuration: " + e);
                    }
                }
            }
        }
        return null;
    }

    private CellularParameters processGeranConfigurationString(String input) {
        if (TextUtils.isEmpty(input)) {
            return null;
        }
        String[] strings = input.split(";");
        for (String s : strings) {
            s = s.trim();

            if (s.startsWith("geran") || s.startsWith("GERAN")) {
                String[] params = getParametersString(s).split("=");
                if (params.length > 1 && params[0].trim().equalsIgnoreCase("rssi")) {
                    try {
                        return new CellularParameters(Integer.parseInt(params[1].trim()));
                    } catch (Exception e) {
                        LogUtils.e(TAG, "Failed to parse GERAN configuration: " + e);
                    }
                }
            }
        }
        return null;
    }

    private CellularParameters processUtranConfigurationString(String input) {
        if (TextUtils.isEmpty(input)) {
            return null;
        }
        String[] outerStrings = input.split(";");
        for (String s : outerStrings) {
            s = s.trim();

            if (s.startsWith("utran") || s.startsWith("UTRAN")) {
                CellularParameters parameters = null;

                String[] innerStrings = getParametersString(s).split(",");
                for (String s2 : innerStrings) {
                    s2 = s2.trim();

                    String[] params = s2.split("=");
                    if (params.length > 1) {
                        try {
                            String n = params[0].trim();
                            if (n.equalsIgnoreCase("rscp")) {
                                if (parameters == null) {
                                    parameters = new CellularParameters(Integer.parseInt(params[1].trim()));
                                } else {
                                    parameters.setSignalThreshold(Integer.parseInt(params[1].trim()));
                                }
                            } else if (n.equalsIgnoreCase("ecno")) {
                                if (parameters == null) {
                                    parameters = new CellularParameters();
                                }
                                parameters.setNoiseThreshold(Integer.parseInt(params[1].trim()));
                            }
                        } catch (Exception e) {
                            LogUtils.e(TAG, "Failed to parse UTRAN configuration: " + e);
                        }
                    }
                }
                return parameters;
            }
        }
        return null;
    }

    private CellularParameters processEutranConfigurationString(String input) {
        if (TextUtils.isEmpty(input)) {
            return null;
        }
        String[] outerStrings = input.split(";");
        for (String s : outerStrings) {
            s = s.trim();

            if (s.startsWith("eutran") || s.startsWith("EUTRAN")) {
                CellularParameters parameters = null;

                String[] innerStrings = getParametersString(s).split(",");
                for (String s2 : innerStrings) {
                    s2 = s2.trim();

                    String[] params = s2.split("=");
                    if (params.length > 1) {
                        try {
                            String n = params[0].trim();
                            if (n.equalsIgnoreCase("rsrp")) {
                                if (parameters == null) {
                                    parameters = new CellularParameters(Integer.parseInt(params[1].trim()));
                                } else {
                                    parameters.setSignalThreshold(Integer.parseInt(params[1].trim()));
                                }
                            } else if (n.equalsIgnoreCase("rssnr")) {
                                if (parameters == null) {
                                    parameters = new CellularParameters();
                                }
                                parameters.setNoiseThreshold(Integer.parseInt(params[1].trim()));
                            } else if (n.equalsIgnoreCase("rsrq")) {
                                if (parameters == null) {
                                    parameters = new CellularParameters();
                                }
                                parameters.setQualityThreshold(Integer.parseInt(params[1].trim()));
                            }
                        } catch (Exception e) {
                            LogUtils.e(TAG, "Failed to parse EUTRAN configuration: " + e);
                        }
                    }
                }
                return parameters;
            }
        }
        return null;
    }

    private CellularParameters processNgranConfigurationString(String input) {
        if (TextUtils.isEmpty(input)) {
            return null;
        }
        String[] outerStrings = input.split(";");
        for (String s : outerStrings) {
            s = s.trim();

            if (s.startsWith("ngran") || s.startsWith("NGRAN")) {
                CellularParameters parameters = null;

                String[] innerStrings = getParametersString(s).split(",");
                for (String s2 : innerStrings) {
                    s2 = s2.trim();

                    String[] params = s2.split("=");
                    if (params.length > 1) {
                        try {
                            String n = params[0].trim();
                            if (n.equalsIgnoreCase("rsrp")) {
                                if (parameters == null) {
                                    parameters = new CellularParameters(Integer.parseInt(params[1].trim()));
                                } else {
                                    parameters.setSignalThreshold(Integer.parseInt(params[1].trim()));
                                }
                            } else if (n.equalsIgnoreCase("rssnr")) {
                                if (parameters == null) {
                                    parameters = new CellularParameters();
                                }
                                parameters.setNoiseThreshold(Integer.parseInt(params[1].trim()));
                            } else if (n.equalsIgnoreCase("rsrq")) {
                                if (parameters == null) {
                                    parameters = new CellularParameters();
                                }
                                parameters.setQualityThreshold(Integer.parseInt(params[1].trim()));
                            }
                        } catch (Exception e) {
                            LogUtils.e(TAG, "Failed to parse NGRAN configuration: " + e);
                        }
                    }
                }
                return parameters;
            }
        }
        return null;
    }

    private HashMap<MapType, CellularParameters> getCdmaHashMap(String defaultString,
                                                                String idle, String call,
                                                                String idleWiFi, String idleCell,
                                                                String callWiFi, String callCell,
                                                                String idleTimer, String callTimer) {
        HashMap<MapType, CellularParameters> map = new HashMap<>();
        CellularParameters parameters = processCdmaConfigurationString(defaultString);
        if (parameters != null) {
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "CDMA default " + parameters);
            map.put(MAP_DEFAULT, parameters);
        }
        parameters = processCdmaConfigurationString(idle);
        if (parameters != null) {
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "CDMA idle " + parameters);
            map.put(MAP_IDLE, parameters);
        }
        parameters = processCdmaConfigurationString(call);
        if (parameters != null) {
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "CDMA call " + parameters);
            map.put(MAP_CALL, parameters);
        }
        parameters = processCdmaConfigurationString(idleWiFi);
        if (parameters != null) {
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "CDMA idle WiFi " + parameters);
            map.put(MAP_IDLE_WIFI, parameters);
        }
        parameters = processCdmaConfigurationString(idleCell);
        if (parameters != null) {
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "CDMA idle cellular preferred " + parameters);
            map.put(MAP_IDLE_CELL, parameters);
        }
        parameters = processCdmaConfigurationString(callWiFi);
        if (parameters != null) {
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "CDMA call WiFi preferred " + parameters);
            map.put(MAP_CALL_WIFI, parameters);
        }
        parameters = processCdmaConfigurationString(callCell);
        if (parameters != null) {
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "CDMA call cellular preferred " + parameters);
            map.put(MAP_CALL_CELL, parameters);
        }
        parameters = processCdmaConfigurationString(idleTimer);
        if (parameters != null) {
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "CDMA idle with hysteresis timer on " + parameters);
            map.put(MAP_IDLE_TIMER, parameters);
        }
        parameters = processCdmaConfigurationString(callTimer);
        if (parameters != null) {
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "CDMA call with hysteresis timer on " + parameters);
            map.put(MAP_CALL_TIMER, parameters);
        }
        return map;
    }

    private HashMap<MapType, CellularParameters> getGeranHashMap(String defaultString,
                                                                String idle, String call,
                                                                String idleWiFi, String idleCell,
                                                                String callWiFi, String callCell,
                                                                String idleTimer, String callTimer) {
        HashMap<MapType, CellularParameters> map = new HashMap<>();
        CellularParameters parameters = processGeranConfigurationString(defaultString);
        if (parameters != null) {
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "GERAN default " + parameters);
            map.put(MAP_DEFAULT, parameters);
        }
        parameters = processGeranConfigurationString(idle);
        if (parameters != null) {
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "GERAN idle " + parameters);
            map.put(MAP_IDLE, parameters);
        }
        parameters = processGeranConfigurationString(call);
        if (parameters != null) {
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "GERAN call " + parameters);
            map.put(MAP_CALL, parameters);
        }
        parameters = processGeranConfigurationString(idleWiFi);
        if (parameters != null) {
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "GERAN idle WiFi preferred " + parameters);
            map.put(MAP_IDLE_WIFI, parameters);
        }
        parameters = processGeranConfigurationString(idleCell);
        if (parameters != null) {
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "GERAN idle cellular preferred " + parameters);
            map.put(MAP_IDLE_CELL, parameters);
        }
        parameters = processGeranConfigurationString(callWiFi);
        if (parameters != null) {
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "GERAN call WiFi preferred " + parameters);
            map.put(MAP_CALL_WIFI, parameters);
        }
        parameters = processGeranConfigurationString(callCell);
        if (parameters != null) {
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "GERAN call cellular preferred " + parameters);
            map.put(MAP_CALL_CELL, parameters);
        }
        parameters = processGeranConfigurationString(idleTimer);
        if (parameters != null) {
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "GERAN idle with hysteresis timer on " + parameters);
            map.put(MAP_IDLE_TIMER, parameters);
        }
        parameters = processGeranConfigurationString(callTimer);
        if (parameters != null) {
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "GERAN call with hysteresis timer on " + parameters);
            map.put(MAP_CALL_TIMER, parameters);
        }
        return map;
    }

    private HashMap<MapType, CellularParameters> getUtranHashMap(String defaultString,
                                                                 String idle, String call,
                                                                 String idleWiFi, String idleCell,
                                                                 String callWiFi, String callCell,
                                                                 String idleTimer, String callTimer) {
        HashMap<MapType, CellularParameters> map = new HashMap<>();
        CellularParameters parameters = processUtranConfigurationString(defaultString);
        if (parameters != null) {
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "UTRAN default " + parameters);
            map.put(MAP_DEFAULT, parameters);
        }
        parameters = processUtranConfigurationString(idle);
        if (parameters != null) {
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "UTRAN idle " + parameters);
            map.put(MAP_IDLE, parameters);
        }
        parameters = processUtranConfigurationString(call);
        if (parameters != null) {
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "UTRAN call " + parameters);
            map.put(MAP_CALL, parameters);
        }
        parameters = processUtranConfigurationString(idleWiFi);
        if (parameters != null) {
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "UTRAN idle WiFi preferred " + parameters);
            map.put(MAP_IDLE_WIFI, parameters);
        }
        parameters = processUtranConfigurationString(idleCell);
        if (parameters != null) {
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "UTRAN idle cellular preferred " + parameters);
            map.put(MAP_IDLE_CELL, parameters);
        }
        parameters = processUtranConfigurationString(callWiFi);
        if (parameters != null) {
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "UTRAN call WiFi preferred " + parameters);
            map.put(MAP_CALL_WIFI, parameters);
        }
        parameters = processUtranConfigurationString(callCell);
        if (parameters != null) {
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "UTRAN call cellular preferred " + parameters);
            map.put(MAP_CALL_CELL, parameters);
        }
        parameters = processUtranConfigurationString(idleTimer);
        if (parameters != null) {
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "UTRAN idle with hysteresis timer on " + parameters);
            map.put(MAP_IDLE_TIMER, parameters);
        }
        parameters = processUtranConfigurationString(callTimer);
        if (parameters != null) {
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "UTRAN call with hysteresis timer on " + parameters);
            map.put(MAP_CALL_TIMER, parameters);
        }
        return map;
    }

    private HashMap<MapType, CellularParameters> getEutranHashMap(String defaultString,
                                                                 String idle, String call,
                                                                 String idleWiFi, String idleCell,
                                                                 String callWiFi, String callCell,
                                                                 String idleTimer, String callTimer) {
        HashMap<MapType, CellularParameters> map = new HashMap<>();
        CellularParameters parameters = processEutranConfigurationString(defaultString);
        if (parameters != null) {
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "EUTRAN default " + parameters);
            map.put(MAP_DEFAULT, parameters);
        }
        parameters = processEutranConfigurationString(idle);
        if (parameters != null) {
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "EUTRAN idle " + parameters);
            map.put(MAP_IDLE, parameters);
        }
        parameters = processEutranConfigurationString(call);
        if (parameters != null) {
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "EUTRAN call " + parameters);
            map.put(MAP_CALL, parameters);
        }
        parameters = processEutranConfigurationString(idleWiFi);
        if (parameters != null) {
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "EUTRAN idle WiFi preferred " + parameters);
            map.put(MAP_IDLE_WIFI, parameters);
        }
        parameters = processEutranConfigurationString(idleCell);
        if (parameters != null) {
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "EUTRAN idle cellular preferred " + parameters);
            map.put(MAP_IDLE_CELL, parameters);
        }
        parameters = processEutranConfigurationString(callWiFi);
        if (parameters != null) {
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "EUTRAN call WiFi preferred " + parameters);
            map.put(MAP_CALL_WIFI, parameters);
        }
        parameters = processEutranConfigurationString(callCell);
        if (parameters != null) {
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "EUTRAN call cellular preferred " + parameters);
            map.put(MAP_CALL_CELL, parameters);
        }
        parameters = processEutranConfigurationString(idleTimer);
        if (parameters != null) {
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "EUTRAN idle with hysteresis timer on " + parameters);
            map.put(MAP_IDLE_TIMER, parameters);
        }
        parameters = processEutranConfigurationString(callTimer);
        if (parameters != null) {
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "EUTRAN call with hysteresis timer on " + parameters);
            map.put(MAP_CALL_TIMER, parameters);
        }
        return map;
    }

    private HashMap<MapType, CellularParameters> getNgranHashMap(String defaultString,
                                                                 String idle, String call,
                                                                 String idleWiFi, String idleCell,
                                                                 String callWiFi, String callCell,
                                                                 String idleTimer, String callTimer,
                                                                 HashMap<MapType, CellularParameters> defaultMap) {
        HashMap<MapType, CellularParameters> map = new HashMap<>();
        CellularParameters parameters = processNgranConfigurationString(idle);
        if (parameters != null) {
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "NGRAN idle " + parameters);
            map.put(MAP_IDLE, parameters);
        }
        parameters = processNgranConfigurationString(call);
        if (parameters != null) {
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "NGRAN call " + parameters);
            map.put(MAP_CALL, parameters);
        }
        parameters = processNgranConfigurationString(idleWiFi);
        if (parameters != null) {
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "NGRAN idle WiFi preferred " + parameters);
            map.put(MAP_IDLE_WIFI, parameters);
        }
        parameters = processNgranConfigurationString(idleCell);
        if (parameters != null) {
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "NGRAN idle cellular preferred " + parameters);
            map.put(MAP_IDLE_CELL, parameters);
        }
        parameters = processNgranConfigurationString(callWiFi);
        if (parameters != null) {
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "NGRAN call WiFi preferred " + parameters);
            map.put(MAP_CALL_WIFI, parameters);
        }
        parameters = processNgranConfigurationString(callCell);
        if (parameters != null) {
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "NGRAN call cellular preferred " + parameters);
            map.put(MAP_CALL_CELL, parameters);
        }
        parameters = processNgranConfigurationString(idleTimer);
        if (parameters != null) {
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "NGRAN idle with hysteresis timer on " + parameters);
            map.put(MAP_IDLE_TIMER, parameters);
        }
        parameters = processNgranConfigurationString(callTimer);
        if (parameters != null) {
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "NGRAN call with hysteresis timer on " + parameters);
            map.put(MAP_CALL_TIMER, parameters);
        }
        // Use LTE map if no thresholds defined for NR
        if (map.isEmpty() && defaultMap.size() > 1) {
            return defaultMap;
        }
        parameters = processNgranConfigurationString(defaultString);
        if (parameters != null) {
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "NGRAN default " + parameters);
            map.put(MAP_DEFAULT, parameters);
        }
        return map;
    }

    private void buildSignalHashMaps() {
        String defaultString = mConfigurationManager.getDefaultImsHandoverIn();
        String idleString = mConfigurationManager.getImsRoveIn();
        String callString = mConfigurationManager.getImsHandIn();
        String idleWiFiString = mConfigurationManager.getImsRoveInWiFiPreferred();
        String idleCellString = mConfigurationManager.getImsRoveInCellPreferred();
        String callWiFiString = mConfigurationManager.getImsHandInWiFiPreferred();
        String callCellString = mConfigurationManager.getImsHandInCellPreferred();
        String idleTimerString = mConfigurationManager.getImsRoveInHysteresisOn();
        String callTimerString = mConfigurationManager.getImsHandInHysteresisOn();

        mSignalIn = new SparseArray<>();
        HashMap<MapType, CellularParameters> map = getCdmaHashMap(defaultString,
                idleString, callString, idleWiFiString, idleCellString,
                callWiFiString, callCellString, idleTimerString, callTimerString);
        mSignalIn.put(AccessNetworkType.CDMA2000, map);
        map = getGeranHashMap(defaultString,
                idleString, callString, idleWiFiString, idleCellString,
                callWiFiString, callCellString, idleTimerString, callTimerString);
        mSignalIn.put(AccessNetworkType.GERAN, map);
        map = getUtranHashMap(defaultString,
                idleString, callString, idleWiFiString, idleCellString,
                callWiFiString, callCellString, idleTimerString, callTimerString);
        mSignalIn.put(AccessNetworkType.UTRAN, map);
        map = getEutranHashMap(defaultString,
                idleString, callString, idleWiFiString, idleCellString,
                callWiFiString, callCellString, idleTimerString, callTimerString);
        mSignalIn.put(AccessNetworkType.EUTRAN, map);
        map = getNgranHashMap(defaultString,
                idleString, callString, idleWiFiString, idleCellString,
                callWiFiString, callCellString, idleTimerString, callTimerString, map);
        mSignalIn.put(AccessNetworkType.NGRAN, map);

        defaultString = mConfigurationManager.getDefaultImsHandoverOut();
        idleString = mConfigurationManager.getImsRoveOut();
        callString = mConfigurationManager.getImsHandOut();
        idleWiFiString = mConfigurationManager.getImsRoveOutWiFiPreferred();
        idleCellString = mConfigurationManager.getImsRoveOutCellPreferred();
        callWiFiString = mConfigurationManager.getImsHandOutWiFiPreferred();
        callCellString = mConfigurationManager.getImsHandOutCellPreferred();
        idleTimerString = mConfigurationManager.getImsRoveOutHysteresisOn();
        callTimerString = mConfigurationManager.getImsHandOutHysteresisOn();

        mSignalOut = new SparseArray<>();
        map = getCdmaHashMap(defaultString,
                idleString, callString, idleWiFiString, idleCellString,
                callWiFiString, callCellString, idleTimerString, callTimerString);
        mSignalOut.put(AccessNetworkType.CDMA2000, map);
        map = getGeranHashMap(defaultString,
                idleString, callString, idleWiFiString, idleCellString,
                callWiFiString, callCellString, idleTimerString, callTimerString);
        mSignalOut.put(AccessNetworkType.GERAN, map);
        map = getUtranHashMap(defaultString,
                idleString, callString, idleWiFiString, idleCellString,
                callWiFiString, callCellString, idleTimerString, callTimerString);
        mSignalOut.put(AccessNetworkType.UTRAN, map);
        map = getEutranHashMap(defaultString,
                idleString, callString, idleWiFiString, idleCellString,
                callWiFiString, callCellString, idleTimerString, callTimerString);
        mSignalOut.put(AccessNetworkType.EUTRAN, map);
        map = getNgranHashMap(defaultString,
                idleString, callString, idleWiFiString, idleCellString,
                callWiFiString, callCellString, idleTimerString, callTimerString, map);
        mSignalOut.put(AccessNetworkType.NGRAN, map);

        String configString = mConfigurationManager.getImsCellMid();

        mSignalMid = new SparseArray<>();
        if (!TextUtils.isEmpty(configString)) {
            CellularParameters parameters = processEutranConfigurationString(configString);
            if (parameters != null) {
                mSignalMid.put(AccessNetworkType.EUTRAN, parameters);
            }
            parameters = processNgranConfigurationString(configString);
            if (parameters != null) {
                mSignalMid.put(AccessNetworkType.NGRAN, parameters);
            }
        }
    }

    public void onConfigurationChanged() {
        buildSignalHashMaps();
    }

    private CellularParameters getSignalRoveIn(int accessTech) {
        HashMap<MapType, CellularParameters> map = mSignalIn.get(accessTech);
        if (map != null) {
            CellularParameters parameters = map.get(MAP_IDLE);
            if (parameters != null) {
                return parameters;
            }
            parameters = map.get(MAP_DEFAULT);
            if (parameters != null) {
                return parameters;
            }
        }
        return new CellularParameters(GOOD_LTE_RSRP);
    }

    private CellularParameters getSignalRoveInHysteresisOn(int accessTech) {
        HashMap<MapType, CellularParameters> map = mSignalIn.get(accessTech);
        if (map != null) {
            CellularParameters parameters = map.get(MAP_IDLE_TIMER);
            if (parameters != null) {
                return parameters;
            }
        }
        return getSignalRoveIn(accessTech);
    }

    private CellularParameters getSignalRoveInWiFiPreferred(int accessTech) {
        HashMap<MapType, CellularParameters> map = mSignalIn.get(accessTech);
        if (map != null) {
            CellularParameters parameters = map.get(MAP_IDLE_WIFI);
            if (parameters != null) {
                return parameters;
            }
        }
        return getSignalRoveIn(accessTech);
    }

    private CellularParameters getSignalRoveInCellPreferred(int accessTech) {
        HashMap<MapType, CellularParameters> map = mSignalIn.get(accessTech);
        if (map != null) {
            CellularParameters parameters = map.get(MAP_IDLE_CELL);
            if (parameters != null) {
                return parameters;
            }
        }
        return getSignalRoveIn(accessTech);
    }

    private CellularParameters getSignalRoveOut(int accessTech) {
        HashMap<MapType, CellularParameters> map = mSignalOut.get(accessTech);
        if (map != null) {
            CellularParameters parameters = map.get(MAP_IDLE);
            if (parameters != null) {
                return parameters;
            }
            parameters = map.get(MAP_DEFAULT);
            if (parameters != null) {
                return parameters;
            }
        }
        return new CellularParameters(NOT_USABLE_LTE_RSRP);
    }

    private CellularParameters getSignalRoveOutHysteresisOn(int accessTech) {
        HashMap<MapType, CellularParameters> map = mSignalOut.get(accessTech);
        if (map != null) {
            CellularParameters parameters = map.get(MAP_IDLE_TIMER);
            if (parameters != null) {
                return parameters;
            }
        }
        return getSignalRoveOut(accessTech);
    }

    private CellularParameters getSignalRoveOutWiFiPreferred(int accessTech) {
        HashMap<MapType, CellularParameters> map = mSignalOut.get(accessTech);
        if (map != null) {
            CellularParameters parameters = map.get(MAP_IDLE_WIFI);
            if (parameters != null) {
                return parameters;
            }
        }
        return getSignalRoveOut(accessTech);
    }

    private CellularParameters getSignalRoveOutCellPreferred(int accessTech) {
        HashMap<MapType, CellularParameters> map = mSignalOut.get(accessTech);
        if (map != null) {
            CellularParameters parameters = map.get(MAP_IDLE_CELL);
            if (parameters != null) {
                return parameters;
            }
        }
        return getSignalRoveOut(accessTech);
    }

    private CellularParameters getSignalHandIn(int accessTech) {
        HashMap<MapType, CellularParameters> map = mSignalIn.get(accessTech);
        if (map != null) {
            CellularParameters parameters = map.get(MAP_CALL);
            if (parameters != null) {
                return parameters;
            }
        }
        return getSignalRoveIn(accessTech);
    }

    private CellularParameters getSignalHandInHysteresisOn(int accessTech) {
        HashMap<MapType, CellularParameters> map = mSignalIn.get(accessTech);
        if (map != null) {
            CellularParameters parameters = map.get(MAP_CALL_TIMER);
            if (parameters != null) {
                return parameters;
            }
        }
        return getSignalRoveInHysteresisOn(accessTech);
    }

    private CellularParameters getSignalHandInWiFiPreferred(int accessTech) {
        HashMap<MapType, CellularParameters> map = mSignalIn.get(accessTech);
        if (map != null) {
            CellularParameters parameters = map.get(MAP_CALL_WIFI);
            if (parameters != null) {
                return parameters;
            }
        }
        return getSignalRoveInWiFiPreferred(accessTech);
    }

    private CellularParameters getSignalHandInCellPreferred(int accessTech) {
        HashMap<MapType, CellularParameters> map = mSignalIn.get(accessTech);
        if (map != null) {
            CellularParameters parameters = map.get(MAP_CALL_CELL);
            if (parameters != null) {
                return parameters;
            }
        }
        return getSignalRoveInCellPreferred(accessTech);
    }

    private CellularParameters getSignalHandOut(int accessTech) {
        HashMap<MapType, CellularParameters> map = mSignalOut.get(accessTech);
        if (map != null) {
            CellularParameters parameters = map.get(MAP_CALL);
            if (parameters != null) {
                return parameters;
            }
        }
        return getSignalRoveOut(accessTech);
    }

    private CellularParameters getSignalHandOutHysteresisOn(int accessTech) {
        HashMap<MapType, CellularParameters> map = mSignalOut.get(accessTech);
        if (map != null) {
            CellularParameters parameters = map.get(MAP_CALL_TIMER);
            if (parameters != null) {
                return parameters;
            }
        }
        return getSignalRoveOutHysteresisOn(accessTech);
    }

    private CellularParameters getSignalHandOutWiFiPreferred(int accessTech) {
        HashMap<MapType, CellularParameters> map = mSignalOut.get(accessTech);
        if (map != null) {
            CellularParameters parameters = map.get(MAP_CALL_WIFI);
            if (parameters != null) {
                return parameters;
            }
        }
        return getSignalRoveOutWiFiPreferred(accessTech);
    }

    private CellularParameters getSignalHandOutCellPreferred(int accessTech) {
        HashMap<MapType, CellularParameters> map = mSignalOut.get(accessTech);
        if (map != null) {
            CellularParameters parameters = map.get(MAP_CALL_CELL);
            if (parameters != null) {
                return parameters;
            }
        }
        return getSignalRoveOutCellPreferred(accessTech);
    }

    private CellularParameters getSignalMid(int accessTech) {
        CellularParameters parameters = mSignalMid.get(accessTech);
        if (parameters != null) {
            return parameters;
        }
        return new CellularParameters(NOT_USABLE_LTE_RSRP);
    }

    public int getRsrpRoveIn() {
        CellularParameters parameters = getSignalRoveIn(AccessNetworkType.EUTRAN);
        return parameters.getSignalThreshold();
    }

    public int getRsrpRoveOut() {
        CellularParameters parameters = getSignalRoveOut(AccessNetworkType.EUTRAN);
        return parameters.getSignalThreshold();
    }

    public int getRsrpMid() {
        CellularParameters parameters = getSignalMid(AccessNetworkType.EUTRAN);
        return parameters.getSignalThreshold();
    }

    public void setRsrpRoveIn(int rsrp) {
        HashMap<MapType, CellularParameters> map = mSignalIn.get(AccessNetworkType.EUTRAN);
        if (map != null) {
            CellularParameters parameters = map.get(MAP_IDLE);
            if (parameters != null) {
                parameters.setSignalThreshold(rsrp);

                // put updated parameters back
                map.put(MAP_IDLE, parameters);
                mSignalIn.put(AccessNetworkType.EUTRAN, map);
            }
        }
    }

    public void setRsrpRoveOut(int rsrp) {
        HashMap<MapType, CellularParameters> map = mSignalOut.get(AccessNetworkType.EUTRAN);
        if (map != null) {
            CellularParameters parameters = map.get(MAP_IDLE);
            if (parameters != null) {
                parameters.setSignalThreshold(rsrp);

                // put updated parameters back
                map.put(MAP_IDLE, parameters);
                mSignalOut.put(AccessNetworkType.EUTRAN, map);
            }
        }
    }

    public void setRsrpMid(int rsrp) {
        CellularParameters parameters = mSignalMid.get(AccessNetworkType.EUTRAN);
        if (parameters != null) {
            parameters.setSignalThreshold(rsrp);
            // put updated parameters back
            mSignalMid.put(AccessNetworkType.EUTRAN, parameters);
        }
    }

    private boolean isSignalStrengthMatchedToCurrentRat(CellSignalStrength cellStrength) {
        int networkType = getNetworkType();
        /* if PS network type is invalid, we should use CS network type */
        if (networkType == TelephonyManager.NETWORK_TYPE_UNKNOWN) {
            networkType = mCsRegState.getAccessNetworkTechnology();
        }
        switch (networkType) {
            case TelephonyManager.NETWORK_TYPE_CDMA:
            case TelephonyManager.NETWORK_TYPE_1xRTT:
            case TelephonyManager.NETWORK_TYPE_EVDO_0:
            case TelephonyManager.NETWORK_TYPE_EVDO_A:
            case TelephonyManager.NETWORK_TYPE_EVDO_B:
            case TelephonyManager.NETWORK_TYPE_EHRPD:
                return (cellStrength instanceof CellSignalStrengthCdma);

            case TelephonyManager.NETWORK_TYPE_TD_SCDMA:
                return (cellStrength instanceof CellSignalStrengthTdscdma);

            case TelephonyManager.NETWORK_TYPE_GPRS:
            case TelephonyManager.NETWORK_TYPE_EDGE:
            case TelephonyManager.NETWORK_TYPE_GSM:
                return (cellStrength instanceof CellSignalStrengthGsm);

            case TelephonyManager.NETWORK_TYPE_UMTS:
            case TelephonyManager.NETWORK_TYPE_HSDPA:
            case TelephonyManager.NETWORK_TYPE_HSUPA:
            case TelephonyManager.NETWORK_TYPE_HSPA:
            case TelephonyManager.NETWORK_TYPE_HSPAP:
                return (cellStrength instanceof CellSignalStrengthWcdma);

            case TelephonyManager.NETWORK_TYPE_LTE:
            case TelephonyManager.NETWORK_TYPE_LTE_CA:
                return (cellStrength instanceof CellSignalStrengthLte);

            case TelephonyManager.NETWORK_TYPE_NR:
                return (cellStrength instanceof CellSignalStrengthNr);

            default:
                return false;
        }
    }

    private CellSignalStrength getLastSignalStrength() {
        SignalStrength signalStrength = mTelephonyManager.getSignalStrength();
        if (signalStrength != null) {
            List<CellSignalStrength> list = signalStrength.getCellSignalStrengths();
            if (list.isEmpty()) {
                return null;
            }
            for (CellSignalStrength strength : list) {
                if (strength.isValid() && isSignalStrengthMatchedToCurrentRat(strength)) {
                    return strength;
                }
            }
        }
        return null;
    }

    @Override
    public String toString() {
        return ("CELL" + mSlotId + ": RegState=" + mPsRegState.registrationStateToString(mPsRegState.getRegistrationState()) +
                " RAT=" + TelephonyManager.getNetworkTypeName(getNetworkType()) +
                " Voice=" + isVoiceSupported() + " Emergency=" + isEmergencySupported() +
                " strength=" + mSignalStrength);
    }

    @VisibleForTesting
    void setSignalStrength(CellSignalStrength strength) { mSignalStrength = strength; }

    @VisibleForTesting
    CellBroadcastReceiver getBroadcastReceiver() { return mBroadcastReceiver; }

    @VisibleForTesting
    Handler getHandler() { return mHandler; }

    @VisibleForTesting
    CellularStateListener getStateListener() { return mStateListener; }
}
