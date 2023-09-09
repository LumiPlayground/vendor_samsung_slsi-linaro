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
 * protocolimsadapter.cpp
 *
 *  Created on: 2014. 11. 19.
 *      Author: mox
 */

#include <memory.h>
#include <string.h>
#include <telephony/ril.h>
#include "protocolimsadapter.h"
#include "rillog.h"
#include "util.h"
#include "rildef.h"
#include "sitdef.h"
#include "modemdata.h"

// add category to display selective logs
#undef RilLogI
#define RilLogI(format, ...) CRilLog::Log(RIL_LOG_CAT_IMS, CRilLog::E_RIL_INFO_LOG, format, ##__VA_ARGS__)
#undef RilLogV
#define RilLogV(format, ...) CRilLog::Log(RIL_LOG_CAT_IMS, CRilLog::E_RIL_VERBOSE_LOG, format, ##__VA_ARGS__)
#undef RilLogW
#define RilLogW(format, ...) CRilLog::Log(RIL_LOG_CAT_IMS, CRilLog::E_RIL_WARNING_LOG, format, ##__VA_ARGS__)
#undef RilLogE
#define RilLogE(format, ...) CRilLog::Log(RIL_LOG_CAT_IMS, CRilLog::E_RIL_CRITICAL_LOG, format, ##__VA_ARGS__)

/**
 * ProtocolImsRespAdapter
 */

BYTE ProtocolImsReasonRespAdapter::GetResult() {
    sit_ims_gen_reason_rsp *rsp = (sit_ims_gen_reason_rsp *)m_pModemData->GetRawData();
    return rsp->result;
}

BYTE ProtocolImsReasonRespAdapter::GetFailReason() {
    sit_ims_gen_reason_rsp *rsp = (sit_ims_gen_reason_rsp *)m_pModemData->GetRawData();
    return rsp->fail_reason;
}

BYTE ProtocolImsRespAdapter::GetResult() {
    sit_ims_gen_rsp *rsp = (sit_ims_gen_rsp *)m_pModemData->GetRawData();
    return rsp->result;
}

/**
 * ProtocolAimsIndAdapter
 */
int ProtocolAimsIndAdapter::GetResultId() const {
    // index 0 : OEM RIL ID
    // index 1 : SIT message ID
    const static int messageMap[][2] = {
        {RIL_UNSOL_OEM_AIMS_CALL_RING, SIT_IND_AIMS_CALL_RING},
        {RIL_UNSOL_OEM_AIMS_CALL_STATUS, SIT_IND_AIMS_CALL_STATUS},
        {RIL_UNSOL_OEM_AIMS_REGISTRATION, SIT_IND_AIMS_REGISTRATION},
        {RIL_UNSOL_OEM_AIMS_CALL_MODIFY, SIT_IND_AIMS_CALL_MODIFY},
        {RIL_UNSOL_OEM_AIMS_EXPIRE_INFO, SIT_IND_AIMS_EXPIRE_INFO},
        {RIL_UNSOL_OEM_AIMS_FRAME_TIME, SIT_IND_AIMS_FRAME_TIME},
        {RIL_UNSOL_OEM_AIMS_SUPP_SVC_NOTIFICATION, SIT_IND_AIMS_SUPP_SVC_NOTIFICATION},
        {RIL_UNSOL_OEM_AIMS_NEW_SMS, SIT_IND_AIMS_NEW_SMS},
        {RIL_UNSOL_OEM_AIMS_NEW_SMS_STATUS_REPORT, SIT_IND_AIMS_NEW_SMS_STATUS_REPORT},
        {RIL_UNSOL_OEM_AIMS_ON_USSD, SIT_IND_AIMS_ON_USSD},
        {RIL_UNSOL_OEM_AIMS_CONFERENCE_CALL_EVENT, SIT_IND_AIMS_CONFERENCE_CALL_EVENT},
        {RIL_UNSOL_OEM_AIMS_PAYLOAD_INFO, SIT_IND_AIMS_PAYLOAD_INFO_IND},
        {RIL_UNSOL_OEM_AIMS_VOWIFI_HO_CALL_INFO, SIT_IND_AIMS_VOWIFI_HO_CALL_INFO},
        {RIL_UNSOL_OEM_AIMS_NEW_CDMA_SMS, SIT_IND_AIMS_NEW_CDMA_SMS},
        {RIL_UNSOL_OEM_AIMS_RINGBACK_TONE, SIT_IND_AIMS_RINGBACK_TONE},
        {RIL_UNSOL_OEM_AIMS_CALL_MANAGE, SIT_IND_AIMS_CALL_MANAGE},
        {RIL_UNSOL_OEM_AIMS_CONF_CALL_ADD_REMOVE_USER, SIT_IND_AIMS_CONF_CALL_ADD_REMOVE_USER},
        {RIL_UNSOL_OEM_AIMS_ENHANCED_CONF_CALL, SIT_IND_AIMS_ENHANCED_CONF_CALL},
        {RIL_UNSOL_OEM_AIMS_CALL_MODIFY_RSP, SIT_IND_AIMS_CALL_MODIFY_RSP},
        {RIL_UNSOL_OEM_AIMS_DTMF_EVENT, SIT_IND_AIMS_DTMF_EVENT},
        {RIL_UNSOL_OEM_AIMS_RTT_NEW_TEXT, SIT_IND_AIMS_RTT_NEW_TEXT},
        {RIL_UNSOL_OEM_AIMS_RTT_FAIL_SENDING_TEXT, SIT_IND_AIMS_RTT_FAIL_SENDING_TEXT},
        {RIL_UNSOL_OEM_AIMS_EXIT_EMERGENCY_CB_MODE, SIT_IND_AIMS_EXIT_EMERGENCY_CB_MODE},
        {RIL_UNSOL_OEM_AIMS_DIALOG_INFO, SIT_IND_AIMS_DIALOG_INFO},
        {RIL_UNSOL_OEM_AIMS_RCS_MULTI_FRAME, SIT_IND_AIMS_RCS_MULTI_FRAME},
        {RIL_UNSOL_OEM_AIMS_RCS_CHAT, SIT_IND_AIMS_RCS_CHAT},
        {RIL_UNSOL_OEM_AIMS_RCS_GROUP_CHAT, SIT_IND_AIMS_RCS_GROUP_CHAT},
        {RIL_UNSOL_OEM_AIMS_RCS_OFFLINE_MODE, SIT_IND_AIMS_RCS_OFFLINE_MODE},
        {RIL_UNSOL_OEM_AIMS_RCS_FILE_TRANSFER, SIT_IND_AIMS_RCS_FILE_TRANSFER},
        {RIL_UNSOL_OEM_AIMS_RCS_COMMON_MESSAGE, SIT_IND_AIMS_RCS_COMMON_MESSAGE},
        {RIL_UNSOL_OEM_AIMS_RCS_CONTENT_SHARE, SIT_IND_AIMS_RCS_CONTENT_SHARE},
        {RIL_UNSOL_OEM_AIMS_RCS_PRESENCE, SIT_IND_AIMS_RCS_PRESENCE},
        {RIL_UNSOL_OEM_AIMS_RCS_XCAP_MANAGE, SIT_IND_AIMS_RCS_XCAP_MANAGE},
        {RIL_UNSOL_OEM_AIMS_RCS_CONFIG_MANAGE, SIT_IND_AIMS_RCS_CONFIG_MANAGE},
        {RIL_UNSOL_OEM_AIMS_RCS_TLS_MANAGE, SIT_IND_AIMS_RCS_TLS_MANAGE},
        {RIL_UNSOL_OEM_WFC_RTP_RTCP_TIMEOUT, SIT_IND_WFC_RTP_RTCP_TIMEOUT},
        {RIL_UNSOL_OEM_WFC_FIRST_RTP, SIT_IND_WFC_FIRST_RTP},
        {RIL_UNSOL_OEM_WFC_RTCP_RX_SR, SIT_IND_WFC_RTCP_RX_SR},
        {RIL_UNSOL_OEM_WFC_RCV_DTMF_NOTI, SIT_IND_WFC_RCV_DTMF_NOTI},
        {RIL_UNSOL_OEM_AIMS_MEDIA_STATUS, SIT_IND_AIMS_MEDIA_STATUS},
        {RIL_UNSOL_OEM_AIMS_SIP_MSG_INFO, SIT_IND_AIMS_SIP_MSG_INFO},
        {RIL_UNSOL_OEM_AIMS_VOICE_RTP_QUALITY, SIT_IND_AIMS_VOICE_RTP_QUALITY},
        {RIL_UNSOL_OEM_AIMS_RTP_RX_STATISTICS, SIT_AIMS_IND_RTP_RX_STATISTICS},
        {RIL_UNSOL_OEM_AIMS_AT_CMD_FORWARDING, SIT_IND_AIMS_AT_CMD_FORWARDING},
        {RIL_UNSOL_OEM_AIMS_FEAT_SUPP, SIT_IND_AIMS_FEAT_SUPP},
        {RIL_UNSOL_OEM_AIMS_CALL_TRANSFER_RSP, SIT_IND_AIMS_CALL_TRANSFER_RSP},
        {RIL_UNSOL_OEM_AIMS_CALL_TRANSFER, SIT_IND_AIMS_CALL_TRANSFER},
        {RIL_UNSOL_OEM_AIMS_MWI_INFO, SIT_IND_AIMS_MWI_INFO},
        {RIL_UNSOL_OEM_AIMS_COMPOSER_MMTEL, SIT_IND_AIMS_COMPOSER_MMTEL},
        {RIL_UNSOL_OEM_AIMS_ENRICHED_CALLING, SIT_IND_AIMS_ENRICHED_CALLING},
        {RIL_UNSOL_OEM_AIMS_RTP_HEADER_EXTENSION_TYPE, SIT_IND_AIMS_RTP_HEADER_EXTENSION_TYPE},
        {RIL_UNSOL_OEM_AIMS_RTP_HEADER_EXTENSION, SIT_IND_AIMS_RTP_HEADER_EXTENSION},
        {RIL_UNSOL_OEM_AIMS_CODEC_INFO, SIT_IND_AIMS_CODEC_INFO},
        {RIL_UNSOL_OEM_AIMS_SIP_CONFIG, SIT_IND_AIMS_SIP_CONFIG},
        {RIL_UNSOL_OEM_AIMS_UL_SIP_MSG_RESULT, SIT_IND_AIMS_UL_SIP_MSG_RESULT},
        {RIL_UNSOL_OEM_AIMS_SEND_DL_SIP_MSG, SIT_IND_AIMS_SEND_DL_SIP_MSG},
        {RIL_UNSOL_OEM_AIMS_DC_SESS_UPDATE, SIT_IND_AIMS_DC_SESS_UPDATE},
        {RIL_UNSOL_OEM_AIMS_DC_SESSION_MANAGE, SIT_IND_AIMS_DC_SESSION_MANAGE},
        {RIL_UNSOL_OEM_AIMS_DC_SESS_MODIFY, SIT_IND_AIMS_DC_SESS_MODIFY},
    };

    if (m_pModemData != NULL) {
        RCM_IND_HEADER *hdr = (RCM_IND_HEADER *)m_pModemData->GetRawData();
        if (hdr != NULL) {
            int size = sizeof(messageMap) / sizeof(messageMap[0]);
            for (int i = 0; i < size; i++) {
                if (hdr->id == messageMap[i][1]) {
                    return messageMap[i][0];
                }
            }  // end for i ~
        }
    }

    return -1;
}

INT32 ProtocolImsRegIndAdapter::GetRegState() {
    sit_ims_ind_reg *rsp = (sit_ims_ind_reg *)m_pModemData->GetRawData();
    if (rsp != NULL) {
        switch (rsp->state) {
        case SITRIL_AIMS_IMSREG_STATE_REGISTERED:
            return RIL_IMS_REGISTERED;
        case SITRIL_AIMS_IMSREG_STATE_NOT_REGISTERED:
            return RIL_IMS_NOT_REGISTERED;
        default:
            return -1;
        }
    } else {
        return -1;
    }
}

BOOL ProtocolImsAimsCallWaitingAdapter::IsEnable() {
    BOOL bEnable = FALSE;
    sit_aims_set_call_waiting_ex *pReq = (sit_aims_set_call_waiting_ex *)m_pRawData;
    if (pReq != NULL) {
        bEnable = (pReq->service_status == 0x01);
    }
    return bEnable;
}

INT32 ProtocolImsAimsCallWaitingAdapter::GetServiceClass() {
    INT32 service_class = 0;
    sit_aims_set_call_waiting_ex *pReq = (sit_aims_set_call_waiting_ex *)m_pRawData;
    if (pReq != NULL) {
        service_class = pReq->service_class;
    }
    return service_class;
}

int ProtocolImsTriggerImsDeregistrationIndAdapter::GetReason() const {
    if (m_pModemData != nullptr) {
        const sit_ims_trigger_ims_deregistration_ind *ind =
            reinterpret_cast<const sit_ims_trigger_ims_deregistration_ind *>(m_pModemData->GetRawData());
        if (ind != nullptr && ind->hdr.id == SIT_IND_IMS_DEREGISTERTAION) {
            return ind->reason;
        }
    }
    return 0;
}

int ProtocolImsStartImsTrafficRespAdapter::GetConnectionFailureReason() const {
    if (m_pModemData != nullptr) {
        const sit_ims_start_ims_traffic_rsp *rsp =
            reinterpret_cast<const sit_ims_start_ims_traffic_rsp *>(m_pModemData->GetRawData());
        if (rsp != nullptr && rsp->hdr.id == SIT_IMS_START_TRAFFIC) {
            return rsp->connectionFailureReason;
        }
    }
    return 0;
}

int ProtocolImsStartImsTrafficRespAdapter::GetCauseCode() const {
    if (m_pModemData != nullptr) {
        const sit_ims_start_ims_traffic_rsp *rsp =
            reinterpret_cast<const sit_ims_start_ims_traffic_rsp *>(m_pModemData->GetRawData());
        if (rsp != nullptr && rsp->hdr.id == SIT_IMS_START_TRAFFIC) {
            return rsp->causeCode;
        }
    }
    return 0;
}

int ProtocolImsStartImsTrafficRespAdapter::GetWaitTimeMillis() const {
    if (m_pModemData != nullptr) {
        const sit_ims_start_ims_traffic_rsp *rsp =
            reinterpret_cast<const sit_ims_start_ims_traffic_rsp *>(m_pModemData->GetRawData());
        if (rsp != nullptr && rsp->hdr.id == SIT_IMS_START_TRAFFIC) {
            return rsp->waitTimeMillis;
        }
    }
    return 0;
}

int ProtocolImsConnectionSetupFailureIndAdapter::GetFailureToken() const {
    if (m_pModemData != nullptr) {
        const sit_ims_rrc_connection_failure_ind *ind =
            reinterpret_cast<const sit_ims_rrc_connection_failure_ind *>(m_pModemData->GetRawData());
        if (ind != nullptr && ind->hdr.id == SIT_IND_IMS_RRC_CONNECTION_FAILURE) {
            return ind->token;
        }
    }
    return 0;
}

int ProtocolImsConnectionSetupFailureIndAdapter::GetConnectionFailureReason() const {
    if (m_pModemData != nullptr) {
        const sit_ims_rrc_connection_failure_ind *ind =
            reinterpret_cast<const sit_ims_rrc_connection_failure_ind *>(m_pModemData->GetRawData());
        if (ind != nullptr && ind->hdr.id == SIT_IND_IMS_RRC_CONNECTION_FAILURE) {
            return ind->connectionFailureReason;
        }
    }
    return 0;
}

int ProtocolImsConnectionSetupFailureIndAdapter::GetCauseCode() const {
    if (m_pModemData != nullptr) {
        const sit_ims_rrc_connection_failure_ind *ind =
            reinterpret_cast<const sit_ims_rrc_connection_failure_ind *>(m_pModemData->GetRawData());
        if (ind != nullptr && ind->hdr.id == SIT_IND_IMS_RRC_CONNECTION_FAILURE) {
            return ind->causeCode;
        }
    }
    return 0;
}

int ProtocolImsConnectionSetupFailureIndAdapter::GetWaitTimeMillis() const {
    if (m_pModemData != nullptr) {
        const sit_ims_rrc_connection_failure_ind *ind =
            reinterpret_cast<const sit_ims_rrc_connection_failure_ind *>(m_pModemData->GetRawData());
        if (ind != nullptr && ind->hdr.id == SIT_IND_IMS_RRC_CONNECTION_FAILURE) {
            return ind->waitTimeMillis;
        }
    }
    return 0;
}

int ProtocolImsTriggerEpsFallbackIndAdapter::GetResult() const {
    if (m_pModemData != nullptr) {
        const sit_set_trigger_epsfb_ind *ind =
            reinterpret_cast<const sit_set_trigger_epsfb_ind *>(m_pModemData->GetRawData());
        if (ind != nullptr && ind->hdr.id == SIT_SET_TRIGGER_EPSFB) {
            return static_cast<int32_t>(ind->result);
        }
    }
    return 0;
}

int ProtocolImsTriggerEpsFallbackIndAdapter::GetRat() const {
    if (m_pModemData != nullptr) {
        const sit_set_trigger_epsfb_ind *ind =
            reinterpret_cast<const sit_set_trigger_epsfb_ind *>(m_pModemData->GetRawData());
        if (ind != nullptr && ind->hdr.id == SIT_SET_TRIGGER_EPSFB) {
            return static_cast<int32_t>(ind->rat);
        }
    }
    return 0;
}
