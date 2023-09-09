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

#ifndef __SITRILl_TEST_SIM_H__
#define __SITRILl_TEST_SIM_H__

#include "sitrilTest.h"
#include <gtest/gtest.h>
#include <sitdef.h>
#include <modemdata.h>

#define TEST_AUTH_RSP "test"
#define TEST_SIMLOCK_RSP "test"

#define TEST_GET_SIM_CARD_STATE_ABSENT 0 //ABSENT
#define TEST_GET_SIM_CARD_STATE_PRESENT 1 //PRESENT
#define TEST_GET_SIM_UNIVERSAL_PIN 0 //VERIFIED
#define TEST_GET_SIM_APPICATION_NUM 2
#define TEST_GET_STACK_ENABLED 1
#define TEST_GET_SLOT_CARD_PRESENT 1
#define TEST_GET_SLOT_CARD_ABSENT 0
#define TEST_GET_SLOT_STATE_ACTIVE 1
#define TEST_GET_SLOT_STATE_INACTIVE 0
#define TEST_PORT_STATE_ACTIVE 1
#define TEST_PORT_STATE_INACTIVE 0

#define UICC_SUB_DEACTIVATED 0
#define UICC_SUB_ACTIVATED 1

#define UICCAPPLICATION_DISABLE 0
#define UICCAPPLICATION_ENABLE 1

#define TEST_GET_STACK_ENABLED 1

typedef enum {
    SIM_CARD_POWERSTATE_POWER_DOWN,
    SIM_CARD_POWERSTATE_POWER_UP,
    SIM_CARD_POWERSTATE_POWER_UP_PASS_THROUGH,
    SIM_CARD_POWERSTATE_POWER_RESET, // Added for secure element operation in Radio v1.2
    SIM_CARD_POWERSTATE_POWER_DOWN_PRESENT, // AP send power down state to cp and expect to
    // receive sim state as present in Radio v1.6
} SIM_CARD_POWER_STATE;

const char USIM_AIDData[] = {
    0xa0, 0x00, 0x00, 0x00, 0x87, 0x10, 0x02, 0xff, 0xff, 0xff, 0xff, 0x89, 0x04, 0x03, 0x00, 0xff};
const char CSIM_AIDData[] = {
    0xa0, 0x00, 0x00, 0x03, 0x43, 0x10, 0x02, 0xf3, 0x10, 0xff, 0xff, 0x89, 0x06, 0x11, 0x00, 0x00};
const char ISIM_AIDData[] = {
    0xa0, 0x00, 0x00, 0x00, 0x87, 0x10, 0x04, 0xff, 0x82, 0xff, 0xff, 0x89, 0x04, 0x03, 0x00, 0xff};

// SLOT_STATUS
const char USIM_ATRData[] = {
    0x3b, 0x9c, 0x95, 0x80, 0x3f, 0xc3, 0xa0, 0x80, 0x31, 0xe0, 0x73, 0xf3, 0x21, 0x1b, 0x64, 0x4b,
    0x45, 0x22, 0x30, 0x4b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00};
const char USIM_ICCIDData[] = {
    0x98, 0x28, 0x03, 0x20, 0x51, 0x10, 0x93, 0x54, 0x81, 0xf5};

const char ESIM_ATRData[] = {
    0x3b, 0x9f, 0x96, 0x80, 0x3f, 0xc7, 0x82, 0x80, 0x31, 0xe0, 0x73, 0xf6, 0x21, 0x57, 0x57, 0x4a,
    0x4d, 0x02, 0x0c, 0x60, 0x30, 0x00, 0x5f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00};

const char ESIM_ICCIDData[] = {
    0x98, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf3};

const char ESIM_EIDData[] = {
    0x89, 0x04, 0x90, 0x32, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x72, 0x80, 0x89, 0x00, 0x59};

#define MAX_SIT_SIM_AID_LEN     (16)
#define MAX_DATA_SIZE           (1500)

typedef enum {
    VERIFY_PIN = 0,
    VERIFY_PIN2,
    VERIFY_PUK,
    VERIFY_PUK2,
    CHANGE_PIN,
    CHANGE_PIN2,
} PIN_PUK_RSP_INDEX;

typedef enum{
    DISPLAY_TEXT_NORMAL = 0,
    GET_INKEY_NORNAL,
    GET_INPUT_NORMAL,
    MORE_TIME,
    PLAY_TONE_NORMAL,
    POLL_INTERVAL,
    REFRESH_NORMAL,
    SET_UP_MENU_NORMAL,
    SELECT_ITEM,
    PROVIDE_LOCAL_INFORMATION,
    SET_UP_EVENT_LIST_NORMAL,
    PERFORM_CARD_APDU,
    POWER_OFF_CARD,
    POWER_ON_CARD,
    TIMER_MANAGEMENT_NORMAL,
    SET_UP_IDLE_MODE_TEXT,
    RUN_AT_COMMAND,
    LANGUAGE_NOTIFICATION,
    LAUNCH_BROWSER,
    GET_CHANNEL_STATUS,
    ACTIVATE,
    CONTACTLESS_STATE_CHANGED,
    PROACTIVE_UICC_COMMANDS_NUM,
    SIM_REFRESH_FILE_UPDATE,
    SIM_REFRESH_INIT,
    SIM_REFRESH_RESET
} PROACTIVE_UICC_COMMANDS;


// For GetFacilityLock
#define FACILITY        "FD"
#define PWD             "1234"
#define SERVICE_CLASS   "1"
#define APPID           "9876"

#pragma pack(1)
typedef struct tagSitSimOpenChannelRequest
{
    unsigned char aid_len;
    unsigned char aid[MAX_SIT_SIM_AID_LEN];
    unsigned char p2;
} SitSimOpenChannelReq;

typedef struct tagSitSimTransmitApduChannelRequest
{
    int session_id;
    int cla;
    int instruction;
    int p1;
    int p2;
    int p3;
    unsigned short data_len;
    unsigned char data[0];
} SitSimTransmitApduChannelReq;

typedef struct tagSitSimApdu {
    unsigned char cla;
    unsigned char instruction;
    unsigned char p1;
    unsigned char p2;
    unsigned char p3;
    unsigned char data[0];
} SitSimApdu;

typedef struct tagSitSimTransmitApduBasicRequest
{
    int session_id;
    unsigned short apdu_len;
    union {
        SitSimApdu apdu_detail;
        unsigned char apdu[0];
    };
} SitSimTransmitApduBasicReq;

typedef struct tagSimStatusParams {
    BYTE card_state;
    BYTE universal_pin_state;
    BYTE application_num;
    BYTE apps_state;
    BYTE perso_substate;
    BYTE pin1_state;

    tagSimStatusParams() {
        card_state = 1;             // RIL_CARDSTATE_PRESENT
        universal_pin_state = 3;    // RIL_PINSTATE_DISABLED
        application_num = 3;
        apps_state = 5;             // RIL_APPSTATE_READY
        perso_substate = 2;         // RIL_PERSOSUBSTATE_READY
        pin1_state = 3;             // RIL_PINSTATE_DISABLED
    }
} tSimStatusParams;

typedef struct tagBipStatusResponse{
    unsigned char cid;
    unsigned char status;
} BipStatusResponse;

typedef struct tagBipDataInfo{
    unsigned char cid;
    unsigned short length;
    unsigned char data[MAX_DATA_SIZE];
} BipDataInfo;

#pragma pack()

class SitrilTestSim : public ::testing::TestWithParam<int> {
    public:
        char testPin[MAX_SIM_PIN_LEN + 1];
        char testPuk[MAX_SIM_PIN_LEN + 1];
        char testAid[(MAX_SIM_AID_LEN*2) + 1];
        char testPwd[MAX_SIM_FACILITY_PASSWORD_LEN + 1];

    protected:
        int mPhoneIndex;
        SitrilTest *mSitRilTest;

    /* static */
    static void SetUpTestSuite() {}
    static void TearDownTestSuite() {}

    public:
    virtual void SetUp() override;
    virtual void TearDown() override;
    virtual void ChangeSimStatus(tSimStatusParams *pParams);
    virtual void SetDefaultSimStatus();

    SitrilTest *getSitrilTest() { return mSitRilTest; }
};
#endif
