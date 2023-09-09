/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef _RIL_LOG_H_
#define _RIL_LOG_H_

#include <librilutils/osallog.h>
#include "types.h"
#include "mutex.h"

#define PROPERTY_RIL_RESET "vendor.ril.reset_count"
#define RIL_LOG_CATEGORY_MASK_ALL 0xFFFFFF

enum {
    RIL_LOG_CAT_CORE = 1,
    RIL_LOG_CAT_CALL = 1<<1,
    RIL_LOG_CAT_SMS = 1<<2,
    RIL_LOG_CAT_SIM = 1<<3,
    RIL_LOG_CAT_NET = 1<<4,    //16
    RIL_LOG_CAT_PDP = 1<<5,
    RIL_LOG_CAT_MISC = 1<<6,
    RIL_LOG_CAT_SOUND = 1<<7,    //128
    RIL_LOG_CAT_OEM = 1<<8,
    RIL_LOG_CAT_RFS = 1<<9,
    RIL_LOG_CAT_IMS = 1<<10,
    RIL_LOG_CAT_GPS = 1<<11,
    RIL_LOG_CAT_WLAN = 1<<12,
    RIL_LOG_CAT_AIMS = 1<<13,
    RIL_LOG_CAT_VSIM = 1<<14,
    RIL_LOG_CAT_NETIF = 1<<15,
    RIL_LOG_CAT_NETL = 1<<16,
    RIL_LOG_CAT_TELPRO = 1<<17,
    RIL_LOG_CAT_OPERTABLE = 1<<18,
    RIL_LOG_CAT_DATA = 1<<19,
    RIL_LOG_CAT_ETC = 1<<20,
    RIL_LOG_CAT_STK = 1<<21,
    RIL_LOG_CAT_EMBMS = 1<<22,
    RIL_LOG_CAT_SIMIO = 1<<23,
    RIL_LOG_CAT_SAP = 1<<24,
};

#define RilLogE(format, ...) CRilLog::Log(CRilLog::E_RIL_CRITICAL_LOG,  format, ##__VA_ARGS__)
#define RilLogW(format, ...) CRilLog::Log(CRilLog::E_RIL_WARNING_LOG,  format, ##__VA_ARGS__)
#undef RilLogI
#define RilLogI(format, ...) CRilLog::Log(CRilLog::E_RIL_INFO_LOG,  format, ##__VA_ARGS__)
#undef RilLogV
#define RilLogV(format, ...) CRilLog::Log(CRilLog::E_RIL_VERBOSE_LOG,  format, ##__VA_ARGS__)
#define RilLog RilLogI

extern bool LOG_VDBG;

class RilLogCapture;

class CRilLog
{
  public:
    static void InitRilLog();
    template <typename... Targs>
    static void Log(int level, const char *const szFormatString, Targs... args);
    static void Log(int category, int level, const char *const szFormatString);
    template <typename... Targs>
    static void Log(int category, int level, const char *const szFormatString, Targs... args);
    template <typename... Targs>
    static char **BufferedLog(char **buffer, const char *const szFormatString, Targs... args);
    static void BufferedLogFlash(int category, int level, char** buffer);

    static void DumpResetLog(const char *reset_reason);
    static const char *GetParentLogPath(void);

    enum
    {
        E_RIL_VERBOSE_LOG  = 0x01,
        E_RIL_INFO_LOG     = 0x02,
        E_RIL_WARNING_LOG  = 0x04,
        E_RIL_CRITICAL_LOG = 0x08
    };

  public:
    static const UINT32 m_uiMaxLogBufferSize = 1024;
    static const UINT32 m_uiMaxBufferedCount = 30;

  private:
    CRilLog() = default;

    static void PrintLogLine(int level, const char *szLogText);
    static void Logv(int level, const char *const szFormatString);
    template <typename... Targs>
    static void Logv(int level, const char *const szFormatString, Targs... args);

    static bool GetLogcatEnabledInProperty();
    static bool GetLogCaptureEnabledInProperty();
    static int GetPrintCategoryInProperty();
    static int GetYieldedLogLevel();
    static bool IsLogFeaturesEnabled();
    static bool IsLogAllowed(int category, int level);
    static bool IsOkToStartLogCapture();
    static bool IsLogCaptureStarted();
    static void StartRilLogCapture();
    static void LogConfigStatus(const char *const callerFunctionName);
    static int GetLogLevelByConfig();
    static int GetLogLevelByDefault();

  private:
    static bool m_enableLogcat;
    static bool m_enableLogCapture;
    static int m_PrintCategory;
    static int m_logLevel;
    static BOOL m_bInitialized;
    static RilLogCapture *m_pRilLogCapture;
};

extern CMutex RilLogMutex;

template <typename... Targs>
char **CRilLog::BufferedLog(char **ppbuffer, const char *const szFormatString, Targs... args) {
    if (ppbuffer == NULL) {
        ppbuffer = new char *[m_uiMaxBufferedCount];
        if (ppbuffer == NULL) {
            return NULL;
        }
        for (unsigned int i = 0; i < m_uiMaxBufferedCount; i++) {
            ppbuffer[i] = NULL;
        }
    }

    int size_s = std::snprintf(nullptr, 0, szFormatString, args...) + 2;
    if (size_s <= 0) {
        Log(E_RIL_CRITICAL_LOG, "Error during Log formatting.");
        return ppbuffer;
    }
    auto size = static_cast<size_t>(size_s);
    auto buf = std::make_unique<char[]>(size);
    std::snprintf(buf.get(), size - 1, szFormatString, args...);

    unsigned int new_len = strlen(buf.get());
    if (new_len > m_uiMaxLogBufferSize) {
        Log(E_RIL_CRITICAL_LOG, "RIL log size is over buffer size, Truncate overflowed logs");
        return ppbuffer;
    }

    unsigned int i = 0;
    for (i = 0; i < m_uiMaxBufferedCount; i++) {
        if (ppbuffer[i] == NULL) {
            break;
        }
    }

    // if log is over 8 * buffer size, just return current saved buffer
    if (i == m_uiMaxBufferedCount) {
        Log(E_RIL_CRITICAL_LOG, "RIL log size is over buffer count, Truncate overflowed logs");
        return ppbuffer;
    }

    ppbuffer[i] = new char[m_uiMaxLogBufferSize] {};
    if (ppbuffer[i] == NULL) {
        Log(E_RIL_CRITICAL_LOG, "log buffer allocation fail, return current buffer pointer : there can be missing logs");
        return ppbuffer;
    }

    SECURELIB::strncpy(ppbuffer[i], m_uiMaxLogBufferSize, buf.get(), SECURELIB::strlen(buf.get()));

    return ppbuffer;
}

template <typename... Targs>
void CRilLog::Log(int level, const char *const szFormatString, Targs... args) {
    if (IsLogAllowed(RIL_LOG_CATEGORY_MASK_ALL, level) && IsLogFeaturesEnabled()) {
        Logv(level, szFormatString, args...);
    }
}

template <typename... Targs>
void CRilLog::Log(int category, int level, const char *const szFormatString, Targs... args) {
    if (IsLogAllowed(category, level) && IsLogFeaturesEnabled()) {
        Logv(level, szFormatString, args...);
    }
}

template <typename... Targs>
void CRilLog::Logv(int level, const char *const szFormatString, Targs... args) {
    int size_s = std::snprintf(nullptr, 0, szFormatString, args...) + 1;
    if (size_s <= 0) {
        PrintLogLine(level, "Error during Log formatting.");
        return;
    }
    auto size = static_cast<size_t>(size_s);
    auto buf = std::make_unique<char[]>(size);
    std::snprintf(buf.get(), size, szFormatString, args...);

    RilLogMutex.lock();
    PrintLogLine(level, buf.get());
    RilLogMutex.unlock();
}

void WaitForTimeUs(const char *caller, unsigned us);
#endif /*_RIL_LOG_H_*/
