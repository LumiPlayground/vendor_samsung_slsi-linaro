/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#include "protocolembmsbuilderjson.h"
#include "protocolutils.h"
#include "rillog.h"
#include "embmsjsondef.h"

ModemData *ProtocolEmbmsBuilderJson::BuildSetService(int state) {
    Json::Value jsonReqData;
    jsonReqData[ns_embms_set_service_req::key_state] = state;

    return GetJsonReqModemData(jsonReqData, SIT_SET_EMBMS_SERVICE);
}

char *ProtocolEmbmsBuilderJson::getStringTypeTmgi(uint64_t tmgi) {
    memset(m_szTmgi, 0x00, sizeof(m_szTmgi));

    m_szTmgi[0] = ((tmgi >> 40) & 0xFF);
    m_szTmgi[1] = ((tmgi >> 32) & 0xFF);
    m_szTmgi[2] = ((tmgi >> 24) & 0xFF);
    m_szTmgi[3] = ((tmgi >> 16) & 0xFF);
    m_szTmgi[4] = ((tmgi >> 8) & 0xFF);
    m_szTmgi[5] = ((tmgi) & 0xFF);

    return m_szTmgi;
}

ModemData *ProtocolEmbmsBuilderJson::BuildSetSession(int state, uint64_t tmgi, int saiListLen,
        const uint32_t *pSaiList, int freqListLen, const uint32_t *pFreqList) {
    Json::Value jsonReqData;
    jsonReqData[ns_embms_set_session_req::key_state] = state;
    jsonReqData[ns_embms_set_session_req::key_tmgi] = Json::arrayValue;
    jsonReqData[ns_embms_set_session_req::key_sai_list] = Json::arrayValue;
    jsonReqData[ns_embms_set_session_req::key_freq_list] = Json::arrayValue;

    BYTE szTmgi[6];
    memcpy(szTmgi, getStringTypeTmgi(tmgi), sizeof(szTmgi));
    if (state == 0x02) {
        for (int i = 0; i < 6; i++) {
            jsonReqData[ns_embms_set_session_req::key_deact_tmgi].append((int) szTmgi[i]);
        }
    }
    for (int i = 0; i < 6; i++) {
        jsonReqData[ns_embms_set_session_req::key_tmgi].append((int) szTmgi[i]);
    }
    if (pSaiList != NULL) {
        ProtocolUtils::LimitLengthToMax<int>(RIL_LOG_CAT_EMBMS, __CLASS_FUNC__, saiListLen, MAX_SAI_LIST_LEN);
        for (int i = 0; i < saiListLen; i++) {
            jsonReqData[ns_embms_set_session_req::key_sai_list].append((uint16_t) pSaiList[i]);
        }
    }
    if (pFreqList != NULL) {
        ProtocolUtils::LimitLengthToMax<int>(RIL_LOG_CAT_EMBMS, __CLASS_FUNC__, freqListLen, MAX_FREQ_LIST_LEN);
        for (int i = 0; i < freqListLen; i++) {
            jsonReqData[ns_embms_set_session_req::key_freq_list].append(pFreqList[i]);
        }
    }
    return GetJsonReqModemData(jsonReqData, SIT_SET_EMBMS_SESSION);
}

ModemData *ProtocolEmbmsBuilderJson::BuildGetSessionList(int state) {
    Json::Value jsonReqData;
    jsonReqData[ns_embms_session_list_req::key_state] = state;

    return GetJsonReqModemData(jsonReqData, SIT_GET_EMBMS_SESSION_LIST);
}

ModemData *ProtocolEmbmsBuilderJson::BuildSignalStrength() {
    return GetJsonNullReqModemData(SIT_GET_EMBMS_SIGNAL_STRENGTH);
}

ModemData *ProtocolEmbmsBuilderJson::BuildNetworkTime() {
    return GetJsonNullReqModemData(SIT_GET_EMBMS_NETWORK_TIME);
}
