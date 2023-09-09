/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef __PsKeepaliveHandler_H__
#define __PsKeepaliveHandler_H__


class PsService;
class Message;

class PsKeepaliveHandler  // Maybe inherit Handler
{
protected:
    PsService *m_pPsService;

public:
    PsKeepaliveHandler();
    virtual ~PsKeepaliveHandler();
    virtual void Init(PsService* pPsService);
    //virtual int DoStartKeepalive(Message *pMsg);
    //virtual int DoStopKeepalive(Message *pMsg);
    //virtual int OnStartKeepaliveDone(Message *pMsg);
    //virtual int OnStopKeepaliveDone(Message *pMsg);
    //virtual int OnUnsolKeepaliveStatus(Message *pMsg);
private:
    bool isValidMessage(Message *pMsg);
    bool isValidRespMessage(Message *pMsg);
};
#endif // __PsKeepaliveHandler_H__
