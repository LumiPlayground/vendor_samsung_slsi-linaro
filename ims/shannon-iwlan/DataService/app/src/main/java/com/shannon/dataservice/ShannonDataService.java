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
package com.shannon.dataservice;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.net.LinkProperties;
import android.os.AsyncResult;
import android.os.Binder;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;
import android.os.Message;
import android.telephony.AccessNetworkConstants.AccessNetworkType;
import android.telephony.Rlog;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
import android.telephony.data.DataCallResponse;
import android.telephony.data.DataProfile;
import android.telephony.data.DataService;
import android.telephony.data.DataServiceCallback;
import android.telephony.data.NetworkSliceInfo;
import android.telephony.data.TrafficDescriptor;
import android.util.SparseArray;

import com.android.internal.annotations.VisibleForTesting;
import com.android.internal.telephony.TelephonyIntents;
import com.shannon.dataservice.util.PackageVersion;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class ShannonDataService extends DataService {
    private static final boolean DBG = true;
    private static final String TAG = "ShannonDS";
    private static ShannonDataService sMe = null;
    public static ShannonDataService getInstance() { return sMe; }

    public static String getServiceTAG(String s) { return TAG.concat(s); }

    class LocalBinder extends Binder {
        ShannonDataService getService() { return ShannonDataService.this; }
    }

    private Context mContext;
    private PackageVersion mVersion;
    private ConfigurationManager mConfiguration;
    private IkeDaemonInterface mDaemonInterface;
    private SparseArray<ShannonDataServiceProvider> mServiceProviders;
    private SimManager mSimManager;

    private static final int SETUP_DATA_CALL_COMPLETE           = 1;
    private static final int DEACTIVATE_DATA_CALL_COMPLETE      = 2;
    private static final int REQUEST_DATA_CALL_LIST_COMPLETE    = 3;
    private static final int DATA_CALL_LIST_CHANGED             = 4;
    private static final int APN_UNTHROTTLED                    = 5;

    protected class ShannonDataServiceProvider extends DataService.DataServiceProvider {
        private final String TAG1;
        private final Map<Message, DataServiceCallback> mCallbackMap = new HashMap<>();
        private final Looper mLooper;
        private final Handler mHandler;
        private final HandlerThread mHandlerThread;
        private final ConnectionManager mDataManager;
        private final ConfigurationManager mConfiguration;

        private ShannonDataServiceProvider(int slotId) {
            super(slotId);

            TAG1 = TAG.concat("-P-" + slotId);

            mHandlerThread = new HandlerThread(ShannonDataService.class.getSimpleName());
            mHandlerThread.start();
            mLooper = mHandlerThread.getLooper();
            mHandler = new Handler(mLooper) {
                @Override
                public void handleMessage(Message message) {
                    DataServiceCallback callback = mCallbackMap.remove(message);

                    AsyncResult ar = (AsyncResult)message.obj;
                    switch (message.what) {
                        case SETUP_DATA_CALL_COMPLETE:
                            DataCallResponse response = (DataCallResponse)ar.result;
                            callback.onSetupDataCallComplete((ar.exception != null || response == null)
                                            ? DataServiceCallback.RESULT_ERROR_ILLEGAL_STATE
                                            : DataServiceCallback.RESULT_SUCCESS,
                                    response);
                            break;
                        case DEACTIVATE_DATA_CALL_COMPLETE:
                            callback.onDeactivateDataCallComplete(ar.exception != null
                                    ? DataServiceCallback.RESULT_ERROR_ILLEGAL_STATE
                                    : DataServiceCallback.RESULT_SUCCESS);
                            break;
                        case REQUEST_DATA_CALL_LIST_COMPLETE:
                            callback.onRequestDataCallListComplete(
                                    ar.exception != null
                                            ? DataServiceCallback.RESULT_ERROR_ILLEGAL_STATE
                                            : DataServiceCallback.RESULT_SUCCESS,
                                    ar.exception != null
                                            ? null : (List<DataCallResponse>)ar.result
                            );
                            break;
                        case DATA_CALL_LIST_CHANGED:
                            notifyDataCallListChanged((List<DataCallResponse>)ar.result);
                            break;
                        case APN_UNTHROTTLED:
                            notifyApnUnthrottled((String)ar.result);
                            break;
                        default:
                            loge("Unexpected event: " + message.what);
                    }
                }
            };

            mConfiguration = new ConfigurationManager(mContext, getSlotIndex());
            mDataManager = new ConnectionManager(getSlotIndex(), mContext, mConfiguration);

            log("Register for data call list changed");
            Message message = Message.obtain(mHandler, DATA_CALL_LIST_CHANGED);
            mDataManager.registerForDataCallListChanged(message);

            log("Register for APN unthrottled");
            message = Message.obtain(mHandler, APN_UNTHROTTLED);
            mDataManager.registerForApnUnthrottled(message);
        }

        @Override
        public void setupDataCall(int accessNetworkType, DataProfile dataProfile, boolean isRoaming,
                                  boolean allowRoaming, int reason, LinkProperties linkProperties,
                                  int pduSessionId, NetworkSliceInfo sliceInfo,
                                  TrafficDescriptor trafficDescriptor, boolean matchAllRuleAllowed,
                                  DataServiceCallback callback) {
            log("setupDataCall accessNetworkType=" + accessNetworkType +
                    " pduID=" + pduSessionId + " " + dataProfile +
                    " isRoaming=" + isRoaming + " allowRoaming=" + allowRoaming +
                    " reason=" + reason + " LP:" + linkProperties +
                    " SI:" + sliceInfo);
            if (accessNetworkType != AccessNetworkType.IWLAN) {
                loge("Invalid access type requested " + accessNetworkType);
                if (callback != null) {
                    callback.onSetupDataCallComplete(DataServiceCallback.RESULT_ERROR_INVALID_ARG,
                            null);
                }
                return;
            }
            mVersion.print();

            Message message = null;
            // Only obtain the message when the caller wants a callback. If the caller doesn't care
            // the request completed or results, then no need to pass the message down.
            if (callback != null) {
                message = Message.obtain(mHandler, SETUP_DATA_CALL_COMPLETE);
                mCallbackMap.put(message, callback);
            }
            mDataManager.setupDataCall(dataProfile, reason, linkProperties, pduSessionId, sliceInfo, message);
        }

        @Override
        public void deactivateDataCall(int cid, int reason, DataServiceCallback callback) {
            log("deactivateDataCall " + cid + " on slot " + getSlotIndex());

            Message message = null;
            // Only obtain the message when the caller wants a callback. If the caller doesn't care
            // the request completed or results, then no need to pass the message down.
            if (callback != null) {
                message = Message.obtain(mHandler, DEACTIVATE_DATA_CALL_COMPLETE);
                mCallbackMap.put(message, callback);
            }
            mDataManager.deactivateDataCall(cid, reason, message);
        }

        @Override
        public void requestDataCallList(DataServiceCallback callback) {
            log("requestDataCallList " + getSlotIndex());

            Message message = null;
            // Only obtain the message when the caller wants a callback. If the caller doesn't care
            // the request completed or results, then no need to pass the message down.
            if (callback != null) {
                message = Message.obtain(mHandler, REQUEST_DATA_CALL_LIST_COMPLETE);
                mCallbackMap.put(message, callback);
            }
            mDataManager.getDataCallList(message);
        }

        @Override
        public void close() {
            mDataManager.destroy();
            mConfiguration.destroy();
            mHandlerThread.quitSafely();
            mServiceProviders.remove(getSlotIndex());
            log("WLAN data service removed for slot " + getSlotIndex());
        }

        public void setPcoData(String apn, int pcoId, byte[] pcoData) {
            if (apn == null || pcoData == null) {
                return;
            }
            mDataManager.setPcoData(apn, pcoId, pcoData);
        }

        private void log(String s) {
            if (DBG) {
               Rlog.d(TAG1, s);
            }
        }

        private void loge(String s) {
            Rlog.e(TAG1, s);
        }
    }

    public void sendPcoData(int slotIndex, String apn, int pcoId, byte[] pcoData) {
        ShannonDataServiceProvider provider = mServiceProviders.get(slotIndex);
        if (provider != null) {
            provider.setPcoData(apn, pcoId, pcoData);
        }
    }

    @Override
    public DataServiceProvider onCreateDataServiceProvider(int slotIndex) {
        log("WLAN data service created for slot " + slotIndex);
        if (!SubscriptionManager.isValidSlotIndex(slotIndex)) {
            loge("Tried to WLAN data service with invalid slotId " + slotIndex);
            return null;
        }
        ShannonDataServiceProvider provider = new ShannonDataServiceProvider(slotIndex);
        mServiceProviders.put(slotIndex, provider);
        return provider;
    }

    @Override
    public void onCreate() {
        mContext = getApplicationContext();
        mDaemonInterface = new IkeDaemonInterface(mContext);
        mServiceProviders = new SparseArray<>();
        mSimManager = new SimManager(mContext);
        mVersion = new PackageVersion(mContext);

        sMe = this;
        log("ShannonDataService created");
        mVersion.print();
    }

    @Override
    public void onDestroy() {
        log("ShannonDataService removed");
        mSimManager.destroy();
        mDaemonInterface.destroy();
        super.onDestroy();
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
    protected void onCreate(Context context, IkeDaemonInterface iface, SparseArray providers,
                            SimManager manger) {
        mContext = context;
        mDaemonInterface = iface;
        mServiceProviders = providers;
        mSimManager = manger;
        sMe = this;
        log("ShannonDataService created");
    }

    @VisibleForTesting
    protected SparseArray<ShannonDataServiceProvider> getServiceProviders() {
        return mServiceProviders;
    }
}
