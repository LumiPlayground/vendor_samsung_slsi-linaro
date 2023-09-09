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
  * psserviceproxy.h
  *
  *  Created on: 2020. 8. 24.
  *  Author: juneho.kwon
  */

#ifndef _SITRIL_CORE_SERVICE_PROXY_PSSERVICEPROXY_H_
#define _SITRIL_CORE_SERVICE_PROXY_PSSERVICEPROXY_H_

class PsService;
class Service;
class PsTestHandler;
class PdpContext;
class DeactivateDataCallRequestData;
class Message;
class RilProperty;

class PsServiceProxy {
    private:
        PsService *mService;

    public:
        PsServiceProxy(Service *service);
        ~PsServiceProxy();

    public:
        bool HasService() { return mService != NULL; }

        int OnRequestDeactDataCall(PdpContext *pPdpContext, int reason = DEACT_REASON_NORMAL);
        int OnRequestDeactDataCall(int cid, int reason = DEACT_REASON_NORMAL);
        int makePermanentFailureForIMS(PdpContext *pPdpContext);
        int isSameTargetOperator(int);
        int OnSimpleRequestDone(Message *);

        bool resetPdpContext(bool bKeepAttachApn);
        bool cleanUpPdpOnInactiveDc(const DataCall *dc);
        bool updatePdpOnActiveDc(const DataCall *dc);

        void cleanUpInactivePdp(PdpContext *pPdpContext);
        void OnNotifyDataCallList();
        void OnDeactDataCallComplete(PdpContext *pPdpContext);

        void setDeactivatingPdpContext(PdpContext * pdpContext);

        int getPdpContextSize();
        template<typename T>
        bool getRilRequestData(Message *pMsg, T *(&rildata));

        PdpContext * getDeactivatingPdpContext();
        PdpContext * getPdpContext(int cid);
        PdpContext * getPdpContextByIndex(int index);
        RilProperty *getRilContextProperty();
        RilProperty *GetRilApplicationProperty();
        PsTestHandler * getPsTestHandler();

        //Newly-added for extended DP
        void printDataProfileTable();
        void clearDataProfileTable();
        void addDataProfile(const DataV2_0::RIL_DataProfileInfo &dpi);
        DataV2_0::RIL_DataProfileInfo* findDataProfile(const char *apn);

        void SetCurRrcState(bool state);
        bool IsRrcConState();
};

#endif /* _PS_SERVICE_PROXY_H_ */
