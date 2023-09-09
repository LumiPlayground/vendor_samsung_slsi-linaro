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

import android.content.Context;
import android.os.Handler;
import android.os.Message;
import android.telephony.TelephonyManager;
import android.telephony.data.ApnSetting;
import android.telephony.data.ThrottleStatus;
import android.util.SparseArray;

import com.android.internal.annotations.VisibleForTesting;
import com.shannon.qualifiednetworksservice.ApnQualifiedNetworksProvider.ApnConditionsHandler;
import com.shannon.qualifiednetworksservice.util.LogUtils;

import java.util.ArrayList;
import java.util.List;

public class HandoverConditions {
    public static final int CONDITIONS_CHANGED = 1;
    public static final int CONFIGURATION_CHANGED = 2;

    public enum ConditionEvents {
        EVENT_AIRPLANE_MODE_CHANGED,
        EVENT_APN_CONNECTED,
        EVENT_APN_CONNECTING,
        EVENT_APN_HANDOVER_COMPLETED,
        EVENT_APN_DISCONNECTED,
        EVENT_APN_FAILURE,
        EVENT_APN_TRANSPORT_AVAILABLE,
        EVENT_APN_IMS_CONNECTION_CHANGED,
        EVENT_APN_UNTHROTTLED,
        EVENT_ATTACH_TIMER_EXPIRED,
        EVENT_BARRING_INFO_CHANGED,
        EVENT_CALL_STARTED,
        EVENT_CALL_RELEASED,
        EVENT_CALL_LAST_FAIL_CAUSE,
        EVENT_CARRIER_CONFIG_CHANGED,
        EVENT_CELL_SIGNAL_STRENGTH_CHANGED,
        EVENT_CONFIGURATION_CHANGED,
        EVENT_CST_SETTINGS_CHANGED,
        EVENT_DEFAULT_LINK_PROPERTIES_CHANGED,
        EVENT_DESTINATION_UNREACHABLE,
        EVENT_EMERGENCY_CALLBACK_END,
        EVENT_IMS_CALL_STARTED,
        EVENT_IMS_CALL_RELEASED,
        EVENT_IMS_CALL_UPGRADED,
        EVENT_IMS_CALL_DOWNGRADED,
        EVENT_IMS_CALL_SRVCC,
        EVENT_IMS_CALL_RTP_EVENT,
        EVENT_IMS_CALL_RTP_TIMEOUT,
        EVENT_IMS_CONFIG_CONNECTED,
        EVENT_IMS_REGISTRATION_COMPLETED,
        EVENT_IMS_REGISTRATION_TIMER_EXPIRED,
        EVENT_WLAN_PS_STATE_CHANGED,
        EVENT_INIT,
        EVENT_HYSTERESIS_TIMER_EXPIRED,
        EVENT_MOBILE_CONNECTED,
        EVENT_MOBILE_DISCONNECTED,
        EVENT_MOBILE_SIGNAL_STRENGTH_CHANGED,
        EVENT_OPERATOR_CHANGED,
        EVENT_PROVISIONED_VALUE_CHANGED,
        EVENT_RAT_CHANGED,
        EVENT_PHONE_SERVICE_STATE_CHANGED,
        EVENT_THROTTLE_TIMER_EXPIRED,
        EVENT_UNKNOWN,
        EVENT_VLT_SETTINGS_CHANGED,
        EVENT_WFC_MODE_CHANGED,
        EVENT_WFC_SETTINGS_CHANGED,
        EVENT_WFC_SETTINGS_BROADCAST,
        EVENT_WIFI_CONNECTED,
        EVENT_WIFI_DISCONNECTED,
        EVENT_WIFI_AP_CHANGED,
        EVENT_WIFI_AP_HANDOVER_TIMER_EXPIRED,
        EVENT_WIFI_BUTTON_STATE_CHANGED,
        EVENT_WIFI_CAPABILITIES_CHANGED,
        EVENT_WIFI_LINK_PROPERTIES_CHANGED,
        EVENT_WIFI_SIGNAL_STRENGTH_CHANGED;
    }

    public interface IConditionChanged {
        public void onConditionChanged(ConditionEvents event);
        public void onConditionChanged(int apnBitmask, ConditionEvents event);
        public void onConditionChanged(int apnBitmask, ConditionEvents event, Object object);
    }

    public interface IConditionHelper {
        public boolean isAirplaneMode();
        public boolean isRoaming();
        public int getRsrpRoveIn();
        public void setRsrpRoveIn(int value);
        public int getRsrpRoveOut();
        public void setRsrpRoveOut(int value);
        public int getRsrpMid();
        public void setRsrpMid(int value);
        public int getCellularNetworkType();
        public void setDisconnectedFromNetwork(boolean value);
        public int getWiFiRssiRoveIn();
        public void setWiFiRssiRoveIn(int value);
        public int getWiFiRssiRoveOut();
        public void setWiFiRssiRoveOut(int value);
        public void setWfcEnabledRoaming(boolean value);
        public boolean isWfcEnabledByPlatform();
        public void clearLastCallFailCause(int networkType);
        public boolean isWiFiTransportNotUsed();
        public boolean isImsConnectedOnWiFi();
        public boolean isInHandoverState(int apnType);
    }

    public static class EventInfo {
        private ConditionEvents mEvent;
        private Object mInfo;

        public EventInfo(ConditionEvents event, Object object) {
            mEvent = event;
            mInfo = object;
        }

        public ConditionEvents getEvent() { return mEvent; }
        public Object getInfo() { return mInfo; }
    }

    protected class HandoverConditionsCallback implements IConditionChanged {
        public void onConditionChanged(ConditionEvents event) {
            updateAllListeners(event);
        }

        public void onConditionChanged(int apnBitmask, ConditionEvents event) {
            updateListeners(apnBitmask, event);
        }

        public void onConditionChanged(int apnBitmask, ConditionEvents event, Object object) {
            updateListeners(apnBitmask, event, object);
        }
    }

    protected class HandoverConditionsHelper implements IConditionHelper {
        public boolean isAirplaneMode() {
            if (mCellConditions != null) {
                return mCellConditions.isAirplaneMode();
            }
            return false;
        }

        public boolean isRoaming() {
            if (mCellConditions != null) {
                if (mCellConditions.isRegistered()) {
                    return mCellConditions.isRoaming();
                }
                if (mCellConditions.isCsRegistered()) {
                    return mCellConditions.isCsRoaming();
                }
            }
            if (mWiFiCondition != null) {
                return mWiFiCondition.isRoaming();
            }
            return false;
        }

        public int getRsrpRoveIn() {
            if (mCellConditions != null) {
                return mCellConditions.getRsrpRoveIn();
            }
            return (-1);
        }

        public void setRsrpRoveIn(int value) {
            if (mCellConditions != null) {
                mCellConditions.setRsrpRoveIn(value);
            }
        }

        public int getRsrpRoveOut() {
            if (mCellConditions != null) {
                return mCellConditions.getRsrpRoveOut();
            }
            return (-1);
        }

        public void setRsrpRoveOut(int value) {
            if (mCellConditions != null) {
                mCellConditions.setRsrpRoveOut(value);
            }
        }

        public int getRsrpMid() {
            if (mCellConditions != null) {
                return mCellConditions.getRsrpMid();
            }
            return (-1);
        }

        public void setRsrpMid(int value) {
            if (mCellConditions != null) {
                mCellConditions.setRsrpMid(value);
            }
        }

        public int getCellularNetworkType() {
            if (mCellConditions != null) {
                return mCellConditions.getNetworkType();
            }
            return TelephonyManager.NETWORK_TYPE_UNKNOWN;
        }

        public void setDisconnectedFromNetwork(boolean value) {
            if (mCellConditions != null) {
                mCellConditions.setDisconnectedFromNetwork(value);
            }
        }

        public int getWiFiRssiRoveIn() {
            if (mWiFiCondition != null) {
                return mWiFiCondition.getRssiRoveIn();
            }
            return (-1);
        }

        public void setWiFiRssiRoveIn(int value) {
            if (mWiFiCondition != null) {
                mWiFiCondition.setRssiRoveIn(value);
            }
        }

        public int getWiFiRssiRoveOut() {
            if (mWiFiCondition != null) {
                return mWiFiCondition.getRssiRoveOut();
            }
            return (-1);
        }

        public void setWiFiRssiRoveOut(int value) {
            if (mWiFiCondition != null) {
                mWiFiCondition.setRssiRoveOut(value);
            }
        }

        public void setWfcEnabledRoaming(boolean value) {
            if (mUserSettings != null) {
                mUserSettings.setWfcEnabledRoaming(value);
            }
        }

        public boolean isWfcEnabledByPlatform() {
            if (mUserSettings != null) {
                return mUserSettings.isWfcEnabledByPlatform();
            }
            return false;
        }

        public void clearLastCallFailCause(int networkType) {
            if (mCallConditions != null) {
                mCallConditions.clearLastCallFailCause(networkType);
            }
        }

        public boolean isWiFiTransportNotUsed() {
            if (mTransportCondition != null) {
                return !mTransportCondition.isWiFiTransportUsed();
            }
            return false;
        }

        public boolean isImsConnectedOnWiFi() {
            if (mTransportCondition != null) {
                return (mTransportCondition.getNetworkType(ApnSetting.TYPE_IMS) ==
                        TelephonyManager.NETWORK_TYPE_IWLAN);
            }
            return false;
        }

        public boolean isInHandoverState(int apnType) {
            if (mDecisionArray.size() > 0) {
                HandoverDecision decision = mDecisionArray.get(apnType);
                if (decision != null) {
                    return decision.isInHandoverState();
                }
            }
            return false;
        }
    }

    private final String TAG;
    private final int mSlotId;
    private final Context mContext;
    private final SimManager mSimManager;
    private final ConfigurationManager mConfigurationManager;
    private final RilOemInterface mRilInterface;
    private ConditionCellular mCellConditions = null;
    private ConditionWiFi mWiFiCondition = null;
    private ConditionCall mCallConditions = null;
    private ConditionUserSetting mUserSettings = null;
    private ConditionProvisioning mProvisionedValues = null;
    private ConditionMobile mMobileConditions = null;
    private ConditionConnectivity mTransportCondition = null;
    private final List<ApnConditionsHandler> mListenersList;
    private final SparseArray<HandoverDecision> mDecisionArray;

    public HandoverConditions(int slotIndex, Context context, ConfigurationManager configManager) {
        mSlotId = slotIndex;
        mContext = context;
        mSimManager = SimManager.getInstance();
        mConfigurationManager = configManager;
        mListenersList = new ArrayList<>();
        mRilInterface = new RilOemInterface(mSlotId);
        mDecisionArray = new SparseArray<>();

        HandoverConditionsCallback callback = new HandoverConditionsCallback();
        HandoverConditionsHelper helper = new HandoverConditionsHelper();

        TAG = ShannonQualifiedNetworksService.getServiceTAG("-" + mSlotId + "-HC");

        mUserSettings = new ConditionUserSetting(mSlotId, mContext, mConfigurationManager, helper, callback);
        mCellConditions = new ConditionCellular(mSlotId, mContext, mRilInterface, mConfigurationManager, callback);
        mWiFiCondition = new ConditionWiFi(mSlotId, mContext, mConfigurationManager, helper, callback);
        mProvisionedValues = new ConditionProvisioning(mSlotId, mContext, mConfigurationManager, helper, callback);
        mCallConditions = new ConditionCall(mSlotId, mContext, mRilInterface, mConfigurationManager, helper, callback);
        mMobileConditions = new ConditionMobile(mSlotId, mContext, mConfigurationManager, callback);
        mTransportCondition = new ConditionConnectivity(mSlotId, mContext, mRilInterface, mConfigurationManager,
                helper, callback);
    }

    @VisibleForTesting
    protected HandoverConditions(int slotIndex, Context context, ConfigurationManager configManager, RilOemInterface oemInterface,
        ConditionUserSetting condUserSetting, ConditionCellular condCellular, ConditionWiFi condWiFi, ConditionProvisioning CondProvisioning,
        ConditionCall condCall, ConditionMobile condMobile, ConditionConnectivity condConnectivity) {
        mSlotId = slotIndex;
        mContext = context;
        mSimManager = SimManager.getInstance();
        mConfigurationManager = configManager;
        mListenersList = new ArrayList<>();
        mRilInterface = oemInterface;
        mDecisionArray = new SparseArray<>();

        TAG = ShannonQualifiedNetworksService.getServiceTAG("-" + mSlotId + "-HC");

        mUserSettings = condUserSetting;
        mCellConditions = condCellular;
        mWiFiCondition = condWiFi;
        mProvisionedValues = CondProvisioning;
        mCallConditions = condCall;
        mMobileConditions = condMobile;
        mTransportCondition = condConnectivity;
    }

    public void destroy() {
        mDecisionArray.clear();
        mTransportCondition.destroy();
        mMobileConditions.destroy();
        mUserSettings.destroy();
        mProvisionedValues.destroy();
        mCallConditions.destroy();
        mWiFiCondition.destroy();
        mCellConditions.destroy();
        mRilInterface.destroy();
    }

    public void registerListener(ApnConditionsHandler handler) {
        synchronized (mListenersList) {
            if (mListenersList.contains(handler)) {
                return;
            }
            mListenersList.add(handler);

            sendNotification(handler, ConditionEvents.EVENT_INIT);
        }
    }

    public void unregisterListener(Handler handler) {
        synchronized (mListenersList) {
            mListenersList.remove(handler);
        }
    }

    public int getSlotIndex() {
        return mSlotId;
    }
    public Context getContext() { return mContext; }
    public ConditionCellular getCellularConditions() { return mCellConditions; }
    public ConditionWiFi getWiFiConditions() { return mWiFiCondition; }
    public ConditionCall getCallConditions() { return mCallConditions; }
    public ConditionUserSetting getUserConditions() { return mUserSettings; }
    public ConditionProvisioning getProvisioningConditions() { return mProvisionedValues; }
    public ConditionMobile getMobileConditions() { return mMobileConditions; }
    public ConfigurationManager getConfigurationManager() { return mConfigurationManager; }
    public ConditionConnectivity getTransportCondition() { return mTransportCondition; }

    public HandoverDecision getHandoverDecisionForApn(int apnType) {
        HandoverDecision res;

        switch (apnType) {
            case ApnSetting.TYPE_MMS:
                res = new MmsHandoverDecision(this);
                break;
            case ApnSetting.TYPE_IMS:
                res = getImsHandoverDecision();
                break;
            case ApnSetting.TYPE_XCAP:
                res = new XcapHandoverDecision(this);
                break;
            case ApnSetting.TYPE_EMERGENCY:
                res = new EmcHandoverDecision(this);
                break;
            case ApnSetting.TYPE_CBS:
                res = new CbsHandoverDecision(this);
                break;
            default:
                res = new HandoverDecision(apnType,this);
        }
        mDecisionArray.put(apnType, res);
        return res;
    }

    private ImsHandoverDecision getImsHandoverDecision() {
        ImsHandoverDecision res = new ImsHandoverDecision(this);

        CarrierType carrier = CarrierType.getEnum(mSimManager.getTelephonyManager(mSlotId));
        if (carrier != null) {
            LogUtils.i(TAG, LogUtils.LOG_CONDITIONS,
                    "Carrier " + carrier + " for MCC=" + mSimManager.getMcc(mSlotId) + " MNC=" + mSimManager.getMnc(mSlotId));
            if (carrier == CarrierType.CARRIER_TYPE_ATT) { // ATT-US(1187), ATT-US-5G(10021), FirstNet(2119), Cricket(1779)
                res = new ImsHandoverDecisionAttUs(this);
            } else if (carrier == CarrierType.CARRIER_TYPE_TMO) { // TMO-US
                res = new ImsHandoverDecisionTmoUs(this);
            } else if (carrier == CarrierType.CARRIER_TYPE_VZW) { // VZW-US
                res = new ImsHandoverDecisionVzwUs(this);
            } else if (carrier == CarrierType.CARRIER_TYPE_SWISSCOM) {
                res = new ImsHandoverDecisionSwisscom(this);
            } else if (carrier == CarrierType.CARRIER_TYPE_DTAG) {
                res = new ImsHandoverDecisionDtag(this);
            } else if (carrier == CarrierType.CARRIER_TYPE_ORANGE) {
                res = new ImsHandoverDecisionOrange(this);
            } else if (carrier == CarrierType.CARRIER_TYPE_RJIO) {
                res = new ImsHandoverDecisionRJio(this);
            }
        }
        return res;
    }

    /* for timers */
    public void onConditionChanged(int apnType, ConditionEvents event) {
        updateListeners(apnType, event);
    }

    private void updateAllListeners(ConditionEvents event) {
        LogUtils.i(TAG, LogUtils.LOG_CONDITIONS, "Conditions changed. " + this);
        synchronized (mListenersList) {
            for (ApnConditionsHandler h : mListenersList) {
                sendNotification(h, event);
            }
        }
    }

    private void updateListeners(int bitmask, ConditionEvents event) {
        LogUtils.i(TAG, LogUtils.LOG_CONDITIONS, "Conditions changed. " + this);
        synchronized (mListenersList) {
            for (ApnConditionsHandler h : mListenersList) {
                if (h.isHandledApnType(bitmask)) {
                    sendNotification(h, event);
                }
            }
        }
    }

    private void updateListeners(int bitmask, ConditionEvents event, Object object) {
        LogUtils.i(TAG, LogUtils.LOG_CONDITIONS, "Conditions changed. " + this);
        synchronized (mListenersList) {
            for (ApnConditionsHandler h : mListenersList) {
                if (h.isHandledApnType(bitmask)) {
                    EventInfo info = new EventInfo(event, object);
                    sendNotification(h, info);
                }
            }
        }
    }

    private void sendNotification(Handler h, Object obj) {
        Message msg = Message.obtain(h, CONDITIONS_CHANGED);
        msg.obj = obj;
        msg.sendToTarget();
    }

    public void onConfigurationChanged() {
        LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "Configuration changed");
        mWiFiCondition.onConfigurationChanged();
        mCellConditions.onConfigurationChanged();
        mTransportCondition.onConfigurationChanged();

        synchronized (mListenersList) {
            for (Handler h : mListenersList) {
                Message msg = Message.obtain(h, CONFIGURATION_CHANGED);
                msg.sendToTarget();
            }
        }
    }

    public void onThrottleStatusChanged(ThrottleStatus status) {
        mTransportCondition.onThrottleStatusChanged(status);
    }

    @Override
    public String toString() {
        return ("Slot" + mSlotId + " {" + mCellConditions + "} {" + mWiFiCondition + "} {" + mCallConditions + "} {" + mUserSettings + "} {" + mProvisionedValues + "}");
    }

    @VisibleForTesting
    protected List<ApnConditionsHandler> getlistenersList() {
        return mListenersList;
    }

}
