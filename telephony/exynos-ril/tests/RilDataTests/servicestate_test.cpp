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
 * servicestate_test.cpp
 *
 *  Created on: 2021. 8. 3.
 */
#include <gtest/gtest.h>
#include <telephony/ril.h>
#include "librilutils/textutils.h"
#include "rildef.h"
#include "servicestate.h"

TEST(ServiceState, ServiceState) {
    std::string testOperatorNumeric = "00101";
    ServiceState ss;
    ss.setVoiceRegState(RIL_RegState::RIL_REG_HOME);
    ss.setDataRegState(RIL_RegState::RIL_REG_HOME);
    ss.setOperatorNumeric(testOperatorNumeric);
    ss.setOperatorNumericRaw(testOperatorNumeric);
    ss.setIsManualSelection(true);
    ss.setVoiceRadioTechnology(RADIO_TECH_LTE);
    ss.setDataRadioTechnology(RADIO_TECH_LTE_CA);
    ss.setOperatorName("test bed long", "test bed short");
    ss.setOperatorNameRaw("test bed long", "test bed short");
    ss.setEmergencyOnly(true);
    ss.setChannelNumber(1000);
    ss.setLteVopsSupport(true);
    ss.setLteEmcBearerSupport(true);
    EXPECT_EQ(ss.getVoiceRegState(), RIL_RegState::RIL_REG_HOME);
    EXPECT_EQ(ss.getDataRegState(), RIL_RegState::RIL_REG_HOME);
    EXPECT_TRUE(TextUtils::Equals(ss.getOperatorNumeric(), "00101"));
    EXPECT_TRUE(TextUtils::Equals(ss.getOperatorNumericRaw(), "00101"));
    EXPECT_TRUE(TextUtils::Equals(ss.getOperatorAlphaLong(), "test bed long"));
    EXPECT_TRUE(TextUtils::Equals(ss.getOperatorAlphaShort(), "test bed short"));
    EXPECT_TRUE(TextUtils::Equals(ss.getOperatorAlphaLongRaw(), "test bed long"));
    EXPECT_TRUE(TextUtils::Equals(ss.getOperatorAlphaShortRaw(), "test bed short"));
    EXPECT_EQ(ss.getIsManualSelection(), true);
    EXPECT_EQ(ss.getVoiceRadioTechnology(), RADIO_TECH_LTE);
    EXPECT_EQ(ss.getDataRadioTechnology(), RADIO_TECH_LTE);
    EXPECT_EQ(ss.isUsingCarrierAggregation(), true);
    EXPECT_EQ(ss.getChannelNumber(), 1000);
    EXPECT_EQ(ss.isEmergencyOnly(), true);
    EXPECT_EQ(ss.getLteVopsSupport(), true);
    EXPECT_EQ(ss.getLteEmcBearerSupport(), true);
    EXPECT_EQ(ss.getNrStatus(), NR_STATUS_NONE);
    ss.updateNrStatus(true, false, true);
    EXPECT_EQ(ss.getNrStatus(), NR_STATUS_NOT_RESTRICTED);
    EXPECT_EQ(ss.isEndcAvailable(), true);
    EXPECT_EQ(ss.isDcNrRestricted(), false);
    EXPECT_EQ(ss.isNrAvailable(), true);
    ss.updateNrStatus(true, true, true);
    EXPECT_EQ(ss.getNrStatus(), NR_STATUS_RESTRICTED);
    ss.setNrStatus(NR_STATUS_CONNECTED);
    EXPECT_EQ(ss.getNrStatus(), NR_STATUS_CONNECTED);
    EXPECT_EQ(ss.isDcNrRestricted(), true);
    EXPECT_FALSE(ss.isVoiceOos());
    EXPECT_FALSE(ss.isDataOos());
    EXPECT_TRUE(ss.toString().length() > 0);

    ServiceState copy = ss;
    EXPECT_TRUE(ss == copy);

    copy.setVoiceRegState(RIL_RegState::RIL_NOT_REG_AND_NOT_SEARCHING);
    copy.setVoiceRegState(-1);
    copy.setDataRegState(RIL_RegState::RIL_NOT_REG_AND_NOT_SEARCHING);
    copy.setDataRegState(-1);
    copy.setVoiceRadioTechnology(100);
    copy.setDataRadioTechnology(100);
    int test[] = {
        RIL_NOT_REG_AND_NOT_SEARCHING,
        RIL_NOT_REG_AND_SEARCHING,
        RIL_UNKNOWN,
        RIL_NOT_REG_AND_EMERGENCY_AVAILABLE_AND_NOT_SEARCHING,
        RIL_NOT_REG_AND_EMERGENCY_AVAILABLE_AND_SEARCHING,
        RIL_UNKNOWN_AND_EMERGENCY_AVAILABLE,
    };
    for (unsigned int i = 0; i < sizeof(test) / sizeof(test[0]); i++) {
        int state = test[i];
        copy.setVoiceRegState(state);
        EXPECT_TRUE(copy.isVoiceOos());
        copy.setDataRegState(state);
        EXPECT_TRUE(copy.isDataOos());
    }
    EXPECT_TRUE(ss != copy);
}
