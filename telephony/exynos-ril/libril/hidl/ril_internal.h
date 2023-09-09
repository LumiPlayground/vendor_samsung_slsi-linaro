/*
 * Copyright (c) 2016 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ANDROID_RIL_INTERNAL_H
#define ANDROID_RIL_INTERNAL_H

#include <functional>
#include <telephony/ril.h>

namespace android {

#define RIL_SERVICE_NAME_BASE "slot"
#define RIL1_SERVICE_NAME "slot1"
#define RIL2_SERVICE_NAME "slot2"
#define RIL3_SERVICE_NAME "slot3"
#define RIL4_SERVICE_NAME "slot4"

/* Constants for response types */
#define RESPONSE_SOLICITED 0
#define RESPONSE_UNSOLICITED 1
#define RESPONSE_SOLICITED_ACK 2
#define RESPONSE_SOLICITED_ACK_EXP 3
#define RESPONSE_UNSOLICITED_ACK_EXP 4

// Enable verbose logging
#define VDBG 1

#define MIN(a,b) ((a)<(b) ? (a) : (b))

// Enable RILC log
#define RILC_LOG 0

#if RILC_LOG
    #define startRequest           sprintf(printBuf, "(")
    #define closeRequest           sprintf(printBuf, "%s)", printBuf)
    #define printRequest(token, req)           \
            RLOGD("[%04d]> %s %s", token, requestToString(req), printBuf)

    #define startResponse           sprintf(printBuf, "%s {", printBuf)
    #define closeResponse           sprintf(printBuf, "%s}", printBuf)
    #define printResponse           RLOGD("%s", printBuf)

    #define clearPrintBuf           printBuf[0] = 0
    #define removeLastChar          printBuf[strlen(printBuf)-1] = 0
    #define appendPrintBuf(x...)    snprintf(printBuf, PRINTBUF_SIZE, x)
#else
    #define startRequest
    #define closeRequest
    #define printRequest(token, req)
    #define startResponse
    #define closeResponse
    #define printResponse
    #define clearPrintBuf
    #define removeLastChar
    #define appendPrintBuf(x...)
#endif

#define NUM_ELEMS(a)     (sizeof (a) / sizeof (a)[0])

enum WakeType {DONT_WAKE, WAKE_PARTIAL};

typedef struct CommandInfo CommandInfo;

typedef struct CommandInfo {
    int requestNumber;
    std::function<int(int, int, int, RIL_Errno, void *, size_t)> responseFunction;
} CommandInfo;

typedef struct {
    int requestNumber;
    std::function<int(int, int, int, RIL_Errno, void *, size_t)> responseFunction;
    WakeType wakeType;
} UnsolResponseInfo;

CommandInfo *getCommandInfo(int request);
CommandInfo *getCommandInfo(uint32_t slot, int request);
void updateCommandInfo(CommandInfo *infos, size_t size);
void updateCommandInfo(const CommandInfo& info);
void updateCommandInfo(uint32_t slot, const CommandInfo& info);
UnsolResponseInfo *getUnsolRespInfo(int unsolResponse);
UnsolResponseInfo *getUnsolRespInfo(uint32_t slot, int unsolResponse);
void updateUnsolResponseInfo(UnsolResponseInfo *infos, size_t size);
void updateUnsolResponseInfo(const UnsolResponseInfo& info);
void updateUnsolResponseInfo(uint32_t slot, const UnsolResponseInfo& info);

extern "C" const char * requestToString(int request);

typedef struct RequestInfo {
    int32_t token;      //this is not RIL_Token
    CommandInfo *pCI;
    struct RequestInfo *p_next;
    char cancelled;
    char local;         // responses to local commands do not go back to command process
    RIL_SOCKET_ID socket_id;
    int wasAckSent;    // Indicates whether an ack was sent earlier
} RequestInfo;

RequestInfo *addRequestToList(int serial, int slotId, int request);
bool addRequestToList(RequestInfo *pRI);

char * RIL_getServiceName();

void grabPartialWakeLock();
void releaseWakeLock();

void onNewCommandConnect(RIL_SOCKET_ID socket_id);

}   // namespace android

/* HAL Version Code macro */
#define HAL_VERSION_CODE(m,n)   (((m) & 0xF) << 4 | ((n) & 0xF))
#define HAL_VERSION_DEFAULT()   HAL_VERSION_CODE(1,0)
#define ENCODE_REQUEST(id,h)    (((id) & 0xFFFFFF) | (((h) & 0xFF) << 24))
#define DECODE_REQUEST(id)      ((id) & 0xFFFFFF)
#define DECODE_HAL(id)          (((id) >> 24) & 0xFF)

#define ENCODE_REQUEST_1_6(id)  ENCODE_REQUEST((id),HAL_VERSION_CODE(1,6))
#define ENCODE_REQUEST_2_0(id)  ENCODE_REQUEST((id),HAL_VERSION_CODE(2,0))

#endif //ANDROID_RIL_INTERNAL_H
