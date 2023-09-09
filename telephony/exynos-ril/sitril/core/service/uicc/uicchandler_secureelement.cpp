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
 * secure_element.cpp
 *
 *  Created on: 2020. 7. 16.
 *      Author: sungwoo48.choi
 */

#include "uicchandler_secureelement.h"
#include "protocolsimbuilder.h"
#include "protocolsimadapter.h"
#include "rillog.h"
#include "simservice.h"

#define TIMEOUT_SE_DEFAULT      30000

/**
 * SeOpenChannelHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SeOpenChannelHandler, MSG_SIM_OEM_OPEN_CHANNEL)

int SeOpenChannelHandler::OnRequest(Message *msg)
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

    ProtocolSimBuilder builder;
    ModemData *pModemData = builder.BuildOemSimRequest(RIL_REQUEST_SIM_OPEN_CHANNEL,
            (BYTE *) rildata->GetRawData(), rildata->GetSize());
    nResult = SendRequest(pModemData, TIMEOUT_SE_DEFAULT, mMessageId);

    return (nResult < 0) ? -1 : 0;
}

int SeOpenChannelHandler::OnResponse(Message *msg)
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

    ProtocolSimResponseAdapter adapter(pModemData);
    UINT uErrCode = adapter.GetErrorCode();
    OnRequestComplete(uErrCode, (void *) adapter.GetParameter(), adapter.GetParameterLength());
    return 0;
}

/**
 * SeTransmitApduLogicalHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SeTransmitApduLogicalHandler,
        MSG_SIM_OEM_TRANSMIT_APDU_LOGICAL)

int SeTransmitApduLogicalHandler::OnRequest(Message *msg)
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

    ProtocolSimBuilder builder;
    ModemData *pModemData = builder.BuildOemSimRequest(RIL_REQUEST_SIM_TRANSMIT_APDU_CHANNEL,
            (BYTE *) rildata->GetRawData(), rildata->GetSize());
    nResult = SendRequest(pModemData, TIMEOUT_SE_DEFAULT, mMessageId);

    return (nResult < 0) ? -1 : 0;
}

int SeTransmitApduLogicalHandler::OnResponse(Message *msg)
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

    ProtocolSimResponseAdapter adapter(pModemData);
    UINT uErrCode = adapter.GetErrorCode();
    if(uErrCode==RIL_E_SUCCESS)
    {
        ProtocolSimTransmitApduChannelAdapter apduAdapter(pModemData);

        int nSW1 = apduAdapter.GetSw1();
        int nSW2 = apduAdapter.GetSw2();

        // ETSI TS 102.201, 7.2.2.3.1
        // SW1 : '6C' : The terminal shall wait for a second procedure byte then immediately
        // repeat the previous command header to the UICC using a length of
        // 'XX', where 'XX' is the value of the second procedure byte (SW2).
        if (nSW1==0x6C) {
            RilLogV("SW1:0x%02X, Retransmission with P3(%d:0x%X)", nSW1, nSW2, nSW2);

            Message *pCurrMsg = mService->GetCurrentMsg();
            if (pCurrMsg != NULL) {
                RawRequestData *rildata = (RawRequestData *)pCurrMsg->GetRequestData();
                if (rildata != NULL) {
                    char *pRawData = (char *)rildata->GetRawData();
                    memcpy(&pRawData[20/*p3*/], &nSW2, sizeof(int));
                    if(this->OnRequest(pCurrMsg)==-1) OnRequestComplete(RIL_E_GENERIC_FAILURE);
                }
            }
        } else {   // Not 0x6C
            OnRequestComplete(RIL_E_SUCCESS, (void *) adapter.GetParameter(),
                    adapter.GetParameterLength());
        }
    }
    else
    {
        OnRequestComplete(RIL_E_GENERIC_FAILURE);
    }
    return 0;
}

/**
 * GetUiccCardPresentHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(GetUiccCardPresentHandler, MSG_SIM_OEM_GET_CARD_PRESENT)

int GetUiccCardPresentHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    SimService *simService = SimService::AsService(mService);
    if (simService == NULL) {
        RilLogE("simService is NULL");
        return -1;
    }

    const V2_0::RIL_CardStatus rilCardStatus = simService->GetRilCardStatus();
    if (rilCardStatus.card_state != RIL_CARDSTATE_PRESENT) {
        ProtocolSimBuilder builder;
        ModemData *pModemData = builder.BuildSimGetStatus();
        int nResult = SendRequest(pModemData, TIMEOUT_SE_DEFAULT, mMessageId);
        return (nResult < 0) ? -1 : 0;
    }

    int nCardState = RIL_CARDSTATE_PRESENT;
    RilLogI("[%s] %s %s() nCardState : %d", mServiceName, TAG, __FUNCTION__, nCardState);

    RilDataInts *pRilData = new RilDataInts(1);
    if (pRilData != NULL) {
        pRilData->SetInt(0, nCardState);
        OnRequestComplete(RIL_E_SUCCESS, pRilData->GetData(), pRilData->GetDataLength());
        delete pRilData;
        pRilData = NULL;
    } else {
        RilLogE("Error: pRilData = NULL");
        OnRequestComplete(RIL_E_GENERIC_FAILURE);
    }

    return 0;
}

int GetUiccCardPresentHandler::OnResponse(Message *msg)
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

    ProtocolSimResponseAdapter adapter(pModemData);
    UINT uErrCode = adapter.GetErrorCode();
    if (uErrCode == RIL_E_SUCCESS) {
        ProtocolSimStatusAdapter adapter(pModemData);
        RIL_CardState eCardState = (RIL_CardState) adapter.GetCardState();

        int nCardState = RIL_CARDSTATE_PRESENT;
        if (eCardState != RIL_CARDSTATE_PRESENT) {
            nCardState = RIL_CARDSTATE_ABSENT;
        }

        RilLogI("[%s] %s %s() nCardState : %d", mServiceName, TAG, __FUNCTION__, nCardState);
        RilDataInts *pRilData = new RilDataInts(1);
        if (pRilData != NULL) {
            pRilData->SetInt(0, nCardState);
            OnRequestComplete(RIL_E_SUCCESS, pRilData->GetData(), pRilData->GetDataLength());
            delete pRilData;
        } else {
            RilLogE("Error: pRilData = NULL");
            OnRequestComplete(RIL_E_GENERIC_FAILURE);
        }
    } else {
        OnRequestComplete(RIL_E_GENERIC_FAILURE);
    }
    return 0;
}

/**
 * SeResetHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SeResetHandler, MSG_SIM_OEM_SE_RESET)

int SeResetHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    ProtocolSimBuilder builder;
    ModemData *pModemData = builder.BuildSetSimCardPower(3);
    int nResult = SendRequest(pModemData, TIMEOUT_SE_DEFAULT, mMessageId);
    return (nResult < 0) ? -1 : 0;
}

int SeResetHandler::OnResponse(Message *msg)
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

    SimService *simService = SimService::AsService(mService);
    if (simService == NULL) {
        RilLogE("simService is NULL");
        return -1;
    }

    ProtocolSimResponseAdapter adapter(pModemData);
    UINT uErrCode = adapter.GetErrorCode();
    OnRequestComplete(uErrCode);

    V2_0::RIL_CardStatus cardStatus = simService->GetRilCardStatus();
    int index = cardStatus.gsm_umts_subscription_app_index;
    RilLog("cardState : %d, appState: %d", cardStatus.card_state,
            cardStatus.applications[index].app_state);
    return 0;
}
