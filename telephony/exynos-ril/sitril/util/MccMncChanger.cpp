/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#include "MccMncChanger.h"
#include "constdef.h"
#include "rillog.h"

#undef RilLogI
#define RilLogI(format, ...) CRilLog::Log(RIL_LOG_CAT_NET, CRilLog::E_RIL_INFO_LOG,  format, ##__VA_ARGS__)
#undef RilLogV
#define RilLogV(format, ...) CRilLog::Log(RIL_LOG_CAT_NET, CRilLog::E_RIL_VERBOSE_LOG,  format, ##__VA_ARGS__)
#undef RilLogW
#define RilLogW(format, ...) CRilLog::Log(RIL_LOG_CAT_NET, CRilLog::E_RIL_WARNING_LOG,  format, ##__VA_ARGS__)
#undef RilLogE
#define RilLogE(format, ...) CRilLog::Log(RIL_LOG_CAT_NET, CRilLog::E_RIL_CRITICAL_LOG,  format, ##__VA_ARGS__)

#define NUM_ELEMS(a)    (sizeof (a) / sizeof (a[0]))

static MccMncItem mccmncList[] = {
#include "../database/mccmncList.h"
};

bool MccMncChanger::changeMccMncList(const char* mccmnc, char* changedMccMnc) {
    bool success = false;
    MccMncItem *items;

    int totalCount = (int) NUM_ELEMS(mccmncList);
    items = mccmncList;
    RilLogI("MccMncChanger mccmncList total count=%d, mccmnc=%s", totalCount, mccmnc);

    for (int i = 0; i < totalCount; i++) {
        RilLogI("MccMncChanger item[%d]=%s, %s", i, items[i].srcMccMnc.c_str(),
                items[i].desMccMnc.c_str());
        if (strncmp(mccmnc, items[i].srcMccMnc.c_str(), (MAX_MCC_LEN + MAX_MNC_LEN)) == 0) {
            strncpy(changedMccMnc, items[i].desMccMnc.c_str(), (MAX_MCC_LEN + MAX_MNC_LEN));
            RilLogI("MccMncChanger changed mccmnc=%s", changedMccMnc);
            success = true;
        }
    }

    return success;
}

bool MccMncChanger::checkMccMncToChange(const char *mccmnc, char *changedMccMnc) {
    if (mccmnc == NULL || changedMccMnc == NULL) {
        RilLogI("MccMncChanger.checkMccMncToChange: mccmnc is NULL");
        return false;
    }

    RilLogI("MccMncChanger.checkMccMncToChange: mccmnc=%s", mccmnc);
    return changeMccMncList(mccmnc, changedMccMnc);
}
