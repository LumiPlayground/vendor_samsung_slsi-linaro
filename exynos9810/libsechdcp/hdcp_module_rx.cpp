/*
 * Copyright (c) 2013 Samsung Electronics Co., Ltd.
 */

#define LOG_NDEBUG 0
#define LOG_TAG "ExynosHDCPModuleRx"
#include <utils/Log.h>

#include <stdio.h>
#include <stdlib.h>
#include <utils/RefBase.h>
#include <media/stagefright/foundation/AMessage.h>
#include <media/stagefright/foundation/ABuffer.h>
#include <media/stagefright/foundation/ADebug.h>
#include <ion/ion.h>
#include <exynos_ion.h>

#include "hdcp_module_rx.h"
#include "hdcp_rx.h"

#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

using namespace android;

#ifdef __cplusplus
extern "C" {
#endif

#define SMEM_PATH	"/dev/s5p-smem"
#define SECMEM_IOC_GET_FD_PHYS_ADDR _IOWR('S', 8, struct SecMemInfo)
#define STREAM_SIZE (((1920*1080*3)/2)/4)

struct DecryptParam {
    uint32_t streamCTR;
    uint64_t inputCTR;
    const uint8_t *inAddr;
    int inFd;
    uint32_t inSize;
    uint8_t *outAddr;
    int outFd;
    uint32_t outSize;
};

HDCPModule *createHDCPModuleForDecryption(
        void *cookie,
        HDCPModule::ObserverFunc observerNotify)
{
    HDCPModule* hdcpModule = NULL;
    hdcpModule = new ExynosHDCPModuleRx(cookie, observerNotify);
    return hdcpModule;
}

ExynosHDCPModuleRx::ExynosHDCPModuleRx(
        void *cookie,
        ObserverFunc observerNotify)
    :  HDCPModule(cookie, observerNotify)
{
    ALOGI("ExynosHDCPModuleRx()");

    int err = 0;

    mSmemFd = open(SMEM_PATH, O_RDWR);
    if (mSmemFd < 0) {
        ALOGE("open S5P-MEM device error");
    }

    mIonFd = ion_open();
    if (mIonFd <= 0) {
        ALOGE("ion_client_create() error");
    }

    err = ion_alloc_fd(mIonFd, STREAM_SIZE, 0, ION_HEAP_EXYNOS_CONTIG_MASK, 0, &mSecMemIn.mFd);
    if (err) {
        ALOGE("ion_alloc_fd(mSecMemIn.mFd) error");
    }

    err = ion_alloc_fd(mIonFd, STREAM_SIZE, 0, ION_HEAP_EXYNOS_CONTIG_MASK, 0, &mSecMemOut.mFd);
    if (err) {
        ALOGE("ion_alloc_fd(SecMemOut.mFd) error");
    }

    mInBufVir = mmap(NULL, STREAM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, mSecMemIn.mFd, 0);
    if (mInBufVir == MAP_FAILED) {
        ALOGE("mmap(mSecMemIn.mFd) error");
    }

    mOutBufVir = mmap(NULL, STREAM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, mSecMemOut.mFd, 0);
    if (mOutBufVir == MAP_FAILED) {
        ALOGE("mmap(mSecMemOut.mFd) error");
    }

    err = ioctl(mSmemFd, SECMEM_IOC_GET_FD_PHYS_ADDR, &mSecMemIn);
    if (err < 0) {
        ALOGE("cannot obtain SecMemOut phys addr for %d (err %d)", mSecMemIn.mFd, err);
    }

    err = ioctl(mSmemFd, SECMEM_IOC_GET_FD_PHYS_ADDR, &mSecMemOut);
    if (err < 0) {
        ALOGE("cannot obtain SecMemOut phys addr for %d (err %d)", mSecMemOut.mFd, err);
    }

    m_Cookie = cookie;
    mObserverFunc = observerNotify;

    mLooper = new ALooper;
    mLooper->setName("ExynosHDCPModuleTx_looper");
    mLooper->start(
            false /* runOnCallingThread */,
            false /* canCallJava */,
            PRIORITY_AUDIO);
    mHDCPRx = this;
    mLooper->registerHandler(mHDCPRx);
}

ExynosHDCPModuleRx::~ExynosHDCPModuleRx()
{
    status_t err;
    err = munmap(mInBufVir, STREAM_SIZE);

    if (err < 0) {
        ALOGE("munmap() error");
    }

    err = munmap(mOutBufVir, STREAM_SIZE);
    if (err < 0) {
        ALOGE("munmap() error");
    }

    if (mIonFd > 0) {
        close(mIonFd);
        mIonFd = 0;
    }

    if (mSmemFd > 0) {
        close(mSmemFd);
        mSmemFd = 0;
    }

    ALOGI("~ExynosHDCPModuleRx()");
}

status_t ExynosHDCPModuleRx::initAsync(
        const char *host,
        unsigned port)
{
    sp<AMessage> notify = new AMessage(kWhatInitAsync, this);
    notify->setString("iface", host);
    notify->setInt32("port", port);
    notify->post();
    return OK;
}

status_t ExynosHDCPModuleRx::shutdownAsync()
{
    sp<AMessage> notify = new AMessage(kWhatShutdownAsync, this);
    notify->post();
    return OK;
}

status_t ExynosHDCPModuleRx::decrypt(
    const void *inData, size_t size,
    uint32_t streamCTR, uint64_t inputCTR,
    void *outData)
{
    status_t err;
    sp<AMessage> response;
    sp<AMessage> notify = new AMessage(kWhatDecrypt, this);

    DecryptParam decryptParam;
    decryptParam.streamCTR = streamCTR;
    decryptParam.inputCTR = inputCTR;
    decryptParam.inAddr = (uint8_t *)inData;
    decryptParam.inSize = size;
    decryptParam.outAddr = (uint8_t *)outData;
    decryptParam.outSize = size;

    notify->setPointer("DecPar", (void *)&decryptParam);
    err = notify->postAndAwaitResponse(&response);

    if (err != OK) {
        return err;
    }

    if (response == NULL || !response->findInt32("err", &err)) {
        err = OK;
    }
    return err;
}

void ExynosHDCPModuleRx::onMessageReceived(
        const sp<AMessage> &msg) {
    switch (msg->what()) {
        case kWhatInitAsync:
            {
                ALOGI("onMessageReceived(kWhatInitAsync)");
                HDCPResult ret;
                int32_t serverPort;
                CHECK(msg->findInt32("port", &serverPort));
                ALOGI("HDCP_RX_initialize()");
                ret = HDCP_RX_initialize();
                if (ret != HDCP_SUCCESS) {
                    ALOGE("HDCP_RX_initialize failed, ret 0x%x", ret);
                }
                ret = HDCP_RX_Link_open(serverPort, &mLinkHandle);

                ALOGI("HDCP_RX_Link_open() done");
                if (ret != HDCP_SUCCESS) {
                    ALOGE("HDCP_RX_Link_open failed, ret 0x%x", ret);
                }

                ret = HDCP_RX_Link_authenticate(mLinkHandle);
                ALOGI("HDCP_RX_Link_authenticate() done");
                if (ret != HDCP_SUCCESS) {
                    ALOGE("HDCP_RX_Link_authenticate failed, ret 0x%x", ret);
                }

                if (ret == HDCP_SUCCESS)
                    (*mObserverFunc)(m_Cookie, HDCP_INITIALIZATION_COMPLETE, 0, 0);
                else
                    (*mObserverFunc)(m_Cookie, HDCP_INITIALIZATION_FAILED, ret, 0);

                break;
            }

        case kWhatShutdownAsync:
            {
                HDCPResult ret;
                ret = HDCP_RX_Link_close(mLinkHandle);
                if (ret != HDCP_SUCCESS) {
                    ALOGE("HDCP_RX_Link_close failed, ret 0x%x", ret);
                }
                ret = HDCP_RX_terminate();
                if (ret != HDCP_SUCCESS) {
                    ALOGE("HDCP_RX_terminate failed, ret 0x%x", ret);
                }

                if (ret == HDCP_SUCCESS)
                    (*mObserverFunc)(m_Cookie, HDCP_SHUTDOWN_COMPLETE, 0, 0);
                else
                    (*mObserverFunc)(m_Cookie, HDCP_SHUTDOWN_FAILED, ret, 0);

                break;
            }

        case kWhatDecrypt:
            {
                int ret = 0;
                HDCPResult hdcpRet;
                sp<AReplyToken> replyID;
                CHECK(msg->senderAwaitsResponse(&replyID));
                void* pTemp;
                DecryptParam* pEncryptParam;
                uint32_t streamCTR = 0;
                uint64_t inputCTR = 0;
                CHECK(msg->findPointer("DecPar", &pTemp));
                pEncryptParam = (DecryptParam*)pTemp;

                streamCTR = pEncryptParam->streamCTR;
                inputCTR = pEncryptParam->inputCTR;
                ALOGI("decrypt!!!!: size: %u, streamCTR: %u, inputCTR:%llu", pEncryptParam->inSize, streamCTR, inputCTR);

                memcpy(mInBufVir, pEncryptParam->inAddr, pEncryptParam->inSize);

                mPESPrivate[0] = 0x00;
                mPESPrivate[1] = (((streamCTR >> 30) & 3) << 1) | 1;
                mPESPrivate[2] = (streamCTR >> 22) & 0xff;
                mPESPrivate[3] = (((streamCTR >> 15) & 0x7f) << 1) | 1;
                mPESPrivate[4] = (streamCTR >> 7) & 0xff;
                mPESPrivate[5] = ((streamCTR & 0x7f) << 1) | 1;
                mPESPrivate[6] = 0x00;
                mPESPrivate[7] = (((inputCTR >> 60) & 0x0f) << 1) | 1;
                mPESPrivate[8] = (inputCTR >> 52) & 0xff;
                mPESPrivate[9] = (((inputCTR >> 45) & 0x7f) << 1) | 1;
                mPESPrivate[10] = (inputCTR >> 37) & 0xff;
                mPESPrivate[11] = (((inputCTR >> 30) & 0x7f) << 1) | 1;
                mPESPrivate[12] = (inputCTR >> 22) & 0xff;
                mPESPrivate[13] = (((inputCTR >> 15) & 0x7f) << 1) | 1;
                mPESPrivate[14] = (inputCTR >> 7) & 0xff;
                mPESPrivate[15] = ((inputCTR & 0x7f) << 1) | 1;

                hdcpRet = HDCP_RX_Link_decrypt(mLinkHandle,
                        mPESPrivate,
                        sizeof(mPESPrivate),
                        (uint8_t *)mSecMemIn.mPhys,
                        pEncryptParam->inSize,
                        (uint8_t *)mSecMemOut.mPhys,
                        &pEncryptParam->outSize);
                if (hdcpRet != HDCP_SUCCESS) {
                    ALOGE("HDCP_TX_Link_encrypt failed, ret 0x%x", hdcpRet);
                }

                memcpy(pEncryptParam->outAddr, mOutBufVir, pEncryptParam->outSize);

                sp<AMessage> response = new AMessage;
                response->setInt32("err", ret);
                response->postReply(replyID);
                break;
            }
        default:
            ALOGE("onMessageReceived() unsupported command");
    }
}

#ifdef __cplusplus
}
#endif

