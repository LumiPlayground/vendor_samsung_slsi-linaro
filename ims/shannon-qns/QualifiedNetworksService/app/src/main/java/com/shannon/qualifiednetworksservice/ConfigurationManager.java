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

import static com.shannon.vendorcarrierconfig.ShannonVendorCarrierConfig.*;

import android.annotation.TargetApi;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.NetworkCapabilities;
import android.os.Build;
import android.os.Bundle;
import android.os.Message;
import android.os.PersistableBundle;
import android.telephony.AccessNetworkConstants;
import android.telephony.CarrierConfigManager;
import android.text.TextUtils;

import com.android.internal.annotations.VisibleForTesting;
import com.android.internal.telephony.data.DataNetworkController.HandoverRule;
import com.android.internal.telephony.PhoneConstants;
import com.shannon.qualifiednetworksservice.ConfigurationBroadcastReceiver.IConfigurationReceiverCallback;
import com.shannon.qualifiednetworksservice.util.LogUtils;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.stream.Collectors;

public class ConfigurationManager {
    private static final String DEFAULT_NETWORK_CELLULAR_INTERFACE_NAME = "rmnet";
    private static final String DEFAULT_NETWORK_IWLAN_INTERFACE_NAME = "epdg";
    private static final String DEFAULT_IMS_ALLOWED_RAT = "LTE,LTE_CA,NR";
    private static final String DEFAULT_XCAP_ALLOWED_RAT = "GSM,GPRS,EDGE,UMTS,HSDPA,HSUPA,HSPA,HSPA+,LTE,LTE_CA,NR";
    private static final String DEFAULT_MOBILE_ALLOWED_RAT = "LTE,LTE_CA,NR";
    private static final String DEFAULT_IMS_HANDOVER_IN = "cdma{rssi=-100};geran{rssi=-100};utran{rscp=-105};eutran{rsrp=-115};iwlan{rssi=-75}";
    private static final String DEFAULT_IMS_HANDOVER_OUT = "cdma{rssi=-105};geran{rssi=-105};utran{rscp=-115};eutran{rsrp=-120};iwlan{rssi=-80}";
    private static final String DEFAULT_MMS_ALLOWED_RAT = "GSM,GPRS,EDGE,UMTS,HSDPA,HSUPA,HSPA,HSPA+,LTE,LTE_CA,NR";
    private static final String DEFAULT_EMERGENCY_HANDOVER_ALLOWED_RAT = "LTE,LTE_CA,NR,IWLAN";

    protected class ConfigurationReceiverCallback implements IConfigurationReceiverCallback {
        @Override
        public void onReceive(Intent intent) {
            handleVendorConfigChanged(intent);
        }
    }

    protected static class DisallowedHandoverRule {
        public final List<String> mSourceRat;
        public final List<String> mTargetRat;
        public final List<String> mCapabilities;
        public final boolean mRoamingOnly;

        public DisallowedHandoverRule(List<String> source, List<String> target,
                                      List<String> capabilities, boolean roamingOnly) {
            mSourceRat = source;
            mTargetRat = target;
            mCapabilities = capabilities;
            mRoamingOnly = roamingOnly;
        }
    }

    private final String TAG;
    private final int mSlotId;
    private final Context mContext;
    private Bundle mBundle;
    private final Message mNotificationMessage;
    private final CarrierConfigManager mCarrierConfigManager;
    private final CarrierConfigurationBroadcastReceiver mBroadcastReceiver;

    /* Carrier Configuration Info */
    private List<DisallowedHandoverRule> mHandoverRulesList;

    public ConfigurationManager(int slotId, Context context, Message message) {
        mSlotId = slotId;
        mContext = context;
        mNotificationMessage = message;
        mBundle = null;
        mHandoverRulesList = new ArrayList<>();
        TAG = ShannonQualifiedNetworksService.getServiceTAG("-" + slotId + "-CFG");

        ConfigurationBroadcastReceiver.registerConfigurationReceiverCallback(mSlotId,
                new ConfigurationReceiverCallback());

        mBroadcastReceiver = new CarrierConfigurationBroadcastReceiver();

        mCarrierConfigManager = mContext.getSystemService(CarrierConfigManager.class);
        updateCarrierConfigInfo();

        IntentFilter filter = new IntentFilter();
        filter.addAction(CarrierConfigManager.ACTION_CARRIER_CONFIG_CHANGED);
        mContext.registerReceiver(mBroadcastReceiver, filter);
    }

    protected class CarrierConfigurationBroadcastReceiver extends BroadcastReceiver {
        @Override
        public void onReceive(Context context, Intent intent) {
            final String action = intent.getAction();
            if (action == null) {
                LogUtils.w(TAG, "NULL action for intent " + intent);
                return;
            }
            LogUtils.v(TAG, LogUtils.LOG_CONDITIONS, "Got intent " + intent);
            switch (action) {
                case CarrierConfigManager.ACTION_CARRIER_CONFIG_CHANGED: {
                    int slotId = intent.getIntExtra(CarrierConfigManager.EXTRA_SLOT_INDEX, -1);
                    if (mSlotId != slotId) {
                        break;
                    }
                    LogUtils.d(TAG, "Carrier config updated for slot " + mSlotId);
                    updateCarrierConfigInfo();
                    break;
                }
                default:;
            }
        }
    }

    public void destroy() {
        ConfigurationBroadcastReceiver.unregisterConfigurationReceiverCallback(mSlotId);
        mContext.unregisterReceiver(mBroadcastReceiver);
    }

    public int getInt(String key, int defaultValue) {
        if (mBundle == null || !mBundle.containsKey(key)) {
            return defaultValue;
        }
        return mBundle.getInt(key);
    }

    public void putInt(String key, int value) {
        if (mBundle == null) {
            return;
        }
        mBundle.putInt(key, value);
    }

    public boolean getBoolean(String key, boolean defaultValue) {
        if (mBundle == null || !mBundle.containsKey(key)) {
            return defaultValue;
        }
        return mBundle.getBoolean(key);
    }

    public void putBoolean(String key, boolean value) {
        if (mBundle == null) {
            return;
        }
        mBundle.putBoolean(key, value);
    }

    public void putBooleanPersisted(String key, boolean value) {
        Bundle bundle = new Bundle();
        bundle.putBoolean(key, value);

        ShannonQualifiedNetworksService.updateVendorCarrierConfigForSlot(mSlotId, bundle);
    }

    public String getString(String key, String defaultValue) {
        if (mBundle == null || !mBundle.containsKey(key)) {
            return defaultValue;
        }
        return mBundle.getString(key);
    }

    public void handleVendorConfigChanged(Intent intent) {
        final String action = intent.getAction();
        if (action == null) {
            LogUtils.w(TAG, "NULL action for intent " + intent);
            return;
        }
        LogUtils.v(TAG, "Got intent " + intent);
        switch (action) {
            case BROADCAST_ACTION_VENDOR_CONFIG_CHANGED: {
                int slotId = intent.getIntExtra(PhoneConstants.SLOT_KEY, -1);
                if (slotId == mSlotId) {
                    Bundle bundle = intent.getBundleExtra(BROADCAST_EXTRA_VENDOR_CONFIG);
                    if (bundle != null) {
                        LogUtils.d(TAG, "Vendor config received for slot " + mSlotId);

                        if (mBundle == null) {
                            mBundle = new Bundle(bundle);
                        } else {
                            updateBundleNonOverlap(mBundle, bundle);
                        }
                        Message message = Message.obtain(mNotificationMessage);
                        message.sendToTarget();
                    }
                } else {
                    LogUtils.v(TAG, "slotId : " + slotId + " from bundle is not matched, ignore");
                }
                break;
            }
        }
    }

    private void updateBundleNonOverlap(Bundle to, Bundle from) {
        if (to == null || from == null || to.isEmpty() || from.isEmpty()) {
            return;
        }
        Bundle bundle = new Bundle(from);
        for (String key : from.keySet()) {
            if (to.containsKey(key)) {
                Object objTo = to.get(key);
                Object objFrom = bundle.get(key);

                if ((objTo == null) || !objTo.equals(objFrom)) {
                    to.remove(key);
                } else {
                    bundle.remove(key);
                }
            } else {
                bundle.remove(key);
            }
        }
        if (!bundle.isEmpty()) {
            /* overwrite all changed values */
            to.putAll(bundle);
        }
    }

    private DisallowedHandoverRule getDisallowedHandoverRule(String input) {
        if (TextUtils.isEmpty(input)) {
            LogUtils.d(TAG, "Empty HandoverRule");
            return null;
        }
        final String disallowed = "disallowed";
        if (!input.contains(disallowed) && !input.contains(disallowed.toUpperCase())) {
            return null;
        }
        String upperInput = input.trim().toUpperCase();

        List<String> source = new ArrayList<>();
        List<String> target = new ArrayList<>();
        List<String> capabilities = new ArrayList<>();
        Boolean roaming = false;

        String[] strings = upperInput.split(",");
        for(String rule : strings) {
            String[] tokens = rule.trim().split("=");
            if (tokens.length != 2) {
                LogUtils.d(TAG, "Abnormal tokens: " +  Arrays.toString(tokens)
                        + ", rule: " + upperInput);
                continue;
            }
            String key = tokens[0].trim();
            String value = tokens[1].trim();
            try {
                final String caseSource = "SOURCE";
                final String caseTarget = "TARGET";
                final String caseCapabilities = "CAPABILITIES";
                final String caseRoaming =  "ROAMING";
                switch (key) {
                    case caseSource:
                        source = Arrays.stream(value.split("\\|"))
                                .map(String::trim)
                                .collect(Collectors.toList());
                        break;
                    case caseTarget:
                        target = Arrays.stream(value.split("\\|"))
                                .map(String::trim)
                                .collect(Collectors.toList());
                        break;
                    case caseCapabilities:
                        capabilities = Arrays.stream(value.split("\\|"))
                                .map(String::trim)
                                .collect(Collectors.toList());
                        break;
                    case caseRoaming:
                        roaming = Boolean.parseBoolean(value);
                        break;
                    default:
                        LogUtils.d(TAG, "Unexpected the key for HandoverRule. key: " + key);
                }
            } catch (Exception e) {
                LogUtils.e(TAG, "Failed to parse a HandoverRule string: " + e);
            }
        }
        return new DisallowedHandoverRule(source, target, capabilities, roaming);
    }

    private void updateCarrierConfigInfo() {
        int subId = SimManager.getInstance().getSubId(mSlotId);
        if (subId < 0) {
            LogUtils.e(TAG, "Invalid subId received " + subId + " for slot " + mSlotId);
            return;
        }
        try {
            PersistableBundle carrierBundle = mCarrierConfigManager.getConfigForSubId(subId);
            if (CarrierConfigManager.isConfigForIdentifiedCarrier(carrierBundle)) {
                /* Initialize array of handover policy strings */
                String[] handoverRulesStrings = carrierBundle.
                        getStringArray(CarrierConfigManager.KEY_IWLAN_HANDOVER_POLICY_STRING_ARRAY);
                mHandoverRulesList = new ArrayList<>();

                if (handoverRulesStrings != null) {
                    for (String ruleString : handoverRulesStrings) {
                        if (TextUtils.isEmpty(ruleString)) continue;

                        LogUtils.v(TAG, "Add the HandoverRule <" + ruleString + ">");
                        DisallowedHandoverRule rule = getDisallowedHandoverRule(ruleString);
                        if (rule != null) {
                            mHandoverRulesList.add(rule);
                        }
                    }
                }
            }
        } catch (Exception e) {
            LogUtils.w(TAG, "Failed to get carrier config");
        }
    }

    public boolean isVoLteVoPsRequired() {
        return getBoolean(KEY_VOLTE_VOPS_REQUIRED, true);
    }

    public boolean isVoLteVoPsNotRequiredActiveCall() {
        return getBoolean(KEY_VOLTE_VOPS_NOT_REQUIRED_ACTIVE_CALL, false);
    }

    public String getVoLteAllowedRat() {
        return getString(KEY_VOLTE_SUPPORTED_RADIO_TECHNOLOGY, DEFAULT_IMS_ALLOWED_RAT);
    }

    public boolean isVoLteRoamingSupported() {
        return getBoolean(KEY_VOLTE_ROAMING_ENABLED, true);
    }

    public boolean isVoLteVoPsRequiredRoaming() {
        return getBoolean(KEY_VOLTE_VOPS_REQUIRED_ROAMING, true);
    }

    public boolean isWfcOemProvisioningRequired() {
        return getBoolean(KEY_VOWIFI_OEM_PROVISIONING_ENABLED, false);
    }

    public boolean isWfcRoamingSupported() {
        return getBoolean(KEY_VOWIFI_ROAMING_ENABLED, true);
    }

    /* Not used. Carrier config KEY_IWLAN_HANDOVER_POLICY_STRING_ARRAY is used instead */
    public boolean isImsHandoverAllowed(boolean roaming) {
        if (roaming) {
            return getBoolean(KEY_IMS_HANDOVER_SUPPORTED_ROAMING, true);
        }
        return getBoolean(KEY_IMS_HANDOVER_SUPPORTED, true);
    }

    /* Not used. Carrier config KEY_IWLAN_HANDOVER_POLICY_STRING_ARRAY is used instead */
    public boolean isImsHandoverAllowedInCall(boolean roaming) {
        if (roaming) {
            return getBoolean(KEY_IMS_HANDOVER_SUPPORTED_IN_CALL_ROAMING, true);
        }
        return getBoolean(KEY_IMS_HANDOVER_SUPPORTED_IN_CALL, true);
    }

    /* Not used. Carrier config KEY_IWLAN_HANDOVER_POLICY_STRING_ARRAY is used instead */
    public String getImsHandoverAllowedRatList() {
        return getString(KEY_IMS_HANDOVER_SUPPORTED_RADIO_TECHNOLOGY, "");
    }

    /* Not used. Carrier config KEY_IWLAN_HANDOVER_POLICY_STRING_ARRAY is used instead */
    public String getImsHandoverNotAllowedRatList() {
        return getString(KEY_IMS_HANDOVER_RESTRICTED_RADIO_TECHNOLOGY, "");
    }

    public int getImsHysteresisTimerToLte() {
        return getInt(KEY_IMS_HANDOVER_HYSTERESIS_TIMER_VOLTE, 0);
    }

    public int getImsHysteresisTimerToWiFi() {
        return getInt(KEY_IMS_HANDOVER_HYSTERESIS_TIMER_VOWIFI, 0);
    }

    public int getImsRegistrationTimer() {
        return getInt(KEY_IMS_HANDOVER_REGISTRATION_TIMER, 5);
    }

    public boolean canIgnoreCallDowngrade() {
        return getBoolean(KEY_IMS_HANDOVER_IGNORE_CALL_DOWNGRADE, false);
    }

    public boolean canIgnoreCallUpgrade() {
        return getBoolean(KEY_IMS_HANDOVER_IGNORE_CALL_UPGRADE, false);
    }

    public int getRtpThreshold() {
        return getInt(KEY_IMS_HANDOVER_RTP_DOWNLINK_LOST_RATE, 40);
    }

    public int getRtpInterval() {
        return getInt(KEY_IMS_HANDOVER_RTP_DOWNLINK_CHECK_INTERVAL, 5);
    }

    public int getRtpJitter() {
        return getInt(KEY_IMS_HANDOVER_RTP_JITTER_MAX, 120);
    }

    public int getNoRtpInterval() { return getInt(KEY_IMS_HANDOVER_NO_RTP_INTERVAL, 2); }

    public boolean isWiFiValidationRequired() {
        return getBoolean(KEY_WIFI_NETWORK_VALIDATION_REQUIRED, true);
    }

    public boolean canUseWiFiIPv6() {
        return getBoolean(KEY_WIFI_CAN_USE_IPV6, false);
    }

    public boolean isWiFiApHandoverSupported() {
        return getBoolean(KEY_WIFI_ACCESS_POINTS_HANDOVER_SUPPORTED, true);
    }

    public int getWiFiApHandoverTimeout() {
        return getInt(KEY_WIFI_ACCESS_POINTS_HANDOVER_TIMEOUT, 10);
    }

    public String getPingDestination() {
        return getString(KEY_WIFI_PING_SERVER, "");
    }

    public int getPingPeriod() {
        return getInt(KEY_WIFI_PING_PERIOD, 0);
    }

    public int getPingInterval() {
        return getInt(KEY_WIFI_PING_INTERVAL, 0);
    }

    public int getPingCount() {
        return getInt(KEY_WIFI_PING_COUNT, 0);
    }

    public int getPingPacketSize() {
        return getInt(KEY_WIFI_PING_PACKET_SIZE, 0);
    }

    public int getPingMaxRtt() {
        return getInt(KEY_IMS_HANDOVER_WIFI_PING_RTT, 0);
    }

    public String getDefaultImsHandoverIn() {
        return getString(KEY_IMS_HANDOVER_DEFAULT_IN, DEFAULT_IMS_HANDOVER_IN);
    }

    public String getImsRoveIn() { return getString(KEY_IMS_HANDOVER_ROVE_IN, ""); }

    public String getImsRoveInRoaming() {
        return getString(KEY_IMS_HANDOVER_ROVE_IN_ROAMING, "");
    }

    public String getImsRoveInHysteresisOn() {
        return getString(KEY_IMS_HANDOVER_ROVE_IN_HYSTERESIS_ON, "");
    }

    public String getImsRoveInWiFiPreferred() {
        return getString(KEY_IMS_HANDOVER_ROVE_IN_WIFI_PREFERRED, "");
    }

    public String getImsRoveInCellPreferred() {
        return getString(KEY_IMS_HANDOVER_ROVE_IN_CELL_PREFERRED, "");
    }

    public String getImsHandIn() { return getString(KEY_IMS_HANDOVER_HAND_IN, ""); }

    public String getImsHandInRoaming() {
        return getString(KEY_IMS_HANDOVER_HAND_IN_ROAMING, "");
    }

    public String getImsHandInHysteresisOn() {
        return getString(KEY_IMS_HANDOVER_HAND_IN_HYSTERESIS_ON, "");
    }

    public String getImsHandInWiFiPreferred() {
        return getString(KEY_IMS_HANDOVER_HAND_IN_WIFI_PREFERRED, "");
    }

    public String getImsHandInCellPreferred() {
        return getString(KEY_IMS_HANDOVER_HAND_IN_CELL_PREFERRED, "");
    }

    public String getDefaultImsHandoverOut() {
        return getString(KEY_IMS_HANDOVER_DEFAULT_OUT, DEFAULT_IMS_HANDOVER_OUT);
    }

    public String getImsRoveOut() { return getString(KEY_IMS_HANDOVER_ROVE_OUT, ""); }

    public String getImsRoveOutHysteresisOn() {
        return getString(KEY_IMS_HANDOVER_ROVE_OUT_HYSTERESIS_ON, "");
    }

    public String getImsRoveOutWiFiPreferred() {
        return getString(KEY_IMS_HANDOVER_ROVE_OUT_WIFI_PREFERRED, "");
    }

    public String getImsRoveOutCellPreferred() {
        return getString(KEY_IMS_HANDOVER_ROVE_OUT_CELL_PREFERRED, "");
    }

    public String getImsHandOut() { return getString(KEY_IMS_HANDOVER_HAND_OUT, ""); }

    public String getImsHandOutHysteresisOn() {
        return getString(KEY_IMS_HANDOVER_HAND_OUT_HYSTERESIS_ON, "");
    }

    public String getImsHandOutWiFiPreferred() {
        return getString(KEY_IMS_HANDOVER_HAND_OUT_WIFI_PREFERRED, "");
    }

    public String getImsHandOutCellPreferred() {
        return getString(KEY_IMS_HANDOVER_HAND_OUT_CELL_PREFERRED, "");
    }

    public String getImsCellMid() { return getString(KEY_IMS_HANDOVER_CELL_MID, ""); }

    public String getCellularNetworkInterfaceName() {
        return getString(KEY_NETWORK_CELLULAR_INTERFACE_NAME, DEFAULT_NETWORK_CELLULAR_INTERFACE_NAME);
    }

    public String getIwlanNetworkInterfaceName() {
        return getString(KEY_NETWORK_IWLAN_INTERFACE_NAME, DEFAULT_NETWORK_IWLAN_INTERFACE_NAME);
    }

    public boolean isEmcOverWiFiSupported() {
        return getBoolean(KEY_EMERGENCY_OVER_WIFI_SUPPORTED, true);
    }

    public boolean isEmcOverWiFiSupportedRoaming() {
        return getBoolean(KEY_EMERGENCY_OVER_WIFI_SUPPORTED_ROAMING, true);
    }

    public boolean isEmcOemProvisioningRequired() {
        return getBoolean(KEY_EMERGENCY_OVER_WIFI_PROVISIONING_ENABLED, false);
    }

    public boolean canIgnoreWfcSettingForEmc() {
        return getBoolean(KEY_EMERGENCY_OVER_WIFI_IGNORE_USER_SETTING, true);
    }

    public boolean canIgnoreWfcSettingForEmcHandover() {
        return getBoolean(KEY_EMERGENCY_OVER_WIFI_IGNORE_USER_SETTING_HANDOVER, true);
    }

    /* Not used. Carrier config KEY_IWLAN_HANDOVER_POLICY_STRING_ARRAY is used instead */
    public boolean isEmcHandoverAllowed() {
        return getBoolean(KEY_EMERGENCY_HANDOVER_ENABLED, false);
    }

    /* Not used. Carrier config KEY_IWLAN_HANDOVER_POLICY_STRING_ARRAY is used instead */
    public boolean isEmcHandoverAllowedAtRoaming() {
        return getBoolean(KEY_EMERGENCY_HANDOVER_ENABLED_ROAMING, false);
    }

    /* Not used. Carrier config KEY_IWLAN_HANDOVER_POLICY_STRING_ARRAY is used instead */
    public String getEmcHandoverAllowedRatList() {
        return getString(KEY_EMERGENCY_HANDOVER_SUPPORTED_RADIO_TECHNOLOGY, DEFAULT_EMERGENCY_HANDOVER_ALLOWED_RAT);
    }

    public int getWiFiEmcRoveIn() {
        return getInt(KEY_EMERGENCY_HANDOVER_WIFI_RSSI_IN, -65);
    }

    public int getWiFiEmcRoveOut() {
        return getInt(KEY_EMERGENCY_HANDOVER_WIFI_RSSI_OUT, -80);
    }

    public int getCellEmcRoveIn() {
        return getInt(KEY_EMERGENCY_HANDOVER_EUTRAN_RSRP_IN, -100);
    }

    public int getCellEmcRoveOut() {
        return getInt(KEY_EMERGENCY_HANDOVER_EUTRAN_RSRP_OUT, -112);
    }

    public int getCellEmcRsrpMid() {
        return getInt(KEY_EMERGENCY_HANDOVER_EUTRAN_RSRP_MID, 0);
    }

    public boolean isCbsOverWiFiSupported() {
        return getBoolean(KEY_CBS_OVER_WIFI_SUPPORTED, false);
    }

    public boolean isMmsOverWiFiSupported() {
        return getBoolean(KEY_MMS_OVER_WIFI_SUPPORTED, true);
    }

    public boolean isWiFiPreferredForMms() {
        return getBoolean(KEY_MMS_OVER_WIFI_PREFERRED, false);
    }

    public String getMmsAllowedRatList() {
        return getString(KEY_MMS_SUPPORTED_RADIO_TECHNOLOGY, DEFAULT_MMS_ALLOWED_RAT);
    }

    public boolean isXcapOverWiFiSupported() {
        return getBoolean(KEY_XCAP_OVER_WIFI_SUPPORTED, true);
    }

    public boolean isWiFiPreferredForXcap() {
        return getBoolean(KEY_XCAP_OVER_WIFI_PREFERRED, false);
    }

    /* Not used. Carrier config KEY_IWLAN_HANDOVER_POLICY_STRING_ARRAY is used instead */
    public boolean isXcapHandoverAllowed() {
        return getBoolean(KEY_XCAP_HANDOVER_ENABLED, false);
    }

    public boolean isXcapImsPreconditionForWiFi() {
        return getBoolean(KEY_XCAP_USE_IMS_PRECONDITION_FOR_WIFI, false);
    }

    public String getXcapAllowedRatList() {
        return getString(KEY_XCAP_SUPPORTED_RADIO_TECHNOLOGY, DEFAULT_XCAP_ALLOWED_RAT);
    }

    public int getHysteresisTimerToLte() {
        return getInt(KEY_HYSTERESIS_TIMER_CELLULAR, 0);
    }

    public int getHysteresisTimerToWiFi() {
        return getInt(KEY_HYSTERESIS_TIMER_WIFI, 0);
    }

    public boolean isVoMobileEnabled() {
        return getBoolean(KEY_VOMOBILE_ENABLED, false);
    }

    public boolean isVoMobileEnabledOnRoamingNetwork() {
        return getBoolean(KEY_VOMOBILE_ONROAMING_ENABLED, false);
    }

    public String getVoMobileAllowedRatList() {
        return getString(KEY_VOMOBILE_SUPPORTED_RADIO_TECHNOLOGY, DEFAULT_MOBILE_ALLOWED_RAT);
    }

    public int getVoMobileOtherSignalIn() {
        return getInt(KEY_VOMOBILE_OTHER_CELLULAR_SIGNAL_IN, -110);
    }

    public int getVoMobileOtherSignalOut() {
        return getInt(KEY_VOMOBILE_OTHER_CELLULAR_SIGNAL_OUT, -116);
    }

    public int getVoMobileSignalIn() {
        return getInt(KEY_VOMOBILE_CURRENT_CELLULAR_SIGNAL_IN, -115);
    }

    public int getVoMobileSignalOut() {
        return getInt(KEY_VOMOBILE_CURRENT_CELLULAR_SIGNAL_OUT, -125);
    }

    public boolean isVoMobileRoamingOverrideSignalStrength() {
        return getBoolean(KEY_VOMOBILE_ROAMING_OVERRIDE_SIGNAL_STRENGTH, true);
    }

    public boolean isVoMobileIgnoreCsState() {
        return getBoolean(KEY_VOMOBILE_IGNORE_CS_STATE, false);
    }

    public boolean isVoMobilePreferred() {
        return getBoolean(KEY_VOMOBILE_PREFERRED, false);
    }

    public String getVoMobileForbiddenNetworksInRoaming() {
        return getString(KEY_VOMOBILE_FORBIDDEN_NETWORKS_IN_ROAMING, "");
    }

    public boolean isVoMobileInterStackHandoverSupported() {
        return getBoolean(KEY_VOMOBILE_INTERSTACK_HANDOVER_SUPPORTED, false);
    }

    public int getCellularPermanentErrorRetryCount() {
        return getInt(KEY_TRANSPORT_CELLULAR_PERMANENT_ERROR_RETRY, 3);
    }

    public int getCellularPermanentErrorRetryTimeout() {
        return getInt(KEY_TRANSPORT_CELLULAR_PERMANENT_ERROR_TIMEOUT, 720);
    }

    public int getCellularPermanentErrorHandoverRetryTimeout() {
        return getInt(KEY_TRANSPORT_CELLULAR_PERMANENT_ERROR_HANDOVER_TIMEOUT, 120);
    }

    public int getIwlanPermanentErrorRetryCount() {
        return getInt(KEY_TRANSPORT_IWLAN_PERMANENT_ERROR_RETRY, 3);
    }

    public int getIwlanPermanentErrorRetryTimeout() {
        return getInt(KEY_TRANSPORT_IWLAN_PERMANENT_ERROR_TIMEOUT, 720);
    }

    public int getIwlanPermanentErrorHandoverRetryTimeout() {
        return getInt(KEY_TRANSPORT_IWLAN_PERMANENT_ERROR_HANDOVER_TIMEOUT, 120);
    }

    public int getInitialNonPermanentErrorRetryCount() {
        return getInt(KEY_TRANSPORT_INITIAL_NON_PERMANENT_ERROR_MAX_RETRY, 3);
    }

    public int getInitialNonPermanentErrorRetryTimeout() {
        return getInt(KEY_TRANSPORT_INITIAL_NON_PERMANENT_ERROR_TIMEOUT, 30);
    }

    public int getHandoverNonPermanentErrorRetryCount() {
        return getInt(KEY_TRANSPORT_HANDOVER_NON_PERMANENT_ERROR_MAX_RETRY, 10);
    }

    public int getHandoverNonPermanentErrorRetryTimeout() {
        return getInt(KEY_TRANSPORT_HANDOVER_NON_PERMANENT_ERROR_TIMEOUT, 30);
    }

    public int getInitialAttachTimeout() {
        return getInt(KEY_TRANSPORT_INITIAL_ATTACH_TIMEOUT, 5);
    }

    public boolean isProfileDefaultNetworkSet() {
        return getBoolean(KEY_IMS_HANDOVER_USE_PROFILE_DEFAULT_NETWORK, false);
    }

    public String getAllowedRoamingNetworks() {
        return getString(KEY_VOLTE_ROAMING_ALLOWED_NETWORKS_LIST, "");
    }

    public String getAllowedDomesticRoamingNetworks() {
        return getString(KEY_VOLTE_DOMESTIC_ROAMING_ALLOWED_NETWORKS_LIST, "");
    }

    public boolean canUseWfcRoamingSettings() {
        return getBoolean(KEY_VOWIFI_USE_ROAMING_SETTINGS, false);
    }

    public boolean getUnknownLocationAsRoaming() {
        return getBoolean(KEY_VOWIFI_UNKNOWN_LOCATION_AS_ROAMING, false);
    }

    public String getRetryParameters() {
        if (mBundle != null) {
            return mBundle.getString(KEY_EPDG_CONNECTION_RETRY_PARAMETERS);
        }
        return null;
    }

    public boolean isWfcSupportedInAirplaneMode() {
        return getBoolean(KEY_VOWIFI_SUPPORTED_IN_AIRPLANE_MODE, true);
    }

    public boolean isHandoverNotAllowed(String capabilityName, int currentAccessType, int preferredAccessType,
                                    boolean roaming) {
        String currentAccessNetwork = AccessNetworkConstants.AccessNetworkType
                .toString(currentAccessType);
        String preferredAccessNetwork = AccessNetworkConstants.AccessNetworkType
                .toString(preferredAccessType);

        List<DisallowedHandoverRule> list = mHandoverRulesList;
        for (DisallowedHandoverRule rule : list) {
            if (rule.mRoamingOnly && !roaming) continue;

            if (rule.mSourceRat.contains(currentAccessNetwork) &&
                    rule.mTargetRat.contains(preferredAccessNetwork)) {
                if (rule.mCapabilities.isEmpty() || rule.mCapabilities.contains(capabilityName)) {
                    return true;
                }
            }
        }
        return false;
    }

    @VisibleForTesting
    protected void setBundle(Bundle bundle) {
        mBundle = bundle;
    }

    @VisibleForTesting
    protected void setHandoverRulesList(List<DisallowedHandoverRule> list) { mHandoverRulesList = list; }

    @VisibleForTesting
    protected BroadcastReceiver getCarrierConfigBroadcastReceiver() { return mBroadcastReceiver; }
}
