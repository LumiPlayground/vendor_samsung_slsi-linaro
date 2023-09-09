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
 * timeutils.cpp
 *
 *  Created on: 2021. 9. 8.
 */
#include <time.h>
#include "systemproperty.h"
#include "timeutils.h"

namespace timeutils {
uint64_t getTimestamp() {
    uint64_t timestamp = 0UL;
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    timestamp = static_cast<uint64_t>(ts.tv_sec) * 1000;
    timestamp += ts.tv_nsec / 1000000;
    return timestamp;
}

std::string getTimezone() {
    // dependency by platform
    return SystemProperty::Get("persist.sys.timezone");
}

uint64_t getTimezoneOffset() {
    uint64_t offset = 0UL;
    time_t t;
    struct tm result;
    time(&t);
    if (localtime_r(&t, &result) != NULL) {
        offset = result.tm_gmtoff * 1000;
    }
    return offset;
}
uint64_t getCurrentTime() {
    uint64_t timestamp = 0UL;
    time_t timeSec;
    struct tm *pTmLocal;

    time(&timeSec);
    pTmLocal = localtime(&timeSec);

    timestamp = pTmLocal->tm_sec + (uint64_t)pTmLocal->tm_min*100 +
            (uint64_t)pTmLocal->tm_hour*10000UL +
            (uint64_t)pTmLocal->tm_mday*1000000UL +
            (uint64_t)(pTmLocal->tm_mon+1)*100000000UL +
            (uint64_t)(pTmLocal->tm_year+1900)*10000000000UL;
    return timestamp;
}
}  // namespace timeutils



