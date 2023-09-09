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

import android.content.Context;
import android.net.ConnectivityManager;
import android.net.Network;
import android.os.AsyncResult;
import android.os.Message;
import android.os.PersistableBundle;
import android.os.SystemClock;
import android.telephony.CarrierConfigManager;
import android.telephony.DataFailCause;
import android.telephony.Rlog;
import android.telephony.TelephonyManager;
import android.telephony.data.ApnSetting;
import android.telephony.data.DataCallResponse;
import android.text.TextUtils;
import android.util.Pair;

import java.net.Inet4Address;
import java.net.Inet6Address;
import java.net.InetAddress;
import java.net.UnknownHostException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.Random;
import java.util.Timer;
import java.util.TimerTask;

import com.android.internal.annotations.VisibleForTesting;

public class RetryHandler {
    private static final boolean DBG = true;

    protected static final int DEFAULT_RETRY_VALUE = (12 * 60 * 1000); // 12 minutes

    // carrier configurations
    private static final String OTHERS_APN_TYPE = "others";
    private static final String DEFAULT_DATA_RETRY_CONFIG = "max_retries=3, 5000, 5000, 5000";

    protected class BackoffTimerTask extends TimerTask {
        public void run() {
            mBackoffTimerTask = null;
            mBackoffTimerExecution = 0;
        }
    }

    private static class RetryRecord {
        long mDelayTime;
        long mRandomizationTime;

        RetryRecord() {
            mDelayTime = 0;
            mRandomizationTime = 0;
        }
    }

    protected String TAG;
    private final Context mContext;
    private final ConnectivityManager mConnectivityManager;
    protected final ApnType mApnType;
    protected final String mApnName;
    protected final int mSlotId;
    protected final ConfigurationManager mConfigurationManager;
    protected final SimInformation mSimInformation;
    protected final DnsResolver mDnsResolver;
    protected ConfigurationParameters mConfigurationParameters;
    protected LinkedList<String> mServerNames;
    protected LinkedList<InetAddress> mPcoServerAddresses;
    protected LinkedList<InetAddress> mServerAddresses;
    protected LinkedList<InetAddress> mServersBlacklist;
    protected HashMap<TunnelError, Integer> mDefaultRetryTimerMap;
    protected TunnelError mLastError;
    protected final Timer mBackoffTimer;
    protected BackoffTimerTask mBackoffTimerTask;
    protected long mBackoffTimerExecution;
    private final Message mApnUnthrottledMessage;
    // carrier config retry
    protected boolean mRetryForever;
    protected int mMaxRetryCount;
    protected int mRetryCount;
    protected final ArrayList<RetryRecord> mRetryArray;

    public RetryHandler(int slotId, ApnType apnType, String apnName, Context context,
                        ConfigurationManager configuration, DnsResolver resolver, Message message) {
        mSlotId = slotId;
        mApnType = apnType;
        mApnName = apnName;
        mContext = context;
        mConfigurationManager = configuration;
        mDnsResolver = resolver;
        mConnectivityManager = context.getSystemService(ConnectivityManager.class);
        mServerNames = new LinkedList<>();
        mPcoServerAddresses = new LinkedList<>();
        mServerAddresses = new LinkedList<>();
        mLastError = TunnelError.ERR_SUCCESS;
        mDefaultRetryTimerMap = new HashMap<>();
        mServersBlacklist = new LinkedList<>();
        mSimInformation = SimManager.getInstance().getSimInformation(slotId);
        mConfigurationParameters = mConfigurationManager.getConnectionConfiguration();
        mBackoffTimer = new Timer();
        mBackoffTimerExecution = 0;
        mBackoffTimerTask = null;
        mApnUnthrottledMessage = message;
        mRetryArray = new ArrayList<>();
        mRetryCount = 0;
        mMaxRetryCount = 0;
        mRetryForever = false;

        TAG = ShannonDataService.getServiceTAG("-RH-" + apnType.getUserFriendlyName() + "-" + slotId);

        onCarrierConfigChanged();
    }

    public void destroy() {
        mBackoffTimer.cancel();
        mBackoffTimer.purge();
        mPcoServerAddresses.clear();
        clearAddresses();
    }

    public void updatePcoServerAddresses(LinkedList<InetAddress> list) {
        mPcoServerAddresses.clear();
        mPcoServerAddresses.addAll(list);
    }

    protected InetAddress getFirstRemoteIpv4() {
        for (InetAddress a : mServerAddresses) {
            if (a instanceof Inet4Address) {
                return a;
            }
        }
        return null;
    }

    protected InetAddress getFirstRemoteIpv6() {
        for (InetAddress a : mServerAddresses) {
            if (a instanceof Inet6Address) {
                return a;
            }
        }
        return null;
    }

    protected boolean hasIpv4(ArrayList<InetAddress> list) {
        for (InetAddress a : list) {
            if (a instanceof Inet4Address) {
                return true;
            }
        }
        return false;
    }

    protected boolean hasIpv6(ArrayList<InetAddress> list) {
        for (InetAddress a : list) {
            if ((a instanceof Inet6Address) && !((Inet6Address)a).isLinkLocalAddress()) {
                return true;
            }
        }
        return false;
    }

    public InetAddress getAddress(ArrayList<InetAddress> localAddresses) {
        mConfigurationParameters = mConfigurationManager.getConnectionConfiguration();
        buildDefaultRetryTimerMap();
        buildServersBlackList();

        mLastError = TunnelError.ERR_SUCCESS;
        if (mServerAddresses.isEmpty()) {
            if (mServerNames.isEmpty()) {
                TelephonyManager tm = mSimInformation.getTelephonyManager();

                FqdnBuilder builder = new FqdnBuilder(mConfigurationParameters, tm, mDnsResolver, mSimInformation.getRecords());
                mServerNames = builder.build(mApnType.getBit(), tm.isNetworkRoaming());
                if (mApnType == ApnType.APN_IMS && !mPcoServerAddresses.isEmpty()) {
                    for (InetAddress a : mPcoServerAddresses) {
                        mServerNames.addFirst(a.getHostName());
                    }
                }
            }
            do {
                mLastError = resolveServer();
            } while ((mLastError != TunnelError.ERR_SUCCESS) && !mServerNames.isEmpty());
            if (mServerAddresses.isEmpty()) {
                loge("No server addresses found");
                return null;
            }
        }
        InetAddress result = null;
        if (hasIpv4(localAddresses)) {
            result = getFirstRemoteIpv4();
        }
        if ((result == null) && hasIpv6(localAddresses)) {
            result = getFirstRemoteIpv6();
        }
        return result;
    }

    public void clearAddresses() {
        mServerAddresses.clear();
    }

    public void removeAddress(TunnelError error, InetAddress address) {
        log("Connect on IP " + address + " failed with " + error);

        mLastError = error;
        mServerAddresses.remove(address);
    }

    public InetAddress getNextAddress(ArrayList<InetAddress> localAddresses) {
        if (mServerAddresses.isEmpty()) {
            TunnelError error;
            do {
                error = resolveServer();
            } while ((error != TunnelError.ERR_SUCCESS) && !mServerNames.isEmpty());
            if (mServerAddresses.isEmpty()) {
                loge("No server addresses found");
                return null;
            }
        }
        InetAddress result = null;
        if (hasIpv4(localAddresses)) {
            result = getFirstRemoteIpv4();
        }
        if ((result == null) && hasIpv6(localAddresses)) {
            result = getFirstRemoteIpv6();
        }
        if (result != null) {
            mLastError = TunnelError.ERR_SUCCESS;
        }
        return result;
    }

    public TunnelError getLastError() {
        return mLastError;
    }

    protected int getRetryTimeout(boolean handover) {
        if (mDefaultRetryTimerMap.containsKey(mLastError)) {
            return getDefaultRetryTimeout(mLastError);
        }
        if (isPermanentFailure(mLastError)) {
            return DEFAULT_RETRY_VALUE;
        }
        return getNextRetryTimeout(handover);
    }

    protected int getDefaultRetryTimeout(TunnelError error) {
        Integer timeout = mDefaultRetryTimerMap.get(error);
        if ((timeout != null) && (timeout > 0)) {
            return timeout * 1000;
        }
        return (-1);
    }

    protected void buildDefaultRetryTimerMap() {
        if (TextUtils.isEmpty(mConfigurationParameters.mDefaultRetryTimeout)) {
            return;
        }
        String[] strings = mConfigurationParameters.mDefaultRetryTimeout.split(",");
        for (String record : strings) {
            try {
                String[] map = record.split(":");
                if (map.length > 1) {
                    TunnelError error = TunnelError.getEnum(Integer.parseInt(map[0].trim()));
                    if (error != TunnelError.ERR_UNKNOWN) {
                        mDefaultRetryTimerMap.put(error, Integer.parseInt(map[1].trim()));
                    }
                }
            } catch (Exception e) {
                // ignore
            }
        }
    }

    protected void buildServersBlackList() {
        mServersBlacklist.clear();
        if (TextUtils.isEmpty(mConfigurationParameters.mServersBlacklist)) {
            return;
        }
        String[] strings = mConfigurationParameters.mServersBlacklist.split(",");
        for (String server : strings) {
            try {
                InetAddress address = InetAddress.getByName(server.trim());
                mServersBlacklist.add(address);
            } catch (Exception e) {
                // ignore
            }
        }
    }

    private boolean isAddressBlacklisted(InetAddress address) {
        return !mServersBlacklist.isEmpty() && mServersBlacklist.contains(address);
    }

    protected TunnelError resolveServer() {
        if (mServerNames.isEmpty()) {
            loge("ePDG Server FQDNs are not available");
            return TunnelError.ERR_DNS_UNKNOWN;
        }
        TunnelError error = TunnelError.ERR_SUCCESS;
        try {
            String name = mServerNames.removeFirst();
            log("Try to resolve " + name);

            InetAddress[] addresses;
            LinkedList<InetAddress> list = new LinkedList<>();

            Network network = mConnectivityManager.getActiveNetwork();
            if (network != null) {
                addresses = network.getAllByName(name);
            } else {
                addresses = InetAddress.getAllByName(name);
            }
            for (InetAddress ia : addresses) {
                log("Resolved to " + ia);
                if (isAddressBlacklisted(ia)) {
                    log("Address " + ia + " is blacklisted");
                    list.add(ia);
                } else {
                    mServerAddresses.add(ia);
                }
            }
            if (!list.isEmpty() && mConfigurationParameters.mUseBlacklistedServers) {
                /* put blacklisted at end of list */
                mServerAddresses.addAll(list);
            }
        } catch (UnknownHostException e) {
            error = TunnelError.ERR_DNS_UNKNOWN;

            Throwable tc = e.getCause();
            if (tc != null) {
                String cause = tc.toString();
                if (cause != null) {
                    if (cause.contains("EAI_NODATA")) {
                        error = TunnelError.ERR_DNS_NO_DATA;
                    } else if (cause.contains("EAI_FAIL")) {
                        error = TunnelError.ERR_DNS_TIMEOUT;
                    }
                }
            }
            loge("Failed to resolve HOST. ERROR : " + error);
        } catch (Exception e) {
            error = TunnelError.ERR_DNS_UNKNOWN;
            loge("Failed to resolve HOST. ERROR : " + error);
        }
        return error;
    }

    protected boolean isPermanentFailure(TunnelError error) {
        return DataFailCause.isPermanentFailure(mContext, error.toDataFailCause(), mSimInformation.getSubId());
    }

    public void onAirplaneModeEnabled() {
        mPcoServerAddresses.clear();
        stopBackoffTimer();
    }

    public void onWiFiDisconnected() {
        clearAddresses();
    }

    protected void startBackoffTimer(int value) {
        if ((mBackoffTimerTask == null) && (value > 0)) {
            mBackoffTimerTask = new BackoffTimerTask();
            mBackoffTimerExecution = SystemClock.elapsedRealtime() + value;
            log("Start backoff timer for " + value + " ms");
            mBackoffTimer.schedule(mBackoffTimerTask, value);
        }
    }

    protected void stopBackoffTimer() {
        if (mBackoffTimerTask != null) {
            mBackoffTimerTask.cancel();
            mBackoffTimerTask = null;
            mBackoffTimerExecution = 0;
            log("Backoff timer stopped");

            notifyApnUnthrottled();
        }
    }

    public boolean isBackoffTimerActive() { return (mBackoffTimerTask != null); }

    public DataCallResponse createFailureResponse(boolean handover) {
        return createFailureResponse(mLastError.toDataFailCause(),
                DataCallResponse.RETRY_DURATION_UNDEFINED, ApnSetting.PROTOCOL_IP, handover);
    }

    public DataCallResponse createFailureResponse(int reason, int protocol, boolean handover) {
        return createFailureResponse(reason, DataCallResponse.RETRY_DURATION_UNDEFINED, protocol, handover);
    }

    public DataCallResponse createFailureResponse(int reason, int timeout, int protocol, boolean handover) {
        if (timeout == DataCallResponse.RETRY_DURATION_UNDEFINED) {
            if (mBackoffTimerTask != null) {
                timeout = (int)(mBackoffTimerExecution - SystemClock.elapsedRealtime());
            } else {
                timeout = getRetryTimeout(handover);
            }
        }
        if (timeout > 0) {
            startBackoffTimer(timeout);
        }
        return createFailureResponse(reason, timeout, mApnType.getCode(mSlotId), protocol, handover);
    }

    public static DataCallResponse createFailureResponse(int reason, int timeout, int cid,
                                                         int protocol, boolean handover) {
        DataCallResponse.Builder builder = new DataCallResponse.Builder()
                .setCause(reason)
                .setRetryDurationMillis(timeout)
                .setId(cid)
                .setLinkStatus(DataCallResponse.LINK_STATUS_INACTIVE)
                .setProtocolType(protocol)
                .setMtu(0)
                .setMtuV4(0)
                .setMtuV6(0)
                .setDefaultQos(null)
                .setSliceInfo(null);
        if (handover) {
            if (timeout == DataCallResponse.RETRY_DURATION_UNDEFINED) {
                builder.setHandoverFailureMode(DataCallResponse.HANDOVER_FAILURE_MODE_DO_FALLBACK);
            } else {
                builder.setHandoverFailureMode(DataCallResponse.HANDOVER_FAILURE_MODE_NO_FALLBACK_RETRY_HANDOVER);
            }
        }
        return builder.build();
    }

    protected void notifyApnUnthrottled() {
        if (mApnUnthrottledMessage != null) {
            Message message = Message.obtain(mApnUnthrottledMessage);
            AsyncResult.forMessage(message, mApnName, null);
            message.sendToTarget();
        }
    }

    protected void onCarrierConfigChanged() {
        CarrierConfigManager ccm = mContext.getSystemService(CarrierConfigManager.class);
        if (ccm != null) {
            PersistableBundle bundle = ccm.getConfigForSubId(mSimInformation.getSubId());
            if ((bundle != null) && CarrierConfigManager.isConfigForIdentifiedCarrier(bundle)) {
                String[] strings = bundle.getStringArray(CarrierConfigManager.KEY_CARRIER_DATA_CALL_RETRY_CONFIG_STRINGS);
                if (strings == null) {
                    return;
                }
                String configString = null;
                for (String s : strings) {
                    if (TextUtils.isEmpty(s)) {
                        continue;
                    }
                    String[] splitStr = s.split(":", 2);
                    if (splitStr.length == 2) {
                        String apnTypeStr = splitStr[0].trim();
                        // Check if this retry pattern is for the APN we want.
                        if (apnTypeStr.equals(ApnSetting.getApnTypeString(mApnType.getBit()))) {
                            // Extract the config string. Note that an empty string is valid
                            // here, meaning no retry for the specified APN.
                            configString = splitStr[1];
                            break;
                        } else if (apnTypeStr.equals(OTHERS_APN_TYPE)) {
                            // Extract the config string. Note that an empty string is valid
                            // here, meaning no retry for all other APNs.
                            configString = splitStr[1];
                        }
                    }
                }
                if (TextUtils.isEmpty(configString)) {
                    // We should never reach here. If we reach here, it must be a configuration
                    // error bug.
                    configString = DEFAULT_DATA_RETRY_CONFIG;
                }
                // Strip quotes if present.
                if ((configString.startsWith("\"") && configString.endsWith("\""))) {
                    configString = configString.substring(1, configString.length() - 1);
                }
                log("Carrier retry config for " + mApnName + " is " + configString);

                long defaultRandomization = 0;
                String[] strArray = configString.split(",");
                for (int i = 0; i < strArray.length; i++) {
                    Pair<Boolean, Integer> value;

                    String[] splitStr = strArray[i].split("=", 2);
                    splitStr[0] = splitStr[0].trim();
                    if (splitStr.length > 1) {
                        splitStr[1] = splitStr[1].trim();
                        if (TextUtils.equals(splitStr[0], "default_randomization")) {
                            value = parseNonNegativeInt(splitStr[0], splitStr[1]);
                            if (value.first) {
                                defaultRandomization = value.second;
                            }
                        } else if (TextUtils.equals(splitStr[0], "max_retries")) {
                            if (TextUtils.equals("infinite", splitStr[1])) {
                                mRetryForever = true;
                            } else {
                                value = parseNonNegativeInt(splitStr[0], splitStr[1]);
                                if (value.first) {
                                    mMaxRetryCount = value.second;
                                }
                            }
                        } else {
                            loge("Unrecognized configuration name value pair: " + strArray[i]);
                            return;
                        }
                    } else {
                        /**
                         * Assume a retry time with an optional randomization value
                         * following a ":"
                         */
                        RetryRecord rr = new RetryRecord();

                        splitStr = strArray[i].split(":", 2);
                        splitStr[0] = splitStr[0].trim();
                        value = parseNonNegativeInt("delayTime", splitStr[0]);
                        if (value.first) {
                            rr.mDelayTime = value.second;
                        } else {
                            continue;
                        }
                        // Check if optional randomization value present
                        if (splitStr.length > 1) {
                            splitStr[1] = splitStr[1].trim();
                            value = parseNonNegativeInt("randomizationTime", splitStr[1]);
                            if (value.first) {
                                rr.mRandomizationTime = value.second;
                            } else {
                                rr.mRandomizationTime = defaultRandomization;
                            }
                        } else {
                            rr.mRandomizationTime = defaultRandomization;
                        }
                        mRetryArray.add(rr);
                    }
                }
                if (mRetryArray.size() > mMaxRetryCount) {
                    mMaxRetryCount = mRetryArray.size();
                }
                log("Max retry count = " + mMaxRetryCount + ", retry forever = " + mRetryForever);
            }
        }
    }

    private Pair<Boolean, Integer> parseNonNegativeInt(String name, String stringValue) {
        Pair<Boolean, Integer> retVal;
        try {
            int value = Integer.parseInt(stringValue);
            retVal = new Pair<>(value >= 0, value);
        } catch (NumberFormatException e) {
            loge(name + " bad value: " + stringValue + e);
            retVal = new Pair<>(false, 0);
        }
        return retVal;
    }

    protected int getNextRetryTimeout(boolean handover) {
        if (mRetryArray.isEmpty() || !handover) {
            return DataCallResponse.RETRY_DURATION_UNDEFINED;
        }
        if (!mRetryForever && ((mRetryCount + 1) > mMaxRetryCount)) {
            log("Reached maximum retry count " + mMaxRetryCount + ".");
            return DataCallResponse.RETRY_DURATION_UNDEFINED;
        }
        int index;
        if (mRetryCount < mRetryArray.size()) {
            index = mRetryCount;
        } else {
            index = mRetryArray.size() - 1;
        }
        RetryRecord rr = mRetryArray.get(index);
        int randomTime = (int)rr.mRandomizationTime;
        if (randomTime != 0) {
            Random r = new Random();
            randomTime = r.nextInt(randomTime);
        }
        long retVal = rr.mDelayTime + randomTime;
        ++mRetryCount;
        return (int)retVal;
    }

    public void reset() {
        mRetryCount = 0;
    }

    protected void log(String s) {
        if (DBG) {
            Rlog.d(TAG, s);
        }
    }

    protected void loge(String s) {
        Rlog.e(TAG, s);
    }
}
