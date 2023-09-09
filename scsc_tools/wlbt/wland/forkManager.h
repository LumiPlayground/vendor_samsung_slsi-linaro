/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef SLSI_FORKMANAGER_H
#define SLSI_FORKMANAGER_H
#include <mutex>
#include <map>

struct ForkData {
    std::function<void(void)> parent;
    std::function<void(void)> child;
    std::function<void(void)> sig_handler;
};

class ForkManager {
private:
    static std::mutex sig_handler_lock;
    static std::map<int, std::function<void(void)>> sig_handler_list;
    static void set_sig_handler_with_lock(int pid, std::function<void(void)> sig_handler);
    static void sig_handler(int sig, siginfo_t *siginfo, void *context);
public:
    static int fork(struct ForkData *fork_data);
    static void start_signal_handler();
};
#endif
