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
 * protocolvsimadapterjson.h
 *
 *  Created on: 2021. 08. 18.
 */

#ifndef __PROTOCOL_VSIMADAPTER_JSON_H__
#define __PROTOCOL_VSIMADAPTER_JSON_H__

#include "protocoljsonadapter.h"
#include "protocolvsimadapterinterface.h"

class ProtocolVsimOperationAdapterJson : public ProtocolVsimOperationAdapterInterface, public ProtocolJsonIndAdapter {
  private:
    char *m_pOperationData;  // null-terminated HEX string
    int m_nOperationDataLength;

  public:
    ProtocolVsimOperationAdapterJson(const ModemData *pModemData);
    ProtocolVsimOperationAdapterJson(const ProtocolVsimOperationAdapterJson&) = delete;
    ProtocolVsimOperationAdapterJson& operator=(ProtocolVsimOperationAdapterJson const&) = delete;
    virtual ~ProtocolVsimOperationAdapterJson() {
        if (m_pOperationData) {
            delete[] m_pOperationData;
            m_pOperationData = NULL;
        }
    }

  public:
    int GetTransactionId() const;
    int GetEventId() const;
    int GetResult() const;
    int GetOperationDataLength() {
        return m_nOperationDataLength;
    }
    const char *GetOperationData() {
        return m_pOperationData;
    }

  private:
    void SetOperationData();
    void Init();
};

#endif /* __PROTOCOL_VSIMADAPTER_LEGACY_H__ */
