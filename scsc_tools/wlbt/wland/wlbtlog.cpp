/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#include <unistd.h>
#include <array>
#include <fstream>
#include <string>
#include "filedir.h"

#define LOG_TAG "WLBTLOGDBG"
#include <utils/Log.h>

#include "common.h"
#include "mxlog.h"
#include "udilog.h"
#include "hip4_profile.h"
#include "wlbtlog.h"

#define UNUSED(p)   ((void)(p))

const std::array<int, 3> logging_cpu_affinity = {1, 2, 3};

bool WlbtLog::bt_logging_running = false;
bool WlbtLog::mxlog_running = false;
bool WlbtLog::udilog_running = false;

void WlbtLog::set_cpu_affinity() {
    cpu_set_t  mask;
    int err;

    CPU_ZERO(&mask);
    for (int i = 0 ; i < logging_cpu_affinity.size() ; i++)
        CPU_SET(logging_cpu_affinity[i], &mask);
    err = sched_setaffinity(UdiLog::get_pid(), sizeof(mask), &mask);
    if (!err)
        ALOGD("cpu affinity is set");
    else
        ALOGD("cpu affinity setup is failed");
}

int WlbtLog::set_log_level(const char* file, const char* value) {
    const std::string path(file);
    std::ofstream fileoutput(path);

    if (!fileoutput.is_open())
        return -1;
    fileoutput << value;
    return 0;
}

void WlbtLog::setup_wifi_log_filter() {
    // With HOST-14007, we do not need to filter Rx logs.
    set_log_level("/sys/module/scsc_logring/parameters/scsc_droplevel_tx", "0");
    set_log_level("/sys/module/scsc_logring/parameters/scsc_droplevel_bin_wifi_data_tx", "0");
    set_log_level("/sys/module/scsc_logring/parameters/scsc_droplevel_bin_wifi_ctrl_tx", "0");
    set_log_level("/sys/module/scsc_logring/parameters/scsc_droplevel_bin_wifi_ctrl_rx", "0");
}

void WlbtLog::revert_wifi_log_filter() {
    // With HOST-14007, we do not need to filter Rx logs.
    set_log_level("/sys/module/scsc_logring/parameters/scsc_droplevel_tx", "11");
    set_log_level("/sys/module/scsc_logring/parameters/scsc_droplevel_bin_wifi_data_tx", "11");
    set_log_level("/sys/module/scsc_logring/parameters/scsc_droplevel_bin_wifi_ctrl_tx", "11");
    set_log_level("/sys/module/scsc_logring/parameters/scsc_droplevel_bin_wifi_ctrl_rx", "11");
}

void WlbtLog::setup_bt_normal_log_filter() {
    set_log_level("/sys/module/scsc_bt/parameters/mxlog_filter", "0x469BF437");
}

void WlbtLog::setup_bt_audio_log_filter() {
    set_log_level("/sys/module/scsc_bt/parameters/mxlog_filter", "0x469BF437");
}

void WlbtLog::setup_bt_custom_log_filter(const char* filter) {
    set_log_level("/sys/module/scsc_bt/parameters/mxlog_filter", filter);
    Common::setPropertyValueString(MXLOG_FILER_PROPOERTY, filter);
}

void WlbtLog::revert_bt_log_filter() {
    set_log_level("/sys/module/scsc_bt/parameters/mxlog_filter", "0x00000000");
}

bool WlbtLog::start_mxlog(const char* prefix, const char* data) {
    UNUSED(data);

    if (mxlog_running) {
        stop_mxlog(NULL, NULL);
    }
    if (bt_logging_running) {
        stop_bt_log(NULL, NULL);
    }

    mxlog_running = MxLog::start(setup_wifi_log_filter, revert_wifi_log_filter, prefix);
    if (mxlog_running) {
        Common::SetMxLogState(MXLOG_WIFI_NORMAL_T);
        Common::SetMxLogPath(prefix);
        set_cpu_affinity();
    }
    return mxlog_running;
}

bool WlbtLog::start_udilog(const char* prefix, const char* data) {
    UNUSED(data);

    if (udilog_running) {
        stop_udilog(NULL, NULL);
    }

    udilog_running = UdiLog::start(NULL, NULL, prefix);
    if (udilog_running) {
        Common::SetUdiLogState(UDILOG_ON_T);
        Common::SetUdiLogPath(prefix);
        set_cpu_affinity();
    }
    return udilog_running;
}

bool WlbtLog::start_hip4_profile(const char* output_file, const char* data) {
    UNUSED(data);

    if (Hip4Profile::start("/dev/hip4_0_sam_off", output_file, DEV_TYPE) == 0) {
        return true;
    }
    return false;
}

bool WlbtLog::start_bt_normal_log(const char* prefix, const char* data) {
    UNUSED(data);

    if (bt_logging_running) {
        stop_bt_log(NULL, NULL);
    }
    if (mxlog_running) {
        stop_mxlog(NULL, NULL);
    }

    bt_logging_running = MxLog::start(setup_bt_normal_log_filter,
        revert_bt_log_filter, prefix);
    if (bt_logging_running) {
        Common::SetMxLogState(MXLOG_BT_NORMAL_T);
        Common::SetMxLogPath(prefix);
    }
    return bt_logging_running;
}

bool WlbtLog::start_bt_audio_log(const char* prefix, const char* data) {
    UNUSED(data);

    if (bt_logging_running) {
        stop_bt_log(NULL, NULL);
    }
    if (mxlog_running) {
        stop_mxlog(NULL, NULL);
    }

    bt_logging_running = MxLog::start(setup_bt_audio_log_filter,
        revert_bt_log_filter, prefix);
    if (bt_logging_running) {
        Common::SetMxLogState(MXLOG_BT_AUDIO_T);
        Common::SetMxLogPath(prefix);
    }
    return bt_logging_running;
}

bool WlbtLog::start_bt_custom_log(const char* prefix, const char* data) {
    if (bt_logging_running) {
        stop_bt_log(NULL, NULL);
    }
    if (mxlog_running) {
        stop_mxlog(NULL, NULL);
    }

    setup_bt_custom_log_filter(data);
    bt_logging_running = MxLog::start(NULL, revert_bt_log_filter, prefix);
    if (bt_logging_running) {
        Common::SetMxLogState(MXLOG_BT_CUSTOM_T);
        Common::SetMxLogPath(prefix);
    }
    return bt_logging_running;
}

bool WlbtLog::stop_mxlog(const char* prefix, const char* data) {
    UNUSED(prefix);
    UNUSED(data);

    if (mxlog_running) {
        mxlog_running = false;
        MxLog::stop();
        Common::SetMxLogPath("");
        Common::SetMxLogState(MXLOG_OFF_T);
    }
    return true;
}

bool WlbtLog::stop_udilog(const char* prefix, const char* data) {
    UNUSED(prefix);
    UNUSED(data);

    if (udilog_running) {
        udilog_running = false;
        UdiLog::stop();
        Common::SetUdiLogPath("");
        Common::SetUdiLogState(UDILOG_OFF_T);
    }
    return true;
}

bool WlbtLog::stop_bt_log(const char* prefix, const char* data) {
    UNUSED(prefix);
    UNUSED(data);

    if (bt_logging_running) {
        bt_logging_running = false;
        MxLog::stop();
        Common::SetMxLogPath("");
        Common::SetMxLogState(MXLOG_OFF_T);
    }
    return true;
}
