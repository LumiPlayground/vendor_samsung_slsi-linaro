/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */
package com.samsung.slsi.telephony.oemril.multisim;

import java.util.ArrayList;
import java.util.List;

import com.android.internal.telephony.PhoneConstants;
import com.android.internal.telephony.TelephonyIntents;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.telephony.SubscriptionInfo;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
import android.telephony.UiccSlotInfo;
import android.util.Log;
import android.util.SparseArray;

public class OemMultiSimSettingController extends Handler {

    public static final String TAG = OemMultiSimSettingController.class.getSimpleName();
    protected Context mContext;
    private TelephonyManager mTelephonyManager;
    private SubscriptionManager mSubscriptionManager;
    private int mNumPhones;
    private SparseArray<PhoneConfig> mPhoneConfigs = new SparseArray<>();
    private DdsConfig mDdsConfig;

    private static final int EVENT_DEFAULT_DATA_SUBSCRIPTION_CHANGED = 0;
    private static final int EVENT_MULTI_SIM_CONFIG_CHANGED = 1;
    private static final int EVENT_SET_NETWORK_CAPABILITY = 2;

    private static int DELAY_FOR_SET_PREFERRED_NTW_TYPE = 1000;

    private final BroadcastReceiver mReceiver = new BroadcastReceiver() {

        @Override
        public void onReceive(Context context, Intent intent) {
            final String action = intent.getAction();
            if (action.equals(TelephonyIntents.ACTION_DEFAULT_DATA_SUBSCRIPTION_CHANGED)) {
                Log.v(TAG, "TelephonyIntents.ACTION_DEFAULT_DATA_SUBSCRIPTION_CHANGED");
                int subId = intent.getIntExtra(PhoneConstants.SUBSCRIPTION_KEY, -1);
                int phoneId = SubscriptionManager.getPhoneId(subId);
                Log.v(TAG, "DDS: phoneId=" + phoneId + " subId=" + subId);
                sendMessage(obtainMessage(EVENT_DEFAULT_DATA_SUBSCRIPTION_CHANGED, new DdsConfig(phoneId, subId)));
            }
            else if (action.equals(TelephonyManager.ACTION_MULTI_SIM_CONFIG_CHANGED)) {
                sendMessage(obtainMessage(EVENT_MULTI_SIM_CONFIG_CHANGED));
            }
            else {

            }
        }
    };

    private static OemMultiSimSettingController sInstance = null;

    /**
     * Static method to get instance.
     */
    public static OemMultiSimSettingController getInstance() {
        if (sInstance == null) {
            Log.wtf(TAG, "getInstance null");
        }
        return sInstance;
    }

    /**
     * Init method to instantiate the object
     * Should only be called once.
     */
    public static OemMultiSimSettingController init(Context context) {
        synchronized (OemMultiSimSettingController.class) {
            if (sInstance == null) {
                sInstance = new OemMultiSimSettingController(context);
            } else {
                Log.wtf(TAG, "init() called multiple times!  sInstance = " + sInstance);
            }
            Log.d(TAG, "OemMultiSimSettingController initialized");
            return sInstance;
        }
    }

    private static class DdsConfig {
        private int mPhoneId;
        private int mSlotIndex;
        private int mSubId;

        public DdsConfig() {
            this(SubscriptionManager.INVALID_PHONE_INDEX, SubscriptionManager.INVALID_SIM_SLOT_INDEX,
                    SubscriptionManager.INVALID_SUBSCRIPTION_ID);
        }

        public DdsConfig(int phoneId, int slotIndex, int subId) {
            mPhoneId = phoneId;
            mSlotIndex = slotIndex;
            mSubId = subId;
        }

        public DdsConfig(int phoneId, int subId) {
            this(phoneId, phoneId, subId);
        }

        public int getPhoneId() { return mPhoneId; }
        public int getSubId() { return mSubId; }

        public boolean isValid() {
            return SubscriptionManager.isValidPhoneId(mPhoneId) &&
                   SubscriptionManager.isValidSubscriptionId(mSubId);
        }

        @Override
        public String toString() {
            StringBuilder sb = new StringBuilder();
            sb.append("DdsConfig:{");
            sb.append(" mPhoneId=" + mPhoneId);
            sb.append(" mSlotIndex=" + mSlotIndex);
            sb.append(" mSubId=" + mSubId);
            sb.append("}");
            return sb.toString();
        }

        @Override
        public boolean equals(Object o) {
            if (o == null) {
                return false;
            }
            return  (((DdsConfig)o).mPhoneId == this.mPhoneId) && (((DdsConfig)o).mSubId == this.mSubId);
        }
    }

    private OemMultiSimSettingController(Context context) {
        super(Looper.getMainLooper());
        mContext = context;
        mDdsConfig = new DdsConfig();

        // TelephonyManager instance
        mTelephonyManager = mContext.getSystemService(TelephonyManager.class);
        mNumPhones = mTelephonyManager.getActiveModemCount();
        updatePhones();

        // SubscriptionManager instance
        mSubscriptionManager = mContext.getSystemService(SubscriptionManager.class);

        // intent broadcast
        IntentFilter filter = new IntentFilter();
        filter.addAction(TelephonyIntents.ACTION_DEFAULT_DATA_SUBSCRIPTION_CHANGED);
        filter.addAction(TelephonyManager.ACTION_MULTI_SIM_CONFIG_CHANGED);
        context.registerReceiver(mReceiver, filter);
    }

    private void updatePhones() {
        Log.v(TAG, "updatePhones: mNumPhones=" + mNumPhones);
        if (mNumPhones > 0) {
            SparseArray<PhoneConfig> configs = new SparseArray<>();
            for (int i = 0; i < mNumPhones; i++) {
                if (mPhoneConfigs.contains(i)) {
                    configs.put(i, mPhoneConfigs.get(i));
                    Log.v(TAG, "[" + i + "] " + configs.get(i) + "(R)");
                }
                else {
                    configs.put(i, new PhoneConfig(mContext, i));
                    Log.v(TAG, "[" + i + "] " + configs.get(i) + "(N)");
                }
            }  // end for i ~
            mPhoneConfigs = configs;
        }
    }

    @Override
    public void handleMessage(Message msg) {
        DdsConfig ddsConfig;
        switch (msg.what) {
        case EVENT_DEFAULT_DATA_SUBSCRIPTION_CHANGED:
            ddsConfig = (DdsConfig) msg.obj;
            onDefaultDataSubscriptionChanged(ddsConfig);
            break;
        case EVENT_MULTI_SIM_CONFIG_CHANGED:
            onMultiSimConfigChanged();
            break;
        case EVENT_SET_NETWORK_CAPABILITY:
            ddsConfig = (DdsConfig) msg.obj;
            onSetNetworkCapability(ddsConfig);
            break;
        default:
            return;
        }
    }

    private void onDefaultDataSubscriptionChanged(DdsConfig ddsConfig) {
        Log.d(TAG, "onDefaultDataSubscriptionChanged");

        Message msg = obtainMessage(EVENT_SET_NETWORK_CAPABILITY, ddsConfig);
        sendMessageDelayed(msg, DELAY_FOR_SET_PREFERRED_NTW_TYPE);
    }

    private void onSetNetworkCapability(DdsConfig ddsConfig) {
        Log.d(TAG, "onSetNetworkCapability");
        DdsConfig oldDdsConfig = mDdsConfig;
        mDdsConfig = ddsConfig;
        if (!mDdsConfig.equals(oldDdsConfig)) {
            Log.d(TAG, "" + oldDdsConfig + "->" + mDdsConfig);
            reEvaluateAll();
        }
    }

    private List<PhoneConfig> getNonDdsPhoneConfigs(int dds) {
        ArrayList<PhoneConfig> configs = new ArrayList<>();
        for (int i = 0; i < mNumPhones; i++) {
            PhoneConfig phoneConfig = mPhoneConfigs.get(i);
            if (phoneConfig != null && phoneConfig.getSubId() != dds) {
                configs.add(phoneConfig);
            }
        }
        return configs;
    }

    private PhoneConfig getDdsPhoneConfig(int dds) {
        return getPhoneConfigUsingSubId(dds);
    }

    private void setDdsPhoneConfig(PhoneConfig phoneConfig) {
        Log.v(TAG, "setDdsPhoneConfig: " + phoneConfig);
        // add NR capability
        if (phoneConfig != null) {
            phoneConfig.onUpdateRadioCapability(true);
        }

        // more configs for DDS (TBD)
    }

    private void setNonDdsPhoneConfig(PhoneConfig phoneConfig) {
        Log.v(TAG, "setNonDdsPhoneConfig: " + phoneConfig);
        // remove NR capability
        if (phoneConfig != null) {
            phoneConfig.onUpdateRadioCapability(false);
        }

        // more configs for Non-DDS (TBD)
    }

    private void setNonDdsPhonesConfig(List<PhoneConfig> phoneConfigs) {
        for (PhoneConfig config : phoneConfigs) {
            setNonDdsPhoneConfig(config);
        }
    }

    private void onMultiSimConfigChanged() {
        int oldNumPhones = mNumPhones;
        mNumPhones = getPhoneCount();

        if (oldNumPhones != mNumPhones) {
            Log.v(TAG, "ActiveModemCount changed from " + oldNumPhones + " to " + mNumPhones);
            updatePhones();
        }
    }

    /**
     * Returns how many phone objects the device supports.
     */
    public int getPhoneCount() {
        return mTelephonyManager.getActiveModemCount();
    }

    private boolean isMultiSim() { return mNumPhones > 1; }

    private PhoneConfig getPhoneConfigUsingSubId(int subId) {
        if (!SubscriptionManager.isValidSubscriptionId(subId)) {
            return null;
        }

        PhoneConfig config = null;
        for (int i = 0; i < mNumPhones; i++) {
            config = mPhoneConfigs.get(i);
            if (config != null && config.getSubId() == subId) {
                break;
            }
        } // end for i ~
        return config;
    }

    public int getSubIdUsingPhoneId(int phoneId) {
        int[] subIds = mSubscriptionManager.getSubscriptionIds(phoneId);
        if (subIds == null || subIds.length == 0) {
            return SubscriptionManager.INVALID_SUBSCRIPTION_ID;
        }
        return subIds[0];
    }

    private boolean isReadyToReevaluate() {
        int presentUiccCount = 0;
        UiccSlotInfo[] uiccSlotInfos = mTelephonyManager.getUiccSlotsInfo();
        if (uiccSlotInfos == null) {
            Log.w(TAG, "No UICC slots info");
            return false;
        }
        for (UiccSlotInfo uiccSlotInfo : uiccSlotInfos) {
            if (uiccSlotInfo != null &&
                uiccSlotInfo.getIsActive() &&
                uiccSlotInfo.getCardStateInfo() == UiccSlotInfo.CARD_STATE_INFO_PRESENT) {
                presentUiccCount++;
            }
        } // end for ~

        int enabledUiccCount = 0;
        List<SubscriptionInfo> subInfos = mSubscriptionManager.getActiveSubscriptionInfoList();
        for (SubscriptionInfo subInfo : subInfos) {
            if (subInfo != null && subInfo.areUiccApplicationsEnabled()) {
                enabledUiccCount++;
            }
        } // end for ~

        int availableSubscriptionCount = presentUiccCount;
        if (availableSubscriptionCount > enabledUiccCount) {
            availableSubscriptionCount = enabledUiccCount;
        }

        int readySubscriptionCount = 0;
        for (int i = 0; i < mNumPhones; i++) {
            PhoneConfig phoneConfig = mPhoneConfigs.get(i);
            if (phoneConfig != null && SubscriptionManager.isValidSubscriptionId(phoneConfig.getSubId())) {
                readySubscriptionCount++;
            }
        }

        // consider a single SIM card in DSDS mode
        int allowedNetworkTypesScore = 0;
        for (int i = 0; i < mNumPhones; i++) {
            PhoneConfig phoneConfig = mPhoneConfigs.get(i);
            if (phoneConfig != null && phoneConfig.isAllowedNetworkTypesLoadedFromDb())
                allowedNetworkTypesScore++;
        }

        Log.d(TAG, "availableSubscriptionCount=" + availableSubscriptionCount +
                " presentUiccCount=" + presentUiccCount +
                " enabledUiccCount=" + enabledUiccCount +
                " readySubscriptions=" + readySubscriptionCount +
                " allowedNetworkTypesScore=" + allowedNetworkTypesScore +
                " " + mDdsConfig +
                " isMultiSim=" + isMultiSim());
        return (availableSubscriptionCount > 0) &&
               (availableSubscriptionCount == readySubscriptionCount) &&
               (allowedNetworkTypesScore >= readySubscriptionCount) &&
               (mDdsConfig != null && mDdsConfig.isValid());
    }

    private void reEvaluateAll() {
        // if (!isMultiSim()) return ;

        if (!isReadyToReevaluate()) return;

        if (mDdsConfig == null ||
            !SubscriptionManager.isValidSubscriptionId(mDdsConfig.getSubId()))
            return;

        Log.d(TAG, "reEvaluateAll");
        int ddsSubId = mDdsConfig.getSubId();
        setNonDdsPhonesConfig(getNonDdsPhoneConfigs(ddsSubId));
        setDdsPhoneConfig(getDdsPhoneConfig(ddsSubId));
    }

    public void notifySubscriptionLoaded(PhoneConfig phoneConfig) {
        Log.d(TAG, "notifySubscriptionLoaded: phoneConfig=" + phoneConfig);
        reEvaluateAll();
    }

    public void notifyAllowedNetworkTypesLoaded(PhoneConfig phoneConfig) {
        Log.d(TAG, "notifyAllowedNetworkTypeChanged: phoneConfig=" + phoneConfig);
        reEvaluateAll();
    }
}
