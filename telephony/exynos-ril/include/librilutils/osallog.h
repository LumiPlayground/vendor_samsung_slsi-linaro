/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#pragma once

#include "systemlogger.h"
#include <memory>
#include <sstream>

class OsalLog;
using OsalLogPtr = std::unique_ptr<const OsalLog>;

enum class OsalLogOsType {
    OSAL_LOG_OS_TYPE_AOSP,
};

class OsalLog {
  public:
    enum class LogLevel {
        OSAL_LOG_LEVEL_UNKNOWN,
        OSAL_LOG_LEVEL_VERBOSE,
        OSAL_LOG_LEVEL_DEBUG,
        OSAL_LOG_LEVEL_INFO,
        OSAL_LOG_LEVEL_WARN,
        OSAL_LOG_LEVEL_ERROR,
        OSAL_LOG_LEVEL_FATAL,
        OSAL_LOG_LEVEL_MAX,
    };

    static void CreateInstance(OsalLogOsType osType);
    static const OsalLog *GetInstance();
    static OsalLogOsType GetOsalLogOsType();

    void PrintRadioLog(LogLevel logLevel, const char *const format) const;
    template <typename... Targs>
    void PrintRadioLog(LogLevel logLevel, const char *const format, Targs... args) const;

  private:
    OsalLog(OsalLogOsType osType);
    const SystemLogger *GetSystemLogger() const;

    std::ostringstream PrintToOss(const char *const format) const;
    template <typename... Targs>
    std::ostringstream PrintToOss(const char *const format, Targs... args) const;
    void PrintRadioLog(LogLevel logLevel, std::ostringstream &oss) const;
    void PrintRadioLogError(const std::ostringstream &oss) const;
    void PrintRadioLogWarning(const std::ostringstream &oss) const;
    void PrintRadioLogDebug(const std::ostringstream &oss) const;
    void PrintRadioLogInfo(const std::ostringstream &oss) const;
    void PrintRadioLogVerbose(const std::ostringstream &oss) const;

  private:
    static OsalLogPtr instance;

    SystemLoggerPtr systemLogger;
};

template <typename... Targs>
void OsalLog::PrintRadioLog(LogLevel logLevel, const char *const format, Targs... args) const {
    std::ostringstream oss = PrintToOss(format, args...);
    PrintRadioLog(logLevel, oss);
}

template <typename... Targs>
std::ostringstream OsalLog::PrintToOss(const char *const format, Targs... args) const {
    std::ostringstream retOss;
    int numOfChars = std::snprintf(nullptr, 0, format, args...);
    if (numOfChars <= 0) {
        retOss << "OsalLog::Error during Log formatting";
        return retOss;
    }
    auto sizeOfBuffer = static_cast<size_t>(numOfChars + 1);
    auto buffer = std::make_unique<char[]>(sizeOfBuffer);
    std::snprintf(buffer.get(), sizeOfBuffer, format, args...);
    retOss << buffer.get();
    return retOss;
}

#define CREATE_OSAL_LOG_INSTANCE (OsalLog::CreateInstance(OsalLog::GetOsalLogOsType()))

#define OSAL_RADIO_LOGE(format, ...) (OsalLog::GetInstance()->PrintRadioLog(OsalLog::LogLevel::OSAL_LOG_LEVEL_ERROR, format, ##__VA_ARGS__))
#define OSAL_RADIO_LOGW(format, ...) (OsalLog::GetInstance()->PrintRadioLog(OsalLog::LogLevel::OSAL_LOG_LEVEL_WARN, format, ##__VA_ARGS__))
#define OSAL_RADIO_LOGD(format, ...) (OsalLog::GetInstance()->PrintRadioLog(OsalLog::LogLevel::OSAL_LOG_LEVEL_DEBUG, format, ##__VA_ARGS__))
#define OSAL_RADIO_LOGI(format, ...) (OsalLog::GetInstance()->PrintRadioLog(OsalLog::LogLevel::OSAL_LOG_LEVEL_INFO, format, ##__VA_ARGS__))
#define OSAL_RADIO_LOGV(format, ...) (OsalLog::GetInstance()->PrintRadioLog(OsalLog::LogLevel::OSAL_LOG_LEVEL_VERBOSE, format, ##__VA_ARGS__))