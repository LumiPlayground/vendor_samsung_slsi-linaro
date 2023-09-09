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

#include "imsdata.h"
#include "rillog.h"
#include <librilutils/textutils.h>
#include <telephony/ril.h>

#undef RilLogI
#define RilLogI(format, ...) CRilLog::Log(RIL_LOG_CAT_IMS, CRilLog::E_RIL_INFO_LOG,  format, ##__VA_ARGS__)
#undef RilLogV
#define RilLogV(format, ...) CRilLog::Log(RIL_LOG_CAT_IMS, CRilLog::E_RIL_VERBOSE_LOG,  format, ##__VA_ARGS__)
#undef RilLogW
#define RilLogW(format, ...) CRilLog::Log(RIL_LOG_CAT_IMS, CRilLog::E_RIL_WARNING_LOG,  format, ##__VA_ARGS__)
#undef RilLogE
#define RilLogE(format, ...) CRilLog::Log(RIL_LOG_CAT_IMS, CRilLog::E_RIL_CRITICAL_LOG,  format, ##__VA_ARGS__)

/**
 * Update Ims Registration Info
 */
UpdateImsRegistrationInfoData::UpdateImsRegistrationInfoData(const int nReq, const Token tok, const ReqType type) : RequestData(nReq, tok, type)
{
    m_regState = -1;
    m_capabilities = -1;
    m_accessNetwork = -1;
    m_suggestedAction = -1;
}

int UpdateImsRegistrationInfoData::encode(char *data, unsigned int length)
{
    if (data == nullptr) {
        RilLogV("Unexpected: data is null");
        return -1;
    }

    if (length != sizeof(RilImsV2_1::RIL_UpdateImsRegistration)) {
        RilLogV("Unexpected: incorrect data length");
        return -1;
    }

    RilImsV2_1::RIL_UpdateImsRegistration *pImsRegInfo =
        reinterpret_cast<RilImsV2_1::RIL_UpdateImsRegistration *>(data);

    m_regState = static_cast<int32_t>(pImsRegInfo->regState);
    m_capabilities = pImsRegInfo->capabilities;
    m_accessNetwork = static_cast<int32_t>(pImsRegInfo->accessNetworkType);
    m_suggestedAction = static_cast<int32_t>(pImsRegInfo->suggestedAction);

    return 0;
}

/**
 * Start Ims Traffic
 */
StartImsTrafficData::StartImsTrafficData(const int nReq, const Token tok, const ReqType type) : RequestData(nReq, tok, type) {
    m_token = TOKEN_INVALID;
    m_imsTrafficType = -1;
    m_accessNetwork = -1;
    m_direction = -1;
}

int StartImsTrafficData::encode(char *data, unsigned int length) {
    if (data == nullptr) {
        RilLogE("Unexpected: data is null");
        return -1;
    }

    if (length != sizeof(RilImsV2_1::RIL_StartImsTraffic)) {
        RilLogE("Unexpected: incorrect data length");
        return -1;
    }

    RilImsV2_1::RIL_StartImsTraffic *pStartImsTraffic =
        reinterpret_cast<RilImsV2_1::RIL_StartImsTraffic *>(data);

    m_token = pStartImsTraffic->token;
    m_imsTrafficType = static_cast<int32_t>(pStartImsTraffic->imsTrafficType);
    m_accessNetwork = static_cast<int32_t>(pStartImsTraffic->accessNetworks);
    m_direction = static_cast<int32_t>(pStartImsTraffic->direction);
    return 0;
}

/**
 * Trigger Eps Fallback
 */
CreateTriggerEpsFallbackData::CreateTriggerEpsFallbackData(const int nReq, const Token tok, const ReqType type) : RequestData(nReq, tok, type) {
    m_reason = -1;
}

int CreateTriggerEpsFallbackData::encode(char *data, unsigned int length) {
    if (data == nullptr) {
        RilLogE("Unexpected: data is null");
        return -1;
    }

    if (length < 1) {
        RilLogE("Unexpected: incorrect data length");
        return -1;
    }

    m_reason = static_cast<int>(*(data));
    return 0;
}

/**
 * Set Srvcc Call Info
 */
SrvccCallInfoData::SrvccCallInfoData(const int nReq, const Token tok, const ReqType type) : RequestData(nReq, tok, type) {
    m_srvccCallInfos.clear();
}

INT32 SrvccCallInfoData::encode(char *data, unsigned int length) {
    if (data == nullptr) {
        RilLogE("Unexpected: data is null");
        return -1;
    }

    if (length % sizeof(RilImsV2_1::RIL_SrvccCall) != 0) {
        RilLogE("Unexpected: incorrect data length");
        return -1;
    }

    size_t size = length / sizeof(RilImsV2_1::RIL_SrvccCall);
    RilImsV2_1::RIL_SrvccCall *infoPtrs =
        reinterpret_cast<RilImsV2_1::RIL_SrvccCall *>(data);
    for (size_t i = 0; i < size; i++) {
        m_srvccCallInfos.push_back(infoPtrs[i]);
    }

    return 0;
}

int SrvccCallInfoData::GetSrvccCallInfoNum()
{
    return m_srvccCallInfos.size();
}

const RilImsV2_1::RIL_SrvccCall& SrvccCallInfoData::at(int pos) const
{
    return m_srvccCallInfos.at(pos);
}

/**
 * Oem Set Srvcc Call Info
 */
OemSrvccCallInfoData::OemSrvccCallInfoData(const int nReq, const Token tok, const ReqType type) : SrvccCallInfoData(nReq, tok, type) {
    m_srvccCallInfos.clear();
}

INT32 OemSrvccCallInfoData::encode(char *data, unsigned int length) {
    if (data == nullptr) {
        RilLogE("Unexpected: data is null");
        return -1;
    }

    if ((length - sizeof(RilImsV2_1::RIL_SrvccCallListOem::callListNum)) % sizeof(RilImsV2_1::RIL_OemSrvccCall) != 0) {
        RilLogE("Unexpected: incorrect data length");
        return -1;
    }

    RilImsV2_1::RIL_SrvccCallListOem info =
        *(reinterpret_cast<RilImsV2_1::RIL_SrvccCallListOem *>(data));
    for (size_t i = 0; i < static_cast<size_t>(info.callListNum); i++) {
        RilImsV2_1::RIL_SrvccCall to = {};
        RilImsV2_1::RIL_OemSrvccCall from =
            static_cast<RilImsV2_1::RIL_OemSrvccCall>(info.callList[i]);
        to.index = from.index;
        to.callType = static_cast<RilImsV2_1::RIL_CallType>(from.callType);
        to.callState = static_cast<int32_t>(from.callState);
        to.callSubstate = static_cast<RilImsV2_1::RIL_CallSubState>(from.callSubstate);
        to.ringbackToneType = static_cast<RilImsV2_1::RIL_ToneType>(from.ringbackToneType);
        to.isMpty = from.isMpty;
        to.isMT = from.isMT;
        to.number = std::string(from.number);
        to.numPresentation = from.numPresentation;
        to.name = std::string(from.name);
        to.namePresentation = from.namePresentation;

        m_srvccCallInfos.push_back(to);
    }

    return 0;
}

