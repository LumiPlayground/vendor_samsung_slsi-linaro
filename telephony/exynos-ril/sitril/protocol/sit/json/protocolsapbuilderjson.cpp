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
 * protocolsapbuilderjson.cpp
 *
 *  Created on: 2021. 8. 3.
 *      Author: aryaman.2019
 */

#include "protocolsapbuilderjson.h"
#include "protocolutils.h"
#include "rillog.h"
#include "simjsondef.h"
#include "json/json.h"

// add category to display selective logs
#undef RilLogI
#define RilLogI(format, ...) CRilLog::Log(RIL_LOG_CAT_SAP, CRilLog::E_RIL_INFO_LOG,  format, ##__VA_ARGS__)
#undef RilLogV
#define RilLogV(format, ...) CRilLog::Log(RIL_LOG_CAT_SAP, CRilLog::E_RIL_VERBOSE_LOG,  format, ##__VA_ARGS__)
#undef RilLogW
#define RilLogW(format, ...) CRilLog::Log(RIL_LOG_CAT_SAP, CRilLog::E_RIL_WARNING_LOG,  format, ##__VA_ARGS__)
#undef RilLogE
#define RilLogE(format, ...) CRilLog::Log(RIL_LOG_CAT_SAP, CRilLog::E_RIL_CRITICAL_LOG,  format, ##__VA_ARGS__)

ModemData *ProtocolSapBuilderJson::BuildSapConnect(int maxMsgSize)
{
    Json::Value jsonReqData;
    jsonReqData[ns_sap_connect_req::key_max_msg_size] = (maxMsgSize & 0xFFFF);

    return GetJsonReqModemData(jsonReqData, SIT_SAP_CONNECT);
}

ModemData *ProtocolSapBuilderJson::BuildSapDisconnect()
{
    return GetJsonNullReqModemData(SIT_SAP_DISCONNECT);
}

ModemData *ProtocolSapBuilderJson::BuildSapTransferApdu(int apduLeng, char *apdu)
{
    Json::Value jsonReqData;
    jsonReqData[ns_sap_transfer_apdu_req::key_req_apdu] = Json::arrayValue;

    ProtocolUtils::LimitLengthToMax(RIL_LOG_CAT_SAP, __CLASS_FUNC__, apduLeng, MAX_SAP_TRANSFER_APDU_LEN);

    for(int i = 0; i < apduLeng; i++) {
        jsonReqData[ns_sap_transfer_apdu_req::key_req_apdu].append((int) apdu[i]);
    }

    return GetJsonReqModemData(jsonReqData, SIT_SAP_TRANSFER_APDU);
}

ModemData *ProtocolSapBuilderJson::BuildSapTransferAtr()
{
    return GetJsonNullReqModemData(SIT_SAP_TRANSFER_ATR);
}

ModemData *ProtocolSapBuilderJson::BuildSapPowerSimOnOff(bool state)
{
    int nSitId;
    if(state == true) {
        nSitId = SIT_SAP_POWER_SIM_ON;
    } else {
        nSitId = SIT_SAP_POWER_SIM_OFF;
    }
    return GetJsonNullReqModemData(nSitId);
}

ModemData *ProtocolSapBuilderJson::BuildSapResetSim()
{
    return GetJsonNullReqModemData(SIT_SAP_RESET_SIM);
}

ModemData *ProtocolSapBuilderJson::BuildSapTransferCardReaderStatus()
{
    return GetJsonNullReqModemData(SIT_SAP_TRANSFER_CARD_READER_STATUS);
}

ModemData *ProtocolSapBuilderJson::BuildSapTranportProtocol(int protocol)
{
    Json::Value jsonReqData;
    jsonReqData[ns_sap_tranport_protocol_req::key_protocol] = protocol;

    return GetJsonReqModemData(jsonReqData, SIT_SAP_SET_TRANPORT_PROTOCOL);
}
