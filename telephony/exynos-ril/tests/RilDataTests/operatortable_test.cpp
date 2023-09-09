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
 * operatortable_test.cpp
 *
 *  Created on: 2021. 8. 3.
 *      Author: sungwoo48.choi
 */
#include <testutils.h>
#include <fcntl.h>
#include <unistd.h>
#include <librilutils/textutils.h>
#include "operatortable.h"
#include "ts25table.h"

using std::string;

TEST(OperatorNameProvider, OperatorContentValue) {
    const string emptyString("");
    const NetworkOperator netOp("45005", "test alpha long", "test alpha short", 0, 0, "*");

    OperatorContentValue test;
    EXPECT_TRUE(TextUtils::IsEmpty(test.GetShortPlmn()));
    EXPECT_TRUE(TextUtils::IsEmpty(test.GetLongPlmn()));
    test.Update({ "45005", "test alpha long", "test alpha short", 0, 0, "*"});
    EXPECT_TRUE(TextUtils::Equals(test.GetShortPlmn(), "test alpha short"));
    EXPECT_TRUE(TextUtils::Equals(test.GetLongPlmn(), "test alpha long"));

    OperatorContentValue copy = test;
    EXPECT_TRUE(copy == test);
    copy.SetLongPlmn("copy alpha long");
    EXPECT_TRUE(copy != test);
    OperatorContentValue copy2;
    copy2 = test;
    EXPECT_TRUE(copy2 == test);
    copy2.SetShortPlmn("copy alpha short");
    EXPECT_TRUE(copy2 != test);

    EXPECT_EQ(OperatorContentValue::NewInstance(INT_MAX, INT_MAX, 0, 0, 0), nullptr);
    OperatorContentValue *newInst =
            OperatorContentValue::NewInstance(450, 5, "test alpha long", "test alpha short", "*");
    EXPECT_NE(newInst, nullptr);
    if (newInst) {
        EXPECT_TRUE(*newInst == test);
        delete newInst;
    }
}

TEST(OperatorNameProvider, EonsContentsValue) {
    const string iccid("9891239791");
    EonsContentsValue dummy;
    EonsContentsValue test(iccid, { "45005", "test alpha long", "test alpha short", 0, 0, "*" });
    EXPECT_TRUE(TextUtils::Equals(test.GetIccId(), iccid));
    EonsContentsValue *newInst =
            EonsContentsValue::NewInstance(iccid, { "45005", "test alpha long", "test alpha short", 0, 0, "*" });
    EXPECT_NE(newInst, nullptr);
    if (newInst) {
        EXPECT_TRUE(*newInst == test);
        EXPECT_TRUE(newInst->GetIccId() == test.GetIccId());
        delete newInst;
    }
}

TEST(OperatorNameProvider, OperatorNameProvider) {
    OperatorNameProvider *test = OperatorNameProvider::MakeInstance();
    ASSERT_NE(test, nullptr);
    EXPECT_FALSE(test->isValidPlmn(""));
    EXPECT_FALSE(test->isValidPlmn("1234567"));
    EXPECT_FALSE(test->isValidPlmn("abcde"));
    EXPECT_TRUE(test->isValidPlmn("00101"));
    EXPECT_FALSE(test->isValidMccMnc(INT_MAX, 1));
    EXPECT_FALSE(test->isValidMccMnc(1, INT_MAX));
    OperatorNameProvider::ReleaseInstance();

    // table drop finally
    unlink("/data/vendor/rild/.operatortable.db");
}

TEST(OperatorNameProvider, OperatorNameProvider_query) {
    OperatorNameProvider *test = OperatorNameProvider::MakeInstance();
    ASSERT_NE(test, nullptr);
    EXPECT_TRUE(test->Contains(450, 6, "*"));
    EXPECT_FALSE(test->Contains(450, 15, "*"));
    EXPECT_EQ(test->Find(INT_MAX, INT_MAX, "*"), nullptr);

    // query from DB
    EXPECT_NE(test->Find(450, 5, "*"), nullptr);
    // cache hit
    EXPECT_NE(test->Find(450, 5, "*"), nullptr);
    // invalid
    EXPECT_EQ(test->Find(450, 15, "*"), nullptr);

    OperatorNameProvider::ReleaseInstance();

    // table drop finally
    unlink("/data/vendor/rild/.operatortable.db");
}

TEST(OperatorNameProvider, OperatorNameProvider_insert) {
    OperatorNameProvider *test = OperatorNameProvider::MakeInstance();
    ASSERT_NE(test, nullptr);

    // invalid mcc/mnc
    EXPECT_FALSE(test->Insert({ "", "alpha long", "alpha short", 0, 0, "*" }));
    EXPECT_FALSE(test->Insert(INT_MAX, INT_MAX, "alpha long", "alpha short", "*"));
    OperatorContentValue *contentValue =
            OperatorContentValue::NewInstance(INT_MAX, INT_MAX, "alpha long", "alpha short", nullptr);
    EXPECT_FALSE(test->Insert(contentValue));
    delete contentValue;

    // already existed
    contentValue = OperatorContentValue::NewInstance(450, 5, "alpha long", "alpha short", nullptr);
    EXPECT_FALSE(test->Insert(contentValue));
    delete contentValue;

    // invalid long/short EONS
    contentValue = OperatorContentValue::NewInstance(1, 1, 0, "alpha short", nullptr);
    EXPECT_FALSE(test->Insert(contentValue));
    delete contentValue;

    contentValue = OperatorContentValue::NewInstance(1, 1, "alpha long", 0, nullptr);
    EXPECT_FALSE(test->Insert(contentValue));
    delete contentValue;

    // insert
    contentValue = OperatorContentValue::NewInstance(1, 1, "alpha long", "alpha short", nullptr);
    EXPECT_TRUE(test->Insert(contentValue));

    OperatorContentValue *ret = test->Find(1, 1, "*");
    ASSERT_NE(ret, nullptr);
    EXPECT_TRUE(*contentValue == *ret);
    delete contentValue;

    // use PLMN but already existed
    EXPECT_FALSE(test->Insert({ "00101", "alpha long", "alpha short", 0, 0, "*" }));

    OperatorNameProvider::ReleaseInstance();

    // table drop finally
    unlink("/data/vendor/rild/.operatortable.db");
}

TEST(OperatorNameProvider, OperatorNameProvider_update) {
    OperatorNameProvider *test = OperatorNameProvider::MakeInstance();
    ASSERT_NE(test, nullptr);

    // invalid mcc/mnc
    EXPECT_FALSE(test->Update(INT_MAX, INT_MAX, "alpha long", "alpha short", "*"));
    OperatorContentValue *contentValue =
            OperatorContentValue::NewInstance(INT_MAX, INT_MAX, "alpha long", "alpha short", "*");;;
    EXPECT_FALSE(test->Update(contentValue));
    delete contentValue;

    // already existed
    contentValue = OperatorContentValue::NewInstance(1, 1, "alpha long", "alpha short", "*");
    EXPECT_FALSE(test->Update(contentValue));
    delete contentValue;

    // invalid long/short EONS
    contentValue = OperatorContentValue::NewInstance(450, 5, 0, "alpha short", "*");
    EXPECT_FALSE(test->Update(contentValue));
    delete contentValue;

    contentValue = OperatorContentValue::NewInstance(450, 5, "alpha long", 0, "*");
    EXPECT_FALSE(test->Update(contentValue));
    delete contentValue;

    // same
    contentValue = OperatorContentValue::NewInstance(450, 5, "SKTelecom", "SKTelecom", "*");
    EXPECT_FALSE(test->Update(contentValue));
    delete contentValue;

    // update
    contentValue = OperatorContentValue::NewInstance(450, 5, "alpha long", "alpha short", "*");
    EXPECT_TRUE(test->Update(contentValue));

    OperatorContentValue *ret = test->Find(450, 5, "*");
    EXPECT_NE(ret, nullptr);
    EXPECT_TRUE(*contentValue == *ret);
    delete contentValue;

    // use PLMN but already existed
    EXPECT_FALSE(test->Insert(NetworkOperator("45005", "alpha long", "alpha short", 0, 0, "*")));

    OperatorNameProvider::ReleaseInstance();

    // table drop finally
    unlink("/data/vendor/rild/.operatortable.db");
}

TEST(OperatorNameProvider, OperatorNameProvider_eons_with_iccid) {
    OperatorNameProvider *test = OperatorNameProvider::MakeInstance();
    ASSERT_NE(test, nullptr);

    const char *iccid = "8982300215013945185";
    EXPECT_EQ(test->FindEons("", "45005"), nullptr);
    EXPECT_EQ(test->FindEons(iccid, "1002345"), nullptr);
    EXPECT_EQ(test->FindEons(iccid, "45005"), nullptr);

    const int testNetworkOperatorSize = 5;
    const NetworkOperator networkOperator[testNetworkOperatorSize] = {
        NetworkOperator("00101", "alpha long", "alpha short", 0, 0, "*"),
        NetworkOperator("", "alpha_long", "alpha short", 0, 0, "*"),
        NetworkOperator("00101", "", "alpha short", 0, 0, "*"),
        NetworkOperator("00101", "alpha long", "", 0, 0, "*"),
        NetworkOperator("45005", "alpha long", "alpha short", 0, 0, "*"),
    };

    EXPECT_FALSE(test->InsertEons("", networkOperator[0]));
    EXPECT_FALSE(test->InsertEons(iccid, networkOperator[1]));
    EXPECT_FALSE(test->InsertEons(iccid, networkOperator[2]));
    EXPECT_FALSE(test->InsertEons(iccid, networkOperator[3]));

    EXPECT_TRUE(test->InsertEons(iccid, networkOperator[0]));
    EXPECT_EQ(test->FindEons(iccid, "45005"), nullptr);
    EonsContentsValue *ret = test->FindEons(iccid, networkOperator[0].GetNumeric());
    EXPECT_NE(ret, nullptr);
    if (ret) {
        EXPECT_TRUE(ret->GetLongPlmn() == networkOperator[0].GetAlphaLong());
        EXPECT_TRUE(ret->GetShortPlmn() == networkOperator[0].GetAlphaShort());
        EXPECT_EQ(ret->GetMcc(), networkOperator[0].GetMcc());
        EXPECT_EQ(ret->GetMnc(), networkOperator[0].GetMnc());
    }

    EXPECT_FALSE(test->UpdateEons(iccid, networkOperator[4]));

    const NetworkOperator NetworkOperatorUpdated("00101", "alpha long2", "alpha short2", 0, 0, "*");
    EXPECT_TRUE(test->UpdateEons(iccid, NetworkOperatorUpdated));
    ret = test->FindEons(iccid, NetworkOperatorUpdated.GetNumeric());
    EXPECT_NE(ret, nullptr);
    if (ret) {
        EXPECT_TRUE(ret->GetLongPlmn() == NetworkOperatorUpdated.GetAlphaLong());
        EXPECT_TRUE(ret->GetShortPlmn() == NetworkOperatorUpdated.GetAlphaShort());
        EXPECT_EQ(ret->GetMcc(), NetworkOperatorUpdated.GetMcc());
        EXPECT_EQ(ret->GetMnc(), NetworkOperatorUpdated.GetMnc());
    }

    OperatorNameProvider::ReleaseInstance();

    // table drop finally
    unlink("/data/vendor/rild/.operatortable.db");
}

TEST(OperatorNameProvider, OperatorNameProvider_DB_exception) {
    // table drop first
    unlink("/data/vendor/rild/.operatortable.db");
    // make a dummy file with a same name
    // and keep opening status until at the end of TC
    int fd = open("/data/vendor/rild/.operatortable.db", O_CREAT | O_WRONLY | O_TRUNC | O_CLOEXEC, S_IWUSR);
    OperatorNameProvider *test = OperatorNameProvider::MakeInstance();
    ASSERT_NE(test, nullptr);
    // load from cache
    EXPECT_NE(test->Find(450, 5, "*"), nullptr);
    OperatorNameProvider::ReleaseInstance();
    if (fd > 0) {
        close(fd);
    }

    // table drop finally
    unlink("/data/vendor/rild/.operatortable.db");
}

TEST(OperatorNameProvider, OperatorNameProvider_etc) {
    OperatorNameProvider::MakeInstance();

    OperatorContentValue ret =  OperatorNameProvider::GetVendorCustomOperatorName("20209", "20210");
    EXPECT_TRUE(ret.GetLongPlmn() == string("Q-TELCOM"));
    EXPECT_TRUE(ret.GetShortPlmn() == string("Q-TELCOM"));
    EXPECT_EQ(ret.GetMcc(), 202);
    EXPECT_EQ(ret.GetMnc(), 10);
    ret =  OperatorNameProvider::GetVendorCustomOperatorName("45005", "45005");

    OperatorNameProvider::ReleaseInstance();

    // table drop finally
    unlink("/data/vendor/rild/.operatortable.db");
}
