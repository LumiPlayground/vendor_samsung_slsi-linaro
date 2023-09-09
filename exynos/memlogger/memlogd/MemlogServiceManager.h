/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef __MEMLOG_SERVICE_MANAGER_H__
#define __MEMLOG_SERVICE_MANAGER_H__

#include "memlogservice.h"

class MemlogServiceManager {
private:
    HANDLE mHandle;
    void *mLib;
    Memlog_ServiceFunctions mFunc;
    Memlog_RegisterService m_pfnRegisterService;
    Memlog_ReleaseService m_pFnReleaseService;
    Memlog_NotifyCallback m_pFnNotifyCallback;

public:
    MemlogServiceManager();
    virtual ~MemlogServiceManager();
public:
    bool init();
    void release();
    int registerService(const char *serviceName, Memlog_ServiceFunctions *func);
    void releaseService();
    int notifyCallback(int type, int id, void *data, unsigned int datalen);
};

#endif // __MEMLOG_SERVICE_MANAGER_H__
