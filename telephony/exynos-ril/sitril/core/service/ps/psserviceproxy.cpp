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
 * PsServiceProxy.cpp
 *
 *  Created on: 2020. 8. 24.
 *      Author: juneho.kwon
 */

#include <psservice.h>
#include <psserviceproxy.h>
#include "PsTestHandler.h"


PsServiceProxy::PsServiceProxy(Service *service)
    : mService(NULL) {
    // mService should be not NULL
    mService = PsService::AsService(service);
}

PsServiceProxy::~PsServiceProxy() {
    mService = NULL;
}

int PsServiceProxy::OnRequestDeactDataCall(PdpContext *pPdpContext, int reason/* = DEACT_REASON_NORMAL*/) {
    if (HasService()) {
        return mService->OnRequestDeactDataCall(pPdpContext, reason);
    }
    return false;
}

int PsServiceProxy::OnRequestDeactDataCall(int cid, int reason/* = DEACT_REASON_NORMAL*/) {
    if (HasService()) {
        return mService->OnRequestDeactDataCall(cid, reason);
    }
    return false;
}

int PsServiceProxy::makePermanentFailureForIMS(PdpContext *pPdpContext) {
    if (HasService()) {
        return mService->makePermanentFailureForIMS(pPdpContext);
    }
    return false;
}

int PsServiceProxy::isSameTargetOperator(int targetOperator) {
    if (HasService()) {
        return mService->isSameTargetOperator(targetOperator);
    }
    return false;
}

int PsServiceProxy::OnSimpleRequestDone(Message *msg) {
    if (HasService()) {
        return mService->OnSimpleRequestDone(msg);
    }
    return false;
}

BOOL PsServiceProxy::resetPdpContext(bool bKeepAttachApn) {
    if (HasService()) {
        return mService->ResetPdpContext(bKeepAttachApn);
    }
    return false;
}

BOOL PsServiceProxy::cleanUpPdpOnInactiveDc(const DataCall *dc) {
    if (HasService()) {
        return mService->cleanUpPdpOnInactiveDc(dc);
    }
    return false;
}

BOOL PsServiceProxy::updatePdpOnActiveDc(const DataCall *dc) {
    if (HasService()) {
        return mService->updatePdpOnActiveDc(dc);
    }
    return false;
}

void PsServiceProxy::cleanUpInactivePdp(PdpContext *pPdpContext) {
    if (HasService()) {
        mService->cleanUpInactivePdp(pPdpContext);
    }
}

void PsServiceProxy::OnNotifyDataCallList() {
    if (HasService()) {
        mService->OnNotifyDataCallList();
    }
}

void PsServiceProxy::OnDeactDataCallComplete(PdpContext *pPdpContext) {
    if (HasService()) {
        mService->OnDeactDataCallComplete(pPdpContext);
    }
}

void PsServiceProxy::setDeactivatingPdpContext(PdpContext * pdpContext) {
    if (HasService()) {
        mService->setDeactivatingPdpContext(pdpContext);
    }
}

int PsServiceProxy::getPdpContextSize() {
    if (HasService()) {
        return mService->m_nPdpContextSize;
    }
    return false;
}

template<typename T>
BOOL PsServiceProxy::getRilRequestData(Message *pMsg, T *(&rildata)) {
    if (HasService()) {
        return mService->GetRilRequestData(pMsg, rildata);
    }
    return false;
}

PdpContext * PsServiceProxy::getDeactivatingPdpContext() {
    if (HasService()) {
        return mService->m_pDeactivatingPdpContext;
    }
    return NULL;
}

PdpContext * PsServiceProxy::getPdpContext(int cid) {
    if (HasService()) {
        return mService->GetPdpContext(cid);
    }
    return NULL;
}

PdpContext * PsServiceProxy::getPdpContextByIndex(int index) {
    if (HasService()) {
        return mService->GetPdpContextByIndex(index);
    }
    return NULL;
}

RilProperty * PsServiceProxy::getRilContextProperty() {
    if (HasService()) {
        return mService->GetRilContextProperty();
    }
    return NULL;
}

RilProperty * PsServiceProxy::GetRilApplicationProperty() {
    if (HasService()) {
        return mService->GetRilApplicationProperty();
    }
    return NULL;
}

PsTestHandler * PsServiceProxy::getPsTestHandler() {
    if (HasService()) {
        return mService->m_pTestHandler;
    }
    return NULL;
}

void PsServiceProxy::printDataProfileTable()
{
    if (HasService())
    {
        mService->printDataProfileTable();
    }
}

void PsServiceProxy::clearDataProfileTable()
{
    if (HasService())
    {
        mService->clearDataProfileTable();
    }
}

void PsServiceProxy::addDataProfile(const DataV2_0::RIL_DataProfileInfo &dpi)
{
    if (HasService())
    {
        mService->addDataProfile(dpi);
    }
}

DataV2_0::RIL_DataProfileInfo * PsServiceProxy::findDataProfile(const char *apn)
{

    if (HasService())
    {
        return mService->findDataProfile(apn);
    }

    return NULL;
}

void PsServiceProxy::SetCurRrcState(bool state) {
    if (HasService()) {
        mService->SetCurRrcState(state);
    }
}

bool PsServiceProxy::IsRrcConState() {
    if (HasService()) {
        return mService->IsRrcConState();
    }
    return false;
}

