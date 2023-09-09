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

#include "sitrilTest.h"
#include "testUtil.h"

#include <android/log.h>

#define DBG false
#define  LOG_TAG    "SIT_UNIT_TEST"
#define  LOGI(...) if(DBG) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

#define UNUSED(x) ((void)(x))

extern "C" void
RIL_onRequestComplete(RIL_Token t, RIL_Errno e, void *response, size_t responselen) {
    int requestId = 0;

    if (t != nullptr) {
        RIL_TestToken *pTestToken = (RIL_TestToken *)t;
        requestId = pTestToken->requestId;
    }
    else {
        printf("[%s] t is null ptr, e(%d)\n", __FUNCTION__, e);
    }

    SitrilTest *_sitrilTest = SitrilTest::GetInstance();
    LOGI("RIL_onRequestComplete mSerial:%d, requestId:%d, responselen:%lu",
        _sitrilTest->mSerial, requestId, (unsigned long)responselen);

    if (_sitrilTest != NULL) {
        _sitrilTest->updateRilRspData(e, response, responselen);
        _sitrilTest->notify(requestId);
    } else {
        printf("[%s] sitril is null\n", __FUNCTION__);
    }
}

extern "C"
#if defined(ANDROID_MULTI_SIM)
void RIL_onUnsolicitedResponse(int unsolResponse, const void *data,
                                size_t datalen, RIL_SOCKET_ID /*socket_id*/) {
#else
void RIL_onUnsolicitedResponse(int unsolResponse, const void *data,
                                size_t datalen) {
#endif
    SitrilTest *_sitrilTest = SitrilTest::GetInstance();
    if (_sitrilTest == NULL) {
        printf("[%s] sitril is null\n", __FUNCTION__);
        return;
    }

    LOGI("RIL_onUnsolicitedResponse mSerial:%d, unsolResponse:%d, datalen:%lu",
        _sitrilTest->mSerial, unsolResponse, (unsigned long)datalen);

    if (_sitrilTest->mSerial == unsolResponse) {
        _sitrilTest->updateRilIndData(data, datalen);
        _sitrilTest->notify(unsolResponse);
    }
}

extern "C" void
RIL_requestTimedCallback(RIL_TimedCallback callback, void *param,
                            const struct timeval *relativeTime) {
    printf("%s, callback(%p), param(%p), timval(%p)\n", __FUNCTION__, callback, param, relativeTime);
}

extern "C" void
RIL_onRequestAck(RIL_Token t) {
    printf("%s token(%p)\n", __FUNCTION__, t);
}

extern "C" void
RIL_onUpdateReqModemData(RIL_Token t, void *data, size_t datalen) {
    int serial = 0;

    if (t != nullptr) {
        RIL_TestToken *pTestToken = (RIL_TestToken *)t;
        serial = pTestToken->serial;
    } else {
        printf("[%s] t is null ptr\n", __FUNCTION__);
    }

    SitrilTest *_sitrilTest = SitrilTest::GetInstance();
    LOGI("RIL_onUpdateReqModemData mSerial:%d serial:%d datalen:%lu",
         _sitrilTest->mSerial, serial, (unsigned long)datalen);

    if (_sitrilTest != NULL) {
        _sitrilTest->updateTxIpcRawData(data, datalen);
        _sitrilTest->notify(serial);
    } else {
        printf("[%s] sitril is null\n", __FUNCTION__);
    }
}

extern "C"
void RIL_onExternalOnUnsolicitedResponse(int unsolResponse, const void *data,
                                size_t datalen, int /*socket_id*/) {
    SitrilTest *_sitrilTest = SitrilTest::GetInstance();
    if (_sitrilTest == NULL) {
        printf("[%s] sitril is null\n", __FUNCTION__);
        return;
    }

    LOGI("RIL_onExternalOnUnsolicitedResponse mSerial:%d, unsolResponse:%d, datalen:%lu",
        _sitrilTest->mSerial, unsolResponse, (unsigned long)datalen);

    if (_sitrilTest->mSerial == unsolResponse) {
        _sitrilTest->updateRilIndData(data, datalen);
        _sitrilTest->notify(unsolResponse);
    }
}

static struct RIL_EnvUnitTest s_rilEnvUnitTest;
static struct RIL_External_EnvUnitTest s_rilExternalEnv;

const char defaultRilConf[100] = "-c 0 -p ./ ";

static int make_argv(char * args, char ** argv, unsigned int offset) {
    int count = offset;
    char *tok, *saveptr;
    char *s = args;

    while ((tok = strtok_r(s, " ", &saveptr))) {
        argv[count] = tok;
        s = NULL;
        count++;
        if (count > MAX_RIL_ARGC) break;
    }
    return count;
}


SitrilTest *SitrilTest::mStaticInstance = nullptr;

// SitrilTest class
SitrilTest::SitrilTest() {
    mDlHandle = nullptr;
    mRilFuncs = nullptr;
    resetTestData();
}

SitrilTest::~SitrilTest() {
    SitrilClose();
}

SitrilTest *SitrilTest::CreateInstance() {
    if (mStaticInstance == nullptr) {
        mStaticInstance = new SitrilTest;
        mStaticInstance->SitrilLoad(1);  // setting test mode;
    }
    return mStaticInstance;
}

void SitrilTest::SitrilLoad(int serviceId) {
    const char *rilLibPath = "libsitril.so";
    const RIL_UnitTestFunctions *(*rilUnitTest)(const struct RIL_Env *, const struct RIL_External_Env *, int, char **);

    mDlHandle = dlopen(rilLibPath, RTLD_NOW);
    if (mDlHandle == nullptr) printf("error %s %s", rilLibPath, dlerror());
    ASSERT_NE(nullptr, mDlHandle);

    char **rilArgv = new char *[MAX_RIL_ARGC];
    char rilConf[100] = { 0 };
    char serviceConf[10] = { 0 };
    sprintf(serviceConf, "-t %d", serviceId);
    size_t len = strlen(defaultRilConf);
    strncpy(rilConf, defaultRilConf, len);
    strncpy(rilConf+len, serviceConf, strlen(serviceConf));

    int argc = make_argv(rilConf, rilArgv, 0);
    rilUnitTest =
        (const RIL_UnitTestFunctions *(*)(const struct RIL_Env *, const struct RIL_External_Env *,int, char **))
        dlsym(mDlHandle, "RIL_UnitTest");
    ASSERT_NE(nullptr, rilUnitTest);

    s_rilEnvUnitTest.OnRequestComplete = RIL_onRequestComplete;
    s_rilEnvUnitTest.OnUnsolicitedResponse = RIL_onUnsolicitedResponse;
    s_rilEnvUnitTest.RequestTimedCallback = RIL_requestTimedCallback;
    s_rilEnvUnitTest.OnRequestAck = RIL_onRequestAck;
    s_rilEnvUnitTest.OnUpdateReqModemData = RIL_onUpdateReqModemData;

    s_rilExternalEnv.ExternalOnRequestComplete = RIL_onRequestComplete;
    s_rilExternalEnv.ExternalOnUnsolicitedResponse = RIL_onExternalOnUnsolicitedResponse;
    s_rilExternalEnv.OnUpdateReqModemData = RIL_onUpdateReqModemData;

    mRilFuncs = rilUnitTest(&s_rilEnvUnitTest, &s_rilExternalEnv, argc, rilArgv);
    ASSERT_NE(nullptr, mRilFuncs);

    /* If it needs to make registration state/power state as expected before start test
     * you need to implement.
     */
    UINT8* rspData = nullptr;
    UINT16 rspDataLen = 0;

    // send SIT_IND_RADIO_READY
    rspData = buildIndRadioReady(&rspDataLen);
    processInd(RIL_UNSOL_RESPONSE_RADIO_STATE_CHANGED, rspData, rspDataLen);
    free(rspData);

    // send SIT_IND_RADIO_STATE_CHANGED
    rspData = buildIndRadioStateChanged(&rspDataLen, 2);    // 2 = SIT_PWR_RADIO_STATE_START_NETWORK
    processInd(RIL_UNSOL_RESPONSE_RADIO_STATE_CHANGED, rspData, rspDataLen);
    free(rspData);
}

void SitrilTest::SitrilClose() {
    int ret = 0;
    if (mDlHandle != nullptr) {
        void (*rilUnitTestClose)();
        rilUnitTestClose = (void (*)())dlsym(mDlHandle, "RIL_UnitTestClose");
        if (rilUnitTestClose) rilUnitTestClose();
        //usleep(2000);
        ret = dlclose(mDlHandle);
        printf("dlclose %d\n", ret);
    }
    mDlHandle = nullptr;
    mRilFuncs = nullptr;
}

void SitrilTest::updateTxIpcRawData(void *data, unsigned int len) {
    char **ppDst = nullptr;
    unsigned int *pDstLen = nullptr;
    if (mTxIpcIndex < MAX_RSP_DATA_NUM) {
        ppDst = &(m_pTxIpcRawData[mTxIpcIndex]);
        pDstLen = &(m_pTxIpcRawDataLen[mTxIpcIndex]);
        mTxIpcIndex++;
        *ppDst = (char*)data;
        *pDstLen = len;
    } else {
        printf("There is no place to save modem data ptr\n");
    }
}

char *SitrilTest::getTxIpcRawData(int index, unsigned int *pDataLen) {
    char *pRet = nullptr;
    if (index < MAX_RSP_DATA_NUM) {
        *pDataLen = m_pTxIpcRawDataLen[index];
        pRet = m_pTxIpcRawData[index];
    } else {
        printf("[%s]index is out of bound\n", __FUNCTION__);
    }
    return pRet;
}

void SitrilTest::updateRilRspData(RIL_Errno e, void *data, unsigned int len) {
    mRilRspErrno = e;
    if (mRilRspIndex < MAX_RSP_DATA_NUM && data != nullptr) {
        mRilRspDataLen[mRilRspIndex] = len;
        memcpy(mRilRspData[mRilRspIndex], data, len);
        mRilRspIndex++;
    }
    if (mHook) {
        mHook->onRequestComplete(data, len);
    }
}

void *SitrilTest::getRilRspData(int index, unsigned int *pDataLen) {
    void *pRet = nullptr;
    if (index < MAX_RSP_DATA_NUM) {
        *pDataLen = mRilRspDataLen[index];
        pRet = mRilRspData[index];
    } else {
        printf("[%s]index is out of bound\n", __FUNCTION__);
    }
    return pRet;
}

void SitrilTest::updateRilIndData(const void *data, unsigned int len) {
    if (mRilIndIndex < MAX_RSP_DATA_NUM && data != nullptr) {
        mRilIndDataLen[mRilIndIndex] = len;
        memcpy(mRilIndData[mRilIndIndex], data, len);
        mRilIndIndex++;
    }
}

const void *SitrilTest::getRilIndData(int index, unsigned int *pDataLen) {
    const void *pRet = nullptr;
    if (index < MAX_RSP_DATA_NUM) {
        *pDataLen = mRilIndDataLen[index];
        pRet = mRilIndData[index];
    } else {
        printf("[%s]index is out of bound\n", __FUNCTION__);
    }
    return pRet;
}

void SitrilTest::notify(int receivedSerial) {
    std::unique_lock<std::mutex> lock(mMtx);
    if (mSerial == receivedSerial) {
        mCount++;
        mCv.notify_one();
    }
}

std::cv_status SitrilTest::wait(int sec) {
    std::unique_lock<std::mutex> lock(mMtx);
    std::cv_status status = std::cv_status::no_timeout;
    auto now = std::chrono::system_clock::now();
    while (mCount == 0) {
        status = mCv.wait_until(lock, now + std::chrono::seconds(sec));
        if (status == std::cv_status::timeout) {
            return status;
        }
    }
    mCount--;
    return status;
}

/* API */
void SitrilTest::resetTestData() {
    for (int i = 0; i < MAX_RSP_DATA_NUM; ++i) {
        m_pTxIpcRawData[i] = nullptr;
        m_pTxIpcRawDataLen[i] = 0;
        memset(mRilRspData[i], 0, MAX_RSP_DATA_SIZE);
        mRilRspDataLen[i] = 0;
        memset(mRilIndData[i], 0, MAX_RSP_DATA_SIZE);
        mRilIndDataLen[i] = 0;
    }
    mTxIpcIndex = 0;
    mRilRspIndex = 0;
    mRilIndIndex = 0;
    mCount = 0;
    mSerial = 0;
    mToken.serial = 0;
    mToken.requestId = -1;
    mRilRspErrno = RIL_E_SUCCESS;
}

void SitrilTest::processReq(int serial, int reqId, void *rilReqData, unsigned int rilReqDataLen, int phoneIndex) {
    mSerial = serial;
    mToken.serial = serial;
    mToken.requestId = reqId;
    if (mRilFuncs != nullptr) {
#if defined(ANDROID_MULTI_SIM)
        mRilFuncs->onRequest(reqId, rilReqData, rilReqDataLen, &mToken, (RIL_SOCKET_ID)phoneIndex);
#else
        UNUSED(phoneIndex);
        mRilFuncs->onRequest(reqId, rilReqData, rilReqDataLen, &mToken);
#endif
    }
}

void SitrilTest::processReqNoIpc(int serial, int reqId, void *rilReqData, unsigned int rilReqDataLen, int phoneIndex) {
    mSerial = reqId;
    mToken.serial = serial;
    mToken.requestId = reqId;
    if (mRilFuncs != nullptr) {
#if defined(ANDROID_MULTI_SIM)
        mRilFuncs->onRequest(reqId, rilReqData, rilReqDataLen, &mToken, (RIL_SOCKET_ID)phoneIndex);
#else
        UNUSED(phoneIndex);
        mRilFuncs->onRequest(reqId, rilReqData, rilReqDataLen, &mToken);
#endif
    }
}

void SitrilTest::processExternReq(int serial, int reqId, void *rilReqData, unsigned int rilReqDataLen, int phoneIndex) {
    mSerial = serial;
    mToken.serial = serial;
    mToken.requestId = reqId;
    if (mRilFuncs != nullptr) {
        mRilFuncs->externOnRequest(reqId, &mToken, rilReqData, rilReqDataLen, (RIL_SOCKET_ID)phoneIndex);
    }
}

void SitrilTest::processExternReqNoIpc(int serial, int reqId, void *rilReqData, unsigned int rilReqDataLen, int phoneIndex) {
    mSerial = reqId;
    mToken.serial = serial;
    mToken.requestId = reqId;
    if (mRilFuncs != nullptr) {
        mRilFuncs->externOnRequest(reqId, &mToken, rilReqData, rilReqDataLen, (RIL_SOCKET_ID)phoneIndex);
    }
}

void SitrilTest::processRsp(int rspId, void *modemData, unsigned int modemDataLen) {
    usleep(1000);
    mSerial = rspId;
    // mToken value is already set by req process
    if (mRilFuncs != nullptr) {
        mRilFuncs->onModemRspSend(modemData, modemDataLen);
    }
}

void SitrilTest::processInd(int unsolId, void *modemData, unsigned int modemDataLen){
    mSerial = unsolId;
    mToken.serial = 0;
    mToken.requestId = -1;
    if (mRilFuncs != nullptr) {
        mRilFuncs->onModemRspSend(modemData, modemDataLen);
    }
}

void SitrilTest::processSapReq(int serial, int reqId, void *rilReqData, unsigned int rilReqDataLen, int phoneIndex) {
    mSerial = serial;
    mToken.serial = serial;
    mToken.requestId = reqId;
    if (mRilFuncs != nullptr) {
#if defined(ANDROID_MULTI_SIM)
        mRilFuncs->onSapRequest(reqId, rilReqData, rilReqDataLen, &mToken, (RIL_SOCKET_ID)phoneIndex);
#else
        UNUSED(phoneIndex);
        mRilFuncs->onSapRequest(reqId, rilReqData, rilReqDataLen, &mToken);
#endif
    }
}

void SitrilTest:: processSitrilStateUpdate(int radioState, int networkState, int simCardState) {
    if (mRilFuncs != nullptr) {
        mRilFuncs->onSitrilStateUpdate(radioState, networkState, simCardState);
    }
    usleep(1000);
}
