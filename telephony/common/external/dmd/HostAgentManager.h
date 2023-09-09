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
 * HostAgent.h
 *
 *  Created on: 2020. 2. 12.
 */

#ifndef __HOST_AGENT_H__
#define __HOST_AGENT_H__

#include <string>
#include <list>

class HostAgentManager;
class ModemProxy;

class HostAgent {
protected:
    std::string mName;
    HostAgentManager *mManager;
public:
    HostAgent(HostAgentManager *manager) : mManager(manager) {}
    virtual ~HostAgent() {}
    std::string getHostName() const { return mName; }
    virtual int Write(const void *data, size_t datalen)=0;
};

class HostAgentManager {
private:
    HostAgent *mActiveAgent;
    std::list<HostAgent *> mHostAgents;
    ModemProxy *mModemProxy;
    static HostAgentManager *instance;
public:
    HostAgentManager();
    void RegisterAgent(HostAgent *agent);
    void SetActiveAgent(HostAgent *agent);
    HostAgent *GetActiveAgent() { return mActiveAgent; }
    void SetModemProxy(ModemProxy *modemProxy) { mModemProxy = modemProxy; }
    ModemProxy *GetModemProxy() { return mModemProxy; }
    int SendToModem(const void *data, size_t datalen);

public:
    static HostAgentManager *GetInstance();
    static void Init();
};

#endif /* __HOST_AGENT_H__ */
