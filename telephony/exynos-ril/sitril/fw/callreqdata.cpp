/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#include "callreqdata.h"
#include "rillog.h"

#undef RilLogI
#define RilLogI(format, ...) CRilLog::Log(RIL_LOG_CAT_CALL, CRilLog::E_RIL_INFO_LOG,  format, ##__VA_ARGS__)
#undef RilLogV
#define RilLogV(format, ...) CRilLog::Log(RIL_LOG_CAT_CALL, CRilLog::E_RIL_VERBOSE_LOG,  format, ##__VA_ARGS__)
#undef RilLogW
#define RilLogW(format, ...) CRilLog::Log(RIL_LOG_CAT_CALL, CRilLog::E_RIL_WARNING_LOG,  format, ##__VA_ARGS__)
#undef RilLogE
#define RilLogE(format, ...) CRilLog::Log(RIL_LOG_CAT_CALL, CRilLog::E_RIL_CRITICAL_LOG,  format, ##__VA_ARGS__)

UusInfo::UusInfo()
{
    Clear();
}

void UusInfo::Clear()
{
    m_uusType = RIL_UUS_TYPE1_IMPLICIT;
    m_uusDcs = RIL_UUS_DCS_USP;
    m_uusLength = 0;
    memset(m_uusData, 0, sizeof(m_uusData));
}

CallDialReqData::CallDialReqData(const int nReq,const Token tok,const ReqType type)
:RequestData(nReq,tok,type)
{
    memset(m_number,0, sizeof(m_number));

    m_clirType = CLIR_DEFAULT;
    m_callType = CALL_TYPE_VOICE;
    m_eccCategory = 0;
}

INT32 CallDialReqData::encode(char *data, unsigned int datalen)
{
    if (NULL == data || 0 == datalen)
    {
        return -1;
    }

    RIL_Dial *dial = (RIL_Dial *)data;
    if (dial->address) {
        strncpy(m_number, dial->address, MAX_DIAL_NUM);
        m_number[MAX_DIAL_NUM] = '\0';
    }
    m_clirType = (ClirType)dial->clir;
    if (dial->uusInfo) {
        m_uusInfo.m_uusType = dial->uusInfo->uusType;
        m_uusInfo.m_uusDcs = dial->uusInfo->uusDcs;
        m_uusInfo.m_uusLength = dial->uusInfo->uusLength;
        if (dial->uusInfo->uusData) {
            memcpy(m_uusInfo.m_uusData, dial->uusInfo->uusData,
                    MIN(dial->uusInfo->uusLength, MAX_UUS_DATA_LEN));
        }
    }
    return 0;
}

CallEmergencyDialReqData::CallEmergencyDialReqData(const int nReq,const Token tok,const ReqType type)
    :CallDialReqData(nReq,tok,type)
{
    m_categories = 0;
    m_renUrns = 0;
    m_urns = NULL;
    m_routing = 0;
    m_hasKnownUserIntentEmergency = false;
    m_isTesting = false;
}

CallEmergencyDialReqData::~CallEmergencyDialReqData()
{
}

INT32 CallEmergencyDialReqData::encode(char *data, unsigned int datalen)
{
    if (NULL == data || 0 == datalen) {
        return -1;
    }

    if (datalen < sizeof(RIL_EmergencyDial)) {
        return -1;
    }

    RIL_EmergencyDial *emergencyDial = (RIL_EmergencyDial *)data;
    if (CallDialReqData::encode((char *)&emergencyDial->dialInfo, sizeof(RIL_Dial)) < 0) {
        return -1;
    }

    m_categories = emergencyDial->categories;
    m_routing = emergencyDial->routing;
    m_isTesting = emergencyDial->isTesting;

    return 0;
}

CallForwardReqData::CallForwardReqData(const int nReq, const Token tok, const ReqType type)
    :RequestData(nReq,tok,type)
{
    m_status = RIL_SS_MODE_DISABLE;
    m_reason = RIL_SS_CF_REASON_UNCONDITIONAL;
    m_classType = RIL_SS_CLASS_UNKNOWN;
    m_toa = 0;
    memset(m_number, 0x00, sizeof(m_number));
    m_timeSeconds = 0;
    m_serviceClass = 0;
}

INT32 CallForwardReqData::encode(char *data, unsigned int /* datalen */)
{
    RIL_CallForwardInfo *cf = (RIL_CallForwardInfo *)data;
    m_status = (SsModeType)cf->status;
    m_reason = (SsCfReason)cf->reason;
    m_classType = (SsClassX)cf->serviceClass;
    m_toa = cf->toa;

    memset(m_number, 0x00, sizeof(m_number));
    if (cf->number)
    {
        int len = MIN(strlen(cf->number), MAX_DIAL_NUM);
        strncpy(m_number, cf->number, len);
    }
    m_timeSeconds = cf->timeSeconds;

    return 0;
}

CallList::CallList()
{
    Clear();
}
CallList::~CallList()
{
}
void CallList::Clear()
{
    m_nCount = 0;
    for(int i=0;i<MAX_CALL_LIST_COUNT;i++)
        m_szCallInfo[i].Clear();
    //memset(m_szCallInfo, 0, sizeof(m_szCallInfo));
}

CallId::CallId()
{
    Init();
}

void CallId::Init()
{
    for (int i = 0; i < MAX_CALL_ID_COUNT; i++)
    {
        m_CallId[i].call_id = -1;
        m_CallId[i].state = CALL_ID_STATE_INACTIVE;
    }
}

int CallId::GetCpIndex(int ApId)
{
    if (0 < ApId && ApId <= MAX_CALL_ID_COUNT)
    {
        return m_CallId[ApId-1].call_id;
    }
    return -1;
}

void CallId::SyncReady()
{
    for (int i = 0; i < MAX_CALL_ID_COUNT; i++)
    {
        m_CallId[i].state = CALL_ID_STATE_INACTIVE;
    }
}

int CallId::AddCallId(int CpId)
{
    for (int i = 0; i < MAX_CALL_ID_COUNT; i++)
    {
        if (m_CallId[i].call_id == CpId)
        {
            m_CallId[i].state = CALL_ID_STATE_ACTIVE;
            return (i + 1);
        }
    }

    for (int i = 0; i < MAX_CALL_ID_COUNT; i++)
    {
        if (m_CallId[i].call_id == -1)
        {
            m_CallId[i].call_id = CpId;
            m_CallId[i].state = CALL_ID_STATE_ACTIVE;
            return (i + 1);
        }
    }
    return -1;
}

void CallId::SyncDone()
{
    for (int i = 0; i < MAX_CALL_ID_COUNT; i++)
    {
        RilLogV("[%d]th Call ID : %d, state(%d)", i + 1, m_CallId[i].call_id, m_CallId[i].state);
        if (m_CallId[i].state == CALL_ID_STATE_INACTIVE)
        {
            m_CallId[i].call_id = -1;
        }
    }
}

CallInfo::CallInfo()
{
    Clear();
}

void CallInfo::Clear()
{
    m_state = RIL_CALL_ACTIVE;
    m_nIndex = -1;
    m_toa = -1;
    m_isMParty = FALSE;
    m_isMt = FALSE;
    m_isVoice = FALSE;
    m_isVideo = FALSE;
    m_isVoicePrivacy = FALSE;
    m_numPresent = RIL_CALL_NAME_PRESENTATION_ALLOW;
    m_namePresent = RIL_CALL_NAME_PRESENTATION_ALLOW;
    m_als = -1;
    memset(m_number, 0, sizeof(m_number));
    memset(m_name, 0, sizeof(m_name));
    m_uusInfo.Clear();
    m_audioQuality = UNSPECIFIED;
}

EccInfo::EccInfo() {
    Clear();
}

EccInfo::~EccInfo() {
}

void EccInfo::SetCount(int count) {
    mCount = count;
}

int EccInfo::GetCount() {
    return mCount;
}

void EccInfo::SetEenlv(int eenlv) {
    mEenlv = eenlv;
}

int EccInfo::GetEenlv() {
    return mEenlv;
}

/**
 * Set MCC into PLMN member.
 *
 * @param mcc mobile country code.
 */
void EccInfo::SetMcc(char *mcc) {
    strncpy(mPlmn, mcc, MAX_MCC_LEN);
}

/**
 * Set MNC into PLMN member after MCC position.
 *
 * @param mnc mobile network code.
 * @param mncLen the length of MNC. It is 2 or 3.
 */
void EccInfo::SetMnc(char *mnc, int mncLen) {
    strncpy(mPlmn + MAX_MCC_LEN, mnc, mncLen);
    mPlmn[MAX_MCC_LEN + mncLen] = 0;
}

/**
 * Get the network PLMN.
 *
 * @return the network PLMN.
 */
char *EccInfo::GetPlmn() {
    return mPlmn;
}

/**
 * Get the emergency number list.
 *
 * @return the emergency number list.
 */
EmergencyNumber *EccInfo::GetEmergencyNumberList() {
    return mEmergencyNumberList;
}

/**
 * Get the emergency number at specific index.
 *
 * @param idx the index of emergency number list.
 * @return the emergency number at index.
 */
EmergencyNumber *EccInfo::GetEmergencyNumber(int idx) {
    return &mEmergencyNumberList[idx];
}

/**
 * Reset all variables.
 */
void EccInfo::Clear() {
    mCount = 0;
    for(int i=0;i<MAX_EMERGENCY_NUMBER_LIST_COUNT;i++)
        mEmergencyNumberList[i].Clear();
    mEenlv = 0;
    memset(mPlmn, 0, sizeof(mPlmn));
}

/**
 * Add emergency number with params at tail of emergency number list.
 */
void EccInfo::AddEmergencyNumber(const char *mcc, const char *mnc, const char *number, int numberLen,
                                 int category, int conditions, int source) {
    this->AddEmergencyNumber(mcc, mnc, number, numberLen, NULL, 0, category, conditions, source);
}

/**
 * Add emergency number with params including URN at tail of emergency number list.
 */
void EccInfo::AddEmergencyNumber(const char *mcc, const char *mnc, const char *number, int numberLen,
                                 const char *urn, int urnLen, int category, int conditions, int source) {
    if (mCount >= MAX_EMERGENCY_NUMBER_LIST_COUNT) {
        RilLogE("AddEmergencyNumber fail: max count.");
        return;
    }
    mEmergencyNumberList[mCount++].Add(mcc, mnc, number, numberLen, urn, urnLen,
                                       category, conditions, source);
}

/**
 * Reset all variables.
 */
void EmergencyNumber::Clear() {
    memset(mMcc, 0, sizeof(mMcc));
    memset(mMnc, 0, sizeof(mMnc));
    memset(mNumber, 0, sizeof(mNumber));
    memset(mUrn, 0, sizeof(mUrn));
    memset(mUrnP, 0, sizeof(mUrnP));
    mNumberLen = 0;
    mUrnCount = 0;
    mSource = -1;
    mCategory = -1;
    mConditions = -1;
    mIsEmergencyType = false;
    mIsEmergencyCallRouting = false;
}

/**
 * Update emergency number informations with params.
 */
void EmergencyNumber::Add(const char *mcc, const char *mnc, const char *number, int numberLen,
                          int category, int conditions, int source) {
    this->Add(mcc, mnc, number, numberLen, NULL, 0, category, conditions, source);
}

/**
 * Update emergency number informations with params including URN.
 */
void EmergencyNumber::Add(const char *mcc, const char *mnc, const char *number, int numberLen,
                          const char *urn, int urnLen, int category, int conditions, int source) {
    int len = MIN(strlen(mcc), sizeof(mMcc) - 1);
    strncpy(mMcc, mcc, len);
    mMcc[len] = 0;
    len = MIN(strlen(mnc), sizeof(mMnc) - 1);
    strncpy(mMnc, mnc, len);
    mMnc[len] = 0;
    numberLen = MIN(numberLen, MAX_EMERGENCY_NUMBER_LEN);
    strncpy(mNumber, number, numberLen);
    mNumber[numberLen] = 0;
    mNumberLen = numberLen;
    mCategory = category;
    mConditions = conditions;
    mSource = source;
    urnLen = MIN(urnLen, MAX_URN_LEN);
    this->AddUrn(urn, urnLen);
    mIsEmergencyType = true;
    mIsEmergencyCallRouting = true;
}

/**
 * Add URN at tail position.
 */
void EmergencyNumber::AddUrn(const char *urn, int urnLen) {
    if (MAX_URN_COUNT == mUrnCount) {
        RilLogE("AddUrn fail: max URN.");
        return;
    }

    if (urnLen > 0) {
        strncpy(mUrn[mUrnCount], urn, urnLen);
        mUrn[mUrnCount][urnLen] = 0;
        mUrnCount++;
    }
}

/**
 * Remove URN by URN string.
 */
void EmergencyNumber::RemoveUrn(const char *urn) {
    int i, j = 0;
    list<string> list;
    for (i = 0; i < mUrnCount; i++) {
        if (strncmp(mUrn[i], urn, strlen(urn)) == 0) {
            RilLogI("RemoveUrn matched.");
            mUrnCount--;
        } else {
            list.push_back(mUrn[i]);
        }
    }
    memset(mUrn, 0, sizeof(mUrn));

    for (const string s : list) {
        strncpy(mUrn[j], s.c_str(), s.length());
        mUrn[j++][s.length()] = 0;
    }
}

char *EmergencyNumber::GetMcc() {
    return mMcc;
}

char *EmergencyNumber::GetMnc() {
    return mMnc;
}

void EmergencyNumber::SetNumber(const char *number, int len) {
    memcpy(mNumber, number, len);
    mNumber[len] = 0;
}

char *EmergencyNumber::GetNumber() {
    return mNumber;
}

void EmergencyNumber::SetNumberLen(int numberLen) {
    mNumberLen = numberLen;
}

int EmergencyNumber::GetNumberLen() {
    return mNumberLen;
}

void EmergencyNumber::SetSource(int source) {
    mSource = source;
}

int EmergencyNumber::GetSource() {
    return mSource;
}

void EmergencyNumber::SetCategory(int category) {
    mCategory = category;
}

int EmergencyNumber::GetCategory() {
    return mCategory;
}

int EmergencyNumber::GetUrnCount() {
    return mUrnCount;
}

char *EmergencyNumber::GetUrn(int idx) {
    return mUrn[idx];
}

char **EmergencyNumber::GetUrns() {
    return mUrnP;
}

int EmergencyNumber::GetConditions() {
    return mConditions;
}

/**
 * Set as emergency type regarding conditions.
 * This flag should be set to update emergency number list.
 *
 * @param isEmergencyType whether if it is emergency number.
 */
void EmergencyNumber::SetEmergencyType(bool isEmergencyType) {
    mIsEmergencyType = isEmergencyType;
}

bool EmergencyNumber::IsEmergencyType() {
    return mIsEmergencyType;
}

void EmergencyNumber::SetEmergencyCallRouting(bool isEmergencyRouting) {
    mIsEmergencyCallRouting = isEmergencyRouting;
}

bool EmergencyNumber::IsEmergencyCallRouting() {
    return mIsEmergencyCallRouting;
}

DtmfInfo::DtmfInfo(const int nReq, const Token tok, const ReqType type)
    :RequestData(nReq,tok,type)
{
    memset(m_szDtmf, 0, sizeof(m_szDtmf));
}

DtmfInfo::DtmfInfo(const int nReq, const Token tok, const char dtmf)
    :RequestData(nReq,tok)
{
    memset(m_szDtmf, 0, sizeof(m_szDtmf));
    m_szDtmf[0] = dtmf;
}

DtmfInfo::DtmfInfo(const int nReq, const Token tok, const char* dtmf)
    :RequestData(nReq,tok)
{
    memset(m_szDtmf, 0, sizeof(m_szDtmf));

    if (dtmf == NULL)
    {
        return;
    }

    int len = MIN(strlen(dtmf), sizeof(m_szDtmf) - 1);
    strncpy(m_szDtmf, dtmf, len);
}
