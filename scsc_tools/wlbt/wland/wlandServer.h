/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#pragma once

#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/select.h>
#include <sys/types.h>

#include <functional>

#include "cutils/sockets.h"

#define SOCKET_WLAND_BUFFER_SIZE (200+1)

class wlandServer {
 private:
    enum RecvError {
        kOk = 0,
        kFail,
        kRetry
    };
    enum ResultCode {
        kResultOk = 0,
        kResultFail = -1
    };
    static const int kDataLenSzie;
    static int serverSockFd;
    static int clientSockFd;
    static fd_set readFds;
    static char recvBuffer[SOCKET_WLAND_BUFFER_SIZE];
    static std::function<bool(char*)> userHandler;

    static bool setUserHandler(std::function<bool(char*)> handler);
    static bool socketSelect();
    static enum RecvError getData();
    static enum RecvError recvData(int remainingbytes);
    static void handlerResponseSend(bool isSuccess);

 public:
    ~wlandServer() {
        if (serverSockFd >= 0) {
            close(serverSockFd);
        }
    }
    static bool createServer(const char *name, std::function<bool(char*)> handler);
    static bool waitClientConnection();
    static void runService();
};
