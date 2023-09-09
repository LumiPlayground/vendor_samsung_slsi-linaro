/*
 *  SIT RIL network Unit test
 *
 *  Copyright Samsung Electronics Co., LTD.
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 */

#include "testUtil.h"
#include "testutils.h"

int GetRandomSerialNumber() {
    return INTrand(false);
}

UINT8* allocRspModemMem(UINT16 size, UINT16 sitId, UINT sitToken) {
    UINT8 *rspData = (UINT8 *)malloc(size);
    if (rspData == nullptr) return nullptr;

    memset(rspData, 0, size);

    RCM_HEADER *header = (RCM_HEADER *) rspData;
    header->type = RCM_TYPE_RESPONSE;
    header->length = size;
    header->id = sitId;
    header->ext.rsp.token = sitToken;

    return rspData;
}

UINT8* allocErrorRspModemMem(UINT16 size, UINT16 sitId, UINT sitToken, BYTE error) {
    UINT8 *rspData = (UINT8 *)malloc(size);
    if (rspData == nullptr) return nullptr;

    memset(rspData, 0, size);

    RCM_HEADER *header = (RCM_HEADER *) rspData;
    header->type = RCM_TYPE_RESPONSE;
    header->length = size;
    header->id = sitId;
    header->ext.rsp.token = sitToken;
    header->ext.rsp.error = error;

    return rspData;
}

UINT8* allocIndModemMem(UINT16 size, UINT16 sitId) {
    UINT8 *rspData = (UINT8 *)malloc(size);
    if (rspData == nullptr) return nullptr;

    memset(rspData, 0, size);

    RCM_IND_HEADER *header = (RCM_IND_HEADER *) rspData;
    header->type = RCM_TYPE_INDICATION;
    header->length = size;
    header->id = sitId;

    return rspData;
}

UINT8* buildErrorRsp(INT16 sitId, UINT sitToken, UINT32 *dataLen, BYTE error) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(null_data_format);
    rspData = allocErrorRspModemMem(len, sitId, sitToken, error);
    *dataLen = len;

    return rspData;
}

UINT8* buildEmptyRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(null_data_format);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    return rspData;
}

UINT8* buildDummyRawRsp(INT16 sitId, UINT sitToken, UINT32 *datalen, BYTE requiredLen) {
    UINT8 *rspData = nullptr;
    const int sizeOfDummyRawData = 12;
    BYTE dummyRawData[sizeOfDummyRawData] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    int sizeOfPayload = sizeOfDummyRawData;

    if ((requiredLen > 0) && (requiredLen < sizeOfDummyRawData)) sizeOfPayload = requiredLen;

    UINT16 len = (UINT16)sizeof(null_data_format) + sizeOfPayload;

    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    memcpy(rspData+(UINT16)sizeof(null_data_format), dummyRawData, sizeOfPayload);

    return rspData;
}

UINT8* buildEmptyInd(INT16 sitId, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16)sizeof(null_ind_data_format);

    rspData = allocIndModemMem(len, sitId);
    *datalen = len;

    return rspData;
}

UINT8* buildDummyRawInd(INT16 sitId, UINT32 *datalen, BYTE requiredLen) {
    UINT8 *rspData = nullptr;
    const int sizeOfDummyRawData = 12;
    BYTE dummyRawData[sizeOfDummyRawData] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    int sizeOfPayload = sizeOfDummyRawData;

    if ((requiredLen > 0) && (requiredLen < sizeOfDummyRawData)) sizeOfPayload = requiredLen;

    UINT16 len = (UINT16)sizeof(null_ind_data_format) + sizeOfPayload;

    rspData = allocIndModemMem(len, sitId);
    *datalen = len;

    memcpy(rspData+(UINT16)sizeof(null_ind_data_format), dummyRawData, sizeOfPayload);

    return rspData;
}

UINT8* buildIndRadioReady(UINT16 *rspDataLen) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16)sizeof(sit_pwr_radio_ready_ind);

    rspData = allocIndModemMem(len, SIT_IND_RADIO_READY);
    *rspDataLen = len;

    return rspData;
}

UINT8* buildIndRadioStateChanged(UINT16 *rspDataLen, int radioState) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16)sizeof(sit_pwr_radio_state_changed_ind);

    rspData = allocIndModemMem(len, SIT_IND_RADIO_STATE_CHANGED);
    *rspDataLen = len;

    sit_pwr_radio_state_changed_ind *ind = (sit_pwr_radio_state_changed_ind*)rspData;
    ind->radio_state = radioState;

    return rspData;
}
