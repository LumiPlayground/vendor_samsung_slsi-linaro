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

#include "protocoljsontestadapter.h"
#include "json/protocolsimbuilderjson.h"
#include "simjsondef.h"
#include "modemdata.h"

using namespace std;

TEST(ProtocolSimBuilderJson, BuildSimGetStatus) {
    ProtocolSimBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildSimGetStatus();
    ASSERT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_SIM_STATUS);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);
    ASSERT_EQ(testAdapter.GetParameter(), nullptr);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

typedef enum {
    PIN1_PUK1 = 0,
    PIN2_PUK2 = 1,
    INVALID_PIN_PUK = 2,
} SIM_PIN_PUK_INDEX;

TEST(ProtocolSimBuilderJson, BuildSimVerifyPin) {
    struct {
        int pinIndex;
        const char* pin;
        const char* aid;
        bool isModemDataNull;
    } testVector[] = {
        { PIN1_PUK1, "1234", "a000000087", false },
        { PIN2_PUK2, "1122", "a09345", false },
        { PIN1_PUK1, "123456789", "a000000087", false }, // case when pinLen > MAX_SIM_PIN_LEN
        { INVALID_PIN_PUK, "1122", "a09345", true }, // built modemdata is nullptr because pinIndex is invalid
        { PIN1_PUK1, nullptr, "a09345", true }, // built modemdata is nullptr because PIN is nullptr
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    for(int i = 0; i < size; i++) {
        ProtocolSimBuilderJson testBuilder;
        ModemData *modemData = testBuilder.BuildSimVerifyPin(testVector[i].pinIndex,
                testVector[i].pin, testVector[i].aid);

        if (testVector[i].isModemDataNull) {
            EXPECT_EQ(modemData, nullptr);
            continue;
        }

        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        if(testVector[i].pinIndex == 0) {
            EXPECT_EQ(testAdapter.GetId(), SIT_VERIFY_SIM_PIN);
        } else {
            EXPECT_EQ(testAdapter.GetId(), SIT_VERIFY_SIM_PIN2);
        }

        EXPECT_NE(testAdapter.GetParameterLength(), 0U);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        Json::Value jsonReqData;
        bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }

        ASSERT_NE(false, parsed);
        string stringPin;
        if(testVector[i].pinIndex == 0) {
            stringPin = jsonReqData[ns_sim_verify_sim_pin_req::key_pin].asString();
        } else {
            stringPin = jsonReqData[ns_sim_verify_sim_pin2_req::key_pin].asString();
        }
        const char* parsedPin = stringPin.c_str();

        if(i == 2) {
            // check when pinLen > MAX_SIM_PIN_LEN
            EXPECT_EQ(strcmp("12345678", parsedPin), 0);
        } else {
            EXPECT_EQ(strcmp(testVector[i].pin, parsedPin), 0);
        }

        int sizeOfAid;
        if(testVector[i].pinIndex == 0) {
            sizeOfAid = jsonReqData[ns_sim_verify_sim_pin_req::key_aid].size();
            EXPECT_EQ(strlen(testVector[i].aid) / 2, (unsigned long) sizeOfAid);
            int expParsedAid[] = {160, 0, 0, 0, 135};
            for(int j = 0; j < sizeOfAid; j++) {
                EXPECT_EQ(expParsedAid[j], jsonReqData[ns_sim_verify_sim_pin_req::key_aid][j].asInt());
            }
        } else {
            sizeOfAid = jsonReqData[ns_sim_verify_sim_pin_req::key_aid].size();
            EXPECT_EQ(strlen(testVector[i].aid) / 2, (unsigned long) sizeOfAid);
            int expParsedAid[] = {160, 147, 69};
            for(int j = 0; j < sizeOfAid; j++) {
                EXPECT_EQ(expParsedAid[j], jsonReqData[ns_sim_verify_sim_pin2_req::key_aid][j].asInt());
            }
        }
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSimBuilderJson, BuildSimVerifyPuk) {
    struct {
        int pukIndex;
        const char *puk;
        const char* newPin;
        const char* aid;
        bool isModemDataNull;
    } testVector[] = {
        { PIN1_PUK1, "123456", "1234", "a000000087", false },
        { PIN2_PUK2, "112233", "1122", "a09345", false },
        // case when pinLen > MAX_SIM_PIN_LEN and pukLen > MAX_SIM_PUK_LEN
        { PIN1_PUK1, "123456789", "123412345", "a000000087", false },
        { INVALID_PIN_PUK, "123456", "1234", "a000000087", true }, // built modemdata is nullptr because pinIndex is invalid
        { PIN1_PUK1, nullptr, "1234", "a000000087", true }, // built modemdata is nullptr because PUK is nullptr
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    for(int i = 0; i < size; i++) {
        ProtocolSimBuilderJson testBuilder;
        ModemData *modemData = testBuilder.BuildSimVerifyPuk(testVector[i].pukIndex,
                testVector[i].puk, testVector[i].newPin, testVector[i].aid);

        if (testVector[i].isModemDataNull) {
            EXPECT_EQ(modemData, nullptr);
            continue;
        }

        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        if(testVector[i].pukIndex == 0) {
            EXPECT_EQ(testAdapter.GetId(), SIT_VERIFY_SIM_PUK);
        } else {
            EXPECT_EQ(testAdapter.GetId(), SIT_VERIFY_SIM_PUK2);
        }

        EXPECT_NE(testAdapter.GetParameterLength(), 0U);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        Json::Value jsonReqData;
        bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }

        ASSERT_NE(false, parsed);
        string stringPuk;
        string stringNewPin;
        if(testVector[i].pukIndex == 0) {
            stringPuk = jsonReqData[ns_sim_verify_sim_puk_req::key_puk].asString();
            stringNewPin = jsonReqData[ns_sim_verify_sim_puk_req::key_new_pin].asString();
        } else {
            stringPuk = jsonReqData[ns_sim_verify_sim_puk2_req::key_puk].asString();
            stringNewPin = jsonReqData[ns_sim_verify_sim_puk2_req::key_new_pin].asString();
        }
        const char* parsedPuk = stringPuk.c_str();
        const char* parsedNewPin = stringNewPin.c_str();

        if(i == 2) {
            // check when pinLen > MAX_SIM_PIN_LEN and pukLen > MAX_SIM_PUK_LEN
            EXPECT_EQ(strcmp("12345678", parsedPuk), 0);
            EXPECT_EQ(strcmp("12341234", parsedNewPin), 0);
        } else {
            EXPECT_EQ(strcmp(testVector[i].puk, parsedPuk), 0);
            EXPECT_EQ(strcmp(testVector[i].newPin, parsedNewPin), 0);
        }

        int sizeOfAid;
        if(testVector[i].pukIndex == 0) {
            sizeOfAid = jsonReqData[ns_sim_verify_sim_puk_req::key_aid].size();
            EXPECT_EQ(strlen(testVector[i].aid) / 2, (unsigned long) sizeOfAid);
            int expParsedAid[] = {160, 0, 0, 0, 135};
            for(int j = 0; j < sizeOfAid; j++) {
                EXPECT_EQ(expParsedAid[j], jsonReqData[ns_sim_verify_sim_puk_req::key_aid][j].asInt());
            }
        } else {
            sizeOfAid = jsonReqData[ns_sim_verify_sim_puk2_req::key_aid].size();
            EXPECT_EQ(strlen(testVector[i].aid) / 2, (unsigned long) sizeOfAid);
            int expParsedAid[] = {160, 147, 69};
            for(int j = 0; j < sizeOfAid; j++) {
                EXPECT_EQ(expParsedAid[j], jsonReqData[ns_sim_verify_sim_puk2_req::key_aid][j].asInt());
            }
        }
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSimBuilderJson, BuildSimChangePin) {
    struct {
        int pinIndex;
        const char* oldPin;
        const char* newPin;
        const char* aid;
        int fac;
        bool isModemDataNull;
    } testVector[] = {
        { PIN1_PUK1, "1234", "1256", "a000000087", SIT_SIM_FAC_SC, false },
        { PIN2_PUK2, "1122", "9874", "a09345", SIT_SIM_FAC_SC2, false },
        // case when oldPinLen and newPinLen is more than MAX_SIM_PIN_LEN
        { PIN1_PUK1, "123456789", "123412345", "a000000087", SIT_SIM_FAC_SC, false },
        { INVALID_PIN_PUK, "1234", "1256", "a000000087", SIT_SIM_FAC_SC, true }, // built modemdata is nullptr because pinIndex is invalid
        { PIN1_PUK1, nullptr, "1256", "a000000087", SIT_SIM_FAC_SC, true }, // built modemdata is nullptr because oldPin is nullptr
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    for(int i = 0; i < size; i++) {
        ProtocolSimBuilderJson testBuilder;
        ModemData *modemData = testBuilder.BuildSimChangePin(testVector[i].pinIndex,
                testVector[i].oldPin, testVector[i].newPin, testVector[i].aid);

        if (testVector[i].isModemDataNull) {
            EXPECT_EQ(modemData, nullptr);
            continue;
        }

        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        if(testVector[i].pinIndex == 0) {
            EXPECT_EQ(testAdapter.GetId(), SIT_CHG_SIM_PIN);
        } else {
            EXPECT_EQ(testAdapter.GetId(), SIT_CHG_SIM_PIN2);
        }

        EXPECT_NE(testAdapter.GetParameterLength(), 0U);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        Json::Value jsonReqData;
        bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }

        ASSERT_NE(false, parsed);
        int nFac;
        string stringOldPin;
        string stringNewPin;
        if(testVector[i].pinIndex == 0) {
            nFac = jsonReqData[ns_sim_change_sim_pin_req::key_fac_type].asInt();
            stringOldPin = jsonReqData[ns_sim_change_sim_pin_req::key_old_pin].asString();
            stringNewPin = jsonReqData[ns_sim_change_sim_pin_req::key_new_pin].asString();
        } else {
            nFac = jsonReqData[ns_sim_change_sim_pin2_req::key_fac_type].asInt();
            stringOldPin = jsonReqData[ns_sim_change_sim_pin2_req::key_old_pin].asString();
            stringNewPin = jsonReqData[ns_sim_change_sim_pin2_req::key_new_pin].asString();
        }
        EXPECT_EQ(nFac, testVector[i].fac);
        const char* parsedOldPin = stringOldPin.c_str();
        const char* parsedNewPin = stringNewPin.c_str();

        if(i == 2) {
            // check when oldPinLen and newPinLen is more than MAX_SIM_PIN_LEN
            EXPECT_EQ(strcmp("12345678", parsedOldPin), 0);
            EXPECT_EQ(strcmp("12341234", parsedNewPin), 0);
        } else {
            EXPECT_EQ(strcmp(testVector[i].oldPin, parsedOldPin), 0);
            EXPECT_EQ(strcmp(testVector[i].newPin, parsedNewPin), 0);
        }

        int sizeOfAid;
        if(testVector[i].pinIndex == 0) {
            sizeOfAid = jsonReqData[ns_sim_change_sim_pin_req::key_aid].size();
            EXPECT_EQ(strlen(testVector[i].aid) / 2, (unsigned long) sizeOfAid);
            int expParsedAid[] = {160, 0, 0, 0, 135};
            for(int j = 0; j < sizeOfAid; j++) {
                EXPECT_EQ(expParsedAid[j], jsonReqData[ns_sim_change_sim_pin_req::key_aid][j].asInt());
            }
        } else {
            sizeOfAid = jsonReqData[ns_sim_change_sim_pin2_req::key_aid].size();
            EXPECT_EQ(strlen(testVector[i].aid) / 2, (unsigned long) sizeOfAid);
            int expParsedAid[] = {160, 147, 69};
            for(int j = 0; j < sizeOfAid; j++) {
                EXPECT_EQ(expParsedAid[j], jsonReqData[ns_sim_change_sim_pin2_req::key_aid][j].asInt());
            }
        }
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSimBuilderJson, BuildSimVerifyNetworkLock) {
    struct {
        int fac;
        int svcClass;
        const char* password;
        const char* aid;
        bool isModemDataNull;
    } testVector[] = {
        { 17 /* FAC_PN */, 0, "1256", "a000000087", false },
        { 18 /* FAC_PC */, 0, "9874", "a000000087", false },
        // case when passwordLen > MAX_SIM_FACILITY_PASSWORD_LEN
        { 20 /* FAC_PU */, 0, "8709203214788514586214789651254786517894", "a000000087", false },
        { 17 /* FAC_PN */, 0, nullptr, "a000000087", true }, // built modemdata is nullptr because password is invalid
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    for(int i = 0; i < size; i++) {
        ProtocolSimBuilderJson testBuilder;
        ModemData *modemData = testBuilder.BuildSimVerifyNetworkLock(testVector[i].fac,
                testVector[i].password, testVector[i].svcClass, testVector[i].aid);

        if (testVector[i].isModemDataNull) {
            EXPECT_EQ(modemData, nullptr);
            continue;
        }

        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_VERIFY_NETWORK_LOCK);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        Json::Value jsonReqData;
        bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }

        ASSERT_NE(false, parsed);
        int expFac = jsonReqData[ns_sim_verify_network_lock_req::key_fac_type].asInt();
        int expSvcClass = jsonReqData[ns_sim_verify_network_lock_req::key_class].asInt();
        string expStringPassword = jsonReqData[ns_sim_verify_network_lock_req::key_pw].asString();
        const char* expPassword = expStringPassword.c_str();

        EXPECT_EQ(testVector[i].fac, expFac);
        EXPECT_EQ(testVector[i].svcClass, expSvcClass);

        if(i == 2) {
            // check when passwordLen > MAX_SIM_FACILITY_PASSWORD_LEN
            EXPECT_EQ(strcmp("870920321478851458621478965125478651789", expPassword), 0);
        } else {
            EXPECT_EQ(strcmp(testVector[i].password, expPassword), 0);
        }

        int sizeOfAid = jsonReqData[ns_sim_verify_network_lock_req::key_aid].size();
        EXPECT_EQ(strlen(testVector[i].aid) / 2, (unsigned long) sizeOfAid);
        int expParsedAid[] = {160, 0, 0, 0, 135};
        for(int j = 0; j < sizeOfAid; j++) {
            EXPECT_EQ(expParsedAid[j], jsonReqData[ns_sim_verify_network_lock_req::key_aid][j].asInt());
        }

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSimBuilderJson, BuildSimIO) {
    struct {
        int cmd;
        int appType;
        int fileId;
        const char* path;
        int p1;
        int p2;
        int p3;
        int dataLen;
        const char data[MAX_SIM_IO_DATA_LEN];
        const char *pin2;
        const char *aid;
        bool isModemDataNull;
    } testVector[] = {
        { 0xC0, 2, 0x2FE2, "3F007FFF", 0, 0, 0x0f, 24, "90001F00621D820241218302", "1234", "a000000087", false },
        { 0xB0, 2, 0x6FAD, "3F007FFF", 0, 0, 0x04, 0, "", "", "a0000000871002fffff00189000001ff", false },
        // case when modemData is null, path is null
        { 0xB0, 2, 0x6FAD, NULL, 0, 0, 0x04, 24, "90001F00621D820241218302", "1234", "a0000000870f02fffff00189000001ff", true },
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);

    for(int i = 0; i < size; i++) {
        ProtocolSimBuilderJson testBuilder;
        ModemData *modemData = testBuilder.BuildSimIO(testVector[i].cmd, testVector[i].appType,
                testVector[i].fileId, testVector[i].path, testVector[i].p1, testVector[i].p2,
                testVector[i].p3, 0 /* unused paramter */, testVector[i].data, testVector[i].pin2,
                testVector[i].aid);

        if (testVector[i].isModemDataNull) {
            ASSERT_EQ(modemData, nullptr);
            continue;
        }

        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_SIM_IO);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        Json::Value jsonReqData;
        bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }

        ASSERT_NE(false, parsed);

        EXPECT_EQ(testVector[i].cmd, jsonReqData[ns_sim_sim_io_req::key_cmd].asInt());
        EXPECT_EQ(testVector[i].appType, jsonReqData[ns_sim_sim_io_req::key_app_type].asInt());
        EXPECT_EQ(testVector[i].fileId, jsonReqData[ns_sim_sim_io_req::key_file_id].asInt());
        EXPECT_EQ(testVector[i].p1, jsonReqData[ns_sim_sim_io_req::key_p1].asInt());
        EXPECT_EQ(testVector[i].p2, jsonReqData[ns_sim_sim_io_req::key_p2].asInt());
        EXPECT_EQ(testVector[i].p3, jsonReqData[ns_sim_sim_io_req::key_p3].asInt());

        string stringPin2 = jsonReqData[ns_sim_sim_io_req::key_pin2].asString();
        const char* parsedPin2 = stringPin2.c_str();
        EXPECT_EQ(strcmp(testVector[i].pin2, parsedPin2), 0);

        int arrPath[] = {0, 63, 255, 127};
        int pathLen = jsonReqData[ns_sim_sim_io_req::key_path].size();
        EXPECT_EQ((unsigned long) pathLen, strlen(testVector[i].path) / 2);
        for(int j = 0; j < pathLen; j++) {
            EXPECT_EQ(arrPath[j], jsonReqData[ns_sim_sim_io_req::key_path][j].asInt());
        }

        int arrData[] = {144, 0, 31, 0, 98, 29, 130, 2, 65, 33, 131, 2};
        int dataLen = jsonReqData[ns_sim_sim_io_req::key_data].size();
        EXPECT_EQ(dataLen, (testVector[i].dataLen) / 2);
        for(int j = 0; j < dataLen; j++) {
            EXPECT_EQ(arrData[j], jsonReqData[ns_sim_sim_io_req::key_data][j].asInt());
        }

        if (i == 0) {
            int arrAid[] = {160, 0, 0, 0, 135};
            int aidLen = jsonReqData[ns_sim_sim_io_req::key_aid].size();
            EXPECT_EQ((unsigned long) aidLen, strlen(testVector[i].aid) / 2);
            for(int j = 0; j < aidLen; j++) {
                EXPECT_EQ(arrAid[j], jsonReqData[ns_sim_sim_io_req::key_aid][j].asInt());
            }
        } else {
            int arrAid[] = {160, 0, 0, 0, 135, 16, 2, 255, 255, 240, 1, 137, 0, 0, 1, 255};
            int aidLen = jsonReqData[ns_sim_sim_io_req::key_aid].size();
            EXPECT_EQ((unsigned long) aidLen, strlen(testVector[i].aid) / 2);
            for(int j = 0; j < aidLen; j++) {
                EXPECT_EQ(arrAid[j], jsonReqData[ns_sim_sim_io_req::key_aid][j].asInt());
            }
        }

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSimBuilderJson, BuildSimGetFacilityLock) {
    struct {
        char pszCode[3];
        int svcClass;
        char password[42];
        char aid[20];
        int facType;
    } testVector[] = {
        { "CS", 0, "1256", "a000000087", SIT_SIM_FAC_CS },
        { "PC", 0, "9874", "a000000087", SIT_SIM_FAC_PC },
        { "CS", 0, "", "a000000087", SIT_SIM_FAC_CS },
        // case when passwordLen > MAX_SIM_FACILITY_PASSWORD_LEN
        { "PU", 0, "8709203214788514586214789651254786517894", "a000000087", SIT_SIM_FAC_PU },
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    for(int i = 0; i < size; i++) {
        ProtocolSimBuilderJson testBuilder;
        ModemData *modemData = testBuilder.BuildSimGetFacilityLock(testVector[i].pszCode,
                testVector[i].password, testVector[i].svcClass, testVector[i].aid);

        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_GET_FACILITY_LOCK);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        Json::Value jsonReqData;
        bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }

        ASSERT_NE(false, parsed);
        int fac = jsonReqData[ns_sim_get_facility_lock_req::key_fac_type].asInt();
        int expSvcClass = jsonReqData[ns_sim_get_facility_lock_req::key_class].asInt();
        string expStringPassword = jsonReqData[ns_sim_get_facility_lock_req::key_pw].asString();
        const char* expPassword = expStringPassword.c_str();

        EXPECT_EQ(testVector[i].facType, fac);
        EXPECT_EQ(testVector[i].svcClass, expSvcClass);

        EXPECT_TRUE(strlen(expPassword) <= MAX_SIM_FACILITY_PASSWORD_LEN);
        EXPECT_EQ(strncmp(expPassword, testVector[i].password, strlen(expPassword)), 0);

        int sizeOfAid = jsonReqData[ns_sim_get_facility_lock_req::key_aid].size();
        EXPECT_EQ(strlen(testVector[i].aid) / 2, (unsigned long) sizeOfAid);
        int expParsedAid[] = {160, 0, 0, 0, 135};
        for(int j = 0; j < sizeOfAid; j++) {
            EXPECT_EQ(expParsedAid[j], jsonReqData[ns_sim_get_facility_lock_req::key_aid][j].asInt());
        }

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSimBuilderJson, BuildSimSetFacilityLock) {
    struct {
        char pszCode[3];
        int lockMode;
        int svcClass;
        char password[42];
        char aid[20];
        int facType;
    } testVector[] = {
        { "CS", 0, 0, "1256", "a000000087", SIT_SIM_FAC_CS },
        { "PC", 0, 0, "9874", "a000000087", SIT_SIM_FAC_PC },
        { "CS", 0, 0, "", "a000000087", SIT_SIM_FAC_CS },
        // case when passwordLen > MAX_SIM_FACILITY_PASSWORD_LEN
        { "PU", 0, 0, "8709203214788514586214789651254786517894", "a000000087", SIT_SIM_FAC_PU },
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    for(int i = 0; i < size; i++) {
        ProtocolSimBuilderJson testBuilder;
        ModemData *modemData = testBuilder.BuildSimSetFacilityLock(testVector[i].pszCode,
                testVector[i].lockMode, testVector[i].password, testVector[i].svcClass, testVector[i].aid);

        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_SET_FACILITY_LOCK);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        Json::Value jsonReqData;
        bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }

        ASSERT_NE(false, parsed);
        int fac = jsonReqData[ns_sim_set_facility_lock_req::key_fac_type].asInt();
        int parsedLockMode = jsonReqData[ns_sim_set_facility_lock_req::key_lock_mode].asInt();
        int expSvcClass = jsonReqData[ns_sim_set_facility_lock_req::key_class].asInt();
        string expStringPassword = jsonReqData[ns_sim_set_facility_lock_req::key_pw].asString();
        const char* expPassword = expStringPassword.c_str();

        EXPECT_EQ(testVector[i].facType, fac);
        EXPECT_EQ(testVector[i].lockMode, parsedLockMode);
        EXPECT_EQ(testVector[i].svcClass, expSvcClass);

        EXPECT_TRUE(strlen(expPassword) <= MAX_SIM_FACILITY_PASSWORD_LEN);
        EXPECT_EQ(strncmp(expPassword, testVector[i].password, strlen(expPassword)), 0);

        int sizeOfAid = jsonReqData[ns_sim_set_facility_lock_req::key_aid].size();
        EXPECT_EQ(strlen(testVector[i].aid) / 2, (unsigned long) sizeOfAid);
        int expParsedAid[] = {160, 0, 0, 0, 135};
        for(int j = 0; j < sizeOfAid; j++) {
            EXPECT_EQ(expParsedAid[j], jsonReqData[ns_sim_set_facility_lock_req::key_aid][j].asInt());
        }

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSimBuilderJson, BuildSimOpenChannel) {
    struct {
        const char *aid;
        int expAid[MAX_SIM_AID_LEN];
    } testVector[] = {
        { "a000000087", {160, 0, 0, 0, 135} },
        { "a0000000871002fffff00189000001ff", {160, 0, 0, 0, 135, 16, 2, 255, 255, 240, 1, 137, 0, 0, 1, 255} },
        { NULL, { } },
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);

    for(int i = 0; i < size; i++) {
        ProtocolSimBuilderJson testBuilder;
        ModemData *modemData = testBuilder.BuildSimOpenChannel(testVector[i].aid);
        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_OPEN_SIM_CHANNEL);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        Json::Value jsonReqData;
        bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }

        ASSERT_NE(false, parsed);

        int sizeOfAid = jsonReqData[ns_sim_open_sim_channel_req::key_aid].size();
        int aidLen = testVector[i].aid == NULL ? 0 : strlen(testVector[i].aid);
        EXPECT_EQ(aidLen / 2, sizeOfAid);
        for(int j = 0; j < sizeOfAid; j++) {
            EXPECT_EQ(testVector[i].expAid[j], jsonReqData[ns_sim_open_sim_channel_req::key_aid][j].asInt());
        }

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSimBuilderJson, BuildSimCloseChannel) {
    int testVector[] = { 1, 2, 10, 1001, INT_MAX };

    int size = sizeof(testVector) / sizeof(testVector[0]);

    for(int i = 0; i < size; i++) {
        ProtocolSimBuilderJson testBuilder;
        ModemData *modemData = testBuilder.BuildSimCloseChannel(testVector[i]);
        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_CLOSE_SIM_CHANNEL);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        Json::Value jsonReqData;
        bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }

        ASSERT_NE(false, parsed);

        EXPECT_EQ(testVector[i], jsonReqData[ns_sim_close_sim_channel_req::key_sid].asInt());

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSimBuilderJson, BuildSimGetATR) {
    ProtocolSimBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildSimGetATR();
    ASSERT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_ATR);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);
    ASSERT_EQ(testAdapter.GetParameter(), nullptr);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSimBuilderJson, BuildSimReadPbEntry) {
    struct {
        int nPbType;
        int nRecordId;
    } testVector[] = {
        { PB_FDN, 1 },
        { PB_EN, 65535},
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);

    for(int i = 0; i < size; i++) {
        ProtocolSimBuilderJson testBuilder;
        ModemData *modemData = testBuilder.BuildSimReadPbEntry(testVector[i].nPbType, testVector[i].nRecordId);
        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_READ_PB_ENTRY);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        Json::Value jsonReqData;
        bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }

        ASSERT_NE(false, parsed);

        EXPECT_EQ(testVector[i].nPbType, jsonReqData[ns_read_pb_entry_req::key_pb_type].asInt());
        EXPECT_EQ(testVector[i].nRecordId, jsonReqData[ns_read_pb_entry_req::key_record_id].asInt());

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSimBuilderJson, BuildSimUpdatePbDelete) {
    struct {
        int mode;
        int pbType;
        int recordId;
    } testVector[] = {
        { 2, PB_FDN, 10 },
        { 2, PB_EN, 65535 },
        { 2, PB_MSISDN, 10001 },
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);

    for(int i = 0; i < size; i++) {
        ProtocolSimBuilderJson testBuilder;
        ModemData *modemData = testBuilder.BuildSimUpdatePbDelete(testVector[i].mode,
                testVector[i].pbType, testVector[i].recordId);

        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_UPDATE_PB_ENTRY);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        Json::Value jsonReqData;
        bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }

        ASSERT_NE(false, parsed);

        EXPECT_EQ(testVector[i].mode, jsonReqData[ns_update_pb_entry_req::key_mode].asInt());
        EXPECT_EQ(testVector[i].pbType, jsonReqData[ns_update_pb_entry_req::key_pb_type].asInt());
        EXPECT_EQ(testVector[i].recordId, jsonReqData[ns_update_pb_entry_req::key_record_id].asInt());

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSimBuilderJson, BuildSimUpdatePb3gEntry) {
    int nMode = 1; // add
    int nPbType = PB_ADN_3G;
    int nRecordId = 10;
    char pb[] = "01110002496e666f204365747202040081313131ff";
    int length = strlen(pb);

    ProtocolSimBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildSimUpdatePb3gEntry(nMode, nPbType, nRecordId, length, pb);
    ASSERT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_UPDATE_PB_ENTRY);
    EXPECT_NE(testAdapter.GetParameterLength(), 0U);
    ASSERT_NE(testAdapter.GetParameter(), nullptr);

    Json::Value jsonReqData;
    bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
    if(!parsed) { delete modemData; modemData = nullptr; }

    ASSERT_NE(false, parsed);
    int parsedMode = jsonReqData[ns_update_pb_entry_req::key_mode].asInt();
    int parsedPbType = jsonReqData[ns_update_pb_entry_req::key_pb_type].asInt();
    int parsedRecordId = jsonReqData[ns_update_pb_entry_req::key_record_id].asInt();
    int pasedDataLen = jsonReqData[ns_update_pb_entry_req::key_entry_data].size();
    EXPECT_EQ(pasedDataLen, length);
    char *parsedPb = new char[length + 1] {};
    ASSERT_NE(parsedPb, nullptr);
    for(int i = 0; i < length; i++) {
        parsedPb[i] = (char) jsonReqData[ns_update_pb_entry_req::key_entry_data][i].asInt();
    }
    EXPECT_EQ(parsedMode, nMode);
    EXPECT_EQ(parsedPbType, nPbType);
    EXPECT_EQ(parsedRecordId, nRecordId);
    EXPECT_EQ(strcmp(parsedPb, pb), 0);

    delete[] parsedPb;
    parsedPb = nullptr;
    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolSimBuilderJson, BuildSimGetPbStorageInfo) {
    int testVector[] = { PB_ADN_3G, PB_ADN_2G, PB_MSISDN, PB_BDN, PB_SDN };

    int size = sizeof(testVector) / sizeof(testVector[0]);

    for(int i = 0; i < size; i++) {
        ProtocolSimBuilderJson testBuilder;
        ModemData *modemData = testBuilder.BuildSimGetPbStorageInfo(testVector[i]);
        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_GET_PB_STORAGE_INFO);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        Json::Value jsonReqData;
        bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }

        ASSERT_NE(false, parsed);

        EXPECT_EQ(testVector[i], jsonReqData[ns_sim_pb_storage_info::key_pb_type].asInt());

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSimBuilderJson, BuildSim3GPbCapa) {
    ProtocolSimBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildSim3GPbCapa();
    ASSERT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_3G_PB_CAPA);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);
    ASSERT_EQ(testAdapter.GetParameter(), nullptr);

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolSimBuilderJson, BuildSimOpenChannelWithP2) {
    struct {
        const char *aid;
        int expAid[MAX_SIM_AID_LEN];
        int p2;
    } testVector[] = {
        { "a000000087", {160, 0, 0, 0, 135}, 1 },
        { "a0000000871002fffff00189000001ff", {160, 0, 0, 0, 135, 16, 2, 255, 255, 240, 1, 137, 0, 0, 1, 255}, 255 },
        { NULL, { }, 10 },
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);

    for(int i = 0; i < size; i++) {
        ProtocolSimBuilderJson testBuilder;
        ModemData *modemData = testBuilder.BuildSimOpenChannelWithP2(testVector[i].aid, testVector[i].p2);
        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_OPEN_SIM_CHANNEL_WITH_P2);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        Json::Value jsonReqData;
        bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }

        ASSERT_NE(false, parsed);

        EXPECT_EQ(testVector[i].p2, jsonReqData[ns_sim_open_sim_channel_with_p2_req::key_p2].asInt());
        int sizeOfAid = jsonReqData[ns_sim_open_sim_channel_req::key_aid].size();
        int aidLen = testVector[i].aid == NULL ? 0 : strlen(testVector[i].aid);
        EXPECT_EQ(aidLen / 2, sizeOfAid);
        for(int j = 0; j < sizeOfAid; j++) {
            EXPECT_EQ(testVector[i].expAid[j], jsonReqData[ns_sim_open_sim_channel_req::key_aid][j].asInt());
        }

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSimBuilderJson, BuildSetUicc) {
    int testVector[] = { 0, 1 }; // 0: deactivated, 1: activated
    int size = sizeof(testVector)/sizeof(testVector[0]);

    for(int i = 0; i < size; i++) {
        int nActiveStatus = testVector[i];

        ProtocolSimBuilderJson testBuilder;
        ModemData *modemData = testBuilder.BuildSetUicc(nActiveStatus);
        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_SET_UICC_SUBSCRIPTION);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        Json::Value jsonReqData;
        bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }

        ASSERT_NE(false, parsed);
        int parsedActiveStatus = jsonReqData[ns_sim_set_uicc_sub_req::key_state].asInt();
        EXPECT_EQ(parsedActiveStatus, nActiveStatus);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSimBuilderJson, BuildSetSimCardPower) {
    int testVector[] = { 0, 1 };  // 0: UP, 1: DOWON
    int size = sizeof(testVector)/sizeof(testVector[0]);

    for(int i = 0; i < size; i++) {
        int nMode = testVector[i];

        ProtocolSimBuilderJson testBuilder;
        ModemData *modemData = testBuilder.BuildSetSimCardPower(nMode);
        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_SET_SIM_CARD_POWER);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        Json::Value jsonReqData;
        bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }

        ASSERT_NE(false, parsed);
        int parsedMode = jsonReqData[ns_sim_set_sim_card_power_req::key_mode].asInt();
        EXPECT_EQ(parsedMode, nMode);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSimBuilderJson, BuildSimTransmitApduBasic) {
    struct {
        int nSessionId;
        int cla;
        int instruction;
        int p1;
        int p2;
        int p3;
        const char *pszApduData;
        int expPdu[MAX_APDU_LEN];
    } testVector[] = {
        { 0, 164, 4, 12, 12, 160, "a000000087", {160, 0, 0, 0, 135} },
        { INT_MAX, 121, 8, 0, 0, 1, "a0000000871002fffff00189000001ff",
            {160, 0, 0, 0, 135, 16, 2, 255, 255, 240, 1, 137, 0, 0, 1, 255} },
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);

    for(int i = 0; i < size; i++) {
        ProtocolSimBuilderJson testBuilder;
        ModemData *modemData = testBuilder.BuildSimTransmitApduBasic(testVector[i].nSessionId, testVector[i].cla,
                testVector[i].instruction, testVector[i].p1, testVector[i].p2, testVector[i].p3,
                testVector[i].pszApduData);
        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_TRANSMIT_SIM_APDU_BASIC);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        Json::Value jsonReqData;
        bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }

        ASSERT_NE(false, parsed);

        EXPECT_EQ(testVector[i].nSessionId, jsonReqData[ns_sim_transmit_sim_apdu_basic_req::key_sid].asInt());
        EXPECT_EQ(testVector[i].cla, jsonReqData[ns_sim_transmit_sim_apdu_basic_req::key_data][0].asInt());
        EXPECT_EQ(testVector[i].instruction, jsonReqData[ns_sim_transmit_sim_apdu_basic_req::key_data][1].asInt());
        EXPECT_EQ(testVector[i].p1, jsonReqData[ns_sim_transmit_sim_apdu_basic_req::key_data][2].asInt());
        EXPECT_EQ(testVector[i].p2, jsonReqData[ns_sim_transmit_sim_apdu_basic_req::key_data][3].asInt());
        EXPECT_EQ(testVector[i].p3, jsonReqData[ns_sim_transmit_sim_apdu_basic_req::key_data][4].asInt());
        // cla + instruction + p1 + p2 + p3 = 5
        int sizeOfApdu = jsonReqData[ns_sim_transmit_sim_apdu_basic_req::key_data].size() - 5;
        for(int j = 5; j < sizeOfApdu; j++) {
            EXPECT_EQ(testVector[i].expPdu[j-5], jsonReqData[ns_sim_transmit_sim_apdu_basic_req::key_data][j].asInt());
        }

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSimBuilderJson, BuildSimTransmitApduChannel) {
    struct {
        int nSessionId;
        int cla;
        int instruction;
        int p1;
        int p2;
        int p3;
        const char *pszApduData;
        int expPdu[MAX_APDU_LEN];
    } testVector[] = {
        { 0, 164, 4, 12, 12, 160, "a000000087", {160, 0, 0, 0, 135} },
        { INT_MAX, 121, 8, 0, 0, 1, "a0000000871002fffff00189000001ff", {160, 0, 0, 0, 135, 16, 2, 255, 255, 240, 1, 137, 0, 0, 1, 255} },
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);

    for(int i = 0; i < size; i++) {
        ProtocolSimBuilderJson testBuilder;
        ModemData *modemData = testBuilder.BuildSimTransmitApduChannel(testVector[i].nSessionId, testVector[i].cla, testVector[i].instruction,
                testVector[i].p1, testVector[i].p2, testVector[i].p3, testVector[i].pszApduData);
        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_TRANSMIT_SIM_APDU_CHANNEL);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        Json::Value jsonReqData;
        bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }

        ASSERT_NE(false, parsed);

        EXPECT_EQ(testVector[i].nSessionId, jsonReqData[ns_sim_transmit_sim_apdu_channel_req::key_sid].asInt());
        EXPECT_EQ(testVector[i].cla, jsonReqData[ns_sim_transmit_sim_apdu_channel_req::key_cla].asInt());
        EXPECT_EQ(testVector[i].instruction, jsonReqData[ns_sim_transmit_sim_apdu_channel_req::key_instruction].asInt());
        EXPECT_EQ(testVector[i].p1, jsonReqData[ns_sim_transmit_sim_apdu_channel_req::key_p1].asInt());
        EXPECT_EQ(testVector[i].p2, jsonReqData[ns_sim_transmit_sim_apdu_channel_req::key_p2].asInt());
        EXPECT_EQ(testVector[i].p3, jsonReqData[ns_sim_transmit_sim_apdu_channel_req::key_p3].asInt());
        int sizeOfApdu = jsonReqData[ns_sim_transmit_sim_apdu_channel_req::key_data].size();
        for(int j = 0; j < sizeOfApdu; j++) {
            EXPECT_EQ(testVector[i].expPdu[j], jsonReqData[ns_sim_transmit_sim_apdu_channel_req::key_data][j].asInt());
        }
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSimBuilderJson, BuildSimGetGbaAuth) {
    struct {
        char gbaData[264];
        bool isModemDataNull;
    } testVector[] = {
        { { 0x01, 0x00, 0x02, 0x04, 0x40, 0x4b, 0x5f, 0x5d, [8 ... 258] = 0x00, 0x04, 0xa8, 0x12, 0xc4, 0xe8 }, false },
        { { 0x00, 0x01, 0x04, 0x04, 0x12, 0x0c, 0x7d, 0xff, [8 ... 258] = 0x00, 0x04, 0x12, 0x6c, 0x1a, 0xb5 }, false },
        { { 0x02, 0x00, 0x02, 0x04, 0x12, 0x0c, 0x7d, 0xff, [8 ... 258] = 0x00, 0x04, 0x12, 0x6c, 0x1a, 0xb5 }, true },
        { { 0x01, 0x02, 0x02, 0x04, 0x12, 0x0c, 0x7d, 0xff, [8 ... 258] = 0x00, 0x04, 0x12, 0x6c, 0x1a, 0xb5 }, true },
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);

    for(int i = 0; i < size; i++) {
        ProtocolSimBuilderJson testBuilder;
        ModemData *modemData = testBuilder.BuildSimGetGbaAuth(testVector[i].gbaData, sizeof(testVector[i].gbaData));

        if (testVector[i].isModemDataNull) {
            ASSERT_EQ(modemData, nullptr);
            continue;
        }

        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_GET_GBA_CONTEXT);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        Json::Value jsonReqData;
        bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }

        ASSERT_NE(false, parsed);

        EXPECT_EQ(testVector[i].gbaData[0], jsonReqData[ns_sim_get_sim_gba_auth_req::key_auth_type].asInt());
        EXPECT_EQ(testVector[i].gbaData[1], jsonReqData[ns_sim_get_sim_gba_auth_req::key_gba_type].asInt());
        EXPECT_EQ(testVector[i].gbaData[2], jsonReqData[ns_sim_get_sim_gba_auth_req::key_gba_tag].asInt());
        int sizeOfData1 = jsonReqData[ns_sim_get_sim_gba_auth_req::key_data1].size();
        EXPECT_EQ(sizeOfData1, 0x04);
        for(int j = 0; j < sizeOfData1; j++) {
            EXPECT_EQ(testVector[i].gbaData[j+4], jsonReqData[ns_sim_get_sim_gba_auth_req::key_data1][j].asInt());
        }
        int sizeOfData2 = jsonReqData[ns_sim_get_sim_gba_auth_req::key_data2].size();
        EXPECT_EQ(sizeOfData2, 0x04);
        for(int j = 0; j < sizeOfData2; j++) {
            EXPECT_EQ(testVector[i].gbaData[j+260], jsonReqData[ns_sim_get_sim_gba_auth_req::key_data2][j].asInt());
        }
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSimBuilderJson, BuildSimGetIsimAuth) {
    struct {
        int authType;
        BYTE authData[MAX_SIM_AUTH_REQ_LEN];
        int authDataLen;
        bool isOutOfBounds;
    } testVector[] = {
        { 0x00, { 0x04, 0x55, 0x55, 0x55, 0x55, 0x04, 0x56, 0x51, 0x50, 0x53 }, 10, false },
        { 0x00, { 0x07, 0x01, 0x04, 0x04, 0x12, 0x0c, 0x7d, 0xff, 0x06, 0x12, 0x6c, 0x1a, 0xb5, 0x53, 0x56 },
            15, false },
        { 0x00, { 0x00, 0x00 }, 2, true },
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);

    for(int i = 0; i < size; i++) {
        ProtocolSimBuilderJson testBuilder;
        ModemData *modemData = testBuilder.BuildSimGetIsimAuth(testVector[i].authType,
                testVector[i].authData, testVector[i].authDataLen);

        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_GET_SIM_AUTH);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        Json::Value jsonReqData;
        bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }

        ASSERT_NE(false, parsed);

        EXPECT_EQ(testVector[i].authType, jsonReqData[ns_sim_get_sim_auth_req::key_auth_type].asInt());
        if (testVector[i].isOutOfBounds) {
            EXPECT_EQ(0U, jsonReqData[ns_sim_get_sim_auth_req::key_rand].size());
            EXPECT_EQ(0U, jsonReqData[ns_sim_get_sim_auth_req::key_autn].size());
        } else {
            int randLen = jsonReqData[ns_sim_get_sim_auth_req::key_rand].size();
            EXPECT_EQ(randLen, testVector[i].authData[0]);
            for(int j = 0; j < randLen; j++) {
                EXPECT_EQ(testVector[i].authData[j+1], jsonReqData[ns_sim_get_sim_auth_req::key_rand][j].asInt());
            }
            int autnLen = jsonReqData[ns_sim_get_sim_auth_req::key_autn].size();
            EXPECT_EQ(autnLen, testVector[i].authData[randLen+1]);
            for(int j = 0; j < autnLen; j++) {
                EXPECT_EQ(testVector[i].authData[randLen+2+j], jsonReqData[ns_sim_get_sim_auth_req::key_autn][j].asInt());
            }
        }
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSimBuilderJson, BuildSimGetSimAuth) {
    struct {
        int nAuthcontext;
        BYTE authData[10] = {0};
        int nAuthLen;
        int nAppType;
        int nAuthType;
        bool isModemDataNull;
    } testVector[] = {
        { 0, {0x04, 0x55, 0x55, 0x55, 0x55, 0x04, 0x56, 0x51, 0x50, 0x53}, 10,
            RIL_APPTYPE_ISIM, SIT_SIM_AUTH_IMS, false },
        { 1, {0x04, 0x55, 0x55, 0x55, 0x55, 0x04, 0x56, 0x51, 0x50, 0x53}, 10,
            RIL_APPTYPE_USIM, SIT_SIM_AUTH_3G, false },
        { 0, {0x09, 0x55, 0x55, 0x55, 0x55, 0x04, 0x56, 0x51, 0x50, 0x53}, 10,
            RIL_APPTYPE_USIM, SIT_SIM_AUTH_GSM, false },
        { 2, { 0x00 }, 0, -1, -1, true }, // built modemdata is nullptr because authContext is invalid
        { 4, { 0x00 }, 0, -1, -1, true }, // built modemdata is nullptr because authContext is invalid
    };
    int size = sizeof(testVector)/sizeof(testVector[0]);

    for(int i = 0; i < size; i++) {
        ProtocolSimBuilderJson testBuilder;
        ModemData *modemData = testBuilder.BuildSimGetSimAuth(testVector[i].nAuthcontext,
                testVector[i].authData, testVector[i].nAuthLen, testVector[i].nAppType);

        if (testVector[i].isModemDataNull) {
            EXPECT_EQ(modemData, nullptr);
            continue;
        }

        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_GET_SIM_AUTH);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        Json::Value jsonReqData;
        bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }

        ASSERT_NE(false, parsed);
        int nParsedAuthType = jsonReqData[ns_sim_get_sim_auth_req::key_auth_type].asInt();
        EXPECT_EQ(nParsedAuthType, testVector[i].nAuthType);
        int randLen = jsonReqData[ns_sim_get_sim_auth_req::key_rand].size();
        EXPECT_EQ(randLen, testVector[i].authData[0]);
        for(int j = 0; j < randLen; j++) {
            EXPECT_EQ(testVector[i].authData[j+1], jsonReqData[ns_sim_get_sim_auth_req::key_rand][j].asInt());
        }
        if(nParsedAuthType == SIT_SIM_AUTH_IMS || nParsedAuthType == SIT_SIM_AUTH_3G) {
            int autnLen = jsonReqData[ns_sim_get_sim_auth_req::key_autn].size();
            EXPECT_EQ(autnLen, testVector[i].authData[randLen+1]);
            for(int j = 0; j < autnLen; j++) {
                EXPECT_EQ(testVector[i].authData[randLen+2+j],
                        jsonReqData[ns_sim_get_sim_auth_req::key_autn][j].asInt());
            }
        }

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSimBuilderJson, BuildSetCarrierRestrictions) {
    CarrierInfo allowedInfos[2] = {{"123", "456", RIL_MATCH_ALL, "test"}, {"123", "45", RIL_MATCH_SPN, "test"}};
    CarrierInfo excludedInfos[2] = {{"345", "678", RIL_MATCH_ALL, "test"}, {"345", "67", RIL_MATCH_SPN, "test"}};

    struct {
        int allowedLength;
        int excludedLength;
        CarrierInfo *pAllowed;
        CarrierInfo *pExcluded;
        bool allowedPrioritized;
        int multiSimPolicy;
        bool isCarrierListNull;
    } testVector[] = {
        { 0, 0, NULL, NULL, false, 0, true },
        { 2, 2, allowedInfos, excludedInfos, true, 2, false },
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolSimBuilderJson testBuilder;

    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildSetCarrierRestrictions(testVector[i].allowedLength, testVector[i].pAllowed,
                testVector[i].excludedLength, testVector[i].pExcluded,
                testVector[i].allowedPrioritized, testVector[i].multiSimPolicy);

        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_SET_CARRIER_RESTRICTIONS);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        Json::Value jsonReqData;
        bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }

        ASSERT_NE(false, parsed);

        EXPECT_EQ(testVector[i].allowedPrioritized,
                jsonReqData[ns_sim_net_getset_carrier_restrictions::key_allowed_priority].asInt());
        EXPECT_EQ(testVector[i].multiSimPolicy,
                jsonReqData[ns_sim_net_getset_carrier_restrictions::key_multi_simlock].asInt());

        int allowedCarrierLen = jsonReqData[ns_sim_net_getset_carrier_restrictions::key_allowed_list].size();
        int excludedCarrierLen = jsonReqData[ns_sim_net_getset_carrier_restrictions::key_excluded_list].size();
        EXPECT_EQ(testVector[i].allowedLength, allowedCarrierLen);
        EXPECT_EQ(testVector[i].excludedLength, excludedCarrierLen);

        if (!testVector[i].isCarrierListNull) {
            Json::Value jsonAllowedList = jsonReqData[ns_sim_net_getset_carrier_restrictions::key_allowed_list];
            for (int j = 0; j < allowedCarrierLen; j++) {
                string mcc = testVector[i].pAllowed[j].mcc;
                string mnc = testVector[i].pAllowed[j].mnc;
                string testPlmn = mcc + mnc;
                string plmn = jsonAllowedList[j][ns_sim_net_carrier_info::key_plmn].asString();
                EXPECT_STREQ(testPlmn.c_str(), plmn.c_str());
                EXPECT_EQ(testVector[i].pAllowed[j].match_type,
                        (RIL_CarrierMatchType) jsonAllowedList[j][ns_sim_net_carrier_info::key_match_type].asInt());
                int matchDataLen = jsonAllowedList[j][ns_sim_net_carrier_info::key_match_data_list].size();
                EXPECT_TRUE(matchDataLen <= MAX_CR_MATCH_DATA_SIZE);
                EXPECT_EQ(strlen(testVector[i].pAllowed[j].match_data), (unsigned int) matchDataLen);
                for (int k = 0; k < matchDataLen; k++) {
                    EXPECT_EQ(testVector[i].pAllowed[j].match_data[k],
                            jsonAllowedList[j][ns_sim_net_carrier_info::key_match_data_list][k].asInt());
                }
            }

            Json::Value jsonExcludedList = jsonReqData[ns_sim_net_getset_carrier_restrictions::key_excluded_list];
            for (int j = 0; j < excludedCarrierLen; j++) {
                string mcc = testVector[i].pExcluded[j].mcc;
                string mnc = testVector[i].pExcluded[j].mnc;
                string testPlmn = mcc + mnc;
                string plmn = jsonExcludedList[j][ns_sim_net_carrier_info::key_plmn].asString();
                EXPECT_STREQ(testPlmn.c_str(), plmn.c_str());
                EXPECT_EQ(testVector[i].pExcluded[j].match_type,
                        (RIL_CarrierMatchType) jsonExcludedList[j][ns_sim_net_carrier_info::key_match_type].asInt());
                int matchDataLen = jsonExcludedList[j][ns_sim_net_carrier_info::key_match_data_list].size();
                EXPECT_TRUE(matchDataLen <= MAX_CR_MATCH_DATA_SIZE);
                EXPECT_EQ(strlen(testVector[i].pExcluded[j].match_data), (unsigned int) matchDataLen);
                for (int k = 0; k < matchDataLen; k++) {
                    EXPECT_EQ(testVector[i].pExcluded[j].match_data[k],
                            jsonExcludedList[j][ns_sim_net_carrier_info::key_match_data_list][k].asInt());
                }
            }

        }

        if (modemData) { delete modemData; modemData = nullptr; }

    }
}

TEST(ProtocolSimBuilderJson, BuildGetCarrierRestrictions) {
    ProtocolSimBuilderJson testBuilder;

    ModemData *modemData = testBuilder.BuildGetCarrierRestrictions();
    ASSERT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_CARRIER_RESTRICTIONS);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);

    if (modemData) { delete modemData; modemData = nullptr; }
}

TEST(ProtocolSimBuilderJson, BuildGetImsi) {
    struct {
        const char* pAid;
        int aid[MAX_SIM_AID_LEN];
    } testVector[] = {
        { "A0000000871002FF33FFFF8906030100", { 0xA0, 0x00, 0x00, 0x00, 0x87, 0x10, 0x02,
                                                  0xFF, 0x33, 0xFF, 0xFF, 0x89, 0x06, 0x03, 0x01, 0x00} },
        { "A000000063504B43532D3135", { 0xA0, 0x00, 0x00, 0x00, 0x63, 0x50, 0x4B, 0x43,
                                            0x53, 0x2D, 0x31, 0x35 } },
        { NULL, { } },
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {

        ProtocolSimBuilderJson testBuilder;
        ModemData *modemData = testBuilder.BuildGetImsi(testVector[i].pAid);

        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_GET_IMSI);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        Json::Value jsonReqData;
        bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }

        ASSERT_NE(false, parsed);

        if (testVector[i].pAid == nullptr) {
            EXPECT_EQ(jsonReqData[ns_sim_id_get_imsi_req::key_aid].size(), 0U);
        } else {
            int aidLen = jsonReqData[ns_sim_id_get_imsi_req::key_aid].size();
            EXPECT_EQ((unsigned long) aidLen, strlen(testVector[i].pAid) / 2);
            for (int j = 0; j < aidLen; j++) {
                EXPECT_EQ(jsonReqData[ns_sim_id_get_imsi_req::key_aid][j], testVector[i].aid[j]);
            }
        }


        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSimBuilderJson, BuildOemSimRequest) {
    ProtocolSimBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildOemSimRequest(RIL_REQUEST_SIM_IO, NULL, 0);
    EXPECT_EQ(modemData, nullptr);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSimBuilderJson, BuildGetSimLockInfo) {
    ProtocolSimBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildGetSimLockInfo();
    EXPECT_EQ(modemData, nullptr);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}
