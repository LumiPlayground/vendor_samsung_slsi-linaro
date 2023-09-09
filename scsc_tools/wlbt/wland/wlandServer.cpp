/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#include "wlandServer.h"

#include <utils/Log.h>
#include <string>


const int wlandServer::kDataLenSzie = 4;

int wlandServer::serverSockFd = -1;
int wlandServer::clientSockFd = -1;
fd_set wlandServer::readFds;
char wlandServer::recvBuffer[SOCKET_WLAND_BUFFER_SIZE];
std::function<bool(char*)> wlandServer::userHandler;

bool wlandServer::setUserHandler(std::function<bool(char*)> handler) {
    if (!handler) {
        return false;
    }
    userHandler = handler;
    return true;
}

bool wlandServer::socketSelect() {
    int ret = 0;

    while (true) {
        FD_ZERO(&readFds);
        FD_SET(clientSockFd, &readFds);

        ret = select(clientSockFd + 1, &readFds, NULL, NULL, NULL);
        if (ret < 0 && errno == EINTR) {
            continue;
        } else if (ret < 0) {
            return false;
        } else {
            break;
        }
    }

    if (FD_ISSET(clientSockFd, &readFds)) {
        return true;
    }
    return false;
}

wlandServer::RecvError wlandServer::getData() {
    int dataLen = 0;
    RecvError recvRet = kOk;

    memset(recvBuffer, 0x0, sizeof(recvBuffer));

    recvRet = recvData(kDataLenSzie);
    if (recvRet != kOk) {
        return recvRet;
    }
    memcpy(&dataLen, recvBuffer, kDataLenSzie);
    dataLen = ntohl(dataLen);

    if (dataLen < 0 || dataLen > SOCKET_WLAND_BUFFER_SIZE-1) {
        ALOGD("Invalid data length [%d]", dataLen);
        return kFail;
    }

    recvRet = recvData(dataLen);
    if (recvRet != kOk) {
        return recvRet;
    }

    ALOGD("JSON: %s", recvBuffer);
    return kOk;
}

wlandServer::RecvError wlandServer::recvData(int dataLen) {
    int recvLen = 0;
    int remainLen = dataLen;

    while (remainLen > 0) {
        recvLen = recv(clientSockFd, recvBuffer + (dataLen - remainLen), remainLen, 0);
        if (recvLen == 0) {
            ALOGE("recvDataLength : No data");
            break;
        } else if (recvLen < 0 && (errno == EINTR || errno == EAGAIN)) {
            ALOGD("recv error [%d]", errno);
            return kRetry;
        } else if (recvLen < 0) {
            break;
        }
        remainLen -= recvLen;
    }
    if (remainLen > 0) {
        ALOGE("Connection lost");
        return kFail;
    }
    return kOk;
}

void wlandServer::handlerResponseSend(bool isSuccess) {
    std::string response;

    if (isSuccess) {
        response = std::to_string(kResultOk);
    } else {
        response = std::to_string(kResultFail);
    }
    send(clientSockFd, response.c_str(), response.length(), 0);
}

bool wlandServer::createServer(const char *name, std::function<bool(char*)> handler) {
    int sockFd = -1;

    if (!name) {
        return false;
    }
    if (!setUserHandler(handler)) {
        return false;
    }

    sockFd = socket_local_server(name, ANDROID_SOCKET_NAMESPACE_ABSTRACT, SOCK_STREAM);
    if (sockFd < 0) {
        ALOGE("Fail to create socket, errno = %d", errno);
        return false;
    }

    serverSockFd = sockFd;
    return true;
}

bool wlandServer::waitClientConnection() {
    int sockFd = -1;

    ALOGD("Server is waiting client...");
    sockFd = accept(serverSockFd, NULL, NULL);
    if (sockFd < 0) {
        return false;
    }

    clientSockFd = sockFd;
    ALOGD("New client is connected... [%d]", clientSockFd);
    return true;
}

void wlandServer::runService() {
    RecvError recvRet = kOk;
    bool userHandlerRet = false;

    while (true) {
       if (!socketSelect()) {
            break;
        }

        recvRet = getData();
        if (recvRet == kRetry) {
            continue;
        } else if (recvRet == kFail) {
            break;
        }

        userHandlerRet = userHandler(recvBuffer);
        handlerResponseSend(userHandlerRet);
        break;
    }
    if (clientSockFd >= 0) {
        close(clientSockFd);
    }
}

