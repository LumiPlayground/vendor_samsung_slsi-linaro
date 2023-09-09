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
 * vsimprotocolutils.cpp
 *
 *  Created on: 2021. 08. 20.
 *      Author: aryaman.2019
 */

#include "vsimprotocolutils.h"
#include "rillog.h"

namespace VsimProtocolUtils {

// ProtocolVsimBuilder(Legacy/Json)
int ConvertVsimNotiRilEventIdToSitEventId(int eventid)
{
    switch(eventid)
    {
        case REQUEST_TYPE_ENABLE_EXTERNAL_SIM:
            return SIT_VSIM_NOTI_EN_EXTERNAL_SIM;
        case REQUEST_TYPE_DISABLE_EXTERNAL_SIM:
            return SIT_VSIM_NOTI_DIS_EXTERNAL_SIM;
        case REQUEST_TYPE_PLUG_OUT:
            return SIT_VSIM_NOTI_PLUG_OUT;
        case REQUEST_TYPE_PLUG_IN:
            return SIT_VSIM_NOTI_PLUG_IN;
        default:
            RilLogE("%s: invalid eventId:%d", __FUNCTION__, eventid);
            return -1;
    }
}

int ConvertVsimOpRilEventIdToSitEventId(int eventid)
{
    switch(eventid)
    {
        case REQUEST_TYPE_ATR_EVENT:
            return SIT_VSIM_OPERATION_ATR;
        case REQUEST_TYPE_APDU_EVENT:
            return SIT_VSIM_OPERATION_APDU;
        case REQUEST_TYPE_CARD_POWER_DOWN:
            return SIT_VSIM_OPERATION_POWERDOWN;
        default:
            RilLogE("%s: invalid eventId:%d", __FUNCTION__, eventid);
            return -1;
    }
}

// ProtocolVsimAdapter(Legacy/Json)
int ConvertVsimOpSitEventIdToRilEventId(int eventid)
{
    switch(eventid)
    {
        case SIT_VSIM_OPERATION_ATR:
            return REQUEST_TYPE_ATR_EVENT;
        case SIT_VSIM_OPERATION_APDU:
            return REQUEST_TYPE_APDU_EVENT;
        case SIT_VSIM_OPERATION_POWERDOWN:
            return REQUEST_TYPE_CARD_POWER_DOWN;
        default:
            RilLogE("%s: invalid eventId:%d", __FUNCTION__, eventid);
            return -1;
    }
}

}  // namespace VsimProtocolUtils
