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
 * mcctable_test.cpp
 *
 *  Created on: 2021. 8. 4.
 */
#include <gtest/gtest.h>
#include "mcctable.h"
#include "librilutils/systemproperty.h"
#include "librilutils/textutils.h"

TEST(MccTable, MccEntry) {
    MccEntry entry1(450, "kr", 2);
    EXPECT_EQ(entry1.mcc, 450);
    EXPECT_TRUE(TextUtils::Equals(entry1.iso, "kr"));
    EXPECT_EQ(entry1.smallestDigitsMcc, 2);
    EXPECT_EQ(entry1.language, nullptr);

    MccEntry entry2(450, "kr", 2, "kr");
    EXPECT_EQ(entry2.mcc, 450);
    EXPECT_TRUE(TextUtils::Equals(entry2.iso, "kr"));
    EXPECT_EQ(entry2.smallestDigitsMcc, 2);
    EXPECT_TRUE(TextUtils::Equals(entry2.language, "kr"));
}

TEST(MccTable, MccTable) {
    MccTable *test = MccTable::GetTestInstance();
    ASSERT_NE(test, nullptr);
    if (test) {
        delete test;
    }
}

TEST(MccTable, GetEntryForMcc) {
    const MccEntry *test = MccTable::GetEntryForMcc(450);
    EXPECT_NE(test, nullptr);
    if (test) {
        EXPECT_EQ(test->mcc, 450);
    }

    EXPECT_EQ(MccTable::GetEntryForMcc(100), nullptr);
}

TEST(MccTable, GetCountryCodeForMcc) {
    EXPECT_TRUE(TextUtils::Equals(MccTable::GetCountryCodeForMcc(450), "kr"));
    EXPECT_TRUE(TextUtils::Equals(MccTable::GetCountryCodeForMcc(311), "us"));
    EXPECT_TRUE(TextUtils::IsEmpty(MccTable::GetCountryCodeForMcc(100)));
}

TEST(MccTable, GetDefaultLanguageForMcc) {
    EXPECT_TRUE(TextUtils::Equals(MccTable::GetDefaultLanguageForMcc(450), "ko"));
    EXPECT_TRUE(TextUtils::Equals(MccTable::GetDefaultLanguageForMcc(311), "en"));
    EXPECT_TRUE(TextUtils::IsEmpty(MccTable::GetDefaultLanguageForMcc(100)));
}

TEST(MccTable, GetSmallestDigitsMccForMcc) {
    EXPECT_EQ(MccTable::GetSmallestDigitsMccForMcc(450), 2);
    EXPECT_EQ(MccTable::GetSmallestDigitsMccForMcc(311), 3);
    EXPECT_EQ(MccTable::GetSmallestDigitsMccForMcc(100), 2);

    EXPECT_EQ(MccTable::GetSmallestDigitsMccForMcc("45005"), 2);
    EXPECT_EQ(MccTable::GetSmallestDigitsMccForMcc("450"), 2);
    EXPECT_EQ(MccTable::GetSmallestDigitsMccForMcc("311480"), 3);
    EXPECT_EQ(MccTable::GetSmallestDigitsMccForMcc("311"), 3);
    EXPECT_EQ(MccTable::GetSmallestDigitsMccForMcc("100"), 2);
    EXPECT_EQ(MccTable::GetSmallestDigitsMccForMcc("10"), 2);
}

TEST(MccTable, GetSmallestDigitsMccForImsi) {
    EXPECT_EQ(MccTable::GetSmallestDigitsMccForImsi(nullptr), -1);
    EXPECT_EQ(MccTable::GetSmallestDigitsMccForImsi("4500501234567891"), -1);
    EXPECT_EQ(MccTable::GetSmallestDigitsMccForImsi("45005012345678"), -1);
    EXPECT_EQ(MccTable::GetSmallestDigitsMccForImsi("40502912345678"), 3);
    EXPECT_EQ(MccTable::GetSmallestDigitsMccForImsi("50214212345678"), 3);
    EXPECT_EQ(MccTable::GetSmallestDigitsMccForImsi(std::string("50214212345678")), 3);
}

TEST(MccTable, FetchCarrierForImsi) {
    char carrier[7] = {};
    EXPECT_FALSE(MccTable::FetchCarrierForImsi("45005012345678", nullptr, 0));
    EXPECT_FALSE(MccTable::FetchCarrierForImsi(nullptr, carrier, sizeof(carrier)));
    EXPECT_FALSE(MccTable::FetchCarrierForImsi("4500501234567891", carrier, sizeof(carrier)));
    EXPECT_TRUE(MccTable::FetchCarrierForImsi("45005012345678", carrier, sizeof(carrier)));
    EXPECT_TRUE(TextUtils::Equals(carrier, "45005"));
    memset(carrier, 0, sizeof(carrier));
    EXPECT_TRUE(MccTable::FetchCarrierForImsi("31148012345678", carrier, sizeof(carrier)));
    EXPECT_TRUE(TextUtils::Equals(carrier, "311480"));
    memset(carrier, 0, sizeof(carrier));
    EXPECT_TRUE(MccTable::FetchCarrierForImsi("40502912345678", carrier, sizeof(carrier)));
    EXPECT_TRUE(TextUtils::Equals(carrier, "405029"));
    memset(carrier, 0, sizeof(carrier));
    EXPECT_FALSE(MccTable::FetchCarrierForImsi("45005912345678", carrier, 3));
}

TEST(MccTable, IsCarrierUsePnnOplForEons) {
    EXPECT_TRUE(MccTable::IsCarrierUsePnnOplForEons(""));
    EXPECT_FALSE(MccTable::IsCarrierUsePnnOplForEons("00101"));
    EXPECT_FALSE(MccTable::IsCarrierUsePnnOplForEons("334020"));
    EXPECT_FALSE(MccTable::IsCarrierUsePnnOplForEons("722310"));
}

TEST(MccTable, IsEsmFlagZeroOperator) {
    EXPECT_FALSE(MccTable::IsEsmFlagZeroOperator(""));
    EXPECT_TRUE(MccTable::IsEsmFlagZeroOperator("46000"));
    EXPECT_TRUE(MccTable::IsEsmFlagZeroOperator("46002"));
    EXPECT_TRUE(MccTable::IsEsmFlagZeroOperator("46007"));
    EXPECT_FALSE(MccTable::IsEsmFlagZeroOperator("45005"));

    std::string name = "persist.vendor.ril.test.esmzeroflag";
    std::string value = SystemProperty::Get(name, "");
    SystemProperty::Set(name, 1);
    EXPECT_TRUE(MccTable::IsEsmFlagZeroOperator("45005"));
    SystemProperty::Set(name, 0);
    EXPECT_FALSE(MccTable::IsEsmFlagZeroOperator("46000"));

    SystemProperty::Set(name, value);
}

TEST(MccTable, SetEsmFlagZeroOperator) {
    MccTable::SetEsmFlagZeroOperator("123456");
    EXPECT_TRUE(MccTable::IsEsmFlagZeroOperator("123456"));
}

TEST(MccTable, isValidPlmn) {
    EXPECT_FALSE(MccTable::isValidPlmn("45005F"));
    EXPECT_FALSE(MccTable::isValidPlmn("450"));
    EXPECT_FALSE(MccTable::isValidPlmn("4500567"));
    EXPECT_TRUE(MccTable::isValidPlmn("45005"));
}

TEST(MccTable, isUnknowNetwork) {
    EXPECT_EQ(MccTable::isUnknowNetwork("45005F"), -1);
    EXPECT_EQ(MccTable::isUnknowNetwork("45005"), -1);
    EXPECT_EQ(MccTable::isUnknowNetwork("72499"), 1);
}

TEST(MccTable, isMvnoNetwork) {
    EXPECT_EQ(MccTable::isMvnoNetwork("45005F"), -1);
    EXPECT_EQ(MccTable::isMvnoNetwork("45005"), -1);
    EXPECT_EQ(MccTable::isMvnoNetwork("72417"), 1);
    EXPECT_EQ(MccTable::isMvnoNetwork("73006"), 1);
    EXPECT_EQ(MccTable::isMvnoNetwork("732154"), 1);
    EXPECT_EQ(MccTable::isMvnoNetwork("74003"), 1);
}

TEST(MccTable, isUsingSpnForOperatorNameInRegHome) {
    EXPECT_FALSE(MccTable::isUsingSpnForOperatorNameInRegHome("45005F"));
    EXPECT_FALSE(MccTable::isUsingSpnForOperatorNameInRegHome("45005"));
    EXPECT_TRUE(MccTable::isUsingSpnForOperatorNameInRegHome("72234"));
    EXPECT_TRUE(MccTable::isUsingSpnForOperatorNameInRegHome("722310"));
    EXPECT_TRUE(MccTable::isUsingSpnForOperatorNameInRegHome("72405"));
    EXPECT_TRUE(MccTable::isUsingSpnForOperatorNameInRegHome("72406"));
    EXPECT_TRUE(MccTable::isUsingSpnForOperatorNameInRegHome("26002"));
    EXPECT_TRUE(MccTable::isUsingSpnForOperatorNameInRegHome("52000"));
    EXPECT_TRUE(MccTable::isUsingSpnForOperatorNameInRegHome("50503"));
}

TEST(MccTable, isNeedCheckPlmnMatcingForSpnUsing) {
    EXPECT_FALSE(MccTable::isNeedCheckPlmnMatcingForSpnUsing("45005F"));
    EXPECT_FALSE(MccTable::isNeedCheckPlmnMatcingForSpnUsing("45005"));
    EXPECT_TRUE(MccTable::isNeedCheckPlmnMatcingForSpnUsing("72405"));
}

TEST(MccTable, isNitzHasPriority) {
    EXPECT_FALSE(MccTable::isNitzHasPriority("45005", "45006"));
    EXPECT_FALSE(MccTable::isNitzHasPriority("45005F", "45005F"));
    EXPECT_FALSE(MccTable::isNitzHasPriority("45005", "45005"));
    EXPECT_TRUE(MccTable::isNitzHasPriority("71605", "71605"));
}

TEST(MccTable, isUsingSpnForAvailablePlmnSrch) {
    EXPECT_FALSE(MccTable::isUsingSpnForAvailablePlmnSrch("45005F"));
    EXPECT_FALSE(MccTable::isUsingSpnForAvailablePlmnSrch("45005"));
    EXPECT_TRUE(MccTable::isUsingSpnForAvailablePlmnSrch("722310"));
    EXPECT_TRUE(MccTable::isUsingSpnForAvailablePlmnSrch("26002"));
    EXPECT_TRUE(MccTable::isUsingSpnForAvailablePlmnSrch("26003"));
    EXPECT_TRUE(MccTable::isUsingSpnForAvailablePlmnSrch("50503"));
    EXPECT_TRUE(MccTable::isUsingSpnForAvailablePlmnSrch("732123"));
}
