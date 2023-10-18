/*
 * Copyright (C) 2013 The Android Open Source Project
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

#ifndef HDCP_MODULE_TX_H_

#define HDCP_MODULE_TX_H_


#include <stdio.h>
#include <stdlib.h>

#include <media/stagefright/foundation/AHandler.h>
#include <media/stagefright/foundation/ALooper.h>

#include <HDCPAPI.h>
#include "hdcp_tx.h"

using namespace android;

#ifdef __cplusplus
extern "C" {
#endif

class ExynosHDCPModuleTx : public HDCPModule, public AHandler {
public:
    ExynosHDCPModuleTx(void *cookie, ObserverFunc observerNotify);
    ~ExynosHDCPModuleTx();

    virtual status_t initAsync(
            const char *host,
            unsigned port);

    virtual status_t shutdownAsync();

    virtual uint32_t getCaps();

    virtual status_t encrypt(
            const void *inData,
            size_t size,
            uint32_t streamCTR,
            uint64_t *outInputCTR,
            void *outData);

    virtual status_t encryptNative(
            buffer_handle_t buffer,
            size_t offset,
            size_t size,
            uint32_t streamCTR,
            uint64_t *outInputCTR,
            void *outData);

protected:
    virtual void onMessageReceived(const sp<AMessage> &msg);

private:
    enum {
        kWhatInitAsync,
        kWhatShutdownAsync,
        kWhatEncrypt,
        kWhatEncryptNative,
    };
    HDCPTxSessionHandle_t mSessionHandle;
    HDCPTxLinkHandle_t mLinkHandle;
    void *m_Cookie;
    ObserverFunc mObserverFunc;
    sp<ALooper> mLooper;
    sp<AHandler> mHDCPTx;
    int mSmemFd;
    int mIonFd;
    int mBufferFd;
    void *mVirtAddr;
};

HDCPModule *createHDCPModule(void *cookie, HDCPModule::ObserverFunc observerNotify);

#ifdef __cplusplus
}
#endif

#endif  // HDCP_MODULE_TX_H_
