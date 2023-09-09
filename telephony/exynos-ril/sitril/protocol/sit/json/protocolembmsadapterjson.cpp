/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#include "protocolembmsadapterjson.h"
#include "protocolutils.h"
#include "embmsjsondef.h"
#include "rillog.h"

// add category to display selective logs
#undef RilLogI
#define RilLogI(format, ...) CRilLog::Log(RIL_LOG_CAT_EMBMS, CRilLog::E_RIL_INFO_LOG,  format, ##__VA_ARGS__)
#undef RilLogV
#define RilLogV(format, ...) CRilLog::Log(RIL_LOG_CAT_EMBMS, CRilLog::E_RIL_VERBOSE_LOG,  format, ##__VA_ARGS__)
#undef RilLogW
#define RilLogW(format, ...) CRilLog::Log(RIL_LOG_CAT_EMBMS, CRilLog::E_RIL_WARNING_LOG,  format, ##__VA_ARGS__)
#undef RilLogE
#define RilLogE(format, ...) CRilLog::Log(RIL_LOG_CAT_EMBMS, CRilLog::E_RIL_CRITICAL_LOG,  format, ##__VA_ARGS__)

#define BITSIZE 8
#define SIGNFLAG (1 << (BITSIZE - 1))
#define DATABITS (SIGNFLAG - 1)

/**
 * ProtocolEmbmsCoverageAdapterJson
 */
int ProtocolEmbmsCoverageAdapterJson::GetCoverage() const {
    if (!m_bParsingSuccess) return EMBMS_UNKNOW_STATE_COVERAGE;

    int coverage = m_jsonData[ns_embms_coverage_ind::key_coverage].asInt();
    RilLogV("Coverage=0x%02x", coverage);
    return coverage;
}

/**
 * ProtocolEmbmsSessionListAdapterJson
 */
ProtocolEmbmsSessionListAdapterJson::ProtocolEmbmsSessionListAdapterJson(const ModemData *pModemData)
    : ProtocolJsonRespAdapter(pModemData) {
    Init();
}

void ProtocolEmbmsSessionListAdapterJson::Init() {
    m_nRecordNum = -1;
    memset(m_tmgiList, 0, sizeof(m_tmgiList));
    ParseJsonPayload(SIT_GET_EMBMS_SESSION_LIST);
    SetTMGI();
}

void ProtocolEmbmsSessionListAdapterJson::SetTMGI() {
    if (!m_bParsingSuccess || ProtocolJsonRespAdapter::GetErrorCode() != RIL_E_SUCCESS) return;
    // for tmgi_list each tmgi_record is of EMBMS_TMGI_LEN BYTES
    // so number of records will be length(tmgi_list)/EMBMS_TMGI_LEN
    int tmgiListLen = m_jsonData[ns_embms_session_list_rsp::key_tmgi_list].size();
    ProtocolUtils::LimitLengthToMax<int>(RIL_LOG_CAT_EMBMS, __CLASS_FUNC__, tmgiListLen, EMBMS_MAX_INTRA_SAILIST_NUMBER * EMBMS_TMGI_LEN);
    m_nRecordNum = tmgiListLen / EMBMS_TMGI_LEN;
    for (int i = 0; i < tmgiListLen; i++) {
        m_tmgiList[i] = (BYTE) m_jsonData[ns_embms_session_list_rsp::key_tmgi_list][i].asInt();
    }
}

const BYTE *ProtocolEmbmsSessionListAdapterJson::GetTMGI() const {
    if (m_nRecordNum > 0) return m_tmgiList;
    return NULL;
}

int ProtocolEmbmsSessionListAdapterJson::GetState() const {
    if (!m_bParsingSuccess) return -1;
    return m_jsonData[ns_embms_session_list_rsp::key_state].asInt();
}

int ProtocolEmbmsSessionListAdapterJson::GetOosReason() const {
    if (!m_bParsingSuccess) return -1;
    return m_jsonData[ns_embms_session_list_rsp::key_oos_reason].asInt();
}

/**
 * ProtocolEmbmsSessionListIndAdapterJson
 */
ProtocolEmbmsSessionListIndAdapterJson::ProtocolEmbmsSessionListIndAdapterJson(const ModemData *pModemData)
    : ProtocolJsonIndAdapter(pModemData) {
    Init();
}

void ProtocolEmbmsSessionListIndAdapterJson::Init() {
    m_nRecordNum = -1;
    memset(m_tmgiList, 0, sizeof(m_tmgiList));
    ParseJsonPayload(SIT_IND_EMBMS_SESSION_LIST);
    SetTMGI();
}

void ProtocolEmbmsSessionListIndAdapterJson::SetTMGI() {
    if (!m_bParsingSuccess) return;
    // for tmgi_list each tmgi_record is of EMBMS_TMGI_LEN BYTES
    // so number of records will be length(tmgi_list)/EMBMS_TMGI_LEN
    unsigned int tmgiListLen = m_jsonData[ns_embms_session_list_rsp::key_tmgi_list].size();
    ProtocolUtils::LimitLengthToMax<unsigned int>(RIL_LOG_CAT_EMBMS, __CLASS_FUNC__, tmgiListLen, EMBMS_MAX_INTRA_SAILIST_NUMBER * EMBMS_TMGI_LEN);
    m_nRecordNum = tmgiListLen / EMBMS_TMGI_LEN;
    for (unsigned int i = 0; i < tmgiListLen; i++) {
        m_tmgiList[i] = (BYTE) m_jsonData[ns_embms_session_list_rsp::key_tmgi_list][i].asInt();
    }
}

const BYTE *ProtocolEmbmsSessionListIndAdapterJson::GetTMGI() const {
    if (m_nRecordNum > 0) return m_tmgiList;
    return NULL;
}

int ProtocolEmbmsSessionListIndAdapterJson::GetState() const {
    if (!m_bParsingSuccess) return -1;
    return m_jsonData[ns_embms_session_list_rsp::key_state].asInt();
}

int ProtocolEmbmsSessionListIndAdapterJson::GetOosReason() const {
    if (!m_bParsingSuccess) return -1;
    return m_jsonData[ns_embms_session_list_rsp::key_oos_reason].asInt();
}

/**
 * ProtocolEmbmsNetworkTimeAdapterJson
 */
uint64_t ProtocolEmbmsNetworkTimeAdapterJson::GetNetworkTime() const {
    if (!m_bParsingSuccess || GetErrorCode() != RIL_E_SUCCESS) return -1;

    int timeInfoType = m_jsonData[ns_embms_network_time_rsp::key_time_info_type].asInt();
    if (timeInfoType == 0x05) return -1;  // 0x02: Time Information from GSM network, 0x05 : no time information

    struct tm t = { };
    time_t t_of_day;

    memset(&t_of_day, 0x00, sizeof(time_t));
    t.tm_year = m_jsonData[ns_embms_network_time_rsp::key_year].asInt() + 100;   // ex) 2019 - 1900 = 119 (years since 1900 in struct tm)
    t.tm_mon = m_jsonData[ns_embms_network_time_rsp::key_month].asInt() - 1;     // Month, 0 - jan (range 0-11 in struct tm)
    t.tm_mday = m_jsonData[ns_embms_network_time_rsp::key_day].asInt();  // Day of the month
    t.tm_hour = m_jsonData[ns_embms_network_time_rsp::key_hour].asInt();
    t.tm_min = m_jsonData[ns_embms_network_time_rsp::key_minute].asInt();
    t.tm_sec = m_jsonData[ns_embms_network_time_rsp::key_second].asInt();
    t.tm_isdst = (m_jsonData[ns_embms_network_time_rsp::key_daylight_adjust].asInt() == 0) ?
        0 : 1;        // Is DST on? 1 = yes, 0 = no, -1 = unknown
    t_of_day = mktime(&t);
    t_of_day += t.tm_gmtoff;

    RilLogI("%s() Time %lld", __FUNCTION__, (uint64_t)t_of_day);
    return (uint64_t)t_of_day;
}

int ProtocolEmbmsNetworkTimeAdapterJson::DayLightValid() const {
    if (!m_bParsingSuccess) return SIT_NITZ_DAYLIGHT_INFO_VALID;
    return m_jsonData[ns_embms_network_time_rsp::key_daylight_valid].asInt();
}

int ProtocolEmbmsNetworkTimeAdapterJson::Year() const {
    if (!m_bParsingSuccess) return 0;
    return m_jsonData[ns_embms_network_time_rsp::key_year].asInt();
}

int ProtocolEmbmsNetworkTimeAdapterJson::Month() const {
    if (!m_bParsingSuccess) return 0;
    return m_jsonData[ns_embms_network_time_rsp::key_month].asInt();
}

int ProtocolEmbmsNetworkTimeAdapterJson::Day() const {
    if (!m_bParsingSuccess) return 0;
    return m_jsonData[ns_embms_network_time_rsp::key_day].asInt();
}

int ProtocolEmbmsNetworkTimeAdapterJson::Hour() const {
    if (!m_bParsingSuccess) return 0;
    return m_jsonData[ns_embms_network_time_rsp::key_hour].asInt();
}

int ProtocolEmbmsNetworkTimeAdapterJson::Minute() const {
    if (!m_bParsingSuccess) return 0;
    return m_jsonData[ns_embms_network_time_rsp::key_minute].asInt();
}

int ProtocolEmbmsNetworkTimeAdapterJson::Second() const {
    if (!m_bParsingSuccess) return 0;
    return m_jsonData[ns_embms_network_time_rsp::key_second].asInt();
}

int ProtocolEmbmsNetworkTimeAdapterJson::TimeZone() const {
    if (!m_bParsingSuccess) return 0;

    int timezone = m_jsonData[ns_embms_network_time_rsp::key_time_zone].asInt();
    if ((timezone & SIGNFLAG) != 0) {                   // signflag set
        timezone= (~timezone & DATABITS) + 1;          // 2s complement without signflag
        timezone= -timezone;                          // negative number
    }
    return timezone;
}

int ProtocolEmbmsNetworkTimeAdapterJson::DayLightAdjust() const {
    if (!m_bParsingSuccess) return SIT_NITZ_DAYLIGHT_ADJUST_NOADJUST;
    return m_jsonData[ns_embms_network_time_rsp::key_daylight_adjust].asInt();
}

int ProtocolEmbmsNetworkTimeAdapterJson::DayofWeek() const {
    if (!m_bParsingSuccess) return SIT_NITZ_DAY_OF_WEEK_SUN;
    return m_jsonData[ns_embms_network_time_rsp::key_day_of_week].asInt();
}

/**
 * ProtocolEmbmsNetworkTimeIndAdapterJson
 */
uint64_t ProtocolEmbmsNetworkTimeIndAdapterJson::GetNetworkTime() const {
    if (!m_bParsingSuccess) return -1;

    int timeInfoType = m_jsonData[ns_embms_network_time_ind::key_time_info_type].asInt();
    if (timeInfoType == 0x05) return -1;  // 0x02: Time Information from GSM network, 0x05 : no time information

    struct tm t = { };
    time_t t_of_day;

    memset(&t_of_day, 0x00, sizeof(time_t));
    t.tm_year = m_jsonData[ns_embms_network_time_ind::key_year].asInt() + 100;   // ex) 2019 - 1900 = 119 (years since 1900 in struct tm)
    t.tm_mon = m_jsonData[ns_embms_network_time_ind::key_month].asInt() - 1;     // Month, 0 - jan (range 0-11 in struct tm)
    t.tm_mday = m_jsonData[ns_embms_network_time_ind::key_day].asInt();  // Day of the month
    t.tm_hour = m_jsonData[ns_embms_network_time_ind::key_hour].asInt();
    t.tm_min = m_jsonData[ns_embms_network_time_ind::key_minute].asInt();
    t.tm_sec = m_jsonData[ns_embms_network_time_ind::key_second].asInt();
    t.tm_isdst = (m_jsonData[ns_embms_network_time_ind::key_daylight_adjust].asInt() == 0) ?
        0 : 1;        // Is DST on? 1 = yes, 0 = no, -1 = unknown
    t_of_day = mktime(&t);
    t_of_day += t.tm_gmtoff;

    RilLogI("%s() Time %lld", __FUNCTION__, (uint64_t)t_of_day);
    return (uint64_t)t_of_day;
}

int ProtocolEmbmsNetworkTimeIndAdapterJson::DayLightValid() const {
    if (!m_bParsingSuccess) return SIT_NITZ_DAYLIGHT_INFO_VALID;
    return m_jsonData[ns_embms_network_time_ind::key_daylight_valid].asInt();
}

int ProtocolEmbmsNetworkTimeIndAdapterJson::Year() const {
    if (!m_bParsingSuccess) return 0;
    return m_jsonData[ns_embms_network_time_ind::key_year].asInt();
}

int ProtocolEmbmsNetworkTimeIndAdapterJson::Month() const {
    if (!m_bParsingSuccess) return 0;
    return m_jsonData[ns_embms_network_time_ind::key_month].asInt();
}

int ProtocolEmbmsNetworkTimeIndAdapterJson::Day() const {
    if (!m_bParsingSuccess) return 0;
    return m_jsonData[ns_embms_network_time_ind::key_day].asInt();
}

int ProtocolEmbmsNetworkTimeIndAdapterJson::Hour() const {
    if (!m_bParsingSuccess) return 0;
    return m_jsonData[ns_embms_network_time_ind::key_hour].asInt();
}

int ProtocolEmbmsNetworkTimeIndAdapterJson::Minute() const {
    if (!m_bParsingSuccess) return 0;
    return m_jsonData[ns_embms_network_time_ind::key_minute].asInt();
}

int ProtocolEmbmsNetworkTimeIndAdapterJson::Second() const {
    if (!m_bParsingSuccess) return 0;
    return m_jsonData[ns_embms_network_time_ind::key_second].asInt();
}

int ProtocolEmbmsNetworkTimeIndAdapterJson::TimeZone() const {
    if (!m_bParsingSuccess) return 0;

    int timezone = m_jsonData[ns_embms_network_time_ind::key_time_zone].asInt();
    if ((timezone & SIGNFLAG) != 0) {                   // signflag set
        timezone= (~timezone & DATABITS) + 1;          // 2s complement without signflag
        timezone= -timezone;                          // negative number
    }
    return timezone;
}

int ProtocolEmbmsNetworkTimeIndAdapterJson::DayLightAdjust() const {
    if (!m_bParsingSuccess) return SIT_NITZ_DAYLIGHT_ADJUST_NOADJUST;
    return m_jsonData[ns_embms_network_time_ind::key_daylight_adjust].asInt();
}

int ProtocolEmbmsNetworkTimeIndAdapterJson::DayofWeek() const {
    if (!m_bParsingSuccess) return SIT_NITZ_DAY_OF_WEEK_SUN;
    return m_jsonData[ns_embms_network_time_ind::key_day_of_week].asInt();
}

/**
 * ProtocolEmbmsSignalStrengthAdapterJson
 */
ProtocolEmbmsSignalStrengthAdapterJson::ProtocolEmbmsSignalStrengthAdapterJson(const ModemData *pModemData)
    : ProtocolJsonRespAdapter(pModemData, SIT_GET_EMBMS_SIGNAL_STRENGTH) {
    SetSnrList();
}

void ProtocolEmbmsSignalStrengthAdapterJson::SetSnrList() {
    memset(m_arrSnrList, 0x0, sizeof(m_arrSnrList));
    m_nNumberOfRecord = 0;

    if (!m_bParsingSuccess) return;

    Json::Value jsonSignalRecord;
    jsonSignalRecord = m_jsonData[ns_embms_signal_strength_rsp::key_signal_record_list];
    m_nNumberOfRecord = m_jsonData[ns_embms_signal_strength_rsp::key_signal_record_list].size();
    ProtocolUtils::LimitLengthToMax<unsigned int>(RIL_LOG_CAT_EMBMS, __CLASS_FUNC__, m_nNumberOfRecord, MAX_SIGNAL_RECORD_NUM);
    for (unsigned int i = 0; i < m_nNumberOfRecord; i++) {
        m_arrSnrList[i] = jsonSignalRecord[i][ns_embms_signal_record::key_snr].asUInt();
    }
}

const UINT32 *ProtocolEmbmsSignalStrengthAdapterJson::GetSnrList() const {
    if (m_nNumberOfRecord > 0) return m_arrSnrList;

    return NULL;
}

/**
 * ProtocolEmbmsSignalStrengthIndAdapterJson
 */
ProtocolEmbmsSignalStrengthIndAdapterJson::ProtocolEmbmsSignalStrengthIndAdapterJson(const ModemData *pModemData)
    : ProtocolJsonIndAdapter(pModemData, SIT_IND_EMBMS_SIGNAL_STRENGTH) {
    SetSnrList();
}

void ProtocolEmbmsSignalStrengthIndAdapterJson::SetSnrList() {
    memset(m_arrSnrList, 0x0, sizeof(m_arrSnrList));
    m_nNumberOfRecord = 0;

    if (!m_bParsingSuccess) return;

    Json::Value jsonSignalRecord;
    jsonSignalRecord = m_jsonData[ns_embms_signal_strength_ind::key_signal_record_list];
    m_nNumberOfRecord = m_jsonData[ns_embms_signal_strength_ind::key_signal_record_list].size();
    ProtocolUtils::LimitLengthToMax<unsigned int>(RIL_LOG_CAT_EMBMS, __CLASS_FUNC__, m_nNumberOfRecord, MAX_SIGNAL_RECORD_NUM);
    for (unsigned int i = 0; i < m_nNumberOfRecord; i++) {
        m_arrSnrList[i] = jsonSignalRecord[i][ns_embms_signal_record::key_snr].asUInt();
    }
}

const UINT32 *ProtocolEmbmsSignalStrengthIndAdapterJson::GetSnrList() const {
    if (m_nNumberOfRecord > 0) return m_arrSnrList;

    return NULL;
}

/**
 * ProtocolEmbmsSaiListAdapterJson
 */
ProtocolEmbmsSaiListAdapterJson::ProtocolEmbmsSaiListAdapterJson(const ModemData *pModemData)
    : ProtocolJsonIndAdapter(pModemData) {
    Init();
}

void ProtocolEmbmsSaiListAdapterJson::Init() {
    memset(&m_saiList, 0, sizeof(m_saiList));
    ParseJsonPayload(SIT_IND_EMBMS_SAI_LIST);

    if (!m_bParsingSuccess) return;

    Json::Value jsonInterSaiData;
    m_saiList.IntraSaiListNum =
        MIN(m_jsonData[ns_embms_sailist_ind::key_intra_sai_list].size(), MAX_INTRA_SAILIST_NUMBER);
    jsonInterSaiData = m_jsonData[ns_embms_sailist_ind::key_inter_sai_list];
    m_saiList.InterSaiListNum =
        MIN(m_jsonData[ns_embms_sailist_ind::key_inter_sai_list].size(), MAX_INTER_SAILIST_NUMBER);
    for (int i = 0; i < m_saiList.IntraSaiListNum; i++) {
        m_saiList.IntraSaiList[i] =
            (UINT16) m_jsonData[ns_embms_sailist_ind::key_intra_sai_list][i].asInt();
    }
    for (int i = 0; i < m_saiList.InterSaiListNum; i++) {
        m_saiList.InterSaiList[i].Frequency =
            jsonInterSaiData[i][ns_embms_inter_sai::key_frequency].asUInt();
        m_saiList.InterSaiList[i].InterSaiNumber =
            MIN(jsonInterSaiData[i][ns_embms_inter_sai::key_inter_sai_info_list].size(),
                    MAX_INTER_SAI_NUMBER);
        for (int j = 0; j < m_saiList.InterSaiList[i].InterSaiNumber; j++) {
            m_saiList.InterSaiList[i].InterSaiInfo[j] =
                (UINT16) jsonInterSaiData[i][ns_embms_inter_sai::key_inter_sai_info_list][j].asInt();
        }
        m_saiList.InterSaiList[i].MultiBandInfoNumber =
            MIN(jsonInterSaiData[i][ns_embms_inter_sai::key_multiband_info].size(),
                    MAX_MULTI_BAND_NUMBER);
        for (int j = 0; j < m_saiList.InterSaiList[i].MultiBandInfoNumber; j++) {
            m_saiList.InterSaiList[i].MultiBandInfo[j] =
                (UINT8) jsonInterSaiData[i][ns_embms_inter_sai::key_multiband_info][j].asInt();
        }
    }
}

const UINT16 *ProtocolEmbmsSaiListAdapterJson::GetIntraSaiList() const {
    if (m_saiList.IntraSaiListNum > 0) return m_saiList.IntraSaiList;
    return NULL;
}

const EMBMS_InterSaiList *ProtocolEmbmsSaiListAdapterJson::GetInterSaiList() const {
    if (m_saiList.InterSaiListNum > 0) return m_saiList.InterSaiList;
    return NULL;
}

/**
 * ProtocolEmbmsGlobalCellIdAdapterJson
 */
ProtocolEmbmsGlobalCellIdAdapterJson::ProtocolEmbmsGlobalCellIdAdapterJson(const ModemData *pModemData)
    : ProtocolJsonIndAdapter(pModemData) {
    Init();
}

void ProtocolEmbmsGlobalCellIdAdapterJson::Init() {
    memset(m_mcc, 0, sizeof(m_mcc));
    memset(m_mnc, 0, sizeof(m_mnc));

    ParseJsonPayload(SIT_IND_EMBMS_GLOBAL_CELL_ID);
    SetMccMnc();
}

void ProtocolEmbmsGlobalCellIdAdapterJson::SetMccMnc() {
    if (!m_bParsingSuccess) return;

    string mcc = m_jsonData[ns_embms_global_cellid_ind::key_mcc].asString();
    ProtocolUtils::LimitStringToMaxLength(RIL_LOG_CAT_EMBMS, __CLASS_FUNC__, mcc, MAX_MCC_LEN);
    if (mcc.length() > 0) memcpy(m_mcc, mcc.c_str(), mcc.length());

    string mnc = m_jsonData[ns_embms_global_cellid_ind::key_mnc].asString();
    ProtocolUtils::LimitStringToMaxLength(RIL_LOG_CAT_EMBMS, __CLASS_FUNC__, mnc, MAX_MNC_LEN);
    if (mnc.length() > 0) memcpy(m_mnc, mnc.c_str(), mnc.length());
}

const char *ProtocolEmbmsGlobalCellIdAdapterJson::GetMcc() const {
    if (strlen(m_mcc) > 0) return m_mcc;
    return NULL;
}

const char *ProtocolEmbmsGlobalCellIdAdapterJson::GetMnc() const {
    if (strlen(m_mnc) > 0) return m_mnc;
    return NULL;
}

UINT32 ProtocolEmbmsGlobalCellIdAdapterJson::GetCellId() const {
    if (!m_bParsingSuccess) return -1;
    return m_jsonData[ns_embms_global_cellid_ind::key_cell_id].asUInt();
}
