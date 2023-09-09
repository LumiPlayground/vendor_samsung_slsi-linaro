/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#include "protocolembmsadapterlegacy.h"
#include "rillog.h"
#include "modemdata.h"

// add category to display selective logs
#undef RilLogI
#define RilLogI(format, ...) CRilLog::Log(RIL_LOG_CAT_EMBMS, CRilLog::E_RIL_INFO_LOG,  format, ##__VA_ARGS__)
#undef RilLogV
#define RilLogV(format, ...) CRilLog::Log(RIL_LOG_CAT_EMBMS, CRilLog::E_RIL_VERBOSE_LOG,  format, ##__VA_ARGS__)
#undef RilLogW
#define RilLogW(format, ...) CRilLog::Log(RIL_LOG_CAT_EMBMS, CRilLog::E_RIL_WARNING_LOG,  format, ##__VA_ARGS__)
#undef RilLogE
#define RilLogE(format, ...) CRilLog::Log(RIL_LOG_CAT_EMBMS, CRilLog::E_RIL_CRITICAL_LOG,  format, ##__VA_ARGS__)

/**
 * ProtocolEmbmsCoverageAdapterLegacy
 */
int ProtocolEmbmsCoverageAdapterLegacy::GetCoverage() const
{
    if (m_pModemData != NULL) {
        sit_embms_coverage_rsp *data = (sit_embms_coverage_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_IND_EMBMS_COVERAGE) {
            RilLogV("Coverage=0x%02x", data->coverage);
            return data->coverage;
        }
    }
    return EMBMS_UNKNOW_STATE_COVERAGE;
}

/**
 * ProtocolEmbmsGlobalCellIdAdapterLegacy
 */
ProtocolEmbmsGlobalCellIdAdapterLegacy::ProtocolEmbmsGlobalCellIdAdapterLegacy(const ModemData *pModemData)
    : ProtocolIndAdapter(pModemData) {
    Init();
}

void ProtocolEmbmsGlobalCellIdAdapterLegacy::Init()
{
    memset(m_mcc, 0, sizeof(m_mcc));
    memset(m_mnc, 0, sizeof(m_mnc));
    m_cellId = -1;

    if (m_pModemData != NULL) {
        sit_embms_global_cellid_rsp *data = (sit_embms_global_cellid_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_IND_EMBMS_GLOBAL_CELL_ID) {
            memcpy(m_mcc, data->mcc, MAX_MCC_LEN);
            if (data->mnc[2] == 'F') {
                memcpy(m_mnc, data->mnc, MAX_MNC_LEN-1);
            } else {
                memcpy(m_mnc, data->mnc, MAX_MNC_LEN);
            }

            m_cellId = data->cellId;
            RilLogV("%s() mcc : %s, mnc : %s, cellid : %d", __FUNCTION__, m_mcc, m_mnc, m_cellId);
        }
    }
}

const char *ProtocolEmbmsGlobalCellIdAdapterLegacy::GetMcc() const
{
    if (strlen(m_mcc) > 0)
        return m_mcc;
    return NULL;
}

const char *ProtocolEmbmsGlobalCellIdAdapterLegacy::GetMnc() const
{
    if (strlen(m_mnc) > 0)
        return m_mnc;
    return NULL;
}

UINT32 ProtocolEmbmsGlobalCellIdAdapterLegacy::GetCellId() const
{
    return m_cellId;
}

/**
 * ProtocolEmbmsSessionListAdapterLegacy
 */
ProtocolEmbmsSessionListAdapterLegacy::ProtocolEmbmsSessionListAdapterLegacy(const ModemData *pModemData)
    : ProtocolRespAdapter(pModemData) {
    Init();
}

void ProtocolEmbmsSessionListAdapterLegacy::Init()
{
    m_state = -1;
    m_oos_reason = -1;
    m_record_num = -1;
    memset(m_tmgi, 0, sizeof(m_tmgi));

    if (m_pModemData != NULL) {
        sit_embms_ssesion_list_rsp *data = (sit_embms_ssesion_list_rsp *)m_pModemData->GetRawData();
        if (GetErrorCode() == RIL_E_SUCCESS && data != NULL && data->hdr.id == SIT_GET_EMBMS_SESSION_LIST) {
            m_state = data->session_state;
            m_oos_reason = data->oos_reason;
            if (data->number_record <= EMBMS_MAX_INTRA_SAILIST_NUMBER) {
                m_record_num = data->number_record;
                memcpy(m_tmgi, data->tmgi, m_record_num*EMBMS_TMGI_LEN);
            } else {
                RilLogE("%s data->number_record %d is invalid", __FUNCTION__, data->number_record);
            }
        }
    }
}

const BYTE *ProtocolEmbmsSessionListAdapterLegacy::GetTMGI() const
{
    if (m_record_num > 0)
        return (const BYTE*)m_tmgi;
    return NULL;
}

/**
 * ProtocolEmbmsSessionListIndAdapterLegacy
 */
ProtocolEmbmsSessionListIndAdapterLegacy::ProtocolEmbmsSessionListIndAdapterLegacy(const ModemData *pModemData)
    : ProtocolIndAdapter(pModemData) {
    Init();
}

void ProtocolEmbmsSessionListIndAdapterLegacy::Init()
{
    m_state = -1;
    m_oos_reason = -1;
    m_record_num = -1;
    memset(m_tmgi, 0, sizeof(m_tmgi));

    if (m_pModemData != NULL) {
        sit_embms_ssesion_list_ind *data = (sit_embms_ssesion_list_ind *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_IND_EMBMS_SESSION_LIST) {
            m_state = data->session_state;
            m_oos_reason = data->oos_reason;
            m_record_num = data->number_record;
            memcpy(m_tmgi, data->tmgi, EMBMS_MAX_INTRA_SAILIST_NUMBER*EMBMS_TMGI_LEN);
        }
    }
}

const BYTE *ProtocolEmbmsSessionListIndAdapterLegacy::GetTMGI() const
{
    if (m_record_num > 0)
        return (const BYTE*)m_tmgi;
    return NULL;
}

/**
 * ProtocolEmbmsNetworkTimeAdapterLegacy
 */
uint64_t ProtocolEmbmsNetworkTimeAdapterLegacy::GetNetworkTime() const
{
    if (m_pModemData != NULL) {
        sit_embms_network_time_rsp *data = (sit_embms_network_time_rsp *)m_pModemData->GetRawData();
        if (GetErrorCode() == RIL_E_SUCCESS && data != NULL && data->hdr.id == SIT_GET_EMBMS_NETWORK_TIME) {
            if(data->TimeInfoType == 0x05) {   // 0x02: Time Information from GSM network, 0x05 : no time information
                return (-1);
            }
            struct tm t = {};
            time_t t_of_day;

            memset(&t_of_day, 0x00, sizeof(time_t));
            t.tm_year = data->year + 100;   // ex) 2019 - 1900 = 119 (years since 1900 in struct tm)
            t.tm_mon = data->month - 1;     // Month, 0 - jan (range 0-11 in struct tm)
            t.tm_mday = data->day;          // Day of the month
            t.tm_hour = data->hour;
            t.tm_min = data->minute;
            t.tm_sec = data->second;
            t.tm_isdst = (data->daylight_adjust == 0) ? 0 : 1;        // Is DST on? 1 = yes, 0 = no, -1 = unknown
            t_of_day = mktime(&t);
            t_of_day += t.tm_gmtoff;

            RilLogI("%s() Time %lld", __FUNCTION__, (uint64_t)t_of_day);
            return (uint64_t)t_of_day;
        }
    }

    return (-1);
}
int ProtocolEmbmsNetworkTimeAdapterLegacy::DayLightValid() const
{
    int daylightvalid = SIT_NITZ_DAYLIGHT_INFO_VALID;
    if (m_pModemData != NULL) {
        sit_embms_network_time_rsp *data = (sit_embms_network_time_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_EMBMS_NETWORK_TIME) {
            daylightvalid = data->daylight_valid;
        }
    }
    return daylightvalid;
}

int ProtocolEmbmsNetworkTimeAdapterLegacy::Year() const
{
    int year = 0;
    if (m_pModemData != NULL) {
        sit_embms_network_time_rsp *data = (sit_embms_network_time_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_EMBMS_NETWORK_TIME) {
            year = data->year;
        }
    }
    return year;
}

int ProtocolEmbmsNetworkTimeAdapterLegacy::Month() const
{
    int month = 0;
    if (m_pModemData != NULL) {
        sit_embms_network_time_rsp *data = (sit_embms_network_time_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_EMBMS_NETWORK_TIME) {
            month = data->month;
        }
    }
    return month;
}

int ProtocolEmbmsNetworkTimeAdapterLegacy::Day() const
{
    int day = 0;
    if (m_pModemData != NULL) {
        sit_embms_network_time_rsp *data = (sit_embms_network_time_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_EMBMS_NETWORK_TIME) {
            day = data->day;
        }
    }
    return day;
}

int ProtocolEmbmsNetworkTimeAdapterLegacy::Hour() const
{
    int hour = 0;
    if (m_pModemData != NULL) {
        sit_embms_network_time_rsp *data = (sit_embms_network_time_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_EMBMS_NETWORK_TIME) {
            hour = data->hour;
        }
    }
    return hour;
}

int ProtocolEmbmsNetworkTimeAdapterLegacy::Minute() const
{
    int minute = 0;
    if (m_pModemData != NULL) {
        sit_embms_network_time_rsp *data = (sit_embms_network_time_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_EMBMS_NETWORK_TIME) {
            minute = data->minute;
        }
    }
    return minute;
}

int ProtocolEmbmsNetworkTimeAdapterLegacy::Second() const
{
    int second = 0;
    if (m_pModemData != NULL) {
        sit_embms_network_time_rsp *data = (sit_embms_network_time_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_EMBMS_NETWORK_TIME) {
            second = data->second;
        }
    }
    return second;
}

int ProtocolEmbmsNetworkTimeAdapterLegacy::TimeZone() const
{
    #define BITSIZE 8
    #define SIGNFLAG (1<<(BITSIZE-1))
    #define DATABITS (SIGNFLAG-1)

    int timezone = 0;
    if (m_pModemData != NULL) {
        sit_embms_network_time_rsp *data = (sit_embms_network_time_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_EMBMS_NETWORK_TIME) {
            timezone = data->time_zone;
            if ((timezone& SIGNFLAG)!=0) {                    // signflag set
                timezone= (~timezone & DATABITS) + 1;         // 2s complement without signflag
                timezone= -timezone;                              // negative number
            }
        }
    }
    return timezone;
}
int ProtocolEmbmsNetworkTimeAdapterLegacy::DayLightAdjust() const
{
    int daylightadj = SIT_NITZ_DAYLIGHT_ADJUST_NOADJUST;
    if (m_pModemData != NULL) {
        sit_embms_network_time_rsp *data = (sit_embms_network_time_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_EMBMS_NETWORK_TIME) {
            daylightadj = data->daylight_adjust;
        }
    }
    return daylightadj;
}

int ProtocolEmbmsNetworkTimeAdapterLegacy::DayofWeek() const
{
    int dayofweek = SIT_NITZ_DAY_OF_WEEK_SUN;
    if (m_pModemData != NULL) {
        sit_embms_network_time_rsp *data = (sit_embms_network_time_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_EMBMS_NETWORK_TIME) {
            dayofweek = data->day_of_week;
        }
    }
    return dayofweek;
}

/**
 * ProtocolEmbmsNetworkTimeIndAdapterLegacy
 */
uint64_t ProtocolEmbmsNetworkTimeIndAdapterLegacy::GetNetworkTime() const
{
    if (m_pModemData != NULL) {
        sit_embms_network_time_ind *data = (sit_embms_network_time_ind *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_IND_EMBMS_NETWORK_TIME) {
            if(data->TimeInfoType == 0x05) {   // 0x02: Time Information from GSM network, 0x05 : no time information
                return (-1);
            }
            struct tm t = {};
            time_t t_of_day;

            t.tm_year = data->year + 100;   // ex) 2019 - 1900 = 119 (years since 1900 in struct tm)
            t.tm_mon = data->month - 1;     // Month, 0 - jan (range 0-11 in struct tm)
            t.tm_mday = data->day;          // Day of the month
            t.tm_hour = data->hour;
            t.tm_min = data->minute;
            t.tm_sec = data->second;
            t.tm_isdst = (data->daylight_adjust == 0) ? 0 : 1;        // Is DST on? 1 = yes, 0 = no, -1 = unknown
            t_of_day = mktime(&t);
            t_of_day += t.tm_gmtoff;

            RilLogI("%s() Time %lld", __FUNCTION__, (uint64_t)t_of_day);
            return (uint64_t)t_of_day;
        }
    }

    return (-1);
}

int ProtocolEmbmsNetworkTimeIndAdapterLegacy::DayLightValid() const
{
    int daylightvalid = SIT_NITZ_DAYLIGHT_INFO_VALID;
    if (m_pModemData != NULL) {
        sit_embms_network_time_ind *data = (sit_embms_network_time_ind *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_IND_EMBMS_NETWORK_TIME) {
            daylightvalid = data->daylight_valid;
        }
    }
    return daylightvalid;
}

int ProtocolEmbmsNetworkTimeIndAdapterLegacy::Year() const
{
    int year = 0;
    if (m_pModemData != NULL) {
        sit_embms_network_time_ind *data = (sit_embms_network_time_ind *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_IND_EMBMS_NETWORK_TIME) {
            year = data->year;
        }
    }
    return year;
}

int ProtocolEmbmsNetworkTimeIndAdapterLegacy::Month() const
{
    int month = 0;
    if (m_pModemData != NULL) {
        sit_embms_network_time_ind *data = (sit_embms_network_time_ind *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_IND_EMBMS_NETWORK_TIME) {
            month = data->month;
        }
    }
    return month;
}

int ProtocolEmbmsNetworkTimeIndAdapterLegacy::Day() const
{
    int day = 0;
    if (m_pModemData != NULL) {
        sit_embms_network_time_ind *data = (sit_embms_network_time_ind *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_IND_EMBMS_NETWORK_TIME) {
            day = data->day;
        }
    }
    return day;
}

int ProtocolEmbmsNetworkTimeIndAdapterLegacy::Hour() const
{
    int hour = 0;
    if (m_pModemData != NULL) {
        sit_embms_network_time_ind *data = (sit_embms_network_time_ind *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_IND_EMBMS_NETWORK_TIME) {
            hour = data->hour;
        }
    }
    return hour;
}

int ProtocolEmbmsNetworkTimeIndAdapterLegacy::Minute() const
{
    int minute = 0;
    if (m_pModemData != NULL) {
        sit_embms_network_time_ind *data = (sit_embms_network_time_ind *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_IND_EMBMS_NETWORK_TIME) {
            minute = data->minute;
        }
    }
    return minute;
}

int ProtocolEmbmsNetworkTimeIndAdapterLegacy::Second() const
{
    int second = 0;
    if (m_pModemData != NULL) {
        sit_embms_network_time_ind *data = (sit_embms_network_time_ind *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_IND_EMBMS_NETWORK_TIME) {
            second = data->second;
        }
    }
    return second;
}

int ProtocolEmbmsNetworkTimeIndAdapterLegacy::TimeZone() const
{
    #define BITSIZE 8
    #define SIGNFLAG (1<<(BITSIZE-1))
    #define DATABITS (SIGNFLAG-1)

    int timezone = 0;
    if (m_pModemData != NULL) {
        sit_embms_network_time_ind *data = (sit_embms_network_time_ind *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_IND_EMBMS_NETWORK_TIME) {
            timezone = data->time_zone;
            if ((timezone& SIGNFLAG)!=0) {                    // signflag set
                timezone= (~timezone & DATABITS) + 1;         // 2s complement without signflag
                timezone= -timezone;                              // negative number
            }
        }
    }
    return timezone;
}
int ProtocolEmbmsNetworkTimeIndAdapterLegacy::DayLightAdjust() const
{
    int daylightadj = SIT_NITZ_DAYLIGHT_ADJUST_NOADJUST;
    if (m_pModemData != NULL) {
        sit_embms_network_time_ind *data = (sit_embms_network_time_ind *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_IND_EMBMS_NETWORK_TIME) {
            daylightadj = data->daylight_adjust;
        }
    }
    return daylightadj;
}

int ProtocolEmbmsNetworkTimeIndAdapterLegacy::DayofWeek() const
{
    int dayofweek = SIT_NITZ_DAY_OF_WEEK_SUN;
    if (m_pModemData != NULL) {
        sit_embms_network_time_ind *data = (sit_embms_network_time_ind *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_IND_EMBMS_NETWORK_TIME) {
            dayofweek = data->day_of_week;
        }
    }
    return dayofweek;
}

/**
 * ProtocolEmbmsSignalStrengthAdapterLegacy
 */
ProtocolEmbmsSignalStrengthAdapterLegacy::ProtocolEmbmsSignalStrengthAdapterLegacy(const ModemData *pModemData)
    : ProtocolRespAdapter(pModemData) {
    Init();
}

void ProtocolEmbmsSignalStrengthAdapterLegacy::Init()
{
    m_number_record = 0;
    memset(m_arrSnrList, 0x0, sizeof(m_arrSnrList));

    if (m_pModemData != NULL) {
        sit_embms_signal_strength_rsp *data =
                (sit_embms_signal_strength_rsp *)m_pModemData->GetRawData();
        if (GetErrorCode() == RIL_E_SUCCESS && data != NULL
                && data->hdr.id == SIT_GET_EMBMS_SIGNAL_STRENGTH) {
            if (data->number_record > 0 && data->number_record <= MAX_SIGNAL_RECORD_NUM) {
                m_number_record = data->number_record;
                for(int i = 0; i < m_number_record; i++) {
                    m_arrSnrList[i] = data->signal_record[i].SNR;
                }
            } else {
                RilLogE("%s m_number_record %d is wrong.", __FUNCTION__, m_number_record);
            }
        }
    }
}

const UINT32 *ProtocolEmbmsSignalStrengthAdapterLegacy::GetSnrList() const
{
    if (m_number_record > 0)
        return m_arrSnrList;

    return NULL;
}

/**
 * ProtocolEmbmsSignalStrengthIndAdapterLegacy
 */
ProtocolEmbmsSignalStrengthIndAdapterLegacy::ProtocolEmbmsSignalStrengthIndAdapterLegacy(const ModemData *pModemData)
    : ProtocolIndAdapter(pModemData) {
    Init();
}

void ProtocolEmbmsSignalStrengthIndAdapterLegacy::Init()
{
    m_number_record = 0;
    memset(m_arrSnrList, 0x0, sizeof(m_arrSnrList));

    if (m_pModemData != NULL) {
        sit_embms_signal_strength_ind *data =
                (sit_embms_signal_strength_ind *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_IND_EMBMS_SIGNAL_STRENGTH) {
            if (data->number_record > 0 && data->number_record <= MAX_SIGNAL_RECORD_NUM) {
                m_number_record = data->number_record;
                for(int i = 0; i < m_number_record; i++) {
                    m_arrSnrList[i] = data->signal_record[i].SNR;
                }
            } else {
                RilLogE("%s m_number_record %d is wrong.", __FUNCTION__, m_number_record);
            }
        }
    }
}

const UINT32 *ProtocolEmbmsSignalStrengthIndAdapterLegacy::GetSnrList() const
{
    if (m_number_record > 0)
        return m_arrSnrList;

    return NULL;
}

/**
 * ProtocolEmbmsSaiListAdapterLegacy
 */
void ProtocolEmbmsSaiListAdapterLegacy::Init()
{
    memset(&m_saiList, 0, sizeof(m_saiList));

    if (m_pModemData != NULL) {
        sit_embms_sailist *data = (sit_embms_sailist *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_IND_EMBMS_SAI_LIST) {
            if (data->IntraSaiListNum <= (int)(sizeof(m_saiList.IntraSaiList) / sizeof(uint16_t))) {
                m_saiList.IntraSaiListNum = data->IntraSaiListNum;
                memcpy(m_saiList.IntraSaiList, data->IntraSaiList,
                        sizeof(uint16_t)*m_saiList.IntraSaiListNum);
            } else {
                RilLogE("%s data->IntraSaiListNum %d is invalid.", __FUNCTION__,
                        data->IntraSaiListNum);
            }
            if (data->InterSaiListNum <=
                    (int)(sizeof(m_saiList.InterSaiList) / sizeof(sit_embms_inter_sailist))) {
                m_saiList.InterSaiListNum = data->InterSaiListNum;
                memcpy(m_saiList.InterSaiList, data->InterSaiList,
                        sizeof(sit_embms_inter_sailist)*m_saiList.InterSaiListNum);
            } else {
                RilLogE("%s data->InterSaiListNum %d is invalid.",   __FUNCTION__,
                        (int)data->InterSaiListNum);
            }
        }
    }
}

const UINT16 *ProtocolEmbmsSaiListAdapterLegacy::GetIntraSaiList() const
{
    if (m_saiList.IntraSaiListNum > 0)
        return m_saiList.IntraSaiList;
    return NULL;
}

const EMBMS_InterSaiList *ProtocolEmbmsSaiListAdapterLegacy::GetInterSaiList() const
{
    if (m_saiList.InterSaiListNum > 0)
        return m_saiList.InterSaiList;
    return NULL;
}
