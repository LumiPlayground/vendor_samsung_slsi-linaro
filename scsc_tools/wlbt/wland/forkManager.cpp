/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#define LOG_TAG "WLBTLOGDBG"
#include <utils/Log.h>
#include <unistd.h>
#include <sys/wait.h>
#include "forkManager.h"

#define UNUSED(p)   ((void)(p))

std::mutex ForkManager::sig_handler_lock;
std::map<int, std::function<void(void)>> ForkManager::sig_handler_list;

void ForkManager::set_sig_handler_with_lock(int pid, std::function<void(void)> sig_handler) {
    if (sig_handler) {
        std::scoped_lock lock(sig_handler_lock);

        sig_handler_list[pid] = sig_handler;
    }
}

void ForkManager::sig_handler(int sig, siginfo_t *siginfo, void *context) {
    int pid = siginfo->si_pid;
    UNUSED(context);

    {
        std::scoped_lock lock(sig_handler_lock);

        auto handler = sig_handler_list.find(pid);
        if (handler != sig_handler_list.end()) {
            ALOGD("Signal[%d] has been raised for %d\n", sig, pid);
            handler->second();
            sig_handler_list.erase(handler);
        }
    }
}

void ForkManager::start_signal_handler() {
    struct sigaction sact;
    sact.sa_flags = SA_SIGINFO | SA_NOCLDWAIT;
    sigemptyset(&sact.sa_mask);

    // signal handler registration
    sact.sa_sigaction = sig_handler;
    sigaction(SIGCHLD, &sact, NULL);
}

int ForkManager::fork(struct ForkData *fork_data) {
    int pid;

    if (!fork_data)
        return -1;

    if (!fork_data->child)
        return -1;

    if (fork_data->child && !fork_data->sig_handler)
        return -1;

    if (!fork_data->child && fork_data->sig_handler)
        return -1;

    pid = ::fork();
    if (pid < -1) {
        return pid;
    }
    if (pid > 0) {
        set_sig_handler_with_lock(pid, fork_data->sig_handler);
        if (fork_data->parent)
            fork_data->parent();
    } else {
        if (fork_data->child)
            fork_data->child();
    }
    return pid;
}
