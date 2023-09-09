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
 * simioservice.cpp
 *
 */

#ifndef __SIMIO_SERVICE_H__
#define __SIMIO_SERVICE_H__

#include "service.h"

#define    TIMEOUT_SIMIO_DEFAULT        30000

class Message;
class RilContext;
class ModemData;

//from TS 11.11 9.1 or elsewhere
#define COMMAND_READ_BINARY     0xb0
#define COMMAND_READ_RECORD     0xb2
#define COMMAND_GET_RESPONSE    0xc0
#define COMMAND_UPDATE_BINARY   0xd6
#define COMMAND_UPDATE_RECORD   0xdc
#define COMMAND_STATUS          0xf2
#define COMMAND_RETRIEVE_DATA   0xa2
#define COMMAND_SET_DATA        0xdb


// GSM SIM file ids from TS 51.011
#define EF_AD   0x6FAD

// Feature Definition
#define SUPPORT_CHANGE_MCCMNC   FALSE
#define SUPPORT_NOTIFY_DEVICE_INFO   FALSE

// IccIoResult
class IccIoResult
{
public:
    int mSw1;
    int mSw2;

private:
    int mDataLen;
    char *mData;

public:
    IccIoResult(int sw1, int sw2, char *data, int dataLen) : mDataLen(0), mData(NULL)
    {
        mSw1 = sw1;
        mSw2 = sw2;
        if (dataLen > 0) {
            mDataLen = (dataLen < MAX_SIM_IO_DATA_LEN) ? dataLen : MAX_SIM_IO_DATA_LEN;
            mData = new char[mDataLen];
            memcpy(mData, data, mDataLen);
        }
    }
    IccIoResult(const IccIoResult &) = delete;
    IccIoResult& operator=(IccIoResult const&) = delete;

    ~IccIoResult() {
        if (mData != NULL) {
            delete[] mData;
            mData = NULL;
        }

        mDataLen = 0;
    }

public:
    bool IsSuccess() { return mSw1 == 0x90 || mSw1 == 0x91 || mSw1 == 0x9e || mSw1 == 0x9f; }
    const char *GetData() const { return mData; };
    int GetDataLength() const { return mDataLen; };
};

class SimIoService : public Service
{
    typedef enum {
        SIM_PIN,
        SIM_PUK
    } SIM_PIN_PUK;

    typedef enum {
        PIN1_PUK1 = 0,
        PIN2_PUK2
    } SIM_PIN_PUK_INDEX;

protected:
    // Member Variables
    static const INT32 SIM_EFID_FDN = 0x6F3B;

public:
    SimIoService(RilContext* pRilContext);
    SimIoService(const SimIoService &) = delete;
    SimIoService& operator=(SimIoService const&) = delete;
    virtual ~SimIoService();

protected:
    virtual int OnCreate(RilContext *pRilContext);

    virtual BOOL OnHandleRequest(Message* pMsg);
    virtual BOOL OnHandleSolicitedResponse(Message* pMsg);
    virtual void OnSimStatusChanged(int cardState, int appState);

protected:
    virtual int OnVerifyPin2Done(Message *pMsg);

    virtual int DoSimIo(Message *pMsg);
    virtual int OnSimIoDone(Message *pMsg);

    virtual int DoGetImsi(Message *pMsg);
    virtual int OnGetImsiDone(Message *pMsg);

    // IMS
    virtual int DoOemImsSimIo(Message *pMsg);
    virtual int OnOemImsSimIoDone(Message *pMsg);

    virtual bool IsPossibleToPassInRadioOffState(int request_id);
#ifdef SUPPORT_CHANGE_MCCMNC
    virtual char* checkMmcMnc(const char* pImsi);
#endif
    string FetchSimOperator(IccIoResult &iccioResult);
    void UpdateImsi(const char *imsi);
    void UpdateImsi(const char *aid, const char *imsi, int imsiType);
    void SendOpenCarrierInfoRilReq(const char *plmn);

    void OnHandleEF_AD(IccIoResult &iccioResult);

protected:

    ModemData *m_pSimIoData;
    string mImsi;
};

#endif /*__SIMIO_SERVICE_H__*/
