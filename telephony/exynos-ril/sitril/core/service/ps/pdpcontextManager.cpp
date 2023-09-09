/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */
#include "./pdpcontextManager.h"
#include "rillog.h"
#include "./pdpcontext.h"
#include "./psservice.h"
#include "../../../base/apnsetting.h"
#include "psdatabuilder.h"
#include <librilutils/textutils.h>

PdpContext *PdpContextManager::GetAvailablePdpContext(PsService *psService, ApnSetting *pApnSetting, int dataProfileId) {
    if (pApnSetting == NULL) {
        return NULL;
    }

    // at first, try to find same data profile and same apn including attach PDP context
    RilLogV("[%s] Try to find reusable PDPContext", psService->GetServiceName());
    RilLogV("[-]Requested: Data ProfileId=%d, ApnSetting{apn=%s,apntype=%s}", dataProfileId, pApnSetting->GetApn(), pApnSetting->GetTypeString());
    for (int i = 0; i < psService->GetPdpContextSize(); i++) {
        PdpContext *pPdpContext = psService->GetPdpContextByIndex(i);
        if (pPdpContext != NULL) {
            const ApnSetting *pExistsApnSetting = pPdpContext->GetApnSetting();

            if (pExistsApnSetting != NULL) {
                RilLogV("[%d]PDPContext{cid=%d,Data Profile=%d,state=%d},ApnSetting{apn=%s,apntype=%s}",
                        i, pPdpContext->GetCID(), pPdpContext->GetDataProfileId(), pPdpContext->GetState(),
                        pExistsApnSetting->GetApn(), pExistsApnSetting->GetTypeString());
                if (pPdpContext->GetDataProfileId() == dataProfileId && TextUtils::Equals(pExistsApnSetting->GetApn(), pApnSetting->GetApn())) {
                    // do not change if selected PDP Context is attach PDP context
                    bool usedForAttach = true;
                    if (pPdpContext != psService->RetrieveAttachPdpContext()) {
                        pPdpContext->SetApnSetting(pApnSetting);
                        usedForAttach = false;
                    }
                    RilLogV("Available PDPContext(cid=%d), UsedForAttach(%d)", pPdpContext->GetCID(), usedForAttach);
                    return pPdpContext;
                }
            }
        }
    }

    // 2nd, find currently available PDP context
    RilLogV("[%s] Try to find Available PDPContext", psService->GetServiceName());
    for (int i = 0; i < psService->GetPdpContextSize(); i++) {
        PdpContext *pPdpContext = psService->GetPdpContextByIndex(i);
        if (pPdpContext != NULL) {
            RilLogV("[%d]PDPContext{cid=%d,state=%d}", i, pPdpContext->GetCID(), pPdpContext->GetState());
            if (pPdpContext->GetCID() == psService->GetAttachCid()) {
                continue;
            }

            if (pPdpContext->IsAvailable()) {
                pPdpContext->SetApnSetting(pApnSetting);
                pPdpContext->SetDataProfileId(dataProfileId);
                RilLogV("Available PDPContext(cid=%d)", pPdpContext->GetCID());
                return pPdpContext;
            }
        }
    }
    RilLogW("[%s] Not found available PDPContext", psService->GetServiceName());

    return NULL;
}

PdpContext *PdpContextManager::GetAvailablePdpContextforTD(PsService *psService, ApnSetting *pApnSetting,
                                                           int dataProfileId, const std::optional<DataV2_0::RIL_TrafficDescriptor> &tdInfo, bool /* matchedAll */) {
    if (pApnSetting == NULL || !tdInfo.has_value() || (tdInfo->osAppIds.empty() && tdInfo->dnn.empty())) {
        return NULL;
    }
    RilLogV("[%s] Try to find reusable PDPContext ~", psService->GetServiceName());

    for (int i = 0; i < psService->GetPdpContextSize(); i++) {
        PdpContext *pPdpContext = psService->GetPdpContextByIndex(i);
        if (pPdpContext != NULL) {
            const ApnSetting *pExistsApnSetting = pPdpContext->GetApnSetting();
            const std::optional<DataV2_0::RIL_TrafficDescriptor> &storedTd = pPdpContext->GetTdSetting();
            if (pExistsApnSetting != NULL && storedTd.has_value()) {
                if (TextUtils::Equals(pExistsApnSetting->GetApn(), pApnSetting->GetApn()) && tdInfo.has_value() && (tdInfo == storedTd)) {
                    RilLogV("[%d]PDPContext{cid=%d,Data Profile=%d,state=%d},ApnSetting{apn=%s,apntype=%s}",
                        i, pPdpContext->GetCID(), pPdpContext->GetDataProfileId(), pPdpContext->GetState(),
                        pExistsApnSetting->GetApn(), pExistsApnSetting->GetTypeString());
                    RilLogV("   storedTD: %s", storedTd->toString().c_str());

                    return pPdpContext;
                }
            }
        }
    } // end for i ~

    // 2nd, find currently available PDP context
    RilLogV("[%s] Try to find Available PDPContext", psService->GetServiceName());
    for (int i = 0; i < psService->GetPdpContextSize(); i++) {
        PdpContext *pPdpContext = psService->GetPdpContextByIndex(i);
        if (pPdpContext != NULL) {
            RilLogV("[%d]PDPContext{cid=%d,state=%d}", i, pPdpContext->GetCID(), pPdpContext->GetState());
            if (pPdpContext->GetCID() == psService->GetAttachCid()) {
                continue;
            }

            if (pPdpContext->IsAvailable()) {
                pPdpContext->SetApnSetting(pApnSetting);
                pPdpContext->SetTdSetting(tdInfo);
                pPdpContext->SetDataProfileId(dataProfileId);
                RilLogV("Available PDPContext(cid=%d)", pPdpContext->GetCID());
                return pPdpContext;
            }
        }
    }

    RilLogW("[%s] Not found available PDPContext", psService->GetServiceName());

    return NULL;
}

bool PdpContextManager::isPdpContextAvailable(PsService *psService, PdpContext *pPdpContext, Message *pMsg)
{
    if (pPdpContext == NULL) {
        RilLogE("No available PDP Context");
        psService->OnSetupDataCallComplete(RIL_E_GENERIC_FAILURE, NULL, pMsg);

        return false;
    }

    ApnSetting* pApnSetting = pPdpContext->GetApnSetting();
    RilLog("[%d] %s", psService->GetRilSocketId(), pApnSetting->ToString().c_str());
    if (!pPdpContext->IsAvailable()) {
        if (pPdpContext->isConnected()) {
            RilLogV("Already connected CID=%d, APN=%s", pPdpContext->GetCID(), pApnSetting->GetApn());
            int halVer = psService->getHalVersion(pMsg);
            PsDataBuilder builder(halVer);
            const RilData *rildata = builder.BuildSetupDataCallResponse(pPdpContext);
            if (rildata != NULL) {
                psService->OnRequestComplete(RIL_E_SUCCESS, rildata->GetData(), rildata->GetDataLength(), pMsg);
                delete rildata;
                return false;
            }
        }
        RilLogE("No available PDP Context. PDP Context State=%d", pPdpContext->GetState());
        // We can examine 'Not a DISCONNECTED' and 'Not a CONNECTED' Cases
        psService->OnSetupDataCallComplete(RIL_E_GENERIC_FAILURE, pPdpContext, pMsg);
        return false;
    }
    return true;
}
