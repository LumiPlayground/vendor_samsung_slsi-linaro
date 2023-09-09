/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd All Rights Reserved
 * PROPRIETARY/CONFIDENTIAL
 *
 * This software is the confidential and proprietary information of
 * SAMSUNG ELECTRONICS ("Confidential Information").
 *
 * You shall not disclose such Confidential Information and shall use it
 * only in accordance with the terms of the license agreement
 * you entered into with SAMSUNG ELECTRONICS.
 *
 * SAMSUNG make no representations or warranties about the suitability of
 * the software, either express or implied, including but not limited to
 * the implied warranties of merchantability, fitness for a particular
 * purpose, or non-infringement. SAMSUNG shall not be liable for any
 * damages suffered by license as a result of using, modifying or
 * distributing this software or its derivatives.
 */
#include <gtest/gtest.h>

#include "modemdatabuilder.h"
#include "modemdata.h"
#include "protocolgpsadapter.h"
#include "sitdef.h"
#include "slsi/ril_oem.h"

TEST(ProtocolGpsAdapter, ProtocolAgpsIndAdapter) {

    // index 0 : OEM RIL ID
    // index 1 : SIT message ID
    int testVector[][2] = {
        { RIL_UNSOL_OEM_GPS_MEASURE_POS_REQ, SIT_IND_GPS_MEASURE_POS_REQ },
        { RIL_UNSOL_OEM_GPS_ASSIST_DATA, SIT_IND_GPS_ASSIST_DATA },
        { RIL_UNSOL_OEM_GPS_RELEASE_GPS, SIT_IND_RELEASE_GPS },
        { RIL_UNSOL_OEM_GPS_MT_LOCATION_REQUEST, SIT_IND_GPS_MT_LOCATION_REQUEST },
        { RIL_UNSOL_OEM_GPS_RESET_GPS_ASSIST_DATA, SIT_IND_RESET_GPS_ASSIST_DATA },
        { RIL_UNSOL_OEM_GPS_LPP_REQUEST_CAPABILITIES, SIT_IND_LPP_REQUEST_CAPABILITIES },
        { RIL_UNSOL_OEM_GPS_LPP_PROVIDE_ASSIST_DATA, SIT_IND_LPP_PROVIDE_ASSIST_DATA },
        { RIL_UNSOL_OEM_GPS_LPP_REQUEST_LOCATION_INFO, SIT_IND_LPP_REQUEST_LOCATION_INFO },
        { RIL_UNSOL_OEM_GPS_LPP_GPS_ERROR_IND, SIT_LPP_GPS_ERROR_IND },
        { RIL_UNSOL_OEM_GPS_SUPL_LPP_DATA_INFO, SIT_IND_SUPL_LPP_DATA_INFO },
        { RIL_UNSOL_OEM_GPS_SUPL_NI_MESSAGE, SIT_IND_SUPL_NI_MESSAGE },
        { RIL_UNSOL_OEM_GPS_SUPL_NI_READY, SIT_SET_GPS_SUPL_NI_READY },
        { RIL_UNSOL_OEM_GPS_START_MDT_LOC, SIT_IND_GPS_START_MDT_LOC },
        { RIL_UNSOL_OEM_GPS_LPP_UPDATE_UE_LOC_INFO, SIT_IND_LPP_UPDATE_UE_LOC_INFO },
        { RIL_UNSOL_OEM_GPS_LOCK_MODE, SIT_IND_GPS_LOCK_MODE },
        // CDMA & HEDGE GANSS
        { RIL_UNSOL_OEM_GPS_3GPP_SEND_GANSS_ASSIT_DATA, SIT_IND_3GPP_SEND_GANSS_ASSIT_DATA },
        { RIL_UNSOL_OEM_GPS_GANSS_MEAS_POS_MSG, SIT_IND_GANSS_MEAS_POS_MSG },
        { RIL_UNSOL_OEM_GPS_CDMA_GPS_POWER_ON, SIT_IND_CDMA_GPS_POWER_ON },
        { RIL_UNSOL_OEM_GPS_CDMA_SEND_ACQUSITION_ASSIT_DATA, SIT_IND_CDMA_SEND_ACQUSITION_ASSIT_DATA },
        { RIL_UNSOL_OEM_GPS_CDMA_SESSION_CANCELLATION, SIT_IND_CDMA_SESSION_CANCELLATION },
        { RIL_UNSOL_OEM_GPS_GANSS_AP_POS_CAP_REQ, SIT_IND_GANSS_AP_POS_CAP_REQ },
        // { -1, -1},  // Impossible TC, UINT type can not care negative invalid case.
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {
        ModemData *modemData = ModemDataBuilder::BuildIndication(testVector[i][1]);
        ASSERT_NE(modemData, nullptr);

        ProtocolAgpsIndAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetResultId(), testVector[i][0]);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    }  // end for i ~
}
