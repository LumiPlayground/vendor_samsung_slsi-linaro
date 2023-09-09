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

import android.hardware.radio.V1_0.RadioResponseInfo;
import vendor.samsung_slsi.telephony.hardware.radio.V1_0.OperatorInfoEx;
import vendor.samsung_slsi.telephony.hardware.radio.V1_0.UplmnInfo;
import vendor.samsung_slsi.telephony.hardware.radio.V1_1.CellInfo;
import vendor.samsung_slsi.telephony.hardware.radio.V1_1.DataRegStateResult;
import vendor.samsung_slsi.telephony.hardware.radio.V1_1.SignalStrength;
import vendor.samsung_slsi.telephony.hardware.radio.V1_2.IOemSamsungslsiResponse;
import vendor.samsung_slsi.telephony.hardware.radio.V1_2.SlicingConfig;

public class VendorRadioResponseBase extends IOemSamsungslsiResponse.Stub {

    @Override
    public void getCellInfoListResponse(RadioResponseInfo info, ArrayList<CellInfo> cellInfo) {
    }

    @Override
    public void getDataRegistrationStateResponse(RadioResponseInfo info,
            DataRegStateResult dataRegResponse) {
    }

    @Override
    public void getEndcModeResponse(RadioResponseInfo info, boolean enable) {
    }

    @Override
    public void getNrModeResponse(RadioResponseInfo info, int mode) {
    }

    @Override
    public void getSignalStrengthResponse(RadioResponseInfo info, SignalStrength signalStrength) {
    }

    @Override
    public void getSmsStorageOnSimResponse(RadioResponseInfo info, int[] ret) {
    }

    @Override
    public void setActivateVsimResponse(RadioResponseInfo info, int status) {
    }

    @Override
    public void setEndcModeResponse(RadioResponseInfo info) {

    }

    @Override
    public void setNrModeResponse(RadioResponseInfo info) {
    }

    @Override
    public void deactivateDataCallWithReasonResponse(RadioResponseInfo info) {
    }

    @Override
    public void dialWithCallTypeResponse(RadioResponseInfo info) {
    }

    @Override
    public void emulateIndResponse(RadioResponseInfo info) {
    }

    @Override
    public void generalResponse(RadioResponseInfo info) {
    }

    @Override
    public void getCdmaHybridModeResponse(RadioResponseInfo info, int mode) {
    }

    @Override
    public void getPreferredUplmnResponse(RadioResponseInfo info, int max,
            ArrayList<UplmnInfo> uplmnInfo) {
    }

    @Override
    public void getSimLockStatusResponse(RadioResponseInfo info, int policy, int status,
            int lockType, int maxRetryCount, int remainCount, ArrayList<String> lockCode) {
    }

    @Override
    public void iccGetAtrResponse(RadioResponseInfo info, String atr) {
    }

    @Override
    public void queryBplmnSearchResponse(RadioResponseInfo info,
            ArrayList<OperatorInfoEx> networkInfos) {
    }

    @Override
    public void queryCOLPResponse(RadioResponseInfo info, int status) {
    }

    @Override
    public void queryCOLRResponse(RadioResponseInfo info, int status) {
    }

    @Override
    public void sendRequestRawResponse(RadioResponseInfo info, ArrayList<Byte> data) {
    }

    @Override
    public void sendUSSDWithDcsResponse(RadioResponseInfo info) {
    }

    @Override
    public void sendVsimNotificationResponse(RadioResponseInfo info) {
    }

    @Override
    public void sendVsimOperationResponse(RadioResponseInfo info) {
    }

    @Override
    public void setBarringPasswordOverMmiResponse(RadioResponseInfo info) {
    }

    @Override
    public void setCdmaHybridModeResponse(RadioResponseInfo info) {
    }

    @Override
    public void setDsNetworkTypeResponse(RadioResponseInfo info) {
    }

    @Override
    public void setDualNetworkTypeAndAllowDataResponse(RadioResponseInfo info) {
    }

    @Override
    public void setEmcStatusResponse(RadioResponseInfo info) {
    }

    @Override
    public void setFemtoCellSearchResponse(RadioResponseInfo info, int result, String numeric) {
    }

    @Override
    public void setNetworkSelectionModeManualResponse(RadioResponseInfo info) {
    }

    @Override
    public void setPreferredUplmnResponse(RadioResponseInfo info) {
    }

    @Override
    public void setVoiceOperationResponse(RadioResponseInfo info) {
    }

    @Override
    public void setupDataCallResponse_1_6(RadioResponseInfo info, android.hardware.radio.V1_5.SetupDataCallResult dcResponse) {
    }

    @Override
    public void getSlicingConfigResponse(RadioResponseInfo info, SlicingConfig slicingConfig) {
    }
}
