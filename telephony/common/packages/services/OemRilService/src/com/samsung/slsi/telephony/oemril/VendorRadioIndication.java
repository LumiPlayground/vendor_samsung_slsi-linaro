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
import static com.samsung.slsi.telephony.oemril.VendorRIL.loge;
import static com.samsung.slsi.telephony.oemril.VendorRILConstants.*;

import android.os.AsyncResult;
import android.telephony.ServiceState;
import android.telephony.data.ApnSetting;
import android.telephony.data.DataProfile;
import android.telephony.data.TrafficDescriptor;

import com.android.internal.telephony.RIL;
import com.android.internal.telephony.RILUtils;
import com.android.telephony.Rlog;

import vendor.samsung_slsi.telephony.hardware.radio.V1_0.NasTimerStatus;
import vendor.samsung_slsi.telephony.hardware.radio.V1_2.DataProfileInfo;
import vendor.samsung_slsi.telephony.hardware.radio.V1_2.IOemSamsungslsiIndication;

/**
 * class VendorRadioIndication
 *
 * - Override only supported indication here.
 *
 */
public class VendorRadioIndication extends VendorRadioIndicationBase {
    VendorRIL mRil;

    public VendorRadioIndication(VendorRIL ril) {
        mRil = ril;
    }

    @Override
    public void endcCapabilityInd(int type, int capability, int cause) {
        mRil.processIndication(type);

        if (VendorRIL.RILJ_LOGD) mRil.unsljLog(RIL_UNSOL_ENDC_CAPABILITY);

        if (mRil.mNrDualConnectivityStateRegistrants != null) {
            int[] result = new int[] { capability, cause };
            mRil.mNrDualConnectivityStateRegistrants.notifyRegistrants(new AsyncResult(null, result, null));
        }
    }

    @Override
    public void iccIdInfoInd(int type, String iccidInfo) {
        mRil.processIndication(type);

        if (VendorRIL.RILJ_LOGD) mRil.unsljLog(RIL_UNSOL_ICCID_INFO);

        if (mRil.mIccIdInfoRegistrants != null) {
            mRil.mIccIdInfoRegistrants.notifyRegistrants(new AsyncResult(null, iccidInfo, null));
        }
    }


    @Override
    public void simTrayStatusChangedInd(int indicationType, int state) {
        mRil.processIndication(indicationType);

        if (VendorRIL.RILJ_LOGD) mRil.unsljLog(RIL_UNSOL_SIM_TRAY_STATUS_CHANGED);

        if (mRil.mSimTrayStatusRegistrants != null) {
            mRil.mSimTrayStatusRegistrants.notifyRegistrants(new AsyncResult(null, state, null));
        }
    }

    @Override
    public void nasTimerStatusInd(int type, NasTimerStatus status) {
        mRil.processIndication(type);

        if (VendorRIL.RILJ_LOGD) mRil.unsljLog(RIL_UNSOL_NAS_TIMER_STATUS_IND);

        if (mRil.mDataNasTimerStatusRegistrants != null) {
            mRil.mDataNasTimerStatusRegistrants.notifyRegistrants(new AsyncResult(null, status, null));
        }
    }

    /* @1.2 Extended */
    private static TrafficDescriptor convertHalTrafficDescriptor(
            android.hardware.radio.V1_6.TrafficDescriptor td) throws IllegalArgumentException {
        String dnn = td.dnn.getDiscriminator()
                == android.hardware.radio.V1_6.OptionalDnn.hidl_discriminator.noinit
                ? null : td.dnn.value();
        byte[] osAppId = td.osAppId.getDiscriminator()
                == android.hardware.radio.V1_6.OptionalOsAppId.hidl_discriminator.noinit
                ? null : RILUtils.arrayListToPrimitiveArray(td.osAppId.value().osAppId);

        TrafficDescriptor.Builder builder = new TrafficDescriptor.Builder();
        if (dnn != null) {
            builder.setDataNetworkName(dnn);
        }
        if (osAppId != null) {
            builder.setOsAppId(osAppId);
        }
        return builder.build();
    }

    public static DataProfile convertToDataProfile(DataProfileInfo dpi) {
        ApnSetting apnSetting = new ApnSetting.Builder()
                .setEntryName(dpi.apn)
                .setApnName(dpi.apn)
                .setApnTypeBitmask(dpi.supportedApnTypesBitmap)
                .setAuthType(dpi.authType)
                .setMaxConnsTime(dpi.maxConnsTime)
                .setMaxConns(dpi.maxConns)
                .setWaitTime(dpi.waitTime)
                .setCarrierEnabled(dpi.enabled)
                .setModemCognitive(dpi.persistent)
                .setMtuV4(dpi.mtuV4)
                .setMtuV6(dpi.mtuV6)
                .setNetworkTypeBitmask(ServiceState.convertBearerBitmaskToNetworkTypeBitmask(
                        dpi.bearerBitmap) >> 1)
                .setProfileId(dpi.profileId)
                .setPassword(dpi.password)
                .setProtocol(dpi.protocol)
                .setRoamingProtocol(dpi.roamingProtocol)
                .setUser(dpi.user)
                .setAlwaysOn(dpi.alwaysOn)
                .build();

        TrafficDescriptor td;
        try {
            td = convertHalTrafficDescriptor(dpi.trafficDescriptor);
        } catch (IllegalArgumentException e) {
            loge("convertToDataProfile: Failed to convert traffic descriptor. e=" + e);
            td = null;
        }

        return new DataProfile.Builder()
                .setType(dpi.type)
                .setPreferred(dpi.preferred)
                .setTrafficDescriptor(td)
                .setApnSetting(apnSetting)
                .build();
    }

    @Override
    public void unthrottleApn(int indicationType, DataProfileInfo dataProfileInfo) {
        mRil.processIndication(indicationType);
        DataProfile response = convertToDataProfile(dataProfileInfo);

        if (VendorRIL.RILJ_LOGD) mRil.unsljLog(RIL_UNSOL_UNTHROTTLE_APN);

        mRil.mUnthrottleApnRegistrants.notifyRegistrants(new AsyncResult(null, response, null));
    }
}
