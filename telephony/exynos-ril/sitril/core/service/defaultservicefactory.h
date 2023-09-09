/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef    __DEFAULT_SERVICE_FACTORY_H__
#define    __DEFAULT_SERVICE_FACTORY_H__

#include "servicefactory.h"

/**
 * DefaultServiceFactory
 */
class DefaultServiceFactory : public ServiceFactory
{
public:
    DefaultServiceFactory() : ServiceFactory() {}
    virtual ~DefaultServiceFactory() {}
    virtual Service *CreateService(RilContext *pRilContext, UINT nServiceId);
public:
    virtual void OnInitialize();
};

#endif    // __DEFAULT_SERVICE_FACTORY_H__

