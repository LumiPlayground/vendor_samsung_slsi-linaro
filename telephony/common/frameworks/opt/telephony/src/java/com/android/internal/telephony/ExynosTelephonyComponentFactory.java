/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

package com.android.internal.telephony;

import android.content.Context;

import com.android.ims.ImsManager;
import com.android.internal.telephony.cdma.EriManager;
import com.android.internal.telephony.dataconnection.DcTracker;
import com.android.internal.telephony.emergency.EmergencyNumberTracker;
import com.android.internal.telephony.imsphone.ImsPhone;
import com.android.internal.telephony.imsphone.ImsPhoneCallTracker;
import com.android.telephony.Rlog;

import com.android.internal.telephony.vendor.dataconnection.ExynosDcTracker;
import com.android.internal.telephony.vendor.ExynosGsmCdmaCallTracker;
import com.android.internal.telephony.vendor.ExynosGsmCdmaPhone;
import com.android.internal.telephony.vendor.imsphone.ExynosImsPhoneCallTracker;
import com.android.internal.telephony.vendor.ExynosServiceStateTracker;

/**
 * {@hide}
 */
public class ExynosTelephonyComponentFactory extends TelephonyComponentFactory {
    private static final String TAG = ExynosTelephonyComponentFactory.class.getSimpleName();

    @Override
    public DcTracker makeDcTracker(Phone phone, int transportType) {
        Rlog.v(TAG, "makeExynosDcTracker");
        return new ExynosDcTracker(phone, transportType);
    }
    @Override
    public DeviceStateMonitor makeDeviceStateMonitor(Phone phone) {
        Rlog.v(TAG, "makeDeviceStateMonitor");
        return super.makeDeviceStateMonitor(phone);
    }

    @Override
    public EmergencyNumberTracker makeEmergencyNumberTracker(Phone phone, CommandsInterface ci) {
        Rlog.v(TAG, "makeEmergencyNumberTracker");
        return super.makeEmergencyNumberTracker(phone, ci);
    }

    @Override
    public EriManager makeEriManager(Phone phone, int eriFileSource) {
        Rlog.v(TAG, "makeEriManager");
        return super.makeEriManager(phone, eriFileSource);
    }

    @Override
    public GsmCdmaCallTracker makeGsmCdmaCallTracker(GsmCdmaPhone phone) {
        Rlog.v(TAG, "makeExynosGsmCdmaCallTracker");
        return new ExynosGsmCdmaCallTracker(phone);
    }

    @Override
    public ImsPhoneCallTracker makeImsPhoneCallTracker(ImsPhone imsPhone) {
        Rlog.v(TAG, "makeExynosImsPhoneCallTracker");
        return new ExynosImsPhoneCallTracker(imsPhone, ImsManager::getConnector);
    }

    @Override
    public ServiceStateTracker makeServiceStateTracker(GsmCdmaPhone phone, CommandsInterface ci) {
        Rlog.v(TAG, "makeExynosServiceStateTracker");
        return new ExynosServiceStateTracker(phone, ci);
    }

    @Override
    public Phone makePhone(Context context, CommandsInterface ci, PhoneNotifier notifier,
            int phoneId, int precisePhoneType,
            TelephonyComponentFactory telephonyComponentFactory) {
        Rlog.v(TAG, "makeExynosGsmCdmaPhone");
        return new ExynosGsmCdmaPhone(context, ci, notifier, phoneId, precisePhoneType,
                       telephonyComponentFactory);
    }

    @Override
    public IccPhoneBookInterfaceManager makeIccPhoneBookInterfaceManager(Phone phone) {
        Rlog.v(TAG, "makeIccPhoneBookInterfaceManager");
        return new ExynosIccPhoneBookInterfaceManager(phone);
    }
}
