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
#include <string>
#include <sstream>

#include "dataprofileinfo.h"
#include <librilutils/textutils.h>

using namespace std;
using std::string;

string ToString(const RIL_DataProfileInfo_V1_5& dpi)
{
    stringstream ss;
    ss << (int)dpi.profileId;
    ss << "/";
    ss << dpi.protocol;
    ss << "/";
    ss << dpi.authType;
    ss << "/";
    ss << dpi.apn;
    ss << "/";
    ss << dpi.user;
    ss << "/";
    ss << dpi.password;
    ss << "/";
    ss << dpi.type;
    ss << "/";
    ss << dpi.maxConnsTime;
    ss << "/";
    ss << dpi.maxConns;
    ss << "/";
    ss << dpi.waitTime;
    ss << "/";
    ss << dpi.enabled;
    ss << "/";
    ss << dpi.supportedApnTypesBitmap;
    ss << "/";
    ss << dpi.roamingProtocol;
    ss << "/";
    ss << dpi.bearerBitmap;
    ss << "/";
    ss << dpi.mtuV4;
    ss << "/";
    ss << dpi.mtuV6;
    ss << "/";
    ss << dpi.persistent;
    ss << "/";
    ss << dpi.preferred;
    return ss.str();
}

TEST(DataProfileInfo, DataProfileInfo_tostring) {

    RIL_DataProfileInfo_V1_5 testVector[] = {
        {
            .profileId = (RIL_DataProfileId) -1,
            .protocol = (PdpProtocolType) 2,
            .authType = (RIL_ApnAuthType) 0,
            .apn = (char *) "lte.sktelecom.com",
            .user = (char *) "",
            .password = (char *) "",
            .type = (RIL_DataProfileInfoType) 0,
            .maxConnsTime = 0,
            .maxConns = 0,
            .waitTime = 0,
            .enabled = 1,
            .supportedApnTypesBitmap = 183,
            .roamingProtocol = (PdpProtocolType) 0,
            .bearerBitmap = 0,
            .mtuV4 = 0,
            .mtuV6 = 0,
            .persistent = 0,
            .preferred = 1,
        },
        {
            .profileId = (RIL_DataProfileId) -1,
            .protocol = (PdpProtocolType) 1,
            .authType = (RIL_ApnAuthType) 0,
            .apn = (char *) "aa.bb.cc",
            .user = (char *) "test",
            .password = (char *) "testpwd",
            .type = (RIL_DataProfileInfoType) 2,
            .maxConnsTime = 1,
            .maxConns = 1,
            .waitTime = 0,
            .enabled = 0,
            .supportedApnTypesBitmap = 256,
            .roamingProtocol = (PdpProtocolType) 1,
            .bearerBitmap = 0,
            .mtuV4 = 0,
            .mtuV6 = 0,
            .persistent = 1,
            .preferred = 0,
        },
        {
            .profileId = (RIL_DataProfileId) -1,
            .protocol = (PdpProtocolType) 1,
            .authType = (RIL_ApnAuthType) 1,
            .apn = (char *) "",
            .user = (char *) "",
            .password = (char *) "",
            .type = (RIL_DataProfileInfoType) 0,
            .maxConnsTime = 0,
            .maxConns = 0,
            .waitTime = 0,
            .enabled = 0,
            .supportedApnTypesBitmap = 0,
            .roamingProtocol = (PdpProtocolType) 0,
            .bearerBitmap = 0,
            .mtuV4 = 1,
            .mtuV6 = 1,
            .persistent = 1,
            .preferred = 1,
        },
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {
        DataProfileInfo dpi(testVector[i]);
        string encodedData = dpi.ToString();
        EXPECT_TRUE(TextUtils::Equals(encodedData, ToString(testVector[i])));
        EXPECT_EQ(dpi.IsValid(), true);

        DataProfileInfo dpiString(encodedData);
        DataProfileInfo dpiCopy = dpiString;
        DataProfileInfo dpiAssign;
        dpiAssign = dpiString;

        EXPECT_TRUE(dpi == dpiString);
        EXPECT_TRUE(dpi == dpiCopy);
        EXPECT_TRUE(dpi == dpiAssign);

    }
}

TEST(DataProfileInfo, DataProfileInfo_decode) {
    string testVector[] = {
        "-1/2/0/lte.sktelecom.com///0/0/0/0/1/183/0/0/0/0/0/1",
        "-1/1/0/aa.bb.cc/dd/ee/1/1/0/0/1/256/0/0/0/0/1/1",
        "-1/0/0////0/0/0/0/1/0/0/0/0/0/0/0",
    };

    string testVector_failcase[] = {
        "",
        "-1/2/0/lte.sktelecom.com///0/0/0/0/1/183/0/",
        "-1/2/0/lte.sktelecom.com///0/0/0/0/1/183/////",
        "-1/2/0/lte.sktelecom.com///0/0/0/0/1/183/0/0/0/0",
    };

    // decode success
    int size = sizeof(testVector) / sizeof(testVector[0]);
    for (int i = 0; i < size; i++) {
        DataProfileInfo dpi(testVector[i]);
        EXPECT_TRUE(TextUtils::Equals(dpi.ToString(), testVector[i]));
        EXPECT_EQ(dpi.IsValid(), true);
    }

    // decode fail
    size = sizeof(testVector_failcase) / sizeof(testVector_failcase[0]);
    for (int i = 0; i < size; i++) {
        DataProfileInfo dpi(testVector_failcase[i]);
        EXPECT_EQ(dpi.IsValid(), false);
    }
}
