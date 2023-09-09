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
 * protocolsimbuilderlegacy.cpp
 *
 *  Created on: 2014. 6. 28.
 *      Author: MOX
 */

#include "uicc/protocolsimbuilderlegacy.h"
#include "rillog.h"
#include "modemdata.h"
#include "util.h"

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

ModemData *ProtocolSimBuilderLegacy::BuildSimGetStatus()
{
    null_data_format req;
    int length = sizeof(null_data_format);
    memset(&req, 0, length);
    InitRequestHeader(&req.hdr, SIT_GET_SIM_STATUS, length);
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolSimBuilderLegacy::BuildSimVerifyPin(int nPinIndex, const char *pszPin, const char *pszAID)
{
    sit_sim_verify_sim_pin_req req;
    int length = sizeof(req);
    int nReqID;

    switch(nPinIndex)
    {
    case PIN1_PUK1: nReqID = SIT_VERIFY_SIM_PIN; break;
    case PIN2_PUK2: nReqID = SIT_VERIFY_SIM_PIN2; break;
    default:
        return NULL;
    }

    if(pszPin==NULL) return NULL;

    memset(&req, 0, length);
    InitRequestHeader(&req.hdr, nReqID, length);

    req.pin_len = (strlen(pszPin)<=MAX_SIM_PIN_LEN)? strlen(pszPin): MAX_SIM_PIN_LEN;
    memcpy(req.pin, pszPin, req.pin_len);
    //req.pin[req.pin_len] = '\0';

    if (pszAID != NULL) {
        // convert HEX string to HEX value
        req.aid_len = (BYTE) HexString2Value(req.aid, sizeof(req.aid), pszAID);
    }
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolSimBuilderLegacy::BuildSimVerifyPuk(int nPukIndex, const char *pszPuk, const char *pszNewPin, const char *pszAID)
{
    sit_sim_verify_sim_puk_req req;
    int length = sizeof(req);
    int nReqID;

    switch(nPukIndex)
    {
    case PIN1_PUK1: nReqID = SIT_VERIFY_SIM_PUK; break;
    case PIN2_PUK2: nReqID = SIT_VERIFY_SIM_PUK2; break;
    default:
        return NULL;
    }

    if(pszPuk==NULL || pszNewPin==NULL) return NULL;

    memset(&req, 0, length);
    InitRequestHeader(&req.hdr, nReqID, length);

    req.puk_len = (strlen(pszPuk)<=MAX_SIM_PUK_LEN)? strlen(pszPuk): MAX_SIM_PUK_LEN;
    memcpy(req.puk, pszPuk, req.puk_len);
    //req.puk[req.puk_len] = '\0';

    req.new_pin_len = (strlen(pszNewPin)<=MAX_SIM_PIN_LEN)? strlen(pszNewPin): MAX_SIM_PIN_LEN;
    memcpy(req.new_pin, pszNewPin, req.new_pin_len);
    //req.new_pin[req.new_pin_len] = '\0';

    if (pszAID != NULL) {
        // convert HEX string to HEX value
        req.aid_len = (BYTE) HexString2Value(req.aid, sizeof(req.aid), pszAID);
    }
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolSimBuilderLegacy::BuildSimChangePin(int nPinIndex, const char *pszOldPin, const char *pszNewPin, const char *pszAID)
{
    sit_sim_change_sim_pin_req req;
    int length = sizeof(req);
    int nReqID, nFAC = 0;

    switch(nPinIndex)
    {
    case PIN1_PUK1: nReqID = SIT_CHG_SIM_PIN; nFAC = SIT_SIM_FAC_SC; break;
    case PIN2_PUK2: nReqID = SIT_CHG_SIM_PIN2; nFAC = SIT_SIM_FAC_SC2; break;
    default:
        return NULL;
    }

    if(pszOldPin==NULL || pszNewPin==NULL) return NULL;

    memset(&req, 0, length);
    InitRequestHeader(&req.hdr, nReqID, length);

    req.fac = nFAC;
    req.old_pin_len = (strlen(pszOldPin)<=MAX_SIM_PIN_LEN)? strlen(pszOldPin): MAX_SIM_PIN_LEN;
    memcpy(req.old_pin, pszOldPin, req.old_pin_len);
    //req.old_pin[req.old_pin_len] = '\0';

    req.new_pin_len = (strlen(pszNewPin)<=MAX_SIM_PIN_LEN)? strlen(pszNewPin): MAX_SIM_PIN_LEN;
    memcpy(req.new_pin, pszNewPin, req.new_pin_len);
    //req.new_pin[req.new_pin_len] = '\0';

    if (pszAID != NULL) {
        // convert HEX string to HEX value
        req.aid_len = (BYTE) HexString2Value(req.aid, sizeof(req.aid), pszAID);
    }

    return new ModemData((char *)&req, length);
}

ModemData *ProtocolSimBuilderLegacy::BuildSimVerifyNetworkLock(int nFac, const char *pszPassword, int nSvcClass, const char *pszAID)
{
    sit_sim_verify_network_lock_req req;
    int length = sizeof(req);

    if(pszPassword==NULL) return NULL;

    memset(&req, 0, length);
    InitRequestHeader(&req.hdr, SIT_VERIFY_NETWORK_LOCK, length);
    req.fac = (BYTE) nFac;

    req.fac = nFac;
    req.password_len = (strlen(pszPassword)<=MAX_SIM_FACILITY_PASSWORD_LEN)? strlen(pszPassword): MAX_SIM_FACILITY_PASSWORD_LEN;
    memcpy(req.password, pszPassword, req.password_len);
    //req.password[req.password_len] = '\0';

    req.service_class = (BYTE) nSvcClass;
    if (pszAID != NULL) {
        // convert HEX string to HEX value
        req.aid_len = (BYTE) HexString2Value(req.aid, sizeof(req.aid), pszAID);
    }
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolSimBuilderLegacy::BuildSimIO(int nCmd, int nAppType, int nFileID, const char *pPath, int p1, int p2, int p3,
                                                int /* nDataLen */, const char *pData, const char *pszPin2, const char *pszAID)
{
    sit_sim_sim_io_req req;
    int length = sizeof(req);

    if(pPath==NULL || pData==NULL) return NULL;

    BYTE abCommandValues[] = { 0xB0, 0xB2, 0xC0, 0xD6, 0xDC, 0xF2 };
    char aszCommands[][16] = {"READ_BINARY", "READ_RECORD", "GET_RESPONSE", "UPDATE_BINARY", "UPDATE_RECORD", "STATUS" };
    char szCmd[16] = "Unknown";
    for(int i=0; i<(int)(sizeof(abCommandValues)/sizeof(BYTE)); i++) {
        if(nCmd==(int) abCommandValues[i]) {
            SECURELIB::strncpy(szCmd, sizeof(szCmd), aszCommands[i], SECURELIB::strlen(aszCommands[i]));
        }
    }
    RilLogV("ProtocolSimBuilderLegacy::%s() Cmd:%s(0x%02X), AppType:%d, FileID:%X, Path:%s", __FUNCTION__, szCmd, nCmd, nAppType, nFileID, pPath);
    RilLogV("ProtocolSimBuilderLegacy::%s() P1:%d=0x%02X, P2:%d=0x%02X, P3:%d=0x%02X", __FUNCTION__, p1, p1, p2, p2, p3, p3);
    if(pszPin2!=NULL && strlen(pszPin2)>0) RilLogV("ProtocolSimBuilderLegacy::%s() PIN2:%s", __FUNCTION__, pszPin2);

    memset(&req, 0, length);
    InitRequestHeader(&req.hdr, SIT_SIM_IO, length);
    req.command = (BYTE) nCmd;
    req.app_type = (BYTE) nAppType;
    req.file_id = (INT16) nFileID;

    if (pPath != NULL && strlen(pPath)>0)
    {
        // convert HEX string to HEX value
        BYTE arPath[MAX_SIM_IO_PATH_LEN] = { 0, };
        req.path_len = (BYTE)HexString2Value(arPath, sizeof(arPath), pPath);
        if ( req.path_len <= 0 )
        {
            return NULL;
        }

        // Swap for Big-Endian
        for(int i=0; i<req.path_len; i+=2)
        {
            //WORD wPath16 = *((WORD *) &arPath[i]);
            //*((WORD *) &arPath[i]) = SWAP16(wPath16);
            *((WORD *) &arPath[i]) = SWAP16(*((WORD *) &arPath[i]));
        }
        memcpy(req.path, arPath, (size_t)req.path_len);
    }

    req.p1 = (BYTE) p1;
    req.p2 = (BYTE) p2;
    req.p3 = (BYTE) p3;

    if(pData!=NULL)
    {
        //req.data_len = (WORD) (nDataLen/2);
        //memcpy(req.data, pData, nDataLen/2);
        req.data_len = (WORD) HexString2Value(req.data, sizeof(req.data), pData);
        RilLogV("ProtocolSimBuilderLegacy::%s() Data Length: %d", __FUNCTION__, req.data_len);
    }

    if(pszPin2!=NULL)
    {
        req.pin2_len = (strlen(pszPin2)<=MAX_SIM_PIN_LEN)? strlen(pszPin2): MAX_SIM_PIN_LEN;
        memcpy(req.pin2, pszPin2, req.pin2_len);
        //req.pin2[req.pin2_len] = '\0';
    }

    if (pszAID != NULL) {
        // convert HEX string to HEX value
        req.aid_len = (BYTE) HexString2Value(req.aid, sizeof(req.aid), pszAID);
    }
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolSimBuilderLegacy::BuildSimGetFacilityLock(char *pszCode, char *pszPassword, int nSvcClass, char *pszAID)
{
    sit_sim_get_facility_lock_req req;
    int length = sizeof(req);

    char aszFacCode[SIT_SIM_FAC_MAX][MAX_FACILITY_CODE_LEN+1] = { "CS", "PS", "PF", "SC", "AO",
                                                              "OI", "OX", "AI", "IR", "NT",
                                                              "NM", "NS", "NA", "AB", "AG",
                                                              "AC", "FD", "PN", "PU", "PP",
                                                              "PC", "SC2" };

    int nCode = SIT_SIM_FAC_MAX;
    for(int i=0; i<SIT_SIM_FAC_MAX; i++)
    {
        if(strcmp(pszCode, aszFacCode[i])==0) { nCode = i; break; }
    }

    RilLogV("ProtocolSimBuilderLegacy::%s() Code:%d, Password:%s, ServiceClass:%d, AID:%s", __FUNCTION__, nCode, pszPassword, nSvcClass, pszAID);

    if(pszPassword==NULL) return NULL;

    memset(&req, 0, length);
    InitRequestHeader(&req.hdr, SIT_GET_FACILITY_LOCK, length);
    req.code = (BYTE) nCode;

    if(strlen(pszPassword)>0)
    {
        req.password_len = (strlen(pszPassword)<=MAX_SIM_FACILITY_PASSWORD_LEN)? strlen(pszPassword): MAX_SIM_FACILITY_PASSWORD_LEN;
        memcpy(req.password, pszPassword, req.password_len);
        //req.password[req.password_len] = '\0';
    }

    req.service_class = (BYTE) nSvcClass;
    if (pszAID != NULL) {
        // convert HEX string to HEX value
        req.aid_len = (BYTE) HexString2Value(req.aid, sizeof(req.aid), pszAID);
    }
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolSimBuilderLegacy::BuildSimSetFacilityLock(char *pszCode, int nLockMode, char *pszPassword, int nSvcClass, char *pszAID)
{
    sit_sim_set_facility_lock_req req;
    int length = sizeof(req);

    char aszFacCode[SIT_SIM_FAC_MAX][MAX_FACILITY_CODE_LEN+1] = { "CS", "PS", "PF", "SC", "AO",
                                                                  "OI", "OX", "AI", "IR", "NT",
                                                                  "NM", "NS", "NA", "AB", "AG",
                                                                  "AC", "FD", "PN", "PU", "PP",
                                                                  "PC", "SC2" };

    int nCode = SIT_SIM_FAC_MAX;
    for(int i=0; i<SIT_SIM_FAC_MAX; i++)
    {
        if(strcmp(pszCode, aszFacCode[i])==0) { nCode = i; break; }
    }

    RilLogV("ProtocolSimBuilderLegacy::%s() Code:%d, Password:%s, ServiceClass:%d, AID:%s", __FUNCTION__, nCode, pszPassword, nSvcClass, pszAID);

    if(pszPassword==NULL) return NULL;

    memset(&req, 0, length);
    InitRequestHeader(&req.hdr, SIT_SET_FACILITY_LOCK, length);
    req.code = (BYTE) nCode;
    req.lock_mode = (BYTE) nLockMode;


    req.password_len = (strlen(pszPassword)<=MAX_SIM_FACILITY_PASSWORD_LEN)? strlen(pszPassword): MAX_SIM_FACILITY_PASSWORD_LEN;
    memcpy(req.password, pszPassword, req.password_len);
    //req.password[req.password_len] = '\0';

    req.service_class = (BYTE) nSvcClass;
    if (pszAID != NULL) {
        // convert HEX string to HEX value
        req.aid_len = (BYTE) HexString2Value(req.aid, sizeof(req.aid), pszAID);
    }
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolSimBuilderLegacy::BuildSimGetIsimAuth(int nAuthType, BYTE *pAuth, int nAuthLengh)
{
    sit_sim_get_sim_auth_req req = {};
    int length = sizeof(sit_sim_get_sim_auth_req);
    InitRequestHeader(&req.hdr, SIT_GET_SIM_AUTH, length);

    req.auth_type = (BYTE) nAuthType;
    if (pAuth != NULL) {
        req.auth_len =
                (BYTE)(nAuthLengh < MAX_SIM_AUTH_REQ_LEN) ? nAuthLengh : MAX_SIM_AUTH_REQ_LEN;
        memcpy(req.auth, pAuth, req.auth_len);
    }
    return new ModemData((char *) &req, length);
}

ModemData *ProtocolSimBuilderLegacy::BuildSimGetSimAuth(int nAuthContext, BYTE *pAuth,
        int nAuthLengh, int nAppType)
{
    RilLogV("ProtocolSimBuilderLegacy::%s() nAuthContext:0x%X, nAuthLengh:%d, nAppType:0x%X",
            __FUNCTION__, nAuthContext, nAuthLengh, nAppType);
    PrintBufferDump("Auth", pAuth, nAuthLengh);

    int nAuthType = -1;
    if (nAppType == RIL_APPTYPE_ISIM) {
        nAuthType = SIT_SIM_AUTH_IMS;
    } else {
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
        switch ((nAuthContext & 0x00000007)) {
        case AUTH_GSM_CONTEXT: nAuthType = SIT_SIM_AUTH_GSM; break;
        case AUTH_3G_CONTEXT: nAuthType = SIT_SIM_AUTH_3G; break;
        case AUTH_VGCS_VBS_CONTEXT:
        case AUTH_GBA_CONTEXT:
        default:
            RilLogE("%s() Not Supported AuthContext(0x%08X)", __FUNCTION__, nAuthContext);
            return NULL;
        }
    }

    sit_sim_get_sim_auth_req req = {};
    int length = sizeof(sit_sim_get_sim_auth_req);
    InitRequestHeader(&req.hdr, SIT_GET_SIM_AUTH, length);
    req.auth_type = (BYTE) nAuthType;
    if (pAuth != NULL && nAuthLengh > 0) {
        req.auth_len =
                (BYTE)(nAuthLengh < MAX_SIM_AUTH_REQ_LEN) ? nAuthLengh : MAX_SIM_AUTH_REQ_LEN;
        memcpy(req.auth, pAuth, req.auth_len);
    }
    return new ModemData((char *) &req, length);
}

ModemData *ProtocolSimBuilderLegacy::BuildSimTransmitApduBasic(int nSessionID, int cla, int instruction,
                                                int p1, int p2, int p3, const char *pszApduData)
{
    int nLength = sizeof(sit_sim_transmit_sim_apdu_basic_req);
    if(pszApduData!=NULL && strlen(pszApduData)>0) nLength += strlen(pszApduData);
    BYTE *pBuffer = new BYTE[nLength];
    sit_sim_transmit_sim_apdu_basic_req *pReq = (sit_sim_transmit_sim_apdu_basic_req *) pBuffer;
    memset(pReq, 0, nLength);
    InitRequestHeader(&pReq->hdr, SIT_TRANSMIT_SIM_APDU_BASIC, nLength);
    pReq->session_id = nSessionID;
    pReq->apdu_len = sizeof(sit_sim_apdu);
    pReq->entry.sim_apdu.cla = cla;
    pReq->entry.sim_apdu.instruction = instruction;
    pReq->entry.sim_apdu.p1 = p1;
    pReq->entry.sim_apdu.p2 = p2;
    pReq->entry.sim_apdu.p3 = p3;

    int nDataLen = 0;
    if(pszApduData != NULL && strlen(pszApduData)>0)
    {
        int nDataLength = strlen(pszApduData);
        BYTE *pData = new BYTE[nDataLength];
        memset(pData, 0, nDataLength);
        nDataLen = HexString2Value(pData, nDataLength, pszApduData);
        if((nDataLen+sizeof(sit_sim_apdu))>MAX_APDU_LEN) nDataLen = MAX_APDU_LEN - sizeof(sit_sim_apdu);
        memcpy(pReq->entry.sim_apdu.data, pData, nDataLen);
        delete []pData;
    }

    pReq->apdu_len += nDataLen;
    ModemData *pModemData = new ModemData((char *) pReq, nLength);
    delete [] pBuffer;

    return pModemData;
}

ModemData *ProtocolSimBuilderLegacy::BuildSimOpenChannel(const char *pszAID)
{
    sit_sim_open_sim_channel_req req;
    int length = sizeof(req);

    memset(&req, 0, length);
    InitRequestHeader(&req.hdr, SIT_OPEN_SIM_CHANNEL, length);
    if(pszAID!=NULL)
    {
        // convert HEX string to HEX value
        req.aid_len = (BYTE) HexString2Value(req.aid, sizeof(req.aid), pszAID);
    }

    return new ModemData((char *)&req, length);
}

ModemData *ProtocolSimBuilderLegacy::BuildSimOpenChannelWithP2(const char *pszAID, int p2)
{
    sit_sim_open_sim_channel_with_p2_req req;
    int length = sizeof(req);

    memset(&req, 0, length);
    InitRequestHeader(&req.hdr, SIT_OPEN_SIM_CHANNEL_WITH_P2, length);
    if(pszAID!=NULL)
    {
        // convert HEX string to HEX value
        req.aid_len = (BYTE) HexString2Value(req.aid, sizeof(req.aid), pszAID);
    }

    req.p2 = (BYTE) p2;

    return new ModemData((char *)&req, length);
}

ModemData *ProtocolSimBuilderLegacy::BuildSimCloseChannel(int nSessionID)
{
    sit_sim_close_sim_channel_req req;
    int length = sizeof(req);
    memset(&req, 0, length);
    InitRequestHeader(&req.hdr, SIT_CLOSE_SIM_CHANNEL, length);
    req.session_id = nSessionID;
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolSimBuilderLegacy::BuildSimTransmitApduChannel(int nSessionID, int cla, int instruction,
                                                int p1, int p2, int p3, const char *pszApduData)
{
    int nLength = sizeof(sit_sim_transmit_sim_apdu_channel_req);
    if(pszApduData!=NULL && strlen(pszApduData)>0) nLength += strlen(pszApduData);
    BYTE *pBuffer = new BYTE[nLength];
    sit_sim_transmit_sim_apdu_channel_req *pReq = (sit_sim_transmit_sim_apdu_channel_req *) pBuffer;
    memset(pReq, 0, nLength);
    InitRequestHeader(&pReq->hdr, SIT_TRANSMIT_SIM_APDU_CHANNEL, nLength);
    pReq->session_id = nSessionID;
    pReq->cla = cla;
    pReq->instruction = instruction;
    pReq->p1 = p1;
    pReq->p2 = p2;
    pReq->p3 = p3;

    if(pszApduData != NULL && strlen(pszApduData)>0)
    {
        int nDataLength = strlen(pszApduData);
        BYTE *pData = new BYTE[nDataLength];
        memset(pData, 0, nDataLength);
        int nDataLen = HexString2Value(pData, nDataLength, pszApduData);
        pReq->data_len = (nDataLen<=MAX_APDU_LEN)? nDataLen: MAX_APDU_LEN;
        memcpy(pReq->data, pData, pReq->data_len);
        delete []pData;
    }

    ModemData *pModemData = new ModemData((char *) pReq, nLength);
    delete [] pBuffer;

    return pModemData;
}

ModemData *ProtocolSimBuilderLegacy::BuildGetImsi(const char *pszAID)
{
    sit_id_get_imsi_req req;
    int length = sizeof(req);
    memset(&req, 0, length);
    InitRequestHeader(&req.hdr, SIT_GET_IMSI, length);
    if (pszAID != NULL && *pszAID != 0) {
        // convert HEX string to HEX value
        req.aid_len = (BYTE) HexString2Value(req.aid, sizeof(req.aid), pszAID);
    }
    return new ModemData((char *)&req, length);
}
ModemData *ProtocolSimBuilderLegacy::BuildSimGetGbaAuth(const char *pGetGbaAuthdata, int dataSize)
{
    int gbaAuthReqSize = sizeof(sit_sim_get_sim_gba_auth_req) - sizeof(RCM_HEADER);
    if (pGetGbaAuthdata == nullptr || dataSize < gbaAuthReqSize) return nullptr;
    else {
        if (pGetGbaAuthdata[0] >= MAX_GBA_AUTH_TYPE || pGetGbaAuthdata[1] >= MAX_GBA_TYPE)
            return nullptr;
    }

    sit_sim_get_sim_gba_auth_req req = {};
    int length = sizeof(sit_sim_get_sim_gba_auth_req);
    InitRequestHeader(&req.hdr, SIT_GET_GBA_CONTEXT, length);

    if (dataSize > gbaAuthReqSize) dataSize = gbaAuthReqSize;
    memcpy(&req.auth_type, pGetGbaAuthdata, dataSize);
    return new ModemData((char *) &req, length);
}

ModemData *ProtocolSimBuilderLegacy::BuildSimGetATR()
{
    null_data_format req;
    int length = sizeof(null_data_format);
    memset(&req, 0, length);
    InitRequestHeader(&req.hdr, SIT_GET_ATR, length);
    return new ModemData((char *)&req, length);
}

/* PhoneBook */
ModemData *ProtocolSimBuilderLegacy::BuildSimReadPbEntry(int pb_type, int recordId)
{
    sit_read_pb_entry_req req;
    int length = sizeof(req);
    memset(&req, 0, length);
    InitRequestHeader(&req.hdr, SIT_READ_PB_ENTRY, length);
    req.pb_type = pb_type;
    req.recordId = recordId;
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolSimBuilderLegacy::BuildSimUpdatePbDelete(int mode, int type, int recordId)
{
    sit_update_pb_entry_req req = {};
    int length = sizeof(sit_update_pb_entry_req);
    InitRequestHeader(&req.hdr, SIT_UPDATE_PB_ENTRY, length);

    req.mode = mode;
    req.pb_type = type;
    req.recordId = recordId;
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolSimBuilderLegacy::BuildSimUpdatePb3gEntry(int mode, int type,
        int recordId, int length, char *pb)
{
    sit_update_pb_entry_req req = {};
    int reqLen = sizeof(sit_update_pb_entry_req);
    InitRequestHeader(&req.hdr, SIT_UPDATE_PB_ENTRY, reqLen);

    req.mode = mode;
    req.pb_type = type;
    req.recordId = recordId;
    if (length > MAX_PB_ENTRY_LEN) length = MAX_PB_ENTRY_LEN;
    req.entry_len = length;
    memcpy(req.entry, pb, length);

    return new ModemData((char *) &req, reqLen);
}

ModemData *ProtocolSimBuilderLegacy::BuildSimGetPbStorageInfo(int pbType)
{
    sit_sim_pb_storage_info req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_GET_PB_STORAGE_INFO, length);
    req.pb_type = pbType;

    return new ModemData((char *)&req, length);
}

ModemData *ProtocolSimBuilderLegacy::BuildSim3GPbCapa()
{
    null_data_format req;
    int length = sizeof(null_data_format);
    memset(&req, 0, length);
    InitRequestHeader(&req.hdr, SIT_GET_3G_PB_CAPA, length);

    return new ModemData((char *)&req, length);
}

ModemData *ProtocolSimBuilderLegacy::BuildSetCarrierRestrictions(int nAllowLength, CarrierInfo *pAllowed, int nExcludeLength, CarrierInfo *pExcluded,
                                                            bool m_bAllowedPrioritized, int nMultiSimPolicy)
{
    int length = sizeof(sit_sim_set_carrier_restriction_req);
    length += ((nAllowLength + nExcludeLength) * sizeof(SIT_CARRIER_INFO));
    // add size of allowed_carrier_prioritized and multi_sim_lock_policy
    length += (sizeof(BYTE) + sizeof(BYTE));
    BYTE *pData = new BYTE[length];
    if (pData == NULL) return NULL;

    memset(pData, 0, length);
    sit_sim_set_carrier_restriction_req *pReq = (sit_sim_set_carrier_restriction_req *) pData;
    InitRequestHeader(&pReq->hdr, SIT_SET_CARRIER_RESTRICTIONS, length);

    pReq->allowed_carriers_len = nAllowLength;
    pReq->excluded_carriers_len = nExcludeLength;

    for(int i = 0; i < nAllowLength; i++) {
        memcpy(pReq->carrier_list[i].mcc, pAllowed[i].mcc, MAX_MCC_LEN);
        memcpy(pReq->carrier_list[i].mnc, pAllowed[i].mnc, MAX_MNC_LEN);
        pReq->carrier_list[i].match_type = pAllowed[i].match_type;
        pReq->carrier_list[i].match_len = strlen(pAllowed[i].match_data);
        if (pReq->carrier_list[i].match_len > MAX_CR_MATCH_DATA_SIZE) pReq->carrier_list[i].match_len = MAX_CR_MATCH_DATA_SIZE;
        memcpy(pReq->carrier_list[i].match_data, pAllowed[i].match_data, pReq->carrier_list[i].match_len);
    }

    for(int i = 0; i < nExcludeLength; i++) {
        SIT_CARRIER_INFO& ci = pReq->carrier_list[i + nAllowLength];
        memcpy(ci.mcc, pExcluded[i].mcc, MAX_MCC_LEN);
        memcpy(ci.mnc, pExcluded[i].mnc, MAX_MNC_LEN);
        ci.match_type = pExcluded[i].match_type;
        ci.match_len = strlen(pExcluded[i].match_data);
        if (ci.match_len > MAX_CR_MATCH_DATA_SIZE) ci.match_len = MAX_CR_MATCH_DATA_SIZE;
        memcpy(ci.match_data, pExcluded[i].match_data, ci.match_len);
    }

    // Below members are added for IRADIO v1.4,
    // but not included to this structure
    // because of variable about CARRIER_INFO Lists.
    pData[length-2] = m_bAllowedPrioritized;
    pData[length-1] = (BYTE) nMultiSimPolicy;

    ModemData *pModem = new ModemData((char *)pReq, length);
    delete[] pData;

    return pModem;
}

ModemData *ProtocolSimBuilderLegacy::BuildGetCarrierRestrictions()
{
    sit_sim_get_carrier_restriction_req req;
    int length = sizeof(sit_sim_get_carrier_restriction_req);
    memset(&req, 0, length);
    InitRequestHeader(&req.hdr, SIT_GET_CARRIER_RESTRICTIONS, length);

    return new ModemData((char *)&req, length);
}

ModemData *ProtocolSimBuilderLegacy::BuildSetUicc(int activeStatus)
{
    sit_sim_set_uicc_sub_req req;
    int length = sizeof(sit_sim_set_uicc_sub_req);
    memset(&req, 0, length);
    InitRequestHeader(&req.hdr, SIT_SET_UICC_SUBSCRIPTION, length);
    req.state = activeStatus;
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolSimBuilderLegacy::BuildSetSimCardPower(int isPowerUp)
{
    sit_sim_set_sim_card_power_req req;
    int length = sizeof(sit_sim_set_sim_card_power_req);
    memset(&req, 0, length);
    InitRequestHeader(&req.hdr, SIT_SET_SIM_CARD_POWER, length);

    req.mode = isPowerUp;

    return new ModemData((char *)&req, length);
}

// Secure Element
ModemData *ProtocolSimBuilderLegacy::BuildOemSimRequest(int msgId, BYTE *pData, int nDataLength)
{
    int nSitMsg;
    switch(msgId)
    {
    // IMS SIM IO
    case RIL_REQUEST_SIM_IO: nSitMsg = SIT_SIM_IO; break;
    // Secure Element
    case RIL_REQUEST_SIM_OPEN_CHANNEL: nSitMsg = SIT_OPEN_SIM_CHANNEL_WITH_P2; break;
    case RIL_REQUEST_SIM_TRANSMIT_APDU_CHANNEL: nSitMsg = SIT_TRANSMIT_SIM_APDU_CHANNEL; break;
    case RIL_REQUEST_SIM_TRANSMIT_APDU_BASIC: nSitMsg = SIT_TRANSMIT_SIM_APDU_BASIC; break;
    default: return NULL;
    }

    BYTE *pBuffer = new BYTE[sizeof(null_data_format)+nDataLength];
    null_data_format *pReq = (null_data_format *) pBuffer;
    int length = sizeof(null_data_format) + nDataLength;
    InitRequestHeader(&pReq->hdr, nSitMsg, length);
    memcpy(pBuffer+sizeof(null_data_format), pData, nDataLength);

    ModemData *pModemData = new ModemData((char *)pReq, length);
    delete [] pBuffer;

    return pModemData;
}

ModemData *ProtocolSimBuilderLegacy::BuildGetSimLockInfo()
{
    sit_sim_get_sim_lock_info_req req;
    int length = sizeof(sit_sim_get_sim_lock_info_req);
    memset(&req, 0, length);
    InitRequestHeader(&req.hdr, SIT_OEM_GET_SIM_LOCK_INFO, length);
    return new ModemData((char *)&req, length);
}

#ifdef AUTO_VERIFY_PIN_WITH_CP
ModemData *ProtocolSimBuilderLegacy::BuildSimVerifyEncryptedPin(
        int nPinLen, const char *pszEncryptedPin, const char *pszAID) {
    sit_sim_verify_sim_encrypted_pin_req req;
    int length = sizeof(req);

    if (nPinLen == (-1) || pszEncryptedPin == NULL) {
        return NULL;
    }

    memset(&req, 0, length);
    InitRequestHeader(&req.hdr, SIT_OEM_VERIFY_SIM_PIN_ENC, length);
    req.pin_len = nPinLen;

    if (pszEncryptedPin != NULL) {
        req.encrypted_pin_len = (BYTE)HexString2Value(req.encrypted_pin, sizeof(req.encrypted_pin), pszEncryptedPin);
    }

    if (pszAID != NULL) {
        // convert HEX string to HEX value
        req.aid_len = (BYTE) HexString2Value(req.aid, sizeof(req.aid), pszAID);
    }
    return new ModemData((char *)&req, length);
}
#endif
