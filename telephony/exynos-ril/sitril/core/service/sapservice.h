/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */
#ifndef __SAP_SERVICE_H__
#define __SAP_SERVICE_H__

#include "service.h"

class SapService : public Service {
public:
    SapService(RilContext* pRilContext);
    virtual ~SapService();

protected:
    virtual int OnCreate(RilContext *pRilContext);
    virtual void OnDestroy();

    virtual bool IsPossibleToPassInRadioOffState(int request_id);
    virtual bool IsPossibleToPassInRadioUnavailableState(int request_id);
};

#endif /* __SAP_SERVICE_H__ */
