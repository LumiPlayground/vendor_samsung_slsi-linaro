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

#include <gtest/gtest.h>
#include "factory/sms/protocolsmsadapterjsonfactory.h"

class ProtocolSmsAdapterJsonFactoryTest : public ::testing::Test {

protected:
    void SetUp() override {
        factory = new ProtocolSmsAdapterJsonFactory();
        modemData = new ModemData(testData, sizeof(testData));
    }

    void TearDown() override {
        delete factory;
        factory = nullptr;

        delete modemData;
        modemData = nullptr;
    }

    static constexpr char testData[] = {0x01, 0x02, 0x03, 0x04, 0x05};
    ProtocolSmsAdapterJsonFactory *factory;
    ModemData *modemData;
};

TEST_F(ProtocolSmsAdapterJsonFactoryTest, createProtocolSendSmsRespAdapter) {
    ASSERT_NE(factory, nullptr);

    ASSERT_NE(modemData, nullptr);

    ProtocolSendSmsRespAdapterJson *sendSmsRespAdapterJson =
        factory->createProtocolSendSmsRespAdapter(modemData);
    ASSERT_NE(sendSmsRespAdapterJson, nullptr);

    delete sendSmsRespAdapterJson;
    sendSmsRespAdapterJson = nullptr;
}

TEST_F(ProtocolSmsAdapterJsonFactoryTest, createProtocolWriteSmsToSimRespAdapter) {
    ASSERT_NE(factory, nullptr);

    ASSERT_NE(modemData, nullptr);

    ProtocolWriteSmsToSimRespAdapterJson *writeSmsToSimRespAdapterJson =
        factory->createProtocolWriteSmsToSimRespAdapter(modemData);
    ASSERT_NE(writeSmsToSimRespAdapterJson, nullptr);

    delete writeSmsToSimRespAdapterJson;
    writeSmsToSimRespAdapterJson = nullptr;
}

TEST_F(ProtocolSmsAdapterJsonFactoryTest, createProtocolSmsCapacityOnSimRespAdapter) {
    ASSERT_NE(factory, nullptr);

    ASSERT_NE(modemData, nullptr);

    ProtocolSmsCapacityOnSimRespAdapterJson *smsCapacityOnSimRespAdapterJson =
        factory->createProtocolSmsCapacityOnSimRespAdapter(modemData);
    ASSERT_NE(smsCapacityOnSimRespAdapterJson, nullptr);

    delete smsCapacityOnSimRespAdapterJson;
    smsCapacityOnSimRespAdapterJson = nullptr;
}

TEST_F(ProtocolSmsAdapterJsonFactoryTest, createProtocolSmscAddrRespAdapter) {
    ASSERT_NE(factory, nullptr);

    ASSERT_NE(modemData, nullptr);

    ProtocolSmscAddrRespAdapterJson *smscAddrRespAdapterJson =
        factory->createProtocolSmscAddrRespAdapter(modemData);
    ASSERT_NE(smscAddrRespAdapterJson, nullptr);

    delete smscAddrRespAdapterJson;
    smscAddrRespAdapterJson = nullptr;
}

TEST_F(ProtocolSmsAdapterJsonFactoryTest, createProtocolNewSmsIndAdapter) {
    ASSERT_NE(factory, nullptr);

    ASSERT_NE(modemData, nullptr);

    ProtocolNewSmsIndAdapterJson *newSmsIndAdapterJson =
        factory->createProtocolNewSmsIndAdapter(modemData);
    ASSERT_NE(newSmsIndAdapterJson, nullptr);

    delete newSmsIndAdapterJson;
    newSmsIndAdapterJson = nullptr;
}

TEST_F(ProtocolSmsAdapterJsonFactoryTest, createProtocolGetBcstSmsConfRespAdapter) {
    ASSERT_NE(factory, nullptr);

    ASSERT_NE(modemData, nullptr);

    ProtocolGetBcstSmsConfRespAdapterJson *getBcstSmsConfRespAdapterJson =
        factory->createProtocolGetBcstSmsConfRespAdapter(modemData);
    ASSERT_NE(getBcstSmsConfRespAdapterJson, nullptr);

    delete getBcstSmsConfRespAdapterJson;
    getBcstSmsConfRespAdapterJson = nullptr;
}

TEST_F(ProtocolSmsAdapterJsonFactoryTest, createProtocolNewBcstSmsAdapter) {
    ASSERT_NE(factory, nullptr);

    ASSERT_NE(modemData, nullptr);

    ProtocolNewBcstSmsAdapterJson *newBcstSmsAdapterJson =
        factory->createProtocolNewBcstSmsAdapter(modemData);
    ASSERT_NE(newBcstSmsAdapterJson, nullptr);

    delete newBcstSmsAdapterJson;
    newBcstSmsAdapterJson = nullptr;
}

TEST_F(ProtocolSmsAdapterJsonFactoryTest, createProtocolCdmaSendSmsRespAdapter) {
    ASSERT_NE(factory, nullptr);

    ASSERT_NE(modemData, nullptr);

    ProtocolCdmaSendSmsRespAdapterJson *cdmaSendSmsRespAdapterJson =
        factory->createProtocolCdmaSendSmsRespAdapter(modemData);
    ASSERT_NE(cdmaSendSmsRespAdapterJson, nullptr);

    delete cdmaSendSmsRespAdapterJson;
    cdmaSendSmsRespAdapterJson = nullptr;
}

TEST_F(ProtocolSmsAdapterJsonFactoryTest, createProtocolCdmaNewSmsIndAdapter) {
    ASSERT_NE(factory, nullptr);

    ASSERT_NE(modemData, nullptr);

    ProtocolCdmaNewSmsIndAdapterJson *cdmaNewSmsIndAdapterJson =
        factory->createProtocolCdmaNewSmsIndAdapter(modemData);
    ASSERT_NE(cdmaNewSmsIndAdapterJson, nullptr);

    delete cdmaNewSmsIndAdapterJson;
    cdmaNewSmsIndAdapterJson = nullptr;
}

TEST_F(ProtocolSmsAdapterJsonFactoryTest, createProtocolCdmaWriteSmsToRuimRespAdapter) {
    ASSERT_NE(factory, nullptr);

    ASSERT_NE(modemData, nullptr);

    ProtocolCdmaWriteSmsToRuimRespAdapterJson *cdmaWriteSmsToRuimRespAdapterJson =
        factory->createProtocolCdmaWriteSmsToRuimRespAdapter(modemData);
    ASSERT_NE(cdmaWriteSmsToRuimRespAdapterJson, nullptr);

    delete cdmaWriteSmsToRuimRespAdapterJson;
    cdmaWriteSmsToRuimRespAdapterJson = nullptr;
}

TEST_F(ProtocolSmsAdapterJsonFactoryTest, createProtocolGetCdmaBcstSmsConfRespAdapter) {
    ASSERT_NE(factory, nullptr);

    ASSERT_NE(modemData, nullptr);

    ProtocolGetCdmaBcstSmsConfRespAdapterJson *getCdmaBcstSmsConfRespAdapterJson =
        factory->createProtocolGetCdmaBcstSmsConfRespAdapter(modemData);
    ASSERT_NE(getCdmaBcstSmsConfRespAdapterJson, nullptr);

    delete getCdmaBcstSmsConfRespAdapterJson;
    getCdmaBcstSmsConfRespAdapterJson = nullptr;
}

TEST_F(ProtocolSmsAdapterJsonFactoryTest, createProtocolCdmaVoiceMsgWaitingInfoIndAdapter) {
    ASSERT_NE(factory, nullptr);

    ASSERT_NE(modemData, nullptr);

    ProtocolCdmaVoiceMsgWaitingInfoIndAdapterJson *cdmaVoiceMsgWaitingInfoIndAdapterJson =
        factory->createProtocolCdmaVoiceMsgWaitingInfoIndAdapter(modemData);
    ASSERT_NE(cdmaVoiceMsgWaitingInfoIndAdapterJson, nullptr);

    delete cdmaVoiceMsgWaitingInfoIndAdapterJson;
    cdmaVoiceMsgWaitingInfoIndAdapterJson = nullptr;
}
