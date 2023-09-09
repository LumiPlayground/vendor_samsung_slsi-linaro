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

import android.annotation.NonNull;
import android.content.Context;
import android.os.Message;
import android.telephony.SubscriptionManager;
import android.telephony.data.DataProfile;

import android.net.LinkProperties;
import android.telephony.data.NetworkSliceInfo;

public class VendorRadioContext {
    @NonNull
    private Context mContext;

    @NonNull
    private VendorCommandsInterface mCi;

    private int mPhoneId = SubscriptionManager.INVALID_PHONE_INDEX;

    @NonNull
    private VendorTelephonyTracker mTracker;

    @NonNull
    private VendorTelephonyNotifier mNotifier;

    public VendorRadioContext(Context context, VendorCommandsInterface ci,
            Integer phoneId, VendorTelephonyNotifier notifer) {
        mContext = context;
        mCi = ci;
        mPhoneId = phoneId == null ? 0 : phoneId;
        mNotifier = notifer;

        mTracker = new VendorTelephonyTracker(this, mCi);
    }

    public Context getContext() { return mContext; }

    public int getPhoneId() { return mPhoneId; }

    public VendorCommandsInterface getRadioInterface() { return mCi; }

    public void notifyIccidInfo(String iccid) {
        mNotifier.notifyIccidInfo(mPhoneId, iccid);
    }

    public void notifyNrDualConnectivityState(boolean enabled, int cause) {
        mNotifier.notifyNrDualConnectivityState(mPhoneId, enabled, cause);
    }

    public void notifySimTrayState(int state) {
        mNotifier.notifySimTrayState(mPhoneId, state);
    }

    void getSmsStorageOnSim(int simId, Message result) {
        mCi.getSmsStorageOnSim(simId, result);
    }

    void setNrMode(int mode, Message result) {
        mCi.setNrMode(mode, result);
    }

    void getNrMode(Message result) {
        mCi.getNrMode(result);
    }

    void setActivateVsim(int slot, String iccid, String imsi, String hplmn,
            int vsimState, int vsimCardType, Message result) {
        mCi.setActivateVsim(slot, iccid, imsi, hplmn, vsimState, vsimCardType, result);
    }

    void setupDataCall_1_6(int accessNetworkType, DataProfile dataProfile, boolean isRoaming,
                                  boolean allowRoaming, int reason, LinkProperties linkProperties, int pduSessionId,
                                  NetworkSliceInfo sliceInfo, android.cmccslice.TrafficDescriptor trafficDescriptor,
                                  boolean matchAllRuleAllowed, Message result) {

        mCi.setupDataCall_1_6(accessNetworkType, dataProfile, isRoaming, allowRoaming, reason,
                                linkProperties, pduSessionId, sliceInfo, trafficDescriptor,
                                matchAllRuleAllowed, result);
    }

    public void notifyNasTimer(int type, int status, int value, String apn) {
        mNotifier.notifyNasTimer(mPhoneId, type, status, value, apn);
    }

    public void notifyUnthrottleApn(DataProfile dataProfile) {
        mNotifier.notifyUnthrottleApn(mPhoneId, dataProfile);
    }
}
