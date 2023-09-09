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
 * rilcontextwrapperhandler.cpp
 *
 *  Created on: 2021. 02. 19.
 */
#include <telephony/librilutils.h>

#include "messageid.h"
#include "rilcontextwrapper.h"
#include "rilcontextwrapperhandler.h"
#include "rildata.h"
#include "rillog.h"
#include "rilcontextwrapperhandler.h"
#include "messageid.h"
#include <librilutils/textutils.h>
#include <librilutils/systemproperty.h>

IMPLEMENT_MODULE_TAG(RilContextWrapperHandler, RilContextWrapperHandler)

// add category to display selective logs
#undef RilLogI
#define RilLogI(format, ...) CRilLog::Log(RIL_LOG_CAT_CORE, CRilLog::E_RIL_INFO_LOG,  format, ##__VA_ARGS__)
#undef RilLogV
#define RilLogV(format, ...) CRilLog::Log(RIL_LOG_CAT_CORE, CRilLog::E_RIL_VERBOSE_LOG,  format, ##__VA_ARGS__)
#undef RilLogW
#define RilLogW(format, ...) CRilLog::Log(RIL_LOG_CAT_CORE, CRilLog::E_RIL_WARNING_LOG,  format, ##__VA_ARGS__)
#undef RilLogE
#define RilLogE(format, ...) CRilLog::Log(RIL_LOG_CAT_CORE, CRilLog::E_RIL_CRITICAL_LOG,  format, ##__VA_ARGS__)

RilContextWrapperHandler::RilContextWrapperHandler(RilContextWrapper *rilcontextwrapper)
    : m_pRilContextWrapper(rilcontextwrapper) { }

RilContextWrapperHandler::~RilContextWrapperHandler() { }

void RilContextWrapperHandler::ProcessIccCardStatus(const void *data, unsigned int datalen) {
    if(!m_pRilContextWrapper) return;

    if (data == NULL || datalen < sizeof(RIL_CardStatus_v6)) return;

    if (datalen >= sizeof(RIL_CardStatus_v6)) {
        // radio 1.0
        RIL_CardStatus_v6 *cardStatus = (RIL_CardStatus_v6 *)data;
        m_pRilContextWrapper->SetCardStatus(cardStatus);

        RilDataInts *rildata = new RilDataInts(2);
        if (rildata != NULL) {
            rildata->SetInt(0, cardStatus->card_state);    // RIL_CardState : Absent(0), Present(1), Error(2)

            if (cardStatus->gsm_umts_subscription_app_index >= 0) {
                RIL_AppStatus *gsmApplication = &cardStatus->applications[cardStatus->gsm_umts_subscription_app_index];
                rildata->SetInt(1, gsmApplication->app_state);    // RIL_AppState : Illegal(-1), Unknown(0), Detected(1), Ready(5)
            }
            else {
                rildata->SetInt(1, (int)-1);
            }

            m_pRilContextWrapper->BroadcastSystemMessage(MSG_SYSTEM_SIM_STATUS_CHANGED, rildata);
        }
    }

    if (datalen >= sizeof(RIL_CardStatus_V1_2)) {
        // ICCID, ATR, Physical SIM slot
    }

    if (datalen >= sizeof(RIL_CardStatus_V1_4)) {
        // EID
        SimCardStatus cardStatus = SimCardStatus::create(*(RIL_CardStatus_V1_4 *)data);
        m_pRilContextWrapper->SetCardStatus(cardStatus);
    }
}

void RilContextWrapperHandler::ProcessImsi(const void *data, unsigned int datalen) {
    if(!m_pRilContextWrapper) return;

    if (data == NULL || datalen == 0) return;

    string imsi = (const char *)data;
    // RIL_CONTEXT_GSM_SIM_AID and RIL_CONTEXT_GSM_SIM_IMSI are set by SIM service.
    // OnGetImsiDone()
    string gsmUmtsAid = m_pRilContextWrapper->GetProperty()->GetString(RIL_CONTEXT_GSM_SIM_AID, "");
    string gsmUmtsImsi = m_pRilContextWrapper->GetProperty()->GetString(RIL_CONTEXT_GSM_SIM_IMSI, "");

    if (!TextUtils::IsEmpty(gsmUmtsAid) && !TextUtils::IsEmpty(gsmUmtsImsi) &&
            TextUtils::Equals(gsmUmtsImsi, imsi)) {
        RilDataStrings *rildata = new RilDataStrings(2);
        if (rildata != NULL) {
            rildata->SetString(0, gsmUmtsAid.c_str());
            rildata->SetString(1, gsmUmtsImsi.c_str());
            m_pRilContextWrapper->BroadcastSystemMessage(MSG_SYSTEM_IMSI_UPDATED, rildata);
        }
    }
}

void RilContextWrapperHandler::ProcessDeviceIdentity(const void *data, unsigned int datalen) {
    if(!m_pRilContextWrapper) return;

    if (Build::IS_USER || data == nullptr || datalen == 0) return;

    char* rilData = ((char **) data)[0];
    std::string imei = rilData ? rilData : "";
    int phoneId = m_pRilContextWrapper->GetRilSocketId();
    SystemProperty::Set(VENDOR_RIL_GSM_IMEI + std::to_string(phoneId), imei);
}
