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
#include "testutils.h"
#include "thread.h"

TEST(Thread, Thread_default) {
    class TestImpl : public Thread {
    public:
        bool run = false;
        TestImpl()=default;
        void Run() { run = true; }
        bool isStarted() { return m_bStarted; }
    } test;

    EXPECT_FALSE(test.isStarted());
    EXPECT_FALSE(test.run);
    test.Start();
    EXPECT_TRUE(test.isStarted());
    test.Stop();
    EXPECT_TRUE(test.run);
}

TEST(Thread, Runnable_default) {
    class TestImpl : public Runnable {
    public:
        bool run = false;
        void Run() { run = true; }
    } test;
    Thread t(&test);
    EXPECT_FALSE(test.run);
    t.Start();
    t.Stop();
    EXPECT_TRUE(test.run);
}

TEST(Thread, Thread_join_by_destructor) {
    bool ret = false;
    {
        class TestImpl : public Thread {
        public:
            bool& result;
            TestImpl(bool& ret) : result(ret) {}
            void Run() { result = true; }
        } test(ret);
        test.Start();
        usleep(100000);
        // TC ends without explicit calling Stop()
        // join by destructor of Thread
    }
    EXPECT_TRUE(ret);
}

TEST(Thread, Thread_detach) {
    class TestImpl : public Thread {
    public:
        bool run = false;
        TestImpl()=default;
        void Run() {
            run = true;
        }
        bool isStarted() { return m_bStarted; }
    } test;

    EXPECT_FALSE(test.isStarted());
    EXPECT_FALSE(test.run);
    test.Start();
    EXPECT_TRUE(test.isStarted());
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    test.Detach();
    // Thread can be terminated without join
}

TEST(Thread, instant_example1) {
    int sum = 0;
    int value = 10;
    // std::thread t = Thread::Execute([&sum](int size) {
    //     for (int i = 0; i < size; i++) sum += i;
    // }, value);
    std::thread t = Thread::Execute([&sum]() {
        for (int i = 0; i < 10; i++) sum += i;
    });
    t.join();
    int ret = 0;
    for (int i = 0; i < value; i++) ret += i;
    EXPECT_EQ(sum, ret);
}

TEST(Thread, instant_example2) {
    int value = 10;
    class TestImpl : public Runnable {
    public:
        int mSum = 0;
        int mSize = 0;
        TestImpl(int size) : mSize(size) {}
        void Run() {
            for (int i = 0; i < mSize; i++) mSum += i;
        }
    } r(value);

    std::thread t = Thread::Execute([&r]() { r.Run(); });
    t.join();
    int ret = 0;
    for (int i = 0; i < value; i++) ret += i;
    EXPECT_EQ(r.mSum, ret);
}

TEST(Thread, instant_example3) {
    int value = 10;
    struct TestImpl {
    public:
        int mSum = 0;
        int mSize = 0;
        TestImpl(int size) : mSize(size) {}
        void operator()() {
            for (int i = 0; i < mSize; i++) mSum += i;
        }
    } r(value);
    auto test = std::ref(r);

    std::thread t = Thread::Execute(test);
    t.join();
    int ret = 0;
    for (int i = 0; i < value; i++) ret += i;
    EXPECT_EQ(r.mSum, ret);
}

TEST(Thread, instant_example4) {
    int value = 10;
    struct TestImpl {
    public:
        int mSum = 0;
        void operator()(int size) {
            for (int i = 0; i < size; i++) mSum += i;
        }
    } r;
    auto test = std::ref(r);

    std::thread t = Thread::Execute(test, value);
    t.join();
    int ret = 0;
    for (int i = 0; i < value; i++) ret += i;
    EXPECT_EQ(r.mSum, ret);
}

TEST(Thread, instant_example5) {
    int value = 10;
    struct TestImpl {
    public:
        int mSize = 0;
        TestImpl(int size) : mSize(size) {}
        void operator()(int& result) {
            for (int i = 0; i < mSize; i++) result += i;
        }
    } r(value);
    int sum = 0;
    std::thread t = Thread::Execute(r, std::ref(sum));
    t.join();
    int ret = 0;
    for (int i = 0; i < value; i++) ret += i;
    EXPECT_EQ(sum, ret);
}