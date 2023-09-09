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
 * protocolvsimadapter.h
 *
 *  Created on: 2021. 06. 18.
 */

#ifndef __PROTOCOL_VSIMADAPTER_H__
#define __PROTOCOL_VSIMADAPTER_H__

#include "protocolvsimadapterinterface.h"
#include <base/types.h>

class ModemData;

class ProtocolVsimOperationAdapter : public ProtocolVsimOperationAdapterInterface {
private:
    ProtocolVsimOperationAdapterInterface *protocolVsimOperationAdapterInterface = NULL;

public:
    ProtocolVsimOperationAdapter(const ModemData *pModemData);
    ProtocolVsimOperationAdapter(const ProtocolVsimOperationAdapter &) = delete;
    ProtocolVsimOperationAdapter& operator=(ProtocolVsimOperationAdapter const&) = delete;
    virtual ~ProtocolVsimOperationAdapter();

public:
    int GetTransactionId() const;
    int GetEventId() const;
    int GetResult() const;
    int GetOperationDataLength();
    const char *GetOperationData();
};

#endif /* __PROTOCOL_VSIMADAPTER_H__ */
