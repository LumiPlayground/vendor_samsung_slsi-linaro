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
 * ril_external_manager.h
 *
 *  Created on: 2020. 2. 10.
 */

#ifndef __RIL_EXTERNAL_MANAGER_H__
#define __RIL_EXTERNAL_MANAGER_H__

#include <vendor/samsung_slsi/telephony/hardware/radioExternal/1.1/IOemSlsiRadioExternal.h>
#include "ril_external_client.h"
#include "rwlockholder.h"

#include <list>
#include <string>

class RilExternalManager {
private:
    std::list<RilExternalClient *> mClientList;
    RwlockHolder mRwlock;

public:
    RilExternalManager();
public:
    RilExternalClient *onNewClientConnect(
            const android::sp<vendor::samsung_slsi::telephony::hardware::radioExternal::V1_0::IOemSlsiRadioExternalRes>& oemResponse,
            const android::sp<vendor::samsung_slsi::telephony::hardware::radioExternal::V1_0::IOemSlsiRadioExternalInd>& oemIndication,
            std::string name);
    void clearResponseFunctions(int32_t clientId);
    void setIndicationFilter(int32_t clientId, const android::hardware::hidl_vec<int32_t>& indicationFilter);
    void sendRequestRawResponse(int32_t clientId, vendor::samsung_slsi::telephony::hardware::radioExternal::V1_0::RadioExternalResponseInfo &responseInfo, void *response, size_t responseLen);
    void rilExternalRawIndication(int32_t clientId, int indication, int slotId, const void *unsolResp, size_t responseLen);
    void broadcastIndication(int indication, int slotId, const void *unsolResp, size_t responseLen, bool isFiltered = false);
    RequestRawSegmentData *allocateBuffer(int serial, int clientId, int requestId, int slotId, size_t totalLen);

private:
    RwlockHolder& getRwlock() { return mRwlock; }
    RilExternalClient *getClient(int32_t clientId);
    RilExternalClient *allocateClient(std::string name);
    void addClient(RilExternalClient *client);
    void removeClient(int32_t clientId);
    void removeClient(RilExternalClient *client);
    void rilExternalRawIndication(RilExternalClient *client, int indication, int slotId, const void *unsolResp, size_t responseLen);
    void broadcastIndication(std::list<RilExternalClient *> &clients, int indication, int slotId,
            const void *unsolResp, size_t responseLen);
    void reset();
    void onNewCommandConnect(RilExternalClient *client);

public:
    static RilExternalManager *getInstance();
};

#endif /* __RIL_EXTERNAL_MANAGER_H__ */
