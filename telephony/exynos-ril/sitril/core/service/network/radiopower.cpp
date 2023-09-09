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
 * radiopower.cpp
 *
 *  Created on: 2020. 7. 17.
 *      Author: sungwoo48.choi
 */

#include "radiopower.h"
#include "cscservice.h"
#include "networkservice.h"
#include "networkserviceproxy.h"
#include "protocoladapter.h"
#include "protocolnetadapter.h"
#include "protocolnetbuilder.h"
#include "rillog.h"
#include "commondef.h"

#define TIMEOUT_NET_SET_RADIO_POWER                 30000
#define TIMEOUT_NET_GET_RADIO_POWER                 5000

/**
 * SetRadioPowerHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetRadioPowerHandler, MSG_NET_RADIO_POWER)

int SetRadioPowerHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    IntsRequestData *rildata = (IntsRequestData *)msg->GetRequestData();
    if (rildata == NULL) {
        RilLogE("rildata is NULL");
        return -1;
    }

    NetworkServiceProxy network(mService);
    if (!network.HasService()) {
        return -1;
    }

    if (network.IsRadioShutdown()) {
        RilLogE("[%s] Under Shutting down, ignore radio power commands", mServiceName);
        return -1;
    }

    BOOL forEmergencyCall = FALSE;
    BOOL preferredForEmergencyCall = FALSE;
    BOOL desiredPowerState = rildata->GetInt(0) > 0 ? TRUE : FALSE;
    if (rildata->GetSize() > 2 && desiredPowerState == TRUE) {
        forEmergencyCall = rildata->GetInt(1) > 0 ? TRUE : FALSE;
        preferredForEmergencyCall = rildata->GetInt(2) > 0 ? TRUE : FALSE;
    }

    int radioState = network.GetRadioState();
    int oldDesiredRadioState = network.GetDesiredRadioState();
    int desiredRadioState = (desiredPowerState ? RADIO_STATE_ON : RADIO_STATE_OFF);
    network.SetDesiredRadioState(desiredRadioState);

    RilLogV("[%d] radioState is %s(%d)", mPhoneId,
            (radioState == RADIO_STATE_ON) ?
                    "RADIO_STATE_ON" : "RADIO_STATE_OFF_OR_UNAVAILABLE",
            radioState);
    RilLogV("[%d] oldDesiredRadioState is %s(%d)", mPhoneId,
            (oldDesiredRadioState == RADIO_STATE_ON) ?
                    "RADIO_STATE_ON" : "RADIO_STATE_OFF_OR_UNAVAILABLE",
            oldDesiredRadioState);
    RilLogV("[%d] m_desiredRadioState is %s(%d)", mPhoneId,
            (desiredRadioState == RADIO_STATE_ON) ?
                    "RADIO_STATE_ON" : "RADIO_STATE_OFF_OR_UNAVAILABLE",
            desiredRadioState);

    // CP is not ready yet
    bool radioUnavailable = (network.GetRadioReady() != RADIO_READY) ||
                            (radioState == RADIO_STATE_UNAVAILABLE);
    if (radioUnavailable) {
        RilLogW("Not ready to request Radio Power state");
        network.SetDelayedRadioPower(true);
        OnRequestComplete(RIL_E_RADIO_NOT_AVAILABLE);
        return 0;
    }

    bool requestRadioPower = ((radioState == RADIO_STATE_OFF && desiredPowerState)
                             || (radioState == RADIO_STATE_ON && !desiredPowerState)) &&
                             (oldDesiredRadioState != desiredRadioState);

    if (!requestRadioPower) {
        OnRequestComplete(RIL_E_SUCCESS);

        // notify the current enabling state to synchronize
        // a radio state between RIL and framework
        OnUnsolicitedResponse(RIL_UNSOL_RESPONSE_RADIO_STATE_CHANGED, &desiredRadioState,
                sizeof(desiredRadioState));
        return 0;
    }

    ProtocolRadioPowerBuilder builder;
    ModemData *pModemData = builder.BuildRadioPower(desiredPowerState, forEmergencyCall, preferredForEmergencyCall);
    if (SendRequest(pModemData, TIMEOUT_NET_SET_RADIO_POWER, mMessageId) < 0) {
        RilLogE("SendRequest error");
        network.SetDesiredRadioState(RADIO_STATE_OFF);
        network.SetDelayedRadioPower(true);
        return -1;
    }

    struct {
        INT32 powerState;
        BYTE forEmergency;
        BYTE preferredForEmergency;
    } radio_state_ind_data;
    radio_state_ind_data.powerState = rildata->GetInt(0) == 0 ? SIT_PWR_RADIO_STATE_STOP_NETWORK :
        SIT_PWR_RADIO_STATE_START_NETWORK;
    radio_state_ind_data.forEmergency = forEmergencyCall;
    radio_state_ind_data.preferredForEmergency = desiredPowerState;
    OnUnsolicitedResponse(RIL_UNSOL_OEM_AIMS_RADIO_POWER_STATE, &radio_state_ind_data,
            sizeof(radio_state_ind_data));

    // Update the flag to prepare emergency call.
    // EmergencyCallHelper will be triggered.
    if (forEmergencyCall && desiredRadioState == RADIO_STATE_ON) {
        NetworkServiceProxy network(mService);
        if (network.HasService()) {
            CscService* pCscService = network.FindCscService();
            if (pCscService != NULL) {
                pCscService->SetRadioOnForEmergencyCall(true);
            }
        }
    }

    return 0;
}

int SetRadioPowerHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    ModemData *pModemData = msg->GetModemData();
    if (pModemData == NULL) {
        RilLogE("pModemData is NULL");
        return -1;
    }

    ProtocolRadioPowerAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    OnRequestComplete(errorCode);

    return 0;
}

/**
 * GetRadioStateHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(GetRadioStateHandler, MSG_NET_GET_RADIO_STATE)

int GetRadioStateHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    NetworkService *network = NetworkService::AsService(mService);
    if (network == NULL) {
        return -1;
    }

    if (network->GetRadioReady() == RADIO_READY) {
        RIL_RadioState radioState = network->GetRadioState();
        OnRequestComplete(RIL_E_SUCCESS, &radioState, sizeof(radioState));
        return 0;
    }

    ProtocolRadioPowerBuilder builder;
    ModemData *pModemData = builder.BuildGetRadioState();
    if (SendRequest(pModemData, TIMEOUT_NET_GET_RADIO_POWER, mMessageId) < 0) {
        return -1;
    }

    return 0;
}

int GetRadioStateHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    ModemData *pModemData = msg->GetModemData();
    if (pModemData == NULL) {
        RilLogE("pModemData is NULL");
        return -1;
    }

    NetworkServiceProxy network(mService);
    if (!network.HasService()) {
        return -1;
    }

    ProtocolGetRadioStateRespAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        // set radio ready state to RADIO_READY
        network.SetRadioReady(RADIO_READY);

        int radioState = adapter.GetRadioState();
        network.UpdateRadioState((RIL_RadioState)radioState, true);
        OnRequestComplete(RIL_E_SUCCESS, &radioState, sizeof(radioState));
    }
    else {
        OnRequestComplete(RIL_E_MODEM_ERR);
    }
    return 0;
}

int GetRadioStateHandler::OnTimeout(Message * /* msg */)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    OnRequestComplete(RIL_E_MODEM_ERR);
    return 0;
}

/**
 * RadioStateChangedHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(RadioStateChangedHandler, MSG_NET_RADIO_STATE_CHANGED)

int RadioStateChangedHandler::OnIndication(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    ModemData *pModemData = msg->GetModemData();
    if (pModemData == NULL) {
        RilLogE("pModemData is NULL");
        return -1;
    }

    NetworkServiceProxy network(mService);
    if (!network.HasService()) {
        return -1;
    }

    ProtocolRadioStateAdapter adapter(pModemData);
    int radioState = adapter.GetRadioState();
    if (radioState >= 0) {
        network.UpdateRadioState((RIL_RadioState)radioState, true);
    }

    return 0;
}

/**
 * ShutdownHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(ShutdownHandler, MSG_NET_SHUTDOWN)

int ShutdownHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    NetworkServiceProxy network(mService);
    if (!network.HasService()) {
        return -1;
    }

    ProtocolRadioPowerBuilder builder;
    ModemData *pModemData = builder.BuildShutdown();
    if (SendRequest(pModemData, TIMEOUT_NET_SET_RADIO_POWER, mMessageId) < 0) {
        return -1;
    }

    network.SetRadioShutdown(true);
    return 0;
}

int ShutdownHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    ModemData *pModemData = msg->GetModemData();
    if (pModemData == NULL) {
        RilLogE("pModemData is NULL");
        return -1;
    }

    NetworkServiceProxy network(mService);
    if (!network.HasService()) {
        return -1;
    }

    // Telephony framework doesn't care the result error code
    ProtocolRespAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    OnRequestComplete(errorCode);

    // update radio state to RADIO_STATE_UNAVAILABLE by force
    // it's a final message during phone goes to power-off.
    network.UpdateRadioState(RADIO_STATE_UNAVAILABLE, true);
    return 0;
}
