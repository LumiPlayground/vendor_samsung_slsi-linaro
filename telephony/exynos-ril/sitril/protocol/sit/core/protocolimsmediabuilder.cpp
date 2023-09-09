/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd All Rights Reserved
 * PROPRIETARY/CONFIDENTIAL
 *
 * This software is the confidential and proprietary information of
 * SAMSUNG ELECTRONICS ("Confidential Information").
 *
 * You shall not disclose such Confidential Information and shall use it
 * only in accordance with the terms of the license agreement
 * you entered into with SAMSUNG ELECTRONICS.
 *
 * SAMSUNG make no representations or warranties about the suitability of
 * the software, either express or implied, including but not limited to
 * the implied warranties of merchantability, fitness for a particular
 * purpose, or non-infringement. SAMSUNG shall not be liable for any
 * damages suffered by license as a result of using, modifying or
 * distributing this software or its derivatives.
 */

#include "commondef.h"
#include "modemdata.h"
#include "protocolimsmediabuilder.h"
#include <slsi/ril_oem.h>

ModemData *ProtocolImsMediaBuilder::BuildImsMediaPDU(int requestId, void *data, unsigned int datalen) {
    static const int messageMap[][2] = {
            {RIL_REQUEST_OEM_IMS_TEST_RCM, SIT_IMS_MEDIA_TEST_RCM},
            {RIL_REQUEST_OEM_IMS_MEDIA_INIT, SIT_IMS_MEDIA_INIT},
            {RIL_REQUEST_OEM_IMS_MEDIA_OPEN_SESSION, SIT_IMS_MEDIA_OPEN_SESSION},
            {RIL_REQUEST_OEM_IMS_MEDIA_CLOSE_SESSION, SIT_IMS_MEDIA_CLOSE_SESSION},
            {RIL_REQUEST_OEM_IMS_MEDIA_MODIFY_SESSION, SIT_IMS_MEDIA_MODIFY_SESSION},
            {RIL_REQUEST_OEM_IMS_MEDIA_SEND_DTMF, SIT_IMS_MEDIA_SEND_DTMF},
            {RIL_REQUEST_OEM_IMS_MEDIA_START_DTMF, SIT_IMS_MEDIA_START_DTMF},
            {RIL_REQUEST_OEM_IMS_MEDIA_STOP_DTMF, SIT_IMS_MEDIA_STOP_DTMF},
            {RIL_REQUEST_OEM_IMS_MEDIA_SEND_TEXT, SIT_IMS_MEDIA_SEND_TEXT},
            {RIL_REQUEST_OEM_IMS_MEDIA_SEND_HEADER_EXTENSION, SIT_IMS_MEDIA_SEND_HEADER_EXTENSION},
            {RIL_REQUEST_OEM_IMS_MEDIA_SET_MEDIA_QUALITY_THRESHOLD, SIT_IMS_MEDIA_SET_MEDIA_QUALITY_THRESHOLD},
            {RIL_REQUEST_OEM_IMS_MEDIA_GET_OPERATION_MODE, SIT_IMS_GET_OPERATION_MODE},
            {RIL_REQUEST_OEM_IMS_SIP_MESSAGE, SIT_IMS_SIP_MESSAGE },
            {RIL_REQUEST_OEM_IMS_MEDIA_WIFI_CONFIG, SIT_IMS_WIFI_CONFIG_REQ},
            {RIL_REQUEST_OEM_IMS_MEDIA_QUERY_MAC_CE_INFO, SIT_QUERY_MAC_CE_INFO},
    };

    if (requestId < 0) {
        return nullptr;
    }

    // TODO check maximum RCM PDU size
    if (data == NULL && datalen > 0) {
        datalen = 0;
    }

    int protocolId = -1;
    size_t size = sizeof(messageMap) / sizeof(messageMap[0]);
    for (size_t i = 0; i < size; i++) {
        if (messageMap[i][0] == requestId) {
            protocolId = messageMap[i][1];
            break;
        }
    }

    if (protocolId < 0) {
        return nullptr;
    }

    int length = sizeof(RCM_HEADER) + datalen;
    char *buf = new char[length];
    InitRequestHeader((RCM_HEADER *)buf, protocolId, length);

    if (data != NULL && datalen > 0) {
        memcpy(buf + sizeof(RCM_HEADER), data, datalen);
    }

    ModemData *modemData = new ModemData(buf, length);
    delete[] buf;
    return modemData;
}

ModemData *ProtocolImsMediaBuilder::BuildImsMediaIndPDU(int requestId, void *data, unsigned int datalen) {
    static const int messageMap[][2] = {
            {},
    };

    if (requestId < 0) {
        return nullptr;
    }

    // TODO check maximum RCM PDU size
    if (data == NULL && datalen > 0) {
        datalen = 0;
    }

    int protocolId = -1;
    size_t size = sizeof(messageMap) / sizeof(messageMap[0]);
    for (size_t i = 0; i < size; i++) {
        if (messageMap[i][0] == requestId) {
            protocolId = messageMap[i][1];
            break;
        }
    }

    if (protocolId < 0) {
        return nullptr;
    }

    int length = sizeof(RCM_IND_HEADER) + datalen;
    char *buf = new char[length];
    InitIndRequestHeader((RCM_IND_HEADER *)buf, protocolId, length);

    if (data != NULL && datalen > 0) {
        memcpy(buf + sizeof(RCM_IND_HEADER), data, datalen);
    }

    ModemData *modemData = new ModemData(buf, length);
    delete[] buf;
    return modemData;
}
