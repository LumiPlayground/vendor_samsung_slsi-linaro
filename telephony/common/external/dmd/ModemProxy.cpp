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
 * ModemProxy.cpp
 *
 *  Created on: 2020. 2. 12.
 */
#include "ModemProxy.h"
#include "base/log.h"
#include "base/thread.h"
#include "base/systemproperty.h"
#include "dmd_test_util.h"
#include "sdmdocenv.h"
#include <errno.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <string.h>

static bool MODEM_VDBG = SystemProperty::GetInt("persist.vendor.config.diag_modem_vdbg") == 1;
static bool HOST_RAW_DUMP = (SystemProperty::GetInt("persist.vendor.config.diag_raw_dump", 0) & 0x2) > 0;
static bool MODEM_RAW_DUMP = (SystemProperty::GetInt("persist.vendor.config.diag_raw_dump", 0) & 0x1) > 0;

enum ExtraConstant {
    defaultCapacity = 1024,
    maxCapacity = (defaultCapacity * 128),
};

Extra::Extra() : mData(nullptr), mSize(0),
        mCapacity(0) {
    realloc(defaultCapacity);
}

Extra::Extra(const Extra& other) : mData(nullptr), mSize(0),
        mCapacity(other.mCapacity) {
    realloc(defaultCapacity);
    setData(other.mData, other.mSize);
}

Extra::~Extra() {
    reset();
}

void Extra::realloc(size_t capacity, bool keep/* = false*/) {
    if (capacity == 0 || (mData && mCapacity == capacity)) {
        return ;
    }

    char *p = new char[capacity] {};
    if (p) {
        if (keep && mData && mSize > 0) {
            if (mSize > capacity) {
                mSize = capacity;
            }
            memcpy(p, mData, mSize);
        } else {
            mSize = 0;
        }

        if (mData) {
            delete[] (char *)mData;
        }

        mData = p;
        mCapacity = capacity;
    }
}

Extra& Extra::setData(const void *data, size_t datalen) {
    if (data != nullptr && datalen > 0) {
        bool needToRealloc = false;
        size_t capacity = mCapacity;
        while (capacity < datalen) {
            if (capacity == 0) {
                capacity = ExtraConstant::defaultCapacity;
            } else {
                capacity *= 2;
            }
            needToRealloc = true;
        }  // end while ~
        if (needToRealloc) {
            realloc(capacity);
        }
        if (mData) {
            memcpy(mData, data, datalen);
            mSize = datalen;
        }
    }
    return *this;
}

Extra& Extra::append(const void *data, size_t datalen) {
    if (data != nullptr && datalen > 0) {
        bool needToRealloc = false;
        size_t capacity = mCapacity;
        while (capacity < datalen + mSize) {
            if (capacity == 0) {
                capacity = ExtraConstant::defaultCapacity;
            } else {
                capacity *= 2;
            }
            needToRealloc = true;
        }  // end while ~
        if (needToRealloc) {
            realloc(capacity, true);
        }
        if (mData) {
            memcpy((char *)mData + mSize, data, datalen);
            mSize += datalen;
        }
    }
    return *this;
}

void Extra::reset() {
    if (mData) {
        delete[] (char *)mData;
    }
    mData = nullptr;
    mSize = 0;
    mCapacity = 0;
}

/**
 * ActiveModemImpl
 */
struct ActiveModemImpl : public ActiveModem, public Runnable {
    static const char *PATH_MODEM;
    static const char *TAG;
    static const size_t MAX_READ_BUF = 65536;
    ModemDataReceiver& mReceiver;
    int mFd;
    Thread mThread;
    ActiveModemImpl(ModemDataReceiver& receiver) : mReceiver(receiver),
            mFd(-1), mThread(this) {
    }
    ~ActiveModemImpl() {
        interrupt();
    }

    int write(const void *data, size_t datalen) override;
    void Run() override;

    bool open();
    void close();
    void start() { mThread.Start(); }
    void interrupt() { /* TODO */ }
    static ActiveModem *newInstance(ModemDataReceiver& receiver);

    static inline void dumpHostRawPackets(const void *data, size_t datalen) {
        std::string filepath = SdmDocEnv::getBaseDir();
        filepath += "host_packets.raw";
        dumpRawPackets(filepath, data, datalen);
    }

    static inline void dumpModemRawPackets(const void *data, size_t datalen) {
        std::string filepath = SdmDocEnv::getBaseDir();
        filepath += "host_packets.raw";
        dumpRawPackets(filepath, data, datalen);
    }

    static void dumpRawPackets(const std::string& filepath, const void *data, size_t datalen) {
        static char buf[1024] = {};
        if (data && datalen > 0) {
            FILE *dumpfile = fopen(filepath.c_str(), "abe");
            if(NULL != dumpfile) {
                struct tm *capT;
                struct timeval val;
                gettimeofday(&val, NULL);
                capT = localtime(&val.tv_sec);

                snprintf(buf, sizeof(buf)-1, "%02d-%02d %02d:%02d:%02d.%06ld\t%d\t%d\n",
                        capT->tm_mon+1, capT->tm_mday, capT->tm_hour, capT->tm_min, capT->tm_sec, val.tv_usec, getpid(), gettid());
                fwrite(buf, 1, strlen(buf), dumpfile);
                std::string rawStr = ::toHexdump(data, datalen) + "\n";
                fwrite(rawStr.c_str(), 1, rawStr.length(), dumpfile);
                fclose(dumpfile);
                dumpfile=NULL;
            } else {
                LOGD("----------Opening dumpfile Fail-----\n");
            }
        }
    }
};

const char *ActiveModemImpl::PATH_MODEM = "/dev/umts_dm0";
const char *ActiveModemImpl::TAG = "ActiveModem";

ActiveModem *ActiveModemImpl::newInstance(ModemDataReceiver& receiver) {
    ActiveModemImpl *inst = new ActiveModemImpl(receiver);
    if (inst) {
        inst->start();
    }
    return inst;
}

bool ActiveModemImpl::open() {
    int n = 10;
    while(n-- > 0) {
        mFd = ::open(PATH_MODEM, O_RDWR | O_NONBLOCK | O_NOCTTY | O_CLOEXEC);
        if (mFd < 0) {
            LOGE("%s : Fail to open %s, errno = %d", __func__, PATH_MODEM, errno);
            sleep(1);
            continue;
        } else {
            break;
        }
    }
    LOGD("%s %s : mFd=%d", TAG, __FUNCTION__, mFd);
    return (mFd >= 0);
}

void ActiveModemImpl::close() {
    if (mFd != -1) {
        ::close(mFd);
        mFd = -1;
    }
}

int ActiveModemImpl::write(const void *data, size_t datalen) {
    if (mFd < 0) {
        return -1;
    }

    if (data == nullptr || datalen > INT_MAX) {
        return 0;
    }

    int w = 0;
    int fd = mFd;
    //LOGV("%s : write %zu byte(s)...", TAG, datalen);
    while ((w = ::write(fd, data, datalen)) < 0) {
        LOGE("%s : write Error(%s)", TAG, strerror(errno));
        if (errno == EINTR || errno == EAGAIN)
            continue;
        return -1;
    }
    if (MODEM_VDBG) {
        LOGV("%s : fd=%d write to modem %d byte(s)", TAG, fd, w);

        if (HOST_RAW_DUMP) {
            // TODO dump into filesystem
        }
    }
    return w;
}

void ActiveModemImpl::Run() {
    char buffer[MAX_READ_BUF] = {};
    while(1) {
        if (mFd < 0) {
            if (!this->open() || mFd < 0) {
                LOGE("%s : Invalid mFd. thread exit", TAG);
                break;
            }
        }
        int fd = mFd;
        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(fd, &rfds);
        int n = select(fd + 1, &rfds, NULL, NULL, NULL);
        //LOGV("%s : after select", TAG);

        if (n < 0) {
            if (errno == EINTR)
                continue;
            LOGE("%s : select err = %d", TAG, errno);
            continue;
        }

        if (FD_ISSET(fd, &rfds)) {
            n = read(fd, &buffer, MAX_READ_BUF - 1);
            if (n < 0) {
                if (errno == EINTR)
                    continue;
                LOGE("%s : read err", TAG);
                continue;
            } else if (n == 0) {  // status unpluged
                LOGE("%s : EOF", TAG);
                this->close();
                this->open();
                continue;
            }

            if (MODEM_VDBG) {
                LOGV("%s: fd=%d read %d byte(s)", TAG, fd, n);

                if (MODEM_RAW_DUMP) {
                    // TODO dump into filesystem
                }
            }
            mReceiver.onReceive(buffer, (size_t)n);
        }
    }
}

/**
 * ModemStateMonitorImpl
 */
#define IOCTL_MODEM_STATUS      _IO('o', 0x27)

struct ModemStateMonitorImpl : public ModemStateMonitor, public Runnable {
    using ModemStateMonitor::State;

    static const char *PATH_BOOT;
    static const char *TAG;
    ModemStateLinstener& mListener;
    int mState;
    bool mSuspend;
    int mFd;
    Thread mThread;
    bool mRunnable = true;
    ModemStateMonitorImpl(ModemStateLinstener& listener) : mListener(listener),
            mState(-1), mSuspend(true), mFd(-1), mThread(this) {
        suspend();
    }
    ~ModemStateMonitorImpl() {
        interrupt();
    }
    void suspend() override {
        LOGD("%s suspend monitoring", TAG);
        mSuspend = true;
    }
    void resume() override {
        LOGD("%s resume monitoring", TAG);
        mSuspend = false;
    }
    void Run() override;
    bool open();
    void close();
    void start() { mThread.Start(); }
    void interrupt() { /* TODO */ }
    int getCurrentState();
    void updateState(int state, bool force = false) {
        int oldState = mState;
        mState = state;
        if (mState != oldState || force) {
            LOGD("%s : modem status [%d]", TAG, mState);
            mListener.onModemStateChanged(mState);
        }
    }
    inline static bool isModemCrash(int state) {
        return (state == STATE_CRASH_RESET) ||
               (state == STATE_CRASH_EXIT) ||
               (state == STATE_CRASH_WATCHDOG);
    }

    inline static bool isUnknownState(int state) {
        return !((state == STATE_OFFLINE) ||
                 (state == STATE_BOOTING) ||
                 (state == STATE_ONLINE) ||
                 isModemCrash(state));
    }

    static ModemStateMonitor *newInstance(ModemStateLinstener& listener);
};
const char *ModemStateMonitorImpl::PATH_BOOT = "/dev/umts_boot0";
const char *ModemStateMonitorImpl::TAG = "ModemStateMonitor";

ModemStateMonitor *ModemStateMonitorImpl::newInstance(ModemStateLinstener& listener) {
    ModemStateMonitorImpl *inst = new ModemStateMonitorImpl(listener);
    if (inst) {
        inst->start();
    }
    return inst;
}

bool ModemStateMonitorImpl::open() {
    if (mFd < 0) {
        mFd = ::open(PATH_BOOT, O_RDWR | O_CLOEXEC);
        if (mFd < 0) {
            LOGE("%s open failure. errno=%d", TAG, errno);
        }
    }
    return mFd >= 0;
}

void ModemStateMonitorImpl::close() {
    if (mFd >= 0) {
        ::close(mFd);
        mFd = -1;
    }
}

int ModemStateMonitorImpl::getCurrentState() {
    int state = -1;
    if (mFd < 0) {
        this->open();
    }

    if (mFd >= 0) {
        state = ioctl(mFd, IOCTL_MODEM_STATUS);
    }
    return state;
}

void ModemStateMonitorImpl::Run() {
    LOGD("%s : start!!!", TAG);
    mRunnable = false;
    if (this->open()) {
        mRunnable = true;
    }

    while(mRunnable) {
        struct pollfd pollfd = {};
        pollfd.fd = mFd;
        pollfd.events = POLLHUP | POLLIN | POLLRDNORM;
        pollfd.revents = 0;
        int ret = poll(&pollfd, 1, mState < 0 ? 0 : -1);
        if (mState < 0 || ret == 0 ||
            (ret > 0 && ((pollfd.revents & POLLHUP) || (pollfd.revents & POLLIN) || (pollfd.revents & POLLRDNORM)))) {
            while (true) {
                updateState(getCurrentState());
                if(mSuspend) {
                    LOGV("%s : A change of modem status is sensed, but suspended.", TAG);
                    sleep(2);
                    break;
                }

                if (mState == STATE_OFFLINE || mState == STATE_BOOTING) {
                    int spin = 600;
                    while (spin--) {
                        int state = getCurrentState();
                        if (state == STATE_ONLINE) {
                            updateState(state, true);
                            break;
                        }
                        usleep(100 * 1000);
                    }

                    if (spin < 0) {
                        LOGE("%s : Modem boot timeout", TAG);
                    }
                }

                if (isModemCrash(mState)) {
                    const unsigned int interval = 2;
                    LOGD("%s : Modem  CRASH!!![%d]", TAG, mState);
                    LOGD("%s : Check the state again after %d seconds later.", TAG, interval);
                    sleep(interval);
                    continue;
                }

                if (mState == STATE_ONLINE) {
                    LOGD("%s : Modem is ONLINE", TAG);
                } else if (isUnknownState(mState)) {
                    // none of state among booting, online and crash
                    sleep(2);
                }
                break;
            }  // end while ~
        } else {

        }
    }
    this->close();
}

/**
 * ModemProxy
 */
ModemProxy::ModemProxy() {
    mModemDataReceiver = nullptr;
    mModemStateListener = nullptr;
    mActiveModem = nullptr;
    mMonitor = nullptr;
}

ModemProxy::~ModemProxy() {
    if (mActiveModem) {
        delete mActiveModem;
    }
}

void ModemProxy::Init() {
    if (mActiveModem == nullptr) {
        mActiveModem = ActiveModemImpl::newInstance(*this);
    }

    if (mMonitor == nullptr) {
        mMonitor = ModemStateMonitorImpl::newInstance(*this);
    }
}

int ModemProxy::Write(const void *data, size_t datalen) {
    if (mActiveModem == nullptr) {
        return -1;
    }

    if (data == NULL || datalen == 0) {
        return 0;
    }

    return mActiveModem->write(data, datalen);
}

int ModemProxy::WriteSafely(const void *data, size_t datalen) {
    //LOGV("ModemProxy::WriteSafely");
    if (mActiveModem == nullptr) {
        return -1;
    }

    if (data == NULL || datalen == 0 || datalen > INT_MAX) {
        return -1;
    }

    char *p = (char *)data;
    size_t size = datalen;
    if (mExtra.size() > 0) {
        ALOGD("%s Extra remained=%zu", __FUNCTION__, mExtra.size());
        p = (char *)mExtra.append(data, datalen).data();
        size = mExtra.size();
    }

    if (MODEM_VDBG) {
        LOGV("WriteSafely: total %zu byte(s)",  datalen);
    }
    size_t cur = 0;
    size_t remain = 0;
    while (cur < size) {
        while (true) {
            if (p[cur] == 0x7F)
                break;
            cur++;
            if (cur >= size) {
                ALOGE("%s : Cannot find a start flag.", __FUNCTION__);
                return 0;
            }
        }

        remain = size - cur;
        if (remain < 3) {
            LOGE("%s : remained data size < 3", __FUNCTION__);
            mExtra.setData(p + cur, remain);
            return remain;
        }

        size_t msg_len = (p[cur + 1] | (p[cur + 2] << 8)) & 0xFFFF;
        if ((cur + msg_len + 1) > size) {
            LOGE("%s : msg_len(%zu) is over datalen(%zu). remain=%zu",
                    __FUNCTION__, msg_len, size, remain);
            mExtra.setData(p + cur, remain);
            return remain;
        }

        if (p[cur + msg_len + 1] != 0x7E) {
            LOGE("%s : Cannot find an end flag. Drop the bad packet.", __FUNCTION__);
            cur = cur + msg_len + 2;
            continue;
        }

        size_t to_send = msg_len + 2;
        if (mActiveModem->write(p + cur, to_send) < 0) {
            LOGE("%s : Write Error(%s)", __FUNCTION__, strerror(errno));
            mExtra.reset();
            return -1;
        }
        cur += to_send;
    }  // end while ~
    mExtra.reset();
    return 0;
}

void ModemProxy::StopForcibly() {
    if (mActiveModem) {
        char REQUEST_STOP[] = {0x7F, 0x0a, 0x00, 0x00, 0x07, 0x00, 0x00, 0xFF, 0xa0, 0x00, 0x02, 0x7E};
        mActiveModem->write(REQUEST_STOP, sizeof(REQUEST_STOP));
    }
}

static ModemProxy sInst;
ModemProxy *ModemProxy::GetInstance() {
    return &sInst;
}
