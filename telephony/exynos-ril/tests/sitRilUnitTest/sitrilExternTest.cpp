/*
 *  SIT RIL Unit test
 *
 *  Copyright Samsung Electronics Co., LTD.
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 */

#include "sitrilExternTest.h"

#define MAX_RETRY (3)
#define SLEEP_RETRY (5000)

#define RIL_CLIENT_LIBPATH_AUDIO "libsitril-audio.so"
#define RIL_CLIENT_LIBPATH_GPS "libsitril-gps.so"
#define RIL_CLIENT_LIBPATH_SE  "libsitril-se.so"
#define RIL_CLIENT_LIBPATH_WLAN "libsitril-wlan.so"

extern "C"
void RILC_onRequestComplete(unsigned int msgId, int /*status*/, void* /*data*/, size_t /*length*/, unsigned int /*channel*/) {
    //printf("[%s] (%d) Rsponse comes, size(%lu)\n", __FUNCTION__, msgId, (unsigned long)length);
    SitrilExternTest *_externTest = SitrilExternTest::GetInstance();
    if (_externTest != NULL) {
        _externTest->setRspMsgId(msgId);
    } else {
        printf("[%s] externTest is null\n", __FUNCTION__);
    }
}

extern "C"
void RILC_onUnsolicitedResponse(unsigned int /*msgId*/, void* /*data*/, size_t /*length*/, unsigned int /*channel*/) {
    //printf("[%s] (%d) Indication comes, size(%lu)\n", __FUNCTION__, msgId, (unsigned long)length);
    printf("[%s] THIS FUNC IS NOT USED\n", __FUNCTION__);
}

extern "C"
void RILC_onRequestSend(unsigned int msgId, void* data, size_t length, unsigned int channel) {
    //printf("[%s] (%d) Request comes, size(%lu)\n", __FUNCTION__, msgId, (unsigned long)length);
    SitrilExternTest *_externTest = SitrilExternTest::GetInstance();
    if (_externTest != NULL) {
        SitrilExternCmdData *reqData = new SitrilExternCmdData(msgId, data, length, channel);
        _externTest->setReqData(reqData);
    } else {
        printf("[%s] externTest is null\n", __FUNCTION__);
    }
}

static struct RILC_EnvUnitTest s_rilcEnvUnitTest;

SitrilExternCmdData::SitrilExternCmdData(unsigned int msgId, void* data, size_t length, unsigned int channel) {
    mMsgId = msgId; mDataLen = length; mChannel = channel;
    if (mDataLen > 0) {
        mData = (char *)malloc(mDataLen);
        memcpy(mData, data, mDataLen);
    } else {
        mData = nullptr;
    }
}

SitrilExternCmdData::~SitrilExternCmdData() {
    if(mData) free(mData);
}

SitrilExternApi::SitrilExternApi() {
    ril_open_client = nullptr;
    ril_close_client = nullptr;
}

SitrilExternTest *SitrilExternTest::mStaticInstance = nullptr;

SitrilExternTest::SitrilExternTest(int clientType, SitrilExternApi *pApi) {
    mHandle = nullptr;
    mConnection = false;
    mpApi = nullptr;
    mpReqData = nullptr;
    mpRspData = nullptr;
    mRspMsgId = mIndMsgId = 0;

    const char *libPath = nullptr;
    switch (clientType) {
        case RIL_EXTERN_AUDIO:
            libPath = RIL_CLIENT_LIBPATH_AUDIO;
            break;
        case RIL_EXTERN_GPS:
            libPath = RIL_CLIENT_LIBPATH_GPS;
            break;
        case RIL_EXTERN_SE:
            libPath = RIL_CLIENT_LIBPATH_SE;
            break;
        case RIL_EXTERN_WLAN:
            libPath = RIL_CLIENT_LIBPATH_WLAN;
            break;
    }

    if (libPath == nullptr) return;

    mHandle = dlopen(libPath, RTLD_NOW);
    mpApi = pApi;
}

SitrilExternTest::~SitrilExternTest() {
    if (mHandle) dlclose(mHandle);
    mHandle = nullptr;
    mConnection = false;
    mpApi = nullptr;
    if (mpReqData) delete mpReqData;
    mpReqData = nullptr;
    if (mpRspData) delete mpRspData;
    mpRspData = nullptr;
}

void *SitrilExternTest::getHandle() {
    return mHandle;
}

void SitrilExternTest::updateConnection(bool isConnect) {
    mConnection = isConnect;
}

bool SitrilExternTest::getConnection() {
    return mConnection;
}

void SitrilExternTest::openClient() {
    s_rilcEnvUnitTest.OnRequestComplete = RILC_onRequestComplete;
    s_rilcEnvUnitTest.OnUnsolicitedResponse = RILC_onUnsolicitedResponse;
    s_rilcEnvUnitTest.OnRequestSend = RILC_onRequestSend;

    if (mpApi != nullptr && mpApi->ril_open_client) {
        int ret = 0;
        for (int retry = 0; retry < MAX_RETRY; retry++) {
            mRilcFuncs = (const RILC_UnitTestFunctions *)mpApi->ril_open_client(&s_rilcEnvUnitTest);
            ret = mRilcFuncs->openStatus;
            if (ret != 0) {
                //printf("%s: Failed to open RILC ! (Try %d)\n", __FUNCTION__, retry+1);
                usleep(SLEEP_RETRY);  // 10ms
            } else {
                //printf("%s: Succeeded in opening RIL AudioClient!\n", __FUNCTION__);
                mConnection = true;
                break;
            }
        }
        if (ret != 0) printf("%s: Failed to open RILC ! (MAX Try %d)\n", __FUNCTION__, MAX_RETRY);
    }
}

int SitrilExternTest::registerCallback() {
    printf("%s: not implemented", __FUNCTION__);
    return 0;
}

void SitrilExternTest::closeClient() {
    if (mConnection == true && mpApi != nullptr && mpApi->ril_close_client) {
        int ret = mpApi->ril_close_client();
        if (ret == 0) {
            //printf("%s: Closed RILC\n", __FUNCTION__);
        } else {
            printf("%s: Failed to close RILC!\n", __FUNCTION__);
        }

        mConnection = false;
    }
}

void SitrilExternTest::resetTestData() {
    if (mpReqData) delete mpReqData;
    mpReqData = nullptr;

    if (mpRspData) delete mpRspData;
    mpRspData = nullptr;

    mRspMsgId = mIndMsgId = 0;
}

void SitrilExternTest::sendRespone(int status, unsigned int msgId, void* data, size_t length, unsigned int channel) {
    SitrilExternCmdData *rspData = new SitrilExternCmdData(msgId, data, length, channel);
    setRspData(rspData);
    if (mRilcFuncs && mRilcFuncs->onRspSend && mpRspData) {
        mRilcFuncs->onRspSend(mpRspData->mMsgId, status, mpRspData->mData, mpRspData->mDataLen, mpRspData->mChannel);
    }
}

void SitrilExternTest::sendIndication(unsigned int msgId, void* data, size_t length, unsigned int channel) {
    SitrilExternCmdData *rspData = new SitrilExternCmdData(msgId, data, length, channel);
    setRspData(rspData);
    if (mRilcFuncs && mRilcFuncs->onIndSend && mpRspData) {
        mRilcFuncs->onIndSend(mpRspData->mMsgId, mpRspData->mData, mpRspData->mDataLen, mpRspData->mChannel);
    }
}
