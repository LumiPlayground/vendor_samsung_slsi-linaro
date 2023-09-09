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


import static com.android.internal.telephony.RILConstants.*;
import static com.samsung.slsi.telephony.oemril.VendorRILConstants.*;

import java.util.Arrays;
import java.util.HashSet;
import java.util.NoSuchElementException;
import java.util.Set;
import java.util.concurrent.atomic.AtomicLong;

import com.android.internal.telephony.CommandException;
import com.android.internal.telephony.HalVersion;
import com.android.telephony.Rlog;

import android.annotation.Nullable;
import android.content.Context;
import android.hardware.radio.V1_0.DataProfileId;
import android.hardware.radio.V1_0.IRadio;
import android.hardware.radio.V1_0.RadioIndicationType;
import android.hardware.radio.V1_0.RadioResponseInfo;
import android.hardware.radio.V1_0.RadioResponseType;

import android.cmccslice.IpDescriptors;

import android.net.LinkAddress;
import android.net.LinkProperties;
import android.os.AsyncResult;
import android.os.Handler;
import android.os.HwBinder;
import android.os.Looper;
import android.os.Message;
import android.os.RemoteException;
import android.os.SystemClock;
import android.os.WorkSource;
import android.telephony.AccessNetworkConstants.AccessNetworkType;
import android.telephony.ServiceState;
import android.telephony.SubscriptionManager;
import android.telephony.data.DataProfile;
import android.telephony.data.NetworkSliceInfo;
import android.telephony.data.Qos;
import android.telephony.data.QosBearerSession;
import android.telephony.data.TrafficDescriptor;
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

import com.android.internal.telephony.RIL;

import java.net.InetAddress;
import java.util.ArrayList;

public class VendorRIL extends BaseVendorCommands implements VendorCommandsInterface {
    static final String RILJ_LOG_TAG = "VRILJ";
    static final boolean RILJ_LOGD = true;
    static final boolean RILJ_LOGV = false; // STOPSHIP if true

    public static final HalVersion RADIO_HAL_VERSION_UNKNOWN = HalVersion.UNKNOWN;

    public static final HalVersion RADIO_HAL_VERSION_1_0 = new HalVersion(1, 0);

    public static final HalVersion RADIO_HAL_VERSION_1_1 = new HalVersion(1, 1);

    public static final HalVersion RADIO_HAL_VERSION_1_2 = new HalVersion(1, 2);

    // Vendor radio version
    private HalVersion mRadioVersion = RADIO_HAL_VERSION_UNKNOWN;

    final Integer mPhoneId;
    Set<Integer> mDisabledRadioServices = new HashSet<Integer>();
    private WorkSource mDefaultWorkSource;

    VendorRadioResponse mRadioResponse;
    VendorRadioIndication mRadioIndication;
    volatile IOemSamsungslsi mRadioProxy = null;
    //volatile vendor.samsung_slsi.telephony.hardware.radio.V1_2.IOemSamsungslsi mRadioProxy = null;
    final AtomicLong mRadioProxyCookie = new AtomicLong(0);
    final RadioProxyDeathRecipient mRadioProxyDeathRecipient;
    final RilHandler mRilHandler;
    SparseArray<RILRequest> mRequestList = new SparseArray<RILRequest>();

    static final String[] HIDL_SERVICE_NAME = {"slot1", "slot2"};

    static final int EVENT_RADIO_PROXY_DEAD     = 6;


    public class RilHandler extends Handler {

        RilHandler() {
            super(Looper.getMainLooper());
        }

      //***** Handler implementation
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
            case EVENT_RADIO_PROXY_DEAD:
                riljLog("handleMessage: EVENT_RADIO_PROXY_DEAD cookie = " + msg.obj +
                        " mRadioProxyCookie = " + mRadioProxyCookie.get());
                if ((long) msg.obj == mRadioProxyCookie.get()) {
                    resetProxyAndRequestList();
                }
                break;
            }
        }
    }

    private void clearRequestList(int error, boolean loggable) {
        RILRequest rr;
        synchronized (mRequestList) {
            int count = mRequestList.size();
            if (RILJ_LOGD && loggable) {
                Rlog.d(RILJ_LOG_TAG, "clearRequestList: mRequestList=" + count);
            }

            for (int i = 0; i < count; i++) {
                rr = mRequestList.valueAt(i);
                if (RILJ_LOGD && loggable) {
                    Rlog.d(RILJ_LOG_TAG, i + ": [" + rr.mSerial + "] "
                            + requestToString(rr.mRequest));
                }
                rr.onError(error, null);
                rr.release();
            }
            mRequestList.clear();
        }
    }

    private RILRequest findAndRemoveRequestFromList(int serial) {
        RILRequest rr = null;
        synchronized (mRequestList) {
            rr = mRequestList.get(serial);
            if (rr != null) {
                mRequestList.remove(serial);
            }
        }
        return rr;
    }

    final class RadioProxyDeathRecipient implements HwBinder.DeathRecipient {
        @Override
        public void serviceDied(long cookie) {
            // Deal with service going away
            riljLog("serviceDied");
            mRilHandler.sendMessage(mRilHandler.obtainMessage(EVENT_RADIO_PROXY_DEAD, cookie));
        }
    }

    private synchronized void resetProxyAndRequestList() {
        mRadioProxy = null;

        // increment the cookie so that death notification can be ignored
        mRadioProxyCookie.incrementAndGet();

        RILRequest.resetSerial();
        // Clear request list on close
        clearRequestList(RADIO_NOT_AVAILABLE, false);

        getRadioProxy(null);
    }

    public synchronized IOemSamsungslsi getRadioProxy(Message result) {
        if (!SubscriptionManager.isValidPhoneId(mPhoneId)) return null;
//        if (!mIsCellularSupported) {
//            if (RILJ_LOGV) riljLog("getRadioProxy: Not calling getService(): wifi-only");
//            if (result != null) {
//                AsyncResult.forMessage(result, null,
//                        CommandException.fromRilErrno(RADIO_NOT_AVAILABLE));
//                result.sendToTarget();
//            }
//            return null;
//        }

        if (mRadioProxy != null) {
            return mRadioProxy;
        }

        try {
            if (mDisabledRadioServices.contains(mPhoneId)) {
                riljLoge("getRadioProxy: mRadioProxy for " + HIDL_SERVICE_NAME[mPhoneId]
                        + " is disabled");
            } else {
                if (mRadioProxy == null) {
                    try {
                        mRadioProxy = vendor.samsung_slsi.telephony.hardware.radio.V1_2.IOemSamsungslsi.getService(
                                HIDL_SERVICE_NAME[mPhoneId], true);
                        mRadioVersion = RADIO_HAL_VERSION_1_2;
                    } catch (NoSuchElementException e) {
                    }
                }

                if (mRadioProxy == null) {
                    try {
                        mRadioProxy = vendor.samsung_slsi.telephony.hardware.radio.V1_1.IOemSamsungslsi.getService(
                                HIDL_SERVICE_NAME[mPhoneId], true);
                        mRadioVersion = RADIO_HAL_VERSION_1_1;
                    } catch (NoSuchElementException e) {
                    }
                }

                if (mRadioProxy == null) {
                    try {
                        mRadioProxy = vendor.samsung_slsi.telephony.hardware.radio.V1_0.IOemSamsungslsi.getService(
                                HIDL_SERVICE_NAME[mPhoneId], true);
                        mRadioVersion = RADIO_HAL_VERSION_1_0;
                    } catch (NoSuchElementException e) {
                    }
                }

                if (mRadioProxy != null) {
                    mRadioProxy.linkToDeath(mRadioProxyDeathRecipient,
                            mRadioProxyCookie.incrementAndGet());
                    mRadioProxy.setResponseFunctions(mRadioResponse, mRadioIndication);
                } else {
                    mDisabledRadioServices.add(mPhoneId);
                    riljLoge("getRadioProxy: mRadioProxy for "
                            + HIDL_SERVICE_NAME[mPhoneId] + " is disabled");
                }
            }
        } catch (RemoteException e) {
            mRadioProxy = null;
            riljLoge("RadioProxy getService/setResponseFunctions: " + e);
        }

        if (mRadioProxy == null) {
            // getService() is a blocking call, so this should never happen
            riljLoge("getRadioProxy: mRadioProxy == null");
            if (result != null) {
                AsyncResult.forMessage(result, null,
                        CommandException.fromRilErrno(RADIO_NOT_AVAILABLE));
                result.sendToTarget();
            }
        }

        return mRadioProxy;
    }

    @Nullable
    private  vendor.samsung_slsi.telephony.hardware.radio.V1_2.IOemSamsungslsi
    getRadioV12(Message msg) {
        IOemSamsungslsi radioProxy = getRadioProxy(msg);
        if (mRadioVersion.greaterOrEqual(RADIO_HAL_VERSION_1_2)) {
            return (vendor.samsung_slsi.telephony.hardware.radio.V1_2.IOemSamsungslsi) radioProxy;
        } else {
            return (vendor.samsung_slsi.telephony.hardware.radio.V1_2.IOemSamsungslsi) null;
        }
    }

    @Nullable
    private  vendor.samsung_slsi.telephony.hardware.radio.V1_1.IOemSamsungslsi
    getRadioV11(Message msg) {
        IOemSamsungslsi radioProxy = getRadioProxy(msg);
        if (mRadioVersion.greaterOrEqual(RADIO_HAL_VERSION_1_1)) {
            return (vendor.samsung_slsi.telephony.hardware.radio.V1_1.IOemSamsungslsi) radioProxy;
        } else {
            return (vendor.samsung_slsi.telephony.hardware.radio.V1_1.IOemSamsungslsi) null;
        }
    }

    public VendorRIL(Context context, Integer instanceId) {
        super(context);
        mPhoneId = instanceId == null ? 0 : instanceId;
        mRadioResponse = new VendorRadioResponse(this);
        mRadioIndication = new VendorRadioIndication(this);
        mRilHandler = new RilHandler();
        mRadioProxyDeathRecipient = new RadioProxyDeathRecipient();
        mDefaultWorkSource = new WorkSource(context.getApplicationInfo().uid,
                context.getPackageName());

        getRadioProxy(null);

        if (RILJ_LOGD) {
            riljLog("Radio HAL version: " + mRadioVersion);
        }
    }

    private void addRequest(RILRequest rr) {
        synchronized (mRequestList) {
            rr.mStartTimeMs = SystemClock.elapsedRealtime();
            mRequestList.append(rr.mSerial, rr);
        }
    }

    private RILRequest obtainRequest(int request, Message result, WorkSource workSource) {
        RILRequest rr = RILRequest.obtain(request, result, workSource);
        addRequest(rr);
        return rr;
    }

    private void handleRadioProxyExceptionForRR(RILRequest rr, String caller, Exception e) {
        riljLoge(caller + ": " + e);
        resetProxyAndRequestList();
    }

    @Override
    public void setNrMode(int mode, Message result) {
        vendor.samsung_slsi.telephony.hardware.radio.V1_1.IOemSamsungslsi radioProxy =
                getRadioV11(result);
        if (radioProxy != null) {
            RILRequest rr = obtainRequest(RIL_REQUEST_SET_NR_MODE, result, mDefaultWorkSource);

            if (RILJ_LOGD) {
                riljLog(rr.serialString() + "> " + requestToString(rr.mRequest));
            }

            try {
                radioProxy.setNrMode(rr.mSerial, mode);
            } catch (RemoteException | RuntimeException e) {
                handleRadioProxyExceptionForRR(rr, "setNrMode", e);
            }
        } else {
            if (RILJ_LOGD) Rlog.d(RILJ_LOG_TAG, "setNrMode: REQUEST_NOT_SUPPORTED");
            AsyncResult.forMessage(result, null,
                    CommandException.fromRilErrno(REQUEST_NOT_SUPPORTED));
            result.sendToTarget();
        }
    }

    @Override
    public void getNrMode(Message result) {
        vendor.samsung_slsi.telephony.hardware.radio.V1_1.IOemSamsungslsi radioProxy =
                getRadioV11(result);
        if (radioProxy != null) {
            RILRequest rr = obtainRequest(RIL_REQUEST_GET_NR_MODE, result, mDefaultWorkSource);

            if (RILJ_LOGD) {
                riljLog(rr.serialString() + "> " + requestToString(rr.mRequest));
            }

            try {
                radioProxy.getNrMode(rr.mSerial);
            } catch (RemoteException | RuntimeException e) {
                handleRadioProxyExceptionForRR(rr, "getNrMode", e);
            }
        } else {
            if (RILJ_LOGD) Rlog.d(RILJ_LOG_TAG, "getNrMode: REQUEST_NOT_SUPPORTED");
            AsyncResult.forMessage(result, null,
                    CommandException.fromRilErrno(REQUEST_NOT_SUPPORTED));
            result.sendToTarget();
        }
    }

    @Override
    public void setActivateVsim(int slot, String iccid, String imsi,
            String hplmn, int vsimState, int vsimCardType, Message result) {
        vendor.samsung_slsi.telephony.hardware.radio.V1_1.IOemSamsungslsi radioProxy =
                getRadioV11(result);
        if (radioProxy != null) {
            RILRequest rr = obtainRequest(RIL_REQUEST_SET_ACTIVATE_VSIM, result,
                    mDefaultWorkSource);
            if (RILJ_LOGD) {
                riljLog(rr.serialString() + "> " + requestToString(rr.mRequest));
            }

            try {
                radioProxy.setActivateVsim(rr.mSerial, slot, iccid, imsi,
                        hplmn, vsimState, vsimCardType);
            } catch (RemoteException | RuntimeException e) {
                handleRadioProxyExceptionForRR(rr, "setActivateVsim", e);
            }
        } else {
            if (RILJ_LOGD) Rlog.d(RILJ_LOG_TAG, "setActivateVsim: REQUEST_NOT_SUPPORTED");
            AsyncResult.forMessage(result, null,
                    CommandException.fromRilErrno(REQUEST_NOT_SUPPORTED));
            result.sendToTarget();
        }
    }

    @Override
    public void getSmsStorageOnSim(int simId, Message result) {
        vendor.samsung_slsi.telephony.hardware.radio.V1_1.IOemSamsungslsi radioProxy =
                getRadioV11(result);
        if (radioProxy != null) {
            RILRequest rr = obtainRequest(RIL_REQUEST_GET_SMS_STORAGE_ON_SIM, result,
                    mDefaultWorkSource);

            if (RILJ_LOGV) {
                riljLog(rr.serialString() + "> "
                        + requestToString(rr.mRequest) + " simId = " + simId);
            }

            try {
                radioProxy.getSmsStorageOnSim(rr.mSerial, simId);
            } catch (RemoteException | RuntimeException e) {
                handleRadioProxyExceptionForRR(rr, "getSmsStorageOnSim", e);
            }
        } else {
            if (RILJ_LOGD) Rlog.d(RILJ_LOG_TAG, "getSmsStorageOnSim: REQUEST_NOT_SUPPORTED");
            AsyncResult.forMessage(result, null,
                    CommandException.fromRilErrno(REQUEST_NOT_SUPPORTED));
            result.sendToTarget();
        }
    }

    @Override
    public void setupDataCall_1_6(int accessNetworkType, DataProfile dataProfile, boolean isRoaming,
                                  boolean allowRoaming, int reason, LinkProperties linkProperties, int pduSessionId,
                                  NetworkSliceInfo sliceInfo, android.cmccslice.TrafficDescriptor trafficDescriptor,
                                  boolean matchAllRuleAllowed, Message result) {

        IOemSamsungslsi radioProxy = getRadioProxy(result);
        //IRadio radioProxy = getRadioProxy(result);

        if (radioProxy != null) {

            RILRequest rr = obtainRequest(RIL_REQUEST_SETUP_DATA_CALL, result,
                                          mDefaultWorkSource);

            ArrayList<String> addresses = new ArrayList<>();
            ArrayList<String> dnses = new ArrayList<>();
            if (linkProperties != null) {
                for (InetAddress address : linkProperties.getAddresses()) {
                    addresses.add(address.getHostAddress());
                }
                for (InetAddress dns : linkProperties.getDnsServers()) {
                    dnses.add(dns.getHostAddress());
                }
            }

            try {
                vendor.samsung_slsi.telephony.hardware.radio.V1_2.IOemSamsungslsi oemSamsungslsiProxy12
                    = getRadioV12(result);

                if (oemSamsungslsiProxy12 == null) {
                    riljLoge("getDataCallList_1_6 ignored. OemSamsungslsiProxy 1.2 is null!");
                    if (result != null) {
                        AsyncResult.forMessage(result, null,
                                               CommandException.fromRilErrno(RADIO_NOT_AVAILABLE));
                        result.sendToTarget();
                    }
                    return;
                }

                if (RILJ_LOGD) {
                    riljLog(rr.serialString() + "> " + requestToString(rr.mRequest));
                }

                // Convert to HAL data profile
                android.hardware.radio.V1_5.DataProfileInfo dpi =
                    convertToHalDataProfile15(dataProfile);

                vendor.samsung_slsi.telephony.hardware.radio.V1_2.OptionalSliceInfo si = convertToHalSliceInfo(sliceInfo);

                ArrayList<android.hardware.radio.V1_5.LinkAddress> addresses15 =
                    convertToHalLinkProperties15(linkProperties);

                OptionalTrafficDescriptor td = convertToHalTrafficDescriptor(trafficDescriptor);

                if (RILJ_LOGD) {
                    riljLog(rr.serialString() + "> " + requestToString(rr.mRequest)
                            + ",accessNetworkType="
                            + AccessNetworkType.toString(accessNetworkType) + ",isRoaming="
                            + isRoaming + ",allowRoaming=" + allowRoaming + "," + dataProfile
                            + ",addresses=" + addresses15 + ",dnses=" + dnses
                            + ",pduSessionId=" + pduSessionId + ",sliceInfo=" + si
                            + ",trafficDescriptor=" + td + ",matchAllRuleAllowed="
                            + matchAllRuleAllowed);
                }


                //oemSamsungslsiProxy12.setupDataCall_1_6(rr.mSerial, accessNetworkType, dpi, allowRoaming,
                //                                        reason, addresses15, dnses, pduSessionId, si, td, matchAllRuleAllowed);


                // from where?

                OptionalVerifyDescriptor verifyDescriptor = new OptionalVerifyDescriptor();
                vendor.samsung_slsi.telephony.hardware.radio.V1_2.VerifyDescriptor vd =
                    new vendor.samsung_slsi.telephony.hardware.radio.V1_2.VerifyDescriptor();

                vd.appCert = "ABCD";
                vd.tdHash = "EFABCD0987654321";
                verifyDescriptor.value(vd);

                oemSamsungslsiProxy12.setupDataCall_1_6(rr.mSerial, accessNetworkType, dpi, allowRoaming,
                                                        reason, addresses15, dnses, pduSessionId, si, td, matchAllRuleAllowed, verifyDescriptor);

            } catch (RemoteException | RuntimeException e) {
                handleRadioProxyExceptionForRR(rr, "setupDataCall", e);
            }

        }
    }

    public static ArrayList<Byte> primitiveArrayToArrayList(byte[] arr) {
        ArrayList<Byte> arrayList = new ArrayList<>(arr.length);
        for (byte b : arr) {
            arrayList.add(b);
        }
        return arrayList;
    }

    private static OptionalSliceInfo convertToHalSliceInfo(@Nullable NetworkSliceInfo sliceInfo) {
        OptionalSliceInfo optionalSliceInfo = new OptionalSliceInfo();
        if (sliceInfo == null) {
            return optionalSliceInfo;
        }

        vendor.samsung_slsi.telephony.hardware.radio.V1_2.SliceInfo si =
            new vendor.samsung_slsi.telephony.hardware.radio.V1_2.SliceInfo();
        si.sst = (byte) sliceInfo.getSliceServiceType();
        si.mappedHplmnSst = (byte) sliceInfo.getMappedHplmnSliceServiceType();
        si.sliceDifferentiator = sliceInfo.getSliceDifferentiator();
        si.mappedHplmnSD = sliceInfo.getMappedHplmnSliceDifferentiator();
        optionalSliceInfo.value(si);
        return optionalSliceInfo;
    }

    private static OptionalTrafficDescriptor convertToHalTrafficDescriptor_aosp(
                                                                           @Nullable TrafficDescriptor trafficDescriptor) {
        OptionalTrafficDescriptor optionalTrafficDescriptor = new OptionalTrafficDescriptor();
        if (trafficDescriptor == null) {
            return optionalTrafficDescriptor;
        }

        vendor.samsung_slsi.telephony.hardware.radio.V1_2.TrafficDescriptor td =
            new vendor.samsung_slsi.telephony.hardware.radio.V1_2.TrafficDescriptor();

        OptionalDnn optionalDnn = new OptionalDnn();
        if (trafficDescriptor.getDataNetworkName() != null) {
            optionalDnn.value(trafficDescriptor.getDataNetworkName());
        }
        td.dnn = optionalDnn;

        OptionalOsAppId optionalOsAppId = new OptionalOsAppId();
        if (trafficDescriptor.getOsAppId() != null) {
            vendor.samsung_slsi.telephony.hardware.radio.V1_2.OsAppId osAppId =
                new vendor.samsung_slsi.telephony.hardware.radio.V1_2.OsAppId();
            osAppId.osAppId = primitiveArrayToArrayList(trafficDescriptor.getOsAppId());
            optionalOsAppId.value(osAppId);
        }
        td.osAppId = optionalOsAppId;

        optionalTrafficDescriptor.value(td);
        return optionalTrafficDescriptor;
    }

    private static OptionalTrafficDescriptor convertToHalTrafficDescriptor(
                                                                           @Nullable android.cmccslice.TrafficDescriptor trafficDescriptor) {
        OptionalTrafficDescriptor optionalTrafficDescriptor = new OptionalTrafficDescriptor();
        if (trafficDescriptor == null) {
            return optionalTrafficDescriptor;
        }

        vendor.samsung_slsi.telephony.hardware.radio.V1_2.TrafficDescriptor td =
            new vendor.samsung_slsi.telephony.hardware.radio.V1_2.TrafficDescriptor();

        OptionalDnn optionalDnn = new OptionalDnn();
        if (trafficDescriptor.getDnn() != null) {
            optionalDnn.value(trafficDescriptor.getDnn());
        }
        td.dnn = optionalDnn;

        OptionalOsAppId optionalOsAppId = new OptionalOsAppId();
        if (trafficDescriptor.getOsAppId() != null) {
            vendor.samsung_slsi.telephony.hardware.radio.V1_2.OsAppId osAppId =
                new vendor.samsung_slsi.telephony.hardware.radio.V1_2.OsAppId();
            //osAppId.osAppId = primitiveArrayToArrayList(trafficDescriptor.getOsAppId());
            optionalOsAppId.value(osAppId);
        }
        td.osAppId = optionalOsAppId;

        OptionalIpDescriptors optionalIpDescriptors = new OptionalIpDescriptors();
        if (trafficDescriptor.getIpDescriptors() != null) {
            vendor.samsung_slsi.telephony.hardware.radio.V1_2.IpDescriptors ipDescriptors =
                new vendor.samsung_slsi.telephony.hardware.radio.V1_2.IpDescriptors();

            IpDescriptors srcIpDesc = trafficDescriptor.getIpDescriptors();
            // Canonical form
            ipDescriptors.ipv4 = srcIpDesc.getIpv4();
            // Canonical form
            ipDescriptors.maskV4 = srcIpDesc.getMaskV4();
            // Canonical form
            ipDescriptors.ipv6 = srcIpDesc.getIpv6();
            ipDescriptors.prefixLength = srcIpDesc.getPrefixLength();
            ipDescriptors.port = srcIpDesc.getDestPort();
            ipDescriptors.portStartRange = srcIpDesc.getDestPortStartRange();
            ipDescriptors.portEndRange = srcIpDesc.getDestPortEndRange();
            ipDescriptors.protocolId = srcIpDesc.getProtocol();

            optionalIpDescriptors.value(ipDescriptors);
        }
        td.ipDescriptors = optionalIpDescriptors;

        OptionalDomainDescriptors optionalDomainDescriptors = new OptionalDomainDescriptors();
        if (trafficDescriptor.getDomainDescriptors() != null) {
            optionalDomainDescriptors.value(trafficDescriptor.getDomainDescriptors());
        }
        td.domainDescriptors = optionalDomainDescriptors;

        OptionalConnectionCapabilities optionalConnectionCapabilities = new OptionalConnectionCapabilities();
        if (trafficDescriptor.getConnectionCapabilities() != null) {
            optionalConnectionCapabilities.value(trafficDescriptor.getConnectionCapabilities());
        }
        td.connectionCapabilities = optionalConnectionCapabilities;

        optionalTrafficDescriptor.value(td);

        return optionalTrafficDescriptor;
    }

    private static ArrayList<android.hardware.radio.V1_5.LinkAddress> convertToHalLinkProperties15(
            LinkProperties linkProperties) {
        ArrayList<android.hardware.radio.V1_5.LinkAddress> addresses15 = new ArrayList<>();
        if (linkProperties != null) {
            for (LinkAddress la : linkProperties.getAllLinkAddresses()) {
                android.hardware.radio.V1_5.LinkAddress linkAddress =
                    new android.hardware.radio.V1_5.LinkAddress();
                linkAddress.address = la.getAddress().getHostAddress();
                linkAddress.properties = la.getFlags();
                linkAddress.deprecationTime = la.getDeprecationTime();
                linkAddress.expirationTime = la.getExpirationTime();
                addresses15.add(linkAddress);
            }
        }
        return addresses15;
    }

    /**
     * Convert to DataProfileInfo defined in radio/1.5/types.hal
     * @param dp Data profile
     * @return A converted data profile
     */
    private static android.hardware.radio.V1_5.DataProfileInfo convertToHalDataProfile15(
            DataProfile dp) {
        android.hardware.radio.V1_5.DataProfileInfo dpi =
                new android.hardware.radio.V1_5.DataProfileInfo();

        dpi.apn = dp.getApn();
        dpi.protocol = dp.getProtocolType();
        dpi.roamingProtocol = dp.getRoamingProtocolType();
        dpi.authType = dp.getAuthType();
        dpi.user = dp.getUserName();
        dpi.password = dp.getPassword();
        dpi.type = dp.getType();
        dpi.maxConnsTime = dp.getMaxConnectionsTime();
        dpi.maxConns = dp.getMaxConnections();
        dpi.waitTime = dp.getWaitTime();
        dpi.enabled = dp.isEnabled();
        dpi.supportedApnTypesBitmap = dp.getSupportedApnTypesBitmask();
        // Shift by 1 bit due to the discrepancy between
        // android.hardware.radio.V1_0.RadioAccessFamily and the bitmask version of
        // ServiceState.RIL_RADIO_TECHNOLOGY_XXXX.
        dpi.bearerBitmap = ServiceState.convertNetworkTypeBitmaskToBearerBitmask(
            dp.getBearerBitmask()) << 1;
        dpi.mtuV4 = dp.getMtuV4();
        dpi.mtuV6 = dp.getMtuV6();
        dpi.persistent = dp.isPersistent();
        dpi.preferred = dp.isPreferred();

        // profile id is only meaningful when it's persistent on the modem.
        dpi.profileId = (dpi.persistent) ? dp.getProfileId() : DataProfileId.INVALID;

        return dpi;
    }

    public RILRequest processResponse(RadioResponseInfo responseInfo) {
        int serial = responseInfo.serial;
        int error = responseInfo.error;

        RILRequest rr = findAndRemoveRequestFromList(serial);
        if (rr == null) {
            Rlog.e(RILJ_LOG_TAG, "processResponse: Unexpected response! serial: " + serial
                    + " error: " + error);
            return null;
        }
        return rr;
    }

    public void processResponseDone(RILRequest rr, RadioResponseInfo responseInfo, Object ret) {
        if (responseInfo.error == 0) {
            if (RILJ_LOGD) {
                if (rr != null) {
                    riljLog(rr.serialString() + "< " + requestToString(rr.mRequest)
                    + " " + retToString(rr.mRequest, ret));
                }
            }
        } else {
            if (RILJ_LOGD) {
                if (rr != null) {
                    riljLog(rr.serialString() + "< " + requestToString(rr.mRequest)
                    + " error " + responseInfo.error);
                }
            }
            rr.onError(responseInfo.error, ret);
        }

        if (rr != null) {
            rr.release();
        }
    }

    void processIndication(int indicationType) {
    }

    void riljLog(String msg) {
        Rlog.d(RILJ_LOG_TAG, msg + (" [PHONE" + mPhoneId + "]"));
    }

    void riljLoge(String msg) {
        Rlog.e(RILJ_LOG_TAG, msg + (" [PHONE" + mPhoneId + "]"));
    }

    void riljLoge(String msg, Exception e) {
        Rlog.e(RILJ_LOG_TAG, msg + (" [PHONE" + mPhoneId + "]"), e);
    }

    void riljLogv(String msg) {
        Rlog.v(RILJ_LOG_TAG, msg + (" [PHONE" + mPhoneId + "]"));
    }

    void unsljLog(int response) {
        riljLog("[UNSL]< " + responseToString(response));
    }

    static String retToString(int req, Object ret) {
        // TODO
        return "";
    }

    static String requestToString(int request) {
        switch(request) {
        case RIL_REQUEST_QUERY_COLP:
            return "RIL_REQUEST_QUERY_COLP";
        case RIL_REQUEST_QUERY_COLR:
            return "RIL_REQUEST_QUERY_COLR";
        case RIL_REQUEST_SIM_GET_ATR:
            return "RIL_REQUEST_SIM_GET_ATR";
        case RIL_REQUEST_SEND_ENCODED_USSD:
            return "RIL_REQUEST_SEND_ENCODED_USSD";
        case RIL_REQUEST_SET_UPLMN:
            return "RIL_REQUEST_SET_UPLMN";
        case RIL_REQUEST_GET_UPLMN:
            return "RIL_REQUEST_GET_UPLMN";
        case RIL_REQUEST_SET_EMERGENCY_CALL_STATUS:
            return "RIL_REQUEST_SET_EMERGENCY_CALL_STATUS";
        case RIL_REQUEST_SET_FEMTO_CELL_SRCH:
            return "RIL_REQUEST_SET_FEMTO_CELL_SRCH";
        case RIL_REQUEST_SET_CDMA_HYBRID_MODE:
            return "RIL_REQUEST_SET_CDMA_HYBRID_MODE";
        case RIL_REQUEST_GET_CDMA_HYBRID_MODE:
            return "RIL_REQUEST_GET_CDMA_HYBRID_MODE";
        case RIL_REQUEST_SET_VOICE_OPERATION:
            return "RIL_REQUEST_SET_VOICE_OPERATION";
        case RIL_REQUEST_SET_DUAL_NETWORK_AND_ALLOW_DATA:
            return "RIL_REQUEST_SET_DUAL_NETWORK_AND_ALLOW_DATA";
        case RIL_REQUEST_SET_DS_NETWORK_TYPE:
            return "RIL_REQUEST_SET_DS_NETWORK_TYPE";
        case RIL_REQUEST_VSIM_NOTIFICATION:
            return "RIL_REQUEST_VSIM_NOTIFICATION";
        case RIL_REQUEST_VSIM_OPERATION:
            return "RIL_REQUEST_VSIM_OPERATION";
        case RIL_REQUEST_DEACTIVATE_DATA_CALL_WITH_REASON:
            return "RIL_REQUEST_DEACTIVATE_DATA_CALL_WITH_REASON";
        case RIL_REQUEST_ENABLE_MODEM:
            return "RIL_REQUEST_ENABLE_MODEM";
        case RIL_REQUEST_GET_MODEM_STATUS:
            return "RIL_REQUEST_GET_MODEM_STATUS";
        case RIL_REQUEST_EMULATE_IND:
            return "RIL_REQUEST_EMULATE_IND";
        case RIL_REQUEST_GET_SIM_LOCK_STATUS:
            return "RIL_REQUEST_GET_SIM_LOCK_STATUS";
        case RIL_REQUEST_SET_INTPS_SERVICE:
            return "RIL_REQUEST_SET_INTPS_SERVICE";
        case RIL_REQUEST_ENABLE_UICC_APPLICATIONS:
            return "RIL_REQUEST_ENABLE_UICC_APPLICATIONS";
        case RIL_REQUEST_GET_UICC_APPLICATIONS_ENABLEMENT:
            return "RIL_REQUEST_GET_UICC_APPLICATIONS_ENABLEMENT";
        case RIL_REQUEST_SET_SYSTEM_SELECTION_CHANNELS:
            return "RIL_REQUEST_SET_SYSTEM_SELECTION_CHANNELS";
        case RIL_REQUEST_CDMA_SEND_SMS_EXPECT_MORE:
            return "RIL_REQUEST_CDMA_SEND_SMS_EXPECT_MORE";
        case RIL_REQUEST_GET_BARRING_INFO:
            return "RIL_REQUEST_GET_BARRING_INFO";
        case RIL_REQUEST_ENTER_SIM_DEPERSONALIZATION:
            return "RIL_REQUEST_ENTER_SIM_DEPERSONALIZATION";
        case RIL_REQUEST_SET_ACTIVATE_VSIM:
            return "RIL_REQUEST_SET_ACTIVATE_VSIM";
        case RIL_REQUEST_SET_NR_MODE:
            return "RIL_REQUEST_SET_NR_MODE";
        case RIL_REQUEST_GET_NR_MODE:
            return "RIL_REQUEST_GET_NR_MODE";
        case RIL_REQUEST_SETUP_DATA_CALL:
            return "RIL_REQUEST_SETUP_DATA_CALL";

        default: return "<unknown request>";
        }
    }

    static String responseToString(int request) {
        switch (request) {
        case RIL_UNSOL_NR_PHYSICAL_CHANNEL_CONFIGS:
            return "RIL_UNSOL_NR_PHYSICAL_CHANNEL_CONFIGS";
        case RIL_UNSOL_ENDC_CAPABILITY:
            return "RIL_UNSOL_IND_ENDC_CAPABILITY";
        case RIL_UNSOL_SIM_TRAY_STATUS_CHANGED:
            return "RIL_UNSOL_IND_SIM_TRAY_STATUS_CHANGED";
        case RIL_UNSOL_IND_POST_URSP:
            return "RIL_UNSOL_IND_POST_URSP";
        case RIL_UNSOL_UNTHROTTLE_APN:  // reuse RILConstants.RIL_UNSOL_UNTHROTTLE_APN
            return "RIL_UNSOL_UNTHROTTLE_APN";
        default:
            return "<unknown response>";
        }
    }

    // [0:15]  OS ID = UUID format
    // [16] : OsAppIdLength
    // [17:272] : OsAppId(Max 255 without null termination)
    public static byte[] extractOsId(byte[] OsId_OsAppId) {
        byte[] extracted = Arrays.copyOfRange(OsId_OsAppId, 0, 16);
        return extracted;
    }
    public static String extractOsAppId(byte[] OsId_OsAppId) {
        int length = OsId_OsAppId[16];
        byte[] extracted = Arrays.copyOfRange(OsId_OsAppId, 17, 17 + length);
        return new String(extracted);
    }
    public static ArrayList<byte[]> splitOsAppId(byte[] OsId_OsAppId) {
        ArrayList<byte[]> list_OsAppId = new ArrayList<byte[]>();
        for(int i = 0; i < OsId_OsAppId.length; ) {
            int i_len = 17;
            if ( i + i_len < OsId_OsAppId.length ) {
                i_len += OsId_OsAppId[i+16];
            }
            list_OsAppId.add(Arrays.copyOfRange(OsId_OsAppId, i, i + i_len));
            i += i_len;
        }
        return list_OsAppId;
    }

    public static void loge(String s) {
        Rlog.e(RILJ_LOG_TAG, s);
    }
}
