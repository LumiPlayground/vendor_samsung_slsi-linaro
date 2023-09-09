/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef __PsInitialAttachApnHandler_H__
#define __PsInitialAttachApnHandler_H__

#include <slsi/radio_v2_0.h>
#include <map>

class ApnSetting;
class PsService;
class Message;
class PdpContext;
class PsTrafficDescriptor;

class PsInitialAttachApnHandler  // Maybe inherit Handler
{
protected:
    PsService *m_pPsService;

public:
    PsInitialAttachApnHandler();
    virtual ~PsInitialAttachApnHandler();
    virtual void Init(PsService* pPsService);
    virtual int DoRefreshInitialAttachApn(Message *pMsg);
    int RequestSetInitialAttachApn(const ApnSetting *pApnSetting, int dataProfileId, const std::optional<DataV2_0::RIL_TrafficDescriptor> &tdInfo);
    PdpContext *ConfigureAttachPdpContext(ApnSetting *pApnSetting, const std::optional<DataV2_0::RIL_TrafficDescriptor> &tdInfo);
    PsTrafficDescriptor *fillTDInfo(const DataV2_0::RIL_TrafficDescriptor &tdInfo);
protected:
    template<typename T>
    bool GetRilRequestData(Message *pMsg, T *(&rildata));
    virtual std::string GetAttachApnType();
private:
    inline bool IsNullMsg(Message *pMsg);

    std::map<std::string, std::string> m_AttachApnTypeTable;
};
#endif // __PsInitialAttachApnHandler_H__
