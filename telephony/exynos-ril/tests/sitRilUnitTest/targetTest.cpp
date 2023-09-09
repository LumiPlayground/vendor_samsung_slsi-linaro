/*
 *  SIT RIL Unit test
 *
 *  Copyright Samsung Electronics Co., LTD.
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 */

#include <gtest/gtest.h>
#include <sitrilTestCs.h>
#include <sitrilTestRadioPower.h>
#include <sitrilTestNetwork.h>
#include <sitrilTestPs.h>
#include <sitrilTestSim.h>
#include <sitrilTestSms.h>
#include <sitrilTestMisc.h>
#include <sitrilTestMiscOem.h>
#include <sitrilTestEmbms.h>
#include <sitrilTestIms.h>
#include <sitrilTestGps.h>
#include <sitrilTestVsim.h>
#include <sitrilTestAudio.h>
#include <sitrilTestSupp.h>
#include <sitrilTestUtil.h>
#include <sitrilTestSap.h>
#include <sitrilTestImsMedia.h>

//Extern test
#include "sitrilExternTestAudio.h"
#include "sitrilExternTestGps.h"

/* sitril lib */

::std::vector<int> GetParameterInts() {
    ::std::vector<int> v;
    v.push_back(0);    // for Phone0
//    v.push_back(1);    // for Phone1
    return v;
}

INSTANTIATE_TEST_SUITE_P(Phone, SitrilTestCs,
    testing::ValuesIn(GetParameterInts())
);

INSTANTIATE_TEST_SUITE_P(Phone, SitrilTestRadioPower,
    testing::ValuesIn(GetParameterInts())
);

INSTANTIATE_TEST_SUITE_P(Phone, SitrilTestNetwork,
    testing::ValuesIn(GetParameterInts())
);

INSTANTIATE_TEST_SUITE_P(Phone, SitrilTestPs,
    testing::ValuesIn(GetParameterInts())
);

INSTANTIATE_TEST_SUITE_P(Phone, SitrilTestSim,
    testing::ValuesIn(GetParameterInts())
);

INSTANTIATE_TEST_SUITE_P(Phone, SitrilTestSms,
    testing::ValuesIn(GetParameterInts())
);

INSTANTIATE_TEST_SUITE_P(Phone, SitrilTestMisc,
    testing::ValuesIn(GetParameterInts())
);

INSTANTIATE_TEST_SUITE_P(Phone, SitrilTestMiscOem,
    testing::ValuesIn(GetParameterInts())
);

INSTANTIATE_TEST_SUITE_P(Phone, SitrilTestEmbms,
    testing::ValuesIn(GetParameterInts())
);

INSTANTIATE_TEST_SUITE_P(Phone, SitrilTestIms,
    testing::ValuesIn(GetParameterInts())
);

INSTANTIATE_TEST_SUITE_P(Phone, SitrilTestImsMedia,
        testing::ValuesIn(GetParameterInts())
);

INSTANTIATE_TEST_SUITE_P(Phone, SitrilTestGps,
    testing::ValuesIn(GetParameterInts())
);

INSTANTIATE_TEST_SUITE_P(Phone, SitrilTestVsim,
    testing::ValuesIn(GetParameterInts())
);

INSTANTIATE_TEST_SUITE_P(Phone, SitrilTestAudio,
    testing::ValuesIn(GetParameterInts())
);

INSTANTIATE_TEST_SUITE_P(Phone, SitrilTestSupp,
    testing::ValuesIn(GetParameterInts())
);

INSTANTIATE_TEST_SUITE_P(Phone, SitrilTestUtil,
    testing::ValuesIn(GetParameterInts())
);

INSTANTIATE_TEST_SUITE_P(Phone, SitrilTestSap,
    testing::ValuesIn(GetParameterInts())
);

//Extern test
INSTANTIATE_TEST_SUITE_P(Phone, SitrilExternTestAudio,
    testing::ValuesIn(GetParameterInts())
);

INSTANTIATE_TEST_SUITE_P(Phone, SitrilExternTestGps,
    testing::ValuesIn(GetParameterInts())
);

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);

    // setup seed for rand function
    int seedSrand = time(NULL);
    std::cout << "seed setup for random function (radio):" + std::to_string(seedSrand) << std::endl;
    srand(seedSrand);

    SitrilTest::CreateInstance();
    //SitrilTest *pSitrilTest = SitrilTest::CreateInstance();

    /* Becasuse of SIT_IND_RADIO_STATE_CHANGED which is triggred forcely,
     * there are some IPC operaton.
     * In order to wait for their TIME OUT, the below code is added.
     */
    sleep(3);

    // ::testing::GTEST_FLAG(filter) = "Phone/SitrilTestNetwork.*";
    int status = RUN_ALL_TESTS();
    std::cout <<  "Test result = " << status;

    //delete pSitrilTest;
    //SitrilTest::SetInstance(nullptr);

    return status;
}
