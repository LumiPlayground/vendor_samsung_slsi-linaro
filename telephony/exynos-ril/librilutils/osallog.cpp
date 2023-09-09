/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#include <librilutils/osallog.h>
#include <librilutils/aospsystemlogger.h>
#include <cassert>

class SystemLoggerFactory {
  public:
    static SystemLoggerPtr CreateLogger(OsalLogOsType osType) {
        switch (osType) {
        case OsalLogOsType::OSAL_LOG_OS_TYPE_AOSP:
            return std::make_shared<const AospSystemLogger>();
        default:
            return std::make_shared<const AospSystemLogger>();
        }
    }
};

OsalLogPtr OsalLog::instance = nullptr;

void OsalLog::CreateInstance(OsalLogOsType osType) {
    instance.reset(new OsalLog(osType));
}

const OsalLog *OsalLog::GetInstance() {
    assert(instance != nullptr);
    return instance.get();
}

OsalLogOsType OsalLog::GetOsalLogOsType() {
    //to do: deciding logic when supporting other OS
    return OsalLogOsType::OSAL_LOG_OS_TYPE_AOSP;
}

void OsalLog::PrintRadioLog(LogLevel logLevel, const char *const format) const {
    std::ostringstream oss = PrintToOss(format);
    PrintRadioLog(logLevel, oss);
}

OsalLog::OsalLog(OsalLogOsType osType) {
    systemLogger = SystemLoggerFactory::CreateLogger(osType);
}

const SystemLogger *OsalLog::GetSystemLogger() const {
    assert(systemLogger != nullptr);
    return systemLogger.get();
}

std::ostringstream OsalLog::PrintToOss(const char *const format) const {
    std::ostringstream retOss;
    retOss << format;
    return retOss;
}

void OsalLog::PrintRadioLog(LogLevel logLevel, std::ostringstream &oss) const {
    switch (logLevel) {
    case LogLevel::OSAL_LOG_LEVEL_ERROR:
        PrintRadioLogError(oss);
        break;
    case LogLevel::OSAL_LOG_LEVEL_WARN:
        PrintRadioLogWarning(oss);
        break;
    case LogLevel::OSAL_LOG_LEVEL_DEBUG:
        PrintRadioLogDebug(oss);
        break;
    case LogLevel::OSAL_LOG_LEVEL_INFO:
        PrintRadioLogInfo(oss);
        break;
    case LogLevel::OSAL_LOG_LEVEL_VERBOSE:
        PrintRadioLogVerbose(oss);
        break;
    default:
        PrintRadioLogInfo(oss);
        break;
    }
}

void OsalLog::PrintRadioLogError(const std::ostringstream &oss) const {
    GetSystemLogger()->PrintRadioLogError(oss);
}

void OsalLog::PrintRadioLogWarning(const std::ostringstream &oss) const {
    GetSystemLogger()->PrintRadioLogWarning(oss);
}

void OsalLog::PrintRadioLogDebug(const std::ostringstream &oss) const {
    GetSystemLogger()->PrintRadioLogDebug(oss);
}

void OsalLog::PrintRadioLogInfo(const std::ostringstream &oss) const {
    GetSystemLogger()->PrintRadioLogInfo(oss);
}

void OsalLog::PrintRadioLogVerbose(const std::ostringstream &oss) const {
    GetSystemLogger()->PrintRadioLogVerbose(oss);
}