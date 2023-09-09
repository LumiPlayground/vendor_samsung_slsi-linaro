/*
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */
/*
 * PsSlicingConfigUpdatedHandler.cpp
 *
 *  Created on: 2022. 1. 5.
 */

#include "rillog.h"
#include "service.h"
#include "datacallreqdata.h"
#include "protocolpsadapter.h"
#include "pshandler.h"
#include "psserviceproxy.h"
#include "pslog.h"

/*
 * PsSlicingConfigUpdatedHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(PsSlicingConfigUpdatedHandler, MSG_PS_SLICING_CONFIG_UPDATED)


int PsSlicingConfigUpdatedHandler::OnIndication(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    PsServiceProxy psProxy(mService);
    if (!psProxy.HasService()) {
        return -1;
    }

    ModemData *pModemData = msg->GetModemData();

    if (pModemData == NULL) {
        return -1;
    }

    ProtocolPsSlicingConfigUpdated adapter(pModemData);
    // Send Direct data format through RIL
    DataV2_0::RIL_SlicingConfig slicingConfig = adapter.GetSlicingConfig();
    OnUnsolicitedResponse(RIL_UNSOL_IND_POST_URSP, ((void *)(&slicingConfig)), adapter.GetPayloadSize());

    return 0;

}
