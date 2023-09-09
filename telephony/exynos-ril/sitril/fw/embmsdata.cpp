/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#include "embmsdata.h"
#include "rillog.h"

// add category to display selective logs
#undef RilLogI
#define RilLogI(format, ...) CRilLog::Log(RIL_LOG_CAT_EMBMS, CRilLog::E_RIL_INFO_LOG,  format, ##__VA_ARGS__)
#undef RilLogV
#define RilLogV(format, ...) CRilLog::Log(RIL_LOG_CAT_EMBMS, CRilLog::E_RIL_VERBOSE_LOG,  format, ##__VA_ARGS__)
#undef RilLogW
#define RilLogW(format, ...) CRilLog::Log(RIL_LOG_CAT_EMBMS, CRilLog::E_RIL_WARNING_LOG,  format, ##__VA_ARGS__)
#undef RilLogE
#define RilLogE(format, ...) CRilLog::Log(RIL_LOG_CAT_EMBMS, CRilLog::E_RIL_CRITICAL_LOG,  format, ##__VA_ARGS__)

/**
 * eMBMS Session Data
 */
EmbmsSessionData::EmbmsSessionData(const int nReq, const Token tok, const ReqType type) : RequestData(nReq, tok, type)
{
    m_State = 0;
    m_Priority = 0;
    m_TMGI = 0;
    memset(&m_InfoBind, 0, sizeof(RIL_InfoBinding));
    m_InfoBindCount = 0;
}

EmbmsSessionData::~EmbmsSessionData()
{
}

INT32 EmbmsSessionData::encode(char *data, unsigned int length)
{
    if ((sizeof(RIL_EmbmsSessionData) != length) || (NULL == data)) {
        RilLogE("EmbmsSessionData::%s() Invalid length (%d)", __FUNCTION__,
                length);
        return -1;
    }

    RIL_EmbmsSessionData *pEmbmsSessionData = (RIL_EmbmsSessionData *)data;

    m_State = pEmbmsSessionData->state;
    m_Priority = pEmbmsSessionData->priority;
    m_TMGI = pEmbmsSessionData->tmgi;
    m_InfoBindCount = pEmbmsSessionData->infobindcount;

    if (0 < m_InfoBindCount) {
        m_InfoBind.uSAICount =
                pEmbmsSessionData->infobind.uSAICount < EMBMS_MAX_INTRA_SAILIST_NUMBER ?
                pEmbmsSessionData->infobind.uSAICount : EMBMS_MAX_INTRA_SAILIST_NUMBER;
        for (int i = 0; i < (int)m_InfoBind.uSAICount;i++) {
            m_InfoBind.nSAIList[i] = pEmbmsSessionData->infobind.nSAIList[i];
        }
        m_InfoBind.uFreqCount =
                pEmbmsSessionData->infobind.uFreqCount < EMBMS_MAX_FREQLIST_NUMBER ?
                pEmbmsSessionData->infobind.uFreqCount : EMBMS_MAX_FREQLIST_NUMBER;
        for (int i = 0; i < (int)m_InfoBind.uFreqCount;i++) {
            m_InfoBind.nFreqList[i] = pEmbmsSessionData->infobind.nFreqList[i];
        }
    }

    RilLogV("EmbmsSessionData::%s() tmgi(%012llX), infobindcount(%d)",
            __FUNCTION__, m_TMGI, m_InfoBindCount);
    return 0;
}

