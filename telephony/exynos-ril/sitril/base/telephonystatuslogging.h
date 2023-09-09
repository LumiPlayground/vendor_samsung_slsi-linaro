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
* SAMSUNG make no representations or warranties about the suitability of
* the software, either express or implied, including but not limited to
* the implied warranties of merchantability, fitness for a particular
* purpose, or non-infringement. SAMSUNG shall not be liable for any
* damages suffered by license as a result of using, modifying or
* distributing this software or its derivatives.
*/
/*
 * telephonystatuslogging.h
 *
 *  Created on: 2021. 10. 22.
 *      Author: sihwan14.lee
 */

#ifndef __TELEPHONY_STATUS_LOGGING_H__
#define __TELEPHONY_STATUS_LOGGING_H__

#include "thread.h"

#define PIPE_FD_IDX_READ 0
#define PIPE_FD_IDX_WRITE 1

#define DEFAULT_DUMP_ALL true
#define DEFAULT_INTERVAL_SEC 5

#define PROPERTY_TELEPHONY_STATUS_LOGGING_ONOFF "persist.vendor.ril.telephonystatuslogging.onoff"
#define PROPERTY_TELEPHONY_STATUS_LOGGING_DUMPALL "persist.vendor.ril.telephonystatuslogging.dumpall"
#define PROPERTY_TELEPHONY_STATUS_LOGGING_INTERVAL "persist.vendor.ril.telephonystatuslogging.interval"

class TelephonyStatusTracker;

class TelephonyStatusLogging : public Runnable {
    DECLARE_MODULE_TAG()

  public:
    const static int NOTI_STOP = 0;

  private:
    static TelephonyStatusLogging *sInstance;
    Thread *mThread;
    int mPipeFds[2];
    bool mRunnable;
    bool mDumpAll;
    int mInterval;

  private:
    TelephonyStatusLogging();
    TelephonyStatusLogging(const TelephonyStatusLogging &) = delete;
    TelephonyStatusLogging& operator=(TelephonyStatusLogging const&) = delete;
    ~TelephonyStatusLogging();

  public:
    static TelephonyStatusLogging *GetInstance();
    static void ResetInstance();

  public:
    bool Notify(UINT noti);
    bool Stop();
    bool Start(bool dumpAll = DEFAULT_DUMP_ALL, int interval = DEFAULT_INTERVAL_SEC);
    void Run();
};

#endif /* __TELEPHONY_STATUS_LOGGING_H__ */
