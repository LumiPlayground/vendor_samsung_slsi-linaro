 /*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#include "miscdata.h"
#include "rillog.h"
#include <librilutils/textutils.h>

// add category to display selective logs
#undef RilLogI
#define RilLogI(format, ...) CRilLog::Log(RIL_LOG_CAT_SMS, CRilLog::E_RIL_INFO_LOG,  format, ##__VA_ARGS__)
#undef RilLogV
#define RilLogV(format, ...) CRilLog::Log(RIL_LOG_CAT_SMS, CRilLog::E_RIL_VERBOSE_LOG,  format, ##__VA_ARGS__)
#undef RilLogW
#define RilLogW(format, ...) CRilLog::Log(RIL_LOG_CAT_SMS, CRilLog::E_RIL_WARNING_LOG,  format, ##__VA_ARGS__)
#undef RilLogE
#define RilLogE(format, ...) CRilLog::Log(RIL_LOG_CAT_SMS, CRilLog::E_RIL_CRITICAL_LOG,  format, ##__VA_ARGS__)

using namespace android::hardware::radio::impl::network;

ApnSettingsData::ApnSettingsData
    (const int nReq, const Token tok, const ReqType type)
    :RequestData(nReq, tok, type)
{
    m_nApnReqType = 0;
}

ApnSettingsData::~ApnSettingsData()
{
}

void ApnSettingsData::SetReqType(int type)
{
    m_nApnReqType = type;
}

UINT8 ApnSettingsData::GetReqType()
{
    return m_nApnReqType;
}
INT32 ApnSettingsData::encode(char *data, unsigned int datalen)
{
    size_t i = 0;
    if (data == NULL || datalen == 0) {
        return -1;
    }

    ToString(data, datalen);
    const char *startWith[] = {
        "AT+VZWAPNE?",
        "AT+VZWAPNE=",
        "AT+VZWAPNE",
    };

    const char *apnTableKey[] = {
        "wapn",
        "apncl",
        "apnni",
        "apntype",
        "apnb",
        "apned",
        "apntime",
    };
    size_t size = sizeof(startWith) / sizeof(startWith[0]);

    for (i = 0; i < size; i++) {
        if (strncasecmp(data, startWith[i], strlen(startWith[i])) == 0) {
            SetReqType(i);
            break;
        }
    }

    if (GetReqType() == 1) // Set Command
    {
        size_t pos = strlen(startWith[1])-1, index = 0, p = 0;
        char buffer[2048] = {0, };
        memset(buffer, 0, sizeof(buffer));

        for (i = strlen(startWith[1]); i < strlen(data); ++i)
        {
            if (sizeof(apnTableKey)/sizeof(apnTableKey[0]) <= index) break;
            if (data[i] == '\n' || data[i] == '\r')
            {
                if (i-pos == 1)
                {
                    //RilLogE("buffer: null, key: %s", apnTableKey[index]);
                    m_ApnTable[apnTableKey[index++]] = "";
                } else {
                    //RilLogE("buffer: %s, key: %s", buffer, apnTableKey[index]);
                    m_ApnTable[apnTableKey[index++]] = buffer;
                }
                break;
            }
            else if (data[i] == ',')
            {
                if (i-pos == 1)
                {
                    //RilLogE("buffer: null, key: %s", apnTableKey[index]);
                    m_ApnTable[apnTableKey[index++]] = "";
                } else
                {
                    //RilLogE("buffer: %s, key: %s", buffer, apnTableKey[index]);
                    m_ApnTable[apnTableKey[index++]] = buffer;
                }
                p = 0;
                pos = i;
            }
            else {
                buffer[p++] = data[i];
                buffer[p] = 0;
            }
        }
    }

    return 0;
}

std::string ApnSettingsData::GetApnValues(char *key)
{
    string str = key;
    return m_ApnTable[str];
}

void ApnSettingsData::ToString(char *data, unsigned int datalen)
{
    static const char *CR = "<CR>";
    static const char *LF = "<LF>";
    static char buffer[2048] = {0, };

    if (data != NULL && datalen > 0) {
        size_t i = 0;
        size_t p = 0;
        memset(buffer, 0, sizeof(buffer));
        for (i = 0, p = strlen(buffer); i < datalen; ++i) {
            if (p + 4 > sizeof(buffer) - 1) {
                RilLogE("%s", buffer);
                *buffer = 0;
                p = 0;
            }
            if (data[i] == '\n') {
                strncat(buffer, LF, strlen(LF));
                p += strlen(LF);
            }
            else if (data[i] == '\r') {
                strncat(buffer, CR, strlen(CR));
                p += strlen(CR);
            }
            else {
                buffer[p++] = data[i];
                buffer[p] = 0;
            }
        }

        if (*buffer != 0) {
            RilLogE("%s", buffer);
        }
    }
}

/**
 * Sim IMSI ENCRYPTION
 */
CarrierInfoForImsiEncryptionData::CarrierInfoForImsiEncryptionData(const int nReq, const Token tok, const ReqType type) : RequestData(nReq, tok,type)
{
    memset(m_pMcc, 0x0, MAX_MCC_LEN + 1);
    memset(m_pMnc, 0x0, MAX_MCC_LEN + 1);
    m_nCarrierKeyLen  = 0;
    m_pCarrierKey = NULL;
    m_nKeyIdLen = 0;
    m_pKeyIdentifier = NULL;
    m_lEpirationTime = 0;
    m_nkeyType = 0;
}

CarrierInfoForImsiEncryptionData::~CarrierInfoForImsiEncryptionData()
{
    if (m_pCarrierKey != NULL) free(m_pCarrierKey);
    if (m_pKeyIdentifier != NULL) free(m_pKeyIdentifier);
}

INT32 CarrierInfoForImsiEncryptionData::encode(char *data, unsigned int length)
{
    int lengthStr;
    if((0 == length) || (NULL == data)) return -1;

    RIL_CarrierInfoForImsiEncryption_1_6 *pReqData = (RIL_CarrierInfoForImsiEncryption_1_6 *)data;

    if (pReqData->mcc != NULL) {
        lengthStr = strlen(pReqData->mcc);
        if(lengthStr > MAX_MCC_LEN) lengthStr = MAX_MCC_LEN;
        memcpy(m_pMcc, pReqData->mcc, lengthStr);
    }

    if (pReqData->mnc != NULL) {
        lengthStr = strlen(pReqData->mnc);
        if(lengthStr > MAX_MNC_LEN) lengthStr = MAX_MNC_LEN;
        memcpy(m_pMnc, pReqData->mnc, lengthStr);
    }

    if (pReqData->carrierKeyLength > 0 && pReqData->carrierKey != NULL) {
        m_nCarrierKeyLen = pReqData->carrierKeyLength;
        m_pCarrierKey = (BYTE *)calloc(m_nCarrierKeyLen+1, sizeof(BYTE));
        memcpy(m_pCarrierKey, pReqData->carrierKey, m_nCarrierKeyLen);
        m_pCarrierKey[m_nCarrierKeyLen] = 0;
    }

    if (pReqData->keyIdentifier != NULL) {
        m_nKeyIdLen = strlen(pReqData->keyIdentifier);
        m_pKeyIdentifier = (char *)calloc(m_nKeyIdLen+1, sizeof(char));
        memcpy(m_pKeyIdentifier, pReqData->keyIdentifier, m_nKeyIdLen);
        m_pKeyIdentifier[m_nKeyIdLen] = 0;
    }

    m_lEpirationTime = pReqData->expirationTime;

    int halVer = GetHalVersion();
    if (halVer < HAL_VERSION_CODE(1, 6)) {
        m_nkeyType = 0; // unknown
    } else {
        m_nkeyType = pReqData->keyType;
    }
    return 0;
}

SignalStrengthReportingCriteria::SignalStrengthReportingCriteria(const int nReq, const Token tok, const ReqType type) : RequestData(nReq, tok,type)
{
    m_hysteresisMs = 0;
    m_hysteresisDb = 0;
    m_numOfThresholdsDbm = 0;
    m_pThresholdsDbm = NULL;
    m_accessNetwork = 0;
    m_measureType = 0;
    m_isEnabled = false;
}

SignalStrengthReportingCriteria::~SignalStrengthReportingCriteria()
{
    if (m_pThresholdsDbm != NULL) delete[] m_pThresholdsDbm;
}

INT32 SignalStrengthReportingCriteria::encode(char *data, unsigned int length)
{
    if((0 == length) || (NULL == data)) return -1;
    int halVer = GetHalVersion();

    if (halVer < HAL_VERSION_CODE(1, 5)) {
        RIL_SignalStrengthReportingCriteria_V1_2 *pReqData = (RIL_SignalStrengthReportingCriteria_V1_2 *)data;
        m_hysteresisMs = pReqData->hysteresisMs;
        m_hysteresisDb = pReqData->hysteresisDb;
        m_numOfThresholdsDbm = pReqData->numOfThresholdsDbm;
        if (m_numOfThresholdsDbm > 0) {
            m_pThresholdsDbm = new int[m_numOfThresholdsDbm];
            for (int i = 0; i < m_numOfThresholdsDbm; i++) {
                m_pThresholdsDbm[i] = pReqData->thresholdsDbm[i];
            }
        }
        m_isEnabled = true;
        m_accessNetwork = pReqData->accessNetwork;
    } else {
        RIL_SignalStrengthReportingCriteria_V1_5 *pReqData = (RIL_SignalStrengthReportingCriteria_V1_5 *)data;
        m_measureType = pReqData->signalThresholdInfo.signalMeasurement;
        m_hysteresisMs = pReqData->signalThresholdInfo.hysteresisMs;
        m_hysteresisDb = pReqData->signalThresholdInfo.hysteresisDb;
        m_numOfThresholdsDbm = pReqData->signalThresholdInfo.len_thresholds;
        if (m_numOfThresholdsDbm > 0) {
            m_pThresholdsDbm = new int[m_numOfThresholdsDbm];
            for (int i = 0; i < m_numOfThresholdsDbm; i++) {
                m_pThresholdsDbm[i] = pReqData->signalThresholdInfo.thresholds[i];
            }
        }
        m_isEnabled = pReqData->signalThresholdInfo.isEnabled;
        m_accessNetwork = pReqData->accessNetwork;
    }

    return 0;
}

bool SignalStrengthReportingCriteria::isValidArguement() const {
    // test invalidHysteresisDb for VTS 1.2
    // hysteresisDb must be smaller than the smallest threshold delta
    // ASSERT_TRUE(MIN(thresholdsDbm[i+1] - thresholdsDbm[i]) >= hysteresisDb)
    // example
    // vector {-109, -103, -97, -89}  hysteresisDb 10
    //   : invalid argument. delta is 6
    if (m_numOfThresholdsDbm > 0 && m_pThresholdsDbm != NULL) {
        for (int i = 0; i < m_numOfThresholdsDbm - 1; i++) {
            if (m_hysteresisDb < 0 ||
                m_pThresholdsDbm[i] > m_pThresholdsDbm[i+1] ||
                m_pThresholdsDbm[i+1] - m_pThresholdsDbm[i] < m_hysteresisDb) {
                return false;
            }
        } // end for i ~
    }
    return true;
}

SignalStrengthReportingCriteria_V2_0::SignalStrengthReportingCriteria_V2_0(
    const int nReq, const Token tok, const ReqType type)
    : RequestData(nReq, tok,type) {}

INT32 SignalStrengthReportingCriteria_V2_0::encode(char *data, unsigned int length) {
    if (data == NULL || length == 0) {
        return -1;
    }

    size_t size = length / sizeof(V2_0::RIL_SignalThresholdInfo);
    V2_0::RIL_SignalThresholdInfo *infoPtrs = (V2_0::RIL_SignalThresholdInfo *)data;
    for (size_t i = 0; i < size; i++) {
        m_signalThresholdInfos.push_back(infoPtrs[i]);
    }
    return 0;
}

bool SignalStrengthReportingCriteria_V2_0::isValidArguements() const {
    for (auto& info : m_signalThresholdInfos) {
        if (!info.isValid())
            return false;
    }
    return true;
}

int SignalStrengthReportingCriteria_V2_0::GetSignalThresholdInfoNum() {
    return m_signalThresholdInfos.size();
}

const V2_0::RIL_SignalThresholdInfo& SignalStrengthReportingCriteria_V2_0::at(int pos) const {
    return m_signalThresholdInfos.at(pos);
}

LinkCapacityReportingCriteria::LinkCapacityReportingCriteria(const int nReq, const Token tok, const ReqType type) : RequestData(nReq, tok,type)
{
    m_hysteresisMs = 0;
    m_hysteresisDlKbps = 0;
    m_hysteresisUlKbps = 0;
    m_numOfThresholdsDownlinkKbps = 0;
    m_pThresholdsDownlinkKbps = NULL;
    m_numOfThresholdsUplinkKbps = 0;
    m_pThresholdsUplinkKbps = NULL;
    m_accessNetwork = 0;

}

LinkCapacityReportingCriteria::~LinkCapacityReportingCriteria()
{
    if (m_pThresholdsDownlinkKbps != NULL) delete[] m_pThresholdsDownlinkKbps;
    if (m_pThresholdsUplinkKbps != NULL) delete[] m_pThresholdsUplinkKbps;
}

INT32 LinkCapacityReportingCriteria::encode(char *data, unsigned int length)
{
    if((0 == length) || (NULL == data)) return -1;

    RIL_LinkCapacityReportingCriteria_V1_5 *pReqData = (RIL_LinkCapacityReportingCriteria_V1_5 *)data;
    m_hysteresisMs = pReqData->hysteresisMs;
    m_hysteresisDlKbps = pReqData->hysteresisDlKbps;
    m_hysteresisUlKbps = pReqData->hysteresisUlKbps;
    m_numOfThresholdsDownlinkKbps = pReqData->numOfThresholdsDownlinkKbps;
    if (m_numOfThresholdsDownlinkKbps > 0) {
        m_pThresholdsDownlinkKbps = new int[m_numOfThresholdsDownlinkKbps];
        for (int i = 0; i < m_numOfThresholdsDownlinkKbps; i++) {
            m_pThresholdsDownlinkKbps[i] = pReqData->thresholdsDownlinkKbps[i];
        }
    }
    m_numOfThresholdsUplinkKbps = pReqData->numOfThresholdsUplinkKbps;
    if (m_numOfThresholdsUplinkKbps > 0) {
        m_pThresholdsUplinkKbps = new int[m_numOfThresholdsUplinkKbps];
        for (int i = 0; i < m_numOfThresholdsUplinkKbps; i++) {
            m_pThresholdsUplinkKbps[i] = pReqData->thresholdsUplinkKbps[i];
        }
    }
    m_accessNetwork = pReqData->accessNetwork;

    return 0;
}

/**
 * SetActivateVsimReqData
 */
SetActivateVsimReqData::SetActivateVsimReqData(const int nReq, const Token tok, const ReqType type) : RequestData(nReq, tok,type)
{
    memset(&m_setActivateVsim, 0, sizeof(m_setActivateVsim));

    m_setActivateVsim.iccid = new char[MAX_ICCID_STRING_LEN+1];
    memset(m_setActivateVsim.iccid, 0, MAX_ICCID_STRING_LEN+1);

    m_setActivateVsim.imsi = new char[MAX_IMSI_LEN+1];
    memset(m_setActivateVsim.imsi, 0, MAX_IMSI_LEN+1);

    m_setActivateVsim.hplmn = new char[MAX_PLMN_LEN+1];
    memset(m_setActivateVsim.hplmn, 0, MAX_PLMN_LEN+1);

    m_setActivateVsim.simSlot= 1;
    m_setActivateVsim.vsimState= 0;
    m_setActivateVsim.vsimCardType = 1;
}

SetActivateVsimReqData::~SetActivateVsimReqData()
{
    if (m_setActivateVsim.iccid != NULL)
        delete[]  m_setActivateVsim.iccid;
    if (m_setActivateVsim.imsi != NULL)
        delete[]  m_setActivateVsim.imsi;
    if (m_setActivateVsim.hplmn != NULL)
        delete[]  m_setActivateVsim.hplmn;
}

INT32 SetActivateVsimReqData::encode(char *data, unsigned int datalen)
{
    if (data == NULL || datalen == 0) {
        return -1;
    }

    RIL_SetActivateVsim *rildata = (RIL_SetActivateVsim *)data;

    if (!TextUtils::IsEmpty(rildata->iccid)) {
        strncpy(m_setActivateVsim.iccid, rildata->iccid, MAX_ICCID_STRING_LEN);
    }

    if (!TextUtils::IsEmpty(rildata->imsi)) {
        strncpy(m_setActivateVsim.imsi, rildata->imsi, MAX_IMSI_LEN);
    }

    if (!TextUtils::IsEmpty(rildata->hplmn)) {
        strncpy(m_setActivateVsim.hplmn, rildata->hplmn, MAX_PLMN_LEN);
    }

    m_setActivateVsim.simSlot = rildata->simSlot;
    m_setActivateVsim.vsimState = rildata->vsimState;
    m_setActivateVsim.vsimCardType = rildata->vsimCardType;

    return 0;
}

int SetActivateVsimReqData::GetSlotId() const
{
    return m_setActivateVsim.simSlot;
}

const char *SetActivateVsimReqData::GetIccid() const
{
    if (*m_setActivateVsim.iccid == 0)
        return NULL;
    return m_setActivateVsim.iccid;
}

const char *SetActivateVsimReqData::GetImsi() const
{
    if (*m_setActivateVsim.imsi == 0)
        return NULL;
    return m_setActivateVsim.imsi;
}

const char *SetActivateVsimReqData::GetHomePlmn() const
{
    if (*m_setActivateVsim.hplmn == 0)
        return NULL;
    return m_setActivateVsim.hplmn;
}

int SetActivateVsimReqData::GetVsimState() const
{
    return m_setActivateVsim.vsimState;
}

int SetActivateVsimReqData::GetVsimCardType() const
{
    return m_setActivateVsim.vsimCardType;
}

/**
 * UICC Subscription
 */
UiccSubscription::UiccSubscription(const int nReq, const Token tok, const ReqType type) : RequestData(nReq, tok,type)
{
    m_nSlot = -1;
    m_nAppIndex = -1;
    m_nSubscriptionType = -1;
    m_nActivationStatus = -1;
}

UiccSubscription::~UiccSubscription()
{

}

UiccSubscription & UiccSubscription::operator=(const UiccSubscription &UiccSub)
{
    if (this == &UiccSub)
        return *this;
    m_nSlot = UiccSub.m_nSlot;
    m_nAppIndex = UiccSub.m_nAppIndex;
    m_nSubscriptionType = UiccSub.m_nSubscriptionType;
    m_nActivationStatus = UiccSub.m_nActivationStatus;

    return *this;
}

INT32 UiccSubscription::encode(char *data, unsigned int length)
{
    if((0 == length) || (NULL == data)) return -1;

    RIL_SelectUiccSub *pUiccSub = (RIL_SelectUiccSub *) data;
    /*DEADCODE
    if(NULL == pUiccSub) return -1;
    */

    m_nSlot = pUiccSub->slot;
    m_nAppIndex = pUiccSub->app_index;
    m_nSubscriptionType = pUiccSub->sub_type;
    m_nActivationStatus = pUiccSub->act_status;

    RilLogV("UiccSubscription::%s() Slot(%d), AppIndex(%d), SubscriptionType(%d), ActivationStatus(%d)", __FUNCTION__,
                                    m_nSlot, m_nAppIndex, m_nSubscriptionType, m_nActivationStatus);

    return 0;
}

