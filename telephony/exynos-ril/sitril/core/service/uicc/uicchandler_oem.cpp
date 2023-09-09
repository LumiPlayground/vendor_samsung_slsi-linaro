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
 * oem_uicchandler.cpp
 *
 *  Created on: 2020. 7. 16.
 *      Author: sungwoo48.choi
 */

#include "uicchandler_oem.h"
#include "protocolsimbuilder.h"
#include "protocolsimadapter.h"
#include "rillog.h"
#include "service.h"
#include "simdatabuilder.h"

#define TIMEOUT_OEM_UICC_DEFAULT        30000

/**
 * SimLockInfoHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(IccDepersonalizationHandler, MSG_SIM_OEM_ICC_DEPERSONALIZATION)

int IccDepersonalizationHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    int nResult = -1;
    RawRequestData *rildata = (RawRequestData *)msg->GetRequestData();
    if (rildata == NULL) {
        RilLogE("rildata is NULL");
        return -1;
    }

    char *data = (char *)rildata->GetRawData();
    unsigned int datalen = rildata->GetSize();
    if (data == NULL || datalen < sizeof(int) * 2) {
        return -1;
    }

    int nFac = *((int *)data);
    unsigned int size = *((int *)data + 1);
    if (size + sizeof(int) * 2 > datalen) {
        return -1;
    }

    char *pszCode = NULL;
    if (size > 0) {
        pszCode = new char[size + 1];
        memcpy(pszCode, (char *)data + (sizeof(int) * 2), size);
        pszCode[size] = '\0';
    }
    RilLog("[%d] %s nFac=%d pszCode length=%d", mService->GetRilSocketId(), __FUNCTION__, nFac, size);

    if (pszCode != NULL && strlen(pszCode) > 0) {
        ProtocolSimBuilder builder;
        ModemData *pModemData = builder.BuildSimVerifyNetworkLock(nFac, pszCode, 0, NULL);
        nResult = SendRequest(pModemData, TIMEOUT_OEM_UICC_DEFAULT, mMessageId);
    }

    if (pszCode != NULL)
        delete[] pszCode;

    return (nResult<0)? -1: 0;
}

int IccDepersonalizationHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    if(msg == NULL) {
        RilLogE("%s::%s() Message = NULL", mServiceName, __FUNCTION__);
        return -1;
    }

    ModemData *pModemData = msg->GetModemData();
    if (pModemData == NULL) {
        RilLogE("pModemData is NULL");
        return -1;
    }

    SimDataBuilder builder;
    ProtocolSimVerifyNetLockAdapter adapterSimVerifyNetLock(pModemData);
    UINT uErrCode = adapterSimVerifyNetLock.GetErrorCode();
    const RilData *pRilData = builder.BuildSimNetworkLockResponse(adapterSimVerifyNetLock.GetRemainCount());
    RilLogV("remain count : %d", adapterSimVerifyNetLock.GetRemainCount());
    if ( pRilData == NULL )
    {
        return OnRequestComplete(RIL_E_GENERIC_FAILURE);
    }

    if (uErrCode==RIL_E_SUCCESS)
        OnRequestComplete(RIL_E_SUCCESS, pRilData->GetData(), pRilData->GetDataLength());
    else
        OnRequestComplete(uErrCode, pRilData->GetData(), pRilData->GetDataLength());

    if (pRilData != NULL)
        delete pRilData;

    return 0;
}
