/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef SLSI_COMMON_H
#define SLSI_COMMON_H

#include <string>

enum MxLogStatus {
    MXLOG_OFF_T,
    MXLOG_WIFI_NORMAL_T,
    MXLOG_BT_NORMAL_T,
    MXLOG_BT_AUDIO_T,
    MXLOG_BT_CUSTOM_T
};

enum UdiLogStatus {
    UDILOG_OFF_T,
    UDILOG_ON_T
};

#define DEFAULT_MAX_FILE_SIZE (50) // 50MB
#define DEFAULT_MAX_FILES (5)  // 5

#ifdef CONFIG_SYSTEM_WLAND
#define MAX_FILE_SIZE_PROPOERTY "persist.wlbtlog.maxfilesize"
#define MAX_FILES_PROPOERTY "persist.wlbtlog.maxfiles"
#define MXLOG_STATUS_PROPOERTY "persist.wlbtlog.mxlog"
#define MXLOG_FILER_PROPOERTY "persist.wlbtlog.mxlogfiler"
#define UDILOG_STATUS_PROPOERTY "persist.wlbtlog.udilog"
#define MXLOG_FILE_PATH_PROPOERTY "persist.wlbtlog.mxfilepath"
#define UDILOG_FILE_PATH_PROPOERTY "persist.wlbtlog.udifilepath"
#define RESTART_REASON_PROPOERTY "persist.wlbtlog.restart_reason"
#else
#define MAX_FILE_SIZE_PROPOERTY "persist.vendor.wlbtlog.maxfilesize"
#define MAX_FILES_PROPOERTY "persist.vendor.wlbtlog.maxfiles"
#define MXLOG_STATUS_PROPOERTY "persist.vendor.wlbtlog.mxlog"
#define MXLOG_FILER_PROPOERTY "persist.vendor.wlbtlog.mxlogfilter"
#define UDILOG_STATUS_PROPOERTY "persist.vendor.wlbtlog.udilog"
#define MXLOG_FILE_PATH_PROPOERTY "persist.vendor.wlbtlog.mxfilepath"
#define UDILOG_FILE_PATH_PROPOERTY "persist.vendor.wlbtlog.udifilepath"
#define RESTART_REASON_PROPOERTY "persist.vendor.wlbtlog.restart_reason"
#endif

class Common {
private:
    static void initDefaultValue(void);

public:
    static void setPropertyValueInt(const char *buffer, int value);
    static int getPropertyValueInt(const char *buffer);
    static void setPropertyValueString(const char *propertyValue, const char *value);
    static std::string getPropertyValueString(const char *propertyValue);
    static void SetMxLogState(MxLogStatus s);
    static void SetUdiLogState(UdiLogStatus s);
    static void SetMxLogPath(const char *path);
    static void SetUdiLogPath(const char *path);
    static void SetRestartReason(const char *reason);
    static void LoadConfiguration();
    static void update_max_file_size(void);
    static void update_max_files(void);
    static void get_time_string(char *buffer, unsigned int size);
    static void send_kill_signal(int pid);
    static bool is_valid_pid(const int pid, const char *cmd);

private:
    static MxLogStatus  mxlog_status;
    static UdiLogStatus udilog_status;

public:
    static int max_file_size;
    static int max_files;
};
#endif
