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
 * radioconfighandler.cpp
 *
 *  Created on: 2020. 7. 13.
 *      Author: sungwoo48.choi
 */
#include "radioconfighandler.h"
#include "requestdata.h"
#include "protocoladapter.h"
#include "protocolradioconfigadapter.h"
#include "protocolradioconfigbuilder.h"
#include "protocolsimbuilder.h"
#include "radioconfigbuilder.h"
#include "rilapplication.h"
#include "rillog.h"
#include "simdata.h"
#include "simdatabuilder.h"
#include "service.h"
#include <librilutils/systemproperty.h>

#define TIMEOUT_RADIO_CONFIG_DEFAULT    5000

/**
 * GetPhoneCapabilityHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(GetPhoneCapabilityHandler, MSG_MISC_GET_PHONE_CAPABILITY)

int GetPhoneCapabilityHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    ProtocolRadioConfigBuilder builder;
    ModemData *pModemData = builder.BuildGetPhoneCapability();
    if (SendRequest(pModemData, TIMEOUT_RADIO_CONFIG_DEFAULT, mMessageId)) {
        return -1;
    }

    return 0;
}

int GetPhoneCapabilityHandler::OnResponse(Message *msg)
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

    int maxActiveData = 1;  // L+L modem it should be 2.
    int maxActiveInternetData = 1;  // DSDS(L+L) 1, DSDA 2
    bool isInternetLingeringSupported = false;
    int size = 2;
    int *logicalModemList = NULL;  // default: logicalModemList [0, 1]

    ProtocolPhoneCapabilityAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        // phone capability by modem
        maxActiveData = adapter.GetMaxActiveData();
        maxActiveInternetData = adapter.GetMaxActiveInternetData();
        isInternetLingeringSupported = false;
        size = adapter.GetLogicalModemListSize();
        logicalModemList = adapter.GetLogicalModemList();
        RilLogV("[PhoneCapability] maxActiveData=%d maxActiveInternetData=%d" \
                " isInternetLingeringSupported=%d logicalModemListSize=%d",
                maxActiveData, maxActiveInternetData, isInternetLingeringSupported, size);
    }
    else {
        // phone capability by platform
        // default: DSDS/Dual VoLTE
        bool isDualVolteEnabled =
                SystemProperty::GetInt(RIL_VENDOR_RADIO_DUAL_VOLTE, 0) == 1;
        if (isDualVolteEnabled) {
            maxActiveData = 2;
        }
        size = 2;
        RilLogV("[PhoneCapability] Use platform default: maxActiveData=%d maxActiveInternetData=%d" \
                " isInternetLingeringSupported=%d logicalModemListSize=%d",
                maxActiveData, maxActiveInternetData, isInternetLingeringSupported, size);
    }
    RadioConfigBuildler builder;
    const RilData *rildata = builder.BuildPhoneCapability(maxActiveData, maxActiveInternetData,
            isInternetLingeringSupported, size, logicalModemList);
    if (rildata != NULL) {
        OnRequestComplete(RIL_E_SUCCESS, rildata->GetData(), rildata->GetDataLength());
        delete rildata;
    }
    else {
        OnRequestComplete(RIL_E_INTERNAL_ERR);
    }

    return 0;
}

/**
 * SetModemConfigHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetModemConfigHandler, MSG_MISC_SET_MODEMS_CONFIG)

int SetModemConfigHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    IntRequestData *rildata = (IntRequestData *)msg->GetRequestData();
    if (rildata == NULL) {
        RilLogE("rildata is NULL");
        return -1;
    }

    int numOfLiveModems = rildata->GetInt();
    RilLogV("ModemsConfig=%d", numOfLiveModems);
    if (numOfLiveModems < 1 || numOfLiveModems > 2) {
        // VTS IRadioConfig@1.1 RadioConfigHidlTest#setModemsConfig_invalidArgument
        OnRequestComplete(RIL_E_INVALID_ARGUMENTS);
        return 0;
    }

    ProtocolRadioConfigBuilder builder;
    ModemData *pModemData = builder.BuildSetModemsConfig(numOfLiveModems);
    if (SendRequest(pModemData, TIMEOUT_RADIO_CONFIG_DEFAULT, mMessageId) < 0) {
        RilLogE("SendRequest error");
        // VTS IRadioConfig@1.1 RadioConfigHidlTest#setModemsConfig_goodRequest
        OnRequestComplete(RIL_E_REQUEST_NOT_SUPPORTED);
    }

    return 0;
}

int SetModemConfigHandler::OnResponse(Message *msg)
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

    ProtocolRespAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode != RIL_E_SUCCESS) {
        // VTS IRadioConfig@1.1 RadioConfigHidlTest#setModemsConfig_goodRequest
        // an allowed error code is among  RIL_E_SUCCESS and RIL_E_REQUEST_NOT_SUPPORTED.
        errorCode = RIL_E_REQUEST_NOT_SUPPORTED;
    }
    OnRequestComplete(errorCode);
    return 0;
}

/**
 * SimSlotStatusHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SimSlotStatusHandler, MSG_SIM_GET_SLOT_STATUS)

int SimSlotStatusHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    int nResult = -1;
    ProtocolRadioConfigBuilder builder;
    ModemData *pModemData = builder.BuildSimGetSlotStatus();
    nResult = SendRequest(pModemData, TIMEOUT_RADIO_CONFIG_DEFAULT, mMessageId);

    return (nResult<0)? -1: 0;
}

int SimSlotStatusHandler::OnResponse(Message *msg)
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

    ProtocolSimSlotStatusAdapter adapter(msg->GetModemData());
    UINT uErrCode = adapter.GetErrorCode();

    if(uErrCode == RIL_E_SUCCESS) {
        bool isLegacySlotStatus = adapter.IsLegacySlotStatus();
        if (isLegacySlotStatus) {
            RIL_SimSlotStatusResult_1_2 simSlotStatus = {};
            int nLength = adapter.FillSimSlotStatusLegacy(simSlotStatus);
            RilLogV("[OnGetSlotStatusDone - legacy] nLength: %d", nLength);
            OnRequestComplete(RIL_E_SUCCESS, (char *) &simSlotStatus, nLength);
        } else {
            configV2_0::RIL_SimSlotStatusResult simSlotStatus = {};
            int nLength = adapter.FillSimSlotStatus(simSlotStatus);
            RilLogV("[OnGetSlotStatusDone - aidl] nLength: %d", nLength);
            OnRequestComplete(RIL_E_SUCCESS, (char *) &simSlotStatus, nLength);
        }
    } else {
        RilLogE("[OnGetSlotStatusDone] uErrCode:%d", uErrCode);
        OnRequestComplete(uErrCode);
    }

    return 0;
}

/**
 * SimSlotStatusChangedHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SimSlotStatusChangedHandler, MSG_SIM_SLOT_STATUS_CHANGED)

int SimSlotStatusChangedHandler::OnIndication(Message *msg)
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

    ProtocolSlotStatusChangedAdapter adapter(msg->GetModemData());
    bool isLegacySlotStatus = adapter.IsLegacySlotStatus();

    if (isLegacySlotStatus) {
        RIL_SimSlotStatusResult_1_2 simSlotStatus = {};
        int nLength = adapter.FillSimSlotStatusLegacy(simSlotStatus);
        RilLogV("[OnUnsolSimSlotsStatusChanged - legacy] nLength: %d", nLength);
        OnUnsolicitedResponse(RIL_UNSOL_ICC_SLOT_STATUS, (char *) &simSlotStatus, nLength);
    } else {
        configV2_0::RIL_SimSlotStatusResult simSlotStatus = {};
        int nLength = adapter.FillSimSlotStatus(simSlotStatus);
        RilLogV("[OnUnsolSimSlotsStatusChanged - aidl] nLength: %d", nLength);
        OnUnsolicitedResponse(RIL_UNSOL_ICC_SLOT_STATUS, (char *) &simSlotStatus, nLength);
    }

    return 0;
}

/**
 * SimSlotMappingHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SimSlotMappingHandler, MSG_SIM_SET_LOGICAL_TO_PHYSICAL_SLOT_MAPPING)

int SimSlotMappingHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    int nResult = -1;

    ModemData *pModemData = NULL;
    int data_size = 0;
    SlotPortMapping *rildata = (SlotPortMapping *) msg->GetRequestData();
    if (rildata == NULL) {
        RilLogE("rildata is NULL");
        return -1;
    }

    int halVer = RilApplication::RIL_HalVersionCode;
    if (mService->GetCurrentRequestData() != NULL) {
        halVer = mService->GetCurrentRequestData()->GetHalVersion();
    }

    if (halVer < HAL_VERSION_CODE(2, 0)) {
        data_size = rildata->GetNumOfSlots();
        int *mappingData = new int[data_size];
        if (mappingData == NULL) {
            RilLogE("Creating mappingData is failed");
            return -1;
        }

        for (int i = 0; i < data_size; i++) {
            mappingData[i] = rildata->GetPhysicalSlotId(i);
            RilLogV("data[%d]: %d", i, mappingData[i]);
        }

        ProtocolRadioConfigBuilder builder;
        pModemData = builder.BuildSimSetLogicalSlotMapping(mappingData, data_size);

        if (mappingData != NULL)
            delete[] mappingData;
    } else {
        data_size = rildata->GetNumOfPorts();

        RilLogE("portSlotMapping size:[%d]", data_size);
        configV2_0::RIL_SlotPortMapping *mappingData =
                new configV2_0::RIL_SlotPortMapping[data_size];
        if (mappingData == NULL) {
            RilLogE("Creating mappingData is failed");
            return -1;
        }

        for (int i = 0; i < data_size; i++) {
            mappingData[i].physicalSlotId = rildata->GetPhysicalSlotId(i);
            mappingData[i].portId = rildata->GetPortId(i);
            RilLogV("index:[%d], physicalSlotId:[%d], portId[%d]",
                    i, mappingData[i].physicalSlotId, mappingData[i].portId);
        }
        ProtocolRadioConfigBuilder builder;
        pModemData = builder.BuildSimSetLogicalSlotPortMapping(mappingData, data_size);

        delete[] mappingData;
    }

    if (pModemData == NULL) {
        RilLogE("invalid argument");
        OnRequestComplete(RIL_E_INVALID_ARGUMENTS, NULL, 0);
        return 0;
    }

    nResult = SendRequest(pModemData, TIMEOUT_RADIO_CONFIG_DEFAULT, mMessageId);

    return (nResult < 0) ? -1 : 0;
}

int SimSlotMappingHandler::OnResponse(Message *msg)
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

    ProtocolRespAdapter adapter(pModemData);
    UINT uErrCode = adapter.GetErrorCode();
    if (uErrCode == RIL_E_SUCCESS) {
        OnRequestComplete(RIL_E_SUCCESS);
    }
    else if (uErrCode == RIL_E_CANCELLED) {
        RilLogI("Need to change from RIL_E_CANCELLED to RIL_E_SUCCESS");
        OnRequestComplete(RIL_E_SUCCESS);
    }
    else {
        OnRequestComplete(RIL_E_REQUEST_NOT_SUPPORTED);
    }

    return 0;
}

