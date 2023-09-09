/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef SLSI_MXLOG_H
#define SLSI_MXLOG_H

#include <stdio.h>
#include <functional>

class MxLog {
private:
    static volatile int running_pid;
    static char prefix[256];
    static std::function<void()> removefilter;
    static void clear_resource();
    static void child_process();
public:
    static bool start(std::function<void()> addfilter,
        std::function<void()> removefilter, const char* prefixstr);
    static void stop();
    static int get_pid() {return running_pid;}
};

#endif
