/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef __INTERFACE_H__
#define __INTERFACE_H__

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

// Definitions

#pragma pack(push, 1)
// SecureElement Hal <-> Interface
typedef struct tagLengthData {
    int32_t length;                   /*!< length of the buffer */
    uint8_t data[MAX_COMMON_DATA];              /*!< pointer to a buffer */
} LengthData;

typedef struct tagTransmitLengthData {
    int32_t length;                   /*!< length of the buffer */
    uint8_t data[MAX_APDU_SIZE];   /*!< pointer to a buffer */
} TransmitLengthData;

typedef struct tagOpenChannelResponse {
    int32_t rilErrno;
    int32_t sessionId;
    int32_t sw1;
    int32_t sw2;
    uint16_t nResponseCount;
    uint8_t aResponse[MAX_COMMON_DATA];
} OpenChannelResponse;

typedef struct tagTransmitApduChannelResponse {
    int32_t rilErrno;
    int32_t sw1;
    int32_t sw2;
    uint16_t nResponseCount;
    uint8_t aResponse[MAX_APDU_SIZE];
} TransmitApduChannelResponse;
#pragma pack(pop)

// #### Initialization Functions ####

// ----------------------------------------------------
// API Name : rilOpen
// Description
//     : Open Secure Element Interaface of SIT RIL.
// Params
//    - void
// Results
//    - int error value of below SitRilSeError
//          SITRIL_SE_ERROR_NONE,
//          SITRIL_SE_ERROR_FAILURE,
//          SITRIL_SE_ERROR_NOT_OPENED_LIB,
//          SITRIL_SE_ERROR_LIB_LOAD_FAIL,
//          SITRIL_SE_ERROR_NO_SYMBOL,
//          SITRIL_SE_ERROR_OPEN_FAIL,
// ----------------------------------------------------
int rilOpen(void);

// ----------------------------------------------------
// API Name : rilClose
// Description
//     : Close Secure Element Interaface of SIT RIL.
// Params
//    - void
// ----------------------------------------------------
int rilClose(void);

// ----------------------------------------------------
// API Name : OpenLogicalChannel
// Description
//     : Open Logical Channel to transmit APDU
// Params
//    - (int) int socketId
//    - (out) SitRilSeOpenChannelResponse &pResponse
//    - (in) char[] aid
//    - (in) int p2
// Results
//    - int error value of below SitRilSeError
//          SITRIL_SE_ERROR_NONE,
//          SITRIL_SE_ERROR_FAILURE,
//          SITRIL_SE_ERROR_NOT_OPENED_LIB,
//          SITRIL_SE_ERROR_LIB_LOAD_FAIL,
//          SITRIL_SE_ERROR_NO_SYMBOL,
//          SITRIL_SE_ERROR_OPEN_FAIL,
//          SITRIL_SE_ERROR_INVALID_SOCKET_ID,
//          SITRIL_SE_ERROR_INVALID_PARAM,
//          SITRIL_SE_ERROR_REGISTRATION_FAIL,
//          SITRIL_SE_ERROR_TIMEOUT,
// ----------------------------------------------------
int OpenLogicalChannel(int socketId, OpenChannelResponse *pResponse, LengthData *pAid,
        int p2);

// ----------------------------------------------------
// API Name : TransmitApduLogicalChannel
// Description
//     : Transmit APDU on Logical Channel
// Params
//    - (int) int socketId
//    - (out) LengthData &apdu
//    - (in) int channel
//    - (in) int cla
//    - (in) int instruction
//    - (in) int p1
//    - (in) int p2
//    - (in) int p3
//    - (in) LengthData data
// Results
//    - int error value of below SitRilSeError
//          SITRIL_SE_ERROR_NONE,
//          SITRIL_SE_ERROR_FAILURE,
//          SITRIL_SE_ERROR_NOT_OPENED_LIB,
//          SITRIL_SE_ERROR_INVALID_SOCKET_ID,
//          SITRIL_SE_ERROR_INVALID_PARAM,
//          SITRIL_SE_ERROR_TIMEOUT,
// ----------------------------------------------------
int TransmitApduLogicalChannel(int socketId, TransmitApduChannelResponse *pResponse,
        int channel, int cla, int instruction, int p1, int p2, int p3, LengthData *pData);

// ----------------------------------------------------
// API Name : CloseLogicalChannel
// Description
//     : Close APDU on Basic Channel
// Params
//    - (int) int socketId
//    - (in) int channel
// Results
//    - int error value of below SitRilSeError
//          SITRIL_SE_ERROR_NONE,
//          SITRIL_SE_ERROR_FAILURE,
//          SITRIL_SE_ERROR_NOT_OPENED_LIB,
//          SITRIL_SE_ERROR_INVALID_SOCKET_ID,
//          SITRIL_SE_ERROR_INVALID_PARAM,
//          SITRIL_SE_ERROR_TIMEOUT,
// ----------------------------------------------------
int CloseLogicalChannel(int socketId, int channel);

// ----------------------------------------------------
// API Name : GetAtr
// Description
//     : Get ATR
// Params
//    - (in) int socketId
//    - (out ) LengthData &atr
// Results
//    - int error value of below SitRilSeError
//          SITRIL_SE_ERROR_NONE,
//          SITRIL_SE_ERROR_FAILURE,
//          SITRIL_SE_ERROR_NOT_OPENED_LIB,
//          SITRIL_SE_ERROR_INVALID_SOCKET_ID,
//          SITRIL_SE_ERROR_INVALID_PARAM,
//          SITRIL_SE_ERROR_TIMEOUT,
// ----------------------------------------------------
int GetAtr(int socketId, LengthData *pAtr);

// ----------------------------------------------------
// API Name : GetCardPresent
// Description
//     : Get Card Status
// Params
//    - (in) int socketId
//    - (out) bool *pConnected
// Results
//    - int error value of below SitRilSeError
//          SITRIL_SE_ERROR_NONE,
//          SITRIL_SE_ERROR_FAILURE,
//          SITRIL_SE_ERROR_INVALID_SOCKET_ID,
// ----------------------------------------------------
int IsCardPresent(int socketId, bool *pConnected);

// ----------------------------------------------------
// API Name : Reset
// Description
//     : Reset to SE
// Params
//    - (in) int socketId
// Results
//    - int error value of below SitRilSeError
//          SITRIL_SE_ERROR_NONE,
//          SITRIL_SE_ERROR_FAILURE,
//          SITRIL_SE_ERROR_INVALID_SOCKET_ID,
// ----------------------------------------------------
int Reset(int socketId);

// ----------------------------------------------------
// API Name : SetUnsolicitedHandler
// Description
//     : Set Unsolicited Handler
// Params
//    - (in) int socketId
// 	  - (in) void* func
// Results
//    - int error value of below SitRilSeError
//          SITRIL_SE_ERROR_NONE,
//          SITRIL_SE_ERROR_FAILURE,
//          SITRIL_SE_ERROR_INVALID_SOCKET_ID,
// ----------------------------------------------------
int SetUnsolicitedHandler(int socketId, void* func);

#ifdef __cplusplus
};
#endif

#endif // __INTERFACE_H__
