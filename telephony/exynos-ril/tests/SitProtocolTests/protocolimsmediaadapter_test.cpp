/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd All Rights Reserved
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
#include "protocolimsmediaadapter.h"
#include "sitdef.h"
#include "rildef.h"
#include "slsi/ril_oem.h"

TEST(ProtocolImsMediaAdapter, ProtocolImsMediaIndAdapter) {
    int testVector[][2] = {
            {RIL_UNSOL_OEM_IMS_TEST_RCM, SIT_IND_IMS_MEDIA_TEST_RCM},
            {RIL_UNSOL_OEM_IMS_MEDIA_ON_DTMF_RECEIVED, SIT_IND_IMS_MEDIA_ON_DTMF_RECEIVED},
            {RIL_UNSOL_OEM_IMS_MEDIA_ON_TEXT_RECEIVED, SIT_IND_IMS_MEDIA_ON_TEXT_RECEIVED},
            {RIL_UNSOL_OEM_IMS_MEDIA_ON_HEADER_EXTENSION_RECEIVED, SIT_IND_IMS_MEDIA_ON_HEADER_EXTENSION_RECEIVED},
            {RIL_UNSOL_OEM_IMS_MEDIA_ON_FIRST_MEDIA_RECEIVED, SIT_IND_IMS_MEDIA_ON_FIRST_MEDIA_RECEIVED},
            {RIL_UNSOL_OEM_IMS_MEDIA_NOTIFY_MEDIA_INACTIVITY, SIT_IND_IMS_MEDIA_NOTIFY_MEDIA_INACTIVITY},
            {RIL_UNSOL_OEM_IMS_MEDIA_NOTIFY_PACKET_LOSS, SIT_IND_IMS_MEDIA_NOTIFY_PACKET_LOSS},
            {RIL_UNSOL_OEM_IMS_MEDIA_NOTIFY_JITTER, SIT_IND_IMS_MEDIA_NOTIFY_JITTER},
            {RIL_UNSOL_OEM_IMS_MEDIA_ON_CALL_QUALITY_CHANGED, SIT_IND_IMS_MEDIA_ON_CALL_QUALITY_CHANGED},
            {RIL_UNSOL_OEM_IMS_MEDIA_TRIGGER_ANBR_QUERY, SIT_IND_IMS_MEDIA_TRIGGER_AMBR_QUERY},
            {RIL_UNSOL_OEM_IMS_MEDIA_MAC_CE_INFO, SIT_IND_MAC_CE_INFO},

            };
    for (auto testElement : testVector) {
        auto modemData = std::unique_ptr<ModemData>(ModemDataBuilder::BuildIndication(testElement[1]));
        ASSERT_NE(modemData, nullptr);
        ProtocolImsMediaIndAdapter testAdapter(modemData.get());
        EXPECT_EQ(testAdapter.GetResultId(), testElement[0]);
    }
}
