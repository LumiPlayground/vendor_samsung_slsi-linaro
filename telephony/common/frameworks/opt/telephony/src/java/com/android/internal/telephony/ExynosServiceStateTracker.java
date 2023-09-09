/*
 * Copyright (C) 2006 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.internal.telephony.vendor;

import com.android.internal.telephony.ServiceStateTracker;
import com.android.telephony.Rlog;

import com.android.internal.telephony.cdnr.CarrierDisplayNameData;
import com.android.internal.telephony.CommandsInterface;
import com.android.internal.telephony.GsmCdmaPhone;
import com.android.internal.telephony.DctConstants;

import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.AsyncResult;
import android.os.Message;
import android.os.PersistableBundle;
import android.os.SystemProperties;
import android.text.TextUtils;
import android.telephony.AccessNetworkConstants;
import android.telephony.AccessNetworkConstants.AccessNetworkType;
import android.telephony.AccessNetworkConstants.TransportType;
import android.telephony.CarrierConfigManager;
import android.telephony.NetworkRegistrationInfo;
import android.telephony.ServiceState;
import android.telephony.ServiceState.RilRadioTechnology;
import android.telephony.TelephonyDisplayInfo;
import android.telephony.TelephonyManager;

import java.util.Arrays;

/**
 * {@hide}
 */
public class ExynosServiceStateTracker extends ServiceStateTracker {
    static final String LOG_TAG = "ExSST";
    static final boolean DBG = true;
    private static final boolean VDBG = true;  // STOPSHIP if true

    /** OEM events */
    protected static final int EVENT_OEM_BASE                          = 1000;
    protected static final int EVENT_UPDATE_SERVICE_STATE              = EVENT_OEM_BASE + 1;
    protected static final int EVENT_TELEPHONY_DISPLAY_INFO_CHANGED    = EVENT_OEM_BASE + 2;
    protected static final int EVENT_SET_AIRPLANE_MODE                 = EVENT_OEM_BASE + 3;

    private TelephonyDisplayInfo mTelephonyDisplayInfo;
    private boolean mShowRatInfo = false;
    private boolean mNameRulePlmnSpn = false;

    public ExynosServiceStateTracker(GsmCdmaPhone phone, CommandsInterface ci) {
        // Start from just extends whole thing.
        super(phone, ci);
        Rlog.i(LOG_TAG, "ExynosServiceStateTracker consturctor");
    }

    @Override
    public void dispose() {
        super.dispose();
        if (mPhone.getDisplayInfoController() != null) {
            mPhone.getDisplayInfoController().unregisterForTelephonyDisplayInfoChanged(this);
        }
    }

    @Override
    public void registerForTelephonyDisplayInfoChanged() {
        if (mPhone.getDisplayInfoController() != null) {
            mPhone.getDisplayInfoController().registerForTelephonyDisplayInfoChanged(
                    this, EVENT_TELEPHONY_DISPLAY_INFO_CHANGED, null);
        }
    }

    @Override
    public void handleMessage(Message msg) {
        AsyncResult ar;

        Rlog.i(LOG_TAG, "received event " + msg.what);
        switch (msg.what) {
            case EVENT_UPDATE_SERVICE_STATE:
                Rlog.v(LOG_TAG, "["+mPhone.getSubId()+"] EVENT_UPDATE_SERVICE_STATE");
                mPhone.notifyServiceStateChanged(mSS);
                break;

            case EVENT_TELEPHONY_DISPLAY_INFO_CHANGED:
                if (msg.obj != null) {
                    ar = (AsyncResult) msg.obj;
                    mTelephonyDisplayInfo = (TelephonyDisplayInfo)ar.result;
                    Rlog.v(LOG_TAG, "["+mPhone.getSubId()+"] DisplayInfo = "+mTelephonyDisplayInfo);
                    updateSpnDisplay();
                }
                break;
            case EVENT_CARRIER_CONFIG_CHANGED:
                updateCarrierConfiguration();
                super.handleMessage(msg);
                break;
            case EVENT_RADIO_POWER_OFF_DONE:
                sendMessage(obtainMessage(EVENT_SET_AIRPLANE_MODE));
                super.handleMessage(msg);
                break;
            case EVENT_SET_AIRPLANE_MODE:
                Rlog.v(LOG_TAG, "["+mPhone.getSubId()+"] EVENT_SET_AIRPLANE_MODE");
                Intent intent = new Intent("android.intent.action.AIRPLANE_MODE_CHANGE_COMPLETE");
                intent.putExtra("phoneId", mPhone.getPhoneId());
                mPhone.getContext().sendBroadcast(intent);
                break;

            default:
                super.handleMessage(msg);
                break;
        }
    }

    private void notifyServiceStateChanged() {
        sendMessage(obtainMessage(EVENT_UPDATE_SERVICE_STATE));
    }

    @Override
    public void setImsRegistrationState(final boolean registered) {
        super.setImsRegistrationState(registered);

        // to apply IMS voiceRegState to update overall voiceRegState
        notifyServiceStateChanged();
    }

    @Override
    public void notifySpnDisplayUpdate(CarrierDisplayNameData data) {
        String plmnName = data.getPlmn();
        String spnName = data.getSpn();

        if(mShowRatInfo && getCombinedRegState(mSS) == ServiceState.STATE_IN_SERVICE) {
            if (!TextUtils.isEmpty(plmnName)) {
                plmnName = addRatInfoForOperatorName(plmnName);
            }

            if (!TextUtils.isEmpty(spnName)) {
                spnName = addRatInfoForOperatorName(spnName);
            }
        }

        super.notifySpnDisplayUpdate(new CarrierDisplayNameData.Builder()
                .setSpn(spnName)
                .setDataSpn(data.getDataSpn())
                .setShowSpn(data.shouldShowSpn())
                .setPlmn(plmnName)
                .setShowPlmn(data.shouldShowPlmn())
                .build());
    }

    private String addRatInfoForOperatorName(String str) {
        String name = str;

        switch(mSS.getDataNetworkType())
        {
            case TelephonyManager.NETWORK_TYPE_1xRTT:
            case TelephonyManager.NETWORK_TYPE_HSDPA:
            case TelephonyManager.NETWORK_TYPE_HSPA:
            case TelephonyManager.NETWORK_TYPE_HSPAP:
            case TelephonyManager.NETWORK_TYPE_HSUPA:
            case TelephonyManager.NETWORK_TYPE_UMTS:
            case TelephonyManager.NETWORK_TYPE_TD_SCDMA:
                name += " 3G";
                break;
            case TelephonyManager.NETWORK_TYPE_LTE:
                if (curNrState()) {
                    name += " 5G";
                } else {
                    name += " 4G";
                }
                break;
            case TelephonyManager.NETWORK_TYPE_LTE_CA:
                if (curNrState()) {
                    name += " 5G";
                } else {
                    name += " 4G+";
                }
                break;
            case TelephonyManager.NETWORK_TYPE_NR:
                name += " 5G";
                break;
        }

        return name;
    }

    private boolean curNrState() {
        boolean ret = false;
        if (mTelephonyDisplayInfo != null) {
            int overrideNetworkType = mTelephonyDisplayInfo.getOverrideNetworkType();
            if (overrideNetworkType >= TelephonyDisplayInfo.OVERRIDE_NETWORK_TYPE_NR_NSA)
                ret = true;
        }
        return ret;
    }

    @Override
    public int getCarrierNameDisplayBitmask(ServiceState ss) {
        PersistableBundle config = CarrierConfigManager.getDefaultConfig();
        CarrierConfigManager configManager = (CarrierConfigManager) mPhone.getContext()
            .getSystemService(Context.CARRIER_CONFIG_SERVICE);
        if (configManager != null) {
            // If an invalid subId is used, this bundle will contain default values.
            config = configManager.getConfigForSubId(mPhone.getSubId());
        }

        if (mNameRulePlmnSpn) {
            Rlog.i(LOG_TAG, "set Display Rule as 3 (PLMN+SPN) for CMCC");
            return CARRIER_NAME_DISPLAY_BITMASK_SHOW_PLMN | CARRIER_NAME_DISPLAY_BITMASK_SHOW_SPN;
        } else if (config.getBoolean("carrier_spn_display_rule_force_show_plmn_bool")) {
            return CARRIER_NAME_DISPLAY_BITMASK_SHOW_PLMN;
        } else if (ss.getOperatorAlphaLong() != null && !ss.getOperatorAlphaLong().equals("")
                && ss.getOperatorAlphaLong().contains("AT&T")) {
            return CARRIER_NAME_DISPLAY_BITMASK_SHOW_PLMN;
        } else {
            return super.getCarrierNameDisplayBitmask(ss);
        }
    }

    private void updateCarrierConfiguration() {
        PersistableBundle config = CarrierConfigManager.getDefaultConfig();
        CarrierConfigManager configManager = (CarrierConfigManager) mPhone.getContext()
            .getSystemService(Context.CARRIER_CONFIG_SERVICE);
        if (configManager != null) {
            // If an invalid subId is used, this bundle will contain default values.
            config = configManager.getConfigForSubId(mPhone.getSubId());
        }
        mShowRatInfo = config.getBoolean("show_rat_info_with_operator_name_bool", false);
        mNameRulePlmnSpn = config.getBoolean("name_rule_to_show_plmn_with_spn_bool", false);
    }

}
