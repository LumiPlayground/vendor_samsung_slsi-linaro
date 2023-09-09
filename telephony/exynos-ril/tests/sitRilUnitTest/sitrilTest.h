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
#ifndef __SITRIL_TEST_H__
#define __SITRIL_TEST_H__

#include <telephony/ril.h>
#include <slsi/ril_external.h>
#include <stdio.h>
#include <string.h>
#include <dlfcn.h>
#include <gtest/gtest.h>
#include <mutex>
#include <chrono>
#include <condition_variable>

#define TIMEOUT_PERIOD (1)
#define MAX_RIL_ARGC (20)
#define MAX_RSP_DATA_NUM (3)
#define MAX_RSP_DATA_SIZE (2048)
#define RIL_VERSION_FOR_TEST (15)

typedef enum {
    RIL_SERVICE_UNKNOWN,
    RIL_SERVICE_CSC,           // 1
    RIL_SERVICE_PS,            // 2
    RIL_SERVICE_SIM,           // 3
    RIL_SERVICE_MISC,          // 4
    RIL_SERVICE_NETWORK,       // 5
    RIL_SERVICE_SMS,           // 6
    RIL_SERVICE_AUDIO,         // 7
    RIL_SERVICE_IMS,           // 8
    RIL_SERVICE_GPS,           // 9
    RIL_SERVICE_VSIM,          // 10
    RIL_SERVICE_STK,           // 11
    RIL_SERVICE_SUPPLEMENTARY, // 12
    RIL_SERVICE_EMBMS,         // 13
    RIL_SERVICE_SIM_IO,        // 14
    RIL_SERVICE_SAP,           // 15
    RIL_SERVICE_UNSUPPORT,
    RIL_SERVICE_MAX,
}RIL_SERVICE_TYPE;

struct RilResponseHook {
    virtual ~RilResponseHook()=default;
    virtual void onRequestComplete(const void *data, size_t datalen)=0;
};

class SitrilTest {
protected:
    std::mutex mMtx;
    std::condition_variable mCv;
    int mCount;

    // handle for vendor ril lib
    void *mDlHandle;
    // functions returned by ril init function in vendor ril
    const RIL_UnitTestFunctions *mRilFuncs;

    int mTxIpcIndex;
    char *m_pTxIpcRawData[MAX_RSP_DATA_NUM];
    unsigned int m_pTxIpcRawDataLen[MAX_RSP_DATA_NUM];
    int mRilRspIndex;
    char mRilRspData[MAX_RSP_DATA_NUM][MAX_RSP_DATA_SIZE];
    unsigned int mRilRspDataLen[MAX_RSP_DATA_NUM];
    int mRilIndIndex;
    char mRilIndData[MAX_RSP_DATA_NUM][MAX_RSP_DATA_SIZE];
    unsigned int mRilIndDataLen[MAX_RSP_DATA_NUM];
    RIL_Errno mRilRspErrno;

public:
    SitrilTest();
    virtual ~SitrilTest();

    void *getHandle() { return mDlHandle; }
    void updateTxIpcRawData(void *data, unsigned int len);
    char *getTxIpcRawData(int index, unsigned int *pDataLen);
    void updateRilRspData(RIL_Errno e, void *data, unsigned int len);
    void *getRilRspData(int index, unsigned int *pDataLen);
    void updateRilIndData(const void *data, unsigned int len);
    const void *getRilIndData(int index, unsigned int *pDataLen);
    RIL_Errno getRilRspErrno() { return mRilRspErrno; }

    /* Used as a mechanism to inform the test about data/event callback */
    void notify(int receivedSerial);

    /* Test code calls this function to wait for response */
    std::cv_status wait(int sec = TIMEOUT_PERIOD);

    /* for time out checking */
    int mSerial;

    /* Ser number for radio request */
    RIL_TestToken mToken;

    RilResponseHook *mHook = nullptr;

    void SitrilLoad(int serviceId);
    void SitrilClose();

    // API
public:
    const RIL_UnitTestFunctions *getRilFunc() { return mRilFuncs; }
    void resetTestData();
    void processReq(int serial, int reqId, void *rilReqData, unsigned int rilReqDataLen, int phoneIndex);
    void processReqNoIpc(int serial, int reqId, void *rilReqData, unsigned int rilReqDataLen, int phoneIndex);
    void processExternReq(int serial, int reqId, void *rilReqData, unsigned int rilReqDataLen, int phoneIndex);
    void processExternReqNoIpc(int serial, int reqId, void *rilReqData, unsigned int rilReqDataLen, int phoneIndex);
    void processRsp(int rspId, void *modemData, unsigned int modemDataLen);
    void processInd(int unsolId, void *modemData, unsigned int modemDataLen);
    void processSapReq(int serial, int reqId, void *rilReqData, unsigned int rilReqDataLen, int phoneIndex);
    void processSitrilStateUpdate(int radioState, int networkState, int simCardState);

private:
    /* static */
    static SitrilTest *mStaticInstance;

public:
    /* static */
    static SitrilTest *GetInstance() { return mStaticInstance; }
    static void SetInstance(SitrilTest *sitrilTest) { mStaticInstance = sitrilTest; }
    static SitrilTest *CreateInstance();

    struct RilResponseHookBase : public RilResponseHook {
        SitrilTest& rilTest;
        RilResponseHookBase(SitrilTest& t) : rilTest(t) {
            rilTest.mHook = this;
        }
        virtual ~RilResponseHookBase() {
            rilTest.mHook = nullptr;
        }
    };
};

#endif
