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

#include "../wlandServer.h"

static const char* kSocketTestWland = "test_wland";

typedef std::function<bool(char*)> handler_t;

static bool testUserHandler(::testing::Unused) GTEST_ATTRIBUTE_UNUSED_;
static bool testUserHandler(::testing::Unused) {
    return true;
}

class MockHandler {
 public:
    MOCK_METHOD(bool, testUserHandler, (char*));
};

class clientFake {
 private:
    int sock;
    const int kResultSize = 1;

 public:
    clientFake() {
        sock = -1;
    }
    ~clientFake() {
        if (sock >= 0) {
            close(sock);
        }
    }
    bool connect(const char* name) {
        sock = socket_local_client(name, ANDROID_SOCKET_NAMESPACE_ABSTRACT, SOCK_STREAM);
        if (sock < 0) {
            return false;
        }
        return true;
    }
    bool send_data(std::string data) {
        if (send(sock, data.c_str(), strlen(data.c_str()), 0) < 0) {
            return false;
        }
        return true;
    }
    bool send_data(int size) {
        size = htonl(size);
        if (send(sock, &size, 4, 0) < 0) {
            return false;
        }
        return true;
    }
    bool recvResultCode() {
        char result = 0;
        int readLen = 0, recvCode = 0;

        readLen = recv(sock, &result, kResultSize, 0);
        if (readLen == kResultSize) {
            recvCode = result - '0';
            return (recvCode == 0) ? true : false;
        }
        return false;
    }
};

class WlandServerTest : public ::testing::Test {
 protected:
    wlandServer* serverTest;
    clientFake* clientTest;
    MockHandler *mock;
    virtual void SetUp() {
        serverTest = new wlandServer();
        clientTest = new clientFake();
        mock = new MockHandler();
    }
    virtual void TearDown() {
        delete serverTest;
        delete clientTest;
        delete mock;
    }
};

TEST_F(WlandServerTest, createServerNullHandlerTest) {
    EXPECT_FALSE(serverTest->createServer(kSocketTestWland, NULL));
}

TEST_F(WlandServerTest, createServerInvalidPathTest) {
    bool ret = false;

    ret = serverTest->createServer(NULL, handler_t(std::bind(&MockHandler::testUserHandler,
        mock, std::placeholders::_1)));
    EXPECT_EQ(ret, false);
}

TEST_F(WlandServerTest, createServerTest) {
    bool ret = false;

    ret = serverTest->createServer(kSocketTestWland, handler_t(std::bind(&MockHandler::testUserHandler,
        mock, std::placeholders::_1)));

    EXPECT_EQ(ret, true);
}

TEST_F(WlandServerTest, connectInvalidPathTest) {
    bool ret = false;

    ret = serverTest->createServer(NULL, handler_t(std::bind(&MockHandler::testUserHandler,
        mock, std::placeholders::_1)));
    EXPECT_EQ(ret, false);

    EXPECT_FALSE(clientTest->connect(kSocketTestWland));
}

TEST_F(WlandServerTest, ConnectNullHandlerTest) {
    bool ret = false;

    ret = serverTest->createServer(kSocketTestWland, NULL);
    EXPECT_EQ(ret, false);
    EXPECT_FALSE(clientTest->connect(kSocketTestWland));
}

TEST_F(WlandServerTest, connectTest) {
    bool ret = false;

    ret = serverTest->createServer(kSocketTestWland, handler_t(std::bind(&MockHandler::testUserHandler,
        mock, std::placeholders::_1)));
    EXPECT_EQ(ret, true);

    EXPECT_TRUE(clientTest->connect(kSocketTestWland));
}

TEST_F(WlandServerTest, runServiceTest) {
    bool ret = false;
    std::string sendString("wland");

    EXPECT_CALL(*mock, testUserHandler(testing::_)).Times(1);

    ret = serverTest->createServer(kSocketTestWland, handler_t(std::bind(&MockHandler::testUserHandler,
        mock, std::placeholders::_1)));
    EXPECT_EQ(ret, true);

    EXPECT_TRUE(clientTest->connect(kSocketTestWland));
    EXPECT_TRUE(serverTest->waitClientConnection());

    EXPECT_TRUE(clientTest->send_data(sendString.length()));
    EXPECT_TRUE(clientTest->send_data(sendString));

    serverTest->runService();
}

TEST_F(WlandServerTest, serverBufferTest) {
    bool ret = false;
    std::string sendString("wland buffer test");

    EXPECT_CALL(*mock, testUserHandler(testing::StrEq(sendString.c_str()))).Times(1);
    ret = serverTest->createServer(kSocketTestWland, handler_t(std::bind(&MockHandler::testUserHandler,
        mock, std::placeholders::_1)));
    EXPECT_EQ(ret, true);

    EXPECT_TRUE(clientTest->connect(kSocketTestWland));
    EXPECT_TRUE(serverTest->waitClientConnection());

    EXPECT_TRUE(clientTest->send_data(sendString.length()));
    EXPECT_TRUE(clientTest->send_data(sendString));

    serverTest->runService();
}

TEST_F(WlandServerTest, serverLongStringBufferTest) {
    bool ret = false;
    std::string sendString("12345678901234567890abcdefghijklmnopqrstuvwxyz12345678901234567890abcdefghijklmnop");

    EXPECT_CALL(*mock, testUserHandler(testing::StrEq(sendString.c_str()))).Times(1);

    ret = serverTest->createServer(kSocketTestWland, handler_t(std::bind(&MockHandler::testUserHandler,
        mock, std::placeholders::_1)));
    EXPECT_EQ(ret, true);

    EXPECT_TRUE(clientTest->connect(kSocketTestWland));
    EXPECT_TRUE(serverTest->waitClientConnection());

    EXPECT_TRUE(clientTest->send_data(sendString.length()));
    EXPECT_TRUE(clientTest->send_data(sendString));

    serverTest->runService();
}

TEST_F(WlandServerTest, negativeDataLenTest) {
    bool ret = false;
    std::string sendString("negative data length -1");

    EXPECT_CALL(*mock, testUserHandler(testing::_)).Times(0);

    ret = serverTest->createServer(kSocketTestWland, handler_t(std::bind(&MockHandler::testUserHandler,
        mock, std::placeholders::_1)));
    EXPECT_EQ(ret, true);

    EXPECT_TRUE(clientTest->connect(kSocketTestWland));
    EXPECT_TRUE(serverTest->waitClientConnection());

    EXPECT_TRUE(clientTest->send_data(-1));
    EXPECT_TRUE(clientTest->send_data(sendString));

    serverTest->runService();
}

TEST_F(WlandServerTest, serverBufferOneKExceedTest) {
#define MAX_BUFFER_TEST (1024+1)
    bool ret = false;
    char sendBuffer[MAX_BUFFER_TEST] = {0};

    for (int i=0; i < sizeof(sendBuffer)-1; i++) {
        sendBuffer[i] = 'a' + (i % 26);
    }

    EXPECT_CALL(*mock, testUserHandler(testing::_)).Times(0);

    ret = serverTest->createServer(kSocketTestWland, handler_t(std::bind(&MockHandler::testUserHandler,
        mock, std::placeholders::_1)));
    EXPECT_EQ(ret, true);

    EXPECT_TRUE(clientTest->connect(kSocketTestWland));
    EXPECT_TRUE(serverTest->waitClientConnection());

    std::string sendString(sendBuffer);
    EXPECT_TRUE(clientTest->send_data(sendString.length()));
    EXPECT_TRUE(clientTest->send_data(sendString));

    serverTest->runService();
}

TEST_F(WlandServerTest, serverBufferOneByteExceedTest) {
    bool ret = false;
    char sendBuffer[SOCKET_WLAND_BUFFER_SIZE+1] = {0};

    for (int i=0; i < sizeof(sendBuffer)-1; i++) {
        sendBuffer[i] = 'a' + (i % 26);
    }

    EXPECT_CALL(*mock, testUserHandler(testing::_)).Times(0);

    ret = serverTest->createServer(kSocketTestWland, handler_t(std::bind(&MockHandler::testUserHandler,
        mock, std::placeholders::_1)));
    EXPECT_EQ(ret, true);

    EXPECT_TRUE(clientTest->connect(kSocketTestWland));
    EXPECT_TRUE(serverTest->waitClientConnection());

    std::string sendString(sendBuffer);
    EXPECT_TRUE(clientTest->send_data(sendString.length()));
    EXPECT_TRUE(clientTest->send_data(sendString));

    serverTest->runService();
}

TEST_F(WlandServerTest, serverBufferMaxSizeTest) {
    bool ret = false;
    char sendBuffer[SOCKET_WLAND_BUFFER_SIZE] = {0};

    for (int i=0; i < sizeof(sendBuffer)-1; i++) {
        sendBuffer[i] = 'a' + (i % 26);
    }

    EXPECT_CALL(*mock, testUserHandler(testing::_))
        .WillOnce(::testing::Invoke([sendBuffer](char* recvBuffer) {
            for (int i=0; i < sizeof(sendBuffer); i++) {
                EXPECT_EQ(sendBuffer[i], recvBuffer[i]);
            }
            return true;
        }));

    ret = serverTest->createServer(kSocketTestWland, handler_t(std::bind(&MockHandler::testUserHandler,
        mock, std::placeholders::_1)));
    EXPECT_TRUE(clientTest->connect(kSocketTestWland));
    EXPECT_TRUE(serverTest->waitClientConnection());

    std::string sendString(sendBuffer);
    EXPECT_TRUE(clientTest->send_data(sendString.length()));
    EXPECT_TRUE(clientTest->send_data(sendString));

    serverTest->runService();
}

TEST_F(WlandServerTest, handlerResponsSuccessTest) {
    bool ret = false;
    std::string sendString("serverResponsSuccessTest");

    EXPECT_CALL(*mock, testUserHandler(testing::_))
        .WillOnce(testing::Return(true));

    ret = serverTest->createServer(kSocketTestWland, handler_t(std::bind(&MockHandler::testUserHandler,
        mock, std::placeholders::_1)));
    EXPECT_EQ(ret, true);

    EXPECT_TRUE(clientTest->connect(kSocketTestWland));
    EXPECT_TRUE(serverTest->waitClientConnection());

    EXPECT_TRUE(clientTest->send_data(sendString.length()));
    EXPECT_TRUE(clientTest->send_data(sendString));

    serverTest->runService();
    EXPECT_TRUE(clientTest->recvResultCode());
}

TEST_F(WlandServerTest, handlerResponsFailTest) {
    bool ret = false;
    std::string sendString("serverResponsSuccessTest");

    EXPECT_CALL(*mock, testUserHandler(testing::_))
        .WillOnce(testing::Return(false));

    ret = serverTest->createServer(kSocketTestWland, handler_t(std::bind(&MockHandler::testUserHandler,
        mock, std::placeholders::_1)));
    EXPECT_EQ(ret, true);

    EXPECT_TRUE(clientTest->connect(kSocketTestWland));
    EXPECT_TRUE(serverTest->waitClientConnection());

    EXPECT_TRUE(clientTest->send_data(sendString.length()));
    EXPECT_TRUE(clientTest->send_data(sendString));

    serverTest->runService();
    EXPECT_FALSE(clientTest->recvResultCode());
}

