/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#include <gtest/gtest.h>
//#include <gmock/gmock.h>

// Ril Service Layer
#include <telephony/ril.h>
#include "MockRilContext.h"
#include "MockRilApplication.h"

// PsService Layer
#include "MockPsService.h"

// PsServceHandler Layer
#include "MockPsServiceProxy.h"
#include <psserviceproxy.h>
#include <pshandler.h>

#include <../RilDataTests/pstestutil.h>

// For Mocking, no connection between Radio HAL Layer - RIL Service
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::_;
using ::testing::Invoke;
using ::testing::StrictMock;

class PsServiceTest : public ::testing::Test {
    protected:
        static void SetUpTestSuite() {
            context = new MockRilContext();
            service = new MockPsService(context);
            psProxy = new MockPsServiceProxy(service);

            context->OnCreate();
            service->OnCreate(context);
            service->Init();
        }
        static void TearDownTestSuite() {
            if (psProxy) delete psProxy;
            if (service) delete service;
            if (context) delete context;
        }

        // Per TEST CASE
        void SetUp() override {
        }
        void TearDown() override {
        }
        const char *err_str = NULL;

        // Internal cross-cut of VendorRIL Library
    public:
        static MockPsService *service;
        static MockRilContext *context;
        static MockPsServiceProxy *psProxy;
};

MockPsService* PsServiceTest::service = nullptr;
MockRilContext* PsServiceTest::context = nullptr;
MockPsServiceProxy* PsServiceTest::psProxy = nullptr;

TEST_F(PsServiceTest, PsServiceInstanceTest) {
    // DummyTest
    EXPECT_EQ(1, 1);
}
