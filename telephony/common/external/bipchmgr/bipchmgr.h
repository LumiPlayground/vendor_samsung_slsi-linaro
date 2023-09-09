/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef __BIPCHMGR_H__
#define __BIPCHMGR_H__

#include <queue>

#ifdef __cplusplus
extern "C" {
#endif

// Definitions
#define MAX_DATA_SIZE       (1500)
#define SIT_HEADER_SIZE     (12)
#define MAX_PACKET_SIZE     (MAX_DATA_SIZE + SIT_HEADER_SIZE)
#define MAX_MODEM_COUNT     (2)
#define MAX_SESSION_COUNT   (7)

// Timeout
#define CONNECTION_TIMEOUT  25
#define SEND_TIMEOUT        25
#define RECEVE_TIMEOUT      25

// BIP Channel Manager Errors
enum {
    BIPCHMGR_ERROR_NONE,
    BIPCHMGR_ERROR_FAILURE,
    BIPCHMGR_ERROR_NOT_OPENED_LIB,
    BIPCHMGR_ERROR_LIB_LOAD_FAIL,
    BIPCHMGR_ERROR_NO_SYMBOL,
    BIPCHMGR_ERROR_OPEN_FAIL,
    BIPCHMGR_ERROR_SEND_FAIL,
    BIPCHMGR_ERROR_INVALID_CHANNEL,
    BIPCHMGR_ERROR_INVALID_PARAM,
    BIPCHMGR_ERROR_REGISTRATION_FAIL,
    BIPCHMGR_ERROR_ALREADY_LIB_LOADED,
    BIPCHMGR_ERROR_ALREADY_REGISTERD,
    BIPCHMGR_ERROR_TIMEOUT,
    BIPCHMGR_ERROR_NOT_SUPPORTED,
    BIPCHMGR_ERROR_SIM_ABSENT,
    BIPCHMGR_ERROR_MAX
} BipChMgrError;

enum {
    MEDIA_UNKNOWN,
    MEDIA_CONNECTED,
    MEDIA_DISCONNECTED
};

enum {
    RESULT_NO_ERROR,
    RESULT_FAILURE
};

enum {
    CHANNEL_EVENT_UNKNOWN,
    CHANNEL_EVENT_RECEIVED,
    CHANNEL_EVENT_CONNECTED,
    CHANNEL_EVENT_CLOSED
};

enum {
    TRANSPORT_UNKNOWN,
    TRANSPORT_TCP,
    TRANSPORT_UDP
};

enum {
    TYPE_IPV4 = 0x01,
    TYPE_IPV6 = 0x02,
    TYPE_IPV4V6 = 0x03,
    TYPE_PDP_PPP = 0x04,
    TYPE_PDP_NON_IP = 0x05
};

// Confrim from CP for receiving response(RESPONSE)
typedef struct {
    unsigned char error;
} BipManagerConfirm;

typedef struct {
    unsigned char session_id;
} BipSessionId;

typedef struct {
    unsigned char session_id;
    unsigned short length;
    unsigned char data[MAX_DATA_SIZE];
} BipDataInfo;

// RILC_REQ_BIPCHMGR_STATUS_RESPONSE
typedef struct {
    unsigned char session_id;
    unsigned char status;
} BipStatusResponse;

// RILC_REQ_BIPCHMGR_CHANNEL_STATUS_REPORT
typedef BipStatusResponse BipChannelStatusReport;

// RILC_UNSOL_BIPCHMGR_OPEN_CHANNEL_REQUEST
typedef struct {
    unsigned char transport_type;
    unsigned int port;
    unsigned char ip_type;
    unsigned short buffer_size;
    unsigned char ipv4[4];
    unsigned char ipv6[16];
} BipOpenChannelRequest;
// RILC_REQ_BIPCHMGR_OPEN_CHANNEL_RESPONSE
typedef BipStatusResponse BipOpenChannelResponse;
typedef BipSessionId BipOpenChannelOnResponse;


// RILC_UNSOL_BIPCHMGR_SEND_DATA_REQUEST
typedef BipDataInfo BipSendDataRequest;
// RILC_REQ_BIPCHMGR_SEND_DATA_RESPONSE
typedef BipStatusResponse BipSendDataResponse;

// RILC_UNSOL_BIPCHMGR_RECEIVE_DATA_REQUEST
typedef BipSessionId BipReceiveDataRequest;
// RILC_REQ_BIPCHMGR_RECEIVE_DATA_RESPONSE
typedef BipDataInfo BipReceiveDataResponse;
typedef BipSessionId BipReceiveDataOnResponse;


// RILC_UNSOL_BIPCHMGR_CLOSE_CHANNEL_REQUEST
typedef BipSessionId BipCloseChannelRequest;
// RILC_REQ_BIPCHMGR_CLOSE_CHANNEL_RESPONSE
typedef BipStatusResponse BipCloseChannelResponse;
#pragma pack()

typedef struct tagReceiveData {
    unsigned char ucChannelEvent;
    unsigned int nDataSize;
    unsigned char aBuffer[MAX_DATA_SIZE];
} tReceiveData;

typedef struct tagBipContext {
    bool bUsed;
    int nSessionId;
    int sock;
    BipOpenChannelRequest tOpenChannelReq;
    bool tcp_connected;
    //unsigned int nRequestId;
    bool receive_thread;
    pthread_t threadId;

    // STD Queue
    std::queue<tReceiveData> tDataQueue;

    tagBipContext() {
        init();
    };

    ~tagBipContext() {
        init();
    };

    void init(int session=0) {
        bUsed = 0;
        nSessionId = session;
        sock = -1;
        memset(&tOpenChannelReq, 0x00, sizeof(BipOpenChannelRequest));
        tcp_connected = 0;
        receive_thread = 0;
        threadId = 0;
        clearQueue(tDataQueue);
    }

    void clearQueue(std::queue<tReceiveData> &q) {
       std::queue<tReceiveData> empty;
       std::swap(q, empty);
    };
} BipContext;

typedef struct tagSlotContext {
    BipContext aBipContexts[MAX_SESSION_COUNT];

    tagSlotContext () {
        init();
    };

    ~tagSlotContext () {
        init();
    };

    void init() {
        for(int i = 0; i < MAX_SESSION_COUNT; i++) {
            aBipContexts[i].init(i);
        }
    }
} SlotContext;

// #### Initialization Functions ####

// ----------------------------------------------------
// API Name : rilOpen
// Description
//   : Open SIT RIL Client.
// Params
//    - void
// Results
//  - int error value of below SitRilSeError
//      BIPCHMGR_ERROR_NONE,
//      BIPCHMGR_ERROR_FAILURE,
//      BIPCHMGR_ERROR_NOT_OPENED_LIB,
//      BIPCHMGR_ERROR_LIB_LOAD_FAIL,
//      BIPCHMGR_ERROR_NO_SYMBOL,
//      BIPCHMGR_ERROR_OPEN_FAIL,
//      BIPCHMGR_ERROR_SEND_FAIL,
//      BIPCHMGR_ERROR_INVALID_CHANNEL,
//      BIPCHMGR_ERROR_INVALID_PARAM,
//      BIPCHMGR_ERROR_REGISTRATION_FAIL,
//      BIPCHMGR_ERROR_ALREADY_LIB_LOADED,
//      BIPCHMGR_ERROR_ALREADY_REGISTERD,
// ----------------------------------------------------
int rilOpen(void);

// Media Status
int ProcessBipMediaStatus(unsigned int slotId);
int ResponseBipMediaStatus(unsigned int slotId, unsigned char result);
void OnResponseBipMediaStatus(unsigned int slotId, unsigned char result);

// Open Channel
int ProcessBipOpenChannel(unsigned int slotId, void* data, size_t length);
int ResponseBipOpenChannel(unsigned int slotId, BipContext *pContext,
        unsigned char result);
void OnResponseBipOpenChannel(unsigned int slotId, void* data, unsigned char result);

// Channel Status Report
int ReportBipChannelStatus(unsigned int slotId, BipContext *pContext,
        unsigned char result);
void OnReportBipChannelStatus(unsigned int /* slotId */, unsigned char /* result */);

// Send Data
int ProcessBipSendData(unsigned int slotId, void* data, size_t length);
int ResponseBipSendData(unsigned int slotId, BipContext *pContext, unsigned char result);
void OnResponseBipSendData(unsigned int /* slotId */, unsigned char /* result */);

// Receive Data
int ProcessBipReceiveData(unsigned int slotId, void* data, size_t length);
int ResponseBipReceiveData(unsigned int slotId, BipContext *pContext);
void OnResponseBipReceiveData(unsigned int slotId, void* data, unsigned char result);

// Close Data
int ProcessBipCloseChannel(unsigned int slotId, void* data, size_t length);
int ResponseBipCloseChannel(unsigned int slotId, BipContext *pContext,
        unsigned char result);
void OnResponseBipCloseChannel(unsigned int /* slotId */, unsigned char /* result */);

int CreateReceiveThread(BipContext *pContext);
void *RecevingThreadProc(void *arg);

BipContext* getEmptyBipContext(int slotId);
BipContext* getBipContext(int slotId, int sessionId);
void ClearBipContext(BipContext *pContext);

// ----------------------------------------------------
// API Name : rilClose
// Description
//   : Close SIT RIL Client.
// Params
//  - void
// ----------------------------------------------------
void rilClose(void);

#ifdef __cplusplus
};
#endif

#endif // #ifndef __BIPCHMGR_H__
