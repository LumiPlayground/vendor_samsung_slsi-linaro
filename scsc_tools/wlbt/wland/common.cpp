/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#include <sys/system_properties.h>
#include <errno.h>

#include <fstream>
#include <ctime>

#define LOG_TAG "WLBTLOGDBG"
#include <utils/Log.h>

#include "common.h"
#include "filedir.h"
#include "wlbtlog.h"

MxLogStatus  Common::mxlog_status;
UdiLogStatus Common::udilog_status;

int Common::max_file_size = DEFAULT_MAX_FILE_SIZE;
int Common::max_files = DEFAULT_MAX_FILES;

void Common::SetMxLogState(MxLogStatus s) {
    mxlog_status = s;
    setPropertyValueInt(MXLOG_STATUS_PROPOERTY, mxlog_status);
}

void Common::SetUdiLogState(UdiLogStatus s) {
    udilog_status = s;
    setPropertyValueInt(UDILOG_STATUS_PROPOERTY, udilog_status);
}

void Common::SetMxLogPath(const char *path) {
    setPropertyValueString(MXLOG_FILE_PATH_PROPOERTY, path);
}

void Common::SetUdiLogPath(const char *path) {
    setPropertyValueString(UDILOG_FILE_PATH_PROPOERTY, path);
}

void Common::SetRestartReason(const char *reason) {
    std::string reason_with_timestamp;
    time_t      now = time(0);
    struct tm   tstruct = *localtime(&now);
    char buffer[32];

    strftime(buffer, sizeof(buffer), "%Y-%m-%d.%X", &tstruct);
    reason_with_timestamp = std::string(buffer) + ":" + std::string(reason);
    setPropertyValueString(RESTART_REASON_PROPOERTY, reason_with_timestamp.c_str());
}

void Common::LoadConfiguration(void) {
    char base_dir_buffer[PROP_VALUE_MAX + 1];
    char buffer[PROP_VALUE_MAX + 1];
    int mxlog_property;
    int udilog_property;

    ALOGD("LoadConfiguration");

    memset(base_dir_buffer, 0, sizeof(base_dir_buffer));
#ifndef CONFIG_SYSTEM_WLAND
    if (__system_property_get("vendor.wlbtlog.path", base_dir_buffer) == 0) {
        strcpy(base_dir_buffer, "/data/vendor/log/wlbt/");
        __system_property_set("vendor.wlbtlog.path", base_dir_buffer);
    }

    if (__system_property_get("vendor.sdcard.path.log", buffer) == 0)
        __system_property_set("vendor.sdcard.path.log", "/sdcard/bbklog/");
#else
    snprintf(base_dir_buffer, sizeof(base_dir_buffer), "%s,", "/data/log/wifi/");
#endif

    create_dir(base_dir_buffer);
    mxlog_property = getPropertyValueInt(MXLOG_STATUS_PROPOERTY);
    udilog_property = getPropertyValueInt(UDILOG_STATUS_PROPOERTY);

    ALOGE("get value from conf files mxlog_property %d udilog_property %d",
        mxlog_property, udilog_property);

    if (mxlog_property == -1 || udilog_property == -1) { // init case
        ALOGD("This is init case");
        initDefaultValue();
        return;
    } else {
        mxlog_status = (MxLogStatus) mxlog_property;
        udilog_status = (UdiLogStatus) udilog_property;
        update_max_file_size();
        update_max_files();
        ALOGD("get value from conf files mxlog_status %u udilog_status %u", mxlog_status, udilog_status);
    }

    ALOGD("Restart reason %s", getPropertyValueString(RESTART_REASON_PROPOERTY).c_str());

    switch (mxlog_status) {
    case MXLOG_WIFI_NORMAL_T:
    WlbtLog::start_mxlog(getPropertyValueString(MXLOG_FILE_PATH_PROPOERTY).c_str(), NULL);
    break;

    case MXLOG_BT_NORMAL_T:
    WlbtLog::start_bt_normal_log(getPropertyValueString(MXLOG_FILE_PATH_PROPOERTY).c_str(), NULL);
    break;

    case MXLOG_BT_AUDIO_T:
    WlbtLog::start_bt_audio_log(getPropertyValueString(MXLOG_FILE_PATH_PROPOERTY).c_str(), NULL);
    break;

    case MXLOG_BT_CUSTOM_T:
    WlbtLog::start_bt_custom_log(getPropertyValueString(MXLOG_FILE_PATH_PROPOERTY).c_str(),
        getPropertyValueString(MXLOG_FILER_PROPOERTY).c_str());
    break;

    default:
    ALOGD("Stop filemxlog");
    break;
    }

    switch (udilog_status) {
    case UDILOG_ON_T:
    WlbtLog::start_udilog(getPropertyValueString(UDILOG_FILE_PATH_PROPOERTY).c_str(), NULL);
    break;

    default:
    ALOGD("Stop udilog");
    break;
    }
}

void Common::initDefaultValue(void) {
    mxlog_status = MXLOG_OFF_T;
    udilog_status = UDILOG_OFF_T;
    max_file_size = getPropertyValueInt(MAX_FILE_SIZE_PROPOERTY);
    max_files = getPropertyValueInt(MAX_FILES_PROPOERTY);

    setPropertyValueInt(MXLOG_STATUS_PROPOERTY, MXLOG_OFF_T);
    setPropertyValueInt(UDILOG_STATUS_PROPOERTY, UDILOG_OFF_T);
    setPropertyValueString(MXLOG_FILE_PATH_PROPOERTY, "");
    setPropertyValueString(UDILOG_FILE_PATH_PROPOERTY, "");
    if (max_file_size == -1) {
        setPropertyValueInt(MAX_FILE_SIZE_PROPOERTY, DEFAULT_MAX_FILE_SIZE);
        max_file_size = DEFAULT_MAX_FILE_SIZE;
    }
    if (max_files == -1) {
        setPropertyValueInt(MAX_FILES_PROPOERTY, DEFAULT_MAX_FILES);
        max_files = DEFAULT_MAX_FILES;
    }
}

void Common::setPropertyValueInt(const char *propertyValue, int value) {
    std::string tmp = std::to_string(value);
    char const *num_char = tmp.c_str();

    if (__system_property_set(propertyValue, num_char) < 0) {
        return;
    }
}

int Common::getPropertyValueInt(const char *propertyValue) {
    char buffer[PROP_VALUE_MAX + 1];
    int value;

    if (__system_property_get(propertyValue, buffer) != 0) {
        if (sscanf(buffer, "%d", &value) != 1)
            value = -1;
    } else {
        value = -1;
    }
    return value;
}

void Common::setPropertyValueString(const char *propertyValue, const char *value) {
    std::string tmp = std::string(value);

    if (__system_property_set(propertyValue, tmp.c_str()) < 0) {
        return;
    }
}

std::string Common::getPropertyValueString(const char *propertyValue) {
    char buffer[PROP_VALUE_MAX + 1];
    std::string value;

    if (__system_property_get(propertyValue, buffer) != 0) {
        value = std::string(buffer);
    } else {
        value = "";
    }
    return value;
}

void Common::update_max_file_size(void) {
    char buffer[PROP_VALUE_MAX + 1];

    if (__system_property_get(MAX_FILE_SIZE_PROPOERTY, buffer) != 0) {
        if (sscanf(buffer, "%u", &Common::max_file_size) != 1)
            Common::max_file_size = DEFAULT_MAX_FILE_SIZE;
        ALOGD("update_max_file_size / getproperty : %u ", max_file_size);
    }
}

void Common::update_max_files(void) {
    char buffer[PROP_VALUE_MAX + 1];

    if (__system_property_get(MAX_FILES_PROPOERTY, buffer) != 0) {
        if (sscanf(buffer, "%u", &Common::max_files) != 1)
            Common::max_files = DEFAULT_MAX_FILES;
        ALOGD("update_max_files / getproperty : %u ", max_files);
    }
}

void Common::get_time_string(char *buffer, unsigned int size) {
    time_t rawtime;
    struct tm *timeinfo;

    memset(buffer, 0, size);
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, size, "Log_%m%d%H%M%S", timeinfo);
}

void Common::send_kill_signal(int pid) {
    if (pid <= 0) {
        return;
    }

    int rc = kill(pid, SIGKILL);
    if (rc < 0) {
        ALOGD("kill failed errno : %d", errno);
    }
}

bool Common::is_valid_pid(const int pid, const char *valid_cmd) {
    const std::string path = "/proc/" + std::to_string(pid) + "/cmdline";
    std::ifstream fileinput(path);
    std::string cmd;
    bool valid = false;

    if (!fileinput.is_open())
        return valid;

    if (std::getline(fileinput, cmd))
        if (cmd.find(std::string(valid_cmd)) != std::string::npos)
            valid = true;
    if (valid)
        ALOGD("PID : %d is valid %s", pid, cmd.c_str());
    else
        ALOGD("PID : %d is invalid %s", pid, cmd.c_str());
    return valid;
}
