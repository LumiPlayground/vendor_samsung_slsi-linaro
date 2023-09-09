/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef __MOCK_PS_SERVICE_H__
#define __MOCK_PS_SERVICE_H__

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <message.h>
#include <psservice.h>
#include <pdpcontext.h>

class MockPsService : public PsService {
    public:
        MockPsService(RilContext* pRilContext) : PsService(pRilContext) {}
        ~MockPsService() {
        }
        int OnCreate(RilContext *pRilContext) {
            return PsService::OnCreate(pRilContext);
        }
        int Init() {
            return Service::Init();
        }
};

#endif /*__MOCK_PS_SERVICE_H__*/
