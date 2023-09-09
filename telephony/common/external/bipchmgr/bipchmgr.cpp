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
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <unistd.h>
#include <utils/Log.h>

#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/socket.h>
#include <linux/if_arp.h>
#include <pthread.h>

#include <sys/types.h>
#include <ifaddrs.h>

#include "bipchmgr.h"
#include "rilclienthelper.h"
#include "sitril-client.h"

#undef LOG_TAG
#define LOG_TAG "BipChMgr"

#define LogD(format, ...)       ALOGD("%s() " format, __FUNCTION__, ##__VA_ARGS__)
#define LogE(format, ...)       ALOGE("%s() " format, __FUNCTION__, ##__VA_ARGS__)
#define LogW(format, ...)       ALOGW("%s() " format, __FUNCTION__, ##__VA_ARGS__)
#define LogI(format, ...)       ALOGI("%s() " format, __FUNCTION__, ##__VA_ARGS__)
#define LogV(format, ...)       ALOGI("%s() " format, __FUNCTION__, ##__VA_ARGS__)

#define ENTER_FUNC()            { ALOGI("%s() [<-- ", __FUNCTION__); }
#define LEAVE_FUNC()            { ALOGI("%s() [--> ", __FUNCTION__); }

// #### Defintions ####
#define SITRIL_CLIENT_LIB_PATH  "libsitril-client.so"
#define INVALID_RIL_CHANNEL     (-1)

// wlan0, epdg0
#define WIFI_IF_NAME            "wlan0"
#define EPDG_IF_NAME            "epdg0"

#define RECEIVE_INTERVAL        (100 * 1000)      // 100ms

// #### Global Variables ####
void *gSITRILC_Handle = nullptr;
RilClientHelper *g_RilClientHelper = nullptr;

SlotContext g_aSlotContexts[MAX_MODEM_COUNT] = {};
bool g_bEpdg = false;

int Value2HexString(char *pszHexStrOut, const unsigned char *pHexDecIn, int nLength);

// #### Internal Functions ####
static const char *GetMessageString(unsigned int nMsgId)
{
    switch (nMsgId) {
        case RILC_REQ_BIPCHMGR_MEDIA_STATUS_RESPONSE:
            return "RILC_REQ_BIPCHMGR_MEDIA_STATUS_RESPONSE";
        case RILC_REQ_BIPCHMGR_CHANNEL_STATUS_REPORT:
            return "RILC_REQ_BIPCHMGR_CHANNEL_STATUS_REPORT";
        case RILC_REQ_BIPCHMGR_OPEN_CHANNEL_RESPONSE:
            return "RILC_REQ_BIPCHMGR_OPEN_CHANNEL_RESPONSE";
        case RILC_REQ_BIPCHMGR_SEND_DATA_RESPONSE:
            return "RILC_REQ_BIPCHMGR_SEND_DATA_RESPONSE";
        case RILC_REQ_BIPCHMGR_RECEIVE_DATA_RESPONSE:
            return "RILC_REQ_BIPCHMGR_RECEIVE_DATA_RESPONSE";
        case RILC_REQ_BIPCHMGR_CLOSE_CHANNEL_RESPONSE:
            return "RILC_REQ_BIPCHMGR_CLOSE_CHANNEL_RESPONSE";
        case RILC_UNSOL_BIPCHMGR_MEDIA_STATUS_REQUEST:
            return "RILC_UNSOL_BIPCHMGR_MEDIA_STATUS_REQUEST";
        case RILC_UNSOL_BIPCHMGR_OPEN_CHANNEL_REQUEST:
            return "RILC_UNSOL_BIPCHMGR_OPEN_CHANNEL_REQUEST";
        case RILC_UNSOL_BIPCHMGR_SEND_DATA_REQUEST:
            return "RILC_UNSOL_BIPCHMGR_SEND_DATA_REQUEST";
        case RILC_UNSOL_BIPCHMGR_RECEIVE_DATA_REQUEST:
            return "RILC_UNSOL_BIPCHMGR_RECEIVE_DATA_REQUEST";
        case RILC_UNSOL_BIPCHMGR_CLOSE_CHANNEL_REQUEST:
            return "RILC_UNSOL_BIPCHMGR_CLOSE_CHANNEL_REQUEST";
    }
    return "Unknown";
}

static const char *GetRilClientStatusString(unsigned int nStatus)
{
    switch (nStatus) {
        case RILC_STATUS_SUCCESS: return "SUCCESS";
        case RILC_STATUS_FAIL: return "FAIL";
        case RILC_STATUS_NOT_CONNECTED: return "NOT_CONNECTED";
        case RILC_STATUS_INVALID_PARAM: return "INVALID_PARAM";
        case RILC_STATUS_IO: return "IO";
        case RILC_STATUS_NO_RESOURCES: return "NO_RESOURCES";
        case RILC_STATUS_REQUEST_NOT_SUPPORTED: return "REQUEST_NOT_SUPPORTED";
        case RILC_STATUS_CANCELLED: return "CANCELLED";
        case RILC_STATUS_MISSING_RESOURCE: return "MISSING_RESOURCE";
        case RILC_STATUS_NO_SUCH_ELEMENT: return "NO_SUCH_ELEMENT";
        case RILC_STATUS_NO_MEMORY: return "NO_MEMORY";
        case RILC_STATUS_INTERNAL_ERR: return "INTERNAL_ERR";
        case RILC_STATUS_SIM_ERR: return "SIM_ERR";
        case RILC_STATUS_INVALID_SIM_STATE: return "INVALID_SIM_STATE";
    }
    return "Unknown";
}

static void SitRilClient_UnsolicitedResponse(unsigned int msgId, void* data,
        size_t length, unsigned int slotId)
{
    switch (msgId) {
        case RILC_UNSOL_BIPCHMGR_MEDIA_STATUS_REQUEST:
            ProcessBipMediaStatus(slotId);
            break;
        case RILC_UNSOL_BIPCHMGR_OPEN_CHANNEL_REQUEST:
            ProcessBipOpenChannel(slotId, data, length);
            break;
        case RILC_UNSOL_BIPCHMGR_SEND_DATA_REQUEST:
            ProcessBipSendData(slotId, data, length);
            break;
        case RILC_UNSOL_BIPCHMGR_RECEIVE_DATA_REQUEST:
            ProcessBipReceiveData(slotId, data, length);
            break;
        case RILC_UNSOL_BIPCHMGR_CLOSE_CHANNEL_REQUEST:
            ProcessBipCloseChannel(slotId, data, length);
            break;
    }
}

static void printData(const unsigned char* data, int length) {
    if (data != nullptr && 0 < length) {
        char *pszHex = new char[length * 2 + 1];
        memset(pszHex, 0x00, (length * 2 + 1));
        int nDataLen = Value2HexString(pszHex, data, length);
        pszHex[nDataLen] = '\0';
        LogD("Data:%s", pszHex);
        delete [] pszHex;
    }
}

static void SitRilClient_OnResponse(unsigned int msgId, int status, void* data,
        size_t length, unsigned int slotId)
{
    ENTER_FUNC();

    LogV("[%d] %s(%d), status(%d:%s), data(%p), length(%zu)", slotId, GetMessageString(msgId),
            msgId, status, GetRilClientStatusString(status), data, length);

    // Print Data Hex String
    printData((const unsigned char*)data, length);

    switch (msgId) {
        case RILC_REQ_BIPCHMGR_MEDIA_STATUS_RESPONSE:
            OnResponseBipMediaStatus(slotId, status);
            break;
        case RILC_REQ_BIPCHMGR_CHANNEL_STATUS_REPORT:
            OnReportBipChannelStatus(slotId, status);
            break;
        case RILC_REQ_BIPCHMGR_OPEN_CHANNEL_RESPONSE:
            OnResponseBipOpenChannel(slotId, data, status);
            break;
        case RILC_REQ_BIPCHMGR_SEND_DATA_RESPONSE:
            OnResponseBipSendData(slotId, status);
            break;
        case RILC_REQ_BIPCHMGR_RECEIVE_DATA_RESPONSE:
            OnResponseBipReceiveData(slotId, data, status);
            break;
        case RILC_REQ_BIPCHMGR_CLOSE_CHANNEL_RESPONSE:
            OnResponseBipCloseChannel(slotId, status);
            break;
        default:
            LogE("Error:  Unkonwn msgId(%d)", msgId);
            LEAVE_FUNC();
            return;
    }

    LEAVE_FUNC();
}

// #### Initialization Functions ####

void SetIndicationFilter()
{
    int filters[] = {
        RILC_UNSOL_BIPCHMGR_MEDIA_STATUS_REQUEST,
        RILC_UNSOL_BIPCHMGR_OPEN_CHANNEL_REQUEST,
        RILC_UNSOL_BIPCHMGR_SEND_DATA_REQUEST,
        RILC_UNSOL_BIPCHMGR_RECEIVE_DATA_REQUEST,
        RILC_UNSOL_BIPCHMGR_CLOSE_CHANNEL_REQUEST
    };
    size_t size = sizeof(filters) / sizeof(filters[0]);
    if (g_RilClientHelper != nullptr) {
        g_RilClientHelper->SetIndicationFilter(gSITRILC_Handle, filters, size);
    }
}

int rilOpen(void)
{
    ENTER_FUNC();

    int ret = BIPCHMGR_ERROR_NONE;
    for (int i =0; i < MAX_MODEM_COUNT; i++) {
        g_aSlotContexts[i].init();
    }

    do {
        g_RilClientHelper = RilClientHelper::GetInstance();
        if (g_RilClientHelper == nullptr) {
            ret = BIPCHMGR_ERROR_LIB_LOAD_FAIL;
            break;
        }

        // Open
        gSITRILC_Handle = g_RilClientHelper->Open("bipchmgr");
        if (gSITRILC_Handle == nullptr) {
            LogE("%s() Fail in gSITRILC_Open()", __FUNCTION__);
            ret = BIPCHMGR_ERROR_OPEN_FAIL;
            break;
        }
        LogV("%s() gSITRILC_Open() ... OK.",__FUNCTION__);

        // Register indications to be received
        SetIndicationFilter();

        // Register Callback
        ret = g_RilClientHelper->RegisterUnsolicitedResponseHandler(gSITRILC_Handle, 
                SitRilClient_UnsolicitedResponse);
        if (ret != BIPCHMGR_ERROR_NONE) {
            LogE("%s() Fail in gSITRILC_Open()",__FUNCTION__);
            ret = BIPCHMGR_ERROR_REGISTRATION_FAIL;
            break;
        }
    } while(0);

    if (ret != BIPCHMGR_ERROR_NONE) {
        rilClose();
    }

    LEAVE_FUNC();
    return ret;
}

void rilClose(void)
{
    ENTER_FUNC();

    if (g_RilClientHelper != nullptr) {
        g_RilClientHelper->Close(gSITRILC_Handle);
        LogI("%s() gSITRILC_Close() ... OK.", __FUNCTION__);
    }

    LEAVE_FUNC();
}

int SendRequest(int slotId, unsigned int msgId, void *data, int data_len)
{
    ENTER_FUNC();

    int ret = BIPCHMGR_ERROR_NONE;

    do {
        // Check loaded library
        if (gSITRILC_Handle == nullptr) {
            LogE("Error: gSITRILC_Handle = nullptr");
            ret = BIPCHMGR_ERROR_NOT_OPENED_LIB;
            break;
        }

        // Sending A message
        ret = g_RilClientHelper->Send(gSITRILC_Handle, msgId, data, data_len,
                SitRilClient_OnResponse, slotId);
        if (ret == BIPCHMGR_ERROR_NONE) {
            LogD("[%d] Sent: %d bytes", slotId, data_len);
        } else {
            LogE("Error:  Fail in g_RilClientHelper->Send()");
            ret = BIPCHMGR_ERROR_SEND_FAIL;
            break;
        }
    } while (0);

    LEAVE_FUNC();
    return ret;
}

int Value2HexString(char *pszHexStrOut, const unsigned char *pHexDecIn, int nLength)
{
    int nResult = -1;
    if (pszHexStrOut && pHexDecIn && nLength > 0) {
        int nSrcOffset = 0;
        for (int i = 0; i < nLength; i++) {
            nSrcOffset += snprintf(&pszHexStrOut[nSrcOffset], 3, "%02X",
                    (unsigned int) pHexDecIn[i]);
        }
        pszHexStrOut[nSrcOffset] = '\0';
        nResult = nSrcOffset;
    }
    return nResult;
}

// WiFi Status
int ProcessBipMediaStatus(unsigned int slotId)
{
    ENTER_FUNC();

    unsigned char cStatus = MEDIA_DISCONNECTED;
    int nRet = BIPCHMGR_ERROR_FAILURE;
    g_bEpdg = false;

    struct ifaddrs *pInfs = nullptr;

    // Zero is success(no error).
    if (getifaddrs(&pInfs) != -1) {
        for (struct ifaddrs *ifa = pInfs; pInfs && ifa; ifa = ifa->ifa_next) {
            if (ifa->ifa_addr) {
                bool bIpAddr = false;

                if (ifa->ifa_addr->sa_family == AF_INET) {  // IPv4
                    char szIpAddress[INET_ADDRSTRLEN] = {0,};
                    struct sockaddr_in *sa = (struct sockaddr_in *) ifa->ifa_addr;
                    //pszIPv4 = inet_ntoa(sa->sin_addr);
                    inet_ntop(AF_INET, &sa->sin_addr, szIpAddress, INET_ADDRSTRLEN);
                    LogV("Interface:%s, %s(IPv4)", ifa->ifa_name, szIpAddress);
                    bIpAddr = true;
                } else if (ifa->ifa_addr->sa_family == AF_INET6) { // IPv6
                    char szIpAddress[INET6_ADDRSTRLEN] = {0,};
                    struct sockaddr_in6 *sa = (struct sockaddr_in6 *) ifa->ifa_addr;
                    inet_ntop(AF_INET6, &sa->sin6_addr, szIpAddress, INET6_ADDRSTRLEN);
                    LogV("Interface:%s, %s(IPv6)", ifa->ifa_name, szIpAddress);
                    bIpAddr = true;
                }

                if (bIpAddr && strcmp(ifa->ifa_name, WIFI_IF_NAME) == 0) {
                    cStatus = MEDIA_CONNECTED;
                    LogV("Found - WiFi:%s", ifa->ifa_name);
                } else if (bIpAddr && strcmp(ifa->ifa_name, EPDG_IF_NAME) == 0) {
                    cStatus = MEDIA_CONNECTED;
                    g_bEpdg = true;
                    LogV("Found - ePDG:%s", ifa->ifa_name);
                }
            }
        }
        if(pInfs) freeifaddrs(pInfs);
    }

    nRet = ResponseBipMediaStatus(slotId, cStatus);

    LEAVE_FUNC();
    return nRet;
}

int ResponseBipMediaStatus(unsigned int slotId, unsigned char status)
{
    ENTER_FUNC();

    unsigned char ucStatus = status;
    int nRet = SendRequest(slotId, RILC_REQ_BIPCHMGR_MEDIA_STATUS_RESPONSE, &ucStatus,
            sizeof(ucStatus));

    LEAVE_FUNC();
    return nRet;
}

void OnResponseBipMediaStatus(unsigned int /*slotId*/, unsigned char /*result*/)
{
}

// OPEN CHANNEL
int ProcessBipOpenChannel(unsigned int slotId, void* data, size_t /* length */)
{
    ENTER_FUNC();

    int nRet = BIPCHMGR_ERROR_FAILURE;
    BipContext *pContext = getEmptyBipContext(slotId);
    if (pContext == nullptr) {
        nRet = ResponseBipOpenChannel(slotId, pContext, RESULT_FAILURE);
        return nRet;
    }

    BipOpenChannelRequest *pReq = (BipOpenChannelRequest *) data;
    char szTransportType[16] = {0, };
    switch (pReq->transport_type) {
        case TRANSPORT_TCP: strcpy(szTransportType, "TCP"); break;
        case TRANSPORT_UDP: strcpy(szTransportType, "UDP"); break;
        default: strcpy(szTransportType, "Unknown"); break;
    }

    int nSockDomain = 0;
    char szIpType[16] = {0,};
    char szIpAddress[INET6_ADDRSTRLEN] = {0,};
    switch (pReq->ip_type) {
        case TYPE_IPV4:
            strcpy(szIpType, "IPv4");
            nSockDomain = AF_INET;
            inet_ntop(nSockDomain, (struct in_addr *)pReq->ipv4, szIpAddress, INET_ADDRSTRLEN);
            break;
        case TYPE_IPV6:
            strcpy(szIpType, "IPv6");
            nSockDomain = AF_INET6;
            inet_ntop(nSockDomain, (struct in6_addr *)pReq->ipv6, szIpAddress,
                    INET6_ADDRSTRLEN);
            break;
        case TYPE_IPV4V6: strcpy(szIpType, "IPv4&IPv6"); break;
        case TYPE_PDP_PPP: strcpy(szIpType, "PPP"); break;
        case TYPE_PDP_NON_IP: strcpy(szIpType, "NonIP"); break;
        default: strcpy(szIpType, "Unknown"); break;
    }

    LogV("Transport type : %d(%s)", pReq->transport_type, szTransportType);
    LogV("Port : %d", pReq->port);
    LogV("IP Type : %d(%s)", pReq->ip_type, szIpType);
    LogV("IP Address : %s", szIpAddress);
    LogV("Max Buffer Size : %d", pReq->buffer_size);
    memcpy(&(pContext->tOpenChannelReq), pReq, sizeof(BipOpenChannelRequest));

    // Close socket if previous socket is not closed.
    if (pContext->sock != -1) {
        close(pContext->sock);
        pContext->sock = -1;
    }

    // Create Socket
    do {
        switch (pContext->tOpenChannelReq.transport_type) {
            case TRANSPORT_TCP:
                pContext->sock = socket(nSockDomain, SOCK_STREAM | SOCK_CLOEXEC, 0);
                break;
            case TRANSPORT_UDP:
                pContext->sock = socket(nSockDomain, SOCK_DGRAM | SOCK_CLOEXEC , 0);
                break;
        }

        if (pContext->sock == -1) {
            break;
        }

        char szInterface[12] = {0, };
        if (g_bEpdg) {
            strcpy(szInterface, EPDG_IF_NAME);
        } else {
            strcpy(szInterface, WIFI_IF_NAME);
        }

        if (setsockopt(pContext->sock, SOL_SOCKET, SO_BINDTODEVICE, szInterface,
                    strlen(szInterface))) {
            LogE("Setsockopt() error[SO_BINDTODEVICE-%s] : %s", szInterface, strerror(errno));
            break;
        }

        struct timeval timeout = { SEND_TIMEOUT, 0 };     // Second
        if (setsockopt(pContext->sock, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout,
                    sizeof(timeout))) {
            LogE("Setsockopt() error[SO_SNDTIMEO-%s] : %s", szInterface, strerror(errno));
            break;
        }

        timeout.tv_sec = RECEVE_TIMEOUT;
        timeout.tv_usec = 0;
        if (setsockopt(pContext->sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout,
                    sizeof(timeout))) {
            LogE("Setsockopt() error[SO_RCVTIMEO-%s] : %s", szInterface, strerror(errno));
            break;
        }

        int nSendBufferSize = 0, nRecvBufferSize = 0;
        socklen_t len = sizeof(int);
        if(getsockopt(pContext->sock, SOL_SOCKET, SO_SNDBUF, &nSendBufferSize, &len)) {
            LogE("GetSockopt() error[SO_SNDBUF] : %s", strerror(errno));
            break;
        }

        len = sizeof(int);
        if(getsockopt(pContext->sock, SOL_SOCKET, SO_RCVBUF, &nRecvBufferSize, &len)) {
            LogE("GetSockopt() error[SO_RCVBUF] : %s", strerror(errno));
            break;
        }

        LogV("Send buffer size : %d", nSendBufferSize);
        LogV("Receive buffer size : %d", nRecvBufferSize);

        // Reset the thread creation flag
        pContext->receive_thread = false;

        // Socket creation success
        nRet = ResponseBipOpenChannel(slotId, pContext, RESULT_NO_ERROR);
        LEAVE_FUNC();
        return nRet;
    } while(0);

    LogE("Socket creation Failed!!!");
    nRet = ResponseBipOpenChannel(slotId, pContext, RESULT_FAILURE);
    LEAVE_FUNC();
    return nRet;
}

int ResponseBipOpenChannel(unsigned int slotId, BipContext *pContext,
        unsigned char result)
{
    ENTER_FUNC();

    BipOpenChannelResponse tRsp;

    tRsp.session_id = (pContext != nullptr) ? pContext->nSessionId : 0;
    tRsp.status = result;
    int nRet = SendRequest(slotId, RILC_REQ_BIPCHMGR_OPEN_CHANNEL_RESPONSE, &tRsp,
            sizeof(BipOpenChannelResponse));

    LEAVE_FUNC();
    return nRet;
}

void OnResponseBipOpenChannel(unsigned int slotId, void* data, unsigned char result)
{
    ENTER_FUNC();

    do {
        if (result != 0) {
            LogE("Error(%d)", result);
            break;
        }

        BipOpenChannelOnResponse *pRsp = (BipOpenChannelOnResponse*) data;
        if (pRsp == nullptr) {
            LogE("pReq is nullptr!!!");
            break;
        }

        BipContext *pContext = getBipContext(slotId, pRsp->session_id);
        if (pContext == nullptr) {
            LogE("pContext is nullptr!!!");
            break;
        }

        // Channel Event for TCP Connection
        LogI("transport_type: %d", pContext->tOpenChannelReq.transport_type);
        if (pContext->tOpenChannelReq.transport_type == TRANSPORT_TCP) {
            if (pContext->sock == -1) {
                 LogE("socket error(%d)", pContext->sock);
                 break;
            }

            struct sockaddr *sockAddress = nullptr;
            int nAddrLength = 0;
            struct sockaddr_in addr = {};
            struct sockaddr_in6 addr6 = {};
            char szIpAddress[INET6_ADDRSTRLEN] = {0,};

            if (pContext->tOpenChannelReq.ip_type == TYPE_IPV4) {
                struct in_addr *sin = (struct in_addr *) pContext->tOpenChannelReq.ipv4;
                addr.sin_family = AF_INET;
                addr.sin_addr = *sin;
                addr.sin_port = htons(pContext->tOpenChannelReq.port);
                sockAddress = (struct sockaddr *) &addr;
                nAddrLength = sizeof(struct sockaddr_in);
                inet_ntop(AF_INET, (struct in_addr *)pContext->tOpenChannelReq.ipv4, szIpAddress,
                        INET_ADDRSTRLEN);
            } else if (pContext->tOpenChannelReq.ip_type == TYPE_IPV6) {
                struct in6_addr *sin = (struct in6_addr *) pContext->tOpenChannelReq.ipv6;
                addr6.sin6_family = AF_INET6;
                addr6.sin6_addr = *sin;
                addr6.sin6_port = htons(pContext->tOpenChannelReq.port);
                sockAddress = (struct sockaddr *) &addr6;
                nAddrLength = sizeof(struct sockaddr_in6);
                inet_ntop(AF_INET6, (struct in6_addr *)pContext->tOpenChannelReq.ipv6,
                        szIpAddress, INET6_ADDRSTRLEN);
            }

            if (sockAddress == nullptr) {
                LogE("pAddr is invalid");
                break;
            }

            LogV("Connecting to %s:%d", szIpAddress, pContext->tOpenChannelReq.port);
            int newSockStat;
            int orgSockStat;
            if ((orgSockStat = fcntl(pContext->sock, F_GETFL, nullptr)) < 0) {
                LogE("F_GETFL error");
                break;
            }

            // Set non-blocking
            newSockStat = orgSockStat | O_NONBLOCK;
            if (fcntl(pContext->sock, F_SETFL, newSockStat) < 0) {
                LogE("F_SETLF error");
                break;
            }

            // connect() returns immediately
            if (connect(pContext->sock, sockAddress, nAddrLength) != 0) {
                // Connection in progress
                if (errno == EINPROGRESS || errno == EWOULDBLOCK) {
                    fd_set fdset;
                    FD_ZERO(&fdset);
                    FD_SET(pContext->sock, &fdset);
                    struct timeval timeout = {CONNECTION_TIMEOUT, 0};     // Second
                    if (select(pContext->sock + 1, nullptr, &fdset, nullptr, &timeout) > 0) {
                        LogV("%s:%d Connection established", szIpAddress,
                                pContext->tOpenChannelReq.port);
                        pContext->tcp_connected = true;
                        ReportBipChannelStatus(slotId, pContext, CHANNEL_EVENT_CONNECTED);
                    } else {
                        LogE("%s:%d Connection timeout", szIpAddress,
                                pContext->tOpenChannelReq.port);
                    }
                } else {
                    LogE("%s:%d Connection Failed: %s", szIpAddress, pContext->tOpenChannelReq.port,
                            strerror(errno));
                }
            } else {
                LogE("%s:%d Connection Failed: %s", szIpAddress, pContext->tOpenChannelReq.port,
                        strerror(errno));
            }

            // Restore setting
            if (fcntl(pContext->sock, F_SETFL, orgSockStat) >= 0) {
                LogV("%s: Restore setting", szIpAddress);
            } else {
                LogE("F_SETLF error");
            }
        }
    } while(0);

    LEAVE_FUNC();
}

// Channel Status Report
int ReportBipChannelStatus(unsigned int slotId, BipContext *pContext,
        unsigned char status)
{
    ENTER_FUNC();

    BipChannelStatusReport tRsp;

    tRsp.session_id = (pContext != nullptr) ? pContext->nSessionId : 0;
    tRsp.status = status;
    int nRet = SendRequest(slotId, RILC_REQ_BIPCHMGR_CHANNEL_STATUS_REPORT, &tRsp,
            sizeof(BipChannelStatusReport));

    LEAVE_FUNC();
    return nRet;
}

void OnReportBipChannelStatus(unsigned int /* slotId */, unsigned char /* result */)
{
}

// SEND DATA
int ProcessBipSendData(unsigned int slotId, void* data, size_t length)
{
    ENTER_FUNC();

    unsigned char result = RESULT_FAILURE;
    BipSendDataRequest *pReq = (BipSendDataRequest *) data;
    BipContext *pContext = getBipContext(slotId, pReq->session_id);

    do {
        if (pContext == nullptr) {
            break;
        }

        if (pContext->sock != -1 && length > 2) { // 2 is length of data
            if (pContext->tOpenChannelReq.transport_type == TRANSPORT_TCP) {
                if (pContext->tcp_connected) {
                    int nSent = send(pContext->sock, pReq->data, pReq->length, 0);
                    if (nSent >= 0) {
                        result = RESULT_NO_ERROR;
                    }
                    if (pContext->receive_thread == false) {
                        LogV("[SendData] Start TCP receive thread...");
                        CreateReceiveThread(pContext);
                    } else {
                        LogV("[SendData] TCP receive thread is already started");
                    }
                } else {
                    LogE("[SendData] TCP socket is not connected");
                }
            } else if (pContext->tOpenChannelReq.transport_type == TRANSPORT_UDP) {
                struct sockaddr *sockAddress = nullptr;
                int nAddrLength = 0;
                struct sockaddr_in addr = {};
                struct sockaddr_in6 addr6 = {};

                if (pContext->tOpenChannelReq.ip_type == TYPE_IPV4) {
                    struct in_addr *sin = (struct in_addr *) pContext->tOpenChannelReq.ipv4;
                    addr.sin_family = AF_INET;
                    addr.sin_addr = *sin;
                    addr.sin_port = htons(pContext->tOpenChannelReq.port);
                    sockAddress = (struct sockaddr *) &addr;
                    nAddrLength = sizeof(struct sockaddr_in);
                } else if (pContext->tOpenChannelReq.ip_type == TYPE_IPV6) {
                    struct in6_addr *sin = (struct in6_addr *) pContext->tOpenChannelReq.ipv6;
                    addr6.sin6_family = AF_INET6;
                    addr6.sin6_addr = *sin;
                    addr6.sin6_port = htons(pContext->tOpenChannelReq.port);
                    sockAddress = (struct sockaddr *) &addr6;
                    nAddrLength = sizeof(struct sockaddr_in6);
                }

                if (sockAddress == nullptr) {
                    LogE("[SendData] result failure! sockAddress is invalid");
                    break;
                }

                int nSent = sendto(pContext->sock, pReq->data, pReq->length, 0,
                        (struct sockaddr *) sockAddress, nAddrLength);
                if (nSent >= 0) {
                    result = RESULT_NO_ERROR;
                }
                if (pContext->receive_thread == false) {
                    LogV("[SendData] Start UDP receive thread...");
                    CreateReceiveThread(pContext);
                } else {
                    LogV("[SendData] TCP receive thread is already started");
                }
            } else {
                LogE("pContext->tOpenChannelReq.transport_type is %d",
                        pContext->tOpenChannelReq.transport_type);
            }
        }
    } while(0);

    ResponseBipSendData(slotId, pContext, result);
    LEAVE_FUNC();
    return 0;
}

int ResponseBipSendData(unsigned int slotId, BipContext *pContext, unsigned char result)
{
    ENTER_FUNC();

    BipSendDataResponse tRsp;

    tRsp.session_id = (pContext != nullptr) ? pContext->nSessionId : 0;
    tRsp.status = result;
    int nRet = SendRequest(slotId, RILC_REQ_BIPCHMGR_SEND_DATA_RESPONSE, &tRsp,
            sizeof(BipSendDataResponse));

    LEAVE_FUNC();
    return nRet;
}

void OnResponseBipSendData(unsigned int /* slotId */, unsigned char /* result */)
{
}

// RECEIVE DATA
int ProcessBipReceiveData(unsigned int slotId, void* data, size_t /* length */)
{
    ENTER_FUNC();

    BipReceiveDataRequest *pReq = (BipReceiveDataRequest *) data;
    BipContext *pContext = getBipContext(slotId, pReq->session_id);
    if (pContext == nullptr) {
        ResponseBipReceiveData(slotId, pContext);
        return 0;
    }

    if (pContext->sock != -1) {
        ResponseBipReceiveData(slotId, pContext);
    } else {
        ResponseBipReceiveData(slotId, nullptr);
    }

    LEAVE_FUNC();
    return 0;
}

int ResponseBipReceiveData(unsigned int slotId, BipContext *pContext)
{
    ENTER_FUNC();

    BipReceiveDataResponse tRsp = {};

    do {
        if (pContext != nullptr) {
            tReceiveData receiveData = {};
            if (pContext->tDataQueue.empty() == false) {
                receiveData = pContext->tDataQueue.front();
                pContext->tDataQueue.pop();

                if (receiveData.ucChannelEvent == CHANNEL_EVENT_RECEIVED) {
                    tRsp.session_id = pContext->nSessionId;
                    tRsp.length = receiveData.nDataSize;
                    if (0 < tRsp.length) {
                        memcpy(tRsp.data, receiveData.aBuffer, tRsp.length);
                    }
                } else {
                    LogE("Channel state is not RECEIVED, Send channel event(%d)",
                            receiveData.ucChannelEvent);
                    ReportBipChannelStatus(pContext->nSessionId, pContext,
                            receiveData.ucChannelEvent);
                    break;
                }
            } else {
                LogE("Receive Data is empty");
                tRsp.session_id = pContext->nSessionId;
                tRsp.length = 0;
            }
        } else {
            LogE("pContext is nullptr");
            tRsp.session_id = 0;
            tRsp.length = 0;
        }

        SendRequest(slotId, RILC_REQ_BIPCHMGR_RECEIVE_DATA_RESPONSE, &tRsp,
                sizeof(BipReceiveDataResponse));
    } while(0);

    LEAVE_FUNC();
    return 0;
}

void OnResponseBipReceiveData(unsigned int slotId, void* data, unsigned char result)
{
    ENTER_FUNC();

    if (result != 0) {
        LogE("Error(%d)", result);
        LEAVE_FUNC();
        return;
    }

    BipReceiveDataOnResponse* pRsp = (BipReceiveDataOnResponse*)data;
    if (pRsp == nullptr) {
        LogE("pRsp is nullptr!!!");
        LEAVE_FUNC();
        return;
    }

    BipContext *pContext = getBipContext(slotId, pRsp->session_id);
    if (pContext == nullptr) {
        LogE("pContext is nullptr!!!");
        LEAVE_FUNC();
        return;
    }

    // Check if there are the data in queue.
    if (pContext->tDataQueue.empty() == false) {
        tReceiveData data = pContext->tDataQueue.front();
        ReportBipChannelStatus(pContext->nSessionId, pContext, data.ucChannelEvent);
        if (data.ucChannelEvent == CHANNEL_EVENT_CLOSED) {
            ClearBipContext(pContext);
        }
    }

    LEAVE_FUNC();
}

// CLOSE CHANNEL
int ProcessBipCloseChannel(unsigned int slotId, void* data, size_t /* length */)
{
    ENTER_FUNC();

    do {
        BipCloseChannelRequest *pReq = (BipCloseChannelRequest *) data;
        BipContext *pContext = getBipContext(slotId, pReq->session_id);

        if (pContext == nullptr) {
            ResponseBipCloseChannel(slotId, nullptr, RESULT_FAILURE);
            break;
        }

        if(pContext->sock != 1) {
            ResponseBipCloseChannel(slotId, pContext, RESULT_NO_ERROR);
        } else {
            ResponseBipCloseChannel(slotId, pContext, RESULT_FAILURE);
        }

        ClearBipContext(pContext);

        LogI("[%d]bUsed : %d, receive_thread : %d", pContext->nSessionId, pContext->bUsed,
                pContext->receive_thread);
    } while(0);

    LEAVE_FUNC();
    return 0;
}

int ResponseBipCloseChannel(unsigned int slotId, BipContext *pContext,
        unsigned char result)
{
    ENTER_FUNC();
    BipCloseChannelResponse tRsp;

    tRsp.session_id = (pContext != nullptr) ? pContext->nSessionId : 0;
    tRsp.status = result;
    int nRet = SendRequest(slotId, RILC_REQ_BIPCHMGR_CLOSE_CHANNEL_RESPONSE, &tRsp,
            sizeof(BipCloseChannelResponse));

    LEAVE_FUNC();
    return nRet;
}

void OnResponseBipCloseChannel(unsigned int /* slotId */, unsigned char /* result */)
{
}

int CreateReceiveThread(BipContext *pContext)
{
    ENTER_FUNC();
    int nRet = -1;

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    nRet = pthread_create(&(pContext->threadId), &attr, &RecevingThreadProc, (void *) pContext);
    if(nRet == 0) {
        LogV("Thread ID :0x%lX", pContext->threadId);
        pContext->receive_thread = true;
    } else {
        LogE("Receiving thread creation failed");
        close(pContext->sock);
        pContext->sock = -1;
    }

    pthread_attr_destroy(&attr);

    LEAVE_FUNC();
    return nRet;
}

// Thread procedure
void *RecevingThreadProc(void *arg)
{
    ENTER_FUNC();
    tReceiveData recvData;
    pthread_t nThreadId = pthread_self();
    BipContext *pContext = (BipContext *) arg;
    LogV("[0x%lX] (%d) Started normally", nThreadId, (pContext)? pContext->nSessionId : -1);
    if (pContext) {
        while (pContext->sock != -1) {
            int nRxDataLen = 0;
            int nResult;
            fd_set readset;
            struct timeval timeout = { RECEVE_TIMEOUT, 0 };
            FD_ZERO(&readset);
            FD_SET(pContext->sock, &readset);
            memset(&recvData, 0x00, sizeof(tReceiveData));
            nResult = select(pContext->sock + 1, &readset, nullptr, nullptr,  &timeout);

            if (nResult > 0) {
                if (FD_ISSET(pContext->sock, &readset)) {
                    // MAX buffer size from OpenChannelRequest from CP
                    unsigned int nMaxBufferSize =
                        (pContext->tOpenChannelReq.buffer_size < MAX_DATA_SIZE)
                        ? pContext->tOpenChannelReq.buffer_size : MAX_DATA_SIZE;

                    /* The socket_fd has data available to be read */
                    if (pContext->tOpenChannelReq.transport_type == TRANSPORT_TCP) {
                        nRxDataLen = recv(pContext->sock, recvData.aBuffer, nMaxBufferSize, 0);
                    } else if (pContext->tOpenChannelReq.transport_type == TRANSPORT_UDP) {
                        struct sockaddr *socketAddress = nullptr;
                        socklen_t nAddrLength = 0;
                        struct sockaddr_in addr = {};
                        struct sockaddr_in6 addr6 = {};
                        if (pContext->tOpenChannelReq.ip_type == TYPE_IPV4) {
                            struct in_addr *sin = (struct in_addr *) pContext->tOpenChannelReq.ipv4;
                            addr.sin_family = AF_INET;
                            addr.sin_addr = *sin;
                            addr.sin_port = htons(pContext->tOpenChannelReq.port);
                            socketAddress = (struct sockaddr *) &addr;
                            nAddrLength = sizeof(struct sockaddr_in);
                        } else if(pContext->tOpenChannelReq.ip_type==TYPE_IPV6) {
                            struct in6_addr *sin =
                                    (struct in6_addr *) pContext->tOpenChannelReq.ipv6;
                            addr6.sin6_family = AF_INET6;
                            addr6.sin6_addr = *sin;
                            addr6.sin6_port = htons(pContext->tOpenChannelReq.port);
                            socketAddress = (struct sockaddr *) &addr6;
                            nAddrLength = sizeof(struct sockaddr_in6);
                        }

                        if (socketAddress == nullptr) {
                            LogE("socketAddress is invalid");
                            break;
                        }

                        nRxDataLen = recvfrom(pContext->sock, (void *) recvData.aBuffer,
                                (size_t) nMaxBufferSize, 0, socketAddress,
                                (socklen_t*) &nAddrLength);
                    }

                    if (0 < nRxDataLen) {
                        LogV("[0x%lX] (%d) RxDataLen : %d bytes", nThreadId, pContext->nSessionId,
                                nRxDataLen);
                        if (pContext->tDataQueue.empty()) {
                            ReportBipChannelStatus(pContext->nSessionId, pContext,
                                    CHANNEL_EVENT_RECEIVED);
                        }
                        recvData.nDataSize = nRxDataLen;
                        recvData.ucChannelEvent = CHANNEL_EVENT_RECEIVED;
                        pContext->tDataQueue.push(recvData);
                    } else if (0 == nRxDataLen) {
                        LogE("[0x%lX] (%d) Socket is closed", nThreadId, pContext->nSessionId);
                        if (pContext->tDataQueue.empty()) {
                            ReportBipChannelStatus(pContext->nSessionId, pContext,
                                    CHANNEL_EVENT_CLOSED);
                            ClearBipContext(pContext);
                        } else {
                            recvData.nDataSize = 0;
                            recvData.ucChannelEvent = CHANNEL_EVENT_CLOSED;
                            pContext->tDataQueue.push(recvData);
                        }
                        break;
                    } else if (errno!=EWOULDBLOCK && errno != EAGAIN) {
                        LogE("[0x%lX] (%d) receive error", nThreadId, pContext->nSessionId);
                        if (pContext->tDataQueue.empty()) {
                            ReportBipChannelStatus(pContext->nSessionId, pContext,
                                    CHANNEL_EVENT_CLOSED);
                            ClearBipContext(pContext);
                        } else {
                            recvData.nDataSize = 0;
                            recvData.ucChannelEvent = CHANNEL_EVENT_CLOSED;
                            pContext->tDataQueue.push(recvData);
                        }
                        break;
                    }
                }
            } else if(nResult == 0) {
                LogE("[0x%lX] (%d) Timeout error : %s ", nThreadId, pContext->nSessionId,
                        strerror(errno));
            } else if (nResult < 0) {
                /* An error ocurred, just print it to stdout */
                LogE("[0x%lX] (%d) Error on select() : %s ", nThreadId, pContext->nSessionId,
                        strerror(errno));
                break;
            }
            usleep(RECEIVE_INTERVAL);
        }
        pthread_exit((void*)0);
    } else {
        LogE("[0x%lX] pContext is nullptr", nThreadId);
    }

    LogV("[0x%lX] (%d) Terminating normally", nThreadId, (pContext)? pContext->nSessionId : -1);
    LEAVE_FUNC();
    return nullptr;
}

BipContext* getEmptyBipContext(int slotId)
{
    for (int i = 0; i < MAX_SESSION_COUNT; i++) {
        if (false == g_aSlotContexts[slotId].aBipContexts[i].bUsed) {
            g_aSlotContexts[slotId].aBipContexts[i].bUsed = true;
            g_aSlotContexts[slotId].aBipContexts[i].nSessionId = i;
            LogI("slotId: %d, Context index : %d", slotId,
                    g_aSlotContexts[slotId].aBipContexts[i].nSessionId);
            return &g_aSlotContexts[slotId].aBipContexts[i];
        }
    }

    LogE("All BipContexts are used!!!!");
    return nullptr;
}

BipContext* getBipContext(int slotId, int sessionId)
{
    if (sessionId < 0 && MAX_SESSION_COUNT <= sessionId) {
        LogE("Session range error!!!(%d)", sessionId);
        return nullptr;
    }

    if (true == g_aSlotContexts[slotId].aBipContexts[sessionId].bUsed) {
        return &g_aSlotContexts[slotId].aBipContexts[sessionId];
    }

    LogE("This session(%d) is not opened !!!", sessionId);
    return nullptr;
}

void ClearBipContext(BipContext *pContext)
{
    ENTER_FUNC();

    if (pContext != nullptr) {
        int sessionId = pContext->nSessionId;
        if (pContext->sock != -1) {
            close(pContext->sock);
        }
        pContext->init(sessionId);
    }

    LEAVE_FUNC();
}

int32_t main(void)
{
    if (rilOpen() == BIPCHMGR_ERROR_NONE) {
        LogV("rilOpen() success");
        while (true) {
            sleep(UINT32_MAX);
        }
    } else {
        LogE("Open() fail");
    }
    return 1;
}

