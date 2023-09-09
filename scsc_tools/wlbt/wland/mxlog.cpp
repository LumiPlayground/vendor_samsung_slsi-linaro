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
#include <fstream>
#define LOG_TAG "WLBTLOGDBG"
#include <utils/Log.h>

#include "forkManager.h"
#include "common.h"
#include "filedir.h"
#include "mxlog.h"

int volatile MxLog::running_pid = -1;
char         MxLog::prefix[256];
std::function<void()>                           MxLog::removefilter = NULL;

void MxLog::clear_resource() {
    ALOGD("Clear resource MX");
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
void MxLog::child_process() {
    std::ifstream input;
    std::ofstream output;
    int logfilesize = 0;
    int logid = 0;
    bool iswifi;

    input.open("/dev/samsg", std::ifstream::in);
    if (!input.is_open())
        input.open("/sys/kernel/debug/scsc/ring0/samsg", std::ifstream::in);
    if (!input.is_open())
        return;

    if (strstr(prefix, "bt_general") || strstr(prefix, "bt_audio") || strstr(prefix, "bt_custom"))
        iswifi = false;
    else
        iswifi = true;

    while (true) {
        std::string log_line;

        if (logfilesize == 0) {
            FILE* file;
            std::string output_file_name;

            output_file_name = std::string(prefix) + "_" + std::to_string(logid) + ".log";
            ALOGD("Create file %s", output_file_name.c_str());
            if (create_dir(output_file_name.c_str()) < 0) {
                return;
            }
            if (output.is_open())
                output.close();
            /**
             * Set file permission using platform specific file operation.
             */
            file = create_file(output_file_name.c_str());
            if (file)
                fclose(file);
            else
                return;

            output.open(output_file_name, std::ofstream::out);
            if (!output.is_open())
                return;
            output_file_name = std::string(prefix) + "_" + std::to_string((logid++ - Common::max_files)) + ".log";
            ALOGD("Delete file %s", output_file_name.c_str());
            remove(output_file_name.c_str());
            logfilesize++;
        }

        if (std::getline(input, log_line)) {
            time_t      now = time(0);
            struct tm   tstruct;
            size_t      timestamp_length;
            char time_buffer[20] = {0}; // YYYY-MM-DD.HH:mm:SS

            if (!iswifi) {
                if (!strstr(log_line.c_str(), "bt_") &&
                    !strstr(log_line.c_str(), "mx_fw")) {
                    continue;
                }
            }

            tstruct = *localtime(&now);
            timestamp_length = strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d.%X", &tstruct);
            time_buffer[timestamp_length] = ' ';
            output.write(time_buffer, sizeof(time_buffer));
            output.write(log_line.c_str(), log_line.size());
            output.write("\n", 1);

            logfilesize += sizeof(time_buffer) + log_line.size() + 1;

            if (logfilesize > Common::max_file_size * (1024 * 1024)) {
                logfilesize = 0;
            }
        }
    }
}

bool MxLog::start(std::function<void()> addfilter, std::function<void()> rmfilter, const char* prefixstr) {
    ForkData fork_data;

    if (running_pid > 0) {
        return true;
    }
    Common::update_max_file_size();
    Common::update_max_files();
    memset(prefix, 0, sizeof(prefix));
    if (prefixstr) {
        /* [Static Analysis] CID:249786 */
        strncpy(prefix, prefixstr, sizeof(prefix)-1);
    }
    if (addfilter) {
        addfilter();
    }
    if (rmfilter) {
        removefilter = rmfilter;
    }
    fork_data.parent = NULL;
    fork_data.child = child_process;
    fork_data.sig_handler = clear_resource;
    running_pid = ForkManager::fork(&fork_data);
    return running_pid != -1;
}

void MxLog::stop() {
    if (!Common::is_valid_pid(running_pid, "wland")) {
        const std::string reason = "Inavlid PID: " + std::to_string(running_pid) + " for mxlog";

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
    ALOGD("Stop MX");
}
