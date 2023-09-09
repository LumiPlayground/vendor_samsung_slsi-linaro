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
 * imsdatabuilder.cpp
 *
 *  Created on: 2014. 11. 27.
 *      Author: Martin
 */

#include "imsdatabuilder.h"
#include "rillog.h"

// add category to display selective logs
#undef RilLogI
#define RilLogI(format, ...) CRilLog::Log(RIL_LOG_CAT_IMS, CRilLog::E_RIL_INFO_LOG,  format, ##__VA_ARGS__)
#undef RilLogV
#define RilLogV(format, ...) CRilLog::Log(RIL_LOG_CAT_IMS, CRilLog::E_RIL_VERBOSE_LOG,  format, ##__VA_ARGS__)
#undef RilLogW
#define RilLogW(format, ...) CRilLog::Log(RIL_LOG_CAT_IMS, CRilLog::E_RIL_WARNING_LOG,  format, ##__VA_ARGS__)
#undef RilLogE
#define RilLogE(format, ...) CRilLog::Log(RIL_LOG_CAT_IMS, CRilLog::E_RIL_CRITICAL_LOG,  format, ##__VA_ARGS__)

#define ENTER_FUNC()        { RilLogV("ImsDataBuilder::%s() [<-- ", __FUNCTION__); }
#define LEAVE_FUNC()        { RilLogV("ImsDataBuilder::%s() [--> ", __FUNCTION__); }
#define BYTEBIT(byte, idx)        ((byte >> (8-idx)) & 0x01)

const RilData *ImsDataBuilder::BuildImsGetConfigResponse(const char * pResponse, int Length)
{
    return new RilDataRaw(pResponse, Length);
}

const RilData *ImsDataStartImsTrafficBuilder::BuildImsDataStartImsTrafficResponse(int reason, int causeCode, int waitTimeMillis) {
    return new RilDataPlaceHolder<RilImsV2_1::RIL_ConnectionFailureInfo>({reason, causeCode, waitTimeMillis});
}

const RilData *ImsDataConnectionSetupFailureBuilder::BuildImsDataConnectionSetupFailure(int token, int reason, int causeCode, int waitTimeMillis) {
    return new RilDataPlaceHolder<RilImsV2_1::RIL_OnConnectionSetupFailure>({token, {reason, causeCode, waitTimeMillis}});
}

const RilData *ImsDataTriggerEpsFallbackIndBuilder::BuildImsDataTriggerEpsFallbackInd(int result, int rat) {
    return new RilDataPlaceHolder<RilImsV2_1::RIL_TriggerFpsFallbackInd>({static_cast<uint8_t>(result), static_cast<uint8_t>(rat)});
}