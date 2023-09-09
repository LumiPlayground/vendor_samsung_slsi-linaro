/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef __PsIpv6Handler_H__
#define __PsIpv6Handler_H__

#include <ctime>
#include <map>

class Message;
class PdpContext;
class PsService;
class PsUtil;

class PsIpv6Handler  // Maybe inherit Handler with better abstraction
{
protected:
    PsService *m_pPsService;
    PsUtil *m_pUtil;

    std::map<int, timer_t> map_timer_id; // cid to timer_id
    std::function<void(int)> m_pCallback;

public:
    PsIpv6Handler();
    virtual ~PsIpv6Handler();
    virtual void Init(PsService* pPsService);
    void SetRSRATimeoutTimer(int cid, const char *ifname, std::function<void(int)> cb_func);
    void RSRATimeoutCallback(int sigNo, siginfo_t *evp, int cid);
    virtual int OnSetupDataCallIPv6Configured(Message *pMsg, const int halVer);

    static void TimerHandler_wrapper(int sigNo, siginfo_t *evp, void *uc);
    bool DeferToGenerateIpv6GlobalAddress(PdpContext *, int&);

    // optional functions for test, future uses, etc...
private:
    const bool isDeferToGenerateIpv6GlobalAddress = false;
};
#endif // __PsIpv6Handler_H__
