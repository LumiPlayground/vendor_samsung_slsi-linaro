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
#include <gmock/gmock.h>
#include <functional>

#include "../wlbtlog.h"
#include "../commandDispatcher.h"

typedef std::function<bool(const char*, const char*)> handler_t;

enum CMD {
    WIFI_START_MX,
    WIFI_START_UDI,
    WIFI_START_ALL,
    WIFI_STOP_MX,
    WIFI_STOP_UDI,
    WIFI_STOP_ALL,
    WIFI_SAVE_HIP4,
    BT_START_GENERAL,
    BT_START_AUDIO,
    BT_START_CUSTOM,
    BT_STOP
};

class MockWLBTLog : public WlbtLog {
 public:
    MOCK_METHOD(bool, start_mxlog, (const char*, const char*));
    MOCK_METHOD(bool, start_udilog, (const char*, const char*));
    MOCK_METHOD(bool, start_hip4_profile, (const char*, const char*));
    MOCK_METHOD(bool, start_bt_normal_log, (const char*, const char*));
    MOCK_METHOD(bool, start_bt_audio_log, (const char*, const char*));
    MOCK_METHOD(bool, start_bt_custom_log, (const char*, const char*));
    MOCK_METHOD(bool, stop_udilog, (const char*, const char*));
    MOCK_METHOD(bool, stop_bt_log, (const char*, const char*));
    MOCK_METHOD(bool, stop_mxlog, (const char*, const char*));
};

static std::map<CMD, const char*> jsonString = {
    {WIFI_START_MX, "{\"name\":\"wifilog\",\"exec\":\"start\",\"option\":\"mxlog\",\"dir\":\"dir\"}"},
    {WIFI_START_UDI, "{\"name\":\"wifilog\",\"exec\":\"start\",\"option\":\"udilog\",\"dir\":\"dir\"}"},
    {WIFI_START_ALL, "{\"name\":\"wifilog\",\"exec\":\"start\",\"option\":\"all\",\"dir\":\"dir\"}"},
    {WIFI_STOP_MX, "{\"name\":\"wifilog\",\"exec\":\"stop\",\"option\":\"mxlog\",\"dir\":\"\"}"},
    {WIFI_STOP_UDI, "{\"name\":\"wifilog\",\"exec\":\"stop\",\"option\":\"udilog\",\"dir\":\"\"}"},
    {WIFI_STOP_ALL, "{\"name\":\"wifilog\",\"exec\":\"stop\",\"option\":\"all\",\"dir\":\"\"}"},
    {WIFI_SAVE_HIP4, "{\"name\":\"wifilog\",\"exec\":\"hip4\",\"option\":\"hip4\",\"dir\":\"dir\"}"},
    {BT_START_GENERAL, "{\"name\":\"btlog\",\"exec\":\"start\",\"option\":\"general\",\"dir\":\"dir\"}"},
    {BT_START_AUDIO, "{\"name\":\"btlog\",\"exec\":\"start\",\"option\":\"audio\",\"dir\":\"dir\"}"},
    {BT_START_CUSTOM,
        "{\"name\":\"btlog\",\"exec\":\"start\",\"option\":\"custom\",\"dir\":\"dir\",\"data\":\"0xffffffff\"}"},
    {BT_STOP, "{\"name\":\"btlog\",\"exec\":\"stop\",\"option\":\"\",\"dir\":\"\"}"}
};

class CommadDispatcherNotRegisterTest : public ::testing::Test {
 protected:
    commandDispatcher* dispatcherTest;
    MockWLBTLog *mock;
    virtual void SetUp() {
        dispatcherTest = new commandDispatcher();
        mock = new MockWLBTLog();
    }
    virtual void TearDown() {
        delete dispatcherTest;
        delete mock;
    }
};

TEST_F(CommadDispatcherNotRegisterTest, NotInvokeWifilogStartMxlog) {
    EXPECT_CALL(*mock, start_mxlog(testing::_, testing::_)).Times(0);

    dispatcherTest->parserBuffer(const_cast<char *>(jsonString[WIFI_START_MX]));
    dispatcherTest->runCommand();
}

TEST_F(CommadDispatcherNotRegisterTest, NotInvokeWifilogStartUdilog) {
    EXPECT_CALL(*mock, start_udilog(testing::_, testing::_)).Times(0);

    dispatcherTest->parserBuffer(const_cast<char *>(jsonString[WIFI_START_UDI]));
    dispatcherTest->runCommand();
}

TEST_F(CommadDispatcherNotRegisterTest, NotInvokeWifilogStartAll) {
    EXPECT_CALL(*mock, start_mxlog(testing::_, testing::_)).Times(0);
    EXPECT_CALL(*mock, start_udilog(testing::_, testing::_)).Times(0);

    dispatcherTest->parserBuffer(const_cast<char *>(jsonString[WIFI_START_ALL]));
    dispatcherTest->runCommand();
}

TEST_F(CommadDispatcherNotRegisterTest, NotInvokeWifilogStopMxlog) {
    EXPECT_CALL(*mock, stop_mxlog(testing::_, testing::_)).Times(0);

    dispatcherTest->parserBuffer(const_cast<char *>(jsonString[WIFI_STOP_MX]));
    dispatcherTest->runCommand();
}

TEST_F(CommadDispatcherNotRegisterTest, NotInvokeWifilogStopUdilog) {
    EXPECT_CALL(*mock, stop_udilog(testing::_, testing::_)).Times(0);

    dispatcherTest->parserBuffer(const_cast<char *>(jsonString[WIFI_STOP_UDI]));
    dispatcherTest->runCommand();
}

TEST_F(CommadDispatcherNotRegisterTest, NotInvokeWifilogStopAll) {
    EXPECT_CALL(*mock, stop_mxlog(testing::_, testing::_)).Times(0);
    EXPECT_CALL(*mock, stop_udilog(testing::_, testing::_)).Times(0);

    dispatcherTest->parserBuffer(const_cast<char *>(jsonString[WIFI_STOP_ALL]));
    dispatcherTest->runCommand();
}

TEST_F(CommadDispatcherNotRegisterTest, NotInvokeWifilogHip4) {
    EXPECT_CALL(*mock, start_hip4_profile(testing::_, testing::_)).Times(0);

    dispatcherTest->parserBuffer(const_cast<char *>(jsonString[WIFI_SAVE_HIP4]));
    dispatcherTest->runCommand();
}

TEST_F(CommadDispatcherNotRegisterTest, NotInvokeBtlogStartGeneral) {
    EXPECT_CALL(*mock, start_bt_normal_log(testing::_, testing::_)).Times(0);
    EXPECT_CALL(*mock, start_udilog(testing::_, testing::_)).Times(0);

    dispatcherTest->parserBuffer(const_cast<char *>(jsonString[BT_START_GENERAL]));
    dispatcherTest->runCommand();
}

TEST_F(CommadDispatcherNotRegisterTest, NotInvokeBtlogStartAudio) {
    EXPECT_CALL(*mock, start_bt_audio_log(testing::_, testing::_)).Times(0);
    EXPECT_CALL(*mock, start_udilog(testing::_, testing::_)).Times(0);

    dispatcherTest->parserBuffer(const_cast<char *>(jsonString[BT_START_AUDIO]));
    dispatcherTest->runCommand();
}

TEST_F(CommadDispatcherNotRegisterTest, NotInvokeBtlogStartCustom) {
    EXPECT_CALL(*mock, start_bt_custom_log(testing::_, testing::_)).Times(0);
    EXPECT_CALL(*mock, start_udilog(testing::_, testing::_)).Times(0);

    dispatcherTest->parserBuffer(const_cast<char *>(jsonString[BT_START_CUSTOM]));
    dispatcherTest->runCommand();
}

TEST_F(CommadDispatcherNotRegisterTest, NotInvokeBtlogStop) {
    EXPECT_CALL(*mock, stop_bt_log(testing::_, testing::_)).Times(0);
    EXPECT_CALL(*mock, stop_udilog(testing::_, testing::_)).Times(0);

    dispatcherTest->parserBuffer(const_cast<char *>(jsonString[BT_STOP]));
    dispatcherTest->runCommand();
}

#define HANDLER_BIND_ARG std::placeholders::_1, std::placeholders::_2
class CommadDispatcherTest : public ::testing::Test {
 protected:
    commandDispatcher* dispatcherTest;
    MockWLBTLog * mock;
    virtual void SetUp() {
        dispatcherTest = new commandDispatcher();
        mock = new MockWLBTLog();

        registerWiifCommand();
        registerBtCommand();
    }
    virtual void TearDown() {
        delete dispatcherTest;
        delete mock;
    }
    void registerWiifCommand() {
        dispatcherTest->registerCommand("wifilog start mxlog",
            { handler_t(std::bind(&MockWLBTLog::start_mxlog, mock, HANDLER_BIND_ARG)) });
        dispatcherTest->registerCommand("wifilog start udilog",
            { handler_t(std::bind(&MockWLBTLog::start_udilog, mock, HANDLER_BIND_ARG)) });
        dispatcherTest->registerCommand("wifilog start all",
            { handler_t(std::bind(&MockWLBTLog::start_mxlog, mock, HANDLER_BIND_ARG)),
            handler_t(std::bind(&MockWLBTLog::start_udilog, mock, HANDLER_BIND_ARG)) });
        dispatcherTest->registerCommand("wifilog stop mxlog",
            { handler_t(std::bind(&MockWLBTLog::stop_mxlog, mock, HANDLER_BIND_ARG)) });
        dispatcherTest->registerCommand("wifilog stop udilog",
            { handler_t(std::bind(&MockWLBTLog::stop_udilog, mock, HANDLER_BIND_ARG)) });
        dispatcherTest->registerCommand("wifilog stop all",
            { handler_t(std::bind(&MockWLBTLog::stop_mxlog, mock, HANDLER_BIND_ARG)),
            handler_t(std::bind(&MockWLBTLog::stop_udilog, mock, HANDLER_BIND_ARG)) });
        dispatcherTest->registerCommand("wifilog hip4 hip4",
            { handler_t(std::bind(&MockWLBTLog::start_hip4_profile, mock, HANDLER_BIND_ARG)) });
    }
    void registerBtCommand() {
        dispatcherTest->registerCommand("btlog start general",
            { handler_t(std::bind(&MockWLBTLog::start_bt_normal_log, mock, HANDLER_BIND_ARG)),
            handler_t(std::bind(&MockWLBTLog::start_udilog, mock, HANDLER_BIND_ARG)) });
        dispatcherTest->registerCommand("btlog start audio",
            { handler_t(std::bind(&MockWLBTLog::start_bt_audio_log, mock, HANDLER_BIND_ARG)),
            handler_t(std::bind(&MockWLBTLog::start_udilog, mock, HANDLER_BIND_ARG)) });
        dispatcherTest->registerCommand("btlog start custom",
            { handler_t(std::bind(&MockWLBTLog::start_bt_custom_log, mock, HANDLER_BIND_ARG)),
            handler_t(std::bind(&MockWLBTLog::start_udilog, mock, HANDLER_BIND_ARG)) });
        dispatcherTest->registerCommand("btlog stop",
            { handler_t(std::bind(&MockWLBTLog::stop_bt_log, mock, HANDLER_BIND_ARG)),
            handler_t(std::bind(&MockWLBTLog::stop_udilog, mock, HANDLER_BIND_ARG)) });
        dispatcherTest->registerCommand("btlog stop",
            { handler_t(std::bind(&MockWLBTLog::stop_bt_log, mock, HANDLER_BIND_ARG)),
            handler_t(std::bind(&MockWLBTLog::stop_udilog, mock, HANDLER_BIND_ARG)) });
    }
};

TEST_F(CommadDispatcherTest, parserBufferInvalidJsonFormatCheck) {
    EXPECT_EQ(false, dispatcherTest->parserBuffer(NULL));
    EXPECT_EQ(false, dispatcherTest->parserBuffer(const_cast<char *>("")));
    EXPECT_EQ(false, dispatcherTest->parserBuffer(const_cast<char *>("}")));
    EXPECT_EQ(false, dispatcherTest->parserBuffer(const_cast<char *>("{")));
    EXPECT_EQ(false, dispatcherTest->parserBuffer(const_cast<char *>("{:}")));
    EXPECT_EQ(false, dispatcherTest->parserBuffer(const_cast<char *>("{key:value}")));
    EXPECT_EQ(false, dispatcherTest->parserBuffer(const_cast<char *>("{\"key\":}")));
    EXPECT_EQ(false, dispatcherTest->parserBuffer(const_cast<char *>("{:\"value\"}")));
    EXPECT_EQ(false, dispatcherTest->parserBuffer(const_cast<char *>("{\"key\":value}")));
}

TEST_F(CommadDispatcherTest, wifilogStartMxlog) {
    EXPECT_CALL(*mock, start_mxlog(testing::_, testing::_)).Times(1);

    dispatcherTest->parserBuffer(const_cast<char *>(jsonString[WIFI_START_MX]));
    dispatcherTest->runCommand();
}

TEST_F(CommadDispatcherTest, wifilogStartMxlogCheckDirArg) {
    EXPECT_CALL(*mock, start_mxlog(testing::StrEq("dir"), testing::_)).Times(1);

    dispatcherTest->parserBuffer(const_cast<char *>(jsonString[WIFI_START_MX]));
    dispatcherTest->runCommand();
}

TEST_F(CommadDispatcherTest, wifilogStartMxlogCheckSocketArg) {
    EXPECT_CALL(*mock, start_mxlog(testing::StrEq("dir"), testing::_)).Times(1);

    dispatcherTest->parserBuffer(const_cast<char *>(jsonString[WIFI_START_MX]));
    dispatcherTest->runCommand();
}

TEST_F(CommadDispatcherTest, wifilogStartUdilog) {
    EXPECT_CALL(*mock, start_udilog(testing::_, testing::_)).Times(1);

    dispatcherTest->parserBuffer(const_cast<char *>(jsonString[WIFI_START_UDI]));
    dispatcherTest->runCommand();
}

TEST_F(CommadDispatcherTest, wifilogStartUdilogCheckDirArg) {
    EXPECT_CALL(*mock, start_udilog(testing::StrEq("dir"), testing::_)).Times(1);

    dispatcherTest->parserBuffer(const_cast<char *>(jsonString[WIFI_START_UDI]));
    dispatcherTest->runCommand();
}

TEST_F(CommadDispatcherTest, wifilogStartUdilogCheckSocketArg) {
    EXPECT_CALL(*mock, start_udilog(testing::StrEq("dir"), testing::_)).Times(1);

    dispatcherTest->parserBuffer(const_cast<char *>(jsonString[WIFI_START_UDI]));
    dispatcherTest->runCommand();
}

TEST_F(CommadDispatcherTest, wifilogStartAll) {
    testing::InSequence seq;

    EXPECT_CALL(*mock, start_mxlog(testing::_, testing::_)).Times(1);
    EXPECT_CALL(*mock, start_udilog(testing::_, testing::_)).Times(1);

    dispatcherTest->parserBuffer(const_cast<char *>(jsonString[WIFI_START_ALL]));
    dispatcherTest->runCommand();
}

TEST_F(CommadDispatcherTest, wifilogStartAllCheckDirPostfixArg) {
    testing::InSequence seq;

    EXPECT_CALL(*mock, start_mxlog(testing::StrEq("dir_mxlog"), testing::_)).Times(1);
    EXPECT_CALL(*mock, start_udilog(testing::StrEq("dir_udilog"), testing::_)).Times(1);

    dispatcherTest->parserBuffer(const_cast<char *>(jsonString[WIFI_START_ALL]));
    dispatcherTest->runCommand();
}

TEST_F(CommadDispatcherTest, wifilogStopMxlog) {
    EXPECT_CALL(*mock, stop_mxlog(testing::_, testing::_)).Times(1);

    dispatcherTest->parserBuffer(const_cast<char *>(jsonString[WIFI_STOP_MX]));
    dispatcherTest->runCommand();
}

TEST_F(CommadDispatcherTest, wifilogStopUdilog) {
    EXPECT_CALL(*mock, stop_udilog(testing::_, testing::_)).Times(1);

    dispatcherTest->parserBuffer(const_cast<char *>(jsonString[WIFI_STOP_UDI]));
    dispatcherTest->runCommand();
}

TEST_F(CommadDispatcherTest, wifilogStopAll) {
    testing::InSequence seq;

    EXPECT_CALL(*mock, stop_mxlog(testing::_, testing::_)).Times(1);
    EXPECT_CALL(*mock, stop_udilog(testing::_, testing::_)).Times(1);

    dispatcherTest->parserBuffer(const_cast<char *>(jsonString[WIFI_STOP_ALL]));
    dispatcherTest->runCommand();
}

TEST_F(CommadDispatcherTest, wifilogSaveHip4CheckArg) {
    EXPECT_CALL(*mock, start_hip4_profile(testing::StrEq("dir"), testing::StrEq(""))).Times(1);

    dispatcherTest->parserBuffer(const_cast<char *>(jsonString[WIFI_SAVE_HIP4]));
    dispatcherTest->runCommand();
}

TEST_F(CommadDispatcherTest, btlogStartGeneral) {
    testing::InSequence seq;

    EXPECT_CALL(*mock, start_bt_normal_log(testing::StrEq("dir_mxlog"), testing::StrEq(""))).Times(1);
    EXPECT_CALL(*mock, start_udilog(testing::StrEq("dir_udilog"), testing::StrEq(""))).Times(1);

    dispatcherTest->parserBuffer(const_cast<char *>(jsonString[BT_START_GENERAL]));
    dispatcherTest->runCommand();
}

TEST_F(CommadDispatcherTest, btlogStartAudio) {
    testing::InSequence seq;

    EXPECT_CALL(*mock, start_bt_audio_log(testing::StrEq("dir_mxlog"), testing::StrEq(""))).Times(1);
    EXPECT_CALL(*mock, start_udilog(testing::StrEq("dir_udilog"), testing::StrEq(""))).Times(1);

    dispatcherTest->parserBuffer(const_cast<char *>(jsonString[BT_START_AUDIO]));
    dispatcherTest->runCommand();
}

TEST_F(CommadDispatcherTest, btlogStartCustom) {
    testing::InSequence seq;

    EXPECT_CALL(*mock, start_bt_custom_log(testing::_, testing::_)).Times(1);
    EXPECT_CALL(*mock, start_udilog(testing::_, testing::_)).Times(1);

    dispatcherTest->parserBuffer(const_cast<char *>(jsonString[BT_START_CUSTOM]));
    dispatcherTest->runCommand();
}

TEST_F(CommadDispatcherTest, btlogStartCustomCheckDataArg) {
    testing::InSequence seq;

    EXPECT_CALL(*mock,
        start_bt_custom_log(testing::StrEq("dir_mxlog"), testing::StrEq("0xffffffff"))).Times(1);
    EXPECT_CALL(*mock,
        start_udilog(testing::StrEq("dir_udilog"), testing::StrEq("0xffffffff"))).Times(1);

    dispatcherTest->parserBuffer(const_cast<char *>(jsonString[BT_START_CUSTOM]));
    dispatcherTest->runCommand();
}

TEST_F(CommadDispatcherTest, btlogStop) {
    testing::InSequence seq;

    EXPECT_CALL(*mock, stop_bt_log(testing::_, testing::_)).Times(1);
    EXPECT_CALL(*mock, stop_udilog(testing::_, testing::_)).Times(1);

    dispatcherTest->parserBuffer(const_cast<char *>(jsonString[BT_STOP]));
    dispatcherTest->runCommand();
}

TEST_F(CommadDispatcherTest, WrongJsonValueCheck) {
    EXPECT_CALL(*mock, start_mxlog(testing::_, testing::_)).Times(0);
    EXPECT_CALL(*mock, start_udilog(testing::_, testing::_)).Times(0);
    EXPECT_CALL(*mock, stop_mxlog(testing::_, testing::_)).Times(0);
    EXPECT_CALL(*mock, stop_udilog(testing::_, testing::_)).Times(0);

    EXPECT_CALL(*mock, start_bt_normal_log(testing::_, testing::_)).Times(0);
    EXPECT_CALL(*mock, start_bt_audio_log(testing::_, testing::_)).Times(0);
    EXPECT_CALL(*mock, start_bt_custom_log(testing::_, testing::_)).Times(0);
    EXPECT_CALL(*mock, start_udilog(testing::_, testing::_)).Times(0);
    EXPECT_CALL(*mock, stop_bt_log(testing::_, testing::_)).Times(0);
    EXPECT_CALL(*mock, stop_udilog(testing::_, testing::_)).Times(0);

    EXPECT_CALL(*mock, start_hip4_profile(testing::_, testing::_)).Times(0);

    dispatcherTest->parserBuffer(
        const_cast<char *>("{\"name\":\"Wrong\",\"exec\":\"Wrong\",\"option\":\"Wrong\",\"dir\":\"Wrong\"}"));
    dispatcherTest->runCommand();
}

