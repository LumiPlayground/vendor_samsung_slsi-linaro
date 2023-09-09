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
 * protocolvsimadapterinterface.h
 *
 *  Created on: 2021. 06. 18.
 */

#ifndef __PROTOCOL_VSIMADAPTER_INTERFACE_H__
#define __PROTOCOL_VSIMADAPTER_INTERFACE_H__

class ProtocolVsimOperationAdapterInterface {
public:
    ProtocolVsimOperationAdapterInterface() {}
    virtual ~ProtocolVsimOperationAdapterInterface() {}

public:
    virtual int GetTransactionId() const = 0;
    virtual int GetEventId() const = 0;
    virtual int GetResult() const = 0;
    virtual int GetOperationDataLength() = 0;
    virtual const char *GetOperationData() = 0;
};

#endif /* __PROTOCOL_VSIMADAPTER_INTERFACE_H__ */
