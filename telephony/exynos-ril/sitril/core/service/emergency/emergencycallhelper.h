/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */
#ifndef __EMERGENCY_CALL_HELPER_H__
#define __EMERGENCY_CALL_HELPER_H__

#include <chrono>
#include <thread>
#include <functional>
#include "callreqdata.h"
#include "rildef.h"

class CscService;

typedef std::chrono::milliseconds Interval;
typedef std::function<void(bool)> Function;

typedef enum {
    WAIT_NONE  = 0,
    WAIT_CS    = 1,
    WAIT_VOLTE = 2,
    WAIT_ANY   = 3,
} WAIT_TYPE;

typedef struct {
    char mccmnc[8];
    WAIT_TYPE wait_type;
} operator_wait_config;

class Timer {
    std::thread mTh;
    bool mRunning = false;
    unsigned int mTick = 0;
public:
    void Start(const Interval &interval, const Function &function, unsigned int maxTick);
    void Stop();
};

class EmergencyCallHelper {
private:
    CscService *mService;
    Timer mTimer;
    bool mIsRadioOnForEmergencyCall;
    bool mUnderWaiting;
    char mNumber[MAX_DIAL_NUM + 1];
    ClirType mClir;
    RIL_UUS_Info mUusInfo;
    char mUusInfoData[MAX_UUS_DATA_LEN];
    UINT mEccType;
    bool mIsTesting;
    WAIT_TYPE mWaitType;
    bool mWaitingRedialIms;
    int mImsRegState;
    CallType mCallType;
    bool mNormalRoutingWhenInService;
public:
    EmergencyCallHelper(CscService *service);
    ~EmergencyCallHelper();
    bool IsRadioOnForEmergencyCall();
    void SetRadioOnForEmergencyCall(bool radioOnForEmergencyCall);
    bool IsWaitingRedialIms();
    void ResetWaitingRedialIms();
    bool NeedToWaitRegi(CallType callType, bool normalRoutingWhenInService);
    void SetDialArgs(char *number, ClirType clir, UusInfo uusInfo/*not used*/,
                     UINT eccType, bool isTesting);
    void WaitForEmergencyCall(char *number, ClirType clir, UusInfo uusInfo/*not used*/,
                              UINT eccType, bool isTesting);
    bool IsUnderWaiting();
    void Clear();
    void MakeEmergencyCall(bool timeout);
    bool IsOkToCall();
    void SetConfig(CallType callType, bool normalRoutingWhenInService);
    WAIT_TYPE GetConfig();
    void GetFakeDialingCallList(CallList* callList);
    void Abort();
};

#endif
