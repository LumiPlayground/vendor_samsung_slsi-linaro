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
 * protocolvsimadapterjson.cpp
 *
 *  Created on: 2021. 08. 18.
 */

#include "protocolvsimadapterjson.h"
#include "vsimprotocolutils.h"
#include "util.h"
#include "vsimjsondef.h"

/**
 * ProtocolVsimOperationAdapterJson
 */
ProtocolVsimOperationAdapterJson::ProtocolVsimOperationAdapterJson(const ModemData *pModemData)
    : ProtocolJsonIndAdapter(pModemData) {
    Init();
}

void ProtocolVsimOperationAdapterJson::Init() {
    ParseJsonPayload(SIT_IND_VSIM_OPERATION);
    m_pOperationData = NULL;
    m_nOperationDataLength = 0;
    SetOperationData();
}

void ProtocolVsimOperationAdapterJson::SetOperationData() {
    if (!m_bParsingSuccess) {
        return;
    }
    // operation data len in value
    int nOperationDataLen = m_jsonData[ns_vsim_opertaion_ind::key_operation_data].size();
    // operation data len in hex string
    m_nOperationDataLength = (nOperationDataLen * 2) + 1;
    BYTE tmpOperationData[nOperationDataLen];
    for(int i = 0; i < nOperationDataLen; i++) {
        tmpOperationData[i] = (BYTE) m_jsonData[ns_vsim_opertaion_ind::key_operation_data][i].asInt();
    }
    m_pOperationData = new char[m_nOperationDataLength];
    if (m_pOperationData != NULL) {
        memset(m_pOperationData, 0, m_nOperationDataLength);
        int ret = Value2HexString(m_pOperationData, tmpOperationData, nOperationDataLen);
        if (ret < 0) {
            delete[] m_pOperationData;
            m_pOperationData = NULL;
            m_nOperationDataLength = 0;
        }
    }
}

int ProtocolVsimOperationAdapterJson::GetTransactionId() const {
    int nTransactionId = -1;
    if (m_bParsingSuccess) {
        nTransactionId = m_jsonData[ns_vsim_opertaion_ind::key_tid].asInt();
    }
    return nTransactionId;
}

int ProtocolVsimOperationAdapterJson::GetEventId() const {
    int nEventId = -1;
    if (m_bParsingSuccess) {
        int nSitEventId = m_jsonData[ns_vsim_opertaion_ind::key_event_type].asInt();
        nEventId = VsimProtocolUtils::ConvertVsimOpSitEventIdToRilEventId(nSitEventId);
    }
    return nEventId;
}

int ProtocolVsimOperationAdapterJson::GetResult() const {
    int nResult = -1;
    if (m_bParsingSuccess) {
        nResult = m_jsonData[ns_vsim_opertaion_ind::key_result].asInt();
    }
    return nResult;
}
