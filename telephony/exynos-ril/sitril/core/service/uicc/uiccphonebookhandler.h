/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef __UICC_PHONEBOOK_HANDLER_H__
#define __UICC_PHONEBOOK_HANDLER_H__

#include <base/types.h>
#include <slsi/radio_v1_6.h>

class SimService;
class Message;

class UiccPhonebookHandler
{
    friend SimService;

public:
    typedef enum {
        PB_NOT_READY = 0,
        PB_READY,
        PB_DISABLED,
    } PB_READY_STATUS;

    typedef struct {
        int maxAdnRecords;
        int usedAdnRecords;
        int maxEmailRecords;
        int usedEmailRecords;
        int maxAdditionalNumberRecords;
        int usedAdditionalNumberRecords;
        int maxNameLen;
        int maxNumberLen;
        int maxEmailLen;
        int maxAdditionalNumberLen;
    } SIM_PhonebookCapacity;

private:
    SimService *m_pSimService;

protected:
    UiccPhonebookHandler();
    virtual ~UiccPhonebookHandler();
    virtual void Init(SimService* pSimService);

private:
    //SIMPB
    int mRecordId;             // it is used to read pb entry
    bool mIsSimPbSupported;    // it is used to check CP PBM is enabled or disabled.
    bool mIsSimPbInitialized;  // it is used to check CP PBM sent PB ready signal.
    bool mIsSimPbLoading;      // it is used to show some error or hotswap happened during sim pb is being loaded.
    SIM_PhonebookCapacity mPbCapa;
    RIL_PhonebookRecInfoResult mPbResult;

public:
    bool IsSimPbSupported() { return mIsSimPbSupported; }
    bool IsSimPbInitialized() { return mIsSimPbInitialized; }
    bool IsSimPbBeingLoaded() { return mIsSimPbLoading; }
    void InitPbFlags();
    void SendLoadingPbError(int error);

protected:
    virtual int SendMessageForReadPbEntry(int recordId);
    virtual int DoReadPbEntry(Message *pMsg);
    virtual int OnReadPbEntryDone(Message *pMsg);

    virtual int DoUpdatePbEntry(Message *pMsg);
    virtual int OnUpdatePbEntryDone(Message *pMsg);

    virtual int DoGetPbStorageInfo();
    virtual int OnGetPbStorageInfoDone(Message *pMsg);

    virtual int DoGet3GPbCapa();
    virtual int OnGet3GPbCapaDone(Message *pMsg);

    virtual int DoGetPbCapa(Message *pMsg);
    virtual int OnGetPbCapaDone(UINT uErrCode);

    virtual int DoGetPbRec(Message *pMsg);
    virtual int OnGetPbRecDone(int status);

    virtual int OnReadPbEntryTimeout(Message *pMsg);

    virtual int OnPbReady(Message *pMsg);

};

#endif /* __UICC_PHONEBOOK_HANDLER_H__ */
