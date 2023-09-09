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

import java.io.IOException;
import java.util.ArrayList;

import android.os.AsyncResult;
import android.os.RemoteException;
import android.util.Log;

import com.samsung.slsi.telephony.oem.io.DataReader;
import com.samsung.slsi.telephony.oem.util.ByteUtil;
import com.samsung.slsi.telephony.oem.util.StringUtil;

import vendor.samsung_slsi.telephony.hardware.radioExternal.V1_0.IOemSlsiRadioExternalInd;


/**
 * {@hide}
 */
public class OemSlsiRadioExternalInd extends IOemSlsiRadioExternalInd.Stub {

    public static final String TAG = "OemSlsiRadioExternalInd";

    ArrayList<Byte> indDataSeg;

    OemRil mOemRil;
    OemSlsiRadioExternalInd(OemRil ril) {
        mOemRil = ril;
    }

    @Override
    public void rilExternalRawIndication(int rilcMsgId, int slotId, ArrayList<Byte> data, int dataLength) throws RemoteException {

        Log.d(TAG, "Unsol response received for ");
        if (mOemRil.getPhoneId() != slotId) {
            return;
        }

        unslJLog(rilcMsgId, slotId);
        switch(rilcMsgId) {
        case RILC_UNSOL_DISPLAY_ENG_MODE:
            displayEngInd(slotId, data, dataLength);
            break;
        case RILC_UNSOL_AM:
            amInd(slotId, data, dataLength);
            break;
        case RILC_UNSOL_DEVICE_RESET:
            deviceReset(slotId, data, dataLength);
            break;
        case RILC_UNSOL_SCAN_RSSI_RESULT:
            scanRssiResult(slotId, data, dataLength);
            break;
        case RILC_UNSOL_FORWARDING_AT_COMMAND:
            forwardingATCommandInd(slotId, data, dataLength);
            break;
        case RILC_UNSOL_SAR_RF_CONNECTION:
            sarRfConnection(slotId, data, dataLength);
            break;
        case RILC_UNSOL_VSIM_OPERATION:
            notifyVsimOperationInd(slotId, data, dataLength);
            break;
        case RILC_UNSOL_MODEM_INFO:
            notifyModemInfoInd(slotId, data, dataLength);
            break;
        case RILC_UNSOL_SELFLOG_STATUS:
            notifySelflogStatus(slotId, data, dataLength);
            break;
        case RILC_UNSOL_CA_BANDWIDTH_FILTER:
            notifyCaInfoStatus(slotId, data, dataLength);
            break;
        case RILC_UNSOL_IMS_SRVCC_HO:
            notifySrvccHo(slotId, data, dataLength);
            break;
        case RILC_UNSOL_AIMS_SIP_MSG_INFO:
            notifySipMessageInd(slotId, data, dataLength);
            break;
        case RILC_UNSOL_AMBR_REPORT:
            notifyAmbrReport(slotId, data, dataLength);
            break;
        case RILC_UNSOL_B2_B1_CONFIG_INFO:
            notifyB2B1ConfigInd(slotId, data, dataLength);
            break;
        case RILC_UNSOL_FREQUENCY_INFO:
            notifyFrequencyInfo(slotId, data, dataLength);
            break;
        case RILC_UNSOL_RADIO_STATE_CHANGED:
            notifyRadioStateChanged(slotId, data, dataLength);
            break;
        case RILC_UNSOL_LTE_RRC_STATE_CHANGED:
            notifyLteRrcStateChanged(slotId, data, dataLength);
            break;
        case RILC_UNSOL_BASESTATION_INFO:
            notifybaseStationInd(slotId, data, dataLength);
            break;
        case RILC_UNSOL_TOTAL_OOS:
            notifyTotalOosInd(slotId, data, dataLength);
            break;
        case RILC_UNSOL_DROPPED_MT_CALL_INFO:
            notifyDroppedMtCallInfoInd(slotId, data, dataLength);
            break;
        default:
            break;
        }
    }

    private void notifyAmbrReport(int slotId, ArrayList<Byte> data, int dataLength) {
        Log.v(TAG, "notifyAmbrReport()");
        if (mOemRil.mAmbrReportRegistrant != null) {
            byte[] bytes = ByteUtil.arrayListToPrimitiveArray(data);
            mOemRil.mAmbrReportRegistrant.notifyRegistrant(new AsyncResult(null, bytes, null));
        }
    }

    private void notifySipMessageInd(int slotId, ArrayList<Byte> data, int dataLength) {
        Log.v(TAG, "notifySipMessageInd()");
        if (mOemRil.mSipMessageIndRegistrant != null) {
            byte[] bytes = ByteUtil.arrayListToPrimitiveArray(data);
            mOemRil.mSipMessageIndRegistrant.notifyRegistrant(new AsyncResult(null, bytes, null));
        }
    }

    private void notifySrvccHo(int slotId, ArrayList<Byte> data, int dataLength) {
        Log.v(TAG, "notifySrvccHo()");
        if (mOemRil.mSrvccHoRegistrant != null) {
            byte[] bytes = ByteUtil.arrayListToPrimitiveArray(data);
            mOemRil.mSrvccHoRegistrant.notifyRegistrant(new AsyncResult(null, bytes, null));
        }
    }

    private void notifyB2B1ConfigInd(int slotId, ArrayList<Byte> data, int dataLength) {
        Log.v(TAG, "notifyB2B1ConfigInd()");
        if (mOemRil.mB2B1ConfigReportRegistrant != null) {
            byte[] bytes = ByteUtil.arrayListToPrimitiveArray(data);
            mOemRil.mB2B1ConfigReportRegistrant.notifyRegistrant(new AsyncResult(null, bytes, null));
        }
    }

    @Override
    public void rilExternalRawIndicationSeg(int rilcMsgId, int slotId, ArrayList<Byte> data, int dataLength, int segIndex, int totalLen) throws RemoteException {

        Log.d(TAG, "Unsol response(Segmented) received for ");
        if (mOemRil.getPhoneId() != slotId) {
            return;
        }

        unslJLog(rilcMsgId, slotId);
        Log.d(TAG, "Need to implement: segIndex(" + segIndex + ") dataLen(" +  dataLength +") totalLen(" + totalLen + ")");

        // It asumed that data always comes in sequence. There is no corruption in order.
        // If not, it should be redesinged.
        // total lenth(16KB for RCS) shall be supported.
        // If several rilc Msg shall be handled at the same time, code change is needed.
        if (segIndex == 0) {
            indDataSeg = new ArrayList<Byte>(data);
        }
        else {
            indDataSeg.addAll(data);
        }

        if (indDataSeg.size() >= totalLen) {
            unslJLog(rilcMsgId, slotId);
            switch(rilcMsgId) {
            // processing rilc Msg

            }
            indDataSeg = null;
        }
    }

    private void amInd(int slotId, ArrayList<Byte> data, int dataLength) {
        Log.d(TAG, "amInd()");
        if (mOemRil.mAmRegistrant != null) {
            mOemRil.mAmRegistrant.notifyRegistrant(new AsyncResult(null, data, null));
        }
    }

    private void deviceReset(int slotId, ArrayList<Byte> data, int dataLength) {
        Log.d(TAG, "deviceReset()");
        if (mOemRil.mDeviceResetRegistrant != null) {
            if (data != null && data.size() > 0) {
                DataReader dr = new DataReader(StringUtil.arrayListToPrimitiveArray(data));
                try {
                    int resetType = dr.getInt();
                    mOemRil.mDeviceResetRegistrant.notifyResult(resetType);
                } catch (IOException e) {
                }
            }
        }
    }

    private void displayEngInd(int slotId, ArrayList<Byte> data, int dataLength) {
        Log.d(TAG, "displayEngInd()");
        if (mOemRil.mDisplayEngRegistrant != null) {
            mOemRil.mDisplayEngRegistrant.notifyRegistrant(new AsyncResult(null, data, null));
        }
    }

    private void scanRssiResult(int slotId, ArrayList<Byte> data, int dataLength) {
        Log.d(TAG, "scanRssiResult()");
        if (mOemRil.mRssiScanResultRegistrant != null) {
            mOemRil.mRssiScanResultRegistrant.notifyResult(data);
        }
    }

    private void forwardingATCommandInd(int slotId, ArrayList<Byte> data, int dataLength) {
        Log.d(TAG, "forwardingATCommandInd()");

        if (mOemRil.mATCommandListenerRegistrant != null) {
            byte[] bytesArray = StringUtil.arrayListToPrimitiveArray(data);
            if (bytesArray != null && bytesArray.length > 0) {
                String result = new String(bytesArray);
                mOemRil.mATCommandListenerRegistrant.notifyResult(result);
            }
        }
    }

    private void sarRfConnection(int slotId, ArrayList<Byte> data, int dataLength) {
        Log.d(TAG, "sarRfConnection()");
        if (mOemRil.mSarRfConnectionRegistrant != null) {
            if (data != null && data.size() > 0) {
                int res = 0xFF & data.get(0);
                mOemRil.mSarRfConnectionRegistrant.notifyResult(res);
            }
        }
    }

    private void notifyVsimOperationInd(int slotId, ArrayList<Byte> data, int dataLength) {
        Log.d(TAG, "notifyVsimOperationInd()");
        if (mOemRil.mVsimOperationRegistrant != null) {
            mOemRil.mVsimOperationRegistrant.notifyRegistrant(new AsyncResult(null, data, null));
        }
    }

    private void notifyModemStateChanged(int slotId, ArrayList<Byte> data, int dataLength) {
        Log.d(TAG, "notifyModemStateChanged()");
        if (mOemRil.mModemStateChangedRegistrants != null) {
            if (data != null && data.size() > 0) {
                DataReader dr = new DataReader(StringUtil.arrayListToPrimitiveArray(data));
                try {
                    // 0: online -> crash
                    // 1: crash -> onlie(recovery)
                    int state = dr.getInt();
                    mOemRil.mModemStateChangedRegistrants.notifyResult(state);
                } catch (IOException e) {
                }
            }
        }
    }

    private void notifyModemInfoInd(int slotId, ArrayList<Byte> data, int dataLength) {
        Log.d(TAG, "notifyModemInfoInd()");
        if (mOemRil.mModemInfoRegistrant != null) {
            byte[] bytes = ByteUtil.arrayListToPrimitiveArray(data);
            mOemRil.mModemInfoRegistrant.notifyRegistrant(new AsyncResult(null, bytes, null));
        }
    }

    private void notifySelflogStatus(int slotId, ArrayList<Byte> data, int dataLength) {
        Log.d(TAG, "notifySelflogStatus()");
        if (mOemRil.mSelflogStatusRegistrant != null) {
            if (data != null && data.size() > 0) {
                int status = 0xFF & data.get(0);
                mOemRil.mSelflogStatusRegistrant.notifyResult(status);
            }
        }
    }

    private void notifyCaInfoStatus(int slotId, ArrayList<Byte> data, int dataLength) {
        Log.d(TAG, "notifyCaInfoStatus()");
        if (mOemRil.mCAInfoForPhoneRegistrant != null) {
            byte[] bytes = ByteUtil.arrayListToPrimitiveArray(data);
            mOemRil.mCAInfoForPhoneRegistrant.notifyRegistrant(new AsyncResult(null, bytes, null));
        }
    }

    private void notifyFrequencyInfo(int slotId, ArrayList<Byte> data, int dataLength) {
        Log.d(TAG, "notifyFrequencyInfo()");
        if (mOemRil.mFrequencyInfoRegistrant != null) {
            if (data != null && data.size() > 0) {
                byte[] bytes = ByteUtil.arrayListToPrimitiveArray(data);
                mOemRil.mFrequencyInfoRegistrant.notifyRegistrant(new AsyncResult(null, bytes, null));
            }
        }
    }

    OemRil.RadioState gettRadioStateFromInt(int  stateInt) {
        OemRil.RadioState state;
        switch (stateInt) {
        case 0:
            state = OemRil.RadioState.RADIO_OFF;
            break;
        case 1:
            state = OemRil.RadioState.RADIO_UNAVAILABLE;
            break;
        case 10:
            state = OemRil.RadioState.RADIO_ON;
            break;
        default:
            throw new RuntimeException("Unrecognized RadioState: " + stateInt);
        }
        return state;
    }

    private void notifyRadioStateChanged(int slotId, ArrayList<Byte> data, int dataLength) {
        if (data != null && data.size() > 0) {
            try {
                DataReader dr = new DataReader(ByteUtil.arrayListToPrimitiveArray(data));
                OemRil.RadioState state = gettRadioStateFromInt(dr.getInt());
                Log.v(TAG, "notifyRadioStateChanged: newState=" + state);
                mOemRil.setRadioState(state);
            }
            catch (RuntimeException | IOException e) {
                Log.w(TAG, "", e);
            }
        }
    }

    private void notifyLteRrcStateChanged(int slotId, ArrayList<Byte> data, int dataLength) {
        Log.v(TAG, "notifyLteRrcStateChanged()");
        if (mOemRil.mLteRrcStateRegistrant != null) {
            if (data != null && data.size() > 0) {
                byte[] bytes = ByteUtil.arrayListToPrimitiveArray(data);
                mOemRil.mLteRrcStateRegistrant.notifyRegistrants(new AsyncResult(null, bytes, null));
            }
        }
    }

    private void notifybaseStationInd(int slotId, ArrayList<Byte> data, int dataLength) {
        Log.v(TAG, "notifybaseStationInd()");
        if (mOemRil.mBaseStationRegistrant != null) {
            byte[] bytes = ByteUtil.arrayListToPrimitiveArray(data);
            mOemRil.mBaseStationRegistrant.notifyRegistrant(new AsyncResult(null, bytes, null));
        }
    }

    private void notifyTotalOosInd(int slotId, ArrayList<Byte> data, int dataLength) {
        Log.v(TAG, "notifyTotalOosInd()");
        if (mOemRil.mTotalOosRegistrant != null) {
            mOemRil.mTotalOosRegistrant.notifyRegistrant();
        }
    }

    private void notifyDroppedMtCallInfoInd(int slotId, ArrayList<Byte> data, int dataLength) {
        Log.v(TAG, "notifyDroppedMtCallInfoInd()");
        if (mOemRil.mDroppedMtCallInfoRegistrant != null) {
            byte[] bytes = ByteUtil.arrayListToPrimitiveArray(data);
            mOemRil.mDroppedMtCallInfoRegistrant.notifyRegistrant(new AsyncResult(null, bytes, null));
        }
    }

    void unslJLog(int response, int slotId) {
        Log.v(TAG, "[UNSL]< " + requestToString(response) + " [SUB" + slotId + "]");
    }
}
