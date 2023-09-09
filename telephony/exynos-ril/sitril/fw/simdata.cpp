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
 * simdata.cpp
 *
 *  Created on: 2014. 6. 28.
 *      Author: mox
 */

#include "simdata.h"
#include "util.h"
#include "rillog.h"

// add category to display selective logs
#undef RilLogI
#define RilLogI(format, ...) CRilLog::Log(RIL_LOG_CAT_SIM, CRilLog::E_RIL_INFO_LOG,  format, ##__VA_ARGS__)
#undef RilLogV
#define RilLogV(format, ...) CRilLog::Log(RIL_LOG_CAT_SIM, CRilLog::E_RIL_VERBOSE_LOG,  format, ##__VA_ARGS__)
#undef RilLogW
#define RilLogW(format, ...) CRilLog::Log(RIL_LOG_CAT_SIM, CRilLog::E_RIL_WARNING_LOG,  format, ##__VA_ARGS__)
#undef RilLogE
#define RilLogE(format, ...) CRilLog::Log(RIL_LOG_CAT_SIM, CRilLog::E_RIL_CRITICAL_LOG,  format, ##__VA_ARGS__)

/**
 * SIM IO
 */
SimIoData::SimIoData(const int nReq, const Token tok, const ReqType type)
    : RequestData(nReq, tok, type),
    m_nCmd(0), m_nFileId(0), m_nP1(0), m_nP2(0), m_nP3(0), m_nDataLen(0), m_strHex(NULL)
{
    memset(m_strData, 0, sizeof(m_strData));
    memset(m_strPin2, 0, sizeof(m_strPin2));
    memset(m_strPath, 0, sizeof(m_strPath));
    memset(m_strAid, 0, sizeof(m_strAid));
}

SimIoData::SimIoData(const SimIoData &simIo)
    : RequestData(simIo.m_nReq, simIo.m_tok, simIo.m_reqType)
{
    m_nCmd = simIo.m_nCmd;
    m_nFileId = simIo.m_nFileId;
    m_nP1 = simIo.m_nP1;
    m_nP2 = simIo.m_nP2;
    m_nP3 = simIo.m_nP3;

    memcpy(m_strPath, simIo.m_strPath, sizeof(m_strPath));
    m_nDataLen = simIo.m_nDataLen;
    memcpy(m_strData, simIo.m_strData, sizeof(m_strData));
    memcpy(m_strPin2, simIo.m_strPin2, sizeof(m_strPin2));
    memcpy(m_strAid, simIo.m_strAid, sizeof(m_strAid));
    m_strHex = nullptr;
}

SimIoData::~SimIoData()
{
    if (m_strHex)
    {
        delete [] m_strHex;
        m_strHex = NULL;
    }
}

SimIoData & SimIoData::operator=(const SimIoData &simIo)
{
    if (&simIo != this)
    {
        m_nReq = simIo.m_nReq;
        m_tok = simIo.m_tok;
        m_reqType = simIo.m_reqType;

        m_nCmd = simIo.m_nCmd;
        m_nFileId = simIo.m_nFileId;
        m_nP1 = simIo.m_nP1;
        m_nP2 = simIo.m_nP2;
        m_nP3 = simIo.m_nP3;

        memcpy(m_strPath, simIo.m_strPath, sizeof(m_strPath));
        m_nDataLen = simIo.m_nDataLen;
        memcpy(m_strData, simIo.m_strData, sizeof(m_strData));
        memcpy(m_strPin2, simIo.m_strPin2, sizeof(m_strPin2));
        memcpy(m_strAid, simIo.m_strAid, sizeof(m_strAid));

        if (m_strHex)
        {
            delete [] m_strHex;
            m_strHex = NULL;
        }
    }
    return *this;
}

void SimIoData::SetRawData(char * /* hexStr */) {
    if (m_strHex)
    {
        delete [] m_strHex;
        m_strHex = NULL;
    }

    // ::ConvertToRaw(hexStr, mData, &mLen);
}

INT32 SimIoData::encode(char *data, unsigned int length)
{
    if((0 == length) || (NULL == data))
        return -1;

    RIL_SIM_IO_v6 *pSimIo = (RIL_SIM_IO_v6 *)data;
    /* DEADCODE <- data cannot be null at this point
    if(NULL == pSimIo)
        return -1;
    */
    m_nCmd = pSimIo->command;
    m_nFileId = pSimIo->fileid;
    RilLogV("[%s,%s] is called, [path==NULL] %d, [data==NULL] %d, [PIN2 == NULL] %d", __FILE__, __FUNCTION__, (NULL==pSimIo->path), (NULL == pSimIo->data), (NULL == pSimIo->pin2));
    //strncpy(m_strPath, pSimIo->path, sizeof(m_strPath) - 1);
    if(pSimIo->path && strlen(pSimIo->path))
        strncpy(m_strPath, pSimIo->path, sizeof(m_strPath) - 1);
    m_nP1 = pSimIo->p1;
    m_nP2 = pSimIo->p2;
    m_nP3 = pSimIo->p3;
    if(pSimIo->data && strlen(pSimIo->data))
        strncpy(m_strData, pSimIo->data, sizeof(m_strData) - 1);
    m_nDataLen = strlen(m_strData);
    //m_nDataLen = (strlen(m_strData)>MAX_SIM_DATA_LEN)? MAX_SIM_DATA_LEN: strlen(m_strData);

    //strncpy(m_strPin2, pSimIo->pin2, sizeof(m_strPin2) - 1);
    if(pSimIo->pin2 && strlen(pSimIo->pin2))
        strncpy(m_strPin2, pSimIo->pin2, sizeof(m_strPin2) - 1);

    if(pSimIo->aidPtr && strlen(pSimIo->aidPtr))
        strncpy(m_strAid, pSimIo->aidPtr, sizeof(m_strAid) - 1);

    return 0;
}

/**
 * Facility Lock
 */
FacilityLock::FacilityLock()
{
    memset(m_szCode, 0, sizeof(m_szCode));
    m_nLockState = SIM_FAC_LOCK_STATE_UNLOCK;
    m_nServiceClass = SIM_FAC_SERVICE_CLASS_BIT_NONE;
    memset(m_szPassword, 0, sizeof(m_szPassword));
    memset(m_szAID, 0, sizeof(m_szAID));
}

BOOL FacilityLock::Parse(StringsRequestData &strReqData)
{
    RilLogI("FacilityLock::%s() [<--", __FUNCTION__);

    char **ppStrings = strReqData.GetStringsContent();
    int nStrCount = strReqData.GetStringCount();

    strncpy(m_szCode, ppStrings[0], MAX_FACILITY_CODE_LEN);
    m_szCode[MAX_FACILITY_CODE_LEN] = '\0';
    RilLogV("FacilityLock::%s() FAC:%s", __FUNCTION__, m_szCode);
    RilLogV("FacilityLock::%s() String Count:%d", __FUNCTION__, nStrCount);

    char *pszPassword = NULL;
    char *pszServiceClass = NULL;
    char *pszAID = NULL;

    // Get
    if(nStrCount==4)
    {
        pszPassword = ppStrings[1];
        pszServiceClass = ppStrings[2];
        pszAID = ppStrings[3];
        RilLogV("FacilityLock::%s() [GET] Password:%s, ServiceClass:%s, AID:%s", __FUNCTION__, pszPassword, pszServiceClass, pszAID);
    }
    // Set
    else if(nStrCount==5)
    {
        m_nLockState = strtol(ppStrings[1], NULL, 10);
        pszPassword = ppStrings[2];
        pszServiceClass = ppStrings[3];
        pszAID = ppStrings[4];
        RilLogV("FacilityLock::%s() [SET] LockState:%d, Password:%s, ServiceClass:%s, AID:%s", __FUNCTION__, m_nLockState, pszPassword, pszServiceClass, pszAID);
    }
    else
    {
        RilLogE("FacilityLock::%s() return FALSE [-->", __FUNCTION__);
        return FALSE;
    }

    if(pszPassword) memcpy(m_szPassword, pszPassword, strlen(pszPassword));
    m_nServiceClass = strtol(pszServiceClass, NULL, 10);
    if(pszAID) memcpy(m_szAID, pszAID, strlen(pszAID));

    RilLogV("FacilityLock::%s() [-->", __FUNCTION__);
    return TRUE;
}

/**
 * ISIM Authentication
 */
IsimAuth::IsimAuth()
{
    m_nLength = 0;
    m_pAuth = NULL;
    m_nAuthType = -1;
}

IsimAuth::~IsimAuth()
{
    Erase();
}

BOOL IsimAuth::Parse(StringRequestData *pString)
{
    if(pString==NULL || pString->GetString()==NULL) return FALSE;

    Erase();
    RilLogV("IsimAuth::%s() Input:%s", __FUNCTION__, pString->GetString());
    PrintBufferDump("Input", (const BYTE *) pString->GetString(), strlen(pString->GetString()));
    BYTE *pszNonce = NULL;
    int nNonceLength = Base64_Decode((BYTE **) &pszNonce, pString->GetString());
    PrintBufferDump("Base64 Decoded", (const BYTE *) pszNonce, nNonceLength);

    int nRandLength = 16;
    int nAutnLength = 16;
    BYTE *pRand = &pszNonce[0];
    BYTE *pAutn = &pszNonce[nRandLength];

    PrintBufferDump("RAND", pRand, nRandLength);
    PrintBufferDump("AUTN", pAutn, nAutnLength);

    m_nLength = 1 + nRandLength + 1 + nAutnLength;
    m_pAuth = new BYTE[m_nLength+1];

    // Must use memcpy() for NULL value on decoded value from BASE64
    //memcpy(m_pAuth, szRandLen, sizeof(szRandLen));
    m_pAuth[0] = nRandLength;
    memcpy(&m_pAuth[1], pRand, nRandLength);
    //memcpy(&m_pAuth[1+nRandLength], szAutnLen, sizeof(szAutnLen));
    m_pAuth[1+nRandLength] = nAutnLength;
    memcpy(&m_pAuth[1+nRandLength+1], pAutn, nAutnLength);
    //m_pAuth[1+nRandLength+1+nAutnLength] = '\0';
    RilLogV("IsimAuth::%s() Length:%d", __FUNCTION__, m_nLength);

    //m_pAuth = new BYTE[strlen(pszEncoded)/2];
    //m_nLength = HexString2Value(m_pAuth, pszEncoded);

    delete [] pszNonce;

    return TRUE;
}

BOOL IsimAuth::ParseWithAuthType(const char *pString)
{
    if(pString==NULL) return FALSE;

    Erase();
    RilLogV("IsimAuth::%s() Input:%s", __FUNCTION__, pString);
    PrintBufferDump("Input", (const BYTE *) pString, strlen(pString));
    BYTE *pszNonce = NULL;
    int nNonceLength = Base64_Decode((BYTE **) &pszNonce, pString);
    PrintBufferDump("Base64 Decoded", (const BYTE *) pszNonce, nNonceLength);

    int nRandLength = 16;
    int nAutnLength = 16;
    int nAuthType = pszNonce[0];
    BYTE *pRand = &pszNonce[1];
    BYTE *pAutn = &pszNonce[nRandLength+1];

    PrintBufferDump("RAND", pRand, nRandLength);
    PrintBufferDump("AUTN", pAutn, nAutnLength);

    m_nLength = 1 + nRandLength + 1 + nAutnLength;
    m_pAuth = new BYTE[m_nLength+1];
    m_nAuthType = nAuthType;

    // Must use memcpy() for NULL value on decoded value from BASE64
    //memcpy(m_pAuth, szRandLen, sizeof(szRandLen));
    m_pAuth[0] = nRandLength;
    memcpy(&m_pAuth[1], pRand, nRandLength);
    //memcpy(&m_pAuth[1+nRandLength], szAutnLen, sizeof(szAutnLen));
    m_pAuth[1+nRandLength] = nAutnLength;
    memcpy(&m_pAuth[1+nRandLength+1], pAutn, nAutnLength);
    //m_pAuth[1+nRandLength+1+nAutnLength] = '\0';
    RilLogV("IsimAuth::%s() Length:%d", __FUNCTION__, m_nLength);

    //m_pAuth = new BYTE[strlen(pszEncoded)/2];
    //m_nLength = HexString2Value(m_pAuth, pszEncoded);

    delete [] pszNonce;

    return TRUE;
}

void IsimAuth::Erase(void)
{
    if(m_pAuth!=NULL)
    {
        delete [] m_pAuth;
        m_pAuth = NULL;
    }
}

/**
 * SIM Authentication
 */
SimAuthentication::SimAuthentication(const int nReq, const Token tok, const ReqType type)
    : RequestData(nReq, tok,type),
      m_nAuthContext(0), m_pAuth(NULL), m_pAid(NULL)
{

}

SimAuthentication::~SimAuthentication()
{
    if (m_pAuth) {
        delete [] m_pAuth;
        m_pAuth = NULL;
    }
    if (m_pAid) {
        delete [] m_pAid;
        m_pAid = NULL;
    }
}

INT32 SimAuthentication::encode(char *data, unsigned int length)
{
    if((0 == length) || (NULL == data)) return -1;

    RIL_SimAuthentication *pSimAuth = (RIL_SimAuthentication *) data;
    int nLength = 0;
    m_nAuthContext = pSimAuth->authContext;
    if (pSimAuth->authData != NULL) {
        nLength = strlen(pSimAuth->authData);
        if (nLength > 0) {
            m_pAuth = new char[nLength + 1];
            memset(m_pAuth, 0, nLength + 1);
            strncpy(m_pAuth, pSimAuth->authData, nLength);
        }
    }

    if (pSimAuth->aid != NULL) {
        nLength = strlen(pSimAuth->aid);
        if (nLength > 0) {
            m_pAid = new char[nLength + 1];
            memset(m_pAid, 0, nLength + 1);
            strncpy(m_pAid, pSimAuth->aid, nLength);
        }
    }
    return TRUE;
}

SimGbaAuth::SimGbaAuth()
{
    m_nLength = 0;
    m_pAuth = NULL;
    m_gba_type = 0;
    m_gba_tag = 0;
}
SimGbaAuth::~SimGbaAuth()
{
    Erase();
}
BOOL SimGbaAuth::Parse(StringRequestData *pString)
{
    if(pString==NULL || pString->GetString()==NULL) return FALSE;
    Erase();
    const char *req_auth = pString->GetString();
    m_gba_type = (*req_auth) - 0x30 ;         // 0x00(Bootstrapping), 0x01(NAF)
    m_gba_tag = *(req_auth+1) - 0x30;      // 0x02(DD), 0x03(DE)
    req_auth = (req_auth+2) ;
    if( m_gba_type == 0x0)      //ISIM,USIM Bootstrapping
    {
        RilLogV("GBA Bootstrapping IsimAuth::%s() Input:%s", __FUNCTION__, req_auth);
        PrintBufferDump("Input", (const BYTE *) req_auth, strlen(req_auth));
        BYTE *pszNonce = NULL;
        int nNonceLength = Base64_Decode((BYTE **) &pszNonce, req_auth);
        PrintBufferDump("Base64 Decoded", (const BYTE *) pszNonce, nNonceLength);
        int nRandLength = 16;
        int nAutnLength = 16;
        BYTE *pRand = &pszNonce[0];
        BYTE *pAutn = &pszNonce[nRandLength];
        PrintBufferDump("RAND", pRand, nRandLength);
        PrintBufferDump("AUTN", pAutn, nAutnLength);
#if 0       //org
        m_nLength = 1 + nRandLength + 1 + nAutnLength;
        m_pAuth = new BYTE[m_nLength+1];
        m_pAuth[0] = nRandLength;
        memcpy(&m_pAuth[1], pRand, nRandLength);
        m_pAuth[1+nRandLength] = nAutnLength;
        memcpy(&m_pAuth[1+nRandLength+1], pAutn, nAutnLength);
        RilLogV("IsimAuth::%s() Length:%d", __FUNCTION__, m_nLength);
#else      //GBA
        int data_len = 255;
        m_nLength = 1 + data_len + 1 + data_len;
        m_pAuth = new BYTE[m_nLength+1];
        m_pAuth[0] = nRandLength;
        memcpy(&m_pAuth[1], pRand, nRandLength);
        m_pAuth[1+data_len] = nAutnLength;
        memcpy(&m_pAuth[1+data_len+1], pAutn, nAutnLength);
        RilLogV("SimGbaAuth::%s() Length:%d", __FUNCTION__, m_nLength);
#endif
        delete [] pszNonce;
    }
    else    //0x01, NAF
    {
        RilLogV("GBA NAF IsimAuth::%s() Input:%s", __FUNCTION__, req_auth);
        PrintBufferDump("Input", (const BYTE *) req_auth, strlen(req_auth));
        BYTE *pszNonce = NULL;
        int nNonceLength = Base64_Decode((BYTE **) &pszNonce, req_auth);
        PrintBufferDump("Base64 Decoded", (const BYTE *) pszNonce, nNonceLength);
        int nRandLength = 16;
        int nAutnLength = 16;
        BYTE *pRand = &pszNonce[0];
        BYTE *pAutn = &pszNonce[nRandLength];
        PrintBufferDump("RAND", pRand, nRandLength);
        PrintBufferDump("AUTN", pAutn, nAutnLength);
        m_nLength = 1 + nRandLength + 1 + nAutnLength;
        m_pAuth = new BYTE[m_nLength+1];
        m_pAuth[0] = nRandLength;
        memcpy(&m_pAuth[1], pRand, nRandLength);
        m_pAuth[1+nRandLength] = nAutnLength;
        memcpy(&m_pAuth[1+nRandLength+1], pAutn, nAutnLength);
        RilLogV("IsimAuth::%s() Length:%d", __FUNCTION__, m_nLength);
        delete [] pszNonce;
    }
    return TRUE;
}
void SimGbaAuth::Erase(void)
{
    if(m_pAuth!=NULL)
    {
        delete [] m_pAuth;
        m_pAuth = NULL;
    }
}

/**
 * Sim APDU
 */
SimAPDU::SimAPDU(const int nReq, const Token tok, const ReqType type) : RequestData(nReq, tok,type)
{
    m_nSessionid = -1;
    m_nCla = -1;
    m_nInstruction = -1;
    m_nP1 = -1;
    m_nP2 = -1;
    m_nP3 = -1;
    m_nDataLength = -1;
    m_pData = NULL;
}

SimAPDU::~SimAPDU()
{
    if(m_pData!=NULL)
    {
        delete [] m_pData;
        m_pData = NULL;
    }
}

void SimAPDU::SetP3(int p3)
{
    m_nP3 = p3;
}

INT32 SimAPDU::encode(char *data, unsigned int length)
{
    if((0 == length) || (NULL == data)) return -1;

    RIL_SIM_APDU *pSimAPDU = (RIL_SIM_APDU *)data;

    if(m_pData!=NULL)
    {
        delete [] m_pData;
        m_pData = NULL;
    }

    m_nSessionid = pSimAPDU->sessionid;
    m_nCla = pSimAPDU->cla;
    m_nInstruction = pSimAPDU->instruction;
    m_nP1 = pSimAPDU->p1;
    m_nP2 = pSimAPDU->p2;
    m_nP3 = pSimAPDU->p3;
    m_nDataLength = (pSimAPDU->data!=NULL) ? strlen(pSimAPDU->data) : 0;

    if(0 < m_nDataLength) {
        m_pData = new char[m_nDataLength+1];
        if(m_pData!=NULL) {
            memset(m_pData, 0x00, m_nDataLength+1);
            memcpy(m_pData, pSimAPDU->data, m_nDataLength);
        }
    }

    RilLogV("SimAPDU::%s() Sessionid(%d), Cla(%d), Instruction(%d), P1(%d), P2(%d), P3(%d), DataLength(%d), Alloc size(%d)", __FUNCTION__,
                                    m_nSessionid, m_nCla, m_nInstruction, m_nP1, m_nP2, m_nP3, m_nDataLength, m_nDataLength+1);

    return 0;
}

/* Update PhoneBook */
SimUpdatePbEntry::SimUpdatePbEntry(const int nReq, const Token tok, const ReqType type) : RequestData(nReq, tok,type)
{
    m_mode = -1;
    m_type = -1;
    m_recordId = -1;
    m_length = 0;
    memset(m_entry, 0, sizeof(m_entry));
}

SimUpdatePbEntry::~SimUpdatePbEntry()
{
}

INT32 SimUpdatePbEntry::encode(char *data, unsigned int length)
{
    if((length <= 0) || (NULL == data)) return -1;

    RIL_UpdatePbEntry *pb = (RIL_UpdatePbEntry *)data;
    RIL_3GPb *p;
    int offset = 0, data_len = 0;
    int pb_cnt = pb->pb3gCnt;

    m_mode = pb->mode;
    /* delete all */
    if (pb->mode == 4) {
        RilLogE("SimUpdatePbEntry::%s, all delete is not implemented",__FUNCTION__);
        return -1;
    }

    /* delete */
    if (pb->mode == 2) {
        m_type = pb->type;
        m_recordId = pb->recordId;
        RilLogV("SimUpdatePbEntry::%s, mode=%d, type=%d, recordId=%d", __FUNCTION__, m_mode, m_type, m_recordId);
        return 0;
    }

    if (pb_cnt == 0) {
        RilLogE("SimUpdatePbEntry::%s, mode=%d, Invalid pb_cnt:%d", __FUNCTION__, m_mode, pb_cnt);
        return 0;
    }

    m_type = pb->type;
    m_recordId = pb->recordId;

    RilLogV("SimUpdatePbEntry::%s, mode=%d, type=%d, recordId=%d, length=%d", __FUNCTION__,
            m_mode, m_type, m_recordId, pb->length);

    switch(m_type) {
    case PB_ADN_3G:
        p = (RIL_3GPb *) pb->pb3g;
        for (int i = 0; i < pb_cnt; i++) {
            m_entry[offset++] = p[i].type3g & 0xFF;
            data_len = p[i].data_len;
            m_entry[offset++] = data_len & 0xFF;
            m_entry[offset++] = (data_len >> 8) & 0xFF;
            m_entry[offset++] = p[i].data_type & 0xFF;
            if (data_len > (MAX_PB_ENTRY_LEN - offset)) {
                RilLogE("SimUpdatePbEntry::%s, pb data len is too big:%d", __FUNCTION__, data_len);
                return -1;
            }
            memcpy(m_entry + offset, p[i].data, p[i].data_len);
            offset += p->data_len;
            RilLogV("SimUpdatePbEntry::encode, offset:%d, current index:%d", offset, i);
        }
        m_length = offset;
        break;
    default:
        break;
    }

    return 0;
}

/**
 * Sim Open Channel
 */
SimOpenChannel::SimOpenChannel(const int nReq, const Token tok, const ReqType type) : RequestData(nReq, tok, type)
{
   m_aid = NULL;
   m_length = 0;
   m_p2 = 0;
}

SimOpenChannel::~SimOpenChannel()
{
   if (m_aid != NULL) delete[] m_aid;
}

INT32 SimOpenChannel::encode(char * data, unsigned int length)
{
    if (data == NULL || length == 0 || length != sizeof(RIL_OpenChannelParams)) {
        return -1;
    }

    RIL_OpenChannelParams *pOpenChannel = (RIL_OpenChannelParams *)data;
    if (pOpenChannel->aidPtr != NULL)
    {
        m_length = strlen(pOpenChannel->aidPtr);
        if (m_length > 0) {
            m_aid = new char[m_length + 1];
            memset(m_aid, 0, m_length + 1);
            strncpy(m_aid, pOpenChannel->aidPtr, m_length);
        }
    }
    m_p2 = pOpenChannel->p2;

    return 0;
}

/**
 * Sim Open Channel With P2
 */
SimOpenChannelWithP2::SimOpenChannelWithP2(const int nReq, const Token tok, const ReqType type) : RequestData(nReq, tok,type)
{
    m_nP2 = -1;
    m_nAidLen = -1;
    memset(m_pAid, 0x00, MAX_SIM_AID_LEN);
}

SimOpenChannelWithP2::~SimOpenChannelWithP2()
{
}

INT32 SimOpenChannelWithP2::encode(char *data, unsigned int length)
{
    if((0 == length) || (NULL == data)) return -1;

    RIL_OpenChannelParams *pSimOpenChannel = (RIL_OpenChannelParams*)data;

    if(pSimOpenChannel->aidPtr != NULL && strlen(pSimOpenChannel->aidPtr)>MAX_SIM_AID_LEN)
    {
        RilLogE("SimOpenChannelWithP2::%s() Error: AID length is incorrect, Length(%d) is over Max Size(%d)", __FUNCTION__,
                                                                     strlen(pSimOpenChannel->aidPtr), MAX_SIM_AID_LEN);
        return -1;
    }

    m_nP2 = pSimOpenChannel->p2;

    if (pSimOpenChannel->aidPtr != NULL)
    {
        m_nAidLen = strlen(pSimOpenChannel->aidPtr);
        memcpy(m_pAid, pSimOpenChannel->aidPtr, m_nAidLen);
    }
    else
    {
        m_nAidLen = 0;
        memset(m_pAid, 0x0, MAX_SIM_AID_LEN);
    }

    RilLogV("SimOpenChannelWithP2::%s() P2(%d)", __FUNCTION__, m_nP2);

    return 0;

}

/**
 * SIM CARRIER_RESTRICTIONS
 */
CarrierRestrictionsData::CarrierRestrictionsData(const int nReq, const Token tok, const ReqType type) : RequestData(nReq, tok,type)
{
    m_nAllowedCarrierLength = 0;
    m_nExcludedCarrierLength = 0;
    m_pAllowedCarriers = NULL;
    m_pExcludedCarriers = NULL;
    m_bAllowedCarriersPrioritized = false;
    m_nMultiSimPolicy = 0;
}

CarrierRestrictionsData::~CarrierRestrictionsData()
{
    if (m_pAllowedCarriers != NULL) {
        delete[] m_pAllowedCarriers;
        m_pAllowedCarriers = NULL;
    }

    if (m_pExcludedCarriers != NULL) {
        delete[] m_pExcludedCarriers;
        m_pExcludedCarriers = NULL;
    }
}

int CarrierRestrictionsData::copyCarrierInfo(CarrierInfo *dstInfo, RIL_Carrier *srcInfo)
{
    int length;
    if (dstInfo == NULL || srcInfo == NULL) return -1;

    length = strlen(srcInfo->mcc);
    length = length > MAX_MCC_LEN ? MAX_MCC_LEN : length;
    memcpy(dstInfo->mcc, srcInfo->mcc, length);
    dstInfo->mcc[length] = 0;

    length = strlen(srcInfo->mnc);
    length = length > MAX_MNC_LEN ? MAX_MNC_LEN : length;
    memcpy(dstInfo->mnc, srcInfo->mnc, length);
    dstInfo->mnc[length] = 0;

    dstInfo->match_type = srcInfo->match_type;

    length = strlen(srcInfo->match_data);
    length = length > MAX_CR_MATCH_DATA_SIZE ? MAX_CR_MATCH_DATA_SIZE : length;
    memcpy(dstInfo->match_data, srcInfo->match_data, length);
    dstInfo->match_data[length] = 0;

    return 0;
}

INT32 CarrierRestrictionsData::encode(char *data, unsigned int length)
{
    int ret;
    if((0 == length) || (NULL == data)) return -1;
    RIL_CarrierRestrictions_V1_4 *pReqData = (RIL_CarrierRestrictions_V1_4 *)data;

    ret = 0;
    m_nAllowedCarrierLength = pReqData->carriers.len_allowed_carriers;
    if (m_nAllowedCarrierLength > 0 && pReqData->carriers.allowed_carriers != NULL) {
        m_pAllowedCarriers = new CarrierInfo[m_nAllowedCarrierLength];
        if (m_pAllowedCarriers == NULL) {
            RilLogE("CarrierRestrictionsData:Memory allocation failed");
            return -1;
        }
        memset(m_pAllowedCarriers, 0, m_nAllowedCarrierLength * sizeof(CarrierInfo));
        for(int index = 0; index < m_nAllowedCarrierLength; index++) {
            CarrierInfo *dstInfo = &(m_pAllowedCarriers[index]);
            RIL_Carrier *srcInfo = &(pReqData->carriers.allowed_carriers[index]);
            ret = copyCarrierInfo(dstInfo, srcInfo);
            if (ret == -1) break;
        }
    }
    m_nExcludedCarrierLength = pReqData->carriers.len_excluded_carriers;
    if (m_nExcludedCarrierLength > 0 && pReqData->carriers.excluded_carriers!= NULL) {
        m_pExcludedCarriers = new CarrierInfo[m_nExcludedCarrierLength];
        if (m_pExcludedCarriers == NULL) {
            RilLogE("CarrierRestrictionsData:Memory allocation failed");
            return -1;
        }
        memset(m_pExcludedCarriers, 0, m_nExcludedCarrierLength * sizeof(CarrierInfo));
        for(int index = 0; index < m_nExcludedCarrierLength; index++) {
            CarrierInfo *dstInfo = &(m_pExcludedCarriers[index]);
            RIL_Carrier *srcInfo = &(pReqData->carriers.excluded_carriers[index]);
            ret = copyCarrierInfo(dstInfo, srcInfo);
            if (ret == -1) break;
        }
    }

    m_bAllowedCarriersPrioritized = pReqData->carriers.allowedCarriersPrioritized;
    m_nMultiSimPolicy = pReqData->multiSimPolicy;

    if (ret == -1) {
        RilLogE("CarrierRestrictionsData:RIL_carrier Memory allocation failed");
        if (m_pAllowedCarriers != NULL)
        {
            delete[] m_pAllowedCarriers;
            m_pAllowedCarriers = NULL;
        }

        if (m_pExcludedCarriers != NULL)
        {
            delete[] m_pExcludedCarriers;
            m_pExcludedCarriers = NULL;
        }
    }

    return ret;
}

/**
 * SLOT PORT MAPPING
 */
namespace configV2_0 = android::hardware::radio::config::V2_0;

SlotPortMapping::SlotPortMapping(const int nReq, const Token tok, const ReqType type) : RequestData(nReq, tok, type)
{
    m_nNum_Slots = 0;
    m_nNum_Ports = 0;
    memset(m_nPhysicalSlotId, 0, MAX_PORT_NUM * sizeof(int));
    memset(m_nPortId, 0, MAX_PORT_NUM * sizeof(int));
}

SlotPortMapping::~SlotPortMapping()
{
}

INT32 SlotPortMapping::encode(char *data, unsigned int length)
{
    if((0 == length) || (NULL == data)) return -1;
    if (mHalVer < HAL_VERSION_CODE(2, 0)) {
        m_nNum_Slots = (int) (length / sizeof(unsigned int));
        for (int i = 0; i < m_nNum_Slots; i++) {
            m_nPhysicalSlotId[i] = *((int *) data + i);
        }
    } else {
        configV2_0::RIL_SlotPortMappingReq *pReqData =
                (configV2_0::RIL_SlotPortMappingReq *) data;

        m_nNum_Ports = pReqData->num_slots;
        for (int i = 0; i < m_nNum_Ports; i++) {
            m_nPhysicalSlotId[i] = pReqData->slotPortMap[i].physicalSlotId;
            m_nPortId[i] = pReqData->slotPortMap[i].portId;
        }
    }

    return 0;
}
