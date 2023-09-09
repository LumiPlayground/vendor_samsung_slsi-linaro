/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef __PsSetupDataCallHandler_H__
#define __PsSetupDataCallHandler_H__

#include "pdpcontextManager.h"

class Message;
class PsService;
class RilContext;

class PsSetupDataCallHandler : public PdpContextManager  // Maybe inherit Handler
{
protected:
    PsService *m_pPsService;

public:
    PsSetupDataCallHandler();
    virtual ~PsSetupDataCallHandler();
    virtual void Init(PsService* pPsService);
    virtual int DoSetupDataCall(Message *pMsg);
    virtual int DoSetFastDormancy(void);

private:
    bool isRoamState();
    RilContext *GetRilContext();
    bool isRejectRatForIMS(int rat);
};
#endif // __PsSetupDataCallHandler_H__
