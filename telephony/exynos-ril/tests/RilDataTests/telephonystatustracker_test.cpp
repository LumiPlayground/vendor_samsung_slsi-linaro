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
/*
 * telephonystatustracker_test.cpp
 *
 *  Created on: 2021. 10. 19.
 */
#include <testutils.h>
#include "base/telephonystatustracker.h"
#include <string>
#include "rillog.h"

TEST(MonitoringItemList, MonitoringItemList_value) {
    CREATE_OSAL_LOG_INSTANCE;
    CRilLog::InitRilLog();
    MonitoringItemList list;
    struct LocalCallback : public MonitoringCallback {
        std::string str;
        LocalCallback(const std::string& str) {
            this->str = str;
        }
        void dump(std::stringstream& ss) {
            ss << str;
        }
    };

    LocalCallback testLsn1("test_monitoring_callback_1");
    LocalCallback testLsn2("test_monitoring_callback_2");
    LocalCallback testLsn3("test_monitoring_callback_3");

    list.add({&testLsn1, true});
    list.add({&testLsn2, false});
    list.add({&testLsn3, true});

    EXPECT_TRUE(list.find(&testLsn1));
    EXPECT_TRUE(list.find(&testLsn2));
    EXPECT_TRUE(list.find(&testLsn3));

    EXPECT_EQ(list.mItems.size(), (unsigned long)3);

    list.clear();
    EXPECT_EQ(list.mItems.size(), (unsigned long)0);
    EXPECT_FALSE(list.find(&testLsn1));
    EXPECT_FALSE(list.find(&testLsn2));
    EXPECT_FALSE(list.find(&testLsn3));
}

TEST(TelephonyStatusTracker, TelephonyStatusTracker_value) {

    struct LocalCallback : public MonitoringCallback {
        std::string str;
        bool called = false;
        LocalCallback(const std::string& str) {
            this->str = str;
        }
        void dump(std::stringstream& ss) {
            ss << str;
            called = true;
        }
    };

    LocalCallback testLsn1("test_monitoring_callback_1");
    LocalCallback testLsn2("test_monitoring_callback_2");
    LocalCallback testLsn3("test_monitoring_callback_3");

    TelephonyStatusTracker &tracker = TelephonyStatusTracker::createForPhoneId(0);
    tracker.registerMonitoringCallback(&testLsn1, true);
    tracker.registerMonitoringCallback(&testLsn2, false);
    tracker.registerMonitoringCallback(&testLsn3, true);
    tracker.dump(true);
    EXPECT_TRUE(testLsn1.called);
    EXPECT_TRUE(testLsn2.called);
    EXPECT_TRUE(testLsn3.called);
}
