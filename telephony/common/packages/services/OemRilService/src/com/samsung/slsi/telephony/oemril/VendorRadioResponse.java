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

import java.util.ArrayList;

import android.hardware.radio.V1_0.RadioError;
import android.hardware.radio.V1_0.RadioResponseInfo;
import android.hardware.radio.V1_5.SetupDataCallResult;
import android.os.AsyncResult;
import android.os.Message;
import android.telephony.data.DataCallResponse;

import vendor.samsung_slsi.telephony.hardware.radio.V1_0.*;
import vendor.samsung_slsi.telephony.hardware.radio.V1_2.IOemSamsungslsiResponse;
import vendor.samsung_slsi.telephony.hardware.radio.V1_1.DataRegStateResult;

//import android.telephony.data.NetworkSlicingConfig;
import com.android.internal.telephony.RIL;
import com.android.internal.telephony.RILUtils;


/**
 * class VendorRadioResponse
 *
 * - override only available responses here.
 *
 */
public class VendorRadioResponse extends VendorRadioResponseBase {
    VendorRIL mRil;

    public VendorRadioResponse(VendorRIL ril) {
        mRil = ril;
    }

    /**
     * Helper function to send response msg
     * @param msg Response message to be sent
     * @param ret Return object to be included in the response message
     */
    static void sendMessageResponse(Message msg, Object ret) {
        if (msg != null) {
            AsyncResult.forMessage(msg, ret, null);
            msg.sendToTarget();
        }
    }

    private void responseVoid(RadioResponseInfo responseInfo) {
        RILRequest rr = mRil.processResponse(responseInfo);

        if (rr != null) {
            Object ret = null;
            if (responseInfo.error == RadioError.NONE) {
                sendMessageResponse(rr.mResult, ret);
            }
            mRil.processResponseDone(rr, responseInfo, ret);
        }
    }

    private void responseInts(RadioResponseInfo responseInfo, int ...var) {
        final ArrayList<Integer> ints = new ArrayList<>();
        for (int i = 0; i < var.length; i++) {
            ints.add(var[i]);
        }
        responseIntArrayList(responseInfo, ints);
    }

    private void responseIntArrayList(RadioResponseInfo responseInfo, ArrayList<Integer> var) {
        RILRequest rr = mRil.processResponse(responseInfo);
        if (rr != null) {
            int[] ret = new int[var.size()];
            for (int i = 0; i < var.size(); i++) {
                ret[i] = var.get(i);
            }
            if (responseInfo.error == RadioError.NONE) {
                sendMessageResponse(rr.mResult, ret);
            }
            mRil.processResponseDone(rr, responseInfo, ret);
        }
    }

    @Override
    public void getNrModeResponse(RadioResponseInfo info, int mode) {
        responseInts(info, mode);
    }

    /**
     * @param responseInfo Response info struct containing response type, serial no. and error
     * @param resp capacity information of sim
     */
    @Override
    public void getSmsStorageOnSimResponse(RadioResponseInfo info, int[] ret) {
        responseInts(info, ret);
    }

    /**
     * @param responseInfo Response info struct containing response type, serial no. and errori
     * @param status 0: fail, 1: success
     */
    @Override
    public void setActivateVsimResponse(RadioResponseInfo info, int status) {
        responseInts(info, status);
    }

    @Override
    public void setNrModeResponse(RadioResponseInfo info) {
        responseVoid(info);
    }

    /**
     * @param info Response info struct containing response type, serial no. and error
     * @param slicingConfig Current slicing configuration
     */
    public void getSlicingConfigResponse(RadioResponseInfo info,
                                         vendor.samsung_slsi.telephony.hardware.radio.V1_2.SlicingConfig slicingConfig) {
        // // Need to check if this can be processed directly
        RILRequest rr = mRil.processResponse(info);

        if (rr != null) {
            NetworkSlicingConfig ret = new NetworkSlicingConfig(slicingConfig);
            if (info.error == RadioError.NONE) {
                sendMessageResponse(rr.mResult, ret);
            }
            mRil.processResponseDone(rr, info, ret);
        }
    }

    // Same to 1.5
    /**
     * @param responseInfo Response info struct containing response type, serial no. and error
     * @param setupDataCallResult Response to data call setup as defined by setupDataCallResult in
     *                            1.6/types.hal
     */
    public void setupDataCallResponse_1_6(
                                          RadioResponseInfo responseInfo,
                                          android.hardware.radio.V1_5.SetupDataCallResult setupDataCallResult) {
        // to call RadioResponse.setupDataCallResponse_1_5(...)
        responseSetupDataCall(responseInfo, setupDataCallResult);
    }

    private void responseSetupDataCall(RadioResponseInfo responseInfo,
                                       Object setupDataCallResult) {
        RILRequest rr = mRil.processResponse(responseInfo);

        if (rr != null) {
            DataCallResponse response = RILUtils.convertHalDataCallResult(setupDataCallResult);
            if (responseInfo.error == RadioError.NONE) {
                sendMessageResponse(rr.mResult, response);
            }
            mRil.processResponseDone(rr, responseInfo, response);
        }
    }

}
