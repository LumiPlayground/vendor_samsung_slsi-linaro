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
#ifndef __TEST_UTIL_H__
#define __TEST_UTIL_H__
#include <sitdef.h>
#include <rildef.h>
#include <stdlib.h>

/*
 * Generate random serial number for radio test
 */
int GetRandomSerialNumber();

/*
 * allocate rsp modem data.
 */
UINT8* allocRspModemMem(UINT16 size, UINT16 sitId, UINT sitToken);
UINT8* allocErrorRspModemMem(UINT16 size, UINT16 sitId, UINT sitToken, BYTE error);
UINT8* allocIndModemMem(UINT16 size, UINT16 sitId);
UINT8* buildErrorRsp(INT16 sitId, UINT sitToken, UINT32 *dataLen, BYTE error);
UINT8* buildEmptyRsp(INT16 sitId, UINT sitToken, UINT32 *datalen);
UINT8* buildDummyRawRsp(INT16 sitId, UINT sitToken, UINT32 *datalen, BYTE requiredLen = 0);
UINT8* buildEmptyInd(INT16 sitId, UINT32 *datalen);
UINT8* buildDummyRawInd(INT16 sitId, UINT32 *datalen, BYTE requiredLen = 0);
UINT8* buildIndRadioReady(UINT16 *rspDataLen);
UINT8* buildIndRadioStateChanged(UINT16 *rspDataLen, int radioState);
#endif
