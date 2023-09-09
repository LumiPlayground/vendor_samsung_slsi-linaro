/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef __PsSlicingConfigHandler_H__
#define __PsSlicingConfigHandler_H__


class PsService;
class Message;

class PsSlicingConfigHandler// Maybe inherit Handler
{
protected:
    PsService *m_pPsService;

public:
    PsSlicingConfigHandler();
    virtual ~PsSlicingConfigHandler();
    virtual void Init(PsService* pPsService);
    virtual int DoSlicingConfig(Message *pMsg);
    virtual int OnSlicingConfigDone(Message *pMsg);
    virtual bool isValidMessage(Message *pMsg);
    virtual bool isValidRespMessage(Message *pMsg);


private:
};
#endif //

