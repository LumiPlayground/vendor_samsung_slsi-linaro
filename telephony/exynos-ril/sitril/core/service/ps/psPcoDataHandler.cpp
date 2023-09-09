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
 * psPcoDataHandler.cpp
 *
 *  Created on: 2021. 2. 8.
 */

#include "pshandler.h"
#include "rillog.h"
#include "psdatabuilder.h"
#include "protocolpsadapter.h"
#include "psserviceproxy.h"
#include "base/rilapplication.h"
#include "message.h"

#include "pslog.h"

/*
 * PcoDataHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(PcoDataHandler, MSG_PS_IND_PCO_DATA)


int PcoDataHandler::OnIndication(Message *msg) {
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
    ProtocolPsPcoDataAdapter adapter(pModemData);
    PsDataBuilder builder(RilApplication::RIL_HalVersionCode);
    int cid = adapter.GetCid();
    int nPdpType = adapter.GetPdpType();
    int pcoNum = adapter.GetPcoNum();

    RilLogI("[%s] %s() : cid:%d, nPdpType:%d, pcoNum:%d",
            mServiceName, __FUNCTION__, cid, nPdpType, pcoNum);
    sit_pdp_pco_data_entry e[256];  // Available MAX Carrier specific PCO Block is 0xFF00~0xFFFF
    for (int i = pcoNum ; i > 0 ; i--) {
        int remainPcoBlocks = adapter.GetPcoData(e[i]);
        if (remainPcoBlocks < 0) {
            RilLogI("[%s] %s() : pconum:%d, cur:%d, remain:%d",
                    mServiceName, __FUNCTION__,
                    pcoNum, pcoNum - i, remainPcoBlocks);
            return 0;
        }

        const RilData *pRilData = builder.BuildPcoData(cid, nPdpType, e[i].pco_id, e[i].contents_len, e[i].contents);
        if (pRilData != NULL) {
            RilLogI("[%s] %s() : sending: pco_id:%x(%d), contents_len:%d, contents:%p",
                    mServiceName, __FUNCTION__,
                    e[i].pco_id, e[i].pco_id, e[i].contents_len, &e[i].contents);
            OnUnsolicitedResponse(RIL_UNSOL_PCO_DATA, pRilData->GetData(), pRilData->GetDataLength());
            delete pRilData;
        }
    }

    return 0;
}
