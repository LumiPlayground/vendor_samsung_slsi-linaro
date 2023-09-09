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
 * sap.h
 *
 *  Created on: 2019. 2. 21.
 */

#ifndef __SAP_H__
#define __SAP_H__

enum SapMsgType {
    UNKNOWN = 0,
    REQUEST = 1,
    RESPONSE = 2,
    UNSOL_RESPONSE = 3,
};

enum SapMsgId {
    UNKNOWN = 0,
    REQUEST = 1,
    RESPONSE = 2,
    UNSOL_RESPONSE = 3,
};

enum SapError {
    SAP_E_SUCCESS = 0,
    SAP_E_RADIO_NOT_AVAILABLE = 1,
    SAP_E_GENERIC_FAILURE = 2,
    SAP_E_REQUEST_NOT_SUPPORTED = 3,
    SAP_E_CANCELLED = 4,
    SAP_E_INVALID_PARAMETER = 5,
    SAP_E_UNUSED = 6,
};

enum {
    SAP_CONNECT_RESPONSE_SUCCESS = 0,
    SAP_CONNECT_RESPONSE_FAILURE = 1,
    SAP_CONNECT_RESPONSE_MSG_SIZE_TOO_LARGE = 2,
    SAP_CONNECT_RESPONSE_MSG_SIZE_TOO_SMALL = 3,
    SAP_CONNECT_RESPONSE_OK_CALL_ONGOING = 4,
};

enum  {
    SAP_DISCONNECT_TYPE_GRACEFUL = 0,
    SAP_DISCONNECT_TYPE_IMMEDIATE = 1,
};

enum Type {
    SAP_TYPE_APDU = 0,
    SAP_TYPE_APDU7816 = 1,
};

enum  {
    SAP_APDU_RESPONSE_SUCCESS = 0,
    SAP_APDU_RESPONSE_GENERIC_FAILURE = 1,
    SAP_APDU_RESPONSE_SIM_NOT_READY = 2,
    SAP_APDU_RESPONSE_SIM_ALREADY_POWERED_OFF = 3,
    SAP_APDU_RESPONSE_SIM_ABSENT = 4,
 };

enum {
    SAP_ATR_RESPONSE_SUCCESS = 0,
    SAP_ATR_RESPONSE_GENERIC_FAILURE = 1,
    SAP_ATR_RESPONSE_SIM_ALREADY_POWERED_OFF = 3,
    SAP_ATR_RESPONSE_SIM_ALREADY_POWERED_ON = 18,
    SAP_ATR_RESPONSE_SIM_ABSENT = 4,
    SAP_ATR_RESPONSE_SIM_DATA_NOT_AVAILABLE = 6,
};

enum {
    SAP_POWER_RESPONSE_SUCCESS = 0,
    SAP_POWER_RESPONSE_GENERIC_FAILURE = 2,
    SAP_POWER_RESPONSE_SIM_ABSENT = 11,
    SAP_POWER_RESPONSE_SIM_ALREADY_POWERED_OFF = 17,
    SAP_POWER_RESPONSE_SIM_ALREADY_POWERED_ON = 18,
};

enum {
    SAP_RESET_SIM_RESPONSE_SUCCESS = 0,
    SAP_RESET_SIM_RESPONSE_GENERIC_FAILURE = 2,
    SAP_RESET_SIM_RESPONSE_SIM_ABSENT = 11,
    SAP_RESET_SIM_RESPONSE_SIM_NOT_READY = 16,
    SAP_RESET_SIM_RESPONSE_SIM_ALREADY_POWERED_OFF = 17,
};

enum  {
    SAP_STATUS_UNKNOWN_ERROR = 0,
    SAP_STATUS_CARD_RESET = 1,
    SAP_STATUS_CARD_NOT_ACCESSIBLE = 2,
    SAP_STATUS_CARD_REMOVED = 3,
    SAP_STATUS_CARD_INSERTED = 4,
    SAP_STATUS_RECOVERED = 5,
};

enum {
    SAP_CARD_READER_STATUS_RESPONSE_SUCCESS = 0,
    SAP_CARD_READER_STATUS_RESPONSE_GENERIC_FAILURE = 2,
    SAP_CARD_READER_STATUS_RESPONSE_SIM_DATA_NOT_AVAILABLE = 6,
};

enum {
    SAP_PROTOCOL_T0 = 0,
    SAP_PROTOCOL_T1 = 1,
};

enum {
    SAP_SET_TRANSFER_PROTOCOL_RESPONSE_SUCCESS = 0,
    SAP_SET_TRANSFER_PROTOCOL_RESPONSE_GENERIC_FAILURE = 2,
    SAP_SET_TRANSFER_PROTOCOL_RESPONSE_SIM_ABSENT = 11,
    SAP_SET_TRANSFER_PROTOCOL_RESPONSE_SIM_NOT_READY = 16,
    SAP_SET_TRANSFER_PROTOCOL_RESPONSE_SIM_ALREADY_POWERED_OFF = 17,
};

#pragma pack(1)

typedef struct {
    int token;
    SapMsgType type;
    SapMsgId id;
    SapError error;
    char payload[0];

} SapMsgHeader;

/**
 * MsgId_RIL_SIM_SAP_CONNECT
 * RIL_SIM_SAP_CONNECT_REQ_fields
 */
typedef struct {
    SapMsgHeader hrd;
    int max_message_size;
} SapConnectReq;

typedef struct {
    SapMsgHeader hrd;
    int max_message_size;
} SapConnectRsp;

typedef struct {
    SapMsgHeader hrd;
} SapDisconnectReq;

typedef struct {
    SapMsgHeader hrd;
} SapDisconnectRsp;

typedef struct {
    SapMsgHeader hrd;
    int disconnectType;
} SapDisconnectInd;

typedef struct {
    SapMsgHeader hrd;
    int type;
    char command[]; // TODO set max size of array
} SapApduReq;

typedef struct {
    SapMsgHeader hrd;
    int type;
    int response;
    char apduResponse[]; // TODO set max size of array
} SapApduRsp;

typedef struct {
    SapMsgHeader hrd;
} SapTransferAtrReq;

typedef struct {
    SapMsgHeader hrd;
    int response;
    char atr[]; // TODO set max size of array
} SapTransferAtrRsp;

typedef struct {
    SapMsgHeader hrd;
    bool state;
} SapPowerReq;

typedef struct {
    SapMsgHeader hrd;
    int response;
} SapPowerRsp;

typedef struct {
    SapMsgHeader hrd;
} SapResetSimReq;

typedef struct {
    SapMsgHeader hrd;
    int response;
} SapResetSimRsp;

typedef struct {
    SapMsgHeader hrd;
    int statusChange;
} SapStatusInd;

typedef struct {
    SapMsgHeader hrd;
} SapTransferCardReaderStatusReq;

typedef struct {
    SapMsgHeader hrd;
    int response;
    int CardReaderStatus;
} SapTransferCardReaderStatusRsp;

typedef struct {
    SapMsgHeader hrd;
} SapErrorRsp;

typedef struct {
    SapMsgHeader hrd;
    int protocol;
} SapSetTransferProtocolReq;

typedef struct {
    SapMsgHeader hrd;
    int response;
} SapSetTransferProtocolRsp;

#pragma pack()

#endif /* __SAP_H__ */
