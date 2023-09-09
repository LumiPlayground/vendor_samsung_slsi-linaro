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
 * vsimservice.cpp
 *
 *  Created on: 2016. 02. 26.
 */

#include "vsimservice.h"
#include "rillog.h"

#undef RilLogI
#define RilLogI(format, ...) CRilLog::Log(RIL_LOG_CAT_VSIM, CRilLog::E_RIL_INFO_LOG,  format, ##__VA_ARGS__)
#undef RilLogV
#define RilLogV(format, ...) CRilLog::Log(RIL_LOG_CAT_VSIM, CRilLog::E_RIL_VERBOSE_LOG,  format, ##__VA_ARGS__)
#undef RilLogW
#define RilLogW(format, ...) CRilLog::Log(RIL_LOG_CAT_VSIM, CRilLog::E_RIL_WARNING_LOG,  format, ##__VA_ARGS__)
#undef RilLogE
#define RilLogE(format, ...) CRilLog::Log(RIL_LOG_CAT_VSIM, CRilLog::E_RIL_CRITICAL_LOG,  format, ##__VA_ARGS__)

VSimService::VSimService(RilContext* pRilContext)
    : Service(pRilContext, RIL_SERVICE_VSIM)
{

}

VSimService::~VSimService()
{
}


int VSimService::OnCreate(RilContext * /* pRilContext */)
{
    RilLog("[%s] %s()", GetServiceName(), __FUNCTION__);
    return 0;
}

bool VSimService::IsPossibleToPassInRadioOffState(int request_id)
{
    switch (request_id) {
        case RIL_LOCAL_REQUEST_VSIM_NOTIFICATION:
        case RIL_LOCAL_REQUEST_VSIM_OPERATION:
        case RIL_REQUEST_OEM_VSIM_NOTIFICATION:
        case RIL_REQUEST_OEM_VSIM_OPERATION:
            break;
        default:
            return false;
    }
    return true;
}

bool VSimService::IsPossibleToPassInRadioUnavailableState(int request_id)
{
    switch(request_id) {
        //case RIL_LOCAL_REQUEST_VSIM_NOTIFICATION:
        case RIL_LOCAL_REQUEST_VSIM_OPERATION:
        //case RIL_REQUEST_OEM_VSIM_NOTIFICATION:
        case RIL_REQUEST_OEM_VSIM_OPERATION:
            break;
        default:
            return false;
    }
    return true;
}
