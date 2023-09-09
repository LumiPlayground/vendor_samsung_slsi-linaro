/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#include <string.h>
#include <unistd.h>

#include <string>
#define LOG_TAG "WLBTLOGDBG"
#include <utils/Log.h>

#include "forkManager.h"
#include "common.h"
#include "filedir.h"
#include "udilog.h"

int volatile UdiLog::running_pid = -1;
char         UdiLog::prefix[256];
std::function<void()>                           UdiLog::removefilter = NULL;

#ifdef CONFIG_SYSTEM_WLAND
#define UDI_CMD "/system/bin/slsi_wlan_udi_log_sys"
#define UDI_TOOL "slsi_wlan_udi_log_sys"
#else
#define UDI_CMD "/vendor/bin/slsi_wlan_udi_log"
#define UDI_TOOL "slsi_wlan_udi_log"
#endif

void UdiLog::clear_resource() {
    ALOGD("Clear resource UDI");
    if (removefilter) {
        removefilter();
    }
    memset(prefix, 0, sizeof(prefix));
    running_pid = -1;
}

/**
 * We do not need to care of resources in child_process(), e.g., file handle, memory, etc.
 * They will be re-claimed on kill.
 */
void UdiLog::child_process() {
    execl(UDI_CMD, UDI_CMD,
                "--file-size-limit", std::to_string(Common::max_file_size * 1024).c_str(),
                "--file-number-limit", std::to_string(Common::max_files).c_str(),
                "--madata-size-limit", "200",
                prefix,
            (char*) NULL);
}

bool UdiLog::start(std::function<void()> addfilter, std::function<void()> rmfilter, const char* prefixstr) {
    ForkData fork_data;

    if (running_pid > 0) {
        return true;
    }
    Common::update_max_file_size();
    Common::update_max_files();
    memset(prefix, 0, sizeof(prefix));
    if (prefixstr) {
        /* [Static Analysis] CID:249797 */
        snprintf(prefix, sizeof(prefix)-1, "%s.log", prefixstr);
    }

    if (addfilter) {
        addfilter();
    }
    if (rmfilter) {
        removefilter = rmfilter;
    }

    ALOGD("Start UDI, file %s", prefix);
    if (create_dir(prefix) < 0) {
        clear_resource();
        return false;
    }
    fork_data.parent = NULL;
    fork_data.child = child_process;
    fork_data.sig_handler = clear_resource;
    running_pid = ForkManager::fork(&fork_data);
    return (running_pid != -1);
}

void UdiLog::stop() {
    if (!Common::is_valid_pid(running_pid, UDI_TOOL)) {
        const std::string reason = "Inavlid PID: " + std::to_string(running_pid) + " for udilog";

        ALOGD("Restart wland.");
        Common::SetRestartReason(reason.c_str());
        /**
         * wland restarts while stopping. We should not start logging.
         */
        Common::SetMxLogState(MXLOG_OFF_T);
        Common::SetMxLogPath("");
        Common::SetUdiLogState(UDILOG_OFF_T);
        Common::SetUdiLogPath("");
        exit(-1);
    }
    Common::send_kill_signal(running_pid);
    while (running_pid > 0) {}
    ALOGD("Stop UDI");
}
