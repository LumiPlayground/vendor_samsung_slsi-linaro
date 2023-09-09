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
 * ril_external_manager.cpp
 *
 *  Created on: 2020. 2. 10.
 */
#define LOG_TAG "RIL_EXTERNAL"

#include <utils/Log.h>
#include "ril_external_manager.h"
#include "ril_external_service.h"

using namespace vendor::samsung_slsi::telephony::hardware::radioExternal::V1_0;
using namespace vendor::samsung_slsi::telephony::hardware::radioExternal;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using android::sp;

static RilExternalManager *instance;

RilExternalManager *RilExternalManager::getInstance()
{
    if (instance == NULL) {
        instance = new RilExternalManager();
    }
    return instance;
}

RilExternalManager::RilExternalManager()
{

}

RilExternalClient *RilExternalManager::getClient(int32_t clientId)
{
    RilExternalClient *ret = NULL;
    for (std::list<RilExternalClient *>::iterator it = mClientList.begin();
            it != mClientList.end(); ++it) {
        if ((*it)->getClientId() == clientId) {
            ret = *it;
            break;
        }
    } // end for ~

    return ret;
}

RilExternalClient *RilExternalManager::allocateClient(std::string name)
{
    RilExternalClient *newClient = RilExternalClient::newInstance(name);
    return newClient;
}

void RilExternalManager::addClient(RilExternalClient *client)
{
    if (client != NULL) {
        mClientList.push_back(client);
    }
}

void RilExternalManager::removeClient(int32_t clientId)
{
    for (std::list<RilExternalClient *>::iterator it = mClientList.begin();
            it != mClientList.end(); ++it) {
        RilExternalClient *p = *it;
        if (p->getClientId() == clientId) {
            mClientList.erase(it);
            delete p;
            break;
        }
    } // end for ~
}

void RilExternalManager::removeClient(RilExternalClient *client)
{
    if (client != NULL) {
        removeClient(client->getClientId());
    }
}

RilExternalClient *RilExternalManager::onNewClientConnect(
        const sp<IOemSlsiRadioExternalRes>& oemResponse,
        const sp<IOemSlsiRadioExternalInd>& oemIndication,
        std::string name)
{
#ifdef VDBG
    RLOGD("%s", __FUNCTION__);
#endif
    RilExternalClient *newClient = allocateClient(name);
    if (newClient != NULL) {
        RwlockHolder &lock = getRwlock();
        lock.rwlock();

        newClient->setResponseFunctions(oemResponse, oemIndication);
        if(newClient->getClientId() < 0) {
            RLOGW("setResponseFunctions fail");
            delete newClient;
            newClient = NULL;
        }
        else {
            addClient(newClient);
        }
        lock.unlock();
    }

    if (newClient != NULL) {
        onNewCommandConnect(newClient);
        RLOGD("onNewClientConnect %s", newClient->toString().c_str());
    }
    return newClient;
}

void RilExternalManager::clearResponseFunctions(int32_t clientId)
{
#ifdef VDBG
    RLOGD("%s", __FUNCTION__);
#endif
    RwlockHolder &lock = getRwlock();
    lock.rwlock();

    RilExternalClient *client = getClient(clientId);
    if (client != NULL) {
        client->clear();
        removeClient(client);
    }

    lock.unlock();
}

void RilExternalManager::setIndicationFilter(int32_t clientId, const hidl_vec<int32_t>& indicationFilter)
{
#ifdef VDBG
    RLOGD("%s", __FUNCTION__);
#endif
    RwlockHolder &lock = getRwlock();
    lock.rwlock();

    RilExternalClient *client = getClient(clientId);
    if (client != NULL) {
        client->setIndicationFilter(indicationFilter);
    }

    lock.unlock();
}

void RilExternalManager::reset()
{
    RwlockHolder &lock = getRwlock();
    lock.rwlock();

    for (std::list<RilExternalClient *>::iterator it = mClientList.begin();
            it != mClientList.end(); ++it) {
        RilExternalClient *p = *it;
        delete p;
    } // end for ~
    mClientList.clear();

    lock.unlock();
}

void RilExternalManager::sendRequestRawResponse(int32_t clientId, RadioExternalResponseInfo &responseInfo,
        void *response, size_t responseLen)
{
#ifdef VDBG
    RLOGD("%s", __FUNCTION__);
#endif
    RwlockHolder &lock = getRwlock();
    lock.rwlock();

    RilExternalClient *client = getClient(clientId);
    if (client != NULL) {
        if (!client->sendRequestRawResponse(responseInfo, response, responseLen)) {
            removeClient(client);
        }
    }

    lock.unlock();
}

void RilExternalManager::rilExternalRawIndication(int32_t clientId, int indication,
        int slotId, const void *unsolResp, size_t responseLen)
{
#ifdef VDBG
    RLOGD("%s", __FUNCTION__);
#endif
    RwlockHolder &lock = getRwlock();
    lock.rwlock();
    RilExternalClient *client = getClient(clientId);
    lock.unlock();

    rilExternalRawIndication(client, indication, slotId, unsolResp, responseLen);
}

void RilExternalManager::rilExternalRawIndication(RilExternalClient *client, int indication,
        int slotId, const void *unsolResp, size_t responseLen)
{
    RwlockHolder &lock = getRwlock();
    lock.rwlock();

    if (client != NULL) {
        if (!client->rilExternalRawIndication(indication, slotId, unsolResp, responseLen)) {
            removeClient(client);
        }
    }

    lock.unlock();
}

void RilExternalManager::broadcastIndication(int indication, int slotId,
        const void *unsolResp, size_t responseLen, bool isFiltered/* = false*/)
{
    RwlockHolder &lock = getRwlock();
    lock.rwlock();

    std::list<RilExternalClient *> dests;
    for (std::list<RilExternalClient *>::iterator it = mClientList.begin();
            it != mClientList.end(); ++it) {
        RilExternalClient *p = *it;
        if (p != NULL &&
           (!isFiltered || !p->isFilteredIndication(indication))) {
            dests.push_back(p);
        }
    } // end for ~

    broadcastIndication(dests, indication, slotId, unsolResp, responseLen);

    lock.unlock();
}

void RilExternalManager::broadcastIndication(std::list<RilExternalClient *> &clients,
        int indication, int slotId, const void *unsolResp, size_t responseLen)
{
    for (std::list<RilExternalClient *>::iterator it = clients.begin();
            it != clients.end(); ++it) {
        RilExternalClient *p = *it;
        if (p != NULL) {
            if (!p->rilExternalRawIndication(indication, slotId, unsolResp, responseLen)) {
                removeClient(p);
            }
        }
    } // end for
}

void RilExternalManager::onNewCommandConnect(RilExternalClient *client)
{
    // notify a connected event
    rilExternalRawIndication(client, RILC_UNSOL_CONNECTED, 0, NULL, 0);

    // notify the current overall radio state
    int phoneCount = GetPhoneCount();
    for (int i = 0; i < phoneCount; i++) {
        int radioState = radioExternal::getOverallRadioState(i);
        rilExternalRawIndication(client, RILC_UNSOL_RADIO_STATE_CHANGED,
                i, &radioState, sizeof(radioState));
    }  // end for i ~
}

RequestRawSegmentData *RilExternalManager::allocateBuffer(int serial, int clientId,
        int requestId, int slotId, size_t totalLen)
{
    RilExternalClient *client = getClient(clientId);
    if (client != NULL) {
        return client->allocateSegmentBuffer(serial, requestId, slotId, totalLen);
    }
    return NULL;
}
