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
 * carrierconfighandler.cpp
 *
 *  Created on: 2021. 1. 14.
 *      Author: ji_young.bae
 */
#include "carrierconfighandler.h"
#include "carrierconfig/carrierconfigmanager.h"
#include <string>
#include "protocoladapter.h"
#include "protocolmiscadapter.h"
#include "protocolmiscbuilder.h"
#include "rillog.h"
#include "service.h"
#include "sitdef.h"
#include <librilutils/textutils.h>

#define TIMEOUT_CARRIERCONFIG_DEFAULT       50000

#define CHECK_NULL_MSG(msg) { if(msg==NULL) { \
                                  RilLogE("[%s] %s %s() msg is NULL", mServiceName, TAG, __FUNCTION__); return -1; } }
#define CHECK_NULL_REQ(msg) { if(msg==NULL || msg->GetRequestData()==NULL) {\
                                  RilLogE("[%s] %s %s() msg or RequestData is NULL", mServiceName, TAG, __FUNCTION__); return -1; } }
#define CHECK_NULL_RSP(msg) { if(msg==NULL || msg->GetModemData()==NULL) {\
                                  RilLogE("[%s] %s %s() msg or ModemData is NULL", mServiceName, TAG, __FUNCTION__); return -1; } }

static void saveGidToProperty(RilProperty *pProperty, string gid1) {
    if (pProperty != NULL) {
        RilLogV("%s() GID1=%s", __FUNCTION__, gid1.c_str());
        pProperty->Put(RIL_CONTEXT_SIM_GID1, gid1.c_str());
    }
}

/**
 * SetCpCarrierConfigHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetCpCarrierConfigHandler, MSG_MISC_SET_CP_CARRIER_CONFIG)

int SetCpCarrierConfigHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    RawRequestData *rildata = (RawRequestData *)msg->GetRequestData();

    ModemData modemData((char*) rildata->GetRawData(), rildata->GetSize());
    ProtocolMiscCarrierConfigSimInfoIndAdapter data(&modemData);

    saveGidToProperty(mService->GetRilContextProperty(), data.getGid1());

    CarrierManifest carrier = {{ManifestProperty::MCCMNC_TUPLE, data.getMccMnc()},
                               {ManifestProperty::IMSI, data.getImsi()},
                               {ManifestProperty::SPN, data.getSpn()},
                               {ManifestProperty::GID1, data.getGid1()},
                               {ManifestProperty::GID2, data.getGid2()},
                               {ManifestProperty::ICCID, data.getIccid()},
                               };

    CarrierConfigManager *manager = CarrierConfigManager::GetInstance();
    if (manager != NULL) {
        const char *configDir = manager->GetCurrentConfigDir();
        carrier = manager->GetCarrierManifest(carrier);
        RilLogI("carrier=%s", carrier.toString().c_str());

        if (CarrierConfigManager::IsValidCarrierManifest(carrier)) {
            ProtocolCarrierConfigBuilder builder;
            auto encodedManifest = carrier.getEncodedManifest();
            ModemData *modemData = builder.BuildSetCpCarrierConfig(
                    configDir, encodedManifest.data(), encodedManifest.size());
            RilLogI("dir=%s", configDir);

            if (SendRequest(modemData, TIMEOUT_CARRIERCONFIG_DEFAULT, mMessageId) < 0) {
                RilLogE("SendRequest error");
                return -1;
            }
            return 0;
        }
        else {
            RilLogW("[%d] Invalid CarrierManifest for %s", mService->GetRilSocketId(), carrier.toString().c_str());
        }
    }
    else {
        RilLogW("[%d] %s CarrierConfigManager not initialized", mService->GetRilSocketId(), __FUNCTION__);
    }

    OnRequestComplete(RIL_E_REQUEST_NOT_SUPPORTED);
    return 0;
}

int SetCpCarrierConfigHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();

    ProtocolRespAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        OnRequestComplete(RIL_E_SUCCESS);
    }
    else {
        OnRequestComplete(RIL_E_MODEM_ERR);
    }
    return 0;
}

/**
 * UnsolCpCarrierConfigSimInfoHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(UnsolCpCarrierConfigSimInfoHandler, MSG_MISC_CARRIER_CONFIG_SIM_INFO_IND)

int UnsolCpCarrierConfigSimInfoHandler::OnIndication(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    mService->GetRilContext()->OnRequest(RIL_REQUEST_OEM_SET_CP_CARRIER_CONFIG,
                               (char*) msg->GetModemData()->GetRawData(),
                               msg->GetModemData()->GetLength(), 0);
    return 0;
}

/**
 * UnsolCpCarrierConfigStatusHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(UnsolCpCarrierConfigStatusHandler, MSG_MISC_CARRIER_CONFIG_STATUS_IND)

int UnsolCpCarrierConfigStatusHandler::OnIndication(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();

    ProtocolMiscCarrierConfigStatusIndAdapter adapter(pModemData);
    RilLogV("[%d] OemCpCarrierConfigStatus=%d", mService->GetRilSocketId(), adapter.GetStatus());
    OnUnsolicitedResponse(RIL_UNSOL_OEM_CP_CARRIER_CONFIG_CHANGED, adapter.GetParameter(), adapter.GetParameterLength());

    return 0;
}
