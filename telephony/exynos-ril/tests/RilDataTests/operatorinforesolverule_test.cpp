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
 * operatorinforesolverule_test.cpp
 *
 *  Created on: 2021. 10. 1.
 *      Author: sungwoo48.choi
 */
#include <gtest/gtest.h>
#include "operatorinforesolverule.h"
#include "eonsResolver.h"
#include "test_api.h"

using std::string;
using std::stoi;

static OperatorInfo npos;

static void PrepareEonsData(EonsResolver &resolver) {
    const int numOfEfs = 2;
    const int maxSizeOfData = 70;
    const int ef_pnn = 0x6fc5;
    const int ef_opl = 0x6fc6;

    struct SimEfData{
        int simFileId;
        int recordLen;
        int numOfRecords;
        BYTE data[maxSizeOfData];
    };

    SimEfData sampleDatas[numOfEfs] = {
        // sample PNN data
        // PLMN 5G, ABCD, CCCDDD
        { ef_pnn, 20, 3,
            {
                0x43, 0x08, 0x86, 0x50, 0x66, 0xd3, 0x09, 0xaa, 0x1d, 0x1b, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                0x43, 0x05, 0x86, 0x41, 0xe1, 0x90, 0x08, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                0x43, 0x07, 0x86, 0xc3, 0xe1, 0x90, 0x48, 0x24, 0x02, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            }
        },
        // sample OPL record
        { ef_opl, 8, 3,
            {
                0x64, 0xf0, 0x10, 0x00, 0x00, 0xff, 0xfe, 0x01,
                0x64, 0xf0, 0x90, 0x00, 0x00, 0xff, 0xfe, 0x02,
                0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            }
        },
    };

    for (int i = 0; i < numOfEfs; ++i) {
        SimEfData* efData = &(sampleDatas[i]);
        BYTE **ppData = new BYTE *[efData->numOfRecords];
        for (int recordIndex = 0; recordIndex < efData->numOfRecords; ++recordIndex) {
            BYTE *pInputRecord = efData->data + (recordIndex * efData->recordLen);
            BYTE *record = new BYTE[efData->recordLen];
            memcpy(record, pInputRecord, efData->recordLen);
            *(ppData + recordIndex) = record;
        }

        if (efData->simFileId == ef_pnn) {
            resolver.resetPnnData();
            if (efData->numOfRecords > 0) resolver.setPnnData(ppData, efData->recordLen, efData->numOfRecords);
        } else if (efData->simFileId == ef_opl) {
            resolver.resetOplData();
            if (efData->numOfRecords > 0) resolver.setOplData(ppData, efData->recordLen, efData->numOfRecords);
        }

        for (int recordIndex = 0; recordIndex < efData->numOfRecords; ++recordIndex) {
            delete [] *(ppData + recordIndex);
        }
        delete [] ppData;
    }
}


TEST(OperatorInfoResolveRule, OperatorNumericRule) {
    string numeric = "00101";
    OperatorNumericRule testRule(nullptr, numeric);
    OperatorInfo ret = testRule.resolve();
    EXPECT_TRUE(ret.numeric == numeric);
    EXPECT_TRUE(ret.alphaLong == numeric);
    EXPECT_TRUE(ret.alphaShort == numeric);
    EXPECT_EQ(ret.mcc, stoi(numeric.substr(0, 3)));
    EXPECT_EQ(ret.mnc, stoi(numeric.substr(3)));

    numeric = "45005";
    testRule = OperatorNumericRule(nullptr, "45005");
    ret = testRule.resolve();
    EXPECT_TRUE(ret.numeric == numeric);
    EXPECT_TRUE(ret.alphaLong == numeric);
    EXPECT_TRUE(ret.alphaShort == numeric);
    EXPECT_EQ(ret.mcc, stoi(numeric.substr(0, 3)));
    EXPECT_EQ(ret.mnc, stoi(numeric.substr(3)));

    numeric = "311480";
    testRule = OperatorNumericRule(nullptr, "311480");
    ret = testRule.resolve();
    EXPECT_TRUE(ret.numeric == numeric);
    EXPECT_TRUE(ret.alphaLong == numeric);
    EXPECT_TRUE(ret.alphaShort == numeric);
    EXPECT_EQ(ret.mcc, stoi(numeric.substr(0, 3)));
    EXPECT_EQ(ret.mnc, stoi(numeric.substr(3)));

    testRule = OperatorNumericRule(nullptr, "311");
    ret = testRule.resolve();
    EXPECT_TRUE(ret == npos);

    testRule = OperatorNumericRule(nullptr, "testbed");
    ret = testRule.resolve();
    EXPECT_TRUE(ret == npos);

    testRule = OperatorNumericRule(nullptr, "311480123");
    ret = testRule.resolve();
    EXPECT_TRUE(ret == npos);
}

TEST(OperatorInfoResolveRule, SpnRule) {
    // The MCCMNC code which do use SPN for available network search
    struct {
        const char *numeric;
        const char *spn;
    } testVector[] = {
        { "722310", "CLARO ARGENTINA" },
        { "722320", "CLARO ARGENTINA" },
        { "722330", "CLARO ARGENTINA" },
        { "26002", "T-Mobile Poland" },
        { "26003", "nju (Orange) Poland" },
        { "50503", "Vodafone-AU or Lebara(MVNO)" },
        { "732123", "Movistar (Colombia)" },
    };
    size_t size = NUM_OF_ELEMENTS(testVector);

    string networkNumeric;
    string simNumeric;
    string spn;
    for (size_t i = 0; i < size; i++) {
        networkNumeric = testVector[i].numeric;
        simNumeric = testVector[i].numeric;
        spn = testVector[i].spn;
        SpnRule testRule(nullptr, networkNumeric, simNumeric, spn);
        OperatorInfo ret = testRule.resolve();
        EXPECT_TRUE(ret.numeric == networkNumeric);
        EXPECT_TRUE(ret.alphaLong == spn);
        EXPECT_TRUE(ret.alphaShort == spn);
        EXPECT_EQ(ret.mcc, stoi(simNumeric.substr(0, 3)));
        EXPECT_EQ(ret.mnc, stoi(simNumeric.substr(3)));
    }

    // not in available list
    networkNumeric = "45005";
    simNumeric = "45005";
    spn = "SKT";
    SpnRule testRule(nullptr, networkNumeric, simNumeric, spn);
    OperatorInfo ret = testRule.resolve();
    EXPECT_TRUE(ret == npos);

    // networkNumeric != simNumeric
    networkNumeric = "722310";
    simNumeric = "722310";
    spn = "";
    testRule = SpnRule(nullptr, networkNumeric, simNumeric, spn);
    ret = testRule.resolve();
    EXPECT_TRUE(ret == npos);
}

TEST(OperatorInfoResolveRule, SpnRule_chain) {
    string networkNumeric = "722310";
    string simNumeric = "722310";
    string spn = "";
    OperatorNumericRule chainRule(nullptr, networkNumeric);
    SpnRule testRule(&chainRule, networkNumeric, simNumeric, spn);
    OperatorInfo ret = testRule.resolve();
    EXPECT_TRUE(ret != npos);
    EXPECT_TRUE(ret.numeric == networkNumeric);
    EXPECT_TRUE(ret.alphaLong == networkNumeric);
    EXPECT_TRUE(ret.alphaShort == networkNumeric);
    EXPECT_EQ(ret.mcc, stoi(networkNumeric.substr(0, 3)));
    EXPECT_EQ(ret.mnc, stoi(networkNumeric.substr(3)));
}

TEST(OperatorInfoResolveRule, VendorCustomRule) {
    string networkNumeric = "20210";
    string simNumeric = "20209";
    string alpha = "Q-TELCOM";

    VendorCustomRule testRule(nullptr, networkNumeric, simNumeric);
    OperatorInfo ret = testRule.resolve();
    EXPECT_TRUE(ret.numeric == networkNumeric);
    EXPECT_TRUE(ret.alphaLong == alpha);
    EXPECT_TRUE(ret.alphaShort == alpha);
    EXPECT_EQ(ret.mcc, stoi(networkNumeric.substr(0, 3)));
    EXPECT_EQ(ret.mnc, stoi(networkNumeric.substr(3)));

    networkNumeric = "45005";
    simNumeric = "45005";
    testRule = VendorCustomRule(nullptr, networkNumeric, simNumeric);
    ret = testRule.resolve();
    EXPECT_TRUE(ret == npos);
}

TEST(OperatorInfoResolveRule, VendorCustomRule_chain) {
    string networkNumeric = "26002";
    string simNumeric = "26002";
    string alpha = "Q-TELCOM";

    SpnRule chainRule(nullptr, networkNumeric, simNumeric, alpha);
    VendorCustomRule testRule(&chainRule, networkNumeric, simNumeric);
    OperatorInfo ret = testRule.resolve();
    EXPECT_TRUE(ret.numeric == networkNumeric);
    EXPECT_TRUE(ret.alphaLong == alpha);
    EXPECT_TRUE(ret.alphaShort == alpha);
    EXPECT_EQ(ret.mcc, stoi(networkNumeric.substr(0, 3)));
    EXPECT_EQ(ret.mnc, stoi(networkNumeric.substr(3)));

    networkNumeric = "45005";
    simNumeric = "45005";
    chainRule = SpnRule(nullptr, networkNumeric, simNumeric, alpha);
    testRule = VendorCustomRule(&chainRule, networkNumeric, simNumeric);
    ret = testRule.resolve();
    EXPECT_TRUE(ret == npos);
}

TEST(OperatorInfoResolveRule, NitzRule) {
    string networkNumeric = "26002";
    string nitzLong = "TEST_LONG";
    string nitzShort = "TEST_SHORT";

    NitzRule testRule(nullptr, networkNumeric, nitzLong, nitzShort);
    OperatorInfo ret = testRule.resolve();
    EXPECT_TRUE(ret.numeric == networkNumeric);
    EXPECT_TRUE(ret.alphaLong == nitzLong);
    EXPECT_TRUE(ret.alphaShort == nitzShort);
    EXPECT_EQ(ret.mcc, stoi(networkNumeric.substr(0, 3)));
    EXPECT_EQ(ret.mnc, stoi(networkNumeric.substr(3)));

    testRule = NitzRule(nullptr, "", nitzLong, nitzShort);
    ret = testRule.resolve();
    EXPECT_TRUE(ret == npos);
}

TEST(OperatorInfoResolveRule, NitzRule_chain) {
    string networkNumeric = "26002";
    string simNumeric = "26002";
    string spn = "Q-TELCOM";

    SpnRule chainRule(nullptr, networkNumeric, simNumeric, spn );
    NitzRule testRule(&chainRule, networkNumeric, "", "");
    OperatorInfo ret = testRule.resolve();
    EXPECT_TRUE(ret.numeric == networkNumeric);
    EXPECT_TRUE(ret.alphaLong == spn );
    EXPECT_TRUE(ret.alphaShort == spn );
    EXPECT_EQ(ret.mcc, stoi(networkNumeric.substr(0, 3)));
    EXPECT_EQ(ret.mnc, stoi(networkNumeric.substr(3)));

    networkNumeric = "45005";
    simNumeric = "45005";
    chainRule = SpnRule(nullptr, networkNumeric, simNumeric, spn);
    testRule = NitzRule(&chainRule, networkNumeric, "", "");
    ret = testRule.resolve();
    EXPECT_TRUE(ret == npos);
}

TEST(OperatorInfoResolveRule, RegNetSpnRule) {
    string networkNumeric = "72402";
    string simNumeric ="72402";
    string spn = "Q-TELCOM";
    bool isNitzAvailable = true;
    bool isRegHome = true;

    RegNetSpnRule testRule(nullptr, networkNumeric, simNumeric, spn, isNitzAvailable, isRegHome);
    OperatorInfo ret = testRule.resolve();
    EXPECT_TRUE(ret.numeric == networkNumeric);
    EXPECT_TRUE(ret.alphaLong == spn);
    EXPECT_TRUE(ret.alphaShort == spn);
    EXPECT_EQ(ret.mcc, stoi(networkNumeric.substr(0, 3)));
    EXPECT_EQ(ret.mnc, stoi(networkNumeric.substr(3)));

    testRule = RegNetSpnRule(nullptr, string("72401"), simNumeric, spn, isNitzAvailable, isRegHome);
    ret = testRule.resolve();
    EXPECT_TRUE(ret == npos);

    testRule = RegNetSpnRule(nullptr, networkNumeric, simNumeric, spn, isNitzAvailable, false);
    ret = testRule.resolve();
    EXPECT_TRUE(ret == npos);
}

TEST(OperatorInfoResolveRule, RegNetSpnRule_chain) {
    string networkNumeric = "72402";
    string simNumeric ="72402";
    string spn = "Q-TELCOM";
    bool isNitzAvailable = true;
    bool isRegHome = false;

    string nitzLong = "TEST_LONG";
    string nitzShort = "TEST_SHORT";

    NitzRule chainRule(nullptr, networkNumeric, nitzLong, nitzShort);
    RegNetSpnRule testRule(&chainRule, networkNumeric, simNumeric, spn, isNitzAvailable, isRegHome);
    OperatorInfo ret = testRule.resolve();
    EXPECT_TRUE(ret.numeric == networkNumeric);
    EXPECT_TRUE(ret.alphaLong == nitzLong);
    EXPECT_TRUE(ret.alphaShort == nitzShort);
    EXPECT_EQ(ret.mcc, stoi(networkNumeric.substr(0, 3)));
    EXPECT_EQ(ret.mnc, stoi(networkNumeric.substr(3)));

    chainRule = NitzRule(nullptr, networkNumeric, "", "");
    testRule = RegNetSpnRule(&chainRule, networkNumeric, simNumeric, spn, isNitzAvailable, isRegHome);
    ret = testRule.resolve();
    EXPECT_TRUE(ret == npos);
}

TEST(PnnOplRule, PnnOplRule) {
    string networkNumeric = "460010";
    EonsResolver &resolver = EonsResolver::createForPhoneId(0);
    PrepareEonsData(resolver);

    string longAlpha = "PLMN 5G";
    string shortAlpha = "PLMN 5G";

    PnnOplRule testRulePass(nullptr, networkNumeric, resolver);
    OperatorInfo ret = testRulePass.resolve();
    EXPECT_TRUE(ret.numeric == networkNumeric);
    EXPECT_TRUE(ret.alphaLong == longAlpha);
    EXPECT_TRUE(ret.alphaShort == shortAlpha);
    EXPECT_EQ(ret.mcc, stoi(networkNumeric.substr(0, 3)));
    EXPECT_EQ(ret.mnc, stoi(networkNumeric.substr(3)));

    PnnOplRule testRuleFail(nullptr, string("470010"), resolver);
    ret = testRuleFail.resolve();
    EXPECT_TRUE(ret == npos);
}

TEST(PnnOplRule, PnnOplRule_chain) {
    string networkNumeric = "450010";
    EonsResolver &resolver = EonsResolver::createForPhoneId(0);
    PrepareEonsData(resolver);

    string nitzLong = "TEST_LONG";
    string nitzShort = "TEST_SHORT";

    NitzRule chainRule(nullptr, networkNumeric, nitzLong, nitzShort);
    PnnOplRule testRulePass(&chainRule, networkNumeric, resolver);
    OperatorInfo ret = testRulePass.resolve();
    EXPECT_TRUE(ret.numeric == networkNumeric);
    EXPECT_TRUE(ret.alphaLong == nitzLong);
    EXPECT_TRUE(ret.alphaShort == nitzShort);
    EXPECT_EQ(ret.mcc, stoi(networkNumeric.substr(0, 3)));
    EXPECT_EQ(ret.mnc, stoi(networkNumeric.substr(3)));

    chainRule = NitzRule(nullptr, networkNumeric, "", "");
    PnnOplRule testRuleFail(&chainRule, networkNumeric, resolver);
    ret = testRuleFail.resolve();
    EXPECT_TRUE(ret == npos);
}

TEST(PnnOplRule, RegNetPnnOplRule) {
    string networkNumeric = "460010";
    string simNumeric = "460010";
    int lac = 1;
    EonsResolver &resolver = EonsResolver::createForPhoneId(0);
    PrepareEonsData(resolver);

    string longAlpha = "PLMN 5G";
    string shortAlpha = "PLMN 5G";

    RegNetPnnOplRule testRulePass(nullptr, networkNumeric, resolver, simNumeric, lac);
    OperatorInfo ret = testRulePass.resolve();
    EXPECT_TRUE(ret.numeric == networkNumeric);
    EXPECT_TRUE(ret.alphaLong == longAlpha);
    EXPECT_TRUE(ret.alphaShort == shortAlpha);
    EXPECT_EQ(ret.mcc, stoi(networkNumeric.substr(0, 3)));
    EXPECT_EQ(ret.mnc, stoi(networkNumeric.substr(3)));

    RegNetPnnOplRule testRuleFail(nullptr, networkNumeric, resolver, simNumeric, -1);
    ret = testRuleFail.resolve();
    EXPECT_TRUE(ret == npos);
}

TEST(PnnOplRule, RegNetPnnOplRule_chain) {
    string networkNumeric = "460010";
    string simNumeric = "460010";
    int lac = -1;
    EonsResolver &resolver = EonsResolver::createForPhoneId(0);
    PrepareEonsData(resolver);

    string nitzLong = "TEST_LONG";
    string nitzShort = "TEST_SHORT";

    NitzRule chainRule(nullptr, networkNumeric, nitzLong, nitzShort);
    RegNetPnnOplRule testRulePass(&chainRule, networkNumeric, resolver, simNumeric, lac);
    OperatorInfo ret = testRulePass.resolve();
    EXPECT_TRUE(ret.numeric == networkNumeric);
    EXPECT_TRUE(ret.alphaLong == nitzLong);
    EXPECT_TRUE(ret.alphaShort == nitzShort);
    EXPECT_EQ(ret.mcc, stoi(networkNumeric.substr(0, 3)));
    EXPECT_EQ(ret.mnc, stoi(networkNumeric.substr(3)));

    chainRule = NitzRule(nullptr, networkNumeric, "", "");
    RegNetPnnOplRule testRuleFail(&chainRule, networkNumeric, resolver, simNumeric, lac);
    ret = testRuleFail.resolve();
    EXPECT_TRUE(ret == npos);
}

TEST(EonsDBRule, EonsDBRule) {
    string networkNumeric = "72402";

    EonsDBRule testRule(nullptr, networkNumeric, "");
    OperatorInfo ret = testRule.resolve();
    EXPECT_TRUE(ret == npos);
}

TEST(EonsDBRule, EonsDBRule_chain) {
    string networkNumeric = "72402";

    string nitzLong = "TEST_LONG";
    string nitzShort = "TEST_SHORT";

    NitzRule chainRule(nullptr, networkNumeric, nitzLong, nitzShort);
    EonsDBRule testRule(&chainRule, networkNumeric, "");
    OperatorInfo ret = testRule.resolve();
    EXPECT_TRUE(ret.numeric == networkNumeric);
    EXPECT_TRUE(ret.alphaLong == nitzLong);
    EXPECT_TRUE(ret.alphaShort == nitzShort);
    EXPECT_EQ(ret.mcc, stoi(networkNumeric.substr(0, 3)));
    EXPECT_EQ(ret.mnc, stoi(networkNumeric.substr(3)));
}

TEST(EonsCashRule, EonsCashRule) {
    string networkNumeric = "72402";
    string iccId ="ABCDEFG";

    EonsCashRule testRule(nullptr, networkNumeric, iccId);
    OperatorInfo ret = testRule.resolve();
    EXPECT_TRUE(ret == npos);
}

TEST(EonsCashRule, EonsCashRule_chain) {
    string networkNumeric = "72402";
    string iccId ="ABCDEFG";

    string nitzLong = "TEST_LONG";
    string nitzShort = "TEST_SHORT";

    NitzRule chainRule(nullptr, networkNumeric, nitzLong, nitzShort);
    EonsCashRule testRule(&chainRule, networkNumeric, iccId);
    OperatorInfo ret = testRule.resolve();
    EXPECT_TRUE(ret.numeric == networkNumeric);
    EXPECT_TRUE(ret.alphaLong == nitzLong);
    EXPECT_TRUE(ret.alphaShort == nitzShort);
    EXPECT_EQ(ret.mcc, stoi(networkNumeric.substr(0, 3)));
    EXPECT_EQ(ret.mnc, stoi(networkNumeric.substr(3)));
}

TEST(WifiOperatorInfoResolver, Resolve) {
    string simNumeric = "460010";
    OperatorInfo ret = WifiOperatorInfoResolver().resolve(simNumeric);
    EXPECT_TRUE(ret == npos);
}

TEST(RegisteredOperatorInfoResolver, Resolve) {
    string networkNumeric = "460010";
    string simNumeric = "460010";
    string iccId ="ABCDEFG";
    string spn = "Q-TELCOM";
    int invalidLac = -1;
    int regState = 1;
    EonsResolver &resolver = EonsResolver::createForPhoneId(0);
    PrepareEonsData(resolver);

    NetworkOperator netOperator(networkNumeric, "", "", regState, invalidLac, "");
    // OperatorNumericRule selected case
    OperatorInfo ret = RegisteredOperatorInfoResolver().resolve(
        netOperator,
        SimOperator(simNumeric, spn, iccId),
        resolver);
    EXPECT_TRUE(ret.numeric == networkNumeric);
    EXPECT_TRUE(ret.alphaLong == networkNumeric);
    EXPECT_TRUE(ret.alphaShort == networkNumeric);
    EXPECT_EQ(ret.mcc, stoi(networkNumeric.substr(0, 3)));
    EXPECT_EQ(ret.mnc, stoi(networkNumeric.substr(3)));

    networkNumeric = "20210";
    simNumeric = "20209";
    string alpha = "Q-TELCOM";

    netOperator.SetValues(networkNumeric, "", "", regState, invalidLac, "");
    // VendorCustomRule selected case
    ret = RegisteredOperatorInfoResolver().resolve(
        netOperator,
        SimOperator(simNumeric, spn, iccId),
        resolver);
    EXPECT_TRUE(ret.numeric == networkNumeric);
    EXPECT_TRUE(ret.alphaLong == alpha);
    EXPECT_TRUE(ret.alphaShort == alpha);
    EXPECT_EQ(ret.mcc, stoi(networkNumeric.substr(0, 3)));
    EXPECT_EQ(ret.mnc, stoi(networkNumeric.substr(3)));


    networkNumeric = "460010";
    simNumeric = "460010";
    int lac = 1;
    string longAlpha = "PLMN 5G";
    string shortAlpha = "PLMN 5G";

    netOperator.SetValues(networkNumeric, longAlpha, shortAlpha, regState, lac, "");
    // PnnOplRule selected case
    ret = RegisteredOperatorInfoResolver().resolve(
        netOperator,
        SimOperator(simNumeric, spn, iccId),
        resolver);
    EXPECT_TRUE(ret.numeric == networkNumeric);
    EXPECT_TRUE(ret.alphaLong == longAlpha);
    EXPECT_TRUE(ret.alphaShort == shortAlpha);
    EXPECT_EQ(ret.mcc, stoi(networkNumeric.substr(0, 3)));
    EXPECT_EQ(ret.mnc, stoi(networkNumeric.substr(3)));
}
