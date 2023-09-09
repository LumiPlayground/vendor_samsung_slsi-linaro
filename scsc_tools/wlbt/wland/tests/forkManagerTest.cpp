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
#include <iostream>
#include <chrono>
#include <sys/wait.h>
#include <unistd.h>

#include "../forkManager.h"

class CallInfo {
 public:
    int parent_call;
    int child_call;
    int sig_call;
};

TEST(ForkManager, InvalidForkDataNULL) {
    ForkData *fork_data = NULL;
    CallInfo info;

    info.parent_call = 0;
    info.child_call = 0;
    info.sig_call = 0;

    EXPECT_EQ(-1, ForkManager::fork(fork_data));
    EXPECT_EQ(0, info.parent_call);
    EXPECT_EQ(0, info.child_call);
    EXPECT_EQ(0, info.sig_call);
}

TEST(ForkManager, InvalidForkDataChildWOSighandler) {
    ForkData fork_data;
    CallInfo info;

    info.parent_call = 0;
    info.child_call = 0;
    info.sig_call = 0;

    fork_data.parent = NULL;
    fork_data.child = [&info](){
        info.child_call = 1;
    };
    fork_data.sig_handler = NULL;

    EXPECT_EQ(-1, ForkManager::fork(&fork_data));
    EXPECT_EQ(0, info.parent_call);
    EXPECT_EQ(0, info.child_call);
    EXPECT_EQ(0, info.sig_call);
}

TEST(ForkManager, InvalidForkDataSighandlerWOChild) {
    ForkData fork_data;
    CallInfo info;

    info.parent_call = 0;
    info.child_call = 0;
    info.sig_call = 0;

    fork_data.parent = [&info](){
        info.parent_call = 1;
    };
    fork_data.child = NULL;
    fork_data.sig_handler = [&info](){
        info.sig_call = 1;
    };
    EXPECT_EQ(-1, ForkManager::fork(&fork_data));
    EXPECT_EQ(0, info.parent_call);
    EXPECT_EQ(0, info.child_call);
    EXPECT_EQ(0, info.sig_call);
}

TEST(ForkManager, ValidForkDataParentOnly) {
    ForkData fork_data;
    CallInfo info;
    int pid;

    info.parent_call = 0;
    info.child_call = 0;
    info.sig_call = 0;

    fork_data.parent = [&info](){
        info.parent_call = 1;
    };
    fork_data.child = NULL;
    fork_data.sig_handler = NULL;
    pid = ForkManager::fork(&fork_data);
    if (pid > 0) {
        EXPECT_EQ(1, info.parent_call);
    } else {
        EXPECT_EQ(0, info.parent_call);
    }
    EXPECT_TRUE(-1 == pid);
}

TEST(ForkManager, ValidForkDataChildSighandler) {
    ForkData fork_data;
    CallInfo info;
    int pid;
    int pipefd[2];
    std::timed_mutex mutex;

    info.parent_call = 0;
    info.child_call = 0;
    info.sig_call = 0;

    ForkManager::start_signal_handler();

    fork_data.parent = NULL;
    fork_data.child = [&info]() {
        info.child_call = 1;
    };
    fork_data.sig_handler = [&info, &pipefd, &mutex](){
        char buffer;

        buffer = '0';
        write(pipefd[1], &buffer, sizeof(buffer));
        read(pipefd[0], &buffer, sizeof(buffer));
        info.sig_call = 1;
        mutex.unlock();
    };
    pipe(pipefd);
    mutex.lock();
    pid = ForkManager::fork(&fork_data);
    if (pid > 0) {
        char buffer;
        int status;

        read(pipefd[0], &buffer, sizeof(buffer));
        buffer++;
        write(pipefd[1], &buffer, sizeof(buffer));
        waitpid(pid, &status, 0);
        mutex.try_lock_for(std::chrono::milliseconds(500));
        EXPECT_EQ(0, info.parent_call);
        EXPECT_EQ(0, info.child_call);
        EXPECT_EQ(1, info.sig_call);
    } else {
        EXPECT_EQ(0, info.parent_call);
        EXPECT_EQ(1, info.child_call);
        EXPECT_EQ(0, info.sig_call);
    }
    EXPECT_TRUE(-1 != pid);
}

TEST(ForkManager, ValidForkDataFull) {
    ForkData fork_data;
    CallInfo info;
    int pid;
    int pipefd[2];
    std::timed_mutex mutex;

    info.parent_call = 0;
    info.child_call = 0;
    info.sig_call = 0;

    ForkManager::start_signal_handler();

    fork_data.parent = [&info](){
        info.parent_call = 1;
    };
    fork_data.child = [&info, &pipefd](){
        char buffer;

        buffer = '0';
        write(pipefd[1], &buffer, sizeof(buffer));
        read(pipefd[0], &buffer, sizeof(buffer));
        info.child_call = 1;
    };
    fork_data.sig_handler = [&info, &mutex](){
        info.sig_call = 1;
        mutex.unlock();
    };
    pipe(pipefd);
    mutex.lock();
    pid = ForkManager::fork(&fork_data);
    if (pid > 0) {
        char buffer;
        int status;

        read(pipefd[0], &buffer, sizeof(buffer));
        buffer++;
        write(pipefd[1], &buffer, sizeof(buffer));
        waitpid(pid, &status, 0);
        mutex.try_lock_for(std::chrono::milliseconds(500));
        EXPECT_EQ(1, info.parent_call);
        EXPECT_EQ(0, info.child_call);
        EXPECT_EQ(1, info.sig_call);
    } else {
        EXPECT_EQ(0, info.parent_call);
        EXPECT_EQ(1, info.child_call);
        EXPECT_EQ(0, info.sig_call);
    }
    EXPECT_TRUE(-1 != pid);
}
