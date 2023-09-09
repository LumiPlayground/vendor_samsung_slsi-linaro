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
 * protocolvsimadapterlegacy.h
 *
 *  Created on: 2016. 02. 26.
 */

#ifndef __PROTOCOL_VSIMADAPTER_LEGACY_H__
#define __PROTOCOL_VSIMADAPTER_LEGACY_H__

#include "protocoladapter.h"
#include "protocolvsimadapterinterface.h"

class ProtocolVsimOperationAdapterLegacy : public ProtocolVsimOperationAdapterInterface, public ProtocolIndAdapter {
  private:
    char *mOperationData;  // null-terminated HEX string
    int mOperationDataLength;

  public:
    ProtocolVsimOperationAdapterLegacy(const ModemData *pModemData);
    ProtocolVsimOperationAdapterLegacy(const ProtocolVsimOperationAdapterLegacy&) = delete;
    ProtocolVsimOperationAdapterLegacy& operator=(ProtocolVsimOperationAdapterLegacy const&) = delete;
    virtual ~ProtocolVsimOperationAdapterLegacy();

  public:
    int GetTransactionId() const;
    int GetEventId() const;
    int GetResult() const;
    int GetOperationDataLength() {
        return mOperationDataLength;
    }
    const char *GetOperationData() {
        return mOperationData;
    }
};

#endif /* __PROTOCOL_VSIMADAPTER_LEGACY_H__ */
