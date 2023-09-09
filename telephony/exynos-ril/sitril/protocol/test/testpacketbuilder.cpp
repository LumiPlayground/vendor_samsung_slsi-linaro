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
 * TestPacketBuilder.cpp
 *
 *  Created on: 2016. 4. 18.
 */
#include "testpacketbuilder.h"
#include "rillog.h"
#include "rcmmgr.h"

#undef RilLogI
#define RilLogI(format, ...) CRilLog::Log(RIL_LOG_CAT_CORE, CRilLog::E_RIL_INFO_LOG,  format, ##__VA_ARGS__)

extern int log_cat_by_rcmid[RCM_CATEGORY];

int TestPacketBuilder::BuildNasTimerStartInd(char *buffer, unsigned int length)
{
    int size = (sizeof(sit_pdp_nas_timer_status_ind) > length) ? length : sizeof(sit_pdp_nas_timer_status_ind);

    const RCM_IND_HEADER nas_ind_header = {
        RCM_TYPE_INDICATION,
        0,
        SIT_IND_NAS_TIMER_STATUS,
        sizeof(sit_pdp_nas_timer_status_ind),
        {{0,}}
    };

    const sit_pdp_nas_timer_status_ind emulated_nas_ind = {
        nas_ind_header,
        2, // TIMER_TYPE: 0x00(invalid), 0x01(T3346), 0x02(T3396)
        1, // TIMER_STATUS: 0x00(invalid), 0x01(STARTED), 0x02(STOPPED), 0x03(EXPIRED)
        30000, // TIMER_VALUE: 0xFFFFFFFF(invalid)
        "VZWINTERNET" // New definition use apn name
           // PROFILE_ID: 0x00(invliad), 0x01(VZWIMS), 0x02(VZWFOTA), 0x03(VZWDEFAULT),
           //             0x04(VZWCBS), 0x05(VZWCAS), 0x06(VZWSUPL), 0x07(VZWIMS),
           //             0x08(VZWTETHERED), 0x09(VZWE911), 0x0A(VZWEMBMS), 0x0B(VZWBIP)
    };

    memcpy(buffer, &emulated_nas_ind, size);
    return size;

}

int TestPacketBuilder::BuildNasTimerExpiredInd(char *buffer, unsigned int length)
{
    int size = (sizeof(sit_pdp_nas_timer_status_ind) > length) ? length : sizeof(sit_pdp_nas_timer_status_ind);

    const RCM_IND_HEADER nas_ind_header = {
        RCM_TYPE_INDICATION,
        0,
        SIT_IND_NAS_TIMER_STATUS,
        sizeof(sit_pdp_nas_timer_status_ind),
        {{0,}}
    };

    const sit_pdp_nas_timer_status_ind emulated_nas_ind = {
        nas_ind_header,
        2,
        3,
        30000,
        "IMS"
    };

    memcpy(buffer, &emulated_nas_ind, size);
    return size;

}

size_t TestPacketBuilder::BuildPcoDataInd(char *buffer, unsigned int length)
{
    const RCM_IND_HEADER pco_data_ind_header = {
        RCM_TYPE_INDICATION,
        0,
        SIT_IND_PCO_DATA,
        sizeof(sit_pdp_pco_data_ind),
        {{0,}}
    };

    unsigned char pco_block1[] = {
        0x00, 0xFF, 0x00, 0x00, // pco_id
        1, // contents_len
        1  // contents
    };
    unsigned char pco_block2[] = {
        0x01, 0xFF, 0x00, 0x00, // pco_id
        1, // contents_len
        5  // contents
    };

    const sit_pdp_pco_data_ind emulated_pco_data_ind = {
        pco_data_ind_header,
        1, // cid
        3, // pdp_type
        2, // pco_num
    };

    size_t size = sizeof(emulated_pco_data_ind) + sizeof(pco_block1) + sizeof(pco_block2);
    if( length < size ) {
        RilLogE("length:%d is not enough for %d", length, size);
        return 0;
    }

    memcpy(buffer, &emulated_pco_data_ind, sizeof(emulated_pco_data_ind));
    memcpy(buffer + sizeof(emulated_pco_data_ind), &pco_block1, sizeof(pco_block1));
    memcpy(buffer + sizeof(emulated_pco_data_ind) + sizeof(pco_block1), &pco_block2, sizeof(pco_block2));
    return size;

}

INT32 TestPacketBuilder::DumpData(char *buffer,UINT32 bufLen, BOOL rx)
{
    return RadioControlMessageMgr::DumpDataStatic(buffer, bufLen, rx, "Emulated", 0/* LOGLV_SIM_IO_NONE */);
}

