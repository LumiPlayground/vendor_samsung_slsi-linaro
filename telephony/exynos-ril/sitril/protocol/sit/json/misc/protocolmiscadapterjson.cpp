/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

/*
 * protocolmiscadapterjson.cpp
 *
 *  Created on: 2021. 08. 16.
 *      Author: p1.bansal
 */
#include "protocolmiscadapterjson.h"
#include "miscjsondef.h"
#include "protocolutils.h"
#include "oemjsondef.h"
#include "rillog.h"
#include "DataWriter.h"
#include "rildef.h"

// add category to display selective logs
#undef RilLogI
#define RilLogI(format, ...) CRilLog::Log(RIL_LOG_CAT_MISC, CRilLog::E_RIL_INFO_LOG,  format, ##__VA_ARGS__)
#undef RilLogV
#define RilLogV(format, ...) CRilLog::Log(RIL_LOG_CAT_MISC, CRilLog::E_RIL_VERBOSE_LOG,  format, ##__VA_ARGS__)
#undef RilLogW
#define RilLogW(format, ...) CRilLog::Log(RIL_LOG_CAT_MISC, CRilLog::E_RIL_WARNING_LOG,  format, ##__VA_ARGS__)
#undef RilLogE
#define RilLogE(format, ...) CRilLog::Log(RIL_LOG_CAT_MISC, CRilLog::E_RIL_CRITICAL_LOG,  format, ##__VA_ARGS__)

using std::string;

/**
 * ProtocolMiscSetManualBandModeAdapterJson
 */
BYTE ProtocolMiscSetManualBandModeAdapterJson::GetCause() {
    if(!m_bParsingSuccess)
        return 0;

    return m_jsonData[ns_misc_net_set_manual_band_mode_rsp::key_manual_mode_cause].asInt();
}

/**
 * ProtocolMiscSetRfDesenseModeAdapterJson
 */
BYTE ProtocolMiscSetRfDesenseModeAdapterJson::GetCause() {
    if(!m_bParsingSuccess)
        return 0;

    return m_jsonData[ns_misc_net_set_rf_desense_mode_rsp::key_desense_cause].asInt();
}

/**
 * ProtocolMiscVersionAdapterJson
 */
ProtocolMiscVersionAdapterJson::ProtocolMiscVersionAdapterJson(const ModemData *pModemData)
    : ProtocolJsonRespAdapter(pModemData) {
    Init();
}

void ProtocolMiscVersionAdapterJson::Init() {
    ParseJsonPayload(SIT_GET_BASEBAND_VERSION);

    m_swVer = m_jsonData[ns_misc_get_baseband_version_rsp::key_sw_ver].asString();
    ProtocolUtils::LimitStringToMaxLength(RIL_LOG_CAT_MISC, __CLASS_FUNC__, m_swVer, MAX_SW_VER_LEN);

    m_hwVer = m_jsonData[ns_misc_get_baseband_version_rsp::key_hw_ver].asString();
    ProtocolUtils::LimitStringToMaxLength(RIL_LOG_CAT_MISC, __CLASS_FUNC__, m_hwVer, MAX_HW_VER_LEN);

    m_rfCalDate = m_jsonData[ns_misc_get_baseband_version_rsp::key_rf_cal_date].asString();
    ProtocolUtils::LimitStringToMaxLength(RIL_LOG_CAT_MISC, __CLASS_FUNC__, m_rfCalDate, MAX_RF_CAL_DATE_LEN);

    m_productCode = m_jsonData[ns_misc_get_baseband_version_rsp::key_product_code].asString();
    ProtocolUtils::LimitStringToMaxLength(RIL_LOG_CAT_MISC, __CLASS_FUNC__, m_productCode, MAX_PRODUCT_CODE_LEN);

    m_modelId = m_jsonData[ns_misc_get_baseband_version_rsp::key_model_id].asString();
    ProtocolUtils::LimitStringToMaxLength(RIL_LOG_CAT_MISC, __CLASS_FUNC__, m_modelId, MAX_MODEL_ID_LEN);

    m_prlVer = m_jsonData[ns_misc_get_baseband_version_rsp::key_prl_ver].asString();
    ProtocolUtils::LimitStringToMaxLength(RIL_LOG_CAT_MISC, __CLASS_FUNC__, m_prlVer, MAX_PRL_VER_LEN);

    m_eriVer = m_jsonData[ns_misc_get_baseband_version_rsp::key_eri_ver].asString();
    ProtocolUtils::LimitStringToMaxLength(RIL_LOG_CAT_MISC, __CLASS_FUNC__, m_eriVer, MAX_ERI_VER_LEN);

    m_cpChipset = m_jsonData[ns_misc_get_baseband_version_rsp::key_cp_chipset].asString();
    ProtocolUtils::LimitStringToMaxLength(RIL_LOG_CAT_MISC, __CLASS_FUNC__, m_cpChipset, MAX_CP_CHIPSET_LEN);
}

int ProtocolMiscVersionAdapterJson::GetMask() const {
    if(!m_bParsingSuccess)
        return 0;

    return m_jsonData[ns_misc_get_baseband_version_rsp::key_ver_mask].asInt();
}

const char * ProtocolMiscVersionAdapterJson::GetSwVer() const {
    if(!m_bParsingSuccess)
        return NULL;

    return m_swVer.c_str();
}

const char * ProtocolMiscVersionAdapterJson::GetHwVer() const {
    if(!m_bParsingSuccess)
        return NULL;

    return m_hwVer.c_str();
}

const char * ProtocolMiscVersionAdapterJson::GetRfCalDate() const {
    if(!m_bParsingSuccess)
        return NULL;

    return m_rfCalDate.c_str();
}

const char * ProtocolMiscVersionAdapterJson::GetProdCode() const {
    if(!m_bParsingSuccess)
        return NULL;

    return m_productCode.c_str();
}

const char * ProtocolMiscVersionAdapterJson::GetModelID() const {
    if(!m_bParsingSuccess)
        return NULL;

    return m_modelId.c_str();
}

int ProtocolMiscVersionAdapterJson::GetPrlNamNum() const {
        return 0; // This field is not part of JSON response as in legacy CP was always sending 0
}

const BYTE * ProtocolMiscVersionAdapterJson::GetPrlVersion() const {
    if(!m_bParsingSuccess)
        return NULL;

    return (const BYTE *)m_prlVer.c_str();
}

int ProtocolMiscVersionAdapterJson::GetEriNamNum() const {
    return 0; // This field is not part of JSON response as in legacy CP was always sending 0
}

const BYTE * ProtocolMiscVersionAdapterJson::GetEriVersion() const {
    if(!m_bParsingSuccess)
        return NULL;

    return (const BYTE *)m_eriVer.c_str();
}

const BYTE * ProtocolMiscVersionAdapterJson::GetCPChipSet() const {
    if(!m_bParsingSuccess)
        return NULL;

    return (const BYTE*)m_cpChipset.c_str();
}

/**
 * ProtocolMiscGetTtyAdapterJson
 */
ProtocolMiscGetTtyAdapterJson::ProtocolMiscGetTtyAdapterJson(const ModemData *pModemData)
    : ProtocolJsonRespAdapter(pModemData) {
        ParseJsonPayload(SIT_GET_TTY_MODE);
}

int ProtocolMiscGetTtyAdapterJson::GetTtyMode() const {
    if(!m_bParsingSuccess)
        return SIT_MISC_TTY_MODE_OFF;

    return m_jsonData[ns_misc_get_tty_mode_rsp::key_tty_mode].asInt();
}

/**
 * ProtocolMiscNITZTimeAdapterJson
 */
ProtocolMiscNITZTimeAdapterJson::ProtocolMiscNITZTimeAdapterJson(const ModemData *pModemData)
    : ProtocolJsonIndAdapter(pModemData) {
        ParseJsonPayload(SIT_IND_NITZ_TIME_RECEIVED);
    }

int ProtocolMiscNITZTimeAdapterJson::TimeInfoType() const {
    return 0; // TODO: Remove support of this API as timeInfo no longer part of indicaiton
}

int ProtocolMiscNITZTimeAdapterJson::DayLightValid() const {
    return SIT_NITZ_DAYLIGHT_INFO_VALID; // TODO: Consider to remove it as not used by RIL anymore
}

int ProtocolMiscNITZTimeAdapterJson::Year() const {
    if(!m_bParsingSuccess)
        return 0;

    return m_jsonData[ns_misc_nitz_time_ind::key_year].asInt();
}

int ProtocolMiscNITZTimeAdapterJson::Month() const {
    if(!m_bParsingSuccess)
        return 0;

    return m_jsonData[ns_misc_nitz_time_ind::key_month].asInt();
}

int ProtocolMiscNITZTimeAdapterJson::Day() const {
    if(!m_bParsingSuccess)
        return 0;

    return m_jsonData[ns_misc_nitz_time_ind::key_day].asInt();
}

int ProtocolMiscNITZTimeAdapterJson::Hour() const {
    if(!m_bParsingSuccess)
        return 0;

    return m_jsonData[ns_misc_nitz_time_ind::key_hour].asInt();
}

int ProtocolMiscNITZTimeAdapterJson::Minute() const {
    if(!m_bParsingSuccess)
        return 0;

    return m_jsonData[ns_misc_nitz_time_ind::key_minute].asInt();
}

int ProtocolMiscNITZTimeAdapterJson::Second() const {
    if(!m_bParsingSuccess)
        return 0;

    return m_jsonData[ns_misc_nitz_time_ind::key_second].asInt();
}

int ProtocolMiscNITZTimeAdapterJson::TimeZone() const {
#define BITSIZE 8
#define SIGNFLAG (1<<(BITSIZE-1))
#define DATABITS (SIGNFLAG-1)

    if(!m_bParsingSuccess)
        return 0;

    int timezone = 0;
    timezone = m_jsonData[ns_misc_nitz_time_ind::key_time_zone].asInt();
    if ((timezone& SIGNFLAG)!=0) {                    // signflag set
        timezone= (~timezone & DATABITS) + 1;         // 2s complement without signflag
        timezone= -timezone;                              // negative number
    }
    return timezone;
}

int ProtocolMiscNITZTimeAdapterJson::DayLightAdjust() const {
    if(!m_bParsingSuccess)
        return SIT_NITZ_DAYLIGHT_ADJUST_NOADJUST;

    return m_jsonData[ns_misc_nitz_time_ind::key_daylight_adjust].asInt();
}

int ProtocolMiscNITZTimeAdapterJson::DayofWeek() const {
    return SIT_NITZ_DAY_OF_WEEK_SUN; // TODO: Consider to remove it as not used by RIL anymore
}

int ProtocolMiscNITZTimeAdapterJson::GetMMInfo() const {
    return 0; // TODO: Consider to remove it as not used by RIL anymore
}

const BYTE * ProtocolMiscNITZTimeAdapterJson::GetPLMN() const {
    return NULL; // TODO: Consider to remove it as not used by RIL anymore
}

/**
 * ProtocolMiscGetActivityInfoAdapterJson
 */
ProtocolMiscGetActivityInfoAdapterJson::ProtocolMiscGetActivityInfoAdapterJson(const ModemData *pModemData)
        : ProtocolJsonRespAdapter(pModemData)
{
    ParseJsonPayload(SIT_GET_ACTIVITY_INFO);

    if(m_bParsingSuccess) {
        for(int i = 0; i < RIL_NUM_TX_POWER_LEVELS; i++) {
            txModeTime[i] = m_jsonData[ns_misc_get_activity_info_rsp::key_tx_mode_time][i].asUInt();
        }
    }
}

UINT32 ProtocolMiscGetActivityInfoAdapterJson::GetSleepPeriod() const {
    if(!m_bParsingSuccess)
        return 0;

    return m_jsonData[ns_misc_get_activity_info_rsp::key_sleep_mode_time].asUInt();
}

UINT32 ProtocolMiscGetActivityInfoAdapterJson::GetIdlePeriod() const {
    if(!m_bParsingSuccess)
        return 0;

    return m_jsonData[ns_misc_get_activity_info_rsp::key_idle_mode_time].asUInt();
}

UINT32* ProtocolMiscGetActivityInfoAdapterJson::GetTxPeriod() const {
    if(!m_bParsingSuccess)
        return NULL;

    return (UINT32 *)&txModeTime;
}

UINT32 ProtocolMiscGetActivityInfoAdapterJson::GetRxPeriod() const {
    if(!m_bParsingSuccess)
        return 0;

    return m_jsonData[ns_misc_get_activity_info_rsp::key_rx_mode_time].asUInt();
}

/**
 * ProtocolCdmaSubscriptionAdapterJson
 */
ProtocolCdmaSubscriptionAdapterJson::ProtocolCdmaSubscriptionAdapterJson(const ModemData *pModemData)
    : ProtocolJsonRespAdapter(pModemData) {
    Init();
}

void ProtocolCdmaSubscriptionAdapterJson::Init()
{
    memset(m_szMdn, 0, MAX_CDMA_MDN_LEN);
    memset(m_szMin, 0, MAX_CDMA_MIN_LEN);
    m_wSid = m_wNid = m_uPrlVersion = 0;

    ParseJsonPayload(SIT_GET_CDMA_SUBSCRIPTION);
    if(m_bParsingSuccess) {
        string mdn = m_jsonData[ns_misc_get_cdma_subscription_rsp::key_mdn].asString();
        ProtocolUtils::LimitStringToMaxLength(RIL_LOG_CAT_MISC, __CLASS_FUNC__, mdn, MAX_CDMA_MDN_LEN);
        memcpy(m_szMdn, mdn.c_str(), mdn.length());

        string min = m_jsonData[ns_misc_get_cdma_subscription_rsp::key_min].asString();
        ProtocolUtils::LimitStringToMaxLength(RIL_LOG_CAT_MISC, __CLASS_FUNC__, min, MAX_CDMA_MIN_LEN);
        memcpy(m_szMin, min.c_str(), min.length());

        m_wSid = m_jsonData[ns_misc_get_cdma_subscription_rsp::key_home_sid].asInt();
        m_wNid = m_jsonData[ns_misc_get_cdma_subscription_rsp::key_home_nid].asInt();
        m_uPrlVersion = m_jsonData[ns_misc_get_cdma_subscription_rsp::key_prl_version].asUInt();
    }
}

/**
 * ProtocolMiscGetActivityInfoAdapterJson
 */
ProtocolMiscGetMslCodeAdapterJson::ProtocolMiscGetMslCodeAdapterJson(const ModemData *pModemData) : ProtocolJsonRespAdapter(pModemData) { ParseJsonPayload(SIT_GET_MSL_CODE); }

const char * ProtocolMiscGetMslCodeAdapterJson::getMslCode() const {
    if(m_bParsingSuccess) {
        return m_jsonData[ns_misc_get_msl_code_rsp::key_msl_code].asCString();
    }
    return NULL;
}

/**
 * ProtocolGetVoiceOperationAdapterJson
 */
ProtocolGetVoiceOperationAdapterJson::ProtocolGetVoiceOperationAdapterJson(const ModemData *pModemData) : ProtocolJsonRespAdapter(pModemData) { ParseJsonPayload(SIT_GET_VOICE_OPERATION); }

int ProtocolGetVoiceOperationAdapterJson::GetMode() const {
    if(m_bParsingSuccess) {
        return (m_jsonData[ns_misc_get_voice_operation_rsp::key_mode].asUInt() > 0U ? 1 : 0);
    }
    return 0;
}

/**
 * ProtocolMiscPinControlAdapterJson
 */
ProtocolMiscPinControlAdapterJson::ProtocolMiscPinControlAdapterJson(const ModemData *pModemData)
    : ProtocolJsonIndAdapter(pModemData)
{
    m_Signal = 0;
    m_Status = 0;

    ParseJsonPayload(SIT_IND_PIN_CONTROL);

    if (m_bParsingSuccess) {
            m_Signal = (BYTE)m_jsonData[ns_misc_pin_control_ind::key_pin_signal].asInt();
            m_Status = (BYTE)m_jsonData[ns_misc_pin_control_ind::key_pin_status].asInt();
    }
}

BYTE ProtocolMiscPinControlAdapterJson::GetSignal() {
    return m_Signal;
}

BYTE ProtocolMiscPinControlAdapterJson::GetStatus() {
    return m_Status;
}

/**
 * ProtocolMiscSarControlStateAdapterJson
 */
ProtocolMiscSarControlStateAdapterJson::ProtocolMiscSarControlStateAdapterJson(const ModemData *pModemData)
    : ProtocolJsonIndAdapter(pModemData)
{
    m_DeviceState = 0;

    ParseJsonPayload(SIT_IND_SAR_CONTROL_STATE);
    if (m_bParsingSuccess) {
        m_DeviceState = (BYTE)m_jsonData[ns_misc_sar_control_state_ind::key_device_state].asInt();
    }
}

BYTE ProtocolMiscSarControlStateAdapterJson::GetDeviceState() {
    return m_DeviceState;
}

/**
 * ProtocolMiscGetPreferredCallCapability
 */
ProtocolMiscGetPreferredCallCapabilityAdapterJson::ProtocolMiscGetPreferredCallCapabilityAdapterJson(const ModemData *pModemData) : ProtocolJsonRespAdapter(pModemData) { ParseJsonPayload(SIT_GET_PREFERRED_CALL_CAPABILITY); }

int ProtocolMiscGetPreferredCallCapabilityAdapterJson::GetMode() const {
    if (m_bParsingSuccess) {
        return m_jsonData[ns_misc_get_preferred_call_capability_rsp::key_mode].asInt();
    }
    return 0;
}

/**
 * ProtocolMiscATCommandAdapterJson
 */
ProtocolMiscATCommandAdapterJson::ProtocolMiscATCommandAdapterJson(const ModemData *pModemData)
    : ProtocolJsonIndAdapter(pModemData)
{
    mCommand = NULL;
    mCommandLength = 0;
    ParseJsonPayload(SIT_IND_FORWARDING_AT_COMMAND);
    Init();
}

void ProtocolMiscATCommandAdapterJson::Init() {
    if(m_bParsingSuccess) {
        string command = m_jsonData[ns_misc_forwarding_at_command_ind::key_data].asString();
        ProtocolUtils::LimitStringToMaxLength(RIL_LOG_CAT_MISC, __CLASS_FUNC__, command, MAX_SIT_AT_COMMAND_LENGTH);

        mCommandLength = command.length();
        mCommand = new char[mCommandLength + 1];
        if (mCommand != NULL) {
            memset(mCommand, 0, mCommandLength + 1);
            memcpy(mCommand, command.c_str(), mCommandLength);
        }
        else {
            mCommandLength = 0;
        }
    }
}

ProtocolMiscATCommandAdapterJson::~ProtocolMiscATCommandAdapterJson() {
    if (mCommand != NULL) {
        delete[] mCommand;
        mCommand = NULL;
    }
}

/**
 * ProtocolMiscSetSelflogAdapterJson
 */
ProtocolMiscSetSelflogAdapterJson::ProtocolMiscSetSelflogAdapterJson(const ModemData *pModemData) : ProtocolJsonRespAdapter(pModemData) { ParseJsonPayload(SIT_SET_SELFLOG); }

int ProtocolMiscSetSelflogAdapterJson::GetSelflogResult() {
    if (m_bParsingSuccess) {
        return m_jsonData[ns_misc_set_selflog_rsp::key_result].asInt();
    }
    return 0;
}

/**
 * ProtocolMiscGetSelflogStatusAdapterJson
 */
ProtocolMiscGetSelflogStatusAdapterJson::ProtocolMiscGetSelflogStatusAdapterJson(const ModemData *pModemData) : ProtocolJsonRespAdapter(pModemData) { ParseJsonPayload(SIT_GET_SELFLOG_STATUS); }

int ProtocolMiscGetSelflogStatusAdapterJson::GetSelflogStatus() {
    if (m_bParsingSuccess) {
        return m_jsonData[ns_misc_get_selflog_status_rsp::key_status].asInt();
    }
    return 0;
}

/**
 * ProtocolMiscSelflogStatusAdapterJson
 */
ProtocolMiscSelflogStatusAdapterJson::ProtocolMiscSelflogStatusAdapterJson(const ModemData *pModemData) : ProtocolJsonIndAdapter(pModemData) { ParseJsonPayload(SIT_IND_SELFLOG_STATUS); }

BYTE ProtocolMiscSelflogStatusAdapterJson::GetIndSelflogStatus() {
    if (m_bParsingSuccess) {
        return (BYTE)m_jsonData[ns_misc_selflog_status_ind::key_status].asInt();
    }
    return 0;
}

/**
 * ProtocolMiscSetModemLogDumpAdapterJson
 */
ProtocolMiscSetModemLogDumpAdapterJson::ProtocolMiscSetModemLogDumpAdapterJson(const ModemData *pModemData) : ProtocolJsonRespAdapter(pModemData) { ParseJsonPayload(SIT_SET_MODEM_LOG_DUMP); }

int ProtocolMiscSetModemLogDumpAdapterJson::GetResult() const {
    if (m_bParsingSuccess) {
        return m_jsonData[ns_misc_set_modem_log_dump_rsp::key_result].asInt();
    }
    return 0;
}

/*
 * ProtocolMiscCurrentLinkCapacityEstimateAdapterJson
 */
void ProtocolMiscCurrentLinkCapacityEstimateAdapterJson::InitLinkCapaEstimate() {
    mCurLinkCapaEstimate.dl_capa_kbps = -1;
    mCurLinkCapaEstimate.ul_capa_kbps = -1;
    mCurLinkCapaEstimate.dcnr_sec_dl_capa_kbps = -1;
    mCurLinkCapaEstimate.dcnr_sec_ul_capa_kbps = -1;
}

ProtocolMiscCurrentLinkCapacityEstimateAdapterJson::ProtocolMiscCurrentLinkCapacityEstimateAdapterJson(const ModemData *pModemData) : ProtocolJsonIndAdapter(pModemData)
{
    InitLinkCapaEstimate();
    ParseJsonPayload(SIT_IND_CURRENT_LINK_CAPACITY_ESTIMATE);

    if(m_bParsingSuccess) {
        int dlCapaKbps, ulCapaKbps;
        int dcnrSecDlCapaKbps, dcnrSecUlCapaKbps;
        dlCapaKbps = m_jsonData[ns_misc_current_link_capacity_estimate_ind::key_dl_capa_kbps].asUInt();
        ulCapaKbps = m_jsonData[ns_misc_current_link_capacity_estimate_ind::key_ul_capa_kbps].asUInt();
        dcnrSecDlCapaKbps = m_jsonData[ns_misc_current_link_capacity_estimate_ind::key_dcnr_sec_dl_capa_kbps].asUInt();
        dcnrSecUlCapaKbps = m_jsonData[ns_misc_current_link_capacity_estimate_ind::key_dcnr_sec_ul_capa_kbps].asUInt();

        if (dlCapaKbps >= 0) {
            mCurLinkCapaEstimate.dl_capa_kbps = dlCapaKbps;
        }
        if (ulCapaKbps >= 0) {
            mCurLinkCapaEstimate.ul_capa_kbps = ulCapaKbps;
        }
        if (dcnrSecDlCapaKbps >= 0) {
            mCurLinkCapaEstimate.dcnr_sec_dl_capa_kbps = dcnrSecDlCapaKbps;
        }
        if (dcnrSecUlCapaKbps >= 0) {
            mCurLinkCapaEstimate.dcnr_sec_ul_capa_kbps = dcnrSecUlCapaKbps;
        }
    }
}

int ProtocolMiscCurrentLinkCapacityEstimateAdapterJson::GetDlCapaKbps() const
{
    return mCurLinkCapaEstimate.dl_capa_kbps;
}

int ProtocolMiscCurrentLinkCapacityEstimateAdapterJson::GetUlCapaKbps() const
{
    return mCurLinkCapaEstimate.ul_capa_kbps;
}

int ProtocolMiscCurrentLinkCapacityEstimateAdapterJson::GetSecDlCapaKbps() const
{
    return mCurLinkCapaEstimate.dcnr_sec_dl_capa_kbps;
}

int ProtocolMiscCurrentLinkCapacityEstimateAdapterJson::GetSecUlCapaKbps() const
{
    return mCurLinkCapaEstimate.dcnr_sec_ul_capa_kbps;
}

/**
 * ProtocolRfConnectionStateAdapterJson
 */
ProtocolRfConnectionStateAdapterJson::ProtocolRfConnectionStateAdapterJson(const ModemData *pModemData) : ProtocolJsonIndAdapter(pModemData) { ParseJsonPayload(SIT_IND_RF_CONNECTION); }

BYTE ProtocolRfConnectionStateAdapterJson::GetState() {
    if (m_bParsingSuccess) {
        int rfCablestate = m_jsonData[ns_misc_rf_connection_ind::key_rf_cable_state].asInt();
            return rfCablestate > 0 ? 1 : 0;
    }
    return 0;
}

/**
 * ProtocolMiscRssiScanResultAdapterJson
 */
ProtocolMiscRssiScanResultAdapterJson::ProtocolMiscRssiScanResultAdapterJson(const ModemData *pModemData)
    : ProtocolJsonIndAdapter(pModemData) {
        ParseJsonPayload(SIT_IND_RSSI_SCAN);
    }

int ProtocolMiscRssiScanResultAdapterJson::GetTotalPage() {
    int totalPage = 0;
    if (m_bParsingSuccess) {
        totalPage = m_jsonData[ns_misc_rssi_scan_ind::key_total_page].asInt();
    }
    return totalPage;
}

int ProtocolMiscRssiScanResultAdapterJson::GetCurrentPage() {
    int currentPage = -1;
    if (m_bParsingSuccess) {
        currentPage = m_jsonData[ns_misc_rssi_scan_ind::key_current_page].asInt();
    }
    return currentPage;
}

int ProtocolMiscRssiScanResultAdapterJson::GetStartFrequency() {
    int startFrequency = 0;
    if (m_bParsingSuccess) {
        startFrequency = m_jsonData[ns_misc_rssi_scan_ind::key_start_freq].asInt();
    }
    return startFrequency;
}

int ProtocolMiscRssiScanResultAdapterJson::GetEndFrequency() {
    int endFrequency = 0;
    if (m_bParsingSuccess) {
        endFrequency = m_jsonData[ns_misc_rssi_scan_ind::key_end_freq].asInt();
    }
    return endFrequency;
}

int ProtocolMiscRssiScanResultAdapterJson::GetStep() {
    int step = 0;
    if (m_bParsingSuccess) {
        step = m_jsonData[ns_misc_rssi_scan_ind::key_offset].asInt();
    }
    return step;
}

int ProtocolMiscRssiScanResultAdapterJson::GetScanResultSize() {
    int len = 0;
    if (m_bParsingSuccess) {
        len = m_jsonData[ns_misc_rssi_scan_ind::key_scan_result].size();
        len = MIN(len, MAX_RSSI_SCAN_RESULT);
    }
    return len;
}

INT16* ProtocolMiscRssiScanResultAdapterJson::GetScanResult() {
    memset(m_scanResult, 0, sizeof(m_scanResult));
    if (m_bParsingSuccess) {
        int len = GetScanResultSize();
        for(int i = 0; i < len; i++) {
            m_scanResult[i] = (INT16)m_jsonData[ns_misc_rssi_scan_ind::key_scan_result][i].asInt();
        }
    }
    return m_scanResult;
}

/**
 * ProtocolSignalStrengthAdapterJson
 */
ProtocolSignalStrengthAdapterJson::ProtocolSignalStrengthAdapterJson(const ModemData *pModemData)
    : ProtocolJsonRespAdapter(pModemData)
{
    ParseJsonPayload(SIT_GET_SIGNAL_STRENGTH);
    memset(&mSignalStrength, 0, sizeof(mSignalStrength));
}

RIL_SignalStrength_V1_6& ProtocolSignalStrengthAdapterJson::GetSignalStrength(int ssVer) {
    if (m_bParsingSuccess) {
        SignalStrengthAdapterFactoryJson signalStrengthAdapterFactoryJson;
        SignalStrengthAdapterJson *signalStrengthAdapterJson = signalStrengthAdapterFactoryJson.GetSignalStrengthJsonAdapter(ssVer);
        if (signalStrengthAdapterJson != NULL) {
            mSignalStrength = signalStrengthAdapterJson->GetSignalStrength(m_jsonData);
        }
    }
    return mSignalStrength;
}

/**
 * ProtocolSignalStrengthIndAdapterJson
 */
ProtocolSignalStrengthIndAdapterJson::ProtocolSignalStrengthIndAdapterJson(const ModemData *pModemData)
    : ProtocolJsonIndAdapter(pModemData)
{
    ParseJsonPayload(SIT_IND_SIGNAL_STRENGTH);
    memset(&mSignalStrength, 0, sizeof(mSignalStrength));
}

RIL_SignalStrength_V1_6& ProtocolSignalStrengthIndAdapterJson::GetSignalStrength(int ssVer) {
    if (m_bParsingSuccess) {
        SignalStrengthAdapterFactoryJson signalStrengthAdapterFactoryJson;
        SignalStrengthAdapterJson *signalStrengthAdapterJson = signalStrengthAdapterFactoryJson.GetSignalStrengthJsonAdapter(ssVer);
        if (signalStrengthAdapterJson != NULL) {
            mSignalStrength = signalStrengthAdapterJson->GetSignalStrength(m_jsonData);
        }
    }
    return mSignalStrength;
}


/*
 * SignalStrengthAdapterJson
 */
SignalStrengthAdapterJson::SignalStrengthAdapterJson() {
    memset(&mSignalStrength, 0, sizeof(mSignalStrength));
    FillDefaultSignalStrength();
}

void SignalStrengthAdapterJson::FillDefaultSignalStrength() {
    mSignalStrength.GSM_SignalStrength.signalStrength = 99;
    mSignalStrength.GSM_SignalStrength.bitErrorRate = 99;
    mSignalStrength.GSM_SignalStrength.timingAdvance = INT_MAX;

    mSignalStrength.WCDMA_SignalStrength.signalStrength = 99;
    mSignalStrength.WCDMA_SignalStrength.bitErrorRate = 99;
    mSignalStrength.WCDMA_SignalStrength.rscp = 255;
    mSignalStrength.WCDMA_SignalStrength.ecno = 255;

    mSignalStrength.CDMA_SignalStrength.dbm = -1;
    mSignalStrength.CDMA_SignalStrength.ecio = -1;

    mSignalStrength.EVDO_SignalStrength.dbm = -1;
    mSignalStrength.EVDO_SignalStrength.ecio = -1;
    mSignalStrength.EVDO_SignalStrength.signalNoiseRatio = INT_MAX;

    mSignalStrength.LTE_SignalStrength.signalStrength = INT_MAX;
    mSignalStrength.LTE_SignalStrength.rsrp = INT_MAX;
    mSignalStrength.LTE_SignalStrength.rsrq = INT_MAX;
    mSignalStrength.LTE_SignalStrength.rssnr = INT_MAX;
    mSignalStrength.LTE_SignalStrength.cqiTableIndex = INT_MAX;
    mSignalStrength.LTE_SignalStrength.cqi = INT_MAX;
    mSignalStrength.LTE_SignalStrength.timingAdvance = INT_MAX;

    mSignalStrength.TD_SCDMA_SignalStrength.signalStrength = 99;
    mSignalStrength.TD_SCDMA_SignalStrength.bitErrorRate = 99;
    mSignalStrength.TD_SCDMA_SignalStrength.rscp = 255;

    mSignalStrength.NR_SignalStrength.ssRsrp = INT_MAX;
    mSignalStrength.NR_SignalStrength.ssRsrq = INT_MAX;
    mSignalStrength.NR_SignalStrength.ssSinr = INT_MAX;
    mSignalStrength.NR_SignalStrength.csiRsrp = INT_MAX;
    mSignalStrength.NR_SignalStrength.csiRsrq = INT_MAX;
    mSignalStrength.NR_SignalStrength.csiSinr = INT_MAX;
    mSignalStrength.NR_SignalStrength.csiCqiTableIndex = INT_MAX;
    mSignalStrength.NR_SignalStrength.csiCqiReportLen = 0;
}

/*
 * SignalStrengthParserV4Json
 */
RIL_SignalStrength_V1_6& SignalStrengthParserV4Json::GetSignalStrength(Json::Value &jsonSSData) {

    FillSignalStrength(jsonSSData);
    return mSignalStrength;
}

void SignalStrengthParserV4Json::FillSignalStrength(Json::Value &jsonSSData) {

    int mask = jsonSSData[ns_misc_get_signal_strength_rsp::key_valid_ss_rat_bitmask].asInt();

    if (mask & SIT_MISC_SIG_RAT_SIG_GSM) {
        FillGsmSignalStrength(jsonSSData[ns_misc_get_signal_strength_rsp::key_gsm_signal_strength]);
    }

    if (mask & SIT_MISC_SIG_RAT_SIG_WCDMA) {
        FillWcdmaSignalStrength(jsonSSData[ns_misc_get_signal_strength_rsp::key_wcdma_signal_strength]);
    }

    if (mask & SIT_MISC_SIG_RAT_SIG_CDMA_V1_4) {
        FillCdmaSignalStrength(jsonSSData[ns_misc_get_signal_strength_rsp::key_cdma_signal_strength]);
    }

    if (mask & SIT_MISC_SIG_RAT_SIG_EVDO_V1_4) {
        FillEvdoSignalStrength(jsonSSData[ns_misc_get_signal_strength_rsp::key_evdo_signal_strength]);
    }

    if (mask & SIT_MISC_SIG_RAT_SIG_LTE_V1_4) {
        FillLteSignalStrength(jsonSSData[ns_misc_get_signal_strength_rsp::key_lte_signal_strength]);
    }

    if (mask & SIT_MISC_SIG_RAT_SIG_TD_SCDMA_V1_4) {
        FillTdscdmaSignalStrength(jsonSSData[ns_misc_get_signal_strength_rsp::key_tdscdma_signal_strength]);
    }

    if (mask & SIT_MISC_SIG_RAT_SIG_NR_V1_4) {
        FillNrSignalStrength(jsonSSData[ns_misc_get_signal_strength_rsp::key_nr_signal_strength]);
    }
}

void SignalStrengthParserV4Json::FillGsmSignalStrength(Json::Value &jsonGsmSS) {
    mSignalStrength.GSM_SignalStrength.signalStrength = jsonGsmSS[ns_misc_gsm_signal_strength::key_signal_strength].asInt();
    mSignalStrength.GSM_SignalStrength.bitErrorRate = jsonGsmSS[ns_misc_gsm_signal_strength::key_ber].asInt();
    mSignalStrength.GSM_SignalStrength.timingAdvance = (int)jsonGsmSS[ns_misc_gsm_signal_strength::key_ta].asUInt();
}

void SignalStrengthParserV4Json::FillWcdmaSignalStrength(Json::Value &jsonWcdmaSS) {
    mSignalStrength.WCDMA_SignalStrength.signalStrength = jsonWcdmaSS[ns_misc_wcdma_signal_strength::key_signal_strength].asInt();
    mSignalStrength.WCDMA_SignalStrength.bitErrorRate = jsonWcdmaSS[ns_misc_wcdma_signal_strength::key_ber].asInt();
    mSignalStrength.WCDMA_SignalStrength.rscp = jsonWcdmaSS[ns_misc_wcdma_signal_strength::key_rscp].asInt();
    mSignalStrength.WCDMA_SignalStrength.ecno = jsonWcdmaSS[ns_misc_wcdma_signal_strength::key_ecno].asInt();
}

void SignalStrengthParserV4Json::FillCdmaSignalStrength(Json::Value &jsonCdmaSS) {
    mSignalStrength.CDMA_SignalStrength.dbm = (int)jsonCdmaSS[ns_misc_cdma_signal_strength::key_dbm].asUInt();
    mSignalStrength.CDMA_SignalStrength.ecio = (int)jsonCdmaSS[ns_misc_cdma_signal_strength::key_ecio].asUInt();
}

void SignalStrengthParserV4Json::FillEvdoSignalStrength(Json::Value &jsonEvdoSS) {
    mSignalStrength.EVDO_SignalStrength.dbm = (int)jsonEvdoSS[ns_misc_evdo_signal_strength::key_dbm].asUInt();
    mSignalStrength.EVDO_SignalStrength.ecio = (int)jsonEvdoSS[ns_misc_evdo_signal_strength::key_ecio].asUInt();
    mSignalStrength.EVDO_SignalStrength.signalNoiseRatio = jsonEvdoSS[ns_misc_evdo_signal_strength::key_snr].asInt();
}

void SignalStrengthParserV4Json::FillLteSignalStrength(Json::Value &jsonLteSS) {
    mSignalStrength.LTE_SignalStrength.signalStrength = jsonLteSS[ns_misc_lte_signal_strength::key_signal_strength].asInt();
    mSignalStrength.LTE_SignalStrength.rsrp = jsonLteSS[ns_misc_lte_signal_strength::key_rsrp].asInt();
    mSignalStrength.LTE_SignalStrength.rsrq = jsonLteSS[ns_misc_lte_signal_strength::key_rsrq].asInt();
    mSignalStrength.LTE_SignalStrength.rssnr = jsonLteSS[ns_misc_lte_signal_strength::key_rssnr].asInt();
    mSignalStrength.LTE_SignalStrength.cqiTableIndex = jsonLteSS[ns_misc_lte_signal_strength::key_cqi_table_index].asInt();
    mSignalStrength.LTE_SignalStrength.cqi = jsonLteSS[ns_misc_lte_signal_strength::key_cqi].asInt();
    mSignalStrength.LTE_SignalStrength.timingAdvance = jsonLteSS[ns_misc_lte_signal_strength::key_ta].asInt();
}

void SignalStrengthParserV4Json::FillTdscdmaSignalStrength(Json::Value &jsonTdscdmaSS) {
    mSignalStrength.TD_SCDMA_SignalStrength.signalStrength = jsonTdscdmaSS[ns_misc_tdscdma_signal_strength::key_signal_strength].asInt();
    mSignalStrength.TD_SCDMA_SignalStrength.bitErrorRate = jsonTdscdmaSS[ns_misc_tdscdma_signal_strength::key_ber].asInt();
    mSignalStrength.TD_SCDMA_SignalStrength.rscp = jsonTdscdmaSS[ns_misc_tdscdma_signal_strength::key_rscp].asInt();
}

void SignalStrengthParserV4Json::FillNrSignalStrength(Json::Value &jsonNrSS) {
    mSignalStrength.NR_SignalStrength.ssRsrp = jsonNrSS[ns_misc_nr_signal_strength::key_ss_rsrp].asInt();
    mSignalStrength.NR_SignalStrength.ssRsrq = jsonNrSS[ns_misc_nr_signal_strength::key_ss_rsrq].asInt();
    mSignalStrength.NR_SignalStrength.ssSinr = jsonNrSS[ns_misc_nr_signal_strength::key_ss_sinr].asInt();
    mSignalStrength.NR_SignalStrength.csiRsrp = jsonNrSS[ns_misc_nr_signal_strength::key_csi_rsrp].asInt();
    mSignalStrength.NR_SignalStrength.csiRsrq = jsonNrSS[ns_misc_nr_signal_strength::key_csi_rsrq].asInt();
    mSignalStrength.NR_SignalStrength.csiSinr = jsonNrSS[ns_misc_nr_signal_strength::key_csi_sinr].asInt();
    mSignalStrength.NR_SignalStrength.csiCqiTableIndex = jsonNrSS[ns_misc_nr_signal_strength::key_cqi_table_index].asInt();

    // CQI
    int val = jsonNrSS[ns_misc_nr_signal_strength::key_wb_cqi].asInt();
    if (val != INT_MAX) {
        mSignalStrength.NR_SignalStrength.csiCqiReportLen = 1;
        mSignalStrength.NR_SignalStrength.csiCqiReport[0] = val;
    } else {
        unsigned int sbCqiLen = jsonNrSS[ns_misc_nr_signal_strength::key_sb_cqi_list].size();
        ProtocolUtils::LimitLengthToMax<unsigned int>(RIL_LOG_CAT_MISC, __CLASS_FUNC__, sbCqiLen, SIT_MAX_NR_SUBBAND_INDEX);
        mSignalStrength.NR_SignalStrength.csiCqiReportLen = sbCqiLen;
        for (unsigned int i = 0; i < sbCqiLen; i++) {
            mSignalStrength.NR_SignalStrength.csiCqiReport[i] = jsonNrSS[ns_misc_nr_signal_strength::key_sb_cqi_list][i].asUInt();
        }
    }
}

/*
 * SignalStrengthAdapterFactoryJson
 */
SignalStrengthAdapterJson *SignalStrengthAdapterFactoryJson::GetSignalStrengthJsonAdapter(int ssVer)
{
    RilLogI("SignalStrengthAdapterFactoryJson::GetSignalStrengthJsonAdapter ssVer=%d", ssVer);
    switch (ssVer) {
        case SIGNAL_STRENGTH_PROTOCOL_V4:
            return &mSignalStrengthV4;
    }
    return &mSignalStrengthV4;;
}

/**
 * ProtocolMiscSetDeviceInfoAdapterJson
 */
ProtocolMiscSetDeviceInfoAdapterJson::ProtocolMiscSetDeviceInfoAdapterJson(const ModemData *pModemData)
    : ProtocolJsonRespAdapter(pModemData)
{
    mDeviceInfo = NULL;
    ParseJsonPayload(SIT_SET_DEVICE_INFO);
    Init();
}

ProtocolMiscSetDeviceInfoAdapterJson::~ProtocolMiscSetDeviceInfoAdapterJson() {
    if (mDeviceInfo != NULL) {
        delete[] mDeviceInfo;
        mDeviceInfo = NULL;
    }
}

void ProtocolMiscSetDeviceInfoAdapterJson::Init() {
    if (m_bParsingSuccess) {
        string sitVerionInfo = m_jsonData[ns_misc_set_device_info_rsp::key_sit_version_info].asString();
        size_t size = sitVerionInfo.length() + 1;
        if (size > 0) {
            mDeviceInfo = new char[size] {};
            if (mDeviceInfo != NULL) {
                strlcpy(mDeviceInfo, sitVerionInfo.c_str(), size);
                char *key = strtok(mDeviceInfo, "=,");
                char *value = NULL;
                while (key != NULL) {
                    value = strtok(NULL, "=,");
                    mRilProp.Put(key, value);
                    key = strtok(NULL, "=,");
                }
            }
        }
    }
}

int ProtocolMiscSetDeviceInfoAdapterJson::GetVersionInfo(const char *input)
{
    return mRilProp.GetInt(input, DEFAULT_CELL_INFO_VER);
}

/**
 * ProtocolMiscIMEIAdapterJson
 */
ProtocolMiscIMEIAdapterJson::ProtocolMiscIMEIAdapterJson(const ModemData *pModemData)
    : ProtocolJsonRespAdapter(pModemData) {
    Init();
}

ProtocolMiscIMEIAdapterJson::~ProtocolMiscIMEIAdapterJson() {
    if (m_pImei) {
        delete[] m_pImei;
        m_pImei = NULL;
    }
}

int ProtocolMiscIMEIAdapterJson::GetIMEILen() const {
    return m_nImeiLen;
}

const BYTE *ProtocolMiscIMEIAdapterJson::GetIMEI() const {
    return m_pImei;
}

// For ProtocolRespAdatper
UINT ProtocolMiscIMEIAdapterJson::GetErrorCode() const {
    return ProtocolRespAdapter::GetErrorCode();
}

void ProtocolMiscIMEIAdapterJson::Init() {
    m_nImeiLen = 0;
    m_pImei = NULL;
    ParseJsonPayload(SIT_GET_IMEI);
    SetIMEI();
}

void ProtocolMiscIMEIAdapterJson::SetIMEI() {
    if (!m_bParsingSuccess) return;
    string imei = m_jsonData[ns_misc_id_get_imei_rsp::key_imei].asString();
    ProtocolUtils::LimitStringToMaxLength(RIL_LOG_CAT_MISC, __CLASS_FUNC__, imei, MAX_IMEI_LEN);
    m_nImeiLen = imei.length();
    m_pImei = new BYTE[m_nImeiLen + 1];
    memset(m_pImei, 0, m_nImeiLen + 1);
    memcpy(m_pImei, imei.c_str(), m_nImeiLen);
}

/**
 * ProtocolMiscIMEISVAdapterJson
 */
ProtocolMiscIMEISVAdapterJson::ProtocolMiscIMEISVAdapterJson(const ModemData *pModemData)
    : ProtocolJsonRespAdapter(pModemData) {
    Init();
}

ProtocolMiscIMEISVAdapterJson::~ProtocolMiscIMEISVAdapterJson() {
    if (m_pImeisv) {
        delete[] m_pImeisv;
        m_pImeisv = NULL;
    }
}

int ProtocolMiscIMEISVAdapterJson::GetIMEISVLen() const {
    return m_nImeisvLen;
}

const BYTE *ProtocolMiscIMEISVAdapterJson::GetIMEISV() const {
    return m_pImeisv;
}

// For ProtocolRespAdatper
UINT ProtocolMiscIMEISVAdapterJson::GetErrorCode() const {
    return ProtocolRespAdapter::GetErrorCode();
}

void ProtocolMiscIMEISVAdapterJson::Init() {
    m_nImeisvLen = 0;
    m_pImeisv = NULL;
    ParseJsonPayload(SIT_GET_IMEISV);
    SetIMEISV();
}

void ProtocolMiscIMEISVAdapterJson::SetIMEISV() {
    if (!m_bParsingSuccess) return;
    string imeisv = m_jsonData[ns_misc_id_get_imeisv_rsp::key_imeisv].asString();
    ProtocolUtils::LimitStringToMaxLength(RIL_LOG_CAT_MISC, __CLASS_FUNC__, imeisv, MAX_IMEISV_LEN);
    m_nImeisvLen = imeisv.length();
    m_pImeisv = new BYTE[m_nImeisvLen + 1];
    memset(m_pImeisv, 0, m_nImeisvLen + 1);
    memcpy(m_pImeisv, imeisv.c_str(), m_nImeisvLen);
}

/**
 * ProtocolMiscDeviceIDAdapterJson
 */
ProtocolMiscDeviceIDAdapterJson::ProtocolMiscDeviceIDAdapterJson(const ModemData *pModemData)
    : ProtocolJsonRespAdapter(pModemData) {
    Init();
}

void ProtocolMiscDeviceIDAdapterJson::Init() {
    m_nImeiLen = m_nImeisvLen = m_nEsnLen = m_nMeidLen = 0;
    m_pImei = m_pImeisv = m_pEsn = m_pMeid = NULL;
    ParseJsonPayload(SIT_GET_DEVICE_ID);
    SetDeviceId();
}

void ProtocolMiscDeviceIDAdapterJson::SetDeviceId() {
    if (!m_bParsingSuccess) return;
    // imei
    string imei = m_jsonData[ns_misc_id_get_deviceid_rsp::key_imei].asString();
    ProtocolUtils::LimitStringToMaxLength(RIL_LOG_CAT_MISC, __CLASS_FUNC__, imei, MAX_IMEI_LEN);
    m_nImeiLen = imei.length();
    m_pImei = new BYTE[m_nImeiLen + 1];
    memset(m_pImei, 0, m_nImeiLen + 1);
    memcpy(m_pImei, imei.c_str(), m_nImeiLen);

    // imeisv
    string imeisv = m_jsonData[ns_misc_id_get_deviceid_rsp::key_imeisv].asString();
    ProtocolUtils::LimitStringToMaxLength(RIL_LOG_CAT_MISC, __CLASS_FUNC__, imeisv, MAX_IMEISV_LEN);
    m_nImeisvLen = imeisv.length();
    m_pImeisv = new BYTE[m_nImeisvLen + 1];
    memset(m_pImeisv, 0, m_nImeisvLen + 1);
    memcpy(m_pImeisv, imeisv.c_str(), m_nImeisvLen);

    // esn
    string esn = m_jsonData[ns_misc_id_get_deviceid_rsp::key_esn].asString();
    ProtocolUtils::LimitStringToMaxLength(RIL_LOG_CAT_MISC, __CLASS_FUNC__, esn, MAX_ESN_LEN);
    m_nEsnLen = esn.length();
    m_pEsn = new BYTE[m_nEsnLen + 1];
    memset(m_pEsn, 0, m_nEsnLen + 1);
    memcpy(m_pEsn, esn.c_str(), m_nEsnLen);

    // meid
    string meid = m_jsonData[ns_misc_id_get_deviceid_rsp::key_meid].asString();
    ProtocolUtils::LimitStringToMaxLength(RIL_LOG_CAT_MISC, __CLASS_FUNC__, meid, MAX_MEID_LEN);
    m_nMeidLen = meid.length();
    m_pMeid = new BYTE[m_nMeidLen + 1];
    memset(m_pMeid, 0, m_nMeidLen + 1);
    memcpy(m_pMeid, meid.c_str(), m_nMeidLen);
}

ProtocolMiscDeviceIDAdapterJson::~ProtocolMiscDeviceIDAdapterJson() {
    if (m_pImei) {
        delete[] m_pImei;
        m_pImei = NULL;
    }
    if (m_pImeisv) {
        delete[] m_pImeisv;
        m_pImeisv = NULL;
    }
    if (m_pEsn) {
        delete[] m_pEsn;
        m_pEsn = NULL;
    }
    if (m_pMeid) {
        delete[] m_pMeid;
        m_pMeid = NULL;
    }
}

/**
 * ProtocolMiscGetStackStatusAdapterJson
 */
int ProtocolMiscGetStackStatusAdapterJson::GetMode() {
    if (!m_bParsingSuccess) return SIT_PWR_STATCK_DISABLE;
    int status = m_jsonData[ns_misc_pwr_get_stack_status_rsp::key_status].asInt();
    return (status == 0x00) ? SIT_PWR_STATCK_DISABLE : SIT_PWR_STATCK_ENABLE;
}

/**
 * ProtocolMiscPhoneResetAdapterJson
 */
BYTE ProtocolMiscPhoneResetAdapterJson::GetResetType() {
    if (!m_bParsingSuccess) return SIT_PWR_RESET_TYPE_PHONE_ONLY;
    return (BYTE) m_jsonData[ns_pwr_phone_reset_ind::key_reset_type].asInt();
}

BYTE ProtocolMiscPhoneResetAdapterJson::GetResetCause() {
    // never sent to f/w, hence removed in JSON
    return 0;
}

/**
 * ProtocolMiscSetApSystemTimeAdapterJson
 */
ProtocolMiscSetApSystemTimeAdapterJson::ProtocolMiscSetApSystemTimeAdapterJson(const ModemData *pModemData) : ProtocolJsonRespAdapter(pModemData) { ParseJsonPayload(SIT_SET_AP_SYSTEM_TIME); }

int ProtocolMiscSetApSystemTimeAdapterJson::GetResult() const
{
    if (m_bParsingSuccess) {
        return m_jsonData[ns_misc_set_ap_system_time_rsp::key_result].asInt();
    }
    return 0;
}

/**
 * ProtocolOemModemInfoAdapterJson
 */
ProtocolOemModemInfoAdapterJson::ProtocolOemModemInfoAdapterJson(const ModemData *pModemData)
    : ProtocolJsonRespAdapter(pModemData) {
    Init();
}

void ProtocolOemModemInfoAdapterJson::Init() {
    ProtocolJsonRespAdapter::ParseJsonPayload(SIT_OEM_NW_INFO);
    m_commandType = -1;
    m_size = 0;
    if(m_bParsingSuccess)
    {
        m_data = m_jsonData[ns_oem_nw_info_rsp::key_buffer].asString();
        m_commandType = m_jsonData[ns_oem_nw_info_rsp::key_command_id].asInt();
        m_size = m_data.length();
    }
}

int ProtocolOemModemInfoAdapterJson::GetCommandType() const {
    return m_commandType;
}

unsigned int ProtocolOemModemInfoAdapterJson::GetSize() const {
    return m_size;
}

void *ProtocolOemModemInfoAdapterJson::GetData() {
    return (void *)m_data.c_str();
}

/**
 * ProtocolOemSwitchModemFunctionAdapterJson
 */
ProtocolOemSwitchModemFunctionAdapterJson::ProtocolOemSwitchModemFunctionAdapterJson(const ModemData *pModemData)
    : ProtocolJsonRespAdapter(pModemData) {
        ParseJsonPayload(SIT_OEM_SET_FUNC_SWITCH_REQ);
}

BYTE ProtocolOemSwitchModemFunctionAdapterJson::GetResult() const{
    if(m_bParsingSuccess) {
        return m_jsonData[ns_oem_set_fun_swtich_rsp::key_result].asInt();
    }
    return -1;
}

/**
 * ProtocolOemGetCqiInfoAdapterJson
 */
ProtocolOemGetCqiInfoAdapterJson::ProtocolOemGetCqiInfoAdapterJson(const ModemData *pModemData)
    : ProtocolJsonRespAdapter(pModemData) {
    ParseJsonPayload(SIT_OEM_GET_CQI_INFO);
}

INT16 ProtocolOemGetCqiInfoAdapterJson::GetCqiType() const {
    if(m_bParsingSuccess) {
        return m_jsonData[ns_oem_get_cqi_info_rsp::key_type].asInt();
    }
    return -1;
}

INT16 ProtocolOemGetCqiInfoAdapterJson::GetCqiInfo0() const {
    if(m_bParsingSuccess) {
        return m_jsonData[ns_oem_get_cqi_info_rsp::key_cqi_info0].asInt();
    }
    return -1;
}

INT16 ProtocolOemGetCqiInfoAdapterJson::GetCqiInfo1() const {
    if(m_bParsingSuccess) {
        return m_jsonData[ns_oem_get_cqi_info_rsp::key_cqi_info1].asInt();
    }
    return -1;
}
INT16 ProtocolOemGetCqiInfoAdapterJson::GetRi() const {
    if(m_bParsingSuccess) {
        return m_jsonData[ns_oem_get_cqi_info_rsp::key_ri].asInt();
    }
    return -1;
}

/**
 * ProtocolMiscSetTcsFciAdapterJson
 */
ProtocolMiscSetTcsFciAdapterJson::ProtocolMiscSetTcsFciAdapterJson(const ModemData *pModemData)
    : ProtocolJsonRespAdapter(pModemData) {
    ParseJsonPayload(SIT_OEM_SET_TCS_FCI_REQ);
}

int ProtocolMiscSetTcsFciAdapterJson::GetResult() const {
    if(m_bParsingSuccess) {
        return m_jsonData[ns_oem_set_tcs_fci_rsp::key_result].asInt();
    }
    return -1;
}

/**
 * ProtocolMiscGetTcsFciAdapterJson
 */
ProtocolMiscGetTcsFciAdapterJson::ProtocolMiscGetTcsFciAdapterJson(const ModemData *pModemData)
    : ProtocolJsonRespAdapter(pModemData) {
    Init();
}

void ProtocolMiscGetTcsFciAdapterJson::Init() {
    ParseJsonPayload(SIT_OEM_GET_TCS_FCI_INFO);
    if(m_bParsingSuccess) {
        m_fci = m_jsonData[ns_oem_get_tcs_fci_rsp::key_fci].asString();
    }
}

const char * ProtocolMiscGetTcsFciAdapterJson::GetFci() const {
    return m_fci.c_str();
}

/**
 * ProtocolOemModemInfoIndAdapterJson
 */
ProtocolOemModemInfoIndAdapterJson::ProtocolOemModemInfoIndAdapterJson(const ModemData *pModemData)
    : ProtocolJsonIndAdapter(pModemData) {
    Init();
}

void ProtocolOemModemInfoIndAdapterJson::Init() {
    ParseJsonPayload(SIT_OEM_IND_NW_INFO);
    m_commandType = -1;
    m_size = 0;
    if(m_bParsingSuccess)
    {
        m_data = m_jsonData[ns_oem_nw_info_rsp::key_buffer].asString();
        m_commandType = m_jsonData[ns_oem_nw_info_rsp::key_command_id].asInt();
        m_size = m_data.length();
    }
}

int ProtocolOemModemInfoIndAdapterJson::GetCommandType() const {
    return m_commandType;
}

unsigned int ProtocolOemModemInfoIndAdapterJson::GetSize() const {
    return m_size;
}

void *ProtocolOemModemInfoIndAdapterJson::GetData() {
    return (void *)m_data.c_str();
}

/**
 * ProtocolMiscBaseStationIndAdapterJson
 */
ProtocolMiscBaseStationIndAdapterJson::ProtocolMiscBaseStationIndAdapterJson(const ModemData *pModemData)
    : ProtocolJsonIndAdapter(pModemData) {
    Init();
}

ProtocolMiscBaseStationIndAdapterJson::~ProtocolMiscBaseStationIndAdapterJson() {
    if(m_legacyData) {
        delete[] m_legacyData;
        m_legacyData = NULL;
    }
}

const char *ProtocolMiscBaseStationIndAdapterJson::GetParameter() const {
#ifdef OEM_JSON_APPFW_CONVERTION
    return m_legacyData;
#else
    return ProtocolIndAdapter::GetParameter();
#endif
}

UINT ProtocolMiscBaseStationIndAdapterJson::GetParameterLength() const {
#ifdef OEM_JSON_APPFW_CONVERTION
    return m_dataLength;
#else
    return ProtocolIndAdapter::GetParameterLength();
#endif
}

/**
* Parse Json data and convert is to SIT format
*/
#ifdef OEM_JSON_APPFW_CONVERTION
static void writeBaseStationInfo(DataWriter &writer, const Json::Value &jsonBsList, int i) {
    //mcc
    string mcc = jsonBsList[i][ns_oem_activity_base_station_info::key_mcc].asString();
    int len = mcc.length();
    unsigned char mcc_array[3] = {'#', '#', '#'};
    if (len > 3) {
        memcpy(mcc_array, mcc.c_str(), 3);
    } else {
        memcpy(mcc_array, mcc.c_str(), len);
    }
    writer.writeBytes(mcc_array, 3);

    //mnc
    string mnc = jsonBsList[i][ns_oem_activity_base_station_info::key_mnc].asString();
    len = mnc.length();
    unsigned char mnc_array[3] = {'#', '#', '#'};
    if (len > 3) {
        memcpy(mnc_array, mnc.c_str(), 3);
    } else {
        memcpy(mnc_array, mnc.c_str(), len);
    }
    writer.writeBytes(mnc_array, 3);

    //rat
    UINT8 rat = jsonBsList[i][ns_oem_activity_base_station_info::key_rat].asInt();
    writer.writeByte(rat);

    //is NSA
    UINT8 is_nsa = jsonBsList[i][ns_oem_activity_base_station_info::key_is_nsa].asInt();
    writer.writeByte(is_nsa);

    //band
    UINT16 band = jsonBsList[i][ns_oem_activity_base_station_info::key_band].asInt();
    writer.writeShort(band);

    //signal level
    UINT8 signal_level = jsonBsList[i][ns_oem_activity_base_station_info::key_signal_level].asInt();
    writer.writeByte(signal_level);

    //count
    int count = jsonBsList[i][ns_oem_activity_base_station_info::key_count].asInt();
    writer.writeInt(count);
}

static int writeBaseStationInfoList(char *(&legacyData), const Json::Value &jsonBsList) {
    DataWriter writer;
    int bs_info_num = jsonBsList.size();
    writer.writeByte((UINT8)bs_info_num);

    for (int i = 0; i < bs_info_num; i++) {
        writeBaseStationInfo(writer, jsonBsList, i);
    }
    int dataLength = writer.getSize();
    try {
        legacyData = new char[dataLength];
        memcpy(legacyData, writer.getBytes(), dataLength);
    } catch (std::bad_alloc) {
        legacyData = NULL;
        dataLength = 0;
    }

    return dataLength;
}
#endif  //OEM_JSON_APPFW_CONVERTION

void ProtocolMiscBaseStationIndAdapterJson::Init() {
    m_legacyData = NULL;
    m_dataLength = 0;

    ParseJsonPayload(SIT_OEM_IND_BASESTATION_INFO);

#ifdef OEM_JSON_APPFW_CONVERTION
    if(m_bParsingSuccess) {
        Json::Value jsonBsList = m_jsonData[ns_oem_base_station_info_ind::key_base_station_info_list];
        m_dataLength = writeBaseStationInfoList(m_legacyData, jsonBsList);
    }
#endif //OEM_JSON_APPFW_CONVERTION
}

/**
 * ProtocolMiscGetCdmaCapabilityInfoAdapterJson
 */
ProtocolMiscGetCdmaCapabilityInfoAdapterJson::ProtocolMiscGetCdmaCapabilityInfoAdapterJson(const ModemData *pModemData)
    : ProtocolJsonRespAdapter(pModemData) {
    ParseJsonPayload(SIT_OEM_GET_CDMA_CAPABILITY_INFO);
}

BYTE ProtocolMiscGetCdmaCapabilityInfoAdapterJson::GetCapability() const {
    if(m_bParsingSuccess) {
        return m_jsonData[ns_oem_get_cdma_capability_info_rsp::key_result].asInt();
    }
    return 0;
}

/**
 * ProtocolMiscSetActiveVsimAdapterJson
 */
ProtocolMiscSetActiveVsimAdapterJson::ProtocolMiscSetActiveVsimAdapterJson(const ModemData *pModemData)
    : ProtocolJsonRespAdapter(pModemData) {
    ParseJsonPayload(SIT_OEM_SET_ACTIVATE_VSIM);
}

int ProtocolMiscSetActiveVsimAdapterJson::GetResult() const {
    if(m_bParsingSuccess) {
        return m_jsonData[ns_oem_set_activate_factory_vsim_rsp::key_result].asInt();
    }
    return 0;
}

/**
 * ProtocolMiscSetForbidNrCellAdapterJson
 */
ProtocolMiscSetForbidNrCellAdapterJson::ProtocolMiscSetForbidNrCellAdapterJson(const ModemData *pModemData)
    : ProtocolJsonRespAdapter(pModemData) {
    ParseJsonPayload(SIT_OEM_SET_FORBID_NR_CELL);
}

int ProtocolMiscSetForbidNrCellAdapterJson::GetResult() const {
    if(m_bParsingSuccess) {
        return m_jsonData[ns_oem_set_forbid_nr_rsp::key_result].asInt();
    }
    return 0;
}

/**
 * ProtocolMiscEndcCapabilityIndAdapterJson
 */
ProtocolMiscEndcCapabilityIndAdapterJson::ProtocolMiscEndcCapabilityIndAdapterJson(const ModemData *pModemData)
    : ProtocolJsonIndAdapter(pModemData) {
    ParseJsonPayload(SIT_OEM_IND_ENDC_CAPABILITY);
}

int ProtocolMiscEndcCapabilityIndAdapterJson::GetCapability() const {
    int ret = 0;
    if (m_bParsingSuccess) {
        ret = m_jsonData[ns_oem_endc_capability_ind::key_endc_capa].asInt();
    }
    return ret;
}

int ProtocolMiscEndcCapabilityIndAdapterJson::GetCause() const {
    int ret = 0;
    if(m_bParsingSuccess) {
        ret = m_jsonData[ns_oem_endc_capability_ind::key_cause].asInt();
    }
    return ret;
}

/**
 * ProtocolMiscGetModemActivityInfoAdapterJson
 */
ProtocolMiscGetModemActivityInfoAdapterJson::ProtocolMiscGetModemActivityInfoAdapterJson(const ModemData *pModemData)
    : ProtocolJsonRespAdapter(pModemData) {
    Init();
}

ProtocolMiscGetModemActivityInfoAdapterJson::~ProtocolMiscGetModemActivityInfoAdapterJson() {
    if(m_legacyData) {
        delete [] m_legacyData;
        m_legacyData = NULL;
    }
}

UINT ProtocolMiscGetModemActivityInfoAdapterJson::GetErrorCode() const {
    return ProtocolJsonRespAdapter::GetErrorCode();
}

const char * ProtocolMiscGetModemActivityInfoAdapterJson::GetParameter() const {
#ifdef OEM_JSON_APPFW_CONVERTION
    return m_legacyData;
#else
    return ProtocolJsonRespAdapter::GetParameter();
#endif
}

UINT ProtocolMiscGetModemActivityInfoAdapterJson::GetParameterLength() const {
#ifdef OEM_JSON_APPFW_CONVERTION
    return m_dataLength;
#else
    return ProtocolJsonRespAdapter::GetParameterLength();
#endif
}

void ProtocolMiscGetModemActivityInfoAdapterJson::Init() {
    m_legacyData = NULL;
    m_dataLength = 0;
    ParseJsonPayload(SIT_OEM_GET_ACTIVITY_INFO);

#ifdef OEM_JSON_APPFW_CONVERTION
    if(m_bParsingSuccess) {
        DataWriter writer;

        //handover count
        int handoverCount = m_jsonData[ns_oem_get_activity_info_rsp::key_handover_count].asInt();
        writer.writeInt(handoverCount);

        //reselection count
        int reSelCount = m_jsonData[ns_oem_get_activity_info_rsp::key_reselection_count].asInt();
        writer.writeInt(reSelCount);

        //cdrc config time
        int cdrxCnfTime = m_jsonData[ns_oem_get_activity_info_rsp::key_cdrx_config_time].asInt();
        writer.writeInt(cdrxCnfTime);

        //rrc connected time
        int rrcConnTime = m_jsonData[ns_oem_get_activity_info_rsp::key_rrc_connected_time].asInt();
        writer.writeInt(rrcConnTime);

        //rx_info_list
        Json::Value jsonRxInfoList = m_jsonData[ns_oem_get_activity_info_rsp::key_rx_info_list];
        int rxInfoSize = jsonRxInfoList.size();
        for(int i = 0; i < SIT_NUM_OF_RECIEVER; i++) {
            if(i < rxInfoSize) {
                //Rx NUM
                UINT8 rxNum = jsonRxInfoList[i][ns_oem_activity_rx_info::key_rx].asInt();
                writer.writeByte(rxNum);

                //Count
                int rxCnt = jsonRxInfoList[i][ns_oem_activity_rx_info::key_count].asInt();
                writer.writeInt(rxCnt);
            } else {
                //no more items. put default value
                writer.writeByte(0);
                writer.writeInt(0);
            }
        }

        //base station list
        Json::Value jsonBsList = m_jsonData[ns_oem_get_activity_info_rsp::key_base_station_info_list];
        m_dataLength = writeBaseStationInfoList(m_legacyData, jsonBsList);
    }
#endif //OEM_JSON_APPFW_CONVERTION
}

/**
 * ProtocolMiscDropMtCallInfoIndAdapterJson
 */
ProtocolMiscDropMtCallInfoIndAdapterJson::ProtocolMiscDropMtCallInfoIndAdapterJson(const ModemData *pModemData)
    : ProtocolJsonIndAdapter(pModemData) {
    Init();
}

ProtocolMiscDropMtCallInfoIndAdapterJson::~ProtocolMiscDropMtCallInfoIndAdapterJson() {
    if(m_legacyData) {
        delete [] m_legacyData;
        m_legacyData = NULL;
    }
}

const char *ProtocolMiscDropMtCallInfoIndAdapterJson::GetParameter() const {
#ifdef OEM_JSON_APPFW_CONVERTION
    return m_legacyData;
#else
    return ProtocolIndAdapter::GetParameter();
#endif
}

UINT ProtocolMiscDropMtCallInfoIndAdapterJson::GetParameterLength() const {
#ifdef OEM_JSON_APPFW_CONVERTION
    return m_dataLength;
#else
    return ProtocolIndAdapter::GetParameterLength();
#endif
}

//TODO. Document is not clearly
void ProtocolMiscDropMtCallInfoIndAdapterJson::Init() {
    m_legacyData = NULL;
    m_dataLength = 0;
    ParseJsonPayload(SIT_OEM_IND_DROPPED_MT_CALL_INFO);

#ifdef OEM_JSON_APPFW_CONVERTION
    if(m_bParsingSuccess) {
        DataWriter writer;

        //call id
        UINT8 callId = m_jsonData[ns_oem_dropped_mt_call_info_ind::key_call_id].asInt();
        writer.writeByte(callId);

        //call end before ring reason
        UINT8 callEndBeforeRingReason = m_jsonData[ns_oem_dropped_mt_call_info_ind::key_call_end_before_ring_reason].asInt();
        writer.writeByte(callEndBeforeRingReason);

        //call domain
        UINT8 callDomain = m_jsonData[ns_oem_dropped_mt_call_info_ind::key_call_domain].asInt();
        writer.writeByte(callDomain);

        //call type
        UINT16 callType = m_jsonData[ns_oem_dropped_mt_call_info_ind::key_call_type].asInt();
        writer.writeShort(callType);

        //call error code
        UINT16 callErrCode = m_jsonData[ns_oem_dropped_mt_call_info_ind::key_call_error_code].asInt();
        writer.writeShort(callErrCode);

        //call error reason text
        string callErrText = m_jsonData[ns_oem_dropped_mt_call_info_ind::key_call_error_reason].asString();
        UINT8 callErrTextLen = callErrText.length();

        //call error reason text length
        writer.writeByte(callErrTextLen);

        //call drop time
        ULONG callDropTime = m_jsonData[ns_oem_dropped_mt_call_info_ind::key_call_drop_time].asLargestUInt();
        writer.writeInt64(callDropTime);

        //call error reason text
        UINT8 arrayErrText[100] = {0, };
        memcpy(arrayErrText, callErrText.data(), callErrTextLen);
        writer.writeBytes(arrayErrText, 100);

        //call party number
        string callPartyNumber = m_jsonData[ns_oem_dropped_mt_call_info_ind::key_calling_party_number].asString();
        int callPartyNumberLen = callPartyNumber.length();
        if(callPartyNumberLen > MAX_CALL_PARTY_NUM_LEN) {
            callPartyNumberLen = MAX_CALL_PARTY_NUM_LEN;
        }
        writer.writeShort((UINT16)callPartyNumberLen);
        writer.writeBytes((UINT8*)callPartyNumber.data(), callPartyNumberLen);

        //active rat
        UINT8 activeRat = m_jsonData[ns_oem_dropped_mt_call_info_ind::key_active_rat].asInt();
        writer.writeByte(activeRat);

        //PLMN
        BYTE plmnArray[3] = {0,};
        Json::Value jsonPlmn = m_jsonData[ns_oem_dropped_mt_call_info_ind::key_plmn_id];
        if (jsonPlmn.size() == 3) {//3 bytes BCD PLMN
            plmnArray[0] = (BYTE)jsonPlmn[0].asInt();
            plmnArray[1] = (BYTE)jsonPlmn[1].asInt();
            plmnArray[2] = (BYTE)jsonPlmn[2].asInt();
        } else {
            RilLogW("PLMN size = %d is not correct", jsonPlmn.size());
        }
        writer.writeBytes(plmnArray, 3);

        //cell id
        ULONG cellId =  m_jsonData[ns_oem_dropped_mt_call_info_ind::key_cell_id].asLargestUInt();
        writer.writeInt64(cellId);

        //TAC
        BYTE tacArray[3] = {0,};
        Json::Value jsonTac  =  m_jsonData[ns_oem_dropped_mt_call_info_ind::key_tac];
        if (jsonPlmn.size() == 3) {//3 bytes BCD TAC
            tacArray[0] = (BYTE)jsonTac[0].asInt();
            tacArray[1] = (BYTE)jsonTac[1].asInt();
            tacArray[2] = (BYTE)jsonTac[2].asInt();
        } else {
            RilLogW("TAC size = %d is not correct", jsonTac.size());
        }
        writer.writeBytes(tacArray, 3);

        //pci
        int pci =  m_jsonData[ns_oem_dropped_mt_call_info_ind::key_pci].asInt();
        writer.writeInt(pci);

        m_dataLength = writer.getSize();
        m_legacyData = new char[m_dataLength];
        if(m_legacyData) {
            memcpy(m_legacyData, writer.getBytes(), m_dataLength);
        } else {
            m_dataLength = 0;
        }
    }
#endif //OEM_JSON_APPFW_CONVERTION
}

/**
 * ProtocolMiscSetAheadRreAdapterJson
 */
ProtocolMiscSetAheadRreAdapterJson::ProtocolMiscSetAheadRreAdapterJson(const ModemData *pModemData)
    : ProtocolJsonRespAdapter(pModemData) {
    ParseJsonPayload(SIT_OEM_SET_AHEAD_RRE);
}

int ProtocolMiscSetAheadRreAdapterJson::GetResult() const {
    if(m_bParsingSuccess) {
        return m_jsonData[ns_oem_set_ahead_rre_rsp::key_result].asInt();
    }
    return 0;
}

UINT ProtocolMiscSetAheadRreAdapterJson::GetErrorCode() const {
    return ProtocolRespAdapter::GetErrorCode();
}

/**
 * ProtocolMiscSetForbidLteCellAdapterJson
 */
ProtocolMiscSetForbidLteCellAdapterJson::ProtocolMiscSetForbidLteCellAdapterJson(const ModemData *pModemData)
    : ProtocolJsonRespAdapter(pModemData) {
    ParseJsonPayload(SIT_OEM_SET_FORBID_LTE_CELL);
}

int ProtocolMiscSetForbidLteCellAdapterJson::GetResult() const {
    if(m_bParsingSuccess) {
        return m_jsonData[ns_oem_set_forbid_lte_rsp::key_result].asInt();
    }
    return 0;
}

/**
 * ProtocolMiscSetForbidFrequencyAdapterJson
 */
ProtocolMiscSetForbidFrequencyAdapterJson::ProtocolMiscSetForbidFrequencyAdapterJson(const ModemData *pModemData)
    : ProtocolJsonRespAdapter(pModemData) {
    ParseJsonPayload(SIT_OEM_SET_FORBIDDEN_FREQUENCY);
}

int ProtocolMiscSetForbidFrequencyAdapterJson::GetResult() const {
    if(m_bParsingSuccess) {
        return m_jsonData[ns_oem_set_forbidden_freq_rsp::key_result].asInt();
    }
    return 0;
}

/**
 * ProtocolMiscDataStateChangeAdapterJson
 */
ProtocolMiscDataStateChangeAdapterJson::ProtocolMiscDataStateChangeAdapterJson(const ModemData *pModemData)
    : ProtocolJsonIndAdapter(pModemData, SIT_IND_DATA_STATE_CHANGE)
{
    m_ExpectedState = 0;

    if (m_bParsingSuccess) {
        m_ExpectedState = m_jsonData[ns_misc_ps_data_state_change_ind::key_expected_state].asInt();
    }
}

BYTE ProtocolMiscDataStateChangeAdapterJson::GetExpectedState()
{
    return m_ExpectedState;
}

const char * ProtocolMiscDataStateChangeAdapterJson::GetParameter() const
{
    return ProtocolIndAdapter::GetParameter();
}

UINT ProtocolMiscDataStateChangeAdapterJson::GetParameterLength() const
{
    return ProtocolIndAdapter::GetParameterLength();
}

/**
 * ProtocolMiscGetTraceDumpCfgAdapterJson
 */
ProtocolMiscGetTraceDumpCfgAdapterJson::ProtocolMiscGetTraceDumpCfgAdapterJson(const ModemData *pModemData)
    : ProtocolJsonRespAdapter(pModemData) {
    ParseJsonPayload(SIT_OEM_GET_TRACE_DUMP_CFG);
}

BYTE ProtocolMiscGetTraceDumpCfgAdapterJson::GetConfig() const {
    if(m_bParsingSuccess) {
        return m_jsonData[ns_oem_get_trace_dump_cfg_rsp::key_config].asInt();
    }
    return 0;
}


