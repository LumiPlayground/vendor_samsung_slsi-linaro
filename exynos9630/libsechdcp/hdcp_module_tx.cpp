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

#define LOG_NDEBUG 0
#define LOG_TAG "HDCPTxAndroid"

#include <utils/RefBase.h>
#include <media/stagefright/foundation/AMessage.h>
#include <media/stagefright/foundation/ADebug.h>
#include "hdcp_module_tx.h"
#include <ion/ion.h>
#include <exynos_ion.h>

#include <fcntl.h>
#include <sys/mman.h>

#define MAX_HEAP_NAME 32

struct ion_heap_data {
    char name[MAX_HEAP_NAME];
    __u32 type;
    __u32 heap_id;
    __u32 size;       /* reserved 0 */
    __u32 heap_flags; /* reserved 1 */
    __u32 reserved2;
};

using namespace android;

#ifdef __cplusplus
extern "C" {
#endif

#define SMEM_PATH   "/dev/s5p-smem"
#define SECMEM_IOC_GET_FD_PHYS_ADDR _IOWR('S', 8, struct SecMemInfo)
#define STREAM_SIZE (((1920*1080*3)/2)/4)
//#define STREAM_SIZE (((3840*2160*3)/2)/4)

/*
 * Build configuration for wfd loopback test(a.k.a. wfd dual mode).
 * It should be defined only when testing wfd dual mode.
 * In normal test case using wfd dongle, it should be undefined.
 */
//#define FEATURE_WFD_DUAL_MODE

struct SecMemInfo {
    int mFd;
    uint64_t mPhys;
};

struct SecMemInfo SecMemIn, SecMemOut;

struct EncryptParam {
    uint8_t *pesPrivate;
    uint32_t strCtr;
    uint64_t inputCtr;
    const uint8_t *inAddr;
    int inFd;
    uint32_t inSize;
    uint8_t *outAddr;
    int outFd;
    uint32_t outSize;
};

HDCPModule *createHDCPModule(
        void *cookie,
        HDCPModule::ObserverFunc observerNotify)
{
    HDCPModule* hdcpModule = NULL;
    hdcpModule = new ExynosHDCPModuleTx(cookie, observerNotify);
    return hdcpModule;
}

ExynosHDCPModuleTx::ExynosHDCPModuleTx(
        void *cookie,
        ObserverFunc observerNotify)
    :  HDCPModule(cookie, observerNotify)
{
    int err = 0;

    mSmemFd = open(SMEM_PATH, O_RDWR);
    if (mSmemFd < 0) {
        ALOGE("open S5P-MEM device error");
    }

    mIonFd = ion_open();
    if (mIonFd <= 0) {
        ALOGE("ion_open() error");
    }

    if (!ion_is_legacy(mIonFd)) {
        int heap_cnt = 0;
        unsigned int heap_id = 0;

        if (ion_query_heap_cnt(mIonFd, &heap_cnt) < 0 || heap_cnt <= 0) {
            ALOGE("fail to query the heap count. heap_cnt: %d", heap_cnt);
        }

        struct ion_heap_data heaps[heap_cnt];

        if (ion_query_get_heaps(mIonFd, heap_cnt, heaps) < 0) {
            ALOGE("fail to query the heaps");
        }

        for (int i = 0; i < heap_cnt; i++) {
            if (strcmp(heaps[i].name, "crypto_heap") == 0)
                heap_id = heaps[i].heap_id;
        }

        ALOGI("libsechdcp heap_id %d", heap_id);

        err = ion_alloc_fd(mIonFd, STREAM_SIZE, 0, 1 << heap_id, 0, &mBufferFd);
        if (err) {
            ALOGE("ion_alloc_fd() error");
        }
    } else {
        err = ion_alloc_fd(mIonFd, STREAM_SIZE, 0, ION_HEAP_EXYNOS_CONTIG_MASK, 0, &mBufferFd);
        if (err) {
            ALOGE("ion_alloc_fd() error");
        }
    }

    mVirtAddr = mmap(NULL, STREAM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, mBufferFd, 0);
    if (mVirtAddr == MAP_FAILED) {
        ALOGE("mmap() error");
    }

    SecMemOut.mFd = mBufferFd;
    err = ioctl(mSmemFd, SECMEM_IOC_GET_FD_PHYS_ADDR, &SecMemOut);
    if (err < 0) {
	    ALOGE("cannot obtain SecMemOut phys addr for %d (err %d)", SecMemOut.mFd, err);
    }

    mLooper = new ALooper;
    mLooper->setName("ExynosHDCPModuleTx_looper");
    mLooper->start(
            false /* runOnCallingThread */,
            false /* canCallJava */,
            PRIORITY_AUDIO);
    mHDCPTxHandler = new HDCPTxHandler(cookie, observerNotify, mSmemFd);
    mLooper->registerHandler(mHDCPTxHandler);
}

ExynosHDCPModuleTx::~ExynosHDCPModuleTx()
{
    status_t err;
    err = munmap(mVirtAddr, STREAM_SIZE);

    ALOGI("~ExynosHDCPModuleTx()");
    if (err < 0) {
        ALOGE("munmap() error");
    }

    if (mLooper != NULL) {
        mLooper->stop();
        mLooper->unregisterHandler(mHDCPTxHandler->id());
    }

    if (mBufferFd > 0) {
        close(mBufferFd);
        mBufferFd = 0;
    }
    if (mIonFd > 0) {
        close(mIonFd);
        mIonFd = 0;
    }

    if (mSmemFd > 0) {
        close(mSmemFd);
        mSmemFd = 0;
    }
}

status_t ExynosHDCPModuleTx::initAsync(
        const char *host,
        unsigned port)
{
    sp<AMessage> notify = new AMessage(HDCPTxHandler::kWhatInitAsync, mHDCPTxHandler);
    notify->setString("iface", host);
    notify->setInt32("port", port);
    notify->post();
    return OK;
}

status_t ExynosHDCPModuleTx::shutdownAsync()
{
    sp<AMessage> notify = new AMessage(HDCPTxHandler::kWhatShutdownAsync, mHDCPTxHandler);
    notify->post();
    return OK;
}

uint32_t ExynosHDCPModuleTx::getCaps()
{
    return HDCPModule::HDCP_CAPS_ENCRYPT_NATIVE | HDCPModule::HDCP_CAPS_ENCRYPT;
}

status_t ExynosHDCPModuleTx::encrypt(
        const void *inData,
        size_t size,
        uint32_t streamCTR,
        uint64_t *inputCTR,
        void *outData)
{
    status_t err;
    sp<AMessage> response;
    sp<AMessage> notify = new AMessage(HDCPTxHandler::kWhatEncrypt, mHDCPTxHandler);

    EncryptParam encryptParam;
    encryptParam.strCtr = streamCTR;
    encryptParam.inAddr = (uint8_t *)inData;
    encryptParam.inSize = size;
    encryptParam.outAddr = (uint8_t *)outData;
    encryptParam.outSize = size;

    notify->setPointer("EncPar", (void *)&encryptParam);
    err = notify->postAndAwaitResponse(&response);

    *inputCTR = encryptParam.inputCtr;

    if (err != OK) {
        return err;
    }

    if (response == NULL || !response->findInt32("err", &err)) {
        err = OK;
    }
    return err;
}

status_t ExynosHDCPModuleTx::encryptNative(
        buffer_handle_t buffer,
        size_t __unused offset,
        size_t __unused size,
        uint32_t streamCTR,
        uint64_t *inputCTR,
        void *outData)
{
    int fd = buffer->data[0];
    size_t dataLeng = buffer->data[2];
    uint8_t HDCP_private_data[16];
    status_t err;
    sp<AMessage> response;
    sp<AMessage> notify = new AMessage(HDCPTxHandler::kWhatEncryptNative, mHDCPTxHandler);

    EncryptParam encryptParam;
    encryptParam.pesPrivate = HDCP_private_data;
    encryptParam.inputCtr = 0;
    encryptParam.strCtr = streamCTR;
    encryptParam.inFd = fd;
    encryptParam.inSize = dataLeng;
    encryptParam.outFd = mBufferFd;
    encryptParam.outSize = dataLeng;

    notify->setPointer("EncPar", (void *)&encryptParam);
    err = notify->postAndAwaitResponse(&response);

    if (err != OK) {
	    return err;
    }

    *inputCTR = encryptParam.inputCtr;
    memcpy(outData, mVirtAddr, dataLeng);

    if (response == NULL || !response->findInt32("err", &err)) {
        err = OK;
    }

    return err;
}

////////////////////////////////////////////////////////////////////////////////

HDCPTxHandler::HDCPTxHandler(
        void *cookie,
        HDCPModule::ObserverFunc observerNotify,
        int smemFd)
    :  m_Cookie(cookie),
    mObserverFunc(observerNotify),
    mSmemFd(smemFd)
{
}

void HDCPTxHandler::onMessageReceived(
        const sp<AMessage> &msg) {
    switch (msg->what()) {
        case kWhatInitAsync:
            {
                int ret = 0;
                int32_t port;
#if !defined(FEATURE_WFD_DUAL_MODE)
                HDCPTxElstream_t els[1];
#endif
                AString iface;
                msg->findString("iface", &iface);
                msg->findInt32("port", &port);

		do {
			ALOGI("HDCP_TX_initialize.");
			ret = HDCP_TX_initialize();
			if (ret != HDCP_SUCCESS){
				ALOGE("HDCP_TX_initialize failed [%x]", ret);
				break;
			}

			ALOGI("HDCP_TX_session_open.");
			ret = HDCP_TX_Session_open(&mSessionHandle);
			if (ret != HDCP_SUCCESS){
				ALOGE("HDCP_TX_Session_open failed [%x]", ret);
				break;
			}

			ALOGI("HDCP_TX_Link_open.");
			ret = HDCP_TX_Link_open(mSessionHandle, (char *)iface.c_str(), port, &mLinkHandle);
			if (ret != HDCP_SUCCESS){
				ALOGE("HDCP_TX_Link_open failed [%x]", ret);
				break;
			}

			ALOGI("HDCP_TX_Link_authenticate.");
			ret = HDCP_TX_Link_authenticate(mLinkHandle);
			if (ret != HDCP_SUCCESS){
				ALOGE("HDCP_TX_Link_authenticate failed [%x]", ret);
				break;
			}

#if !defined(FEATURE_WFD_DUAL_MODE)
			els[0].type = 0x00; /* transmit to all Receiver */
			els[0].stream_pid = 0x1011; /* Video Stream PID */
			els[0].stream_ctr = 0x00000000; /* Stream Counter */
			ALOGI("HDCP_TX_Link_stream_manage.");
			ret = HDCP_TX_Link_stream_manage(mLinkHandle, 1, els);
			if (ret != HDCP_SUCCESS)
				ALOGE("HDCP_TX_Link_stream_manage failed [%x]", ret);
#endif
		} while(0);

		if (ret == HDCP_SUCCESS)
			(*mObserverFunc)(m_Cookie, HDCPModule::HDCP_INITIALIZATION_COMPLETE, 0, 0);
		else
			(*mObserverFunc)(m_Cookie, HDCPModule::HDCP_INITIALIZATION_FAILED, ret, 0);

                break;
            }

        case kWhatShutdownAsync:
            {
                int ret;

                ret = HDCP_TX_Link_close(mLinkHandle);
                if (ret != HDCP_SUCCESS)
                    ALOGE("HDCP_TX_Link_close failed");

                ret = HDCP_TX_Session_close(mSessionHandle);
                if (ret != HDCP_SUCCESS)
                    ALOGE("HDCP_TX_Session_close failed");

                if (ret == HDCP_SUCCESS)
                    (*mObserverFunc)(m_Cookie, HDCPModule::HDCP_SHUTDOWN_COMPLETE, 0, 0);
                else
                    (*mObserverFunc)(m_Cookie, HDCPModule::HDCP_SHUTDOWN_FAILED, ret, 0);

                break;
            }

        case kWhatEncrypt:
            {
                int ret = -1;
                sp<AReplyToken> replyID;
                CHECK(msg->senderAwaitsResponse(&replyID));
                void* pTemp;
                EncryptParam* pEncryptParam;
                CHECK(msg->findPointer("EncPar", &pTemp));
                pEncryptParam = (EncryptParam*)pTemp;

                ALOGE("ExynosHDCPModuleTx don't support HDCP encrypt(vadd)");

                sp<AMessage> response = new AMessage;
                response->setInt32("err", ret);
                response->postReply(replyID);
                break;
            }

        case kWhatEncryptNative:
            {
                int ret;
                HDCPResult hdcpRet;
                sp<AReplyToken> replyID;
                CHECK(msg->senderAwaitsResponse(&replyID));
                void* pTemp;
                EncryptParam* pEncryptParam;
                CHECK(msg->findPointer("EncPar", &pTemp));
                pEncryptParam = (EncryptParam*)pTemp;

                SecMemIn.mFd = pEncryptParam->inFd;

                ret = ioctl(mSmemFd, SECMEM_IOC_GET_FD_PHYS_ADDR, &SecMemIn);
                if (ret < 0) {
                    ALOGE("cannot obtain SecMemIn phys addr for %d (err %d)", SecMemIn.mFd, errno);
                    sp<AMessage> response = new AMessage;
                    response->setInt32("err", ret);
                    response->postReply(replyID);
                    break;
                }

                hdcpRet = HDCP_TX_Link_encrypt(mLinkHandle,
                        (uint8_t *)pEncryptParam->pesPrivate,
                        (uint32_t)sizeof(pEncryptParam->pesPrivate),
                        (uint8_t *)(&(pEncryptParam->strCtr)),
                        (uint8_t *)(&(pEncryptParam->inputCtr)),
                        (uint64_t)SecMemIn.mPhys,
                        pEncryptParam->inSize,
                        (uint64_t)SecMemOut.mPhys,
                        &(pEncryptParam->outSize),
                        SecMemIn.mFd,
                        SecMemOut.mFd);

                ALOGI("HDCP_TX_Link_encrypt done.");
                if (hdcpRet != HDCP_SUCCESS) {
                    ALOGE("HDCP_TX_Link_encrypt failed");
                }

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
