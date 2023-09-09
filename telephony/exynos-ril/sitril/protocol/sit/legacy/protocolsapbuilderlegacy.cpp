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
 * protocolsapbuilderlegacy.cpp
 *
 *  Created on: 2014. 12. 2.
 *      Author: sungwoo48.choi
 */

#include "protocolsapbuilderlegacy.h"
#include "rillog.h"
#include "modemdata.h"

// add category to display selective logs
#undef RilLogI
#define RilLogI(format, ...) CRilLog::Log(RIL_LOG_CAT_SAP, CRilLog::E_RIL_INFO_LOG,  format, ##__VA_ARGS__)
#undef RilLogV
#define RilLogV(format, ...) CRilLog::Log(RIL_LOG_CAT_SAP, CRilLog::E_RIL_VERBOSE_LOG,  format, ##__VA_ARGS__)
#undef RilLogW
#define RilLogW(format, ...) CRilLog::Log(RIL_LOG_CAT_SAP, CRilLog::E_RIL_WARNING_LOG,  format, ##__VA_ARGS__)
#undef RilLogE
#define RilLogE(format, ...) CRilLog::Log(RIL_LOG_CAT_SAP, CRilLog::E_RIL_CRITICAL_LOG,  format, ##__VA_ARGS__)

ModemData *ProtocolSapBuilderLegacy::BuildSapConnect(int maxMsgSize)
{
    sit_sap_connect_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_SAP_CONNECT, length);
    req.max_msg_size = (UINT16)(maxMsgSize & 0xFFFF);

    return new ModemData((char *)&req, length);
}

ModemData *ProtocolSapBuilderLegacy::BuildSapDisconnect()
{
    null_data_format req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_SAP_DISCONNECT, length);

    return new ModemData((char *)&req, length);
}

ModemData *ProtocolSapBuilderLegacy::BuildSapTransferApdu(int apduLeng, char *apdu)
{
    sit_sap_transfer_apdu_req req;
    int length = sizeof(req.hdr) + apduLeng + sizeof(UINT16);
    InitRequestHeader(&req.hdr, SIT_SAP_TRANSFER_APDU, length);

    req.apdu_len = (UINT16) apduLeng;
    memset(req.apdu, 0, sizeof(req.apdu));
    memcpy(req.apdu, apdu, apduLeng);

    return new ModemData((char *)&req, length);
}

ModemData *ProtocolSapBuilderLegacy::BuildSapTransferAtr()
{
    null_data_format req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_SAP_TRANSFER_ATR, length);

    return new ModemData((char *)&req, length);
}

ModemData *ProtocolSapBuilderLegacy::BuildSapPowerSimOnOff(bool state)
{
    null_data_format req;
    int length = sizeof(req);
    if (state == true) {
        InitRequestHeader(&req.hdr, SIT_SAP_POWER_SIM_ON, length);
    } else {
        InitRequestHeader(&req.hdr, SIT_SAP_POWER_SIM_OFF, length);
    }

    return new ModemData((char *)&req, length);
}

ModemData *ProtocolSapBuilderLegacy::BuildSapResetSim()
{
    null_data_format req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_SAP_RESET_SIM, length);

    return new ModemData((char *)&req, length);
}

ModemData *ProtocolSapBuilderLegacy::BuildSapTransferCardReaderStatus()
{
    null_data_format req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_SAP_TRANSFER_CARD_READER_STATUS, length);

    return new ModemData((char *)&req, length);
}

ModemData *ProtocolSapBuilderLegacy::BuildSapTranportProtocol(int protocol)
{
    sit_sap_tranport_protocol_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_SAP_SET_TRANPORT_PROTOCOL, length);
    req.protocol = (BYTE) protocol;

    return new ModemData((char *)&req, length);
}
