/* Copyright Samsung Electronics Co., LTD.

 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#include "PsSlicingConfigHandler.h"
#include "rillog.h"
#include "psservice.h"
#include "PsTimeoutConfig.h"
#include "protocolpsbuilder.h"
#include "protocolpsadapter.h"
#include "PsTestHandler.h"
#include <librilutils/systemproperty.h>

/*
 * This value is self-managed version information with CP.
 * Each version has different parameters given by CP.
 * Whenever a parameter given by CP is added, the version changes.
 */
#define SLICING_CONFIG_VERSION_V2 2
#define SLICING_CONFIG_VERSION_V1 1


PsSlicingConfigHandler::PsSlicingConfigHandler()
{
    m_pPsService = NULL;
}

PsSlicingConfigHandler::~PsSlicingConfigHandler()
{
}


void PsSlicingConfigHandler::Init(PsService* pPsService)
{
    if(pPsService != NULL)
        m_pPsService = pPsService;
}

int PsSlicingConfigHandler::DoSlicingConfig(Message *pMsg)
{
    RilLogI("[%s] %s()", m_pPsService->GetServiceName(), __FUNCTION__);

    if (!isValidMessage(pMsg)) return -1;

    RequestData *rildata = (RequestData *)pMsg->GetRequestData();
    if (rildata == NULL) {
        RilLogE("rildata is NULL");
        return -1;
    }

    int halVer = rildata->GetHalVersion();

    // TestLoopback with mock unsol response
    int testmode = SystemProperty::GetInt(RIL_VENDOR_RADIO_TEST_SLICING_CONFIG, 0);
    if (testmode != 0) {
        ModemData *pModemData = PsTestHandler::generateModemDataForTest(testmode + 2);
        ProtocolPsSlicingConfig adapter(pModemData);
        DataV2_0::RIL_SlicingConfig slicingConfig = adapter.GetSlicingConfig();
        int errorCode = adapter.GetErrorCode();
        if (errorCode == RIL_E_SUCCESS) {
            m_pPsService->OnRequestComplete(RIL_E_SUCCESS, ((void *)(&slicingConfig)), adapter.GetPayloadSize());
            // Generate IND
            m_pPsService->OnUnsolicitedResponse(RIL_UNSOL_IND_POST_URSP, ((void *)(&slicingConfig)), adapter.GetPayloadSize());
        } else {
            m_pPsService->OnRequestComplete(RIL_E_REQUEST_NOT_SUPPORTED);
        }
        return 0;
    }
    ProtocolPsBuilder builder;
    ModemData *pModemData;
    if (halVer >= HAL_VERSION_CODE(2, 0)) {
        pModemData = builder.BuildSlicingConfig(SLICING_CONFIG_VERSION_V2);
    } else {
        // Assume GRF/VF on HAL 1.6
        pModemData = builder.BuildSlicingConfig(SLICING_CONFIG_VERSION_V1);
    }

    if (m_pPsService->SendRequest(pModemData, TIMEOUT_DEFAULT_PS, MSG_PS_SLICING_CONFIG_DONE) < 0) {
        return -1;
    }

    return 0;
}

int PsSlicingConfigHandler::OnSlicingConfigDone(Message *pMsg)
{
    RilLogI("[%s] %s()", m_pPsService->GetServiceName(), __FUNCTION__);

    if (!isValidRespMessage(pMsg)) return -1;
    ModemData *pModemData = pMsg->GetModemData();

    ProtocolPsSlicingConfig adapter(pModemData);

    DataV2_0::RIL_SlicingConfig slicingConfig = adapter.GetSlicingConfig();

    int errorCode = adapter.GetErrorCode();

    if (errorCode == RIL_E_SUCCESS) {
        m_pPsService->OnRequestComplete(RIL_E_SUCCESS, ((void *)(&slicingConfig)), adapter.GetPayloadSize());
        int testmode = SystemProperty::GetInt(RIL_VENDOR_RADIO_TEST_POST_URSP_WITH_RSP, 0);
        if (testmode != 0) {
            m_pPsService->OnUnsolicitedResponse(RIL_UNSOL_IND_POST_URSP, ((void *)(&slicingConfig)), adapter.GetPayloadSize());
        }
    } else if(errorCode == RIL_E_REQUEST_NOT_SUPPORTED) {
        m_pPsService->OnRequestComplete(RIL_E_MODEM_ERR);
    } else {
        m_pPsService->OnRequestComplete(RIL_E_REQUEST_NOT_SUPPORTED);
    }
    return 0;
}

bool PsSlicingConfigHandler::isValidMessage(Message *pMsg)
{
    if (pMsg == NULL) {
        RilLogE("pMsg is NULL");
        return false;
    }
    return true;
}

bool PsSlicingConfigHandler::isValidRespMessage(Message *pMsg)
{
    if (!isValidMessage(pMsg)) return false;

    ModemData *pModemData = pMsg->GetModemData();
    if (pModemData == NULL) {
        RilLogE("pModemData is NULL");
        return false;
    }
    return true;
}
