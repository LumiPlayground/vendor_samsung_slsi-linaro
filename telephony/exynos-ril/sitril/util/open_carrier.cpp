/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#include "open_carrier.h"
#include "rillog.h"

#define OPEN_CARRIER_INFO_FILE    "vendor/etc/open_carrier_info.dat"
#define MAX_INPUT_STRING 20
#define MAX_OPEN_CARRIERO_TYPE_NUM 100
#define MAX_OPEN_CARRIER_PLMN_NUM 100

unsigned int GetOcNameByMccMnc(const char* mccmnc)
{
    char * temp;
    unsigned int numeric = 0;
    unsigned int ret = (unsigned int)OC_NONE;

    if ( mccmnc == NULL ) {
        return ret;
    }

    numeric = strtol(mccmnc, &temp, 10);
    if ( temp == mccmnc ) {
        return ret;
    } else {
        FILE *fp;
        int err;
        unsigned int numOfTypes = 0;
        unsigned int filePlmn = 0;
        unsigned int loopIndexI = 0;
        unsigned int loopIndexJ = 0;
        unsigned int type = 0;
        unsigned int numOfPlmns = 0;
        char typeName[MAX_INPUT_STRING] = { 0, };
        char buf[2][MAX_INPUT_STRING] = {{0}};

        if ((fp = fopen(OPEN_CARRIER_INFO_FILE, "re")) == NULL) {
            RilLogE("%s(): Couldn't open proc file %s", __FUNCTION__, OPEN_CARRIER_INFO_FILE);
            return ret;
        }

        err = fscanf(fp, "%19s\n", buf[0]);
        numOfTypes = strtoul(buf[0], NULL, 10);
        if (err == 1 ) {
            if (numOfTypes > MAX_OPEN_CARRIERO_TYPE_NUM) {
                RilLogE("%s(): numOfTypes(=%d) is out of range", __FUNCTION__, numOfTypes);
                numOfTypes = MAX_OPEN_CARRIERO_TYPE_NUM;
            }

            for (loopIndexI = 0; loopIndexI < numOfTypes; loopIndexI++)
            {
                err = fscanf(fp, "%19s %19s %19s", typeName, buf[0], buf[1]);
                type = strtoul(buf[0], NULL, 16);
                numOfPlmns = strtoul(buf[1], NULL, 10);
                if (err == 3) {
                    if (numOfPlmns > MAX_OPEN_CARRIER_PLMN_NUM) {
                        RilLogE("%s(): numOfPlmns(=%d) for (%d) is out of range", __FUNCTION__, numOfPlmns, loopIndexI);
                        numOfPlmns = MAX_OPEN_CARRIER_PLMN_NUM;
                    }

                    for (loopIndexJ = 0; loopIndexJ < numOfPlmns; loopIndexJ++)
                    {
                        err = fscanf(fp, "%19s", buf[0]);
                        filePlmn = strtoul(buf[0], NULL, 10);
                        if ( err == 1 ) {
                            if (filePlmn == numeric)
                            {
                                loopIndexI = numOfTypes + 1;
                                loopIndexJ = numOfPlmns + 1;
                                ret = type;
                            }
                        }else {
                            RilLogE("%s(): File read error in inner loop %d %d %d", __FUNCTION__, err, loopIndexI, loopIndexJ);
                            loopIndexJ = numOfPlmns + 1;
                            filePlmn = 0;
                        }
                    }
                } else {
                    RilLogE("%s(): File read error in outer loop %d %d %d", __FUNCTION__, err, loopIndexI, loopIndexJ);
                    loopIndexI = numOfTypes + 1;
                }
            }
        } else {
            RilLogE("%s(): File read error for numOfTypes(%d)", __FUNCTION__, err);
        }

        fclose(fp);
    }

    return ret;
}
