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
 * protocolcallbuilderjson.h
 *
 *  Created on: 2021. 07. 13.
 *      Author: p1.bansal
 */

#ifndef __PROTOCOL_CALL_BUILDER_JSON_H__
#define __PROTOCOL_CALL_BUILDER_JSON_H__

#include "protocolcallbuilderinterface.h"
#include "protocoljsonbuilder.h"

/**
 * ProtocolCallBuilderJson
 */
class ProtocolCallBuilderJson : public ProtocolJsonBuilder, public ProtocolCallBuilderInterface {
  public:
    ProtocolCallBuilderJson()
        : ProtocolJsonBuilder(), ProtocolCallBuilderInterface() {}
    virtual ~ProtocolCallBuilderJson() = default;

  public:
    ModemData *BuildGetCallList();
    ModemData *BuildDial(char *number, ClirType clir, UusInfo &uusInfo /*not used*/, CallType cType,
                         UINT eccType = 0x00, UINT eCallUi = 0x00);
    ModemData *BuildAnswer();
    ModemData *BuildExplicitCallTransfer();
    ModemData *BuildHangup(int callId);
    ModemData *BuildHangupMulti(int callId);
    ModemData *BuildLastCallFailCause();
    ModemData *BuildCdmaBurstDtmf(int dmtf_length, const char *dtmf,
                                  int on_len, const char *on_length, int off_len, const char *off_length);

    ModemData *BuildCdmaSetPreferredVoicePrivacyMode(int vpMode);
    ModemData *BuildCdmaGetPreferredVoicePrivacyMode();

    ModemData *BuildHangupWaitingOrBackground();
    ModemData *BuildHangupForegroundResumeBackground();
    ModemData *BuildSwitchWaitingOrHoldingAndActive();
    ModemData *BuildConference();
    ModemData *BuildCdmaFlash(const char *flash);
    ModemData *BuildUdub();
    ModemData *BuildGetClip();
    ModemData *BuildGetClir();
    ModemData *BuildSetCallForwarding(SsModeType status, SsCfReason reason, SsClassX service_class, int toa, char *number, int timeseconds);
    ModemData *BuildGetCallForwardingStatus(SsStatusType status, SsCfReason reason, SsClassX service_class, int toa, char *number, int timeseconds);
    ModemData *BuildSetCallWaiting(SsModeType status /*not used*/, SsClassX service_class);
    ModemData *BuildGetCallWaiting(SsClassX service_class);
    ModemData *BuildChangeBarringPwd(const char *oldpasswd, const char *newpasswd, const char *newpasswd_again);
    ModemData *BuildSendUssd(const char *ussd, bool user_initiated);
    ModemData *BuildCancelUssd();
    ModemData *BuildSeparateConnection(int callId);

    ModemData *BuildQueryColp();
    ModemData *BuildQueryColr();
    ModemData *BuildSendEncodedUssd(BYTE dcs, const char *encodedUssd, bool user_initiated);
    ModemData *BuildSendCallConfirm();

    ModemData *BuildExitEmergencyCbMode();
};

#endif /* __PROTOCOL_CALL_BUILDER_JSON_H__ */
