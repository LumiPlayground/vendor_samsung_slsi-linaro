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
 * protocoljsonadapter.h
 *
 *  Created on: 2021. 08. 09.
 *      Author: aryaman.2019
 */

#pragma once

#include "protocoladapter.h"
#include "json/json.h"

class ProtocolJsonRespAdapter : public ProtocolRespAdapter {
protected:
    Json::Value m_jsonData;
    bool m_bParsingSuccess;
public:
    ProtocolJsonRespAdapter(const ModemData *pModemData) : ProtocolRespAdapter(pModemData) {
        m_jsonData = Json::nullValue;
        m_bParsingSuccess = false;
    }
    ProtocolJsonRespAdapter(const ModemData *pModemData, int sitId, bool isRxPayloadPresent = true);
    virtual ~ProtocolJsonRespAdapter() = default;

    bool ParseJsonPayload(UINT id, bool isRxPayloadPresent = true);
};

class ProtocolJsonIndAdapter : public ProtocolIndAdapter {
protected:
    Json::Value m_jsonData;
    bool m_bParsingSuccess;
public:
    ProtocolJsonIndAdapter(const ModemData *pModemData) : ProtocolIndAdapter(pModemData) {
        m_jsonData = Json::nullValue;
        m_bParsingSuccess = false;
    }
    ProtocolJsonIndAdapter(const ModemData *pModemData, int sitId, bool isRxPayloadPresent = true);
    virtual ~ProtocolJsonIndAdapter() = default;

    bool ParseJsonPayload(UINT id, bool isRxPayloadPresent = true);
};
