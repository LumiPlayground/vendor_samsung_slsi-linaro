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

import com.android.internal.telephony.GlobalSettingsHelper;
import com.android.internal.telephony.PhoneConstants;
import com.samsung.slsi.telephony.oemril.OemCommandsHelper;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.database.ContentObserver;
import android.net.Uri;
import android.os.AsyncResult;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.os.PersistableBundle;
import android.os.SystemProperties;
import android.preference.PreferenceManager;
import android.provider.Settings;
import android.telephony.CarrierConfigManager;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyCallback;
import android.telephony.TelephonyManager;
import android.telephony.SubscriptionManager.OnSubscriptionsChangedListener;
import android.telephony.ims.ImsManager;
import android.telephony.ims.ImsMmTelManager;
import android.text.TextUtils;

import java.util.concurrent.Executor;
import android.util.Log;

import static com.samsung.slsi.telephony.oemril.multisim.MobileNetworkUtils.*;

public class PhoneConfig extends Handler {
    private static final String LOG_TAG = PhoneConfig.class.getSimpleName();
    private final Context mContext;
    private final int mPhoneId;
    private int mSubId = -1;
    private OemCommandsHelper mCi;
    private TelephonyManager mTelephonyManager;
    private CarrierConfigManager mCarrierConfigManager;
    private SubscriptionManager mSubscriptionManager;
    private SimInfoObserver mSimInfoObserver;
    private boolean mLastAdvancedCallingSettingEnabled = false;
    private boolean mIsDds;
    private boolean mForceUpdate = false;
    private AllowedNetworkTypesListener mAllowedNetworkTypesListener;
    private boolean mIsAllowedNetworkTypesLoadedFromDb = false;
    private DataStateListener mDataStateListener;

    private boolean mDataRoamingEnabled = false;
    private boolean mDataEnabled = false;

    private static final int EVENT_CARRIER_CONFIG_CHANGED = 0;
    private static final int EVENT_SET_VOICE_OPERTION_DONE = 1;
    private static final int EVENT_GET_VOICE_OPERTION = 2;
    private static final int EVENT_GET_VOICE_OPERTION_DONE = 3;
    private static final int EVENT_ADVANCED_CALLING_SETTING_CHANGED = 4;
    private static final int EVENT_NR_SWTICH_CHANGED = 5;
    private static final int EVENT_ALLOWED_NETWORK_TYPES_AVAILABLE = 6;
    private static final int EVENT_SET_MOBILE_DATA_STATE = 7;

    private static final String REASON_CARRIER_CONFIG_CHANGED = "carrier_config_changed";

    private static final String ACTION_NR_SWITCH_CHANGED = "vendor.samsung_slsi.telephony.NR_SWITCH_CHANGED";
    private static final String EXTRA_STATE = "enable";
    private static final String CURR_SUBID = "cur_subid";
    public static final String PROPERTY_ALLOWED_NETWORK_TYPES_LOADED = "vendor.radio.allowed_types_loaded";
    private static final String PROPERTY_SUPPORT_NR_DS = "persist.vendor.ril.support_nr_ds";
    public static final int DELAY_ESTIMATE_ALLOWED_NETWORK_TYPES_TIMEOUT = 10 * 1000;

    private final BroadcastReceiver mReceiver = new BroadcastReceiver() {

        @Override
        public void onReceive(Context context, Intent intent) {
            final String action = intent.getAction();
            if (action.equals(CarrierConfigManager.ACTION_CARRIER_CONFIG_CHANGED)) {
                // Only handle carrier config changes for this phone id.
                if (mPhoneId == intent.getIntExtra(CarrierConfigManager.EXTRA_SLOT_INDEX, -1)) {
                    final PersistableBundle carrierConfig = mCarrierConfigManager.getConfigForSubId(mSubId);
                    if (carrierConfig.getBoolean(CarrierConfigManager.KEY_CARRIER_CONFIG_APPLIED_BOOL)) {
                        sendMessage(obtainMessage(EVENT_CARRIER_CONFIG_CHANGED));
                        sendMessage(obtainMessage(EVENT_ALLOWED_NETWORK_TYPES_AVAILABLE));
                    }
                }
            }
            else if (action.equals(TelephonyManager.ACTION_SIM_CARD_STATE_CHANGED)) {
                if (mPhoneId == intent.getIntExtra(PhoneConstants.PHONE_KEY, -1)) {
                    int cardState = intent.getIntExtra(TelephonyManager.EXTRA_SIM_STATE, -1);
                    if (cardState == TelephonyManager.SIM_STATE_PRESENT) {
                        doGetVoiceOperation();
                    }
                }
            }
            else if (action.equals(ACTION_NR_SWITCH_CHANGED)) {
                int subId = intent.getIntExtra(SubscriptionManager.EXTRA_SUBSCRIPTION_INDEX, -1);
                if (isValidSubscriptionId(subId) && subId == mSubId) {
                    boolean enabled = intent.getBooleanExtra(EXTRA_STATE, false);
                    tryEnableNr(enabled);
                }
            }
            else {
            }
        }
    };

    public class SimInfoObserver extends ContentObserver {
        public SimInfoObserver(Handler handler) {
            super(handler);
        }

        public void register(Context context) {
            final Uri uri = SubscriptionManager.CONTENT_URI;
            context.getContentResolver().registerContentObserver(uri, false, this);
        }

        public void unregister(Context context) {
            context.getContentResolver().unregisterContentObserver(this);
        }

        @Override
        public void onChange(boolean selfChange) {
            super.onChange(selfChange);
            // Expect to get changing status of SubscriptionManager.ENHANCED_4G_MODE_ENABLED
            onDataRoamingChanged();
        }
    }

    private final OnSubscriptionsChangedListener mOnSubscriptionChangedListener = new OnSubscriptionsChangedListener() {

        private int getSubId() {
            if (mSubscriptionManager == null) {
                return  SubscriptionManager.DEFAULT_SUBSCRIPTION_ID;
            }

            int[] subIds = mSubscriptionManager.getSubscriptionIds(mPhoneId);
            if (subIds == null || subIds.length == 0) {
                return SubscriptionManager.INVALID_SUBSCRIPTION_ID;
            }
            return subIds[0];
        }

        @Override
        public void onSubscriptionsChanged() {
            int prevSubId = mSubId;
            mSubId = getSubId();
            if (prevSubId != mSubId) {
                log("onSubscriptionsChanged prev=" + prevSubId + " subId=" + mSubId);
                if (isValidSubscriptionId(mSubId)) {
                    if (getStoredSubId() != mSubId) {
                        mForceUpdate = true;
                    }
                    OemMultiSimSettingController.getInstance().notifySubscriptionLoaded(PhoneConfig.this);
                    // a state recovery from OemRilService crash because of  no onAllowedNetworkTypesChanged callback
                    updateAllowedNetworkTypesLoadedFromProperty();

                    // AllowedNetworkTypes is not loaded yet or unknown.
                    // try to listen onAllowedNetworkTypesChanged callback
                    if (!mIsAllowedNetworkTypesLoadedFromDb) {
                        regAllowedNetworkTypesListener(mSubId);
                    }

                    // try to listen DataEnabledTelephonyCallback
                    regDataStateListener(mSubId);
                } else if (isValidSubscriptionId(prevSubId) && !isValidSubscriptionId(mSubId)) {
                    unRegAllowedNetworkTypesListener(prevSubId);
                    unRegDataStateListener(prevSubId);
                    setAllowedNetworkTypesLoaded(false);
                }
                storeSubId(mSubId);
            }
        }
    };

    public PhoneConfig(Context context, int phoneId) {
        super(Looper.getMainLooper());
        mContext = context;
        mPhoneId = phoneId;

        mCi = OemCommandsHelper.from(mContext, mPhoneId);
        mTelephonyManager = (TelephonyManager) context.getSystemService(Context.TELEPHONY_SERVICE);
        mCarrierConfigManager = (CarrierConfigManager) context.getSystemService(Context.CARRIER_CONFIG_SERVICE);

        // subscription changed event
        mSubscriptionManager = (SubscriptionManager) context.getSystemService(Context.TELEPHONY_SUBSCRIPTION_SERVICE);
        mSubscriptionManager.addOnSubscriptionsChangedListener(mOnSubscriptionChangedListener);

        // intent broadcast
        IntentFilter filter = new IntentFilter();
        filter.addAction(CarrierConfigManager.ACTION_CARRIER_CONFIG_CHANGED);
        filter.addAction(TelephonyManager.ACTION_SIM_CARD_STATE_CHANGED);
        filter.addAction(ACTION_NR_SWITCH_CHANGED);
        context.registerReceiver(mReceiver, filter);

        // SimInfo Observer
        mSimInfoObserver = new SimInfoObserver(this);
        mSimInfoObserver.register(context);

        // AllowedNetworkType Listener
        mAllowedNetworkTypesListener = new AllowedNetworkTypesListener(
                mContext.getMainExecutor());
        mAllowedNetworkTypesListener.setAllowedNetworkTypesListener(
                () -> onAllowedNetworkTypesChanged());

        // Data State Listener
        mDataStateListener = new DataStateListener(mContext.getMainExecutor());
        mDataStateListener.setDataStateListener(
                (enabled) -> PhoneConfig.this.onDataEnabledChanged(enabled));
    }

    public int getSubId() {
        return mSubId;
    }

    private boolean isValidSubscriptionId() {
        return SubscriptionManager.isValidSubscriptionId(mSubId);
    }

    private boolean isValidSubscriptionId(int subId) {
        return SubscriptionManager.isValidSubscriptionId(subId);
    }

    private boolean isAdvancedCallingSettingEnabled() {
        ImsMmTelManager imsMmtelManager = new ImsManager(mContext).getImsMmTelManager(mSubId);
        return imsMmtelManager.isAdvancedCallingSettingEnabled();
    }

    private void checkAdvancedCallingSettingChanged(String reason) {
        if (!isValidSubscriptionId()) {
            return ;
        }

        // Advanced Calling Setting (VoLTE)
        try {
            boolean enabled = isAdvancedCallingSettingEnabled();
            if (mLastAdvancedCallingSettingEnabled != enabled) {
                sendMessage(obtainMessage(EVENT_ADVANCED_CALLING_SETTING_CHANGED, enabled));
                log("Request doSetVoiceOperation due to " + reason);
                mLastAdvancedCallingSettingEnabled = enabled;
            }
        }
        catch (IllegalArgumentException e) {
            logw("IllegalArgumentException: subId" + mSubId);
        }
        catch (RuntimeException e) {
            logw("RuntimeException: subId" + mSubId);
        }
    }

    private void storeSubId(int subId) {
        // Update stored subId
        SharedPreferences sp = PreferenceManager.getDefaultSharedPreferences(mContext);
        SharedPreferences.Editor editor = sp.edit();
        editor.putInt(CURR_SUBID + mPhoneId, subId);
        editor.apply();
    }

    private int getStoredSubId() {
        SharedPreferences sp = PreferenceManager.getDefaultSharedPreferences(mContext);
        int storedSubId = sp.getInt(CURR_SUBID + mPhoneId, -1);
        log("storedSubId:" + storedSubId + " for phoneId " + mPhoneId);
        return storedSubId;
    }

    @Override
    public void handleMessage(Message msg) {
        AsyncResult ar;
        switch (msg.what) {
        case EVENT_CARRIER_CONFIG_CHANGED: {
            log("EVENT_CARRIER_CONFIG_CHANGED");
            checkAdvancedCallingSettingChanged(REASON_CARRIER_CONFIG_CHANGED);
            break;
        }
        case EVENT_SET_VOICE_OPERTION_DONE: {
            ar = (AsyncResult) msg.obj;
            boolean enabled = ((boolean) ar.userObj);
            if (ar.exception == null) {
                log("doSetVoiceOperation success: " + !enabled + "->" + enabled);
            }
            else {
                loge("doSetVoiceOperation error.");
            }
            break;
        }
        case EVENT_GET_VOICE_OPERTION:
            doGetVoiceOperation();
            break;
        case EVENT_GET_VOICE_OPERTION_DONE:
            ar = (AsyncResult) msg.obj;
            if (ar.exception == null) {
                mLastAdvancedCallingSettingEnabled = (boolean) ar.result;
                log("mLastAdvancedCallingSettingEnabled=" + mLastAdvancedCallingSettingEnabled);
            }
            else {
                loge("doGetVoiceOperation error" + ar.exception);
            }
            break;
        case EVENT_ADVANCED_CALLING_SETTING_CHANGED: {
            log("EVENT_ADVANCED_CALLING_SETTING_CHANGED");
            boolean enabled = (boolean) msg.obj;
            doSetVoiceOperation(enabled);
            break;
        }
        case EVENT_NR_SWTICH_CHANGED: {
            boolean enabled = (boolean) msg.obj;
            enableNr(enabled);
            break;
        }
        case EVENT_ALLOWED_NETWORK_TYPES_AVAILABLE: {
            log("mIsAllowedNetworkTypesLoadedFromDb=" + mIsAllowedNetworkTypesLoadedFromDb);

            // Phone#mIsAllowedNetworkTypesLoadedFromDb can be updated by
            // ACTION_CARRIER_CONFIG_CHANGED. But PhoneConfig can receive the intent
            // more ealier than  GsmCdmaPhone.
            // A recovery for the worst case because there is no way to know whether
            //  Phone#mIsAllowedNetworkTypesLoadedFromDb is updated or not.
            if (!mIsAllowedNetworkTypesLoadedFromDb) {
                log("call onAllowedNetworkTypesChanged() by gussing");
                postDelayed(() -> onAllowedNetworkTypesChanged(),
                    DELAY_ESTIMATE_ALLOWED_NETWORK_TYPES_TIMEOUT);
            }
            break;
        }
        case EVENT_SET_MOBILE_DATA_STATE: {
            log("EVENT_SET_MOBILE_DATA_STATE");
            ar = (AsyncResult) msg.obj;
            if (ar.exception != null) {
                loge("doSetMobileDataState error.");
            }
            break;
        }
        default:
            return;
        }  // end switch ~
    }

    private void enableNr(boolean enabled) {
        log("enableNr " + enabled);

        if (!isSupported5gRadioAccessFamily()) {
            // don't need to update NR capability
            // NR not supported by modem RAF
            return ;
        }

        boolean isSupported5gNetworkType =
                MobileNetworkUtils.checkSupportedRadioBitmask(getPreferredNetworkTypeBitmask(), NR);

        if (isSupported5gNetworkType && enabled) {
            log("enableNr: NR already enabled.");
            return ;
        }

        if (!isSupported5gNetworkType && !enabled) {
            log("enableNr: NR already disabled.");
            return ;
        }

        // add NR capability if enabled true
        // no change if enabled false
        if (enabled) {
            updateAllowedNetworkTypes(-1);
        }

        // set network type
        NetworkModeSelector selector = new NetworkModeSelector(mContext, mSubId);
        TelephonyManager tm = mTelephonyManager.createForSubscriptionId(mSubId);
        int desiredNetworkType = selector.getDesiredNetworkType(enabled);
        long desiredNetworkTypeBitmask = selector.getDesiredNetworkTypeBitmask(enabled);
        logv("desiredNetworkType=" + desiredNetworkType
            + " desiredNetworkTypeBitmask=" + String.format("%08X", desiredNetworkTypeBitmask));
        if (!tm.setPreferredNetworkTypeBitmask(desiredNetworkTypeBitmask)) {
            loge("Fail to setPreferredNetworkTypeBitmask.");
        }
    }

    private long getPreferredNetworkTypeBitmask() {
        return MobileNetworkUtils.getPreferredNetworkTypeBitmask(mContext, mSubId);
    }

    private void doSetVoiceOperation(boolean enabled) {
        log("doSetVoiceOperation");
        mCi.setVoiceOperation(enabled, obtainMessage(EVENT_SET_VOICE_OPERTION_DONE, enabled));
    }

    private void doGetVoiceOperation() {
        log("doGetVoiceOperation");
        mCi.getVoiceOperation(obtainMessage(EVENT_GET_VOICE_OPERTION_DONE));
    }

    public void onUpdateRadioCapability(boolean isDds) {
        mIsDds = isDds;
        reEvaluateRadioCapability();
    }

    private void reEvaluateRadioCapability() {
        log("reEvaluateRadioCapability");
        if (!isValidSubscriptionId()) {
            return ;
        }

        onUpdateNrCapability();
    }

    private void onUpdateNrCapability() {
        log("onUpdateNrCapability");
        if (!isValidSubscriptionId()) {
            logw("invalid subId");
            return ;
        }

        if (!isSupported5gRadioAccessFamily()) {
            // don't need to update NR capability
            // NR not supported by modem RAF
            return ;
        }

        NetworkModeSelector selector = new NetworkModeSelector(mContext, mSubId);
        long allowedNetworkTypes = selector.getAllowedNetworkTypes();
        boolean isNrDsSupported = SystemProperties.getInt(PROPERTY_SUPPORT_NR_DS, 0) == 1;
        long desiredAllowedNetworkTypes = selector.getDesiredAllowedNetworkTypes(mIsDds, isNrDsSupported);
        log("allowedNetworkTypes=" +  String.format("%08X", allowedNetworkTypes)
            + " desiredAllowedNetworkTypes=" + String.format("%08X", desiredAllowedNetworkTypes)
            + " mForceUpdate=" + mForceUpdate);
        if (allowedNetworkTypes != desiredAllowedNetworkTypes || mForceUpdate) {
            updateAllowedNetworkTypes(desiredAllowedNetworkTypes);
            mForceUpdate = false;
        } else {
            log("No change for phoneId " + mPhoneId +" (DDS:" + mIsDds + ")");
        }
    }

    public boolean updateAllowedNetworkTypes(long allowedNetworkTypes) {
        if (!isValidSubscriptionId()) return false;

        TelephonyManager tm = mTelephonyManager.createForSubscriptionId(mSubId);
        tm.setAllowedNetworkTypesForReason(
                TelephonyManager.ALLOWED_NETWORK_TYPES_REASON_CARRIER,
                allowedNetworkTypes);
        return true;
    }

    private void tryEnableNr(boolean enabled) {
        sendMessage(obtainMessage(EVENT_NR_SWTICH_CHANGED, enabled));
    }

    private boolean isSupported5gRadioAccessFamily() {
        if (!isValidSubscriptionId()) return false;

        TelephonyManager tm = mTelephonyManager.createForSubscriptionId(mSubId);
        boolean supported5gRadioAccessFamily = MobileNetworkUtils.checkSupportedRadioBitmask(
                tm.getSupportedRadioAccessFamily(), TelephonyManager.NETWORK_TYPE_BITMASK_NR);
        return supported5gRadioAccessFamily;
    }

    private void regAllowedNetworkTypesListener(int subId) {
        if (subId != SubscriptionManager.INVALID_SUBSCRIPTION_ID
                && mAllowedNetworkTypesListener != null) {
            mAllowedNetworkTypesListener.register(mContext, subId);
        }
    }

    private void unRegAllowedNetworkTypesListener(int subId) {
        if (subId != SubscriptionManager.INVALID_SUBSCRIPTION_ID
                && mAllowedNetworkTypesListener != null) {
            mAllowedNetworkTypesListener.unregister(mContext, subId);
        }
    }

    private void setAllowedNetworkTypesLoaded(boolean isLoaded) {
        mIsAllowedNetworkTypesLoadedFromDb = isLoaded;
        /*
        if (SubscriptionManager.isValidPhoneId(mPhoneId)) {
            SystemProperties.set(PROPERTY_ALLOWED_NETWORK_TYPES_LOADED + mPhoneId, isLoaded ? "1" : "0");
        }
        */
    }

    private void updateAllowedNetworkTypesLoadedFromProperty() {
        if (SubscriptionManager.isValidPhoneId(mPhoneId)) {
            String val = SystemProperties.get(PROPERTY_ALLOWED_NETWORK_TYPES_LOADED + mPhoneId, "");
            if (TextUtils.isDigitsOnly(val)) {
                log("updateAllowedNetworkTypesLoadedFromProperty for phoneId " +
                        mPhoneId + " " + val);
                mIsAllowedNetworkTypesLoadedFromDb = "1".equals(val);
            }
        }
    }

    public boolean isAllowedNetworkTypesLoadedFromDb() {
        return mIsAllowedNetworkTypesLoadedFromDb;
    }

    public void onAllowedNetworkTypesChanged() {
        // ignore re-evaluate because
        // the scenario can be conflict with CTS android.telephony.cts.TelephonyManagerTest#testDisAllowedNetworkTypes
        if (!mIsAllowedNetworkTypesLoadedFromDb) {
            // stop listening
            unRegAllowedNetworkTypesListener(mSubId);
            setAllowedNetworkTypesLoaded(true);
            OemMultiSimSettingController.getInstance().notifyAllowedNetworkTypesLoaded(PhoneConfig.this);
        } else {
            log("mIsAllowedNetworkTypesLoadedFromDb already loaded");
        }
    }

    public static class AllowedNetworkTypesListener extends TelephonyCallback implements
            TelephonyCallback.AllowedNetworkTypesListener {
        private static final String LOG_TAG = "AllowedNetworkTypesListener";

        OnAllowedNetworkTypesListener mListener;
        private long mAllowedNetworkType = -1;
        private Executor mExecutor;

        public AllowedNetworkTypesListener(Executor executor) {
            super();
            mExecutor = executor;
        }

        public void setAllowedNetworkTypesListener(OnAllowedNetworkTypesListener lsn) {
            mListener = lsn;
        }

        /**
         * Register a TelephonyCallback for Allowed Network Types changed.
         * @param context the Context
         * @param subId the subscription id.
         */
        public void register(Context context, int subId) {
            TelephonyManager telephonyManager = context.getSystemService(
                    TelephonyManager.class).createForSubscriptionId(subId);
            telephonyManager.registerTelephonyCallback(mExecutor, this);
        }

        /**
         * Unregister a TelephonyCallback for Allowed Network Types changed.
         * @param context the Context
         * @param subId the subscription id.
         */
        public void unregister(Context context, int subId) {
            TelephonyManager telephonyManager = context.getSystemService(
                    TelephonyManager.class).createForSubscriptionId(subId);
            telephonyManager.unregisterTelephonyCallback(this);
        }

        @Override
        public void onAllowedNetworkTypesChanged(int reason, long newAllowedNetworkType) {
            if (mListener != null) {
                mListener.onAllowedNetworkTypesChanged();
            }
        }

        /**
         * Listener for update of Preferred Network Mode change
         */
        public interface OnAllowedNetworkTypesListener {
            /**
             * Notify the allowed network type changed.
             */
            void onAllowedNetworkTypesChanged();
        }
    }

    @Override
    public String toString() {
        StringBuilder sb = new StringBuilder();
        sb.append("PhoneConfig:{");
        sb.append(" mPhoneId=" + mPhoneId);
        sb.append(" mSubId=" + mSubId);
        sb.append("}");
        return sb.toString();
    }

    protected final void logv(String s) {
        if (Log.isLoggable(LOG_TAG, Log.VERBOSE)) {
            Log.v(LOG_TAG, "[" + mPhoneId + "] " + s);
        }
    }

    protected final void log(String s) {
        Log.d(LOG_TAG, "[" + mPhoneId + "] " + s);
    }

    protected final void logw(String s) {
        Log.w(LOG_TAG, "[" + mPhoneId + "] " + s);
    }

    protected final void loge(String s) {
        Log.e(LOG_TAG, "[" + mPhoneId + "] " + s);
    }

    private void onDataRoamingChanged() {
        if (!isValidSubscriptionId()) {
            return ;
        }

        try {
            boolean enabled = GlobalSettingsHelper.getBoolean(mContext, Settings.Global.DATA_ROAMING, mSubId, false);
            if (mDataRoamingEnabled != enabled) {
                Log.d(LOG_TAG, "checkDataRoamingChanged() enabled=" + enabled);
                mDataRoamingEnabled = enabled;
                doSetMobileDataState(mDataEnabled, mDataRoamingEnabled);
            }
        }
        catch (IllegalArgumentException e) {
            logw("IllegalArgumentException: subId" + mSubId);
        }
        catch (RuntimeException e) {
            logw("RuntimeException: subId" + mSubId);
        }
    }

    private void doSetMobileDataState(boolean mobileData, boolean roamingData) {
        log("doSetMobileDataState: mobile=" + mobileData + ", roaming=" + roamingData);
        mCi.setMobileDataState(mobileData, roamingData, obtainMessage(EVENT_SET_MOBILE_DATA_STATE));
    }

    private void regDataStateListener(int subId) {
        if (subId != SubscriptionManager.INVALID_SUBSCRIPTION_ID && mDataStateListener != null) {
            mDataStateListener.register(mContext, subId);
        }
    }

    private void unRegDataStateListener(int subId) {
        if (subId != SubscriptionManager.INVALID_SUBSCRIPTION_ID && mDataStateListener != null) {
            mDataStateListener.unregister(mContext, subId);
        }
    }

    public void onDataEnabledChanged(boolean enabled) {
        if (mDataEnabled != enabled) {
            Log.d(LOG_TAG, "onDataEnabledChanged() mSubId=" + mSubId + ", enabled=" + enabled);
            mDataEnabled = enabled;
            doSetMobileDataState(mDataEnabled, mDataRoamingEnabled);
        }
    }

    public static class DataStateListener extends TelephonyCallback implements TelephonyCallback.DataEnabledListener {

        OnDataStateListener mListener;
        private final Executor mExecutor;

        public DataStateListener(Executor executor) {
            super();
            mExecutor = executor;
        }

        public void setDataStateListener(OnDataStateListener lsn) {
            mListener = lsn;
        }

        /**
         * Register a TelephonyCallback for Data Enabled changed.
         * @param context the Context
         * @param subId the subscription id.
         */
        public void register(Context context, int subId) {
            TelephonyManager telephonyManager = context.getSystemService(TelephonyManager.class).createForSubscriptionId(subId);
            telephonyManager.registerTelephonyCallback(mExecutor, this);
        }

        /**
         * Unregister a TelephonyCallback for Data Enabled changed.
         * @param context the Context
         * @param subId the subscription id.
         */
        public void unregister(Context context, int subId) {
            TelephonyManager telephonyManager = context.getSystemService(TelephonyManager.class).createForSubscriptionId(subId);
            telephonyManager.unregisterTelephonyCallback(this);
        }

        @Override
        public void onDataEnabledChanged(boolean enabled, int reason) {
            if (mListener != null) {
                mListener.onDataEnabledChanged(enabled);
            }
        }

        public interface OnDataStateListener {
            void onDataEnabledChanged(boolean enabled);
        }
    }
}
