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

package com.android.internal.telephony.vendor.dataconnection;

import android.content.BroadcastReceiver;
import android.content.ContentResolver;
import android.content.Context;
import android.content.Intent;
import android.database.Cursor;
import android.net.Uri;
import android.os.Message;
import android.os.SystemProperties;
import android.provider.Telephony;
import android.telephony.AccessNetworkConstants;
import android.telephony.AccessNetworkConstants.TransportType;
import android.telephony.TelephonyManager;
import android.telephony.TelephonyManager.SimState;
import android.telephony.data.ApnSetting;
import android.text.TextUtils;

import com.android.internal.telephony.DctConstants;
import com.android.internal.telephony.Phone;
import com.android.internal.telephony.PhoneConstants;
import com.android.internal.telephony.TelephonyIntents;
import com.android.internal.telephony.dataconnection.ApnContext;
import com.android.internal.telephony.dataconnection.DataConnection;
import com.android.internal.telephony.dataconnection.DcTracker;
import com.android.telephony.Rlog;

import com.android.internal.telephony.vendor.VendorTelephonyCallback;
import com.android.internal.telephony.vendor.VendorTelephonyManager;

import java.util.List;
import java.util.concurrent.atomic.AtomicInteger;

/**
 * {@hide}
 */
public class ExynosDcTracker extends DcTracker {
    private static final boolean DBG = true;
    private static final boolean VDBG = true; // STOPSHIP if true
    private static final boolean VDBG_STALL = true; // STOPSHIP if true
    private static final boolean RADIO_TESTS = false;

    static final String REASON_DEFAULT_DATA_SUBSCRIPTION_CHANGED = "defaultDataSubIdChanged";
    public static final int EVENT_APN_ALLOW_LIST_CHANGE = DctConstants.BASE + 552;
    public static final int EVENT_NAS_TIMER_STATUS_CHANGED = DctConstants.BASE + 553;

    // member variables
    protected final Phone mPhone;

    private int mPrevSubId;

    static final Uri PREFERAPN_NO_UPDATE_URI_USING_SUBID =
                        Uri.parse("content://telephony/carriers/preferapn_no_update/subId/");

    /** Allows the generation of unique Id's for DataConnection objects */
    private AtomicInteger mUniqueIdGenerator = new AtomicInteger(0);

    private final BroadcastReceiver mIntentReceiver = new BroadcastReceiver () {
        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            if (action.equals(TelephonyIntents.ACTION_DEFAULT_DATA_SUBSCRIPTION_CHANGED)) {
                if (DBG) log("trySetupData when DefaultDataSub changes");
                    setupDataOnAllConnectableApns(REASON_DEFAULT_DATA_SUBSCRIPTION_CHANGED, RetryFailures.ALWAYS);
            } else {
                if (DBG) log("onReceive: Unknown action=" + action);
            }
        }
    };

    public ExynosDcTracker(Phone phone, @TransportType int transportType) {
        super(phone, transportType);
        mPhone = phone;
        if (DBG) log("ExDCT.constructor");

        VendorTelephonyManager.from(mPhone.getContext()).registerTelephonyCallback(mPhone.getContext().getMainExecutor(), mCallback);
    }

    @Override
    protected void finalize() {
        if(DBG && mPhone != null) log("finalize");
    }

    @Override
    public void handleMessage (Message msg) {
        if (DBG) log("ExhandleMessage msg=" + msg);
        super.handleMessage(msg);


        switch (msg.what) {
            case EVENT_NAS_TIMER_STATUS_CHANGED: {
                handleNasTimer((NasTimerResponse)msg.obj);
                break;
            }
            case DctConstants.EVENT_SIM_STATE_UPDATED:
                int simState = msg.arg1;
                onSimStateUpdatedEx(simState);
                break;
            default:
                Rlog.e("ExDcTracker", "Unhandled event=" + msg);
                break;

        }
    }

    // APR-4791
    @Override
    protected void setupDataOnAllConnectableApns(String reason, RetryFailures retryFailures) {
        if (VDBG) log("setupDataOnAllConnectableApns: " + reason);

        if (DBG && !VDBG) {
            StringBuilder sb = new StringBuilder(120);
            for (ApnContext apnContext : getPrioritySortedApnContexts()) {
                sb.append(apnContext.getApnType());
                sb.append(":[state=");
                sb.append(apnContext.getState());
                sb.append(",enabled=");
                sb.append(apnContext.isEnabled());
                sb.append("] ");
            }
            log("setupDataOnAllConnectableApns: " + reason + " " + sb);
        }

        for (ApnContext apnContext : getPrioritySortedApnContexts()) {
            // For DefaultDataSubscriptionChanged, Just care about Default APN
            if (reason.equals(REASON_DEFAULT_DATA_SUBSCRIPTION_CHANGED) &&
                !apnContext.getApnType().equals(PhoneConstants.APN_TYPE_DEFAULT) ) continue;
            setupDataOnConnectableApn(apnContext, reason, retryFailures);
        }
    }

    @Override
    protected DataConnection createDataConnection() {
        if (DBG) log("createDataConnectionEx E");

        int id = mUniqueIdGenerator.getAndIncrement();
        boolean doAllocatePduSessionId =
                getTransportType() == AccessNetworkConstants.TRANSPORT_TYPE_WLAN;
        DataConnection dataConnection = ExynosDataConnection.makeDataConnection(mPhone, id, this,
                getDataServiceManager(), getDcTesterFailBringUpAll(), getDcc(), doAllocatePduSessionId);
        getDataConnections().put(id, dataConnection);
        if (DBG) log("createDataConnectionEx() X id=" + id + " dc=" + dataConnection);
        return dataConnection;
    }

    public class NasTimerResponse {
         public int type = 0;
         public int status = 0;
         public int value = 0;
         public String apn = "";

         public static final int NAS_TIMER_TYPE_PLMN = 0;
         public static final int NAS_TIMER_STATUS_STARTED = 1;
         public static final int NAS_TIMER_STATUS_STOPPED = 2;
         public static final int NAS_TIMER_STATUS_EXPIRED = 3;
    }

    private void handleNasTimer(NasTimerResponse DataNasTimer) {
        switch(DataNasTimer.type) {
          case NasTimerResponse.NAS_TIMER_TYPE_PLMN:
             if(DataNasTimer.status == NasTimerResponse.NAS_TIMER_STATUS_STARTED) {
             if(!TextUtils.isEmpty(DataNasTimer.apn)){
                    log("handleNasTimer: NAS_TIMER_TYPE_PLMN (handling PLMN sr... ) :"
                           + DataNasTimer.apn);
                    ApnSetting mDisconnectApn = null;
                    ApnContext ConnectedApn  = null;
                    List<Integer> ApnTypeList = null;
                    String selection = "apn = '" + DataNasTimer.apn + "'";
                    Cursor cursor = mPhone.getContext().getContentResolver().query(
                            Telephony.Carriers.CONTENT_URI, null, selection, null, null);
                    if(mApnContexts==null || mApnContexts.size() == 0) {
                          log("handleNasTimer: mApnContexts is NULL or size 0 case");
                          return;
                    }
                    if (cursor != null) {
                       cursor.moveToFirst();
                        while(cursor.getCount() > 0 ) {
                          mDisconnectApn = ApnSetting.makeApnSetting(cursor);
                          if(mDisconnectApn != null ) {
                          //Abnormal case : Once APN type didn't set properly at APN.xml,
                          //This logic will cover for that abnormal case.
                          String[] ApnTypes = {"default","mms","supl","fota","cbs"};
                          ApnTypeList = mDisconnectApn.getApnTypes();
                          if(ApnTypeList==null || ApnTypeList.isEmpty()
                          || TextUtils.equals(ApnSetting.getApnTypeString(ApnTypeList.get(0)),"*")){
                          log("handleNasTimer: Wildcard case, Abnormal APN_TypeInfo :"
                          + mDisconnectApn.toString());
                              for (int i = 0; i < ApnTypes.length; i++) {
                                     ApnContext tmp = mApnContexts.get(ApnTypes[i]);
                                          if(tmp!=null && tmp.isConnectedOrConnecting()){
                                               ConnectedApn = tmp;
                                               log("handleNasTimer:"
                                               +"(Type:" +ApnTypes[i]+ ")  ConnectedApn_ab is "
                                               + tmp.toString() );
                                         }
                                   }
                             }else{
                              //Normal case : Once APNType is set properly at APN.xml.
                                   ApnTypeList = mDisconnectApn.getApnTypes();
                                   for (int i = 0; i < ApnTypeList.size(); i++) {
                                String curApnType = ApnSetting.getApnTypeString(ApnTypeList.get(i));
                                    ApnContext tempContext = mApnContexts.get(curApnType);
                                    log("handleNasTimer: APN_TypeInfo :"
                                         + mDisconnectApn.toString());
                                     if(tempContext!=null
                                     && tempContext.isConnectedOrConnecting()){
                                               ConnectedApn = tempContext;
                                               log("handleNasTimer:"
                                                   +" (Type: " +curApnType+ ") ConnectedApn is "
                                                   + tempContext.toString());
                                         }
                                    }
                              }
                           }
                           if( ConnectedApn!=null){
                               break;
                           }else{
                                if(!cursor.moveToNext()){
                                    log("handleNasTimer: Next Cursor Null case");
                                    break;
                               }
                           }
                         }
                         cursor.close();
                    }
                    if(ConnectedApn != null){
                        log("handleNasTimer:Find connected one :"
                            +ConnectedApn.toString());
                        cleanUpConnection(ConnectedApn);
                        //cleanUpConnectionInternal(true,RELEASE_TYPE_DETACH,ConnectedApn);
                    } else {
                        log("handleNasTimer: X APN is not in the database");
                        return;
                    }
               }else{
                  log("handleNasTimer: Got NULL APN(abnormal case)");
                  return;
              }
            } else if ((DataNasTimer.status == NasTimerResponse.NAS_TIMER_STATUS_STOPPED) ||
              (DataNasTimer.status == NasTimerResponse.NAS_TIMER_STATUS_EXPIRED)) {
                log("handleNasTimer: Reconnect PDNs");
                setupDataOnAllConnectableApns(Phone.REASON_DATA_ATTACHED,RetryFailures.ALWAYS);
            }
            break;
            default:
                log("handleNasTimer: default");
                break;
        }
    }

    private void sendNasTimerChanged(int type, int status, int value, String apn) {
        Message msg = obtainMessage(EVENT_NAS_TIMER_STATUS_CHANGED);
        NasTimerResponse data = new NasTimerResponse();
        data.type = type;
        data.status = status;
        data.value = value;
        data.apn = apn;
        msg.obj = data;
        sendMessage(msg);
    }

    // This will clear PreferredApn parallely to original DcTracker
    private void onSimStateUpdatedEx(@SimState int simState) {
        boolean usePreferred = !mPhone.getContext().getResources().getBoolean(com.android
                .internal.R.bool.config_dontPreferApn);
        log("onSimStateUpdatedEx: usePreferred=" + usePreferred + ", simState=" + simState +
            ", mPrevSubId=" + String.valueOf(mPrevSubId) + ", subId=" + Long.toString(mPhone.getSubId()));
        if (!usePreferred &&
            (simState == TelephonyManager.SIM_STATE_ABSENT ||
             simState == TelephonyManager.SIM_STATE_NOT_READY)) {
            log("onSimStateUpdatedEx: SimState=" + simState + ", usePreferred is false, clear PreferredApn");
            Uri uri = Uri.withAppendedPath(PREFERAPN_NO_UPDATE_URI_USING_SUBID, String.valueOf(mPrevSubId));
            ContentResolver resolver = mPhone.getContext().getContentResolver();
            resolver.delete(uri, null, null);
        }
        if (simState == TelephonyManager.SIM_STATE_LOADED) {
            log("onSimStateUpdatedEx: SimState=" + simState + ", usePreferred=" + usePreferred);
            mPrevSubId = mPhone.getSubId();
        }
    }


    private VendorTelephonyCallbackImpl mCallback = new VendorTelephonyCallbackImpl();

    class VendorTelephonyCallbackImpl extends VendorTelephonyCallback
        implements VendorTelephonyCallback.NasTimerChangedListener {

         @Override
         public void onNasTimerChanged(int type, int status, int value, String apn) {
             log("onNasTimerChanged:" + type + ", " + status + ", " + value + ", " + apn);
             sendNasTimerChanged(type, status, value, apn);
        }
    }
}
