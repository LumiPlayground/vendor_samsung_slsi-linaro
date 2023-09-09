/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd All Rights Reserved
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

#include "modemdatabuilder.h"
#include "modemdata.h"
#include "rildef.h"
#include "protocoladapter.h"
#include "legacy/uicc/protocolsimbuilderlegacy.h"
#include "util.h"

#define    SWAP16(val)        ( (((val) << 8) & 0xFF00) | (((val) >> 8) & 0x00FF) )

static const char aszFacCode[SIT_SIM_FAC_MAX][MAX_FACILITY_CODE_LEN + 1] = { 
    "CS", "PS", "PF", "SC", "AO", "OI", "OX", "AI", "IR", "NT",
    "NM", "NS", "NA", "AB", "AG", "AC", "FD", "PN", "PU", "PP",
    "PC", "SC2"
};

TEST(ProtocolSimBuilderLegacy, BuildSimGetStatus) {
    ProtocolSimBuilderLegacy testBuilder;
    ModemData *modemData = testBuilder.BuildSimGetStatus();
    ASSERT_NE(modemData, nullptr);
    EXPECT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_SIM_STATUS);
    EXPECT_EQ(testAdapter.GetLength(), sizeof(null_data_format));
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolSimBuilderLegacy, BuildSimVerifyPin) {
    struct {
        int nPinIndex;
        char *pszPin;
        char *pszAID;
        uint32_t nReqId;
    } testVector[] = {
        {0 /* PIN1_PUK1 */, (char*)"pin1", (char*)"123", SIT_VERIFY_SIM_PIN},
        {1 /* PIN2_PUK2 */, (char*)"pin2", (char*)"1234", SIT_VERIFY_SIM_PIN2},
        {0 /* PIN2_PUK2 */, (char*)"1234567890", (char*)"1234", SIT_VERIFY_SIM_PIN},
        // Error Case
        {2 /* ERROR */, (char*)"pin1", (char*)"123", SIT_VERIFY_SIM_PIN},
        {0 /* ERROR */, nullptr, (char*)"123", SIT_VERIFY_SIM_PIN},
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);
    ProtocolSimBuilderLegacy testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildSimVerifyPin(testVector[i].nPinIndex,
                testVector[i].pszPin, testVector[i].pszAID);
        if (i >= 3) {
            EXPECT_EQ(modemData, nullptr);
            continue;
        }

        ASSERT_NE(modemData, nullptr);
        EXPECT_NE(modemData, nullptr);

        ProtocolReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), testVector[i].nReqId);
        EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_sim_verify_sim_pin_req));
        EXPECT_EQ(testAdapter.GetParameterLength(),
                sizeof(sit_sim_verify_sim_pin_req) - sizeof(RCM_HEADER));

        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        sit_sim_verify_sim_pin_req *p = (sit_sim_verify_sim_pin_req *)modemData->GetRawData();
        for (int j = 0; j < p->pin_len; j++) {
            EXPECT_EQ(p->pin[j], testVector[i].pszPin[j]);
        }
        char aidHexStr[MAX_SIM_AID_LEN * 2] = {};
        Value2HexString(aidHexStr, p->aid, p->aid_len);
        for (int j = 0; j < (int) strlen(testVector[i].pszAID); j++) {
            EXPECT_EQ(aidHexStr[j], testVector[i].pszAID[j]);
        }
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSimBuilderLegacy, BuildSimVerifyPuk) {
    struct {
        int nPinIndex;
        char *pszPuk;
        char *pszNewPin;
        char *pszAID;
        uint32_t nReqId;
    } testVector[] = {
        {0, (char*)"12345678", (char*)"1234", (char*)"AD00", SIT_VERIFY_SIM_PUK},
        {1, (char*)"87654321", (char*)"1234", (char*)"AD01", SIT_VERIFY_SIM_PUK2},
        {0, (char*)"1234567890", (char*)"1234", (char*)"AD02", SIT_VERIFY_SIM_PUK},
        {0, (char*)"12345678", (char*)"1234567890", (char*)"AD03", SIT_VERIFY_SIM_PUK},
        // Error Case
        {2, (char*)"12345678", (char*)"1234", (char*)"AD04", SIT_VERIFY_SIM_PUK},
        {0, nullptr, (char*)"1234", (char*)"AD05", SIT_VERIFY_SIM_PUK},
        {0, (char*)"12345678", nullptr, (char*)"AD06", SIT_VERIFY_SIM_PUK},
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);
    ProtocolSimBuilderLegacy testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildSimVerifyPuk(testVector[i].nPinIndex,
                testVector[i].pszPuk, testVector[i].pszNewPin, testVector[i].pszAID);
        if (i >= 4) {
            EXPECT_EQ(modemData, nullptr);
            continue;
        }

        ASSERT_NE(modemData, nullptr);
        EXPECT_NE(modemData, nullptr);

        ProtocolReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), testVector[i].nReqId);
        EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_sim_verify_sim_puk_req));
        EXPECT_EQ(testAdapter.GetParameterLength(),
                sizeof(sit_sim_verify_sim_puk_req) - sizeof(RCM_HEADER));

        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        sit_sim_verify_sim_puk_req *p = (sit_sim_verify_sim_puk_req *)modemData->GetRawData();
        for (int j = 0; j < p->puk_len; j++) {
            EXPECT_EQ(p->puk[j], testVector[i].pszPuk[j]);
        }
        for (int j = 0; j < p->new_pin_len; j++) {
            EXPECT_EQ(p->new_pin[j], testVector[i].pszNewPin[j]);
        }
        char aidHexStr[MAX_SIM_AID_LEN * 2] = {};
        Value2HexString(aidHexStr, p->aid, p->aid_len);
        for (int j = 0; j < (int) strlen(testVector[i].pszAID); j++) {
            EXPECT_EQ(aidHexStr[j], testVector[i].pszAID[j]);
        }
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSimBuilderLegacy, BuildSimChangePin) {
    struct {
        int nPinIndex;
        char *pszOldPin;
        char *pszNewPin;
        char *pszAID;
        uint32_t nReqId;
    } testVector[] = {
        {0, (char*)"1234", (char*)"4321", (char*)"AD00", SIT_CHG_SIM_PIN},
        {1, (char*)"4321", (char*)"1234", (char*)"AD01", SIT_CHG_SIM_PIN2},
        {0, (char*)"1234567890", (char*)"1234", (char*)"AD02", SIT_CHG_SIM_PIN},
        {0, (char*)"1234", (char*)"0987654321", (char*)"AD03", SIT_CHG_SIM_PIN},
        // Error Case
        {2, (char*)"1234", (char*)"4321", (char*)"AD04", SIT_CHG_SIM_PIN},
        {0, nullptr, (char*)"1234", (char*)"AD05", SIT_CHG_SIM_PIN},
        {0, (char*)"1234", nullptr, (char*)"AD06", SIT_CHG_SIM_PIN},
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);
    ProtocolSimBuilderLegacy testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildSimChangePin(testVector[i].nPinIndex,
                testVector[i].pszOldPin, testVector[i].pszNewPin, testVector[i].pszAID);
        if (i >= 4) {
            EXPECT_EQ(modemData, nullptr);
            continue;
        }

        ASSERT_NE(modemData, nullptr);
        EXPECT_NE(modemData, nullptr);

        ProtocolReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), testVector[i].nReqId);
        EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_sim_change_sim_pin_req));
        EXPECT_EQ(testAdapter.GetParameterLength(),
                sizeof(sit_sim_change_sim_pin_req) - sizeof(RCM_HEADER));

        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        sit_sim_change_sim_pin_req *p = (sit_sim_change_sim_pin_req *)modemData->GetRawData();
        for (int j = 0; j < p->old_pin_len; j++) {
            EXPECT_EQ(p->old_pin[j], testVector[i].pszOldPin[j]);
        }
        for (int j = 0; j < p->new_pin_len; j++) {
            EXPECT_EQ(p->new_pin[j], testVector[i].pszNewPin[j]);
        }
        char aidHexStr[MAX_SIM_AID_LEN * 2] = {};
        Value2HexString(aidHexStr, p->aid, p->aid_len);
        for (int j = 0; j < (int) strlen(testVector[i].pszAID); j++) {
            EXPECT_EQ(aidHexStr[j], testVector[i].pszAID[j]);
        }
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSimBuilderLegacy, BuildSimVerifyNetworkLock) {
    struct {
        int nFac;
        char *pszPassword;
        int nSvcClass;
        char *pszAID;
    } testVector[] = {
        // MAX_SIM_FACILITY_PASSWORD_LEN : 39
        {0, (char*)"1234", 0, (char*)"AD00"},
        {0, (char*)"1234567890123456789012345678901234567890", 0, (char*)"AD01"},
        // Error Case
        {0, nullptr, 0, (char*)"AD02"},
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);
    ProtocolSimBuilderLegacy testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildSimVerifyNetworkLock(testVector[i].nFac,
                testVector[i].pszPassword, testVector[i].nSvcClass, testVector[i].pszAID);
        if (i >= 2) {
            EXPECT_EQ(modemData, nullptr);
            continue;
        }

        ASSERT_NE(modemData, nullptr);
        EXPECT_NE(modemData, nullptr);

        ProtocolReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_VERIFY_NETWORK_LOCK);
        EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_sim_verify_network_lock_req));
        EXPECT_EQ(testAdapter.GetParameterLength(),
                sizeof(sit_sim_verify_network_lock_req) - sizeof(RCM_HEADER));

        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        sit_sim_verify_network_lock_req *p =
                (sit_sim_verify_network_lock_req *)modemData->GetRawData();
        EXPECT_EQ(p->fac, testVector[i].nFac);
        for (int j = 0; j < p->password_len; j++) {
            EXPECT_EQ(p->password[j], testVector[i].pszPassword[j]);
        }
        EXPECT_EQ(p->service_class, testVector[i].nSvcClass);
        char aidHexStr[MAX_SIM_AID_LEN * 2] = {};
        Value2HexString(aidHexStr, p->aid, p->aid_len);
        for (int j = 0; j < (int) strlen(testVector[i].pszAID); j++) {
            EXPECT_EQ(aidHexStr[j], testVector[i].pszAID[j]);
        }
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSimBuilderLegacy, BuildSimIO) {
    struct {
        int nCmd;
        int nAppType;
        int nFileID;
        const char *pPath;
        int p1;
        int p2;
        int p3;
        const char *pData;
        const char *pszPin2;
        const char *pszAID;
    } testVector[] = {
        // MAX_SIM_PIN_LEN : 8
        {0xB0, 0, 0x6FC8, (char*)"3F007FFF", 1, 2, 3, (char*)"ABAB", (char*)"1234", (char*)"AD00"},
        {0xB0, 0, 0x6FC8, (char*)"3F007FFF", 1, 2, 3, (char*)"ABAB", (char*)"1234567890", (char*)"AD01"},

        // Error Case
        {0, 0, 0x6FC8, nullptr, 1, 2, 3, (char*)"ABAB", (char*)"1234", (char*)"AD02"},
        {0, 0, 0x6FC8, (char*)"3F007FFF", 1, 2, 3, nullptr, (char*)"1234", (char*)"AD03"},
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);
    ProtocolSimBuilderLegacy testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildSimIO(testVector[i].nCmd,
                testVector[i].nAppType, testVector[i].nFileID, testVector[i].pPath,
                testVector[i].p1, testVector[i].p2, testVector[i].p3, 0, testVector[i].pData,
                testVector[i].pszPin2, testVector[i].pszAID);
        if (i >= 2) {
            EXPECT_EQ(modemData, nullptr);
            continue;
        }

        ASSERT_NE(modemData, nullptr);
        EXPECT_NE(modemData, nullptr);

        ProtocolReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_SIM_IO);
        EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_sim_sim_io_req));
        EXPECT_EQ(testAdapter.GetParameterLength(),
                sizeof(sit_sim_sim_io_req) - sizeof(RCM_HEADER));

        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        sit_sim_sim_io_req *p =
                (sit_sim_sim_io_req *)modemData->GetRawData();

        EXPECT_EQ(p->command, testVector[i].nCmd);
        EXPECT_EQ(p->app_type, testVector[i].nAppType);
        EXPECT_EQ(p->file_id, testVector[i].nFileID);

        char tmpHexString[MAX_SIM_IO_DATA_LEN * 2] = {};    // max buffer size
        BYTE tmpPath[MAX_SIM_IO_PATH_LEN] = {};
        memcpy(tmpPath, p->path, p->path_len);
        // Swap for Big-Endian
        for (int j = 0; j < p->path_len; j += 2) {
            *((WORD *) &tmpPath[j]) = SWAP16(*((WORD *) &tmpPath[j]));
        }
        Value2HexString(tmpHexString, tmpPath, p->path_len);
        for (int j = 0; j < (int) strlen(testVector[i].pPath); j++) {
            EXPECT_EQ(tmpHexString[j], testVector[i].pPath[j]);
        }

        EXPECT_EQ(p->p1, testVector[i].p1);
        EXPECT_EQ(p->p2, testVector[i].p2);
        EXPECT_EQ(p->p3, testVector[i].p3);

        memset(tmpHexString, 0x00, sizeof(tmpHexString));
        Value2HexString(tmpHexString, p->data, p->data_len);
        for (int j = 0; j < (int) strlen(testVector[i].pData); j++) {
            EXPECT_EQ(tmpHexString[j], testVector[i].pData[j]);
        }

        for (int j = 0; j < p->pin2_len; j++) {
            EXPECT_EQ(p->pin2[j], testVector[i].pszPin2[j]);
        }

        memset(tmpHexString, 0x00, sizeof(tmpHexString));
        Value2HexString(tmpHexString, p->aid, p->aid_len);
        for (int j = 0; j < (int) strlen(testVector[i].pszAID); j++) {
            EXPECT_EQ(tmpHexString[j], testVector[i].pszAID[j]);
        }

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSimBuilderLegacy, BuildSimGetFacilityLock) {
    struct {
        char *pszCode;
        char *pszPassword;
        int nSvcClass;
        char *pszAID;
    } testVector[] = {
        {(char*)"SC2", (char*)"1234", 0, (char*)"AD00"},
        {(char*)"CS", (char*)"1234567890123456789012345678901234567890", 0, (char*)"AD01"},

        // Error Case
        {(char*)"CS", nullptr, 0, (char*)"AD02"},
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);
    ProtocolSimBuilderLegacy testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildSimGetFacilityLock(testVector[i].pszCode,
                testVector[i].pszPassword, testVector[i].nSvcClass, testVector[i].pszAID);
        if (i >= 2) {
            EXPECT_EQ(modemData, nullptr);
            continue;
        }

        ASSERT_NE(modemData, nullptr);
        EXPECT_NE(modemData, nullptr);

        ProtocolReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_GET_FACILITY_LOCK);
        EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_sim_get_facility_lock_req));
        EXPECT_EQ(testAdapter.GetParameterLength(),
                sizeof(sit_sim_get_facility_lock_req) - sizeof(RCM_HEADER));

        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        sit_sim_get_facility_lock_req *p =
                (sit_sim_get_facility_lock_req *)modemData->GetRawData();

        for (int j = 0; j < (int)strlen(testVector[i].pszCode); j++) {
            EXPECT_EQ(aszFacCode[p->code][j], testVector[i].pszCode[j]);
        }

        for (int j = 0; j < p->password_len; j++) {
            EXPECT_EQ(p->password[j], testVector[i].pszPassword[j]);
        }
        EXPECT_EQ(p->service_class, testVector[i].nSvcClass);

        char aidHexStr[MAX_SIM_AID_LEN * 2] = {};
        Value2HexString(aidHexStr, p->aid, p->aid_len);
        for (int j = 0; j < (int) strlen(testVector[i].pszAID); j++) {
            EXPECT_EQ(aidHexStr[j], testVector[i].pszAID[j]);
        }
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSimBuilderLegacy, BuildSimSetFacilityLock) {
    struct {
        char *pszCode;
        int nLockMode;
        char *pszPassword;
        int nSvcClass;
        char *pszAID;
    } testVector[] = {
        {(char*)"SC2", 0, (char*)"1234", 0, (char*)"AD00"},
        {(char*)"CS", 1, (char*)"1234567890123456789012345678901234567890", 0, (char*)"AD01"},

        // Error Case
        {(char*)"CS", 0, nullptr, 0, (char*)"AD02"},
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);
    ProtocolSimBuilderLegacy testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildSimSetFacilityLock(testVector[i].pszCode,
                testVector[i].nLockMode, testVector[i].pszPassword, testVector[i].nSvcClass,
                testVector[i].pszAID);
        if (i >= 2) {
            EXPECT_EQ(modemData, nullptr);
            continue;
        }

        ASSERT_NE(modemData, nullptr);
        EXPECT_NE(modemData, nullptr);

        ProtocolReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_SET_FACILITY_LOCK);
        EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_sim_set_facility_lock_req));
        EXPECT_EQ(testAdapter.GetParameterLength(),
                sizeof(sit_sim_set_facility_lock_req) - sizeof(RCM_HEADER));

        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        sit_sim_set_facility_lock_req *p =
                (sit_sim_set_facility_lock_req *)modemData->GetRawData();

        for (int j = 0; j < (int)strlen(testVector[i].pszCode); j++) {
            EXPECT_EQ(aszFacCode[p->code][j], testVector[i].pszCode[j]);
        }
        EXPECT_EQ(p->lock_mode, testVector[i].nLockMode);
        for (int j = 0; j < p->password_len; j++) {
            EXPECT_EQ(p->password[j], testVector[i].pszPassword[j]);
        }
        EXPECT_EQ(p->service_class, testVector[i].nSvcClass);

        char aidHexStr[MAX_SIM_AID_LEN * 2] = {};
        Value2HexString(aidHexStr, p->aid, p->aid_len);
        for (int j = 0; j < (int) strlen(testVector[i].pszAID); j++) {
            EXPECT_EQ(aidHexStr[j], testVector[i].pszAID[j]);
        }
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSimBuilderLegacy, BuildSimGetIsimAuth) {
    struct {
        int nAuthType;
        BYTE *pAuth;
        int nAuthLengh;
    } testVector[] = {
        {1, (BYTE*)"1234567890", 10},
        {2, (BYTE*)"12345678901234567890123456789012345678901234567890123456789012345678901234567890", 80},
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);
    ProtocolSimBuilderLegacy testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildSimGetIsimAuth(testVector[i].nAuthType,
                testVector[i].pAuth, testVector[i].nAuthLengh);
        ASSERT_NE(modemData, nullptr);
        EXPECT_NE(modemData, nullptr);

        ProtocolReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_GET_SIM_AUTH);
        EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_sim_get_sim_auth_req));
        EXPECT_EQ(testAdapter.GetParameterLength(),
                sizeof(sit_sim_get_sim_auth_req) - sizeof(RCM_HEADER));

        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        sit_sim_get_sim_auth_req *p =
                (sit_sim_get_sim_auth_req *)modemData->GetRawData();

        EXPECT_EQ(p->auth_type, testVector[i].nAuthType);
        if (MAX_SIM_AUTH_REQ_LEN < testVector[i].nAuthLengh) {
            EXPECT_EQ(p->auth_len, MAX_SIM_AUTH_REQ_LEN);
        } else {
            EXPECT_EQ(p->auth_len, testVector[i].nAuthLengh);
        }
        for (int j = 0; j < p->auth_len; j++) {
            EXPECT_EQ(p->auth[j], testVector[i].pAuth[j]);
        }

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSimBuilderLegacy, BuildSimGetSimAuth) {
    struct {
        /* _____XXX
         * 000 : GSM context
         * 001 : 3G context
         * 010 : VGCS/VBS context
         * 100 : GBA context
         */
        int nAuthContext;
        BYTE *pAuth;
        int nAuthLengh;
        int nAppType;
    } testVector[] = {
        {0b000, (BYTE*)"1234567890", 10, RIL_APPTYPE_USIM},
        {0b001, (BYTE*)"1234567890", 10, RIL_APPTYPE_ISIM},
        {0b001, (BYTE*)"12345678901234567890123456789012345678901234567890123456789012345678901234567890", 80, RIL_APPTYPE_USIM},

        // Error
        {0b010, (BYTE*)"1234567890", 10, RIL_APPTYPE_USIM},
        {0b100, (BYTE*)"1234567890", 10, RIL_APPTYPE_USIM},
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);
    ProtocolSimBuilderLegacy testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildSimGetSimAuth(testVector[i].nAuthContext,
                testVector[i].pAuth, testVector[i].nAuthLengh, testVector[i].nAppType);
        if (i >= 3) {
            EXPECT_EQ(modemData, nullptr);
            continue;
        }

        ASSERT_NE(modemData, nullptr);
        EXPECT_NE(modemData, nullptr);

        ProtocolReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_GET_SIM_AUTH);
        EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_sim_get_sim_auth_req));
        EXPECT_EQ(testAdapter.GetParameterLength(),
                sizeof(sit_sim_get_sim_auth_req) - sizeof(RCM_HEADER));
        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        sit_sim_get_sim_auth_req *p =
                (sit_sim_get_sim_auth_req *)modemData->GetRawData();

        const int AUTH_GSM_CONTEXT = 0b00000000;
        const int AUTH_3G_CONTEXT = 0b00000001;
        int nAuthContext = 10;
        // printf("p->auth_type : %d\n", p->auth_type);
        switch (p->auth_type) {
        case SIT_SIM_AUTH_GSM :
            nAuthContext = AUTH_GSM_CONTEXT;
            break;
        case SIT_SIM_AUTH_3G :
            nAuthContext = AUTH_3G_CONTEXT;
            break;
        case SIT_SIM_AUTH_IMS :
            // When nAppType is ISIM, AuthType is set regardless of AuthContext.
            // Do skip to check nAuthContext;
            nAuthContext = -1;
            EXPECT_EQ(RIL_APPTYPE_ISIM, testVector[i].nAppType);
            break;
        }
        if (0 <= nAuthContext) {
            EXPECT_EQ(nAuthContext, testVector[i].nAuthContext);
        }
        if (MAX_SIM_AUTH_REQ_LEN < testVector[i].nAuthLengh) {
            EXPECT_EQ(p->auth_len, MAX_SIM_AUTH_REQ_LEN);
        } else {
            EXPECT_EQ(p->auth_len, testVector[i].nAuthLengh);
        }

        for (int j = 0; j < p->auth_len; j++) {
            EXPECT_EQ(p->auth[j], testVector[i].pAuth[j]);
        }

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSimBuilderLegacy, BuildSimTransmitApduBasic) {
    struct {
        int nSessionID;
        int cla;
        int instruction;
        int p1;
        int p2;
        int p3;
        char *pszApduData;
    } testVector[] = {
        {0, 1, 2, 1, 2, 3, (char*)"ABCDEF"},
        {0, 1, 2, 1, 2, 3, (char*)""}
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);
    ProtocolSimBuilderLegacy testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildSimTransmitApduBasic(testVector[i].nSessionID,
                testVector[i].cla, testVector[i].instruction, testVector[i].p1,
                testVector[i].p2, testVector[i].p3, testVector[i].pszApduData);
        if (i >= 2) {
            EXPECT_EQ(modemData, nullptr);
            continue;
        }

        ASSERT_NE(modemData, nullptr);
        EXPECT_NE(modemData, nullptr);

        ProtocolReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_TRANSMIT_SIM_APDU_BASIC);
        uint32_t modemDataSize =
                sizeof(sit_sim_transmit_sim_apdu_basic_req) + strlen(testVector[i].pszApduData);
        EXPECT_EQ(testAdapter.GetLength(), modemDataSize);
        EXPECT_EQ(testAdapter.GetParameterLength(), modemDataSize - sizeof(RCM_HEADER));

        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        sit_sim_transmit_sim_apdu_basic_req *p =
                (sit_sim_transmit_sim_apdu_basic_req *)modemData->GetRawData();

        EXPECT_EQ(p->session_id, testVector[i].nSessionID);
        EXPECT_EQ(p->entry.sim_apdu.cla, testVector[i].cla);
        EXPECT_EQ(p->entry.sim_apdu.instruction, testVector[i].instruction);
        EXPECT_EQ(p->entry.sim_apdu.p1, testVector[i].p1);
        EXPECT_EQ(p->entry.sim_apdu.p2, testVector[i].p2);
        EXPECT_EQ(p->entry.sim_apdu.p3, testVector[i].p3);

        char tmpHexString[20] = {};
        Value2HexString(tmpHexString, p->entry.sim_apdu.data, p->apdu_len - sizeof(sit_sim_apdu));
        for (int j = 0; j < (int) strlen(testVector[i].pszApduData); j++) {
            EXPECT_EQ(tmpHexString[j], testVector[i].pszApduData[j]);
        }

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSimBuilderLegacy, BuildSimOpenChannel) {
    char testVector[2][10] = {
        "AABBCCDD",
        ""
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);
    ProtocolSimBuilderLegacy testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildSimOpenChannel(testVector[i]);
        ASSERT_NE(modemData, nullptr);
        EXPECT_NE(modemData, nullptr);

        ProtocolReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_OPEN_SIM_CHANNEL);
        EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_sim_open_sim_channel_req));
        EXPECT_EQ(testAdapter.GetParameterLength(),
                sizeof(sit_sim_open_sim_channel_req) - sizeof(RCM_HEADER));
        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        sit_sim_open_sim_channel_req *p =
                (sit_sim_open_sim_channel_req *)modemData->GetRawData();

        char tmpHexString[MAX_SIM_AID_LEN*2] = {};
        Value2HexString(tmpHexString, p->aid, p->aid_len);
        for (int j = 0; j < (int) strlen(testVector[j]); j++) {
            EXPECT_EQ(tmpHexString[j], testVector[i][j]);
        }

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSimBuilderLegacy, BuildSimOpenChannelWithP2) {
    struct {
        char *pszAid;
        int p2;
    } testVector[] = {
        {(char*)"ABCDEF", 0},
        {(char*)"", -1}
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);
    ProtocolSimBuilderLegacy testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData =
                testBuilder.BuildSimOpenChannelWithP2(testVector[i].pszAid, testVector[i].p2);
        ASSERT_NE(modemData, nullptr);
        EXPECT_NE(modemData, nullptr);

        ProtocolReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_OPEN_SIM_CHANNEL_WITH_P2);
        EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_sim_open_sim_channel_with_p2_req));
        EXPECT_EQ(testAdapter.GetParameterLength(),
                sizeof(sit_sim_open_sim_channel_with_p2_req) - sizeof(RCM_HEADER));
        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        sit_sim_open_sim_channel_with_p2_req *p =
                (sit_sim_open_sim_channel_with_p2_req *)modemData->GetRawData();

        char tmpHexString[MAX_SIM_AID_LEN*2] = {};
        Value2HexString(tmpHexString, p->aid, p->aid_len);
        for (int j = 0; j < (int) strlen(testVector[i].pszAid); j++) {
            EXPECT_EQ(tmpHexString[j], testVector[i].pszAid[j]);
        }
        EXPECT_EQ(p->p2, static_cast<BYTE>(testVector[i].p2));

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSimBuilderLegacy, BuildSimCloseChannel) {
    static int testVector = 1;

    ProtocolSimBuilderLegacy testBuilder;
    ModemData *modemData = testBuilder.BuildSimCloseChannel(testVector);
    ASSERT_NE(modemData, nullptr);
    EXPECT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_CLOSE_SIM_CHANNEL);
    EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_sim_close_sim_channel_req));
    EXPECT_EQ(testAdapter.GetParameterLength(),
            sizeof(sit_sim_close_sim_channel_req) - sizeof(RCM_HEADER));
    EXPECT_NE(testAdapter.GetParameter(), nullptr);
    sit_sim_close_sim_channel_req *p =
            (sit_sim_close_sim_channel_req *)modemData->GetRawData();
    EXPECT_EQ(p->session_id, testVector);
    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolSimBuilderLegacy, BuildSimTransmitApduChannel) {
    struct {
        int nSessionID;
        int cla;
        int instruction;
        int p1;
        int p2;
        int p3;
        char *pszApduData;
    } testVector[] = {
        {0, 1, 2, 1, 2, 3, (char*)"ABCDEF"},
        {0, 1, 2, 1, 2, 3, (char*)""}
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);
    ProtocolSimBuilderLegacy testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildSimTransmitApduChannel(testVector[i].nSessionID,
                testVector[i].cla, testVector[i].instruction, testVector[i].p1,
                testVector[i].p2, testVector[i].p3, testVector[i].pszApduData);
        if (i >= 2) {
            EXPECT_EQ(modemData, nullptr);
            continue;
        }

        ASSERT_NE(modemData, nullptr);
        EXPECT_NE(modemData, nullptr);

        ProtocolReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_TRANSMIT_SIM_APDU_CHANNEL);
        uint32_t modemDataSize =
                sizeof(sit_sim_transmit_sim_apdu_channel_req) + strlen(testVector[i].pszApduData);
        EXPECT_EQ(testAdapter.GetLength(), modemDataSize);
        EXPECT_EQ(testAdapter.GetParameterLength(), modemDataSize - sizeof(RCM_HEADER));

        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        sit_sim_transmit_sim_apdu_channel_req *p =
                (sit_sim_transmit_sim_apdu_channel_req *)modemData->GetRawData();

        EXPECT_EQ(p->session_id, testVector[i].nSessionID);
        EXPECT_EQ(p->cla, testVector[i].cla);
        EXPECT_EQ(p->instruction, testVector[i].instruction);
        EXPECT_EQ(p->p1, testVector[i].p1);
        EXPECT_EQ(p->p2, testVector[i].p2);
        EXPECT_EQ(p->p3, testVector[i].p3);

        char tmpHexString[20] = {};
        Value2HexString(tmpHexString, p->data, p->data_len);
        for (int j = 0; j < (int) strlen(testVector[i].pszApduData); j++) {
            EXPECT_EQ(tmpHexString[j], testVector[i].pszApduData[j]);
        }

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSimBuilderLegacy, BuildGetImsi) {
    char testVector[3][10] = {
        "ABBCCDEE",
        "",
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);
    ProtocolSimBuilderLegacy testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildGetImsi(testVector[i]);
        ASSERT_NE(modemData, nullptr);
        EXPECT_NE(modemData, nullptr);

        ProtocolReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_GET_IMSI);
        EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_id_get_imsi_req));
        EXPECT_EQ(testAdapter.GetParameterLength(),
                sizeof(sit_id_get_imsi_req) - sizeof(RCM_HEADER));
        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        sit_id_get_imsi_req *p =
                (sit_id_get_imsi_req *)modemData->GetRawData();

        char tmpHexString[MAX_SIM_AID_LEN*2] = {};
        Value2HexString(tmpHexString, p->aid, p->aid_len);
        for (int j = 0; j < (int)strlen(testVector[i]); j++) {
            EXPECT_EQ(tmpHexString[j], testVector[i][j]);
        }

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSimBuilderLegacy, BuildSimGetGbaAuth) {
    struct {
        char gbaData[515];
        bool isModemDataNull;
    } testVector[] = {
        { { 0x01, 0x00, 0x02, 0x04, 0x40, 0x4b, 0x5f, 0x5d, [8 ... 514] = 0x01 }, false },
        { { 0x00, 0x01, 0x04, 0x04, 0x12, 0x0c, 0x7d, 0xff, [8 ... 514] = 0x0a }, false },
        { { 0x02, 0x00, 0x02, 0x04, 0x12, 0x0c, 0x7d, 0xff, [8 ... 514] = 0x02 }, true },
        { { 0x01, 0x02, 0x02, 0x04, 0x12, 0x0c, 0x7d, 0xff, [8 ... 514] = 0x0e }, true },
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);
    ProtocolSimBuilderLegacy testBuilder;
    ModemData *modemData = nullptr;
    for (int i = 0; i < (size + 1); i++) {
        if (i >= size) {
            modemData = testBuilder.BuildSimGetGbaAuth(nullptr, 0);
            EXPECT_EQ(modemData, nullptr);
            return;
        } else {
            modemData = testBuilder.BuildSimGetGbaAuth(testVector[i].gbaData,
                    sizeof(testVector[i].gbaData));
            if (testVector[i].isModemDataNull) {
                EXPECT_EQ(modemData, nullptr);
                continue;
            }
            ASSERT_NE(modemData, nullptr);
            EXPECT_NE(modemData, nullptr);
        }

        ProtocolReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_GET_GBA_CONTEXT);
        EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_sim_get_sim_gba_auth_req));
        EXPECT_EQ(testAdapter.GetParameterLength(),
                sizeof(sit_sim_get_sim_gba_auth_req) - sizeof(RCM_HEADER));
        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        sit_sim_get_sim_gba_auth_req *p =
                (sit_sim_get_sim_gba_auth_req *)modemData->GetRawData();
        EXPECT_EQ(p->auth_type, testVector[i].gbaData[0]);
        EXPECT_EQ(p->gba_type, testVector[i].gbaData[1]);
        EXPECT_EQ(p->gba_tag, testVector[i].gbaData[2]);
        EXPECT_EQ(p->data1_len, testVector[i].gbaData[3]);
        for (int k = 4, j = 0; j < p->data1_len; k++, j++) {
            EXPECT_EQ(p->data1[j], testVector[i].gbaData[k]);
        }
        EXPECT_EQ(p->data2_len, testVector[i].gbaData[4+255]);
        for (int k = (4+255+1), j = 0; j < p->data2_len; k++, j++) {
            EXPECT_EQ(p->data2[j], testVector[i].gbaData[k]);
        }
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSimBuilderLegacy, BuildSimGetATR) {
    ProtocolSimBuilderLegacy testBuilder;
    ModemData *modemData = testBuilder.BuildSimGetATR();
    ASSERT_NE(modemData, nullptr);
    EXPECT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_ATR);
    EXPECT_EQ(testAdapter.GetLength(), sizeof(null_data_format));
    EXPECT_EQ(testAdapter.GetParameterLength(),
            sizeof(null_data_format) - sizeof(RCM_HEADER));

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolSimBuilderLegacy, BuildSimReadPbEntry) {
    struct {
        int pb_type;
        int recordId;
    } testVector = {
        0, 1
    };

    ProtocolSimBuilderLegacy testBuilder;
    ModemData *modemData = testBuilder.BuildSimReadPbEntry(testVector.pb_type, testVector.recordId);
    ASSERT_NE(modemData, nullptr);
    EXPECT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_READ_PB_ENTRY);
    EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_read_pb_entry_req));
    EXPECT_EQ(testAdapter.GetParameterLength(),
            sizeof(sit_read_pb_entry_req) - sizeof(RCM_HEADER));

    EXPECT_NE(testAdapter.GetParameter(), nullptr);
    sit_read_pb_entry_req *p = (sit_read_pb_entry_req *)modemData->GetRawData();
    EXPECT_EQ(p->pb_type, testVector.pb_type);
    EXPECT_EQ(p->recordId, testVector.recordId);

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolSimBuilderLegacy, BuildSimUpdatePbDelete) {
    struct {
        int mode;
        int type;
        int recordId;
    } testVector = {
        0, 1, 2
    };

    ProtocolSimBuilderLegacy testBuilder;
    ModemData *modemData = testBuilder.BuildSimUpdatePbDelete(testVector.mode, testVector.type,
            testVector.recordId);

    ASSERT_NE(modemData, nullptr);
    EXPECT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_UPDATE_PB_ENTRY);
    uint32_t modemDataSize = sizeof(sit_update_pb_entry_req);
    EXPECT_EQ(testAdapter.GetLength(), modemDataSize);
    EXPECT_EQ(testAdapter.GetParameterLength(), (modemDataSize - sizeof(RCM_HEADER)));

    EXPECT_NE(testAdapter.GetParameter(), nullptr);
    sit_update_pb_entry_req *p = (sit_update_pb_entry_req *)modemData->GetRawData();
    EXPECT_EQ(p->mode, testVector.mode);
    EXPECT_EQ(p->pb_type, testVector.type);
    EXPECT_EQ(p->recordId, testVector.recordId);

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolSimBuilderLegacy, BuildSimUpdatePb3gEntry) {
    static char pb_buffer[] = {
        0x02, 0x07, 0x00, 0x01, 0x70, 0x62, 0x5F, 0x64, 0x61, 0x74, 0x61
    };
    static struct testArgs {
        int mode;
        int type;
        int recordId;
        int length;
        char *pb;
    } testVector = {
        0, 1, 2, sizeof(pb_buffer), pb_buffer
    };

    ProtocolSimBuilderLegacy testBuilder;
    ModemData *modemData = testBuilder.BuildSimUpdatePb3gEntry(testVector.mode, testVector.type,
            testVector.recordId, testVector.length, testVector.pb);

    ASSERT_NE(modemData, nullptr);
    EXPECT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_UPDATE_PB_ENTRY);
    uint32_t modemDataSize = sizeof(sit_update_pb_entry_req);
    EXPECT_EQ(testAdapter.GetLength(), modemDataSize);
    EXPECT_EQ(testAdapter.GetParameterLength(), (modemDataSize - sizeof(RCM_HEADER)));
    EXPECT_NE(testAdapter.GetParameter(), nullptr);
    sit_update_pb_entry_req *p = (sit_update_pb_entry_req *)modemData->GetRawData();
    EXPECT_EQ(p->mode, testVector.mode);
    EXPECT_EQ(p->pb_type, testVector.type);
    EXPECT_EQ(p->recordId, testVector.recordId);
    EXPECT_EQ(p->entry_len, testVector.length);
    for (int i = 0; i < p->entry_len; i++) {
        EXPECT_EQ(p->entry[i], testVector.pb[i]);
    }
    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolSimBuilderLegacy, BuildSimGetPbStorageInfo) {
    static int testVector = 0;

    ProtocolSimBuilderLegacy testBuilder;
    ModemData *modemData = testBuilder.BuildSimGetPbStorageInfo(testVector);
    ASSERT_NE(modemData, nullptr);
    EXPECT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_PB_STORAGE_INFO);
    EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_sim_pb_storage_info));
    EXPECT_EQ(testAdapter.GetParameterLength(),
            sizeof(sit_sim_pb_storage_info) - sizeof(RCM_HEADER));

    EXPECT_NE(testAdapter.GetParameter(), nullptr);
    sit_sim_pb_storage_info *p = (sit_sim_pb_storage_info *)modemData->GetRawData();
    EXPECT_EQ(p->pb_type, testVector);

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolSimBuilderLegacy, BuildSim3GPbCapa) {
    ProtocolSimBuilderLegacy testBuilder;
    ModemData *modemData = testBuilder.BuildSim3GPbCapa();
    ASSERT_NE(modemData, nullptr);
    EXPECT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_3G_PB_CAPA);
    EXPECT_EQ(testAdapter.GetLength(), sizeof(null_data_format));
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolSimBuilderLegacy, BuildSetCarrierRestrictions) {
    static CarrierInfo allowed[] = {
        {"001", "01", RIL_MATCH_ALL, "ALL"},
        {"001", "01", RIL_MATCH_SPN, "SPN"},
        {"001", "01", RIL_MATCH_SPN, ""}
    };
    static CarrierInfo excluded[] = {
        {"001", "01", RIL_MATCH_ALL, "ALL"},
        {"001", "01", RIL_MATCH_SPN, "SPN"},
        {"001", "01", RIL_MATCH_SPN, ""}
    };

    struct {
        int nAllowLength;
        CarrierInfo *pAllowed;
        int nExcludeLength;
        CarrierInfo *pExcluded;
        bool m_bAllowedPrioritized;
        int nMultiSimPolicy;
    } testVector = {
        2, allowed, 2, excluded, true, 0
    };


    ProtocolSimBuilderLegacy testBuilder;
    ModemData *modemData = testBuilder.BuildSetCarrierRestrictions(testVector.nAllowLength,
            testVector.pAllowed, testVector.nExcludeLength, testVector.pExcluded,
            testVector.m_bAllowedPrioritized, testVector.nMultiSimPolicy);

    ASSERT_NE(modemData, nullptr);
    EXPECT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_SET_CARRIER_RESTRICTIONS);
    uint32_t modemDataSize =
            sizeof(sit_sim_set_carrier_restriction_req)
            + sizeof(SIT_CARRIER_INFO) * (testVector.nAllowLength + testVector.nExcludeLength) + 2;
    EXPECT_EQ(testAdapter.GetLength(), modemDataSize);
    EXPECT_EQ(testAdapter.GetParameterLength(), modemDataSize - sizeof(RCM_HEADER));
    EXPECT_NE(testAdapter.GetParameter(), nullptr);

    sit_sim_set_carrier_restriction_req *p =
        (sit_sim_set_carrier_restriction_req *)modemData->GetRawData();
    EXPECT_EQ(p->allowed_carriers_len, testVector.nAllowLength);

    for (int i = 0; i < p->allowed_carriers_len; i++) {
        for (int k = 0; k < 3; k++) {
            EXPECT_EQ(p->carrier_list[i].mcc[k], testVector.pAllowed[i].mcc[k]);
        }
    }

    EXPECT_EQ(p->excluded_carriers_len, testVector.nExcludeLength);
    for (int i = p->allowed_carriers_len, j = 0; i < (p->allowed_carriers_len + p->excluded_carriers_len); i++, j++) {
        for (int k = 0; k < 3; k++) {
            EXPECT_EQ(p->carrier_list[i].mcc[k], testVector.pExcluded[j].mcc[k]);
        }
    }

    const char *pp = testAdapter.GetParameter();
    int length = testAdapter.GetParameterLength();
    EXPECT_EQ(pp[length-2], static_cast<char>(testVector.m_bAllowedPrioritized));
    EXPECT_EQ(pp[length-1], static_cast<char>(testVector.nMultiSimPolicy));

    delete modemData;
    modemData = nullptr;

}

TEST(ProtocolSimBuilderLegacy, BuildGetCarrierRestrictions) {
    ProtocolSimBuilderLegacy testBuilder;
    ModemData *modemData = testBuilder.BuildGetCarrierRestrictions();
    ASSERT_NE(modemData, nullptr);
    EXPECT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_CARRIER_RESTRICTIONS);
    EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_sim_get_carrier_restriction_req));
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolSimBuilderLegacy, BuildSetUicc) {
    static int testVector = 1;

    ProtocolSimBuilderLegacy testBuilder;
    ModemData *modemData = testBuilder.BuildSetUicc(testVector);
    ASSERT_NE(modemData, nullptr);
    EXPECT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_SET_UICC_SUBSCRIPTION);
    EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_sim_set_uicc_sub_req));
    EXPECT_EQ(testAdapter.GetParameterLength(),
            sizeof(sit_sim_set_uicc_sub_req) - sizeof(RCM_HEADER));

    EXPECT_NE(testAdapter.GetParameter(), nullptr);
    sit_sim_set_uicc_sub_req *p = (sit_sim_set_uicc_sub_req *)modemData->GetRawData();
    EXPECT_EQ(p->state, testVector);

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolSimBuilderLegacy, BuildSetSimCardPower) {
    static int testVector = 1;

    ProtocolSimBuilderLegacy testBuilder;
    ModemData *modemData = testBuilder.BuildSetSimCardPower(testVector);
    ASSERT_NE(modemData, nullptr);
    EXPECT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_SET_SIM_CARD_POWER);
    EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_sim_set_sim_card_power_req));
    EXPECT_EQ(testAdapter.GetParameterLength(),
            sizeof(sit_sim_set_sim_card_power_req) - sizeof(RCM_HEADER));

    EXPECT_NE(testAdapter.GetParameter(), nullptr);
    sit_sim_set_sim_card_power_req *p = (sit_sim_set_sim_card_power_req *)modemData->GetRawData();
    EXPECT_EQ(p->mode, testVector);

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolSimBuilderLegacy, BuildOemSimRequest) {
    static struct testArgs {
        int msgId;
        BYTE* pData;
        int nDataLength;
        uint32_t sitMsg;
    } testVector[] = {
        {RIL_REQUEST_SIM_IO, (BYTE*)"1", (int)strlen("1"), SIT_SIM_IO},
        {RIL_REQUEST_SIM_OPEN_CHANNEL, (BYTE*)"12", (int)strlen("12"), SIT_OPEN_SIM_CHANNEL_WITH_P2},
        {RIL_REQUEST_SIM_TRANSMIT_APDU_CHANNEL, (BYTE*)"123", (int)strlen("123"),
                SIT_TRANSMIT_SIM_APDU_CHANNEL},
        {RIL_REQUEST_SIM_TRANSMIT_APDU_BASIC, (BYTE*)"1234", (int)strlen("1234"),
                SIT_TRANSMIT_SIM_APDU_BASIC},

        // Error
        {RIL_REQUEST_SIM_CLOSE_CHANNEL, (BYTE*)"1234", (int)strlen("1234"), SIT_CLOSE_SIM_CHANNEL},
    };

    ProtocolSimBuilderLegacy testBuilder;
    int size = sizeof(testVector) / sizeof(testVector[0]);
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildOemSimRequest(testVector[i].msgId,
                testVector[i].pData, testVector[i].nDataLength);
        if (i >= 4) {
            EXPECT_EQ(modemData, nullptr);
            continue;
        }
        ASSERT_NE(modemData, nullptr);
        EXPECT_NE(modemData, nullptr);

        ProtocolReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), testVector[i].sitMsg);
        uint32_t modemDataSize = sizeof(null_data_format) + testVector[i].nDataLength;
        EXPECT_EQ(testAdapter.GetLength(), modemDataSize);
        EXPECT_EQ(testAdapter.GetParameterLength(), modemDataSize - sizeof(RCM_HEADER));
        EXPECT_NE(testAdapter.GetParameter(), nullptr);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSimBuilderLegacy, BuildGetSimLockInfo) {
    ProtocolSimBuilderLegacy testBuilder;
    ModemData *modemData = testBuilder.BuildGetSimLockInfo();
    ASSERT_NE(modemData, nullptr);
    EXPECT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_OEM_GET_SIM_LOCK_INFO);
    EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_sim_get_sim_lock_info_req));
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);

    delete modemData;
    modemData = nullptr;
}
