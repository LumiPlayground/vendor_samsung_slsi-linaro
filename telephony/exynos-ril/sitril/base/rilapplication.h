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
 * rilapplication.h
 *
 *  Created on: 2014. 11. 12.
 *      Author: sungwoo48.choi
 */

#ifndef __RIL_APPLICATION_H__
#define __RIL_APPLICATION_H__

#include "rilapplicationcontext.h"
#include <slsi/ril_external.h>
#include "rilproperty.h"
#include "thread.h"

class ProductFactory;
class RilContext;
class ModemStateMonitor;
class RilResponseListener;
class SignalMonitor;
class ServiceFactory;

/* SAP */
class SapResponseListener;

/* RIL vendor external */
class RilExternalResponseListener;

class OperatorDbMakeRunnable : public Runnable {
public:
    OperatorDbMakeRunnable();
    ~OperatorDbMakeRunnable();

    void Run();
};

class RilApplication : public RilApplicationContext {
    DECLARE_MODULE_TAG()

protected:
    const struct RIL_Env *m_pRilEnv;
    RilContext *m_RilContext[SIM_COUNT];
    ModemStateMonitor *m_pModemStateMonitor;
    RilResponseListener *m_pRilRespListener;
    RilProperty m_RilAppProperty;
    SignalMonitor *m_pSignalMonitor;

    ProductFactory *m_pProductFactory;
    ServiceFactory *m_pServiceFactory;

    /* SAP */
    const struct RIL_Env *m_pSapEnv;
    SapResponseListener *m_pSapRespListener;

    /* RIL vendor external */
    const struct RIL_External_Env *m_pRilExternalEnv;
    RilExternalResponseListener *m_pRilExternalRespListener;

    /* for unit Test */
    int mUnitTestMode;
    int mUnitTestReqSerial;

    // constructor
public:
    RilApplication(const struct RIL_Env *pRilEnv = NULL);
    RilApplication(const RilApplication &) = delete;
    RilApplication& operator=(RilApplication const&) = delete;
    virtual ~RilApplication();

private:
    Thread *m_pOpDbThread;
    OperatorDbMakeRunnable *m_pOpDbRunnable;

protected:
    int InitInstance();
    void ExitInstance();

protected:
    virtual int OnInitialize();
    virtual int OnFinalize();
    int RegisterRilContext(unsigned int id, RilContext *pRilContext);

    virtual void LoadConfigToRilProperty();
    int LoadTargetOperator();

public:
    RilContext *GetDefaultRilContext();
    RilContext *GetRilContext(RIL_SOCKET_ID socket_id);
    RilProperty *GetProperty() { return &m_RilAppProperty; }
    void OnModemStateChanged(int state);
    void StartModemIpc();
    void ResetModem(const char *reason);

    /* for unit Test */
    int UpdateReqModemData(RIL_Token t, void *data, unsigned int len);
    void UpdateUnitTestSerial(RIL_Token t);
    void SetUnitTestMode(int utMode) { mUnitTestMode = utMode; }
    int GetUnitTestMode() { return mUnitTestMode; }

    typedef enum {OEM_CAT_AUDIO, OEM_CAT_IMS, OEM_CAT_SIM}OEM_CATEGORY;
    RIL_SOCKET_ID GetProperSocketIdForOem(OEM_CATEGORY OemCategory);

private:
    RIL_SOCKET_ID GetProperSocketId(int request, void *data, unsigned int datalen);
    int getIfIndex();

    // @Overriding
public:
    // Implement RIL_RadioFunctions
    void OnRequest(int request, void *data, unsigned int datalen, RIL_Token t, RIL_SOCKET_ID socket_id);
    void OnRequest(int request, void *data, unsigned int datalen, RIL_Token t);
    RIL_RadioState OnRadioStateRequest(RIL_SOCKET_ID socket_id);
    RIL_RadioState OnRadioStateRequest();
    void OnRequestComplete(RIL_Token t, RIL_Errno e, void *response, unsigned int responselen);
    void OnUnsolicitedResponse(int unsolResponse, const void *data, unsigned int datalen);
    void OnUnsolicitedResponse(int unsolResponse, const void *data, unsigned int datalen, RIL_SOCKET_ID socket_id);
    void OnRequestAck(RIL_Token t);

    // Extended
    void OnOemRequest(int /* request */, void * /* data */, unsigned int /* datalen */, RIL_Token, RIL_SOCKET_ID) { /* Deprecated */ };
    void OnOemRequest(int /* request */, void * /* data */, unsigned int /* datalen */, RIL_Token) { /* Deprecated */ };
    void OnRequestComplete(RilContext *context, RIL_Token t, RIL_Errno e, void *response, unsigned int responselen);
    void OnUnsolicitedResponse(RilContext *context, int unsolResponse, const void *data, unsigned int datalen);
    // for unit test
    void OnModemRspSend(void *data, unsigned int datalen);
    void OnSitrilStateUpdate(int radioState, int networkRegState, int cardState);
    void OnUpdateReqModemData(RIL_Token t, void *data, unsigned int len);

    /* SAP */
    void SetSapEnv(const struct RIL_Env *pSapEnv);
    void OnSapRequest(int request, void *data, unsigned int datalen, RIL_Token t, RIL_SOCKET_ID socket_id);
    void OnSapRequest(int request, void *data, unsigned int datalen, RIL_Token t);

    /* RIL vendor external */
    void externalOnRequest(int reqOemId, void *data, unsigned int datalen, RIL_Token t, RIL_SOCKET_ID socket_id);
    void setRilExternalEnv(const struct RIL_External_Env *pRilEnv);

    // static
private:
    static RilApplication *instance;
public:
    static RilApplication *CreateInstance(const struct RIL_Env *pRilEnv, int utMode = 0);
    static RilApplication *GetInstance();
    static void SetInstance(RilApplication *ptr);
    static RilApplicationContext *GetRilApplicationContext();

    static void RIL_OnRequest(int request, void *data, unsigned int datalen, RIL_Token t, RIL_SOCKET_ID socket_id);
    static void RIL_OnRequest(int request, void *data, unsigned int datalen, RIL_Token t);
    static RIL_RadioState RIL_OnRadioStateRequest(RIL_SOCKET_ID socket_id);
    static RIL_RadioState RIL_OnRadioStateRequest();
    static int RIL_OnSupports(int requestCode);
    static void RIL_OnCancel(RIL_Token t);
    static const char *RIL_OnGetVersion();
    static void RIL_OnTimedCallback(void *param);
    static int RIL_VersionCode;
    static int RIL_HalVersionCode;
    // for unit test
    static void RIL_OnModemRspSend(void *data, unsigned int datalen);
    static void RIL_OnSitrilStateUpdate(int radioState, int networkRegState, int cardState);

    /* SAP */
    static void SAP_OnRequest(int request, void *data, unsigned int datalen, RIL_Token t, RIL_SOCKET_ID socket_id);
    static void SAP_OnRequest(int request, void *data, unsigned int datalen, RIL_Token t);

    /* RIL vendor external */
    static void RIL_externalOnRequest(int reqOemId, void *data, unsigned int datalen, RIL_External_Token t, RIL_SOCKET_ID socket_id);

    // multi-sim configs
    static bool IsMultiSimEnabled();
    static int GetPhoneCount();
};

#endif /* __RIL_APPLICATION_H__ */
