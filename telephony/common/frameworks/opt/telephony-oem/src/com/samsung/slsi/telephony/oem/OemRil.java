/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */
package com.samsung.slsi.telephony.oem;

import static com.samsung.slsi.telephony.oem.OemRilConstants.*;

import android.content.Context;
import android.os.AsyncResult;
import android.os.Binder;
import android.os.Build;
import android.os.Handler;
import android.os.HwBinder;
import android.os.Message;
import android.os.Registrant;
import android.os.RegistrantList;
import android.os.RemoteException;
import android.util.Log;
import android.util.SparseArray;

import com.android.internal.telephony.HalVersion;
import com.samsung.slsi.telephony.oem.io.DataWriter;
import com.samsung.slsi.telephony.oem.util.StringUtil;

import java.io.IOException;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.atomic.AtomicLong;
import java.util.ArrayList;
import java.util.NoSuchElementException;

import vendor.samsung_slsi.telephony.hardware.radioExternal.V1_0.IOemSlsiRadioExternal;
import vendor.samsung_slsi.telephony.hardware.radioExternal.V1_0.IOemSlsiRadioExternalInd;
import vendor.samsung_slsi.telephony.hardware.radioExternal.V1_0.IOemSlsiRadioExternalRes;
import vendor.samsung_slsi.telephony.hardware.radioExternal.V1_0.RadioExternalResponseInfo;

public class OemRil implements OemCommandsInterface {
    private static final String TAG = "OemRil";

    private Context mContext;
    private int mPhoneId = RILC_SOCKET_ID_1;
    private int mClientId = -1;
    private String mName;
    private ArrayList<Integer> mIndicationFilter;
    private static final boolean VDBG = !Build.TYPE.equals("user");
    private static final int EVENT_RADIO_PROXY_DEAD = 100;
    private static final int IRADIO_GET_SERVICE_DELAY_MILLIS = 4 * 1000;

    private static final int MAX_RADIO_DATA_SIZE = (1 << 20);    // set 2's multiplier
    private static final int MAX_RADIO_DATA_MOD = (MAX_RADIO_DATA_SIZE - 1);

    //  OemRil.RdioState
    enum RadioState {
        RADIO_OFF,         /* Radio explicitly powered off (eg CFUN=0) */
        RADIO_UNAVAILABLE, /* Radio unavailable (eg, resetting or not booted) */
        RADIO_ON;          /* Radio is on */

        public boolean isOn() /* and available...*/ {
            return this == RADIO_ON;
        }

        public boolean isAvailable() {
            return this != RADIO_UNAVAILABLE;
        }
    }
    public static final int RADIO_OFF = 0;
    public static final int RADIO_UNAVAILABLE = 1;
    public static final int RADIO_ON = 2;

    protected RadioState mState = RadioState.RADIO_UNAVAILABLE;
    protected RegistrantList mRadioStateChangedRegistrants = new RegistrantList();
    protected RegistrantList mOnRegistrants = new RegistrantList();
    protected RegistrantList mAvailRegistrants = new RegistrantList();
    protected RegistrantList mOffOrNotAvailRegistrants = new RegistrantList();
    protected RegistrantList mNotAvailRegistrants = new RegistrantList();
    protected RegistrantList mLteRrcStateRegistrant = new RegistrantList();
    protected RegistrantList mModemStateChangedRegistrants = new RegistrantList();

    protected Object mStateMonitor = new Object();
    static AtomicInteger sNextSerial = new AtomicInteger(0);

    private static OemRil sInstance = null;
    private IOemSlsiRadioExternal mOemSlsiRadioExternalProxy = null;
    private IOemSlsiRadioExternalRes mOemSlsiRadioExternalRes = null;
    private IOemSlsiRadioExternalInd mOemSlsiRadioExternalInd = null;

    public static final String RADIO_EXTERNAL_SEVICE_NAME = "rilExternal";

    public static final HalVersion RADIO_EXTERNAL_HAL_VERSION_UNKNOWN = HalVersion.UNKNOWN;
    public static final HalVersion RADIO_EXTERNAL__HAL_VERSION_1_0 = new HalVersion(1, 0);
    public static final HalVersion RADIO_EXTERNAL__HAL_VERSION_1_1 = new HalVersion(1, 1);

    // IOemSlsiRadioExternal version
    private HalVersion mRadioVersion = RADIO_EXTERNAL_HAL_VERSION_UNKNOWN;

    SparseArray<RILRequest> mRequestList = new SparseArray<RILRequest>();
    private RegistrantList mOemRilConntectedRegistrants = new RegistrantList();
    private RegistrantList mOemRilDisconntectedRegistrants = new RegistrantList();

    /* registrant for Indication */
    protected Registrant mDisplayEngRegistrant;
    protected Registrant mAmRegistrant;
    protected Registrant mDeviceResetRegistrant;
    protected Registrant mRssiScanResultRegistrant;
    protected Registrant mATCommandListenerRegistrant;
    protected Registrant mSarRfConnectionRegistrant;
    protected Registrant mVsimOperationRegistrant;
    protected Registrant mModemInfoRegistrant;
    protected Registrant mSelflogStatusRegistrant;
    protected Registrant mCAInfoForPhoneRegistrant;
    protected Registrant mSrvccHoRegistrant;
    protected Registrant mSipMessageIndRegistrant;
    protected Registrant mAmbrReportRegistrant;
    protected Registrant mB2B1ConfigReportRegistrant;
    protected Registrant mFrequencyInfoRegistrant;
    protected Registrant mBaseStationRegistrant;
    protected Registrant mTotalOosRegistrant;
    protected Registrant mDroppedMtCallInfoRegistrant;

    public void registerAm(Handler h, int what, Object obj) {
        mAmRegistrant = new Registrant(h, what, obj);
    }

    public void registerDeviceReset(Handler h, int what, Object obj) {
        mDeviceResetRegistrant = new Registrant(h, what, obj);
    }

    public void registerDisplayEng(Handler h, int what, Object obj) {
        mDisplayEngRegistrant = new Registrant(h, what, obj);
    }

    public void registerRssiScanResult(Handler h, int what, Object obj) {
        mRssiScanResultRegistrant = new Registrant(h, what, obj);
    }

    public void registerForATCommandListener(Handler h, int what, Object obj) {
        mATCommandListenerRegistrant = new Registrant(h, what, obj);
    }

    public void registerSarRfConnection(Handler h, int what, Object obj) {
        mSarRfConnectionRegistrant = new Registrant(h, what, obj);
    }

    public void registerVsimOperation(Handler h, int what, Object obj) {
        mVsimOperationRegistrant = new Registrant(h, what, obj);
    }

    public void registerForModemStateChanged(Handler h, int what, Object obj) {
        Registrant r = new Registrant (h, what, obj);
        synchronized (mStateMonitor) {
            mModemStateChangedRegistrants.add(r);
        }
    }

    public void unregisterForModemStateChanged(Handler h) {
        synchronized (mStateMonitor) {
            mModemStateChangedRegistrants.remove(h);
        }
    }

    public void registerSelflogStatus(Handler h, int what) {
        mSelflogStatusRegistrant = new Registrant(h, what, null);
    }

    public void registerForCAInfoForPhone(Handler h, int what) {
        mCAInfoForPhoneRegistrant = new Registrant(h, what, null);
    }

    public void unregisterAm(Handler h) {
        if (mAmRegistrant != null && mAmRegistrant.getHandler() == h) {
            mAmRegistrant.clear();
            mAmRegistrant = null;
        }
    }

    public void unregisterDeviceReset(Handler h) {
        if (mDeviceResetRegistrant != null && mDeviceResetRegistrant.getHandler() == h) {
            mDeviceResetRegistrant.clear();
            mDeviceResetRegistrant = null;
        }
    }

    public void unregisterDisplayEng(Handler h) {
        if (mDisplayEngRegistrant != null && mDisplayEngRegistrant.getHandler() == h) {
            mDisplayEngRegistrant.clear();
            mDisplayEngRegistrant = null;
        }
    }

    public void unregisterRssiScanResult(Handler h) {
        if (mRssiScanResultRegistrant != null && mRssiScanResultRegistrant.getHandler() == h) {
            mRssiScanResultRegistrant.clear();
            mRssiScanResultRegistrant = null;
        }
    }

    public void unregisterForATCommandListener(Handler h) {
        if (mATCommandListenerRegistrant != null && mATCommandListenerRegistrant.getHandler() == h) {
            mATCommandListenerRegistrant.clear();
            mATCommandListenerRegistrant = null;
        }
    }

    public void unregisterSarRfConnection(Handler h) {
        if (mSarRfConnectionRegistrant != null && mSarRfConnectionRegistrant.getHandler() == h) {
            mSarRfConnectionRegistrant.clear();
            mSarRfConnectionRegistrant = null;
        }
    }

    public void unregisterVsimOperation(Handler h) {
        if (mVsimOperationRegistrant != null && mVsimOperationRegistrant.getHandler() == h) {
            mVsimOperationRegistrant.clear();
            mVsimOperationRegistrant = null;
        }
    }

    public void registerForModemInfo(Handler h, int what, Object obj) {
        mModemInfoRegistrant = new Registrant(h, what, obj);
    }

    public void registerForFrequencyInfo(Handler h, int what, Object obj) {
        mFrequencyInfoRegistrant = new Registrant(h, what, obj);
    }

    public void unregisterForFrequencyInfo(Handler h) {
        if (mFrequencyInfoRegistrant != null && mFrequencyInfoRegistrant.getHandler() == h) {
            mFrequencyInfoRegistrant.clear();
            mFrequencyInfoRegistrant = null;
        }
    }

    public void unregisterForModemInfo(Handler h) {
        if (mModemInfoRegistrant != null && mModemInfoRegistrant.getHandler() == h) {
            mModemInfoRegistrant.clear();
            mModemInfoRegistrant = null;
        }
    }

    public void unregisterSelflogStatus(Handler h) {
        if (mSelflogStatusRegistrant != null && mSelflogStatusRegistrant.getHandler() == h) {
            mSelflogStatusRegistrant.clear();
            mSelflogStatusRegistrant = null;
        }
    }

    public void unregisterForCAInfoForPhone(Handler h) {
        if (mCAInfoForPhoneRegistrant != null && mCAInfoForPhoneRegistrant.getHandler() == h) {
            mCAInfoForPhoneRegistrant.clear();
            mCAInfoForPhoneRegistrant = null;
        }
    }

    public void registerForSrvccHoInd(Handler h, int what, Object obj) {
        mSrvccHoRegistrant = new Registrant(h, what, obj);
    }

    public void unregisterForSrvccHoInd(Handler h) {
        if (mSrvccHoRegistrant != null && mSrvccHoRegistrant.getHandler() == h) {
            mSrvccHoRegistrant.clear();
            mSrvccHoRegistrant = null;
        }
    }

    public void registerForSipMessageInd(Handler h, int what, Object obj) {
        mSipMessageIndRegistrant = new Registrant(h, what, obj);
    }

    public void unregisterForSipMessageInd(Handler h) {
        if (mSipMessageIndRegistrant != null && mSipMessageIndRegistrant.getHandler() == h) {
            mSipMessageIndRegistrant.clear();
            mSipMessageIndRegistrant = null;
        }
    }

    public void registerForAmbrReport(Handler h, int what, Object obj) {
        mAmbrReportRegistrant = new Registrant(h, what, obj);
    }

    public void unregisterForAmbrReport(Handler h) {
        if (mAmbrReportRegistrant != null && mAmbrReportRegistrant.getHandler() == h) {
            mAmbrReportRegistrant.clear();
            mAmbrReportRegistrant = null;
        }
    }

    public void registerForB2B1ConfigInd(Handler h, int what, Object obj) {
        mB2B1ConfigReportRegistrant = new Registrant(h, what, obj);
    }

    public void unregisterForB2B1ConfigReport(Handler h) {
        if (mB2B1ConfigReportRegistrant != null && mB2B1ConfigReportRegistrant.getHandler() == h) {
            mB2B1ConfigReportRegistrant.clear();
            mB2B1ConfigReportRegistrant = null;
        }
    }

    public void registerForBaseStation(Handler h, int what, Object obj) {
        mBaseStationRegistrant = new Registrant(h, what, obj);
    }

    public void unregisterForBaseStation(Handler h) {
        if (mBaseStationRegistrant != null && mBaseStationRegistrant.getHandler() == h) {
            mBaseStationRegistrant.clear();
            mBaseStationRegistrant = null;
        }
    }

    public void registerForTotalOos(Handler h, int what, Object obj) {
        mTotalOosRegistrant = new Registrant(h, what, obj);
    }

    public void unregisterForTotalOos(Handler h) {
        if (mTotalOosRegistrant != null && mTotalOosRegistrant.getHandler() == h) {
            mTotalOosRegistrant.clear();
            mTotalOosRegistrant = null;
        }
    }

    public void registerForDroppedMtCallInfo(Handler h, int what, Object obj) {
        mDroppedMtCallInfoRegistrant = new Registrant(h, what, obj);
    }

    public void unregisterForDroppedMtCallInfo(Handler h) {
        if (mDroppedMtCallInfoRegistrant != null && mDroppedMtCallInfoRegistrant.getHandler() == h) {
            mDroppedMtCallInfoRegistrant.clear();
            mDroppedMtCallInfoRegistrant = null;
        }
    }

    public static OemRil init(Context context, int instanceId) {
        return OemRil.init(context, instanceId, "unknown");
    }

    public static OemRil init(Context context, int instanceId, String name) {
        return new OemRil(context, instanceId, name, null);
    }

    public static OemRil init(Context context, int instanceId, String name, ArrayList<Integer> indicationFilter) {
        return new OemRil(context, instanceId, name, indicationFilter);
    }

    public static OemRil getInstance() {
        return sInstance;
    }

    public Context getContext() {
        return mContext;
    }

    public int getPhoneId() {
        return mPhoneId;
    }

    RILRequest CreateRILRequest(int request, Message result) {
        return new RILRequest(request, result);
    }

    public OemRil(Context context, int phoneId) {
        this(context, phoneId, "OEM");
    }

    public OemRil(Context context, int phoneId, String name) {
        mContext = context;
        mPhoneId = phoneId;
        mName = String.format("%s{pid=%d uid=%d callingPacakge=%s}",
            name, Binder.getCallingPid(), Binder.getCallingUid(), context.getOpPackageName());
        mOemSlsiRadioExternalRes = new OemSlsiRadioExternalRes(this);
        mOemSlsiRadioExternalInd = new OemSlsiRadioExternalInd(this);
        mOemSlsiRadioExternalProxyDeathRecipient = new RadioProxyDeathRecipient();
        getOemSlsiRadioExternalProxy(null);
    }

    public OemRil(Context context, int phoneId, String name, ArrayList<Integer> indicationFilter) {
        mContext = context;
        mPhoneId = phoneId;
        mName = String.format("%s{pid=%d uid=%d callingPacakge=%s}",
            name, Binder.getCallingPid(), Binder.getCallingUid(), context.getOpPackageName());
        if (indicationFilter == null) {
            indicationFilter = new ArrayList<Integer>();
        }
        mIndicationFilter = new ArrayList<Integer>(indicationFilter);
        mOemSlsiRadioExternalRes = new OemSlsiRadioExternalRes(this);
        mOemSlsiRadioExternalInd = new OemSlsiRadioExternalInd(this);
        mOemSlsiRadioExternalProxyDeathRecipient = new RadioProxyDeathRecipient();
        getOemSlsiRadioExternalProxy(null);
    }

    final AtomicLong mOemSlsiRadioExternalProxyCookie = new AtomicLong(0);
    final RadioProxyDeathRecipient mOemSlsiRadioExternalProxyDeathRecipient;
    private Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
            case EVENT_RADIO_PROXY_DEAD:
                Log.e(TAG, "handleMessage: EVENT_RADIO_PROXY_DEAD cookie = " + msg.obj + " mRadioProxyCookie = " + mOemSlsiRadioExternalProxyCookie.get());
                if ((long) msg.obj == mOemSlsiRadioExternalProxyCookie.get()) {
                    resetProxyAndRequestList();
                    getOemSlsiRadioExternalProxy(null);
                }
                break;
            }
        }
    };

    private void resetProxyAndRequestList() {
        mOemSlsiRadioExternalProxy = null;
        // increment the cookie so that death notification can be ignored
        mOemSlsiRadioExternalProxyCookie.incrementAndGet();
    }

    private void handleRadioProxyExceptionForRR(RILRequest rr, String caller, Exception e) {
        Log.e(TAG, caller + ": " + e);
        resetProxyAndRequestList();

        // service most likely died, handle exception like death notification to try to get service
        // again
        mHandler.sendMessageDelayed(
                mHandler.obtainMessage(EVENT_RADIO_PROXY_DEAD, mOemSlsiRadioExternalProxyCookie.incrementAndGet()), IRADIO_GET_SERVICE_DELAY_MILLIS);
    }

    class RILRequest {
        int mChannel;
        int mSerial;
        int mRequest;
        Message mResult;
        DataWriter mOut;

        RILRequest(int request, Message result) {
            mChannel = mPhoneId;
            mSerial = sNextSerial.getAndIncrement();
            mOut = new DataWriter();
            mRequest = request;
            mResult = result;
        }

        void release() {
            mSerial = RILC_TRANSACTION_NONE;
            mRequest = 0;
            mResult = null;
            mOut.reset();
        }

        void onError(int error, Object ret) {
            Exception ex = OemRilConstants.fromRilErrno(error);

            Log.e(TAG, "Error: " + ex + " ret=" + StringUtil.retToString(mRequest, ret));
            if (mResult != null) {
                AsyncResult.forMessage(mResult, ret, ex);
                mResult.sendToTarget();
            }
            release();
        }

        @Override
        public String toString() {
            return String.format("RILRequest{channel:%d request:%d serial:%d data:%s}",
                    mChannel, mRequest, mSerial, StringUtil.bytesToHexString(mOut.toByteArray()));
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

    @Override
    public void detach() {
        Log.d(TAG, "detach()");
        try {
            mHandler.removeMessages(EVENT_RADIO_PROXY_DEAD);
            mOemSlsiRadioExternalProxy.clearResponseFunctions(mClientId);
            mOemSlsiRadioExternalProxy.unlinkToDeath(mOemSlsiRadioExternalProxyDeathRecipient);
            mOemSlsiRadioExternalProxy = null;
        } catch (RemoteException e) {
            e.printStackTrace();
        }
    }

    private void send(RILRequest rr) {
        IOemSlsiRadioExternal oemSlsiRadioExternalProxy = getOemSlsiRadioExternalProxy(rr.mResult);
        if (oemSlsiRadioExternalProxy != null) {

            synchronized (mRequestList) {
                mRequestList.append(rr.mSerial, rr);
            }
            try {
                oemSlsiRadioExternalProxy.sendRequestRaw(rr.mSerial, mClientId, rr.mRequest, mPhoneId, rr.mOut.size(), StringUtil.primitiveArrayToArrayList(rr.mOut.toByteArray()));
            } catch (RemoteException | RuntimeException e) {
                handleRadioProxyExceptionForRR(rr, "send", e);
            }
        }
    }

    private void sendMultiFrame(RILRequest rr) {
        Log.d(TAG, "need to implement: multi frame");
    }

    @Override
    public void invokeRequestRaw(int request, byte[] data, Message response) {
        RILRequest rr = CreateRILRequest(request, response);
        try {
            rr.mOut.writeBytes(data);
            Log.v(TAG, "[" + TAG + "_" + rr.mSerial + "]> " +
                   requestToString(rr.mRequest) + " " + StringUtil.retToString(rr.mRequest, data) +
                   " [SUB" + rr.mChannel + "]");

            // size() always returns 0 and RIL_MAX_COMMAND_BYTES is limited 4KB. DataWriter shall be modified.
            if ( rr.mOut.size() > MAX_RADIO_DATA_SIZE ) sendMultiFrame(rr);
            else send(rr);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void setIndicationFilter(int[] filters) {
        ArrayList<Integer> list = primitiveArrayToArrayList(filters);
        setIndicationFilter(list);
    }

    public void setIndicationFilter(ArrayList<Integer> filters) {
        mIndicationFilter = filters;
        IOemSlsiRadioExternal oemSlsiRadioExternalProxy = getOemSlsiRadioExternalProxy(null);
        if (oemSlsiRadioExternalProxy != null) {
            try {
                if (mRadioVersion.greaterOrEqual(RADIO_EXTERNAL__HAL_VERSION_1_1)) {
                    vendor.samsung_slsi.telephony.hardware.radioExternal.V1_1.IOemSlsiRadioExternal oemSlsiRadioExternalProxy11
                        = (vendor.samsung_slsi.telephony.hardware.radioExternal.V1_1.IOemSlsiRadioExternal) oemSlsiRadioExternalProxy;
                    oemSlsiRadioExternalProxy11.setIndicationFilter(mClientId, mIndicationFilter);
                }
            }
            catch (RemoteException | RuntimeException e) {
                handleRadioProxyExceptionForRR(null, "send", e);
            }
        }
    }

    public static ArrayList<Integer> primitiveArrayToArrayList(int[] ar) {
        ArrayList<Integer> arrayList = new ArrayList<>();
        for (int i : ar) {
            arrayList.add(i);
        }
        return arrayList;
    }

    private IOemSlsiRadioExternal getOemSlsiRadioExternalProxy(Message result) {

        if (mOemSlsiRadioExternalProxy != null) {
            return mOemSlsiRadioExternalProxy;
        }

        try {
            try {
                mOemSlsiRadioExternalProxy = vendor.samsung_slsi.telephony.hardware.radioExternal.V1_1.IOemSlsiRadioExternal.getService(
                                     RADIO_EXTERNAL_SEVICE_NAME, true);
                mRadioVersion = RADIO_EXTERNAL__HAL_VERSION_1_1;
            }
            catch (NoSuchElementException e) {
            }

            if (mOemSlsiRadioExternalProxy == null) {
                try {
                    mOemSlsiRadioExternalProxy = vendor.samsung_slsi.telephony.hardware.radioExternal.V1_0.IOemSlsiRadioExternal.getService(
                                         RADIO_EXTERNAL_SEVICE_NAME, true);
                    mRadioVersion = RADIO_EXTERNAL__HAL_VERSION_1_0;
                }
                catch (NoSuchElementException e) {
                }
            }

            if (mOemSlsiRadioExternalProxy != null) {
                // not calling linkToDeath() as ril service runs in the same process and death
                // notification for that should be sufficient
                mOemSlsiRadioExternalProxy.linkToDeath(mOemSlsiRadioExternalProxyDeathRecipient, mOemSlsiRadioExternalProxyCookie.incrementAndGet());
                if (mRadioVersion.greaterOrEqual(RADIO_EXTERNAL__HAL_VERSION_1_1)) {
                    vendor.samsung_slsi.telephony.hardware.radioExternal.V1_1.IOemSlsiRadioExternal oemSlsiRadioExternalProxy11
                        = (vendor.samsung_slsi.telephony.hardware.radioExternal.V1_1.IOemSlsiRadioExternal) mOemSlsiRadioExternalProxy;
                    mClientId = oemSlsiRadioExternalProxy11.setResponseFunctions_1_1(mOemSlsiRadioExternalRes, mOemSlsiRadioExternalInd, mName);
                    if (mIndicationFilter != null) {
                        oemSlsiRadioExternalProxy11.setIndicationFilter(mClientId, mIndicationFilter);
                    }
                }
                else {
                    mClientId = mOemSlsiRadioExternalProxy.setResponseFunctions(mOemSlsiRadioExternalRes, mOemSlsiRadioExternalInd);
                }
                mOemRilConntectedRegistrants.notifyRegistrants();
            } else {
                Log.e(TAG, "getOemSlsiRadioExternalProxy: mOemSlsiRadioExternalProxy == null");
            }
        } catch (RemoteException | RuntimeException e) {
            mOemSlsiRadioExternalProxy = null;
            Log.e(TAG, "OemSamsungslsiProxy getService/setResponseFunctions: " + e);
        }

        if (mOemSlsiRadioExternalProxy == null) {
            if (result != null) {
                AsyncResult.forMessage(result, null,
                        OemRilConstants.fromRilErrno(RILC_STATUS_FAIL));
                result.sendToTarget();
            }

            // if service is not up, treat it like death notification to try to get service again
            mHandler.sendMessageDelayed(mHandler.obtainMessage(EVENT_RADIO_PROXY_DEAD, mOemSlsiRadioExternalProxyCookie.incrementAndGet()), IRADIO_GET_SERVICE_DELAY_MILLIS);
        }
        return mOemSlsiRadioExternalProxy;
    }

    RILRequest processResponse(RadioExternalResponseInfo responseInfo) {
        int serial = responseInfo.serial;
        int error = responseInfo.error;

        RILRequest rr = null;
        rr = findAndRemoveRequestFromList(serial);

        if (rr == null) {
            Log.e(TAG, "processResponse: Unexpected response! serial: " + serial
                    + " error: " + error);
            return null;
        }
        return rr;
    }

    void processResponseDone(RILRequest rr,
            RadioExternalResponseInfo responseInfo, Object ret) {
        if (responseInfo.error == 0) {
            Log.v(TAG, "[" + TAG + "_" + rr.mSerial + "]< " + requestToString(rr.mRequest) + " "
                    + StringUtil.retToString(rr.mRequest, ret) + " [SUB" + rr.mChannel + "]" );
        } else {
            Log.v(TAG, "[" + TAG + "_" + rr.mSerial + "]< " + requestToString(rr.mRequest)
                    + " error " + responseInfo.error);
            rr.onError(responseInfo.error, ret);
        }
    }

    @Override
    public void registerForOemRilConnected(Handler h, int what) {
        Registrant r = new Registrant (h, what, null);
        synchronized (mStateMonitor) {
            mOemRilConntectedRegistrants.add(r);
        }
        if (mOemSlsiRadioExternalProxy != null) {
            r.notifyRegistrant();
        }
    }

    /* OEM RIL connection/disconnection */
    @Override
    public void registerForOemRilDisconnected(Handler h, int what) {
        Registrant r = new Registrant (h, what, null);
        synchronized (mStateMonitor) {
            mOemRilDisconntectedRegistrants.add(r);
        }
    }

    @Override
    public void unregisterForOemRilConnected(Handler h) {
        synchronized (mStateMonitor) {
            mOemRilConntectedRegistrants.remove(h);
        }
    }

    @Override
    public void unregisterForOemRilDisconnected(Handler h) {
        synchronized (mStateMonitor) {
            mOemRilDisconntectedRegistrants.remove(h);
        }
    }

    void notifyForOemRilConnected() {
        mOemRilConntectedRegistrants.notifyRegistrants();
    }

    void notifyForOemRilDisconnected() {
        mOemRilDisconntectedRegistrants.notifyRegistrants();
    }

    final class RadioProxyDeathRecipient implements HwBinder.DeathRecipient {
        @Override
        public void serviceDied(long cookie) {
            // Deal with service going away
            Log.e(TAG, "serviceDied");
            notifyForOemRilDisconnected();
            mHandler.sendMessageDelayed(mHandler.obtainMessage(EVENT_RADIO_PROXY_DEAD, cookie), IRADIO_GET_SERVICE_DELAY_MILLIS);
        }
    }

    public void registerForRadioStateChanged(Handler h, int what, Object obj) {
        Registrant r = new Registrant (h, what, obj);

        synchronized (mStateMonitor) {
            mRadioStateChangedRegistrants.add(r);
            r.notifyRegistrant();
        }
    }

    public void unregisterForRadioStateChanged(Handler h) {
        synchronized (mStateMonitor) {
            mRadioStateChangedRegistrants.remove(h);
        }
    }

    public void registerForOn(Handler h, int what, Object obj) {
        Registrant r = new Registrant (h, what, obj);

        synchronized (mStateMonitor) {
            mOnRegistrants.add(r);

            if (mState.isOn()) {
                r.notifyRegistrant(new AsyncResult(null, null, null));
            }
        }
    }

    public void unregisterForOn(Handler h) {
        synchronized (mStateMonitor) {
            mOnRegistrants.remove(h);
        }
    }


    public void registerForAvailable(Handler h, int what, Object obj) {
        Registrant r = new Registrant (h, what, obj);

        synchronized (mStateMonitor) {
            mAvailRegistrants.add(r);

            if (mState.isAvailable()) {
                r.notifyRegistrant(new AsyncResult(null, null, null));
            }
        }
    }

    public void unregisterForAvailable(Handler h) {
        synchronized(mStateMonitor) {
            mAvailRegistrants.remove(h);
        }
    }

    public void registerForNotAvailable(Handler h, int what, Object obj) {
        Registrant r = new Registrant (h, what, obj);

        synchronized (mStateMonitor) {
            mNotAvailRegistrants.add(r);

            if (!mState.isAvailable()) {
                r.notifyRegistrant(new AsyncResult(null, null, null));
            }
        }
    }

    public void unregisterForNotAvailable(Handler h) {
        synchronized (mStateMonitor) {
            mNotAvailRegistrants.remove(h);
        }
    }

    public void registerForOffOrNotAvailable(Handler h, int what, Object obj) {
        Registrant r = new Registrant (h, what, obj);

        synchronized (mStateMonitor) {
            mOffOrNotAvailRegistrants.add(r);

            if (mState == RadioState.RADIO_OFF || !mState.isAvailable()) {
                r.notifyRegistrant(new AsyncResult(null, null, null));
            }
        }
    }

    public void unregisterForOffOrNotAvailable(Handler h) {
        synchronized(mStateMonitor) {
            mOffOrNotAvailRegistrants.remove(h);
        }
    }

    void setRadioState(RadioState newState) {
        RadioState oldState;

        synchronized (mStateMonitor) {
            oldState = mState;
            mState = newState;

            if (oldState == mState) {
                // no state transition
                return;
            }

            mRadioStateChangedRegistrants.notifyRegistrants();

            if (mState.isAvailable() && !oldState.isAvailable()) {
                mAvailRegistrants.notifyRegistrants();
            }

            if (!mState.isAvailable() && oldState.isAvailable()) {
                mNotAvailRegistrants.notifyRegistrants();
            }

            if (mState.isOn() && !oldState.isOn()) {
                mOnRegistrants.notifyRegistrants();
            }

            if ((!mState.isOn() || !mState.isAvailable())
                && !((!oldState.isOn() || !oldState.isAvailable()))
            ) {
                mOffOrNotAvailRegistrants.notifyRegistrants();
            }
        }
    }

    public RadioState getRadioState() {
        return mState;
    }

    public void registerForLteRrcStateChanged(Handler h, int what, Object obj) {
        Registrant r = new Registrant (h, what, obj);

        synchronized (mStateMonitor) {
            mLteRrcStateRegistrant.add(r);
            r.notifyRegistrant();
        }
    }

    public void unregisterForLteRrcStateChanged(Handler h) {
        synchronized (mStateMonitor) {
            mLteRrcStateRegistrant.remove(h);
        }
    }


}
