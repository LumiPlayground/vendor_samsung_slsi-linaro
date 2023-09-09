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
#include "factory/sms/protocolsmsadapterlegacyfactory.h"

class ProtocolSmsAdapterLegacyFactoryTest : public ::testing::Test {

protected:
    void SetUp() override {
        factory = new ProtocolSmsAdapterLegacyFactory();
        modemData = new ModemData(testData, sizeof(testData));
    }

    void TearDown() override {
        delete factory;
        factory = nullptr;

        delete modemData;
        modemData = nullptr;
    }

    static constexpr char testData[] = {0x01, 0x02, 0x03, 0x04, 0x05};
    ProtocolSmsAdapterLegacyFactory *factory;
    ModemData *modemData;
};

TEST_F(ProtocolSmsAdapterLegacyFactoryTest, createProtocolSendSmsRespAdapter) {
    ASSERT_NE(factory, nullptr);

    ASSERT_NE(modemData, nullptr);

    ProtocolSendSmsRespAdapterLegacy *sendSmsRespAdapterLegacy =
        factory->createProtocolSendSmsRespAdapter(modemData);
    ASSERT_NE(sendSmsRespAdapterLegacy, nullptr);

    delete sendSmsRespAdapterLegacy;
    sendSmsRespAdapterLegacy = nullptr;
}

TEST_F(ProtocolSmsAdapterLegacyFactoryTest, createProtocolWriteSmsToSimRespAdapter) {
    ASSERT_NE(factory, nullptr);

    ASSERT_NE(modemData, nullptr);

    ProtocolWriteSmsToSimRespAdapterLegacy *writeSmsToSimRespAdapterLegacy =
        factory->createProtocolWriteSmsToSimRespAdapter(modemData);
    ASSERT_NE(writeSmsToSimRespAdapterLegacy, nullptr);

    delete writeSmsToSimRespAdapterLegacy;
    writeSmsToSimRespAdapterLegacy = nullptr;
}

TEST_F(ProtocolSmsAdapterLegacyFactoryTest, createProtocolSmsCapacityOnSimRespAdapter) {
    ASSERT_NE(factory, nullptr);

    ASSERT_NE(modemData, nullptr);

    ProtocolSmsCapacityOnSimRespAdapterLegacy *smsCapacityOnSimRespAdapterLegacy =
        factory->createProtocolSmsCapacityOnSimRespAdapter(modemData);
    ASSERT_NE(smsCapacityOnSimRespAdapterLegacy, nullptr);

    delete smsCapacityOnSimRespAdapterLegacy;
    smsCapacityOnSimRespAdapterLegacy = nullptr;
}

TEST_F(ProtocolSmsAdapterLegacyFactoryTest, createProtocolSmscAddrRespAdapter) {
    ASSERT_NE(factory, nullptr);

    ASSERT_NE(modemData, nullptr);

    ProtocolSmscAddrRespAdapterLegacy *smscAddrRespAdapterLegacy =
        factory->createProtocolSmscAddrRespAdapter(modemData);
    ASSERT_NE(smscAddrRespAdapterLegacy, nullptr);

    delete smscAddrRespAdapterLegacy;
    smscAddrRespAdapterLegacy = nullptr;
}

TEST_F(ProtocolSmsAdapterLegacyFactoryTest, createProtocolNewSmsIndAdapter) {
    ASSERT_NE(factory, nullptr);

    ASSERT_NE(modemData, nullptr);

    ProtocolNewSmsIndAdapterLegacy *newSmsIndAdapterLegacy =
        factory->createProtocolNewSmsIndAdapter(modemData);
    ASSERT_NE(newSmsIndAdapterLegacy, nullptr);

    delete newSmsIndAdapterLegacy;
    newSmsIndAdapterLegacy = nullptr;
}

TEST_F(ProtocolSmsAdapterLegacyFactoryTest, createProtocolGetBcstSmsConfRespAdapter) {
    ASSERT_NE(factory, nullptr);

    ASSERT_NE(modemData, nullptr);

    ProtocolGetBcstSmsConfRespAdapterLegacy *getBcstSmsConfRespAdapterLegacy =
        factory->createProtocolGetBcstSmsConfRespAdapter(modemData);
    ASSERT_NE(getBcstSmsConfRespAdapterLegacy, nullptr);

    delete getBcstSmsConfRespAdapterLegacy;
    getBcstSmsConfRespAdapterLegacy = nullptr;
}

TEST_F(ProtocolSmsAdapterLegacyFactoryTest, createProtocolNewBcstSmsAdapter) {
    ASSERT_NE(factory, nullptr);

    ASSERT_NE(modemData, nullptr);

    ProtocolNewBcstSmsAdapterLegacy *newBcstSmsAdapterLegacy =
        factory->createProtocolNewBcstSmsAdapter(modemData);
    ASSERT_NE(newBcstSmsAdapterLegacy, nullptr);

    delete newBcstSmsAdapterLegacy;
    newBcstSmsAdapterLegacy = nullptr;
}

TEST_F(ProtocolSmsAdapterLegacyFactoryTest, createProtocolCdmaSendSmsRespAdapter) {
    ASSERT_NE(factory, nullptr);

    ASSERT_NE(modemData, nullptr);

    ProtocolCdmaSendSmsRespAdapterLegacy *cdmaSendSmsRespAdapterLegacy =
        factory->createProtocolCdmaSendSmsRespAdapter(modemData);
    ASSERT_NE(cdmaSendSmsRespAdapterLegacy, nullptr);

    delete cdmaSendSmsRespAdapterLegacy;
    cdmaSendSmsRespAdapterLegacy = nullptr;
}

TEST_F(ProtocolSmsAdapterLegacyFactoryTest, createProtocolCdmaNewSmsIndAdapter) {
    ASSERT_NE(factory, nullptr);

    ASSERT_NE(modemData, nullptr);

    ProtocolCdmaNewSmsIndAdapterLegacy *cdmaNewSmsIndAdapterLegacy =
        factory->createProtocolCdmaNewSmsIndAdapter(modemData);
    ASSERT_NE(cdmaNewSmsIndAdapterLegacy, nullptr);

    delete cdmaNewSmsIndAdapterLegacy;
    cdmaNewSmsIndAdapterLegacy = nullptr;
}

TEST_F(ProtocolSmsAdapterLegacyFactoryTest, createProtocolCdmaWriteSmsToRuimRespAdapter) {
    ASSERT_NE(factory, nullptr);

    ASSERT_NE(modemData, nullptr);

    ProtocolCdmaWriteSmsToRuimRespAdapterLegacy *cdmaWriteSmsToRuimRespAdapterLegacy =
        factory->createProtocolCdmaWriteSmsToRuimRespAdapter(modemData);
    ASSERT_NE(cdmaWriteSmsToRuimRespAdapterLegacy, nullptr);

    delete cdmaWriteSmsToRuimRespAdapterLegacy;
    cdmaWriteSmsToRuimRespAdapterLegacy = nullptr;
}

TEST_F(ProtocolSmsAdapterLegacyFactoryTest, createProtocolGetCdmaBcstSmsConfRespAdapter) {
    ASSERT_NE(factory, nullptr);

    ASSERT_NE(modemData, nullptr);

    ProtocolGetCdmaBcstSmsConfRespAdapterLegacy *getCdmaBcstSmsConfRespAdapterLegacy =
        factory->createProtocolGetCdmaBcstSmsConfRespAdapter(modemData);
    ASSERT_NE(getCdmaBcstSmsConfRespAdapterLegacy, nullptr);

    delete getCdmaBcstSmsConfRespAdapterLegacy;
    getCdmaBcstSmsConfRespAdapterLegacy = nullptr;
}

TEST_F(ProtocolSmsAdapterLegacyFactoryTest, createProtocolCdmaVoiceMsgWaitingInfoIndAdapter) {
    ASSERT_NE(factory, nullptr);

    ASSERT_NE(modemData, nullptr);

    ProtocolCdmaVoiceMsgWaitingInfoIndAdapterLegacy *cdmaVoiceMsgWaitingInfoIndAdapterLegacy =
        factory->createProtocolCdmaVoiceMsgWaitingInfoIndAdapter(modemData);
    ASSERT_NE(cdmaVoiceMsgWaitingInfoIndAdapterLegacy, nullptr);

    delete cdmaVoiceMsgWaitingInfoIndAdapterLegacy;
    cdmaVoiceMsgWaitingInfoIndAdapterLegacy = nullptr;
}
