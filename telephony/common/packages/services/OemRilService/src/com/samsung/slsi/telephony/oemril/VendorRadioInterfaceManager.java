/*
 * Copyright Samsung Electronics Co., LTD.
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
package com.samsung.slsi.telephony.oemril;

import com.android.internal.telephony.CommandException;
import com.android.internal.telephony.TelephonyPermissions;
import com.android.internal.telephony.vendor.IVendorTelephonyListener;
import com.android.internal.telephony.vendor.IVendorTelephony;

import android.content.Context;
import android.os.AsyncResult;
import android.os.Binder;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.os.RemoteException;
import android.os.ServiceManager;
import android.os.WorkSource;
import android.telephony.Rlog;
import android.telephony.SubscriptionManager;

import android.cmccslice.IpDescriptors;
import android.cmccslice.TrafficDescriptor;

import android.net.LinkAddress;
import android.net.LinkProperties;

import android.telephony.AccessNetworkConstants.AccessNetworkType;
import android.telephony.ServiceState;
import android.telephony.SubscriptionManager;
import android.telephony.data.DataProfile;
import android.telephony.data.NetworkSliceInfo;
import android.telephony.data.Qos;
import android.telephony.data.QosBearerSession;
import android.util.SparseArray;
import vendor.samsung_slsi.telephony.hardware.radio.V1_0.IOemSamsungslsi;
import vendor.samsung_slsi.telephony.hardware.radio.V1_2.IOemSamsungslsiResponse;
import vendor.samsung_slsi.telephony.hardware.radio.V1_2.IOemSamsungslsiIndication;
import vendor.samsung_slsi.telephony.hardware.radio.V1_2.OptionalDnn;
import vendor.samsung_slsi.telephony.hardware.radio.V1_2.OptionalOsAppId;
import vendor.samsung_slsi.telephony.hardware.radio.V1_2.OptionalSliceInfo;
import vendor.samsung_slsi.telephony.hardware.radio.V1_2.OptionalTrafficDescriptor;
import vendor.samsung_slsi.telephony.hardware.radio.V1_2.OptionalIpDescriptors;
import vendor.samsung_slsi.telephony.hardware.radio.V1_2.OptionalDomainDescriptors;
import vendor.samsung_slsi.telephony.hardware.radio.V1_2.OptionalConnectionCapabilities;
import vendor.samsung_slsi.telephony.hardware.radio.V1_2.OptionalVerifyDescriptor;
//import android.telephony.data.TrafficDescriptor;


//
import android.telephony.data.ApnSetting;

public class VendorRadioInterfaceManager extends IVendorTelephony.Stub {

    private static final String LOG_TAG = "VendorRadioInterfaceManager";
    private static final boolean DBG = true;
    public static final String VENDOR_RADIO_SERVICE_NAME = "telephony.oem";
    private static VendorRadioInterfaceManager sInstance = null;

    private OemRilGlobals mGlobals;
    private MainThreadHandler mMainThreadHandler;

    // SIM tray state
    public static final int SIM_TRAY_STATE_UNKNOWN = 0;
    public static final int SIM_TRAY_STATE_INSERTED = 1;
    public static final int SIM_TRAY_STATE_REMOVED = 2;

    // SIM type
    public static final int SIM_TYPE_USIM = 0;
    public static final int SIM_TYPE_RUIM = 1;

    public static final int CMD_GET_SMS_STORAGE_ON_SIM = 0;
    public static final int EVENT_GET_SMS_STORAGE_ON_SIM_DONE = 1;
    public static final int CMD_SET_NR_MODE = 2;
    public static final int EVENT_SET_NR_MODE_DONE = 3;
    public static final int CMD_GET_NR_MODE = 4;
    public static final int EVENT_GET_NR_MODE_DONE = 5;
    public static final int CMD_SET_ACTIVATE_VSIM = 6;
    public static final int EVENT_SET_ACTIVATE_VSIM_DONE = 7;
    public static final int EVENT_ICCID_INFO_UPDATED = 8;
    public static final int EVENT_NR_DUAL_CONNECTIVITY_STATE_CHANGED = 9;
    public static final int EVENT_SIM_TRAY_STATE_CHANGED = 10;
    public static final int EVENT_NAS_TIMER_STATUS_CHANGED = 11;
    public static final int EVENT_UNTHROTTLE_APN = 12;
    public static final int CMD_SETUP_DATA_CALL = 13;
    public static final int EVENT_SETUP_DATA_CALL_DONE = 14;

    static VendorRadioInterfaceManager init(OemRilGlobals globals) {
        synchronized (VendorRadioInterfaceManager.class) {
            if (sInstance == null) {
                sInstance = new VendorRadioInterfaceManager(globals);
            } else {

            }
            return sInstance;
        }
    }

    public static VendorRadioInterfaceManager getInstance() {
        return sInstance;
    }

    private VendorRadioInterfaceManager(OemRilGlobals globals) {
        mGlobals = globals;
        mMainThreadHandler = new MainThreadHandler();
        publish();
    }

    private void publish() {
        logv("publish=" + this);
        ServiceManager.addService(VENDOR_RADIO_SERVICE_NAME, this);
    }

    private VendorRadioContext getRadioFromRequest(MainThreadRequest request) {
        if (request.radioContext != null) {
            return request.radioContext;
        } else {
            return mGlobals.getRadioContext(request.phoneId);
        }
    }

    private final class MainThreadHandler extends Handler {
        MainThreadHandler() {
            super(Looper.myLooper());
        }

        @Override
        public void handleMessage(Message msg) {
            MainThreadRequest request;
            Message onCompleted;
            AsyncResult ar;

            switch (msg.what) {
            case CMD_GET_SMS_STORAGE_ON_SIM: {
                request = (MainThreadRequest) msg.obj;
                int argument = (int) request.argument;
                onCompleted = obtainMessage(EVENT_GET_SMS_STORAGE_ON_SIM_DONE, request);
                getRadioFromRequest(request).getSmsStorageOnSim(argument, onCompleted);
                break;
            }
            case EVENT_GET_SMS_STORAGE_ON_SIM_DONE:
                ar = (AsyncResult) msg.obj;
                request = (MainThreadRequest) ar.userObj;
                if (ar.exception == null && ar.result != null) {
                    request.result = ar.result;     // Integer
                } else {
                    // request.result must be set to something non-null
                    // for the calling thread to unblock
                    request.result = new int[] { 0, 0 };
                    if (ar.result == null) {
                        loge("getSmsStorageOnSim: Empty response");
                    } else if (ar.exception instanceof CommandException) {
                        loge("getSmsStorageOnSim: CommandException: "
                                + ar.exception);
                    } else {
                        loge("getSmsStorageOnSim: Unknown exception");
                    }
                }
                notifyRequester(request);
                break;
            case CMD_SET_NR_MODE: {
                request = (MainThreadRequest) msg.obj;
                int argument = (int) request.argument;
                onCompleted = obtainMessage(EVENT_SET_NR_MODE_DONE, request);
                getRadioFromRequest(request).setNrMode(argument, onCompleted);
                break;
            }

            case EVENT_SET_NR_MODE_DONE:
                handleNullReturnEvent(msg, "setNrMode");
                break;

            case CMD_GET_NR_MODE:
                request = (MainThreadRequest) msg.obj;
                onCompleted = obtainMessage(EVENT_GET_NR_MODE_DONE, request);
                getRadioFromRequest(request).getNrMode(onCompleted);
                break;
            case EVENT_GET_NR_MODE_DONE: {
                ar = (AsyncResult) msg.obj;
                request = (MainThreadRequest) ar.userObj;
                if (ar.exception == null && ar.result != null) {
                    request.result = ar.result;     // Integer
                } else {
                    // request.result must be set to something non-null
                    // for the calling thread to unblock
                    request.result = new int[]{-1};
                    if (ar.result == null) {
                        loge("getNrMode: Empty response");
                    } else if (ar.exception instanceof CommandException) {
                        loge("getNrMode: CommandException: "
                                + ar.exception);
                    } else {
                        loge("getNrMode: Unknown exception");
                    }
                }
                notifyRequester(request);
                break;
            }

            case CMD_SET_ACTIVATE_VSIM: {
                request = (MainThreadRequest) msg.obj;
                FakeVsimData args = (FakeVsimData) request.argument;
                onCompleted = obtainMessage(EVENT_SET_ACTIVATE_VSIM_DONE, request);
                getRadioFromRequest(request).setActivateVsim(
                        args.slot, args.iccid, args.imsi,
                        args.hplmn, args.vsimState, args.vsimCardType,
                        onCompleted);
                break;
            }

            case EVENT_SET_ACTIVATE_VSIM_DONE:
                ar = (AsyncResult) msg.obj;
                handleNullReturnEvent(msg, "setActivateVsim");
                break;

            case CMD_SETUP_DATA_CALL: {

                request = (MainThreadRequest) msg.obj;
                FakeSetupDataCall args = (FakeSetupDataCall) request.argument;
                onCompleted = obtainMessage(EVENT_SETUP_DATA_CALL_DONE, request);

                getRadioFromRequest(request).setupDataCall_1_6(
                        args.accessNetworkType, args.dataProfile, args.isRoaming,
                        args.allowRoaming, args.reason, args.linkProperties,
                        args.pduSessionId, null, args.trafficDescriptor,
                        args.matchAllRuleAllowed, onCompleted);

                break;
            }
            case EVENT_SETUP_DATA_CALL_DONE:
                ar = (AsyncResult) msg.obj;
                handleNullReturnEvent(msg, "setupDataCall");
                break;

            default:
                break;
            }  // end switch ~
        }
    }

    private void notifyRequester(MainThreadRequest request) {
        synchronized (request) {
            request.notifyAll();
        }
    }

    private void handleNullReturnEvent(Message msg, String command) {
        AsyncResult ar = (AsyncResult) msg.obj;
        MainThreadRequest request = (MainThreadRequest) ar.userObj;
        if (ar.exception == null) {
            request.result = true;
        } else {
            request.result = false;
            if (ar.exception instanceof CommandException) {
                loge(command + ": CommandException: " + ar.exception);
            } else {
                loge(command + ": Unknown exception");
            }
        }
        notifyRequester(request);
    }

    private static final class MainThreadRequest {
        public Object argument;
        public Object result;
        public Integer phoneId = SubscriptionManager.INVALID_PHONE_INDEX;

        // In cases where subId is unavailable, the caller needs to specify the phone.
        public VendorRadioContext radioContext;

        public WorkSource workSource;

        MainThreadRequest(Object argument, VendorRadioContext radioContext, WorkSource workSource) {
            this.argument = argument;
            if (radioContext != null) {
                this.radioContext = radioContext;
            }
            this.workSource = workSource;
        }

        MainThreadRequest(Object argument, Integer phoneId, WorkSource workSource) {
            this.argument = argument;
            if (phoneId != null) {
                this.phoneId = phoneId;
            }
            this.workSource = workSource;
        }
    }

    private Object sendRequest(int command, Object argument, Integer phoneId) {
        return sendRequest(command, argument, phoneId, null, null);
    }

    private Object sendRequest(
            int command, Object argument, Integer phoneId, VendorRadioContext radioContext, WorkSource workSource) {
        if (Looper.myLooper() == mMainThreadHandler.getLooper()) {
            throw new RuntimeException("This method will deadlock if called from the main thread.");
        }

        MainThreadRequest request = null;
        if (phoneId != SubscriptionManager.INVALID_PHONE_INDEX && radioContext != null) {
            throw new IllegalArgumentException("phoneId and phone cannot both be specified!");
        } else if (radioContext != null) {
            request = new MainThreadRequest(argument, radioContext, workSource);
        } else {
            request = new MainThreadRequest(argument, phoneId, workSource);
        }

        Message msg = mMainThreadHandler.obtainMessage(command, request);
        msg.sendToTarget();

        // Wait for the request to complete
        synchronized (request) {
            while (request.result == null) {
                try {
                    request.wait();
                } catch (InterruptedException e) {
                    // Do nothing, go back and wait until the request is complete
                }
            }
        }
        return request.result;
    }

    @Override
    public int[] getSmsStorageOnSim(int phoneId, int simId,
            String callingPackage, String callingFeatureId) throws RemoteException {
        if (DBG) {
            log("getSmsStorageOnSim: " + " simId: " + simId);
        }
        int[] subIds = SubscriptionManager.getSubId(phoneId);
        int subId = (subIds != null && subIds.length > 0) ? subIds[0] : SubscriptionManager.INVALID_SUBSCRIPTION_ID;
        if (!TelephonyPermissions.checkCallingOrSelfReadPhoneState(
                    mGlobals, subId, callingPackage, callingFeatureId,
                    "getSmsStorageOnSim")) {
            return new int[] { 0, 0 };
        }

        final long identity = Binder.clearCallingIdentity();
        try {
            int[] ret = (int[]) sendRequest(CMD_GET_SMS_STORAGE_ON_SIM, simId, phoneId);
            if (ret != null && ret.length >= 2) {
                if (DBG) log("getSmsStorageOnSim: {" + ret[0] + ", " + ret[1] + "}");
            } else {
                if (DBG) log("getSmsStorageOnSim: invalid response");
                ret = new int[] { 0, 0 };
            }
            return ret;
        } finally {
            Binder.restoreCallingIdentity(identity);
        }
    }

    @Override
    public boolean setNrMode(int phoneId, int mode) throws RemoteException {
        int[] subIds = SubscriptionManager.getSubId(phoneId);
        int subId = (subIds != null && subIds.length > 0) ? subIds[0] : SubscriptionManager.INVALID_SUBSCRIPTION_ID;
        TelephonyPermissions.enforceCallingOrSelfModifyPermissionOrCarrierPrivilege(
                mGlobals, subId, "setNrMode");

        final long identity = Binder.clearCallingIdentity();
        try {
            Boolean success = (Boolean) sendRequest(CMD_SET_NR_MODE, mode, phoneId);

            if (DBG) log("setNrMode: " + (success ? "ok" : "fail"));
            return success;
        } finally {
            Binder.restoreCallingIdentity(identity);
        }
    }

    @Override
    public int getNrMode(int phoneId,
            String callingPackage, String callingFeatureId) throws RemoteException {
        int[] subIds = SubscriptionManager.getSubId(phoneId);
        int subId = (subIds != null && subIds.length > 0) ? subIds[0] : SubscriptionManager.INVALID_SUBSCRIPTION_ID;
        if (!TelephonyPermissions.checkCallingOrSelfReadPhoneState(
                    mGlobals, subId, callingPackage, callingFeatureId,
                    "getNrMode")) {
            return -1;
        }
        final long identity = Binder.clearCallingIdentity();
        try {
            int[] ret = (int[]) sendRequest(CMD_GET_NR_MODE, null, phoneId);
            int mode = (ret != null && ret.length > 0 ? ret[0] : -1);
            if (DBG) log("getNrMode: " + mode);
            return mode;
        } finally {
            Binder.restoreCallingIdentity(identity);
        }
    }

    private static final class FakeVsimData {
        public int slot;
        public String iccid;
        public String imsi;
        public String hplmn;
        public int vsimState;
        public int vsimCardType;

        FakeVsimData(int slot, String iccid, String imsi,
            String hplmn, int vsimState, int vsimCardType) {
            this.slot = slot;
            this.iccid = iccid;
            this.imsi = imsi;
            this.hplmn = hplmn;
            this.vsimState = vsimState;
            this.vsimCardType = vsimCardType;
        }

        @Override
        public String toString() {
            StringBuilder sb = new StringBuilder();
            sb.append("[");
            sb.append(slot).append("/");
            sb.append(iccid).append("/");
            sb.append(imsi).append("/");
            sb.append(hplmn).append("/");
            sb.append(vsimState).append("/");
            sb.append(vsimCardType);
            sb.append("]");
            return sb.toString();
        }
    }

    @Override
    public boolean setActivateVsim(int slot, String iccid, String imsi,
            String hplmn, int vsimState, int vsimCardType)
            throws RemoteException {
        int[] subIds = SubscriptionManager.getSubId(slot);
        int subId = (subIds != null && subIds.length > 0) ? subIds[0] : SubscriptionManager.INVALID_SUBSCRIPTION_ID;
        TelephonyPermissions.enforceCallingOrSelfModifyPermissionOrCarrierPrivilege(
                mGlobals, subId, "setActivateVsim");

        FakeVsimData arguement = new FakeVsimData(slot, iccid, imsi, hplmn, vsimState, vsimCardType);
        if (DBG) {
            log("setActivateVsim: " + arguement);
        }
        final long identity = Binder.clearCallingIdentity();
        try {
            Boolean success = (Boolean) sendRequest(CMD_SET_ACTIVATE_VSIM, arguement, 0);
            if (DBG) log("setActivateVsim: " + (success ? "ok" : "fail"));
            return success;
        } finally {
            Binder.restoreCallingIdentity(identity);
        }
    }

    private static final class FakeSetupDataCall {
        public int accessNetworkType;
        public DataProfile dataProfile;
        public boolean isRoaming;
        public boolean allowRoaming;
        public int reason;
        public LinkProperties linkProperties;
        public int pduSessionId;
        public NetworkSliceInfo sliceInfo;
        public android.cmccslice.TrafficDescriptor trafficDescriptor;
        public boolean matchAllRuleAllowed;
        ApnSetting testapn;

        String a = "TEST setOsAppId";
        String b = "TEST setDnn";
        String c = "TEST setDomainDescriptors";
        String d = "test ipV4";
        String e = "test maskV4";
        String f = "test ipV6";
        String g = "TEST setDomainDescriptors";

        FakeSetupDataCall(int accessNetworkType, DataProfile dataProfile, boolean isRoaming, boolean allowRoaming,
                            int reason, LinkProperties linkProperties, int pduSessionId, NetworkSliceInfo sliceInfo,
                            android.cmccslice.TrafficDescriptor trafficDescriptor, boolean matchAllRuleAllowed) {

            testapn = new ApnSetting.Builder()
                .setId(0)
                .setOperatorNumeric("00101")
                .setEntryName("setEntryName")
                .setApnName("TEST APN")
                .setProxyAddress("setProxyAddress")
                .setProxyPort(0)
                //.setMmsc("http://www.contoso.com/")
                .setMmsProxyAddress("setMmsProxyAddress")
                .setMmsProxyPort(0)
                .setUser("TEST USER")
                .setPassword("TEST PASSWORD")
                .setAuthType(0)
                .setApnTypeBitmask(0)
                .setProtocol(2)
                .setRoamingProtocol(1)
                .setCarrierEnabled(false)
                .setNetworkTypeBitmask(2)
                .setLingeringNetworkTypeBitmask(1)
                .setProfileId(0)
                .setModemCognitive(false)
                .setMaxConns(0)
                .setWaitTime(0)
                .setMaxConnsTime(0)
                .setMtuV4(3)
                .setMtuV6(4)
                .setMvnoType(0)
                .setMvnoMatchData("TEST")
                .setApnSetId(1)
                .setCarrierId(0)
                .setSkip464Xlat(0)
                .setAlwaysOn(false)
                .buildWithoutCheck();

            this.accessNetworkType = accessNetworkType;
            this.dataProfile = new DataProfile.Builder()
                        .setApnSetting(testapn)
                        .setTrafficDescriptor(new android.telephony.data.TrafficDescriptor(testapn.getApnName(), null))
                        .setPreferred(false)
                        .build();

            this.isRoaming = isRoaming;
            this.allowRoaming = allowRoaming;
            this.reason = reason;
            this.linkProperties = linkProperties;
            this.pduSessionId = pduSessionId;
            this.sliceInfo = sliceInfo;
            //this.trafficDescriptor = trafficDescriptor;
            this.matchAllRuleAllowed = matchAllRuleAllowed;

            try{
                this.trafficDescriptor.setOsAppId(a);
                this.trafficDescriptor.setDnn(b);
                this.trafficDescriptor.setDomainDescriptors(c);
                this.trafficDescriptor.setIpDescriptors(new IpDescriptors(d, e, f, 1, 2, 3, 4, 5));
                this.trafficDescriptor.setDomainDescriptors(g);
                this.trafficDescriptor.setMatchAll(false);
            }catch (NullPointerException e){
                log("FakeSetupDataCall NullPointerException");
            }

        }

        @Override
        public String toString() {
            StringBuilder sb = new StringBuilder();
            sb.append("[");
            sb.append(accessNetworkType).append("/");
            //sb.append(dataProfile).append("/");
            sb.append(isRoaming).append("/");
            sb.append(allowRoaming).append("/");
            sb.append(reason).append("/");
            //sb.append(linkProperties).append("/");
            sb.append(pduSessionId).append("/");
            //sb.append(sliceInfo).append("/");
            //sb.append(trafficDescriptor).append("/");
            sb.append(matchAllRuleAllowed);
            sb.append("]");
            return sb.toString();
        }
    }

    @Override
    public boolean setupDataCall_1_6()
            throws RemoteException {

        int[] subIds = SubscriptionManager.getSubId(0);
        int subId = (subIds != null && subIds.length > 0) ? subIds[0] : SubscriptionManager.INVALID_SUBSCRIPTION_ID;
        TelephonyPermissions.enforceCallingOrSelfModifyPermissionOrCarrierPrivilege(
                mGlobals, subId, "setupDataCall_1_6");

        FakeSetupDataCall arguement = new FakeSetupDataCall(0, null, false, false, 0, null, 0, null, null, false);
        if (DBG) {
            log("setupDataCall_1_6: " + arguement);
        }
        final long identity = Binder.clearCallingIdentity();
        try {
            Boolean success = (Boolean) sendRequest(CMD_SETUP_DATA_CALL, arguement, 0);
            if (DBG) log("setupDataCall_1_6: " + (success ? "ok" : "fail"));
            return success;
        } finally {
            Binder.restoreCallingIdentity(identity);
        }
    }

    @Override
    public void listenWithEventList(int phoneId, String callingPackage,
            IVendorTelephonyListener callback, int[] events, boolean notifyNow) {
        final VendorTelephonyRegistry registry = VendorTelephonyRegistry.getInstance();
        if (registry == null) {
            throw new IllegalStateException("registry is null.");
        }
        registry.listenWithEventList(phoneId, callingPackage, callback, events, notifyNow);
    }

    @Override
    public String toString() {
        return "VendorRadioInterfaceManager [mGlobals=" + mGlobals + ", mMainThreadHandler="
                + mMainThreadHandler + "]";
    }

    private static void log(String msg) {
        Rlog.d(LOG_TAG, "[VendorRadioIntfMgr] " + msg);
    }

    private static void logv(String msg) {
        Rlog.v(LOG_TAG, "[VendorRadioIntfMgr] " + msg);
    }

    private static void loge(String msg) {
        Rlog.e(LOG_TAG, "[VendorRadioIntfMgr] " + msg);
    }
}
