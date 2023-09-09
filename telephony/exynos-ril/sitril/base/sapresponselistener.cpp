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
 * rilresponselistener.cpp
 *
 *  Created on: 2019. 2. 19.
 */

#include "sapresponselistener.h"
#include "rillog.h"
#include "ril_vendor_internal.h"

// add category to display selective logs
#undef RilLogI
#define RilLogI(format, ...) CRilLog::Log(RIL_LOG_CAT_CORE, CRilLog::E_RIL_INFO_LOG,  format, ##__VA_ARGS__)
#undef RilLogV
#define RilLogV(format, ...) CRilLog::Log(RIL_LOG_CAT_CORE, CRilLog::E_RIL_VERBOSE_LOG,  format, ##__VA_ARGS__)
#undef RilLogW
#define RilLogW(format, ...) CRilLog::Log(RIL_LOG_CAT_CORE, CRilLog::E_RIL_WARNING_LOG,  format, ##__VA_ARGS__)
#undef RilLogE
#define RilLogE(format, ...) CRilLog::Log(RIL_LOG_CAT_CORE, CRilLog::E_RIL_CRITICAL_LOG,  format, ##__VA_ARGS__)

#define UNUSED(x) ((void)(x))

SapResponseListener::SapResponseListener(const struct RIL_Env *pSapEnv)
    : m_pSapEnv(pSapEnv)
{
    m_pRilApp = NULL;
}

SapResponseListener::~SapResponseListener()
{

}

IMPLEMENT_MODULE_TAG(SapResponseListener, SapResponseListener)

// response
void SapResponseListener::OnRequestComplete(RIL_Token t, RIL_Errno e, void *response, unsigned int responselen)
{
    //RilLogI("[%s] %s ", TAG, __FUNCTION__);

    // In case of RIL request
    if (m_pSapEnv != NULL && m_pSapEnv->OnRequestComplete != NULL) {
        m_pSapEnv->OnRequestComplete(t, e, response, responselen);
    }
    else {
        RilLogW("[%s] %s cannot invoke m_pSapEnv->OnRequestComplete()", TAG, __FUNCTION__);
    }
}

void SapResponseListener::OnUnsolicitedResponse(int unsolResponse, const void *data, unsigned int datalen)
{
    OnUnsolicitedResponse(unsolResponse, data, datalen, RIL_SOCKET_1);
}

void SapResponseListener::OnUnsolicitedResponse(int unsolResponse, const void *data, unsigned int datalen, RIL_SOCKET_ID socket_id)
{
    //RilLogI("[%s] %s ", TAG, __FUNCTION__);

    if (FilterOut(unsolResponse, data, datalen)) {
        RilLogV("SAP::[%s] %s Filter out Unsolicited response=%d", TAG, __FUNCTION__, unsolResponse);
        return;
    }

    // In order to match the value to librilutils/proto/sap-api.proto
    if (unsolResponse > RIL_REQUEST_SAP_UNSOL_RESPONSESAP) {
        unsolResponse -= RIL_REQUEST_SAP_UNSOL_RESPONSESAP;
    }

    // To the RIL daemon
    if (m_pSapEnv != NULL && m_pSapEnv->OnUnsolicitedResponse != NULL) {
#ifdef ANDROID_MULTI_SIM
        m_pSapEnv->OnUnsolicitedResponse(unsolResponse, data, datalen, socket_id);
#else
        UNUSED(socket_id);
        m_pSapEnv->OnUnsolicitedResponse(unsolResponse, data, datalen);
#endif // ANDROID_MULTI_SIM
    }
    else {
        RilLogW("[%s] %s cannot invoke m_pSapEnv->OnUnsolicitedResponse()", TAG, __FUNCTION__);
    }
}

// Filter out unsolicited response
bool SapResponseListener::FilterOut(int unsolResponse, const void * /* data */, unsigned int /* datalen */)
{
    switch (unsolResponse) {
    case RIL_UNSOL_RESPONSE_SIM_SAP_DISCONNECT:
    case RIL_UNSOL_RESPONSE_SIM_SAP_STATUS:
        break;
    default:
        return true;
    } // end switch ~

    return false;

}

void SapResponseListener::OnUpdateReqModemData(RIL_Token t, void *data, unsigned int len) {
    // In case of RIL request
    const struct RIL_EnvUnitTest *pRil_EnvUnitTest = (RIL_EnvUnitTest *)m_pSapEnv;
    if (pRil_EnvUnitTest != NULL && pRil_EnvUnitTest->OnUpdateReqModemData != NULL) {
        pRil_EnvUnitTest->OnUpdateReqModemData(t, data, len);
    } else {
        RilLogW("[%s] %s cannot invoke pRil_EnvUnitTest->OnUpdateReqModemData()", TAG, __FUNCTION__);
    }
}
