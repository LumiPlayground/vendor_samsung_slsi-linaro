/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd All Rights Reserved
 * PROPRIETARY/CONFIDENTIAL
 *
 * This software is the confidential and proprietary information of
 * SAMSUNG ELECTRONICS ("Confidential Information").
 *
 * You shall not disclose such Confidential Information and shall use it
 * only in accordance with the terms of the license agreement
 * you entered into with SAMSUNG ELECTRONICS.
 *
 * SAMSUNG make no representations or warranties about the suitability of
 * the software, either express or implied, including but not limited to
 * the implied warranties of merchantability, fitness for a particular
 * purpose, or non-infringement. SAMSUNG shall not be liable for any
 * damages suffered by license as a result of using, modifying or
 * distributing this software or its derivatives.
 */
/*
 * telephonystatustracker.cpp
 *
 *  Created on: 2021. 10. 19.
 */

#include <telephony/ril.h>
#include "telephonystatustracker.h"
#include "rillog.h"
#include <librilutils/systemproperty.h>
#include "rilversioninfo.h"

void TelephonyStatusTracker::registerMonitoringCallback(MonitoringCallback *callback, bool periodic) {
    mList.add( { callback, periodic });
}

void TelephonyStatusTracker::unregisterMonitoringCallback(MonitoringCallback *callback) {
    mList.remove(callback);
}

void TelephonyStatusTracker::dump(bool forceDump/* = true*/) {
    std::vector<std::string> logs;
    for(const auto &cur : mList.mItems) {
        if (cur.callback && (forceDump || cur.periodic)) {
            std::stringstream ss;
            cur.callback->dump(ss);
            logs.push_back(ss.str());
        }
    }

    RilLogV("================TelephonyStatusTracker(%d)=======================",
            mPhoneId);
    for (const auto &s : logs) {
        RilLogV("%s", s.c_str());
    }
    RilLogV("=============================================================");

    if(mPhoneId == 0){
        RilLogV("Build Info : %s", SystemProperty::Get(RIL_VENDOR_RADIO_BUILD_INFO).c_str());
    }
}

TelephonyStatusTracker TelephonyStatusTracker::sTracker[SIM_COUNT];
TelephonyStatusTracker& TelephonyStatusTracker::createForPhoneId(int phoneId) {
    if (phoneId < 0 || phoneId >= SIM_COUNT) {
        phoneId = 0;
    }

    TelephonyStatusTracker &tracker = sTracker[phoneId];
    if (tracker.mPhoneId == -1) {
        tracker.mPhoneId = phoneId;
    }
    return tracker;
}
