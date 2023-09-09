/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any    means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#include "callutils.h"
#include <telephony/ril.h>

const char* CallUtils::CallStateToString(int state) {
    static const char* CALL_STATE[] = {"ACTIVE", "HOLDING", "DIALING",
        "ALERTING", "INCOMING", "WAITING", "UNKNOWN"};
    if (RIL_CALL_ACTIVE <= state && state <= RIL_CALL_WAITING) {
        return CALL_STATE[state];
    }
    return CALL_STATE[RIL_CALL_WAITING + 1];
}

const char* CallUtils::getPresentationString(int present) {
    switch (present) {
        case 0:
            return "ALLOWED";
        case 1:
            return "RESTRICTED";
        case 2:
            return "UNKNOWN";
        case 3:
            return "PAYPHONE";
        default:
            return "UNKNOWN";
    }
}
