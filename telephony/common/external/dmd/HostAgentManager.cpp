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
#include "base/log.h"
#include "base/systemproperty.h"
#include "base/thread.h"


static bool HOST_VDBG = SystemProperty::GetInt("persist.vendor.config.diag_host_vdbg") == 1;

class UsbAgent : public HostAgent, public Runnable {
protected:
    Thread *mThread;
    int mFd;
    int mWriteFailureCount;
    bool Init();
    bool InitUsb();
    void CloseUsb();
public:
    const char *DEV = "/dev/ttyGS1";
    UsbAgent(HostAgentManager *manager) : HostAgent(manager), mThread(NULL), mFd(-1) {
        mName = "usb";
        mWriteFailureCount = 0;
        Init();
    }
    virtual ~UsbAgent() {}
    int Write(const void *data, size_t datalen);
    void Run();
};

bool UsbAgent::Init()
{
    ALOGD("UsbAgent Init");
    mThread = new Thread(this);
    if (mThread == NULL || mThread->Start() < 0) {
        return false;
    }
    return true;
}

bool UsbAgent::InitUsb()
{
    ALOGD("UsbAgent InitUsb");
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

    if (mFd < 0) {
        LOGE("UsbAgent::InitUsb: Failed to open %s(%d)", DEV, errno);
        return false;
    }

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
    ALOGD("UsbAgent CloseUsb");
    if (mFd > 0) {
        close(mFd);
    }
    mFd = -1;
}

void UsbAgent::Run()
{
    fd_set rfds;
    const int MAX_BUF_SIZE = (64*1024);
    char *buffer = new char[MAX_BUF_SIZE];
    LOGD("Enter %s ", __FUNCTION__);
    while(1) {
        if (!InitUsb()) {
            break;
        }

        int fd = mFd;
        for( ;; ) {
            FD_ZERO(&rfds);
            FD_SET(fd, &rfds);

            int n = select(fd + 1, &rfds, NULL, NULL, NULL);
            //ALOGD("%s : after select", __func__);

            if (n < 0) {
                if (errno == EINTR)
                    continue;
                LOGE("UsbAgent : select err = %d", errno);
                break;
            }

            if (FD_ISSET(fd, &rfds)) {
                int r = read(fd, buffer, MAX_BUF_SIZE);
                if (r < 0) {
                    if (errno == EINTR)
                        continue;
                    ALOGE("%s : read err", __func__);
                    break;
                } else if (r == 0) { // status unpluged
                    LOGE("UsbAgent : read buffer count : 0");
                    LOGE("Warning! USB connection may be unplugged.");
                    break;
                }

                if (HOST_VDBG) {
                    LOGV("UsbAgent : fd=%d read %d byte(s)", fd, r);
                }
                mManager->SetActiveAgent(this);

                mManager->SendToModem(buffer, r);
            }
        } // end for ~
        sleep(1);
        CloseUsb();
    } // end while ~

    if (buffer != NULL) {
        delete[] buffer;
    }
}

int UsbAgent::Write(const void *data, size_t datalen)
{
    int fd = mFd;
    if (fd < 0) {
        ALOGE("UsbAgent : write fail. fd is not initialized.");
        return -1;
    }

    if (data == NULL || datalen == 0) {
        return -1;
    }

    const int TIMEOUT_DM_STOP_REQUEST = (1 * 60 * 1000); // 1 minutes (msec)
    const int TICK_WRITABLE = 500; // 500 msec
    const int MAX_COUNT =  TIMEOUT_DM_STOP_REQUEST / TICK_WRITABLE;

    fd_set wfds;
    FD_ZERO(&wfds);
    FD_SET(fd, &wfds);
    struct timeval tv = { 0, (TICK_WRITABLE * 1000) };  // usec
    int n = select(mFd + 1, NULL, &wfds, NULL, &tv);
    if (n == 0) {
        LOGE("UsbAgent : couldn't write to the fd=%d. DM packet dropped(%zu bytes)", fd, datalen);
        if (mWriteFailureCount++ > MAX_COUNT) {
            // stop DM service
            mWriteFailureCount = 0;
            ALOGI("UsbAgent : Never sent data to DM Host. Stop DM Service forcibly");
            ModemProxy::GetInstance()->StopForcibly();
        }
        return -1;
    }
    else if (n > 0) {
        if (mWriteFailureCount > 0) {
            ALOGI("UsbAgent : Recovered to be writable state. fd=%d", fd);
            mWriteFailureCount = 0;
        }
    }

    // write data
    while (write(mFd, data, datalen) < 0) {
        if (errno == EINTR || errno == EAGAIN)
            continue;
        break;
    } // end while ~

    if (HOST_VDBG) {
        LOGV("UsbAgent : Write fd=%d %zu byte(s)", mFd, datalen);
    }
    return datalen;
}

class SocketAgent : public HostAgent, public Runnable {
    const int DEFAULT_PORT = 50002;
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
        mPort = SystemProperty::GetInt("persist.vendor.config.dm_server_port", DEFAULT_PORT);
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
    int Write(const void *data, size_t datalen);
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

int SocketAgent::Write(const void *data, size_t datalen)
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

    if (HOST_VDBG) {
        LOGV("SocketAgent : Write fd=%d %zu byte(s)", mClientSock, sent);
    }
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
                    int r = recv(mClientSock, buffer, MAX_BUF_SIZE, 0);
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

                    if (HOST_VDBG) {
                        LOGV("SocketAgent : fd=%d read %d byte(s) %d", mClientSock, r, VDBG);
                    }
                    mManager->SetActiveAgent(this);

                    mManager->SendToModem(buffer, r);
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
            instance->RegisterAgent(new UsbAgent(instance));
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

HostAgentManager::HostAgentManager() : mActiveAgent(NULL), mModemProxy(nullptr) {
}

void HostAgentManager::RegisterAgent(HostAgent *agent)
{
    if (agent != NULL) {
        mHostAgents.push_back(agent);
    }
}

void HostAgentManager::SetActiveAgent(HostAgent *agent) {
    if (mActiveAgent != agent) {
        LOGD("[HostAgentManager] Active HostAgent is changed from %s to %s",
                mActiveAgent ? mActiveAgent->getHostName().c_str() : "unknown",
                agent ? agent->getHostName().c_str() : "unknown");
        mActiveAgent = agent;
    }
}

int HostAgentManager::SendToModem(const void *data, size_t datalen) {
    int ret = 0;
    if (mModemProxy) {
        ret = mModemProxy->WriteSafely(data, datalen);
    } else {
        LOGW("[HostAgentManager] No ModemProxy available."
             " HostAgent might not work in the current state. Please check the working mode.");
    }
    return ret;
}
