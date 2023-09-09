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
 * vsimbuilder.cpp
 *
 *  Created on: 2016. 02. 26.
 */
#include "vsimdatabuilder.h"

const RilData *VsimDataBuilder::BuildVsimOperationExt(int tid, int eventid, int result, int datalen, const char* data)
{
    if (datalen < 0) {
        datalen = 0;
    }

    int totalLen = 4 * 4 + datalen;
    char *buf = new char[totalLen];
    if (buf == NULL) {
        return NULL;
    }

    ((int *)buf)[0] = tid;
    ((int *)buf)[1] = eventid;
    ((int *)buf)[2] = result;
    ((int *)buf)[3] = datalen;

    if (datalen > 0 && data != NULL) {
        memcpy(&((int *)buf)[4], data, datalen);
    }

    RilDataRaw *rildata = new RilDataRaw(buf, totalLen);
    delete[] buf;

    return rildata;
}
