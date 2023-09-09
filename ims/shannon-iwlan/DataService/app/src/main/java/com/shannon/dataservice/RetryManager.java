/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or
 * distributed, transmitted, transcribed, stored in a retrieval system or
 * translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed to third parties
 * without the express written permission of Samsung Electronics.
 */
package com.shannon.dataservice;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Message;
import android.telephony.CarrierConfigManager;
import android.telephony.DataFailCause;
import android.telephony.Rlog;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
import android.telephony.data.ApnSetting;
import android.telephony.data.DataCallResponse;
import android.text.TextUtils;

import com.android.internal.annotations.VisibleForTesting;
import com.android.internal.telephony.PhoneConstants;
import com.android.internal.telephony.TelephonyIntents;

import java.net.InetAddress;
import java.net.UnknownHostException;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.Locale;

import static android.telephony.data.DataService.REQUEST_REASON_HANDOVER;

public class RetryManager {
    private static final boolean DBG = true;

    private static final String RETRY_SECTION_NAME = "name";
    private static final String RETRY_LOGIC_TMO = "TMO";

    protected class RetryBroadcastReceiver extends BroadcastReceiver {
        @Override
        public void onReceive(Context context, Intent intent) {
            final String action = intent.getAction();
            if (action == null) {
                loge("NULL action for intent " + intent);
                return;
            }
            log("Intent received " + intent);
            switch (action) {
                case Intent.ACTION_AIRPLANE_MODE_CHANGED: {
                    boolean mode = intent.getBooleanExtra("state", false);
                    if (mode) {
                        mPcoServerAddresses.clear();

                        for (RetryHandler handler : mApnMap.values()) {
                            handler.onAirplaneModeEnabled();
                        }
                    }
                    break;
                }
                case CarrierConfigManager.ACTION_CARRIER_CONFIG_CHANGED: {
                    int slotId = intent.getIntExtra(CarrierConfigManager.EXTRA_SLOT_INDEX, -1);
                    if ((slotId < 0) || (slotId != mSlotId)) {
                        return;
                    }
                    log("Carrier config changed for slot " + slotId);
                    for (RetryHandler handler : mApnMap.values()) {
                        handler.onCarrierConfigChanged();
                    }
                }
                case TelephonyIntents.ACTION_SIM_STATE_CHANGED:
                    int slotId = intent.getIntExtra(PhoneConstants.PHONE_KEY, -1);
                    if (slotId < 0 || slotId != mSlotId) {
                        return;
                    }
                    int state = SubscriptionManager.getSimStateForSlotIndex(slotId);
                    switch (state) {
                        case TelephonyManager.SIM_STATE_UNKNOWN:
                        case TelephonyManager.SIM_STATE_ABSENT: {
                            log("SIM removed for slot " + slotId + ", clear retry handler");
                            for (RetryHandler handler : mApnMap.values()) {
                                handler.destroy();
                            }
                            mApnMap.clear();
                            break;
                        }
                        default:;
                    }
                default:;
            }
        }
    }

    private final String TAG;
    private final int mSlotId;
    private final Context mContext;
    private final ConfigurationManager mConfigurationManager;
    private final DnsResolver mDnsResolver;
    private final RetryBroadcastReceiver mBroadcastReceiver;
    private LinkedList<InetAddress> mPcoServerAddresses;
    private HashMap<ApnType, RetryHandler> mApnMap;
    private Message mApnUnthrottledMessage;

    public RetryManager(int slotId, Context context, ConfigurationManager configuration) {
        mContext = context;
        mSlotId = slotId;
        mConfigurationManager = configuration;
        mApnUnthrottledMessage = null;

        mPcoServerAddresses = new LinkedList<>();
        mBroadcastReceiver = new RetryBroadcastReceiver();
        mDnsResolver = new DnsResolver(mSlotId);
        mApnMap = new HashMap<>();

        TAG = ShannonDataService.getServiceTAG("-RM-" + mSlotId);

        IntentFilter filter = new IntentFilter();
        filter.addAction(Intent.ACTION_AIRPLANE_MODE_CHANGED);
        filter.addAction(CarrierConfigManager.ACTION_CARRIER_CONFIG_CHANGED);
        filter.addAction(TelephonyIntents.ACTION_SIM_STATE_CHANGED);
        mContext.registerReceiver(mBroadcastReceiver, filter);
    }

    public void destroy() {
        for (RetryHandler handler : mApnMap.values()) {
            handler.destroy();
        }
        mApnMap.clear();
        mPcoServerAddresses.clear();
        mDnsResolver.destroy();
        mContext.unregisterReceiver(mBroadcastReceiver);
    }

    public void registerForApnUnthrottled(Message message) {
        mApnUnthrottledMessage = message;
    }

    public RetryHandler getApnRetryHandler(ApnType apnType, String apnName) {
        RetryHandler handler = mApnMap.get(apnType);
        if (handler == null) {
            String parameterString = mConfigurationManager.getRetryParameters();
            if (!TextUtils.isEmpty(parameterString)) {
                String[] parameters = parameterString.split(",");
                for (String s : parameters) {
                    s = s.trim();
                    if (s.startsWith(RETRY_SECTION_NAME)) {
                        String[] strings = s.split("=");
                        if (strings.length > 1) {
                            if (strings[1].trim().equalsIgnoreCase(RETRY_LOGIC_TMO)) {
                                handler = new RetryHandlerTmo(mSlotId, apnType, apnName, mContext,
                                        mConfigurationManager, mDnsResolver, mApnUnthrottledMessage);
                            }
                        }
                        break;
                    }
                }
            }
            if (handler == null) {
                handler = new RetryHandler(mSlotId, apnType, apnName, mContext,
                        mConfigurationManager, mDnsResolver, mApnUnthrottledMessage);
            }
            handler.updatePcoServerAddresses(mPcoServerAddresses);
            mApnMap.put(apnType, handler);
        }
        return handler;
    }

    private boolean isPcoEpdgAddress(int pcoId) {
        String list = mConfigurationManager.getSupportedPcoIdList();
        if (list == null) {
            return false;
        }

        String[] strings = list.split(",");
        for (String record : strings) {
            if (pcoId == Integer.parseInt(record.trim(), 16)) {
                return true;
            }
        }
        return false;
    }

    public void setPcoData(String apn, int pcoId, byte[] pcoData) {
        int apnBitmask = ApnSetting.getApnTypesBitmaskFromString(apn);
        /* IMS APN is supported only */
        if ((apnBitmask & ApnSetting.TYPE_IMS) == 0) {
            return;
        }
        try {
            if (isPcoEpdgAddress(pcoId)) {
                InetAddress address = InetAddress.getByAddress(pcoData);
                mPcoServerAddresses.add(address);
                log("Add [" + address + "] to PcoServerAddresses");

                RetryHandler handler = mApnMap.get(ApnType.APN_IMS);
                if (handler != null) {
                    handler.updatePcoServerAddresses(mPcoServerAddresses);
                }
            } else {
                log("Unhandled pcoId received : " + String.format(Locale.US, "%X(%d)", pcoId, pcoId));
            }
        } catch (UnknownHostException e) {
            loge("Failed to convert ePDG server address" + e);
        }
    }

    public void onWifiDisconnected() {
        for (RetryHandler handler : mApnMap.values()) {
            handler.onWiFiDisconnected();
        }
    }

    public boolean isApnThrottled(ApnType apnType) {
        RetryHandler handler = mApnMap.get(apnType);
        if (handler != null) {
            return handler.isBackoffTimerActive();
        }
        return false;
    }

    public DataCallResponse createFailureResponse(ApnType apnType, int requestType) {
        RetryHandler handler = mApnMap.get(apnType);
        if (handler != null) {
            return handler.createFailureResponse(requestType == REQUEST_REASON_HANDOVER);
        }
        return RetryHandler.createFailureResponse(DataFailCause.UNKNOWN,
                DataCallResponse.RETRY_DURATION_UNDEFINED, apnType.getCode(mSlotId),
                ApnSetting.PROTOCOL_IP, requestType == REQUEST_REASON_HANDOVER);
    }

    public DataCallResponse createFailureResponse(int reason, ApnType apnType, int requestType) {
        int cid = -1;
        if (apnType != null) {
            RetryHandler handler = mApnMap.get(apnType);
            if (handler != null) {
                return handler.createFailureResponse(reason, ApnSetting.PROTOCOL_IP,
                        requestType == REQUEST_REASON_HANDOVER);
            }
            cid = apnType.getCode(mSlotId);
        }
        return RetryHandler.createFailureResponse(reason, DataCallResponse.RETRY_DURATION_UNDEFINED,
                cid, ApnSetting.PROTOCOL_IP, requestType == REQUEST_REASON_HANDOVER);
    }

    private void log(String s) {
        if (DBG) {
            Rlog.d(TAG, s);
        }
    }

    private void loge(String s) {
        Rlog.e(TAG, s);
    }

    @VisibleForTesting
    protected RetryBroadcastReceiver getRetryBroadcastReceiver() {
        return mBroadcastReceiver;
    }

    @VisibleForTesting
    protected void setApnMap(ApnType apnType, RetryHandler retryHandler) {
        mApnMap.put(apnType, retryHandler);
    }
}
