/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef __PDPCONTEXT_MANAGER_H__
#define __PDPCONTEXT_MANAGER_H__

#include <optional>

class PdpContext;
class ApnSetting;
class PsService;
class Message;

namespace android::hardware::radio::impl::data::V2_0 {
    struct RIL_TrafficDescriptor;
}
namespace DataV2_0 = android::hardware::radio::impl::data::V2_0;

class PdpContextManager {
public:
    PdpContext *GetAvailablePdpContext(PsService *psService, ApnSetting *pApnSetting, int dataProfile);
    PdpContext *GetAvailablePdpContextforTD(PsService *psService,ApnSetting *pApnSetting, int dataProfileId, const std::optional<DataV2_0::RIL_TrafficDescriptor> &tdInfo, bool matchedAll);
    bool isPdpContextAvailable(PsService *psService, PdpContext *, Message *);
};

#endif  // __PDPCONTEXT_MANAGER_H__
