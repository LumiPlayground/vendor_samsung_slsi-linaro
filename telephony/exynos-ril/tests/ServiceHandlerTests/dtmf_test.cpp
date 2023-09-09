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
 * dtmf_test.cpp
 *
 *  Created on: 2021. 10. 19.
 */
#include <gtest/gtest.h>
#include "misc/dtmf.h"
#include <modemdatabuilder.h>
#include "messageid.h"
#include "rilparser.h"
#include "service.h"
#include "sitdef.h"
#include "tokengen.h"

TEST(Servicehandler, DtmfHandler_OnRequest) {
    TokenGen::Init();

    static const char *testVector = "123";
    struct LocalService : public Service {
        bool called = false;
        LocalService() : Service(0) {}

        int SendRequest(ModemData *pModemData, UINT timeout, UINT nResult,
                Message */*pMsg = NULL*/) {
            called = true;
            std::unique_ptr<ModemData> modemData = std::unique_ptr<ModemData>(pModemData);
            EXPECT_NE(modemData, nullptr);
            EXPECT_EQ(timeout, 15000UL);
            EXPECT_EQ(nResult, MSG_MISC_DTMF);
            if (modemData) {
                sit_call_dtmf_req *data = (sit_call_dtmf_req *)modemData->GetRawData();
                EXPECT_EQ(data->dtmf_len, strlen(testVector));
                EXPECT_TRUE(memcmp(data->dtmf_digit, testVector, data->dtmf_len) == 0);
            }
            return 0;
        }
    } localService;

    std::unique_ptr<Message> message = std::unique_ptr<Message>(
            Message::ObtainMessage(
                    RilParser().GetRequestData(RIL_REQUEST_DTMF, 0,
                    (char*)testVector, 3), 0, MSG_MISC_DTMF));
    if (message) {
        std::unique_ptr<DtmfHandler> test =
                std::unique_ptr<DtmfHandler>(DtmfHandler::Create(&localService));
        if (test) {
            EXPECT_TRUE(test->OnHandleRequest(message.get()));
        }
    }
    EXPECT_TRUE(localService.called);
}

TEST(Servicehandler, DtmfHandler_OnRequest_invalid) {
    TokenGen::Init();

    static const char *testVector = "123";
    struct LocalService : public Service {
        bool called = false;
        LocalService() : Service(0) {}

        int SendRequest(ModemData *pModemData, UINT timeout, UINT nResult,
                Message */*pMsg = NULL*/) {
            called = true;
            EXPECT_EQ(timeout, 15000UL);
            EXPECT_EQ(nResult, MSG_MISC_DTMF);
            std::unique_ptr<ModemData> modemData = std::unique_ptr<ModemData>(pModemData);
            return -1;
        }
    } localService;

    std::unique_ptr<Message> message = std::unique_ptr<Message>(
            Message::ObtainMessage(
                    RilParser().GetRequestData(RIL_REQUEST_DTMF, 0,
                    (char*)testVector, 3), 0, MSG_MISC_DTMF));
    if (message) {
        std::unique_ptr<DtmfHandler> test =
                std::unique_ptr<DtmfHandler>(DtmfHandler::Create(&localService));
        if (test) {
            EXPECT_FALSE(test->OnHandleRequest(message.get()));
        }
    }
    EXPECT_TRUE(localService.called);
}

TEST(Servicehandler, DtmfHandler_OnResponse) {
    struct LocalService : public Service {
        bool called = false;
        LocalService() : Service(0) {}
        int OnRequestComplete(int result, void */*data = NULL*/, int/* length = 0*/,
                Message */*pMsg = NULL*/) {
            called = true;
            EXPECT_EQ(result, RIL_E_SUCCESS);
            return 0;
        }
    } localService;

    std::unique_ptr<Message> message = std::unique_ptr<Message>(
            Message::ObtainMessage(ModemDataBuilder::BuildResponse(SIT_DTMF, RCM_E_SUCCESS),
                    0, MSG_MISC_DTMF));
    if (message) {
        std::unique_ptr<DtmfHandler> test =
                std::unique_ptr<DtmfHandler>(DtmfHandler::Create(&localService));
        if (test) {
            EXPECT_TRUE(test->OnHandleSolicitedResponse(message.get()));
        }
    }
    EXPECT_TRUE(localService.called);
}
