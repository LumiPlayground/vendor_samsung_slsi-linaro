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
 * protocolsimbuilderjson.cpp
 *
 *  Created on: 2021. 7. 19.
 *      Author: aryaman.2019
 */

#include "protocolsimbuilderjson.h"
#include "protocolutils.h"
#include "rillog.h"
#include "simjsondef.h"
#include "json/json.h"
#include "util.h"

using std::string;

// add category to display selective logs
#undef RilLogI
#define RilLogI(format, ...) CRilLog::Log(RIL_LOG_CAT_SIM, CRilLog::E_RIL_INFO_LOG,  format, ##__VA_ARGS__)
#undef RilLogV
#define RilLogV(format, ...) CRilLog::Log(RIL_LOG_CAT_SIM, CRilLog::E_RIL_VERBOSE_LOG,  format, ##__VA_ARGS__)
#undef RilLogW
#define RilLogW(format, ...) CRilLog::Log(RIL_LOG_CAT_SIM, CRilLog::E_RIL_WARNING_LOG,  format, ##__VA_ARGS__)
#undef RilLogE
#define RilLogE(format, ...) CRilLog::Log(RIL_LOG_CAT_SIM, CRilLog::E_RIL_CRITICAL_LOG,  format, ##__VA_ARGS__)

#define    SWAP16(val)        ( (((val) << 8) & 0xFF00) | (((val) >> 8) & 0x00FF) )
#define    SWAP32(val)        ( (((val) & 0x000000FF) << 24) | (((val) & 0x0000FF00) <<  8) | (((val) & 0x00FF0000) >>  8) | (((val) & 0xFF000000) >> 24) )

static int GetCodeForFacilityLock(char *pszCode) {
    char aszFacCode[SIT_SIM_FAC_MAX][MAX_FACILITY_CODE_LEN+1] = { "CS", "PS", "PF", "SC", "AO",
        "OI", "OX", "AI", "IR", "NT",
        "NM", "NS", "NA", "AB", "AG",
        "AC", "FD", "PN", "PU", "PP",
        "PC", "SC2" };

    int nCode = SIT_SIM_FAC_MAX;
    for(int i=0; i<SIT_SIM_FAC_MAX; i++) {
        if(strcmp(pszCode, aszFacCode[i])==0) {
            nCode = i;
            break;
        }
    }
    return nCode;
}

ModemData *ProtocolSimBuilderJson::BuildSimGetStatus()
{
    return GetJsonNullReqModemData(SIT_GET_SIM_STATUS);
}

ModemData *ProtocolSimBuilderJson::BuildSimVerifyPin(int nPinIndex, const char *pszPin, const char *pszAID)
{
    int nReqID;

    switch(nPinIndex)
    {
    case PIN1_PUK1: nReqID = SIT_VERIFY_SIM_PIN; break;
    case PIN2_PUK2: nReqID = SIT_VERIFY_SIM_PIN2; break;
    default:
        return NULL;
    }

    if(pszPin == NULL || strlen(pszPin) == 0) return NULL;
    Json::Value jsonReqData;
    string pin(pszPin, MIN(strlen(pszPin), MAX_SIM_PIN_LEN));
    if(nReqID == SIT_VERIFY_SIM_PIN) {
        jsonReqData[ns_sim_verify_sim_pin_req::key_pin] = pin;
    } else {
        jsonReqData[ns_sim_verify_sim_pin2_req::key_pin] = pin;
    }

    int aidLen = 0;
    if(nReqID == SIT_VERIFY_SIM_PIN) {
        jsonReqData[ns_sim_verify_sim_pin_req::key_aid] = Json::arrayValue;
    } else {
        jsonReqData[ns_sim_verify_sim_pin2_req::key_aid] = Json::arrayValue;
    }
    if(pszAID != NULL) {
        BYTE aid[MAX_SIM_AID_LEN];
        memset(aid, 0, MAX_SIM_AID_LEN);
        aidLen = HexString2Value(aid, sizeof(aid), pszAID);
        if(nReqID == SIT_VERIFY_SIM_PIN) {
            for(int i = 0; i < aidLen; i++) {
                jsonReqData[ns_sim_verify_sim_pin_req::key_aid].append((int) aid[i]);
            }
        } else {
            for(int i = 0; i < aidLen; i++) {
                jsonReqData[ns_sim_verify_sim_pin2_req::key_aid].append((int) aid[i]);
            }
        }
    }
    return GetJsonReqModemData(jsonReqData, nReqID);
}

ModemData *ProtocolSimBuilderJson::BuildSimVerifyPuk(int nPukIndex, const char *pszPuk, const char *pszNewPin, const char *pszAID)
{
    int nReqID;

    switch(nPukIndex)
    {
    case PIN1_PUK1: nReqID = SIT_VERIFY_SIM_PUK; break;
    case PIN2_PUK2: nReqID = SIT_VERIFY_SIM_PUK2; break;
    default:
        return NULL;
    }

    if(pszPuk == NULL || pszNewPin == NULL || strlen(pszPuk) == 0
            || strlen(pszNewPin) == 0) return NULL;

    Json::Value jsonReqData;
    string puk(pszPuk, MIN(strlen(pszPuk), MAX_SIM_PUK_LEN));
    string newPin(pszNewPin, MIN(strlen(pszNewPin), MAX_SIM_PIN_LEN));
    if(nReqID == SIT_VERIFY_SIM_PUK) {
        jsonReqData[ns_sim_verify_sim_puk_req::key_puk] = puk;
        jsonReqData[ns_sim_verify_sim_puk_req::key_new_pin] = newPin;
    } else {
        jsonReqData[ns_sim_verify_sim_puk2_req::key_puk] = puk;
        jsonReqData[ns_sim_verify_sim_puk2_req::key_new_pin] = newPin;
    }

    int aidLen = 0;
    if(nReqID == SIT_VERIFY_SIM_PUK) {
        jsonReqData[ns_sim_verify_sim_puk_req::key_aid] = Json::arrayValue;
    } else {
        jsonReqData[ns_sim_verify_sim_puk2_req::key_aid] = Json::arrayValue;
    }
    if(pszAID != NULL) {
        BYTE aid[MAX_SIM_AID_LEN];
        memset(aid, 0, MAX_SIM_AID_LEN);
        aidLen = HexString2Value(aid, sizeof(aid), pszAID);
        if(nReqID == SIT_VERIFY_SIM_PUK) {
            for(int i = 0; i < aidLen; i++) {
                jsonReqData[ns_sim_verify_sim_puk_req::key_aid].append((int) aid[i]);
            }
        } else {
            for(int i = 0; i < aidLen; i++) {
                jsonReqData[ns_sim_verify_sim_puk2_req::key_aid].append((int) aid[i]);
            }
        }
    }
    return GetJsonReqModemData(jsonReqData, nReqID);
}

ModemData *ProtocolSimBuilderJson::BuildSimChangePin(int nPinIndex, const char *pszOldPin, const char *pszNewPin, const char *pszAID)
{
    int nReqID, nFAC = 0;

    switch(nPinIndex)
    {
    case PIN1_PUK1: nReqID = SIT_CHG_SIM_PIN; nFAC = SIT_SIM_FAC_SC; break;
    case PIN2_PUK2: nReqID = SIT_CHG_SIM_PIN2; nFAC = SIT_SIM_FAC_SC2; break;
    default:
        return NULL;
    }

    if(pszOldPin == NULL || pszNewPin == NULL || strlen(pszOldPin) == 0
            || strlen(pszNewPin) == 0) return NULL;

    Json::Value jsonReqData;
    string oldPin(pszOldPin, MIN(strlen(pszOldPin), MAX_SIM_PIN_LEN));
    string newPin(pszNewPin, MIN(strlen(pszNewPin), MAX_SIM_PIN_LEN));
    if(nReqID == SIT_CHG_SIM_PIN) {
        jsonReqData[ns_sim_change_sim_pin_req::key_fac_type] = nFAC;
        jsonReqData[ns_sim_change_sim_pin_req::key_old_pin] = oldPin;
        jsonReqData[ns_sim_change_sim_pin_req::key_new_pin] = newPin;
    } else {
        jsonReqData[ns_sim_change_sim_pin2_req::key_fac_type] = nFAC;
        jsonReqData[ns_sim_change_sim_pin2_req::key_old_pin] = oldPin;
        jsonReqData[ns_sim_change_sim_pin_req::key_new_pin] = newPin;
    }

    int aidLen = 0;
    if(nReqID == SIT_CHG_SIM_PIN) {
        jsonReqData[ns_sim_change_sim_pin_req::key_aid] = Json::arrayValue;
    } else {
        jsonReqData[ns_sim_change_sim_pin2_req::key_aid] = Json::arrayValue;
    }
    if(pszAID != NULL) {
        BYTE aid[MAX_SIM_AID_LEN];
        memset(aid, 0, MAX_SIM_AID_LEN);
        aidLen = HexString2Value(aid, sizeof(aid),pszAID);
        if(nReqID == SIT_CHG_SIM_PIN) {
            for(int i = 0; i < aidLen; i++) {
                jsonReqData[ns_sim_change_sim_pin_req::key_aid].append((int) aid[i]);
            }
        } else {
            for(int i = 0; i < aidLen; i++) {
                jsonReqData[ns_sim_change_sim_pin2_req::key_aid].append((int) aid[i]);
            }
        }
    }
    return GetJsonReqModemData(jsonReqData, nReqID);
}

ModemData *ProtocolSimBuilderJson::BuildSimVerifyNetworkLock(int nFac, const char *pszPassword, int nSvcClass, const char *pszAID)
{
    if(pszPassword == NULL || strlen(pszPassword) == 0) return NULL;

    Json::Value jsonReqData;
    jsonReqData[ns_sim_verify_network_lock_req::key_fac_type] = nFac;
    string password(pszPassword, MIN(strlen(pszPassword), MAX_SIM_FACILITY_PASSWORD_LEN));
    jsonReqData[ns_sim_verify_network_lock_req::key_pw] = password;
    jsonReqData[ns_sim_verify_network_lock_req::key_class] = nSvcClass;

    int aidLen = 0;
    jsonReqData[ns_sim_verify_network_lock_req::key_aid] = Json::arrayValue;
    if(pszAID != NULL) {
        BYTE aid[MAX_SIM_AID_LEN];
        memset(aid, 0, MAX_SIM_AID_LEN);
        aidLen = HexString2Value(aid, sizeof(aid),pszAID);
        for(int i = 0; i < aidLen; i++) {
            jsonReqData[ns_sim_verify_network_lock_req::key_aid].append((int) aid[i]);
        }
    }
    return GetJsonReqModemData(jsonReqData, SIT_VERIFY_NETWORK_LOCK);
}

ModemData *ProtocolSimBuilderJson::BuildSimIO(int nCmd, int nAppType, int nFileID, const char *pPath, int p1, int p2, int p3,
                                                int /* nDataLen */, const char *pData, const char *pszPin2, const char *pszAID)
{
    if(pPath == NULL || pData == NULL) return NULL;

    BYTE abCommandValues[] = { 0xB0, 0xB2, 0xC0, 0xD6, 0xDC, 0xF2 };
    char aszCommands[][16] = {"READ_BINARY", "READ_RECORD", "GET_RESPONSE", "UPDATE_BINARY", "UPDATE_RECORD", "STATUS" };
    char szCmd[16] = "Unknown";
    for(int i = 0; i < (int)(sizeof(abCommandValues)/sizeof(BYTE)); i++) {
        if(nCmd == (int) abCommandValues[i]) {
            SECURELIB::strncpy(szCmd, sizeof(szCmd), aszCommands[i], SECURELIB::strlen(aszCommands[i]));
        }
    }
    RilLogV("ProtocolSimBuilderJson::%s() Cmd:%s(0x%02X), AppType:%d, FileID:%X, Path:%s", __FUNCTION__, szCmd, nCmd, nAppType, nFileID, pPath);
    RilLogV("ProtocolSimBuilderJson::%s() P1:%d=0x%02X, P2:%d=0x%02X, P3:%d=0x%02X", __FUNCTION__, p1, p1, p2, p2, p3, p3);
    if(pszPin2 != NULL && strlen(pszPin2) > 0) RilLogV("ProtocolSimBuilderJson::%s() PIN2:%s", __FUNCTION__, pszPin2);

    Json::Value jsonReqData;
    jsonReqData[ns_sim_sim_io_req::key_cmd] = nCmd;
    jsonReqData[ns_sim_sim_io_req::key_app_type] = nAppType;
    jsonReqData[ns_sim_sim_io_req::key_file_id] = nFileID;
    jsonReqData[ns_sim_sim_io_req::key_path] = Json::arrayValue;

    if (pPath != NULL && strlen(pPath)>0) {
        // convert HEX string to HEX value
        BYTE arPath[MAX_SIM_IO_PATH_LEN] = { 0, };
        int path_len = (BYTE)HexString2Value(arPath, sizeof(arPath), pPath);
        if ( path_len <= 0 ) {
            return NULL;
        }

        // Swap for Big-Endian
        for(int i = 0; i < path_len; i += 2) {
            //WORD wPath16 = *((WORD *) &arPath[i]);
            //*((WORD *) &arPath[i]) = SWAP16(wPath16);
            *((WORD *) &arPath[i]) = SWAP16(*((WORD *) &arPath[i]));
        }
        for(int i = 0; i < path_len; i++) {
            jsonReqData[ns_sim_sim_io_req::key_path].append((int) arPath[i]);
        }
    }
    jsonReqData[ns_sim_sim_io_req::key_p1] = p1;
    jsonReqData[ns_sim_sim_io_req::key_p2] = p2;
    jsonReqData[ns_sim_sim_io_req::key_p3] = p3;
    jsonReqData[ns_sim_sim_io_req::key_data] = Json::arrayValue;

    BYTE data[MAX_SIM_IO_DATA_LEN];
    memset(data, 0, MAX_SIM_IO_DATA_LEN);
    int dataLen = HexString2Value(data, sizeof(data), (char *) pData);
    for(int i = 0; i < dataLen; i++) {
        jsonReqData[ns_sim_sim_io_req::key_data].append((int) data[i]);
    }

    if(pszPin2 != NULL && strlen(pszPin2) > 0) {
        string pin2(pszPin2, MIN(strlen(pszPin2), MAX_SIM_PIN_LEN));
        jsonReqData[ns_sim_sim_io_req::key_pin2] = pin2;
    } else {
        string pin2 = "";
        jsonReqData[ns_sim_sim_io_req::key_pin2] = pin2;
    }

    jsonReqData[ns_sim_sim_io_req::key_aid] = Json::arrayValue;
    if(pszAID != NULL) {
        BYTE aid[MAX_SIM_AID_LEN];
        memset(aid, 0, MAX_SIM_AID_LEN);
        int aidLen = HexString2Value(aid, sizeof(aid), pszAID);
        for(int i = 0; i < aidLen; i++) {
            jsonReqData[ns_sim_sim_io_req::key_aid].append((int) aid[i]);
        }
    }
    return GetJsonReqModemData(jsonReqData, SIT_SIM_IO);
}

ModemData *ProtocolSimBuilderJson::BuildSimGetFacilityLock(char *pszCode, char *pszPassword, int nSvcClass, char *pszAID)
{
    int nCode = GetCodeForFacilityLock(pszCode);
    RilLogV("ProtocolSimBuilderJson::%s() Code:%d, Password:%s, ServiceClass:%d, AID:%s", __FUNCTION__, nCode, pszPassword, nSvcClass, pszAID);

    Json::Value jsonReqData;
    jsonReqData[ns_sim_get_facility_lock_req::key_fac_type] = nCode;

    if(strlen(pszPassword) > 0) {
        string password(pszPassword, MIN(strlen(pszPassword), MAX_SIM_FACILITY_PASSWORD_LEN));
        jsonReqData[ns_sim_get_facility_lock_req::key_pw] = password;
    } else {
        string password = "";
        jsonReqData[ns_sim_get_facility_lock_req::key_pw] = password;
    }
    jsonReqData[ns_sim_get_facility_lock_req::key_class] = nSvcClass;

    jsonReqData[ns_sim_get_facility_lock_req::key_aid] = Json::arrayValue;
    if(pszAID != NULL) {
        BYTE aid[MAX_SIM_AID_LEN];
        memset(aid, 0, MAX_SIM_AID_LEN);
        int aidLen = HexString2Value(aid, sizeof(aid), pszAID);
        for(int i = 0; i < aidLen; i++) {
            jsonReqData[ns_sim_get_facility_lock_req::key_aid].append((int) aid[i]);
        }
    }
    return GetJsonReqModemData(jsonReqData, SIT_GET_FACILITY_LOCK);
}

ModemData *ProtocolSimBuilderJson::BuildSimSetFacilityLock(char *pszCode, int nLockMode, char *pszPassword, int nSvcClass, char *pszAID)
{
    int nCode = GetCodeForFacilityLock(pszCode);
    RilLogV("ProtocolSimBuilderJson::%s() Code:%d, Password:%s, ServiceClass:%d, AID:%s", __FUNCTION__, nCode, pszPassword, nSvcClass, pszAID);

    Json::Value jsonReqData;
    jsonReqData[ns_sim_set_facility_lock_req::key_fac_type] = nCode;

    if(strlen(pszPassword) > 0) {
        string password(pszPassword, MIN(strlen(pszPassword), MAX_SIM_FACILITY_PASSWORD_LEN));
        jsonReqData[ns_sim_set_facility_lock_req::key_pw] = password;
    } else {
        string password = "";
        jsonReqData[ns_sim_set_facility_lock_req::key_pw] = password;
    }
    jsonReqData[ns_sim_set_facility_lock_req::key_class] = nSvcClass;
    jsonReqData[ns_sim_set_facility_lock_req::key_lock_mode] = nLockMode;

    jsonReqData[ns_sim_set_facility_lock_req::key_aid] = Json::arrayValue;
    if(pszAID != NULL) {
        BYTE aid[MAX_SIM_AID_LEN];
        memset(aid, 0, MAX_SIM_AID_LEN);
        int aidLen = HexString2Value(aid, sizeof(aid), pszAID);
        for(int i = 0; i < aidLen; i++) {
            jsonReqData[ns_sim_set_facility_lock_req::key_aid].append((int) aid[i]);
        }
    }
    return GetJsonReqModemData(jsonReqData, SIT_SET_FACILITY_LOCK);
}

ModemData *ProtocolSimBuilderJson::BuildSimGetIsimAuth(int nAuthType, BYTE *pAuth, int nAuthLength)
{
    Json::Value jsonReqData;
    jsonReqData[ns_sim_get_sim_auth_req::key_auth_type] = nAuthType;
    ProtocolUtils::LimitLengthToMax<int>(RIL_LOG_CAT_SIM, __CLASS_FUNC__, nAuthLength, MAX_SIM_AUTH_REQ_LEN);
    if(pAuth != NULL) {
        // see sit_sim_get_sim_auth_req
        int i = 0, j = 0;
        int randLen = j < nAuthLength  ? pAuth[j] : 0;
        i = 0;  j++;
        for(  ; i < randLen && j < nAuthLength ; i++, j++) {
            jsonReqData[ns_sim_get_sim_auth_req::key_rand].append((int) pAuth[j]);
        }

        int autnLen = j < nAuthLength ? pAuth[j]: 0;
        i = 0; j++;
        for( ; i < autnLen && j < nAuthLength; i++, j++) {
            jsonReqData[ns_sim_get_sim_auth_req::key_autn].append((int) pAuth[j]);
        }

        // handling out of bound cases
        if(randLen == 0)  jsonReqData[ns_sim_get_sim_auth_req::key_rand] = Json::arrayValue;
        if(autnLen == 0)  jsonReqData[ns_sim_get_sim_auth_req::key_autn] = Json::arrayValue;
    } else {
        //sending empty array as pAuth is NULL
        jsonReqData[ns_sim_get_sim_auth_req::key_rand] = Json::arrayValue;
        jsonReqData[ns_sim_get_sim_auth_req::key_autn] = Json::arrayValue;
    }
    return GetJsonReqModemData(jsonReqData, SIT_GET_SIM_AUTH);
}

ModemData *ProtocolSimBuilderJson::BuildSimGetSimAuth(int nAuthContext, BYTE *pAuth, int nAuthLength, int nAppType)
{
    RilLogV("ProtocolSimBuilderLegacy::%s() nAuthContext:0x%X, nAuthLengh:%d, nAppType:0x%X", __FUNCTION__, nAuthContext, nAuthLength, nAppType);
    PrintBufferDump("Auth", pAuth, nAuthLength);

    int nAuthType = -1;

    ProtocolUtils::LimitLengthToMax<int>(RIL_LOG_CAT_SIM, __CLASS_FUNC__, nAuthLength, MAX_SIM_AUTH_REQ_LEN);
    if(nAppType==RIL_APPTYPE_ISIM) nAuthType = SIT_SIM_AUTH_IMS;
    else {
        /* _____XXX
         * 000 : GSM context
         * 001 : 3G context
         * 010 : VGCS/VBS context
         * 100 : GBA context
         */
        const int AUTH_GSM_CONTEXT = 0b00000000;
        const int AUTH_3G_CONTEXT = 0b00000001;
        const int AUTH_VGCS_VBS_CONTEXT = 0b00000010;
        const int AUTH_GBA_CONTEXT = 0b00000100;

        // After removing Specific reference data
        switch((nAuthContext & 0x00000007))
        {
        case AUTH_GSM_CONTEXT: nAuthType = SIT_SIM_AUTH_GSM; break;
        case AUTH_3G_CONTEXT: nAuthType = SIT_SIM_AUTH_3G; break;
        case AUTH_VGCS_VBS_CONTEXT:
        case AUTH_GBA_CONTEXT:
        default:
            RilLogE("%s() Not Supported AuthContext(0x%08X)", __FUNCTION__, nAuthContext);
            return NULL;
        }
    }

    Json::Value jsonReqData;
    jsonReqData[ns_sim_get_sim_auth_req::key_auth_type] = nAuthType;
    if(nAuthType == SIT_SIM_AUTH_GSM) {
        if(pAuth != NULL) {
            // see sit_sim_get_sim_auth_req
            int i = 0, j = 0;
            int randLen = j < nAuthLength  ? pAuth[j] : 0;
            i = 0;  j++;
            for(  ; i < randLen && j < nAuthLength ; i++, j++) {
                    jsonReqData[ns_sim_get_sim_auth_req::key_rand].append((int) pAuth[j]);
            }

            // handling out of bound cases
            if (randLen == 0)  jsonReqData[ns_sim_get_sim_auth_req::key_rand] = Json::arrayValue;
        } else {
            //sending empty array as pAuth is NULL
            jsonReqData[ns_sim_get_sim_auth_req::key_rand] = Json::arrayValue;
        }
     } else {
        if(pAuth != NULL) {
            // see sit_sim_get_sim_auth_req
            int i = 0, j = 0;
            int randLen = j < nAuthLength  ? pAuth[j] : 0;
            i = 0;  j++;
            for(  ; i < randLen && j < nAuthLength ; i++, j++) {
                    jsonReqData[ns_sim_get_sim_auth_req::key_rand].append((int) pAuth[j]);
            }

            int autnLen = j < nAuthLength ? pAuth[j]: 0;
            i = 0; j++;
            for( ; i < autnLen && j < nAuthLength; i++, j++) {
                    jsonReqData[ns_sim_get_sim_auth_req::key_autn].append((int) pAuth[j]);
            }

            // handling out of bound cases
            if(randLen == 0)  jsonReqData[ns_sim_get_sim_auth_req::key_rand] = Json::arrayValue;
            if(autnLen == 0)  jsonReqData[ns_sim_get_sim_auth_req::key_autn] = Json::arrayValue;
        } else {
            // sending empty array as pAuth is NULL
            jsonReqData[ns_sim_get_sim_auth_req::key_rand] = Json::arrayValue;
            jsonReqData[ns_sim_get_sim_auth_req::key_autn] = Json::arrayValue;
        }
    }
    return GetJsonReqModemData(jsonReqData, SIT_GET_SIM_AUTH);
}

ModemData *ProtocolSimBuilderJson::BuildSimTransmitApduBasic(int nSessionID, int cla, int instruction,
                                                int p1, int p2, int p3, const char *pszApduData)
{
    Json::Value jsonReqData;
    jsonReqData[ns_sim_transmit_sim_apdu_basic_req::key_sid] = nSessionID;
    jsonReqData[ns_sim_transmit_sim_apdu_basic_req::key_data].append(cla);
    jsonReqData[ns_sim_transmit_sim_apdu_basic_req::key_data].append(instruction);
    jsonReqData[ns_sim_transmit_sim_apdu_basic_req::key_data].append(p1);
    jsonReqData[ns_sim_transmit_sim_apdu_basic_req::key_data].append(p2);
    jsonReqData[ns_sim_transmit_sim_apdu_basic_req::key_data].append(p3);

    int nDataLen = 0;
    if(pszApduData != NULL && strlen(pszApduData) > 0) {
        int nDataLength = strlen(pszApduData);
        BYTE *pData = new BYTE[nDataLength];
        memset(pData, 0, nDataLength);
        nDataLen = HexString2Value(pData, nDataLength, pszApduData);
        if((nDataLen + sizeof(sit_sim_apdu)) > MAX_APDU_LEN) nDataLen = MAX_APDU_LEN - sizeof(sit_sim_apdu);
        for(int i = 0; i < nDataLen; i++) {
            jsonReqData[ns_sim_transmit_sim_apdu_basic_req::key_data].append((int) pData[i]);
        }
        delete []pData;
    }

    return GetJsonReqModemData(jsonReqData, SIT_TRANSMIT_SIM_APDU_BASIC);
}

ModemData *ProtocolSimBuilderJson::BuildSimOpenChannel(const char *pszAID)
{
    Json::Value jsonReqData;
    jsonReqData[ns_sim_open_sim_channel_req::key_aid] = Json::arrayValue;
    if(pszAID != NULL) {
        BYTE aid[MAX_SIM_AID_LEN];
        memset(aid, 0, MAX_SIM_AID_LEN);
        int aidLen = HexString2Value(aid, sizeof(aid), pszAID);
        for(int i = 0; i < aidLen; i++) {
            jsonReqData[ns_sim_open_sim_channel_req::key_aid].append((int) aid[i]);
        }
    }
    return GetJsonReqModemData(jsonReqData, SIT_OPEN_SIM_CHANNEL);
}

ModemData *ProtocolSimBuilderJson::BuildSimOpenChannelWithP2(const char *pszAID, int p2)
{
    Json::Value jsonReqData;
    jsonReqData[ns_sim_open_sim_channel_with_p2_req::key_aid] = Json::arrayValue;
    if(pszAID != NULL) {
        BYTE aid[MAX_SIM_AID_LEN];
        memset(aid, 0, MAX_SIM_AID_LEN);
        int aidLen = HexString2Value(aid, sizeof(aid), pszAID);
        for(int i = 0; i < aidLen; i++) {
            jsonReqData[ns_sim_open_sim_channel_with_p2_req::key_aid].append((int) aid[i]);
        }
    }
    jsonReqData[ns_sim_open_sim_channel_with_p2_req::key_p2] = p2;

    return GetJsonReqModemData(jsonReqData, SIT_OPEN_SIM_CHANNEL_WITH_P2);
}

ModemData *ProtocolSimBuilderJson::BuildSimCloseChannel(int nSessionID)
{
    Json::Value jsonReqData;
    jsonReqData[ns_sim_close_sim_channel_req::key_sid] = nSessionID;

    return GetJsonReqModemData(jsonReqData, SIT_CLOSE_SIM_CHANNEL);
}

ModemData *ProtocolSimBuilderJson::BuildSimTransmitApduChannel(int nSessionID, int cla, int instruction,
                                                int p1, int p2, int p3, const char *pszApduData)
{
    Json::Value jsonReqData;
    jsonReqData[ns_sim_transmit_sim_apdu_channel_req::key_sid] = nSessionID;
    jsonReqData[ns_sim_transmit_sim_apdu_channel_req::key_cla] = cla;
    jsonReqData[ns_sim_transmit_sim_apdu_channel_req::key_instruction] = instruction;
    jsonReqData[ns_sim_transmit_sim_apdu_channel_req::key_p1] = p1;
    jsonReqData[ns_sim_transmit_sim_apdu_channel_req::key_p2] = p2;
    jsonReqData[ns_sim_transmit_sim_apdu_channel_req::key_p3] = p3;
    jsonReqData[ns_sim_transmit_sim_apdu_channel_req::key_data] = Json::arrayValue;

    unsigned int nDataLen = 0;
    if(pszApduData != NULL && strlen(pszApduData) > 0) {
        unsigned int nDataLength = strlen(pszApduData);
        BYTE *pData = new BYTE[nDataLength];
        memset(pData, 0, nDataLength);
        nDataLen = HexString2Value(pData, nDataLength, pszApduData);
        ProtocolUtils::LimitLengthToMax<unsigned int>(RIL_LOG_CAT_SIM, __CLASS_FUNC__, nDataLen, MAX_APDU_LEN);
        nDataLen = (nDataLen <= MAX_APDU_LEN)? nDataLen : MAX_APDU_LEN;
        for(unsigned int i = 0; i < nDataLen; i++) {
            jsonReqData[ns_sim_transmit_sim_apdu_channel_req::key_data].append((int) pData[i]);
        }
        delete []pData;
    }

    return GetJsonReqModemData(jsonReqData, SIT_TRANSMIT_SIM_APDU_CHANNEL);
}

ModemData *ProtocolSimBuilderJson::BuildGetImsi(const char *pszAID)
{
    Json::Value jsonReqData;
    jsonReqData[ns_sim_id_get_imsi_req::key_aid] = Json::arrayValue;

    if(pszAID != NULL && *pszAID != 0) {
        BYTE aid[MAX_SIM_AID_LEN];
        int aidLen = HexString2Value(aid, sizeof(aid), pszAID);
        for(int i = 0; i < aidLen; i++) {
            jsonReqData[ns_sim_id_get_imsi_req::key_aid].append((int) aid[i]);
        }
    }

    return GetJsonReqModemData(jsonReqData, SIT_GET_IMSI);
}

ModemData *ProtocolSimBuilderJson::BuildSimGetGbaAuth(const char *pGetGbaAuthdata, int /* dataSize */)
{
    if(pGetGbaAuthdata == NULL) return NULL;

    Json::Value jsonReqData;
    if(pGetGbaAuthdata[0] != 0 && pGetGbaAuthdata[0] != 1) return NULL;
    jsonReqData[ns_sim_get_sim_gba_auth_req::key_auth_type] = pGetGbaAuthdata[0];
    if(pGetGbaAuthdata[1] != 0 && pGetGbaAuthdata[1] != 1) return NULL;
    jsonReqData[ns_sim_get_sim_gba_auth_req::key_gba_type] = pGetGbaAuthdata[1];
    jsonReqData[ns_sim_get_sim_gba_auth_req::key_gba_tag] = pGetGbaAuthdata[2];
    int nData1Len = MIN(pGetGbaAuthdata[3], MAX_GBA_AUTH_REQ_DATA_LEN);

    jsonReqData[ns_sim_get_sim_gba_auth_req::key_data1] = Json::arrayValue;
    jsonReqData[ns_sim_get_sim_gba_auth_req::key_data2] = Json::arrayValue;

    // copy data from index 4 to index 4+nData1Len
    for(int i = 0; i < nData1Len; i++) {
        jsonReqData[ns_sim_get_sim_gba_auth_req::key_data1].append(pGetGbaAuthdata[i+4]);
    }
    int nData2Len = MIN(pGetGbaAuthdata[4 + MAX_GBA_AUTH_REQ_DATA_LEN], MAX_GBA_AUTH_REQ_DATA_LEN);
    // copy data from index 4+255+1 to index 4+255+1+nData2Len
    for(int i = 0; i < nData2Len; i++) {
        jsonReqData[ns_sim_get_sim_gba_auth_req::key_data2].append(pGetGbaAuthdata[i+4+255+1]);
    }
    return GetJsonReqModemData(jsonReqData, SIT_GET_GBA_CONTEXT);
}

ModemData *ProtocolSimBuilderJson::BuildSimGetATR()
{
    return GetJsonNullReqModemData(SIT_GET_ATR);
}

/* PhoneBook */
ModemData *ProtocolSimBuilderJson::BuildSimReadPbEntry(int pb_type, int recordId)
{
    Json::Value jsonReqData;
    jsonReqData[ns_read_pb_entry_req::key_pb_type] = pb_type;
    jsonReqData[ns_read_pb_entry_req::key_record_id] = recordId;

    return GetJsonReqModemData(jsonReqData, SIT_READ_PB_ENTRY);
}

ModemData *ProtocolSimBuilderJson::BuildSimUpdatePbDelete(int mode, int type, int recordId)
{
    Json::Value jsonReqData;
    jsonReqData[ns_update_pb_entry_req::key_mode] = mode;
    jsonReqData[ns_update_pb_entry_req::key_pb_type] = type;
    jsonReqData[ns_update_pb_entry_req::key_record_id] = recordId;

    return GetJsonReqModemData(jsonReqData, SIT_UPDATE_PB_ENTRY);
}

ModemData *ProtocolSimBuilderJson::BuildSimUpdatePb3gEntry(int mode, int type, int recordId, int length, char *pb)
{
    Json::Value jsonReqData;
    ProtocolUtils::LimitLengthToMax<int>(RIL_LOG_CAT_SIM, __CLASS_FUNC__, length, MAX_PB_ENTRY_LEN);
    jsonReqData[ns_update_pb_entry_req::key_mode] = mode;
    jsonReqData[ns_update_pb_entry_req::key_pb_type] = type;
    jsonReqData[ns_update_pb_entry_req::key_record_id] = recordId;
    jsonReqData[ns_update_pb_entry_req::key_entry_data] = Json::arrayValue;
    for(int i = 0; i < length; i++) {
        jsonReqData[ns_update_pb_entry_req::key_entry_data].append((int) pb[i]);
    }

    return GetJsonReqModemData(jsonReqData, SIT_UPDATE_PB_ENTRY);
}

ModemData *ProtocolSimBuilderJson::BuildSimGetPbStorageInfo(int pbType)
{
    Json::Value jsonReqData;
    jsonReqData[ns_sim_pb_storage_info::key_pb_type] = pbType;

    return GetJsonReqModemData(jsonReqData, SIT_GET_PB_STORAGE_INFO);
}

ModemData *ProtocolSimBuilderJson::BuildSim3GPbCapa()
{
    return GetJsonNullReqModemData(SIT_GET_3G_PB_CAPA);
}

ModemData *ProtocolSimBuilderJson::BuildSetCarrierRestrictions(int nAllowLength, CarrierInfo *pAllowed, int nExcludeLength, CarrierInfo *pExcluded,
                                                            bool m_bAllowedPrioritized, int nMultiSimPolicy)
{
    Json::Value jsonReqData;

    // allowed list
    if (nAllowLength == 0 || pAllowed == NULL) {
        jsonReqData[ns_sim_net_getset_carrier_restrictions::key_allowed_list] = Json::arrayValue;
    } else {
        for (int i = 0; i < nAllowLength; i++) {
            Json::Value carrierInfo;
            string plmn(pAllowed[i].mcc);
            plmn = plmn + pAllowed[i].mnc;
            carrierInfo[ns_sim_net_carrier_info::key_plmn] = plmn;
            carrierInfo[ns_sim_net_carrier_info::key_match_type] = (int)(pAllowed[i].match_type);
            int matchDataLen = strlen(pAllowed[i].match_data);
            if (matchDataLen == 0) {
                carrierInfo[ns_sim_net_carrier_info::key_match_data_list] = Json::arrayValue;
            } else {
                for(int j = 0; j < matchDataLen && j < MAX_CR_MATCH_DATA_SIZE; j++) {
                    carrierInfo[ns_sim_net_carrier_info::key_match_data_list].append((int) pAllowed[i].match_data[j]);
                }
            }
            jsonReqData[ns_sim_net_getset_carrier_restrictions::key_allowed_list].append(carrierInfo);
        }
    }

    // excluded list
    if (nExcludeLength == 0 || pExcluded == NULL) {
        jsonReqData[ns_sim_net_getset_carrier_restrictions::key_excluded_list] = Json::arrayValue;
    } else {
        for (int i = 0; i < nExcludeLength; i++) {
            Json::Value carrierInfo;
            string plmn(pExcluded[i].mcc);
            plmn = plmn + pExcluded[i].mnc;
            carrierInfo[ns_sim_net_carrier_info::key_plmn] = plmn;
            carrierInfo[ns_sim_net_carrier_info::key_match_type] = (int)(pExcluded[i].match_type);
            int matchDataLen = strlen(pExcluded[i].match_data);
            if (matchDataLen == 0) {
                carrierInfo[ns_sim_net_carrier_info::key_match_data_list] = Json::arrayValue;
            } else {
                for(int j = 0; j < matchDataLen && j < MAX_CR_MATCH_DATA_SIZE; j++) {
                    carrierInfo[ns_sim_net_carrier_info::key_match_data_list].append((int) pExcluded[i].match_data[j]);
                }
            }
            jsonReqData[ns_sim_net_getset_carrier_restrictions::key_excluded_list].append(carrierInfo);
        }
    }

    // Below members are added for IRADIO v1.4,
    // but not included to this structure
    // because of variable about CARRIER_INFO Lists.
    jsonReqData[ns_sim_net_getset_carrier_restrictions::key_allowed_priority] = (int)m_bAllowedPrioritized;
    jsonReqData[ns_sim_net_getset_carrier_restrictions::key_multi_simlock] = nMultiSimPolicy;

    return GetJsonReqModemData(jsonReqData, SIT_SET_CARRIER_RESTRICTIONS);
}

ModemData *ProtocolSimBuilderJson::BuildGetCarrierRestrictions()
{
    return GetJsonNullReqModemData(SIT_GET_CARRIER_RESTRICTIONS);
}

ModemData *ProtocolSimBuilderJson::BuildSetUicc(int activeStatus)
{
    Json::Value jsonReqData;
    jsonReqData[ns_sim_set_uicc_sub_req::key_state] = activeStatus;

    return GetJsonReqModemData(jsonReqData, SIT_SET_UICC_SUBSCRIPTION);
}

ModemData *ProtocolSimBuilderJson::BuildSetSimCardPower(int isPowerUp)
{
    Json::Value jsonReqData;
    jsonReqData[ns_sim_set_sim_card_power_req::key_mode] = isPowerUp;

    return GetJsonReqModemData(jsonReqData, SIT_SET_SIM_CARD_POWER);
}

// Secure Element
ModemData *ProtocolSimBuilderJson::BuildOemSimRequest(int /* msgId */, BYTE * /* pData */, int /* nDataLength */) {
    return NULL;
}

ModemData *ProtocolSimBuilderJson::BuildGetSimLockInfo()
{
    return NULL;
}

#ifdef AUTO_VERIFY_PIN_WITH_CP
ModemData *ProtocolSimBuilderJson::BuildSimVerifyEncryptedPin(
        int /* nPinLen */, const char * /* pszEncryptedPin */, const char * /* pszAID */) {
    return NULL;
}
#endif
