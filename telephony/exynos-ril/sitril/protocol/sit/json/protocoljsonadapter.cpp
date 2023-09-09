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
 * protocoljsonadpater.cpp
 *
 *  Created on: 2021. 08. 09.
 *      Author: aryaman.2019
 */

#include "protocoljsonadapter.h"
#include "protocoljsonlogger.h"
#include "protocolutils.h"
#include "rillog.h"
#include <string>

using std::string;
using std::stringstream;

/**
 * ProtocolJsonRespAdapter
 */
ProtocolJsonRespAdapter::ProtocolJsonRespAdapter(const ModemData *pModemData, int sitId, bool isRxPayloadPresent) : ProtocolRespAdapter(pModemData) {
    m_jsonData = Json::nullValue;
    m_bParsingSuccess = false;
    ParseJsonPayload(sitId, isRxPayloadPresent);
}

bool ProtocolJsonRespAdapter::ParseJsonPayload(UINT id, bool isRxPayloadPresent) {
    if (isRxPayloadPresent == false) {
        return m_bParsingSuccess;
    } else {
        if (GetParameter() != NULL && GetId() == id) {
            stringstream sstream(GetParameter());
            Json::CharReaderBuilder jsonReader;
            string errs;
            if (!Json::parseFromStream(jsonReader, sstream, &m_jsonData, &errs)) {
                RilLogE("Error while parsing JSON data = (%s) for %s", errs.c_str(),
                        ProtocolUtils::rcmMsgToString(id));
                m_bParsingSuccess = false;
                return m_bParsingSuccess;
            }
        } else {
            RilLogE("Error in GetParameter() or GetId() for %s", ProtocolUtils::rcmMsgToString(id));
            m_bParsingSuccess = false;
            return m_bParsingSuccess;
        }
        ProtocolJsonLogger::LogJsonRespData(id, GetParameter());

        m_bParsingSuccess = true;
    }
    return m_bParsingSuccess;
}

/**
 * ProtocolJsonIndAdapter
 */
ProtocolJsonIndAdapter::ProtocolJsonIndAdapter(const ModemData *pModemData, int sitId, bool isRxPayloadPresent) : ProtocolIndAdapter(pModemData)  {
    m_jsonData = Json::nullValue;
    m_bParsingSuccess = false;
    ParseJsonPayload(sitId, isRxPayloadPresent);
}

bool ProtocolJsonIndAdapter::ParseJsonPayload(UINT id, bool isRxPayloadPresent) {
    if (isRxPayloadPresent == false) {
        return m_bParsingSuccess;
    } else {
        if (GetParameter() != NULL && GetId() == id) {
            stringstream sstream(GetParameter());
            Json::CharReaderBuilder jsonReader;
            string errs;
            if (!Json::parseFromStream(jsonReader, sstream, &m_jsonData, &errs)) {
                RilLogE("Error while parsing JSON data = (%s) for %s", errs.c_str(),
                        ProtocolUtils::rcmMsgToString(id));
                m_bParsingSuccess = false;
                return m_bParsingSuccess;
            }
        } else {
            RilLogE("Error in GetParameter() or GetId() for %s", ProtocolUtils::rcmMsgToString(id));
            m_bParsingSuccess = false;
            return m_bParsingSuccess;
        }
        ProtocolJsonLogger::LogJsonIndData(id, GetParameter());

        m_bParsingSuccess = true;
    }
    return m_bParsingSuccess;
}
