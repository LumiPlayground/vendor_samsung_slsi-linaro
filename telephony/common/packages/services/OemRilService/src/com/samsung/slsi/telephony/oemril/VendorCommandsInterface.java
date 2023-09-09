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

import android.os.Handler;
import android.os.Message;

import android.telephony.data.DataProfile;
import android.net.LinkProperties;

import android.cmccslice.TrafficDescriptor;
import android.telephony.data.NetworkSliceInfo;

public interface VendorCommandsInterface {
    /**
     * setNrMode
     *
     * @param serial Serial number of request.
     * @param mode 0x00=No NR 0x01=NSA 0x02=SA 0x03=NSA+SA 0x04=Reset to default
     */
    void setNrMode(int mode, Message result);

    /**
     * getNrMode
     *
     * @param serial Serial number of request.
     */
    void getNrMode(Message result);

    /**
     * setActivateVsim
     *
     * @param serial       Serial number of request.
     * @param slot         slot ID( 1 = SLOT_1, 2 = SLOT_2, 3 = SLOT_3)
     * @param iccid        ICCIC
     * @param imsi         IMSI
     * @param hplmn        Home PLMN numeric string
     * @param vsimState    VSIM's state ( 0: deactvate, 1: activate)
     * @param vsimCardType VSIM's card type ( 1 = CDMA vsim, 2 = 2G vsim, 3 = 3G
     *                     vsim, 4 = 4G vsim )
     */
    void setActivateVsim(int slot, String iccid, String imsi, String hplmn,
            int vsimState, int vsimCardType, Message result);

    /**
     * Get sms capacity of SIM memory (EF_SMS).
     *
     * @param simId is sim description 0x00: SIM, 0x01: RUIM
     * @param response sent when operation completes.
     *                  response.obj will be an AsyncResult, and will indicate
     *                  any error that may have occurred.
     */
    void getSmsStorageOnSim(int simId, Message result);

    /**
     * setupDataCall_1_6
     */
    void setupDataCall_1_6(int accessNetworkType, DataProfile dataProfile, boolean isRoaming,
                                  boolean allowRoaming, int reason, LinkProperties linkProperties, int pduSessionId,
                                  NetworkSliceInfo sliceInfo, android.cmccslice.TrafficDescriptor trafficDescriptor,
                                  boolean matchAllRuleAllowed, Message result);

    void registerForIccIdInfo(Handler h, int what, Object obj);

    void unregisterForIccIdInfo(Handler h);

    void registerForNrDualConnectivityState(Handler h, int what, Object obj);

    void unregisterForNrDualConnectivityState(Handler h);

    /**
    * Register for unsolicited SIM Tray Status Changed Indications
    *
    * @param h Handler for notification message.
    * @param what User-defined message code.
    * @param obj User object.
    */
    void registerForSimTrayStatus(Handler h, int what, Object obj);

    /**
     * Deregister for unsolicited SIM Tray Status Changed Indications
     *
     * @param h Handler for notification message.
     */
    void unregisterForSimTrayStatus(Handler h);

    /**
     * Merged for supporting NasTimer
     */
    void registerForDataNasTimerStatusChanged(Handler h, int what, Object obj);

    void unregisterForDataNasTimerStatusChanged(Handler h);

    void registerForUnthrottleApn(Handler h, int what, Object obj);

    void unregisterForUnthrottleApn(Handler h);
}
