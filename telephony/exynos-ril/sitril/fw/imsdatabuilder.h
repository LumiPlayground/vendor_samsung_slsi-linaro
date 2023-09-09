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
 * miscdatabuilder.h
 *
 *  Created on: 2014. 11. 24.
 *      Author: sungwoo48.choi
 */

#ifndef __IMS_DATA_BUILDER_H__
#define __IMS_DATA_BUILDER_H__

#include "rildatabuilder.h"
#include <slsi/radio_v2_1.h>

class ImsDataBuilder : public RilDataBuilder {
public:
    const RilData *BuildImsGetConfigResponse(const char * pResponse, int Length);
};

class ImsDataStartImsTrafficBuilder : public RilDataBuilder {
public:
    const RilData *BuildImsDataStartImsTrafficResponse(int reason, int causeCode, int waitTimeMillis);
};

class ImsDataConnectionSetupFailureBuilder : public RilDataBuilder {
public:
    const RilData *BuildImsDataConnectionSetupFailure(int token, int reason, int causeCode, int waitTimeMillis);
};

class ImsDataTriggerEpsFallbackIndBuilder : public RilDataBuilder {
public:
    const RilData *BuildImsDataTriggerEpsFallbackInd(int result, int rat);
};

#endif /* __IMS_DATA_BUILDER_H__ */
