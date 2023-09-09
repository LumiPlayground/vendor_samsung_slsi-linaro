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
 * dmstatemachine_test.cpp
 *
 *  Created on: 2021. 10. 12.
 */
#include <gtest/gtest.h>
#include "dmd_test_util.h"
#include "dmstatemachine.h"
#include <string>

TEST(DMStateMachine, DMStateMachine) {
    struct TestState : public DMState {
        std::string name;
        bool entered = false;
        TestState() : TestState("") {};
        TestState(const std::string& name) {
            this->name = name;
        }
        std::string toString() const { return name; }
        void enter() { entered = true; }
        void exit() { entered = false; }
        void onModemDataReceive(const void */*data*/, size_t /*datalen*/) {

        }
        void onModemStateChanged(bool /*online*/) {

        }
    };

    struct TestFactory : public DMStateFactory {
        TestState *host;
        TestState *silent;
        TestState *onboard;
        TestState *factory;

        TestFactory() {
            host = new TestState("Host");
            silent = new TestState("Silent");
            onboard = new TestState("OnBoard");
            factory = new TestState("Factory");
        }
        DMState *createHostState(DMStateMachine& /*dsm*/) { return host; }
        DMState *createSilentLoggingState(DMStateMachine& /*dsm*/) { return silent; }
        DMState *createOnBoardAppState(DMStateMachine& /*dsm*/) { return onboard; }
        DMState *createFactoryState(DMStateMachine& /*dsm*/) { return factory; }
    };

    TestFactory testFactory;
    DMStateMachine test;
    test.init(&testFactory);

    // the first state
    EXPECT_EQ(test.getCurrentState(), testFactory.host);

    // to silent
    test.setState(DM_STATE_SIELNT_LOGGING_STATE);
    EXPECT_EQ(test.getLastState(), testFactory.host);
    EXPECT_EQ(test.getCurrentState(), testFactory.silent);
    EXPECT_FALSE(testFactory.host->entered);
    EXPECT_TRUE(testFactory.silent->entered);

    // to onboard
    test.setState(DM_STATE_ON_BOARD_APP_STATE);
    EXPECT_EQ(test.getLastState(), testFactory.silent);
    EXPECT_EQ(test.getCurrentState(), testFactory.onboard);
    EXPECT_FALSE(testFactory.silent->entered);
    EXPECT_TRUE(testFactory.onboard->entered);

    // to factory
    test.setState(DM_STATE_FACTORY_DM_STATE);
    EXPECT_EQ(test.getLastState(), testFactory.onboard);
    EXPECT_EQ(test.getCurrentState(), testFactory.factory);
    EXPECT_FALSE(testFactory.onboard->entered);
    EXPECT_TRUE(testFactory.factory->entered);

    // to host
    test.setState(testFactory.host);
    EXPECT_EQ(test.getLastState(), testFactory.factory);
    EXPECT_EQ(test.getCurrentState(), testFactory.host);
    EXPECT_FALSE(testFactory.factory->entered);
    EXPECT_TRUE(testFactory.host->entered);

    // to silent
    test.setState(testFactory.silent);
    EXPECT_EQ(test.getLastState(), testFactory.host);
    EXPECT_EQ(test.getCurrentState(), testFactory.silent);
    EXPECT_FALSE(testFactory.host->entered);
    EXPECT_TRUE(testFactory.silent->entered);

    // to onboard
    test.setState(testFactory.onboard);
    EXPECT_EQ(test.getLastState(), testFactory.silent);
    EXPECT_EQ(test.getCurrentState(), testFactory.onboard);
    EXPECT_FALSE(testFactory.silent->entered);
    EXPECT_TRUE(testFactory.onboard->entered);

    // to factory
    test.setState(testFactory.factory);
    EXPECT_EQ(test.getLastState(), testFactory.onboard);
    EXPECT_EQ(test.getCurrentState(), testFactory.factory);
    EXPECT_FALSE(testFactory.onboard->entered);
    EXPECT_TRUE(testFactory.factory->entered);
}

TEST(DMStateMachine, onModemDataReceive) {
    struct TestState : public DMState {
        std::string name;
        bool online = false;
        std::string result;
        TestState() : TestState("") {};
        TestState(const std::string& name) {
            this->name = name;
            result = "";
        }
        std::string toString() const { return name; }
        void enter() {}
        void exit() { result = ""; }
        void onModemDataReceive(const void *data, size_t datalen) {
            result = ::toHexdump(data, datalen);
        }
        void onModemStateChanged(bool online) {
            this->online = online;
        }
    };

    struct TestFactory : public DMStateFactory {
        TestState *host;
        TestState *silent;
        TestState *onboard;
        TestState *factory;

        TestFactory() {
            host = new TestState("Host");
            silent = new TestState("Silent");
            onboard = new TestState("OnBoard");
            factory = new TestState("Factory");
        }
        DMState *createHostState(DMStateMachine& /*dsm*/) { return host; }
        DMState *createSilentLoggingState(DMStateMachine& /*dsm*/) { return silent; }
        DMState *createOnBoardAppState(DMStateMachine& /*dsm*/) { return onboard; }
        DMState *createFactoryState(DMStateMachine& /*dsm*/) { return factory; }
    };

    TestFactory testFactory;
    DMStateMachine test;
    test.init(&testFactory);

    char testVector[10];
    test.onReceive(testVector, sizeof(testVector));
    // the first state
    EXPECT_EQ(test.getCurrentState(), testFactory.host);
    EXPECT_TRUE(::toHexdump(testVector, sizeof(testVector)) == testFactory.host->result);

    // to silent
    test.setState(DM_STATE_SIELNT_LOGGING_STATE);
    EXPECT_EQ(test.getLastState(), testFactory.host);
    EXPECT_EQ(test.getCurrentState(), testFactory.silent);
    test.onReceive(testVector, sizeof(testVector));
    EXPECT_EQ(testFactory.host->result.length(), 0);
    EXPECT_TRUE(::toHexdump(testVector, sizeof(testVector)) == testFactory.silent->result);

    // to onboard
    test.setState(DM_STATE_ON_BOARD_APP_STATE);
    EXPECT_EQ(test.getLastState(), testFactory.silent);
    EXPECT_EQ(test.getCurrentState(), testFactory.onboard);
    test.onReceive(testVector, sizeof(testVector));
    EXPECT_EQ(testFactory.silent->result.length(), 0);
    EXPECT_TRUE(::toHexdump(testVector, sizeof(testVector)) == testFactory.onboard->result);

    // to factory
    test.setState(DM_STATE_FACTORY_DM_STATE);
    EXPECT_EQ(test.getLastState(), testFactory.onboard);
    EXPECT_EQ(test.getCurrentState(), testFactory.factory);
    test.onReceive(testVector, sizeof(testVector));
    EXPECT_EQ(testFactory.onboard->result.length(), 0);
    EXPECT_TRUE(::toHexdump(testVector, sizeof(testVector)) == testFactory.factory->result);
}

TEST(DMStateMachine, onModemStateChanged) {
    struct TestState : public DMState {
        std::string name;
        bool online = false;
        std::string result;
        TestState() : TestState("") {};
        TestState(const std::string& name) {
            this->name = name;
            result = "";
        }
        std::string toString() const { return name; }
        void enter() {}
        void exit() { result = ""; }
        void onModemDataReceive(const void *data, size_t datalen) {
            result = ::toHexdump(data, datalen);
        }
        void onModemStateChanged(bool online) {
            this->online = online;
        }
    };

    struct TestFactory : public DMStateFactory {
        TestState *host;
        TestState *silent;
        TestState *onboard;
        TestState *factory;

        TestFactory() {
            host = new TestState("Host");
            silent = new TestState("Silent");
            onboard = new TestState("OnBoard");
            factory = new TestState("Factory");
        }
        DMState *createHostState(DMStateMachine& /*dsm*/) { return host; }
        DMState *createSilentLoggingState(DMStateMachine& /*dsm*/) { return silent; }
        DMState *createOnBoardAppState(DMStateMachine& /*dsm*/) { return onboard; }
        DMState *createFactoryState(DMStateMachine& /*dsm*/) { return factory; }
    };

    TestFactory testFactory;
    TestState *states[] = {
        testFactory.host,
        testFactory.silent,
        testFactory.onboard,
        testFactory.factory,
    };
    size_t size = sizeof(states) / sizeof(states[0]);
    DMStateMachine test;
    test.init(&testFactory);
    test.onModemStateChanged(ModemStateMonitor::State::STATE_ONLINE);
    for (size_t i = 0; i < size; i++) {
        EXPECT_TRUE(states[i]->online);
    }
    test.onModemStateChanged(ModemStateMonitor::State::STATE_CRASH_EXIT);
    for (size_t i = 0; i < size; i++) {
        EXPECT_FALSE(states[i]->online);
    }
    test.onModemStateChanged(ModemStateMonitor::State::STATE_ONLINE);
    for (size_t i = 0; i < size; i++) {
        EXPECT_TRUE(states[i]->online);
    }
    test.onModemStateChanged(ModemStateMonitor::State::STATE_CRASH_RESET);
    for (size_t i = 0; i < size; i++) {
        EXPECT_FALSE(states[i]->online);
    }
    test.onModemStateChanged(ModemStateMonitor::State::STATE_ONLINE);
    for (size_t i = 0; i < size; i++) {
        EXPECT_TRUE(states[i]->online);
    }
    test.onModemStateChanged(ModemStateMonitor::State::STATE_OFFLINE);
    for (size_t i = 0; i < size; i++) {
        EXPECT_FALSE(states[i]->online);
    }
    test.onModemStateChanged(ModemStateMonitor::State::STATE_ONLINE);
    for (size_t i = 0; i < size; i++) {
        EXPECT_TRUE(states[i]->online);
    }
    test.onModemStateChanged(ModemStateMonitor::State::STATE_BOOTING);
    for (size_t i = 0; i < size; i++) {
        EXPECT_FALSE(states[i]->online);
    }
    test.onModemStateChanged(ModemStateMonitor::State::STATE_ONLINE);
    for (size_t i = 0; i < size; i++) {
        EXPECT_TRUE(states[i]->online);
    }
    test.onModemStateChanged(ModemStateMonitor::State::STATE_ONLINE);
    for (size_t i = 0; i < size; i++) {
        EXPECT_TRUE(states[i]->online);
    }
    test.onModemStateChanged(ModemStateMonitor::State::STATE_CRASH_EXIT);
    for (size_t i = 0; i < size; i++) {
        EXPECT_FALSE(states[i]->online);
    }
    test.onModemStateChanged(ModemStateMonitor::State::STATE_CRASH_RESET);
    for (size_t i = 0; i < size; i++) {
        EXPECT_FALSE(states[i]->online);
    }
    test.onModemStateChanged(ModemStateMonitor::State::STATE_BOOTING);
    for (size_t i = 0; i < size; i++) {
        EXPECT_FALSE(states[i]->online);
    }
    test.onModemStateChanged(ModemStateMonitor::State::STATE_ONLINE);
    for (size_t i = 0; i < size; i++) {
        EXPECT_TRUE(states[i]->online);
    }
}
