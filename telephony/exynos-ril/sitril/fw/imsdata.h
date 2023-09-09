/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd All Rights Reserved
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
#pragma once

#include "requestdata.h"
#include <slsi/radio_v2_1.h>

class UpdateImsRegistrationInfoData : public RequestData
{
public:
    UpdateImsRegistrationInfoData(const int nReq, const Token tok, const ReqType type = REQ_FW);

    int GetRegState() { return m_regState; }
    int GetCapabilities() { return m_capabilities; }
    int GetAccessNetwork() { return m_accessNetwork; }
    int GetSuggestedAction() { return m_suggestedAction; }

    virtual int encode(char *data, unsigned int length);

private:
    int m_regState;
    int m_capabilities;
    int m_accessNetwork;
    int m_suggestedAction;
};

class StartImsTrafficData : public RequestData {
public:
    StartImsTrafficData(const int nReq, const Token tok, const ReqType type = REQ_FW);

    int GetToken() { return m_token; }
    int GetImsTrafficType() { return m_imsTrafficType; }
    int GetAccessNetwork() { return m_accessNetwork; }
    int GetDirection() { return m_direction; }

    virtual int encode(char *data, unsigned int length);

private:
    int m_token;
    int m_imsTrafficType;
    int m_accessNetwork;
    int m_direction;
};

class CreateTriggerEpsFallbackData : public RequestData {
public:
    CreateTriggerEpsFallbackData(const int nReq, const Token tok, const ReqType type = REQ_FW);

    int GetReason() { return m_reason; }

    virtual int encode(char *data, unsigned int length);

private:
    int m_reason;
};

class SrvccCallInfoData : public RequestData {
public:
    SrvccCallInfoData(const int nReq, const Token tok, const ReqType type = REQ_FW);

    INT32 encode(char *data, unsigned int length);

    int GetSrvccCallInfoNum();
    const RilImsV2_1::RIL_SrvccCall& at(int pos) const;
    const std::vector<RilImsV2_1::RIL_SrvccCall>& GetSrvccCallInfo() { return m_srvccCallInfos; }

protected:
    std::vector<RilImsV2_1::RIL_SrvccCall> m_srvccCallInfos;
};

class OemSrvccCallInfoData : public SrvccCallInfoData {
public:
    OemSrvccCallInfoData(const int nReq, const Token tok, const ReqType type = REQ_FW);

    INT32 encode(char *data, unsigned int length);
};