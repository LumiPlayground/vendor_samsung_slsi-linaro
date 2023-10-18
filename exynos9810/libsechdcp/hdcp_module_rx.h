/*
 * Copyright (c) 2013 Samsung Electronics Co., Ltd.
 */

#ifndef HDCP_TX_ANDROID_H_

#define HDCP_TX_ANDROID_H_


#include <stdio.h>
#include <stdlib.h>

#include <media/stagefright/foundation/AHandler.h>
#include <media/stagefright/foundation/ALooper.h>

#include <HDCPAPI.h>
#include "hdcp_rx.h"

using namespace android;

#ifdef __cplusplus
extern "C" {
#endif

struct SecMemInfo {
    int mFd;
    uint64_t mPhys;
};

class ExynosHDCPModuleRx : public HDCPModule, public AHandler {
public:
    ExynosHDCPModuleRx(void *cookie, ObserverFunc observerNotify);
    ~ExynosHDCPModuleRx();

    virtual status_t initAsync(
            const char *host,
            unsigned port);

    virtual status_t shutdownAsync();

    virtual status_t decrypt(
            const void *inData, size_t size,
            uint32_t streamCTR, uint64_t inputCTR,
            void *outData);

protected:
    virtual void onMessageReceived(const sp<AMessage> &msg);

private:
    enum {
        kWhatInitAsync,
        kWhatShutdownAsync,
        kWhatDecrypt,
    };
    uint8_t mPESPrivate[16];
    HDCPRxLinkHandle_t mLinkHandle;
    void *m_Cookie;
    ObserverFunc mObserverFunc;
    sp<ALooper> mLooper;
    sp<AHandler> mHDCPRx;
    int mSmemFd;
    int mIonFd;
    SecMemInfo mSecMemIn;
    SecMemInfo mSecMemOut;
    void *mInBufVir;
    void *mOutBufVir;

};

HDCPModule *createHDCPModuleForDecryption(void *cookie, HDCPModule::ObserverFunc observerNotify);

#ifdef __cplusplus
}
#endif

#endif  // HDCP_TX_ANDROID_H_
