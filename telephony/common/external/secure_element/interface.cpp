/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#include <stdio.h>
#include <string.h>
#include <utils/Log.h>
#include <pthread.h>
#include <time.h>

#include "secureelement.h"
#include "rilclienthelper.h"
#include "sitril-client.h"
#include "telephony/ril.h"
#include "utils.h"

#undef LOG_TAG
#define LOG_TAG "UiccHal-Interface"

#define LogD(format, ...)    ALOGD("%s() " format, __FUNCTION__, ##__VA_ARGS__)
#define LogE(format, ...)    ALOGE("%s() " format, __FUNCTION__, ##__VA_ARGS__)
#define LogW(format, ...)    ALOGW("%s() " format, __FUNCTION__, ##__VA_ARGS__)
#define LogI(format, ...)    ALOGI("%s() " format, __FUNCTION__, ##__VA_ARGS__)
#define LogV(format, ...)    ALOGI("%s() " format, __FUNCTION__, ##__VA_ARGS__)

#define ENTER_FUNC()        { ALOGI("%s() [<-- ", __FUNCTION__); }
#define LEAVE_FUNC()        { ALOGI("%s() [--> ", __FUNCTION__); }

// #### Defintions ####
#define INVALID_SOCKET_ID       (-1)

// wait 30 secs
#define TIMEOUT                 (30)

#define MAX_AID_LEN             (16)
#define MAX_ATR_LEN             (33 * 2)

void *gSITRILC_Handle = nullptr;
RilClientHelper *g_RilClientHelper = nullptr;

pthread_mutex_t g_Mutex = PTHREAD_MUTEX_INITIALIZER;
int mutexCnt = 0;
bool m_bOpened = false;

#pragma pack(1)
// Interface to RIL request struct
typedef struct tagSitSimOpenChannelRequest
{
    unsigned char aid_len;
    unsigned char aid[MAX_AID_LEN];
    unsigned char p2;
} SitSimOpenChannelReq;

typedef struct tagSitSimOpenChannelResponse
{
    int session_id;
    unsigned char sw1;
    unsigned char sw2;
    short response_len;
    unsigned char response[MAX_COMMON_DATA];
} SitSimOpenChannelRsp;

typedef struct tagSitSimTransmitApduChannelRequest
{
    int session_id;
    int cla;
    int instruction;
    int p1;
    int p2;
    int p3;
    unsigned short data_len;
    unsigned char data[0];
} SitSimTransmitApduChannelReq;

typedef struct tagSitSimTransmitApduChannelResponse
{
    unsigned char sw1;
    unsigned char sw2;
    unsigned short apdu_len;
    unsigned char apdu[0];
} SitSimTransmitApduChannelRsp;

typedef struct tagSitAtrResponse {
    uint8_t atr[MAX_ATR_LEN + 1];
} SitAtrRsp;
#pragma pack()

enum {
    SE_IDX_OPEN_CHANNEL,
    SE_IDX_TRANSMIT_APDU_LOGICAL,
    SE_IDX_CLOSE_CHANNEL,
    SE_IDX_GET_ICC_ATR,
    SE_IDX_GET_CARD_PRESENT,
    SE_IDX_RESET,
    SE_IDX_MAX
};

typedef struct tagSecureElementResponse {
    int nMsgID;
    int bSuccess;
    int nRilErrno;
    void *pValue;
    pthread_cond_t cond;

    void Initialize(void)
    {
        bSuccess = false;
        nRilErrno = 0;
        cond = PTHREAD_COND_INITIALIZER;
        if (pValue != nullptr) {
            switch(nMsgID) {
            case RILC_REQ_SE_OPEN_CHANNEL: {
                    SitSimOpenChannelRsp *p = (SitSimOpenChannelRsp *) pValue;
                    delete p;
                }
                break;
            case RILC_REQ_SE_TRANSMIT_APDU_LOGICAL: {
                    unsigned char *p = (unsigned char *) pValue;
                    delete [] p;
                }
                break;
            case RILC_REQ_SE_GET_ICC_ATR: {
                    SitAtrRsp *p = (SitAtrRsp *) pValue;
                    delete p;
                }
                break;
            case RILC_REQ_SE_CLOSE_CHANNEL:
            case RILC_REQ_SE_GET_CARD_PRESENT: {
                    int *p = (int *) pValue;
                    delete p;
                }
                break;
            case RILC_REQ_SE_RESET:
                break;
            }
            pValue = nullptr;
        }
    }
} SecureElementResponse;

// #### Global Variables ####
static SecureElementResponse g_aSeResponse[SE_IDX_MAX] = {
    {RILC_REQ_SE_OPEN_CHANNEL, false, 0,  nullptr, PTHREAD_COND_INITIALIZER},
    {RILC_REQ_SE_TRANSMIT_APDU_LOGICAL, false, 0, nullptr, PTHREAD_COND_INITIALIZER},
    {RILC_REQ_SE_CLOSE_CHANNEL, false, 0, nullptr, PTHREAD_COND_INITIALIZER},
    {RILC_REQ_SE_GET_ICC_ATR, false, 0, nullptr, PTHREAD_COND_INITIALIZER},
    {RILC_REQ_SE_GET_CARD_PRESENT, false, 0, nullptr, PTHREAD_COND_INITIALIZER},
    {RILC_REQ_SE_RESET, false, 0, nullptr, PTHREAD_COND_INITIALIZER}
};

typedef void (*_unsolicited_handler)(int socketId, void* data);
static _unsolicited_handler g_unsolicited_handler[SIM_COUNT] = {nullptr, nullptr};

void *g_hLibrary = nullptr;
static bool s_bConnected[SIM_COUNT];

// #### Internal Functions ####
int GetMessageIndex(unsigned int nMsgId)
{
    switch(nMsgId) {
    case RILC_REQ_SE_OPEN_CHANNEL:          return SE_IDX_OPEN_CHANNEL;
    case RILC_REQ_SE_TRANSMIT_APDU_LOGICAL: return SE_IDX_TRANSMIT_APDU_LOGICAL;
    case RILC_REQ_SE_CLOSE_CHANNEL:         return SE_IDX_CLOSE_CHANNEL;
    case RILC_REQ_SE_GET_ICC_ATR:           return SE_IDX_GET_ICC_ATR;
    case RILC_REQ_SE_GET_CARD_PRESENT:      return SE_IDX_GET_CARD_PRESENT;
    case RILC_REQ_SE_RESET:                 return SE_IDX_RESET;
    }

    return -1;
}

static void SitRilClient_UnsolicitedResponse(unsigned int msgId, void* data,
        size_t /* length */, unsigned int channel)
{
    if (msgId == RILC_UNSOL_SE_SIM_STATE_CHANGED) {
        int cardState = *(int *)data;

        bool connected = (cardState == RIL_CARDSTATE_PRESENT);
        if (s_bConnected[channel] != connected) {
            if (g_unsolicited_handler[channel] != nullptr) {
                g_unsolicited_handler[channel](channel, &connected);
            }
            s_bConnected[channel] = connected;
        }
    }
}

static void SitRilClient_OnResponse(unsigned int msgId, int status, void* data,
        size_t length, unsigned int /* channel */)
{
    void *pResponse = nullptr;
    unsigned int nRspLength = 0;
    int nIndex = GetMessageIndex(msgId);

    if (0 <= nIndex) {
        switch(msgId) {
        case RILC_REQ_SE_OPEN_CHANNEL:
            pResponse = (void *) new SitSimOpenChannelRsp();
            nRspLength = sizeof(SitSimOpenChannelRsp);
            if (pResponse != nullptr) {
                g_aSeResponse[nIndex].bSuccess = true;
            }
            break;
        case RILC_REQ_SE_TRANSMIT_APDU_LOGICAL:
            nRspLength = sizeof(SitSimTransmitApduChannelRsp) + length;
            pResponse = (void *) new unsigned char[nRspLength];
            if (pResponse != nullptr) {
                g_aSeResponse[nIndex].bSuccess = true;
            }
            break;
        case RILC_REQ_SE_CLOSE_CHANNEL:
            if (status == RILC_STATUS_SUCCESS) {
                g_aSeResponse[nIndex].bSuccess = true;
            }
            break;
        case RILC_REQ_SE_GET_ICC_ATR:
            pResponse = (void *) new SitAtrRsp();
            nRspLength = length;
            if (pResponse != nullptr && status == RILC_STATUS_SUCCESS) {
                g_aSeResponse[nIndex].bSuccess = true;
            }
            break;
        case RILC_REQ_SE_GET_CARD_PRESENT:
            pResponse = (void *) new int;
            nRspLength = sizeof(int);
            if (pResponse != nullptr && status == RILC_STATUS_SUCCESS) {
                g_aSeResponse[nIndex].bSuccess = true;
            }
            break;
        case RILC_REQ_SE_RESET:
            if (status == RILC_STATUS_SUCCESS) g_aSeResponse[nIndex].bSuccess = true;
            break;
        default:
            LogE("Error:  Unkonwn msgId(%d)", msgId);
            LEAVE_FUNC();
            return;
        }

        if (pResponse != nullptr) {
            memset(pResponse, 0, nRspLength);
            // Buffer size is nRspLength
            if (length > nRspLength) {
                length = nRspLength;
            }
            memcpy(pResponse, data, length);

            g_aSeResponse[nIndex].pValue = (void *) pResponse;
            memcpy(g_aSeResponse[nIndex].pValue, data, length);
            g_aSeResponse[nIndex].nRilErrno = status;
        }
        pthread_cond_signal(&g_aSeResponse[nIndex].cond);
    }
}

// #### Initialization Functions ####

void SetIndicationFilter()
{
    int filters[] = {
        RILC_UNSOL_SE_SIM_STATE_CHANGED
    };

    size_t size = sizeof(filters) / sizeof(filters[0]);
    if (g_RilClientHelper != nullptr) {
        g_RilClientHelper->SetIndicationFilter(gSITRILC_Handle, filters, size);
    }
}

int rilOpen(void)
{
    ENTER_FUNC();
    int ret = SITRIL_SE_ERROR_NONE;

    do {
        g_RilClientHelper = RilClientHelper::GetInstance();
        if (g_RilClientHelper == nullptr) {
            ret = SITRIL_SE_ERROR_LIB_LOAD_FAIL;
            break;
        }

        // Open
        gSITRILC_Handle = g_RilClientHelper->Open("SeApi");
        if (gSITRILC_Handle == nullptr) {
            LogE("%s() Fail in gSITRILC_Open()", __FUNCTION__);
            ret = SITRIL_SE_ERROR_NO_DEVICE;
            break;
        }
        LogV("%s() g_RilClientHelper->Open() ... OK.",__FUNCTION__);

        // Register indications to be received
        SetIndicationFilter();

        // Register Callback
        ret = g_RilClientHelper->RegisterUnsolicitedResponseHandler(gSITRILC_Handle,
                SitRilClient_UnsolicitedResponse);
        if (ret) {
            LogE("%s() Fail in RegisterUnsolicitedResponseHandler()",__FUNCTION__);
            ret = SITRIL_SE_ERROR_REGISTRATION_FAIL;
            break;
        }

        LogD("%s() rilOpen() ... OK.",__FUNCTION__);
    } while (0);

    if (ret != SITRIL_SE_ERROR_NONE) {
        rilClose();
    }

    for (int i = 0; i < SIM_COUNT; i++) {
        s_bConnected[i] = false;
    }

    LEAVE_FUNC();
    return ret;
}

int rilClose(void)
{
    ENTER_FUNC();

    if (g_RilClientHelper != nullptr) {
        g_RilClientHelper->Close(gSITRILC_Handle);
        LogI("%s() gSITRILC_Close() ... OK.", __FUNCTION__);
    }

    for (int i = 0; i < SIM_COUNT; i++) {
        s_bConnected[i] = false;
    }

    LEAVE_FUNC();
    return SITRIL_SE_ERROR_NONE;
}

int SendRequest(int socketId, unsigned int msgId, void *data, int data_len)
{
    ENTER_FUNC();
    int ret = SITRIL_SE_ERROR_NONE;
    struct timeval now {};
    struct timespec timeout {};
    gettimeofday(&now, nullptr);
    timeout.tv_sec = now.tv_sec + TIMEOUT;

    do {
        // Check loaded library
        if (gSITRILC_Handle == nullptr) {
            LogE("Error: gSITRILC_Handle = nullptr");
            ret = SITRIL_SE_ERROR_NOT_OPENED_LIB;
            break;
        }

        // Sending A message
        LogV("socket id : %d", socketId);
        int nIndex = GetMessageIndex(msgId);
        if (nIndex < 0) {
            return SITRIL_SE_ERROR_INVALID_PARAM;
        }
        pthread_mutex_lock(&g_Mutex);
        ret = g_RilClientHelper->Send(gSITRILC_Handle, msgId, (data), data_len,
                SitRilClient_OnResponse, socketId);
        if (ret) {
            LogE("Error:  Fail in g_RilClientHelper->Send()");
            ret = SITRIL_SE_ERROR_SEND_FAIL;
            break;
        }
        int retMutex = pthread_cond_timedwait(&g_aSeResponse[nIndex].cond, &g_Mutex, &timeout);
        if (retMutex == ETIMEDOUT) {
            LogE("Response TimeOUT!!!!!");
            ret = SITRIL_SE_ERROR_TIMEOUT;
        }
    } while (0);

    pthread_mutex_unlock(&g_Mutex);
    LEAVE_FUNC();
    return ret;
}

int OpenLogicalChannel(int socketId, OpenChannelResponse *pResponse, LengthData *pAid,
        int p2)
{
    ENTER_FUNC();
    int ret = SITRIL_SE_ERROR_NONE;

    do {
        // Check Socket ID
        if (socketId == INVALID_SOCKET_ID) {
            LogE("Error:  Invalid Socket ID(%d)", socketId);
            ret = SITRIL_SE_ERROR_INVALID_SOCKET_ID;
            break;
        }

        if (pResponse == nullptr || pAid == nullptr) {
            // Check parameters
            LogE("Error:  pResponse(0x%p), pAid(0x%p)", pResponse, pAid);
            ret = SITRIL_SE_ERROR_INVALID_PARAM;
            break;
        }

        // Set Sending Data
        SitSimOpenChannelReq tReq;
        memset(&tReq, 0, sizeof(SitSimOpenChannelReq));
        tReq.aid_len = pAid->length;
        memcpy(&tReq.aid, pAid->data, pAid->length);
        tReq.p2 = p2;
        ret = SendRequest(socketId, RILC_REQ_SE_OPEN_CHANNEL, &tReq, sizeof(SitSimOpenChannelReq));
        if (ret == SITRIL_SE_ERROR_NONE) {
            memset(pResponse, 0, sizeof(OpenChannelResponse));
            SecureElementResponse *pSeRsp =
                    (SecureElementResponse *) &g_aSeResponse[SE_IDX_OPEN_CHANNEL];
            SitSimOpenChannelRsp *pValue = (SitSimOpenChannelRsp *) pSeRsp->pValue;
            if (pValue != nullptr) {
                //memcpy(pResponse, pValue, sizeof(SecureElementResponse));
                pResponse->rilErrno = pSeRsp->nRilErrno;
                pResponse->sessionId = pValue->session_id;
                pResponse->sw1 = (int) pValue->sw1;
                pResponse->sw2 = (int) pValue->sw2;
                pResponse->nResponseCount = (int) pValue->response_len;
                memcpy(pResponse->aResponse, pValue->response, pValue->response_len);
                pResponse->aResponse[pResponse->nResponseCount++] = (uint8_t)pResponse->sw1;
                pResponse->aResponse[pResponse->nResponseCount++] = (uint8_t)pResponse->sw2;
            }
            else ret = SITRIL_SE_ERROR_FAILURE;
            pSeRsp->Initialize();
        }
    } while (0);

    LEAVE_FUNC();
    return ret;
}

int TransmitApduLogicalChannel(int socketId, TransmitApduChannelResponse *pResponse,
        int channel, int cla, int instruction, int p1, int p2, int p3, LengthData *pData)
{
    ENTER_FUNC();
    int ret = SITRIL_SE_ERROR_NONE;

    do {
        // Check Socket ID
        if (socketId == INVALID_SOCKET_ID) {
            LogE("Error:  Invalid Socket ID(%d)", socketId);
            ret = SITRIL_SE_ERROR_INVALID_SOCKET_ID;
            break;
        }

        if (pResponse == nullptr) {
            // Check parameters
            LogE("Error:  pResponse(0x%p)", pResponse);
            ret = SITRIL_SE_ERROR_INVALID_PARAM;
            break;
        }

        // Set Sending Data
        int nDataLen = (pData == nullptr) ? 0 : pData->length;
        int nReqLength = sizeof(SitSimTransmitApduChannelReq) + nDataLen;
        unsigned char *pBuffer = new unsigned char[nReqLength];
        SitSimTransmitApduChannelReq *pReq = (SitSimTransmitApduChannelReq *) pBuffer;
        memset(pReq, 0, nReqLength);
        pReq->session_id = channel;
        pReq->cla = cla;
        pReq->instruction = instruction;
        pReq->p1 = p1;
        pReq->p2 = p2;
        pReq->p3 = p3;

        if (0 < nDataLen) {
            memcpy(pReq->data, pData->data, pData->length);
            pReq->data_len = pData->length;
        }
        ret = SendRequest(socketId, RILC_REQ_SE_TRANSMIT_APDU_LOGICAL, pReq, nReqLength);
        if (ret == SITRIL_SE_ERROR_NONE) {
            memset(pResponse, 0, sizeof(TransmitApduChannelResponse));
            SecureElementResponse *pSeRsp =
                    (SecureElementResponse *) &g_aSeResponse[SE_IDX_TRANSMIT_APDU_LOGICAL];
            SitSimTransmitApduChannelRsp *pValue = (SitSimTransmitApduChannelRsp *) pSeRsp->pValue;
            if (pValue != nullptr) {
                pResponse->rilErrno = pSeRsp->nRilErrno;
                pResponse->sw1 = (int) pValue->sw1;
                pResponse->sw2 = (int) pValue->sw2;
                pResponse->nResponseCount = pValue->apdu_len;
                memcpy(pResponse->aResponse, pValue->apdu, pValue->apdu_len);
                pResponse->aResponse[pResponse->nResponseCount++] = pValue->sw1;
                pResponse->aResponse[pResponse->nResponseCount++] = pValue->sw2;
            }
            else ret = SITRIL_SE_ERROR_FAILURE;
            pSeRsp->Initialize();
        }
        delete[] pBuffer;

    } while (0);

    LEAVE_FUNC();
    return ret;
}

int CloseLogicalChannel(int socketId, int channel)
{
    ENTER_FUNC();
    int ret = SITRIL_SE_ERROR_NONE;

    do {
        // Check Socket ID
        if (socketId == INVALID_SOCKET_ID) {
            LogE("Error:  Invalid Socket ID(%d)", socketId);
            ret = SITRIL_SE_ERROR_INVALID_SOCKET_ID;
            break;
        }

        ret = SendRequest(socketId, RILC_REQ_SE_CLOSE_CHANNEL, &channel, sizeof(int));
        if (ret == SITRIL_SE_ERROR_NONE) {
            SecureElementResponse *pSeRsp =
                    (SecureElementResponse *) &g_aSeResponse[SE_IDX_CLOSE_CHANNEL];
            pSeRsp->Initialize();
        }
    } while (0);

    LEAVE_FUNC();
    return ret;
}

int GetAtr(int socketId, LengthData *pAtr)
{
    ENTER_FUNC();
    int ret = SITRIL_SE_ERROR_NONE;

    do {
        // Check Socket ID
        if (socketId == INVALID_SOCKET_ID) {
            LogE("Error:  Invalid Socket ID(%d)", socketId);
            ret = SITRIL_SE_ERROR_INVALID_SOCKET_ID;
            break;
        }

        if (pAtr == nullptr) {
            // Check parameters
            LogE("Error:  pAtr(0x%p)", pAtr);
            ret = SITRIL_SE_ERROR_INVALID_PARAM;
            break;
        }

        // Set Sending Data
        ret = SendRequest(socketId, RILC_REQ_SE_GET_ICC_ATR, nullptr, 0);
        if (ret == SITRIL_SE_ERROR_NONE) {
            SecureElementResponse *pSeRsp
                    = (SecureElementResponse *) &g_aSeResponse[SE_IDX_GET_ICC_ATR];
            SitAtrRsp *pValue = (SitAtrRsp *) pSeRsp->pValue;
            if (pValue != nullptr) {
                pAtr->length =
                        HexString2Value((unsigned char*)pAtr->data, (const char *)pValue->atr);
            }
            else ret = SITRIL_SE_ERROR_FAILURE;
            pSeRsp->Initialize();
        }
    } while (0);

    LEAVE_FUNC();
    return ret;
}

int IsCardPresent(int socketId, bool *pConnected)
{
    ENTER_FUNC();
    int ret = SITRIL_SE_ERROR_NONE;

    do {
        // Check Socket ID
        if (socketId == INVALID_SOCKET_ID) {
            LogE("Error:  Invalid Socket ID(%d)", socketId);
            ret = SITRIL_SE_ERROR_INVALID_SOCKET_ID;
            break;
        }

        if (pConnected == nullptr) {
            LogE("Error:  SITRIL_SE_ERROR_INVALID_PARAM");
            ret = SITRIL_SE_ERROR_INVALID_PARAM;
            break;
        }

        ret = SendRequest(socketId, RILC_REQ_SE_GET_CARD_PRESENT, nullptr, 0);
        if (ret == SITRIL_SE_ERROR_NONE) {
            SecureElementResponse *pSeRsp =
                    (SecureElementResponse *) &g_aSeResponse[SE_IDX_GET_CARD_PRESENT];
            int *pValue = (int *) pSeRsp->pValue;
            if (pValue != nullptr) {
                s_bConnected[socketId] = (*pValue == RIL_CARDSTATE_PRESENT);
                *pConnected = s_bConnected[socketId];
            }
            else ret = SITRIL_SE_ERROR_FAILURE;
            pSeRsp->Initialize();
        }
    } while (0);

    LEAVE_FUNC();
    return ret;
}

int Reset(int socketId)
{
    ENTER_FUNC();
    int ret = SITRIL_SE_ERROR_NONE;

    do {
        // Check Socket ID
        if (socketId == INVALID_SOCKET_ID) {
            LogE("Error:  Invalid Socket ID(%d)", socketId);
            ret = SITRIL_SE_ERROR_INVALID_SOCKET_ID;
            break;
        }
        ret = SendRequest(socketId, RILC_REQ_SE_RESET, nullptr, 0);
        if (ret == SITRIL_SE_ERROR_NONE) {
            SecureElementResponse *pSeRsp = (SecureElementResponse *) &g_aSeResponse[SE_IDX_RESET];
            pSeRsp->Initialize();
        } else ret = SITRIL_SE_ERROR_FAILURE;
    } while (0);

    LEAVE_FUNC();
    return ret;
}

int SetUnsolicitedHandler(int socketId, void* func) {
    ENTER_FUNC();
    int ret = SITRIL_SE_ERROR_NONE;

    do {
        if (func == nullptr) {
            LogE("Error:  SITRIL_SE_ERROR_INVALID_PARAM");
            ret = SITRIL_SE_ERROR_INVALID_PARAM;
            break;
        }
        g_unsolicited_handler[socketId] = (_unsolicited_handler)func;
    } while (0);

    LEAVE_FUNC();
    return ret;
}

