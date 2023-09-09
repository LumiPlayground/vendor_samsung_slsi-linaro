/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd All Rights Reserved
 * PROPRIETARY/CONFIDENTIAL
 *
 * This software is the confidential and proprietary information of
 * SAMSUNG ELECTRONICS ("Confidential Information").
 *
 * You shall not disclose such Confidential Information and shall use it
 * only in accordance with the terms of the license agreement
 * you entered into with SAMSUNG ELECTRONICS.
 *
 * SAMSUNG make no representations or warranties about the suitability of
 * the software, either express or implied, including but not limited to
 * the implied warranties of merchantability, fitness for a particular
 * purpose, or non-infringement. SAMSUNG shall not be liable for any
 * damages suffered by license as a result of using, modifying or
 * distributing this software or its derivatives.
 */


#include "sitrilTestImsMedia.h"
#include "testUtil.h"
#include <string>

static void cleanup(SitrilTest *psitRilTest, UINT8* rspData, ModemData *pModemData = nullptr, UINT32* repDataLen = nullptr) {
    if (pModemData) { delete pModemData; }
    if (rspData) { free(rspData); }
    if (repDataLen) { *repDataLen = 0; }
    psitRilTest->resetTestData();
}

void SitrilTestImsMedia::SetUp() {
    mPhoneIndex = GetParam();
    mSitRilTest = SitrilTest::CreateInstance();
    mSitRilTest->resetTestData();

    mpModemData = nullptr;
    mRspData = nullptr;
    mRspDataLen = 0;
}

void SitrilTestImsMedia::TearDown() {
    mSitRilTest = nullptr;

    if (mpModemData) { delete mpModemData; mpModemData = nullptr; }
    if (mRspData) { free(mRspData); mRspData = nullptr; }
    mRspDataLen = 0;
}

TEST_P(SitrilTestImsMedia, oemSolicitedTestAllList) {
    const static int messageMap[][2] = {

            {RIL_REQUEST_OEM_IMS_TEST_RCM,                          SIT_IMS_MEDIA_TEST_RCM},
            {RIL_REQUEST_OEM_IMS_MEDIA_INIT,                        SIT_IMS_MEDIA_INIT},
            {RIL_REQUEST_OEM_IMS_MEDIA_OPEN_SESSION,                SIT_IMS_MEDIA_OPEN_SESSION},
            {RIL_REQUEST_OEM_IMS_MEDIA_CLOSE_SESSION,               SIT_IMS_MEDIA_CLOSE_SESSION},
            {RIL_REQUEST_OEM_IMS_MEDIA_MODIFY_SESSION,              SIT_IMS_MEDIA_MODIFY_SESSION},
            {RIL_REQUEST_OEM_IMS_MEDIA_SEND_DTMF,                   SIT_IMS_MEDIA_SEND_DTMF},
            {RIL_REQUEST_OEM_IMS_MEDIA_START_DTMF,                  SIT_IMS_MEDIA_START_DTMF},
            {RIL_REQUEST_OEM_IMS_MEDIA_STOP_DTMF,                   SIT_IMS_MEDIA_STOP_DTMF},
            {RIL_REQUEST_OEM_IMS_MEDIA_SEND_TEXT,                   SIT_IMS_MEDIA_SEND_TEXT},
            {RIL_REQUEST_OEM_IMS_MEDIA_SEND_HEADER_EXTENSION,       SIT_IMS_MEDIA_SEND_HEADER_EXTENSION},
            {RIL_REQUEST_OEM_IMS_MEDIA_SET_MEDIA_QUALITY_THRESHOLD, SIT_IMS_MEDIA_SET_MEDIA_QUALITY_THRESHOLD},
            {RIL_REQUEST_OEM_IMS_MEDIA_GET_OPERATION_MODE,          SIT_IMS_GET_OPERATION_MODE},
            {RIL_REQUEST_OEM_IMS_SIP_MESSAGE,                       SIT_IMS_SIP_MESSAGE},
            {RIL_REQUEST_OEM_IMS_MEDIA_WIFI_CONFIG,                 SIT_IMS_WIFI_CONFIG_REQ},
            {RIL_REQUEST_OEM_IMS_MEDIA_QUERY_MAC_CE_INFO,           SIT_QUERY_MAC_CE_INFO},

            };

    for (auto testElement : messageMap) {
        int requestMessage = testElement[0];
        int sitMessage = testElement[1];
        SitrilTest *psitRilTest = getSitrilTest();
        ASSERT_NE(nullptr, psitRilTest);
        const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
        ASSERT_NE(nullptr, prilFuncs);

        int testRilCmdId = requestMessage;
        UINT16 sitId = sitMessage;
        UINT sitToken = TOKEN_INVALID;

        int rilVer = prilFuncs->version;
        EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

        psitRilTest->processExternReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

        ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

        UINT32 sizeOfModemData = sizeof(null_data_format);
        UINT32 txIpcRawDataLen = 0;
        char *txIpcRawData = nullptr;

        txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
        ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

        mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
        ASSERT_EQ(mpModemData->GetMessageId(), sitId);

        sitToken = mpModemData->GetToken();
        mRspDataLen = 100;
        mRspData = allocRspModemMem(mRspDataLen, sitId, sitToken);
        psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

        ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

        UINT32 rilRspDataLen = 0;
        void *rilRspData= nullptr;
        rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
        EXPECT_NE((unsigned int)0, rilRspDataLen);

        cleanup(psitRilTest, mRspData, mpModemData, &mRspDataLen);
        mpModemData = nullptr;
        mRspData = nullptr;
    }
}

TEST_P(SitrilTestImsMedia, oemUnsolicitedTestAllList) {

    const static int messageMap[][2] = {
            {RIL_UNSOL_OEM_IMS_TEST_RCM, SIT_IND_IMS_MEDIA_TEST_RCM},
            {RIL_UNSOL_OEM_IMS_MEDIA_ON_DTMF_RECEIVED, SIT_IND_IMS_MEDIA_ON_DTMF_RECEIVED},
            {RIL_UNSOL_OEM_IMS_MEDIA_ON_TEXT_RECEIVED, SIT_IND_IMS_MEDIA_ON_TEXT_RECEIVED},
            {RIL_UNSOL_OEM_IMS_MEDIA_ON_HEADER_EXTENSION_RECEIVED, SIT_IND_IMS_MEDIA_ON_HEADER_EXTENSION_RECEIVED},
            {RIL_UNSOL_OEM_IMS_MEDIA_ON_FIRST_MEDIA_RECEIVED, SIT_IND_IMS_MEDIA_ON_FIRST_MEDIA_RECEIVED},
            {RIL_UNSOL_OEM_IMS_MEDIA_NOTIFY_MEDIA_INACTIVITY, SIT_IND_IMS_MEDIA_NOTIFY_MEDIA_INACTIVITY},
            {RIL_UNSOL_OEM_IMS_MEDIA_NOTIFY_PACKET_LOSS, SIT_IND_IMS_MEDIA_NOTIFY_PACKET_LOSS},
            {RIL_UNSOL_OEM_IMS_MEDIA_NOTIFY_JITTER, SIT_IND_IMS_MEDIA_NOTIFY_JITTER},
            {RIL_UNSOL_OEM_IMS_MEDIA_ON_CALL_QUALITY_CHANGED, SIT_IND_IMS_MEDIA_ON_CALL_QUALITY_CHANGED},
            {RIL_UNSOL_OEM_IMS_MEDIA_TRIGGER_ANBR_QUERY, SIT_IND_IMS_MEDIA_TRIGGER_AMBR_QUERY},
            {RIL_UNSOL_OEM_IMS_MEDIA_MAC_CE_INFO, SIT_IND_MAC_CE_INFO},

            };

    for (auto testElement : messageMap) {
        int unsolMessage = testElement[0];
        int sitMessage = testElement[1];

        SitrilTest *psitRilTest = getSitrilTest();
        ASSERT_NE(nullptr, psitRilTest);

        const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
        ASSERT_NE(nullptr, prilFuncs);

        UINT16 sitId = sitMessage;

        int rilVer = prilFuncs->version;
        EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

        mRspData = buildEmptyInd(sitId, &mRspDataLen);
        psitRilTest->processInd(unsolMessage, mRspData, mRspDataLen);

        ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());
        UINT32 indLen = 0;
        const void *rilIndData= nullptr;
        rilIndData = psitRilTest->getRilIndData(0, &indLen);
        EXPECT_EQ((UINT32)0, indLen);

        cleanup(psitRilTest, mRspData, mpModemData, &mRspDataLen);
        mpModemData = nullptr;
        mRspData = nullptr;
    }
}
