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
 * servicecallbacknotifier_test.cpp
 *
 *  Created on: 2021. 9. 27.
 *      Author: sungwoo48.choi
 */
#include <gtest/gtest.h>
#include <slsi/radio_v1_4.h>
#include "modemstatemonitor.h"
#include "servicecallback.h"
#include "servicecallbacknotifier.h"
#include "servicestate.h"
#include "simcardstatus.h"

TEST(ServiceCallbackNotifier, RadioStateNotifier) {
    using ServiceCallback::RadioStateListener;
    using ServiceCallback::RadioStateNotifier;

    struct LocalListener : public RadioStateListener {
        int state;
        bool isOnRadioNotAvailable;
        bool isOnRadioOffOrNotAvailable;
        bool isOnRadioAvailable;
        bool isOnRadioOn;
        LocalListener() : state(RADIO_STATE_UNAVAILABLE) {
            isOnRadioNotAvailable = false;
            isOnRadioOffOrNotAvailable = false;
            isOnRadioAvailable = false;
            isOnRadioOn = false;
        }
        void OnRadioStateChanged(int state) {
            this->state = state;
        }
        void OnRadioNotAvailable() {
            isOnRadioNotAvailable = true;
        }
        void OnRadioOffOrNotAvailable() {
            isOnRadioOffOrNotAvailable = true;
        }
        void OnRadioAvailable() {
            isOnRadioAvailable = true;
        }
        void OnRadioOn() {
            isOnRadioOn = true;
        }
    };

    LocalListener testLsn[2];
    size_t size = sizeof(testLsn) / sizeof(testLsn[0]);
    RadioStateNotifier test;
    for (size_t i = 0; i < size; i++) {
        test.addListener(&testLsn[i]);
    }

    test.notifyRadioStateChanged((int)RADIO_STATE_UNAVAILABLE);
    for (size_t i = 0; i < size; i++) {
        EXPECT_EQ(testLsn[i].state, (int)RADIO_STATE_UNAVAILABLE);
    }
    test.notifyRadioStateChanged((int)RADIO_STATE_OFF);
    for (size_t i = 0; i < size; i++) {
        EXPECT_EQ(testLsn[i].state, (int)RADIO_STATE_OFF);
    }
    test.notifyRadioStateChanged((int)RADIO_STATE_ON);
    for (size_t i = 0; i < size; i++) {
        EXPECT_EQ(testLsn[i].state, (int)RADIO_STATE_ON);
    }

    test.notifyRadioNotAvailable();
    test.notifyRadioOffOrNotAvailable();
    test.notifyRadioAvailable();
    test.notifyRadioOn();
    for (size_t i = 0; i < size; i++) {
        EXPECT_TRUE(testLsn[i].isOnRadioNotAvailable);
        EXPECT_TRUE(testLsn[i].isOnRadioOffOrNotAvailable);
        EXPECT_TRUE(testLsn[i].isOnRadioAvailable);
        EXPECT_TRUE(testLsn[i].isOnRadioOn);
    }
}

TEST(ServiceCallbackNotifier, RadioStateNotifier_all) {
    using ServiceCallback::RadioStateListener;
    using ServiceCallback::RadioStateNotifier;

    struct LocalListener : public RadioStateListener {
        int state;
        bool isOnRadioNotAvailable;
        bool isOnRadioOffOrNotAvailable;
        bool isOnRadioAvailable;
        bool isOnRadioOn;
        LocalListener() : state(RADIO_STATE_UNAVAILABLE) {
            isOnRadioNotAvailable = false;
            isOnRadioOffOrNotAvailable = false;
            isOnRadioAvailable = false;
            isOnRadioOn = false;
        }
        void OnRadioStateChanged(int state) {
            this->state = state;
        }
        void OnRadioNotAvailable() {
            isOnRadioNotAvailable = true;
        }
        void OnRadioOffOrNotAvailable() {
            isOnRadioOffOrNotAvailable = true;
        }
        void OnRadioAvailable() {
            isOnRadioAvailable = true;
        }
        void OnRadioOn() {
            isOnRadioOn = true;
        }
        void reset() {
            isOnRadioNotAvailable = false;
            isOnRadioOffOrNotAvailable = false;
            isOnRadioAvailable = false;
            isOnRadioOn = false;
        }
    };

    LocalListener testLsn[2];
    size_t size = sizeof(testLsn) / sizeof(testLsn[0]);
    RadioStateNotifier test;
    for (size_t i = 0; i < size; i++) {
        test.addListener(&testLsn[i]);
    }

    // radioStateChanged, radioAvailable
    for (size_t i = 0; i < size; i++) {
        testLsn[i].reset();
    }
    test.notifyRadioStateChanged((int)RADIO_STATE_UNAVAILABLE,
            (int)RADIO_STATE_OFF);
    for (size_t i = 0; i < size; i++) {
        EXPECT_EQ(testLsn[i].state, (int)RADIO_STATE_OFF);
        EXPECT_FALSE(testLsn[i].isOnRadioNotAvailable);
        EXPECT_FALSE(testLsn[i].isOnRadioOffOrNotAvailable);
        EXPECT_TRUE(testLsn[i].isOnRadioAvailable);
        EXPECT_FALSE(testLsn[i].isOnRadioOn);
    }

    // radioStateChanged, radioAvailable, radioOn
    for (size_t i = 0; i < size; i++) {
        testLsn[i].reset();
    }
    test.notifyRadioStateChanged((int)RADIO_STATE_UNAVAILABLE,
            (int)RADIO_STATE_ON);
    for (size_t i = 0; i < size; i++) {
        EXPECT_EQ(testLsn[i].state, (int)RADIO_STATE_ON);
        EXPECT_FALSE(testLsn[i].isOnRadioNotAvailable);
        EXPECT_FALSE(testLsn[i].isOnRadioOffOrNotAvailable);
        EXPECT_TRUE(testLsn[i].isOnRadioAvailable);
        EXPECT_TRUE(testLsn[i].isOnRadioOn);
    }

    // radioStateChanged, radioOn
    for (size_t i = 0; i < size; i++) {
        testLsn[i].reset();
    }
    test.notifyRadioStateChanged((int)RADIO_STATE_OFF,
            (int)RADIO_STATE_ON);
    for (size_t i = 0; i < size; i++) {
        EXPECT_EQ(testLsn[i].state, (int)RADIO_STATE_ON);
        EXPECT_FALSE(testLsn[i].isOnRadioNotAvailable);
        EXPECT_FALSE(testLsn[i].isOnRadioOffOrNotAvailable);
        EXPECT_FALSE(testLsn[i].isOnRadioAvailable);
        EXPECT_TRUE(testLsn[i].isOnRadioOn);
    }

    // radioStateChanged, radioOffOrNotAvailable
    for (size_t i = 0; i < size; i++) {
        testLsn[i].reset();
    }
    test.notifyRadioStateChanged((int)RADIO_STATE_ON,
            (int)RADIO_STATE_OFF);
    for (size_t i = 0; i < size; i++) {
        EXPECT_EQ(testLsn[i].state, (int)RADIO_STATE_OFF);
        EXPECT_FALSE(testLsn[i].isOnRadioNotAvailable);
        EXPECT_TRUE(testLsn[i].isOnRadioOffOrNotAvailable);
        EXPECT_FALSE(testLsn[i].isOnRadioAvailable);
        EXPECT_FALSE(testLsn[i].isOnRadioOn);
    }

    // radioStateChanged, radioNotAvailable, radioOffOrNotAvailable
    for (size_t i = 0; i < size; i++) {
        testLsn[i].reset();
    }
    test.notifyRadioStateChanged((int)RADIO_STATE_ON,
            (int)RADIO_STATE_UNAVAILABLE);
    for (size_t i = 0; i < size; i++) {
        EXPECT_EQ(testLsn[i].state, (int)RADIO_STATE_UNAVAILABLE);
        EXPECT_TRUE(testLsn[i].isOnRadioNotAvailable);
        EXPECT_TRUE(testLsn[i].isOnRadioOffOrNotAvailable);
        EXPECT_FALSE(testLsn[i].isOnRadioAvailable);
        EXPECT_FALSE(testLsn[i].isOnRadioOn);
    }

    // radioStateChanged, radioNotAvailable
    for (size_t i = 0; i < size; i++) {
        testLsn[i].reset();
    }
    test.notifyRadioStateChanged((int)RADIO_STATE_OFF,
            (int)RADIO_STATE_UNAVAILABLE);
    for (size_t i = 0; i < size; i++) {
        EXPECT_EQ(testLsn[i].state, (int)RADIO_STATE_UNAVAILABLE);
        EXPECT_TRUE(testLsn[i].isOnRadioNotAvailable);
        EXPECT_FALSE(testLsn[i].isOnRadioOffOrNotAvailable);
        EXPECT_FALSE(testLsn[i].isOnRadioAvailable);
        EXPECT_FALSE(testLsn[i].isOnRadioOn);
    }

    // no change
    for (size_t i = 0; i < size; i++) {
        testLsn[i].reset();
    }
    test.notifyRadioStateChanged((int)RADIO_STATE_ON,
            (int)RADIO_STATE_ON);
    for (size_t i = 0; i < size; i++) {
        EXPECT_FALSE(testLsn[i].isOnRadioNotAvailable);
        EXPECT_FALSE(testLsn[i].isOnRadioOffOrNotAvailable);
        EXPECT_FALSE(testLsn[i].isOnRadioAvailable);
        EXPECT_FALSE(testLsn[i].isOnRadioOn);
    }

    // force update
    for (size_t i = 0; i < size; i++) {
        testLsn[i].reset();
    }
    test.notifyRadioStateChanged((int)RADIO_STATE_UNAVAILABLE,
            (int)RADIO_STATE_UNAVAILABLE, true);
    for (size_t i = 0; i < size; i++) {
        EXPECT_EQ(testLsn[i].state, (int)RADIO_STATE_UNAVAILABLE);
        EXPECT_FALSE(testLsn[i].isOnRadioNotAvailable);
        EXPECT_FALSE(testLsn[i].isOnRadioOffOrNotAvailable);
        EXPECT_FALSE(testLsn[i].isOnRadioAvailable);
        EXPECT_FALSE(testLsn[i].isOnRadioOn);
    }
    for (size_t i = 0; i < size; i++) {
        testLsn[i].reset();
    }
    test.notifyRadioStateChanged((int)RADIO_STATE_OFF,
            (int)RADIO_STATE_OFF, true);
    for (size_t i = 0; i < size; i++) {
        EXPECT_EQ(testLsn[i].state, (int)RADIO_STATE_OFF);
        EXPECT_FALSE(testLsn[i].isOnRadioNotAvailable);
        EXPECT_FALSE(testLsn[i].isOnRadioOffOrNotAvailable);
        EXPECT_FALSE(testLsn[i].isOnRadioAvailable);
        EXPECT_FALSE(testLsn[i].isOnRadioOn);
    }
    for (size_t i = 0; i < size; i++) {
        testLsn[i].reset();
    }
    test.notifyRadioStateChanged((int)RADIO_STATE_ON,
            (int)RADIO_STATE_ON, true);
    for (size_t i = 0; i < size; i++) {
        EXPECT_EQ(testLsn[i].state, (int)RADIO_STATE_ON);
        EXPECT_FALSE(testLsn[i].isOnRadioNotAvailable);
        EXPECT_FALSE(testLsn[i].isOnRadioOffOrNotAvailable);
        EXPECT_FALSE(testLsn[i].isOnRadioAvailable);
        EXPECT_FALSE(testLsn[i].isOnRadioOn);
    }
}

TEST(ServiceCallbackNotifier, ModemStateNotifier) {
    using ServiceCallback::ModemStateListener;
    using ServiceCallback::ModemStateNotifier;

    struct LocalListener : public ModemStateListener {
        int state;
        bool isOnModemOnline;
        bool isOnModemOffline;
        /*
         * MS_OFFLINE,
         * MS_CRASH_RESET,
         * MS_CRASH_EXIT,
         * MS_BOOTING,
         * MS_ONLINE,
         */
        LocalListener() {
            reset();
        }
        void OnModemStateChanged(int state) {
            this->state = state;
        }
        void OnModemOnline() {
            isOnModemOnline = true;
        }
        void OnModemOffline() {
            isOnModemOffline = true;
        }
        void reset() {
            state = MS_OFFLINE;
            isOnModemOnline = false;
            isOnModemOffline = false;
        }
    };

    LocalListener testLsn[2];
    size_t size = sizeof(testLsn) / sizeof(testLsn[0]);
    ModemStateNotifier test;
    for (size_t i = 0; i < size; i++) {
        test.addListener(&testLsn[i]);
    }

    int testVector[] = {
            MS_OFFLINE, MS_CRASH_RESET, MS_CRASH_EXIT, MS_BOOTING, MS_ONLINE,
    };

    for (size_t t = 0; t < sizeof(testVector) / sizeof(testVector[0]); t++) {
        for (size_t i = 0; i < size; i++) {
            test.notifyModemStateChanged(testVector[t]);
            EXPECT_EQ(testLsn[i].state, testVector[t]);
        }
    }

    for (size_t i = 0; i < size; i++) {
        testLsn[i].reset();
    }
    test.notifyModemOffline();
    for (size_t i = 0; i < size; i++) {
        EXPECT_TRUE(testLsn[i].isOnModemOffline);
    }

    for (size_t i = 0; i < size; i++) {
        testLsn[i].reset();
    }
    test.notifyModemOnline();
    for (size_t i = 0; i < size; i++) {
        EXPECT_TRUE(testLsn[i].isOnModemOnline);
    }
}

TEST(ServiceCallbackNotifier, ModemStateNotifier_all) {
    using ServiceCallback::ModemStateListener;
    using ServiceCallback::ModemStateNotifier;

    struct LocalListener : public ModemStateListener {
        int state;
        bool isOnModemOnline;
        bool isOnModemOffline;
        /*
         * MS_OFFLINE,
         * MS_CRASH_RESET,
         * MS_CRASH_EXIT,
         * MS_BOOTING,
         * MS_ONLINE,
         */
        LocalListener() {
            reset();
        }
        void OnModemStateChanged(int state) {
            this->state = state;
        }
        void OnModemOnline() {
            isOnModemOnline = true;
        }
        void OnModemOffline() {
            isOnModemOffline = true;
        }
        void reset() {
            state = MS_OFFLINE;
            isOnModemOnline = false;
            isOnModemOffline = false;
        }
    };

    LocalListener testLsn[2];
    size_t size = sizeof(testLsn) / sizeof(testLsn[0]);
    ModemStateNotifier test;
    for (size_t i = 0; i < size; i++) {
        test.addListener(&testLsn[i]);
    }

    // offline -> booting
    for (size_t i = 0; i < size; i++) {
        testLsn[i].reset();
    }
    test.notifyModemStateChanged(MS_OFFLINE, MS_BOOTING);
    for (size_t i = 0; i < size; i++) {
        EXPECT_EQ(testLsn[i].state, MS_BOOTING);
        EXPECT_FALSE(testLsn[i].isOnModemOffline);
        EXPECT_FALSE(testLsn[i].isOnModemOnline);
    }

    // offline -> online
    for (size_t i = 0; i < size; i++) {
        testLsn[i].reset();
    }
    test.notifyModemStateChanged(MS_OFFLINE, MS_ONLINE);
    for (size_t i = 0; i < size; i++) {
        EXPECT_EQ(testLsn[i].state, MS_ONLINE);
        EXPECT_FALSE(testLsn[i].isOnModemOffline);
        EXPECT_TRUE(testLsn[i].isOnModemOnline);
    }

    // booting -> online
    for (size_t i = 0; i < size; i++) {
        testLsn[i].reset();
    }
    test.notifyModemStateChanged(MS_BOOTING, MS_ONLINE);
    for (size_t i = 0; i < size; i++) {
        EXPECT_EQ(testLsn[i].state, MS_ONLINE);
        EXPECT_FALSE(testLsn[i].isOnModemOffline);
        EXPECT_TRUE(testLsn[i].isOnModemOnline);
    }

    // online -> crash exit
    for (size_t i = 0; i < size; i++) {
        testLsn[i].reset();
    }
    test.notifyModemStateChanged(MS_ONLINE, MS_CRASH_EXIT);
    for (size_t i = 0; i < size; i++) {
        EXPECT_EQ(testLsn[i].state, MS_CRASH_EXIT);
        EXPECT_TRUE(testLsn[i].isOnModemOffline);
        EXPECT_FALSE(testLsn[i].isOnModemOnline);
    }

    // online -> offline
    for (size_t i = 0; i < size; i++) {
        testLsn[i].reset();
    }
    test.notifyModemStateChanged(MS_ONLINE, MS_OFFLINE);
    for (size_t i = 0; i < size; i++) {
        EXPECT_EQ(testLsn[i].state, MS_OFFLINE);
        EXPECT_TRUE(testLsn[i].isOnModemOffline);
        EXPECT_FALSE(testLsn[i].isOnModemOnline);
    }

    // crash -> offline
    for (size_t i = 0; i < size; i++) {
        testLsn[i].reset();
    }
    test.notifyModemStateChanged(MS_CRASH_EXIT, MS_OFFLINE);
    for (size_t i = 0; i < size; i++) {
        EXPECT_EQ(testLsn[i].state, MS_OFFLINE);
        EXPECT_FALSE(testLsn[i].isOnModemOffline);
        EXPECT_FALSE(testLsn[i].isOnModemOnline);
    }
}

TEST(ServiceCallbackNotifier, NetworkStateNotifier) {
    using ServiceCallback::NetworkStateListener;
    using ServiceCallback::NetworkStateNotifier;

    struct LocalListener : public NetworkStateListener {
        ServiceState serviceState;
        int voiceRegState;
        int dataRegState;
        LocalListener() {
            reset();
        }
        void OnVoiceRegistrationStateChanged(int state) {
            voiceRegState = state;
        }
        void OnDataRegistrationStateChanged(int state) {
            dataRegState = state;
        }
        void OnServiceStateChanged(const ServiceState& serviceState) {
            this->serviceState = serviceState;
        }
        void reset() {
            serviceState.setNullState();
            voiceRegState = RIL_RegState::RIL_NOT_REG_AND_NOT_SEARCHING;
            dataRegState = RIL_RegState::RIL_NOT_REG_AND_NOT_SEARCHING;
        }
    };

    LocalListener testLsn[2];
    size_t size = sizeof(testLsn) / sizeof(testLsn[0]);
    NetworkStateNotifier test;
    for (size_t i = 0; i < size; i++) {
        test.addListener(&testLsn[i]);
    }

    int testVector[] = {
            RIL_NOT_REG_AND_NOT_SEARCHING,
            RIL_REG_HOME,
            RIL_NOT_REG_AND_SEARCHING,
            RIL_REG_DENIED,
            RIL_UNKNOWN,
            RIL_REG_ROAMING,
            RIL_NOT_REG_AND_EMERGENCY_AVAILABLE_AND_NOT_SEARCHING,
            RIL_NOT_REG_AND_EMERGENCY_AVAILABLE_AND_SEARCHING,
            RIL_REG_DENIED_AND_EMERGENCY_AVAILABLE,
            RIL_UNKNOWN_AND_EMERGENCY_AVAILABLE,
    };

    for (size_t t = 0; t < sizeof(testVector) / sizeof(testVector[0]); t++) {
        int state = testVector[t];
        test.notifyVoiceRegistrationStateChanged(state);
        test.notifyDataRegistrationStateChanged(state);

        for (size_t i = 0; i < size; i++) {
            EXPECT_EQ(testLsn[i].voiceRegState, state);
            EXPECT_EQ(testLsn[i].dataRegState, state);
            testLsn[i].reset();
        }
    }

    for (size_t i = 0; i < size; i++) {
        testLsn[i].reset();
    }
    ServiceState ss;
    ss.setDataRadioTechnology(RADIO_TECH_LTE);
    ss.setEmergencyOnly(true);
    ss.setOperatorName("testbed", "testbed");
    ss.setOperatorNumeric("00101");
    test.notifyServiceStateChanged(ss);
    for (size_t i = 0; i < size; i++) {
        EXPECT_TRUE(testLsn[i].serviceState == ss);
    }
}


TEST(ServiceCallbackNotifier, SimStatusNotifier) {
    using ServiceCallback::SimStatusListener;
    using ServiceCallback::SimStatusNotifier;

    struct LocalListener : public SimStatusListener {
        int cardState;;
        int appState;
        SimCardStatus cardStatus;
        LocalListener() {
            reset();
        }
        void OnSimStatusChanged(int cardState, int appState) {
            this->cardState = cardState;
            this->appState = appState;
        }
        void OnSimStatusChanged(const SimCardStatus& cardStatus) {
            this->cardStatus = cardStatus;
        }
        void reset() {
            cardState = RIL_CARDSTATE_ABSENT;
            appState = RIL_APPSTATE_UNKNOWN;
            cardStatus = SimCardStatus();
        }
    };

    LocalListener testLsn[2];
    size_t size = sizeof(testLsn) / sizeof(testLsn[0]);
    SimStatusNotifier test;
    for (size_t i = 0; i < size; i++) {
        test.addListener(&testLsn[i]);
    }

    {
        struct {
            int cardState;
            int appState;
        } testVector[] = {
            { RIL_CARDSTATE_ABSENT, RIL_APPSTATE_DETECTED },
            { RIL_CARDSTATE_PRESENT, RIL_APPSTATE_PIN },
            { RIL_CARDSTATE_ERROR, RIL_APPSTATE_PUK },
            { RIL_CARDSTATE_RESTRICTED, RIL_APPSTATE_SUBSCRIPTION_PERSO },
            { RIL_CARDSTATE_PRESENT, RIL_APPSTATE_READY },
        };

        for (size_t t = 0; t < sizeof(testVector) / sizeof(testVector[0]); t++) {
            test.notifySimStatusChanged(testVector[t].cardState, testVector[t].appState);

            for (size_t i = 0; i < size; i++) {
                EXPECT_EQ(testLsn[i].cardState, testVector[t].cardState);
                EXPECT_EQ(testLsn[i].appState, testVector[t].appState);
                testLsn[i].reset();
            }
        }
    }

    {
        RIL_CardStatus_V1_4 testVector = {
            .card_state = RIL_CARDSTATE_PRESENT,
            .physicalSlotId = 1,
            .atr = (char*) "3BDE9600803FC7A0FFFFE073FE215B64534304308105F2",
            .iccid = (char*) "8982051506304310894F",
            .eid =(char*) "89033023426200000330025426265873",
        };
        SimCardStatus cardStatus = SimCardStatus::create(testVector);
        test.notifySimStatusChanged(cardStatus);
        for (size_t i = 0; i < size; i++) {
            EXPECT_TRUE(testLsn[i].cardStatus == cardStatus);
            testLsn[i].reset();
        }
    }
}
