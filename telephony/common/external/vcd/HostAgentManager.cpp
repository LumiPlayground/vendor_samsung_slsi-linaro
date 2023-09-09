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
 * HostAgentManager.cpp
 *
 *  Created on: 2020. 2. 12.
 */
#include <errno.h>
#include <fcntl.h>
#include <cutils/properties.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/un.h>
#include <termios.h>
#include <unistd.h>

#include "HostAgentManager.h"
#include "ModemProxy.h"
#include "utils.h"
#include "base/log.h"
#include "base/systemproperty.h"
#include "base/thread.h"

class BufferedWriter {
    const size_t MAX_CMD_BUF_SIZE = (1024 * 4);
private:
    char *mBuffer;
    size_t mPos;
    bool mFlag;
public:
    BufferedWriter(bool flag = false) {
        mFlag = flag;
        mPos = 0;
        mBuffer = new char[MAX_CMD_BUF_SIZE + 1];
        if (mBuffer != NULL) {
            memset(mBuffer, 0, MAX_CMD_BUF_SIZE + 1);
        }
    }
    BufferedWriter(const BufferedWriter &) = delete;
    BufferedWriter& operator=(BufferedWriter const&) = delete;

    ~BufferedWriter() {
        if (mBuffer != NULL) {
            delete[] mBuffer;
        }
    }

    int Write(void *data, size_t datalen) {
        // not initialized
        if (mBuffer == NULL) {
            return -1;
        }

        // invalid data
        if (data == NULL || datalen == 0) {
            return -1;
        }

        if (!mFlag) {
            // non-buffered mode
            Flush(data, datalen);
        }
        else {
            // buffered mode
            char *cmd = (char *)data;
            size_t i = 0;
            while (i < datalen) {
                bool flush = false;
                char ch = cmd[i++];
                mBuffer[mPos++] = ch;
                flush = (ch == '\r' || ch == '\n');
                if (i < datalen && ch == '\r' && cmd[i] == '\n') {
                    if (mPos < MAX_CMD_BUF_SIZE) {
                        mBuffer[mPos++] = cmd[i++];
                    }
                    else {
                        i++;
                    }
                }
                mBuffer[mPos] = 0;

                if (mPos == MAX_CMD_BUF_SIZE) {
                    flush = true;
                }

                if (flush) {
                    Flush();
                }
            }
        }
        return datalen;
    }

    void Flush() {
        if (mBuffer != NULL && mPos > 0) {
            LOGV("BufferedWriter : Flush %zu byte(s)", mPos);
            ModemProxy::GetInstance()->Write(mBuffer, mPos);
            memset(mBuffer, 0, MAX_CMD_BUF_SIZE + 1);
            mPos = 0;
        }
        else {
            LOGE("BufferedWriter : No buffered data");
        }
    }

    int Flush(void *data, size_t datalen) {
        LOGV("BufferedWriter : write %zu byte(s)", datalen);
        return ModemProxy::GetInstance()->Write(data, datalen);
    }
};

class UsbAgent : public HostAgent, public Runnable {
protected:
    Thread *mThread;
    int mFd;
    int mPipe[2];
    bool Init();
    bool InitPipe();
    void ClosePipe();
    bool InitUsb();
    void CloseUsb();

protected:
    int BufferedInput(void *data, size_t datalen);
public:
    const char *DEV = "/dev/ttyGS0";

    UsbAgent(HostAgentManager *manager) : HostAgent(manager), mThread(NULL), mFd(-1) {
        mName = "ttyGS0";
        memset(mPipe, -1, sizeof(mPipe));
        Init();
    }
    UsbAgent(const UsbAgent &) = delete;
    UsbAgent& operator=(UsbAgent const&) = delete;
    virtual ~UsbAgent();
    int Write(void *data, size_t datalen);
    void Reset();
    void Run();
    void OnFlush(void *data, size_t datalen);
};

UsbAgent::~UsbAgent()
{
    CloseUsb();
    ClosePipe();
}

bool UsbAgent::Init()
{
    LOGD("UsbAgent : Init");
    InitPipe();

    mThread = new Thread(this);
    if (mThread == NULL || mThread->Start() < 0) {
        return false;
    }
    return true;
}

bool UsbAgent::InitPipe()
{
    if (pipe2(mPipe, O_CLOEXEC) == 0) {
        return true;
    }
    return false;
}

void UsbAgent::ClosePipe()
{
    if (mPipe[0] > 0) {
        close(mPipe[0]);
    }
    if (mPipe[1] > 0) {
        close(mPipe[1]);
    }
}

bool UsbAgent::InitUsb()
{
    LOGD("UsbAgent : InitUsb");
    struct termios usb_termios;
    if (mFd > 0) {
        // already opened
        return true;
    }

    int retry = 10;
    while(--retry > 0) {
        mFd = open(DEV, O_RDWR | O_CLOEXEC);
        if (mFd < 0) {
            usleep(500000);
            continue;
        } else {
            break;
        }
    }
    LOGD("UsbAgent : %s fd=%d", DEV, mFd);

    //set params
    memset((char*)&usb_termios, 0, sizeof(struct termios));

    if (tcgetattr(mFd, &usb_termios) < 0) {
        return false;
    }

    cfmakeraw(&usb_termios);
    usb_termios.c_iflag &= ~ICRNL;
    usb_termios.c_iflag &= ~INLCR;
    usb_termios.c_oflag &= ~OCRNL;
    usb_termios.c_oflag &= ~ONLCR;
    usb_termios.c_lflag &= ~ICANON;
    usb_termios.c_lflag &= ~ECHO;

    if (tcsetattr(mFd, TCSANOW, &usb_termios) <0) {
        return false;
    }
    return true;
}

void UsbAgent::CloseUsb()
{
    LOGD("UsbAgent : CloseUsb");
    if (mFd > 0) {
        close(mFd);
    }
    mFd = -1;
}

void UsbAgent::OnFlush(void *data, size_t datalen)
{
    LOGD("UsbAgent : OnFlush");
    ModemProxy::GetInstance()->Write(data, datalen);
}

void UsbAgent::Run()
{
    fd_set rfds;
    const int MAX_BUF_SIZE = (64*1024);
    char *buffer = new char[MAX_BUF_SIZE];
    if (buffer == NULL) {
        // no memory
        return ;
    }

    bool flag = SystemProperty::GetInt("persist.vendor.radio.dun_settings", 0) ||
                SystemProperty::GetInt("persist.vendor.config.buffered_input", 0);
    LOGD("UsbAgent : Buffered mode: %d", flag);
    BufferedWriter out(flag);

    LOGD("Enter %s ", __FUNCTION__);
    while(true) {
        InitUsb();
        int fds[2] = { mFd, mPipe[0] };

        while (true) {
            int maxFd = -1;
            FD_ZERO(&rfds);
            for (int i = 0; i < 2; i++) {
                FD_SET(fds[i], &rfds);
                if (maxFd < fds[i]) {
                    maxFd = fds[i];
                }
            } // end for i ~

            int n = select(maxFd + 1, &rfds, NULL, NULL, NULL);
            if (n < 0) {
                if (errno == EINTR)
                    continue;
                LOGE("UsbAgent : select err = %d", errno);
                break;
            }

            if (FD_ISSET(mFd, &rfds)) {
                int r = read(mFd, buffer, MAX_BUF_SIZE - 1);
                if (r < 0) {
                    if (errno == EINTR)
                        continue;
                    LOGE("%s : read err", __func__);
                    break;
                } else if (r == 0) { // status unpluged
                    LOGE("UsbAgent : read buffer count : 0");
                    LOGE("Warning! USB connection may be unplugged.");
                    break;
                }

                buffer[r] = 0;
                LOGV("UsbAgent : fd=%d read %d byte(s)", mFd, r);
                mManager->SetActiveAgent(this);
                PrintATCommands("UsbAgent", (char *)buffer, r);
                out.Write(buffer, r);
                // echo only if buffered mode
                if (flag) {
                    this->Write(buffer, r);
                }
            }
            else if (FD_ISSET(mPipe[0], &rfds)) {
                // reset
                break;
            }
        } // end for ~
        sleep(1);
        CloseUsb();
    } // end while ~

    if (buffer != NULL) {
        delete[] buffer;
    }
}

int UsbAgent::Write(void *data, size_t datalen)
{
    int fd = mFd;
    if (fd < 0) {
        ALOGE("UsbAgent : write fail. fd is not initialized.");
        return -1;
    }

    if (data == NULL || datalen == 0) {
        return -1;
    }

    // write data
    while (write(mFd, data, datalen) < 0) {
        if (errno == EINTR || errno == EAGAIN)
            continue;
        break;
    } // end while ~

    LOGV("UsbAgent : Write fd=%d %zu byte(s)", mFd, datalen);
    return datalen;
}

void UsbAgent::Reset()
{
    int fd = mPipe[1];
    if (fd > 0) {
        // wakeup thread
        while (write(fd, " ", 1) < 0) {
            if (errno == EINTR || errno == EAGAIN)
                continue;
            break;
        } // end while ~
    }
    LOGV("UsbAgent : Reset USB");
}

class SocketAgent : public HostAgent, public Runnable {
    const int DEFAULT_PORT = 60002;
private:
    int mServerSock;
    int mClientSock;
    int mPort;
    Thread *mThread;
protected:
    bool Init();
    bool InitSocket();
    void CloseSocket();
    void LoadConfig() {
        mPort = SystemProperty::GetInt("persist.vendor.config.at_server_port", DEFAULT_PORT);
    }

public:
    SocketAgent(HostAgentManager *manager) : HostAgent(manager), mThread(NULL) {
        mName = "socket";
        mServerSock = -1;
        mClientSock = -1;
        mPort = 0;
        Init();
    }
    virtual ~SocketAgent() {}
    int Write(void *data, size_t datalen);
    void Run();
};

bool SocketAgent::Init()
{
    LOGD("SocketAgent : Init");
    LoadConfig();
    mThread = new Thread(this);
    if (mThread == NULL || mThread->Start() < 0) {
        return false;
    }
    return true;
}

bool SocketAgent::InitSocket()
{
    LOGD("SocketAgent : InitSocket");
    mServerSock = socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC, 0);
    if (mServerSock < 0) {
        LOGE("SocketAgent : socket error. errno=%d", errno);
        return false;
    }
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(struct sockaddr_in));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(mPort);
    if (::bind(mServerSock, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr_in)) < 0) {
        LOGE("SocketAgent : Fail to bind socket. errno=%d", errno);
        close(mServerSock);
        return false;
    }

    if (listen(mServerSock, 1) < 0) {
        LOGE("SocketAgent : Fail to listen. errno=%d", errno);
        close(mServerSock);
        return false;
    }

    LOGV("SocketAgent : bind and listen. port number: %d", mPort);
    return true;
}

void SocketAgent::CloseSocket()
{
    if (mServerSock > 0) {
        close(mServerSock);
    }
    mServerSock = -1;

    if (mClientSock > 0) {
        close(mClientSock);
    }
    mClientSock = -1;
}

int SocketAgent::Write(void *data, size_t datalen)
{
    if (mClientSock < 0) {
        return -1;
    }

    size_t sent = 0;
    while (sent < datalen) {
        int ret = send(mClientSock, (char *)data + sent, (datalen - sent), 0);
        if (ret < 0) {
            if (errno == EINTR || errno == EAGAIN)
                continue;
            break;
        }
        sent += (size_t)ret;
    } // end while ~

    LOGV("SocketAgent : Write fd=%d %zu byte(s)", mClientSock, sent);
    return (int)sent;
}

void SocketAgent::Run()
{
    ALOGD("SocketAgent Run");

    const int MAX_BUF_SIZE = (64*1024);
    char *buffer = new char[MAX_BUF_SIZE];
    if (buffer == NULL) {
        // no memory
        return ;
    }

    BufferedWriter out;
    while (true) {
        if (!InitSocket()) {
            break;
        }

        while (true) {
            struct sockaddr_in client_addr;
            socklen_t addrlen = sizeof(client_addr);
            if ((mClientSock = accept4(mServerSock, (struct sockaddr *)&client_addr, &addrlen, SOCK_CLOEXEC)) < 0) {
                LOGE("SocketAgent : Fail to accept. errno=%d", errno);
                break;;
            }
            ALOGD("SocketAgent : accept fd=%d", mClientSock);

            while (true) {
                fd_set readFds;
                FD_ZERO(&readFds);
                FD_SET(mClientSock, &readFds);
                int n = select(mClientSock + 1, &readFds, NULL, NULL, NULL);
                if (n < 0) {
                    if (errno == EINTR)
                        continue;
                    LOGE("SocketAgent : select err=%d", errno);
                    break;
                }

                if (FD_ISSET(mClientSock, &readFds)) {
                    int r = recv(mClientSock, buffer, MAX_BUF_SIZE - 1, 0);
                    if (r < 0) {
                        if (errno == EINTR || errno == EAGAIN) {
                            continue;
                        }
                        LOGE("SocketAgent : recv fail. errnor=%d", errno);
                        break;
                    }
                    else if (r == 0) {
                        // EOF
                        LOGD("SocketAgent : Disconnected by peer");
                        break;
                    }

                    LOGV("SocketAgent : fd=%d read %d byte(s)", mClientSock, r);
                    mManager->SetActiveAgent(this);
                    PrintATCommands("SocketAgent", (char *)buffer, r);
                    out.Write(buffer, r);
                }
            } // end while ~ (reader)

            if (mClientSock > 0) {
                close(mClientSock);
            }
        } // end while ~
        CloseSocket();
    } // end for ~ (server socket)

    if (buffer != NULL) {
        delete[] buffer;
    }
}

/**
 * Implement HostAgentManager
 */
HostAgentManager *HostAgentManager::instance = NULL;
HostAgentManager *HostAgentManager::GetInstance()
{
    return instance;
}

void HostAgentManager::Init()
{
    if (instance == NULL) {
        instance = new HostAgentManager();
        if (instance != NULL) {
            bool useTty = SystemProperty::GetInt("persist.vendor.config.ttygs0", 0) == 0;
            if (useTty) {
                instance->RegisterAgent(new UsbAgent(instance));
            } else {
                LOGE("****************************************************");
                LOGE("HostAgentManager : UsbAgent not registered by config");
                LOGE("****************************************************");
            }
            bool tcpPort = SystemProperty::GetInt("persist.vendor.config.tcp.port.disable", 1) == 0;
            if (tcpPort) {
                instance->RegisterAgent(new SocketAgent(instance));
            } else {
                LOGE("****************************************************");
                LOGE("HostAgentManager : SocketAgent not registered by config");
                LOGE("****************************************************");
            }
        }
    }
}

HostAgentManager::HostAgentManager() : mActiveAgent(NULL)
{

}

void HostAgentManager::RegisterAgent(HostAgent *agent)
{
    if (agent != NULL) {
        mHostAgents.push_back(agent);
        if (mHostAgents.size() == 1) {
            SetActiveAgent(agent);
        }
    }
}
