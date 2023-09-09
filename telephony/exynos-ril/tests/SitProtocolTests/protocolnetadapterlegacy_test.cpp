/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */
#include <gtest/gtest.h>

#include "modemdatabuilder.h"
#include "modemdata.h"
#include "networkutils.h"
#include "netprotocolutils.h"
#include "legacy/network/protocolnetadapterlegacy.h"

#include <librilutils/textutils.h>

#define NUM_ELEMENTS(ar) (sizeof(ar)/sizeof(ar[0]))

TEST(ProtocolNetAdapterLegacy, ProtocolNetVoiceRegStateAdapterLegacy_cs_reg_state) {
    char packets[] =  {
        0x01, 0x00, 0x00, 0x07, 0x40, 0x00, 0xfa, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x0e, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x33, 0x27, 0x00, 0x00, 0xd3, 0x00, 0x00, 0x00,
        0x27, 0xd4, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xff, 0xff, 0xff, 0x7f, 0xff, 0xff, 0xff, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01,
    };

    ModemData *modemData = new ModemData(packets, sizeof(packets));
    ASSERT_NE(modemData, nullptr);

    ProtocolNetVoiceRegStateAdapterLegacy testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_CS_REG_STATE);
    EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_net_get_cs_reg_state_rsp));
    EXPECT_NE(testAdapter.GetParameter(), nullptr);
    EXPECT_EQ(testAdapter.GetParameterLength(), 52U);

    EXPECT_EQ(testAdapter.GetRegState(), REGISTERED_HOME);
    EXPECT_EQ(testAdapter.GetRejectCause(), SIT_NET_REJ_CAUSE_GENERAL);
    EXPECT_EQ(testAdapter.GetRadioTech(), RADIO_TECH_LTE);
    EXPECT_EQ(testAdapter.GetLAC(), 0);
    EXPECT_EQ(testAdapter.GetCellId(), 0);
    EXPECT_EQ(testAdapter.GetPSC(), 0);
    EXPECT_EQ(testAdapter.GetTAC(), 0x2733);
    EXPECT_EQ(testAdapter.GetPCID(), 0xd3);
    EXPECT_EQ(testAdapter.GetECI(), 0x12d427);
    EXPECT_EQ(testAdapter.GetStationId(), 0);
    EXPECT_EQ(testAdapter.GetStationLat(), 0x7FFFFFFF);
    EXPECT_EQ(testAdapter.GetStationLong(), 0x7FFFFFFF);
    EXPECT_EQ(testAdapter.GetConCurrent(), 0);
    EXPECT_EQ(testAdapter.GetSystemId(), 0);
    EXPECT_EQ(testAdapter.GetNetworkId(), 0);
    EXPECT_EQ(testAdapter.GetRoamingInd(), 1);
    EXPECT_EQ(testAdapter.GetRegPrl(), 0);
    EXPECT_EQ(testAdapter.GetRoamingIndPrl(), 1);
    EXPECT_EQ(testAdapter.getChannelNumber(), 0);
    EXPECT_EQ(testAdapter.getNrCid(), 0);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolNetAdapterLegacy, ProtocolNetVoiceRegStateAdapterLegacy_cs_reg_state_v1_0) {
    char packets[] =  {
        0x01, 0x00, 0x00, 0x07, 0x44, 0x00, 0xfa, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x0e, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x33, 0x27, 0x00, 0x00, 0xd3, 0x00, 0x00, 0x00,
        0x27, 0xd4, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xff, 0xff, 0xff, 0x7f, 0xff, 0xff, 0xff, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01,
        0xc4, 0x09, 0x00, 0x00,
    };

    ModemData *modemData = new ModemData(packets, sizeof(packets));
    ASSERT_NE(modemData, nullptr);

    ProtocolNetVoiceRegStateAdapterLegacy testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_CS_REG_STATE);
    EXPECT_EQ(testAdapter.GetLength(), sizeof(RCM_HEADER) + sizeof(sit_cs_reg_state_v1_0));
    EXPECT_NE(testAdapter.GetParameter(), nullptr);
    EXPECT_EQ(testAdapter.GetParameterLength(), 56U);

    EXPECT_EQ(testAdapter.GetRegState(), REGISTERED_HOME);
    EXPECT_EQ(testAdapter.GetRejectCause(), SIT_NET_REJ_CAUSE_GENERAL);
    EXPECT_EQ(testAdapter.GetRadioTech(), RADIO_TECH_LTE);
    EXPECT_EQ(testAdapter.GetLAC(), 0);
    EXPECT_EQ(testAdapter.GetCellId(), 0);
    EXPECT_EQ(testAdapter.GetPSC(), 0);
    EXPECT_EQ(testAdapter.GetTAC(), 0x2733);
    EXPECT_EQ(testAdapter.GetPCID(), 0xd3);
    EXPECT_EQ(testAdapter.GetECI(), 0x12d427);
    EXPECT_EQ(testAdapter.GetStationId(), 0);
    EXPECT_EQ(testAdapter.GetStationLat(), 0x7FFFFFFF);
    EXPECT_EQ(testAdapter.GetStationLong(), 0x7FFFFFFF);
    EXPECT_EQ(testAdapter.GetConCurrent(), 0);
    EXPECT_EQ(testAdapter.GetSystemId(), 0);
    EXPECT_EQ(testAdapter.GetNetworkId(), 0);
    EXPECT_EQ(testAdapter.GetRoamingInd(), 1);
    EXPECT_EQ(testAdapter.GetRegPrl(), 0);
    EXPECT_EQ(testAdapter.GetRoamingIndPrl(), 1);
    EXPECT_EQ(testAdapter.getChannelNumber(), 0x9c4);
    EXPECT_EQ(testAdapter.getNrCid(), 0);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolNetAdapterLegacy, ProtocolNetVoiceRegStateAdapterLegacy_cs_reg_state_v1_1) {
    char packets[] =  {
        0x01, 0x00, 0x00, 0x07, 0x58, 0x00, 0xfa, 0x01, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x15, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x33, 0x27, 0x00, 0x00, 0xd3, 0x00, 0x00, 0x00,
        0x27, 0xd4, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xff, 0xff, 0xff, 0x7f, 0xff, 0xff, 0xff, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01,
        0xc4, 0x09, 0x00, 0x00, 0x56, 0x34, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x54, 0x76, 0x98, 0x00,
        0x56, 0x34, 0x12, 0x00, 0xb8, 0x0b, 0x00, 0x00,
    };

    ModemData *modemData = new ModemData(packets, sizeof(packets));
    ASSERT_NE(modemData, nullptr);

    ProtocolNetVoiceRegStateAdapterLegacy testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_CS_REG_STATE);
    EXPECT_EQ(testAdapter.GetLength(), sizeof(RCM_HEADER) + sizeof(sit_cs_reg_state_v1_1));
    EXPECT_NE(testAdapter.GetParameter(), nullptr);
    EXPECT_EQ(testAdapter.GetParameterLength(), 76U);

    EXPECT_EQ(testAdapter.GetRegState(), REGISTERED_ROAMING);
    EXPECT_EQ(testAdapter.GetRejectCause(), SIT_NET_REJ_CAUSE_GENERAL);
    EXPECT_EQ(testAdapter.GetRadioTech(), RADIO_TECH_NR);
    EXPECT_EQ(testAdapter.GetLAC(), 0);
    EXPECT_EQ(testAdapter.GetCellId(), 0);
    EXPECT_EQ(testAdapter.GetPSC(), 0);
    EXPECT_EQ(testAdapter.GetTAC(), 0x123456);
    EXPECT_EQ(testAdapter.GetPCID(), 0x987654);
    EXPECT_EQ(testAdapter.GetECI(), 0x12d427);
    EXPECT_EQ(testAdapter.GetStationId(), 0);
    EXPECT_EQ(testAdapter.GetStationLat(), 0x7FFFFFFF);
    EXPECT_EQ(testAdapter.GetStationLong(), 0x7FFFFFFF);
    EXPECT_EQ(testAdapter.GetConCurrent(), 0);
    EXPECT_EQ(testAdapter.GetSystemId(), 0);
    EXPECT_EQ(testAdapter.GetNetworkId(), 0);
    EXPECT_EQ(testAdapter.GetRoamingInd(), 1);
    EXPECT_EQ(testAdapter.GetRegPrl(), 0);
    EXPECT_EQ(testAdapter.GetRoamingIndPrl(), 1);
    EXPECT_EQ(testAdapter.getChannelNumber(), 0xbb8);
    EXPECT_EQ(testAdapter.getNrCid(), 0x123456);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolNetAdapterLegacy, ProtocolNetDataRegStateAdapterLegacy_ps_reg_state) {
    char packets[] =  {
        0x01, 0x00, 0x01, 0x07, 0x2F, 0x00, 0xd0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x0e,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x33, 0x27, 0x00, 0x00, 0xd3, 0x00, 0x00,
        0x00, 0x27, 0xd4, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
    };
    ModemData *modemData = new ModemData(packets, sizeof(packets));
    ASSERT_NE(modemData, nullptr);

    ProtocolNetDataRegStateAdapterLegacy testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_PS_REG_STATE);
    EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_net_get_ps_reg_state_rsp));
    EXPECT_NE(testAdapter.GetParameter(), nullptr);
    EXPECT_EQ(testAdapter.GetParameterLength(), 35U);

    EXPECT_EQ(testAdapter.GetRegState(), REGISTERED_HOME);
    EXPECT_EQ(testAdapter.GetRejectCause(), SIT_NET_REJ_CAUSE_GENERAL);
    EXPECT_EQ(testAdapter.GetMaxSDC(), 0);
    EXPECT_EQ(testAdapter.GetRadioTech(), RADIO_TECH_LTE);
    EXPECT_EQ(testAdapter.GetLAC(), 0);
    EXPECT_EQ(testAdapter.GetCellId(), 0);
    EXPECT_EQ(testAdapter.GetPSC(), 0);
    EXPECT_EQ(testAdapter.GetTAC(), 0x2733);
    EXPECT_EQ(testAdapter.GetPCID(), 0xd3);
    EXPECT_EQ(testAdapter.GetECI(), 0x12d427);
    EXPECT_EQ(testAdapter.GetCSGID(), 0);
    EXPECT_EQ(testAdapter.GetTADV(), 0);
    EXPECT_EQ(testAdapter.GetImsVops(), 1);
    EXPECT_EQ(testAdapter.GetEmcService(), 1);
    EXPECT_EQ(testAdapter.getChannelNumber(), 0);
    EXPECT_EQ(testAdapter.IsEndcAvailable(), false);
    EXPECT_EQ(testAdapter.IsDcNrRestricted(), false);
    EXPECT_EQ(testAdapter.IsNrAvailable(), false);
    EXPECT_EQ(testAdapter.getNrCid(), 0);
    EXPECT_EQ(testAdapter.GetEmf(), 0);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolNetAdapterLegacy, ProtocolNetDataRegStateAdapterLegacy_ps_reg_state_v1_0) {
    char packets[] =  {
        0x01, 0x00, 0x01, 0x07, 0x33, 0x00, 0xd0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x0e,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x33, 0x27, 0x00, 0x00, 0xd3, 0x00, 0x00,
        0x00, 0x27, 0xd4, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0xc4,
        0x09, 0x00, 0x00,
    };
    ModemData *modemData = new ModemData(packets, sizeof(packets));
    ASSERT_NE(modemData, nullptr);

    ProtocolNetDataRegStateAdapterLegacy testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_PS_REG_STATE);
    EXPECT_EQ(testAdapter.GetLength(), sizeof(RCM_HEADER) + sizeof(sit_ps_reg_state_v1_0));
    EXPECT_NE(testAdapter.GetParameter(), nullptr);
    EXPECT_EQ(testAdapter.GetParameterLength(), 39U);

    EXPECT_EQ(testAdapter.GetRegState(), REGISTERED_HOME);
    EXPECT_EQ(testAdapter.GetRejectCause(), SIT_NET_REJ_CAUSE_GENERAL);
    EXPECT_EQ(testAdapter.GetMaxSDC(), 0);
    EXPECT_EQ(testAdapter.GetRadioTech(), RADIO_TECH_LTE);
    EXPECT_EQ(testAdapter.GetLAC(), 0);
    EXPECT_EQ(testAdapter.GetCellId(), 0);
    EXPECT_EQ(testAdapter.GetPSC(), 0);
    EXPECT_EQ(testAdapter.GetTAC(), 0x2733);
    EXPECT_EQ(testAdapter.GetPCID(), 0xd3);
    EXPECT_EQ(testAdapter.GetECI(), 0x12d427);
    EXPECT_EQ(testAdapter.GetCSGID(), 0);
    EXPECT_EQ(testAdapter.GetTADV(), 0);
    EXPECT_EQ(testAdapter.GetImsVops(), 1);
    EXPECT_EQ(testAdapter.GetEmcService(), 1);
    EXPECT_EQ(testAdapter.getChannelNumber(), 0x9c4);
    EXPECT_EQ(testAdapter.IsEndcAvailable(), false);
    EXPECT_EQ(testAdapter.IsDcNrRestricted(), false);
    EXPECT_EQ(testAdapter.IsNrAvailable(), false);
    EXPECT_EQ(testAdapter.getNrCid(), 0);
    EXPECT_EQ(testAdapter.GetEmf(), 0);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolNetAdapterLegacy, ProtocolNetDataRegStateAdapterLegacy_ps_reg_state_v1_1) {
    char packets[] =  {
        0x01, 0x00, 0x01, 0x07, 0x36, 0x00, 0xd0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x0e,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x33, 0x27, 0x00, 0x00, 0xd3, 0x00, 0x00,
        0x00, 0x27, 0xd4, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0xc4,
        0x09, 0x00, 0x00, 0x00, 0x01, 0x01
    };
    ModemData *modemData = new ModemData(packets, sizeof(packets));
    ASSERT_NE(modemData, nullptr);

    ProtocolNetDataRegStateAdapterLegacy testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_PS_REG_STATE);
    EXPECT_EQ(testAdapter.GetLength(), sizeof(RCM_HEADER) + sizeof(sit_ps_reg_state_v1_1));
    EXPECT_NE(testAdapter.GetParameter(), nullptr);
    EXPECT_EQ(testAdapter.GetParameterLength(), 42U);

    EXPECT_EQ(testAdapter.GetRegState(), REGISTERED_HOME);
    EXPECT_EQ(testAdapter.GetRejectCause(), SIT_NET_REJ_CAUSE_GENERAL);
    EXPECT_EQ(testAdapter.GetMaxSDC(), 0);
    EXPECT_EQ(testAdapter.GetRadioTech(), RADIO_TECH_LTE);
    EXPECT_EQ(testAdapter.GetLAC(), 0);
    EXPECT_EQ(testAdapter.GetCellId(), 0);
    EXPECT_EQ(testAdapter.GetPSC(), 0);
    EXPECT_EQ(testAdapter.GetTAC(), 0x2733);
    EXPECT_EQ(testAdapter.GetPCID(), 0xd3);
    EXPECT_EQ(testAdapter.GetECI(), 0x12d427);
    EXPECT_EQ(testAdapter.GetCSGID(), 0);
    EXPECT_EQ(testAdapter.GetTADV(), 0);
    EXPECT_EQ(testAdapter.GetImsVops(), 1);
    EXPECT_EQ(testAdapter.GetEmcService(), 1);
    EXPECT_EQ(testAdapter.getChannelNumber(), 0x9c4);
    EXPECT_EQ(testAdapter.IsEndcAvailable(), false);
    EXPECT_EQ(testAdapter.IsDcNrRestricted(), true);
    EXPECT_EQ(testAdapter.IsNrAvailable(), true);
    EXPECT_EQ(testAdapter.getNrCid(), 0);
    EXPECT_EQ(testAdapter.GetEmf(), 0);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolNetAdapterLegacy, ProtocolNetDataRegStateAdapterLegacy_ps_reg_state_v1_2) {
    char packets[] =  {
        0x01, 0x00, 0x01, 0x07, 0x4a, 0x00, 0xd0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x15,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x33, 0x27, 0x00, 0x00, 0xd3, 0x00, 0x00,
        0x00, 0x27, 0xd4, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0xc4,
        0x09, 0x00, 0x00, 0x00, 0x01, 0x01, 0x56, 0x34, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x54, 0x76,
        0x98, 0x00, 0x56, 0x34, 0x12, 0x00, 0xb8, 0x0b, 0x00, 0x00
    };
    ModemData *modemData = new ModemData(packets, sizeof(packets));
    ASSERT_NE(modemData, nullptr);

    ProtocolNetDataRegStateAdapterLegacy testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_PS_REG_STATE);
    EXPECT_EQ(testAdapter.GetLength(), sizeof(RCM_HEADER) + sizeof(sit_ps_reg_state_v1_2));
    EXPECT_NE(testAdapter.GetParameter(), nullptr);
    EXPECT_EQ(testAdapter.GetParameterLength(), 62U);

    EXPECT_EQ(testAdapter.GetRegState(), REGISTERED_HOME);
    EXPECT_EQ(testAdapter.GetRejectCause(), SIT_NET_REJ_CAUSE_GENERAL);
    EXPECT_EQ(testAdapter.GetMaxSDC(), 0);
    EXPECT_EQ(testAdapter.GetRadioTech(), RADIO_TECH_NR);
    EXPECT_EQ(testAdapter.GetLAC(), 0);
    EXPECT_EQ(testAdapter.GetCellId(), 0);
    EXPECT_EQ(testAdapter.GetPSC(), 0);
    EXPECT_EQ(testAdapter.GetTAC(), 0x123456);
    EXPECT_EQ(testAdapter.GetPCID(), 0x987654);
    EXPECT_EQ(testAdapter.GetECI(), 0x12d427);
    EXPECT_EQ(testAdapter.GetCSGID(), 0);
    EXPECT_EQ(testAdapter.GetTADV(), 0);
    EXPECT_EQ(testAdapter.GetImsVops(), 1);
    EXPECT_EQ(testAdapter.GetEmcService(), 1);
    EXPECT_EQ(testAdapter.getChannelNumber(), 0xbb8);
    EXPECT_EQ(testAdapter.IsEndcAvailable(), false);
    EXPECT_EQ(testAdapter.IsDcNrRestricted(), true);
    EXPECT_EQ(testAdapter.IsNrAvailable(), true);
    EXPECT_EQ(testAdapter.getNrCid(), 0x123456);
    EXPECT_EQ(testAdapter.GetEmf(), 0);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolNetAdapterLegacy, ProtocolNetDataRegStateAdapterLegacy_ps_reg_state_v1_3) {
    char packets[] =  {
        0x01, 0x00, 0x01, 0x07, 0x4e, 0x00, 0xd0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x15,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x33, 0x27, 0x00, 0x00, 0xd3, 0x00, 0x00,
        0x00, 0x27, 0xd4, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0xc4,
        0x09, 0x00, 0x00, 0x00, 0x01, 0x01, 0x56, 0x34, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x54, 0x76,
        0x98, 0x00, 0x56, 0x34, 0x12, 0x00, 0xb8, 0x0b, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
    };
    ModemData *modemData = new ModemData(packets, sizeof(packets));
    ASSERT_NE(modemData, nullptr);

    ProtocolNetDataRegStateAdapterLegacy testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_PS_REG_STATE);
    EXPECT_EQ(testAdapter.GetLength(), sizeof(RCM_HEADER) + sizeof(sit_ps_reg_state_v1_3));
    EXPECT_NE(testAdapter.GetParameter(), nullptr);
    EXPECT_EQ(testAdapter.GetParameterLength(), 66U);

    EXPECT_EQ(testAdapter.GetRegState(), REGISTERED_HOME);
    EXPECT_EQ(testAdapter.GetRejectCause(), SIT_NET_REJ_CAUSE_GENERAL);
    EXPECT_EQ(testAdapter.GetMaxSDC(), 0);
    EXPECT_EQ(testAdapter.GetRadioTech(), RADIO_TECH_NR);
    EXPECT_EQ(testAdapter.GetLAC(), 0);
    EXPECT_EQ(testAdapter.GetCellId(), 0);
    EXPECT_EQ(testAdapter.GetPSC(), 0);
    EXPECT_EQ(testAdapter.GetTAC(), 0x123456);
    EXPECT_EQ(testAdapter.GetPCID(), 0x987654);
    EXPECT_EQ(testAdapter.GetECI(), 0x12d427);
    EXPECT_EQ(testAdapter.GetCSGID(), 0);
    EXPECT_EQ(testAdapter.GetTADV(), 0);
    EXPECT_EQ(testAdapter.GetImsVops(), 1);
    EXPECT_EQ(testAdapter.GetEmcService(), 1);
    EXPECT_EQ(testAdapter.getChannelNumber(), 0xbb8);
    EXPECT_EQ(testAdapter.IsEndcAvailable(), false);
    EXPECT_EQ(testAdapter.IsDcNrRestricted(), true);
    EXPECT_EQ(testAdapter.IsNrAvailable(), true);
    EXPECT_EQ(testAdapter.getNrCid(), 0x123456);
    EXPECT_EQ(testAdapter.GetEmf(), 2);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolNetAdapterLegacy, ProtocolNetOperatorAdapterLegacy) {
    sit_net_operator_v1_1 testVector[] = {
        { {'4', '5', '0', '0', '5', '#'}, "SKT", "SKTelecom", 1, 0x2733 },
        { {'4', '5', '0', '0', '6', '#'}, "", "", 1, 0x2733 },
        { {'4', '6', '0', '0', '0', '0'}, "CMCC", "China Mobile", 1, 0x2733 },
        { {'0', '0', '1', '0', '1', '#'}, "Test Bed", "Test Bed", 1, 0x2733 },
        { {'3', '1', '1', '4', '8', '0'}, "VZW", "Verizon Wireless", 0, 0x456 },
        { {0, }, {0, }, {0, }, 0, 0 },
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {
        ModemData *modemData = ModemDataBuilder::BuildResponse(
                SIT_GET_OPERATOR, 0, RCM_E_SUCCESS,
                &testVector[i], sizeof(testVector[i]));
        ASSERT_NE(modemData, nullptr);

        ProtocolNetOperatorAdapterLegacy testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), SIT_GET_OPERATOR);
        EXPECT_EQ(testAdapter.GetLength(), sizeof(RCM_HEADER) + sizeof(sit_net_operator_v1_1));
        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetParameterLength(), sizeof(sit_net_operator_v1_1));

        if (testVector[i].plmn[0] == 0) {
            EXPECT_EQ(testAdapter.GetPlmn(), nullptr);
        } else {
            char plmn[7] = {0, };
            memcpy(plmn, testVector[i].plmn, 6);
            if (plmn[5] == '#') {
                plmn[5] = 0;
            }
            EXPECT_TRUE(TextUtils::Equals(testAdapter.GetPlmn(), plmn));
        }

        if (TextUtils::IsEmpty(testVector[i].long_name)) {
            EXPECT_EQ(testAdapter.GetShortPlmn(), nullptr);
        } else {
            EXPECT_TRUE(TextUtils::Equals(testAdapter.GetLongPlmn(), testVector[i].long_name));
        }

        if (TextUtils::IsEmpty(testVector[i].short_name)) {
            EXPECT_EQ(testAdapter.GetShortPlmn(), nullptr);
        } else {
            EXPECT_TRUE(TextUtils::Equals(testAdapter.GetShortPlmn(), testVector[i].short_name));
        }
        EXPECT_EQ(testAdapter.GetRegState(), testVector[i].reg_state);
        EXPECT_EQ(testAdapter.GetLac(), testVector[i].lac);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    }  // end for i ~
}

TEST(ProtocolNetAdapterLegacy, ProtocolNetSelModeAdapterLegacy) {
    int testVector[] = {
        SIT_NET_NETWORK_MODE_AUTOMATIC, SIT_NET_NETWORK_MODE_MANUAL,
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {
        ModemData *modemData = ModemDataBuilder::BuildResponse(
                SIT_GET_NTW_MODE, 0, RCM_E_SUCCESS,
                &testVector[i], sizeof(testVector[i]));
        ASSERT_NE(modemData, nullptr);

        ProtocolNetSelModeAdapterLegacy testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), SIT_GET_NTW_MODE);
        EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_net_get_network_mode_rsp));
        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetParameterLength(), 4U);
        EXPECT_EQ(testAdapter.GetNetworkSelectionMode(), testVector[i]);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    }  // end for i ~
}

TEST(ProtocolNetAdapterLegacy, ProtocolNetPhysicalChannelConfigs_sit_net_physical_channel_config_ind) {
    sit_net_physical_channel_config_ind testVector;
    testVector.config_len = 2;
    testVector.configs[0].cell_status = (BYTE)PRIMARY_SERVING;
    testVector.configs[0].cell_bandwidth_downlink = 20000;
    testVector.configs[0].rat = (BYTE)14; // LTE
    testVector.configs[0].frequency_range = (BYTE)2;
    testVector.configs[0].channel = 1550;
    testVector.configs[0].context_len = (BYTE)2;
    testVector.configs[0].context_id[0] = 1;
    testVector.configs[0].context_id[1] = 2;
    testVector.configs[0].physical_cellid = 57;

    testVector.configs[1].cell_status = (BYTE)SECONDARY_SERVING;
    testVector.configs[1].cell_bandwidth_downlink = 80;
    testVector.configs[1].rat = (BYTE)21; // NR
    testVector.configs[1].frequency_range = (BYTE)3;
    testVector.configs[1].channel = 625334;
    testVector.configs[1].context_len = (BYTE)0;
    testVector.configs[1].physical_cellid = 59;

    ModemData *modemData = ModemDataBuilder::BuildIndication(
            SIT_IND_PHYSICAL_CHANNEL_CONFIG, ((sit_simple_ind_data_format *)&testVector)->data, sizeof(testVector) - sizeof(RCM_IND_HEADER));
    ASSERT_NE(modemData, nullptr);

    ProtocolNetPhysicalChannelConfigsLegacy testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
    EXPECT_EQ(testAdapter.GetId(), SIT_IND_PHYSICAL_CHANNEL_CONFIG);
    EXPECT_NE(testAdapter.GetParameter(), nullptr);

    list<PhysicalChannelConfigsData> &pCcList = testAdapter.GetConfigList();
    list<PhysicalChannelConfigsData>::iterator iter;

    int i = 0;
    for (iter = pCcList.begin(); iter != pCcList.end(); iter++) {
        EXPECT_EQ(iter->getStatus(), testVector.configs[i].cell_status);
        EXPECT_EQ(iter->getCellBandwidthDownlink(), testVector.configs[i].cell_bandwidth_downlink);
        ASSERT_TRUE(iter->getRat() == RADIO_TECH_LTE || iter->getRat() == RADIO_TECH_NR);

        if (iter->getRat() == RADIO_TECH_NR) {
            EXPECT_EQ(iter->getRfInfoType(), RF_INFO_TYPE_RANGE);
            EXPECT_EQ(iter->getRange(), testVector.configs[i].frequency_range & 0xFF);
        } else {
            EXPECT_EQ(iter->getRfInfoType(), RF_INFO_TYPE_CHANNEL_NUMBER);
            EXPECT_EQ(iter->getChannelNumber(), testVector.configs[i].channel);
        }

        EXPECT_EQ(iter->getLenContextIds(), testVector.configs[i].context_len);
        int *data = iter->getContextIds();
        for (int j = 0; j < iter->getLenContextIds(); j++) {
            EXPECT_EQ(data[j], testVector.configs[i].context_id[j]);
        }
        EXPECT_EQ((unsigned int)/* NEED TO CHECK */ iter->getPhysicalCellId(), testVector.configs[i].physical_cellid);
        i++;
    }

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolNetAdapterLegacy, ProtocolNetPhysicalChannelConfigsLegacy_sit_net_physical_channel_config_ind_v1_6) {
    sit_net_physical_channel_config_ind_v1_6 testVector;
    testVector.config_len = 2;
    testVector.configs[0].cell_status = (BYTE)PRIMARY_SERVING;
    testVector.configs[0].cell_bandwidth_downlink = 20000;
    testVector.configs[0].rat = (BYTE)14; // LTE
    testVector.configs[0].frequency_range = (BYTE)2;
    testVector.configs[0].channel = 1550;
    testVector.configs[0].context_len = (BYTE)2;
    testVector.configs[0].context_id[0] = 1;
    testVector.configs[0].context_id[1] = 2;
    testVector.configs[0].physical_cellid = 57;
    testVector.configs[0].channel_uplink = 19550;
    testVector.configs[0].cell_bandwidth_uplink = 20000;
    testVector.configs[0].band_number = 3;

    testVector.configs[1].cell_status = (BYTE)SECONDARY_SERVING;
    testVector.configs[1].cell_bandwidth_downlink = 80;
    testVector.configs[1].rat = (BYTE)21; // NR
    testVector.configs[1].frequency_range = (BYTE)3;
    testVector.configs[1].channel = 625334;
    testVector.configs[1].context_len = (BYTE)0;
    testVector.configs[1].physical_cellid = 59;
    testVector.configs[1].channel_uplink = 625334;
    testVector.configs[1].cell_bandwidth_uplink = 80;
    testVector.configs[1].band_number = 78;

    ModemData *modemData = ModemDataBuilder::BuildIndication(
            SIT_IND_PHYSICAL_CHANNEL_CONFIG, ((sit_simple_ind_data_format *)&testVector)->data, sizeof(testVector) - sizeof(RCM_IND_HEADER));
    ASSERT_NE(modemData, nullptr);

    ProtocolNetPhysicalChannelConfigsLegacy testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
    EXPECT_EQ(testAdapter.GetId(), SIT_IND_PHYSICAL_CHANNEL_CONFIG);
    EXPECT_NE(testAdapter.GetParameter(), nullptr);

    list<PhysicalChannelConfigsData> &pCcList = testAdapter.GetConfigList();
    list<PhysicalChannelConfigsData>::iterator iter;

    int i = 0;
    for (iter = pCcList.begin(); iter != pCcList.end(); iter++) {
        EXPECT_EQ(iter->getStatus(), testVector.configs[i].cell_status);
        ASSERT_TRUE(iter->getRat() == RADIO_TECH_LTE || iter->getRat() == RADIO_TECH_NR);
        EXPECT_EQ(iter->getChannelNumber(), testVector.configs[i].channel);
        EXPECT_EQ(iter->getUplinkChannelNumber(), testVector.configs[i].channel_uplink);
        EXPECT_EQ(iter->getCellBandwidthDownlink(), testVector.configs[i].cell_bandwidth_downlink);
        EXPECT_EQ(iter->getCellBandwidthUplink(), testVector.configs[i].cell_bandwidth_uplink);
        EXPECT_EQ(iter->getLenContextIds(), testVector.configs[i].context_len);
        int *data = iter->getContextIds();
        for (int j = 0; j < iter->getLenContextIds(); j++) {
            EXPECT_EQ(data[j], testVector.configs[i].context_id[j]);
        }
        EXPECT_EQ((unsigned int)/* NEED TO CHECK */ iter->getPhysicalCellId(), testVector.configs[i].physical_cellid);
        EXPECT_EQ(iter->getBandNumber(), testVector.configs[i].band_number);
        i++;
    }

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolNetAdapterLegacy, ProtocolNetSystemSelectionChannelsAdapterLegacy) {
    sit_net_radio_access_specifier testRas[] = {
        {
            .radio_access_network = SIT_NET_ACCESS_RADIO_TYPE_GERAN,
            .bands_len = 4,
            .bands.geran_bands = {1, 2, 3, 4},
            .channels_len = 10,
            .channels.geran_channels = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10},
        },
        {
            .radio_access_network = SIT_NET_ACCESS_RADIO_TYPE_UTRAN,
            .bands_len = 6,
            .bands.utran_bands = {5, 6, 7, 8, 9, 10},
            .channels_len = 15,
            .channels.utran_channels = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
        },
        {
            .radio_access_network = SIT_NET_ACCESS_RADIO_TYPE_EUTRAN,
            .bands_len = 7,
            .bands.eutran_bands = {31, 32, 43, 54, 66, 67, 68},
            .channels_len = 20,
            .channels.eutran_channels = {100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209},
        },
        {
            .radio_access_network = SIT_NET_ACCESS_RADIO_TYPE_NGRAN,
            .bands_len = 8,
            .bands.ngran_bands = {1, 8, 12, 20, 25, 70, 86, 257},
            .channels_len = 32,
            .channels.ngran_channels = {100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 1000, 1001, 1002, 1003, 1004, 1005, 1006, 1007, 1008, 1009, 3200, 4000},
        },
    };
    int testVector[] = { 0, 1, 2, 3, 4 };
    size_t size = sizeof(testVector) / sizeof(testVector[0]);
    for (size_t i = 0; i < size; i++) {
        sit_net_get_system_selection_channels_rsp data {};
        size_t payloadSize = sizeof(data.radio_access_specifier_len);
        data.radio_access_specifier_len = (uint8_t)testVector[i];
        size_t rasSize = sizeof(sit_net_radio_access_specifier) * i;
        memcpy(data.radio_access_specifier_array, testRas, rasSize);
        payloadSize += rasSize;
        ModemData *modemData = ModemDataBuilder::BuildResponse(
                SIT_GET_SYSTEM_SELECTION_CHANNELS, 0, RCM_E_SUCCESS,
                ((sit_simple_data_format *)&data)->data, payloadSize);
        ASSERT_NE(modemData, nullptr);

        ProtocolNetSystemSelectionChannelsAdapterLegacy testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), SIT_GET_SYSTEM_SELECTION_CHANNELS);
        EXPECT_EQ(testAdapter.GetLength(), payloadSize + sizeof(RCM_HEADER));
        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetParameterLength(), payloadSize);

        EXPECT_EQ(testAdapter.GetSpecifiersLength(), testVector[i]);
        if (testAdapter.GetSpecifiersLength() > 0) {
            ASSERT_NE(testAdapter.GetRadioAccessSpecifier(), nullptr);
            for (int ras_len = 0; ras_len < testAdapter.GetSpecifiersLength(); ras_len++) {
                RIL_RadioAccessSpecifier_V1_5 *rilRas = testAdapter.GetRadioAccessSpecifier() + ras_len;
                sit_net_radio_access_specifier *sitRas = nullptr;
                uint32_t *rilRasBands = nullptr;
                uint16_t *sitRasBands = nullptr;
                uint32_t *rilRasChannels = nullptr;
                uint32_t *sitRasChannels = nullptr;
                switch ((int)rilRas->radio_access_network) {
                    case RIL_RAN_GERAN:
                        sitRas = &testRas[0];
                        sitRasBands = sitRas->bands.geran_bands;
                        rilRasBands = (uint32_t *)rilRas->bands.geran_bands;
                        sitRasChannels = sitRas->channels.geran_channels;
                        rilRasChannels = (uint32_t *)rilRas->channels;
                        break;
                    case RIL_RAN_UTRAN:
                        sitRas = &testRas[1];
                        sitRasBands = sitRas->bands.utran_bands;
                        rilRasBands = (uint32_t *)rilRas->bands.utran_bands;
                        sitRasChannels = sitRas->channels.utran_channels;
                        rilRasChannels = (uint32_t *)rilRas->channels;
                        break;
                    case RIL_RAN_EUTRAN:
                        sitRas = &testRas[2];
                        sitRasBands = sitRas->bands.eutran_bands;
                        rilRasBands = (uint32_t *)rilRas->bands.eutran_bands;
                        sitRasChannels = sitRas->channels.eutran_channels;
                        rilRasChannels = (uint32_t *)rilRas->channels;
                        break;
                    case RIL_RAN_NGRAN:
                        sitRas = &testRas[3];
                        sitRasBands = sitRas->bands.ngran_bands;
                        rilRasBands = (uint32_t *)rilRas->bands.ngran_bands;
                        sitRasChannels = sitRas->channels.ngran_channels;
                        rilRasChannels = (uint32_t *)rilRas->channels;
                        break;
                    default:
                        // assert
                        ASSERT_EQ(true, false);
                        break;
                }
                EXPECT_EQ((int)rilRas->radio_access_network, (int)sitRas->radio_access_network);

                EXPECT_EQ((int)rilRas->bands_length, (int)sitRas->bands_len);
                for (uint32_t bandLen = 0; bandLen < rilRas->bands_length; bandLen++) {
                    EXPECT_EQ(rilRasBands[bandLen], sitRasBands[bandLen]);
                }

                EXPECT_EQ((int)rilRas->channels_length, (int)sitRas->channels_len);
                for (uint32_t channelLen = 0; channelLen < rilRas->channels_length; channelLen++) {
                    EXPECT_EQ(rilRasChannels[channelLen], sitRasChannels[channelLen]);
                }

            }  // end for ras_len ~
        }

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    }  // end for i ~
}

TEST(ProtocolNetAdapterLegacy, ProtocolNetSystemSelectionChannelsAdapterLegacy_invalid) {
    ModemData *modemData = nullptr;
    RIL_RadioAccessSpecifier_V1_5 *rilRas = nullptr;

    // too many RAS
    sit_net_get_system_selection_channels_rsp testManyRas = {
        .radio_access_specifier_len = 10,
        .radio_access_specifier_array[0].radio_access_network = 1,
        .radio_access_specifier_array[1].radio_access_network = 2,
        .radio_access_specifier_array[2].radio_access_network = 3,
        .radio_access_specifier_array[3].radio_access_network = 4,
    };
    modemData = ModemDataBuilder::BuildResponse(
            SIT_GET_SYSTEM_SELECTION_CHANNELS, 0, RCM_E_SUCCESS,
            ((sit_simple_data_format *)&testManyRas)->data,
            sizeof(sit_net_get_system_selection_channels_rsp) - sizeof(RCM_HEADER));
    ASSERT_NE(modemData, nullptr);

    ProtocolNetSystemSelectionChannelsAdapterLegacy testAdapterManyRas(modemData);
    EXPECT_EQ(testAdapterManyRas.GetSpecifiersLength(), MIN(SIT_MAX_NO_RAS, MAX_RADIO_ACCESS_NETWORKS));

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }

    // unknown RAN
    sit_net_get_system_selection_channels_rsp testUnknownRan = {
        .radio_access_specifier_len = 1,
        .radio_access_specifier_array = {
            {
                .radio_access_network = SIT_NET_ACCESS_RADIO_TYPE_UNKNOWN_ALLRAN,
                .bands_len = 8,
            },
        },
    };
    modemData = ModemDataBuilder::BuildResponse(
            SIT_GET_SYSTEM_SELECTION_CHANNELS, 0, RCM_E_SUCCESS,
            ((sit_simple_data_format *)&testUnknownRan)->data,
            sizeof(sit_net_get_system_selection_channels_rsp) - sizeof(RCM_HEADER));
    ASSERT_NE(modemData, nullptr);

    ProtocolNetSystemSelectionChannelsAdapterLegacy testAdapterUnknownRan(modemData);
    rilRas = testAdapterUnknownRan.GetRadioAccessSpecifier();
    EXPECT_EQ((int)rilRas->bands_length, 0);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }

    // too many channels
    sit_net_get_system_selection_channels_rsp testManyChannels = {
        .radio_access_specifier_len = 1,
        .radio_access_specifier_array = {
            {
                .radio_access_network = SIT_NET_ACCESS_RADIO_TYPE_EUTRAN,
                .channels_len = 101,
            },
        },
    };
    modemData = ModemDataBuilder::BuildResponse(
            SIT_GET_SYSTEM_SELECTION_CHANNELS, 0, RCM_E_SUCCESS,
            ((sit_simple_data_format *)&testManyChannels)->data,
            sizeof(sit_net_get_system_selection_channels_rsp) - sizeof(RCM_HEADER));
    ASSERT_NE(modemData, nullptr);

    ProtocolNetSystemSelectionChannelsAdapterLegacy testAdapterManyChannels(modemData);
    rilRas = testAdapterManyChannels.GetRadioAccessSpecifier();
    EXPECT_EQ((int)rilRas->channels_length, MIN(SIT_MAX_NO_SSC_EUTRAN_CHANNELS, MAX_CHANNELS));

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolNetAdapterLegacy, ProtocolGetBarringInfoRspAdapter_cell_identity_lte_v4) {
    char testVector[] = {
        0x01, 0x00, 0x4d, 0x07, 0xa1, 0x01, 0x0b, 0x07, 0x00, 0x00, 0x00, 0x00, 0x02, 0x34, 0x36, 0x36,
        0x30, 0x31, 0x23, 0x0f, 0x9e, 0x6b, 0x03, 0x98, 0x01, 0x00, 0x00, 0x72, 0x6b, 0x00, 0x00, 0x32,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x64, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x64, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xd1,
        0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xd2, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00
    };
    size_t size = sizeof(testVector) / sizeof(testVector[0]);

    ModemData modemData(testVector, size);
    ProtocolGetBarringInfoRspAdapterLegacy testAdapter(&modemData, 4);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_BARRING_INFOS);
    EXPECT_NE(testAdapter.GetParameter(), nullptr);
    EXPECT_EQ(testAdapter.GetParameterLength(), (size - sizeof(RCM_HEADER)));
    RIL_CellIdAndBarringInfo *barringInfo = testAdapter.GetCellIdAndBarringInfo();
    EXPECT_NE(barringInfo, nullptr);
    if (barringInfo != nullptr) {
        EXPECT_EQ(barringInfo->cellIdentity.cellInfoType, RIL_CELL_INFO_TYPE_LTE);
        EXPECT_EQ(barringInfo->numOfrecord, 7U);
        EXPECT_NE(barringInfo->precords, nullptr);
    }
}
TEST(ProtocolNetAdapterLegacy, ProtocolGetBarringInfoRspAdapter_cell_identity_wcdma_v4) {
    char testVector[] = {
        0x01, 0x00, 0x4d, 0x07, 0x45, 0x01, 0x6a, 0x01, 0x00, 0x00, 0x00, 0x00, 0x03, 0x34, 0x35, 0x30,
        0x30, 0x35, 0x23, 0x21, 0xa1, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x17, 0x01, 0x00, 0x00, 0xf1,
        0x29, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00
    };
    size_t size = sizeof(testVector) / sizeof(testVector[0]);

    ModemData modemData(testVector, size);
    ProtocolGetBarringInfoRspAdapterLegacy testAdapter(&modemData, 4);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_BARRING_INFOS);
    EXPECT_NE(testAdapter.GetParameter(), nullptr);
    EXPECT_EQ(testAdapter.GetParameterLength(), (size - sizeof(RCM_HEADER)));
    RIL_CellIdAndBarringInfo *barringInfo = testAdapter.GetCellIdAndBarringInfo();
    EXPECT_NE(barringInfo, nullptr);
    if (barringInfo != nullptr) {
        EXPECT_EQ(barringInfo->cellIdentity.cellInfoType, RIL_CELL_INFO_TYPE_WCDMA);
        EXPECT_EQ(barringInfo->numOfrecord, 3U);
        EXPECT_NE(barringInfo->precords, nullptr);
    }
}

TEST(ProtocolNetAdapterLegacy, ProtocolBarringInfoChangedAdapter_cell_identity_v4) {
    char testVector[] = {
        0x02, 0x00, 0x4b, 0x07, 0x9f, 0x01, 0x00, 0x00, 0x02, 0x34, 0x36, 0x36, 0x30, 0x31, 0x23, 0x0f,
        0x9e, 0x6b, 0x03, 0x98, 0x01, 0x00, 0x00, 0x72, 0x6b, 0x00, 0x00, 0x32, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x64,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x64, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xd1, 0x07, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xd2, 0x07, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    size_t size = sizeof(testVector) / sizeof(testVector[0]);

    ModemData modemData(testVector, size);
    ProtocolBarringInfoChangedAdapterLegacy testAdapter(&modemData, 4);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
    EXPECT_EQ(testAdapter.GetId(), SIT_IND_BARRING_INFO_CHANGED);
    EXPECT_NE(testAdapter.GetParameter(), nullptr);
    EXPECT_EQ(testAdapter.GetParameterLength(), (size - sizeof(RCM_IND_HEADER)));
    RIL_CellIdAndBarringInfo *barringInfo = testAdapter.GetCellIdAndBarringInfo();
    EXPECT_NE(barringInfo, nullptr);
    if (barringInfo != nullptr) {
        EXPECT_EQ(barringInfo->cellIdentity.cellInfoType, RIL_CELL_INFO_TYPE_LTE);
        EXPECT_EQ(barringInfo->numOfrecord, 7U);
        EXPECT_NE(barringInfo->precords, nullptr);
    }
}

TEST(ProtocolNetAdapterLegacy, ProtocolBarringInfoChangedAdapter_cell_identity_wcdma_v4) {
    char testVector[] = {
        0x02, 0x00, 0x4b, 0x07, 0x41, 0x01, 0x00, 0x00, 0x03, 0x34, 0x35, 0x30, 0x30, 0x35, 0x23, 0x21,
        0xa1, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x17, 0x01, 0x00, 0x00, 0xf1, 0x29, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00
    };
    size_t size = sizeof(testVector) / sizeof(testVector[0]);

    ModemData modemData(testVector, size);
    ProtocolBarringInfoChangedAdapterLegacy testAdapter(&modemData, 4);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
    EXPECT_EQ(testAdapter.GetId(), SIT_IND_BARRING_INFO_CHANGED);
    EXPECT_NE(testAdapter.GetParameter(), nullptr);
    EXPECT_EQ(testAdapter.GetParameterLength(), (size - sizeof(RCM_IND_HEADER)));
    RIL_CellIdAndBarringInfo *barringInfo = testAdapter.GetCellIdAndBarringInfo();
    EXPECT_NE(barringInfo, nullptr);
    if (barringInfo != nullptr) {
        EXPECT_EQ(barringInfo->cellIdentity.cellInfoType, RIL_CELL_INFO_TYPE_WCDMA);
        EXPECT_EQ(barringInfo->numOfrecord, 3U);
        EXPECT_NE(barringInfo->precords, nullptr);
    }
}

cell_identity_lte_v4 testCellIdentityLteV4 = {
    .plmn = {'4', '5', '0', '0', '5', '#'},
    .cell_id = 1234,
    .phy_cell_id = 501,
    .tac = 119,
    .earfcn = 201,
    .bandwidth = 10000,
    .plmn_info = {
        .plmn_short_name = {
            0x53, 0x4b, 0x54, 0x65, 0x6c, 0x65, 0x63, 0x6f, 0x6d, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        .plmn_long_name = {
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        .additional_plmn_len = 0,
        .additional_plmn_info = {
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
    },
    .csg_info = {
        .csg_indication = 0,
        .hnb_name = {
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0},
        .csg_identity = INT_MAX
    },
    .band_len = 1,
    .band_info = {0, 0, 0, 0, 0, 0, 0, 0}
};

cell_identity_wcdma_v4 testCellIdentityWcdmaV4 = {
    .plmn = {'4', '5', '0', '0', '5', '#'},
    .lac = 20000,
    .cid = 9999,
    .psc = 111,
    .uarfcn = 11,
    .plmn_info = {
        .plmn_short_name = "SKT",
        .plmn_long_name = "SKT Telecom",
        .additional_plmn_len = 0,
        .additional_plmn_info = {
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        }
    },
    .csg_info = {
        .csg_indication = 1,
        .hnb_name = {
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0},
        .csg_identity = 9
    }
};

TEST(ProtocolNetAdapterLegacy, ProtocolRegistrationFailedAdapter) {
#pragma pack(1)
    struct SitRegFailed;
    struct SitRegFailed {
        BYTE plmn[MAX_PLMN_LEN];
        BYTE service_domain;
        BYTE rej_cause;
        BYTE rej_cause_additional;
        BYTE cell_info_type;
        cell_identity_lte_v4 cell_identity;
    };
#pragma pack()

    SitRegFailed testVector[] = {
        {
            .plmn = { '4', '5', '0', '0', '5', '#' },
            .service_domain = SERVICE_DOMAIN_CSPS,
            .rej_cause = 0,
            .rej_cause_additional = 0,
            .cell_info_type = SIT_NET_CELL_INFO_TYPE_LTE,
            .cell_identity = testCellIdentityLteV4,
        },
        {
            .plmn = { '3', '1', '1', '4', '8', '0' },
            .service_domain = SERVICE_DOMAIN_CS,
            .rej_cause = 0,
            .rej_cause_additional = 0,
            .cell_info_type = SIT_NET_CELL_INFO_TYPE_LTE,
            .cell_identity = testCellIdentityLteV4,
        },
        {
             .plmn = { '0', '0', '1', '0', '1', '#' },
             .service_domain = SERVICE_DOMAIN_PS,
             .rej_cause = 255,
             .rej_cause_additional = 255,
             .cell_info_type = SIT_NET_CELL_INFO_TYPE_LTE,
             .cell_identity = testCellIdentityLteV4,
         },
    };
    size_t size = sizeof(testVector) / sizeof(testVector[0]);
    for (size_t i = 0; i < size; i++) {
        ModemDataBuilder buidler;
        ModemData *modemData = ModemDataBuilder::BuildIndication(SIT_IND_REG_FAILED,
                &testVector[i], sizeof(testVector[i]));
        ASSERT_NE(modemData, nullptr);
        if (modemData) {
            ProtocolRegistrationFailedAdapterLegacy adapter(modemData, 4);
            RIL_RegistrationFailed *result = adapter.GetRegistrationFailed();
            ASSERT_NE(result, nullptr);
            char testPlmn[7] {};
            memcpy(testPlmn, testVector[i].plmn, MAX_PLMN_LEN);
            if (testPlmn[5] == '#') testPlmn[5] = 0;
            EXPECT_TRUE(TextUtils::Equals(result->chosenPlmn, testPlmn));
            if (testVector[i].service_domain == SERVICE_DOMAIN_CSPS) {
                EXPECT_EQ(result->domainBitmap, DOMAIN_CS | DOMAIN_PS);
            } else if (testVector[i].service_domain == SERVICE_DOMAIN_CS) {
                EXPECT_EQ(result->domainBitmap, DOMAIN_CS);
            } else if (testVector[i].service_domain == SERVICE_DOMAIN_PS) {
                EXPECT_EQ(result->domainBitmap, DOMAIN_PS);
            } else {
                ASSERT_NE(INT_MAX, INT_MAX);
            }

            if (testVector[i].rej_cause == 255) {
                EXPECT_EQ(result->causeCode, INT_MAX);
            } else {
                EXPECT_EQ(result->causeCode, testVector[i].rej_cause);
            }

            if (testVector[i].rej_cause_additional == 255) {
                EXPECT_EQ(result->additionalCauseCode, INT_MAX);
            } else {
                EXPECT_EQ(result->additionalCauseCode, testVector[i].rej_cause);
            }

            delete modemData;
        }
    }
}

TEST(ProtocolNetAdapterLegacy, ProtocolRegistrationFailedAdapter_cell_indentity_lte_v4) {
#pragma pack(1)
    struct SitRegFailed;
    struct SitRegFailed {
        BYTE plmn[MAX_PLMN_LEN];
        BYTE service_domain;
        BYTE rej_cause;
        BYTE rej_cause_additional;
        BYTE cell_info_type;
        cell_identity_lte_v4 cell_identity;
    };
#pragma pack()

    SitRegFailed testVector = {
        .plmn = { '4', '5', '0', '0', '5', '#' },
        .service_domain = SERVICE_DOMAIN_CSPS,
        .rej_cause = 0,
        .rej_cause_additional = 0,
        .cell_info_type = SIT_NET_CELL_INFO_TYPE_LTE,
        .cell_identity = testCellIdentityLteV4,
    };

    ModemDataBuilder buidler;
    ModemData *modemData = ModemDataBuilder::BuildIndication(SIT_IND_REG_FAILED,
            &testVector, sizeof(testVector));
    ASSERT_NE(modemData, nullptr);
    if (modemData) {
        ProtocolRegistrationFailedAdapterLegacy adapter(modemData, 4);
        RIL_RegistrationFailed *result = adapter.GetRegistrationFailed();
        ASSERT_NE(result, nullptr);

        EXPECT_EQ(result->cellIdentity.cellInfoType, RIL_CELL_INFO_TYPE_LTE);
        RIL_CellIdentityLte_V1_5 cellIdentityLte = result->cellIdentity.lte;

        EXPECT_EQ(cellIdentityLte.mcc, CellInfoProtocolUtils::Legacy::FetchMcc(testVector.cell_identity.plmn));
        EXPECT_EQ(cellIdentityLte.mnc, CellInfoProtocolUtils::Legacy::FetchMnc(testVector.cell_identity.plmn));
        EXPECT_EQ(cellIdentityLte.ci, testVector.cell_identity.cell_id);
        EXPECT_EQ(cellIdentityLte.pci, testVector.cell_identity.phy_cell_id);
        EXPECT_EQ(cellIdentityLte.tac, testVector.cell_identity.tac);
        EXPECT_EQ(cellIdentityLte.earfcn, testVector.cell_identity.earfcn);
        EXPECT_EQ(cellIdentityLte.bandwidth, testVector.cell_identity.bandwidth);
        EXPECT_EQ(cellIdentityLte.optionalCsgInfo.csgInfo.csgIndication, testVector.cell_identity.csg_info.csg_indication);
        EXPECT_TRUE(strncmp(cellIdentityLte.optionalCsgInfo.csgInfo.homeNodebName,
            testVector.cell_identity.csg_info.hnb_name, sizeof(testVector.cell_identity.csg_info.hnb_name)) == 0);
        EXPECT_EQ(cellIdentityLte.optionalCsgInfo.csgInfo.csgIdentity, testVector.cell_identity.csg_info.csg_identity);
        EXPECT_EQ(cellIdentityLte.leng_bands, testVector.cell_identity.band_len);
        EXPECT_TRUE(memcmp(cellIdentityLte.bands, testVector.cell_identity.band_info, MAX_BANDS) == 0);

        delete modemData;
    }
}

TEST(ProtocolNetAdapterLegacy, ProtocolRegistrationFailedAdapter_cell_indentity_wcdma_v4) {
#pragma pack(1)
    struct SitRegFailed;
    struct SitRegFailed {
        BYTE plmn[MAX_PLMN_LEN];
        BYTE service_domain;
        BYTE rej_cause;
        BYTE rej_cause_additional;
        BYTE cell_info_type;
        cell_identity_wcdma_v4 cell_identity;
    };
#pragma pack()

    SitRegFailed testVector = {
        .plmn = { '4', '5', '0', '0', '5', '#' },
        .service_domain = SERVICE_DOMAIN_CSPS,
        .rej_cause = 0,
        .rej_cause_additional = 0,
        .cell_info_type = SIT_NET_CELL_INFO_TYPE_WCDMA,
        .cell_identity = testCellIdentityWcdmaV4,
    };

    ModemDataBuilder buidler;
    ModemData *modemData = ModemDataBuilder::BuildIndication(SIT_IND_REG_FAILED,
            &testVector, sizeof(testVector));
    ASSERT_NE(modemData, nullptr);
    if (modemData) {
        ProtocolRegistrationFailedAdapterLegacy adapter(modemData, 4);
        RIL_RegistrationFailed *result = adapter.GetRegistrationFailed();
        ASSERT_NE(result, nullptr);

        EXPECT_EQ(result->cellIdentity.cellInfoType, RIL_CELL_INFO_TYPE_WCDMA);
        RIL_CellIdentityWcdma_V1_5 cellIdentityWcdma = result->cellIdentity.wcdma;

        EXPECT_EQ(cellIdentityWcdma.mcc, CellInfoProtocolUtils::Legacy::FetchMcc(testVector.cell_identity.plmn));
        EXPECT_EQ(cellIdentityWcdma.mnc, CellInfoProtocolUtils::Legacy::FetchMnc(testVector.cell_identity.plmn));
        EXPECT_EQ(cellIdentityWcdma.lac, testVector.cell_identity.lac);
        EXPECT_EQ(cellIdentityWcdma.cid, testVector.cell_identity.cid);
        EXPECT_EQ(cellIdentityWcdma.psc, testVector.cell_identity.psc);
        EXPECT_EQ(cellIdentityWcdma.uarfcn, testVector.cell_identity.uarfcn);
        EXPECT_EQ(cellIdentityWcdma.optionalCsgInfo.csgInfo.csgIndication, testVector.cell_identity.csg_info.csg_indication);
        EXPECT_TRUE(strncmp(cellIdentityWcdma.optionalCsgInfo.csgInfo.homeNodebName,
            testVector.cell_identity.csg_info.hnb_name, sizeof(testVector.cell_identity.csg_info.hnb_name)) == 0);
        EXPECT_EQ(cellIdentityWcdma.optionalCsgInfo.csgInfo.csgIdentity, testVector.cell_identity.csg_info.csg_identity);

        delete modemData;
    }
}

TEST(ProtocolNetAdapterLegacy, ProtocolNetGetVonrCapaAdapterLegacy) {
    for (int i = 0; i <= 1; i++) {
        ModemData *modemData = ModemDataBuilder::BuildResponse(
                SIT_GET_VONR_CAPABILITY, 0, RCM_E_SUCCESS,
                &i, sizeof(int));
        ASSERT_NE(modemData, nullptr);

        ProtocolNetGetVonrCapaAdapterLegacy testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), SIT_GET_VONR_CAPABILITY);
        EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_net_get_vonr_capa_rsp));
        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetParameterLength(), (uint32_t)4);
        EXPECT_EQ(testAdapter.GetVonrCapa(), i);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    }  // end for i ~
}

TEST(ProtocolNetAdatper, ProtocolNetDsdsStatusIndAdapterLegacy) {
    uint8_t testVector[] = { DSDS_STATUS_RESUME, DSDS_STATUS_SUSPEND, 0xFF };
    for (auto it : testVector) {
        ModemData *modemData = ModemDataBuilder::BuildIndication(
                SIT_IND_DSDS_STATUS, &it, sizeof(it));
        ASSERT_NE(modemData, nullptr);

        ProtocolNetDsdsStatusIndAdapterLegacy testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
        EXPECT_EQ(testAdapter.GetId(), SIT_IND_DSDS_STATUS);
        EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_net_dsds_status_ind));
        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetParameterLength(), sizeof(it));
        // No need to test Private method, if it's needed, please redesign
        // EXPECT_EQ(testAdapter.GetStatus(), it);
        EXPECT_EQ(testAdapter.IsPsRestricted(), it == DSDS_STATUS_SUSPEND);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    }  // end for i ~
}
