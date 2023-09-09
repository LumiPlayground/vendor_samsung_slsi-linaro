/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

/*
 * protocolcallbuilderinterface.h
 *
 *  Created on: 2021. 06. 08.
 *      Author: p1.bansal
 */

#ifndef __PROTOCOL_CALL_BUILDER_INTERFACE_H__
#define __PROTOCOL_CALL_BUILDER_INTERFACE_H__

#include <base/rildef.h>

class ModemData;
class UusInfo;

/**
 * ProtocolCallBuilderInterface
 */
class ProtocolCallBuilderInterface
{
public:
    ProtocolCallBuilderInterface() = default;
    virtual ~ProtocolCallBuilderInterface() = default;

public:
    virtual ModemData *BuildGetCallList() = 0;
    virtual ModemData *BuildDial(char *number, ClirType clir, UusInfo &uusInfo/*not used*/, CallType cType,
                            UINT eccType = 0x00, UINT eCallUi = 0x00) = 0;
    virtual ModemData *BuildAnswer() = 0;
    virtual ModemData *BuildExplicitCallTransfer() = 0;
    virtual ModemData *BuildHangup(int callId) = 0;
    virtual ModemData *BuildHangupMulti(int callId) = 0;
    virtual ModemData *BuildLastCallFailCause() = 0;
    virtual ModemData *BuildCdmaBurstDtmf(int dmtf_length, const char* dtmf,
            int on_len, const char* on_length, int off_len, const char* off_length) = 0;

    virtual ModemData *BuildCdmaSetPreferredVoicePrivacyMode(int vpMode) = 0;
    virtual ModemData *BuildCdmaGetPreferredVoicePrivacyMode() = 0;

    virtual ModemData *BuildHangupWaitingOrBackground() = 0;
    virtual ModemData *BuildHangupForegroundResumeBackground() = 0;
    virtual ModemData *BuildSwitchWaitingOrHoldingAndActive() = 0;
    virtual ModemData *BuildConference() = 0;
    virtual ModemData *BuildCdmaFlash(const char* flash) = 0;
    virtual ModemData *BuildUdub() = 0;
    virtual ModemData *BuildGetClip() = 0;
    virtual ModemData *BuildGetClir() = 0;
    virtual ModemData *BuildSetCallForwarding(SsModeType status, SsCfReason reason, SsClassX service_class, int toa, char* number, int timeseconds) = 0;
    virtual ModemData*BuildGetCallForwardingStatus(SsStatusType status /*not sued*/, SsCfReason reason, SsClassX service_class, int toa, char* number, int timeseconds) = 0;
    virtual ModemData *BuildSetCallWaiting(SsModeType status, SsClassX service_class) = 0;
    virtual ModemData *BuildGetCallWaiting(SsClassX service_class) = 0;
    virtual ModemData *BuildChangeBarringPwd(const char* oldpasswd, const char* newpasswd, const char* newpasswd_again) = 0;
    virtual ModemData *BuildSendUssd(const char* ussd, bool user_initiated) = 0;
    virtual ModemData *BuildCancelUssd() = 0;
    virtual ModemData *BuildSeparateConnection(int callId) = 0;

    virtual ModemData *BuildQueryColp() = 0;
    virtual ModemData *BuildQueryColr() = 0;
    virtual ModemData *BuildSendEncodedUssd(BYTE dcs, const char* encodedUssd, bool user_initiated) = 0;
    virtual ModemData *BuildSendCallConfirm() = 0;

    virtual ModemData *BuildExitEmergencyCbMode() = 0;
};

#endif /* __PROTOCOL_CALL_BUILDER_INTERFACE_H__ */
