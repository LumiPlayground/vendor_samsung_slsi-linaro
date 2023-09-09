/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef __MOCK_PS_SERVICE_PROXY_H__
#define __MOCK_PS_SERVICE_PROXY_H__

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <psserviceproxy.h>

class MockPsServiceProxy : public PsServiceProxy {
    public:
        MockPsServiceProxy(Service *service) : PsServiceProxy(service) {}
        ~MockPsServiceProxy() {
        }
};

#endif /*__MOCK_PS_SERVICE_PROXY_H__*/
