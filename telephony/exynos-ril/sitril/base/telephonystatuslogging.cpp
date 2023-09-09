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
 * telephonystatuslogging.cpp
 *
 *  Created on: 2021. 10. 22.
 *      Author: sihwan14.lee
 */

#include <librilutils/systemproperty.h>
#include <telephony/ril.h>
#include "build.h"
#include "rillog.h"
#include "telephonystatuslogging.h"
#include "telephonystatustracker.h"
#include <unistd.h>
#include <fcntl.h>

IMPLEMENT_MODULE_TAG(TelephonyStatusLogging, TelephonyStatusLogging)

TelephonyStatusLogging::TelephonyStatusLogging()
    : mRunnable(false), mDumpAll(DEFAULT_DUMP_ALL), mInterval(DEFAULT_INTERVAL_SEC) {
    mPipeFds[PIPE_FD_IDX_READ] = -1;
    mPipeFds[PIPE_FD_IDX_WRITE] = -1;
    mThread = nullptr;
}

TelephonyStatusLogging::~TelephonyStatusLogging() {
    Stop();
}

TelephonyStatusLogging *TelephonyStatusLogging::sInstance = nullptr;

TelephonyStatusLogging *TelephonyStatusLogging::GetInstance() {
    if (!Build::IS_ENG && !Build::IS_USERDEBUG) {
        RilLogE("[%s::%s] Logging doesn't work on current build mode(%s mode)", TAG, __FUNCTION__, Build::Type().c_str());
        return nullptr;
    }

    if (sInstance == nullptr) {
        SystemProperty::Set(PROPERTY_TELEPHONY_STATUS_LOGGING_ONOFF, "Off");
        sInstance = new TelephonyStatusLogging();
    }
    return sInstance;
}

void TelephonyStatusLogging::ResetInstance() {
    if (sInstance != nullptr) {
        delete sInstance;
        sInstance = nullptr;
    }
}

bool TelephonyStatusLogging::Notify(UINT noti) {
    if (mPipeFds[PIPE_FD_IDX_WRITE] == -1) {
        RilLogE("[%s::%s] write pipe is not opened", TAG, __FUNCTION__);
        return false;
    }
    int writeCount = write(mPipeFds[PIPE_FD_IDX_WRITE], &noti, sizeof(UINT));
    if (writeCount <= 0) {
        RilLogE("[%s::%s] Failed to write pipe(%s)", TAG, __FUNCTION__, (writeCount < 0 ? strerror(errno) : "no data"));
        return false;
    }
    return true;
}

bool TelephonyStatusLogging::Stop() {
    if (mRunnable) {
        mRunnable = false;
        if (!Notify(NOTI_STOP))
            return false;
    }
    // TODO: Terminate std::thread before deleting mThread
    if (mThread != nullptr) {
        delete mThread;
        mThread = nullptr;
    }
    return true;
}

bool TelephonyStatusLogging::Start(bool dumpAll /* = DEFAULT_DUMP_ALL*/, int interval /* = DEFAULT_INTERVAL_SEC*/) {
    if (mThread != nullptr || SystemProperty::Get(PROPERTY_TELEPHONY_STATUS_LOGGING_ONOFF) == "On") {
        RilLogE("[%s::%s] Logging is already running", TAG, __FUNCTION__);
        return false;
    }

    if (pipe2(mPipeFds, O_CLOEXEC) == -1) {
        RilLogE("[%s::%s] Failed to open pipe(%s)", TAG, __FUNCTION__, strerror(errno));
        return false;
    }

    mDumpAll = dumpAll;
    mInterval = interval;

    mThread = new Thread(this);
    mThread->Start();
    mThread->Detach();

    return true;
}

void TelephonyStatusLogging::Run() {
    RilLogV("[%s::%s] Start Telephony Status Logging", TAG, __FUNCTION__);
    SystemProperty::Set(PROPERTY_TELEPHONY_STATUS_LOGGING_ONOFF, "On");
    SystemProperty::Set(PROPERTY_TELEPHONY_STATUS_LOGGING_DUMPALL, mDumpAll ? "On" : "Off");
    SystemProperty::Set(PROPERTY_TELEPHONY_STATUS_LOGGING_INTERVAL, mInterval);

    mRunnable = true;

    while (mRunnable) {
        fd_set fdSet;
        FD_ZERO(&fdSet);
        FD_SET(mPipeFds[PIPE_FD_IDX_READ], &fdSet);
        int maxFd = mPipeFds[PIPE_FD_IDX_READ] + 1;

        struct timeval tv;
        tv.tv_sec = mInterval;
        tv.tv_usec = 0;

        for (int phoneId = 0; phoneId < SIM_COUNT; phoneId++) {
            TelephonyStatusTracker::createForPhoneId(phoneId).dump(mDumpAll);
        }

        int ret = select(maxFd, &fdSet, NULL, NULL, &tv);
        if (ret > 0) {
            if (FD_ISSET(mPipeFds[PIPE_FD_IDX_READ], &fdSet)) {
                UINT noti;
                int readCount = read(mPipeFds[PIPE_FD_IDX_READ], &noti, sizeof(UINT));
                if (readCount <= 0) {
                    RilLogE("[%s::%s] Failed to read pipe(%s)", TAG, __FUNCTION__, (readCount < 0 ? strerror(errno) : "no data"));
                }
            }
        } else if (ret < 0) {
            RilLogE("[%s::%s] select() error(%s)", TAG, __FUNCTION__, strerror(errno));
            mRunnable = false;
        }
    }

    close(mPipeFds[PIPE_FD_IDX_READ]);
    close(mPipeFds[PIPE_FD_IDX_WRITE]);
    mPipeFds[PIPE_FD_IDX_READ] = -1;
    mPipeFds[PIPE_FD_IDX_WRITE] = -1;
    SystemProperty::Set(PROPERTY_TELEPHONY_STATUS_LOGGING_ONOFF, "Off");
    RilLogV("[%s::%s] Stop Telephony Status Logging", TAG, __FUNCTION__);
}
