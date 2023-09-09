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
#ifndef __SITRIL_EXTERN_TEST_H__
#define __SITRIL_EXTERN_TEST_H__

#include <telephony/ril.h>
#include <slsi/ril_external.h>
#include <libsitril-client/sitril-client.h>
#include <stdio.h>
#include <string.h>
#include <dlfcn.h>
#include <gtest/gtest.h>
#include <mutex>
#include <chrono>
#include <condition_variable>


enum {
    RIL_EXTERN_AUDIO = 0,
    RIL_EXTERN_GPS,
    RIL_EXTERN_SE,
    RIL_EXTERN_WLAN,
    RIL_EXTERN_NUMBER
};


class SitrilExternApi {
public:
    /* Function pointers */
    void *(*ril_open_client)(const struct RILC_EnvUnitTest *env);
    int (*ril_close_client)(void);

    SitrilExternApi();
};

class SitrilExternCmdData {
public:
    unsigned int mMsgId;
    size_t mDataLen;
    char *mData;
    unsigned int mChannel;

    SitrilExternCmdData(unsigned int msgId, void* data, size_t length, unsigned int channel);
    ~SitrilExternCmdData();
};

class SitrilExternTest {
protected:
    /* The pointer of interface library for RIL Client*/
    void *mHandle;
    bool mConnection;
    SitrilExternApi *mpApi;
    // functions returned by rilc_open for unit test.
    const RILC_UnitTestFunctions *mRilcFuncs;

    // processing data
    SitrilExternCmdData *mpReqData;
    SitrilExternCmdData *mpRspData;
    unsigned int mRspMsgId;
    int mIndMsgId;

public:
    SitrilExternTest(int clientType, SitrilExternApi *pApi);
    ~SitrilExternTest();

    void setReqData(SitrilExternCmdData *reqData) {
        if(mpReqData) delete mpReqData;
        mpReqData = reqData;
    }
    SitrilExternCmdData *getReqData() { return mpReqData; }

    void setRspData(SitrilExternCmdData *rspData) {
        if(mpRspData) delete mpRspData;
        mpRspData = rspData;
    }

    void setRspMsgId(unsigned int msgId) { mRspMsgId = msgId; }
    unsigned int getRspMsgId() { return mRspMsgId; }
    void setIndMsgId(int msgId) { mIndMsgId = msgId; }
    int getIndMsgId() { return mIndMsgId; }

    void *getHandle();
    void updateConnection(bool isConnect);
    bool getConnection();
    void openClient();
    int registerCallback();
    void closeClient();
    void resetTestData();

    void sendRespone(int status, unsigned int msgId, void* data, size_t length, unsigned int channel);
    void sendIndication(unsigned int msgId, void* data, size_t length, unsigned int channel);

private:
    /* static */
    static SitrilExternTest *mStaticInstance;

public:
    /* static */
    static SitrilExternTest *GetInstance() { return mStaticInstance; }
    static void SetInstance(SitrilExternTest *externTest) { mStaticInstance = externTest; }
};
#endif
