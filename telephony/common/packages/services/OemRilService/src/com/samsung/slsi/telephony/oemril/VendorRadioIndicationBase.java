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

import vendor.samsung_slsi.telephony.hardware.radio.V1_0.NasTimerStatus;
import vendor.samsung_slsi.telephony.hardware.radio.V1_0.VsimOperationEvent;
import vendor.samsung_slsi.telephony.hardware.radio.V1_1.CellInfo;
import vendor.samsung_slsi.telephony.hardware.radio.V1_1.SignalStrength;
import vendor.samsung_slsi.telephony.hardware.radio.V1_2.DataProfileInfo;
import vendor.samsung_slsi.telephony.hardware.radio.V1_2.IOemSamsungslsiIndication;
import vendor.samsung_slsi.telephony.hardware.radio.V1_2.UrspRule;

public class VendorRadioIndicationBase extends IOemSamsungslsiIndication.Stub {
    @Override
    public void cellInfoList(int type, ArrayList<CellInfo> records) {
    }

    @Override
    public void currentNrPhysicalChannelConfigs(int indicationType, int rat, int status) {
    }

    @Override
    public void currentSignalStrength(int type, SignalStrength signalStrength) {
    }

    @Override
    public void endcCapabilityInd(int type, int capability, int cause) {
    }

    @Override
    public void callPresentInd(int type) {
    }

    @Override
    public void emergencyActInd(int type, int act, int status) {
    }

    @Override
    public void emergencySupportRatModeInd(int type, int supportRatMode) {
    }

    @Override
    public void iccIdInfoInd(int type, String iccidInfo) {
    }

    @Override
    public void nasTimerStatusInd(int type, NasTimerStatus status) {
    }

    @Override
    public void oemHookRaw(int type, ArrayList<Byte> data) {
    }

    @Override
    public void onUssdWithDcsInd(int type, String modeType, String msg, String dcs) {
    }

    @Override
    public void suppSvcReturnResult(int type, String result) {
    }

    @Override
    public void ussdCanceledInd(int indicationType) {
    }

    @Override
    public void volteAvailableInfoInd(int type, int volteAvailable, int emcAvailable) {
    }

    @Override
    public void vsimOperationInd(int type, VsimOperationEvent event) {
    }

    @Override
    public void wbAmrReportInd(int type, boolean on) {
    }

    @Override
    public void simTrayStatusChangedInd(int indicationType, int state) {
    }

    @Override
    public void postUrsp_1_6(int indicationType, int slotIndex, int dataType, String originalUrsp, ArrayList<UrspRule> urspList) {
    }

    @Override
    public void unthrottleApn(int indicationType, DataProfileInfo dataProfileInfo) {
    }
}
