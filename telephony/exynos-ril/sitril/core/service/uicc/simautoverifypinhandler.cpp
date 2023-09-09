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
 * Simautoverifypinhandler.cpp
 *
 *  Created on: 2020. 9. 3.
 *      Author: hjun78.park
 */

#include "simautoverifypinhandler.h"
#include <librilutils/systemproperty.h>
#include "rillog.h"
#include "simservice.h"
#include "protocolsimbuilder.h"
#include "protocolsimadapter.h"
#include "simdatabuilder.h"
#include <librilutils/textutils.h>
#include "util.h"
#include "iccUtil.h"

#undef RilLogI
#define RilLogI(format, ...) CRilLog::Log(RIL_LOG_CAT_SIM, CRilLog::E_RIL_INFO_LOG,  format, ##__VA_ARGS__)
#undef RilLogV
#define RilLogV(format, ...) CRilLog::Log(RIL_LOG_CAT_SIM, CRilLog::E_RIL_VERBOSE_LOG,  format, ##__VA_ARGS__)
#undef RilLogW
#define RilLogW(format, ...) CRilLog::Log(RIL_LOG_CAT_SIM, CRilLog::E_RIL_WARNING_LOG,  format, ##__VA_ARGS__)
#undef RilLogE
#define RilLogE(format, ...) CRilLog::Log(RIL_LOG_CAT_SIM, CRilLog::E_RIL_CRITICAL_LOG,  format, ##__VA_ARGS__)

#define PARAM_NULL(msg)         { if(msg==NULL) { RilLogE("%s::%s() Parameter = NULL", m_pSimService->m_szSvcName, __FUNCTION__); return -1; } }
#define NULL_REQ(msg)           { if(msg==NULL || msg->GetRequestData()==NULL) { RilLogE("%s::%s() RequestData = NULL", m_pSimService->m_szSvcName, __FUNCTION__); return -1; } }
#define NULL_RSP(msg)           { if(msg==NULL || msg->GetModemData()==NULL) { RilLogE("%s::%s() ModemData = NULL", m_pSimService->m_szSvcName, __FUNCTION__); return -1; } }
#define ENTER_FUNC()            { RilLogI("%s::%s() [<-- ", m_pSimService->m_szSvcName, __FUNCTION__); }
#define LEAVE_FUNC()            { RilLogI("%s::%s() [--> ", m_pSimService->m_szSvcName, __FUNCTION__); }
#define NOT_IMPLEMENT()         { RilLogE("%s::%s() Not Implemented", m_pSimService->m_szSvcName, __FUNCTION__); }

#define LOGI(format, ...)       RilLogI("%s::%s() " format, m_pSimService->m_szSvcName, __FUNCTION__, ##__VA_ARGS__)
#define LOGV(format, ...)       RilLogV("%s::%s() " format, m_pSimService->m_szSvcName, __FUNCTION__, ##__VA_ARGS__)
#undef LOGD
#define LOGD(format, ...)       RilLog("%s::%s() " format, m_pSimService->m_szSvcName, __FUNCTION__, ##__VA_ARGS__)
#define LOGE(format, ...)       RilLogE("%s::%s() " format, m_pSimService->m_szSvcName, __FUNCTION__, ##__VA_ARGS__)

#ifdef AUTO_VERIFY_PIN_WITH_CP
#define INVALID_PIN_LEN             (-1)
#define INVALID_PIN_STR             ""
#endif


SimAutoVerifyPinHandler::SimAutoVerifyPinHandler()
{
    memset(m_szAutoPinPropertyKey, 0, sizeof(m_szAutoPinPropertyKey));
    SetAutoPinState(AUTO_PIN_STATE_DISABLED);
    m_pSimService = NULL;

}

SimAutoVerifyPinHandler::~SimAutoVerifyPinHandler()
{
    m_pSimService = NULL;
    ClearEncryptedPin();
}

void SimAutoVerifyPinHandler::Init(SimService* pSimService)
{
    if (pSimService != NULL) {
        m_pSimService = pSimService;
    }

#ifdef AUTO_VERIFY_PIN_WITH_CP
    RilProperty *pProperty = m_pSimService->GetRilContextProperty();
    bool bSavedEncrytpedDataInProperty = true;
    char szPinLenPropertyName[PROPERTY_NAME_LEN] = {0};
    char szPinStrPropertyName[PROPERTY_NAME_LEN] = {0};

    snprintf(szPinLenPropertyName, (PROPERTY_NAME_LEN - 1), VENDOR_RIL_PIN_LEN,
            m_pSimService->GetRilSocketId());
    snprintf(szPinStrPropertyName, (PROPERTY_NAME_LEN - 1), VENDOR_RIL_ENCRYPTED_PIN,
            m_pSimService->GetRilSocketId());

    if (NULL != pProperty) {
        int nPinLen = SystemProperty::GetInt((const char*)szPinLenPropertyName, INVALID_PIN_LEN);
        if (INVALID_PIN_LEN < nPinLen) {
            pProperty->Put(RIL_CONTEXT_SIM_PIN_LEN, nPinLen);
            SystemProperty::Set((const char*)szPinLenPropertyName, INVALID_PIN_LEN);
        } else {
            bSavedEncrytpedDataInProperty = false;
        }

        string encryptedPin = SystemProperty::Get((const char*)szPinStrPropertyName,
                INVALID_PIN_STR);
        if (bSavedEncrytpedDataInProperty && (0 < encryptedPin.length())) {
            pProperty->Put(RIL_CONTEXT_SIM_ENCRYPTED_PIN, (const char*)encryptedPin.c_str());
            SystemProperty::Set((const char*)szPinStrPropertyName, INVALID_PIN_STR);
        } else {
            bSavedEncrytpedDataInProperty = false;
        }
    } else {
        bSavedEncrytpedDataInProperty = false;
    }

    if (bSavedEncrytpedDataInProperty == true) {
        SetAutoPinState(AUTO_PIN_STATE_ENABLED);
    } else {
        // If there is no data at all, all clear saved pin
        ClearEncryptedPin();
        SystemProperty::Set((const char*)szPinLenPropertyName, INVALID_PIN_LEN);
        SystemProperty::Set((const char*)szPinStrPropertyName, INVALID_PIN_STR);
    }
#else
    snprintf(m_szAutoPinPropertyKey, (PROPERTY_NAME_LEN - 1), PROP_AUTO_VERIFY_PIN,
            m_pSimService->GetRilSocketId());

    char szPropertyName[PROPERTY_NAME_LEN] = {0, };
    snprintf(szPropertyName, (PROPERTY_NAME_LEN - 1), AUTO_PIN_STATE_ENABLED_AND_RIL_CRASH,
            m_pSimService->GetRilSocketId());
    bool bAutoPinStateEnabledAndRilCrash = SystemProperty::GetInt(
                (const char*)szPropertyName, 0);
    if (bAutoPinStateEnabledAndRilCrash) {
        SetAutoPinState(AUTO_PIN_STATE_ENABLED);
        SystemProperty::Set((const char*)szPropertyName, 0);
    }
#endif
}

int SimAutoVerifyPinHandler::DoAutoVerifyPin()
{
    ENTER_FUNC();

    int nResult = -1;

#ifdef AUTO_VERIFY_PIN_WITH_CP
    std::string encryptedPin = "";
    int nPinLen = 0;
    RilProperty *pProperty = m_pSimService->GetRilContextProperty();

    if (pProperty != NULL) {
        nPinLen = pProperty->GetInt(RIL_CONTEXT_SIM_PIN_LEN, INVALID_PIN_LEN);
        encryptedPin = pProperty->GetString(RIL_CONTEXT_SIM_ENCRYPTED_PIN, INVALID_PIN_STR);
    } else {
        LOGE("pProperty is NULL");
    }

    LOGI("nPinLen : %d, encryptedPin length : %d", nPinLen, encryptedPin.length());

    if (0 < encryptedPin.length() && INVALID_PIN_LEN < nPinLen) {
        ProtocolSimBuilder builder;
        ModemData *pModemData = builder.BuildSimVerifyEncryptedPin(
                nPinLen, (const char *)encryptedPin.c_str(), m_pSimService->m_aszAID[0]);
        if (pModemData != NULL) {
            nResult = m_pSimService->SendRequest(pModemData, TIMEOUT_SIM_DEFAULT,
                    MSG_SIM_VERIFY_ENCRYPTED_PIN_DONE);
        } else {
            LOGE("pModemData is NULL");
        }
    } else {
        LOGE("There is no saved encrypted password");
    }
#else
    char szPin[MAX_SIM_PIN_LEN + 1] = {0, };
    GetAutoVerifyPin(szPin);

    ProtocolSimBuilder builder;
    ModemData *pModemData = builder.BuildSimVerifyPin(PIN1_PUK1, szPin, m_pSimService->m_aszAID[0]);
    nResult = m_pSimService->SendRequest(pModemData, TIMEOUT_SIM_DEFAULT, MSG_SIM_VERIFY_PIN_DONE);
#endif
    SetAutoPinState(AUTO_PIN_STATE_RECOVERY);
    LEAVE_FUNC();
    return nResult;
}

void SimAutoVerifyPinHandler::SavePin(const char *pszPin)
{
    ENTER_FUNC();
    if (pszPin != NULL &&  strlen(pszPin) > 0) {
        EncryptAutoVerifyPin(pszPin);
        SetAutoPinState(AUTO_PIN_STATE_ENABLED);
    } else {
        SetAutoPinState(AUTO_PIN_STATE_DISABLED);
    }
    LEAVE_FUNC();
}

void SimAutoVerifyPinHandler::SetAutoPinState(int autoPinState)
{
    m_nAutoPinState = autoPinState;
    RilLogI("SetAutoPinState() m_nAutoPinState = %s",
            IccUtil::covertAutoPinStateToString(m_nAutoPinState).c_str());
}

void SimAutoVerifyPinHandler::GetAutoVerifyPin(char *pszPin)
{
    if (pszPin != NULL) {
        DecryptAutoVerifyPin(pszPin);
    }
}

void SimAutoVerifyPinHandler::ClearEncryptedPin()
{
#ifdef AUTO_VERIFY_PIN_WITH_CP
    RilProperty *pProperty = m_pSimService->GetRilContextProperty();
    if (pProperty != NULL) {
        pProperty->Put(RIL_CONTEXT_SIM_PIN_LEN, INVALID_PIN_LEN);
        pProperty->Put(RIL_CONTEXT_SIM_ENCRYPTED_PIN, INVALID_PIN_STR);
    } else {
        LOGE("pProperty is NULL");
    }
#else
    SystemProperty::Set(m_szAutoPinPropertyKey, "");
#endif
    SetAutoPinState(AUTO_PIN_STATE_DISABLED);
}

void SimAutoVerifyPinHandler::EncryptAutoVerifyPin(const char *pszPin)
{
    // Zigzag Scan from H.264
    int nScramble[MAX_SIM_PIN_LEN*2] = {  1,  2,  5,  9,
        6,  3,  4,  7,
        10, 13, 14, 11,
        8, 12, 15, 16 };

    // RilLogI("[%s] %s(): ----- Encryption -----", m_szSvcName, __FUNCTION__);

    if (pszPin != NULL) {
        char szCode[MAX_SIM_PIN_LEN];
        char szHexStr[BUFFER_SIZE];
        char szBuffer[BUFFER_SIZE];
        memset(szHexStr, 0, sizeof(szHexStr));
        memset(szBuffer, 0, sizeof(szBuffer));

        // Fill space character
        memset(szCode, 0x20, sizeof(szCode));
        memcpy(szCode, pszPin, strlen(pszPin));
        int nHexLen = Value2HexString(szHexStr, (BYTE *) szCode, sizeof(szCode));

        // Scrambling
        for (int i = 0; i < (MAX_SIM_PIN_LEN * 2); i++) szBuffer[i] = szHexStr[nScramble[i] - 1];

        // GSM 7bit Coding
        unsigned char curr, next;
        for (int nIdx = 0, i = 0; i < nHexLen; i++, nIdx++) {
            curr = (unsigned char) szBuffer[i];
            next = (unsigned char) (((i+1)<nHexLen)? szBuffer[i+1]: 0);
            szHexStr[nIdx] = (unsigned char) (((curr >> (nIdx==0? 0: nIdx%7)) & (0x00FF >> ((nIdx==0? 0: nIdx%7) + 1)))
                    | (((next << 8) & 0xFF00) >> (((nIdx==0? 0: nIdx%7) + 1))));
            if ((nIdx + 1) % 7 == 0) i++;
        }

        // XOR Key
        for (int i = 0; i < nHexLen; i++) szHexStr[i] ^= m_szAutoPinPropertyKey[i%strlen(m_szAutoPinPropertyKey)];

        // To Hex String
        char szTempEncryptedPin[BUFFER_SIZE+1] = {0,};
        nHexLen = Value2HexString(szTempEncryptedPin, (BYTE *) szHexStr, nHexLen-(nHexLen/8));
        SystemProperty::Set(m_szAutoPinPropertyKey, szTempEncryptedPin);
    }
}

void SimAutoVerifyPinHandler::DecryptAutoVerifyPin(char *pszPin)
{
    // Zigzag Scan from H.264
    int nScramble[MAX_SIM_PIN_LEN*2] = {  1,  2,  5,  9,
        6,  3,  4,  7,
        10, 13, 14, 11,
        8, 12, 15, 16 };

    // RilLogI("[%s] %s(): ----- Decryption -----", m_szSvcName, __FUNCTION__);
    string EncryptedPin = SystemProperty::Get(m_szAutoPinPropertyKey, "");

    if (pszPin != NULL && !TextUtils::IsEmpty(EncryptedPin))
    {
        char szHexDec[BUFFER_SIZE];
        char szBuffer[BUFFER_SIZE];
        memset(szHexDec, 0, sizeof(szHexDec));
        memset(szBuffer, 0, sizeof(szBuffer));
        int nValLen = HexString2Value((BYTE *) szHexDec, sizeof(szHexDec), (const char *) EncryptedPin.c_str());

        // XOR Key
        for (int i = 0; i < nValLen; i++) szHexDec[i] ^= m_szAutoPinPropertyKey[i%strlen(m_szAutoPinPropertyKey)];

        // 8bit Character Coding
        unsigned char by = 0;
        for (int nIdx = 0, i = 0; i < nValLen; i++, nIdx++)
        {
            unsigned char curr = szHexDec[i];
            szBuffer[nIdx] = (unsigned char) 0x00 | ((curr & (0x00FF >> (((i>0)? i%7:0)+1))) << ((i>0)? (i%7):0)) | by;
            by = (unsigned char) 0x00 | ((curr & (0xFF00 >> (((i>0)? i%7:0)+1))) >> (7-(((i>0)? (i%7):0))));

            if ((i + 1) % 7 == 0) {
                szBuffer[++nIdx] = (unsigned char) 0x00 | by;
                by = 0;
            }
        }

        // Unscrambling
        memset(szHexDec, 0, sizeof(szHexDec));
        for (int i = 0; i < (MAX_SIM_PIN_LEN*2); i++) szHexDec[nScramble[i]-1] = szBuffer[i];

        // Fill space character
        char szCode[MAX_SIM_PIN_LEN];
        memset(szCode, 0x20, sizeof(szCode));
        nValLen = HexString2Value((BYTE *) szCode, sizeof(szCode), (const char *) szHexDec);

        // Remove Space Character
        if (0 < nValLen) {
            for(; 0 < nValLen && szCode[nValLen-1] == 0x20; nValLen--) szCode[nValLen-1] = '\0';
            strncpy(pszPin, szCode, nValLen);
        }
    }

}

int SimAutoVerifyPinHandler::CheckBootReason()
{
    int state = BOOT_STATE_UNKNOWN;
    string strReason = SystemProperty::Get(SYS_BOOT_REASON, "");

    if (TextUtils::IsEmpty(strReason)) {
        state = BOOT_STATE_UNKNOWN;
    } else if (strReason.find("kernel_panic") != string::npos) {
        state = BOOT_STATE_KERNEL_PANIC;
    } else {
        state = BOOT_STATE_NORMAL;
    }
    return state;
}

bool SimAutoVerifyPinHandler::IsEncryptedPinPropertyExist()
{
    string EncryptedPin = "";
#ifdef AUTO_VERIFY_PIN_WITH_CP
    RilProperty *pProperty = m_pSimService->GetRilContextProperty();
    if (pProperty != NULL) {
        EncryptedPin = pProperty->GetString(RIL_CONTEXT_SIM_ENCRYPTED_PIN, INVALID_PIN_STR);
    }
#else
    char szPropertyName[PROPERTY_NAME_LEN] = {0, };
    snprintf(szPropertyName, (PROPERTY_NAME_LEN - 1),
            PROP_AUTO_VERIFY_PIN, m_pSimService->GetRilSocketId());
    EncryptedPin = SystemProperty::Get(szPropertyName, INVALID_PIN_STR);
#endif
    if (EncryptedPin.length() == 0) {
        return false;
    }
    return true;
}

#ifdef AUTO_VERIFY_PIN_WITH_CP
int SimAutoVerifyPinHandler::OnVerifyEncryptedPinDone(Message *pMsg)
{
    PARAM_NULL(pMsg);

    SimDataBuilder builder;
    ProtocolSimVerifyPinAdapter adapter(pMsg->GetModemData());
    UINT uErrCode = adapter.GetErrorCode();
    const RilData *pRilData = builder.BuildSimPinPukResponse(adapter.GetRemainCount());
    if (pRilData == NULL) {
        m_pSimService->OnRequestComplete(RIL_E_GENERIC_FAILURE);
        // Error case
        ClearEncryptedPin();
    } else {
        m_pSimService->DoGetSimStatus(pMsg);
        if (uErrCode == RIL_E_SUCCESS) {
            SetAutoPinState(AUTO_PIN_STATE_ENABLED);
            m_pSimService->DoGetSimStatus(pMsg);
        } else {
            // Error case
            ClearEncryptedPin();
        }
        if (pRilData != NULL) delete pRilData;
    }
    return 0;
}

int SimAutoVerifyPinHandler::OnReceiveEncryptedPin(Message *pMsg)
{
    NULL_RSP(pMsg);
    ModemData *pModemData = pMsg->GetModemData();
    ProtocolSimEncryptedPinAdapter adapter(pModemData);
    int nPinLength = adapter.GetPinLen();
    unsigned long nEncryptedPinLength = adapter.GetEncryptedPinLen();
    const char *pEncryptedPin = adapter.GetEncryptedPin();
    BOOL bAllVerified = true;

    RilProperty *pProperty = m_pSimService->GetRilContextProperty();
    if(pProperty != NULL) {
        if (4 <= nPinLength && nPinLength <= 8) {
            pProperty->Put(RIL_CONTEXT_SIM_PIN_LEN, nPinLength);
        } else {
            bAllVerified = false;
        }

        // LOGI("nEncryptedPinLength : %d, pEncryptedPin length : %d", nEncryptedPinLength, strlen(pEncryptedPin));
        // pEncryptedPin is hexstring so length is times 2.
        if (bAllVerified && ((nEncryptedPinLength * 2) == strlen(pEncryptedPin))) {
            pProperty->Put(RIL_CONTEXT_SIM_ENCRYPTED_PIN, pEncryptedPin);
        } else {
            bAllVerified = false;
        }

        if (bAllVerified == true) {
            SetAutoPinState(AUTO_PIN_STATE_ENABLED);
            LOGI("Automatic Verify PIN Done");
        } else {
            pProperty->Put(RIL_CONTEXT_SIM_PIN_LEN, INVALID_PIN_LEN);
            pProperty->Put(RIL_CONTEXT_SIM_ENCRYPTED_PIN, INVALID_PIN_STR);
            LOGE("Fail to receive encrypted password");
        }
    } else {
        ClearEncryptedPin();
        LOGI("pProperty is NULL");
    }

    return 0;
}
#endif
