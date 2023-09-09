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
 * ModemProxy.h
 *
 *  Created on: 2020. 2. 12.
 */

#ifndef __MODEM_PROXY_H__
#define __MODEM_PROXY_H__

#include <unistd.h>

struct ModemDataReceiver {
    virtual ~ModemDataReceiver()=default;
    virtual void onReceive(const void *data, size_t datalen)=0;
};

struct ModemStateLinstener {
    virtual ~ModemStateLinstener()=default;
    virtual void onModemStateChanged(int state)=0;
    virtual void onSuspended() {}
    virtual void onResume() {}
};

class Extra {
private:
    void *mData;
    size_t mSize;
    size_t mCapacity;

public:
    Extra();
    Extra(const Extra& other);
    Extra& operator=(Extra const&) = delete;
    ~Extra();
    void realloc(size_t capacity, bool keep = false);
    Extra& setData(const void *data, size_t datalen);
    Extra& append(const void *data, size_t datalen);
    void *data() { return mData; }
    size_t size() { return mSize; }
    size_t capacity() { return mCapacity; }
    void reset();
};

/**
 * ActiveModem
 */
struct ActiveModem {
    virtual ~ActiveModem()=default;
    virtual int write(const void *data, size_t datalen)=0;
};

/**
 * ModemStateMonitor
 */
struct ModemStateMonitor {
    enum State {
        STATE_OFFLINE,
        STATE_CRASH_RESET,          /* silent reset */
        STATE_CRASH_EXIT,           /* cp ramdump */
        STATE_BOOTING,
        STATE_ONLINE,
        STATE_CRASH_WATCHDOG = 9,   /* cp watchdog crash */
    };
    virtual ~ModemStateMonitor()=default;
    virtual void suspend()=0;
    virtual void resume()=0;
};

/**
 * ModemProxy
 */
class ModemProxy : public ModemDataReceiver, public ModemStateLinstener {
private:
    Extra mExtra;
    ModemDataReceiver *mModemDataReceiver;
    ModemStateLinstener *mModemStateListener;
    ActiveModem *mActiveModem;
    ModemStateMonitor *mMonitor;

public:
    ModemProxy();
    ModemProxy(const ModemProxy &) = delete;
    ModemProxy& operator=(ModemProxy const&) = delete;
    ~ModemProxy();
    void Init();
    int Write(const void *data, size_t datalen);
    int WriteSafely(const void *data, size_t datalen);
    void StopForcibly();
    void SetModemDataReceiver(ModemDataReceiver *receiver) { mModemDataReceiver = receiver; }
    void SetModemStateLinstener(ModemStateLinstener *listener) {
        mModemStateListener = listener;
        if (mMonitor) {
            if (mModemStateListener) {
                mMonitor->resume();
            } else {
                mMonitor->suspend();
            }
        }
    }

    // callback to the receiver
    void onReceive(const void *data, size_t datalen) {
        if (mModemDataReceiver) mModemDataReceiver->onReceive(data, datalen);
    }

    // callback to the listener
    void onModemStateChanged(int state) {
        if (mModemStateListener) mModemStateListener->onModemStateChanged(state);
    }
public:
    static ModemProxy *GetInstance();
};

#endif /* __MODEM_PROXY_H__ */
