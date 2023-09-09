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
 * protocolcallbuilder.cpp
 *
 *  Created on: 2021. 06. 08.
 *      Author: p1.bansal
 */
#include "protocolcallbuilder.h"
#include "json/cs/protocolcallbuilderjson.h"
#include "legacy/cs/protocolcallbuilderlegacy.h"

/**
 * ProtocolCallBuilder
 */
ModemData *ProtocolCallBuilder::BuildGetCallList() {
    return protocolCallBuilderInterface->BuildGetCallList();
}

ModemData *ProtocolCallBuilder::BuildDial(char *number, ClirType clir, UusInfo &uusInfo, CallType cType, UINT eccType, UINT eCallUi) {
    return protocolCallBuilderInterface->BuildDial(number, clir, uusInfo, cType, eccType, eCallUi);
}

ModemData *ProtocolCallBuilder::BuildAnswer() {
    return protocolCallBuilderInterface->BuildAnswer();
}

ModemData *ProtocolCallBuilder::BuildExplicitCallTransfer() {
    return protocolCallBuilderInterface->BuildExplicitCallTransfer();
}
ModemData *ProtocolCallBuilder::BuildHangup(int callId) {
    return protocolCallBuilderInterface->BuildHangup(callId);
}

ModemData *ProtocolCallBuilder::BuildHangupMulti(int callId) {
    return protocolCallBuilderInterface->BuildHangupMulti(callId);
}

ModemData *ProtocolCallBuilder::BuildLastCallFailCause() {
    return protocolCallBuilderInterface->BuildLastCallFailCause();
}

ModemData *ProtocolCallBuilder::BuildCdmaBurstDtmf(int dtmf_length, const char *dtmf,
                                                   int on_len, const char *on_length, int off_len, const char *off_length) {
    return protocolCallBuilderInterface->BuildCdmaBurstDtmf(dtmf_length, dtmf, on_len, on_length, off_len, off_length);
}

ModemData *ProtocolCallBuilder::BuildCdmaSetPreferredVoicePrivacyMode(int vpMode) {
    return protocolCallBuilderInterface->BuildCdmaSetPreferredVoicePrivacyMode(vpMode);
}

ModemData *ProtocolCallBuilder::BuildCdmaGetPreferredVoicePrivacyMode() {
    return protocolCallBuilderInterface->BuildCdmaGetPreferredVoicePrivacyMode();
}

ModemData *ProtocolCallBuilder::BuildHangupWaitingOrBackground() {
    return protocolCallBuilderInterface->BuildHangupWaitingOrBackground();
}

ModemData *ProtocolCallBuilder::BuildHangupForegroundResumeBackground() {
    return protocolCallBuilderInterface->BuildHangupForegroundResumeBackground();
}

ModemData *ProtocolCallBuilder::BuildSwitchWaitingOrHoldingAndActive() {
    return protocolCallBuilderInterface->BuildSwitchWaitingOrHoldingAndActive();
}

ModemData *ProtocolCallBuilder::BuildConference() {
    return protocolCallBuilderInterface->BuildConference();
}

ModemData *ProtocolCallBuilder::BuildCdmaFlash(const char *flash) {
    return protocolCallBuilderInterface->BuildCdmaFlash(flash);
}

ModemData *ProtocolCallBuilder::BuildUdub() {
    return protocolCallBuilderInterface->BuildUdub();
}

ModemData *ProtocolCallBuilder::BuildGetClip() {
    return protocolCallBuilderInterface->BuildGetClip();
}

ModemData *ProtocolCallBuilder::BuildGetClir() {
    return protocolCallBuilderInterface->BuildGetClir();
}

ModemData *ProtocolCallBuilder::BuildSetCallForwarding(SsModeType status, SsCfReason reason, SsClassX service_class,
                                                       int toa, char *number, int timeseconds) {
    return protocolCallBuilderInterface->BuildSetCallForwarding(status, reason, service_class, toa, number, timeseconds);
}

ModemData *ProtocolCallBuilder::BuildGetCallForwardingStatus(SsStatusType status, SsCfReason reason, SsClassX service_class,
                                                             int toa, char *number, int timeseconds) {
    return protocolCallBuilderInterface->BuildGetCallForwardingStatus(status, reason, service_class, toa, number, timeseconds);
}

ModemData *ProtocolCallBuilder::BuildSetCallWaiting(SsModeType status, SsClassX service_class) {
    return protocolCallBuilderInterface->BuildSetCallWaiting(status, service_class);
}

ModemData *ProtocolCallBuilder::BuildGetCallWaiting(SsClassX service_class) {
    return protocolCallBuilderInterface->BuildGetCallWaiting(service_class);
}

ModemData *ProtocolCallBuilder::BuildChangeBarringPwd(const char *oldpasswd, const char *newpasswd, const char *newpasswd_again) {
    return protocolCallBuilderInterface->BuildChangeBarringPwd(oldpasswd, newpasswd, newpasswd_again);
}

ModemData *ProtocolCallBuilder::BuildSendUssd(const char *ussd, bool user_initiated) {
    return protocolCallBuilderInterface->BuildSendUssd(ussd, user_initiated);
}

ModemData *ProtocolCallBuilder::BuildCancelUssd() {
    return protocolCallBuilderInterface->BuildCancelUssd();
}

ModemData *ProtocolCallBuilder::BuildSeparateConnection(int callId) {
    return protocolCallBuilderInterface->BuildSeparateConnection(callId);
}

ModemData *ProtocolCallBuilder::BuildQueryColp() {
    return protocolCallBuilderInterface->BuildQueryColp();
}

ModemData *ProtocolCallBuilder::BuildQueryColr() {
    return protocolCallBuilderInterface->BuildQueryColr();
}

ModemData *ProtocolCallBuilder::BuildSendEncodedUssd(BYTE dcs, const char *encodedUssd, bool user_initiated) {
    return protocolCallBuilderInterface->BuildSendEncodedUssd(dcs, encodedUssd, user_initiated);
}

ModemData *ProtocolCallBuilder::BuildSendCallConfirm() {
    return protocolCallBuilderInterface->BuildSendCallConfirm();
}

ModemData *ProtocolCallBuilder::BuildExitEmergencyCbMode() {
    return protocolCallBuilderInterface->BuildExitEmergencyCbMode();
}

ProtocolCallBuilder::ProtocolCallBuilder()
    : ProtocolCallBuilderInterface() {
    if (IsPayloadTypeJson())
        protocolCallBuilderInterface = new ProtocolCallBuilderJson();
    else
        protocolCallBuilderInterface = new ProtocolCallBuilderLegacy();
}
ProtocolCallBuilder::~ProtocolCallBuilder() {
    delete protocolCallBuilderInterface;
    protocolCallBuilderInterface = NULL;
}
