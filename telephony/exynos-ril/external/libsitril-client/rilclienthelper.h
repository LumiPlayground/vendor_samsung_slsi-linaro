/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */
#ifndef __RIL_CLIENT_HELPER_H__
#define __RIL_CLIENT_HELPER_H__

#include "sitril-client.h"

#define HANDLE void *

class RilClientHelper {
public:
    RilClientHelper() {}
    virtual ~RilClientHelper() {}

public:
    // wrapper functions
    virtual void *Open()=0;
    virtual void *Open(const char *name)=0;
    virtual int Close(void* client)=0;
    virtual int Reconnect(void* client)=0;
    virtual int Send(void* client, unsigned msgId, void* data, size_t length, Rilc_OnResponse handler)=0;
    virtual int Send(void* client, unsigned msgId, void* data, size_t length, Rilc_OnResponse handler, unsigned int channel)=0;
    virtual int RegisterUnsolicitedResponseHandler(void* client, Rilc_OnUnsolicitedResponse handler)=0;

    /* for unit test */
    virtual int SetUnitTest(void* client, RILC_ReqSend handler)=0;

    // 1.1 compatible
    virtual int SetIndicationFilter(void* client, int *filters, size_t size)=0;

    static RilClientHelper *GetInstance();
};

#endif // __RIL_CLIENT_HELPER_H__
