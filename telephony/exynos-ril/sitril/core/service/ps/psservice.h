/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef __PsService_H__
#define __PsService_H__

#include "service.h"
#include "telephonystatustracker.h"

class PdpContext;
class ApnSetting;
class PsTrafficDescriptor;
class PsSetupDataCallHandler;
class PsTestHandler;
class SetupDataCallRequestData;
class PsDedicatedBearerInfoUpdatedHandler;
class PsInitialAttachApnHandler;
class PsSlicingConfigHandler;
class PsServiceProxy;
class PsSetDataCallHandler;

/*
#define    TIMEOUT_DEFAULT_PS             5000
#define    TIMEOUT_SETUP_DATA_CALL        150000
#define    TIMEOUT_DEACT_DATA_CALL        60000
*/

class PsService : public Service, public MonitoringCallback
{
    INTERFACE_AS_SERVICE(PsService, RIL_SERVICE_PS)
    friend PsServiceProxy;

protected:
    bool m_bIsAttachDone = false;
    PdpContext *m_PdpContext[MAX_DATA_CALL_SIZE] = { 0 };
    PdpContext *m_AttachPdpContext = NULL;
    int m_nPdpContextSize = 0;
    int m_nAttachCid = 0;
    PdpContext *m_pDeactivatingPdpContext = NULL;
    bool m_bDataCallStateArray[MAX_DATA_CALL_SIZE*2+1] = { false };

    char m_imsi[MAX_IMSI_LEN+1] = "";
    bool m_isRrcCon = false;

private:
    PsSetupDataCallHandler *m_pSetupDataCallHandler = NULL;
    PsTestHandler *m_pTestHandler = NULL;
    PsDedicatedBearerInfoUpdatedHandler *m_pPsDedicatedBearerInfoUpdatedHandler = NULL;
    PsInitialAttachApnHandler *m_pInitialAttachApnHandler = NULL;
    PsSetDataCallHandler *mSetDataCallHandler = NULL;
    PsSlicingConfigHandler *m_pPsSlicingConfigHandler = NULL;

    // For extended DP
    void printDataProfileTable();
    void clearDataProfileTable();
    void addDataProfile(const DataV2_0::RIL_DataProfileInfo &dpi);
    DataV2_0::RIL_DataProfileInfo* findDataProfile(const char * apn);
    vector<DataV2_0::RIL_DataProfileInfo> m_DataProfileInfo;

    // the valuse is determined by Communicaiton Channels between modem and modemif
    // and also affected by linux NetworkInterface preparation by modemif driver
    // ex) "16" can support rmnet0 ~rmnet15 for slot 0 (one psservice instance)
    //                      rmnet16~rmnet31 for slot 1
    //     but modemif just prepared total 30 network interface
    //     so actual confiugration is
    //         rmnet0 ~rmnet14 for slot 0
    //         rmnet15~rmnet30 for slot 1
    const static int MAX_CID_RANGE = 16;

public:
    PsService(RilContext* pRilContext);
    PsService(const PsService&) = delete;
    PsService& operator=(const PsService&) = delete;
    virtual ~PsService();

protected:
    virtual int OnCreate(RilContext *pRilContext);
    virtual void OnDestroy();

    virtual BOOL OnHandleRequest(Message* pMsg);
    virtual BOOL OnHandleSolicitedResponse(Message* pMsg);
    virtual BOOL OnHandleRequestTimeout(Message* pMsg);

public:
    virtual int SendRequest(ModemData *pModemData, UINT nTimeout) {
        return Service::SendRequest(pModemData, nTimeout);
    }
    virtual int SendRequest(ModemData *pModemData, UINT nTimeout, UINT nResult, Message *pMsg = NULL) {
        return Service::SendRequest(pModemData, nTimeout, nResult, pMsg);
    }
    virtual int SendRequest(ModemData *pModemData) {
        return Service::SendRequest(pModemData);
    }
    inline int OnRequestComplete(int result, void *data = NULL, int length = 0, Message *pMsg = NULL) {
        return Service::OnRequestComplete(result, data, length, pMsg);
    }
    virtual int OnUnsolicitedResponse(int id, const void *data = NULL, int length = 0) {
        return Service::OnUnsolicitedResponse(id, data, length);
    }

    virtual void OnRadioStateChanged(int radioState);
    virtual void OnSimStatusChanged(int cardState, int appState);
    virtual void OnImsiUpdated(const char *imsi);
    virtual bool IsPossibleToPassInRadioOffState(int request_id); // Called by Service

    // wrapper to open accessibility
    inline RilContext *GetRilContext() { return Service::GetRilContext(); }
    inline int GetPdpContextSize() { return m_nPdpContextSize; }
    inline string GetSimOperatorNumeric() { return Service::GetSimOperatorNumeric(); }
    inline RIL_SOCKET_ID GetRilSocketId() { return Service::GetRilSocketId(); }
    inline RilProperty *GetRilApplicationProperty()  { return Service::GetRilApplicationProperty(); }

protected:
    BOOL InitPdpContext();
    BOOL ResetPdpContext(bool bKeepAttachApn = false);
    inline PdpContext *GetAttachPdpContext(int /* cid */) { return ((m_AttachPdpContext) ? m_AttachPdpContext : NULL); }

    template<typename T>
    int OnSetupDataCallDone(Message *pMsg);
    template<typename T>
    void OnSetupDataCallDoneInternal(T &adapter, Message *pMsg);
    virtual int OnSetupDataCallTimeout(Message *pMsg);
public:
    PdpContext *GetPdpContext(int cid);
    virtual void OnSetupDataCallComplete(int errorCode, PdpContext *pPdpContext, Message *pMsg);
    virtual void OnSetupDataCallCompletePdpFail(int pdp_fail_status, Message *pMsg);
    void tryDeactPdpOnAbnormalState(PdpContext *pPdpContext, int errorCode);
    bool waitForRSRAComplete(PdpContext *pPdpContext);
    virtual void OnNotifyDataCallList();
    // SIM 1 : Attach CID may be 1
    // SIM 2 : Attach CID may be 5
    inline int GetAttachCid() const { return m_nAttachCid; }
    PdpContext *GetPdpContextByIndex(int index);
    PdpContext *RetrieveAttachPdpContext();
    inline int GetDataNetworkState() const { return Service::GetDataNetworkState(); }

protected:
    virtual void OnDeactDataCallComplete(PdpContext *pPdpContext);
    virtual int OnRequestDeactDataCall(PdpContext *pPdpContext, int reason = DEACT_REASON_NORMAL);
    virtual int OnRequestDeactDataCall(int cid, int reason = DEACT_REASON_NORMAL);
    bool cleanUpPdpOnInactiveDc(const DataCall *dc);
    bool updatePdpOnActiveDc(const DataCall *dc);
    int makePermanentFailureForIMS(PdpContext *pPdpContext);
    void cleanUpInactivePdp(PdpContext *pPdpContext);
    virtual void setDeactivatingPdpContext(PdpContext *pPdpContext) { m_pDeactivatingPdpContext = pPdpContext; }

    virtual int OnSimpleRequestDone(Message *pMsg);
    virtual int DoSetDataProfileInternal(SetupDataCallRequestData *datacall);

public:
    void RSRATimeoutCallback(int cid);
    int isSameTargetOperator(int targetOperator);
    int getHalVersion(Message *pMsg);
    void SetCurRrcState(bool state) { m_isRrcCon = state; }
    bool IsRrcConState() { return m_isRrcCon; }
private:
    inline bool IsNullMsg(Message *pMsg);
    bool IsNullResponse(Message *pMsg);
    void notifyAMBR(ApnSetting* pApnSetting, const DataCall *);
private:
    template<typename T>
    bool GetRilRequestData(Message *pMsg, T *(&rildata));
    bool needDnsV4First();
    template<typename T>
    void cleanUpVar(T &v);
    template<typename T>
    void InitHandler(T *(&h));
    bool isEqualCid(Message *pMsg, int rspCid);
    void ResetInactivePdpContext(Message *pMsg);
    void dump(std::stringstream& ss);
};
#endif
