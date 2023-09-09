/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#include "rillog.h"
#include <time.h>
#include <librilutils/build.h>
#include "rillog.h"
#include "rilversioninfo.h"
#include "rillogcapture.h"
#include <librilutils/systemproperty.h>
#include <librilutils/textutils.h>
#include <string>

#define PROPERTY_RIL_CONFIG_VDBG "persist.vendor.config.vdbg"
#define PROPERTY_RIL_LOG_CATEGORY_MASK "persist.vendor.radio.log.categorymask"

#define PROPERTY_RIL_LOG_LEVEL "persist.vendor.ril.debug_level"
#define RIL_LOG_LEVEL_LOW "0x6c65"
#define RIL_LOG_LEVEL_MID "0x6d69"
#define RIL_LOG_LEVEL_HIGH "0x6876"

#define PROPERTY_RIL_LOG_FEATURE_MASK "persist.vendor.ril.log_mask"
#define RIL_LOG_FEATURE_MASK_LOGCAT_FIELD (0x1)
#define RIL_LOG_FEATURE_MASK_LOGCAPTURE_FIELD (0x2)

bool LOG_VDBG = (SystemProperty::GetInt(PROPERTY_RIL_CONFIG_VDBG, 0) != 0);

CMutex RilLogMutex;

bool CRilLog::m_enableLogcat = true;
bool CRilLog::m_enableLogCapture = false;
int CRilLog::m_PrintCategory = RIL_LOG_CATEGORY_MASK_ALL;
int CRilLog::m_logLevel = 0;
BOOL CRilLog::m_bInitialized = FALSE;
RilLogCapture *CRilLog::m_pRilLogCapture = nullptr;

void CRilLog::InitRilLog() {
    m_enableLogcat = GetLogcatEnabledInProperty();
    m_enableLogCapture = GetLogCaptureEnabledInProperty();
    m_logLevel = GetYieldedLogLevel();
    m_PrintCategory = GetPrintCategoryInProperty();

    if (IsOkToStartLogCapture()) {
        StartRilLogCapture();
    }

    m_bInitialized = TRUE;

    LogConfigStatus(__FUNCTION__);
}

void CRilLog::Log(int category, int level, const char *const szFormatString) {
    if (IsLogAllowed(category, level) && IsLogFeaturesEnabled()) {
        Logv(level, szFormatString);
    }
}

void CRilLog::Logv(int level, const char *const szFormatString) {
    int numOfChars = std::snprintf(nullptr, 0, szFormatString, 0) + 1;
    if (numOfChars <= 0) {
        RilLogMutex.lock();
        PrintLogLine(level, "Error during Log formatting.");
        RilLogMutex.unlock();
        return;
    }
    auto size = static_cast<size_t>(numOfChars);
    auto buf = std::make_unique<char[]>(size);
    std::snprintf(buf.get(), size, szFormatString, 0);

    RilLogMutex.lock();
    PrintLogLine(level, buf.get());
    RilLogMutex.unlock();
}

void CRilLog::PrintLogLine(int level, const char *szLogText) {
    if (IsLogCaptureStarted()) {
        m_pRilLogCapture->notifyNewRilLog(szLogText);
    }

    if (m_enableLogcat) {
        switch(level)
        {
        case E_RIL_CRITICAL_LOG:
            OSAL_RADIO_LOGE("%s", szLogText);
            break;
        case E_RIL_WARNING_LOG:
            OSAL_RADIO_LOGW("%s", szLogText);
            break;
        case E_RIL_VERBOSE_LOG:
            OSAL_RADIO_LOGV("%s", szLogText);
            break;
        case E_RIL_INFO_LOG:
        default:
            OSAL_RADIO_LOGI("%s", szLogText);
            break;
        }
    }
}

void CRilLog::BufferedLogFlash(int category, int level, char **ppbuffer) {
    if (ppbuffer == NULL) {
        return;
    }

    if (IsLogAllowed(category, level) && IsLogFeaturesEnabled()) {
        RilLogMutex.lock();
        for (unsigned int i = 0; i < m_uiMaxBufferedCount; i++) {
            if ( ppbuffer[i] == NULL ) {
                break;
            }

            PrintLogLine(level, ppbuffer[i]);
        }
        RilLogMutex.unlock();
    }

    // free all buffer memory
    for (unsigned int i = 0; i < m_uiMaxBufferedCount; i++) {
        if (ppbuffer[i] != NULL) {
            delete[] ppbuffer[i];
        }
    }
    delete [] ppbuffer;
}

void CRilLog::DumpResetLog(const char *reset_reason) {
    int nResetCnt = SystemProperty::GetInt(PROPERTY_RIL_RESET, 0);
    Log(E_RIL_INFO_LOG, "%s: RIL reset count : %d, Reason : %s", __FUNCTION__, nResetCnt, reset_reason);
    SystemProperty::Set(PROPERTY_RIL_RESET, nResetCnt + 1);
}

const char *CRilLog::GetParentLogPath(void) {
    return "/data/vendor/dump";
}

bool CRilLog::GetLogcatEnabledInProperty() {
    return (SystemProperty::GetInt(PROPERTY_RIL_LOG_FEATURE_MASK, 1) & RIL_LOG_FEATURE_MASK_LOGCAT_FIELD);
}

bool CRilLog::GetLogCaptureEnabledInProperty() {
    return (SystemProperty::GetInt(PROPERTY_RIL_LOG_FEATURE_MASK, 1) & RIL_LOG_FEATURE_MASK_LOGCAPTURE_FIELD);
}

int CRilLog::GetPrintCategoryInProperty() {
    return (SystemProperty::GetInt(PROPERTY_RIL_LOG_CATEGORY_MASK, RIL_LOG_CATEGORY_MASK_ALL) & RIL_LOG_CATEGORY_MASK_ALL);
}

int CRilLog::GetYieldedLogLevel() {
    if (GetLogLevelByConfig() > 0) {
        return GetLogLevelByConfig();
    } else {
        return GetLogLevelByDefault();
    }
}

bool CRilLog::IsLogFeaturesEnabled() {
    return (m_enableLogcat || m_enableLogCapture);
}

bool CRilLog::IsLogAllowed(int category, int level) {
    return (m_bInitialized == true && m_logLevel <= level && (m_PrintCategory == RIL_LOG_CATEGORY_MASK_ALL || ((m_PrintCategory & category) != 0)));
}

bool CRilLog::IsOkToStartLogCapture() {
    return m_enableLogCapture && (m_logLevel == E_RIL_VERBOSE_LOG);
}

bool CRilLog::IsLogCaptureStarted() {
    return (m_enableLogCapture && m_pRilLogCapture != nullptr);
}

void CRilLog::StartRilLogCapture() {
    m_pRilLogCapture = RilLogCapture::MakeInstance();
    if (m_pRilLogCapture == nullptr || m_pRilLogCapture->Start() < 0) {
        delete m_pRilLogCapture;
        m_pRilLogCapture = nullptr;
    }
}

void CRilLog::LogConfigStatus(const char *const callerFunctionName) {
    // The bleow log shall be printed regardless of CRilLog config values. That's why low level API(=OSAL_RADIO_LOGD) is used.
    OSAL_RADIO_LOGD("%s: m_enableLogcat=%d m_enableLogCapture=%d", callerFunctionName, m_enableLogcat, m_enableLogCapture);
    OSAL_RADIO_LOGD("%s: log level = 0x%x", callerFunctionName, m_logLevel);
    OSAL_RADIO_LOGD("%s: Log Category is %d", callerFunctionName, m_PrintCategory);
    OSAL_RADIO_LOGD("%s: RilLogCapture started: %s", __FUNCTION__, (IsLogCaptureStarted() ? "YES" : "NOT"));
    OSAL_RADIO_LOGD("%s: RIL library build info : %s", callerFunctionName, RILVersionInfo::getBuildId().c_str());
}

int CRilLog::GetLogLevelByConfig() {
    std::string config = SystemProperty::Get(PROPERTY_RIL_LOG_LEVEL, "");
    if (TextUtils::IsEmpty(config)) {
        return 0;
    }

    // use magic code
    if (TextUtils::Equals(config.c_str(), RIL_LOG_LEVEL_LOW)) {
        return E_RIL_CRITICAL_LOG;
    } else if (TextUtils::Equals(config.c_str(), RIL_LOG_LEVEL_MID)) {
        return E_RIL_INFO_LOG;
    } else if (TextUtils::Equals(config.c_str(), RIL_LOG_LEVEL_HIGH)) {
        return E_RIL_VERBOSE_LOG;
    }
    return E_RIL_CRITICAL_LOG;
}

int CRilLog::GetLogLevelByDefault() {
    bool isDebug = !Build::IS_USER;
    return (isDebug ? E_RIL_VERBOSE_LOG : E_RIL_WARNING_LOG);
}

void WaitForTimeUs(const char *caller, unsigned us) {
    timespec ts, ts_r;
    ts.tv_sec = us / 1000'000U;
    ts.tv_nsec = (us % 1000'000U) * 1000U;
    ts_r = ts;
    int ret = nanosleep(&ts, &ts_r);
    if (ret == -1) {
        RilLogE("%s: sleep was interrupted, remaining:%d.%09u of %d.%09u sec, ret=%d, errno=%d", caller,
                ts_r.tv_sec, ts_r.tv_nsec, ts.tv_sec, ts.tv_nsec, ret, errno);
    }
}
