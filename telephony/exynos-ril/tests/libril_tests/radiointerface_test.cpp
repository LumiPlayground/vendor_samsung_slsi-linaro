/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd All Rights Reserved
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
#include <testutils.h>
#include <ril_service_1_6.h>
#include <basecommand.h>
#include <radiointerface.h>
#include <radioserviceconnection.h>
#include <radioproxy.h>

#include <string>
#include <vector>

TEST(RadioInterface, RadioInterface) {
    uint32_t slot = 0;
    RadioInterface test(slot, RadioProxy::make(slot, nullptr, nullptr));
    EXPECT_FALSE(test.hidl());

    auto hidlRadio = radio::impl::v1_6::make(slot);
    test.hidl(hidlRadio);
    EXPECT_TRUE(test.hidl() == hidlRadio);
}

TEST(RadioInterface, RadioInterface_Command) {
    uint32_t slot = 0;
    RadioInterface test(slot, RadioProxy::make(slot, nullptr, nullptr));

    struct TestArgs {
        int32_t serial;
        int32_t i;
        std::string str;
        std::vector<int32_t> vec;
    };

    struct TestCommand : public BaseCommand {
        TestArgs mArgs;
        TestChecker checker;
        TestCommand(uint32_t slot) : BaseCommand(slot), mArgs{} {}
        void onRequest(int32_t serial, int32_t i, const std::string& str,
                        const std::vector<int32_t>& vec) {
            checker.checked();
            mArgs.serial = serial;
            mArgs.i = i;
            mArgs.str = str;
            mArgs.vec = vec;
        }
        int onResponse(int /* slotId */, int /* responseType */, int /* serial */, RIL_Errno /* e */,
                    void */* response */, size_t /* responselen */) {
            return 0;
        }
        void testOnRequest(const TestArgs& args) {
            EXPECT_TRUE(mArgs.serial == args.serial);
            EXPECT_TRUE(mArgs.i == args.i);
            EXPECT_TRUE(mArgs.str == args.str);
            EXPECT_TRUE(mArgs.vec == args.vec);
        }
    };

    constexpr int32_t request = 90000;
    auto testCommand = std::make_shared<TestCommand>(slot);
    test.updateCommand(request, testCommand);
    EXPECT_TRUE(test.getCommand<TestCommand>(request) &&
                test.getCommand<TestCommand>(request) == testCommand);

    TestArgs args = { 9876, 1, "test_str", {1, 2, 3, 4, 5}};
    test.onRequest(testCommand, args.serial, args.i, args.str, args.vec);
    testCommand->testOnRequest(args);
}

TEST(RadioInterface, RadioInterface_Indication) {
    uint32_t slot = 0;
    RadioInterface test(slot, RadioProxy::make(slot, nullptr, nullptr));

    struct TestIndication : public BaseIndication {
        TestIndication(uint32_t slot) : BaseIndication(slot) {}
        int onIndication(int /* slotId */, int /* indicationType */, int /* token */, RIL_Errno /* e */,
                            void */* response */, size_t /* responselen */) {
            return 0;
        }
    };
    constexpr int32_t indication = 100000;
    auto testIndication = std::make_shared<TestIndication>(slot);
    test.updateIndication(indication, testIndication);
    EXPECT_TRUE(testIndication->mIndicationId == indication);
}

TEST(RadioInterface, RadioServiceConnectionListener) {
    uint32_t slot = 0;
    RadioInterface test(slot, RadioProxy::make(slot, nullptr, nullptr));

    struct ListenerImpl : public RadioServiceConnectionListener {
        TestChecker connected;
        TestChecker disconnected;
        void onRadioServiceConnected() {
            connected.checked();
        }
        void onRadioServiceDionnected() {
            disconnected.checked();
        }
    };

    std::vector<ListenerImpl> testListener(10);
    for (size_t i = 0; i < testListener.size(); i++) {
        test.registerForRadioServiceConnectionListener(&testListener[i]);
        // try twice
        test.registerForRadioServiceConnectionListener(&testListener[i]);
    }

    test.updateRadioServiceConnectionState(true);
    test.updateRadioServiceConnectionState(false);

    for (size_t i = 0; i < testListener.size(); i++) {
        test.unregisterForRadioServiceConnectionListener(&testListener[i]);
        // try twice
        test.unregisterForRadioServiceConnectionListener(&testListener[i]);
    }
}