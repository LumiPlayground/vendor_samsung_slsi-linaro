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
 * protocolvsimbuilderjson.cpp
 *
 *  Created on: 2021. 08. 18.
 */

#include "protocolvsimbuilderjson.h"
#include "vsimprotocolutils.h"
#include "util.h"
#include "rillog.h"
#include "vsimjsondef.h"

ModemData *ProtocolVsimBuilderJson::BuildVsimNotification(int tid, int eventid, int simType)
{
    int nEventId = VsimProtocolUtils::ConvertVsimNotiRilEventIdToSitEventId(eventid);
    if (nEventId == -1) return NULL;

    Json::Value jsonReqData;
    jsonReqData[ns_vsim_notification_req::key_tid] = tid;
    jsonReqData[ns_vsim_notification_req::key_event_type] = nEventId;
    jsonReqData[ns_vsim_notification_req::key_sim_type] = simType;

    return GetJsonReqModemData(jsonReqData, SIT_VSIM_NOTIFICATION);
}

ModemData *ProtocolVsimBuilderJson::BuildVsimOperation(int tid, int eventid, int result, int /* dataLen */, const char *data) {
    int nEventId = VsimProtocolUtils::ConvertVsimOpRilEventIdToSitEventId(eventid);
    if (nEventId == -1) return NULL;

    Json::Value jsonReqData;
    jsonReqData[ns_vsim_opertaion_req::key_tid] = tid;
    jsonReqData[ns_vsim_opertaion_req::key_event_type] = nEventId;
    jsonReqData[ns_vsim_opertaion_req::key_result] = result;

    // initializing to empty array
    jsonReqData[ns_vsim_opertaion_req::key_operation_data] = Json::arrayValue;

    if (data != NULL) {
        if ((strlen(data) + 1) / 2 <= MAX_VSIM_DATA_LEN) {
            int nDataLen = 0;
            BYTE rawData[MAX_VSIM_DATA_LEN] = {0, };
            nDataLen = HexString2Value(rawData, sizeof(rawData), data);
            if (0 < nDataLen && nDataLen <= MAX_VSIM_DATA_LEN) {
                for (int i = 0; i < nDataLen; i++) {
                    jsonReqData[ns_vsim_opertaion_req::key_operation_data].append((int) rawData[i]);
                }
                RilLogV("%s: data length:%d", __FUNCTION__, nDataLen);
            }
            else {
                // converting error
                jsonReqData[ns_vsim_opertaion_req::key_result] = RCM_E_GENERIC_FAILURE;
                RilLogE("%s: failed to convert Hexadecimal string", __FUNCTION__);
            }
        } else {
            // APDU overflow
            jsonReqData[ns_vsim_opertaion_req::key_result] = RCM_E_GENERIC_FAILURE;
            RilLogE("%s: APDU Overflow, Data is larger than %d bytes", __FUNCTION__, MAX_VSIM_DATA_LEN);
        }
    }

    return GetJsonReqModemData(jsonReqData, SIT_VSIM_OPERATION);
}
